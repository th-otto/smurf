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

/******************************************************************	*/
/*						SMURF Grafikkonverter						*/
/*	Alle Funktionen zum Generieren der Modul-Listen in der globalen	*/
/*	SYSTEM_INFO-Struktur.											*/
/*																	*/
/*	Ausgelagert und umgestellt auf systemunabh�ngige Filefunktionen	*/
/*	16.03.97 - 19.03.97 Christian									*/
/*																	*/
/******************************************************************	*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "import.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurf.h"
#include "globdefs.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"
#include "debug.h"
#include "plugin.h"
#include "smplugin.h"

static short files_read;

/* ----------------------------------------------------------------	*/
/*----------------- Edit-Module suchen und eintragen ---------------*/
/* Der Ordner MODULES\\EDIT wird nach Files mit der Extension		*/
/* .SEM durchsucht. Die Edit-Module sind in Gruppen unterteilt, 	*/
/* f�r die repr�sentativ die Unterordner stehen, in dem sie sich	*/
/* befinden.                                                        */
/* ----------------------------------------------------------------	*/
void f_scan_edit(void)
{
	char *editpath;						/* voller Modulpfad, Original */
	char *edit_path;					/* voller Modulpfad, editable */
	char *swapstr;
	char alert[128];
	const MODULE_START *module_start;
	char edstring[64];
	char strn[4];
	short t;
	short tt;
	short biggest;
	short pathlen;
	long entrlen;
	long temp;
	BASPAG *edit_baspag;
	const MOD_INFO *module_info;
	struct DIRENTRY *filelist;
	struct DIRENTRY *actual;

	DEBUG_MSG(("Lade Edit-Module\n"));

	/*---- Pfade vorbereiten ----*/
	editpath = calloc(1, strlen(Sys_info.standard_path) + strlen("\\modules\\edit\\") + 1);
	strcpy(editpath, Sys_info.standard_path);
	strcat(editpath, "\\modules\\edit\\");

	Name_Max = get_maxnamelen(editpath);

	pathlen = (short) (strlen(editpath) + Name_Max);
	edit_path = (char *) calloc(1, pathlen + 1);
	swapstr = (char *) calloc(1, pathlen + 1);

	filelist = build_up_filelist(editpath, "sem", pathlen);

	Dialog.emodList.anzahl = 0;

	actual = filelist;
	while (actual != NULL)
	{
		/*
		 * Modul laden und Basepage ermitteln
		 */
		strcpy(edit_path, editpath);
		strcat(edit_path, actual->modname);

		temp = Pexec(3, edit_path, "", NULL);
		if (temp <= 0)
		{
			DEBUG_MSG(("  Fehler bei %s: %li\n", actual->modname, temp));
			if (temp == EACCDN)
			{
				sprintf(alert, "[1][Modul|%s|besitzt keine Berechtigung|ausgef�hrt zu werden|(Exec-Flag?)!][ OK ]", actual->modname);	/* FIXME: translate */
			} else
			{
				sprintf(alert, "[1][Fehler in File|%s|im Ordner|\\modules\\edit\\!][ OK ]", actual->modname);	/* FIXME: translate */
			}

			form_alert(1, alert);
		} else
		{
			edit_baspag = (BASPAG *) temp;

			module_start = get_module_start(edit_baspag);	/* Textsegment-Startadresse holen */
			module_info = module_start->info;

			/* Zeiger auf Magic (mu� MOD_MAGIC_EDIT sein!) */
			if (module_start->magic != MOD_MAGIC_EDIT)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_MODULE], actual->modname, "edit", "Editmodul");
				form_alert(1, alert);
			} else if (module_info->compiler_id != COMPILER_ID)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_COMPILER], actual->modname, "edit");
				form_alert(1, alert);
			} else
			{
				/*
				 * L�nge des gesamten Tochterprozesses ermitteln
				 */
				start_module(edit_baspag);

				if (Dialog.emodList.anzahl < MAX_EDIT_MODS)
				{
					/*
					 * Modul eintragen
					 */
					edit_modules[Dialog.emodList.anzahl] = malloc(pathlen + 1);	/* keine variable L�nge wegen Sortierung! */
					strcpy(edit_modules[Dialog.emodList.anzahl], edit_path);
	
					Dialog.emodList.modNames[Dialog.emodList.anzahl] = (char *) calloc(1, 29);
					strncpy(Dialog.emodList.modNames[Dialog.emodList.anzahl], module_info->mod_name, 28);
	
					entrlen = strlen(Dialog.emodList.modNames[Dialog.emodList.anzahl]);
					if (entrlen < 28)
						strncat(Dialog.emodList.modNames[Dialog.emodList.anzahl], "                           ", 28 - entrlen);
	
					Dialog.emodList.anzahl++;
	
					/*---- gescante Module im Startupdialog hochz�hlen */
					if ((Dialog.emodList.anzahl & 3) == 0)
					{
						/* FIXME: translate */
						strcpy(edstring, itoa(Dialog.emodList.anzahl, strn, 10));
						strcat(edstring, " Editmodule");
						set_startupdial(edstring);
					}
	
					DEBUG_MSG(("  --> Name   : %s\n", module_info->mod_name));
					DEBUG_MSG(("      File   : %s\n", actual->modname));
					DEBUG_MSG(("      Version: %x\n", module_info->version));
					DEBUG_MSG(("      Autor  : %s\n", module_info->autor));
				}
			}

