/*
 * ***** BEGIN LICENSE BLOCK *****
 *
 * The contents of this file are subject to the GNU General Public License
 * Version 2 (the "License"); you may not use this file except in compliance
 * with the GPL. You may obtain a copy of the License at
 * http://www.gnu.org/copyleft/gpl.html or the file GPL.TXT from the program
 * or source code package.
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
 * the specific language governing rights and limitations under the GPL.
 *
 * The Original Code is Therapy Seriouz Software code.
 *
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "select.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "globdefs.h"
#include "smurf.h"
#include "smurfine.h"

#include "smurfobs.h"
#include "ext_obs.h"


#define LOAD	1
#define SAVE	2

static char *namelist[257];



/* Packt eine Fileliste des jeweiligen Typs in eine Liste des */
/* Standardformates fÅr file_load() und gibt einen Zeiger darauf zurÅck */
/* FSBOX = Liste von Selectric, Array von Zeigern - wird eigentlich in get_multfiles() gemacht */
/* FSBOX2 = Ein einzelner Filename in einem String */
/* ARGS = Liste von D&D-Type ARGS, String mit gequoteten Namen */
/* START = Liste vom ARGV-Verfahren, Array von Zeigern */
/* VA = Liste von VA_START, String mit gequoteten Namen */
static void f_build_filelist(char **param, int mode)
{
	char *arg_name;
	int i;

	switch (mode)
	{
	case START:
		i = 0;
		while (i < 256 && (arg_name = (param[i + 1])) != NULL)
		{
			if ((namelist[i] = (char *) malloc(strlen(arg_name) + 1)) != NULL)
				strcpy(namelist[i++], arg_name);
			else
				break;
		}
		namelist[i] = NULL;				/* terminieren */
		break;

	case VA:
	case ARGS:
		i = 0;
		while (i < 256 && (arg_name = strargvtok(*param)) != NULL)
		{
			if ((namelist[i] = (char *) malloc(strlen(arg_name) + 1)) != NULL)
				strcpy(namelist[i++], arg_name);
			else
				break;
		}
		namelist[i] = NULL;				/* terminieren */
		break;

	case FSBOX2:
		if ((namelist[0] = (char *) malloc(strlen(*param) + 1)) != NULL)
			strcpy(namelist[0], *param);
		namelist[1] = NULL;				/* terminieren */
		break;
	}
}


static void f_free_filelist(void)
{
	int i;

	/* angeforderte Zeiger wieder freigeben */
	i = 0;
	while (i < 256 && namelist[i])
		free(namelist[i++]);
}


/* Ist nun die Hauptladefunktion und wird nicht mehr */
/* von f_loadpic() aufgerufen, sondern andersherum. */
void file_load(char *ltext, char **dateien, short mode)
{
	char *buf;
	char *fullname;
	char *ext;
	short t;
	int fh;
	long dummy;

	/* Drag&Drop, ARGV oder AV-BildÅbergabe ---------------------------- */
	if (DraufschmeissBild)
		f_build_filelist(dateien, mode);
	else if (f_fsbox(loadpath, ltext, LOAD) == FALSE)
		return;

	t = 0;
	while (t < 256 && (fullname = namelist[t]) != NULL)
	{
		/* Sonderbehandlung fÅr Photo-CDs */
		ext = strrchr(fullname, '.');
		if (ext != NULL && strnicmp(ext, ".PCD", 4) == 0)
		{
			buf = f_do_pcd(fullname);
		} else
		{
			if ((fh = (int) Fopen(fullname, FO_READ)) >= 0)	/* Datei îffnen */
			{
				f_len = Fseek(0, fh, 2);	/* FilelÑnge ermitteln */
				Fseek(0, fh, 0);		/* und Dateizeiger wieder auf Anfang */

				if ((buf = SMalloc(f_len)) == NULL)
				{
					Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_MEM].TextCast, NULL, NULL, NULL, 1);
				} else
				{
					dummy = Fread(fh, f_len, buf);

					if (dummy < f_len)
					{
						Dialog.winAlert.openAlert(Dialog.winAlert.alerts[PICLOAD_READERR].TextCast, NULL, NULL, NULL, 1);
						SMfree(buf);
						buf = NULL;
					}
				}

				Fclose(fh);
			} else						/* Fehler beim ôffnen */
			{
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[FILEOPEN_ERR].TextCast, NULL, NULL, NULL, 1);
				buf = NULL;
			}
		}

		if (buf)						/* konnte File geladen werden, Bild handlen */
		{
			if (f_loadpic(buf, fullname) == 0)
			{
				if (!DraufschmeissBild || DraufschmeissBild == START)
					strcpy(loadpath, fullname);
			} else
				break;
		}

		t++;
	}

	DraufschmeissBild = 0;				/* Ja, jetzt erst */

	f_free_filelist();
}


