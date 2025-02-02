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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* modconf.c		31.8. - 22.9.98, Olaf
	Dieses File enth�lt die Routinen zum Schreiben und Auslesen der MODCONF.CNF,
	in der sich die Konfigurationen aller Editmodule befinden, und au�erdem
	zwei Funktionen zum Merken und Zur�ckholen der letzten Einstellungen der
	Module.
	Das ganze besteht aus einer Menge Freads und Fseeks und ist ein wenig knifflig,
	da ich nicht immer die komplette Datei einlesen will. Man kann nie wissen, wie lang
	die bei manchen Usern und bestimmten Modulen werden kann.
	Die beiden Routinen mconfSave() und mconfLoad() werden von den Modulen (bzw. von
	Smurf aus mpref_change() heraus) aufgerufen. Beide �ffnen ggfs. ein 'Pseudopopup',
	in dem die gespeicherten Konfigurationen ausgew�hlt werden k�nnen.
	save_to_modconf und load_from_modconf speichern bzw. laden eine Modulkonfiguration
	in/aus der MODCONF.CNF. save_to legt die Datei, wenn nicht vorhanden, neu an.
	seek_modconf sucht in der Datei nach der zum jeweiligen Modul passenden	MCA (Module
	Configuration Area), die mit dem Magic MCAB anf�ngt und mit MCAE endet. 
	Dateiaufbau:
	
	<l�nge in bytes>MCAB<modulname>\0<anzahl>
	MCNF<cnf-name>\0<l�nge in bytes><cnf-block>
	MCNF<cnf-name>\0<...
	MCAE
	<l�nge in bytes>MCAB<modulname2>\0...
	...
	
	<l�nge in bytes> enth�lt die L�nge des gesamten MCAB inklusive Anfangs- und Endekennung,
	<anzahl> ist die Anzahl an MCNF-Bl�cken f�r jedes Modul in der Datei.
	<cnf-name> ist immer 33 Bytes lang, wenn ein k�rzerer Name eingegeben wurde, wird mit	
	Nullbytes aufgef�llt. <cnf-block> ist der beim Speichern vom jeweiligen Modul 
	�bermittelte Konfigurationsblock der L�nge <l�nge in bytes>.

	Noch zu machen:
	 - l�schen von Konfigurationen (CTRL+Klick im Popup?)

	Die Magics MCAB/MCAE etc. sind noch ein wenig gef�hrlich, da so keine Modulnamen und
	-konfigurationen diese Buchstabenkombinationen enthalten d�rfen! Da m�ssen noch ein paar
	Kennbytes rein, die da garantiert nicht vorkommen k�nnen...
	*/


#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "import.h"
#include "smurf.h"
#include "globdefs.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurfine.h"
#include "plugin.h"
#include "smplugin.h"

#include "smurfobs.h"
#include "ext_obs.h"

#define SEEKBUF_SIZE 2048


OBJECT *confsave_dialog;
OBJECT *modconf_popup;


/*
 * die Default-Konfigurationen f�r die Editmodule
 */
void *edit_cnfblock[100];
static short edit_cnflen[100];


/* Schnelle Suchroutine nach Boyer-Moore */
/* implementiert 24.5.95 - 10.6.95 von Christian Eyrich */
/* sucht m in n */
static long Idxtab(unsigned char *n, const char *m, long len)
{
	unsigned char skiptab[256];
	short t;
	short lenofm;
	short lenofm2;
	short j;
	long i;
	long lenofn = len;

	lenofm = strlen(m);
	lenofm2 = lenofm - 1;

	for (i = 0; i < 256; i++)			/* initialisieren der Tabelle */
		skiptab[i] = lenofm;			/* mit der L�nge des Suchstrings */

	for (i = 0; i < lenofm; i++)
		skiptab[(unsigned char)m[i]] = lenofm - i - 1;

	/* die eigentliche Suchroutine */
	for (i = j = lenofm2; j > 0; i--, j--)
	{
		while (n[i] != (unsigned char)m[j] && i < lenofn)
		{
			t = skiptab[n[i]];
			i += (lenofm - j > t) ? lenofm - j : t;
			j = lenofm2;
		}

		if (i > lenofn)					/* wurde nichts gefunden */
			return -1;				/* wird -1 zur�ckgegeben */
	}

	return i;							/* wurde die Routine f�ndig, */
}										/* wird die Fundstelle zur�ckgegeben */


