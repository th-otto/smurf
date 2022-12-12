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
/*						Scatter-Modul						*/
/* Version 0.1  --  06.11.96								*/
/*	  8 (PP), 16, 24 Bit									*/
/* Version 0.2  --  11.11.96								*/
/*	  Beschleunigt, aber nicht mehr ganz so zuf„llige		*/
/*	  Verteilung.											*/
/* Version 0.3  --  10.03.97								*/
/*	  Bl”der Fehler beim Einbau der transparenten Pixel auf	*/
/*	  Olafs Zuruf. Da das bei 8 und 16 Bit mehr Aufwand		*/
/*	  w„re, wird nun dort ganz normal verteilt und nur bei	*/
/*	  bei 24 Bit transparent.								*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Breite"
#define TEXT2 "H”he"
#elif COUNTRY==0
#define TEXT1 "Latitude"
#define TEXT2 "Height"
#elif COUNTRY==2
#define TEXT1 "Latitude"
#define TEXT2 "Height"
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
#define random(x) (rand() % (x))

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

short (*busybox)(short pos);

MOD_INFO module_info = {
	"Scatman's World",
	0x0030,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ "", "", "", "" },
	{ "Clean", "", "", "" },
	{ TEXT1, TEXT2, "", "" },
	{ { 0, 64 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { 2, 16 }, { 2, 16 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 3, 3, 0, 0 },
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	8, 16, 24, 0, 0,
	0, 0, 0,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Scatman's World					*/
/*		8 (PP), 16 und 24 Bit 						*/
/* Der Scatter (so heižt das Modul in anderen		*/
/* Programmen) ersetzt die Pixel der Reihe nach		*/
/* durch andere im (in der Gr”že einstellbaren) 	*/
/* umliegenden Bereich zuf„llig ausgew„hlte 		*/
/* Der Clean-Modus holt die Pixel immer aus dem 	*/
/* Quellbild, braucht daher jedoch mehr Speicher.	*/
/* Der Dirty-Modus h„lt nur ein Bild aus dem		*/
/* gelesen und in das geschrieben wird. Braucht		*/
/* weniger Speicher, die Ergebnisse k”nnen speziell	*/
/* bei grožen Quellbereichen jedoch sehr vermatscht	*/
/* aussehen.										*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *ziel;
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t clean;
	uint8_t BitsPerPixel;
	uint8_t pixlen;

	short randwidth, randheight, randwidth2, randheight2;
	unsigned short width, height;
	unsigned short bh, bl;
	unsigned short i;

	long Scatadubbelidau;
	long scatland[512];
	long realwidth;
	long length;
	long lengthtorun;


	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* Wenn das Modul zum ersten Mal gestartet wurde, */
		/* ein Einstellformular anfordern....             */
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		/* Einstellformular wurde mit START verlassen - Funktion ausfhren */
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;

		busybox = smurf_struct->services->busybox;

		smurf_struct->services->reset_busybox(0, "scatten ...");
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		/* Zufallsgenerator zuf„llig initialisieren */
		srand((unsigned int) Random());

		clean = smurf_struct->check[0];
		randwidth = (short) smurf_struct->edit[0];
		randwidth2 = randwidth / 2;
		randheight = (short) smurf_struct->edit[1];
		randheight2 = randheight / 2;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		ziel = smurf_struct->smurf_pic->pic_data;

		if (clean)
		{
			if ((buffer = SMalloc(((unsigned long) width * (unsigned long) height * BitsPerPixel) >> 3)) == 0)
			{
				smurf_struct->module_mode = M_PICDONE;
				return;
			}
			memcpy(buffer, ziel, ((unsigned long) width * (unsigned long) height * BitsPerPixel) >> 3);
		} else
			buffer = ziel;

		obuffer = buffer;

		pixlen = BitsPerPixel >> 3;

		realwidth = (unsigned long) width * (unsigned long) pixlen;

		length = (unsigned long) width * (unsigned long) height;

		if ((bh = (short) (length / 20)) == 0)	/* busy-height */
			bh = (short) length;
		bl = 0;							/* busy-length */

		i = 0;
		do
		{
			scatland[i] =
				(long) (random(randheight) - randheight2) * realwidth + (long) (random(randwidth) -
																				randwidth2) * pixlen;
		} while (++i < 512);


		lengthtorun = length;

		if (BitsPerPixel == 24)
		{
			do
			{
				if (!(lengthtorun % bh))
				{
					busybox(bl);
					bl += 6;
				}

				Scatadubbelidau = scatland[random(512)];
				if (Scatadubbelidau < lengthtorun && Scatadubbelidau > (lengthtorun - length))
				{
					uint8_t v;
					v = *ziel;
					*ziel++ = (*(buffer + 0 + Scatadubbelidau) + v) >> 1;
					v = *ziel;
					*ziel++ = (*(buffer + 1 + Scatadubbelidau) + v) >> 1;
					v = *ziel;
					*ziel++ = (*(buffer + 2 + Scatadubbelidau) + v) >> 1;
				} else
				{
					ziel += pixlen;
				}
				buffer += pixlen;
			} while (--lengthtorun);
		} else
		{
			do
			{
				if (!(lengthtorun % bh))
				{
					busybox(bl);
					bl += 6;
				}

				Scatadubbelidau = scatland[random(512)];
				if (Scatadubbelidau < lengthtorun && Scatadubbelidau > (lengthtorun - length))
				{
					*ziel++ = *(buffer + Scatadubbelidau);

					if (BitsPerPixel == 16)
						*ziel++ = *(buffer + 1 + Scatadubbelidau);
				} else
					ziel += pixlen;

				buffer += pixlen;
			} while (--lengthtorun);
		}

		buffer = obuffer;
		if (clean)
			SMfree(buffer);

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_PICDONE;
		break;

	case MTERM:
		/* Mterm empfangen - Speicher freigeben und beenden */
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
