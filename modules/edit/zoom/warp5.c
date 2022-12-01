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
 * The Initial Developer of the Original Code is
 * Bjoern Spruck
 *
 * This code is a module for the programm SMURF developed by
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *         
 * Contributor(s):
 *         
 *
 * ***** END LICENSE BLOCK *****
 */

/*
		Smurf Warps mir
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "import.h"
#include "smurfine.h"

#include "country.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Zoom",
	0x0080,
	"Bj”rn Spruck",
	{ "", "", "", "", "", "", "", "", "", "" },
/* Objekttitel */

#if COUNTRY==1
	"St„rke", "", "", "",
	"radial", "tangential", "negativ", "",
#elif COUNTRY==0
	"Strength", "", "", "",
	"radial", "tangential", "negativ", "",
#elif COUNTRY==2
	"Strength", "", "", "",
	"radial", "tangential", "negativ", "",
#else
#error "keine Sprache!"
#endif

	"", "", "", "",
/* Min/Max Slider */
	1, 100, 0, 128, 0, 128, 0, 128,
/* Min/Max Editfelder */
	1, 100, 0, 10, 0, 10, 0, 10,
/* Slider-Defaultwerte */
	10, 0, 0, 0,
	2, 3, 3, 0,
	0, 0, 0, 0,
/* Anzahl Bilder */
	1,
/* Bildbeschreibung */
	NULL, NULL, NULL, NULL, NULL, NULL,
};

MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	0,
};

static SERVICE_FUNCTIONS *services;

static short warp_rad_int(SMURF_PIC *bild, short MAXBR, short mx, short my)
{
	uint8_t *bt;
	uint8_t *bildto;
	uint8_t *bildfr;
	short bb, hh;
	short i, j;
	long bb3;
	long distfak;

	bb = bild->pic_width;
	hh = bild->pic_height;
	bildto = bt = (uint8_t *)Malloc((long) bb * hh * 3);
	if (bildto == 0)
		return 0;

	bildfr = bild->pic_data;
	bb3 = bb * 3;

	{
		short bb1 = bb > 1;
		short hh1 = hh >> 1;

		if (mx >= bb1 && my >= hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) mx * mx + (double) my * my));
		if (mx < bb1 && my >= hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) (bb - mx) * (bb - mx) + (double) my * my));
		if (mx >= bb1 && my < hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) mx * mx + (double) (hh - my) * (hh - my)));
		if (mx < bb1 && my < hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) (bb - mx) * (bb - mx) + (double) (hh - my) * (hh - my)));
	}

	for (i = 0; i < hh; i++)
	{
		short dy;
		long ddy;

		if ((i & 0xf) == 0)
			services->busybox((short) ((long) 128 * i / hh));

		dy = i - my;
		ddy = distfak * dy / MAXBR;

		for (j = 0; j < bb; j++)
		{
			short dx, c, k;
			long ddx;
			long nx, ny;
			unsigned short rot, gruen, blau;

			dx = j - mx;
			ddx = distfak * dx / MAXBR;

			rot = 0;
			gruen = 0;
			blau = 0;
			c = 0;

			nx = ((long) j << 8) - (long) ddx *MAXBR + 128;
			ny = ((long) i << 8) - (long) ddy *MAXBR + 128;

			for (k = 0; k < MAXBR; k++)
			{
				short nnx, nny;

				nnx = (short) (nx >> 8);
				nny = (short) (ny >> 8);
				if (nnx >= 0 && nnx < bb && nny >= 0 && nny < hh)
				{
					uint8_t *f;
					f = bildfr + (long) nnx *3 + (long) nny *bb3;

					rot += *f++;
					gruen += *f++;
					blau += *f;
					c++;
				} else
					break;
				nx += ddx;
				ny += ddy;
			}

			rot /= c;
			if (rot > 255)
				rot = 255;
			*bt++ = rot;
			gruen /= c;
			if (gruen > 255)
				gruen = 255;
			*bt++ = gruen;
			blau /= c;
			if (blau > 255)
				blau = 255;
			*bt++ = blau;
		}
	}
	memcpy(bild->pic_data, bildto, (long) bb * hh * 3);
	Mfree(bildto);
	return -1;
}


static short warp_tan_int(SMURF_PIC *bild, short MAXBR, short mx, short my)
{
	uint8_t *bt;
	uint8_t *bildto;
	uint8_t *bildfr;
	short bb, hh;
	short i, j;
	long bb3;
	long distfak;

	bb = bild->pic_width;
	hh = bild->pic_height;
	bildto = bt = (uint8_t *)Malloc((long) bb * hh * 3);
	if (bildto == 0)
		return 0;

	bildfr = bild->pic_data;
	bb3 = bb * 3;

	{
		short bb1 = bb > 1;
		short hh1 = hh >> 1;

		if (mx >= bb1 && my >= hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) mx * mx + (double) my * my));
		if (mx < bb1 && my >= hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) (bb - mx) * (bb - mx) + (double) my * my));
		if (mx >= bb1 && my < hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) mx * mx + (double) (hh - my) * (hh - my)));
		if (mx < bb1 && my < hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) (bb - mx) * (bb - mx) + (double) (hh - my) * (hh - my)));
	}

	for (i = 0; i < hh; i++)
	{
		short dy;
		long ddy;

		if ((i & 0xf) == 0)
			services->busybox((short) ((long) 128 * i / hh));

		dy = i - my;
		ddy = distfak * dy / MAXBR;

		for (j = 0; j < bb; j++)
		{
			short dx, c, k;
			long ddx;
			long nx, ny;
			unsigned short rot, gruen, blau;

			dx = j - mx;
			ddx = distfak * dx / MAXBR;

			rot = 0;
			gruen = 0;
			blau = 0;
			c = 0;

			nx = ((long) j << 8) + (long) ddy *MAXBR + 128;
			ny = ((long) i << 8) - (long) ddx *MAXBR + 128;

			for (k = 0; k < MAXBR; k++)
			{
				short nnx, nny;

				nnx = (short) (nx >> 8);
				nny = (short) (ny >> 8);
				if (nnx >= 0 && nnx < bb && nny >= 0 && nny < hh)
				{
					uint8_t *f;
					f = bildfr + (long) nnx *3 + (long) nny *bb3;

					rot += *f++;
					gruen += *f++;
					blau += *f;
					c++;
				}
				nx -= ddy;
				ny += ddx;
			}

			rot /= c;
			if (rot > 255)
				rot = 255;
			*bt++ = rot;
			gruen /= c;
			if (gruen > 255)
				gruen = 255;
			*bt++ = gruen;
			blau /= c;
			if (blau > 255)
				blau = 255;
			*bt++ = blau;
		}
	}
	memcpy(bild->pic_data, bildto, (long) bb * hh * 3);
	Mfree(bildto);
	return -1;
}


