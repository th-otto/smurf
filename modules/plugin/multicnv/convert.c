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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "import.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "globdefs.h"
#include "plugin.h"
#include "popdefin.h"
#include "../../../src/rsc/en/smurf.h"

#undef NUM_STRINGS
#undef NUM_FRSTR
#undef NUM_UD
#undef NUM_IMAGES
#undef NUM_BB
#undef NUM_FRIMG
#undef NUM_IB
#undef NUM_CIB
#undef NUM_TI
#undef NUM_OBS
#undef NUM_TREE
#undef INFOICON							/* conflicts with smurf.h */
#undef DITHER_CB						/* conflicts with smurf.h */
#include "en/multicon.h"
#include "wildcard.h"
#include "tools.h"
#include "multi.h"

static long files_read = 0;
static long allfiles = 0;

#if !defined(__TOS) && !defined(_file_h_) && !defined(_filesys_h_) && !defined(__XATTR)
typedef struct xattr
{
	unsigned short st_mode;
	long           st_ino;	/* must be 32 bits */
	unsigned short st_dev;	/* must be 16 bits */
	unsigned short st_rdev;	/* not supported by the kernel */
	unsigned short st_nlink;
	unsigned short st_uid;	/* must be 16 bits */
	unsigned short st_gid;	/* must be 16 bits */
	long           st_size;
	long           st_blksize;
	long           st_blocks;
	unsigned long  st_mtime; /* actually time&date in DOSTIME format */
	unsigned long st_atime;
	unsigned long st_ctime;
	short          st_attr;
	short res1;		/* reserved for future kernel use */
	long res2[2];
} XATTR;
#endif
#ifndef FA_SUBDIR
#define FA_SUBDIR 0x10
#endif



#define TextCast ob_spec.tedinfo->te_ptext

long f_len;
static short PCDwidth;
static short PCDheight;
static short PCD;

static SMURF_PIC *current_pic;

static char dest_path[256];


/* build_up_filelist ----------------------------------------------------------------
    Baut eine verkettete Liste der im Verzeichnis vorhandenen Dateien auf.
    Bevorzugt werden hierfr die Verzeichnisfunktionen Dopendir() und Dxreaddir() verwendet.
    Nur wenn diese nicht vorhanden sind, wird auf Fsfirst()/Fsnext() ausgewichen. Die
    Wildcards werden jetzt immer mit check_wildcard() gefiltert auch bei Fsfirst()/Fsnext()!
    ----------------------------------------------------------------------------------*/
static struct DIRENTRY *build_up_filelist(char *path, char *wildcard, int pathlen)
{
	char buf[4 + 257];
	char *_buf;
	char checkpath[257];
	char wildcard_copy[65];
	int len;
	long back;
	long dirhandle;

	DTA *old_dta;
	DTA *new_dta;
	struct DIRENTRY *begin;
	struct DIRENTRY *actual;
	struct DIRENTRY Element;
	XATTR attrib;

	/* printf("\n Filelist builder: path %s, wildcard %s", path, wildcard); */

	len = 4 + pathlen + 1;

