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
#include "plugin.h"
#include "smplugin.h"

#include "smurfobs.h"
#include "ext_obs.h"

#define	ABS(i)	(i>0? i : -i)

/*--------------	Slidergrenzen f�r Modulformular */
long smax[4];				/* Maxima */
long smin[4];				/* Minima */
short sval[4];					/* eingestellte Sliderwerte */
short edit_mod_num = -1;					/* Modul-ID des Moduls, das das Einstellformular benutzt */

int prev_zoom = 1;
SMURF_PIC move_prev;
static SMURF_PIC *module_preview = NULL;
static SMURF_PIC *oldpic;
static WORD oldxoff, oldyoff;

static WORD const check[4] = { CHECK1, CHECK2, CHECK3, CHECK4 };
static WORD const ed[4] = { ED1, ED2, ED3, ED4 };
static WORD const pdslider[4] = { PDSLIDER1, PDSLIDER2, PDSLIDER3, PDSLIDER4 };
static WORD const s_tit[4] = { S1_TIT, S2_TIT, S3_TIT, S4_TIT };
static WORD const ed_tit[4] = { ED1_TIT, ED2_TIT, ED3_TIT, ED4_TIT };
static WORD const check_text[4] = { CHECK1_TEXT, CHECK2_TEXT, CHECK3_TEXT, CHECK4_TEXT };
static WORD const s_f[4] = { S1_F, S2_F, S3_F, S4_F };
static WORD const s_a[4] = { S1_A, S2_A, S3_A, S4_A };
static WORD const s_r[4] = { S1_R, S2_R, S3_R, S4_R };

	
/* ------------------------------------------------------------------------	*/
/*	En-/Disablen der Preview-Zoom-Buttons nach Pr�fung der Gr��en			*/
/* ------------------------------------------------------------------------	*/
static void check_prevzoom(void)
{
	OBJECT *modtree;
	SMURF_PIC *pic;

	pic = smurf_picture[active_pic];
	if (pic->block != NULL)
		pic = pic->block;

	modtree = wind_s[WIND_MODFORM].resource_form;

	if ((modtree[PREV_BOX].ob_width) * (prev_zoom + 1) >= pic->pic_width &&
		(modtree[PREV_BOX].ob_height) * (prev_zoom + 1) >= pic->pic_height)
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_DEC, OS_DISABLED, 1);
	else
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_DEC, OS_ENABLED, 1);

	if (prev_zoom <= 1)
	{
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_INC, OS_DISABLED, 1);
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_RESET, OS_DISABLED, 1);
	} else
	{
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_INC, OS_ENABLED, 1);
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_RESET, OS_ENABLED, 1);
	}
}


static void applyConfig(long *loadcnf)
{
	OBJECT *modtree;
	short t;
	
	modtree = wind_s[WIND_MODFORM].resource_form;

	for (t = 0; t < 4; t++)
	{
		sval[t] = (WORD) loadcnf[t];
		modtree[check[t]].ob_state = (WORD) loadcnf[t + 4];
		ltoa(loadcnf[t + 8], modtree[ed[t]].TextCast, 10);
	}
}


