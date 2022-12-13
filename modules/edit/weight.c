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
/*						Weight of Color						*/
/* Version 0.1  --  06.11.96								*/
/*	  1-8, 16, 24 Bit										*/
/* Version 0.2  --  23.02.97								*/
/*	  Fuck gebixt, bei 16 Bit wurde aus data und nicht		*/
/*	  data16 ausgelesen -> schwarzes Bild					*/
/* Version 0.3  --  30.07.97								*/
/*	  Ein Wuxel ward mir noch weggeschl�pft. Die Tabellen	*/
/*	  m�ssen lat�rnich immer best�ckt werden.				*/
/* Version 0.4  --  18.08.98								*/
/*	  Da auch �nderungen an monochromen Bildern auffallen	*/
/*	  m�ssen, dort aber eine ge�nderte Palette nichts be-	*/
/*	  wirkt, wird jetzt 2 Bit als kleinste Farbtiefe fest-	*/
/*	  gelegt.												*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Weight of Color"
#define TEXT2 "Rot"
#define TEXT3 "Gr�n"
#define TEXT4 "Blau"
#elif COUNTRY==0
#define TEXT1 "Weight of Color"
#define TEXT2 "Red"
#define TEXT3 "Green"
#define TEXT4 "Blue"
#elif COUNTRY==2
#define TEXT1 "Weight of Color"
#define TEXT2 "Rouge"
#define TEXT3 "Vert"
#define TEXT4 "Bleu"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

MOD_INFO module_info = {
	TEXT1,
	0x0040,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ TEXT2, TEXT3, TEXT4, "" },
	{ "", "", "", "" },
	{ "", "", "",	"" },
	{ { -255, 255 }, { -255, 255 }, { -255, 255 }, { 0, 64 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	2, 4, 7, 8, 16, 24, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Weight of Color					*/
/*		1-8, 16 und 24 Bit							*/
/* Und noch ein Modul mit so einem sch�nen Namen.	*/
/* Es kann hiermit die Gewichtung des Rot-, Gr�n-	*/
/* und Blauanteils ver�ndert werden.				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t r, g, b;
	uint8_t BitsPerPixel;
	WORD transr[256];
	WORD transg[256]; 
	WORD transb[256];
	short i, k;
	short rweight, gweight, bweight;
	uint16_t *data16;
	unsigned short width, height;
	unsigned long length;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde, */
	/* ein Einstellformular anfordern....             */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Einstellformular wurde mit START verlassen - Funktion ausf�hren */
	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		rweight = (short) smurf_struct->slide[0];
		gweight = (short) smurf_struct->slide[1];
		bweight = (short) smurf_struct->slide[2];

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		i = 0;
		do
		{
			k = i + rweight;
			if (k < 0)
				transr[i] = 0;
			else if (k > 255)
				transr[i] = 255;
			else
				transr[i] = k;
		} while (++i < 256);

		i = 0;
		do
		{
			k = i + gweight;
			if (k < 0)
				transg[i] = 0;
			else if (k > 255)
				transg[i] = 255;
			else
				transg[i] = k;
		} while (++i < 256);

		i = 0;
		do
		{
			k = i + bweight;
			if (k < 0)
				transb[i] = 0;
			else if (k > 255)
				transb[i] = 255;
			else
				transb[i] = k;
		} while (++i < 256);


		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				data = smurf_struct->smurf_pic->pic_data;

				length = (unsigned long) width *(unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;

				length = SM_PALETTE_MAX;
			}

			do
			{
				uint8_t v;
				v = transr[*data];
				*data++ = v;
				v = transr[*data];
				*data++ = v;
				v = transr[*data];
				*data++ = v;
			} while (--length);
		} else
		{
			data16 = (uint16_t *) smurf_struct->smurf_pic->pic_data;

			length = (unsigned long) width *(unsigned long) height;

			do
			{
				r = transr[(*data16 & 0xf800) >> 8];
				g = transg[(*data16 & 0x7e0) >> 3];
				b = transb[(*data16 & 0x1f) << 3];

				*data16++ = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
			} while (--length);
		}

#if TIMER
		/* wie schnell waren wir? */
		printf("\n%lu\n", get_timer());
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
