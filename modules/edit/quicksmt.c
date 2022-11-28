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
#define TEXT4 "Bildanpassung..."
#else
#define TEXT1 "Blur"
#define TEXT2 "Blurring..."
#define TEXT3 "Strength"
#define TEXT4 "Bildanpassung..."
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
	TEXT3,
	"",
	"",
	"",
/* 4 Checkboxueberschriften: */
	"",
	"",
	"",
	"",
/* 4 Edit-Objekt-Ueberschriften: */
	"",
	"",
	"",
	"",
/* min/max-Werte fÅr Slider */
	1, 15,
	0, 0,
	0, 0,
	0, 0,
/* min/max fÅr Editobjekte */
	0, 0,
	0, 0,
	0, 0,
	0, 0,
/* Defaultwerte fÅr Slider, Check und Edit */
	3, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
/* Anzahl der Bilder */
	1,
/* Bezeichnung fuer Bilder */
	"", NULL, NULL, NULL, NULL, NULL
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



void smooth_me(long *par) ASM_NAME("_smooth_me");


/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture; /* a0 */
	void *(*SMalloc)(long amount); /* 88 */
	void (*SMfree)(void *ptr); /* 84 */
	short module_id; /* 82 */
	short width, height; /* 80, 78 */
	short x, y; /* 76, 74 */
	uint8_t *data; /* 72 */
	uint8_t *datacopy; /* a4 */
	uint8_t *dest_data; /* 68 */
	uint8_t *dest; /* a3 */

	/* long red, green, blue; */
	long linelen; /* d6 */
	long *offset_table; /* 64 */
	long counter; /* 60 */
	long curr_counter; /* 56 */
	long offset;
	long old_offset; /* d5 */
	short matw, math; /* 54, 52 */
	short ypos, xpos;
	long par[10];
	long bpp;
	
	bpp = 3;
	/* Wenn das Modul aufgerufen wurde, */
	if (smurf_struct->module_mode == MSTART)
	{
		module_id = smurf_struct->module_number;
		smurf_struct->services->f_module_prefs(&module_info, module_id);
		smurf_struct->module_mode = M_WAITING;
		return;
	}

	/* Wenn das Modul gestartet wurde */
	if (smurf_struct->module_mode == MEXEC)
	{
		picture = smurf_struct->smurf_pic;
		data = picture->pic_data;
		width = picture->pic_width;
		height = picture->pic_height;

		datacopy = data;
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;
		
		linelen = (long) width * bpp;
		
		dest_data = dest = (uint8_t *)SMalloc((long) height * linelen);
		memset(dest_data, '*', (long) height * linelen);
		
		/*-------------------- Offset-Table vorbereiten --------------  */
		/*--------------------- Delta-Table (schnell!)  --------------- */
		matw = (short) smurf_struct->slide1;
		math = matw;

		counter = ((long) matw + 1) * ((long) math + 1);

		offset_table = (long *)SMalloc(counter * sizeof(*offset_table) + 100);
		curr_counter = 0;
		old_offset = 0;

		x = -matw;
		for (y = 0; y < math; y++)
		{
			for (x = 0; x < matw; x++)
			{
				xpos = x - (matw / 2);
				ypos = y - (math / 2);

				offset = ((long) ypos * linelen) + (xpos * 3L);

				offset_table[curr_counter] = offset - old_offset;

				/* Deltaoffset -2 ( wegen move.b (a1)+ ) */
				old_offset = offset + 2;

				curr_counter++;
			}
		}

		smurf_struct->services->reset_busybox(0, TEXT2);

		par[0] = counter;
		par[1] = (long) datacopy;
		par[2] = (long) dest;
		par[3] = (long) offset_table;

		for (y = 0; y < height; y++)
		{
			if ((y & 15) == 0)
				smurf_struct->services->busybox((short) (((long) y << 7L) / (long) height));

			for (x = 0; x < width; x++)
			{
				smooth_me(par);
				par[2] += 3;
				par[1] += 3;
			}
		}


		SMfree(offset_table);
		SMfree(data);

		smurf_struct->smurf_pic->pic_data = dest_data;
		smurf_struct->module_mode = M_PICDONE;

		return;
	}

	/* Wenn das Modul sich verpissen soll */
	if (smurf_struct->module_mode == MTERM)
	{
		smurf_struct->module_mode = M_EXIT;
		return;
	}

	smurf_struct->module_mode = M_WAITING;
}
