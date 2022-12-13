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
/*					Cubicomb Picturemaker					*/
/* Version 0.1  --  11.01.96								*/
/*    xxx													*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR "[1][Die Farbpalette konnte nicht|geladen werden!][ Stop ]"
#elif COUNTRY==0
#define ERROR "[1][Die Farbpalette konnte nicht|geladen werden!][ Stop ]"
#elif COUNTRY==2
#define ERROR "[1][Die Farbpalette konnte nicht|geladen werden!][ Stop ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Cubicomb Import-Modul",
	0x0010,
	"Christian Eyrich",
	{ "R8", "G8", "B8", "A8", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};



/* --- STRSRCR --- */

static int strsrcr(char *s, char c)
{
	int i;

	for (i = (int) strlen(s) - 1; i >= 0 && s[i] != c; i--)
		;

	return i;
}


/* --- FLOAD --- */
static void *fload(char *Path, short header)
{
	uint8_t *fil;
	long f_len;
	int file;

	file = (int) Fopen(Path, FO_READ);	/* Datei ”ffnen */
	if (file >= 0)
	{
		f_len = Fseek(0, file, 2);		/* L„nge ermitteln */

		if ((fil = (uint8_t *)Malloc((long) f_len - header)) != NULL)
		{
			Fseek(header, file, 0);		/* Anfang suchen */

			if (fil != 0)
				Fread(file, f_len - header, fil);	/* Bild lesen */
		}
		Fclose(file);					/* Kanal schliežen */
	} else
	{
		form_alert(1, ERROR);
		fil = NULL;
	}

	return fil;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Cubicomp Picturemaker  .R8+G8+B8+A8			*/
/*		24 Bit, unkomprimiert						*/
/*	Ein genialer Einfall, ein Bild ber vier Dateien*/
/*	zu verteilen. Klasse komprimiert und gerade		*/
/*	richtig fr fr User mit dicker Platte.			*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	char name[SM_PATH_MAX];
	int help;
	unsigned short width, height;

	unsigned long i;

	struct rgb
	{
		char b;
		char g;
		char r;
	} *ziel, *oziel;


	buffer = smurf_struct->smurf_pic->pic_data;

	/* Header Check */
	if (!(*(buffer + 1) == 0x0c && *(buffer + 2) == 0xff && *(buffer + 3) == 0x02))
		return M_INVALID;

	width = *(buffer + 0x0a) + (*(buffer + 0x0b) << 8);
	height = *(buffer + 0x0c) + (*(buffer + 0x0d) << 8);

	strcpy(smurf_struct->smurf_pic->format_name, "Cubicomb Picturemaker");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 24;

	smurf_struct->services->reset_busybox(128, "Cubicomb 24 Bit");

	if ((ziel = (struct rgb *)Malloc((unsigned long) width * height * 3)) == NULL)
		return M_MEMORY;
	oziel = ziel;

	/* Extender holen */
	help = strsrcr(smurf_struct->smurf_pic->filename, '.');
	strncpy(name, smurf_struct->smurf_pic->filename, help + 1);

	/* Rotanteil folgt */

	if (stricmp(smurf_struct->smurf_pic->filename + help + 1, "R8") == 0)
	{
		buffer = smurf_struct->smurf_pic->pic_data;
		buffer += 0x80;
	} else
	{
		strcpy(name + help + 1, "R8");
		buffer = fload(name, 0x80);
	}

	ziel = oziel;
	for (i = 0; i < (width * height); i++)
		ziel++->r = *buffer++;

	if (buffer != smurf_struct->smurf_pic->pic_data)
		Mfree(buffer);

	/* Grnanteil folgt */

	if (stricmp(smurf_struct->smurf_pic->filename + help + 1, "G8") != 0)
	{
		buffer = smurf_struct->smurf_pic->pic_data;
		buffer += 0x80;
	} else
	{
		strcpy(name + help + 1, "G8");
		buffer = fload(name, 0x80);
	}

	ziel = oziel;
	for (i = 0; i < (width * height); i++)
		ziel++->g = *buffer++;

	if (buffer != smurf_struct->smurf_pic->pic_data)
		Mfree(buffer);

	/* Blauanteil folgt */

	if (stricmp(smurf_struct->smurf_pic->filename + help + 1, "B8") != 0)
	{
		buffer = smurf_struct->smurf_pic->pic_data;
		buffer += 0x80;
	} else
	{
		strcpy(name + help + 1, "B8");
		buffer = fload(name, 0x80);
	}

	ziel = oziel;
	for (i = 0; i < (width * height); i++)
		ziel++->b = *buffer++;

	if (buffer != smurf_struct->smurf_pic->pic_data)
		Mfree(buffer);

	Mfree(smurf_struct->smurf_pic->pic_data);
	smurf_struct->smurf_pic->pic_data = ziel;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	return M_PICDONE;
}
