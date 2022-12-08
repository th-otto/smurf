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
#include "vaproto.h"
#include "destruct.h"
#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"

#define	TOP		1
#define	BOTTOM	2
#define	RIGHT	4
#define	LEFT	8
#define MOVE	16

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))
#ifndef __PUREC__
#define IOREC _IOREC
#endif

static WORD lastmousex = 0;
static WORD lastmousey = 0;



BLOCKMODE blockmode_conf = {
	BCONF_REPLACE,
	100L,
	0
};


/*-------------------------------------------------------------------------------------
	int	mouse_block_position
	Legt in *hmode / *vmode anhand der Mauskoordinaten mx/my fest, an welcher Kante der 
	Block in picwindow angefažt wurde. Wenn hierbei ein neuer Block aufgezogen wird,
	ist der Rckgabewert 1, und 0, wenn ein bereits vorhandener Block ge„ndert wird.
	-----------------------------------------------------------------------------------	*/
static BOOLEAN mouse_block_position(WINDOW *picwindow, WORD mx, WORD my, WORD *hmode, WORD *vmode)
{
	WORD n_mx, n_my;
	WORD bx1, by1, bx2, by2;
	BOOLEAN new_block = TRUE;
	short zoom;
	WORD windx, windy, dummy, xoff, yoff;
	SMURF_PIC *picture;

	picture = picwindow->picture;

	/* ist berhaupt ein Block aufgezogen? */
	if (picture->blockheight == 0 || picture->blockwidth == 0)
		return TRUE;

	zoom = picwindow->picture->zoom + 1;

	*(hmode) = *(vmode) = 0;

	Window.windGet(picwindow->whandlem, WF_WORKXYWH, &windx, &windy, &dummy, &dummy);

	/*--- relative Position des Mauszeigers zum Fenster */
	n_mx = mx - windx;
	n_my = my - (windy + TOOLBAR_HEIGHT);

	n_mx *= zoom;
	n_my *= zoom;
	n_mx -= 1;

	bx1 = picture->blockx;
	by1 = picture->blocky;
	bx2 = picture->blockwidth + bx1;
	by2 = picture->blockheight + by1;

	xoff = picwindow->xoffset * zoom;
	yoff = picwindow->yoffset * zoom;

	/* ---------------Mauszeiger irgendwo im Block? ---------------------- */
	if (n_mx >= bx1 - 2 - xoff && n_mx <= bx2 + 2 - xoff && n_my >= by1 - 2 - yoff && n_my <= by2 + 2 - yoff)
	{
		new_block = FALSE;

		if (n_mx <= bx1 - xoff + 4)
			*(hmode) = LEFT;			/* H-Modus: links */
		else if (n_mx >= bx2 - xoff - 4)
			*(hmode) = RIGHT;			/* H-Modus: rechts */

		if (n_my <= by1 - yoff + 4)
			*vmode = TOP;				/* V-Modus: oben */
		else if (n_my >= by2 - yoff - 4)
			*(vmode) = BOTTOM;			/* V-Modus: unten */
	}

	return new_block;
}


static BOOLEAN crosshair_mouse(WINDOW *window, WORD mx, WORD my)
{
	WORD arrnum = -1;
	WORD x,	y;
	WORD dummy;
	WORD all_yoff, all_xoff;
	WORD modwin_handle;
	WORD top_handle;

	arrnum = imageWindow.findCrosshair(window);

	if (arrnum == -1)
		return FALSE;
	if (position_markers[arrnum].mod_pic[0] == -1)
		return FALSE;
	if (module.smStruct[arrnum] == NULL)
		return FALSE;

	/*-------- Ist das dazugeh”rige Modulfenster getoppt? -----*/
	modwin_handle = module.smStruct[arrnum]->wind_struct->whandlem;
	Window.windGet(0, WF_TOP, &top_handle, &dummy, &dummy, &dummy);
	if (modwin_handle != top_handle)
		return FALSE;

	/* absolute Fensterkoordinaten (Bildfl„che) ausrechnen */
	Window.windGet(window->whandlem, WF_WORKXYWH, &all_xoff, &all_yoff, &dummy, &dummy);

	all_xoff = all_xoff - window->xoffset;
	all_yoff = all_yoff + TOOLBAR_HEIGHT - window->yoffset;
	x = position_markers[arrnum].xpos[0] + all_xoff;
	y = position_markers[arrnum].ypos[0] + all_yoff;

	if (my > y - 3 && my < y + 3 && mx > x - 3 && mx < x + 3)
		return TRUE;
	else
		return FALSE;
}


static void move_crosshair(WINDOW *window)
{
	WORD arrnum = -1;
	WORD all_yoff, all_xoff;
	WORD move_x, move_y;
	WORD omx = 0;
	WORD omy = 0;
	WORD mbutton;
	WORD dummy;
	WORD windx, windy, windx2, windy2;
	WORD clip[6];
	WORD window_redraw = 0;
	WORD scroll;
	WORD modwin_handle;
	WORD top_handle;
	short save;

	arrnum = imageWindow.findCrosshair(window);

	if (arrnum == -1)
		return;
	if (position_markers[arrnum].mod_pic[0] == -1)
		return;
	if (module.smStruct[arrnum] == NULL)
		return;

	/*-------- Ist das dazugeh”rige Modulfenster getoppt? -----*/
	modwin_handle = module.smStruct[arrnum]->wind_struct->whandlem;
	Window.windGet(0, WF_TOP, &top_handle, &dummy, &dummy, &dummy);
	if (modwin_handle != top_handle)
		return;

	/* absolute Fensterkoordinaten (Bildfl„che) ausrechnen */
	Window.windGet(window->whandlem, WF_WORKXYWH, &windx, &windy, &windx2, &windy2);

	windx2 += windx;
	windy2 += windy;
	windy += TOOLBAR_HEIGHT;

	/*
	 * VDI-Clipping auf Bildfl„che setzen
	 */
	clip[0] = windx;
	clip[1] = windy;
	clip[2] = windx2 - 1;				/* weil Koordinaten und nicht Breite */
	clip[3] = windy2 - 1;				/* weil Koordinaten und nicht H”he */

	Window.windGet(window->whandlem, WF_WORKXYWH, &all_xoff, &all_yoff, &dummy, &dummy);

	all_yoff += TOOLBAR_HEIGHT;

	/* Fenster einmal ohne Positionsmarker neu zeichnen */
	save = position_markers[arrnum].mod_pic[0];
	position_markers[arrnum].mod_pic[0] = -1;
	Window.redraw(window, NULL, 0, DRAWNOTREE);
	position_markers[arrnum].mod_pic[0] = save;

	do
	{
		graf_mouse(M_OFF, NULL);
		vs_clip(Sys_info.vdi_handle, TRUE, clip);
		imageWindow.drawCrosshair(window);	/* Positionsmarker zeichnen */
		graf_mouse(M_ON, NULL);

		do
		{
			graf_mkstate(&move_x, &move_y, &mbutton, &dummy);
		} while (move_x == omx && move_y == omy && mbutton != 0 &&
				 move_x > windx && move_y > windy && move_x < windx2 && move_y < windy2);

		omx = move_x;
		omy = move_y;

		imageWindow.displayCoords(window, omx, omy, 0);

		graf_mouse(M_OFF, NULL);
		vs_clip(Sys_info.vdi_handle, TRUE, clip);
		wind_update(BEG_UPDATE);
		wind_update(BEG_MCTRL);
		imageWindow.drawCrosshair(window);	/* l”schen... */
		wind_update(END_MCTRL);
		wind_update(END_UPDATE);
		graf_mouse(M_ON, NULL);

		/*--------------------- ggfs. Bild scrollen ------*/
		window_redraw = 0;
		scroll = 0;
		if (move_x > windx2)
		{
			scroll = move_x - windx2;
			move_x = windx2;
			window_redraw = WA_RTLINE;
		} else if (move_x < windx)
		{
			scroll = windx - move_x;
			move_x = windx;
			window_redraw = WA_LFLINE;
		}

		if (move_y > windy2)
		{
			scroll = move_y - windy2;
			move_y = windy2;
			window_redraw = WA_DNLINE;
		} else if (move_y < windy)
		{
			scroll = windy - move_y;
			move_y = windy;
			window_redraw = WA_UPLINE;
		}

		if (window_redraw)
		{
			save = position_markers[arrnum].mod_pic[0];
			position_markers[arrnum].mod_pic[0] = -1;
			imageWindow.arrowWindow(window_redraw, window, scroll);
			position_markers[arrnum].mod_pic[0] = save;
		}

		position_markers[arrnum].xpos[0] = move_x - all_xoff + window->xoffset;
		position_markers[arrnum].ypos[0] = move_y - all_yoff + window->yoffset;

		if (position_markers[arrnum].xpos[0] < 0)
			position_markers[arrnum].xpos[0] = 0;
		else if (position_markers[arrnum].xpos[0] > window->picture->pic_width)
			position_markers[arrnum].xpos[0] = window->picture->pic_width;

		if (position_markers[arrnum].ypos[0] < 0)
			position_markers[arrnum].ypos[0] = 0;
		else if (position_markers[arrnum].ypos[0] > window->picture->pic_height - 1)
			position_markers[arrnum].ypos[0] = window->picture->pic_height - 1;
	} while (mbutton != 0);

	Window.redraw(window, NULL, 0, DRAWNOTREE);
}


