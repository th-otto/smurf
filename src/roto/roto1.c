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
#include <math.h>
#include "import.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "../smurf_f.h"
#include "smurf.h"
#include "globdefs.h"
#include "../smurfobs.h"
#include "../ext_obs.h"
#include "bindings.h"


void rotozoom(short *img, short *padr, short xstep, short xystep, short pos) ASM_NAME("_rotozoom");


static int *Sin;
static int *Cos;

static WORD old_timer;
static SMURF_PIC rotoimage;
static short phi, phi2, phi3;

static short xpos[] = { -100, -100, 100, 100, -100, -100, 100, 100 };
static short ypos[] = { -100, 100, 100, -100, -100, 100, 100, -100 };
static short zpos[] = { -100, -100, -100, -100, 100, 100, 100, 100 };


static WORD x2d[100];
static WORD y2d[100];

#define pointNum 8
#define polyNum  6

static WORD polyPointNum[polyNum] = { 4, 4, 4, 4, 4, 4 };

static WORD polyPoints[polyNum][4];

static WORD offscreenHandle = -1;
static WORD rotowork_in[40];
static WORD rotowork_out[40];

void init_roto(void)
{
	double bog;
	long t;

	phi = 0;

	old_timer = Sys_info.Event_Timer;
	graf_mouse(BUSYBEE, NULL);

	Sin = SMalloc((long) 370 * sizeof(*Sin));
	Cos = SMalloc((long) 370 * sizeof(*Cos));
	if (Sin == NULL || Cos == NULL)
		return;

	/*---- F�llen der Sin/Cos-Tabelle ----*/
	for (t = 0; t < 365; t++)
	{
		bog = (float) t * M_PI / 180.F;

		Sin[t] = sin(bog) * 256.F;
		Cos[t] = cos(bog) * 256.F;
	}


	/* Objekt vorbereiten
	 */
	polyPoints[0][0] = 0;
	polyPoints[0][1] = 1;
	polyPoints[0][2] = 2;
	polyPoints[0][3] = 3;

	polyPoints[1][0] = 0;
	polyPoints[1][1] = 4;
	polyPoints[1][2] = 5;
	polyPoints[1][3] = 1;

	polyPoints[2][0] = 4;
	polyPoints[2][1] = 7;
	polyPoints[2][2] = 6;
	polyPoints[2][3] = 5;

	polyPoints[3][0] = 3;
	polyPoints[3][1] = 2;
	polyPoints[3][2] = 6;
	polyPoints[3][3] = 7;

	polyPoints[4][0] = 1;
	polyPoints[4][1] = 5;
	polyPoints[4][2] = 6;
	polyPoints[4][3] = 2;

	polyPoints[5][0] = 0;
	polyPoints[5][1] = 3;
	polyPoints[5][2] = 7;
	polyPoints[5][3] = 4;

	rotoimage.pic_data = SMalloc((160L * 160L) + 1000);
	rotoimage.screen_pic = SMalloc(sizeof(MFDB));
	rotoimage.screen_pic->fd_addr = SMalloc((160L * 160L) + 1000);

	rotoimage.pic_width = 160;
	rotoimage.pic_height = 160;
	rotoimage.depth = Sys_info.bitplanes;
	rotoimage.zoom = 0;

	rotoimage.screen_pic->fd_w = 160;
	rotoimage.screen_pic->fd_h = 160;
	rotoimage.screen_pic->fd_wdwidth = 160 / 16;
	rotoimage.screen_pic->fd_stand = 0;
	rotoimage.screen_pic->fd_nplanes = Sys_info.bitplanes;

	memset(rotowork_in, 0, sizeof(rotowork_in));
	rotowork_in[11] = rotoimage.screen_pic->fd_w - 1;
	rotowork_in[12] = rotoimage.screen_pic->fd_h - 1;
	v_opnbm(rotowork_in, rotoimage.screen_pic, &offscreenHandle, rotowork_out);

	wind_s[WIND_INFO].picture = &rotoimage;
	wind_s[WIND_INFO].pic_xpos = wind_s[WIND_INFO].resource_form[ROTOBOX].ob_x;
	wind_s[WIND_INFO].pic_ypos = wind_s[WIND_INFO].resource_form[ROTOBOX].ob_y;
	wind_s[WIND_INFO].xoffset = 0;
	wind_s[WIND_INFO].xoffset = 0;
	wind_s[WIND_INFO].clipwid = 160;
	wind_s[WIND_INFO].cliphgt = 160;

	/* Timer umschalten
	 */
	Sys_info.Event_Timer = 20;

	wind_s[WIND_INFO].resource_form[ROTOBOX].ob_flags &= ~OF_HIDETREE;

	Window.redraw(&wind_s[WIND_INFO], NULL, 0, 0);
	graf_mouse(ARROW, NULL);
}



