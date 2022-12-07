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

#include <string.h>
#include <stdio.h>

#include "import.h"
#include "smurfine.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "globdefs.h"
#include "destruct.h"
#include "plugin.h"
#include "smplugin.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"
#include "debug.h"



void f_init_menu(void)
{
	menu_bar(menu_tree, 1);
}


void f_exit_menu(void)
{
	menu_bar(menu_tree, 0);
}


/*	-------------------------------------------	*/
/*					Menu Handler				*/
/*	-------------------------------------------	*/
WORD f_handle_menuevent(WORD *message)
{
	WORD menu_entry;
	WORD quit = 0;
	WORD dummy;
	WORD wh;
	WORD wnum;
	WORD nexthandle;
	WORD t;
	WINDOW *nextwin;
	SMURF_PIC *picture;

	menu_entry = message[4];

	if (Smurf_locked)					/* GUI gesperrt? */
		return 0;

	picture = smurf_picture[active_pic];

#if 0
	if ((Sys_info.OS & MAG_PC) && (key_at_event == KEY_ALT || key_at_event == KEY_CTRL))
		menu_title -= 5;
#endif

	openmode = 0;						/* Immer davon ausgehen, daû der Dialog                 */
	/* (welcher auch immer) noch nicht offen ist,           */
	/* um bei einem Menuevent einen folgenden Buttonevent   */
	/* zu vermeiden / zu ignorieren.                        */


	if (menu2plugin[menu_entry] != -1)	/* ein Plugin hat sich hier eingehÑngt! */
	{
		call_plugin(menu_entry);
		return 0;
	}

	switch (menu_entry)
	{
	case DESK_INFO:
		f_info();
		break;

	case MFILE_LOAD:
		file_load("Bild laden", (char **) &"", 0);
		break;
	
	case FILE_RELOAD:
		reload_pic(&picture_windows[active_pic]);
		break;

	case FILE_SAVEAS:
		Dialog.expmodList.handleList();
		break;

	case FILE_SAVE:
		save_file();
		break;
	
	case FILE_INFO:
		f_pic_info();
		break;

#if 0
	case FILE_SCAN;
		gdps_main();
		break;
#endif

	case FILE_QUIT:
		quit = -1;
		if (picthere)
		{
			for (t = 1; t <= picthere; t++)
			{
				if (picture_windows[t].wtitle[11] == '*' && !(Sys_info.profi_mode & OS_SELECTED))
					if (Dialog.winAlert.openAlert(Dialog.winAlert.alerts[QUIT_ALERT].TextCast, "Nein", " Ja ", NULL, 1) == 1) /* FIXME: translate */
						quit = 0;
			}
		}
		break;

	case WIND_CLOSE:
		Window.windGet(0, WF_TOP, &wh, &dummy, &dummy, &dummy);
		Window.close(wh);
		break;
	
	case FILE_NEW:
		f_newpic(0);
		break;

	case DISP_KONFIG:
		Dialog.dispOpt.displayOptions();
		break;

	case DISP_NEXTWIN:
		/*---- Dialogfenster cyclen -------*/
		Window.windGet(0, WF_TOP, &wh, &dummy, &dummy, &dummy);
		wnum = Window.myWindow(wh);
		if (wnum > 0)
		{
			/*----- ist ein Dialog oben? ---*/
			nextwin = (WINDOW *) (wind_s[wnum].next_window);
			if (nextwin != NULL)
				nexthandle = nextwin->whandlem;
		} else
		{
			/*----- oder ein Bild? */
			if (Dialog.topDialog != 256)
			{
				nexthandle = Dialog.topDialog;
			} else
			{
				nexthandle = 256;
				for (t = 0; t < MAX_PIC; t++)
				{
					if (wind_s[t].whandlem != -1 && wind_s[t].whandlem < nexthandle)
						nexthandle = wind_s[t].whandlem;
				}
			}
		}

		if (nexthandle != 256)
		{
			Dialog.topDialog = nexthandle;
			Window.top(nexthandle);
		}
		break;

	case DISP_NEXTPIC:
		/*---- Bildfenster cyclen ---*/
		Window.windGet(0, WF_TOP, &wh, &dummy, &dummy, &dummy);
		wnum = Window.myWindow(wh);
		if (wnum < 0)
		{
			/*------ ist ein Bild oben? ---*/
			nextwin = (WINDOW *) (picture_windows[-wnum].next_window);
			if (nextwin != NULL)
				nexthandle = nextwin->whandlem;
		} else
		{
			/*--- oder ein Dialog? ---*/
			if (imageWindow.topPicwin != 256)
			{
				nexthandle = imageWindow.topPicwin;
			} else
			{
				nexthandle = 256;
				for (t = 0; t < MAX_PIC; t++)
				{
					if (picture_windows[t].whandlem != -1 && picture_windows[t].whandlem < nexthandle)
						nexthandle = picture_windows[t].whandlem;
				}
			}
		}

		if (nexthandle != 256)
		{
			imageWindow.topPicwin = nexthandle;
#if 0
			Window.top(nexthandle);
#endif
			wnum = Window.myWindow(nexthandle);
			if (wnum < 0)
			{
				Window.topNow(&picture_windows[-wnum]);
				f_activate_pic(-wnum);
			} else
				Window.topNow(&picture_windows[-wnum]);
		}
		break;

	case EDIT_EDIT:
		f_edit_pop();
		break;

	case EDIT_COLORS:
		color_choose();
		break;

	case EDIT_COLRED:
		transform_pic();
		break;

	case DUPLICATE_PIC:
		duplicate_pic(&picture_windows[active_pic]);
		break;

	case SELECT_ALL:
		block_over_all(&picture_windows[active_pic]);
		break;

	case EDIT_COPY:
		block2clip(picture, 0, NULL);
		break;

	case EDIT_INSERT:
		clip2block(picture, NULL, -1, -1);
		Window.redraw(&picture_windows[active_pic], NULL, 0, 0);
		break;

	case EDIT_CUT:
		if (block2clip(picture, 1, NULL) != -1)
		{
			imageWindow.toggleAsterisk(&picture_windows[active_pic], 1);
			f_dither(picture, &Sys_info, 1, NULL, &Display_Opt);
			Window.redraw(&picture_windows[active_pic], NULL, 0, 0);
		}
		break;

	case EDIT_RELEASE:
		imageWindow.removeBlock(&picture_windows[active_pic]);
		blockfunctions_off();
		Window.redraw(&picture_windows[active_pic], NULL, 0, 0);
		if (wind_s[WIND_BTYPEIN].whandlem != -1)
			insert_blockcoords(picture);
		break;

	case EDIT_CROP:
		block_freistellen(&picture_windows[active_pic]);
		blockfunctions_off();
		break;

	case OPT_OPTIONS:
		Dialog.smurfOpt.options();
		break;

	case OPT_STATUS:
		Window.open(&wind_s[WIND_BUSY]);
		break;

	case OPT_PICMAN:
		Window.open(&wind_s[WIND_PICMAN]);
		break;

	case OPT_SAVEOPT:
		save_config();
		break;

	case PLUGIN1:
	case PLUGIN2:
	case PLUGIN3:
	case PLUGIN4:
	case PLUGIN5:
	case PLUGIN6:
	case PLUGIN7:
	case PLUGIN8:
	case PLUGIN9:
	case PLUGIN10:
		call_plugin(menu_entry);
		break;
	}

	return quit;
}


