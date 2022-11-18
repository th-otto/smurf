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

/*------------------------------------------------------------------*/
/* Modul-Funkitonen fÅr SMURF                                       */
/*																	*/
/* Modulschnitstelle SCHLUMPFINE V0.8								*/
/*	Olaf Piesche													*/
/*------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "import.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurf.h"
#include "globdefs.h"
#include "plugin.h"

#include "smurfobs.h"
#include "ext_obs.h"

static char modname[25];

/* Standardpaletten im VDI-Format fÅr TC-Iconwandlung */
#if 0									/* unused */
static char stdpal1bit[] = {
	0xff, 0xff, 0xff,
	0x00, 0x00, 0x00
};

static char stdpal2bit[] = {
	0xff, 0xff, 0xff,
	0xff, 0x00, 0x00,
	0x00, 0xff, 0x00,
	0x00, 0x00, 0x00
};
#endif
static char stdpal4bit[] = {
	0xff, 0xff, 0xff,
	0xff, 0x00, 0x00,
	0x00, 0xff, 0x00,
	0xff, 0xff, 0x00,
	0x00, 0x00, 0xff,
	0xff, 0x00, 0xff,
	0x00, 0xff, 0xff,
	0xc0, 0xc0, 0xc0,
	0x80, 0x80, 0x80,
	0xb6, 0x00, 0x00,
	0x00, 0xb6, 0x00,
	0xb6, 0xb6, 0x00,
	0x00, 0x00, 0xb6,
	0xb6, 0x00, 0xb6,
	0x00, 0xb6, 0xb6,
	0x00, 0x00, 0x00
};


/*-----------------------------------------------------------------	*/
/* 				Startfunktion fÅr Import-Modul						*/
/*-----------------------------------------------------------------	*/
short start_imp_module(char *modpath, SMURF_PIC *imp_pic)
{
	char *textseg_begin;
	long mod_magic;
	short (*module_main)(GARGAMEL *smurf_struct);
	short module_return;
	short back;
	long ProcLen;
	long temp;
	long lback;
	char alstring[80];

	BASPAG *mod_basepage;
	MOD_INFO *module_info;
	GARGAMEL sm_struct;


	/* Modul als Overlay laden und Basepage ermitteln */
	temp = Pexec(3, modpath, "", NULL);
	if (temp < 0)
	{
		strcpy(alstring, Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast);
		strcat(alstring, strrchr(modpath, '\\') + 1);
		Dialog.winAlert.openAlert(alstring, NULL, NULL, NULL, 1);
		module_return = M_STARTERR;
	} else
	{
		mod_basepage = (BASPAG *) temp;

		mod_magic = get_modmagic(mod_basepage);	/* Zeiger auf Magic (muû MOD_MAGIC_IMPORT sein!) */
		if (mod_magic != MOD_MAGIC_IMPORT)
			return M_MODERR;

		/* LÑnge des gesamten Tochterprozesses ermitteln */
		ProcLen = get_proclen(mod_basepage);
		back = _Mshrink(mod_basepage, ProcLen);	/* Speicherblock verkÅrzen */
		if (back != 0)
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_SHRINK_ERR].TextCast, NULL, NULL, NULL, 1);

		mod_basepage->p_hitpa = (void *) ((char *) mod_basepage + ProcLen);

		lback = Pexec(4, NULL, (char *) mod_basepage, NULL);
		if (lback < 0L)
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);

		textseg_begin = mod_basepage->p_tbase;	/* Textsegment-Startadresse holen */

		module_info = *((MOD_INFO **) (textseg_begin + MOD_INFO_OFFSET));	/* Zeiger auf Modulinfostruktur */
		memset(modname, 0x0, 25);
		strncpy(modname, module_info->mod_name, 24);

		sm_struct.smurf_pic = imp_pic;
		sm_struct.services = &global_services;

		module_main = (short (*)(GARGAMEL *)) (textseg_begin + MAIN_FUNCTION_OFFSET);
		module_return = module_main(&sm_struct);

#if 0
		Pexec(102, NULL, mod_basepage, NULL);	/* Modul systemkonform tîten */
#endif
		SMfree(mod_basepage->p_env);
		SMfree(mod_basepage);			/* Modul-Basepage freigeben */
	}

	return module_return;
}


/*-----------------------------------------------------------------	*/
/* 				Startfunktion fÅr Edit-Modul						*/
/*-----------------------------------------------------------------	*/
BASPAG *start_edit_module(char *modpath, BASPAG *edit_basepage, short mode, short mod_id, GARGAMEL *sm_struct)
{
	void (*module_main)(GARGAMEL *smurf_struct);
	char *textseg_begin;
	short back;
	long ProcLen;
	long temp;
	long lback;
	long mod_magic;

	MOD_ABILITY *mod_abs;


	if (mod_id < 0 || mod_id >= MAX_MODS)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[SMURF_ID_ERR].TextCast, NULL, NULL, NULL, 1);

	/*
	 * Modul als Overlay laden und Basepage ermitteln
	 */
	if (edit_basepage == NULL)			/* Modul wurde noch nicht gestartet! */
	{
		temp = Pexec(3, modpath, "", NULL);
		if (temp < 0)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[EMOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
			sm_struct->module_mode = M_STARTERR;
			return NULL;
		} else
		{
			edit_basepage = (BASPAG *) temp;

			mod_magic = get_modmagic(edit_basepage);	/* Zeiger auf Magic (muû MOD_MAGIC_EDIT sein!) */
			if (mod_magic != MOD_MAGIC_EDIT)
			{
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
				return NULL;
			}

			/* LÑnge des gesamten Tochterprozesses ermitteln */
			ProcLen = get_proclen(edit_basepage);
			back = _Mshrink(edit_basepage, ProcLen);	/* Speicherblock verkÅrzen */
			if (back != 0)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_SHRINK_ERR].TextCast, NULL, NULL, NULL, 1);

			edit_basepage->p_hitpa = (void *) ((char *) edit_basepage + ProcLen);

			lback = Pexec(4, NULL, (char *) edit_basepage, NULL);
			if (lback < 0L)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
		}
	}

	if (edit_basepage > 0)
	{
		textseg_begin = (char *) (edit_basepage->p_tbase);

		/* Modulkennung (als wievieltes Modul gestartet?) */
		if (mode == MSTART)
			sm_struct->module_number = mod_id;

		/* Message von Smurf */
		sm_struct->module_mode = mode;	/* 0=laden und Starten, 1=Redraw ausfÅhren , -1 = Beenden, 2=aktivieren */

		/* Funktionen einhÑngen */
		sm_struct->services = &global_services;

		/* EVENT im Modulfenster ! */
		if (mode == MBEVT || mode == MKEVT || mode == SMURF_AES_MESSAGE)
		{
			sm_struct->mousex = mouse_xpos;
			sm_struct->mousey = mouse_ypos;
			sm_struct->klicks = klicks;

			if (mode != SMURF_AES_MESSAGE)
				sm_struct->event_par[0] = obj;

			if (mode == MKEVT)
			{
				sm_struct->event_par[1] = key_scancode;
				sm_struct->event_par[2] = key_ascii;
				sm_struct->event_par[3] = key_at_event;
			}
		}

		if (mode != MQUERY)
		{
			if (mode == MEXEC)
				graf_mouse(BUSYBEE, dummy_ptr);

			module_main = (void (*)(GARGAMEL *smurf_struct)) (textseg_begin + MAIN_FUNCTION_OFFSET);
			module_main(sm_struct);

			graf_mouse(ARROW, dummy_ptr);
		}

		if (mode == MQUERY)
		{
			module.bp[mod_id & 0xFF] = edit_basepage;
			mod_abs = *((MOD_ABILITY **) (textseg_begin + MOD_ABS_OFFSET));	/* Module Abilities */
			return ((BASPAG *) mod_abs);
		}
	}

	return edit_basepage;
}


