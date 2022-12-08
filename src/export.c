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
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurf.h"
#include "smurfine.h"
#include "globdefs.h"
#include "popdefin.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "olgagems.h"
#include "plugin.h"
#include "smplugin.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"


/*--------- Exporterspezifisches ------*/
EXPORT_CONFIG exp_conf;

/* ------------------------------------------------------------	*/
/*							Bild exportieren  					*/
/*	Handled das Export-Listfeld und ruft, je nach Useraktion	*/
/*	f_export_form oder f_save_pic auf. Die mîglichen Exporter-	*/
/*	farbtiefen und -datenformate werden aus dem evtl. 			*/
/*	gestarteten Modul ausgelesen und in die Felder export_depth	*/
/*	und export_format eingetragen. Auûerdem wird die erweiterte	*/
/*	Konfiguration (Aufrufen, Senden) hier geregelt.				*/
/* ------------------------------------------------------------	*/
void f_export_pic(void)
{
	short mod_num;
	WORD info;
	WORD button = 0;
	short mod_index;
	short omod_index;
	const MOD_ABILITY *export_mabs;
	const MOD_INFO *modinfo;
	const MODULE_START *module_start;
	OBJECT *exp_form;
	WORD my_scancode;

	static MOD_ABILITY expmabs;

	my_scancode = key_scancode >> 8;
	info = Dialog.expmodList.tree[EXMOD_INFO].ob_state & OS_SELECTED;
	exp_form = Dialog.expmodList.tree;

	/* aktuelles Listen-Objekt vor énderung ermittelten */
	omod_index = f_listfield(&wind_s[WIND_EXPORT], 0, 0, &Dialog.expmodList.modList);

	if (my_scancode != KEY_DOWN && my_scancode != KEY_UP)
	{
		button = Dialog.init(WIND_EXPORT, START_EMOD);
		if (obj == DIALOG_EXIT)
		{
			if (!info)
				button = START_EMOD;
			else
				button = EXMOD_INFO_OK;
		}
	}

	/*
	 * selektiertes Listen-Objekt ermitteln
	 * der Index im Feld wird hier zurÅckgegeben, nicht der Objektindex!
	 */
	if (!info)
		mod_index = f_listfield(&wind_s[WIND_EXPORT], button, key_scancode, &Dialog.expmodList.modList);
	else
		mod_index = f_listfield(&wind_s[WIND_EXPORT], 0, 0, &Dialog.expmodList.modList);

	if ((key_scancode && my_scancode != KEY_UP && my_scancode != KEY_DOWN) || !openmode)
		Window.windSet(wind_s[WIND_EXPORT].whandlem, WF_INFO, LONG2_2INT(Dialog.expmodList.modList.autolocator), 0, 0);

	/*
	 * wenn gescrollt oder ein anderes Modul selektiert wurde, 
	 * muû der Standardexport-Schalter u.U. selektiert oder deselektiert werden.
	 */
	if (omod_index != mod_index)
	{
		if (Sys_info.defaultExporter != mod_index && IsSelected(exp_form[EXPORT_STANDARD]))
			change_object(&wind_s[WIND_EXPORT], EXPORT_STANDARD, OS_UNSEL, 1);
		else if (Sys_info.defaultExporter == mod_index && !IsSelected(exp_form[EXPORT_STANDARD]))
			change_object(&wind_s[WIND_EXPORT], EXPORT_STANDARD, OS_SELECTED, 1);

		/*
		 * Options-Button enablen bzw. disablen
		 */
		mod_num = give_free_module();
		mod_num |= MOD_EXPORT;				/* als Exporter kennzeichnen */
		module.smStruct[mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
		memset(module.smStruct[mod_num & MOD_ID_MASK], 0, sizeof(GARGAMEL));
		export_mabs = (const MOD_ABILITY *) module.comm.start_exp_module(export_modules[mod_index], MQUERY, NULL, module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num);

		if (export_mabs->ext_flag & M_MORE)
			change_object(&wind_s[WIND_EXPORT], EXPORT_OPTIONS, OS_ENABLED, 1);
		else
			change_object(&wind_s[WIND_EXPORT], EXPORT_OPTIONS, OS_DISABLED, 1);

		module.smStruct[mod_num & MOD_ID_MASK]->module_mode = M_EXIT;	/* Modulende "simulieren" */
		check_and_terminate(MTERM, mod_num & MOD_ID_MASK);
	}

	/*
	 * Infodialog ein- oder ausschalten
	 */
	if (button == EXMOD_INFO && mod_index != -1 && openmode && Dialog.expmodList.modNames[mod_index] != NULL
		&& strlen(Dialog.expmodList.modNames[mod_index]) != 0)
		Dialog.expmodList.infoOn(mod_index);
	else if (info && button == EXMOD_INFO_OK && openmode)
	{
		Dialog.expmodList.infoOff();
		Window.redraw(&wind_s[WIND_EXPORT], NULL, 0, 0);
	}
	/*
	 * Standard-Exporter festlegen
	 * Der Standard-Exporter wird bei "Sichern" aufgerufen.
	 */
	else if (button == EXPORT_STANDARD)
	{
		if (IsSelected(exp_form[EXPORT_STANDARD]))
		{
			Sys_info.defaultExporter = mod_index;
			menu_tree[FILE_SAVE].ob_state &= ~OS_DISABLED;
		} else
		{
			Sys_info.defaultExporter = -1;
			menu_tree[FILE_SAVE].ob_state |= OS_DISABLED;
		}
	}
	/*
	 * Optionsdialog îffnen
	 * Das Modul wird normal gestartet. Terminierung bei OK im Optionsfenster 
	 * oder dessen Schlieûen wird vorgenommen, wenn der Exporter laut 
	 * EXPORT_CONFIG exp_conf nicht lÑuft (also, wenn das Exportformular nicht mit
	 * diesem Exporter offen ist).
	 */
	else if (!info && button == EXPORT_OPTIONS)
	{
		mod_num = give_free_module();
		mod_num |= MOD_EXPORT;				/* als Exporter kennzeichnen */
		module.smStruct[mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
		memset(module.smStruct[mod_num & MOD_ID_MASK], 0x0, sizeof(GARGAMEL));

		module.comm.start_exp_module(export_modules[mod_index], MQUERY, NULL, module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num);

		module_start = get_module_start(module.bp[exp_conf.export_mod_num & MOD_ID_MASK]);
		modinfo = module_start->info;	/* Zeiger auf Modulinfostruktur */

		if (export_cnfblock[mod_index] == NULL)
		{
			export_cnflen[mod_index] = 0;
			export_cnfblock[mod_index] = load_from_modconf(modinfo, "", &export_cnflen[mod_index], MOD_MAGIC_EXPORT);
		}

		if (export_cnfblock[mod_index] != NULL)
		{
			void **pp = (void **)module.smStruct[mod_num & MOD_ID_MASK]->event_par;
			*pp = export_cnfblock[mod_index];
			module.smStruct[mod_num & MOD_ID_MASK]->event_par[2] = export_cnflen[mod_index];
		}


		export_path = export_modules[mod_index];
		module.bp[mod_num & MOD_ID_MASK] = (BASPAG *) module.comm.start_exp_module(export_path, MSTART, smurf_picture[active_pic], module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num);
		module.comm.start_exp_module(export_path, MMORE, smurf_picture[active_pic], module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num);

		change_object(&wind_s[WIND_EXPORT], EXPORT_OPTIONS, OS_UNSEL, 1);
	}
	/*
	 * Exporter starten
	 */
	else if ((!info && button == START_EMOD) || (klicks == 2 && button >= EMODULE1 && button <= EMODULE9)
			 || my_scancode == SCAN_RETURN || my_scancode == SCAN_ENTER)
	{
		change_object(&wind_s[WIND_EXPORT], START_EMOD, OS_UNSEL, 1);

		if (!picthere)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOPIC_TO_SAVE].TextCast, NULL, NULL, NULL, 1);
		} else
		{
			/*
			 * freie Modulstruktur ermitteln
			 */
			mod_num = give_free_module();
			if (mod_num == -1)
			{
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
				return;
			}

			mod_num |= MOD_EXPORT;			/* als Exporter kennzeichnen */
			exp_conf.export_mod_num = mod_num;

			module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
			memset(module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK], 0x0, sizeof(GARGAMEL));

			/*
			 * Modul analysieren
			 */
			export_path = export_modules[mod_index];
			export_mabs = (const MOD_ABILITY *) module.comm.start_exp_module(export_path, MQUERY, NULL, module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num);
			memcpy(&expmabs, export_mabs, sizeof(MOD_ABILITY));

			module_start = get_module_start(module.bp[exp_conf.export_mod_num & MOD_ID_MASK]);
			modinfo = module_start->info;	/* Zeiger auf Modulinfostruktur */

			export_depth[0] = expmabs.depth1;
			export_depth[1] = expmabs.depth2;
			export_depth[2] = expmabs.depth3;
			export_depth[3] = expmabs.depth4;
			export_depth[4] = expmabs.depth5;
			export_depth[5] = expmabs.depth6;
			export_depth[6] = expmabs.depth7;
			export_depth[7] = expmabs.depth8;

			export_format[0] = expmabs.form1;
			export_format[1] = expmabs.form2;
			export_format[2] = expmabs.form3;
			export_format[3] = expmabs.form4;
			export_format[4] = expmabs.form5;
			export_format[5] = expmabs.form6;
			export_format[6] = expmabs.form7;
			export_format[7] = expmabs.form8;

			prepare_depthpopup();

			check_and_terminate(MTERM, exp_conf.export_mod_num & MOD_ID_MASK);	/* Modul wieder terminieren */

			/* 
			 * und einen MSTART schicken (nur beim ersten Aufruf des Exporters)
			 * Dann kann sich das Modul initialisieren, z.B. Rsrc-fix u.Ñ.
			 */
			module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
			memset(module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK], 0, sizeof(GARGAMEL));

			if (export_cnfblock[mod_index] == NULL)
			{
				export_cnflen[mod_index] = 0;
				export_cnfblock[mod_index] = load_from_modconf(modinfo, "", &export_cnflen[mod_index], MOD_MAGIC_EXPORT);
			}

			if (export_cnfblock[mod_index] != NULL)
			{
				void **pp = (void **) module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK]->event_par;
				*pp = export_cnfblock[mod_index];
				module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK]->event_par[2] = export_cnflen[mod_index];
			}

			module.bp[exp_conf.export_mod_num & MOD_ID_MASK] = (BASPAG *) start_exp_module(export_path, MSTART, smurf_picture[active_pic],
				module.bp[exp_conf.export_mod_num & MOD_ID_MASK], module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK], exp_conf.export_mod_num);

			openmode = 0;
			obj = -1;
			if (key_at_event == KEY_ALT)
			{
				openmode = 0;
				f_export_formular();
			} else
			{
				exp_conf.exp_dither = -1;	/* signalisiert, daû die Autokonversion gÅltig ist */
				f_save_pic(&expmabs);
			}
		}
	}
}