/* seekInFile --------------------------------------------------------------
	Sucht den String SeekString in der Datei filehandle ab der aktuellen
	Position und gibt relativ zur Dateiposition beim Aufruf den Abstand des
	gefundenen Strings in der Datei zur�ck, bzw. -1, wenn nix gefunden wurde.
	-------------------------------------------------------------------------*/
static long seekInFile(int filehandle, char *SeekString)
{
	unsigned char *seekbuffer;
	long pos;
	long filepos = 0;
	long read_bytes;
	long oldpos;
	long seekLen;

	seekLen = strlen(SeekString);
	oldpos = Fseek(0, filehandle, 1);

	/*
	 * jetzt wird die Datei 2K-blockweise nach SeekString durchsucht
	 */
	seekbuffer = malloc(SEEKBUF_SIZE + 8);
	memset(seekbuffer, 0x0, SEEKBUF_SIZE + 8);
	filepos = 0;
	do
	{
		read_bytes = Fread(filehandle, SEEKBUF_SIZE, seekbuffer);

		if ((pos = Idxtab(seekbuffer, SeekString, SEEKBUF_SIZE)) != -1)	/* versuchen, SeekString zu finden */
			break;

		Fseek(-seekLen, filehandle, 1);	/* SeekLen zur�ck */
		filepos += SEEKBUF_SIZE - seekLen;
	} while (read_bytes == SEEKBUF_SIZE);

	free(seekbuffer);

	Fseek(oldpos, filehandle, 0);

	if (pos != -1)
		filepos += pos;
	else
		filepos = -1;

	return filepos;
}


/* seek_modconf -----------------------------------------------
	Sucht eine MCA (Module Config Area, gekennzeichnet durch 'MCAB')
	zum Modul modinfo in der Datei filehandle (MODCONF.CNF) und positioniert 
	den Dateipointer auf das 'MCAB'.
	Zur�ckgegeben wird die neue Position relativ zur alten vor dem Aufruf.
	-------------------------------------------------------------*/
static long seek_modconf(int filehandle, const MOD_INFO *modinfo)
{
	char SeekString[128] = "MCAB";
	long filepos;

	/*
	 * Suchstring zusammenbasteln 
	 */
	strcat(SeekString, modinfo->mod_name);

	/* lossuchen, R�ckgabe ist -1 wenn nicht gefunden */
	filepos = seekInFile(filehandle, SeekString);
	Fseek(filepos, filehandle, 1);

	return filepos;
}


/* open_modconf_popup ---------------------------------------------------
	l�dt die zum Modul modinfo geh�rigen Konfigurationsnamen aus der MODCONF.CNF,
	�ffnet das Pseudopopup mit den Modulkonfigurationen des Moduls mod_info,
	handled dessen Bedienung durch den User und gibt den darin angeklickten 
	Eintrag zur�ck.
	Scrolling im Popup mu� noch gemacht werden!
	----------------------------------------------------------------------*/