/*-----------------------------------------------------------------	*/
/* 				Startfunktion fÅr Export-Modul						*/
/*-----------------------------------------------------------------	*/
EXPORT_PIC *start_exp_module(char *modpath, short message, SMURF_PIC *pic_to_export, BASPAG *exbase, GARGAMEL *sm_struct, short module_number)
{
	char *textseg_begin;
	short back;
	long ProcLen;
	long temp;
	long lback;
	long mod_magic;
	MOD_ABILITY *mod_abs;
	EXPORT_PIC *encoded_pic;
	EXPORT_PIC *(*export_module_main)(GARGAMEL *smurf_struct);
	BASPAG *export_basepage;

	export_basepage = (BASPAG *) exbase;

	/*
	 * Modul als Overlay laden und Basepage ermitteln
	 */
	if (export_basepage == NULL)
	{
		temp = Pexec(3, modpath, "", NULL);
		if (temp < 0)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
			sm_struct->module_mode = M_MODERR;
			return NULL;
		} else
		{
			export_basepage = (BASPAG *) temp;

			mod_magic = get_modmagic(export_basepage);	/* Zeiger auf Magic (muû MOD_MAGIC_EXPORT sein!) */
			if (mod_magic != MOD_MAGIC_EXPORT)
			{
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
				return NULL;
			}

			/* LÑnge des gesamten Tochterprozesses ermitteln */
			ProcLen = get_proclen(export_basepage);
			back = _Mshrink(export_basepage, ProcLen);	/* Speicherblock verkÅrzen */
			if (back != 0)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_SHRINK_ERR].TextCast, NULL, NULL, NULL, 1);

			export_basepage->p_hitpa = (void *) ((char *) export_basepage + ProcLen);

			lback = Pexec(4, NULL, (char *) export_basepage, NULL);
			if (lback < 0)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);
		}
	}

	if (export_basepage > 0)
	{
		textseg_begin = export_basepage->p_tbase;	/* Textsegment-Startadresse holen */

		sm_struct->services = &global_services;
		sm_struct->smurf_pic = pic_to_export;
		sm_struct->module_number = module_number;
		sm_struct->module_mode = message;

		/*
		 * EVENT im Modulfenster!
		 */
		if (message == MBEVT || message == MKEVT || message == SMURF_AES_MESSAGE)
		{
			sm_struct->mousex = mouse_xpos;
			sm_struct->mousey = mouse_ypos;
			sm_struct->klicks = klicks;
			if (message != SMURF_AES_MESSAGE)
				sm_struct->event_par[0] = obj;

			if (message == MKEVT)
			{
				sm_struct->event_par[1] = key_scancode;
				sm_struct->event_par[2] = key_ascii;
				sm_struct->event_par[3] = key_at_event;
			}
		}

		if (message != MQUERY)
		{
			if (message == MEXEC)
				graf_mouse(BUSYBEE, dummy_ptr);

			export_module_main = (EXPORT_PIC *(*)(GARGAMEL *))(textseg_begin + MAIN_FUNCTION_OFFSET);	/* Main-Funktion holen...   */
			encoded_pic = export_module_main(sm_struct);	/* ...und aufrufen  */

			graf_mouse(ARROW, dummy_ptr);
		}

		if (message == MQUERY || message == MEXTEND)
		{
			module.bp[module_number & 0xFF] = export_basepage;
			mod_abs = *((MOD_ABILITY **) (textseg_begin + MOD_ABS_OFFSET));	/* Module Abilities */
			return (EXPORT_PIC *) mod_abs;
		}
	}

	if (sm_struct->module_mode == M_WAITING)
		return (EXPORT_PIC *) export_basepage;
	else
		return encoded_pic;
}


/*	-----------------------------------------------------------	*/
/*					Modulevent bearbeiten						*/
/*	-----------------------------------------------------------	*/
short handle_modevent(short event_type, WINDOW *mod_window)
{
	WORD which_object;
	OBJECT *mod_resource;

	if (event_type & MU_BUTTON)
	{
		mod_resource = mod_window->resource_form;
		which_object = objc_find(mod_resource, 0, MAX_DEPTH, mouse_xpos, mouse_ypos);

		/*------- Hintergrundobjekt angeklickt */
		if (which_object == 0 || mod_resource[which_object].ob_type == G_FTEXT || IsDisabled(mod_resource[which_object])	/* || !IsSelectable(mod_resource[which_object]) &&
																															   (mod_resource[which_object].ob_type&0xff00) == 0 */ )
		{
			Window.top(mod_window->whandlem);

			if (mod_resource[which_object].ob_type != G_FTEXT)
				which_object = -1;
		}
		/*------- Objekt selektieren/deselektieren */
		else if (which_object != -1 && IsSelectable(mod_resource[which_object]))
		{
			if (!IsDisabled(mod_resource[which_object]))
			{
				if (!IsSelected(mod_resource[which_object]))
					change_object(mod_window, which_object, OS_SELECTED, 1);
				else
					change_object(mod_window, which_object, OS_UNSEL, 1);
			} else
				which_object = -1;
		}

	  /*------- Radiobutton angeklickt (Window toppen) */
		if (which_object != -1 && mod_resource[which_object].ob_flags & OF_RBUTTON)
			Window.topNow(mod_window);

		return which_object;
	}

	return 0;
}


