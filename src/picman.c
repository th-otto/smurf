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
#include <stdlib.h>
#include <math.h>
#include "import.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "popdefin.h"
#include "plugin.h"
#include "smplugin.h"

#include "globdefs.h"
#include "vaproto.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"


/* ****************************************************************	*/
/*							Bildmanager								*/
/* ****************************************************************	*/
void f_picman(void)
{
	char *ptr;
	char *swapstr;
	char *namestr;
	char pathstr[SM_PATH_MAX];
	char avname[66];
	WORD button = 0;
	WORD popbutton = 0;
	WORD lf_back;
	short picture_num = -1;
	WORD drag_beginx, drag_beginy;
	WORD drag_endx, drag_endy;
	WORD xc, yc;
	WORD endwh;
	WORD endwind;
	WORD endob;
	WORD swapnum;
	WORD buttonnum;
	WORD endobnum;
	short pathlen;
	WORD old_scroll_offset;
	WORD old_sel, new_sel;
	WORD newmb;
	WORD dummy;
	WORD ap_buf[16];
	WORD my_word1, my_word2;
	WORD my_word3, my_word4;
	WORD my_scancode;
	WORD dest_whandle;

	OBJECT *ob;
	SMURF_PIC *pic;
	WINDOW *mod_wind = 0;
	WINDOW *mwindow = 0;
	const MOD_INFO *minf;


	my_scancode = (key_scancode & 0xff00) >> 8;

	ob = Dialog.picMan.tree;

	if (my_scancode != KEY_UP && my_scancode != KEY_DOWN)
		button = Dialog.init(WIND_PICMAN, 0);

	/* Bildmanager aktualisieren */
	old_sel = f_listfield(Dialog.picMan.window, 0, 0, &Dialog.picMan.pictureList);
	lf_back = f_listfield(Dialog.picMan.window, button, key_scancode, &Dialog.picMan.pictureList);

	new_sel = lf_back;
	lf_back = lf_back - Dialog.picMan.pictureList.scroll_offset + PM_PIC1;


	/*---------- Preview angeklickt (Fensterausschnitt verschieben oder neues Preview?) */
	if (button == PICMAN_PREVIEW)
	{
		objc_offset(ob, PICMAN_PREVIEW, &xc, &yc);

		Dialog.picMan.navigateWindow();
	}

	/* Bildname angeklickt oder Return und Bild selektiert? */
	if ((button >= PM_PIC1 && button <= PM_PIC8)
		|| (lf_back >= PM_PIC1 && lf_back <= PM_PIC8 && key_ascii == KEY_RETURN))
	{
		/* dazugehîriges Bild ermitteln */
		picture_num = Dialog.picMan.picmanList[(lf_back - PM_PIC1) + Dialog.picMan.pictureList.scroll_offset];
		pic = smurf_picture[picture_num];

		f_activate_pic(picture_num);

		/* Immediate Preview? */

		if (picture_num != -1 && picture_windows[picture_num].whandlem != -1)
		{
			graf_mkstate(&dummy, &dummy, &newmb, &dummy);

			/* Doppelklick / Return? */
			if (klicks == 2 || key_ascii == KEY_RETURN)
			{
				Window.top(picture_windows[picture_num].whandlem);

				/* mit Alternate (Verzeichnisfenster îffnen)? */
				if (key_at_event == KEY_ALT)
				{
					namestr = strrchr(picture_windows[picture_num].picture->filename, '\\') + 1;
					pathlen = (short) (namestr - picture_windows[picture_num].picture->filename);
					strncpy(pathstr, picture_windows[picture_num].picture->filename, pathlen);
					strcpy(avname, namestr);
					pathstr[pathlen] = 0;

					my_word1 = (short) ((long) pathstr >> 16);
					my_word2 = (short) (long) pathstr;
					my_word3 = (short) ((long) avname >> 16);
					my_word4 = (short) (long) avname;

					if (Sys_info.ENV_avserver != -1)
					{
						ap_buf[0] = AV_XWIND;
						ap_buf[1] = Sys_info.app_id;
						ap_buf[2] = 0;
						ap_buf[3] = my_word1;
						ap_buf[4] = my_word2;
						ap_buf[5] = my_word3;
						ap_buf[6] = my_word4;
						ap_buf[7] = 0x0003;
						appl_write(Sys_info.ENV_avserver, 16, ap_buf);
					}
				}
				/* Control (Viewer starten)? */
				else if (key_at_event == KEY_CTRL)
				{
					ptr = picture_windows[picture_num].picture->filename;
					my_word1 = (WORD) ((long) ptr >> 16);
					my_word2 = (WORD) (long) ptr;
					Comm.sendAESMsg(Sys_info.ENV_avserver, AV_STARTPROG, my_word1, my_word2, 0, 0, PM_VIEWCALL, -1);
				}
			}
			/* Drag-Operation */
			else if (newmb && !IsDisabled(ob[button]))
			{
				objc_offset(ob, button, &drag_beginx, &drag_beginy);
				old_scroll_offset = Dialog.picMan.pictureList.scroll_offset;

				graf_mouse(FLAT_HAND, NULL);
				strncpy(u_tree[DRAGTXT_INSIDE].TextCast, ob[button].TextCast, 12);
				f_drag_object(Dialog.picMan.window, button, &drag_endx, &drag_endy, 1, Dialog.picMan.autoScroll);
				graf_mouse(ARROW, NULL);

				/* D&D-Target ermitteln */
				endwh = wind_find(drag_endx, drag_endy);
				endwind = Window.myWindow(endwh);
				if (!endwind)
					mod_wind = Window.myModuleWindow(endwh);
				else if (endwind != 0)
					mwindow = &wind_s[endwind];

				if (mod_wind != 0)
					mwindow = mod_wind;

				/* Bildmanager selbst */
				if (endwind == WIND_PICMAN && (drag_beginx != drag_endx || drag_beginy != drag_endx))
				{
					endob = objc_find(wind_s[endwind].resource_form, 0, MAX_DEPTH, drag_endx, drag_endy);

					/* Eintrag? */
					if (endob >= PM_PIC1 && endob <= PM_PIC8 && endob != button)
					{
						buttonnum = (button - PM_PIC1) + old_scroll_offset;
						endobnum = (endob - PM_PIC1) + Dialog.picMan.pictureList.scroll_offset;

						/* steht im Zielobjekt ein Bild? */
						if (Dialog.picMan.picmanList[endobnum] != -1)
						{
							swapnum = Dialog.picMan.picmanList[buttonnum];
							Dialog.picMan.picmanList[buttonnum] = Dialog.picMan.picmanList[endobnum];
							Dialog.picMan.picmanList[endobnum] = swapnum;

							swapstr = picnames[endobnum];
							picnames[endobnum] = picnames[buttonnum];
							picnames[buttonnum] = swapstr;
							f_listfield(Dialog.picMan.window, F_REDRAW, 0, &Dialog.picMan.pictureList);
							Window.redraw(Dialog.picMan.window, NULL, 0, 0);
						}
					}
				} else
					/* Modulfenster oder PD? */
				if (mod_wind != 0 || endwind == WIND_MODFORM)
				{
					minf = ready_modpics_popup(mwindow);

					if (minf->how_many_pix > 1)
					{
						/* In die Bildliste des Moduls einhÑngen ----------- */
						strncpy(picorder_popup[DRAGPIC].TextCast, strrchr(pic->filename, '\\') + 1, 12);
						popbutton = f_pop(&popups[POPUP_PICORDER], 1, 0, NULL);	/* Bildreihenfolge fÅrs Modul festlegen */
						module_pics[mwindow->module][popbutton - P1] = picture_num;

						/*-------------- evtl. Fadenkreuz ein -----------*/
						if (position_markers[mwindow->module].anzahl &&
							position_markers[mwindow->module].mod_pic[0] == popbutton - P1)
						{
							/* Defaultkoordinaten erfragen */
							module.smStruct[mwindow->module]->smurf_pic = picture_windows[picture_num].picture;
							module.comm.start_edit_module("", module.bp[mwindow->module], MCH_DEFCOO, mwindow->module, module.smStruct[mwindow->module]);
							if (module.smStruct[mwindow->module]->module_mode == M_CHDEFCOO)
							{
								position_markers[mwindow->module].xpos[0] =
									module.smStruct[mwindow->module]->event_par[0];
								position_markers[mwindow->module].ypos[0] =
									module.smStruct[mwindow->module]->event_par[1];
							}

							position_markers[mwindow->module].smurfpic[0] = picture_num;
							imageWindow.drawCrosshair(&picture_windows[picture_num]);
						}
					}
				} else
				{
					dest_whandle = wind_find(drag_endx, drag_endy);
					if (send_dragdrop(smurf_picture[picture_num], dest_whandle, drag_endx, drag_endy) < 0)
					{
						if (dest_whandle == 0)
						{
							Comm.avComm.type = AV_IMAGE;
							Comm.avComm.windowhandle = picture_windows[picture_num].whandlem;
							Comm.avComm.keystate = key_at_event;
							Comm.sendAESMsg(Sys_info.ENV_avserver, AV_WHAT_IZIT, drag_endx, drag_endy, -1);
						}
					}
				}

			}							/* D&D-Operation? */
		}								/* angeklicktes Bild vorhanden? */
	}									/* Bildname angeklickt? */

	picture_num = Dialog.picMan.picmanList[(lf_back - PM_PIC1) + Dialog.picMan.pictureList.scroll_offset];

	if (picture_num >= 0 && picture_num != Dialog.picMan.selectedPic)
		Dialog.picMan.showWH(smurf_picture[picture_num]);

	/* Leereintrag selektiert oder BM leer? */
	if (picture_num == -1 || smurf_picture[picture_num] == NULL)
	{
		if (lf_back > PM_PIC1)
		{
			change_object(Dialog.picMan.window, lf_back, OS_UNSEL, 1);
			change_object(Dialog.picMan.window, lf_back - 1, OS_SELECTED, 1);
		} else if (ob[PM_PIC1].ob_state & OS_SELECTED)
		{
			if (Dialog.picMan.thumbnail.screen_pic)
			{
				SMfree(Dialog.picMan.thumbnail.screen_pic->fd_addr);
				Dialog.picMan.thumbnail.screen_pic->fd_addr = NULL;
				free(Dialog.picMan.thumbnail.screen_pic);
				Dialog.picMan.thumbnail.screen_pic = NULL;
				Dialog.picMan.window->picture = NULL;
			}

			change_object(Dialog.picMan.window, PM_PIC1, OS_UNSEL, 1);
			Dialog.picMan.window->picture = NULL;
			strcpy(ob[PM_WIDTH].TextCast, "");
			ob[X1].ob_flags |= OF_HIDETREE;
			strcpy(ob[PM_HEIGHT].TextCast, "");
			ob[X2].ob_flags |= OF_HIDETREE;
			strcpy(ob[PM_DEPTH].TextCast, "");
			Window.redraw(Dialog.picMan.window, NULL, 0, 0);
		}
	}
	/*
	 * Thumbnail erzeugen/Bild aktivieren
	 */
	else if ((picture_num != -1 && new_sel != old_sel) || openmode == 0)
	{
		if (Sys_info.immed_prevs == OS_SELECTED)
			Dialog.picMan.makeThumbnail(picture_num);

		f_activate_pic(picture_num);
	}
}