/*------------------- Modul-Einstell-Formular �ffnen ------------------	*/
/* Vom Modul aufgerufen: mod_id (GARGAMEL->module_number) ist			*/
/* Die Nummer des Moduls, das gerade das Einstellformular benutzt.		*/
/*---------------------------------------------------------------------	*/
void f_module_prefs(MOD_INFO *infostruct, short mod_id)
{
	char edd[6];
	char cmp_modname[30];
	WORD sd[4];
	BOOLEAN redraw_me = FALSE;
	WORD back;
	short t, o;
	WORD endwidth, endheight;
	WORD zoom;
	WORD index;
	long editval[4];
	long *cnfblock;
	OBJECT *pref_form;

	/* evtl. altes Modul terminieren */
	if (edit_mod_num != -1)
	{
		module.comm.start_edit_module("", module.bp[edit_mod_num], MTERM, edit_mod_num, module.smStruct[edit_mod_num]);
		check_and_terminate(module.smStruct[edit_mod_num]->module_mode, edit_mod_num);
		for (t = 0; t < 7; t++)
			module_pics[edit_mod_num][t] = 0;

		edit_mod_num = -1;
		Dialog.busy.dispRAM();
		redraw_me = TRUE;
	}

	if (mod_id < 0 || mod_id > 20)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MODINIT_ID_ERR].TextCast, NULL, NULL, NULL, 1);


	/* altes Preview freigeben */
	if (module_preview != NULL)
	{
		if (module_preview->screen_pic != NULL)
			SMfree(module_preview->screen_pic->fd_addr);

		free(module_preview->screen_pic);
		SMfree(module_preview);
	}

	module_preview = NULL;


	/* neues Modul holen */
	edit_mod_num = mod_id;
	pref_form = wind_s[WIND_MODFORM].resource_form;
	strncpy(wind_s[WIND_MODFORM].wtitle, infostruct->mod_name, 40);
	if (wind_s[WIND_MODFORM].whandlem > 0)
		Window.windSet(wind_s[WIND_MODFORM].whandlem, WF_NAME, LONG2_2INT(wind_s[WIND_MODFORM].wtitle), 0, 0);

	for (t = 0; t < 4; t++)
	{
		/*
		 * unbenutzte Elemente verstecken
		 */
		if (infostruct->slide[t] == NULL || infostruct->slide[t][0] == '\0')
		{
			pref_form[s_f[t]].ob_flags |= OF_HIDETREE;
			pref_form[s_a[t]].ob_flags |= OF_HIDETREE;
			pref_form[s_r[t]].ob_flags |= OF_HIDETREE;
		} else
		{
			pref_form[s_f[t]].ob_flags &= ~OF_HIDETREE;
			pref_form[s_a[t]].ob_flags &= ~OF_HIDETREE;
			pref_form[s_r[t]].ob_flags &= ~OF_HIDETREE;
		}
		
		if (infostruct->check[t] == NULL || infostruct->check[t][0] == '\0')
			pref_form[check[t]].ob_flags |= OF_HIDETREE;
		else
			pref_form[check[t]].ob_flags &= ~OF_HIDETREE;
		
		if (infostruct->edit[t] == NULL || infostruct->edit[t][0] == '\0')
			pref_form[ed[t]].ob_flags |= OF_HIDETREE;
		else
			pref_form[ed[t]].ob_flags &= ~OF_HIDETREE;
		
		if (infostruct->how_many_pix > 1)
			pref_form[PICS_INFO].ob_flags &= ~OF_HIDETREE;
		else
			pref_form[PICS_INFO].ob_flags |= OF_HIDETREE;
		
		/*----------------------------- neue Formulartexte eintragen */
		strcpy(pref_form[s_tit[t]].TextCast, infostruct->slide[t] ? infostruct->slide[t] : "");
		strcpy(pref_form[ed_tit[t]].TextCast, infostruct->edit[t] ? infostruct->edit[t] : "");
		strcpy(pref_form[check_text[t]].TextCast, infostruct->check[t] ? infostruct->check[t] : "");
		
		/*------------------------------ Slider-Mins / Maxs auslesen */
		sliders[pdslider[t]].max_val = (WORD) infostruct->srange[t].max;
		sliders[pdslider[t]].min_val = (WORD) infostruct->srange[t].min;
		
		/*------------------------------- Slider-Defaults auslesen */
		sval[t] = sd[t] = (WORD) infostruct->sdef[t];
		
		/*--------------------------------- Edit-Defaults setzen */
		editval[t] = infostruct->edef[t];
		if (editval[t] < infostruct->erange[t].min)
			editval[t] = infostruct->erange[t].min;
		else if (editval[t] > infostruct->erange[t].max)
			editval[t] = infostruct->erange[t].max;
		
		ltoa(editval[t], edd, 10);
		strncpy(pref_form[ed[t]].TextCast, edd, 5);
		
		/*---------------------------- Check-Defaults setzen */
		/* Radiobutton oder Checkbox? */
		pref_form[check[t]].ob_type &= ~0xFF00;
		
		if (infostruct->cdef[t] >= 2)
		{
			pref_form[check[t]].ob_type &= ~(CHECKBOX << 8);
			pref_form[check[t]].ob_type |= RADIOBUTTON << 8;
			pref_form[check[t]].ob_flags |= OF_RBUTTON;
		} else
		{
			pref_form[check[t]].ob_type &= ~(RADIOBUTTON << 8);
			pref_form[check[t]].ob_type |= CHECKBOX << 8;
			pref_form[check[t]].ob_flags &= ~OF_RBUTTON;
		}
	}
		
	/* 
	 * Mu� der Cursor komplett ausgeschaltet werden (kein Editobjekt da)?
	 */
	if ((pref_form[ED1].ob_flags & OF_HIDETREE) &&
		(pref_form[ED2].ob_flags & OF_HIDETREE) &&
		(pref_form[ED3].ob_flags & OF_HIDETREE) &&
		(pref_form[ED4].ob_flags & OF_HIDETREE))
		wind_s[WIND_MODFORM].editob = 0;
	else
		wind_s[WIND_MODFORM].editob = ED1;

	/*
	 * und jetzt alle abgeschalteten Objekte disablen 
	 */
	for (o = pref_form[0].ob_head; o <= pref_form[0].ob_tail; o++)
	{
		if (pref_form[o].ob_flags & OF_HIDETREE)
			pref_form[o].ob_state |= OS_DISABLED;
		else
			pref_form[o].ob_state &= ~OS_DISABLED;
	}
		
	f_treewalk(pref_form);

	for (t = 0; t < 4; t++)
	{
		if (infostruct->cdef[t] != 0 && infostruct->cdef[t] != 3)
			pref_form[check[t]].ob_state |= OS_SELECTED;
		else
			pref_form[check[t]].ob_state &= ~OS_SELECTED;
	}


	/*
	 * evtl. gemerkte Konfiguration wieder in den Dialog �bertragen
	 */
	strcpy(cmp_modname, infostruct->mod_name);
	if (strlen(cmp_modname) < 28)
		strncat(cmp_modname, "                           ", 28 - strlen(cmp_modname));
	for (index = 0; index < Dialog.emodList.anzahl; index++)
	{
		if (strcmp(Dialog.emodList.modNames[index], cmp_modname) == 0)
			break;
	}

	if (edit_cnfblock[index] != NULL)
	{
		cnfblock = (long *) edit_cnfblock[index];
		applyConfig(cnfblock);
		sd[0] = (WORD) cnfblock[0];
		sd[1] = (WORD) cnfblock[1];
		sd[2] = (WORD) cnfblock[2];
		sd[3] = (WORD) cnfblock[3];
	}

	/*
	 * Windowstruktur fertigmachen
	 */
	wind_s[WIND_MODFORM].module = edit_mod_num;

	wind_s[WIND_MODFORM].pic_xpos = pref_form[PREV_OUTER].ob_x + pref_form[PREV_BOX].ob_x;
	wind_s[WIND_MODFORM].pic_ypos = pref_form[PREV_OUTER].ob_y + pref_form[PREV_BOX].ob_y;
	wind_s[WIND_MODFORM].clipwid = pref_form[PREV_BOX].ob_width;
	wind_s[WIND_MODFORM].cliphgt = pref_form[PREV_BOX].ob_height;

	zoom = prev_zoom = 1;
	if (smurf_picture[active_pic]->block == NULL)
	{
		endwidth = smurf_picture[active_pic]->pic_width / zoom;
		endheight = smurf_picture[active_pic]->pic_height / zoom;
	} else
	{
		endwidth = smurf_picture[active_pic]->block->pic_width / zoom;
		endheight = smurf_picture[active_pic]->block->pic_height / zoom;
	}

	if (pref_form[PREV_BOX].ob_width > endwidth)
		wind_s[WIND_MODFORM].clipwid = endwidth;
	if (pref_form[PREV_BOX].ob_height > endheight)
		wind_s[WIND_MODFORM].cliphgt = endheight;

	/*
	 * Move-Preview - Struktur vorbereiten
	 */
	if (smurf_picture[active_pic]->block == NULL)
	{
		memcpy(&move_prev, smurf_picture[active_pic], sizeof(SMURF_PIC));
		move_prev.local_nct = NULL;
	} else
	{
		memcpy(&move_prev, smurf_picture[active_pic]->block, sizeof(SMURF_PIC));
		move_prev.local_nct = NULL;
	}

	/*
	 * Echtzeitdithering erzwingen und move_prev ins Fenster h�ngen
	 */
	move_prev.screen_pic = NULL;
	move_prev.zoom = 0;
	wind_s[WIND_MODFORM].picture = &move_prev;

	check_prevzoom();					/* Zoombuttons f�rs Preview ein/ausschalten */

	if (wind_s[WIND_MODFORM].whandlem <= 0)	/* Fenster neu? -> kein weiterer Redraw n�tig. */
		redraw_me = FALSE;

	back = Window.open(&wind_s[WIND_MODFORM]);

	if (back != -1)
	{
		if (redraw_me)
			Window.redraw(&wind_s[WIND_MODFORM], NULL, 0, 0);

		/* Slider drauf setzen */
		for (t = 0; t < 4; t++)
			setslider(&sliders[pdslider[t]], sd[t]);
	}

	module.smStruct[edit_mod_num]->wind_struct = &wind_s[WIND_MODFORM];

	Dialog.busy.ok();
}