#if 0
			Pexec(102, NULL, edit_baspag, NULL);
#endif
			SMfree(edit_baspag->p_env);
			SMfree(edit_baspag);
		}

		actual = actual->next;
	}

	/* FIXME: translate */
	strcpy(edstring, itoa(Dialog.emodList.anzahl, strn, 10));
	strcat(edstring, " Editmodule");
	set_startupdial(edstring);

	/*---------------- Liste alphabetisch sortieren -------------*/
	for (t = 0; t < Dialog.emodList.anzahl; t++)
	{
		biggest = t;
		for (tt = t + 1; tt < Dialog.emodList.anzahl; tt++)
			if (stricmp(Dialog.emodList.modNames[tt], Dialog.emodList.modNames[biggest]) < 0)
				biggest = tt;

		strncpy(swapstr, Dialog.emodList.modNames[t], 29);
		strncpy(Dialog.emodList.modNames[t], Dialog.emodList.modNames[biggest], 29);
		strncpy(Dialog.emodList.modNames[biggest], swapstr, 29);
		strcpy(swapstr, edit_modules[t]);
		strcpy(edit_modules[t], edit_modules[biggest]);
		strcpy(edit_modules[biggest], swapstr);
	}

	destroy_filelist(filelist);

	free(swapstr);
	free(edit_path);
	free(editpath);
}


/* ----------------------------------------------------------------	*/
/*		Extensionen eines Importers in Import_list speichern		*/
/* ----------------------------------------------------------------	*/
static void save_extensions(const MOD_INFO *module_info)
{
	int t;

	t = 0;
	do
	{
		Import_list.mod_exts[t][anzahl_importmods] = strdup(module_info->ext[t]);
		strupr(Import_list.mod_exts[t][anzahl_importmods]);
	} while (++t < 10);
}