/*	-----------------------------------------------------------	*/
/*				Handler fÅr EDIT-MODUL-MESSAGES					*/
/*	Diese MSGs kommen normalerweise als Reaktion auf MBEVT		*/
/*	-----------------------------------------------------------	*/
void f_handle_modmessage(GARGAMEL *smurf_struct)
{
	char *textseg_begin;
	short message;
	short back;
	short mod_num;
	WINDOW *window_to_handle;
	DISPLAY_MODES thisDisplay;
	BASPAG *bsp;
	MOD_INFO *modinfo;

	message = smurf_struct->module_mode;
	mod_num = smurf_struct->module_number;

	switch (message)
	{
		/*----------------------------- "Das Bild in meinem Fenster wurde geÑndert" */
	case M_MODPIC:
			/*-------- Display-Options temporÑr umbauen ---------	*/
		thisDisplay.dither_24 = Sys_info.PreviewDither;
		thisDisplay.dither_8 = Sys_info.PreviewDither;
		thisDisplay.dither_4 = Sys_info.PreviewDither;
		thisDisplay.syspal_24 = CR_SYSPAL;
		thisDisplay.syspal_8 = CR_SYSPAL;
		thisDisplay.syspal_4 = CR_SYSPAL;

			/*----------- dithern */
		if (smurf_struct->wind_struct->picture->screen_pic)
		{
			SMfree(smurf_struct->wind_struct->picture->screen_pic->fd_addr);
			free(smurf_struct->wind_struct->picture->screen_pic);
		}
		back = f_dither(smurf_struct->wind_struct->picture, &Sys_info, 0, NULL, &thisDisplay);
		if (back != 0)
			Dialog.winAlert.openAlert("Fehler beim Dithern des Previews!", NULL, NULL, NULL, 1); /* FIXME: translate */

		/* jetzt dem Modul sagen, daû fertig gedietert ist. */
		module.comm.start_edit_module("", module.bp[mod_num], MDITHER_READY, 0, smurf_struct);

		/* Dialog.busy.reset(128, smurf_struct->wind_struct->wtitle); */
		break;


	case M_MOREOK:
		if (smurf_struct->event_par[0] != 0)
			memorize_expmodConfig(module.bp[mod_num & 0xFF], module.smStruct[mod_num & 0xFF], 0);

		window_to_handle = smurf_struct->wind_struct;
		Window.close(window_to_handle->whandlem);
		break;

		/*
		 * Smurf soll anbei Åbermittelte Modulkonfiguration abspeichern
		 */
	case M_CONFSAVE:
		if (smurf_struct->event_par[0] != 0)
			memorize_expmodConfig(module.bp[mod_num & 0xFF], module.smStruct[mod_num & 0xFF], 1);

		break;

		/*
		 * Modul fordert Runtime ein Fadenkreuz an
		 */
	case M_CROSSHAIR:
		bsp = module.bp[mod_num & 0xFF];
		textseg_begin = bsp->p_tbase;
		modinfo = *((MOD_INFO **) (textseg_begin + MOD_INFO_OFFSET));	/* Zeiger auf Modulinfostruktur */

		position_markers[mod_num & 0xFF].anzahl = smurf_struct->event_par[0];
		position_markers[mod_num & 0xFF].mod_pic[0] = smurf_struct->event_par[1];

		/* Defaultkoordinaten erfragen */
		module.comm.start_edit_module("", module.bp[mod_num & 0xFF], MCH_DEFCOO, mod_num, smurf_struct);
		if (smurf_struct->module_mode == M_CHDEFCOO)
		{
			position_markers[mod_num & 0xFF].xpos[0] = smurf_struct->event_par[0];
			position_markers[mod_num & 0xFF].ypos[0] = smurf_struct->event_par[1];
		}

		if (modinfo->how_many_pix == 1 && position_markers[mod_num & 0xFF].anzahl >= 1)
		{
			position_markers[mod_num & 0xFF].mod_pic[0] = -2;	/* aktives Bild */
			position_markers[mod_num & 0xFF].smurfpic[0] = active_pic;	/* aktives Bild */
			module_pics[mod_num & 0xFF][0] = active_pic;
			imageWindow.drawCrosshair(&picture_windows[active_pic]);
		}
		break;

		/*
		 * Modul will ein Fadenkreuz wieder ausschalten
		 */
	case M_CH_OFF:
		back = position_markers[mod_num & 0xFF].smurfpic[0];
		position_markers[mod_num & 0xFF].smurfpic[0] = -1;
		Window.redraw(&picture_windows[back], NULL, 0, 0);
		break;
	}
}


/*	-----------------------------------------------------------	*/
/*				Importmodulmessage analysieren					*/
/*	-----------------------------------------------------------	*/
short analyze_message(short module_ret, short picture_to_load)
{
	char alertstr[40];
	short picerror = 0;

	switch (module_ret)
	{
		/*
		 * Bild konnte nicht erkannt werden
		 */
	case M_INVALID:
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[UNKNOWN_FORMAT].TextCast, NULL, NULL, NULL, 1);
		picerror = 1;
		break;

		/*
		 * Kein Speicher um Bild zu importieren
		 */
	case M_MEMORY:
		strcpy(alertstr, modname);
		strcat(alertstr, ": ");
		strcat(alertstr, Dialog.winAlert.alerts[NOMEM_IMPORT].TextCast);
		Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
		picerror = 1;
		break;

		/*
		 * Modulfehler
		 */
	case M_MODERR:
		strcpy(alertstr, modname);
		strcat(alertstr, ": Modulfehler!");
		Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
		picerror = 1;
		break;

		/*
		 * Fehler im Bild
		 */
	case M_PICERR:
		strcpy(alertstr, modname);
		strcat(alertstr, ": Fehler im Bild!");
		Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
		picerror = 1;
		break;

		/*
		 * Palettenfehler
		 */
	case M_PALERR:
		strcpy(alertstr, modname);
		strcat(alertstr, ": Keine oder fehlerhafte Palette!");
		Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
		picerror = 1;
		break;

		/*
		 * Bildtyp unbekannt
		 */
	case M_UNKNOWN_TYPE:
		strcpy(alertstr, modname);
		strcat(alertstr, ": unbekanntes Unterformat!");
		/*strcat(alertstr, "Bild bitte an uns schicken."); */
		Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
		picerror = 1;
		break;


		/*
		 * Bild wurde geladen - Checks Åber Bildbreite/-hîhe, Farbtiefe
		 */
	case M_PICDONE:
		if (smurf_picture[picture_to_load]->pic_width < 0 || smurf_picture[picture_to_load]->pic_height < 0)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[LOADPIC_SIZEERR].TextCast, NULL, NULL, NULL, 1);
			picerror = 1;
		} else if (smurf_picture[picture_to_load]->depth < 0 || smurf_picture[picture_to_load]->depth > 32)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[LOADPIC_DEPTHERR].TextCast, NULL, NULL, NULL, 1);
			picerror = 1;
		}
		break;

		/*
		 * Fehler beim Modulstart
		 */
	case M_STARTERR:
		picerror = 1;
		break;

		/*
		 * Fehler beim Modulstart
		 */
	case M_SILENT_ERR:
		picerror = 1;
		break;
	}

	return picerror;
}