static void pic_popup(WINDOW *picwindow)
{
	WORD back;
	WORD dummy;
	WORD kstate;
	SMURF_PIC *picture = picwindow->picture;

	back = f_pop(&popups[POPUP_PIC], 1, 0, NULL);
	switch (back)
	{
	case PIC_REDITHER:
		graf_mkstate(&dummy, &dummy, &dummy, &kstate);
		if (!(kstate & KEY_SHIFT))
		{
			Window.topNow(&wind_s[WIND_BUSY]);

			if (picture->local_nct)
			{
				SMfree(picture->local_nct);
				picture->local_nct = NULL;
			}

			if (!Sys_info.realtime_dither)
				f_dither(picture, &Sys_info, 1, NULL, &Display_Opt);

			if (picture->block != NULL)
			{
				memcpy(picture->block->red, picture->red, SM_PALETTE_MAX * sizeof(picture->red[0]));
				memcpy(picture->block->grn, picture->grn, SM_PALETTE_MAX * sizeof(picture->grn[0]));
				memcpy(picture->block->blu, picture->blu, SM_PALETTE_MAX * sizeof(picture->blu[0]));
				picture->block->local_nct = picture->local_nct;
				picture->block->not_in_nct = picture->not_in_nct;
				f_dither(picture->block, &Sys_info, 1, NULL, &Display_Opt);
			}

			Window.topNow(picwindow);
			Window.redraw(picwindow, NULL, 0, 0);
			Dialog.busy.ok();
		} else
		{
			f_set_syspal();
			openmode = 0;
			Dialog.dispOpt.displayOptions();

			if (picture->depth <= 4)
				obj = DITHER_4;
			else if (picture->depth > 4 && picture->depth <= 8)
				obj = DITHER_8;
			else if (picture->depth > 8)
				obj = DITHER_24;

			openmode = 2;
			Dialog.dispOpt.displayOptions();
		}
		break;

	case PIC_DUPLICATE:
		duplicate_pic(picwindow);
		break;

	case PIC_RELOAD:
		reload_pic(picwindow);
		break;

	case PIC_EDIT:
		Dialog.emodList.handleList();
		break;

	case PIC_EXPORT:
		Dialog.expmodList.handleList();
		break;

	case PIC_BLOCK:
		f_block_popup(picwindow);
		break;

	case INFOBUTTON:
		f_info();
		break;

	default:
		f_set_picpal(picture);
	}
}


/*-------------------------------------------------------------------------------------
	void drop_block
	Der Block aus picwindow wurde an den Mauskoordinaten mx/my aužerhalb von picwindow
	abgelegt. Diese Funktion handelt alle weiteren Aktionen wie šbertragung mittels 
	Drag&Drop-Protokoll, Speichern mit AV-Protokoll, šbertragen in ein anderes Smurf-
	Bildfenster, etc.
	-----------------------------------------------------------------------------------	*/
static void drop_block(WINDOW *picwindow, WORD mx, WORD my)
{
	char newPicName[SM_PATH_MAX];
	WORD dest_whandle;
	WORD my_wnum;
	WORD key;
	WORD dummy;
	short newPic = 1;
	WINDOW *dest_picwindow;
	SMURF_PIC *dest_picture;
	SMURF_PIC *srcpic;
	GRECT redraw;

	dest_whandle = wind_find(mx, my);
	my_wnum = Window.myWindow(dest_whandle);


	/*
	 * smurfeigenes Bildfenster? -> Block verschieben
	 */
	if (my_wnum < 0)
	{
		my_wnum = -my_wnum;
		dest_picwindow = &picture_windows[my_wnum];
		dest_picture = dest_picwindow->picture;
		srcpic = picwindow->picture;

		if (dest_picture->block != NULL)
		{
			f_set_syspal();
			if (Dialog.winAlert.
				openAlert("Bereits vorhandenen Block im Zielbild entfernen?", "Nein", " Ja ", NULL, 1) == 1)
				return;
			else
				remove_block(dest_picwindow);
		}

		dest_picture->block = srcpic->block;

		/*
		 * Hier muž u.U. eine leere LNCT angefordert werden, denn sonst gibt es Probleme, wenn das
		 * Bild mit MedianCut gedithert wird und keine da ist - dann gibt es frs Blockdithering
		 * n„mlich auch keine. Das ist z.B. der Fall, wenn Zielbild=Bildschirmfarbtiefe und mit
		 * MedianCut gedithert, denn dann wird keine NCT fr dieses Bild angelegt.
		 */
#if 0
		if (dest_picture->local_nct == NULL)
		{
			dest_picture->not_in_nct = 0;
			dest_picture->local_nct = SMalloc(32768L);
			memset(dest_picture->local_nct, 0x0, 32768L);
		}
#endif
		dest_picture->block->local_nct = dest_picture->local_nct;

		memcpy(dest_picture->block->red, dest_picture->red, SM_PALETTE_MAX * sizeof(dest_picture->red[0]));
		memcpy(dest_picture->block->grn, dest_picture->grn, SM_PALETTE_MAX * sizeof(dest_picture->grn[0]));
		memcpy(dest_picture->block->blu, dest_picture->blu, SM_PALETTE_MAX * sizeof(dest_picture->blu[0]));

		dest_picture->blockx = (mx - dest_picwindow->wx) - (srcpic->blockwidth / 2);
		dest_picture->blocky = (my - dest_picwindow->wy - TOOLBAR_HEIGHT) - (srcpic->blockheight / 2);
		dest_picture->blockwidth = picwindow->picture->blockwidth;
		dest_picture->blockheight = picwindow->picture->blockheight;

		redraw.g_x = srcpic->blockx - picwindow->xoffset + picwindow->wx;
		redraw.g_y = srcpic->blocky - picwindow->yoffset + picwindow->wy;
		redraw.g_w = srcpic->blockwidth;
		redraw.g_h = srcpic->blockheight;
		Window.redraw(picwindow, &redraw, 0, DRAWNOBLOCK | NOBLOCKBOX);

		/*
		 * Vor remove_block mssen die Koordinaten im Quellbild auf 0 gesetzt werden,
		 * damit der Blockspeicher nicht freigegeben wird - der wird im Zielbild 
		 * noch ben”tigt!
		 */
		picwindow->picture->blockx = 0;
		picwindow->picture->blocky = 0;
		picwindow->picture->blockwidth = 0;
		picwindow->picture->blockheight = 0;
		picwindow->picture->block = NULL;

		if (!Sys_info.realtime_dither)
			f_dither(dest_picwindow->picture->block, &Sys_info, 1, NULL, &Display_Opt);

		Window.redraw(dest_picwindow, NULL, 0, BLOCK_ONLY);
	}
	/*
	 * smurfeigenes Dialogfenster? -> Block in neues Bild verschieben
	 */
	else if (my_wnum > 0)
	{
		/*
		 * erstes freies Bild ermitteln 
		 */
		while (smurf_picture[newPic] != NULL)
			newPic++;
		if (newPic > MAX_PIC)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_PIC_FREE].TextCast, NULL, NULL, NULL, 1);
			return;
		}

		/*
		 * neues Bild + Fenster anlegen
		 */
		if ((smurf_picture[newPic] = alloc_smurfpic(picwindow->picture->block, TRUE)) == NULL)
			return;

		smurf_picture[newPic]->changed = 0;

		strcpy(newPicName, "Block aus "); /* FIXME: translate */
		strcat(newPicName, (picwindow->wtitle) + 12);
		make_pic_window(newPic, picwindow->picture->block->pic_width, picwindow->picture->block->pic_height, newPicName);

		/*
		 * Block aus dem Originalbild entfernen und Bildschirmdarstellung l”schen
		 */
		if (picwindow->picture->block->screen_pic)
		{
			SMfree(picwindow->picture->block->screen_pic->fd_addr);
			free(picwindow->picture->block->screen_pic);
		}

		picwindow->picture->block->local_nct = NULL;	/* nicht freigeben!, da das nur eine Referenz auf die lnct im Quellbild ist */

		picwindow->picture->blockx = 0;
		picwindow->picture->blocky = 0;
		picwindow->picture->blockwidth = 0;
		picwindow->picture->blockheight = 0;
		SMfree(picwindow->picture->block);
		picwindow->picture->block = NULL;


		/*
		 * Dither/Redraw des neuen Fensters
		 */
		if (!Sys_info.realtime_dither)
			f_dither(smurf_picture[newPic], &Sys_info, 1, NULL, &Display_Opt);

		Window.open(&picture_windows[newPic]);
		picthere++;
		imageWindow.clipPicwin(&picture_windows[newPic]);
		Dialog.picMan.insertPic(newPic);
		f_activate_pic(newPic);

		Window.redraw(&picture_windows[newPic], NULL, 0, 0);

		Dialog.busy.dispRAM();			/* wieviel Ram? */
		actualize_menu();				/* Meneintr„ge ENABLEn / DISABLEn */
	}

	/*
	 * geht D&D schief mit AV-Protokoll versuchen
	 */
	else if (Comm.sendDragdrop(picwindow->picture->block, dest_whandle, mx, my) < 0)
		/*
		 *  Desktop?
		 */
		if (dest_whandle == 0)
		{
			graf_mkstate(&dummy, &dummy, &dummy, &key);
			Comm.avComm.type = AV_BLOCK;
			Comm.avComm.windowhandle = picwindow->whandlem;
			Comm.avComm.keystate = key;
			Comm.sendAESMsg(AV_WHAT_IZIT, mx, my, -1);
		}
}



