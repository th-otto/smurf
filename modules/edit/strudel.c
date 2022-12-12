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

/*  -------------   Strudel V0.5    --------------- */
/*      FÅr SMURF Bildkonverter, 19.10.95           */

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Strudel"
#define TEXT2 "Winkel"
#define TEXT3 "Von auûen"
#define TEXT4 "Tunnel"
#define TEXT5 "Make tables..."
#define TEXT6 "Twirling..."
#elif COUNTRY==0
#define TEXT1 "Twirl"
#define TEXT2 "Angle"
#define TEXT3 "Use edges"
#define TEXT4 "Tunnel"
#define TEXT5 "Make tables..."
#define TEXT6 "Twirling..."
#elif COUNTRY==2
#define TEXT1 "Twirl"
#define TEXT2 "Angle"
#define TEXT3 "Use edges"
#define TEXT4 "Tunnel"
#define TEXT5 "Make tables..."
#define TEXT6 "Twirling..."
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	TEXT1,
	0x0100,
	"Olaf Piesche",
	{ "", "", "", "", "", "", "", "", "", "" },
	TEXT2, "", "", "",
	TEXT3, TEXT4, "", "",
	"", "", "", "",
	0, 360,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	90, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

MOD_ABILITY module_ability = {
/* Farbtiefen, die vom Modul unterstÅtzt werden:			*/
	24, 0, 0, 0, 0, 0, 0, 0,
/*	Dazugehîrige Datenformate (FORM_PIXELPAK/FORM_STANDARD/FORM_BOTH) */
	FORM_PIXELPAK, 1, 1, 1, 1, 1, 1, 1,

/* UnterstÅtzte Orientierung (0=o->u, 1=u->o)):				*/
	1
};



/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short t;
	BOOLEAN umgekehrt;
	uint8_t *picdata;
	uint8_t *pdat;
	uint8_t *destpic;
	uint8_t *dest;
	short width, height;
	short xmax, ymax;
	long x, y;
	long phi;
	long xsrc, ysrc;
	long xmitte, ymitte;
	long xb, yb;
	long offset, srcoffset;
	long bytewidth;
	long cosval, sinval;
	float bog;

	long abstand, divi;
	SMURF_PIC *picture;
	long sliderval;
	uint8_t red, green, blue;
	long tr, tg, tb;

	long yoffset = 0;
	long ym_qu, xm_qu, dist_fak;
	BOOLEAN tunnel;

	static long Cos[370];
	static long Sin[370];

	switch (smurf_struct->module_mode)
	{
	/*--------------------- Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		smurf_struct->services->reset_busybox(0, TEXT5);
		for (t = 0; t < 370; t++)
		{
#if 0
			if (umgekehrt)
				bog = ((360 - t) * M_PI) / 180.0F;
			else
#endif
			bog = (t * M_PI) / 180.0F;
			Sin[t] = sin(bog) * 32768.0F;
			Cos[t] = cos(bog) * 32768.0F;
		}

		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);	/* Gib mir 'n PD! */

		smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */
		break;


	/*-------------------------------------	Modul soll loslegen */
	case MEXEC:
		picture = smurf_struct->smurf_pic;	/* Bild         */
		picdata = picture->pic_data;
		sliderval = smurf_struct->slide1;	/* Sliderwert auslesen */

		umgekehrt = smurf_struct->check1;	/* Checkboxen auslesen */
		tunnel = smurf_struct->check2;

		width = picture->pic_width;		/* Bildabmessungen */
		height = picture->pic_height;
		bytewidth = (long) width *3L;

		xmax = width - 1;
		ymax = height - 1;

		destpic = (uint8_t *)Malloc((long) width * (long) height * 3L);

		xmitte = width / 2;				/* Mittelpunkt */
		ymitte = height / 2;

		xm_qu = xmitte * xmitte;
		ym_qu = ymitte * ymitte;
		if (height < width)
			dist_fak = ym_qu;
		else
			dist_fak = xm_qu;

		dist_fak = (1024L * 1024L) / dist_fak;

		if (sliderval < 0)
			sliderval = 360 + sliderval;

		smurf_struct->services->reset_busybox(0, TEXT6);

		for (y = 0; y < height; y++)
		{
			if (!(y & 7))
				smurf_struct->services->busybox((int) (((long) y << 7L) / (long) height));

			for (x = 0; x < width; x++)
			{
				xb = x - xmitte;
				yb = y - ymitte;

				abstand = yb * yb + xb * xb;	/* die sqrt wird unten durch das quadrieren von ymitte gespart */

				divi = 1024 - ((abstand * dist_fak) >> 10);
				phi = (sliderval * divi) >> 10;

				if (phi < 0)
				{
					if (!umgekehrt)
						phi = 0;
					else
						phi += (float) phi * (360.0F / (float) phi);
				} else if (phi > 360)
				{
					if (!umgekehrt)
						phi = 360;
					else
						phi -= (float) phi * (360.0F / (float) phi);
				}

				if (phi != 0 && phi != 360)
				{

					/* Punkt rotieren */
					sinval = Sin[phi];
					cosval = Cos[phi];
					xsrc = ((long) (xb * cosval - yb * sinval) >> 15L) + xmitte;
					ysrc = ((long) (xb * sinval + yb * cosval) >> 15L) + ymitte;

					if (xsrc < 0)
						xsrc = 0;
					else if (xsrc > xmax)
						xsrc -= width;
					if (ysrc < 0)
						ysrc = 0;
					else if (ysrc > ymax)
						ysrc -= height;
					offset = (yoffset) + (x + x + x);
					srcoffset = (ysrc * bytewidth) + (xsrc + xsrc + xsrc);
				} else
				{
					offset = (yoffset) + (x + x + x);
					srcoffset = (yoffset) + (x + x + x);
				}

				dest = destpic + offset;
				pdat = picdata + srcoffset;

				red = *(pdat++);
				green = *(pdat++);
				blue = *(pdat++);


				if (tunnel && phi && divi <= 1024 && divi >= 0)
				{
					tr = red * (1024 - divi);
					tg = green * (1024 - divi);
					tb = blue * (1024 - divi);
					red = tr >> 10;
					green = tg >> 10;
					blue = tb >> 10;
				}

				*(dest++) = red;
				*(dest++) = green;
				*(dest++) = blue;

			}

			yoffset += bytewidth;		/* Wieder ne Multiplikation weniger */
		}

		Mfree(picdata);
		picture->pic_data = destpic;

		smurf_struct->module_mode = M_PICDONE;
		break;

	/* --------------------------------------Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