/*------------------------------------------------------------*/
/* -----------------------------------------------------------*/
/* ----- Fenster fÅr Modul îffnen	--------------------------*/
/* -----------------------------------------------------------*/
/*------------------------------------------------------------*/
short f_open_module_window(WINDOW *module_window)
{
	WORD m_whandle;
	WORD m_wind_x, m_wind_y, m_wind_w, m_wind_h;
	WORD flags;

	module_window->pflag = 0;

	m_wind_x = module_window->wx;
	m_wind_y = module_window->wy;
	m_wind_w = module_window->ww;
	m_wind_h = module_window->wh;
	m_whandle = module_window->whandlem;

	flags = NAME | CLOSER | MOVER | SMALLER;

	/*------ ggfs. Formular zentrieren */
	if (module_window->wx == -1 || module_window->wy == -1)
		form_center(module_window->resource_form, &m_wind_x, &m_wind_y, &m_wind_w, &m_wind_h);

	module_window->resource_form->ob_x = m_wind_x;
	module_window->resource_form->ob_y = m_wind_y;

	/* aus Nettokoordinaten Bruttokoordinaten machen */
	wind_calc(WC_BORDER, flags, m_wind_x, m_wind_y, m_wind_w, m_wind_h, &m_wind_x, &m_wind_y, &m_wind_w, &m_wind_h);

	/*------ Userdefs initialisieren */
	f_treewalk(module_window->resource_form, 0);

	walk_module_tree(module_window, 0);

	if (m_whandle <= 0)
	{
		module_window->whandlem = wind_create(flags, m_wind_x, m_wind_y, m_wind_w, m_wind_h);
		if (module_window->whandlem < 0)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MODULE_WINDERR].TextCast, NULL, NULL, NULL, 1);
			return -1;
		}

		if (Sys_info.OSFeatures & BEVT)
			Window.windSet(module_window->whandlem, WF_BEVENT, 1, 0, 0, 0);

		Window.windSet(module_window->whandlem, WF_NAME, LONG2_2INT(module_window->wtitle), 0, 0);

		wind_open(module_window->whandlem, m_wind_x, m_wind_y, m_wind_w, m_wind_h);

		Window.windowToList(module_window);
	} else
		Window.top(m_whandle);

	/* aus Bruttokoordinaten wieder Nettokoordinaten machen */
	wind_calc(WC_WORK, flags, m_wind_x, m_wind_y, m_wind_w, m_wind_h, &m_wind_x, &m_wind_y, &m_wind_w, &m_wind_h);

	/* Werte in Struktur aktualisieren */
	module_window->wx = m_wind_x;
	module_window->wy = m_wind_y;
	module_window->ww = m_wind_w;
	module_window->wh = m_wind_h;

	/*------ Editobjekt initialisieren */
	if (module_window->editob != 0)
		objc_edit(module_window->resource_form, module_window->editob, 0, &(module_window->editx), ED_INIT);

	f_set_syspal();

	return 0;
}


/* --------------------------------------------------------------------------------	*/
/*	Ermittelt ID fÅr freies Modul und gibt diese oder -1 (keins mehr frei) zurÅck	*/
/* --------------------------------------------------------------------------------	*/
short give_free_module(void)
{
	short mod_num;

	/* freie Modulstruktur ermitteln */
	mod_num = 0;
	while (mod_num < MAX_MODS && module.smStruct[mod_num] != NULL)
		mod_num++;
	if (mod_num >= MAX_MODS)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_MORE_MODULES].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}
	return mod_num;
}


/* --------------------------------------------------------------------------------	*/
/*	  Terminiert nach PrÅfung von Message und Strukturen das Modul module_number	*/
/* --------------------------------------------------------------------------------	*/
void check_and_terminate(short mode, short module_number)
{
	/* MTERM: Programmodul-Speicher freigeben */
	if (module.smStruct[module_number]
		&& (mode == MTERM || module.smStruct[module_number]->module_mode == M_DONEEXIT
			|| module.smStruct[module_number]->module_mode == M_EXIT))
	{
#if 0
		Pexec(102, NULL, module.bp[module_number], NULL);
#endif
		SMfree(module.bp[module_number]->p_env);
		SMfree(module.bp[module_number]);
		module.bp[module_number] = NULL;
		free(module.smStruct[module_number]);
		module.smStruct[module_number] = NULL;
	}
}


/*-----------------------------------------------------------------	*/
/* 				Startfunktion fÅr Dither-Modul						*/
/*	mode = Message ans Modul										*/
/*	mod_id = Nummer des Dithermoduls (index auf BASPAG-Array)		*/
/*-----------------------------------------------------------------	*/
BASPAG *start_dither_module(short mode, short mod_id, DITHER_DATA *ditherdata)
{
	char *textseg_begin;

	short (*module_main)(DITHER_DATA *smurf_struct);

	BASPAG *dither_basepage;


	if (mod_id < 0 || mod_id >= MAX_MODS)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[SMURF_ID_ERR].TextCast, NULL, NULL, NULL, 1);

	if (mod_id >= 0)
	{
		dither_basepage = Dithermod_Basepage[mod_id];
		textseg_begin = dither_basepage->p_tbase;	/* Textsegment-Startadresse holen */

		/* Message von Smurf */
		ditherdata->message = mode;		/* 0=dithers mir, -1 = Beenden, 2=Config */

		/* Funktionen einhÑngen */
		ditherdata->services = &global_services;

		module_main = (short (*)(DITHER_DATA *)) (textseg_begin + MAIN_FUNCTION_OFFSET);
		if (mode != MQUERY)
			module_main(ditherdata);

		if (mode == MQUERY)
			return dither_basepage;
	}

	return dither_basepage;
}


