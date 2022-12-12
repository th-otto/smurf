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

/*  ------------- Smooth-Modul V0.5 ------------------  */
/*          FÅr SMURF Bildkonverter, 26.04.96           */

#include "country.h"

#if COUNTRY == 1
#define TEXT1 "Weichzeichnen"
#define TEXT2 "Weichzeichnen..."
#define TEXT3 "St\204rke"
#define TEXT4 "Bildanpassung..."
#elif COUNTRY == 0
#define TEXT1 "Blur"
#define TEXT2 "Blurring..."
#define TEXT3 "Strength"
#define TEXT4 "Picture adjust..."
#else
#define TEXT1 "Blur"
#define TEXT2 "Blurring..."
#define TEXT3 "Strength"
#define TEXT4 "Picture adjust..."
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"


/*------ Infostruktur fÅr Hauptprogramm -----*/
MOD_INFO module_info = {
	TEXT1,
	0x0100,
	"Olaf Piesche",		/* Author */
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ TEXT3, "", "", "" },
/* 4 Checkboxueberschriften: */
	{ "", "", "", "" },
/* 4 Edit-Objekt-Ueberschriften: */
	{ "", "", "", "" },
/* min/max-Werte fÅr Slider */
	{ { 1, 15 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max fÅr Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fÅr Slider, Check und Edit */
	{ 3, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
/* Bezeichnung fuer Bilder */
	{ "", NULL, NULL, NULL, NULL, NULL }
};


/*--------------------- Was kann ich ? ----------------------*/
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
	0
};


struct smooth_par {
	long counter;
	uint8_t *src;
	uint8_t *dst;
	long *offset_table;
};


void smooth_me(struct smooth_par *par) ASM_NAME("_smooth_me");


/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	void *(*SMalloc)(long amount);
	void (*SMfree)(void *ptr);
	short width, height;
	short x;
	short y;
	uint8_t *data;
	uint8_t *datacopy;
	uint8_t *dest_data;
	uint8_t *dest;

	long linelen;
	long *offset_table;
	long counter;
	long curr_counter;
	long offset;
	long old_offset;
	short matw;
	short math;
	short xpos;
	short ypos;
	struct smooth_par par;
	short w;
	long red;
	long green;
	long blue;
	
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* Wenn das Modul aufgerufen wurde, */
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		/* Wenn das Modul gestartet wurde */
		picture = smurf_struct->smurf_pic;
		data = picture->pic_data;
		width = picture->pic_width;
		height = picture->pic_height;

		datacopy = data;
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;
		
		linelen = (long) width * 3L;
		
		dest_data = dest = (uint8_t *)SMalloc((long) height * linelen);
		if (dest_data == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}
		memset(dest_data, 0, (long) height * linelen);
		
		/*-------------------- Offset-Table vorbereiten --------------  */
		/*--------------------- Delta-Table (schnell!)  --------------- */
		matw = (short) smurf_struct->slide[0];
		math = matw;

		counter = ((long) matw * 2 + 1) * ((long) math * 2 + 1);

		offset_table = (long *)SMalloc(counter * sizeof(*offset_table) + 100);
		if (offset_table == NULL)
		{
			SMfree(dest);
			smurf_struct->module_mode = M_MEMORY;
			return;
		}
		curr_counter = 0;
		old_offset = 0;

		for (y = -math; y <= math; y++)
		{
			for (x = -matw; x <= matw; x++)
			{
				offset = ((long) y * linelen) + (x * 3L);

				offset_table[curr_counter] = offset - old_offset;

				/* Deltaoffset -2 ( wegen move.b (a1)+ ) */
				old_offset = offset + 2;

				curr_counter++;
			}
		}

		smurf_struct->services->reset_busybox(0, TEXT2);

		w = matw;
		par.counter = counter;
		par.offset_table = offset_table;

		for (y = math; y < height - math; y++)
		{
			par.src = datacopy + y * linelen + w * 3L;
			par.dst = dest + y * linelen + w * 3L;
			
			if ((y & 15) == 0)
				smurf_struct->services->busybox((short) (((long) y << 7L) / (long) height));

			for (x = matw; x < width - matw; x++)
			{
				smooth_me(&par);
				par.dst += 3;
				par.src += 3;
			}
		}

		smurf_struct->services->reset_busybox(0, TEXT4);

		for (y = 0; y < math; y++)
		{
			datacopy = data + y * linelen;
			dest = dest_data + y * linelen;
			for (x = 0; x < width; x++)
			{
				red = green = blue = 0;
				counter = 0;
				for (ypos = -y; ypos <= math; ypos++)
				{
					for (xpos = -matw; xpos <= matw; xpos++)
					{
						if (x + xpos > 0 && x + xpos < width)
						{
							red += datacopy[ypos * linelen + xpos * 3L + 0];
							green += datacopy[ypos * linelen + xpos * 3L + 1];
							blue += datacopy[ypos * linelen + xpos * 3L + 2];
							counter++;
						}
					}
				}
				if (counter != 0)
				{
					red /= counter;
					green /= counter;
					blue /= counter;
				}
				dest[0] = red;
				dest[1] = green;
				dest[2] = blue;
				datacopy += 3;
				dest += 3;
			}
		}

		for (y = 0; y < math; y++)
		{
			datacopy = data + y * linelen + (height - math) * linelen;
			dest = dest_data + y * linelen + (height - math) * linelen;
			for (x = 0; x < width; x++)
			{
				red = green = blue = 0;
				counter = 0;
				for (ypos = -math; ypos < math - y; ypos++)
				{
					for (xpos = -matw; xpos <= matw; xpos++)
					{
						if (x + xpos > 0 && x + xpos < width)
						{
							red += datacopy[ypos * linelen + xpos * 3L + 0];
							green += datacopy[ypos * linelen + xpos * 3L + 1];
							blue += datacopy[ypos * linelen + xpos * 3L + 2];
							counter++;
						}
					}
				}
				if (counter != 0)
				{
					red /= counter;
					green /= counter;
					blue /= counter;
				}
				dest[0] = red;
				dest[1] = green;
				dest[2] = blue;
				datacopy += 3;
				dest += 3;
			}
		}
		
		for (y = 0; y < height; y++)
		{
			datacopy = data + y * linelen;
			dest = dest_data + y * linelen;
			for (x = 0; x < matw; x++)
			{
				red = green = blue = 0;
				counter = 0;
				for (ypos = -math; ypos <= math; ypos++)
				{
					for (xpos = -x; xpos <= matw; xpos++)
					{
						if (y + ypos > 0 && y + ypos < height)
						{
							red += datacopy[ypos * linelen + xpos * 3L + 0];
							green += datacopy[ypos * linelen + xpos * 3L + 1];
							blue += datacopy[ypos * linelen + xpos * 3L + 2];
							counter++;
						}
					}
				}
				if (counter != 0)
				{
					red /= counter;
					green /= counter;
					blue /= counter;
				}
				dest[0] = red;
				dest[1] = green;
				dest[2] = blue;
				datacopy += 3;
				dest += 3;
			}
		}

		for (y = 0; y < height; y++)
		{
			datacopy = data + y * linelen + (width - matw) * 3L;
			dest = dest_data + y * linelen + (width - matw) * 3L;
			for (x = 0; x < matw; x++)
			{
				red = green = blue = 0;
				counter = 0;
				for (ypos = -math; ypos <= math; ypos++)
				{
					for (xpos = -matw; xpos < matw - x; xpos++)
					{
						if (y + ypos > 0 && y + ypos < height)
						{
							red += datacopy[ypos * linelen + xpos * 3L + 0];
							green += datacopy[ypos * linelen + xpos * 3L + 1];
							blue += datacopy[ypos * linelen + xpos * 3L + 2];
							counter++;
						}
					}
				}
				if (counter != 0)
				{
					red /= counter;
					green /= counter;
					blue /= counter;
				}
				dest[0] = red;
				dest[1] = green;
				dest[2] = blue;
				datacopy += 3;
				dest += 3;
			}
		}

		SMfree(smurf_struct->smurf_pic->pic_data);
		smurf_struct->smurf_pic->pic_data = dest_data;
		SMfree(offset_table);

		smurf_struct->module_mode = M_PICDONE;
		break;

	case MTERM:
		/* Wenn das Modul sich verpissen soll */
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;
		break;
	}
}
