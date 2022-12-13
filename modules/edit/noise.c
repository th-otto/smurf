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
/*					Verrauschen-Modul						*/
/* Version 0.1  --  14.02.97								*/
/*	  24 Bit, grau und farbig								*/
/* Version 0.2  --  13.10.97								*/
/*	  Es werden nicht mehr nur positive Rauschwerte erzeugt.*/
/*	  Beschleunigung um Faktor 2,5							*/
/* Version 0.3  --  17.10.97								*/
/*	  FarbkanÑle unabhÑngig voneinander verrauschen und		*/
/*	  nochmalige Beschleunigung um Faktor 2 - 2,5 durch		*/
/*	  Cliptables											*/
/* Version 0.4  --  09.05.99								*/
/*	  Mehr kommentiert.										*/
/* Version 0.5  --  xx.xx.99								*/
/*	  16 Bit												*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Noise"
#define TEXT2 "StÑrke Grau"
#define TEXT3 "StÑrke R"
#define TEXT4 "StÑrke G"
#define TEXT5 "StÑrke B"
#define TEXT6 "Grau"
#define TEXT7 "Farbig"
#elif COUNTRY==0
#define TEXT1 "Noise"
#define TEXT2 "Grey amount"
#define TEXT3 "R amount"
#define TEXT4 "G amount"
#define TEXT5 "B amount"
#define TEXT6 "Grey"
#define TEXT7 "Colored"
#elif COUNTRY==2
#define TEXT1 "Noise"
#define TEXT2 "Grey amount"
#define TEXT3 "R amount"
#define TEXT4 "G amount"
#define TEXT5 "B amount"
#define TEXT6 "Grey"
#define TEXT7 "Colored"
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

static short (*busybox)(short pos);

MOD_INFO module_info = {
	TEXT1,
	0x0040,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ TEXT2, TEXT3, TEXT4, TEXT5 },
	{ TEXT6, TEXT7, "", "" },
	{ "", "", "", "" },
	{ { 0, 100 }, { 0, 100 }, { 0, 100 }, { 0, 100 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 20, 20, 20, 20 },
	{ 4, 3, 3, 3 },
	{ 0, 0, 0, 0 },
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,
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


static void noise_grey(uint8_t *data, short width, short height, short amountgrey)
{
	uint8_t cliptable_grey[512];
	uint8_t newpix;
	short plusval;
	unsigned short x, y;
	unsigned short bh, bl;
	unsigned short i, j, k;


	/* amountgrey auf Wert zwischen 0 und 16000 hochschieben */
	amountgrey <<= 5;

	/* k auf Wert zwischen 0 und 250 runterschieben */
	k = amountgrey >> 7;

	/* erstellen der Cliptable */
	for (i = 0; i < k; i++)
		cliptable_grey[i] = 0;
	for (k += 255, j = 0; i < k; i++, j++)
		cliptable_grey[i] = j;
	for (k += amountgrey >> 7; i < k; i++)
		cliptable_grey[i] = 255;


	if ((bh = height / 10) == 0)		/* busy-height */
		bh = height;
	bl = 0;								/* busy-length */

	y = 0;
	do
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 12;
		}

		x = 0;
		do
		{
			/* plusval aus dem Bereich von 0 bis 250 erzeugen. */
			/* Damit werden zwar die neuen Pixel nur um */
			/* +/- 125 bewegt, aber das langt IMHO. */
			plusval = (random(amountgrey) >> 6);

			newpix = cliptable_grey[*data + plusval];
			*data++ = newpix;

			newpix = cliptable_grey[*data + plusval];
			*data++ = newpix;

			newpix = cliptable_grey[*data + plusval];
			*data++ = newpix;
		} while (++x < width);
	} while (++y < height);
}


static void noise_color(uint8_t *data, short width, short height, short amountr, short amountg, short amountb)
{
	uint8_t cliptable_r[512];
	uint8_t cliptable_g[512];
	uint8_t cliptable_b[512];
	uint8_t newpix;
	unsigned short x, y;
	unsigned short bh, bl;
	unsigned short i, j, k;


	amountr <<= 5;
	k = amountr >> 7;

	/* Erstellen der Cliptable fÅr Rot */
	for (i = 0; i < k; i++)
		cliptable_r[i] = 0;
	for (k += 255, j = 0; i < 255; i++, j++)
		cliptable_r[i] = j;
	for (k += amountr >> 7; i < k; i++)
		cliptable_r[i] = 255;


	amountg <<= 5;
	k = amountg >> 7;

	/* Erstellen der Cliptable fÅr GrÅn */
	for (i = 0; i < k; i++)
		cliptable_g[i] = 0;
	for (k += 255, j = 0; i < 255; i++, j++)
		cliptable_g[i] = j;
	for (k += amountg >> 7; i < k; i++)
		cliptable_g[i] = 255;


	amountb <<= 5;
	k = amountb >> 7;

	/* Erstellen der Cliptable fÅr Blau */
	for (i = 0; i < k; i++)
		cliptable_b[i] = 0;
	for (k += 255, j = 0; i < 255; i++, j++)
		cliptable_b[i] = j;
	for (k += amountb >> 7; i < k; i++)
		cliptable_b[i] = 255;


	if ((bh = height / 10) == 0)		/* busy-height */
		bh = height;
	bl = 0;								/* busy-length */

	y = 0;
	do
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 12;
		}

		x = 0;
		do
		{
			newpix = cliptable_r[*data + (random(amountr) >> 6)];
			*data++ = newpix;

			newpix = cliptable_g[*data + (random(amountg) >> 6)];
			*data++ = newpix;

			newpix = cliptable_b[*data + (random(amountb) >> 6)];
			*data++ = newpix;
		} while (++x < width);
	} while (++y < height);
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Verrauschen						*/
/*		24 Bit										*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t mode;
	short amountgrey, amountr, amountg, amountb;
	unsigned short width, height;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;
	
	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		busybox = smurf_struct->services->busybox;

		if (smurf_struct->check[0] > smurf_struct->check[1])
			mode = 0;
		else
			mode = 1;
		/* mit den Shiftings 5 Bit hoch und 6 Bit herunter */
		/* ergibt sich somit eine Skalierung von 100 auf 250 */
		amountgrey = ((short) smurf_struct->slide[0] * 5);
		amountr = ((short) smurf_struct->slide[1] * 5);
		amountg = ((short) smurf_struct->slide[2] * 5);
		amountb = ((short) smurf_struct->slide[3] * 5);

		/* Zufallsgenerator zufÑllig initialisieren */
		srand((unsigned int)Random());

		/* BitsPerPixel = smurf_struct->smurf_pic->depth; */

		data = smurf_struct->smurf_pic->pic_data;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		if (mode == 0)
			noise_grey(data, width, height, amountgrey);
		else
			noise_color(data, width, height, amountr, amountg, amountb);

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