/* save_file -----------------------------------------------------
	Lîst das Speichern des aktiven Bildes mit dem Defaultexporter
	mittels f_save_pic() aus. Ein ôffnen des Exportdialoges steht
	hier (noch) nicht zur VerfÅgung.
	Der Modulkonfigurationsblock wird vor dem Aufruf von f_save_pic()
	an das Modul geschickt.
	---------------------------------------------------------------*/
void save_file(void)
{
	short mod_num;
	short t;
	const MOD_INFO *modinfo;
	const MOD_ABILITY *export_mabs;
	MOD_ABILITY expmabs;
	const MODULE_START *module_start;
	char module_name[30];

	if (Sys_info.defaultExporter == -1)
		return;

	if (!picthere)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOPIC_TO_SAVE].TextCast, NULL, NULL, NULL, 1);
	} else
	{
		/*
		 * freie Modulstruktur ermitteln
		 */
		mod_num = give_free_module();
		if (mod_num == -1)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
			return;
		}

		mod_num |= MOD_EXPORT;				/* als Exporter kennzeichnen */
		exp_conf.export_mod_num = mod_num;

		module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
		memset(module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK], 0x0, sizeof(GARGAMEL));
		export_path = export_modules[Sys_info.defaultExporter];

		/*
		 * Modul analysieren
		 */
		export_mabs = (MOD_ABILITY *) module.comm.start_exp_module(export_path, MQUERY, NULL, module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num);
		memcpy(&expmabs, export_mabs, sizeof(MOD_ABILITY));

		module_start = get_module_start(module.bp[exp_conf.export_mod_num & MOD_ID_MASK]);
		modinfo = module_start->info;	/* Zeiger auf Modulinfostruktur */
		strncpy(module_name, modinfo->mod_name, 30);

		if (key_at_event & KEY_ALT)
		{
			export_depth[0] = expmabs.depth1;
			export_depth[1] = expmabs.depth2;
			export_depth[2] = expmabs.depth3;
			export_depth[3] = expmabs.depth4;
			export_depth[4] = expmabs.depth5;
			export_depth[5] = expmabs.depth6;
			export_depth[6] = expmabs.depth7;
			export_depth[7] = expmabs.depth8;

			export_format[0] = expmabs.form1;
			export_format[1] = expmabs.form2;
			export_format[2] = expmabs.form3;
			export_format[3] = expmabs.form4;
			export_format[4] = expmabs.form5;
			export_format[5] = expmabs.form6;
			export_format[6] = expmabs.form7;
			export_format[7] = expmabs.form8;

			prepare_depthpopup();
		}

		check_and_terminate(MTERM, exp_conf.export_mod_num & MOD_ID_MASK);	/* Modul wieder terminieren */
		module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
		memset(module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK], 0x0, sizeof(GARGAMEL));

		/*
		 * Exporternummer suchen, um den Konfigurationsblock zu ermitteln
		 * eigentlich mÅûte der Index des Blocks gleich mod_index, also
		 * Sys_info.defaultExporter sein, das muû noch geprÅft werden.
		 */
		for (t = 0; t < Dialog.expmodList.anzahl; t++)
		{
			if (strncmp(module_name, Dialog.expmodList.modNames[t], strlen(module_name)) == 0)
				break;
		}

		if (export_cnfblock[t] == NULL)
		{
			export_cnflen[t] = 0;
			export_cnfblock[t] = load_from_modconf(modinfo, "", &export_cnflen[t], MOD_MAGIC_EXPORT);
		}

		if (export_cnfblock[t] != NULL)
		{
			void **pp = (void **)module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK]->event_par;
			*pp = export_cnfblock[t];
			module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK]->event_par[2] = export_cnflen[t];
		}

		module.bp[exp_conf.export_mod_num & MOD_ID_MASK] = (BASPAG *) start_exp_module(export_path, MSTART, smurf_picture[active_pic],
			module.bp[exp_conf.export_mod_num & MOD_ID_MASK], module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK], exp_conf.export_mod_num);

		obj = -1;
		if (key_at_event & KEY_ALT)
		{
			openmode = 0;
			f_export_formular();
		} else
		{
			exp_conf.exp_dither = -1;	/* signalisiert, daû die Autokonversion gÅltig ist */
			f_save_pic(&expmabs);
		}
	}
}