/* ----------------------------------------------------------------	*/
/*					Bild in Bildmanager einfÅgen					*/
/* ----------------------------------------------------------------	*/
void insert_to_picman(short pic_to_insert)
{
	short pmentry;
	WORD sc_off;
	WORD max_entr;
	short action;
	short t;
	WORD first;
	WORD last;
	BOOLEAN scrolled = FALSE;
	WORD entry_to_select;
	OBJECT *res;

	pmentry = 0;
	while (picnames[pmentry] != NULL)
		pmentry++;

	sc_off = Dialog.picMan.pictureList.scroll_offset;
	max_entr = Dialog.picMan.pictureList.max_entries;
	res = Dialog.picMan.tree;
	first = res[Dialog.picMan.pictureList.parent_obj].ob_head;
	last = res[Dialog.picMan.pictureList.parent_obj].ob_tail;

#if 0
	pic = picture_windows[pic_to_insert].picture;
#endif

	/* Bild in den Bildmanager eintragen */
	picnames[pmentry] = picture_windows[pic_to_insert].wtitle + 12;
	Dialog.picMan.picmanList[pmentry] = pic_to_insert;
	Dialog.picMan.pictureList.number_entries++;

	/*------------------------------- ggfs. Scrolloffset anpassen */
	if (pmentry > (max_entr + sc_off - 1))
	{
		action = Dialog.picMan.pictureList.slar_dn;
		scrolled = TRUE;
	} else
		action = F_REDRAW;

	/* Bild im BM aktivieren */
	for (t = first; t < last; t++)
		res[t].ob_state &= ~OS_SELECTED;

	if (scrolled)
		entry_to_select = last;
	else
		entry_to_select = first + pmentry - sc_off;

	res[entry_to_select].ob_state |= OS_SELECTED;

	Dialog.picMan.makeThumbnail(pic_to_insert);

	f_listfield(Dialog.picMan.window, action, 0, &Dialog.picMan.pictureList);

	Dialog.picMan.selectedPic = pic_to_insert;
}