/*-------------------------------------------------------------
	Aktualisiert die MenÅzeile je nachdem, ob ein Bild geladen 
	ist, oder nicht. 
	ZusÑtzlich werden die Start-Buttons in den Modullisten und im
	Standard-Editmoduldialog en-/disabled.
	----------------------------------------------------------*/
void actualize_menu(void)
{
	DEBUG_MSG(("actualize_menu...\n"));

	menu_tree[WIND_CLOSE].ob_state |= OS_DISABLED;
	menu_tree[FILE_RELOAD].ob_state |= OS_DISABLED;
	menu_tree[FILE_INFO].ob_state |= OS_DISABLED;
	menu_tree[FILE_PRINT].ob_state |= OS_DISABLED;
	menu_tree[FILE_SAVE].ob_state |= OS_DISABLED;
	menu_tree[EDIT_COLRED].ob_state |= OS_DISABLED;
	menu_tree[DUPLICATE_PIC].ob_state |= OS_DISABLED;
	menu_tree[SELECT_ALL].ob_state |= OS_DISABLED;
	menu_tree[EDIT_INSERT].ob_state |= OS_DISABLED;
	menu_tree[DISP_NEXTPIC].ob_state |= OS_DISABLED;
	menu_tree[DISP_NEXTWIN].ob_state |= OS_DISABLED;

	change_object(&wind_s[WIND_MODULES], START_MODULE, OS_DISABLED, 1);
	change_object(&wind_s[WIND_EXPORT], START_EMOD, OS_DISABLED, 1);
	change_object(&wind_s[WIND_MODFORM], START_MOD, OS_DISABLED, 1);

	if (Dialog.emodList.anzahl == 0)
		menu_tree[EDIT_EDIT].ob_state |= OS_DISABLED;

	if (Dialog.expmodList.anzahl == 0)
		menu_tree[FILE_SAVEAS].ob_state |= OS_DISABLED;

	if (picwindthere || dialwindthere)
	{
		menu_tree[WIND_CLOSE].ob_state &= ~OS_DISABLED;

		if (picwindthere > 0)
		{
			menu_tree[FILE_RELOAD].ob_state &= ~OS_DISABLED;
			menu_tree[FILE_INFO].ob_state &= ~OS_DISABLED;

			if (printplug_found)
				menu_tree[FILE_PRINT].ob_state &= ~OS_DISABLED;

			if (Sys_info.defaultExporter != -1)
				menu_tree[FILE_SAVE].ob_state &= ~OS_DISABLED;

			menu_tree[EDIT_COLRED].ob_state &= ~OS_DISABLED;
			menu_tree[DUPLICATE_PIC].ob_state &= ~OS_DISABLED;
			menu_tree[SELECT_ALL].ob_state &= ~OS_DISABLED;
			menu_tree[EDIT_INSERT].ob_state &= ~OS_DISABLED;
			menu_tree[DISP_NEXTPIC].ob_state &= ~OS_DISABLED;

			/*
			 * mal was anderes, hat nichts mit der MenÅzeile
			 * zu tun, der Status wird aber genauso bestimmt
			 */
			change_object(&wind_s[WIND_MODULES], START_MODULE, OS_ENABLED, 1);
			change_object(&wind_s[WIND_EXPORT], START_EMOD, OS_ENABLED, 1);
			change_object(&wind_s[WIND_MODFORM], START_MOD, OS_ENABLED, 1);
		}

		if (dialwindthere > 0)
			menu_tree[DISP_NEXTWIN].ob_state &= ~OS_DISABLED;
	}
}