/* ----------------------------------------------------------------	*/
/*				Blockbox mit der Maus aufziehen/„ndern				*/
/* Die Monsterfunktion												*/
/*	Der Frame kann in allen Ecken und Kanten mit der Maus gezogen 	*/
/*	werden, wird innerhalb des Frames geklickt, kann er bewegt 		*/
/*	werden.															*/
/* ----------------------------------------------------------------	*/
static void do_block_box(WINDOW *picwindow, WORD mx, WORD my)
{
	WORD n_mx, n_my;
	BOOLEAN new_block;
	BOOLEAN oldblock = FALSE;
	WORD mbutton;
	WORD move_x, move_y;
	WORD dummy;
	WORD xoff, yoff;
	WORD windx, windy, windx2, windy2;
	WORD omx, omy;
	WORD window_redraw = 0;
	WORD clip[6];
	WORD iconclip[6];
	WORD savepxy[10];
	WORD scroll;
	WORD hmode, vmode;
	WORD oldheight, oldwidth;
	short display_mode = 0;
	short zoom;
	WORD movxdif = 0;
	WORD movydif = 0;
	WORD newx = 0;
	WORD newy = 0;
	WORD newwid, newhgt;
	WORD ow, oh;
	WORD merkxoffset, merkyoffset;
	WORD temp;
	WORD old_iconxpos, old_iconypos;
	WORD old_xpos, old_ypos;
	WORD old_wid, old_hgt;
	WORD mouse_wnum;
	BOOLEAN done;

	MFDB screenm;
	MFDB buffer;
	WINDOW *mouse_window;
	GRECT redraw;
	SMURF_PIC *picture;

	picture = picwindow->picture;
	zoom = picture->zoom + 1;

	/*
	 * absolute Fensterkoordinaten (Bildfl„che) ausrechnen 
	 */
	Window.windGet(picwindow->whandlem, WF_WORKXYWH, &windx, &windy, &windx2, &windy2);
	ow = windx2;
	oh = windy2;

	windx2 += windx;
	windy2 += windy;
	windy += TOOLBAR_HEIGHT;

	/*
	 * VDI-Clipping auf Bildfl„che setzen
	 */
	clip[0] = windx;
	clip[1] = windy;
	clip[2] = windx2 - 1;				/* weil Koordinaten und nicht Breite */
	clip[3] = windy2 - 1;				/* weil Koordinaten und nicht H”he */

	old_xpos = picwindow->picture->blockx;
	old_ypos = picwindow->picture->blocky;
	old_wid = picwindow->picture->blockwidth;
	old_hgt = picwindow->picture->blockheight;

	merkxoffset = picwindow->xoffset;
	merkyoffset = picwindow->yoffset;

	/*
	 * relative Position des Mauszeigers zum Fenster
	 */
	n_mx = mx - windx;
	n_my = my - windy;

	movxdif = n_mx - (picwindow->picture->blockx / zoom - picwindow->xoffset);
	movydif = n_my - (picwindow->picture->blocky / zoom - picwindow->yoffset);

	new_block = TRUE;
	vmode = -1;
	hmode = -1;

	if (picture->block != NULL)
		oldblock = TRUE;

	new_block = mouse_block_position(picwindow, mx, my, &hmode, &vmode);
	if (vmode == -1 && hmode == -1)
		new_block = TRUE;

	/*
	 * neuer Block - Anfangsposition setzen 
	 */
	if (new_block == TRUE)
	{
		imageWindow.removeBlock(picwindow);

		graf_mouse(POINT_HAND, NULL);

		picture->blockx = n_mx + picwindow->xoffset;
		picture->blocky = n_my + picwindow->yoffset;

		xoff = windx + picture->blockx;
		yoff = windy + picture->blocky;

		picture->blockx *= zoom;
		picture->blocky *= zoom;

		hmode = RIGHT;					/* H: rechts! */
		vmode = BOTTOM;					/* V: unten!  */
	}
	/*
	 * bisheriger Block - Žnderungspunkt / Kante festlegen 
	 */
	else
	{
		if (hmode == LEFT)
			xoff = picture->blockwidth + picture->blockx;
		else if (hmode == RIGHT)
			xoff = windx + picture->blockx;
		else
			xoff = 0;

		if (vmode == TOP)
			yoff = picture->blockheight + picture->blocky;
		else if (vmode == BOTTOM)
			yoff = windy + picture->blocky;
		else
			yoff = 0;

		if (hmode == 0 && vmode == 0)
			hmode = vmode = MOVE;
	}


	/*
	 * Objektmaže des Blockicons holen
	 */
	iconclip[0] = screen.g_x;
	iconclip[1] = screen.g_y;
	iconclip[2] = screen.g_w;
	iconclip[3] = screen.g_h;

	screenm.fd_addr = NULL;

	buffer.fd_addr = SMalloc((32 + 7) / 8 * 32 * Sys_info.bitplanes);
	buffer.fd_w = 32;
	buffer.fd_h = 32;
	buffer.fd_wdwidth = (32 + 15) / 16;
	buffer.fd_stand = 0;
	buffer.fd_nplanes = Sys_info.bitplanes;

	if (!new_block)
	{
#if 0
		graf_mouse(M_OFF, NULL);
		vs_clip(Sys_info.vdi_handle, TRUE, clip);
		wind_update(BEG_UPDATE);
#endif

		Window.redraw(picwindow, NULL, 0, DRAWNOTREE | BLOCK_ONLY | DRAWNOBLOCK);
#if 0
		imageWindow.drawBlockbox(picwindow);	/* Blockbox l”schen */
		wind_update(END_UPDATE);
		graf_mouse(M_ON, NULL);
#endif
	}

	/*--------- Mauspos einmal speichern ---------------*/
	graf_mkstate(&move_x, &move_y, &mbutton, &dummy);
	omx = move_x;
	omy = move_y;

	/*--------- Und los gehz... --------*/
	wind_update(BEG_MCTRL);

	do
	{
		/*---------------------- Blockbox/Blockicon zeichnen ------*/
		graf_mouse(M_OFF, NULL);

		if (display_mode == 0)
		{
			vs_clip(Sys_info.vdi_handle, TRUE, clip);
			wind_update(BEG_UPDATE);
			imageWindow.drawBlockbox(picwindow);	/* zeichnen */
			wind_update(END_UPDATE);
		} else
		{
			vs_clip(Sys_info.vdi_handle, TRUE, iconclip);

			savepxy[0] = omx - 31;
			savepxy[1] = omy - 31;
			savepxy[2] = omx;
			savepxy[3] = omy;
			savepxy[4] = 0;
			savepxy[5] = 0;
			savepxy[6] = 31;
			savepxy[7] = 31;

			if (savepxy[0] < 0)
			{
				savepxy[0] = 0;
				savepxy[2] = 31;
			}
			if (savepxy[1] < 0)
			{
				savepxy[1] = 0;
				savepxy[3] = 31;
			}

			vro_cpyfm(Sys_info.vdi_handle, S_ONLY, savepxy, &screenm, &buffer);	/* Hintergrund retten */
			old_iconxpos = u_tree[BLOCK_DRAG].ob_x = savepxy[0];
			old_iconypos = u_tree[BLOCK_DRAG].ob_y = savepxy[1];
			objc_draw(u_tree, BLOCK_DRAG, MAX_DEPTH, screen.g_x, screen.g_y, screen.g_w, screen.g_h);	/* Objekt neu zeichnen  */
		}

		graf_mouse(M_ON, NULL);

		imageWindow.displayCoords(picwindow, picture->blockx, picture->blocky, 1);
#if 0
		Goto_pos(1, 0);
		printf("blockx: %d, blocky: %d\n", picture->blockx, picture->blocky);
		printf("blockw: %d, blockh: %d", picture->blockwidth, picture->blockheight);
#endif
		if (wind_s[WIND_BTYPEIN].whandlem != -1)
			insert_blockcoords(picture);

		/*
		 * Mausbewegung abwarten
		 */
		do
		{
			graf_mkstate(&move_x, &move_y, &mbutton, &dummy);

			if ((move_x < windx && picwindow->xoffset > 0) ||
				(move_y < windy && picwindow->yoffset > 0) ||
				(move_x > windx2 && picwindow->xoffset < picture->pic_width - ow) ||
				(move_y > windy2 && picwindow->yoffset < picture->pic_height - oh + TOOLBAR_HEIGHT))
				break;
		} while (move_x == omx && move_y == omy && mbutton != 0);

		if (!mbutton)
			hmode = vmode = 0;

		mouse_wnum = Window.myWindow(wind_find(move_x, move_y));
		if (mouse_wnum > 0)
			mouse_window = &wind_s[mouse_wnum];
		else if (mouse_wnum < 0)
			mouse_window = &picture_windows[mouse_wnum];
		else
			mouse_window = NULL;

		if (mouse_window == picwindow)
			mouse_window = NULL;

		omx = move_x;
		omy = move_y;

		/*
		 * ggfs. Bildscrolling festlegen
		 */
		window_redraw = 0;

		if (move_x > windx2 && move_x < windx2 + 20)
		{
			scroll = move_x - windx2;
			if (picwindow->xoffset < picture->pic_width - ow)
			{
				window_redraw = WA_RTLINE;
				move_x = windx2;
			}
		} else if (move_x < windx && move_x > windx - 20)
		{
			scroll = windx - move_x;
			if (picwindow->xoffset > 0)
			{
				window_redraw = WA_LFLINE;
				move_x = windx;
			}
		}
		if (move_y > windy2 && move_y < windy2 + 20)
		{
			scroll = move_y - windy2;
			if (picwindow->yoffset < picture->pic_height - oh + TOOLBAR_HEIGHT)
			{
				window_redraw = WA_DNLINE;
				move_y = windy2;
			}
		} else if (move_y < windy && move_y > windy - 20)
		{
			scroll = windy - move_y;
			if (picwindow->yoffset > 0)
			{
				window_redraw = WA_UPLINE;
				move_y = windy;
			}
		}

		newx = picture->blockx;
		newy = picture->blocky;
		newwid = picture->blockwidth;
		newhgt = picture->blockheight;

		/*
		 * neue Blockbox-Koordinaten und -Gr”že festlegen
		 */
		if (hmode == LEFT)
		{
			newx = (move_x + picwindow->xoffset - windx) * zoom;
			newwid = xoff - newx;

			if (newwid < 0)
			{
				hmode = RIGHT;
				temp = xoff;
				xoff = newx;
				newx = temp;
				newwid = -newwid;
			}
		} else if (hmode == RIGHT)
		{
			xoff = (move_x + picwindow->xoffset - windx) * zoom;
			newwid = xoff - newx;

			if (newwid < 0)
			{
				hmode = LEFT;
				temp = xoff;
				xoff = newx;
				newx = temp;
				newwid = -newwid;
			}
		}

		if (vmode == TOP)
		{
			newy = (move_y + picwindow->yoffset - windy) * zoom;
			newhgt = yoff - newy;

			if (newhgt < 0)
			{
				vmode = BOTTOM;
				temp = yoff;
				yoff = newy;
				newy = temp;
				newhgt = -newhgt;
			}
		} else if (vmode == BOTTOM)
		{
			yoff = (move_y + picwindow->yoffset - windy) * zoom;
			newhgt = yoff - newy;

			if (newhgt < 0)
			{
				vmode = TOP;
				temp = yoff;
				yoff = newy;
				newy = temp;
				newhgt = -newhgt;
			}
		}

		if (hmode == MOVE && vmode == MOVE)
		{
			newx = (move_x - movxdif + picwindow->xoffset - windx) * zoom;
			newy = (move_y - movydif + picwindow->yoffset - windy) * zoom;
		} else
		{
			if (hmode)
				newwid++;
			if (vmode)
				newhgt++;
		}

		/*
		 * Blockbox/Blockicon wieder l”schen 
		 */
		graf_mouse(M_OFF, NULL);

		if (display_mode == 0)
		{
			vs_clip(Sys_info.vdi_handle, TRUE, clip);
			wind_update(BEG_UPDATE);
			imageWindow.drawBlockbox(picwindow);	/* l”schen */
			wind_update(END_UPDATE);
		} else
		{
			vs_clip(Sys_info.vdi_handle, TRUE, iconclip);
			savepxy[0] = 0;
			savepxy[1] = 0;
			savepxy[2] = 31;
			savepxy[3] = 31;
			savepxy[4] = old_iconxpos;
			savepxy[5] = old_iconypos;
			savepxy[6] = old_iconxpos + 31;
			savepxy[7] = old_iconypos + 31;

			vro_cpyfm(Sys_info.vdi_handle, S_ONLY, savepxy, &buffer, &screenm);	/* Hintergrund restoren */
		}

		graf_mouse(M_ON, NULL);

		/*
		 * umschalten zwischen Blockbox und Block-Icon?
		 */
		display_mode = 0;

		if (picwindow->picture->block != NULL)
		{
			if ((omy < windy - 20 || omy > windy2 + 20 || omx < windx - 20 || omx > windx2 + 20))
				display_mode = 1;
		}

		/*
		 * ggfs. Window scrollen und neue Blockbox-Koordinaten in die Bildstruktur
		 */
		if (display_mode == 0)
		{
			if (window_redraw)
			{
				oldwidth = picture->blockwidth;
				oldheight = picture->blockheight;
				picture->blockwidth = 0;
				picture->blockheight = 0;
				imageWindow.arrowWindow(window_redraw, picwindow, scroll);
				picture->blockwidth = oldwidth;
				picture->blockheight = oldheight;
			}

			picture->blockx = newx;
			picture->blocky = newy;
			picture->blockwidth = newwid;
			picture->blockheight = newhgt;
		}
	} while (mbutton != 0);

	wind_update(END_MCTRL);


	/*----- H”he und Breite korrigieren ---*/
	if (picture->blockwidth < 0)
	{
		picture->blockx += picture->blockwidth;
		picture->blockwidth = -picture->blockwidth;
	}
	if (picture->blockheight < 0)
	{
		picture->blocky += picture->blockheight;
		picture->blockheight = -picture->blockheight;
	}

	SMfree(buffer.fd_addr);

	if (display_mode == 1)
	{
		picwindow->picture->blockx = old_xpos;
		picwindow->picture->blocky = old_ypos;
		picwindow->picture->blockwidth = old_wid;
		picwindow->picture->blockheight = old_hgt;
		drop_block(picwindow, omx, omy);
	}


	/*
	 * alten Block & Box l”schen und neu zeichnen
	 * das ist ein wenig kompliziert, da ich nur die Bereiche
	 * neu zeichnen will, bei denen das auch wirklich n”tig ist (realtimedither!).
	 * Das ganze muž nur sein, wenn bereits ein Block in dem Bild war, da die
	 * Blockbox sowieso gel”scht wird.
	 */
	if (oldblock)
	{
		Dialog.busy.disable();

		/*
		 * Block wurde verschoben - umliegende Rechtecke neu zeichnen
		 */
		if (!new_block)
		{
			done = FALSE;

			/*
			 * rechtes/linkes Rechteck
			 */
			if (picture->blockx < old_xpos)
			{
				if (picture->blockx + picture->blockwidth > old_xpos)
				{
					redraw.g_x = (picture->blockx + picture->blockwidth) / zoom + windx - picwindow->xoffset;
					redraw.g_w = abs((old_xpos + old_wid) - (picture->blockx + picture->blockwidth)) / zoom;
				} else
				{
					redraw.g_x = old_xpos / zoom + windx - picwindow->xoffset;
					redraw.g_w = old_wid / zoom;
					done = TRUE;
				}
			} else if (picture->blockx > old_xpos)
			{
				redraw.g_x = old_xpos / zoom + windx - picwindow->xoffset;

				if (picture->blockx < old_xpos + old_wid)
					redraw.g_w = abs(old_xpos - picture->blockx) / zoom;
				else
				{
					redraw.g_w = old_wid / zoom;
					done = TRUE;
				}
			}

			redraw.g_y = old_ypos / zoom + windy - picwindow->yoffset;
			redraw.g_h = old_hgt / zoom;

			if (redraw.g_w > 0 && redraw.g_h > 0)
				Window.redraw(picwindow, &redraw, 0, NOBLOCKBOX | DRAWNOBLOCK);	/* Bildrechteck neu zeichnen */

			/*
			 * oberes/unteres Rechteck
			 */
			if (done == FALSE)
			{
				if (picture->blocky < old_ypos)
					redraw.g_y = (picture->blocky + picture->blockheight) / zoom + windy - picwindow->yoffset;
				else if (picture->blocky > old_ypos)
					redraw.g_y = old_ypos / zoom + windy - picwindow->yoffset;

				if (picture->blockx > old_xpos)
				{
					redraw.g_x = picture->blockx / zoom + windx - picwindow->xoffset;
					redraw.g_w = (old_xpos + old_wid - picture->blockx) / zoom;
				} else
				{
					redraw.g_x = old_xpos / zoom + windx - picwindow->xoffset;
					redraw.g_w = (picture->blockx + picture->blockwidth - old_xpos) / zoom;
				}

				redraw.g_h = abs(old_ypos - picture->blocky) / zoom;

				Window.redraw(picwindow, &redraw, 0, NOBLOCKBOX | DRAWNOBLOCK);	/* zeichnen */
			}

		}

		/*
		 * neuer Block wurde aufgezogen - alten Blockbereich im
		 * Bild neuzeichnen!
		 */
		else
		{
			redraw.g_x = old_xpos - picwindow->xoffset + windx;
			redraw.g_y = old_ypos - picwindow->yoffset + windy;
			redraw.g_w = old_wid;
			redraw.g_h = old_hgt;
			if (redraw.g_w > 0 && redraw.g_h > 0)
				Window.redraw(picwindow, &redraw, 0, 0);	/* Block&Box neu zeichnen */
		}

		Dialog.busy.enable();
	}

	/*
	 * monochromer Block wurde im weiž/schwarz-transparent - Modus verschoben 
	 * und Realtime-Dither ist an -> neuer Blockbereich muž im Bild neugezeichnet werden
	 */
	if (display_mode == 0 && picture->block != NULL)
		if ((Sys_info.realtime_dither) && !new_block && picture->block->depth == 1 && blockmode_conf.transparent != 0)
		{
			Dialog.busy.disable();
			redraw.g_x = picture->blockx - picwindow->xoffset + windx;
			redraw.g_y = picture->blocky - picwindow->yoffset + windy;
			redraw.g_w = picture->blockwidth;
			redraw.g_h = picture->blockheight;
			Window.redraw(picwindow, &redraw, 0, DRAWNOTREE | DRAWNOBLOCK | NOBLOCKBOX);
			Dialog.busy.enable();
		}

	Dialog.busy.disable();
	Window.redraw(picwindow, NULL, 0, DRAWNOTREE | BLOCK_ONLY);	/* Block&Box neu zeichnen */
	Dialog.busy.enable();

	if (wind_s[WIND_BTYPEIN].whandlem != -1)
		insert_blockcoords(picture);

	blockpopup[BLOCK_RELEASE].ob_state &= ~OS_DISABLED;
	blockpopup[BLOCK_FREE].ob_state &= ~OS_DISABLED;

	if (picwindow->xoffset != merkxoffset || picwindow->yoffset != merkyoffset)
		Window.redraw(Dialog.picMan.window, NULL, PICMAN_PREVIEW, 0);
}


