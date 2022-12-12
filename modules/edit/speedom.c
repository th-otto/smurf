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

/*  -------------   Speed-O-Move V0.5   --------------- */
/*          FÅr SMURF Bildkonverter, 19.10.95           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY == 1
#define TEXT1 "Speed-O-Move"
#define TEXT2 "StÑrke"
#define TEXT3 "Winkel"
#define TEXT4 "Motor anlassen..."
#define TEXT5 "Gas geben..."
#elif COUNTRY == 0
#define TEXT1 "Speed-O-Move"
#define TEXT2 "Strength"
#define TEXT3 "Angle"
#define TEXT4 "Start engine..."
#define TEXT5 "Accelerate..."
#else
#define TEXT1 "Speed-O-Move"
#define TEXT2 "Strength"
#define TEXT3 "Angle"
#define TEXT4 "Start engine..."
#define TEXT5 "Accelerate..."
#endif

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	TEXT1,
	0x0080,
	"Olaf Piesche",
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ TEXT2, TEXT3, "", "" },
/* 4 Checkboxueberschriften: */
	{ "", "", "", "" },
/* 4 Edit-Objekt-Ueberschriften: */
	{ "", "", "", "" },
/* min/max-Werte fÅr Slider */
	{ { 1, 100 }, { 0, 360 }, { 0, 128 }, { 0, 128 } },
/* min/max fÅr Editobjekte */
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
/* Defaultwerte fÅr Slider, Check und Edit */
	{ 20, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
/* Bezeichnung fuer Bilder */
	{ "", NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,			/* Farbtiefen */
	/* Grafikmodi: */
	FORM_PIXELPAK,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	0									/* Extra Flag */
};


/*---------------------------  FUNCTION MAIN -----------------------------*/

void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *picdata;
	uint8_t *pd;
	uint8_t *dest;
	uint8_t *dest_data;
	uint8_t *read;
	short width;
	short height;
	long x;
	long y;
	SMURF_PIC *picture;
	long sliderval;
	short red, green, blue;
	short strcounter;
	short divide;
	short count_orig;
	long yoffset;
	long bytewidth;
	long offset;
	short angle;
	short x1;
	short y1;
	short x2;
	short y2;
	short dx;
	short dy;
	short xcalc;
	short dxabs;
	short dyabs;
	long incx;
	long incy;
	long xpos;
	long ypos;

	float dist;
	float bog;
	long *offset_buffer;
	long *current_offset;
	
	/* Hier werden die Funktionen aus der GARGAMEL-Struktur geholt. */

	/*--------------------- Wenn das Modul zum ersten Mal gestartet wurde */
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);	/* Gib mir 'n PD! */

		smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */
		break;

	case MEXEC:
		/*------------------------------------- Modul soll loslegen */
		picture = smurf_struct->smurf_pic;	/* Bild         */
		pd = picdata = picture->pic_data;
		sliderval = smurf_struct->slide[0];	/* Sliderwert auslesen */
		angle = (short) smurf_struct->slide[1];	/* Sliderwert auslesen */

		width = picture->pic_width;		/* Bildabmessungen */
		height = picture->pic_height;
		bytewidth = (long) width * 3L;

		/*--------------- Vorberechnungen -------------*/

		/* Start- und Anfangspunkt ermitteln */
		bog = (angle * M_PI) / 180.0F;
		x1 = cos(bog) * sliderval;
		y1 = sin(bog) * sliderval;

		x2 = 0;
		y2 = 0;

		dx = x1 - x2;
		dy = y1 - y2;

		dist = sqrt(dx * dx + dy * dy);

		incx = (dx * 1024L) / dist;
		incy = (dy * 1024L) / dist;

		dest_data = dest = (uint8_t *)Malloc((long) width * (long) height * 3L);

		if (sliderval < 0)
			sliderval = 360 + sliderval;

		dxabs = abs(dx);
		dyabs = abs(dy);

		/*--------------------- Counter vorbestimmen -----------*/

		count_orig = dyabs;
		if (dxabs > dyabs)
			count_orig = dxabs;

		/*--------------------- Offsettabelle anlegen -----------*/

		strcounter = count_orig;

		offset_buffer = (long *)Malloc(strcounter * sizeof(*offset_buffer));

		xpos = x1;
		ypos = y1;

		for (strcounter = 0; strcounter < count_orig; strcounter++)
		{
			yoffset = (ypos >> 10) * bytewidth;
			xcalc = (short) (xpos >> 10L);

			offset = yoffset + xcalc + xcalc + xcalc;

			offset_buffer[strcounter] = offset;

			xpos += incx;
			ypos += incy;
		}

		/*--------------------- Hauptschleife --------------------*/

		smurf_struct->services->reset_busybox(0, TEXT4);

		for (y = 0; y < height; y++)
		{
			if ((y & 7) == 0)
				smurf_struct->services->busybox((short) (((long) y << 7L) / (long) height));

			for (x = 0; x < width; x++)
			{
				red = green = blue = 0;

				divide = strcounter = count_orig;

				current_offset = offset_buffer;

				while (strcounter--)
				{
					offset = *(current_offset++);

					read = picdata + offset;
					red += *(read++);
					green += *(read++);
					blue += *(read);
				}

				if (divide == 0)
					divide = 1;
				picdata += 3;

				red /= divide;
				green /= divide;
				blue /= divide;

				*(dest++) = red;
				*(dest++) = green;
				*(dest++) = blue;
			}
		}

		Mfree(pd);
		Mfree(offset_buffer);
		picture->pic_data = dest_data;

		smurf_struct->module_mode = M_PICDONE;
		break;

	case MTERM:
		/* --------------------------------------Mterm empfangen - Speicher freigeben und beenden */
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