/* ----------------------------------------------------------------	*/
/*			Hîhe und Breite im Bildmanager updaten					*/
/* ----------------------------------------------------------------	*/
void show_picman_wh(SMURF_PIC *pic)
{
	char string[10];
	OBJECT *ob;

	if (pic)
	{
		ob = Dialog.picMan.tree;

		itoa(pic->pic_width, string, 10);
		strcpy(ob[PM_WIDTH].TextCast, string);
		ob[X1].ob_flags &= ~OF_HIDETREE;
		itoa(pic->pic_height, string, 10);
		strcpy(ob[PM_HEIGHT].TextCast, string);
		ob[X2].ob_flags &= ~OF_HIDETREE;
		itoa(pic->depth, string, 10);
		strncpy(ob[PM_DEPTH].TextCast, string, 2);
		Window.redraw(Dialog.picMan.window, NULL, HW_BOX, 0);
	}
}


/* ----------------------------------------------------------------	*/
/*			Picman-Autoscroll beim Draggen eines Eintrags			*/
/* ----------------------------------------------------------------	*/
BOOLEAN pm_autoscroll(WORD mx, WORD my)
{
	WORD objct;
	BOOLEAN redraw = FALSE;

	objct = objc_find(Dialog.picMan.tree, 0, MAX_DEPTH, mx, my);

	switch (objct)
	{
	case PM_UP:
	case PM_DN:
		f_listfield(Dialog.picMan.window, objct, 0, &Dialog.picMan.pictureList);
		redraw = TRUE;						/* Drag-Bereich neu holen/zeichnen */
		break;
	default:
		break;
	}

	return redraw;
}