static short open_modconf_popup(const MOD_INFO *modinfo)
{
	char cnfpath[SM_PATH_MAX + 1];
	char *omca;
	char *mca;
	WORD x, y, w, h;
	WORD back;
	short t;
	WORD mx, my, mb, key;
	short mconf_index = 0;
	int fhandle;
	short num_confs;
	char **confnames;
	long mca_len;
	long areaheader_pos;
	long magic;
	long len;

	/*
	 * Position des Dialogs festlegen
	 */
	x = mouse_xpos - modconf_popup->ob_width / 2;
	y = mouse_ypos - modconf_popup->ob_height / 2;
	w = modconf_popup->ob_width;
	h = modconf_popup->ob_height;

	if (x < 2)
		x = 2;
	else if (x > screen.g_w - w - 4)
		x = screen.g_w - w - 4;
	if (y < 2)
		y = 2;
	else if (y > screen.g_h - h - 4)
		x = screen.g_h - h - 4;

	modconf_popup->ob_x = x;
	modconf_popup->ob_y = y;

	/*
	 * Jetzt werden die Konfigurationsnamen aus der Datei geladen
	 * und in confnames[n] eingetragen.
	 */
	strcpy(cnfpath, Sys_info.home_path);
	strcat(cnfpath, "\\modconf.cnf");
	fhandle = (int) Fopen(cnfpath, FO_RW);
	if (fhandle < 0)
		return NEW_CONF;

	areaheader_pos = seek_modconf(fhandle, modinfo);
	if (areaheader_pos == -1)
	{
		Fclose(fhandle);
		return NEW_CONF;				/* f�r dieses Modul keine CNF vorhanden */
	}

	Fseek(-4, fhandle, 1);
	Fread(fhandle, 4, &mca_len);
	omca = mca = SMalloc(mca_len);
	Fread(fhandle, mca_len, mca);
	Fclose(fhandle);

	mca += 4 + strlen(modinfo->mod_name) + 1;
	num_confs = *(short *) mca;			/* Konfigurationsanzahl auslesen */
	mca += 2;

	confnames = (char **) malloc(num_confs * sizeof(confnames[0]));
	memset(confnames, 0x0, num_confs * sizeof(confnames[0]));

	for (t = 0; t < num_confs; t++)
	{
		magic = *(int32_t *) mca;
		mca += 4;

		if (magic == 0x4d434e46L)		/* 'MCNF' */
		{
			confnames[t] = malloc(33);
			strcpy(confnames[t], mca);	/* Konfignamen lesen */
			mca += 33;
			len = *(int32_t *) mca;		/* und seine L�nge */
			mca += 4 + len;
		}
	}

	SMfree(omca);

	/*
	 * dann die Konfigurationen in den Dialog eintragen...
	 */
	for (t = 0; t < 8; t++)
	{
		if (t > num_confs - 1)
		{
			modconf_popup[t + MCONF1].ob_state |= OS_DISABLED;	/* Rest disablen */
			modconf_popup[t + MCONF1].TextCast[0] = 0;
		} else
		{
			modconf_popup[t + MCONF1].ob_state &= ~OS_DISABLED;	/* enablen */
			modconf_popup[t + MCONF1].TextCast = confnames[t];
		}
	}

	/*
	 * ggfs. Scrollbuttons disablen bzw. enablen
	 */
	modconf_popup[MCONF_UP].ob_state |= OS_DISABLED;
	if (num_confs < 8)
		modconf_popup[MCONF_DN].ob_state |= OS_DISABLED;
	else
		modconf_popup[MCONF_DN].ob_state &= ~OS_DISABLED;


	/*
	 * jetzt den Dialog �ffnen und hoffen, da� er dem User gef�llt...
	 */
	wind_update(BEG_UPDATE);
	wind_update(BEG_MCTRL);
	form_dial(FMD_START, x, y, w, h, x, y, w, h);
	objc_draw(modconf_popup, 0, MAX_DEPTH, x, y, w, h);

	do
	{
		evnt_button(1, 1, 1, &mx, &my, &mb, &key);
		back = objc_find(modconf_popup, 0, MAX_DEPTH, mx, my);

		if (back == MCONF_UP || back == MCONF_DN)
		{
			if (num_confs >= 8)
			{
				if (back == MCONF_UP && mconf_index > 0)
					mconf_index--;
				if (back == MCONF_DN && mconf_index < num_confs - 8)
					mconf_index++;
				for (t = 0; t < 8; t++)
				{
					modconf_popup[t + MCONF1].ob_state &= ~OS_DISABLED;	/* Enablen */
					modconf_popup[t + MCONF1].TextCast = confnames[t + mconf_index];
					objc_draw(modconf_popup, MCONF1 + t, MAX_DEPTH, x, y, w, h);
				}
			}
		}

		if (mconf_index <= 0)
			modconf_popup[MCONF_UP].ob_state |= OS_DISABLED;
		else
			modconf_popup[MCONF_UP].ob_state &= ~OS_DISABLED;
		if (mconf_index >= num_confs - 8)
			modconf_popup[MCONF_DN].ob_state |= OS_DISABLED;
		else
			modconf_popup[MCONF_DN].ob_state &= ~OS_DISABLED;

		objc_draw(modconf_popup, MCONF_UP, MAX_DEPTH, x, y, w, h);
		objc_draw(modconf_popup, MCONF_DN, MAX_DEPTH, x, y, w, h);
	} while (back == MCONF_UP || back == MCONF_DN ||
			 modconf_popup[back].ob_state & OS_DISABLED || (!IsSelectable(modconf_popup[back]) && back != -1));

	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	wind_update(END_MCTRL);
	wind_update(END_UPDATE);

	for (t = 0; t < num_confs; t++)
		free(confnames[t]);
	free(confnames);

	if (back == -1)
		return -1;

	return back + mconf_index;
}