/* ----------------------------------------------------------------	*/
/*				Auswertung eines Events im Bildfenster				*/
/* ----------------------------------------------------------------	*/
void f_pic_event(WINDOW *picwindow, short event_type, short windnum)
{
	WORD backbutton;
	short back;
	WORD dummy;
	WORD mstate;
	WORD w_x, w_y, w_w, w_h;
	short oldzoom;
	WORD hmode;
	WORD vmode;
	BOOLEAN out_block;
	BOOLEAN chmouse;
	SMURF_PIC *picture;
	OBJECT *pic_form;

	static WORD oldwindnum;
	static WORD outpos;
	static WINDOW *oldwindow;

	picture = picwindow->picture;
	pic_form = picwindow->resource_form;

	Window.windGet(picwindow->whandlem, WF_WORKXYWH, &w_x, &w_y, &w_w, &w_h);

	obj = -1;

	if (windnum != oldwindnum)
	{
		outpos = 0;
		imageWindow.nullCoords(oldwindow);
	}

	/*--------- Mauszeiger im Bild? */
	if (mouse_xpos >= w_x && mouse_xpos < w_x + w_w && mouse_ypos >= w_y + TOOLBAR_HEIGHT && mouse_ypos < w_y + w_h &&
		mouse_ypos < w_y + (picture->pic_height / (picture->zoom + 1)) + TOOLBAR_HEIGHT &&
		mouse_xpos < w_x + (picture->pic_width / (picture->zoom + 1)))
	{
		if (event_type == MU_TIMER)
		{
			if ((windnum == oldwindnum && picwindow->picture->own_pal && syspalset) || windnum != oldwindnum)
			{
				if (Display_Opt.palette_mode == PAL_MOUSE)
				{
					f_set_picpal(picwindow->picture);
					if (Sys_info.OS & MAG_PC)
						Window.redraw(picwindow, NULL, 0, DRAWNOTREE);
				}
			}

			if (lastmousex != mouse_xpos || lastmousey != mouse_ypos)
				imageWindow.displayCoords(picwindow, mouse_xpos, mouse_ypos, 0);

			chmouse = crosshair_mouse(picwindow, mouse_xpos, mouse_ypos);

			if (!chmouse)
			{
				hmode = vmode = 0;
				out_block = mouse_block_position(picwindow, mouse_xpos, mouse_ypos, &hmode, &vmode);
				if (out_block)
				{
					graf_mouse(OUTLN_CROSS, NULL);
				} else
				{
					if (hmode && !vmode)
						graf_mouse(USER_DEF, lr_arrow);
					else if (!hmode && vmode)
						graf_mouse(USER_DEF, ud_arrow);
					else if (hmode && vmode)
						graf_mouse(USER_DEF, lrud_arrow);
					else
						graf_mouse(FLAT_HAND, NULL);
				}
			} else
			{
				graf_mouse(THIN_CROSS, NULL);
			}
		} else if (event_type == MU_BUTTON)
		{
			if (mouse_button == 0x1 &&
				mouse_ypos < w_y + (picture->pic_height / (picture->zoom + 1)) + TOOLBAR_HEIGHT &&
				mouse_xpos < w_x + (picture->pic_width / (picture->zoom + 1)))
			{
				graf_mkstate(&dummy, &dummy, &mstate, &dummy);

				if (klicks == 1)
				{
					if (mstate == 0)
					{
						if (&picture_windows[active_pic] != picwindow)
						{
#if 0
							/* Testweise raus da anscheinend berhaupt nicht ben”tigt */
							Dialog.picMan.makeThumbnail(picwindow->wnum);
							obj = 0;
							Dialog.picMan.handlePicman();
#endif
							Window.top(picwindow->whandlem);
						}
					} else
					{
						if (crosshair_mouse(picwindow, mouse_xpos, mouse_ypos))
						{
							move_crosshair(picwindow);
						} else
						{
							do_block_box(picwindow, mouse_xpos, mouse_ypos);
							if (picwindow->picture->blockwidth && picwindow->picture->blockheight)
								blockfunctions_on();
						}
					}
				} else if (klicks == 2)
				{
					out_block = mouse_block_position(picwindow, mouse_xpos, mouse_ypos, &hmode, &vmode);
					if (out_block == FALSE)
						block_dklick(picwindow);
				}
			} else if (mouse_button == 0x2)
			{
				f_activate_pic(picwindow->wnum);
				openmode = 0;
				popups[POPUP_PIC].item = 0;

				out_block = mouse_block_position(picwindow, mouse_xpos, mouse_ypos, &hmode, &vmode);

				/* OutBlock + kein Shift oder InBlock und Shift */
				if ((out_block != FALSE && !(key_at_event & KEY_SHIFT)) || (out_block == FALSE && (key_at_event & KEY_SHIFT)))
					pic_popup(picwindow);

				/* OutBlock + Shift oder InBlock und kein Shift */
				else if ((out_block == FALSE && !(key_at_event & KEY_SHIFT)) ||
						 (out_block != FALSE && (key_at_event & KEY_SHIFT)))
					imageWindow.blockPop(picwindow);
			}
		}

		outpos = 0;
	}
	/*--------- Mauszeiger aužerhalb des Bildes */
	else
	{
		if (!outpos)
		{
			graf_mouse(ARROW, NULL);
			imageWindow.nullCoords(picwindow);

			if (!syspalset)
				if (Display_Opt.palette_mode == PAL_MOUSE)
					f_set_syspal();

			outpos = 1;
		}

		if (!(event_type & MU_TIMER))
		{
			if (mouse_button == 0x02)
			{
				if (mouse_ypos < w_y + TOOLBAR_HEIGHT)
					Comm.bubbleGem(-(picwindow->wnum), mouse_xpos, mouse_ypos, 0);
				else
					pic_popup(picwindow);
			} else
			{
				f_activate_pic(picwindow->wnum);

				pic_form->ob_x = w_x;
				pic_form->ob_y = w_y;
				backbutton = objc_find(pic_form, 0, MAX_DEPTH, mouse_xpos, mouse_ypos);
				if (backbutton == ZOOM_FACTOR)
				{
					back = f_pop(&popups[POPUP_ZOOM], 0, 0, NULL);
					oldzoom = picture->zoom;

					switch (back)
					{
					case DURCH2:
						picture->zoom = 1;
						break;
					case DURCH3:
						picture->zoom = 3;
						break;
					case DURCH4:
						picture->zoom = 9;
						break;
					case DURCH5:
						picture->zoom = 19;
						break;
					case DURCH6:
						picture->zoom = 29;
						break;
					case ZOOM1:
						picture->zoom = 0;
						break;
					}

					if (oldzoom != picture->zoom)
					{
						if (!Sys_info.realtime_dither)
							f_dither(picture, &Sys_info, 1, NULL, &Display_Opt);

						picwindow->clipwid = (picture->pic_width) / (picture->zoom + 1);
						picwindow->cliphgt = (picture->pic_height) / (picture->zoom + 1);

						if (picture->block)
						{
							picture->block->zoom = picture->zoom;
							if (!Sys_info.realtime_dither)
								f_dither(picture->block, &Sys_info, 1, NULL, &Display_Opt);
						}

						imageWindow.clipPicwin(picwindow);
						Window.redraw(picwindow, NULL, 0, 0);
						wind_s[WIND_MODFORM].xoffset = 0;
						wind_s[WIND_MODFORM].yoffset = 0;
						Window.redraw(&wind_s[WIND_MODFORM], NULL, 0, 0);
						imageWindow.setSliders(picwindow);
						Dialog.busy.ok();
					}
				} else if (backbutton == PICWIND_INFO)
				{
					openmode = 0;
					f_pic_info();
				}
			}
		}
	}

	oldwindow = picwindow;
	oldwindnum = windnum;
}