BOOLEAN file_save(char *stext, uint8_t *buf, long length)
{
	char desk_name[9];
	int fh;
	WORD out1, out2, out3, out4;
	WORD mbuf[8];
	WORD desk_id;
	WORD drive;
	WORD attrib;
	long check;

  start:
	if (f_fsbox(savepath, stext, SAVE) == FALSE)
		return FALSE;

	/* mit einem Trick testen, ob die Datei da ist */
	/* Ergebnis < 0 wenn die Datei noch nicht vorhanden ist */
	if (Fattrib(savepath, 0, 0) >= 0)
		if (Dialog.winAlert.openAlert("Es existiert bereits eine Datei dieses Namens! Soll Sie ersetzt werden?", "Nein", " Ja ", NULL, 2) == 1)	/* FIXME: translate */
			goto start;

	if ((fh = (int) Fcreate(savepath, 0)) >= 0)
	{
		check = Fwrite(fh, length, buf);
		Fclose(fh);

		if (check != length)
		{
			if (check < 0)
				Dialog.winAlert.openAlert("Es ist ein fataler Schreibfehler aufgetreten! Laufwerk voll, oder schlimmeres.", NULL, NULL, NULL, 1);	/* FIXME: translate */
			else
				Dialog.winAlert.openAlert("Die Datei konnte nicht vollstÑndig geschrieben werden. Wahrscheinlich ist das Laufwerk voll.", NULL, NULL, NULL, 1);	/* FIXME: translate */
		}

										  /*------- "Laufwerksinhalt verÑndert" ans Desktop -------*/
		/* Erst mal appl_getinfo() fragen, weil das TOS sonst "falscher */
		/* AES-Funktionsaufruf" bringt - Scheiûe, kann ihm doch egal sein. */
		if ((Sys_info.OSFeatures & GETINFO) && appl_getinfo(4, &out1, &out2, &out3, &out4) && out3 == 1)
			if (appl_search(2, desk_name, &attrib, &desk_id) == 1)
			{
				mbuf[0] = SH_WDRAW;
				mbuf[1] = appl_id;
				mbuf[2] = 0;
				drive = *savepath;
				if (drive >= 'a')
					drive -= 'a';
				else if (drive >= 'A')
					drive -= 'A';
				else
					drive -= '0';
				mbuf[3] = drive;
				appl_write(desk_id, 16, mbuf);
			}

	} else
	{
		Dialog.winAlert.openAlert("Es ist ein Fehler beim Anlegen der Datei aufgetreten!", NULL, NULL, NULL, 1);	/* FIXME: translate */
		return FALSE;
	}

	return TRUE;
}


/* Messagehandler fÅr Betrieb des FSEL im Fenster */
/* 
 * Die Einsprungadresse dieser Funktion wird als Callback an BoxKite
 * Åbergeben. 
 * Wie man sieht, kann sie auch andere Messagetypen als WM_REDRAW
 * behandeln. WM_MOVED ist sogar ratsam, da es immerhin Betriebssystem-
 * versionen gibt, die das Verschieben von hintenliegenden Fenstern
 * ermîglichen. Messages, die implizit neue Fenster îffnen oder vorhandene 
 * nach oben bringen, sollten dagegen ignoriert oder aufgehoben und erst 
 * nach der RÅckkehr aus dem Fileselector behandelt werden.
 * WM_TOPPED-Messages werden nicht an den Message-Callback durchgereicht.
 */
static void cdecl message_handler(WORD *msg)
{
	switch (msg[0])
	{
	case WM_REDRAW:
		memcpy(messagebuf, msg, 16);
		f_handle_message();
		break;
	case WM_MOVED:
		memcpy(messagebuf, msg, 16);
		f_handle_message();
		break;
	default:
		break;
	}
}


/* Anlegen und initialisieren der Filenamenliste, welche durch den */
/* Fileselektor bei einer Mehrfachselektion dann gefÅllt wird. */
static void set_multfiles(SLCT_STR *slct_str, char **sellist)
{
	int i;

	if (!slct_str || slct_str->config.onoff == 0)	/* Selectric nicht da oder aus? */
		return;

	/* Mehrfachselektion ein, Array von Zeigern */
	slct_str->comm = 0x01;

	i = 0;
	do
	{
		if ((sellist[i] = (char *) calloc(1, 65)) == NULL)
		{
			i--;						/* letzter war der Letzte */
			break;
		}
	} while (++i < 30);

	/* konnte zumindest ein Zeiger angefordert werden? */
	if (i >= 0)
	{
		/* maximale Anzahl Bilder */
		slct_str->out_count = i;
		slct_str->out_ptr = sellist;
	}
}