/*-----------------------------------------------------------------	*/
/*							f_save_pic								*/
/*	Handled das Aufrufen der Dither-, Convert- und Codierungsfunk-	*/
/*	tionen fÅrs Exportieren mit dem Modul export_mabs.				*/
/*	Zu exportierendes Bild ist immer smurf_picture[active_pic]		*/
/*-----------------------------------------------------------------	*/
int f_save_pic(MOD_ABILITY *export_mabs)
{
	char savepathback[SM_PATH_MAX];
	char *name;
	char *save_ext;
	char *expext;
	char module_name[30];
	const MODULE_START *module_start;
	char titleString[64];
	uint8_t *picture;
	short max_expdepth;
	short dest_format;
	short ext_number;
	short dest_colsys;
	short old_picdepth;
	short t;
	char str[10];
	long len;
	long whlen;

	EXPORT_PIC *pic_to_save;
	SMURF_PIC *pic_to_export;
	SMURF_PIC *converted_pic;
	const MOD_INFO *modinfo;
	GARGAMEL *exp_gstruct;
	BASPAG *exp_bp;
	WINDOW *saved_window;

	exp_gstruct = module.smStruct[exp_conf.export_mod_num & MOD_ID_MASK];
	exp_bp = module.bp[exp_conf.export_mod_num & MOD_ID_MASK];

	/*
	 * Maximale Farbtiefe des Exporters und zugehîriges Datenformat ermitteln
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

	exp_conf.exp_depth = max_expdepth;
	exp_conf.exp_form = dest_format;

	pic_to_export = smurf_picture[active_pic];

	/*
	 * Bild konvertieren / dithern
	 */
	if (export_mabs != NULL)
	{
		/*----------- neue SMURF_PIC generieren -------------*/
		converted_pic = alloc_smurfpic(pic_to_export, TRUE);
		converted_pic->screen_pic = NULL;

		/* --- Maximale Farbtiefe des Xporters ausreichend? */
		/* sollte eigentlich mit kleinster Exporterfarbtiefe verglichen werden */
		if (pic_to_export->depth > max_expdepth)
		{
			if (pic_to_export->depth == 24 && max_expdepth == 16)
			{
				/* FIXME: translate */
				if (Dialog.winAlert.openAlert("Beim Export in 16 Bit wird das Bild an FarbqualitÑt verlieren! Trotzdem in 16 Bit exportieren?", "Nein", " Ja ", NULL, 1) == 1)
					return -2;
				else
					tfm_24_to_16(converted_pic, NEW);
			} else
			{
				/*
				 * Farbsystem holen
				 */
				old_picdepth = pic_to_export->depth;
				pic_to_export->depth = max_expdepth;

				start_exp_module(export_path, MCOLSYS, pic_to_export, exp_bp, exp_gstruct, exp_conf.export_mod_num);
				if (exp_gstruct->module_mode == M_COLSYS)
					dest_colsys = exp_gstruct->event_par[0];
				else
					dest_colsys = RGB;
				pic_to_export->depth = old_picdepth;

				if (max_expdepth == 8 && dest_colsys == GREY)
				{
					/* FIXME: translate */
					if (Dialog.winAlert.openAlert("Das Bild muû fÅr den Export in 8 Bit nach Graustufen gewandelt werden! Sind Sie damit einverstanden?", "Nein", " Ja ", NULL, 1) == 1)
						return -2;

					if (f_convert(converted_pic, export_mabs, GREY, NEW, 0) != 0)
						return -2;
				} else if (max_expdepth < 16 && max_expdepth < pic_to_export->depth)
				{
					if (exp_conf.exp_dither == -1)
					{
						exp_conf.exp_dither = Sys_info.AutoconvDither;

						if (max_expdepth == 1)
							exp_conf.exp_colred = CR_SYSPAL;
						else
							exp_conf.exp_colred = Sys_info.AutoconvPalmode;
					}

					if (dither_for_export(export_mabs, max_expdepth, dest_format, converted_pic) != 0)
						return -2;
				}
			}
		} else if (f_convert(converted_pic, export_mabs, 255, NEW, 0) != 0)
		{
			return -2;
		}
	}

	/*  
	 * Extension ermitteln, Exporter aufrufen 
	 */
	module.comm.start_exp_module(export_path, MEXTEND, converted_pic, exp_bp, exp_gstruct, exp_conf.export_mod_num);
	module_start = get_module_start(exp_bp);
	modinfo = module_start->info;	/* Zeiger auf Modulinfostruktur */

	if (exp_gstruct->module_mode == M_EXTEND)
		ext_number = exp_gstruct->event_par[0] - 1;	/* Da 0 der erste ist */
	else
		ext_number = 0;

	expext = modinfo->ext[ext_number];

	/*
	 * Speichername zusammenbasteln 
	 */
	strncpy(module_name, modinfo->mod_name, 28);
	strcpy(savepathback, savepath);		/* Backup vom Savepath */
	strcpy(strrchr(savepath, '\\') + 1, strrchr(smurf_picture[active_pic]->filename, '\\') + 1);
	strlwr(expext);

	if ((save_ext = strrchr(savepath, '.')) != NULL && save_ext > strrchr(savepath, '\\'))
		strcpy(save_ext + 1, expext);	/* Extender gegen neuen ersetzen */
	else
	{
		strcat(savepath, ".");			/* Punkt und */
		strncat(savepath, expext, 4);	/* neuen Extender an Filenamen ohne Extender hÑngen */
	}

	/* FIXME: translate */
	Dialog.busy.reset(0, "codiere Bild...");

	pic_to_save = module.comm.start_exp_module(export_path, MEXEC, converted_pic, exp_bp, exp_gstruct, exp_conf.export_mod_num);

	saved_window = &picture_windows[active_pic];

	if (exp_gstruct->module_mode == M_MEMORY)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOMEM_EXPORT].TextCast, NULL, NULL, NULL, 1);
		strcpy(savepath, savepathback);
	} else if (exp_gstruct->module_mode == M_PICERR)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOMEM_PICERR].TextCast, NULL, NULL, NULL, 1);
		strcpy(savepath, savepathback);
	} else if (exp_gstruct->module_mode == M_DONEEXIT)
	{
		picture = pic_to_save->pic_data;
		len = pic_to_save->f_len;
		f_set_syspal();
		Dialog.busy.reset(0, "speichere Bild...");

		strcpy(titleString, expext);
		strcat(titleString, " speichern");
		if (file_save(titleString, picture, len) == TRUE)
		{
			if (strcmp(smurf_picture[active_pic]->filename, savepath) != 0)
			{
				/* FIXME: translate */
				if (Dialog.winAlert.openAlert("Neuen Namen Åbernehmen?", "Nein", " Ja ", NULL, 1) == 2)
				{
					Comm.renameOLGA(smurf_picture[active_pic]->filename, savepath);
					strncpy(smurf_picture[active_pic]->filename, savepath, sizeof(smurf_picture[active_pic]->filename));

					memset(saved_window->wtitle, 0, sizeof(saved_window->wtitle));
					itoa(smurf_picture[active_pic]->pic_width, str, 10);
					strcpy(saved_window->wtitle, str);
					strcat(saved_window->wtitle, "*");
					itoa(smurf_picture[active_pic]->pic_height, str, 10);
					strcat(saved_window->wtitle, str);

					whlen = strlen(saved_window->wtitle);
					if (whlen < 12)
						strncat(saved_window->wtitle, "            ", 12 - whlen);

					if ((name = strrchr(savepath, '\\')) != NULL)
						name++;
					else
						name = savepath;
					strcat(saved_window->wtitle, shorten_name(name, 41 - (short)strlen(saved_window->wtitle)));
					Window.windSet(saved_window->whandlem, WF_NAME, LONG2_2INT(saved_window->wtitle), 0, 0);

					t = 0;
					while (Dialog.picMan.picmanList[t] != active_pic)
						t++;
					picnames[t] = saved_window->wtitle + 12;
					f_listfield(Dialog.picMan.window, F_REDRAW, 0, &Dialog.picMan.pictureList);
				}
			}

			if (saved_window->wtitle[11] == '*')
				Comm.updateOLGA(savepath);

			imageWindow.toggleAsterisk(saved_window, 0);
			Comm.updateDhst(savepath);
		} else
		{
			strcpy(savepath, savepathback);
		}

		/* if, damit bei raw-Export nicht das Original flîten geht ... */
		if (pic_to_save->pic_data != pic_to_export->pic_data)
		{
			SMfree(pic_to_save->pic_data);
		}

		SMfree(pic_to_save);
	}

	/* Muû sein, da bei Export ohne Konvertierung converted_pic noch aufs Original zeigt! */
	if (converted_pic->pic_data != pic_to_export->pic_data)
		SMfree(converted_pic->pic_data);

	SMfree(converted_pic);

	/*
	 * Exportformular zu, Modul terminieren und tschÅû
	 */
	Dialog.close(FORM_EXPORT);
	check_and_terminate(MTERM, exp_conf.export_mod_num & MOD_ID_MASK);
	Dialog.busy.ok();

	return 0;
}