/* ----------------------------------------------------------------	*/
/*						Bildliste umsortieren						*/
/*		Entfernt die leeren EintrÑge aus der Bildliste.				*/
/*		Die folgenden EintrÑge werden nachgerÅckt.					*/
/* ----------------------------------------------------------------	*/
void f_resort_piclist(void)
{
	short t;

	for (t = 0; t < MAX_PIC - 1; t++)
	{
		if (Dialog.picMan.picmanList[t] == -1 || picnames[t] == NULL
			|| smurf_picture[Dialog.picMan.picmanList[t]] == NULL)
		{
			picnames[t] = picnames[t + 1];
			picnames[t + 1] = NULL;
			Dialog.picMan.picmanList[t] = Dialog.picMan.picmanList[t + 1];
			Dialog.picMan.picmanList[t + 1] = -1;
		}
	}
}


/* ----------------------------------------------------------------	*/
/*						ready_modpics_popup							*/
/*		Bereitet das Bildreihenfolge-Popup fÅr ein Modul vor		*/
/*		und gibt dessen MOD_INFO zurÅck.							*/
/* ----------------------------------------------------------------	*/

const MOD_INFO *ready_modpics_popup(WINDOW *mwindow)
{
	const MODULE_START *module_start;
	char *dest_str;
	char *src_str;
	char *miptr[8];
	char *src_str2;
	char *dest_str2;
	short t;
	const MOD_INFO *minf;
	BASPAG *modbp;

	modbp = module.bp[mwindow->module];
	module_start = get_module_start(modbp);
	minf = module_start->info;

	miptr[0] = minf->pic_descr1;
	miptr[1] = minf->pic_descr2;
	miptr[2] = minf->pic_descr3;
	miptr[3] = minf->pic_descr4;
	miptr[4] = minf->pic_descr5;
	miptr[5] = minf->pic_descr6;

	/*------- Wenn mehrere Bilder benîtigt werden, den Kram reinkopieren ----*/
	if (minf->how_many_pix > 1)
	{
		for (t = 0; t < 6; t++)
			picorder_popup[P1 + t].ob_state |= OS_DISABLED;

		for (t = 0; t < minf->how_many_pix; t++)
		{
			dest_str = picorder_popup[PDES1 + t].TextCast;
			src_str = miptr[t];

			picorder_popup[P1 + t].ob_state &= ~OS_DISABLED;

			src_str2 = picture_windows[module_pics[mwindow->module][t]].picture->filename;
			dest_str2 = picorder_popup[P1 + t].TextCast;
			if (src_str)
				strncpy(dest_str, src_str, 15);
			else
				strncpy(dest_str, "                ", 15);
			if (src_str2)
				strncpy(dest_str2, strrchr(src_str2, '\\') + 1, 12);
			else
				strncpy(dest_str2, "________.___", 12);
		}
	}

	return minf;
}