	/* 
	 * Hier Abfrage, ob die Verzeichnisfunktionen fr dieses Laufwerk
	 * berhaupt m”glich sind einbauen 
	 */
	if ((back = Dopendir(path, 0)) != EINVFN)	/* Verzeichnis im Normalmodus ”ffnen */
	{									/* und Test ob Dopendir() existiert */
		Element.next = NULL;			/* Initial auf "keine Dateien enthalten" setzen */

		if ((back & 0xff000000L) != 0xff000000L)	/* Directory gefunden */
		{								/* negativer Backval ist _nicht_ automatisch Fehler! */
			dirhandle = back;			/* Dopendir-Rckgabe an die richtige Variable geben */

			actual = &Element;
			do
			{
				if ((back = Dreaddir(len, dirhandle, buf)) != ENMFIL)	/* noch eine Datei vorhanden */
				{
					_buf = buf + 4;		/* Dateiindex bergehen da immer im Normalmodus ge”ffnet */
					strcpy(checkpath, path);
					strcat(checkpath, _buf);

					Fxattr(0, checkpath, &attrib);
#ifdef _MINT_MINTBIND_H
					if (attrib.st_attr != FA_SUBDIR)	/* Unterverzeichnisse nicht mitnehmen! */
#else
					if (attrib.attr != FA_SUBDIR)	/* Unterverzeichnisse nicht mitnehmen! */
#endif
					{
						/*
						 * wird nicht zwischen Grož- und Kleinschreibung unterschieden?
						 */
						strcpy(wildcard_copy, wildcard);
						if (Dpathconf(checkpath, 6) != 0)
						{
							lower(_buf);
							lower(wildcard_copy);
						}

						/*
						 * Wildcard-Prfung
						 */
						if (check_wildcard(_buf, wildcard_copy))
						{
							actual = smurf_functions->insert_entry(actual, _buf);	/* neuen Eintrag einh„ngen */
							files_read++;
						}
					}
				} else
					break;				/* und wech ... */
			} while (actual != NULL);

			Dclosedir(dirhandle);
		}

		begin = Element.next;
	} else								/* Fsfirst()/Fsnext() */
	{
		old_dta = Fgetdta();			/* DTA holen */
		new_dta = malloc(sizeof(DTA));
		Fsetdta(new_dta);				/* neue DTA setzen */

		/*---- Pfade vorbereiten ----*/
		strcpy(checkpath, path);
		strcat(checkpath, "*.*");

		Element.next = NULL;			/* Initial auf "keine Dateien enthalten" setzen */

		/*---- erstes File ermitteln ----*/
		if (Fsfirst(checkpath, 0) == 0)
		{
			actual = &Element;
			do
			{
				/*
				 * es wird nie zwischen Grož- und Kleinschreibung unterschieden
				 */
				strcpy(wildcard_copy, wildcard);
				lower(new_dta->d_fname);
				lower(wildcard_copy);

				/*
				 * Wildcard-Prfung
				 */
				if (check_wildcard(new_dta->d_fname, wildcard_copy))
				{
					actual = smurf_functions->insert_entry(actual, new_dta->d_fname);	/* neuen Eintrag einh„ngen */
					files_read++;
				}

							  /*---- n„chsten Modulpfad ermitteln */
				strcpy(checkpath, path);
				strcat(checkpath, new_dta->d_fname);
			} while (Fsnext() == 0);	/* und das fr alle Module */
		}

		begin = Element.next;

		Fsetdta(old_dta);				/* alte DTA zurcksetzen */
		free(new_dta);
	}

	return begin;
}



/* make_filelist ---------------------------------------------------------------------
    Baut eine Dateiliste der extensionen in extensions (nacheinander) auf und baut fr
    jede Fileliste die Dateinamen in filelist auf, und zwar mit den Wildcards verknpft.
    ---------------------------------------------------------------------------------*/
static struct DIRENTRY *make_filelist(char *path, char *wildcard)
{
	struct DIRENTRY *first_file;

	first_file = build_up_filelist(path, wildcard, (int) strlen(path) + 64);

	return first_file;
}



/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
/* -------                  Bild laden                      ---- */
/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
static short f_loadpic(uint8_t *pic, char *picpath)
{
	char PCDmodpath[SM_PATH_MAX];
	char *namename;
	char *nameext;
	char ext[5] = "";
	short module_ret;

	strcpy(PCDmodpath, smurf_vars->Sys_info->standard_path);
	strcat(PCDmodpath, "\\modules\\pcd.sim");

	current_pic->pic_data = pic;

	graf_mouse(BUSYBEE, NULL);

	strcpy(current_pic->filename, picpath);

	namename = strrchr(picpath, '\\') + 1;	/* Dateinamen abtrennen */

	if (PCD)
	{
		current_pic->pic_width = PCDwidth;
		current_pic->pic_height = PCDheight;
		current_pic->file_len = f_len;
		current_pic->palette = malloc(1025);	/* Paletten-Puffer */

		module_ret = smurf_functions->start_imp_module(PCDmodpath, current_pic);

		if (module_ret == -1)
		{
			services->f_alert(alerts[NO_PCD], NULL, NULL, NULL, 1);
			module_ret = M_MODERR;
			services->reset_busybox(128, "Error!");
		} else
		{
			services->reset_busybox(128, "OK");
		}
	} else
	{
		if ((nameext = strrchr(namename, '.')) != NULL && nameext > namename)	/* Extender abtrennen */
		{
			strncpy(ext, nameext + 1, 4);
			strupr(ext);
		} else
			strcpy(ext, "");

		module_ret = smurf_functions->f_import_pic(current_pic, ext);
	}

	return module_ret;				/* und zurck */
}