/******************************************************************	*/
/*					Importerliste speichern							*/
/******************************************************************	*/
static void save_import_list(void)
{
	char *string;
	char listpath[SM_PATH_MAX];
	short len;
	int filehandle;
	short t;
	short extnum;
	short werror = 0;
	short bh;
	short bl;

	strcpy(listpath, Sys_info.home_path);
	strcat(listpath, "\\ilist.dat");

	if ((filehandle = (int) Fcreate(listpath, 0)) < 0)
	{
		Dialog.busy.reset(128, "Error");
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[CNF_SAVEERR].TextCast, NULL, NULL, NULL, 1);
	} else
	{
		Dialog.busy.reset(0, "Liste speichern...");

		if (Fwrite(filehandle, 2, &anzahl_importmods) != 2)	/* Anzahl speichern */
			werror = 1;

		if (!werror)
		{
			bh = anzahl_importmods / 6;	/* busy-height */
			bl = 0;						/* busy-length */

			for (t = 0; t < anzahl_importmods; t++)
			{
				if (!(t % bh))
				{
					Dialog.busy.draw(bl);
					bl += 10;
				}

				/* Modulname speichern */
				string = Import_list.imp_mod_list[t];
				len = (short) strlen(string) + 1;
				if (Fwrite(filehandle, len, string) != len)
				{
					werror = 1;
					break;
				}
			}
		}

		if (!werror)
		{
			for (t = 0; t < anzahl_importmods; t++)
			{
				if (!(t % bh))
				{
					Dialog.busy.draw(bl);
					bl += 11;
				}

				for (extnum = 0; extnum < 10; extnum++)
				{
					string = Import_list.mod_exts[extnum][t];
					len = (char) strlen(string) + 1;
					if (Fwrite(filehandle, len, string) != len)	/* EXT speichern */
					{
						werror = 1;
						break;
					}
				}
			}
		}

		Fclose(filehandle);
	}

	if (werror)
		Dialog.winAlert.openAlert("Fehler beim Schreiben der Importerliste!", NULL, NULL, NULL, 1);
	else
		Dialog.busy.ok();
}


/* ----------------------------------------------------------------	*/
/*----------------- Import-Module suchen und eintragen -------------*/
/*	Durchsucht den Ordner MODULES\\IMPORT nach allen Importmodulen	*/
/*	und tr�gt diese mit deren unterst�tzten Extensionen in die ent- */
/*	sprechenden Arrays ein. Beim Bild-Laden wird dann zuerst diese	*/
/*	Liste nach der Extension des zu ladenden Files durchsucht und 	*/
/*	alle Importer aufgerufen, die diese Extension unterst�tzen.		*/
/*	Fruchtet dies nicht, werden die Importer der Reihe nach			*/
/*	aufgerufen, falls die Extension des Files nicht stimmt.			*/
/* ----------------------------------------------------------------	*/
void f_scan_import(void)
{
	char *importpath;					/* voller Modulpfad, Original */
	char *import_path;					/* voller Modulpfad, editable */
	const MODULE_START *module_start;
	char alert[128];
	short pathlen;
	short bh;
	short bl;
	short anzahl_extensions;
	short t;
	short tt;
	long temp;
	BASPAG *import_baspag;
	const MOD_INFO *module_info;
	struct DIRENTRY *filelist;
	struct DIRENTRY *actual;

	DEBUG_MSG(("Lade Import-Module...\n"));

	/*---- Pfade vorbereiten ----*/
	importpath = calloc(1, strlen(Sys_info.standard_path) + strlen("\\modules\\import\\") + 1);
	strcpy(importpath, Sys_info.standard_path);
	strcat(importpath, "\\modules\\import\\");

	Name_Max = get_maxnamelen(importpath);

	pathlen = (short) (strlen(importpath) + Name_Max);
	import_path = (char *) calloc(1, pathlen + 1);

	files_read = 0;
	filelist = build_up_filelist(importpath, "sim", pathlen);

	anzahl_importmods = 0;
	anzahl_extensions = 0;

	Dialog.busy.reset(0, "Importmodule...");

	bh = files_read / 8;				/* busy-height */
	bl = 0;								/* busy-length */

	/* bisherige Liste freigeben */
	free(Import_list.imp_mod_list[0]);

	files_read = 0;
	actual = filelist;
	while (actual != NULL)
	{
		if (!(files_read % bh))
		{
			Dialog.busy.draw(bl);
			bl += 16;
		}

		/*---- Modul laden und Basepage ermitteln */
		strcpy(import_path, importpath);
		strcat(import_path, actual->modname);

		temp = Pexec(3, import_path, "", NULL);
		if (temp <= 0)
		{
			DEBUG_MSG(("  Fehler bei %s: %li\n", actual->modname, temp));
			sprintf(alert, "[1][Fehler in File|%s|im Ordner|\\modules\\import\\!][ OK ]", actual->modname);	/* FIXME: translate */
			form_alert(1, alert);
		} else
		{
			import_baspag = (BASPAG *) temp;

			module_start = get_module_start(import_baspag);	/* Textsegment-Startadresse holen */
			module_info = module_start->info;

			/* Zeiger auf Magic (mu� MOD_MAGIC_IMPORT sein!) */
			if (module_start->magic != MOD_MAGIC_IMPORT)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_MODULE], actual->modname, "import", "importmodul");
				form_alert(1, alert);
			} else if (module_info->compiler_id != COMPILER_ID)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_COMPILER], actual->modname, "import");
				form_alert(1, alert);
			} else
			{
				/*---- L�nge des gesamten Tochterprozesses ermitteln */
				start_module(import_baspag);

				/*---- Modul eintragen */
				if (anzahl_importmods < MAX_IMPORT_MODS)
				{
					Import_list.imp_mod_list[anzahl_importmods] = strdup(actual->modname);
	
					/*---- Extensionen merken */
					save_extensions(module_info);
	
					for (t = 0; t < 10; t++)
					{
						if (strlen(Import_list.mod_exts[t][anzahl_importmods]) != 0)
							anzahl_extensions++;
					}
	
					anzahl_importmods++;
	
					DEBUG_MSG(("  --> Name   : %s\n", module_info->mod_name));
					DEBUG_MSG(("      File   : %s\n", actual->modname));
					DEBUG_MSG(("      Version: %x\n", module_info->version));
					DEBUG_MSG(("      Autor  : %s\n", module_info->autor));
				}
			}