/* ------------------------------------------------------------------------	*/
/*	init_module_tree														*/
/*	DurchlÑuft rekursiv einen Objektbaum (Modulresource) und initialisiert	*/
/*	die darin enthaltenen Farbicons mit init_modtree();						*/
/* ------------------------------------------------------------------------	*/
void walk_module_tree(WINDOW *wind, WORD start)
{
	WORD index;
	OBJECT *tree;

	tree = wind->resource_form;

	for (index = tree[start].ob_head; index != -1 && index != start; index = tree[index].ob_next)
	{
		if (tree[index].ob_type == G_CICON)
		{
			init_modtree(tree, index);
			convert_icon(tree, index);
		}

		if ((tree[index].ob_type >> 8) == PREVIEW_ACTION)
		{
			wind->xoffset = 0;
			wind->yoffset = 0;
			wind->pic_xpos = tree[index].ob_x;
			wind->pic_ypos = tree[index].ob_y;
			wind->clipwid = tree[index].ob_width - 1;
			wind->cliphgt = tree[index].ob_height - 1;
			if (smurf_picture[active_pic]->block == NULL)
				wind->picture = smurf_picture[active_pic];
			else
				wind->picture = smurf_picture[active_pic]->block;
		}

		walk_module_tree(wind, index);	/* rekursiv fÅr alle Kinder! */
	}
}


/*------------------------------------------------------------------------	*/
/*	init_modtree															*/
/*	DurchlÑuft die Farbtiefen des Icons index und sucht die am besten		*/
/*	zur momentanen Bildschirmauflîsung passende raus.						*/
/*	Diese wird dann direkt bei tree[index].ob_spec.iconblk->mainlist		*/
/*	eingehÑngt, damit das AES sie zuallererst findet.						*/
/*------------------------------------------------------------------------	*/
void init_modtree(OBJECT *tree, WORD index)
{
	CICON *img;
	CICON *best_img;

#if 0
	/* nur unter MagiC nîtig */
	if (!(Sys_info.OS & MATSCHIG))
		return;
#endif

	best_img = img = tree[index].ob_spec.ciconblk->mainlist;

	while (img != NULL)
	{
		if (img->num_planes <= Sys_info.bitplanes)
		{
			best_img = img;
			img = img->next_res;
		} else
			break;
	}

	/* leider nîtig da manchmal im Farbicon keine monochrome Darstellung */
	/* vorhanden ist ... */
	if (Sys_info.bitplanes == 1 && best_img->num_planes > 1)
	{
		best_img->col_data = tree[index].ob_spec.ciconblk->monoblk.ib_pdata;
		best_img->col_mask = tree[index].ob_spec.ciconblk->monoblk.ib_pmask;
		best_img->num_planes = 1;
	}
#if 0
	printf("best_img->num_planes: %d\n", best_img->num_planes);
	if (best_img->next_res == NULL)
		printf("best_img->next_res == NULL\n");
	else
		printf("best_img->next_res != NULL\n");
	getch();
#endif

	tree[index].ob_spec.ciconblk->mainlist = best_img;
}


/* convert_icon -------------------------------------------------------------
	Konnvertiert die Daten eines Farbicons vom Standard- ins gerÑteabhÑnige
	Format, weil die Icons in den Modul-RSHs nicht von rsrc_load gewandelt 
	werden kînnen. Nur unter nicht MagiC-Systemen nîtig!
	---------------------------------------------------------------------*/
void convert_icon(OBJECT *tree, WORD index)
{
	uint8_t *pixbuf;
	uint8_t *line;
	uint8_t get;
	uint8_t set;
	WORD *imgdata;
	WORD **img_data[2];
	short t;
	WORD icon_bitplanes;
	WORD icon_w, icon_w16, icon_h, x, y;
	long icon_planelength;
	MFDB icon_data;
	MFDB dest_data;
	CICONBLK *ciconblk;
	SMURF_PIC smpic;


	/* unter MagiC werden die Icons beim Zeichnen gewandelt */
	if (Sys_info.OS & MATSCHIG)
		return;

	/*----- STF->gerÑteabhÑngig konvertieren? ------*/
	icon_bitplanes = Sys_info.bitplanes;
	ciconblk = tree[index].ob_spec.ciconblk;
	img_data[0] = &ciconblk->mainlist->col_data;
	img_data[1] = &ciconblk->mainlist->sel_data;

	icon_w = ciconblk->monoblk.ib_wicon;
	icon_w16 = (icon_w + 15) / 16;
	icon_h = ciconblk->monoblk.ib_hicon;
	icon_planelength = (long) icon_w16 * 2L * (long) icon_h;

	for (t = 0; t < 2; t++)
	{
		if (*img_data[t])
		{
			if (icon_bitplanes <= 8)
			{
				/*
				 * nur aufblasen wenn wirklich nîtig
				 */
				if (icon_bitplanes > ciconblk->mainlist->num_planes)
				{
					imgdata = (WORD *) SMalloc(icon_planelength * icon_bitplanes);
					memset(imgdata, 0, icon_planelength * icon_bitplanes);
					memcpy(imgdata, *img_data[t], icon_planelength * ciconblk->mainlist->num_planes);
					SMfree(*img_data[t]);
					*img_data[t] = imgdata;

					pixbuf = SMalloc(icon_w + 15);

					get = (1 << ciconblk->mainlist->num_planes) - 1;
					set = (1 << icon_bitplanes) - 1;

					y = 0;
					do
					{
						memset(pixbuf, 0, icon_w);
						getpix_std_line((uint8_t *) imgdata, pixbuf, icon_bitplanes, icon_planelength, icon_w);
						line = pixbuf;

						x = 0;
						do
						{
							/* Schwarz auf Schwarz setzen */
							if (*line == get)
								*line = set;
							line++;
						} while (++x < icon_w);

						imgdata = (WORD *) ((char *) imgdata + setpix_std_line(pixbuf, (uint8_t *) imgdata, icon_bitplanes, icon_planelength, icon_w));
					} while (++y < icon_h);

					SMfree(pixbuf);
				}

				icon_data.fd_addr = *img_data[t];
				icon_data.fd_stand = 1;
				icon_data.fd_w = icon_w;
				icon_data.fd_h = icon_h;
				icon_data.fd_wdwidth = icon_w16;
				icon_data.fd_nplanes = icon_bitplanes;

				dest_data.fd_addr = *img_data[t];
				dest_data.fd_stand = 0;
				dest_data.fd_w = icon_w;
				dest_data.fd_h = icon_h;
				dest_data.fd_wdwidth = icon_w16;
				dest_data.fd_nplanes = icon_bitplanes;

				vr_trnfm(Sys_info.vdi_handle, &icon_data, &dest_data);
			} else
			{
				imgdata = (WORD *) SMalloc(icon_planelength * icon_bitplanes);

				smpic.pic_data = (char *) *img_data[t];
				smpic.pic_width = icon_w;
				smpic.pic_height = icon_h;
				smpic.depth = ciconblk->mainlist->num_planes;
				smpic.format_type = FORM_STANDARD;
				smpic.palette = stdpal4bit;
				smpic.zoom = 0;

				bplanes = icon_bitplanes;
				direct2screen(&smpic, (char *) imgdata, NULL);
				bplanes = Sys_info.bitplanes;
				Dialog.busy.ok();

				SMfree(*img_data[t]);
				*img_data[t] = imgdata;
			}
		}
	}

	ciconblk->mainlist->num_planes = icon_bitplanes;
}