/* ------------------------------------------------------------------------	*/
/*				PD-Parameter in GARGAMEL-Struktur einf�gen					*/
/* ------------------------------------------------------------------------	*/
static void f_insert_prefs(GARGAMEL *smurf_st, SMURF_PIC *picture)
{
	OBJECT *modtree;
	short t;

	modtree = wind_s[WIND_MODFORM].resource_form;

	smurf_st->smurf_pic = picture;
	smurf_st->picwind_x = picture_windows[active_pic].wx;
	smurf_st->picwind_y = picture_windows[active_pic].wy;
	smurf_st->picwind_w = picture_windows[active_pic].ww;
	smurf_st->picwind_h = picture_windows[active_pic].wh;
	for (t = 0; t < 4; t++)
	{
		smurf_st->slide[t] = sval[t];
		smurf_st->edit[t] = atol(modtree[ed[t]].TextCast);
		smurf_st->check[t] = modtree[check[t]].ob_state & OS_SELECTED;
	}
}



/* ------------------------------------------------------------------------	*/
/*								Preview erzeugen							*/
/* ------------------------------------------------------------------------	*/
static void f_make_preview(short redraw_flag)
{
	SMURF_PIC *source_pic;
	SMURF_PIC *add_pix[7];
	GARGAMEL smurf_st;
	char alertstr[70];
	char helpstr[3];
	long PicLen;
	long Awidth;
	short t;
	WORD w, h;
	short picnum;
	short piccol;
	const MOD_ABILITY *mod_abs;
	MOD_ABILITY new_mod;
	const MOD_INFO *mod_inf;
	const MODULE_START *module_start;
	DISPLAY_MODES thisDisplay;

	module_start = get_module_start(module.bp[edit_mod_num]);
	mod_abs = module_start->ability;
	mod_inf = module_start->info;


	/*
	 * und jetzt pr�fen, ob alle vom Modul ben�tigten Bilder 
	 * vorhanden sind.
	 */
	for (t = 0; t < mod_inf->how_many_pix; t++)
	{
		picnum = module_pics[edit_mod_num][t];

		if (mod_inf->how_many_pix > 1 && picture_windows[picnum].whandlem == -1)
		{
			strcpy(alertstr, "Modul braucht ");
			strncat(alertstr, itoa(mod_inf->how_many_pix, helpstr, 10), 4);
			strcat(alertstr, " Bilder!");
			Dialog.winAlert.openAlert(alertstr, NULL, NULL, NULL, 1);
			return;
		}
	}


	/* ------------- Preview-Pic-Struktur vorbereiten ----------- */
	source_pic = smurf_picture[active_pic];
	if (source_pic->block != NULL)
		source_pic = source_pic->block;

	/* ---alte Bildschirmdarstellung freigeben--- */
	if (module_preview != NULL)
	{
		SMfree(module_preview->screen_pic->fd_addr);
		free(module_preview->screen_pic);
		SMfree(module_preview);
	}

	if ((module_preview = alloc_smurfpic(source_pic, TRUE)) == NULL)
		return;
	module_preview->pic_width = wind_s[WIND_MODFORM].clipwid;
	module_preview->pic_height = wind_s[WIND_MODFORM].cliphgt;
	Awidth = ((((long) module_preview->pic_width + 7) / 8) << 3);
	PicLen = (Awidth * (long) module_preview->pic_height * (long) module_preview->depth) / 8L;
	module_preview->pic_data = SMalloc(PicLen);
	module_preview->local_nct = NULL;

	Dialog.busy.disable();

	/*----------- Bildausschnitt kopieren -------------------------------*/
	copy_preview(source_pic, module_preview, &wind_s[WIND_MODFORM]);
	f_convert(module_preview, mod_abs, RGB, SAME, 0);

	/*
	 * jetzt die Bilder ans Modul �bergeben
	 */
	if (mod_inf->how_many_pix > 1)
	{
		for (t = 0; t < mod_inf->how_many_pix; t++)
		{
			module.smStruct[edit_mod_num]->event_par[0] = t;
			module.comm.start_edit_module(edit_modules[edit_mod_num], module.bp[edit_mod_num], MPICS, module.smStruct[edit_mod_num]->module_number, module.smStruct[edit_mod_num]);

			if (module.smStruct[edit_mod_num]->module_mode == M_PICTURE)
			{
				picnum = module_pics[edit_mod_num][t];
				if (picture_windows[picnum].whandlem == -1)
				{
					/* FIXME: translate */
					Dialog.winAlert.openAlert("Fehler: zu verwendendes Bild existiert nicht! Weisen Sie die Bilder durch Drag&Drop aus dem Bildmanager neu zu.", NULL, NULL, NULL, 1);
					continue;
				}

				source_pic = picture_windows[picnum].picture;
				add_pix[t] = alloc_smurfpic(source_pic, TRUE);
				add_pix[t]->pic_width = wind_s[WIND_MODFORM].clipwid;
				add_pix[t]->pic_height = wind_s[WIND_MODFORM].cliphgt;
				Awidth = ((((long) module_preview->pic_width + 7) / 8) << 3);
				PicLen = (Awidth * (long) add_pix[t]->pic_height * (long) add_pix[t]->depth) / 8L;
				add_pix[t]->pic_data = SMalloc(PicLen);

				copy_preview(source_pic, add_pix[t], &wind_s[WIND_MODFORM]);

				memset(&new_mod, 0, sizeof(MOD_ABILITY));
				new_mod.depth1 = module.smStruct[edit_mod_num]->event_par[0];
				new_mod.form1 = module.smStruct[edit_mod_num]->event_par[1];
				piccol = module.smStruct[edit_mod_num]->event_par[2];
				f_convert(add_pix[t], &new_mod, piccol, SAME, 0);

				smurf_st.event_par[0] = t;
				smurf_st.smurf_pic = add_pix[t];
				module.comm.start_edit_module(edit_modules[edit_mod_num], module.bp[edit_mod_num], MPICTURE, module.smStruct[edit_mod_num]->module_number, &smurf_st);
				if (smurf_st.module_mode != M_WAITING)
					break;
			}
		}
	}


	f_insert_prefs(&smurf_st, module_preview);

	Dialog.busy.reset(0, "Preview...");
	graf_mouse(BUSYBEE, NULL);

	module.comm.start_edit_module(edit_modules[edit_mod_num], module.bp[edit_mod_num], MEXEC, module.smStruct[edit_mod_num]->module_number, &smurf_st);

	/*-------- ver�ndertes Bild kopieren -----*/
	if (mod_inf->how_many_pix > 1)
	{
		picnum = smurf_st.event_par[0];
		w = add_pix[picnum]->pic_width;
		h = add_pix[picnum]->pic_height;
		PicLen = (w * (long) h * (long) add_pix[picnum]->depth) / 8L;

		SMfree(module_preview->pic_data);
		module_preview->pic_data = SMalloc(PicLen);
		memcpy(module_preview->pic_data, add_pix[picnum]->pic_data, PicLen);

		module_preview->pic_width = add_pix[picnum]->pic_width;
		module_preview->pic_height = add_pix[picnum]->pic_height;
		module_preview->depth = add_pix[picnum]->depth;
		module_preview->format_type = add_pix[picnum]->format_type;
		module_preview->col_format = add_pix[picnum]->col_format;
		memcpy(module_preview->palette, add_pix[picnum]->palette, SM_PALETTE_SIZE);
	}

	module_preview->zoom = 0;

	/* -------- Display-Options tempor�r umbauen --------- */
	/* (Preview wird nur nach SysPal gedithert) */
	thisDisplay.dither_24 = Sys_info.PreviewDither;
	thisDisplay.dither_8 = Sys_info.PreviewDither;
	thisDisplay.dither_4 = Sys_info.PreviewDither;
	thisDisplay.syspal_24 = CR_SYSPAL;
	thisDisplay.syspal_8 = CR_SYSPAL;
	thisDisplay.syspal_4 = CR_SYSPAL;

	f_dither(module_preview, &Sys_info, 0, NULL, &thisDisplay);

	Dialog.busy.enable();

	oldxoff = wind_s[WIND_MODFORM].xoffset;
	oldyoff = wind_s[WIND_MODFORM].yoffset;
	oldpic = wind_s[WIND_MODFORM].picture;

	wind_s[WIND_MODFORM].xoffset = 0;
	wind_s[WIND_MODFORM].yoffset = 0;
	wind_s[WIND_MODFORM].picture = module_preview;

	Window.redraw(&wind_s[WIND_MODFORM], NULL, PREV_OUTER, redraw_flag);

	graf_mouse(ARROW, NULL);

	if (mod_inf->how_many_pix == 1)
	{
		SMfree(module_preview->pic_data);
	} else
		for (t = 0; t < mod_inf->how_many_pix; t++)
		{
			if (add_pix[t])
			{
				SMfree(add_pix[t]->pic_data);
				SMfree(add_pix[t]);
			}
		}

	Dialog.busy.ok();
	Dialog.busy.dispRAM();
}