/* ----------------------------------------------------------------	*/
/*							Block-Popup								*/
/* ----------------------------------------------------------------	*/
void f_block_popup(WINDOW *picwindow)
{
	WORD back;
	SMURF_PIC *picture;

	picture = picwindow->picture;

	/*------- ist berhaupt ein Block aufgezogen und ein Clipboardpfad vorhanden? */
	if (picture->blockheight == 0 || picture->blockwidth == 0 || Sys_info.scrp_path == NULL)
		blockfunctions_off();
	else
		blockfunctions_on();

	back = f_pop(&popups[POPUP_BLOCK], 1, 0, NULL);

	switch (back)
	{
	case BLOCK_RELEASE:
		imageWindow.removeBlock(picwindow);
		blockfunctions_off();
		if (wind_s[WIND_BTYPEIN].whandlem != -1)
			insert_blockcoords(picture);
		break;

	case BLOCK_FREE:
		block_freistellen(picwindow);
		if (wind_s[WIND_BTYPEIN].whandlem != -1)
			insert_blockcoords(picture);
		break;

	case BLOCK_TYPEIN:
		block_type_in();
		break;

	case BLOCK_COPY:
		block2clip(picture, 0, NULL);
		break;

	case BLOCK_INSERT:
		clip2block(picture, NULL, -1, -1);
		Window.redraw(picwindow, NULL, 0, 0);
		break;

	case BLOCK_CUT:
		if (block2clip(picture, 1, NULL) != -1)
		{
			imageWindow.toggleAsterisk(picwindow, 1);
			f_dither(picture, &Sys_info, 1, NULL, &Display_Opt);
			Window.redraw(picwindow, NULL, 0, 0);
		}
		break;

	case BLOCK_MODE:
		blockmode();
		break;
	}
}