/* mconfLoad --------------------------------------------
	�ffnet das Pseudopopup und l�dt eine Konfiguration. Diese wird
	an das Modul zur�ckgegeben.
	------------------------------------------------------*/
void *mconfLoad(const MOD_INFO *modinfo, short mod_id, char *name)
{
	char cnfname[33];
	short back;
	void *block = NULL;

	(void) mod_id;
	(void) name;

	modconf_popup[NEW_CONF].ob_state |= OS_DISABLED;
	back = open_modconf_popup(modinfo);	/* erstmal das Popup auf */
	modconf_popup[NEW_CONF].ob_state &= ~OS_DISABLED;
	if (back <= 0)
		return NULL;

	if (back == NEW_CONF)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_NOCONF].TextCast, NULL, NULL, NULL, 1);
	else
	{
		strcpy(cnfname, modconf_popup[back].TextCast);
		block = load_from_modconf(modinfo, cnfname, &back, MOD_MAGIC_EDIT);
	}

	return block;
}


/* expandFile -----------------------------------------
	Macht in der Datei handle an der aktuellen Fileposition len bytes 
	Platz. Der Dateipointer wird nicht ver�ndert. Dateipuffer ist 2048
	Bytes, sollte vielleicht noch gr��er gew�hlt werden?
	----------------------------------------------------*/
static void expandFile(int handle, long len)
{
	char *buffer;
	long bytes;
	long oldpos;

	if (len == 0)
		return;

	oldpos = Fseek(0, handle, 1);

	bytes = Fseek(0, handle, 2) - oldpos;	/* wieviel Speicher wird gebraucht? */
	buffer = SMalloc(bytes + len);		/* soviel + len Bytes anfordern */
	Fseek(oldpos, handle, 0);			/* und wieder zur�ck */

	Fread(handle, bytes, buffer);		/* jetzt Rest puffern */
	Fwrite(handle, len, buffer);		/* len Bytes Platz machen */

	Fseek(-bytes, handle, 1);			/* Rest Bytes zur�ck */
	Fwrite(handle, bytes, buffer);		/* und Puffer wegschreiben */

	Fseek(oldpos, handle, 0);			/* und wieder zur�ck */

	SMfree(buffer);
}


/* save_to_modconf ------------------------------------------------------------
	Speichert den Konfigurationsbock *confblock des Moduls modinfo/mod_id mit
	der L�nge len unter dem Namen name in der MODCONF.CNF. 
	Wird von mconfSave() aufgerufen!
	-------------------------------------------------------------------------*/