static short warp_neg_int(SMURF_PIC *bild, short MAXBR, short mx, short my)
{
	uint8_t *bt;
	uint8_t *bildto;
	uint8_t *bildfr;
	short bb, hh;
	short i, j;
	long bb3;
	long distfak;

	bb = bild->pic_width;
	hh = bild->pic_height;
	bildto = bt = (uint8_t *)Malloc((long) bb * hh * 3);
	if (bildto == 0)
		return 0;

	bildfr = bild->pic_data;
	bb3 = bb * 3;

	{
		short bb1 = bb > 1;
		short hh1 = hh >> 1;

		if (mx >= bb1 && my >= hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) mx * mx + (double) my * my));
		if (mx < bb1 && my >= hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) (bb - mx) * (bb - mx) + (double) my * my));
		if (mx >= bb1 && my < hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) mx * mx + (double) (hh - my) * (hh - my)));
		if (mx < bb1 && my < hh1)
			distfak = (long) (128 * MAXBR / sqrt((double) (bb - mx) * (bb - mx) + (double) (hh - my) * (hh - my)));
	}

	for (i = 0; i < hh; i++)
	{
		short dy;
		long ddy;

		if ((i & 0xf) == 0)
			services->busybox((short) ((long) 128 * i / hh));

		dy = i - my;
		ddy = distfak * dy / MAXBR;

		for (j = 0; j < bb; j++)
		{
			short dx, c, k;
			long ddx;
			long nx, ny;
			unsigned short rot, gruen, blau;

			dx = j - mx;
			ddx = distfak * dx / MAXBR;

			rot = 0;
			gruen = 0;
			blau = 0;
			c = 0;

			nx = ((long) j << 8) - (long) ddy *MAXBR + 128;
			ny = ((long) i << 8) - (long) ddx *MAXBR + 128;

			for (k = 0; k < MAXBR; k++)
			{
				short nnx, nny;

				nnx = (short) (nx >> 8);
				nny = (short) (ny >> 8);
				if (nnx >= 0 && nnx < bb && nny >= 0 && nny < hh)
				{
					uint8_t *f;
					f = bildfr + (long) nnx *3 + (long) nny *bb3;

					rot += *f++;
					gruen += *f++;
					blau += *f;
					c++;
				}
				nx += ddy;
				ny += ddx;
			}

			rot /= c;
			if (rot > 255)
				rot = 255;
			*bt++ = rot;
			gruen /= c;
			if (gruen > 255)
				gruen = 255;
			*bt++ = gruen;
			blau /= c;
			if (blau > 255)
				blau = 255;
			*bt++ = blau;
		}
	}
	memcpy(bild->pic_data, bildto, (long) bb * hh * 3);
	Mfree(bildto);
	return -1;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Warps mir (by Sage) 						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	static short crhx, crhy;

	services = smurf_struct->services;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MCROSSHAIR:
		smurf_struct->event_par[0] = 1;
		smurf_struct->event_par[1] = 0;
		smurf_struct->module_mode = M_CROSSHAIR;
		break;

	case MCH_DEFCOO:					/* Smurf fragt: und die Default-Koordinaten? */
		crhx = smurf_struct->smurf_pic->pic_width >> 1;
		crhy = smurf_struct->smurf_pic->pic_height >> 1;
		smurf_struct->event_par[0] = crhx;
		smurf_struct->event_par[1] = crhy;
		smurf_struct->module_mode = M_CHDEFCOO;
		break;

	case MCH_COORDS:					/* Smurf sagt: hier sind die eingestellten Fadenkreuzkoordinaten */
		crhx = smurf_struct->event_par[0];
		crhy = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		smurf_struct->services->reset_busybox(0, "Zoo...ooM");
		if (smurf_struct->check1 & 1)
		{
			if (warp_rad_int(smurf_struct->smurf_pic, (short) smurf_struct->slide1, crhx, crhy) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
			} else
			{
				smurf_struct->module_mode = M_PICDONE;
			}
		}
		if (smurf_struct->check2 & 1)
		{
			if (warp_tan_int(smurf_struct->smurf_pic, (short) smurf_struct->slide1, crhx, crhy) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
			} else
			{
				smurf_struct->module_mode = M_PICDONE;
			}
		}
		if (smurf_struct->check3 & 1)
		{
			if (warp_neg_int(smurf_struct->smurf_pic, (short) smurf_struct->slide1, crhx, crhy) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
			} else
			{
				smurf_struct->module_mode = M_PICDONE;
			}
		}
		break;

	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
