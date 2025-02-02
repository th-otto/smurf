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
/*						90� Drehen-Modul					*/
/* Version 0.1  --  26.06.96								*/
/*	  TC-Bilder only										*/
/* Version 0.2  --  30.06.96								*/
/*	  8 und 16 Bit pp										*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#include "spin.rsh"

#define TIMER 0

MOD_INFO module_info = {
	"Drehen 90�",
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ NULL, NULL, NULL, NULL },
/* 4 Checkboxueberschriften: */
	{ NULL, NULL, NULL, NULL },
/* 4 Edit-Objekt-�berschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	8, 16, 24, 0, 0, 0, 0, 0,
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

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

static WORD (*popup) (POP_UP *popup_struct, WORD mouseflag, WORD button, OBJECT *poptree);

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Drehen um 90�					*/
/*		8, 16 und 24 Bit pp							*/
/*	Dieses Modul ist jetzt schon zum Tode verur-	*/
/*	teilt da es im frei drehen-Modul als Sonderfall	*/
/*	aufgehen wird.									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data1;
	uint8_t *data2;
	uint8_t BitsPerPixel;
	uint8_t bytes;
	WORD t;
	static uint8_t left = 0;
	static uint8_t right = 0;
	static uint8_t two = 0;
	int item;
	unsigned short width, height;
	unsigned short x, y, x2;
	unsigned long pos2, jmp, length;
	OBJECT *pop_form;
	POP_UP pop_up;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		pop_form = rs_trindex[SPIN90];

		popup = smurf_struct->services->popup;

		pop_up.popup_tree = SPIN90;
		pop_up.item = 0;
		pop_up.display_tree = 0;
		pop_up.display_obj = -1;
		pop_up.Cyclebutton = -1;
		item = popup(&pop_up, 1, 1, pop_form);

		switch (item)
		{
		case -1:
			smurf_struct->module_mode = M_EXIT;
			return;
		case LEFT:
			left = 1;
			break;
		case RIGHT:
			right = 1;
			break;
		case TWO:
			two = 1;
			break;
		default:
			break;
		}

		smurf_struct->module_mode = M_STARTED;
		break;

	case MEXEC:
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		bytes = BitsPerPixel >> 3;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		data1 = smurf_struct->smurf_pic->pic_data;
		if ((data2 = SMalloc((unsigned long) width * (unsigned long) height * (long) bytes)) == 0)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}

#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		if (left)
		{
			jmp = (height + 1) * bytes;
			length = ((unsigned long) width - 1) * ((unsigned long) height * bytes);
			y = 0;
			do
			{
				pos2 = length;
				x = 0;
				do
				{
					data2[pos2++] = *data1++;
					if (bytes > 1)
					{
						data2[pos2++] = *data1++;
						if (bytes > 2)
							data2[pos2++] = *data1++;
					}
					pos2 -= jmp;
				} while (++x < width);
				length += bytes;
			} while (++y < height);
		} else if (right)
		{
			jmp = (height - 1) * bytes;
			x2 = (height - 1) * bytes;
			y = 0;
			do
			{
				pos2 = x2;
				x = 0;
				do
				{
					data2[pos2++] = *data1++;
					if (bytes > 1)
					{
						data2[pos2++] = *data1++;
						if (bytes > 2)
							data2[pos2++] = *data1++;
					}
					pos2 += jmp;
				} while (++x < width);
				x2 -= bytes;
			} while (++y < height);
		} else if (two)
		{
			jmp = bytes * 2;
			length = (unsigned long) width *(unsigned long) height;
			pos2 = (unsigned long) width *(unsigned long) height *bytes - bytes;

			do
			{
				data2[pos2++] = *data1++;
				if (bytes > 1)
				{
					data2[pos2++] = *data1++;
					if (bytes > 2)
						data2[pos2++] = *data1++;
				}
				pos2 -= jmp;
			} while (--length);
		}
#if TIMER
		/* wie schnell waren wir? */
		printf("\n%lu", get_timer());
		(void)Cnecin();
#endif

		SMfree(smurf_struct->smurf_pic->pic_data);
		smurf_struct->smurf_pic->pic_data = data2;
		if (!two)
		{
			smurf_struct->smurf_pic->pic_width = height;
			smurf_struct->smurf_pic->pic_height = width;
		}
		smurf_struct->module_mode = M_DONEEXIT;
		break;

		/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