/* ----------------------------------------------------------------	*/
/*					Thumbnail im Bildmanager erzeugen 				*/
/* ----------------------------------------------------------------	*/
void make_picman_thumbnail(short picture_num)
{
	short t;
	WORD first_sel;
	SMURF_PIC *pic;
	OBJECT *ob;

#if 0
	/* Oh doch, sonst wird weder Preview noch WH beim ôffnen des Picman
	   angezeigt wenn vorher schon ein Bild geladen worden war */
	/* wenn BM zu, kein Preview berechnen. */
	if (Dialog.picMan.window->whandlem == -1)
		return;
#endif
	ob = Dialog.picMan.tree;

	Dialog.picMan.thumbnail.pic_width = ob[PM_PREVBOX].ob_width;
	Dialog.picMan.thumbnail.pic_height = ob[PM_PREVBOX].ob_height;
	Dialog.picMan.window->pic_xpos = ob[PM_PREVBOX].ob_x + 1;
	Dialog.picMan.window->pic_ypos = ob[PM_PREVBOX].ob_y + 1;
	Dialog.picMan.window->clipwid = ob[PM_PREVBOX].ob_width;
	Dialog.picMan.window->cliphgt = ob[PM_PREVBOX].ob_height;

	for (t = 0; t < MAX_PIC; t++)
	{
		if (Dialog.picMan.picmanList[t] == picture_num)
			break;
	}

	first_sel = get_selected_object(ob, PM_PIC1, PM_PIC8);
	ob[first_sel].ob_state &= ~OS_SELECTED;

	if (t < Dialog.picMan.pictureList.scroll_offset
		|| t > (Dialog.picMan.pictureList.scroll_offset + Dialog.picMan.pictureList.max_entries - 1))
		Dialog.picMan.pictureList.scroll_offset = t;

	if (Dialog.picMan.pictureList.scroll_offset >
		Dialog.picMan.pictureList.number_entries - Dialog.picMan.pictureList.max_entries)
		Dialog.picMan.pictureList.scroll_offset =
			Dialog.picMan.pictureList.number_entries - Dialog.picMan.pictureList.max_entries;
	if (Dialog.picMan.pictureList.scroll_offset < 0)
		Dialog.picMan.pictureList.scroll_offset = 0;

	ob[(t - Dialog.picMan.pictureList.scroll_offset + PM_PIC1)].ob_state = OS_SELECTED;

	f_listfield(Dialog.picMan.window, F_REDRAW, 0, &Dialog.picMan.pictureList);

	pic = smurf_picture[picture_num];

	/* Hîhe und Breite anzeigen */
	Dialog.picMan.showWH(pic);

	Dialog.busy.disable();

	if (pic)
	{
		make_thumbnail(pic, &Dialog.picMan.thumbnail, Sys_info.PicmanDither);
		Dialog.picMan.window->picture = &Dialog.picMan.thumbnail;
	} else
		Dialog.picMan.window->picture = NULL;

	Window.redraw(Dialog.picMan.window, NULL, PICMAN_PREVIEW, 0);

	Dialog.busy.enable();
}