/********************************************************************/
/********************************************************************/
/*						Bild fÅr Export dithern						*/
/*	Es wird eine SYSTEM_INFO-Struktur gefÑlscht und die zum Export	*/
/*	nîtigen Daten eingehÑngt, gleiches gilt fÅr DISPLAY_MODES.		*/
/*	Dithermodule / Konverter werden aufgerufen, Palette Åbertragen, */
/*	etc.															*/
/********************************************************************/
/********************************************************************/
short dither_for_export(const MOD_ABILITY *mod_abs, short max_expdepth, short dest_format, SMURF_PIC *converted_pic)
{
	uint8_t *dest_pal;
	uint8_t dest_depth;
	short dest_form;
	short t;
	SYSTEM_INFO Cheat_sysinfo;
	DISPLAY_MODES new_display;

	dest_depth = max_expdepth;
	dest_form = dest_format;

	/*----------- Sysinfo zum Dithern generieren -----------*/
	memcpy(&Cheat_sysinfo, &Sys_info, sizeof(SYSTEM_INFO));

	/*------------------------ Sind wir in einer anderen Farbtiefe (muû eine andere NCT geladen werden)? */
	if (exp_conf.exp_colred == CR_SYSPAL && dest_depth != Sys_info.bitplanes)
	{
		if (loadNCT(dest_depth, &Cheat_sysinfo) != 0)
		{
			/* FIXME: translate */
			Dialog.winAlert.openAlert("Kein NCT-File fÅr die gewÑhlte Farbtiefe gefunden. Dithering unmîglich.", NULL, NULL, NULL, 1);
			return -1;
		}
	}

	Cheat_sysinfo.bitplanes = dest_depth;
	Cheat_sysinfo.Max_col = (1 << dest_depth) - 1;

	/*---------------- Neue Display_modes zurechtbescheiûen */
	memcpy(&new_display, &Display_Opt, sizeof(DISPLAY_MODES));

	new_display.syspal_24 = exp_conf.exp_colred;
	new_display.syspal_8 = exp_conf.exp_colred;
	new_display.syspal_4 = exp_conf.exp_colred;
	new_display.dither_24 = exp_conf.exp_dither;
	new_display.dither_8 = exp_conf.exp_dither;
	new_display.dither_4 = exp_conf.exp_dither;

	export_dither_dispatcher(converted_pic, &Cheat_sysinfo, &new_display, exp_conf.exp_fix_red, exp_conf.exp_fix_green, exp_conf.exp_fix_blue);

	/*---------------- Bilddaten aus dem MFDB Åbertragen */
	converted_pic->pic_data = converted_pic->screen_pic->fd_addr;
	converted_pic->depth = Cheat_sysinfo.bitplanes;
	converted_pic->format_type = dest_form;

	/* und gleich den nun unnîtigen MFDB freigeben */
	free(converted_pic->screen_pic);

	/*---------------- Palette Åbertragen */
	dest_pal = converted_pic->palette;
	for (t = 0; t < SM_PALETTE_MAX; t++)
	{
		*dest_pal++ = converted_pic->red[t];
		*dest_pal++ = converted_pic->grn[t];
		*dest_pal++ = converted_pic->blu[t];
	}

	/*---------------- Und jetzt noch konvertieren, dann sind wir fertig. */
	f_convert(converted_pic, mod_abs, 255, SAME, 0);

	/*---------------- alles aus der Cheat_sysinfo wegwerfen, -----*/
	/*----------	was in der load_nct angefordert wurde ----*/
	if (exp_conf.exp_colred == CR_SYSPAL && dest_depth != Sys_info.bitplanes)
	{
		free(Cheat_sysinfo.red);
		free(Cheat_sysinfo.grn);
		free(Cheat_sysinfo.blu);
		free(Cheat_sysinfo.plane_table);
		SMfree(Cheat_sysinfo.nc_table);
	}

	return 0;
}