/*------------------------------------------------------------------------	*/
/*							get_pic											*/
/*	Dienstfunktion, die es einem Edit-Modul ermîglicht, seine Bilder (wenn	*/
/*	mehrere) ohne den Umweg Åber die Message zu holen.						*/
/*------------------------------------------------------------------------	*/
SMURF_PIC *get_pic(WORD num, short mod_id, MOD_INFO *mod_info, WORD depth, short form, short col)
{
	char alertstr[128];
	WORD piccol;
	short back;
	SMURF_PIC *current_pic;
	MOD_ABILITY new_mod;

	current_pic = smurf_picture[module_pics[mod_id][num]];

	if (current_pic == NULL)
	{
		if (num == 0)
			strcpy(alertstr, mod_info->pic_descr1);
		else if (num == 1)
			strcpy(alertstr, mod_info->pic_descr2);
		else if (num == 2)
			strcpy(alertstr, mod_info->pic_descr3);
		else if (num == 3)
			strcpy(alertstr, mod_info->pic_descr4);
		else if (num == 4)
			strcpy(alertstr, mod_info->pic_descr5);
		else if (num == 5)
			strcpy(alertstr, mod_info->pic_descr6);

		strcat(alertstr, " wird noch benîtigt!"); /* FIXME: translate */
		Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);

		return NULL;
	} else
	{
		memset(&new_mod, 0x0, sizeof(MOD_ABILITY));
		new_mod.depth1 = depth;
		new_mod.form1 = form;
		piccol = col;

		back = f_convert(current_pic, &new_mod, piccol, SAME, 0);
		if (back != 0)
			return NULL;
	}

	return current_pic;
}


/* ------------------------------------------------------------------------	*/
/* 						Bilder ans Modul Åbergeben 							*/
/*	FÅr Module, die mehrere Bilder benîtigen. Diese werden mit der			*/
/*	Message MPICTURE an das Modul Åbergeben. Das Modul muû mit M_WAITING	*/
/*	bestÑtigen. In event_par[0] wird die lfd. Nummer des Åbergebenen Bildes	*/
/*	abgelegt, beginnend mit 0.												*/
/* ------------------------------------------------------------------------	*/
short f_give_pics(MOD_INFO *mod_info, MOD_ABILITY *mod_abs, short module_number)
{
	char alertstr[257];
	short t;
	short back;
	WORD piccol;
	MOD_ABILITY new_mod;
	SMURF_PIC *current_pic;

	(void) mod_abs;
	for (t = 0; t < mod_info->how_many_pix; t++)
	{
		current_pic = smurf_picture[module_pics[module_number][t]];

		if (current_pic != NULL && current_pic->block != NULL)
			current_pic = current_pic->block;

		module.smStruct[module_number]->event_par[0] = t;
		module.comm.start_edit_module(edit_modules[module_number], module.bp[module_number], MPICS, module_number, module.smStruct[module_number]);

		if (module.smStruct[module_number]->module_mode == M_PICTURE)
		{
			if (current_pic == NULL)
			{
				if (t == 0)
					strcpy(alertstr, mod_info->pic_descr1);
				else if (t == 1)
					strcpy(alertstr, mod_info->pic_descr2);
				else if (t == 2)
					strcpy(alertstr, mod_info->pic_descr3);
				else if (t == 3)
					strcpy(alertstr, mod_info->pic_descr4);
				else if (t == 4)
					strcpy(alertstr, mod_info->pic_descr5);
				else if (t == 5)
					strcpy(alertstr, mod_info->pic_descr6);

				/* FIXME: translate */
				strcat(alertstr, " muû noch zugewiesen werden! Ziehen Sie das gewÅnschte Bild aus dem Bildmanager auf das Modul");
				Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);

				return -1;
			} else
			{
				memset(&new_mod, 0x0, sizeof(MOD_ABILITY));
				new_mod.depth1 = module.smStruct[module_number]->event_par[0];
				new_mod.form1 = module.smStruct[module_number]->event_par[1];
				piccol = module.smStruct[module_number]->event_par[2];
				back = f_convert(current_pic, &new_mod, piccol, SAME, 0);
				if (back != 0)
					return back;
				module.smStruct[module_number]->smurf_pic = current_pic;
				module.smStruct[module_number]->event_par[0] = t;
				module.comm.start_edit_module(edit_modules[module_number], module.bp[module_number], MPICTURE, module_number, module.smStruct[module_number]);
			}
		}
	}

	return 0;
}


/* inform_modules -------------------------------------------------------------
	Informiert alle laufenden Editmodule und Plugins Åber Bildaktivierungen und
	Ñhnliches, das durch message spezifiziert wird.
	Das Bild *picture wird in der Kommunikationsstruktur des Moduls 
	mitgeschickt.
	----------------------------------------------------------------------*/