#if 0
			Pexec(102, NULL, import_baspag, NULL);
#endif
			SMfree(import_baspag->p_env);
			SMfree(import_baspag);
		}

		actual = actual->next;
		files_read++;
	}									/* while(actual != NULL) */

	if (filelist == NULL)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_IMODS_FOUND].TextCast, NULL, NULL, NULL, 1);

	save_import_list();

	destroy_filelist(filelist);

	free(import_path);
	free(importpath);

	/*
	 * reservierten Speicher wieder freigeben
	 */
	for (t = 0; t < anzahl_importmods; t++)
	{
		free(Import_list.imp_mod_list[t]);

		for (tt = 0; tt < 10; tt++)
			free(Import_list.mod_exts[tt][t]);
	}

	load_import_list();

	DEBUG_MSG(("Lade Import-Module...Ende\n"));
}



/******************************************************************	*/
/*						Importerliste laden							*/
/******************************************************************	*/
int load_import_list(void)
{
	char *buf;
	char listpath[SM_PATH_MAX];
	int filehandle;
	short t;
	short extnum;
	long f_len;

	DEBUG_MSG(("load_import_list...\n"));

	strcpy(listpath, Sys_info.home_path);
	strcat(listpath, "\\ilist.dat");

	if ((filehandle = (int) Fopen(listpath, FO_READ)) < 0)
	{
		DEBUG_MSG(("load_import_list...Ende -1\n"));
		return -1;
	} else
	{
		Dialog.busy.reset(0, "ILIST.DAT laden...");

		f_len = Fseek(0, filehandle, 2) - 2;
		Fseek(0, filehandle, 0);

		Fread(filehandle, 2, &anzahl_importmods);	/* Anzahl Importer auslesen */

		buf = (char *) malloc(f_len);
		Fread(filehandle, f_len, buf);

		/*---------------------------------------------- Importer-Pfade laden ------------*/
		for (t = 0; t < anzahl_importmods; t++)
		{
			Import_list.imp_mod_list[t] = buf;
			buf += strlen(buf) + 1;
		}

		/*---------------------------------------------- Importer-Extensions laden ------------*/
		for (t = 0; t < anzahl_importmods; t++)
		{
			for (extnum = 0; extnum < 10; extnum++)
			{
				Import_list.mod_exts[extnum][t] = buf;
				buf += strlen(buf) + 1;
			}
		}

		Fclose(filehandle);
	}

	Dialog.busy.ok();

	DEBUG_MSG(("load_import_list...Ende\n"));

	return 0;
}