/*-----------------------------------------------------------------	*/
/*						init_exmod_info								*/
/*	Baut den Dialog fÅr die Exportmodulinfo des Modules mod_index	*/
/*	aus dem Exporter-Listfeld auf									*/
/*-----------------------------------------------------------------	*/
void init_exmod_info(short mod_index)
{
	const MODULE_START *module_start;
	char str[10];
	char *filename;
	short mod_num;
	const MOD_INFO *info_mi;
	const MOD_ABILITY *info_mabs;


	mod_num = give_free_module();
	mod_num |= MOD_EXPORT;					/* als Exporter kennzeichnen */
	module.smStruct[mod_num & MOD_ID_MASK] = (GARGAMEL *) malloc(sizeof(GARGAMEL));
	memset(module.smStruct[mod_num & MOD_ID_MASK], 0x0, sizeof(GARGAMEL));
	if ((info_mabs = (const MOD_ABILITY *) module.comm.start_exp_module(export_modules[mod_index], MQUERY, NULL, module.bp[mod_num & MOD_ID_MASK], module.smStruct[mod_num & MOD_ID_MASK], mod_num)) == NULL)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[EMOD_START_ERR].TextCast, NULL, NULL, NULL, 1);

	module_start = get_module_start(module.bp[mod_num & MOD_ID_MASK]);
	info_mi = module_start->info;

	strncpy(Dialog.expmodList.infoTree[EI_MODNAME].TextCast, Dialog.expmodList.modNames[mod_index], 27);

	BCD2string(Dialog.expmodList.infoTree[EI_MODVER].TextCast, info_mi->version);

	strcpy(Dialog.expmodList.infoTree[EI_MODAUTOR].TextCast, "");
	strncat(Dialog.expmodList.infoTree[EI_MODAUTOR].TextCast, info_mi->autor, 27);

	filename = strrchr(export_modules[mod_index], '\\');
	strncpy(Dialog.expmodList.infoTree[EI_MODPATH].TextCast, filename + 1, 12);

	strcpy(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, "");

	if (info_mabs->depth1 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth1, str, 10), 2);
	}
	if (info_mabs->depth2 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth2, str, 10), 2);
	}
	if (info_mabs->depth3 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth3, str, 10), 2);
	}
	if (info_mabs->depth4 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth4, str, 10), 2);
	}
	if (info_mabs->depth5 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth5, str, 10), 2);
	}
	if (info_mabs->depth6 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth6, str, 10), 2);
	}
	if (info_mabs->depth7 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth7, str, 10), 2);
	}
	if (info_mabs->depth8 > 0)
	{
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, ", ", 2);
		strncat(Dialog.expmodList.infoTree[EI_MODDEPTH].TextCast, itoa(info_mabs->depth8, str, 10), 2);
	}