void picman_windowmove(void)
{
	WORD x, y;
	WORD mbutton;
	WORD dummy;
	WORD endx, endy;
	WORD xc1, yc1, xc2, yc2;
	short zoom;
	short piczoom;
	short picnum;
	WORD picwid, pichgt;
	WORD outx, outy, outw, outh;
	OBJECT *pmtree;

	if (picwindthere == 0)
		return;

	graf_mkstate(&x, &y, &mbutton, &dummy);

	if (mbutton == 0x01)
	{
		picnum = f_listfield(Dialog.picMan.window, 0, 0, &Dialog.picMan.pictureList);
		picnum = Dialog.picMan.picmanList[picnum];

		pmtree = Dialog.picMan.tree;
		objc_offset(pmtree, PICMAN_PREVIEW, &outx, &outy);

		zoom = compute_zoom(smurf_picture[picnum], pmtree[PM_PREVBOX].ob_width, pmtree[PM_PREVBOX].ob_height) + 1;

		piczoom = picture_windows[picnum].picture->zoom + 1;
		Window.windGet(picture_windows[picnum].whandlem, WF_WORKXYWH, &dummy, &dummy, &picwid, &pichgt);
		pichgt -= TOOLBAR_HEIGHT;

		xc1 = picture_windows[picnum].xoffset * piczoom / zoom + outx;
		yc1 = picture_windows[picnum].yoffset * piczoom / zoom + outy;
		xc2 = picwid * piczoom / zoom;
		yc2 = pichgt * piczoom / zoom;

		outw = picture_windows[picnum].picture->pic_width / zoom;
		outh = picture_windows[picnum].picture->pic_height / zoom;

		if (xc1 < outx)
			xc1 = outx;
		else if (xc1 > outx + outw)
			xc1 = outx;

		if (yc1 < outy)
			yc1 = outy;
		else if (yc1 > outy + outh)
			yc1 = outy;

		if (xc2 > outw)
			xc2 = outw;

		if (yc2 > outh)
			yc2 = outh;

		graf_dragbox(xc2, yc2, xc1, yc1, outx, outy, outw, outh, &endx, &endy);
		if (endx != xc1 || endy != yc1)
		{
			picture_windows[picnum].xoffset = (endx - outx) * zoom / piczoom;
			picture_windows[picnum].yoffset = (endy - outy) * zoom / piczoom;
			Window.redraw(Dialog.picMan.window, NULL, PM_PREVBOX, 0);
			imageWindow.setSliders(&picture_windows[picnum]);
			Window.redraw(&picture_windows[picnum], NULL, 0, DRAWNOTREE);
		}
	}
}


WORD compute_zoom(SMURF_PIC *picture, WORD twid, WORD thgt)
{
	WORD owid, ohgt;
	WORD zoom;

	if (picture == NULL)
		return -1;

	owid = picture->pic_width;
	ohgt = picture->pic_height;

	if (owid >= ohgt && owid > twid)
		zoom = owid / twid;
	else if (ohgt >= owid && ohgt > thgt)
		zoom = ohgt / thgt;
	else
		zoom = 0;

	return zoom;
}