/*  --------------------------------------- */
/*  Extension stimmt - Is dat Ding en PCD?  */
/*  --------------------------------------- */
static uint8_t *f_do_pcd(char *Path)
{
	uint8_t *buffer;
	int file;
	WORD imgtype;
	long DatenOffset;
	long len;
	long back;
	long dummy;
	WORD idummy;
	WORD key;

	/* Die ersten 3600 Bytes laden... */
	if ((buffer = (uint8_t *)Malloc(3700)) == 0)
	{
		services->f_alert(smurf_vars->alerts[NO_MEM].TextCast, NULL, NULL, NULL, 1);
	} else
	{
		if ((dummy = Fopen(Path, FO_READ)) < 0)
		{
			services->f_alert(smurf_vars->alerts[FILEOPEN_ERR].TextCast, NULL, NULL, NULL, 1);
			Mfree(buffer);
		} else
		{
			file = (int) dummy;
			back = Fread(file, 3600, buffer);
			Fclose(file);

			if (back != 3600)
			{
				services->f_alert(alerts[PCD_READ_ERROR], NULL, NULL, NULL, 1);
				Mfree(buffer);
				return NULL;
			}

			/* isseseins? */
			if (strcmp((char *)buffer + 2048, "PCD_IPI") != 0)
			{
				Mfree(buffer);
				return NULL;			/* N”! */
			} else						/* Jupp! */
			{
				Mfree(buffer);

				PCD = 1;

				imgtype = smurf_vars->Sys_info->PCD_Defsize;

				graf_mkstate(&idummy, &idummy, &idummy, &key);

				if (key & KEY_ALT)
					imgtype = services->popup(&(smurf_vars->pop_ups[POPUP_KODAK]), 1, PCD_DEF_PB, NULL);

				switch (imgtype)
				{
				case BASE_D16:
					DatenOffset = 0x2000;
					PCDwidth = 192;
					PCDheight = 128;
					break;
				case BASE_D4:
					DatenOffset = 0xB800;
					PCDwidth = 384;
					PCDheight = 256;
					break;
				case BASE:
					DatenOffset = 0x30000L;
					PCDwidth = 768;
					PCDheight = 512;
					break;
				default:
					break;
				}

				/* Die Bilddaten laden... */
				len = (((long) PCDwidth * (long) PCDheight * 3L) >> 1L);
				f_len = len;
				if ((buffer = (uint8_t *)Malloc(len + 1024L)) == 0)
				{
					services->f_alert(smurf_vars->alerts[NO_MEM].TextCast, NULL, NULL, NULL, 1);
				} else
				{
					if ((dummy = Fopen(Path, FO_READ)) < 0)
					{
						services->f_alert(smurf_vars->alerts[FILEOPEN_ERR].TextCast, NULL, NULL, NULL, 1);
						Mfree(buffer);
					} else
					{
						file = (int) dummy;
						Fseek(DatenOffset, file, 0);	/* Ab zum DatenOffset */
						back = Fread(file, len, buffer);
						Fclose(file);
						if (back != len)
						{
							services->f_alert(alerts[PCD_READ_ERROR], NULL, NULL, NULL, 1);
							Mfree(buffer);
						} else
							return buffer;
					}					/* Open fr Bilddaten */
				}						/* Malloc fr Bilddaten */
			}							/* Magic-Vergleich */
		}								/* Open fr Erkennung */
	}									/* Malloc fr Erkennung */

	return NULL;
}