/******************************************************************	*/
/*					Importerliste durchsuchen						*/
/*	Sucht die Importerlisten nach einer Extension ab, startet das	*/
/*	dazugeh�rige Importmodul und liefert dessen R�ckgabe zur�ck.	*/
/*	Wird '*' als Extension �bergeben, werden alle Importer 			*/
/*	durchlaufen.													*/
/******************************************************************	*/
int seek_module(SMURF_PIC *picture, char *extension)
{
	char modpath[SM_PATH_MAX + 1];
	char new_ext[5];
	short t = 0;
	short seek;
	short mod_ret = M_INVALID;

	if (anzahl_importmods <= 0)
		return M_INVALID;

	strncpy(new_ext, extension, 5);
	strupr(new_ext);

	if (*extension == '*')
	{
		t = 0;
		do
		{
			Dialog.busy.reset(0, Import_list.imp_mod_list[t]);
			strncpy(modpath, Sys_info.standard_path, sizeof(modpath));
			strcat(modpath, "\\modules\\import\\");
			strcat(modpath, Import_list.imp_mod_list[t]);
			if ((mod_ret = module.comm.start_imp_module(modpath, picture)) == M_STARTERR)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
		} while (++t < anzahl_importmods && mod_ret != M_PICDONE && mod_ret != M_DONEEXIT);
	} else
	{
		t = 0;
		do
		{
			for (seek = 0; seek < 10; seek++)
			{
				if (!Import_list.mod_exts[seek][t])
					break;
				if (strncmp(new_ext, Import_list.mod_exts[seek][t], 4) == 0)
				{
					Dialog.busy.reset(0, Import_list.imp_mod_list[t]);
					strncpy(modpath, Sys_info.standard_path, sizeof(modpath));
					strcat(modpath, "\\modules\\import\\");
					strcat(modpath, Import_list.imp_mod_list[t]);

					if ((mod_ret = module.comm.start_imp_module(modpath, picture)) == M_STARTERR)
						Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
					break;
				}
			}
		} while (++t < anzahl_importmods && mod_ret != M_PICDONE && mod_ret != M_DONEEXIT);
	}

	return mod_ret;
}