/* ----------------------------------------------------------------------------	*/
/*	�berpr�ft das Clipping des Previewausschnittes im Module-Preferences-Dialog	*/
/*	anhand Bildbreite, Zoom und PrevBox-Abmessungen und pa�t es ggfs. an.		*/
/* ----------------------------------------------------------------------------	*/
static void check_clipping(void)
{
	OBJECT *modtree = wind_s[WIND_MODFORM].resource_form;


	wind_s[WIND_MODFORM].clipwid = modtree[PREV_BOX].ob_width;
	wind_s[WIND_MODFORM].cliphgt = modtree[PREV_BOX].ob_height;

	if (smurf_picture[active_pic]->pic_width / prev_zoom < modtree[PREV_BOX].ob_width)
		wind_s[WIND_MODFORM].clipwid = smurf_picture[active_pic]->pic_width / prev_zoom;
	if (smurf_picture[active_pic]->pic_height / prev_zoom < modtree[PREV_BOX].ob_height)
		wind_s[WIND_MODFORM].cliphgt = smurf_picture[active_pic]->pic_height / prev_zoom;
}


/* ------------------------------------------------------------------------	*/
/* --------------------------- MODUL- FORMULAR ----------------------------	*/
/* Nimmt Einstellungen im Modulformular vor und �bergibt ggfs. ans Modul 	*/
/* ------------------------------------------------------------------------ */
void f_mpref_change(void)
{
	char str[10];
	char confname[33] = "";
	const MODULE_START *module_start;
	WORD button;
	WORD back;
	short picnum;
	WORD oldtop;
	WORD oldw, oldh;
	WORD newwid, newhgt;
	short clip;
	BOOLEAN close_me = FALSE;
	BOOLEAN mem = FALSE;
	WORD dummy;
	WORD staste;
	long editval[4];
	long cnfblock[12];
	long *loadcnf;
	long *cnf_save;
	long **pp;
	short t;

	OBJECT *modtree;
	SMURF_PIC *edit_picture;
	SMURF_PIC *pic;
	const MOD_ABILITY *mod_abs;
	const MOD_INFO *mod_inf;


	modtree = wind_s[WIND_MODFORM].resource_form;

	button = Dialog.init(WIND_MODFORM, START_MOD);

	if ((key_scancode >> 8) == KEY_UP || (key_scancode >> 8) == KEY_DOWN)
	{
		module_start = get_module_start(module.bp[edit_mod_num]);

		mod_inf = module_start->info;
		mod_abs = module_start->ability;

		for (t = 0; t < 4; t++)
		{
			clip = FALSE;
			editval[t] = atol(modtree[ed[t]].TextCast);
			if (editval[t] < mod_inf->erange[t].min)
			{
				editval[t] = mod_inf->erange[t].min;
				clip = TRUE;
			} else if (editval[t] > mod_inf->erange[t].max)
			{
				editval[t] = mod_inf->erange[t].max;
				clip = TRUE;
			}
			if (clip)
			{
				ltoa(editval[t], str, 10);
				strncpy(modtree[ed[t]].TextCast, str, 5);
				Window.redraw(&wind_s[WIND_MODFORM], NULL, ed[t], DRAWNOPICTURE);
			}
		}
	}

	switch (button)
	{
	case PREVZOOM_DEC:
		prev_zoom++;
		check_clipping();
		f_make_preview(0);
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_DEC, OS_UNSEL, 1);
		break;

	case PREVZOOM_INC:
		if (prev_zoom > 1)
			prev_zoom--;
		check_clipping();
		f_make_preview(0);
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_INC, OS_UNSEL, 1);
		break;

	case PREVZOOM_RESET:
		prev_zoom = 1;
		check_clipping();
		f_make_preview(0);
		change_object(&wind_s[WIND_MODFORM], PREVZOOM_RESET, OS_UNSEL, 1);
		break;


		/*---- Schiebereglerbedienung: ----*/
	case S1_F:
	case S1_R:
		sval[0] = f_rslid(&sliders[PDSLIDER1]);
		break;
	case S2_F:
	case S2_R:
		sval[1] = f_rslid(&sliders[PDSLIDER2]);
		break;
	case S3_F:
	case S3_R:
		sval[2] = f_rslid(&sliders[PDSLIDER3]);
		break;
	case S4_F:
	case S4_R:
		sval[3] = f_rslid(&sliders[PDSLIDER4]);
		break;

	case S1_A:
		sval[0] = f_numedit(S1_A, modtree, sval[0]);
		if (sval[0] > sliders[PDSLIDER1].max_val)
			sval[0] = (WORD) sliders[PDSLIDER1].max_val;
		else if (sval[0] < sliders[PDSLIDER1].min_val)
			sval[0] = (WORD) sliders[PDSLIDER1].min_val;
		setslider(&sliders[PDSLIDER1], sval[0]);
		break;
	case S2_A:
		sval[1] = f_numedit(S2_A, modtree, sval[1]);
		if (sval[1] > sliders[PDSLIDER2].max_val)
			sval[1] = (WORD) sliders[PDSLIDER2].max_val;
		else if (sval[1] < sliders[PDSLIDER2].min_val)
			sval[1] = (WORD) sliders[PDSLIDER2].min_val;
		setslider(&sliders[PDSLIDER2], sval[1]);
		break;
	case S3_A:
		sval[2] = f_numedit(S3_A, modtree, sval[2]);
		if (sval[2] > sliders[PDSLIDER3].max_val)
			sval[2] = (WORD) sliders[PDSLIDER3].max_val;
		else if (sval[2] < sliders[PDSLIDER3].min_val)
			sval[2] = (WORD) sliders[PDSLIDER3].min_val;
		setslider(&sliders[PDSLIDER3], sval[2]);
		break;
	case S4_A:
		sval[3] = f_numedit(S4_A, modtree, sval[3]);
		if (sval[3] > sliders[PDSLIDER4].max_val)
			sval[3] = (WORD) sliders[PDSLIDER4].max_val;
		else if (sval[3] < sliders[PDSLIDER4].min_val)
			sval[3] = (WORD) sliders[PDSLIDER4].min_val;
		setslider(&sliders[PDSLIDER4], sval[3]);
		break;

		/* Preview-Icon angeklickt! */
	case PREV_BOX:
		if (picthere)
		{
			pic = smurf_picture[active_pic];
			if (pic->block != NULL)
				pic = pic->block;
			f_move_preview(&wind_s[WIND_MODFORM], pic, PREV_ACTION);
			f_make_preview(DRAWNOTREE);
		}
		break;

	case PICS_INFO:
		ready_modpics_popup(&wind_s[WIND_MODFORM]);
		f_pop(&popups[POPUP_PICORDER], 1, 0, NULL);	/* Bildreihenfolge-popup �ffnen */
		change_object(&wind_s[WIND_MODFORM], PICS_INFO, OS_UNSEL, 1);
		break;

	case MCONF_STDSAVE:
		module_start = get_module_start(module.bp[edit_mod_num]);
		mod_inf = module_start->info;
		for (t = 0; t < 4; t++)
		{
			cnfblock[t] = sval[t];
			cnfblock[t + 4] = modtree[check[t]].ob_state;
			cnfblock[t + 8] = atol(modtree[ed[t]].TextCast);
		}
		mconfSave(mod_inf, edit_mod_num, cnfblock, 12 * 4, confname);
		change_object(&wind_s[WIND_MODFORM], MCONF_STDSAVE, OS_UNSEL, 1);
		break;

	case MCONF_STDLOAD:
		module_start = get_module_start(module.bp[edit_mod_num]);
		mod_inf = module_start->info;
		loadcnf = (long *) mconfLoad(mod_inf, edit_mod_num, confname);
		if (loadcnf != NULL)
		{
			applyConfig(loadcnf);
			SMfree(loadcnf);
			Window.redraw(&wind_s[WIND_MODFORM], NULL, 0, 0);
			for (t = 0; t < 4; t++)
				setslider(&sliders[pdslider[t]], sval[t]);
		}
		change_object(&wind_s[WIND_MODFORM], MCONF_STDLOAD, OS_UNSEL, 1);
		break;

		/* Los! (Start gedr�ckt): */
	case START_MOD:
		change_object(&wind_s[WIND_MODFORM], START_MOD, OS_UNSEL, 1);

		module_start = get_module_start(module.bp[edit_mod_num]);
		mod_inf = module_start->info;

		/*
		 * Editfeld-Werte erneut clippen, da diese vor dem Modulstart
		 * noch ge�ndert werden k�nnen
		 */
		for (t = 0; t < 4; t++)
		{
			editval[t] = atol(modtree[ed[t]].TextCast);
			if (editval[t] < mod_inf->erange[t].min)
				editval[t] = mod_inf->erange[t].min;
			else if (editval[t] > mod_inf->erange[t].max)
				editval[t] = mod_inf->erange[t].max;
			ltoa(editval[t], modtree[ed[t]].TextCast, 10);
		}

		/*
		 * Default-Konfigurationsblock anfordern
		 */
		cnf_save = SMalloc(50);
		cnf_save[0] = sval[0];
		cnf_save[1] = sval[1];
		cnf_save[2] = sval[2];
		cnf_save[3] = sval[3];
		cnf_save[4] = modtree[CHECK1].ob_state;
		cnf_save[5] = modtree[CHECK2].ob_state;
		cnf_save[6] = modtree[CHECK3].ob_state;
		cnf_save[7] = modtree[CHECK4].ob_state;
		cnf_save[8] = editval[0];
		cnf_save[9] = editval[1];
		cnf_save[10] = editval[2];
		cnf_save[11] = editval[3];
		
		pp = (long **)&module.smStruct[edit_mod_num]->event_par[0];
		*pp = cnf_save;
		module.smStruct[edit_mod_num]->event_par[2] = 12 * (short)sizeof(*cnf_save);
		memorize_emodConfig(module.bp[edit_mod_num], module.smStruct[edit_mod_num]);

		if (!picthere)
			break;

		/*
		 * Gargamel-Struktur f�llen
		 */
		Window.top_window_now(&wind_s[WIND_BUSY]);
		module_start = get_module_start(module.bp[edit_mod_num]);
		mod_inf = module_start->info;
		mod_abs = module_start->ability;

		pic = smurf_picture[active_pic];

		if (mod_inf->how_many_pix > 1)
		{
			back = f_give_pics(mod_inf, mod_abs, edit_mod_num);
			if (back != 0)
				return;
		} else
		{
			if (pic->block != NULL)
				pic = pic->block;

			back = f_convert(pic, mod_abs, RGB, SAME, 0);
			if (back == M_MEMORY)
			{
				mem = TRUE;
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_MEM].TextCast, NULL, NULL, NULL, 1);
				Dialog.busy.reset(128, "Speicher ...");
			}
		}

		if (mem == FALSE)
		{
			f_insert_prefs(module.smStruct[edit_mod_num], pic);

			oldw = pic->pic_width;
			oldh = pic->pic_height;

			/*
			 * Modul starten... 
			 */
			Window.windGet(0, WF_TOP, &oldtop, 0, 0, 0);
			Window.top_window_now(&wind_s[WIND_BUSY]);
			Window.redraw(&wind_s[WIND_BUSY], NULL, 0, 0);

			module.smStruct[edit_mod_num]->event_par[0] = position_markers[edit_mod_num & 0xFF].xpos[0];
			module.smStruct[edit_mod_num]->event_par[1] = position_markers[edit_mod_num & 0xFF].ypos[0];
			module.comm.start_edit_module("", module.bp[edit_mod_num], MCH_COORDS, edit_mod_num, module.smStruct[edit_mod_num]);

			/* FIXME: translate */
			Dialog.busy.reset(0, "Modul arbeitet...");
			if (key_at_event & KEY_ALT)
				close_me = TRUE;

			module.comm.start_edit_module(edit_modules[edit_mod_num], module.bp[edit_mod_num], MEXEC, module.smStruct[edit_mod_num]->module_number, module.smStruct[edit_mod_num]);
			f_handle_modmessage(module.smStruct[edit_mod_num]);

			Dialog.busy.ok();

			/*
			 * alles erledigt?
			 */
			if (module.smStruct[edit_mod_num]->module_mode == M_PICDONE)
			{
				if (mod_inf->how_many_pix > 1)
				{
					picnum = module.smStruct[edit_mod_num]->event_par[0];
					picnum = module_pics[edit_mod_num][picnum];
					edit_picture = smurf_picture[picnum];
				} else
				{
					edit_picture = module.smStruct[edit_mod_num]->smurf_pic;
					if (edit_picture->block)
					{
						edit_picture->blockwidth = edit_picture->block->pic_width;
						edit_picture->blockheight = edit_picture->block->pic_width;
						edit_picture = edit_picture->block;
					}

					picnum = active_pic;
				}

				newwid = picture_windows[picnum].picture->pic_width;
				newhgt = picture_windows[picnum].picture->pic_height;
				if (oldw != newwid || oldh != newhgt)
					imageWindow.clipPicwin(&picture_windows[picnum]);

				picture_windows[picnum].clipwid = newwid;
				picture_windows[picnum].cliphgt = newhgt;

				/* damit bei Bild�nderungen im Fall von Median-Cut */
				/* auch die Palette neu berechnet wird und sowieso */
				if (edit_picture->local_nct)
				{
					SMfree(edit_picture->local_nct);
					edit_picture->local_nct = NULL;
				}

				if (!Sys_info.realtime_dither)
					f_dither(edit_picture, &Sys_info, 1, NULL, &Display_Opt);

				Window.redraw(&picture_windows[picnum], NULL, 0, 0);
				f_pic_changed(&picture_windows[picnum], 1);

				Dialog.busy.ok();
			}							/* picdone */

			Window.top_windowhandle(oldtop);
		}								/* genug Speicher */

		if (close_me)
			Window.close(wind_s[WIND_MODFORM].whandlem);
		break;
	}

	check_prevzoom();

	/*------------ Immediate-Preview? -------------*/
	graf_mkstate(&dummy, &dummy, &dummy, &staste);
	if (Sys_info.immed_prevs == OS_SELECTED && !(staste & 0x03) &&
		(button == S1_R || button == S2_R || button == S3_R || button == S4_R))
	{
		wind_s[WIND_MODFORM].xoffset = oldxoff;
		wind_s[WIND_MODFORM].yoffset = oldyoff;
		f_make_preview(DRAWNOTREE);
	}

	if (!close_me)
		check_and_terminate(module.smStruct[edit_mod_num]->module_mode, edit_mod_num);
}