/* Alle Files der Liste werden direkt Åber die f_loadpic() geladen */
/* und der Laderoutine dann ein Abbruch vorgegaukelt. */
static void get_multfiles(SLCT_STR *slct_str, char *pathname, char **sellist)
{
	char *part;
	short i;
	short sel;

	if (!slct_str)
		return;

	sel = slct_str->out_count;

	/* initial holen, Position bleibt in der Schleife gleich */
	strcpy(commpath, pathname);
	part = strrchr(commpath, '\\') + 1;

	/* Liste in die Standardliste Åbertragen */
	i = 0;
	while (i < 256 && i < sel)
	{
		strcpy(commpath, pathname);
		strcpy(part, sellist[i]);

		if ((namelist[i] = (char *) calloc(1, strlen(commpath) + 1)) != NULL)
			strcpy(namelist[i++], commpath);
		else
			break;
	}

	namelist[i] = NULL;					/* Ende markieren */

	/* angeforderte Zeiger wieder freigeben */
	i = 0;
	while (i < 30 && sellist[i])
		free(sellist[i++]);

	/* Mehrfachselektion aus */
	slct_str->comm = 0x0;
}


/* Datei mittels Fileselector îffnen. */
BOOLEAN f_fsbox(char *Path, const char *fbtext, uint8_t selectart)
{
	char pathname[SM_PATH_MAX + 1];
	char filename[65];
	char *part;
	char *sellist[30];
	WORD back;
	WORD t;
	SLCT_STR *slct_str = NULL;

	strncpy(pathname, Path, t = strsrchr(Path, '\\') + 1);	/* Pfadteil kopieren */
	pathname[t] = '\0';					/* und */
	strcat(pathname, "*.*");			/* Maske auf alles setzen. */
	part = strrchr(Path, '\\') + 1;		/* Zeiger auf Fileteil holen */
	if (strcmp(part, "*.*") != 0)		/* und wenn nicht gleich "*.*", */
		strcpy(filename, part);			/* Filenamen extrahieren. */
	else								/* Ansonsten */
		strcpy(filename, "");			/* leeren Filenamen eintragen */

	get_cookie(0x4653454cL, (unsigned long *) &slct_str);	/* 'FSEL' */

	if (selectart == LOAD)
		set_multfiles(slct_str, sellist);

	f_set_syspal();

	/* version enthÑlt die Version der Schnittstelle */
	/* 1.0 ist Original Selectric */
	/* 1.02 ist Boxkite ab nonmodal */
	/* 2.0 ist Freedom mit nonmodal */
	/* Bei NonmodalitÑt keine wind_update()-Klammerung! */
#if 0
	if (!slct_str || slct_str->id != 0x534c4354L || (slct_str->version <= 0x0110 && slct_str->version != 0x0102))
#endif
		wind_update(BEG_UPDATE);

	if (Sys_info.AES_version >= 0x0140 || (slct_str && slct_str->id == 0x534c4354L))
		back = fsel_boxinput(pathname, filename, &t, fbtext, message_handler);
	else
		back = fsel_input(pathname, filename, &t);

#if 0
	if (!slct_str || slct_str->id != 0x534c4354L || (slct_str->version <= 0x0110 && slct_str->version != 0x0102))
#endif
		wind_update(END_UPDATE);

	if (back == FALSE)
	{
		Dialog.winAlert.openAlert("Fehler beim Aufruf der Dateiauswahl!", NULL, NULL, NULL, 1);	/* FIXME: translate */
		return FALSE;
	}

	if (t == 1 && (strlen(filename) > 0 || selectart == PATH))	/* wurde der Selektor erfolgreich verlassen */
	{
		/* Dieser Mechanismus beruht darauf, daû bei allen Selektoren mit */
		/* selectrickompatibler Mehrfachselektion auch bei nur einem selektierten */
		/* File dieser Name in der Mehrfachliste steht! */
		/* Sollte dies nicht der Fall sein, hammer verschissen. */
		if (selectart == LOAD && slct_str && slct_str->config.onoff == 1)	/* Selectric da und an? */
		{
			get_multfiles(slct_str, pathname, sellist);
		} else
		{
			strncpy(Path, pathname, t = strsrchr(pathname, '\\') + 1);
			Path[t] = '\0';

			if (selectart != PATH)
			{
				strcat(Path, filename);
				f_build_filelist(&Path, FSBOX2);
			}
		}

		return TRUE;
	}
	return FALSE;
}
