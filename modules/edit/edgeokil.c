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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/*  ------------- Edge-O-Kill-Modul V1.1 -------------  */
/*          F�r SMURF Bildkonverter, 26.04.96           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/*--------------- Funktionen -------------------*/
/*----------------------------------------------*/
/*-----> Smurf --------*/
void prev(SMURF_PIC * smurfpic, SMURF_PIC * preview);

/*-----> Modul --------*/


/*--------------- Globale Variablen --------------------------*/
static unsigned long busycount;
static unsigned long busymax;
static unsigned long busycall;


/*--------------- Infostruktur f�r Hauptprogramm -----*/
MOD_INFO module_info = {
	"Edge-O-Kill",						/* Name des Moduls */
	0x0110,
	"J�rg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen f�r Importer */
/* 4 Slider�berschriften: max 8 */
	{ "X-Blend %", "Y-Blend %", "", "" },
/* 4 Checkbox�berschriften: */
	{ "Use-Res", "", "", "" },
/* 4 Edit-Objekt-�berschriften: */
	{ "X-Pixel", "Y-Pixel", "", "" },
/* min/max-Werte f�r Slider */
	{ { 0, 100 }, { 0, 100 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 0, 32768L }, { 0, 32768L }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte f�r Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	COMPILER_ID,
	{ "Bild 1", NULL, NULL, NULL, NULL, NULL }
};


/*--------------------- Was kann ich ? ----------------------*/
MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,			/* Farbtiefen */
	/* Grafikmodi: */
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0									/* Extra-Flag */
};





/*--------------- SCALING ----------------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short x_fak, y_fak;
	short x_res, y_res;
	short blend_x, blend_y;
	short width, height;
	uint8_t *pic;
	uint8_t *offset_a;
	uint8_t *offset_b;
	long red_a, green_a, blue_a;
	long red_b, green_b, blue_b;
	long bpl;
	unsigned short step, fak_a;
	unsigned short fak_a1, fak_a2;
	short x, y;

	/*--- Slider auslesen ---------------------- */

	x_fak = (int) smurf_struct->slide[0];
	y_fak = (int) smurf_struct->slide[1];
	x_res = (int) smurf_struct->edit[0];
	y_res = (int) smurf_struct->edit[1];

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	bpl = (long) width *3L;

	/*--- �berblendbereich festlegen ----------------*/
	if (smurf_struct->check[0] != 0)
	{
		blend_x = x_res;
		blend_y = y_res;
	} else
	{
		blend_x = (int) ((long) x_fak * (long) width / 200);
		blend_y = (int) ((long) y_fak * (long) height / 200);
	}

	/*--- Kanten-Bereich Clippen --------------------*/
	if (blend_x > (width >> 1))
		blend_x = (width >> 1);
	if (blend_y > (height >> 1))
		blend_y = (height >> 1);



	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 1;
	busymax = blend_x + blend_y;
	if (busymax > 32)
		busycall = 7;
	if (busymax > 256)
		busycall = 31;
	busymax = (busymax << 10) / 127;


	/*--- Hauptroutine --------------------*/

	if (blend_x != 0)  /*--- X-Kanten �berblend ----*/
	{
		step = (unsigned int) (0x10000L / ((((long) blend_x << 1)) + 1));
		fak_a = 0x8000 + (step >> 1);

		fak_a1 = fak_a;
		fak_a2 = (int) (0x10000L - fak_a);

		for (x = 0; x < blend_x; x++)
		{
			busycount++;
			if (!(busycount & busycall))
				smurf_struct->services->busybox((int) ((busycount << 10) / busymax));

			offset_a = pic + x + x + x;
			offset_b = pic + (long) (width - 1 - x) * 3L;
			for (y = 0; y < height; y++)
			{
				red_a = *(offset_a++);
				green_a = *(offset_a++);
				blue_a = *(offset_a);
				offset_a -= 2;

				red_b = *(offset_b++);
				green_b = *(offset_b++);
				blue_b = *(offset_b);
				offset_b -= 2;

				*(offset_a++) = (char) ((red_a * fak_a1 + red_b * fak_a2) >> 16);
				*(offset_a++) = (char) ((green_a * fak_a1 + green_b * fak_a2) >> 16);
				*(offset_a) = (char) ((blue_a * fak_a1 + blue_b * fak_a2) >> 16);
				offset_a += (bpl - 2);

				*(offset_b++) = (char) ((red_a * fak_a2 + red_b * fak_a1) >> 16);
				*(offset_b++) = (char) ((green_a * fak_a2 + green_b * fak_a1) >> 16);
				*(offset_b) = (char) ((blue_a * fak_a2 + blue_b * fak_a1) >> 16);
				offset_b += (bpl - 2);

			}
			fak_a1 += step;
			fak_a2 -= step;
		}
	}

	if (blend_y != 0)  /*--- Y-Kanten �berblend ----*/
	{
		step = (unsigned int) (0x10000L / ((((long) blend_y << 1)) + 1));
		fak_a = 0x8000 + (step >> 1);

		fak_a1 = fak_a;
		fak_a2 = (int) (0x10000L - fak_a);

		for (y = 0; y < blend_y; y++)
		{
			busycount++;
			if (!(busycount & busycall))
				smurf_struct->services->busybox((int) ((busycount << 10) / busymax));

			offset_a = pic + y * bpl;
			offset_b = pic + (long) (height - 1 - y) * bpl;
			for (x = 0; x < width; x++)
			{
				red_a = *(offset_a++);
				green_a = *(offset_a++);
				blue_a = *(offset_a);
				offset_a -= 2;

				red_b = *(offset_b++);
				green_b = *(offset_b++);
				blue_b = *(offset_b);
				offset_b -= 2;

				*(offset_a++) = (char) ((red_a * fak_a1 + red_b * fak_a2) >> 16);
				*(offset_a++) = (char) ((green_a * fak_a1 + green_b * fak_a2) >> 16);
				*(offset_a++) = (char) ((blue_a * fak_a1 + blue_b * fak_a2) >> 16);

				*(offset_b++) = (char) ((red_a * fak_a2 + red_b * fak_a1) >> 16);
				*(offset_b++) = (char) ((green_a * fak_a2 + green_b * fak_a1) >> 16);
				*(offset_b++) = (char) ((blue_a * fak_a2 + blue_b * fak_a1) >> 16);
			}
			fak_a1 += step;
			fak_a2 -= step;
		}
	}


	return M_PICDONE;
}


/*-----------------------  FUNCTION MAIN --------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul aufgerufen wurde, */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Wenn das Modul gestartet wurde */
	case MEXEC:
		smurf_struct->module_mode = do_it(smurf_struct);
		break;

	/* Wenn das Modul sich verpissen soll */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}


/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel gro�es Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview mu� in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* �bernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul mu� nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierf�r bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* f�r die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC * smurfpic, SMURF_PIC * preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}
