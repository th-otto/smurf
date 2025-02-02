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
 * The Initial Developer of the Original Code is
 * Christian Eyrich
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =========================================================*/
/*				Bilderrahmen (� by Olaf Piesche)			*/
/* Version 0.1  --  28.02.98								*/
/*	  1-8, 16, 24 Bit										*/
/* Version 0.2  --  01.03.98								*/
/*	  Standardformatbilder hatten noch M�ll von rechts mit-	*/
/*	  gezogen.												*/
/* Version 0.3  --  03.03.98								*/
/*	  Rahmenfarbe bei Palettenbildern wird aufwendig		*/
/*	  ermittelt.											*/
/* Version 0.4  --  27.03.98								*/
/*	  Zeilenweise Assemblerleseroutine eingebaut.			*/
/* Version 0.5  --  16.05.98								*/
/*	  es wird nun die Rearrange Line benutzt, die auch auf	*/
/*	  STs l�uft. Weiterhin ist der Fehler gefixt, da� die	*/
/*	  Planel�nge zum F�llen des neuen Standardformatbildes	*/
/*	  aus der Gr��e des urspr�nglichen Bildes berechnet		*/
/*	  wurde.												*/
/* Version 0.6  --  28.05.98								*/
/*	  Fehler im Standardformatteil gefixt der die ersten 8	*/
/*	  Pixel der n�chsten Zeile immer noch mit dem Daten	der	*/
/*	  letzten 8 Pixel der aktuellen Zeile f�llte.			*/
/*	  Au�erdem Beschleunigung der Standardformatroutine		*/
/*	  durch Einsparung von 3 Modulo in der Schleife.		*/
/* Version 0.7  --  15.07.98								*/
/*	  Kleinen Fehler gefixt. Rearrange2 wurde mit L�nge		*/
/*	  realwidth und nicht realwidth + leftborder8, also		*/
/*	  der alten und nicht neuen Breite aufgerufen.			*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Bilderrahmen"
#define TEXT2 "Rot"
#define TEXT3 "Gr�n"
#define TEXT4 "Blau"
#define TEXT5 "zentriert"
#define TEXT6 "Farbe benutzen"
#define TEXT7 "links"
#define TEXT8 "rechts"
#define TEXT9 "oben"
#define TEXT10 "unten"
#elif COUNTRY==0
#define TEXT1 "Bilderrahmen"
#define TEXT2 "Red"
#define TEXT3 "Green"
#define TEXT4 "Blue"
#define TEXT5 "zentriert"
#define TEXT6 "Farbe benutzen"
#define TEXT7 "Left"
#define TEXT8 "Right"
#define TEXT9 "Top"
#define TEXT10 "Bottom"
#elif COUNTRY==2
#define TEXT1 "Bilderrahmen"
#define TEXT2 "Rouge"
#define TEXT3 "Vert"
#define TEXT4 "Bleu"
#define TEXT5 "zentriert"
#define TEXT6 "Farbe benutzen"
#define TEXT7 "Left"
#define TEXT8 "Right"
#define TEXT9 "Top"
#define TEXT10 "Bottom"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

MOD_INFO module_info = {
	TEXT1,
	0x0070,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ TEXT2, TEXT3, TEXT4, "" },
	{ TEXT5, TEXT6, "", "" },
	{ TEXT7, TEXT8, TEXT9, TEXT10 },
	{ { 0, 255 }, { 0, 255 }, { 0, 255 }, { 0, 64 } },
	{ { 0, 32767 }, { 0, 32767 }, { 0, 32767 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 2, 4, 7, 8, 16, 24, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	0
};


static uint8_t usecolor;
static uint8_t red, green, blue;




static void backcolor(SMURF_PIC *picture, uint8_t *ziel, uint8_t *buf, unsigned short newwidth, unsigned short newheight)
{
	uint8_t *buffer;
	uint8_t *pal;
	uint8_t *palette;
	uint8_t histo[256];
	uint8_t *line;
	uint8_t *pixbuf;
	uint8_t BitsPerPixel;
	uint8_t idx;
	unsigned short x, y;
	unsigned short width, height;
	unsigned short i;
	unsigned short cols;
	long planelength;
	long w;

	buffer = picture->pic_data;
	BitsPerPixel = picture->depth;
	width = picture->pic_width;
	height = picture->pic_height;
	cols = 1 << BitsPerPixel;
	planelength = (long) (width + 7) / 8 * (long) height;

	pixbuf = (uint8_t *) Malloc(newwidth + 15);

	if (usecolor)						/* Farbe des linken oberen Pixels benutzen */
	{
		if (picture->format_type == FORM_PIXELPAK)
		{
			idx = *buffer;
		} else
		{
			pixbuf[0] = 0;
			getpix_std_line(buffer, pixbuf, BitsPerPixel, planelength, 1);
			idx = pixbuf[0];
		}
	} else
	{
		palette = picture->palette;
		pal = palette;

		i = 0;
		do
		{
			if (*pal == red && *(pal + 1) == green && *(pal + 2) == blue)
				break;
			else
				pal += 3;
		} while (++i < cols);

		if (i != cols)					/* Farbe in Palette gefunden */
		{
			idx = i;
		} else
		{
			w = (width + 7) / 8;
			memset(histo, 0, sizeof(histo));

			if (picture->format_type == FORM_PIXELPAK)
			{
				y = 0;
				do
				{
					x = 0;
					do
					{
						histo[*buffer++] = 1;	/* Vorhandensein notieren */
					} while (++x < width);
				} while (++y < height);
			} else
			{
				y = 0;
				do
				{
					memset(pixbuf, 0x0, width);
					getpix_std_line(buffer, pixbuf, BitsPerPixel, planelength, width);
					buffer += w;
					line = pixbuf;

					x = 0;
					do
					{
						histo[*line++] = 1;	/* Vorhandensein notieren */
					} while (++x < width);
				} while (++y < height);
			}

			i = 0;
			do
			{
				if (histo[i] == 0)
					break;
			} while (++i < cols);

			if (i != cols)				/* Leereintrag in Palette gefunden */
			{
				pal = palette + i + i + i;
				*pal++ = red;
				*pal++ = green;
				*pal = blue;

				idx = i;
			} else
			{
				idx = 0;
			}
		}
	}

	/* Hintergrund malen */
	if (picture->format_type == FORM_PIXELPAK)
	{
		/* Bei Pixelpacked einfach weil nur ein Byte pro Pixel zu setzen ist */
		memset(ziel, idx, (long) newwidth * (long) newheight);
	} else
	{
		planelength = (long) (newwidth + 7) / 8 * (long) newheight;
		memset(pixbuf, idx, newwidth + 15);	/* pixbuf mit Zielindex f�llen */

		y = 0;
		do
		{
			ziel += setpix_std_line(pixbuf, ziel, BitsPerPixel, planelength, newwidth);
		} while (++y < newheight);

		planelength = (long) (newwidth + 15) / 8;
		setpix_std_line(pixbuf, buf, BitsPerPixel, planelength, (newwidth + 15) / 8 * 8);
	}

	Mfree(pixbuf);
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Bilderrahmen					*/
/*	Addiert nach den Angaben des Users einen Rahmen	*/
/*	an die Seiten des Bildes.						*/
/*	Ist "zentriert" eingeschaltet, wird nur der Wert*/
/*	f�r links und oben verwendet und aufgeteilt.	*/
/*		1-8, 16 und 24 Bit							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *buf;
	uint8_t BitsPerPixel;
	uint8_t center;
	uint8_t p;
	uint8_t wm8;

	uint16_t *buffer16;
	uint16_t *ziel16;
	uint16_t *oziel16;
	unsigned short x, y;
	unsigned short width, height;
	unsigned short newheight, newwidth;
	unsigned short upperborder, lowerborder, leftborder, leftborder8, rightborder;
	unsigned short temp;
	unsigned short color;
	unsigned short pixels;
	long offset, realwidth, newrealwidth, newplh;


	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* Wenn das Modul zum ersten Mal gestartet wurde, */
		/* ein Einstellformular anfordern....             */
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

		/* Einstellformular wurde mit START verlassen - Funktion ausf�hren */
	case MEXEC:
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;