#if 0
	/*---- Schlumpfine - Versionsnummer auslesen ----*/
	version = (unsigned short)module_start->version;	/* Versionsnummer */
	BCD2string(version, Dialog.expmodList.infoTree[EI_SMURFVER].TextCast);
#endif

	module.smStruct[mod_num & MOD_ID_MASK]->module_mode = M_EXIT;	/* Modulende "simulieren" */
	check_and_terminate(MTERM, mod_num & MOD_ID_MASK);
	wind_s[WIND_EXPORT].resource_form = Dialog.expmodList.infoTree;

	Dialog.expmodList.infoTree[0].ob_x = wind_s[WIND_EXPORT].wx;
	Dialog.expmodList.infoTree[0].ob_y = wind_s[WIND_EXPORT].wy;

	Window.redraw(&wind_s[WIND_EXPORT], NULL, 0, 0);
}


/*------------------ Ausschalten der Exportmodul-Info -------------*/
void exmod_info_off(void)
{
	/*
	 * ist die Info Åberhaupt eingeschaltet?
	 */
	if (wind_s[WIND_EXPORT].resource_form == Dialog.expmodList.infoTree)
	{
		change_object(&wind_s[WIND_EXPORT], EXMOD_INFO_OK, OS_UNSEL, 0);
		wind_s[WIND_EXPORT].resource_form = Dialog.expmodList.tree;

		Dialog.expmodList.tree[0].ob_x = wind_s[WIND_EXPORT].wx;
		Dialog.expmodList.tree[0].ob_y = wind_s[WIND_EXPORT].wy;

		change_object(&wind_s[WIND_EXPORT], EXMOD_INFO, OS_UNSEL, 0);
	}
}