/* ------------------------------------------------------------------------		*/
/*							Preview bewegen										*/
/* Handled das Verschieben des Previews von bild orig_pic im Fenster			*/
/* window mit der Maus. Zum Redraw wird Objekt redraw_object benutzt.			*/
/* Der Redraw wird in jedem Fall mit Online-Dithering ausgef�hrt, um			*/
/* voneinander verschiedene Preview- und Bild-Zooms richtig handlen zu			*/
/* k�nnen und immer den Preview-Dither zu sehen. Es wird je ein waagrechtes		*/
/* und ein senkrechtes Redraw-Rechteck erzeugt, die die Breite bzw. H�he		*/
/* der Mauszeigerbewegung und die H�he bzw. Breite des gesamten Preview-		*/
/* ausschnittes haben.															*/
/* Die beim Ausschnittsweise Dithern entstehenden Fehler sind hier relativ		*/
/* wurscht, da der Ausschnitt sowieso gleich nach dem Loslassen vom Modul		*/
/*	bearbeitet und komplett neu dargestellt wird.								*/
/* Das Dithermodul schreibt dann die Darstelung des angeforderten Ausschnitts	*/
/* an die in der �bergebenen DITHER_DATA stehenden Koordinaten der Bildschirm-	*/
/* Darstellung. Also wird immer nur das in die Darstellung reingedithert, was	*/
/* verschoben wurde.															*/
/* ------------------------------------------------------------------------		*/
void f_move_preview(WINDOW *window, SMURF_PIC *orig_pic, WORD redraw_object)
{
	WORD dummy;
	WORD mbutt;
	WORD mx, my;
	WORD omx, omy;
	WORD max_xoff, max_yoff;
	SMURF_PIC move_pic;
	SMURF_PIC *old_pic;
	GRECT redraw;
	DISPLAY_MODES old;

	graf_mkstate(&omx, &omy, &mbutt, &dummy);	/* immer noch gedr�ckt? */

	graf_mouse(FLAT_HAND, NULL);
	wind_update(BEG_UPDATE);
	wind_update(BEG_MCTRL);

	if (window->wnum == WIND_MODFORM)
	{
		window->xoffset = oldxoff;
		window->yoffset = oldyoff;
	}

	/*
	 * Jetzt das aktive Bild nach move_pic kopieren und den Screenzeiger freigeben.
	 */
	memcpy(&move_pic, orig_pic, sizeof(SMURF_PIC));
	move_pic.screen_pic = NULL;
	move_pic.zoom = prev_zoom - 1;
	move_pic.local_nct = NULL;
	old_pic = window->picture;
	window->picture = &move_pic;

	max_xoff = (window->picture->pic_width / prev_zoom) - window->clipwid;
	max_yoff = (window->picture->pic_height / prev_zoom) - window->cliphgt;

	if (max_xoff < 0)
		max_yoff = 0;
	if (max_yoff < 0)
		max_yoff = 0;

	graf_mkstate(&mx, &my, &mbutt, &dummy);
	omx = mx;
	omy = my;

	/*
	 * Display-Options faken
	 */
	make_singular_display(&old, Sys_info.PreviewMoveDither, CR_SYSPAL);

	do
	{
		graf_mkstate(&mx, &my, &mbutt, &dummy);	/* immer noch gedr�ckt? */

		if (mx != omx || my != omy)
		{
			window->xoffset += (omx - mx) /* *prev_zoom */ * 2;
			window->yoffset += (omy - my) /* *prev_zoom */ * 2;

			if (window->xoffset < 0)
				window->xoffset = 0;
			if (window->xoffset > max_xoff)
				window->xoffset = max_xoff;

			if (window->yoffset < 0)
				window->yoffset = 0;
			if (window->yoffset > max_yoff)
				window->yoffset = max_yoff;

			redraw.g_x = window->wx + window->pic_xpos;
			redraw.g_y = window->wy + window->pic_ypos;
			redraw.g_w = window->clipwid;
			redraw.g_h = window->cliphgt;
			Window.redraw(window, &redraw, redraw_object, DRAWNOTREE);

			omx = mx;
			omy = my;
		}
	} while (mbutt);

	restore_display(&old);

	wind_update(END_UPDATE);
	wind_update(END_MCTRL);
	graf_mouse(ARROW, NULL);

	window->picture = old_pic;
}


