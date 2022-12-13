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
/*							Wind-Modul						*/
/* Version 0.1  --  22.09.97								*/
/*	  24 Bit, eine St„rke und nur von links					*/
/* Version 0.2  --  29.09.97								*/
/*	  Nun auch von rechts und in beiden Richtungen in zwei	*/
/*	  St„rken.												*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Wind von links"
#define TEXT2 "Wind von rechts"
#define TEXT3 "Sturm von links"
#define TEXT4 "Sturm von rechts"
#elif COUNTRY==0
#define TEXT1 "Wind from left"
#define TEXT2 "Wind from right"
#define TEXT3 "Storm from left"
#define TEXT4 "Storm from right"
#elif COUNTRY==2
#define TEXT1 "Wind from left"
#define TEXT2 "Wind from right"
#define TEXT3 "Storm from left"
#define TEXT4 "Storm from right"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

#undef random
#define random( x ) (rand() % (x))

#define WIND	1
#define STORM	2
#define LINKS	1
#define RECHTS	2

MOD_INFO module_info = {
	"Wind",
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ "", "", "", "" },
	{ TEXT1, TEXT2, TEXT3, TEXT4 },
	{ "", "", "", "" },
	{ { 0, 64 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 2, 3, 3, 3 },
	{ 0, 0, 0, 0 },
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0,	0, 0, 0,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};




/* bl„st von links sanft durchs Bild */
static void wind_left(uint8_t *data, unsigned short width, unsigned short height)
{
	uint8_t r, g, b;
	uint8_t init_pix, this_pix;
	short intens;
	unsigned short x, y, daempfung;
	uint8_t v;

	y = height;
	while (y--)
	{
		init_pix = (uint8_t) ((((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12);

		r = *data++;
		g = *data++;
		b = *data++;

		/* Wert fr ersten Start initialisieren */
		intens = 95;
		daempfung = (random(45) >> 1) + 3;

		this_pix = (uint8_t) ((((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12);

		x = width - 1;
		while (x--)
		{
			while (init_pix > this_pix && intens > 0 && x)
			{
				v = ((r * intens + *data * (128 - intens)) >> 7);
				*data++ = v;
				v = ((g * intens + *data * (128 - intens)) >> 7);
				*data++ = v;
				v = ((b * intens + *data * (128 - intens)) >> 7);
				*data++ = v;
				x--;

				intens -= daempfung;

				this_pix = (uint8_t) ((((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12);
			}

			data -= 3;
			x++;

			do
			{
				data += 3;
				x--;

				init_pix = this_pix;

				this_pix = (uint8_t) ((((long) *(data + 3) * 872L) + ((long) *(data + 4) * 2930L) + ((long) *(data + 5) * 296L)) >> 12);
			} while (init_pix <= this_pix && x);

			r = *data++;
			g = *data++;
			b = *data++;

			/* Wert fr neuen Start initialisieren */
			intens = 95;
			daempfung = (random(45) >> 1) + 3;
		}
	}
}


/* bl„st von rechts sanft durchs Bild */
static void wind_right(uint8_t *data, unsigned short width, unsigned short height)
{
	char r, g, b;
	uint8_t init_pix, this_pix;
	short intens;
	unsigned short x, y, daempfung;
	uint8_t v;

	/* auf Blaukomponente des letzten Bildpixels setzen */
	data += (long) width * (long) height * 3L - 1;

	y = height;
	while (y--)
	{
		init_pix = (uint8_t) ((((long) *(data - 2) * 872L) + ((long) *(data - 1) * 2930L) + ((long) *data * 296L)) >> 12);

		b = *data--;
		g = *data--;
		r = *data--;

		/* Wert fr ersten Start initialisieren */
		intens = 95;
		daempfung = (random(45) >> 1) + 3;

		this_pix = (uint8_t) ((((long) *(data - 2) * 871L) + ((long) *(data - 1) * 2929L) + ((long) *data * 295L)) >> 12);

		x = width - 1;
		while (x--)
		{
			intens -= daempfung;

			while (init_pix > this_pix && intens > 0 && x)
			{
				v = ((b * intens + *data * (128 - intens)) >> 7);
				*data-- = v;
				v = ((g * intens + *data * (128 - intens)) >> 7);
				*data-- = v;
				v = ((r * intens + *data * (128 - intens)) >> 7);
				*data-- = v;
				x--;

				intens -= daempfung;

				this_pix = (uint8_t) ((((long) *(data - 2) * 872L) + ((long) *(data - 1) * 2930L) + ((long) *data * 296L)) >> 12);
			}

			data += 3;
			x++;

			do
			{
				data -= 3;
				x--;

				init_pix = this_pix;

				this_pix = (uint8_t) ((((long) *(data - 5) * 872L) + ((long) *(data - 4) * 2930L) + ((long) *(data - 3) * 296L)) >> 12);
			} while (init_pix <= this_pix && x);

			b = *data--;
			g = *data--;
			r = *data--;

			/* Wert fr neuen Start initialisieren */
			intens = 95;
			daempfung = (random(45) >> 1) + 3;
		}
	}
}


/* bl„st von links kr„ftig durchs Bild */
static void storm_left(uint8_t *data, unsigned short width, unsigned short height)
{
	uint8_t r, g, b;
	uint8_t init_pix, this_pix;
	short intens, intens2;
	unsigned short x, y, daempfung;
	uint8_t v;

	y = height;
	while (y--)
	{
		init_pix = (uint8_t) ((((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12);

		r = *data++;
		g = *data++;
		b = *data++;

		/* Wert fr ersten Start initialisieren */
		intens2 = intens = 105;
		daempfung = (random(45) >> 1) + 3;

		this_pix = (uint8_t) ((((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12);

		x = width - 1;
		while (x--)
		{
			while (init_pix > this_pix && intens > 0 && x)
			{
				v = ((r * intens2 + *data * (128 - intens2)) >> 7);
				*data++ = v;
				v = ((g * intens2 + *data * (128 - intens2)) >> 7);
				*data++ = v;
				v = ((b * intens2 + *data * (128 - intens2)) >> 7);
				*data++ = v;
				x--;

				intens -= daempfung;

				this_pix = (uint8_t) ((((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12);
			}

			data -= 3;
			x++;

			do
			{
				data += 3;
				x--;

				init_pix = this_pix;

				this_pix = (uint8_t) ((((long) *(data + 3) * 872L) + ((long) *(data + 4) * 2930L) + ((long) *(data + 5) * 296L)) >> 12);
			} while (init_pix <= this_pix && x);

			r = *data++;
			g = *data++;
			b = *data++;

			/* Wert fr neuen Start initialisieren */
			intens2 = intens = 105;
			daempfung = (random(45) >> 1) + 3;
		}
	}
}


/* bl„st von rechts kr„ftig durchs Bild */
static void storm_right(uint8_t *data, unsigned short width, unsigned short height)
{
	uint8_t r, g, b;
	uint8_t init_pix, this_pix;
	short intens, intens2;
	unsigned short x, y, daempfung;
	uint8_t v;

	/* auf Blaukomponente des letzten Bildpixels setzen */
	data += (long) width * (long) height * 3L - 1;

	y = height;
	while (y--)
	{
		init_pix = (uint8_t) ((((long) *(data - 2) * 872L) + ((long) *(data - 1) * 2930L) + ((long) *data * 296L)) >> 12);

		b = *data--;
		g = *data--;
		r = *data--;

		/* Wert fr ersten Start initialisieren */
		intens2 = intens = 105;
		daempfung = (random(45) >> 1) + 3;

		this_pix = (uint8_t) ((((long) *(data - 2) * 872L) + ((long) *(data - 1) * 2930L) + ((long) *data * 296L)) >> 12);

		x = width - 1;
		while (x--)
		{
			intens -= daempfung;

			while (init_pix > this_pix && intens > 0 && x)
			{
				v = ((b * intens2 + *data * (128 - intens2)) >> 7);
				*data-- = v;
				v = ((g * intens2 + *data * (128 - intens2)) >> 7);
				*data-- = v;
				v = ((r * intens2 + *data * (128 - intens2)) >> 7);
				*data-- = v;
				x--;

				intens -= daempfung;

				this_pix = (uint8_t) ((((long) *(data - 2) * 872L) + ((long) *(data - 1) * 2930L) + ((long) *data * 296L)) >> 12);
			}

			data += 3;
			x++;

			do
			{
				data -= 3;
				x--;

				init_pix = this_pix;

				this_pix = (uint8_t) ((((long) *(data - 5) * 872L) + ((long) *(data - 4) * 2930L) + ((long) *(data - 3) * 296L)) >> 12);
			} while (init_pix <= this_pix && x);

			b = *data--;
			g = *data--;
			r = *data--;

			/* Wert fr neuen Start initialisieren */
			intens2 = intens = 105;
			daempfung = (random(45) >> 1) + 3;
		}
	}
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*						Wind						*/
/*	 Macht m„chtig Wind um nichts, s. Photoshop		*/
/*		24 Bit 										*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	unsigned short width, height;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde, */
	/* ein Einstellformular anfordern....             */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Einstellformular wurde mit START verlassen - Funktion ausfhren */
	case MEXEC:
		data = smurf_struct->smurf_pic->pic_data;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		/* Zufallsgenerator zuf„llig initialisieren */
		srand((unsigned int) Random());

		if (smurf_struct->check[0])
			wind_left(data, width, height);
		else if (smurf_struct->check[1])
			wind_right(data, width, height);
		else if (smurf_struct->check[2])
			storm_left(data, width, height);
		else
			storm_right(data, width, height);

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_PICDONE;
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