static void save_to_modconf(const MOD_INFO *modinfo, void *confblock, long len, char *name, long type)
{
	char cnfpath[SM_PATH_MAX + 1];
	int filehandle;
	short num_confs;
	long mca_len = 0;
	long pos;
	long areaname_len;
	long areaheader_pos;

	strcpy(cnfpath, Sys_info.home_path);
	strcat(cnfpath, "\\modconf.cnf");

	/*
	 * MODCONF.CNF �ffnen oder erzeugen
	 */
	filehandle = (int) Fopen(cnfpath, FO_RW);
	if (filehandle < 0)
	{
		filehandle = (int) Fcreate(cnfpath, 0);
		if (filehandle < 0)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_CREATEERR].TextCast, NULL, NULL, NULL, 1);
			return;
		}
		pos = -1;
	} else
	{
		/*
		 * Modulbereich in der cnf suchen
		 */
		pos = seek_modconf(filehandle, modinfo);
	}

	/*
	 * in vorhandenen Filebereich des Moduls (sofern gefunden)
	 * einen neuen Eintrag einf�gen
	 */
	if (pos != -1)
	{
		Fseek(-4, filehandle, 1);
		Fread(filehandle, 4, &mca_len);

		/*
		 * zuerst das Word f�r die Konfigurationsanzahl erh�hen
		 */
		areaheader_pos = Fseek(0, filehandle, 1);
		areaname_len = strlen(modinfo->mod_name) + 1;
		Fseek(4 + areaname_len, filehandle, 1);
		Fread(filehandle, 2, &num_confs);
		num_confs++;
		Fseek(-2, filehandle, 1);
		Fwrite(filehandle, 2, &num_confs);
		Fseek(areaheader_pos + mca_len - 4, filehandle, 0);	/* ans Ende des MCA stellen */

		/*
		 * dann Platz in der Datei machen und die neue Config schreiben
		 */
		expandFile(filehandle, 4 + 33 + 4 + len);
		Fwrite(filehandle, 4, "MCNF");	/* "Module Configuration" */
		Fwrite(filehandle, 33, name);	/* Konfigurationsname schreiben */
		Fwrite(filehandle, 4, &len);	/* L�nge schreiben */
		Fwrite(filehandle, len, confblock);	/* Konfigurationsblock schreiben */
		mca_len = Fseek(0, filehandle, 1) + 4 - areaheader_pos;
		Fseek(areaheader_pos - 4, filehandle, 0);
		Fwrite(filehandle, 4, &mca_len);	/* MCA-L�nge schreiben */
	}
	/*
	 * oder einen neuen Bereich f�r das Modul an die Datei anh�ngen?
	 */
	else
	{
		Fseek(0, filehandle, 2);

		pos = Fwrite(filehandle, 4, &mca_len);	/* MCA-L�nge initial schreiben */

		areaheader_pos = Fseek(0, filehandle, 1);
		Fwrite(filehandle, 4, "MCAB");	/* "Module Conf. Area Begin" oder so. */
		areaname_len = strlen(modinfo->mod_name) + 1;
		Fwrite(filehandle, areaname_len, modinfo->mod_name);	/* Modulname inkl. Nullbyte schreiben */
		num_confs = 1;
		Fwrite(filehandle, 2, &num_confs);	/* Konf-Anzahl schreiben */

		Fwrite(filehandle, 4, "MCNF");	/* "Module Configuration" */
		if (type == MOD_MAGIC_EDIT)
			Fwrite(filehandle, 33, name);	/* Konfigurationsname inkl. Nullbyte schreiben */
		Fwrite(filehandle, 4, &len);	/* L�nge schreiben */
		Fwrite(filehandle, len, confblock);	/* Konfigurationsblock schreiben */

		Fwrite(filehandle, 4, "MCAE");	/* "Module Conf. Area End" oder so. */

		mca_len = Fseek(0, filehandle, 1) - areaheader_pos;
		Fseek(areaheader_pos - 4, filehandle, 0);
		Fwrite(filehandle, 4, &mca_len);	/* MCA-L�nge schreiben */
	}

	Fclose(filehandle);
}


/* overwriteMCNF ----------------------------------------------------
	�berschreibt eine Modulkonfiguration mit dem Index num in der Datei
	mit dem neuen Block confblock der L�nge newlen.
	-------------------------------------------------------------------*/
static short overwriteMCNF(const MOD_INFO *modinfo, char *confblock, long newlen, char *name, short num, long type)
{
	char SeekString[37] = "MCNF";
	char cnfpath[SM_PATH_MAX + 1];
	uint8_t *mca;
	int filehandle;
	short num_confs;
	long back;
	long mca_len;
	long len;
	long areaheader_pos;

	strcpy(cnfpath, Sys_info.home_path);
	strcat(cnfpath, "\\modconf.cnf");

	/*
	 * erstmal die Datei auf
	 */
	filehandle = (int) Fopen(cnfpath, FO_RW);
	if (filehandle < 0)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_OPENERR].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}

	areaheader_pos = seek_modconf(filehandle, modinfo);
	if (areaheader_pos == -1)
	{
		Fclose(filehandle);
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_READERR].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}

	Fseek(-4, filehandle, 1);
	Fread(filehandle, 4, &mca_len);
	mca = SMalloc(mca_len);
	Fread(filehandle, mca_len, mca);

	num_confs = *(short *) (mca + 4 + strlen(modinfo->mod_name) + 1);	/* Konfigurationsanzahl auslesen */

	/*
	 * ist die Nummer �berhaupt m�glich?
	 */
	if (num > num_confs)
	{
		SMfree(mca);
		Fclose(filehandle);
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_NOTFOUND].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}

	strcat(SeekString, name);
	back = Idxtab(mca, SeekString, mca_len);
	SMfree(mca);

	if (back == -1)						/* eigentlich unm�glich */
	{
		Fclose(filehandle);
		return -1;
	}

	Fseek(areaheader_pos + back + 4, filehandle, 0);	/* bis zum MCNF und noch dr�ber */

	if (type == MOD_MAGIC_EDIT)
		Fread(filehandle, 33, name);	/* Konfignamen lesen */
	Fread(filehandle, 4, &len);			/* und seine L�nge */

	/*
	 * und jetzt die neue Konfiguration schreiben. Wenn newlen gr��er ist
	 * als die in der Datei f�r die zu �berschreibende Konfiguration angegebene
	 * L�nge, wird Platz in der Datei gemacht.
	 */
	Fseek(-4, filehandle, 1);
	if (newlen > len)
		expandFile(filehandle, newlen - len);
	Fwrite(filehandle, 4, &newlen);		/* L�nge schreiben */
	Fwrite(filehandle, newlen, confblock);	/* Konfigurationsblock schreiben */

	Fclose(filehandle);

	return 0;
}