/*	void set_nullcoord --------------------------------------------------------	
	Resetted die Anzeigen in der Bild-Toolbar. Wird z.B. aufgerufen, wenn ein
	Bildfenster in den Hintergrund gelegt wird.
	---------------------------------------------------------------------------	*/
void set_nullcoord(WINDOW *picwindow)
{
	OBJECT *pic_formular;


	if (picwindow && picwindow->whandlem != -1)
	{
		pic_formular = picwindow->resource_form;
		strcpy(pic_formular[X_COORD].TextCast, "");
		strcpy(pic_formular[Y_COORD].TextCast, "");

		if (picwindow->picture->blockwidth == 0 || picwindow->picture->blockheight == 0)
		{
			strcpy(pic_formular[PW_WID].TextCast, "");
			strcpy(pic_formular[PW_HGT].TextCast, "");
		}

		strcpy(pic_formular[RED_VAL].TextCast, "");
		strcpy(pic_formular[GREEN_VAL].TextCast, "");
		strcpy(pic_formular[BLUE_VAL].TextCast, "");
		strcpy(pic_formular[COL_IDX].TextCast, "");
		Window.redraw(picwindow, NULL, COO_REDRAW, DRAWNOPICTURE);	/* Koordinatenanzeige redrawen */
		Window.redraw(picwindow, NULL, COL_REDRAW, DRAWNOPICTURE);	/* Farbanzeige redrawen */
		lastmousex = lastmousey = -1;
	}

	nullcoordset = TRUE;
}


/* f_display_coords --------------------------------------------------------------
	Aktualisiert die Koordinaten- und Farbanzeigen im Bildfenster pic_window bei den
	Mauskoordinaten mx/my.
	-------------------------------------------------------------------------------*/
