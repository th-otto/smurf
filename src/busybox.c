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
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "import.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "vaproto.h"

#include "globdefs.h"
#include "smurfobs.h"
#include "ext_obs.h"
#include "debug.h"




/* ---------------------------------------------------------------- */
/*					Status-Box setzen und Text rein				  	*/
/* ----------------------------------------------------------------	*/
void reset_busybox(short lft, const char *txt)
{
	char *busytxt;
	GRECT busy_form;
	OBJECT *busytree = Dialog.busy.busyTree;

	if (Dialog.busy.disabled)
		return;

	Dialog.busy.resetClock = clock();
	busytxt = busytree[BW_TXT].TextCast;

	busy_form.g_x = wind_s[WIND_BUSY].wx;
	busy_form.g_y = wind_s[WIND_BUSY].wy;
	busy_form.g_w = wind_s[WIND_BUSY].ww;
	busy_form.g_h = wind_s[WIND_BUSY].wh;
	busytree[BW_BOX].ob_x = busytree[BW_ICON].ob_x + lft;
	busytree[BW_BOX].ob_width = 128 - lft;
	strncpy(busytxt, txt, 20);
	busytxt[20] = '\0';					/* nullterminieren */

	Window.redraw(&wind_s[WIND_BUSY], &busy_form, 0, 0);
}


void ok_busybox(void)
{
	if (Dialog.busy.disabled)
		return;

	Dialog.busy.reset(128, "OK");
	timer_fx_max[1] = 0;
}


/* ----------------------------------------------------------------	*/
/*			Statusbox setzen und redrawen (incl. Eventabarbeitung)	*/
/* ---------------------------------------------------------------- */
short draw_busybox(short lft)
{
	WORD dummy;
	WORD ev_type;
	WORD keys;
	WORD wind_x, wind_y;
	OBJECT *busytree = Dialog.busy.busyTree;
	GRECT box;

	if (Dialog.busy.disabled || wind_s[WIND_BUSY].whandlem <= 0)
		return 0;

	if (!Startup && !Dialog.busy.noEvents)
	{
		ev_type = evnt_multi(MU_KEYBD | MU_MESAG | MU_TIMER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, messagebuf,
							 EVNT_TIME(1), &mouse_xpos, &mouse_ypos, &dummy, &keys, &dummy, &dummy);
		if (ev_type == MU_MESAG && messagebuf[0] != WM_CLOSED)
			f_handle_message();
	}

	Window.windGet(wind_s[WIND_BUSY].whandlem, WF_WORKXYWH, &wind_x, &wind_y, &dummy, &dummy);

	box.g_x = wind_x + busytree[BW_ICON].ob_x;
	box.g_y = wind_y + busytree[BW_ICON].ob_y;
	box.g_w = lft;
	box.g_h = busytree[BW_ICON].ob_height;
	Window.redraw(&wind_s[WIND_BUSY], &box, BW_ICON, 0);

	keys = (int) Kbshift(-1);
	if (keys == 0x003)
		return -1;					/* Abbruch (R+L - Shift)? */
	else
		return 0;
}


/* ---------------------------------------------------------------- */
/*              RAM-Anzeige in Statusbox aktualisieren              */
/* ---------------------------------------------------------------- */
void actualize_ram(void)
{
	char ram_str[20];
	WORD dummy;
	WORD wind_x, wind_y;
	long frei_mem;
	OBJECT *busytree = Dialog.busy.busyTree;
	GRECT box;

	DEBUG_MSG(("actualize_ram...\n"));

	if (Dialog.busy.disabled || wind_s[WIND_BUSY].whandlem <= 0)
		return;

	frei_mem = (long) Malloc(-1);
	frei_mem /= 1024L;					/* 1024 = Kilobyte */

	ltoa(frei_mem, ram_str, 10);
	strncpy(busytree[BW_MEMORY].TextCast, ram_str, 8);

	Window.windGet(wind_s[WIND_BUSY].whandlem, WF_WORKXYWH, &wind_x, &wind_y, &dummy, &dummy);
	box.g_x = wind_x + busytree[BW_MEMORY].ob_x;
	box.g_y = wind_y + busytree[BW_MEMORY].ob_y - 3;
	box.g_w = busytree[BW_MEMORY].ob_width + 16;
	box.g_h = busytree[BW_MEMORY].ob_height + 7;
	Window.redraw(&wind_s[WIND_BUSY], &box, 0, 0);

	DEBUG_MSG(("actualize_ram...Ende\n"));
}


/*---------------------------------------------------------------
	Leere Busyboxfunktionen
	------------------------------------------------------------*/


/* empty_bb --------------------------------------------------
	'leere' draw_busybox, wenn die Zeit vom letzten Reset bis zum
	Aufruf > 1s ist, wird die Busybox wieder angeschaltet (z.B. um
	bei langsamen Modulen das Preview beobachten zu k�nnen.
	------------------------------------------------------------*/
static short empty_bb(short lft)
{
	(void) lft;
	if (clock() - Dialog.busy.resetClock > 200)
		Dialog.busy.enable();
	return 0;
}


/* fullempty_bb --------------------------------------------------
	wirklich leere draw_busybox
	------------------------------------------------------------*/
static short fullempty_bb(short lft)
{
	(void) lft;
	return 0;
}


/* empty_rbb --------------------------------------------------
	leere reset_busybox
	------------------------------------------------------------*/
static void empty_rbb(short lft, const char *txt)
{
	(void) lft;
	(void) txt;
}

/*---------------------------------------------------------------
	Funktionen zum Ein- und Ausschalten der Busybox
	------------------------------------------------------------*/
void enable_busybox(void)
{
	Dialog.busy.disabled = FALSE;
	global_services.busybox = Dialog.busy.draw;
	global_services.reset_busybox = Dialog.busy.reset;
}


void disable_busybox(void)
{
	global_services.busybox = empty_bb;
	global_services.reset_busybox = empty_rbb;
}


void fulldisable_busybox(void)
{
	Dialog.busy.disabled = TRUE;
	global_services.busybox = fullempty_bb;
	global_services.reset_busybox = empty_rbb;
}
