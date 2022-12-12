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
/*					VDI-/hardwarekonform wandeln			*/
/* Version 0.1  --  11.06.97								*/
/*	  1-8 Bit												*/
/* Version 0.2  --  27.03.98								*/
/*	  Zeilenweise Assemblerleseroutine eingebaut.			*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Konform wandeln"
#define TEXT2 "VDI-konform"
#define TEXT3 "Hardwarekonform"
#elif COUNTRY==0
#define TEXT1 "Conform convert"
#define TEXT2 "VDI-conform"
#define TEXT3 "Hardwareconform"
#elif COUNTRY==2
#define TEXT1 "Conform convert"
#define TEXT2 "VDI-conform"
#define TEXT3 "Hardwareconform"
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

#define VDI		0
#define HARD	1

short (*busybox)(short pos);

MOD_INFO module_info = {
	TEXT1,
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ "", "", "", "" },
/* 4 Checkboxueberschriften: */
	{ TEXT2, TEXT3, "", "" },
/* 4 Edit-Objekt-šberschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max fr Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 2, 3, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	2, 3, 4, 5, 6, 7, 8, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				VDI-/hardwarekonform wandeln		*/
/*		2-8 Bit 									*/
/* Das Modul kann nur Palettenbilder brauchen, wird */
/* aber eigentlich nur ben”tigt, wenn keine Palette */
/* im Bild enthalten ist.							*/
/* In 2, 3 und 4 Bit ist die Umsetzung normiert,	*/
/* was mache ich aber in anderen Farbtiefen?		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	static uint8_t const table2_to_vdi[] = { 0, 2, 3, 1 };
	static uint8_t const table3_to_vdi[] = { 0, 2, 3, 6, 4, 7, 5, 1 };
	static uint8_t const table4_to_vdi[] = { 0, 15, 1, 2, 4, 6, 3, 5, 7, 8, 9, 10, 12, 14, 11, 13 };
	static uint8_t const table2_to_hard[] = { 0, 2, 3, 1 };
	static uint8_t const table3_to_hard[] = { 0, 2, 3, 6, 4, 7, 5, 1 };
	static uint8_t const table4_to_hard[] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };
	uint8_t *data;
	uint8_t *pixbuf;
	uint8_t *line;
	const uint8_t *plane_table;
	uint8_t BitsPerPixel;
	uint8_t mode;

	unsigned short x, y;
	unsigned short width, height;
	unsigned short bh, bl;

	unsigned long planelength;
	unsigned long w;
	uint8_t v;

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
	
		if (smurf_struct->check[0] > smurf_struct->check[1])
			mode = VDI;
		else
			mode = HARD;

		BitsPerPixel = smurf_struct->smurf_pic->depth;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		data = smurf_struct->smurf_pic->pic_data;

		busybox = smurf_struct->services->busybox;

		if (mode == VDI)
		{
			switch (BitsPerPixel)
			{
			case 2:
				plane_table = table2_to_vdi;
				break;
			case 3:
				plane_table = table3_to_vdi;
				break;
			case 4:
				plane_table = table4_to_vdi;
				break;
			default:
				smurf_struct->module_mode = M_INVALID;
				return;
			}
		} else
		{
			switch (BitsPerPixel)
			{
			case 2:
				plane_table = table2_to_hard;
				break;
			case 3:
				plane_table = table3_to_hard;
				break;
			case 4:
				plane_table = table4_to_hard;
				break;
			default:
				smurf_struct->module_mode = M_INVALID;
				return;
			}
		}
		pixbuf = (uint8_t *) Malloc(width + 7);

		if ((bh = height / 10) == 0)	/* busy-height */
			bh = height;
		bl = 0;							/* busy-length */

		if (smurf_struct->smurf_pic->format_type == FORM_PIXELPAK)
		{
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
					v = plane_table[*data];
					*data++ = v;
				} while (++x < width);
			} while (++y < height);
		} else
		{
			w = (unsigned long) ((width + 7) / 8);
			planelength = (unsigned long) ((width + 7) / 8) * (unsigned long) height;	/* L„nge einer Plane in Bytes */

			y = 0;
			do
			{
				if (!(y % bh))
				{
					busybox(bl);
					bl += 12;
				}

				memset(pixbuf, 0, width);
				getpix_std_line(data, pixbuf, BitsPerPixel, planelength, width);
				line = pixbuf;

				x = 0;
				do
				{
					v = plane_table[*line];
					*line++ = v;
				} while (++x < width);
				setpix_std_line(pixbuf, data, BitsPerPixel, planelength, width);
				data += w;
			} while (++y < height);
		}

		Mfree(pixbuf);

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