/* ----------------------------------------------------------------	*/
/* -------------- Export-Module suchen und eintragen ---------------*/
/* ----------------------------------------------------------------	*/
void f_scan_export(void)
{
	char *expath;						/* voller Modulpfad, Original */
	char *ex_path;						/* voller Modulpfad, editable */
	char *swapstr;
	char alert[128];
	const MODULE_START *module_start;
	char edstring[64];
	char strn[4];
	short t;
	short tt;
	short biggest;
	short pathlen;
	long entrlen;
	long temp;
	BASPAG *export_baspag;
	const MOD_INFO *module_info;
	struct DIRENTRY *filelist;
	struct DIRENTRY *actual;

	DEBUG_MSG(("Lade Export-Module\n"));

	/*---- Pfade vorbereiten ----*/
	expath = calloc(1, strlen(Sys_info.standard_path) + strlen("\\modules\\export\\") + 1);
	strcpy(expath, Sys_info.standard_path);
	strcat(expath, "\\modules\\export\\");

	Name_Max = get_maxnamelen(expath);

	pathlen = (short) (strlen(expath) + Name_Max);
	ex_path = (char *) calloc(1, pathlen + 1);
	swapstr = (char *) calloc(1, pathlen + 1);

	filelist = build_up_filelist(expath, "sxm", pathlen);

	Dialog.expmodList.anzahl = 0;

	actual = filelist;
	while (actual != NULL)
	{
		/*---- Modul als Overlay laden und Basepage ermitteln */
		strcpy(ex_path, expath);
		strcat(ex_path, actual->modname);

		temp = Pexec(3, ex_path, "", NULL);
		if (temp <= 0)
		{
			DEBUG_MSG(("  Fehler bei %s: %li\n", actual->modname, temp));
			sprintf(alert, "[1][Fehler in File|%s|im Ordner|\\modules\\export\\!][ OK ]", actual->modname);	/* FIXME: translate */
			form_alert(1, alert);
		} else
		{
			export_baspag = (BASPAG *) temp;

			module_start = get_module_start(export_baspag);	/* Textsegment-Startadresse holen */
			module_info = module_start->info;

			/* Zeiger auf Magic (mu� MOD_MAGIC_EXPORT sein!) */
			if (module_start->magic != MOD_MAGIC_EXPORT)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_MODULE], actual->modname, "export", "Exportmodul");
				form_alert(1, alert);
			} else if (module_info->compiler_id != COMPILER_ID)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_COMPILER], actual->modname, "export");
				form_alert(1, alert);
			} else
			{
				/*---- L�nge des gesamten Tochterprozesses ermitteln */
				start_module(export_baspag);

				if (Dialog.expmodList.anzahl < MAX_EXPORT_MODS)
				{
					/*---- Modul eintragen */
					export_modules[Dialog.expmodList.anzahl] = malloc(pathlen + 1);	/* keine variable L�nge wegen Sortierung! */
					strcpy(export_modules[Dialog.expmodList.anzahl], ex_path);
	
					Dialog.expmodList.modNames[Dialog.expmodList.anzahl] = (char *) calloc(1, 29);
					strncpy(Dialog.expmodList.modNames[Dialog.expmodList.anzahl], module_info->mod_name, 28);
					entrlen = strlen(Dialog.expmodList.modNames[Dialog.expmodList.anzahl]);
					if (entrlen < 28)
						strncat(Dialog.expmodList.modNames[Dialog.expmodList.anzahl], "                           ", 28 - entrlen);
	
					Dialog.expmodList.anzahl++;
	
					/*---- gescante Module im Startupdialog hochz�hlen */
					if (!(Dialog.expmodList.anzahl & 3))
					{
						strcpy(edstring, itoa(Dialog.expmodList.anzahl, strn, 10));
						/* FIXME: translate */
						strcat(edstring, " Exporter");
						set_startupdial(edstring);
					}

					DEBUG_MSG(("  --> Name   : %s\n", module_info->mod_name));
					DEBUG_MSG(("      File   : %s\n", actual->modname));
					DEBUG_MSG(("      Version: %x\n", module_info->version));
					DEBUG_MSG(("      Autor  : %s\n", module_info->autor));
				}
			}

#if 0
			Pexec(102, NULL, export_baspag, NULL);
#endif
			SMfree(export_baspag->p_env);
			SMfree(export_baspag);
		}

		actual = actual->next;
	}

	strcpy(edstring, itoa(Dialog.expmodList.anzahl, strn, 10));
	/* FIXME: translate */
	strcat(edstring, " Exporter");
	set_startupdial(edstring);

	/*---------------- Liste alphabetisch sortieren -------------*/
	for (t = 0; t < Dialog.expmodList.anzahl; t++)
	{
		biggest = t;
		for (tt = t + 1; tt < Dialog.expmodList.anzahl; tt++)
			if (stricmp(Dialog.expmodList.modNames[tt], Dialog.expmodList.modNames[biggest]) < 0)
				biggest = tt;

		strncpy(swapstr, Dialog.expmodList.modNames[t], 29);
		strncpy(Dialog.expmodList.modNames[t], Dialog.expmodList.modNames[biggest], 29);
		strncpy(Dialog.expmodList.modNames[biggest], swapstr, 29);
		strcpy(swapstr, export_modules[t]);
		strcpy(export_modules[t], export_modules[biggest]);
		strcpy(export_modules[biggest], swapstr);
	}

	destroy_filelist(filelist);

	free(swapstr);
	free(ex_path);
	free(expath);
}