/* Testet ob der Konfigname schon vergeben f�r dieses Modul */
/* R�ckgabe 0: alles ist klar, 1: schon vergeben */
static short nametest(const MOD_INFO *modinfo, const char *name)
{
	char cnfpath[SM_PATH_MAX + 1];
	char SeekString[37] = "MCNF";
	uint8_t *mca;
	int filehandle;
	long back;
	long mca_len = 0;

	strcpy(cnfpath, Sys_info.home_path);
	strcat(cnfpath, "\\modconf.cnf");

	/*
	 * MODCONF.CNF �ffnen
	 */
	filehandle = (int) Fopen(cnfpath, FO_READ);
	if (filehandle < 0)
		return 0;

	/*
	 * Modulbereich in der cnf suchen
	 */
	if (seek_modconf(filehandle, modinfo) == -1)
	{
		Fclose(filehandle);
		return 0;
	} else
	{
		Fseek(-4, filehandle, 1);
		Fread(filehandle, 4, &mca_len);
		mca = SMalloc(mca_len);
		Fread(filehandle, mca_len, mca);
		Fclose(filehandle);

		strcat(SeekString, name);
		back = Idxtab(mca, SeekString, mca_len);

		SMfree(mca);

		if (back == -1)
			return 0;
		else
			return 1;
	}
}


/* mconfSave --------------------------------------------
	Die Dienstfunktion, die vom Modul aufgerufen wird.
	�ffnet das Pseudopopup mit den Modulkonfigs und dann ggfs. den
	Dialog zur Eingabe eines neuen Namens, dann wird die Konfig
	in die MONDCONF.CNF gespeichert. Zur�ckgegeben wird ein Zeiger
	auf den vom User eingegebenen Namen (max. 32 Zeichen).
	------------------------------------------------------*/
void mconfSave(const MOD_INFO *modinfo, short mod_id, void *confblock, long len, char *name)
{
	char cnfname[33];
	short back;
	WORD x, y, w, h;

	(void) mod_id;
	(void) name;

	back = open_modconf_popup(modinfo);	/* Erstmal das Popup auf */
	if (back <= 0)
		return;

	/*
	 * Es soll eine neue Konfiguration gesichert werden?
	 * (-1 hei�t, es gibt noch gar keine, also auf jeden Fall eine neue) 
	 */
	if (back == NEW_CONF)
	{
	  again:
		x = confsave_dialog->ob_x - 2;
		y = confsave_dialog->ob_y - 2;
		w = confsave_dialog->ob_width + 4;
		h = confsave_dialog->ob_height + 4;

		wind_update(BEG_UPDATE);
		wind_update(BEG_MCTRL);
		form_dial(FMD_START, x, y, w, h, x, y, w, h);

		/* damit das Feld leer ist und der Cursor vorne steht */
		strcpy(confsave_dialog[MODCONF_NAME].TextCast, "");

		do
		{
			objc_draw(confsave_dialog, 0, MAX_DEPTH, x, y, w, h);
			back = form_do(confsave_dialog, MODCONF_NAME);
			confsave_dialog[back].ob_state &= ~OS_SELECTED;
		} while ((back != MODCONF_SAVE || strlen(confsave_dialog[MODCONF_NAME].TextCast) == 0) &&
				 back != MODCONF_CANCEL);

		form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
		wind_update(END_MCTRL);
		wind_update(END_UPDATE);


		if (back == MODCONF_SAVE)
		{
			strcpy(cnfname, confsave_dialog[MODCONF_NAME].TextCast);

			if (nametest(modinfo, cnfname))
			{
				back =
					Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_EXISTS].TextCast, "Nein", " Ja ", NULL, 1);
				if (back == 1)			/* nicht �berschreiben */
					goto again;
				else					/* �berschreiben */
					overwriteMCNF(modinfo, confblock, len, cnfname, 0, MOD_MAGIC_EDIT);
			} else
			{
				save_to_modconf(modinfo, confblock, len, cnfname, MOD_MAGIC_EDIT);
			}
		}
	} else
	{
		strcpy(cnfname, modconf_popup[back].TextCast);
		overwriteMCNF(modinfo, confblock, len, cnfname, back, MOD_MAGIC_EDIT);
	}
}