void f_display_coords(WINDOW *pic_window, WORD mx, WORD my, BOOLEAN blockflag)
{
	uint8_t red, green, blue;
	uint8_t index = 0;
	char numstr[5];
	uint8_t *pic;
	uint8_t *pal;
	uint16_t *pic16;
	uint16_t pixel16;
	WORD wx, wy, ww, wh;
	WORD maxx, maxy, minx, miny;
	WORD xcoo, ycoo, xoff, yoff;
	short picval;
	short depth;
	short form;
	WORD width, height;
	short zoom;
	WORD byte_width;
	WORD byte_xpos;
	long planelen;
	long pic_offset;

	OBJECT *pic_formular;
	SMURF_PIC *spic;


	spic = pic_window->picture;
	xoff = pic_window->xoffset;
	yoff = pic_window->yoffset;
	zoom = pic_window->picture->zoom + 1;

	lastmousex = mx;
	lastmousey = my;

	/*
	 * Fenster-Arbeitsfl„che ermitteln
	 */
	Window.windGet(pic_window->whandlem, WF_WORKXYWH, &wx, &wy, &ww, &wh);

	minx = wx;
	maxx = minx + (spic->pic_width / (spic->zoom + 1)) - xoff - 1;	/* - 1 weil die Endkoordinate und nicht die Breite gefragt ist */
	miny = wy + TOOLBAR_HEIGHT;
	maxy = miny + (spic->pic_height / (spic->zoom + 1)) - yoff - 1;	/* - 1 weil die Endkoordinate und nicht die H”he gefragt ist */

	if (!pic_window->shaded && (blockflag || (mx >= minx && mx <= maxx && my >= miny && my <= maxy)))
	{
		if (!blockflag)
		{
			pic = pic_window->picture->pic_data;
			pic16 = (uint16_t *) pic;
			width = pic_window->picture->pic_width;
			height = pic_window->picture->pic_height;
			depth = pic_window->picture->depth;
			form = pic_window->picture->format_type;

			xcoo = (mx - wx + xoff) * zoom;	/* Bildkoordinaten berechnen */
			ycoo = ((my - wy + yoff) - TOOLBAR_HEIGHT) * zoom;

			/*  Farbwerte holen */
			if (form == FORM_PIXELPAK)
			{
				switch (depth)
				{
				case 24:
					pic_offset = ((long) ycoo * (long) (width + width + width)) + (long) (xcoo + xcoo + xcoo);
					red = *(pic + pic_offset);
					green = *(pic + pic_offset + 1);
					blue = *(pic + pic_offset + 2);
					break;

				case 16:
					pic_offset = ((long) ycoo * (long) width) + (long) xcoo;
					pixel16 = *(pic16 + pic_offset);
					red = pixel16 >> 11;
					green = (pixel16 >> 5) & 0x3f;
					blue = pixel16 & 0x1f;
					break;

				case 8:
					pal = pic_window->picture->palette;
					pic_offset = (((long) ycoo * (long) (width)) + (long) xcoo);
					picval = *(pic + pic_offset);
					index = picval;
					picval *= 3;
					red = *(pal + picval);
					green = *(pal + picval + 1);
					blue = *(pal + picval + 2);
					break;
				default:
					return;
				}
			} else
			{
				byte_width = (width + 7) >> 3;
				byte_xpos = xcoo >> 3;
				planelen = byte_width * height;
				pic_offset = ((long) ycoo * (long) byte_width) + byte_xpos;
				getpix_std_1(pic + pic_offset, &picval, depth, planelen, xcoo - (byte_xpos << 3));

				pal = pic_window->picture->palette;
				index = picval;
				picval *= 3;
				red = *(pal + picval);
				green = *(pal + picval + 1);
				blue = *(pal + picval + 2);
			}
		} else
		{
			xcoo = mx;
			ycoo = my;
			depth = 0;
			red = green = blue = 0;
		}

		pic_formular = pic_window->resource_form;

		itoa(xcoo, numstr, 10);
		strcpy(pic_formular[X_COORD].TextCast, numstr);
		itoa(ycoo, numstr, 10);
		strcpy(pic_formular[Y_COORD].TextCast, numstr);

		Window.redraw(pic_window, NULL, COO_REDRAW, DRAWNOPICTURE);	/* Koordinatenanzeige redrawen */

		f_display_bwh(pic_window);

		if (!blockflag && (mx >= minx && mx <= maxx && my >= miny && my <= maxy))
		{
			itoa(red, numstr, 10);
			strcpy(pic_formular[RED_VAL].TextCast, numstr);
			itoa(green, numstr, 10);
			strcpy(pic_formular[GREEN_VAL].TextCast, numstr);
			itoa(blue, numstr, 10);
			strcpy(pic_formular[BLUE_VAL].TextCast, numstr);

			if (depth < 16)
			{
				itoa(index, numstr, 10);
				strcpy(pic_formular[COL_IDX].TextCast, numstr);
			} else
			{
				strcpy(pic_formular[COL_IDX].TextCast, "");
			}
		} else
		{
			strcpy(pic_formular[COL_IDX].TextCast, "");
			strcpy(pic_formular[RED_VAL].TextCast, "");
			strcpy(pic_formular[GREEN_VAL].TextCast, "");
			strcpy(pic_formular[BLUE_VAL].TextCast, "");
		}

		Window.redraw(pic_window, NULL, COL_REDRAW, DRAWNOPICTURE);	/* Farbanzeige redrawen */
	}

	nullcoordset = FALSE;
}



/* display_bwh -----------------------------------------------------
	Stellt Blockbreite und -h”he im Bildfenster pic_window neu dar.
	-----------------------------------------------------------------*/
void f_display_bwh(WINDOW *pic_window)
{
	char numstr[10];
	WORD bw, bh;
	static WORD obw, obh;
	OBJECT *pic_formular;

	pic_formular = pic_window->resource_form;

	bw = pic_window->picture->blockwidth;
	bh = pic_window->picture->blockheight;

	/*
	 * Koordinaten ge„ndert oder Breite im Formular
	 * durch andere Routine gel”scht? 
	 */
	if (bw != obw || bh != obh)
	{
		if (bw && bh)
		{
			itoa(bw, numstr, 10);
			strcpy(pic_formular[PW_WID].TextCast, numstr);
			itoa(bh, numstr, 10);
			strcpy(pic_formular[PW_HGT].TextCast, numstr);
		} else
		{
			strcpy(pic_formular[PW_WID].TextCast, "");
			strcpy(pic_formular[PW_HGT].TextCast, "");
		}

		Window.redraw(pic_window, NULL, BWH_REDRAW, DRAWNOPICTURE);	/* Breiten-/H”hen-Anzeige redrawen */

		obw = bw;
		obh = bh;
	}
}


/* void	reload_pic --------------------------------------------------------
	Bild berladen - verschickt zum Neuladen an Smurf selbst eine VA_START-
	Message mit dem Dateinamen.
	-----------------------------------------------------------------------	*/
void reload_pic(WINDOW *picwindow)
{
	WORD back;

	/*
	 * gibt es die Datei berhaupt (noch)?
	 */
	if (Fattrib(picwindow->picture->filename, 0, 0) < 0)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[FILEOPEN_ERR].TextCast, NULL, NULL, NULL, 1);
		return;
	}

	if (picwindow->wtitle[11] == '*')
	{
		if (Sys_info.profi_mode & OS_SELECTED)
			imageWindow.toggleAsterisk(picwindow, 0);
		else
		{
			back = Dialog.winAlert.openAlert(Dialog.winAlert.alerts[WCLOSE_ALERT].TextCast, "Abbruch", "Nein", " Ja ", 1);
			if (back == 1)
				return;
			else if (back == 2)
				goto Reload;
			else
				imageWindow.toggleAsterisk(picwindow, 0);
		}
	}

	/*
	 * Bildfenster schliežen
	 */
	Dialog.busy.noEvents = TRUE;
	strcpy(loadpath, picwindow->picture->filename);	/* Filename retten */
	Window.close(picwindow->whandlem);
#if 0
	Comm.avComm.type = AV_IMAGE | 0xf0;
	Comm.avComm.windowhandle = picwindow->whandlem;
	Comm.avComm.keystate = 0;
#endif
  Reload:

	/*
	 * ...und dann eine VA_START-Message mit dem Bildpfad
	 */
	Comm.sendAESMsg(Sys_info.app_id, VA_START, LONG2_2INT(loadpath), -1);
}



/* f_activate_pic -------------------------------------------------------------------------
	Aktiviert das Bild mit der ID windnum.
	Der PD wird neu gezeichnet und seine Bildstruktur angepažt, wenn kein Bildfenster 
	mehr da ist, wird active_pic auf -1 gesetzt. Aužerdem wird der Bildmanager-Thumbnail
	und alle Modulpreviews aktualisiert, alle Fadenkreuze im vormals aktiven Bild umgeh„ngt,
	die Blockfunktionen en- oder disabled und alle Module ber die Aktivierung des neuen
	Bildes informiert.
	----------------------------------------------------------------------------------------*/