/* ----------------------------------------------------------------	*/
/* -------------- Dither-Module suchen und eintragen ---------------*/
/*	Dithermodule stellen eine Sonderklasse der Module dar, da sie	*/
/*	grunds�tzlich resident gehalten werden und spezielle Info-		*/
/*	strukturen haben. 												*/
/* ----------------------------------------------------------------	*/
void f_scan_dither(void)
{
	char ditpath[SM_PATH_MAX];
	char *dit_path_end;
	char alert[128];
	char string[20];
	const DITHER_START *dither_start;
	long temp;
	BASPAG *dit_baspag;
	struct DIRENTRY *filelist;
	struct DIRENTRY *actual;

	DEBUG_MSG(("Lade Dither-Module\n"));

	/*---- Pfade vorbereiten ----*/
	strcpy(ditpath, Sys_info.standard_path);
	strcat(ditpath, "\\modules\\dither\\");
	dit_path_end = ditpath + strlen(ditpath);

	filelist = build_up_filelist(ditpath, "sdm", SM_PATH_MAX);

	anzahl_dithermods = 0;

	actual = filelist;
	while (actual != NULL)
	{
		/*---- Modul als Overlay laden und Basepage ermitteln */
		strcpy(dit_path_end, actual->modname);

		DEBUG_MSG(("  anzahl_dithermods:  %i\n", anzahl_dithermods));
		DEBUG_MSG(("  dit_path:  %s\n", dit_path));
		DEBUG_MSG(("  Speicher %li\n", Malloc(-1)));

		temp = Pexec(3, ditpath, "", NULL);

		if (temp <= 0)
		{
			DEBUG_MSG(("  Fehler bei %s: %li\n", actual->modname, temp));
			sprintf(alert, "[1][Fehler '%li' in File|%s|im Ordner|\\modules\\dither\\!][ OK ]", temp, actual->modname);	/* FIXME: translate */
			form_alert(1, alert);
		} else
		{
			dit_baspag = (BASPAG *) temp;

			dither_start = get_module_start(dit_baspag);	/* Textsegment-Startadresse holen */

			/* Zeiger auf Magic (mu� MOD_MAGIC_DITHER sein!) */
			if (dither_start->magic != MOD_MAGIC_DITHER)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_MODULE], actual->modname, "dither", "Dithermodul");
				form_alert(1, alert);
				SMfree(dit_baspag->p_env);
				SMfree(dit_baspag);
			} else if (dither_start->info->compiler_id != COMPILER_ID)
			{
				sprintf(alert, smurf_frstr[AL_WRONG_COMPILER], actual->modname, "dither");
				form_alert(1, alert);
				SMfree(dit_baspag->p_env);
				SMfree(dit_baspag);
			} else
			{
				/*---- L�nge des gesamten Tochterprozesses ermitteln */
				start_module(dit_baspag);

				if (anzahl_dithermods < MAX_DITHER_MODS)
				{
					ditmod_info[anzahl_dithermods] = dither_start->info;
					Dithermod_Basepage[anzahl_dithermods] = dit_baspag;
					strncpy(string, ditmod_info[anzahl_dithermods]->algo_name, 15);
					string[15] = '\0';
					set_startupdial(string);
	
					/* Algorithmus ins Popup eintragen und Button einschalten */
					strcpy(col_pop[anzahl_dithermods + 1].TextCast, string);
					col_pop[anzahl_dithermods + 1].ob_state &= ~OS_DISABLED;
	
					anzahl_dithermods++;
				} else
				{
					SMfree(dit_baspag->p_env);
					SMfree(dit_baspag);
				}
			}
		}

		actual = actual->next;
	}

	destroy_filelist(filelist);
}


/* build_up_filelist baut eine doppelt verkettete Liste der im Verzeichnis vorhandenen */
/* Dateien auf. */
/* Bevorzugt werden hierf�r die MiNT-Funktionen Dopendir() und Dxreaddir() verwendet. */
/* Nur wenn diese nicht vorhanden sind, wird auf Fsfirst()/Fsnext() ausgewichen. */
/* Die Extensions werden jetzt auch bei Fsfirst()/Fsnext() manuell getestet! */
struct DIRENTRY *build_up_filelist(char *path, char *ext, short pathlen)
{
	char mod_path[SM_PATH_MAX];
	char *buf;
	char *_buf;
	char *temp;
	short buflen;
	long back;
	long dirhandle;
	DTA *old_dta;
	DTA new_dta;
	struct DIRENTRY *begin;
	struct DIRENTRY *actual;
	struct DIRENTRY Element;

