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

/*  ---------- Farbs�ttigung-Modul V1.2 --------------  */
/*          F�r SMURF Bildkonverter, 26.04.96           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/*--------------- Funktionen -------------------*/
/*----------------------------------------------*/
/*-----> Smurf --------*/
void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);

/*-----> Modul --------*/

/*--------------- Infostruktur f�r Hauptprogramm -----*/
MOD_INFO module_info = {
	"Farb-S�ttigung",					/* Name des Moduls */
	0x0120,
	"J�rg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen f�r Importer */
/* 4 Slider�berschriften: max 8 */
	{ "F-S�t %", "", "", "" },
/* 4 Checkbox�berschriften: */
	{ "", "", "", "" },
/* 4 Edit-Objekt-�berschriften: */
	{ "", "", "", "" },
/* min/max-Werte f�r Slider */
	{ { 0, 200 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte f�r Slider, Check und Edit */
	{ 100, 0, 0, 0 },
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


/*--------------- Globale Variablen --------------------------*/
static unsigned long busycount;
static unsigned long busymax;
static unsigned long busycall;


/*--------------- Farb-S�ttigung ----------------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short width, height;
	short colsat;
	uint8_t *pic;
	uint8_t *cs_tab = 0;
	uint8_t *offset;
	uint8_t *cs_offset;
	short *div_tab;
	short *div_offset;
	long div;
	float cs_fak;
	short x, y;
	long red, green, blue, grey;

	/*--- Slider auslesen ---------------------- */

	colsat = (short) smurf_struct->slide[0];
	if (colsat == 100 || smurf_struct->smurf_pic->col_format == GREY)
		return M_PICDONE;													  /*--- Keine �nderung --> BEENDEN --------*/

	cs_fak = colsat / 100.0;

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	if (colsat != 0)
	{
		/*--- Speicher anfordern ----------------------*/
		cs_tab = (uint8_t *)Malloc(256L * 256L * sizeof(*cs_tab));
		if (cs_tab == NULL)
			return M_MEMORY;			/*  Kein Speicher !!! */
		div_tab = (short *)Malloc(512L * sizeof(*div_tab));
		if (div_tab == NULL)
		{
			Mfree(cs_tab);
			return M_MEMORY;			/*  Kein Speicher !!! */
		}
		/*--- Farb-S�ttigungs-Tabelle berechnen ------------*/

		div_offset = div_tab;
		for (x = 0; x < 512; x++)
		{
			*(div_offset++) = ((x - 255) * cs_fak);
		}

		offset = cs_tab;
		for (y = 0; y < 256; y++)
		{
			for (x = 0; x < 256; x++)
			{
				div = y + *(div_tab + (long) ((x - y) + 255));
				if (div < 0)
					div = 0;
				if (div > 255)
					div = 255;

				*(offset++) = div;
			}
		}
	} else
	{
#ifdef __GNUC__
		div_tab = 0;
#endif
	}


	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 3;
	busymax = height;
	if (busymax > 32)
		busycall = 15;
	if (busymax > 256)
		busycall = 63;
	busymax = (busymax << 10) / 127;


	/*--- Hauptroutine --------------------*/
	if (colsat != 0)
	{						/*--- Freie Farb-S�ttigung ----------*/
		offset = pic;
		for (y = 0; y < height; y++)
		{
			busycount++;
			if (!(busycount & busycall))
				smurf_struct->services->busybox((short) ((busycount << 10) / busymax));
			for (x = 0; x < width; x++)
			{
				red = *(offset++);
				green = *(offset++);
				blue = *(offset);
				offset -= 2;

				if (red > green)
				{
					grey = red;
					if (green > blue)
					{
						grey += green;
					} else
					{
						grey += blue;
					}
				} else
				{
					grey = green;
					if (red > blue)
					{
						grey += red;
					} else
					{
						grey += blue;
					}
				}

				grey = (grey >> 1L);

				cs_offset = cs_tab + (grey << 8L);

				*(offset++) = *(cs_offset + red);
				*(offset++) = *(cs_offset + green);
				*(offset++) = *(cs_offset + blue);
			}
		}
	} else
	{						/*--- Farb-S�ttigung ist NULL --> Graubild -------*/
		offset = pic;
		for (y = 0; y < height; y++)
		{
			busycount++;
			if (!(busycount & busycall))
				smurf_struct->services->busybox((short) ((busycount << 10) / busymax));
			for (x = 0; x < width; x++)
			{
				red = *(offset++);
				green = *(offset++);
				blue = *(offset);
				offset -= 2;

				grey = ((red + green + blue) * 21846L) >> 16;

				*(offset++) = grey;
				*(offset++) = grey;
				*(offset++) = grey;
			}
		}
	}

	if (colsat != 0)
	{
		Mfree(div_tab);
		Mfree(cs_tab);
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