void roto(void)
{
	short pos;
	short t;
	WORD pxy[8];
	short normvek;
	long mySin, myCos;
	short xrot[100], yrot[100], zrot[100];

	if (Sin == NULL || Cos == NULL || offscreenHandle <= 0)
		return;

	/*---- Jetz' gehts los ----*/
	phi += 4;
	if (phi > 359)
		phi = 4;
	phi2 += 3;
	if (phi2 > 359)
		phi2 = 3;
	phi3 += 2;
	if (phi3 > 359)
		phi3 = 2;

	/* rotieren & projezieren
	 */
	for (t = 0; t < pointNum; t++)
	{
		/* y-achse */
		mySin = Sin[phi];
		myCos = Cos[phi];
		xrot[t] = (short) ((((long) xpos[t] * myCos) - ((long) zpos[t] * mySin)) >> 8);
		zrot[t] = (short) ((((long) xpos[t] * mySin) + ((long) zpos[t] * myCos)) >> 8);

		pos = zrot[t];

		/* x-achse */
		mySin = Sin[phi2];
		myCos = Cos[phi2];
		zrot[t] = (short) ((((long) zrot[t] * myCos) - ((long) ypos[t] * mySin)) >> 8);
		yrot[t] = (short) ((((long) pos * mySin) + ((long) ypos[t] * myCos)) >> 8);

		pos = xrot[t];

		/* z-achse */
		mySin = Sin[phi3];
		myCos = Cos[phi3];
		xrot[t] = (short) ((((long) xrot[t] * myCos) - ((long) yrot[t] * mySin)) >> 8);
		yrot[t] = (short) ((((long) pos * mySin) + ((long) yrot[t] * myCos)) >> 8);


		/* projektion
		 */
		x2d[t] = (xrot[t] << 7) / (zrot[t] + 300) + 80;
		y2d[t] = (yrot[t] << 7) / (zrot[t] + 300) + 80;
	}

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = 160;
	pxy[3] = 160;
	vsf_interior(offscreenHandle, FIS_SOLID);
	vswr_mode(offscreenHandle, MD_REPLACE);
	vsf_color(offscreenHandle, 1);
	v_bar(offscreenHandle, pxy);

	for (t = 0; t < polyNum; t++)
	{
		normvek = (x2d[polyPoints[t][2]] - x2d[polyPoints[t][0]]) * (y2d[polyPoints[t][1]] - y2d[polyPoints[t][0]]) -
			(x2d[polyPoints[t][1]] - x2d[polyPoints[t][0]]) * (y2d[polyPoints[t][2]] - y2d[polyPoints[t][0]]);

		if (normvek < 0)
			continue;

		pxy[0] = x2d[polyPoints[t][0]];
		pxy[1] = y2d[polyPoints[t][0]];
		pxy[2] = x2d[polyPoints[t][1]];
		pxy[3] = y2d[polyPoints[t][1]];
		pxy[4] = x2d[polyPoints[t][2]];
		pxy[5] = y2d[polyPoints[t][2]];
		pxy[6] = x2d[polyPoints[t][3]];
		pxy[7] = y2d[polyPoints[t][3]];
		vsf_color(offscreenHandle, t + 7);
		v_fillarea(offscreenHandle, polyPointNum[t], pxy);
	}
	Window.redraw(&wind_s[WIND_INFO], NULL, 0, DRAWNOTREE | DRAWNOBLOCK | NOBLOCKBOX);
}



void deinit_roto(void)
{
	wind_s[WIND_INFO].picture = 0;

	if (rotoimage.pic_data)
		SMfree(rotoimage.pic_data);

	if (rotoimage.screen_pic)
	{
		SMfree(rotoimage.screen_pic->fd_addr);
		SMfree(rotoimage.screen_pic);
		rotoimage.pic_data = NULL;
		rotoimage.screen_pic = NULL;
	}

	SMfree(Sin);
	SMfree(Cos);
	Sin = Cos = NULL;

	if (offscreenHandle > 0)
	{
		v_clsbm(offscreenHandle);
		offscreenHandle = -1;
	}

	Sys_info.Event_Timer = old_timer;
	wind_s[WIND_INFO].resource_form[ROTOBOX].ob_flags |= OF_HIDETREE;
	wind_s[WIND_INFO].resource_form[INFO_YEAH].ob_state &= ~OS_SELECTED;
	Window.redraw(&wind_s[WIND_INFO], NULL, 0, 0);
}
