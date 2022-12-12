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

/*  ---------- Extrapolieren-Modul V0.1 --------------  */
/*          FÅr SMURF Bildkonverter, 26.04.96           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);


/*------ Infostruktur fÅr Hauptprogramm -----*/
MOD_INFO module_info = {
	"Extrapolieren",					/* Name des Moduls */
	0x0010,
	"Jîrg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen fÅr Importer */
/* 4 SliderÅberschriften: max 8 */
	{ "X-Fk 1/x", "Y-Fk 1/x", "", "" },
/* 4 CheckboxÅberschriften: */
	{ "", "", "", "" },
/* 4 Edit-Objekt-öberschriften: */
	{ "", "", "", "" },
/* min/max-Werte fÅr Slider */
	{ { 1, 16 }, { 1, 16 }, { 0, 0 }, { 0, 0 } },
/* min/max fÅr Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fÅr Slider, Check und Edit */
	{ 1, 0, 0, 0 },
	{ 1, 0, 0, 0 },
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
	0									/* Extra-Flag */
};


/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short width, height;
	short n_width, n_height;
	short x, y;
	short xm, ym;
	short xmax, ymax;
	uint8_t *pic;
	uint8_t *n_pic;
	uint8_t *n_offset;
	unsigned short red, green, blue;
	long bpl;
	long yoffset;
	uint8_t *moffset;
	short x_fak, y_fak;
	short div;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul aufgerufen wurde, */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Wenn das Modul gestartet wurde */
	case MEXEC:
		picture = smurf_struct->smurf_pic;
		pic = picture->pic_data;
		width = picture->pic_width;
		height = picture->pic_height;

		x_fak = (short) smurf_struct->slide[0];
		y_fak = (short) smurf_struct->slide[1];

		if (x_fak == 1 && y_fak == 1)	/* Keine Scalierung (->beenden) */
		{
			smurf_struct->module_mode = M_PICDONE;
			return;
		}

		bpl = width * 3L;
		n_width = (width + x_fak - 1) / x_fak;
		n_height = (height + y_fak - 1) / y_fak;
		n_offset = n_pic = (uint8_t *)Malloc((long) n_width * n_height * 3L);

		if (n_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}

		for (y = 0; y < height; y += y_fak)
		{
			if (!(y % 20))
				smurf_struct->services->busybox((short) (((long) y << 7L) / (long) height));
			for (x = 0; x < width; x += x_fak)
			{
				xmax = x + y_fak;
				if (xmax > width)
					xmax = width;
				ymax = y + y_fak;
				if (ymax > height)
					ymax = height;

				div = 0;
				red = green = blue = 0;
				yoffset = y * bpl + x * 3L;
				for (ym = y; ym < ymax; ym++)
				{
					moffset = pic + yoffset;
					for (xm = x; xm < xmax; xm++)
					{
						red += *(moffset++);
						green += *(moffset++);
						blue += *(moffset++);
						div++;

					}
					yoffset += bpl;
				}

				*(n_offset++) = red / div;
				*(n_offset++) = green / div;
				*(n_offset++) = blue / div;

			}
		}

		Mfree(pic);

		picture->pic_width = n_width;
		picture->pic_height = n_height;
		picture->pic_data = n_pic;

		smurf_struct->module_mode = M_PICDONE;
		break;

	/* Wenn das Modul sich verpissen soll */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}


/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel groûes Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview muû in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* Åbernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul muû nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierfÅr bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* fÅr die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}