short inform_modules(short message, SMURF_PIC *picture)
{
	short t;
	long mod_magic;
	BASPAG *curr_baspag;

	/*
	 * alle Editmodule informieren
	 */
	for (t = 0; t < MAX_MODS - 1; t++)
	{
		curr_baspag = module.bp[t];
		if (curr_baspag)
		{
			mod_magic = get_modmagic(curr_baspag);
			if (mod_magic == MOD_MAGIC_EDIT)
			{
				module.smStruct[t]->smurf_pic = picture;
				module.comm.start_edit_module("", curr_baspag, message, 0, module.smStruct[t]);
			}
		}
	}

	/*
	 * alle residenten oder nichtresidenten laufenden Plugins informieren
	 */
	for (t = 0; t < anzahl_plugins; t++)
	{
		curr_baspag = plugin_bp[t];
		if (curr_baspag)
		{
			mod_magic = get_modmagic(curr_baspag);
			if (mod_magic == MOD_MAGIC_PLUGIN)
			{
				plg_data[t]->event_par[0] = active_pic;
				start_plugin(curr_baspag, message, 0, plg_data[t]);
			}
		}
	}

	return 0;
}


/* get_proclen -------------------------------------------------
	Ermittelt die GesamtlÑnge des Prozesses mit der Basepage baspag.
	----------------------------------------------------------------*/
long get_proclen(BASPAG *baspag)
{
	long TextLen;
	long BSSLen;
	long DataLen;
	long ProcLen;

	TextLen = baspag->p_tlen;
	BSSLen = baspag->p_blen;
	DataLen = baspag->p_dlen;
	/* BASEPAGE + Textsegment + Datensegment + BSS + Stack */
	ProcLen = sizeof(BASPAG) + TextLen + DataLen + BSSLen + 1024L;

	return ProcLen;
}


/* get_modmagic -------------------------------------------------
	Ermittelt das Magic eines Moduls (4 Bytes) und gibt dieses zurÅck.
	----------------------------------------------------------------*/
long get_modmagic(BASPAG *basepage)
{
	char *textseg_begin;

	if (basepage == NULL)
		return 0;

	textseg_begin = basepage->p_tbase;
	return (*((long *) (textseg_begin + MAGIC_OFFSET)));
}


/* AESmsg_to_module -------------------------------------------------
	Wird aus dem Messagehandler aufgerufen, um AES-Messages an Module
	weiterzuleiten. Das passiert dann, wenn Smurf eine Message nicht
	kennt oder wenn eine msg direkt an ein Modulfenster gerichtet ist.
	Kennung fÅr eine AES-msg ist 255 in GARGAMEL->module_mode, der
	messagebuffer wird komplett in event_par[0-15] kopiert.
	----------------------------------------------------------------*/
void AESmsg_to_modules(WORD *msgbuf)
{
	short t;
	long magic;
	BASPAG *curr_bp;

	/*
	 * Editmodule und Exporter informieren
	 */
	for (t = 0; t < MAX_MODS - 1; t++)
	{
		curr_bp = module.bp[t];

		if (curr_bp != 0 && module.smStruct[t] != 0)
		{
			magic = get_modmagic(module.bp[t]);

			memcpy(module.smStruct[t]->event_par, msgbuf, 16);

			if (magic == MOD_MAGIC_EDIT)
				module.comm.start_edit_module(NULL, curr_bp, SMURF_AES_MESSAGE, 0, module.smStruct[t]);
			else if (magic == MOD_MAGIC_EXPORT)
				start_exp_module(NULL, SMURF_AES_MESSAGE, NULL, curr_bp, module.smStruct[t], 0);

			f_handle_modmessage(module.smStruct[t]);
		}
	}

	/*
	 * und jetzt die laufenden Plugins - nichtresidente nichtlaufende werden nicht benachrichtigt!
	 */
	for (t = 0; t < 11; t++)
	{
		curr_bp = plugin_bp[t];

		if (curr_bp != 0)
		{
			magic = get_modmagic(plugin_bp[t]);

			if (magic == MOD_MAGIC_PLUGIN)
			{
				memcpy(plg_data[t]->event_par, msgbuf, 16);
				start_plugin(curr_bp, SMURF_AES_MESSAGE, 0, plg_data[t]);
			}
		}
	}
}


