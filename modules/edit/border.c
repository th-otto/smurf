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

/*  ------------- Border-Modul V0.5 ------------------  */
/*          FÅr SMURF Bildkonverter, 26.04.96           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/*-----> Modul --------*/


/*--------------- Globale Variablen --------------------------*/
static unsigned long busycount;
static unsigned long busymax;
static unsigned long busycall;


/*--------------- Infostruktur fÅr Hauptprogramm -----*/
MOD_INFO module_info = {
	"Border-Me-Up..",					/* Name des Moduls */
	0x0010,
	"Jîrg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen fÅr Importer */
/* 4 SliderÅberschriften: max 8 */
	{ "Red", "Green", "Blue", "" },
/* 4 CheckboxÅberschriften: */
	{ "Center", "Use Color", "", "" },
/* 4 Edit-Objekt-öberschriften: */
	{ "left", "right", "up", "down" },
/* min/max-Werte fÅr Slider */
	{ { 0, 255 }, { 0, 255 }, { 0, 255 }, { 0, 0 } },
/* min/max fÅr Editobjekte */
	{ { 0, 32768L }, { 0, 32768L }, { 0, 32768L }, { 0, 32768L } },
/* Defaultwerte fÅr Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
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
	0
};





/*--------------- SCALING ----------------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short width, height;
	uint8_t *pic;
	uint8_t *n_pic;
	uint8_t *offset_a;
	uint8_t *offset_b;
#if 0
	long bpl_a;
#endif
	long bpl_b;
	uint8_t bkgrd_r, bkgrd_g, bkgrd_b;
	short bd_left, bd_right, bd_up, bd_down;
	short n_width, n_height, x_pos, y_pos;
	short x, y;

	/*--- Slider auslesen ---------------------- */

	bd_left = (short) smurf_struct->edit[0];
	bd_right = (short) smurf_struct->edit[1];
	bd_up = (short) smurf_struct->edit[2];
	bd_down = (short) smurf_struct->edit[3];

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

#if 0
	bpl_a = (long) width * 3L;
#endif

	/*--- Neues Bild festlegen ----------------*/
	n_width = bd_left + bd_right + width;
	n_height = bd_up + bd_down + height;
	bpl_b = (long) n_width *3L;

	n_pic = (uint8_t *)Malloc((long) n_width * n_height * 3L);
	if (n_pic == NULL)
		return M_MEMORY;


	if (smurf_struct->check[0])			/* Zentriert */
	{
		x_pos = (n_width - width) / 2;
		y_pos = (n_height - height) / 2;
	} else
	{
		x_pos = bd_left;
		y_pos = bd_up;
	}

	if (smurf_struct->check[1])
	{
		bkgrd_r = smurf_struct->slide[0];
		bkgrd_g = smurf_struct->slide[1];
		bkgrd_b = smurf_struct->slide[2];
	} else
	{
		bkgrd_r = pic[0];
		bkgrd_g = pic[1];
		bkgrd_b = pic[2];
	}

	offset_b = n_pic;
	for (y = 0; y < n_height; y++)
	{
		for (x = 0; x < n_width; x++)
		{
			*offset_b++ = bkgrd_r;
			*offset_b++ = bkgrd_g;
			*offset_b++ = bkgrd_b;
		}
	}



	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 1;
	busymax = height;
	if (busymax > 32)
		busycall = 7;
	if (busymax > 256)
		busycall = 31;
	busymax = (busymax << 10) / 127;


	/*--- Hauptroutine --------------------*/

	offset_a = pic;
	for (y = 0; y < height; y++)
	{
		busycount++;
		if (!(busycount & busycall))
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		offset_b = n_pic + (long) (y + y_pos) * bpl_b + (long) (x_pos) * 3L;
		for (x = 0; x < width; x++)
		{
			*offset_b++ = *offset_a++;
			*offset_b++ = *offset_a++;
			*offset_b++ = *offset_a++;
		}
	}

	picture->pic_width = n_width;
	picture->pic_height = n_height;
	picture->pic_data = n_pic;

	Mfree(pic);

	smurf_struct->event_par[0] = 0;
	smurf_struct->event_par[1] = -1;

	return M_PICDONE;
}


/*-----------------------  FUNCTION MAIN --------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short module_id;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* Wenn das Modul aufgerufen wurde, */
		module_id = smurf_struct->module_number;
		smurf_struct->services->f_module_prefs(&module_info, module_id);
		smurf_struct->module_mode = M_WAITING;
		break;
	case MEXEC:
		/* Wenn das Modul gestartet wurde */
		smurf_struct->module_mode = do_it(smurf_struct);
		break;
	case MTERM:
		/* Wenn das Modul sich verpissen soll */
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