void lock_Smurf(void)
{
	menu_ienable(menu_tree, DESK_INFO, 0);	/* MenÅeintrag ausschalten */
	menu_ienable(menu_tree, MENU_FILE, 0);	/* MenÅeintrag ausschalten */
	menu_ienable(menu_tree, MENU_DISP, 0);	/* MenÅeintrag ausschalten */
	menu_ienable(menu_tree, MENU_EDIT, 0);	/* MenÅeintrag ausschalten */
	menu_ienable(menu_tree, MENU_OPTIONS, 0);	/* MenÅeintrag ausschalten */
	menu_ienable(menu_tree, MENU_PLUGIN, 0);	/* MenÅeintrag ausschalten */

	menu_bar(menu_tree, 1);

	Smurf_locked = 1;
}


void unlock_Smurf(void)
{
	menu_ienable(menu_tree, DESK_INFO, 1);	/* MenÅeintrag einschalten */
	menu_ienable(menu_tree, MENU_FILE, 1);	/* MenÅeintrag einschalten */
	menu_ienable(menu_tree, MENU_DISP, 1);	/* MenÅeintrag einschalten */
	menu_ienable(menu_tree, MENU_EDIT, 1);	/* MenÅeintrag einschalten */
	menu_ienable(menu_tree, MENU_OPTIONS, 1);	/* MenÅeintrag einschalten */
	menu_ienable(menu_tree, MENU_PLUGIN, 1);	/* MenÅeintrag einschalten */

	menu_bar(menu_tree, 1);

	Smurf_locked = 0;
}