	Element.next = NULL;			/* Initial auf "keine Dateien enthalten" setzen */
	actual = &Element;

	if ((back = Dopendir(path, 0)) != EINVFN)	/* Verzeichnis im Normalmodus �ffnen */
	{									/* und Test ob Dopendir() existiert */
		if ((back & 0xff000000L) != 0xff000000L)	/* Directory gefunden? */
		{								/* negativer Backval ist _nicht_ automatisch Fehler! */
			dirhandle = back;			/* Dopendir-R�ckgabe an die richtige Variable geben */

			buflen = 4 + pathlen + 1;
			buf = (char *) calloc(1, buflen);

			do
			{
				/* ok MagicPC is giving me -49 not -47 at the end
				 * of the list.  This modification isn't perfect but
				 * does catch the error condition
				 */
				if ((back = Dreaddir(buflen, dirhandle, buf)) != ENMFIL)	/* Noch eine Datei vorhanden */
				{
					_buf = buf + 4;		/* Dateiindex �bergehen */

					if ((temp = strrchr(_buf, '.')) != NULL)
						if (stricmp(temp + 1, ext) == 0)	/* Dreaddir beherrscht n�mlich keine Wildcards! */
						{
							actual = insert_entry(actual, _buf);	/* neuen Eintrag einh�ngen */
							files_read++;
						}
				} else
					break;				/* und wech wenn keine Datei mehr vorhanden */
			} while (actual != NULL);

			Dclosedir(dirhandle);		/* Verzeichnis wieder zu */
		}

		begin = Element.next;			/* Zeiger auf den ersten Eintrag holen */
	} else								/* Fsfirst()/Fsnext() */
	{
		old_dta = Fgetdta();			/* DTA holen */
		Fsetdta(&new_dta);				/* neue DTA setzen */

		/*---- Pfade vorbereiten ----*/
		strcpy(mod_path, path);
		strcat(mod_path, "*.*");

		/*---- erstes File ermitteln ----*/
		if (Fsfirst(mod_path, 0) == 0)
		{
			do
			{
				if ((temp = strrchr(new_dta.d_fname, '.')) != NULL)
					if (stricmp(temp + 1, ext) == 0)
					{
						actual = insert_entry(actual, new_dta.d_fname);	/* neuen Eintrag einh�ngen */
						files_read++;
					}
			} while (Fsnext() == 0);	/* und das f�r alle Module */
		}

		Fsetdta(old_dta);				/* alte DTA zur�cksetzen */

		begin = Element.next;
	}

	return begin;
}


/* insert_entry f�gt einen Eintrag (eine Datei) in die verkettete Liste ein. */
/* Argumente sind ein Zeiger auf den einzuf�genden Eintrag sowie das aktuelle */
/* Ende der Liste. */
struct DIRENTRY *insert_entry(struct DIRENTRY *ende, char *string)
{
	struct DIRENTRY *new;				/* zeigt auf den neuen Speicherblock */

	/* Speicher f�r Struktur und String anfordern */
	if ((new = (struct DIRENTRY *) malloc(sizeof(struct DIRENTRY))) == 0 ||
		(new->modname = (char *) malloc(strlen(string) + 1)) == 0)
	{
		free(new);
		return NULL;
	}

	strcpy(new->modname, string);		/* String reinkopieren */
	new->next = NULL;					/* neue Ende-Markierung */

	ende->next = new;					/* Vorg�nger (also bisher letzter) zeigt auf neues Element */

	return new;
}


/* L�st die verkettete Liste wieder auf. */
/* Oder mu� ich von hinten kommen (d.h. doppelt verketten), */
/* um Speicherzersplitterung zu vermeiden? */
void destroy_filelist(struct DIRENTRY *actual)
{
	struct DIRENTRY *next;

	while (actual != NULL)
	{
		next = actual->next;			/* wegsichern, da nach Freigabe von */
		free(actual->modname);			/* actual ja nicht mehr darauf */
		free(actual);					/* zugegriffen werden darf */
		actual = next;
	}
}