/*-----------------------------------------------------------------	*/
/*	NCT-File, Bintable und Palette fÅr Export in Bildschirmfremden	*/
/*	Farbtiefen temporÑr einladen.									*/
/*	Da z.B. im 4Bit-Screenmodus keine Palette und Umsetzungs-		*/
/*	tabellen vorhanden sind, werden diese sofern vorhanden eingela-	*/
/*	den. D.h., sie mÅssen, damit Systempaletten-Export in anderen	*/
/*	Farbtiefen mîglich ist, erstmal erzeugt werden. 				*/
/*	Unschîn, geht aber nicht anders.								*/
/*	nc_table, plane_table, red, grn und blu werden neu angefordert!	*/
/*-----------------------------------------------------------------	*/
short loadNCT(short loadplanes, SYSTEM_INFO *sysinfo)
{
	char tablename[SM_PATH_MAX];
	char bpstring[4];
	char *palbuf;
	short attrib = 0;
	short maxc;
	short t;
	WORD *palette;

	Dialog.busy.reset(0, "lade NCT");

	if (loadplanes > 8)
		return 0;

	/* Tabellen-Filename zusammenbasteln */
	strcpy(tablename, Sys_info.home_path);
	strcat(tablename, "\\smp.");
	itoa(loadplanes, bpstring, 10);
	strcat(tablename, bpstring);

	maxc = 1 << loadplanes;

	/* Mit einem Trick testen, ob die Datei vorhanden ist */
	if (Fattrib(tablename, 0, attrib) >= 0)
	{
		if ((palbuf = fload(tablename, 0)) == NULL)
			return -1;

		sysinfo->nc_table = SMalloc(32800L);
		memcpy(sysinfo->nc_table, palbuf + maxc * 6 + SM_PALETTE_MAX, 32768L);

		sysinfo->plane_table = malloc(SM_PALETTE_MAX);
		memcpy(sysinfo->plane_table, palbuf + maxc * 6, SM_PALETTE_MAX);
		palette = (WORD *)palbuf;

		sysinfo->red = malloc(SM_PALETTE_MAX * sizeof(*sysinfo->red));
		sysinfo->grn = malloc(SM_PALETTE_MAX * sizeof(*sysinfo->grn));
		sysinfo->blu = malloc(SM_PALETTE_MAX * sizeof(*sysinfo->blu));

		for (t = 0; t < SM_PALETTE_MAX; t++)
		{
			sysinfo->pal_red[t] = palette[t * 3 + 0];
			sysinfo->pal_green[t] = palette[t * 3 + 1];
			sysinfo->pal_blue[t] = palette[t * 3 + 2];
			sysinfo->red[t] = (WORD) (palette[t * 3 + 0] * 31L / 1000L);
			sysinfo->grn[t] = (WORD) (palette[t * 3 + 1] * 31L / 1000L);
			sysinfo->blu[t] = (WORD) (palette[t * 3 + 2] * 31L / 1000L);
		}

		return 0;
	}
	return -1;
}
