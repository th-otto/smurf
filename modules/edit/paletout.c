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
/*				Palettenausgabe-Modul						*/
/* Version 0.1  --  06.09.97								*/
/*	  1-8 Bit												*/
/* Version 0.1  --  02.02.98								*/
/*	  Extender BIN wird nun vorgeblendet und als Standard	*/
/*	  verwendet, wird keiner angegeben.						*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Palettenausgabe"
#elif COUNTRY==0
#define TEXT1 "Palettenausgabe"
#elif COUNTRY==2
#define TEXT1 "Palettenausgabe"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

MOD_INFO module_info = {
	TEXT1,
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ NULL, NULL, NULL, NULL },
/* 4 Checkboxueberschriften: */
	{ NULL, NULL, NULL, NULL },
/* 4 Edit-Objekt-šberschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max fr Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 2, 3, 4, 5, 6, 7, 8,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	0,
};


static void conv(uint8_t val, char *wert)
{
	static char const hexchars[16] = "0123456789abcdef";

	wert[0] = '0';
	wert[1] = 'x';
	wert[2] = hexchars[val >> 4];
	wert[3] = hexchars[val & 0x0f];
	wert[4] = ',';
	wert[5] = ' ';
	wert[6] = '\0';
}


static void writeline(short fp, char *wert1, char *wert2, char *wert3)
{
	Fwrite(fp, 4, "\t\t\t ");
	Fwrite(fp, 6, wert1);
	Fwrite(fp, 6, wert2);
	Fwrite(fp, 4, wert3);
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Palettenausgabe					*/
/*		1-8 Bit										*/
/*	Gibt die jeweilige Bildpalette als ASCII-Datei	*/
/*	(Extender .ASC, Hexwerte) oder als Bin„rdatei	*/
/*	(Extender .BIN) unter dem anzugebenden Namen aus*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	char path[SM_PATH_MAX];
	char file[65];
	char wert1[8];
	char wert2[8];
	char wert3[8];
	char palette[SM_PATH_MAX];
	char *part;
	char *extender;
	uint8_t BitsPerPixel;
	WORD button;
	short fp;
	unsigned short i;
	uint16_t pal[3];

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_STARTED;
		break;

	case MEXEC:
		data = smurf_struct->smurf_pic->palette;
		BitsPerPixel = smurf_struct->smurf_pic->depth;
		i = (1 << BitsPerPixel);

		strcpy(path, "*.bin");			/* PAL als Extender vorblenden */
		fsel_input(path, file, &button);
		if (button && strlen(file))
		{
			extender = strrchr(file, '.');
			if (extender)				/* kein NULL-Zeiger, d.h. ein Punkt */
			{
				if (stricmp(extender, ".") == 0)	/* kein Extender nach dem Punkt */
					strcat(file, "bin");
			} else						/* weder Punkt noch Extender vorhanden */
			{
				strcat(file, ".bin");
				extender = strrchr(file, '.');
			}

			extender++;					/* den Punkt bergehen */

			strcpy(palette, path);
			part = strrchr(palette, '\\') + 1;
			strcpy(part, file);
			fp = (short) Fcreate(palette, 0);

			if (stricmp(extender, "asc") == 0)
			{
				Fwrite(fp, sizeof("palette[] = {") - 1, "palette[] = {");

				i--;					/* damit am Ende noch eine Farbe bleibt */
				while (i--)
				{
					conv(*data++, wert1);
					conv(*data++, wert2);
					conv(*data++, wert3);
					writeline(fp, wert1, wert2, wert3);
					Fwrite(fp, 3, ",\r\n");
				}

				conv(*data++, wert1);
				conv(*data++, wert2);
				conv(*data++, wert3);
				writeline(fp, wert1, wert2, wert3);
				Fwrite(fp, 6, "\r\n};\r\n");
			} else
			{
				while (i--)
				{
					pal[0] = (((unsigned long) *data++ * 1000L) / 255);
					pal[1] = (((unsigned long) *data++ * 1000L) / 255);
					pal[2] = (((unsigned long) *data++ * 1000L) / 255);
					Fwrite(fp, 6, pal);
				}
			}

			Fclose(fp);
		}

		/* we did not modify picture, so return error to avoid redraws */
		smurf_struct->module_mode = M_MODERR;
		break;

	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