/*----------------------------------------------------------------- */
/*                          f_save_pic                              */
/*  Handled das Aufrufen der Dither-, Convert- und Codierungsfunk-  */
/*  tionen frs Exportieren mit dem Modul export_mabs.              */
/*  Zu exportierendes Bild ist immer smurf_picture[active_pic]      */
/*----------------------------------------------------------------- */
static short f_save_pic(const MOD_ABILITY *export_mabs, SMURF_PIC *pic)
{
	char savepath[257];
	char *save_ext;
	const char *expext;
	char module_name[31];
	const MODULE_START *module_start;
	uint8_t *picture;

	short max_expdepth;
	short dest_format;
	short ext_number;
	short dest_colsys;
	short old_picdepth;
	int filehandle;

	long len;
	long back;

	EXPORT_PIC *pic_to_save;
	SMURF_PIC *pic_to_export;
	SMURF_PIC *converted_pic;
	const MOD_INFO *modinfo;
	GARGAMEL *exp_gstruct;
	BASPAG *exp_bp;
	short old_export_mod_num;

	/*
	 * Exporternummer eintragen, damit Smurf bei der Konvertierung den richtigen erwischt
	 */
	old_export_mod_num = exp_conf->export_mod_num;
	exp_conf->export_mod_num = mod_num;

	exp_gstruct = smurf_struct[mod_num & 0xFF];
	exp_bp = edit_bp[mod_num & 0xFF];

	/*
	 * Kommunikationsstruktur und Basepage prfen
	 */
	if (exp_gstruct == NULL)
	{
		services->f_alert(alerts[NO_COM_STRUCT], NULL, NULL, NULL, 1);
		return -1;
	}
	if (exp_bp == NULL)
	{
		services->f_alert(alerts[NO_BASEPAGE], NULL, NULL, NULL, 1);
		return -1;
	}

	/*
	 * Maximale Farbtiefe des Exporters und zugeh”riges Datenformat ermitteln ---
	 */
	max_expdepth = export_mabs->depth1;
	dest_format = export_mabs->form1;

	if (export_mabs->depth2 > max_expdepth)
	{
		max_expdepth = export_mabs->depth2;
		dest_format = export_mabs->form2;
	}
	if (export_mabs->depth3 > max_expdepth)
	{
		max_expdepth = export_mabs->depth3;
		dest_format = export_mabs->form3;
	}
	if (export_mabs->depth4 > max_expdepth)
	{
		max_expdepth = export_mabs->depth4;
		dest_format = export_mabs->form4;
	}
	if (export_mabs->depth5 > max_expdepth)
	{
		max_expdepth = export_mabs->depth5;
		dest_format = export_mabs->form5;
	}
	if (export_mabs->depth6 > max_expdepth)
	{
		max_expdepth = export_mabs->depth6;
		dest_format = export_mabs->form6;
	}
	if (export_mabs->depth7 > max_expdepth)
	{
		max_expdepth = export_mabs->depth7;
		dest_format = export_mabs->form7;
	}
	if (export_mabs->depth8 > max_expdepth)
	{
		max_expdepth = export_mabs->depth8;
		dest_format = export_mabs->form8;
	}

	pic_to_export = pic;

	/*
	 * Bild konvertieren / dithern
	 */
/*  if(expmabs)*/
	{
		/*
		 * neue SMURF_PIC generieren 
		 */
		converted_pic = services->SMalloc(sizeof(SMURF_PIC));
		memcpy(converted_pic, pic_to_export, sizeof(SMURF_PIC));
		converted_pic->palette = malloc(1025);
		memcpy(converted_pic->palette, pic_to_export->palette, 1025);
		converted_pic->screen_pic = NULL;

		/* --- Maximale Farbtiefe des Xporters ausreichend? */
		/* sollte eigentlich mit kleinster Exporterfarbtiefe verglichen werden */
		if (pic_to_export->depth >= max_expdepth)
		{
			if (pic_to_export->depth == 24 && max_expdepth == 16)
			{
				smurf_functions->tfm_24_to_16(converted_pic, NEW);
			} else
			{
				/*
				 * Farbsystem holen
				 */
				old_picdepth = pic_to_export->depth;
				pic_to_export->depth = max_expdepth;

				smurf_functions->start_exp_module(export_path, MCOLSYS, pic_to_export, exp_bp, exp_gstruct, mod_num);
				if (exp_gstruct->module_mode == M_COLSYS)
					dest_colsys = exp_gstruct->event_par[0];
				else
					dest_colsys = RGB;
				pic_to_export->depth = old_picdepth;


				if (max_expdepth == 8 && dest_colsys == GREY)
				{
					if (smurf_functions->f_convert(converted_pic, export_mabs, GREY, NEW, 0) != 0)
						return -2;
				} else if (max_expdepth < 16 && max_expdepth < pic_to_export->depth)
				{
					if (smurf_functions->dither_for_export(export_mabs, max_expdepth, dest_format, converted_pic) != 0)
						return -2;
				}
			}
		} else if (smurf_functions->f_convert(converted_pic, export_mabs, 255, NEW, 0) != 0)
			return -2;
	}

	/*------- Infotext eintragen --------*/
	strcpy(converted_pic->infotext, "Created with SMURF graphics file converter.");

	/*  
	 * Extension ermitteln, Exporter aufrufen 
	 */
	smurf_functions->start_exp_module(export_path, MEXTEND, converted_pic, exp_bp, exp_gstruct, mod_num);
	module_start = smurf_functions->get_module_start(exp_bp);
	modinfo = module_start->info;	/* Zeiger auf Modulinfostruktur */

	if (exp_gstruct->module_mode == M_EXTEND)
		ext_number = exp_gstruct->event_par[0] - 1;
	else
		ext_number = 0;

	expext = modinfo->ext[ext_number];

/*  printf("\n save extension is %s", expext);*/

	/*
	 * Speichername zusammenbasteln 
	 */
	strncpy(module_name, modinfo->mod_name, 30);

	strlwr(pic->filename);
	strcpy(savepath, dest_path);
	strcat(savepath, pic->filename);

	if ((save_ext = strrchr(savepath, '.')) != NULL && save_ext > strrchr(savepath, '\\'))
	{
		save_ext++;
		strcpy(save_ext, expext);	/* Extender gegen neuen ersetzen */
	} else
	{
		strcat(savepath, ".");			/* Punkt und */
		save_ext = savepath + strlen(savepath);
		strcpy(save_ext, expext);	/* neuen Extender an Filenamen ohne Extender h„ngen */
	}
	strlwr(save_ext);

	/* FIXME: translate */
	services->reset_busybox(0, "codiere Bild...");

/*  printf("\n image encoding");*/

	pic_to_save = smurf_functions->start_exp_module(export_path, MEXEC, converted_pic, exp_bp, exp_gstruct, mod_num);

	if (exp_gstruct->module_mode == M_MEMORY)
		services->f_alert(smurf_vars->alerts[NOMEM_EXPORT].TextCast, NULL, NULL, NULL, 1);
	else if (exp_gstruct->module_mode == M_PICERR)
		services->f_alert(smurf_vars->alerts[NOMEM_PICERR].TextCast, NULL, NULL, NULL, 1);
	else if (exp_gstruct->module_mode == M_DONEEXIT)
	{
		picture = pic_to_save->pic_data;
		len = pic_to_save->f_len;
		services->reset_busybox(0, "speichere Bild...");


		back = Fcreate(savepath, 0);
		if (back >= 0)
		{
			filehandle = (int) back;
			back = Fwrite(filehandle, len, picture);
			if (back != len)
				services->f_alert(alerts[WRITE_ERROR], NULL, NULL, NULL, 1);

			Fclose(filehandle);

#if 0									/*          appl_id ist dem Plugin leider nicht bekannt */

			/*------- "Laufwerksinhalt ver„ndert" ans Desktop -------*/
			/* Erst mal appl_getinfo() fragen, weil das TOS sonst "falscher */
			/* AES-Funktionsaufruf" bringt - Scheiže, kann ihm doch egal sein. */
			if (smurf_vars->Sys_info->OSFeatures & GETINFO && appl_getinfo(4, &out1, &out2, &out3, &out4) && out3 == 1)
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
#endif

		}


		/*
		 * IF, damit bei raw-Export nicht das Original fl”ten geht ... 
		 */
		if (pic_to_save->pic_data != pic_to_export->pic_data)
		{
			if (Mfree(pic_to_save->pic_data) != 0)
				services->f_alert(smurf_vars->alerts[EXP_MFREE_ERR].TextCast, NULL, NULL, NULL, 1);
		}

		if (Mfree(pic_to_save) != 0)
			services->f_alert(smurf_vars->alerts[EXP_MFREE_ERR].TextCast, NULL, NULL, NULL, 1);

/*      printf("\n buffer memory abadoned.");*/
	}

	/* 
	 * Muž sein, da bei Export ohne Konvertierung converted_pic noch aufs Original zeigt!
	 */
	if (converted_pic->pic_data != pic_to_export->pic_data)
		Mfree(converted_pic->pic_data);

	free(converted_pic->palette);
	services->SMfree(converted_pic);

	services->reset_busybox(128, "OK");

/*  printf("\n image file export complete.");*/

	exp_conf->export_mod_num = old_export_mod_num;	/* Exporter-ID restoren */

	return 0;
}


