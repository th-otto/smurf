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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "import.h"
#include "globdefs.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurf.h"
#include "demolib.h"

#include "smurfobs.h"
#include "ext_obs.h"

#define TIMER 0


static DITHER_DATA ditherdata;

/********************************************************************/
/*				    		FD - FAST								*/
/*			     	 Fehlerdiffusionsraster							*/
/********************************************************************/
void f_dither_24_1(SYSTEM_INFO *sysinfo, SMURF_PIC *picture, uint8_t *where_to, int mode, uint8_t *nct, GRECT *part)
{
	uint8_t *plantab;
	uint8_t empty_planetable[SM_PALETTE_MAX];
	WORD red24[SM_PALETTE_MAX], grn24[SM_PALETTE_MAX], blu24[SM_PALETTE_MAX];
	WORD t;
	WORD *red, *grn, *blu;
	WORD *c_red, *c_grn, *c_blu;
	WORD width, height;
	WORD endwid, endhgt;

	width = picture->pic_width;
	height = picture->pic_height;
	bplanes = sysinfo->bitplanes;

	for (t = 0; t < SM_PALETTE_MAX; t++)
		empty_planetable[t] = t;

	if (mode & PLANTAB)
		plantab = sysinfo->plane_table;
	else
		plantab = empty_planetable;

	if (mode & LNCT)
	{
		for (t = 0; t < SM_PALETTE_MAX; t++)
		{
			red24[t] = picture->red[t];
			grn24[t] = picture->grn[t];
			blu24[t] = picture->blu[t];
		}

		red = picture->red;
		grn = picture->grn;
		blu = picture->blu;
	} else
	{
		for (t = 0; t < SM_PALETTE_MAX; t++)
		{
			red24[t] = (WORD)(sysinfo->pal_red[t] * 255L / 1000L);
			grn24[t] = (WORD)(sysinfo->pal_green[t] * 255L / 1000L);
			blu24[t] = (WORD)(sysinfo->pal_blue[t] * 255L / 1000L);
		}

		red = sysinfo->red;
		grn = sysinfo->grn;
		blu = sysinfo->blu;
	}

	if (part == NULL)
	{
		endwid = width / (picture->zoom + 1);
		endhgt = height / (picture->zoom + 1);
	} else
	{
		endwid = part->g_w / (picture->zoom + 1);
		endhgt = part->g_h / (picture->zoom + 1);
	}

	/*
	 * Median-Cut: MC-Pal auf 15 Bit skalieren
	 */
	if (mode & SCALEPAL)
	{
		c_red = malloc((SM_PALETTE_MAX + 4) * sizeof(*c_red));
		c_grn = malloc((SM_PALETTE_MAX + 4) * sizeof(*c_grn));
		c_blu = malloc((SM_PALETTE_MAX + 4) * sizeof(*c_blu));

		for (t = 0; t < SM_PALETTE_MAX; t++)
		{
			c_red[t] = ((unsigned char)picture->red[t]) >> 3;
			c_grn[t] = ((unsigned char)picture->grn[t]) >> 3;
			c_blu[t] = ((unsigned char)picture->blu[t]) >> 3;
		}
	} else
	{
#ifdef __GNUC__
		c_red = c_grn = c_blu = 0;
#endif
	}


	/*
	 * DITHER_DATA-Struktur f�llen
	 */
	ditherdata.dst_depth = bplanes;		/* Farbtiefe, auf die gedithert werden soll */
	ditherdata.red = red;
	ditherdata.blue = blu;
	ditherdata.green = grn;				/* Palette, auf die gedithert werden soll */
	ditherdata.nc_tab = nct;			/* Nearest-Color-Table */
	ditherdata.planetable = plantab;	/* Plane-Table f�r VDI-Indizes */
	ditherdata.picture = picture;		/* das zu dithernde Bild */
	if (part == NULL)
	{
		ditherdata.x = 0;				/* der zu dithernde Ausschnitt des Bildes */
		ditherdata.y = 0;				/* der zu dithernde Ausschnitt des Bildes */
		ditherdata.w = width;			/* der zu dithernde Ausschnitt des Bildes */
		ditherdata.h = height;			/* der zu dithernde Ausschnitt des Bildes */
	} else
	{
		ditherdata.x = part->g_x;		/* der zu Dithernde Ausschnitt des Bildes */
		ditherdata.y = part->g_y;		/* der zu Dithernde Ausschnitt des Bildes */
		ditherdata.w = part->g_w;		/* der zu Dithernde Ausschnitt des Bildes */
		ditherdata.h = part->g_h;		/* der zu Dithernde Ausschnitt des Bildes */
	}
	ditherdata.dest_mem = where_to;		/* Zielspeicher, in den gedithert werden soll */
	ditherdata.zoom = picture->zoom;
	ditherdata.red24 = red24;
	ditherdata.blue24 = blu24;
	ditherdata.green24 = grn24;			/* 24Bit-Palette, auf die gedithert werden soll */
	if (mode & LNCT)
		ditherdata.not_in_nct = picture->not_in_nct;
	else
		ditherdata.not_in_nct = -1;

	/*---- 8Bit-Standardformat? */
	if (mode & STAND8)
	{
		if (exp_conf.exp_form == FORM_STANDARD)
		{
			ditherdata.set_16pixels = setpix_std_line;
			ditherdata.dest_planelength = (long)((endwid + 7) / 8) * (long)endhgt;
		} else
		{
			ditherdata.set_16pixels = setpix_pp;
			ditherdata.dest_planelength = (long)endwid * (long)endhgt;
		}
	} else
	{
		ditherdata.set_16pixels = setpix_std_line16;
		ditherdata.dest_planelength = ((long)(endwid + 15) / 16) * 2L * (long)endhgt;
	}

	/*---- Median Cut oder geladene Palette? */
	if (mode & LNCT)
	{
		ditherdata.nc_tab = picture->local_nct;
	}
	if (mode & SCALEPAL)
	{
		ditherdata.red = c_red;
		ditherdata.green = c_grn;
		ditherdata.blue = c_blu;
	}

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	/********* Und Dithermodul starten! ************/
	module.comm.start_dither_module(MSTART, dithermode - 1, &ditherdata);

#if TIMER
	/* wie schnell waren wir? */
	printf("\n%lu", get_timer());
	(void)Cnecin();
#endif

	if (mode & SCALEPAL)
	{
		free(c_red);
		free(c_grn);
		free(c_blu);
	}
}
