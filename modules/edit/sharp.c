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

/*  ------------- Sharpen-Modul V0.2 -----------------  */
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


/*--------------- Globale Variablen --------------------------*/


/*--------------- Infostruktur f�r Hauptprogramm -----*/
MOD_INFO module_info = {
	"Sharpen",							/* Name des Moduls */
	0x0020,
	"J�rg Dittmer/Dale Russell",		/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen f�r Importer */
/* 4 Slider�berschriften: max 8 */
	"St�rke in %",
	"",
	"",
	"",
/* 4 Checkbox�berschriften: */
	"",
	"",
	"",
	"",
/* 4 Edit-Objekt-�berschriften: */
	"",
	"",
	"",
	"",
/* min/max-Werte f�r Slider */
	0, 100,
	0, 0,
	0, 0,
	0, 0,
/* min/max f�r Editobjekte */
	0, 0,
	0, 0,
	0, 0,
	0, 0,
/* Defaultwerte f�r Slider, Check und Edit */
	50, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	"Bild 1", NULL, NULL, NULL, NULL, NULL
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



/*--------------- Sharp(en)ing ----------------------------*/

static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short width, height;
	uint8_t *pic;
	short strength;
	uint8_t *oben;
	uint8_t *unten;
	uint8_t *links;
	uint8_t *rechts;
	uint8_t *mitte;
	uint8_t *buf_eins;
	uint8_t *buf_zwei;
	uint8_t *buffer_eins;
	uint8_t *buffer_zwei;
	long r, g, b;
	short  xmax, ymax;
	short x, y;
	long yoffset, bpl;
	long faktor_1[256];
	long faktor_2[256];
	long mul5[256];
	unsigned long busycount, busymax, busycall;

	/*--- Slider auslesen ---------------------- */

	strength = (short) smurf_struct->slide1;

	for (r = 0; r < 256; r++)
	{
		faktor_1[r] = ((r * (long) strength) << 8L) / 100L;
		faktor_2[r] = ((r * (100L - (long) strength)) << 8L) / 100L;
		mul5[r] = r * 3;
	}

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	bpl = width * 3L;


	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 3;
	busymax = height;
	if (busymax > 32)
		busycall = 15;
	if (busymax > 256)
		busycall = 63;
	busymax = (busymax << 10) / 127;


	/*--------- Bild ohne R�nder --------------------*/

	ymax = height - 2;
	xmax = width - 1;
	yoffset = bpl;
	buf_eins = (uint8_t *)Malloc(2 * bpl * sizeof(*buf_eins));
	if (buf_eins == NULL)
		return M_MEMORY;
	buf_zwei = buf_eins + bpl;
	memset(buf_eins, 0, 2 * bpl * sizeof(*buf_eins));

	y = 0;
	for (;;)
	{
		busycount++;
		if (!(busycount & busycall))
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		/* 1. Zeile */

		buffer_eins = buf_eins;
		mitte = pic + yoffset + 3;		/* +3 weil erst ab 2.Pixel! */

		oben = mitte - bpl;
		unten = mitte + bpl;
		links = mitte - 3;
		rechts = mitte + 3;

		rechts = mitte - 3 - bpl;
		links = mitte + 3 + bpl;

		for (x = 1; x < xmax; x++)
		{
			r = mul5[mitte[0]];
			g = mul5[mitte[1]];
			b = mul5[mitte[2]];

			r -= rechts[0];
			g -= rechts[1];
			b -= rechts[2];

			r -= links[0];
			g -= links[1];
			b -= links[2];

#if 0
			r -= oben[0];
			g -= oben[1];
			b -= oben[2];

			r -= unten[0];
			g -= unten[1];
			b -= unten[2]
#endif
			if (r < 0)
				r = 0;
			else if (r > 255)
				r = 255;
			if (g < 0)
				g = 0;
			else if (g > 255)
				g = 255;
			if (b < 0)
				b = 0;
			else if (b > 255)
				b = 255;


			*buffer_eins++ = (faktor_1[r] + faktor_2[mitte[0]]) >> 8L;
			*buffer_eins++ = (faktor_1[g] + faktor_2[mitte[1]]) >> 8L;
			*buffer_eins++ = (faktor_1[b] + faktor_2[mitte[2]]) >> 8L;

			mitte += 3;
			oben += 3;
			unten += 3;
			links += 3;
			rechts += 3;
		}
		y++;
		if (y >= ymax)
		{
			/* Die 1. Zeile zur�ckschreiben */

			mitte = pic + yoffset + 3;
			memcpy(mitte, buf_eins, bpl - 6L);
			break;
		}
		yoffset += bpl;

		/* 2. Zeile */

		busycount++;
		if (!(busycount & busycall))
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		buffer_zwei = buf_zwei;
		mitte = pic + yoffset + 3;		/* +3 weil erst ab 2.Pixel! */

		/*  oben = mitte-bpl;
		   unten = mitte+bpl;
		   links = mitte-3;
		   rechts = mitte+3; */

		rechts = mitte - 3 - bpl;
		links = mitte + 3 + bpl;

		for (x = 1; x < xmax; x++)
		{
			r = mul5[mitte[0]];
			g = mul5[mitte[1]];
			b = mul5[mitte[2]];
			r -= rechts[0];
			g -= rechts[1];
			b -= rechts[2];
			r -= links[0];
			g -= links[1];
			b -= links[2];
#if 0
			r -= oben[0];
			g -= oben[1];
			b -= oben[2];
			r -= unten[0];
			g -= unten[1];
			b -= unten[2];
#endif
			if (r < 0)
				r = 0;
			else if (r > 255)
				r = 255;
			if (g < 0)
				g = 0;
			else if (g > 255)
				g = 255;
			if (b < 0)
				b = 0;
			else if (b > 255)
				b = 255;
			*buffer_zwei++ = (faktor_1[r] + faktor_2[mitte[0]]) >> 8L;
			*buffer_zwei++ = (faktor_1[g] + faktor_2[mitte[1]]) >> 8L;
			*buffer_zwei++ = (faktor_1[b] + faktor_2[mitte[2]]) >> 8L;
			mitte += 3;
			oben += 3;
			unten += 3;
			links += 3;
			rechts += 3;
		}

		/* Die 2 Zeilen zur�ckschreiben */

		mitte = pic + yoffset + 3 - bpl;
		memcpy(mitte, buf_eins, bpl - 6L);
		mitte = pic + yoffset + 3;
		memcpy(mitte, buf_zwei, bpl - 6L);
		y++;
		if (y >= ymax)
			break;
		yoffset += bpl;
	}

	Mfree(buf_eins);
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

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}