/* load_from_modconf -------------------------------------------------------------
	L�dt die Konfiguration Nummer <num> f�r das Modul modinfo aus
	der MODCONF.CNF. Der Name der Konfig wird in name kopiert.
	Ein Zeiger auf den geladenen Konfigurationsblock (SMalloc) wird zur�ckgegeben
	und in num kommt die L�nge zur�ck.
	-------------------------------------------------------------------------------*/
void *load_from_modconf(const MOD_INFO *modinfo, char *name, short *num, long type)
{
	char SeekString[37] = "MCNF";
	char cnfpath[SM_PATH_MAX + 1];
	uint8_t *mca;
	char *block;
	int fhandle;
	short num_confs;
	long back;
	long mca_len;
	long len;
	long areaheader_pos;

	strcpy(cnfpath, Sys_info.home_path);
	strcat(cnfpath, "\\modconf.cnf");

	/*
	 * MODCONF.CNF �ffnen
	 */
	fhandle = (int) Fopen(cnfpath, FO_RW);
	if (fhandle < 0)
	{
		if (type == MOD_MAGIC_EDIT)
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_OPENERR].TextCast, NULL, NULL, NULL, 1);
		return NULL;					/* f�r dieses Modul keine CNF vorhanden */
	}

	areaheader_pos = seek_modconf(fhandle, modinfo);
	if (areaheader_pos == -1)
	{
		Fclose(fhandle);
		if (type == MOD_MAGIC_EDIT)
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_READERR].TextCast, NULL, NULL, NULL, 1);
		return NULL;					/* f�r dieses Modul keine CNF vorhanden */
	}

	Fseek(-4, fhandle, 1);
	Fread(fhandle, 4, &mca_len);
	mca = SMalloc(mca_len);
	Fread(fhandle, mca_len, mca);
	Fclose(fhandle);

	num_confs = *(short *) (mca + 4 + strlen(modinfo->mod_name) + 1);	/* Konfigurationsanzahl auslesen */

	/*
	 * ist die Nummer �berhaupt m�glich?
	 */
	if (*num > num_confs)
	{
		SMfree(mca);
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MCONF_NOTFOUND].TextCast, NULL, NULL, NULL, 1);
		return NULL;
	}

	strcat(SeekString, name);
	back = Idxtab(mca, SeekString, mca_len);

	if (back == -1)
		return NULL;

	strcpy(name, (char *)mca + back + 4);		/* Konfignamen lesen */
	if (type == MOD_MAGIC_EDIT)
		len = *(int32_t *) (mca + back + 4 + 33);	/* und seine L�nge */
	else
		len = *(int32_t *) (mca + back + 4);	/* und seine L�nge */
	*num = (short) len;

	block = SMalloc(len);
	if (type == MOD_MAGIC_EDIT)
		memcpy(block, mca + back + 4 + 33 + 4, len);	/* und schlie�lich die Config */
	else
		memcpy(block, mca + back + 4 + 4, len);	/* und schlie�lich die Config */

	SMfree(mca);

	return block;
}


/* memorize_emodConfig ----------------------------------------------------------
	"merkt" sich die vom Modul modbase geschickten Default-Konfigurationsblock
	cnfblock (wird bei M_EXIT mitgeschickt) mit der L�nge length in den
	Feldern edit_cnfblock[] und edit_cnflen[].
	-------------------------------------------------------------------------*/
void memorize_emodConfig(BASPAG *modbase, GARGAMEL *smurf_struct)
{
	char cmp_modname[30];
	const MODULE_START *module_start;
	void *cnfblock;
	short index;
	const MOD_INFO *modinfo;
	void **pp;
	
	/*
	 * Erstmal mu� der passende Index gefunden werden.
	 * Dazu nehmen wir uns den Namen des Moduls und suchen
	 * ihn unter den eingetragenen Editmodulen
	 */
	module_start = get_module_start(modbase);
	modinfo = module_start->info;

	strcpy(cmp_modname, modinfo->mod_name);
	if (strlen(cmp_modname) < 28)
		strncat(cmp_modname, "                           ", 28 - strlen(cmp_modname));

	for (index = 0; index < Dialog.emodList.anzahl; index++)
	{
		if (strcmp(Dialog.emodList.modNames[index], cmp_modname) == 0)
			break;
	}

	if (edit_cnfblock[index] != NULL)
		free(edit_cnfblock[index]);

	pp = (void **) &smurf_struct->event_par[0];
	cnfblock = *pp;
	edit_cnflen[index] = smurf_struct->event_par[2];

	edit_cnfblock[index] = malloc(edit_cnflen[index]);
	memcpy(edit_cnfblock[index], cnfblock, edit_cnflen[index]);

	SMfree(cnfblock);
}


