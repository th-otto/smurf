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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "import.h"
#include "demolib.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "popdefin.h"
#include "plugin.h"
#include "olgagems.h"
#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"
#include "globdefs.h"
#include "debug.h"

/* ****************************************************************	*/
/*				Formular DARSTELLUNG / OPTIONEN						*/
/*		Die Einstellungen werden bis zur �bernahme mit Setzen oder	*/
/*		OK in der Sys_info zwischengespeichert und bei Setzen oder	*/
/*		OK in die Display_options �bernommen.						*/
/* ****************************************************************	*/
void f_display_opt(void)
{
	char *pal_loadpath;
	static char filepal_name[32] = "lade palette";	/* FIXME: trsanslate */
	WORD button = 0;
	WORD t;
	WORD popback;
	WORD olditem;
	WORD ditmode;
	OBJECT *dopt_tree;

	dopt_tree = wind_s[WIND_DOPT].resource_form;

	if (!openmode)
		Dialog.dispOpt.updateWindow(1, 1);

	button = Dialog.init(WIND_DOPT, DISPLAY_OK);

	/* Farbtiefen-Radiobutton wurde angeklickt */
	if (button == DITHER_24 || button == DITHER_8 || button == DITHER_4)
	{
		Dialog.dispOpt.updateWindow(0, 1);
	} else if (button == DITHER_POPBUT || button == DITHER_CB)
	{
		/* Dithering einstellen */
		if (dopt_tree[DITHER_24].ob_state & OS_SELECTED)
			ditmode = Sys_info.dither24;
		else if (dopt_tree[DITHER_8].ob_state & OS_SELECTED)
			ditmode = Sys_info.dither8;
		else /* if (dopt_tree[DITHER_4].ob_state & OS_SELECTED) */
			ditmode = Sys_info.dither4;

		olditem = popups[POPUP_DITHER].item;
		popups[POPUP_DITHER].item = ditmode;

		popback = f_pop(&popups[POPUP_DITHER], 0, button, NULL);

		popups[POPUP_DITHER].item = olditem;

		if (popback > 0)
		{
			if (dopt_tree[DITHER_4].ob_state & OS_SELECTED)
			{
				Sys_info.dither4 = popback;
				if (ditmod_info[popback - 1]->pal_mode == FIXPAL)
					Sys_info.pal4 = CR_FIXPAL;
				else /* if (Sys_info.pal4 == CR_FIXPAL) */
					Sys_info.pal4 = CR_SYSPAL;
			} else if (dopt_tree[DITHER_8].ob_state & OS_SELECTED)
			{
				Sys_info.dither8 = popback;
				if (ditmod_info[popback - 1]->pal_mode == FIXPAL)
					Sys_info.pal8 = CR_FIXPAL;
				else /* if (Sys_info.pal8 == CR_FIXPAL) */
					Sys_info.pal8 = CR_SYSPAL;
			} else if (dopt_tree[DITHER_24].ob_state & OS_SELECTED)
			{
				Sys_info.dither24 = popback;
				if (ditmod_info[popback - 1]->pal_mode == FIXPAL)
					Sys_info.pal24 = CR_FIXPAL;
				else /* if (Sys_info.pal24 == CR_FIXPAL) */
					Sys_info.pal24 = CR_SYSPAL;
			}

			Dialog.dispOpt.updateWindow(0, 1);
		} else
		{
			f_deselect_popup(&wind_s[WIND_DOPT], DITHER_POPBUT, DITHER_CB);
		}
	} else if (button == COLRED_POPBUT || button == COLRED_CB)
	{
		if (dopt_tree[DITHER_24].ob_state & OS_SELECTED)
			ditmode = Sys_info.pal24;
		else if (dopt_tree[DITHER_8].ob_state & OS_SELECTED)
			ditmode = Sys_info.pal8;
		else /* if (dopt_tree[DITHER_4].ob_state & OS_SELECTED) */
			ditmode = Sys_info.pal4;

		olditem = popups[POPUP_COLRED].item;
		popups[POPUP_COLRED].item = ditmode;

		strcpy(colred_popup[CR_FILEPAL].TextCast, filepal_name);

		popback = f_pop(&popups[POPUP_COLRED], 0, button, NULL);

		popups[POPUP_COLRED].item = olditem;

		if (popback > 0)
		{
			if (dopt_tree[DITHER_4].ob_state & OS_SELECTED)
				Sys_info.pal4 = popback;
			else if (dopt_tree[DITHER_8].ob_state & OS_SELECTED)
				Sys_info.pal8 = popback;
			else /* if (dopt_tree[DITHER_24].ob_state & OS_SELECTED) */
				Sys_info.pal24 = popback;

			Dialog.dispOpt.updateWindow(0, 1);
		} else
		{
			f_deselect_popup(&wind_s[WIND_DOPT], COLRED_POPBUT, COLRED_CB);
		}
	} else if (button == LOAD_PAL)
	{
		/*--------- feste Palette laden -------*/
		pal_loadpath = load_palfile(Sys_info.standard_path, fix_red, fix_green, fix_blue, Sys_info.Max_col + 1);
		if (pal_loadpath != NULL)
		{
			strcpy(filepal_name, strrchr(pal_loadpath, '\\') + 1);
			strcpy(colred_popup[CR_FILEPAL].TextCast, filepal_name);
			strcpy(dopt_tree[COLRED_POPBUT].TextCast, filepal_name);
		}

		change_object(&wind_s[WIND_DOPT], COLRED_POPBUT, OS_UNSEL, 1);
		change_object(&wind_s[WIND_DOPT], LOAD_PAL, OS_UNSEL, 1);
	}


	/*---------------------- Einstellungen �bernehmen */
	if (button == DISPLAY_OK || button == DISPLAY_SET)
	{
		Display_Opt.dither_24 = Sys_info.dither24;
		Display_Opt.dither_8 = Sys_info.dither8;
		Display_Opt.dither_4 = Sys_info.dither4;

		Display_Opt.syspal_4 = Sys_info.pal4;
		Display_Opt.syspal_8 = Sys_info.pal8;
		Display_Opt.syspal_24 = Sys_info.pal24;

		if (Dialog.dispOpt.tree[PAL_MOUSE].ob_state & OS_SELECTED)
			Display_Opt.palette_mode = PAL_MOUSE;
		else if (Dialog.dispOpt.tree[PAL_TOPWIN].ob_state & OS_SELECTED)
			Display_Opt.palette_mode = PAL_TOPWIN;
		else /* if (Dialog.dispOpt.tree[PAL_SYSTEM].ob_state & OS_SELECTED) */
			Display_Opt.palette_mode = PAL_SYSTEM;

		Sys_info.Event_Timer = atoi(Dialog.dispOpt.tree[PAL_TIMER].TextCast);

		/* ist unbedingt n�tig da bei 0 im Sys_info.Event_Timer zum Ruckeln */
		/* bei Fenstermoves und seltsamerweise zum WIND_CLOSE beim Verschieben */
		/* des Displayoptions-Fensters f�hrt */
		if (Sys_info.Event_Timer == 0)
			Sys_info.Event_Timer = 1;

		if (button == DISPLAY_OK)
		{
			Dialog.dispOpt.updateWindow(1, 0);
			change_object(&wind_s[WIND_DOPT], DISPLAY_OK, OS_UNSEL, 1);

			Dialog.close(WIND_DOPT);
		}

		if (Sys_info.realtime_dither)
		{
			Dialog.busy.reset(0, "Updating display");
			for (t = 0; t <= picwindthere; t++)
			{
				Dialog.busy.draw(t * 128 / (picwindthere + 1));
				Window.redraw(&picture_windows[t], NULL, 0, DRAWNOTREE);
			}

			Dialog.busy.ok();
		}
	}

	if (button == DISPLAY_SET)
		change_object(&wind_s[WIND_DOPT], DISPLAY_SET, OS_UNSEL, 1);
}


