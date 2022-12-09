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
/*					Q0 Japan Image File						*/
/* Version 0.1  --  13.01.96								*/
/*	 Ob die Bildgr”ženerkennung wohl geht?					*/
/*==========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR "[1][Bildteil konnte nicht|geladen werden!][ Stop ]"
#elif COUNTRY==0
#define ERROR "[1][Image portion could|not be loaded!][ Stop ]"
#elif COUNTRY==2
#define ERROR "[1][Image portion could|not be loaded!][ Stop ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Q0 Japan Image",
	0x0010,
	"Christian Eyrich",
	{ "FAL", "Q0", "", "", "", "", "", "", "", "" },
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,
	NULL, NULL, NULL, NULL, NULL, NULL
};



static char *fileext(const char *filename)
{
	char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
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

			Fread(file, (long) f_len - header, fil);	/* Bild lesen */
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
/*			Q0 Japan Image File-Format (FAL)		*/
/*	24 Bit, unkomprimiert							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *buffer;
	uint8_t *obuffer;
	char *fname;
	char name[257];
	char bf[12];
	uint8_t c;
	unsigned short x, y, width, height;

	buffer = smurf_struct->smurf_pic->pic_data;

	fname = smurf_struct->smurf_pic->filename;
	if ((stricmp(fileext(fname), "FAL") != 0 && stricmp(fileext(fname), "Q0") != 0) ||
		strncmp((char *)buffer, "F_ALL(V1.00)", 12) != 0)
		return M_INVALID;
	if (strcmp(fileext(fname), "Q0") == 0)
	{
		strcpy(name, fname);
		strcpy(fileext(name), "FAL");

		smurf_struct->services->reset_busybox(128, "Japan Image 24 Bit");

		/* Headerfile laden */
		if ((buffer = fload(name, 0)) == 0)
			return (M_MODERR);
		if (strcmp((char *)buffer, "F_ALL(V1.00)") != 0)
		{
			Mfree(buffer);
			return M_INVALID;
		}
	}

	c = 14;
	while (c < 12 && *(buffer + c) != ' ')	/* 14 = Erkennungsstring + crlf */
		c++;
	strncpy(bf, (char *)buffer + c, 12);
	width = atoi(strtok(bf, " "));
	height = atoi(strtok(NULL, " "));

	smurf_struct->services->SMfree(buffer);

	if (strcmp(fileext(fname), "Q0") == 0)
	{
		buffer = smurf_struct->smurf_pic->pic_data;
	} else
	{
		strcpy(name, fname);
		strcpy(fileext(name), "Q0");

		/* Bildfile laden */
		if ((buffer = fload(name, 0)) == 0)
			return M_MODERR;
	}

	strcpy(smurf_struct->smurf_pic->format_name, "Q0 Japan Image .FAL + .Q0");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 24;

	if ((ziel = (uint8_t *)Malloc((long) width * (long) height * 3)) == 0)
		return M_MEMORY;

	oziel = ziel;
	obuffer = buffer;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
			*ziel++ = *buffer++;
			*ziel++ = *buffer++;
		} while (++x < width);
	} while (++y < height);

	ziel = oziel;
	buffer = obuffer;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