void f_activate_pic(short windnum)
{
	short t;
	short old_active;

	OBJECT *pref_form;
	DISPLAY_MODES old;

	if (active_pic == windnum)			/* ist das Bild schon das aktive? */
		return;

	old_active = active_pic;
	active_pic = windnum;
	pref_form = wind_s[WIND_MODFORM].resource_form;

	wind_s[WIND_MODFORM].xoffset = 0;
	wind_s[WIND_MODFORM].yoffset = 0;

	if (windnum != -1)
	{
		Window.redraw(&picture_windows[old_active], NULL, 0, DRAWNOPICTURE);
		Window.redraw(&picture_windows[active_pic], NULL, 0, DRAWNOPICTURE);

		wind_s[WIND_MODFORM].pic_xpos = pref_form[PREV_OUTER].ob_x + pref_form[PREV_BOX].ob_x;
		wind_s[WIND_MODFORM].pic_ypos = pref_form[PREV_OUTER].ob_y + pref_form[PREV_BOX].ob_y;
		wind_s[WIND_MODFORM].clipwid = pref_form[PREV_BOX].ob_width;
		wind_s[WIND_MODFORM].cliphgt = pref_form[PREV_BOX].ob_height;

		if (pref_form[PREV_BOX].ob_width > smurf_picture[active_pic]->pic_width / prev_zoom)
			wind_s[WIND_MODFORM].clipwid = smurf_picture[active_pic]->pic_width / prev_zoom;
		if (pref_form[PREV_BOX].ob_height > smurf_picture[active_pic]->pic_height / prev_zoom)
			wind_s[WIND_MODFORM].cliphgt = smurf_picture[active_pic]->pic_height / prev_zoom;

		memcpy(&move_prev, smurf_picture[active_pic], sizeof(SMURF_PIC));
		move_prev.local_nct = NULL;
		move_prev.screen_pic = NULL;
		move_prev.zoom = prev_zoom - 1;
		wind_s[WIND_MODFORM].picture = &move_prev;

		openmode = 2;
		f_pic_info();
	} else
	{
		wind_s[WIND_MODFORM].picture = NULL;
		if (wind_s[WIND_PICINFO].whandlem > 0)
			Dialog.close(WIND_PICINFO);
	}

	make_singular_display(&old, Sys_info.PreviewMoveDither, CR_SYSPAL);

	/* Flag 0 damit das Preview geleert wird wenn das letzte Bild gechlossen wird */
	Window.redraw(&wind_s[WIND_MODFORM], NULL, PREV_OUTER, 0);
	restore_display(&old);

	if (wind_s[FORM_EXPORT].whandlem > 0)
	{
		openmode = 2;
		obj = 0;
		f_export_formular();
		Window.redraw(&wind_s[FORM_EXPORT], NULL, 0, 0);
	}

	if (wind_s[WIND_TRANSFORM].whandlem > 0)
	{
		openmode = 2;
		obj = 0;
		transform_pic();
	}

	if (wind_s[WIND_BTYPEIN].whandlem > 0)
	{
		openmode = 2;
		obj = 0;
		block_type_in();
	}

	/*
	 * Modul mit Fadenkreuz im aktiven Bild suchen, Fadenkreuz umh„ngen
	 * und alle Modulpreviews aktualisieren
	 */
	if (active_pic != old_active && windnum != -1)
	{
		for (t = 0; t < 20; t++)
		{
			/*
			 * Fadenkreuze ...
			 */
			if (position_markers[t].smurfpic[0] == old_active)
			{
				if (position_markers[t].mod_pic[0] == -2)
				{
					position_markers[t].smurfpic[0] = active_pic;
					if (position_markers[t].xpos[0] > picture_windows[active_pic].picture->pic_width)
						position_markers[t].xpos[0] = picture_windows[active_pic].picture->pic_width;
					if (position_markers[t].ypos[0] > picture_windows[active_pic].picture->pic_height)
						position_markers[t].ypos[0] = picture_windows[active_pic].picture->pic_height;
					Window.redraw(&picture_windows[old_active], NULL, 0, DRAWNOTREE);
					Window.redraw(&picture_windows[active_pic], NULL, 0, DRAWNOTREE);
				}
			}

			/*
			 * ... und Previews aktualisieren
			 * Kann momentan nur gemacht werden, wenn das alte aktive Bild noch ge”ffnet
			 * ist. Beim Schliežen von Bildern werden die Previews also noch nicht aktualisiert!
			 */
			if (module.smStruct[t] && module.smStruct[t]->wind_struct && module.smStruct[t]->wind_struct->picture &&
				smurf_picture[old_active]
				&& module.smStruct[t]->wind_struct->picture->pic_data == smurf_picture[old_active]->pic_data)
			{
				module.smStruct[t]->wind_struct->picture = smurf_picture[active_pic];
				Window.redraw(module.smStruct[t]->wind_struct, NULL, 0, 0);
			}
		}
	}

	/*
	 * Je nachdem, ob sich in dem aktiven Bild ein Block befindet, die
	 * Blockfunktionen ein- oder ausschalten.
	 */
	if (picture_windows[windnum].whandlem > 0 && picture_windows[windnum].picture != NULL)
	{
		if (picture_windows[windnum].picture->blockwidth == 0 && picture_windows[windnum].picture->blockheight == 0)
			blockfunctions_off();
		else
			blockfunctions_on();
	}

	/*
	 * Bildmanager-Thumbnail updaten und alle Module ber die Bildaktivierung informieren
	 */
	if (windnum != Dialog.picMan.selectedPic && windnum != -1)
	{
#if 0
		Dialog.picMan.handlePicman();
#endif
		Dialog.picMan.makeThumbnail(windnum);
		Dialog.picMan.selectedPic = windnum;
	}

	inform_modules(MPIC_UPDATE, smurf_picture[active_pic]);
}


/* picwin_keyboard -------------------------------------------------------------
	Handled das Scrollen in Bildfenstern, inklusive Zusammenfassen oder l”schen
	der Keyboardevents (je nach Einstellung im Optionsdialog).
	-----------------------------------------------------------------------------*/
void picwin_keyboard(WORD key_scancode, WORD key_at_event, WINDOW *picwin)
{
	WORD keyboard_back = 0;
	WORD dummy;
	WORD evback;
	WORD amount;
	WORD innerwid, innerhgt;

	/*
	 * ggfs. Tastaturevents zusammenfassen
	 */
	if (Sys_info.keyevents == KBEV_JOIN)
	{
		amount = 0;
		do
		{
			evback = evnt_multi(MU_KEYBD | MU_TIMER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								messagebuf, EVNT_TIME(0), &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
			amount++;
		} while (evback & MU_KEYBD);

		amount *= 8;
	} else
	{
		amount = 8;
	}

	if (key_at_event == 0x001 || key_at_event == 0x002)	/* mit Shift */
	{
		switch (key_scancode >> 8)
		{
		case 72:
			keyboard_back = WA_UPPAGE;
			break;
		case 75:
			keyboard_back = WA_LFPAGE;
			break;
		case 77:
			keyboard_back = WA_RTPAGE;
			break;
		case 80:
			keyboard_back = WA_DNPAGE;
			break;
		case KEY_HOME:
			Window.windGet(picwin->whandlem, WF_WORKXYWH, &dummy, &dummy, &innerwid, &innerhgt);
			picwin->xoffset = picwin->picture->pic_width - innerwid;
			picwin->yoffset = picwin->picture->pic_height - (innerhgt - TOOLBAR_HEIGHT);
			break;
		}
	} else
	{
		switch (key_scancode >> 8)
		{
		case 72:
			keyboard_back = WA_UPLINE;
			break;
		case 75:
			keyboard_back = WA_LFLINE;
			break;
		case 77:
			keyboard_back = WA_RTLINE;
			break;
		case 80:
			keyboard_back = WA_DNLINE;
			break;
		case KEY_HOME:
			picwin->xoffset = 0;
			picwin->yoffset = 0;
			break;
		}
	}

	if ((key_scancode >> 8) == 72 ||
		(key_scancode >> 8) == 75 ||
		(key_scancode >> 8) == 77 ||
		(key_scancode >> 8) == 80)
		imageWindow.arrowWindow(keyboard_back, picwin, amount);

	/*
	 * Tastaturbuffer gegen nachlaufen l”schen
	 */
	if (Sys_info.keyevents == KBEV_DELETE)
	{
		IOREC *iorec = Iorec(1);
		iorec->ibufhd = 0;
		iorec->ibuftl = 0;
	}
	Window.redraw(Dialog.picMan.window, NULL, PM_PREVBOX, 0);
}