/*------------------------ Display-Options Popups updaten. -----------------*/
/* mode: 0=aus Sysinfo (nicht �bernommene Werte)							*/
/* 		 1=aus Display_Opt (�bernommene Werte)								*/
void f_update_dwindow(short mode, BOOLEAN redraw)
{
	short dit4, dit8, dit24;
	short pal4, pal8, pal24;
	short rpal;
	short rdit = 0;
	OBJECT *dopt_tree;

	DEBUG_MSG(("f_update_dwindow...\n"));

	if (mode == 0)
	{
		dit4 = Sys_info.dither4;
		dit8 = Sys_info.dither8;
		dit24 = Sys_info.dither24;
		pal4 = Sys_info.pal4;
		pal8 = Sys_info.pal8;
		pal24 = Sys_info.pal24;
	} else /* if (mode == 1) */
	{
		dit4 = Display_Opt.dither_4;
		dit8 = Display_Opt.dither_8;
		dit24 = Display_Opt.dither_24;
		pal4 = Display_Opt.syspal_4;
		pal8 = Display_Opt.syspal_8;
		pal24 = Display_Opt.syspal_24;
	}

	dopt_tree = wind_s[WIND_DOPT].resource_form;

	if (dopt_tree[DITHER_4].ob_state & OS_SELECTED)
	{
		rdit = dit4;
		rpal = pal4;
	} else if (dopt_tree[DITHER_8].ob_state & OS_SELECTED)
	{
		rdit = dit8;
		rpal = pal8;
	} else /* if (dopt_tree[DITHER_24].ob_state & OS_SELECTED) */
	{
		rdit = dit24;
		rpal = pal24;
	}

	strncpy(dopt_tree[DITHER_POPBUT].TextCast, col_pop[rdit].TextCast, 15);
	strncpy(dopt_tree[COLRED_POPBUT].TextCast, colred_popup[rpal].TextCast, 15);

	if (rpal == CR_FILEPAL)
		change_object(&wind_s[WIND_DOPT], LOAD_PAL, OS_ENABLED, 1);
	else
		change_object(&wind_s[WIND_DOPT], LOAD_PAL, OS_DISABLED, 1);


	if (rpal == CR_FIXPAL)
	{
		change_object(&wind_s[WIND_DOPT], COLRED_POPBUT, OS_DISABLED, 0);
		change_object(&wind_s[WIND_DOPT], COLRED_CB, OS_DISABLED, 0);
	} else
	{
		change_object(&wind_s[WIND_DOPT], COLRED_POPBUT, OS_ENABLED, 0);
		change_object(&wind_s[WIND_DOPT], COLRED_CB, OS_ENABLED, 0);
	}

	if (redraw)
	{
		f_deselect_popup(&wind_s[WIND_DOPT], DITHER_POPBUT, DITHER_CB);
		f_deselect_popup(&wind_s[WIND_DOPT], COLRED_POPBUT, COLRED_CB);
	}

	DEBUG_MSG(("f_update_dwindow...Ende\n"));
}