/* ------------------------------------------------------------------------	*/
/*								Preview erzeugen							*/
/* ------------------------------------------------------------------------	*/
void make_modpreview(WINDOW *wind)
{
	char *textbeg;
	short mod_num;
	short t;
	WORD w, h;
	short picnum;
	short mod_index;
	short piccol;
	long PicLen;
	long Awidth;
	SMURF_PIC *source_pic;
	SMURF_PIC *add_pix[7];
	MOD_ABILITY *mod_abs;
	MOD_ABILITY new_mod;
	MOD_INFO *mod_inf;
	DISPLAY_MODES thisDisplay;

	mod_num = wind->module;
	mod_index = mod_num & 0xFF;

	textbeg = module.bp[mod_index]->p_tbase;
	mod_abs = *((MOD_ABILITY **) (textbeg + MOD_ABS_OFFSET));
	mod_inf = *((MOD_INFO **) (textbeg + MOD_INFO_OFFSET));

#if 0
	for (t = 0; t < mod_inf->how_many_pix; t++)
	{
		SMfree(add_pix[t]);
		picnum = module_pics[mod_index][t];

		if (mod_inf->how_many_pix > 1 && picture_windows[picnum].whandlem == -1)
		{
			/* FIXME: translate */
			strcpy(alertstr, "Das Modul braucht ");
			strncat(alertstr, itoa(mod_inf->how_many_pix, helpstr, 10), 4);
			strcat(alertstr, " Bilder!");
			Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
			return;
		}
	}
#endif


	/* ------------- Preview-Pic-Struktur vorbereiten ----------- */
	source_pic = smurf_picture[active_pic];
	if (source_pic->block != NULL)
		source_pic = source_pic->block;

#if 0
	/* ---alte Bildschirmdarstellung freigeben--- */
	if (wind->picture != NULL && wind->picture->changed == 0x80)
	{
		SMfree(wind->picture->screen_pic->fd_addr);
		free(wind->picture->screen_pic);
		free(wind->picture->palette);
		SMfree(wind->picture);
	}
#endif

	/*
	 * Speicher fÅr berechnetes Preview anfordern
	 */
	wind->picture = SMalloc(sizeof(SMURF_PIC));
	memcpy(wind->picture, source_pic, sizeof(SMURF_PIC));
	wind->picture->palette = malloc(1025);
	memcpy(wind->picture->palette, source_pic->palette, 1025);
	wind->picture->pic_width = wind->clipwid;
	wind->picture->pic_height = wind->cliphgt;
	Awidth = ((((long) wind->picture->pic_width + 7) / 8) << 3);
	PicLen = (Awidth * (long) wind->picture->pic_height * (long) wind->picture->depth) / 8L;
	wind->picture->pic_data = SMalloc(PicLen);
	wind->picture->local_nct = NULL;

	Dialog.busy.disable();

	/*----------- Bildausschnitt kopieren -------------------------------*/
	copy_preview(source_pic, wind->picture, wind);
	f_convert(wind->picture, mod_abs, RGB, SAME, 0);

	if (mod_inf->how_many_pix > 1)
	{
		for (t = 0; t < mod_inf->how_many_pix; t++)
		{
			module.smStruct[mod_index]->event_par[0] = t;
			module.comm.start_edit_module(edit_modules[mod_index], module.bp[mod_index], MPICS, module.smStruct[mod_index]->module_number, module.smStruct[mod_index]);

			if (module.smStruct[mod_index]->module_mode == M_PICTURE)
			{
				picnum = module_pics[mod_index][t];
				if (picture_windows[picnum].whandlem == -1)
				{
					/* FIXME: translate */
					Dialog.winAlert.openAlert("Fehler: zu verwendendes Bild nicht zugewiesen!", NULL, NULL, NULL, 1);
					return;
				}

				source_pic = picture_windows[picnum].picture;
				add_pix[t] = SMalloc(sizeof(SMURF_PIC));
				memcpy(add_pix[t], source_pic, sizeof(SMURF_PIC));
				add_pix[t]->palette = malloc(1025);
				memcpy(add_pix[t]->palette, source_pic->palette, 1025);
				add_pix[t]->pic_width = wind->clipwid;
				add_pix[t]->pic_height = wind->cliphgt;
				Awidth = ((((long) add_pix[t]->pic_width + 7) / 8) << 3);
				PicLen = (Awidth * (long) add_pix[t]->pic_height * (long) add_pix[t]->depth) / 8L;
				add_pix[t]->pic_data = SMalloc(PicLen);

				copy_preview(source_pic, add_pix[t], wind);

				memset(&new_mod, 0x0, sizeof(MOD_ABILITY));
				new_mod.depth1 = module.smStruct[mod_index]->event_par[0];
				new_mod.form1 = module.smStruct[mod_index]->event_par[1];
				piccol = module.smStruct[mod_index]->event_par[2];
				f_convert(add_pix[t], &new_mod, piccol, SAME, 0);

				module.smStruct[mod_index]->event_par[0] = t;
				module.smStruct[mod_index]->smurf_pic = add_pix[t];
				module.comm.start_edit_module(edit_modules[mod_index], module.bp[mod_index], MPICTURE, module.smStruct[mod_index]->module_number, module.smStruct[mod_index]);
				if (module.smStruct[mod_index]->module_mode != M_WAITING)
					break;
			}
		}
	}


	Dialog.busy.reset(0, "Preview...");
	graf_mouse(BUSYBEE, dummy_ptr);

	module.smStruct[mod_index]->event_par[0] = position_markers[mod_index].xpos[0] - wind->xoffset;
	module.smStruct[mod_index]->event_par[1] = position_markers[mod_index].ypos[0] - wind->yoffset;
	module.comm.start_edit_module("", module.bp[mod_index], MCH_COORDS, mod_num, module.smStruct[mod_index]);
	module.smStruct[mod_index]->smurf_pic = wind->picture;
	module.comm.start_edit_module(edit_modules[mod_index], module.bp[mod_index], MEXEC, module.smStruct[mod_index]->module_number, module.smStruct[mod_index]);

	/*-------- verÑndertes Bild kopieren -----*/
	if (mod_inf->how_many_pix > 1)
	{
		picnum = module.smStruct[mod_index]->event_par[0];
		w = add_pix[picnum]->pic_width;
		h = add_pix[picnum]->pic_height;
		PicLen = (w * (long) h * (long) add_pix[picnum]->depth) / 8L;

		SMfree(wind->picture->pic_data);
		wind->picture->pic_data = SMalloc(PicLen);
		memcpy(wind->picture->pic_data, add_pix[picnum]->pic_data, PicLen);

		wind->picture->pic_width = add_pix[picnum]->pic_width;
		wind->picture->pic_height = add_pix[picnum]->pic_height;
		wind->picture->depth = add_pix[picnum]->depth;
		wind->picture->format_type = add_pix[picnum]->format_type;
		wind->picture->col_format = add_pix[picnum]->col_format;
		memcpy(wind->picture->palette, add_pix[picnum]->palette, 1025);
	}

	wind->picture->zoom = 0;

	/* -------- Display-Options temporÑr umbauen --------- */
	/* (Preview wird nur nach SysPal gedithert) */
	thisDisplay.dither_24 = Sys_info.PreviewDither;
	thisDisplay.dither_8 = Sys_info.PreviewDither;
	thisDisplay.dither_4 = Sys_info.PreviewDither;
	thisDisplay.syspal_24 = CR_SYSPAL;
	thisDisplay.syspal_8 = CR_SYSPAL;
	thisDisplay.syspal_4 = CR_SYSPAL;

	f_dither(wind->picture, &Sys_info, 0, NULL, &thisDisplay);

	Dialog.busy.enable();

	graf_mouse(ARROW, dummy_ptr);

	wind->picture->changed = 0x80;		/* damit's vor dem nÑxten Bewegen wieder freigegeben wird */

	if (mod_inf->how_many_pix == 1)
	{
		SMfree(wind->picture->pic_data);
		free(wind->picture->palette);
	} else
		for (t = 0; t < mod_inf->how_many_pix; t++)
		{
			if (add_pix[t])
			{
				SMfree(add_pix[t]->pic_data);
				free(add_pix[t]->palette);
				SMfree(add_pix[t]);
			}
		}

	Dialog.busy.ok();
	Dialog.busy.dispRAM();
}