void start_conversion(void)
{
	char source_path[256];
	char wildcard_string[256];
	char num[8];
	char import_path[256];
	struct DIRENTRY *first_file = NULL;
	struct DIRENTRY *file;
	char *extension;
	short wcard;
	short t;
	int filehandle;
	short loadback;
	short saveback;
	short files_converted = 0;
	long back;
	void *loaded_file;
	MOD_ABILITY user_modabs;

	if (strlen(dialog[SOURCE_PATH].TextCast) == 0)
	{
		services->f_alert(alerts[NO_SOURCE], NULL, NULL, NULL, 1);
		return;
	}
	if (strlen(dialog[DEST_PATH].TextCast) == 0)
	{
		services->f_alert(alerts[NO_DEST], NULL, NULL, NULL, 1);
		return;
	}

	strcpy(source_path, dialog[SOURCE_PATH].TextCast);
	strcpy(dest_path, dialog[DEST_PATH].TextCast);
	strcpy(wildcard_string, dialog[WILDCARDS].TextCast);

	graf_mouse(BUSYBEE, NULL);

	num_of_wildcards = 0;
	allfiles = 0;
	tokenize_wildcards(wildcard_string);

	/*
	 * MOD_ABILITY nach Usereinstellungen fertigmachen
	 */
	memset(&user_modabs, 0x0, sizeof(MOD_ABILITY));
	user_modabs.depth1 = depth_button2depth(exp_depth);

	if (expmabs.depth1 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form1;
	if (expmabs.depth2 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form2;
	if (expmabs.depth3 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form3;
	if (expmabs.depth4 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form4;
	if (expmabs.depth5 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form5;
	if (expmabs.depth6 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form6;
	if (expmabs.depth7 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form7;
	if (expmabs.depth8 == user_modabs.depth1)
		user_modabs.form1 = expmabs.form8;

	/*
	 * Smurfs originale EXPORT_CONFIG retten und anpassen
	 */
	memcpy(&original_exp_conf, exp_conf, sizeof(EXPORT_CONFIG));

	exp_conf->exp_form = user_modabs.form1;
	exp_conf->exp_dither = my_exp_conf.exp_dither;
	exp_conf->exp_colred = my_exp_conf.exp_colred;
	memcpy((exp_conf->exp_fix_red), fix_red, 256 * 2);
	memcpy((exp_conf->exp_fix_green), fix_green, 256 * 2);
	memcpy((exp_conf->exp_fix_blue), fix_blue, 256 * 2);

	/*
	 * Prozežbalken setzen und aufdecken
	 */
	dialog[PROC_BAR].ob_width = 0;
	strcpy(wildcard_string, "  0 Dateien konvertiert...");
	strcpy(dialog[PROC_TEXT].TextCast, wildcard_string);
	dialog[PROC_BACK].ob_flags &= ~OF_HIDETREE;
	services->redraw_window(&window, NULL, PROC_BACK, 0);

	/*
	 * Alle Dateien aus der Liste nacheinander importieren und speichern
	 */
	for (wcard = 0; wcard < num_of_wildcards; wcard++)
	{
		files_read = 0;
		first_file = make_filelist(source_path, wildcards[wcard]);
		file = first_file;
		allfiles += files_read;

		while (file != NULL)
		{
			PCD = 0;
			loaded_file = NULL;
			strcpy(import_path, source_path);
			strcat(import_path, file->modname);

			if (Fattrib(import_path, 0, 0) >= 0)	/* gibbet die Datei? */
			{
				/*
				 * PCD? -> Sonderbehandlung
				 */
				extension = strrchr(file->modname, '.');
				if (extension != NULL && strnicmp(extension, "PCD", 3) == 0)
				{
					loaded_file = f_do_pcd(import_path);
					if (loaded_file != NULL)
						PCD = 1;
				}

				/*
				 * ansonsten Datei laden und importieren
				 */
				if (!PCD)
				{
					back = Fopen(import_path, FO_READ);
					if (back > 0)
					{
						filehandle = (int) back;
						f_len = Fseek(0, filehandle, 2);
						Fseek(0, filehandle, 0);
						if (f_len)
						{
							loaded_file = services->SMalloc(f_len);
							if (loaded_file)
								Fread(filehandle, f_len, loaded_file);
							else
								services->f_alert(alerts[NO_LOADMEM], NULL, NULL, NULL, 1);
						}
						Fclose(filehandle);
					}

					if (loaded_file == NULL)
					{
						file = file->next;
						continue;
					}
				}

				current_pic = (SMURF_PIC *) services->SMalloc(sizeof(SMURF_PIC));
				loadback = f_loadpic(loaded_file, import_path);

				/*
				   if(smurf_struct[mod_num&0xFF] == NULL)
				   form_alert(1,"[0][Fehler bei der Kommunikation| mit dem Modul. Die Konvertierung|kann nicht fortgesetzt werden.][ OK ]");
				 */


				if (loadback == M_INVALID || loadback == M_MEMORY)
				{
					services->f_alert(alerts[IMPORT_ERROR], NULL, NULL, NULL, 1);
					Mfree(loaded_file);
					file = file->next;
					continue;
				}

				strcpy(current_pic->filename, file->modname);
				if (dialog[KEEP_DEPTH].ob_state & OS_SELECTED)
					saveback = f_save_pic(&expmabs, current_pic);
				else
					saveback = f_save_pic(&user_modabs, current_pic);
				(void) saveback;

				smurf_functions->destroy_smurfpic(current_pic);
			}

			file = file->next;

			files_converted++;
			dialog[PROC_BAR].ob_width =
				(WORD) (((long) files_converted * (long) dialog[PROC_BACK].ob_width / files_read) / num_of_wildcards *
					   (wcard + 1));
			itoa(files_converted, num, 10);
			strcpy(wildcard_string, num);
			strcat(wildcard_string, " Dateien konvertiert...");
			strcpy(dialog[PROC_TEXT].TextCast, wildcard_string);
			services->redraw_window(&window, NULL, PROC_BACK, 0);
		}
	}

	graf_mouse(ARROW, NULL);
	smurf_functions->destroy_filelist(first_file);

	memcpy(exp_conf, &original_exp_conf, sizeof(EXPORT_CONFIG));

	for (t = 0; t < num_of_wildcards; t++)
		free(wildcards[t]);

	dialog[PROC_BACK].ob_flags |= OF_HIDETREE;
	services->redraw_window(&window, NULL, 0, 0);
}