/* memorize_expmodConfig ----------------------------------------------------------
	"merkt" sich die vom Modul modbase geschickten Default-Konfigurationsblock
	cnfblock (wird bei M_EXIT mitgeschickt) mit der L�nge length in den
	Feldern export_cnfblock[] und export_cnflen[].
	-------------------------------------------------------------------------*/
void memorize_expmodConfig(BASPAG *modbase, GARGAMEL *smurf_struct, char save)
{
	char cmp_modname[30];
	const MODULE_START *module_start;
	char *cnfblock;
	short index;
	short length;
	const MOD_INFO *modinfo;
	void **pp;

	/*
	 * Erstmal mu� der passende Index gefunden werden.
	 * Dazu nehmen wir uns den Namen des Moduls und suchen
	 * ihn unter den eingetragenen Exportmodulen
	 */
	module_start = get_module_start(modbase);
	modinfo = module_start->info;

	strcpy(cmp_modname, modinfo->mod_name);
	if (strlen(cmp_modname) < 28)
		strncat(cmp_modname, "                           ", 28 - strlen(cmp_modname));

	for (index = 0; index < Dialog.expmodList.anzahl; index++)
	{
		if (strcmp(Dialog.expmodList.modNames[index], cmp_modname) == 0)
			break;
	}

	if (export_cnfblock[index] != NULL)
		free(export_cnfblock[index]);

	pp = (void **)&smurf_struct->event_par[0];
	cnfblock = *pp;
	length = smurf_struct->event_par[2];
	if (!save)
	{
		export_cnflen[index] = length;
		export_cnfblock[index] = malloc(export_cnflen[index]);
		memcpy(export_cnfblock[index], cnfblock, export_cnflen[index]);
	} else if (nametest(modinfo, ""))
	{
		overwriteMCNF(modinfo, cnfblock, length, "", 0, MOD_MAGIC_EXPORT);
	} else
	{
		save_to_modconf(modinfo, cnfblock, length, "", MOD_MAGIC_EXPORT);
	}
}


/* transmitConfig ---------------------------------------------
	schickt einen mit memorize_emodConfig() gemerkten Konfigurationsblock
	(wenn �berhaupt einer gemerkt wurde) ans Modul modbase/smurf_struct
	zur�ck. Das Modul mu� diesen dann in seine internen Strukturen kopieren
	und entsprechend reagieren (Dialogupdate). Antwort vom Modul soll entweder
	M_WAITING oder M_MODPIC sein (wobei letzteres momentan noch nicht funkt!).
	---------------------------------------------------------------------*/
void transmitConfig(BASPAG *modbase, GARGAMEL *smurf_struct)
{
	const MODULE_START *module_start;
	char cmp_modname[30];
	short index;
	const MOD_INFO *modinfo;

	/*
	 * Erstmal mu� der passende Index gefunden werden.
	 * Dazu nehmen wir uns den Namen des Moduls und suchen
	 * ihn unter den eingetragenen Editmodulen
	 */
	module_start = get_module_start(modbase);
	modinfo = module_start->info;

	strcpy(cmp_modname, modinfo->mod_name);
	if (strlen(cmp_modname) < 28)
		strncat(cmp_modname, "                           ", 28 - strlen(cmp_modname));

	for (index = 0; index < Dialog.emodList.anzahl; index++)
	{
		if (strcmp(Dialog.emodList.modNames[index], cmp_modname) == 0)
			break;
	}

	if (edit_cnfblock[index])
	{
		void **pp = (void **)&smurf_struct->event_par[0];
		*pp = edit_cnfblock[index];
		smurf_struct->event_par[2] = edit_cnflen[index];

		module.comm.start_edit_module("", modbase, CONFIG_TRANSMIT, smurf_struct->module_number, smurf_struct);
		f_handle_modmessage(smurf_struct);
	}
}