#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		red = smurf_struct->slide[0];
		green = smurf_struct->slide[1];
		blue = smurf_struct->slide[2];
		center = smurf_struct->check[0];
		usecolor = smurf_struct->check[1];
		leftborder = smurf_struct->edit[0];
		rightborder = smurf_struct->edit[1];
		upperborder = smurf_struct->edit[2];
		lowerborder = smurf_struct->edit[3];

		/* nichts zu tun */
		if ((center && upperborder == 0 && leftborder == 0) ||
			(!center && upperborder == 0 && lowerborder == 0 && leftborder == 0 && rightborder == 0))
		{
			smurf_struct->module_mode = M_STARTED;
			return;
		}

		/* Werte aufteilen */
		if (center)
		{
			temp = leftborder;
			leftborder = (temp + 1) / 2;
			rightborder = temp / 2;
			temp = upperborder;
			upperborder = (temp + 1) / 2;
			lowerborder = temp / 2;
		}

		obuffer = buffer = smurf_struct->smurf_pic->pic_data;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		newwidth = leftborder + width + rightborder;
		newheight = upperborder + height + lowerborder;

		if (BitsPerPixel == 24)
		{
			realwidth = (long) width *3L;
			newrealwidth = (long) newwidth *3L;

			if ((ziel = SMalloc(newrealwidth * (long) newheight)) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
				return;
			}
			oziel = ziel;

			offset = (long) (leftborder + rightborder) * 3L;

			/* Farbe des linken oberen Pixels benutzen */
			if (usecolor)
			{
				red = *buffer;
				green = *(buffer + 1);
				blue = *(buffer + 2);
			}

			/* Hintergrund malen */
			y = 0;
			do
			{
				x = 0;
				do
				{
					*ziel++ = red;
					*ziel++ = green;
					*ziel++ = blue;
				} while (++x < newwidth);
			} while (++y < newheight);

			ziel = oziel;

			ziel += (long) upperborder *(long) newrealwidth + (long) leftborder *3L;

			y = 0;
			do
			{
				memcpy(ziel, buffer, realwidth);
				ziel += realwidth + offset;
				buffer += realwidth;
			} while (++y < height);
		}								/* BitsPerPixel == 24? */
		else if (BitsPerPixel == 8 && smurf_struct->smurf_pic->format_type == FORM_PIXELPAK)
		{
			if ((ziel = SMalloc((long) newwidth * (long) newheight)) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
				return;
			}
			oziel = ziel;

			offset = leftborder + rightborder;

			backcolor(smurf_struct->smurf_pic, ziel, NULL, newwidth, newheight);

			ziel += (long) upperborder *(long) newwidth + (long) leftborder;

			y = 0;
			do
			{
				memcpy(ziel, buffer, width);
				ziel += width + offset;
				buffer += width;
			} while (++y < height);
		}								/* BitsPerPixel == 8 && FORM_PIXELPAK? */
		else if (smurf_struct->smurf_pic->format_type == FORM_STANDARD)
		{
			realwidth = ((long) width + 7) / 8;
			newrealwidth = ((long) newwidth + 7) / 8;

			if ((ziel = SMalloc(newrealwidth * (long) newheight * BitsPerPixel + 7)) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
				return;
			}
			oziel = ziel;

			buf = (uint8_t *)Malloc((newrealwidth + 2) * BitsPerPixel);

			leftborder8 = (leftborder + 7) / 8;
			wm8 = width % 8;
			pixels = 8 - (leftborder8 * 8 - leftborder);	/* Verschiebung nach links */

			/* Planeh�he neue Gr��e */
			newplh = newrealwidth * (long) newheight;

			backcolor(smurf_struct->smurf_pic, ziel, buf, newwidth, newheight);

			p = 0;
			do
			{
				ziel = oziel + (long) p *newplh + (long) upperborder *(long) newrealwidth;

				y = 0;
				do
				{
					if (wm8 == 0)
						memcpy(buf + leftborder8, buffer, realwidth);	/* normal kopieren */
					else
					{					/* vom letzten Byte nur die wichtigen Bits kopieren */
						memcpy(buf + leftborder8, buffer, realwidth - 1);
						*(buf + leftborder8 + realwidth - 1) =
							(*(buffer + realwidth - 1) & (0xff << (8 - wm8))) | (*(buf + leftborder8 + realwidth - 1) &
																				 (0xff >> wm8));
					}

					rearrange_line2(buf, ziel, newrealwidth /*realwidth + leftborder8 */ , pixels);
					ziel += newrealwidth;
					buffer += realwidth;
				} while (++y < height);

				buf += ((long) newwidth + 15) >> 3;
			} while (++p < BitsPerPixel);

			Mfree(buf);
		}								/* FORM_STANDARD? */
		else if (BitsPerPixel == 16)
		{
			buffer16 = (uint16_t *) buffer;

			if ((ziel = SMalloc((long) newwidth * (long) newheight * 2L)) == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
				return;
			}
			oziel = ziel;
			oziel16 = ziel16 = (uint16_t *) ziel;

			realwidth = (long) width *2L;

			offset = leftborder + rightborder;

			/* Farbe des linken oberen Pixels benutzen */
			/* ... immer, weil per Slider eingestellte */
			/* Farbe auch nicht in Palette sein kann. */
			if (usecolor)
				color = *buffer16;
			else
				color = ((red & 0x00f8) << 8) | ((green & 0x00fc) << 3) | (blue >> 3);

			/* Hintergrund malen */
			y = 0;
			do
			{
				x = 0;
				do
				{
					*ziel16++ = color;
				} while (++x < newwidth);
			} while (++y < newheight);

			ziel16 = oziel16;

			ziel16 += (long) upperborder *(long) newwidth + (long) leftborder;

			y = 0;
			do
			{
				memcpy(ziel16, buffer16, realwidth);
				ziel16 += width + offset;
				buffer16 += width;
			} while (++y < height);
		} else
		{
			smurf_struct->module_mode = M_INVALID;
			return;
		}

		buffer = obuffer;
		ziel = oziel;

#if TIMER
		/* wie schnell waren wir? */
		printf("\n%lu", get_timer());
		(void)Cnecin();
#endif

		SMfree(buffer);

		smurf_struct->smurf_pic->pic_data = ziel;

		smurf_struct->smurf_pic->pic_width = newwidth;
		smurf_struct->smurf_pic->pic_height = newheight;

		smurf_struct->module_mode = M_PICDONE;
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