void copy_preview(SMURF_PIC *source_pic, SMURF_PIC *preview, WINDOW *prev_window)
{
	long CutOff, BPP;
	long SrcPlanelen, DestPlanelen;
	long SrcLineLen, DestLineLen;
	long Soff = 0;
	long Doff = 0;
	WORD y, plane, x;
	uint8_t *Destdata;
	uint8_t *Srcdata;
	uint8_t *sptr;
	uint8_t *dptr;
	uint8_t *linebuf;
	uint8_t *src;
	uint8_t *dest;
	int16_t *sptr16;
	int16_t *dptr16;
	WORD prev_endhgt, prev_endwid;
	WORD desty_count, destx_count;

	Srcdata = source_pic->pic_data;
	Destdata = preview->pic_data;

	if (source_pic->format_type == FORM_PIXELPAK)
	{
		SrcLineLen = ((long) source_pic->pic_width * (long) source_pic->depth) / 8L;
		DestLineLen = ((long) preview->pic_width * (long) preview->depth) / 8L;
		BPP = (long) preview->depth / 8L;
	} else
	{
		SrcLineLen = ((long) source_pic->pic_width + 7) / 8L;
		DestLineLen = ((long) preview->pic_width + 7) / 8L;
		SrcPlanelen = SrcLineLen * (long) source_pic->pic_height;
		DestPlanelen = DestLineLen * (long) preview->pic_height;
		BPP = 1;
	}

	/*------------------ Bildausschnitt ins Preview kopieren - Pixelpacked */
	if (source_pic->format_type == FORM_PIXELPAK)
	{
		CutOff = (long) (prev_window->yoffset) * SrcLineLen;
		CutOff += (long) (prev_window->xoffset) * BPP;

		prev_endhgt = preview->pic_height * prev_zoom;
		if (prev_endhgt > source_pic->pic_height)
			prev_endhgt = source_pic->pic_height;

		prev_endwid = preview->pic_width * prev_zoom;
		if (prev_endwid > source_pic->pic_width)
			prev_endwid = source_pic->pic_width;

		desty_count = 0;
		for (y = 0; y < prev_endhgt; y += prev_zoom)
		{
			Soff = y * SrcLineLen;
			Doff = desty_count * DestLineLen;

			destx_count = 0;
			if (source_pic->depth == 24)
			{
				for (x = 0; x < prev_endwid; x += prev_zoom)
				{
					dptr = Destdata + Doff + destx_count;
					sptr = Srcdata + Soff + CutOff + x * 3;
					*(dptr) = *(sptr);
					*(dptr + 1) = *(sptr + 1);
					*(dptr + 2) = *(sptr + 2);
					destx_count += 3;
				}
			} else if (source_pic->depth == 16)
			{
				for (x = 0; x < prev_endwid; x += prev_zoom)
				{
					dptr16 = (int16_t *) (Destdata + Doff + destx_count);
					sptr16 = (int16_t *) (Srcdata + Soff + CutOff + x + x);
					*(dptr16) = *(sptr16);
					destx_count += 2;
				}
			} else if (source_pic->depth == 8)
			{
				for (x = 0; x < prev_endwid; x += prev_zoom)
				{
					dptr = Destdata + Doff + destx_count;
					sptr = Srcdata + Soff + CutOff + x;
					*(dptr) = *(sptr);
					destx_count++;
				}
			}

			desty_count++;
		}
	}
	/*---------------------------------- Standardformat */
	else
	{
		CutOff = (long) (prev_window->yoffset) * SrcLineLen;
		CutOff += (long) (prev_window->xoffset) / 8;

		/*
		 * ohne Zoom
		 */
		if (prev_zoom == 1)
		{
			for (plane = 0; plane < preview->depth; plane++)
			{
				for (y = 0; y < preview->pic_height; y++)
				{
					Soff = y * SrcLineLen;
					Doff = y * DestLineLen;
					memcpy(Destdata + Doff, Srcdata + Soff + CutOff, DestLineLen);
				}
				Destdata += DestPlanelen;
				Srcdata += SrcPlanelen;
			}
		}
		/*
		 * mit Zoom
		 */
		else
		{
			linebuf = SMalloc(preview->pic_width * prev_zoom + 32);

			for (y = 0; y < preview->pic_height; y++)
			{
				memset(linebuf, 0, preview->pic_width * prev_zoom + 32);
				getpix_std_line(Srcdata + Soff + CutOff, linebuf, source_pic->depth, SrcPlanelen, preview->pic_width * prev_zoom);

				src = linebuf;
				dest = linebuf;
				x = 0;
				do
				{
					*dest++ = *src++;
					src += prev_zoom - 1;
				} while (++x < preview->pic_width);
				setpix_std_line16(linebuf, Destdata + Doff, source_pic->depth, DestPlanelen, preview->pic_width);

				Soff = y * SrcLineLen * prev_zoom;
				Doff = y * DestLineLen;
			}

			SMfree(linebuf);
		}
	}
}
