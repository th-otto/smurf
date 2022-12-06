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
 * The Original Code is from Gerhard Stoll.
 *
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =================================================================*/
/*	PGF files Importer												*/
/*																	*/
/* Version 0.4  --  26.04.2003         								*/
/*                  Umstellung auf Smurf Malloc/Free				*/
/* Version 0.3  --  05.04.2003										*/
/*                  Kann nun auch PGX-Dateien lesen. Es wird nur das*/
/*									erste Bild geladen.				*/
/* Version 0.2  --  01.04.2003										*/
/* Version 0.1  --  29.03.2003										*/
/*																	*/
/* =================================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "Falsche Grîûe (!=1920) fÅr ein PGF!"
#define ERROR2 "Diese Portfolio PGX-Datei enthÑlt keine|PGC Grafik."
#define ERROR3 "Dies ist keine Portfolio PGX-Datei"
#define ERROR4 "Dies ist keine Portfolio PGC-Datei"
#elif COUNTRY==0
#define ERROR1 "Wrong size (not=1920) for a PGF!"
#define ERROR2 "This Portfolio PGX file doesn't hold|a PGC graphic!"
#define ERROR3 "This is not a Portfolio PGX file!"
#define ERROR4 "This is not a Portfolio PGC file!"
#elif COUNTRY==2
#define ERROR1 "Wrong size (not=1920) for a PGF!"
#define ERROR2 "This Portfolio PGX file doesn't hold|a PGC graphic!"
#define ERROR3 "This is not a Portfolio PGX file!"
#define ERROR4 "This is not a Portfolio PGC file!"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"



/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Portfolio PGF",
	0x0030,
	"Gerhard Stoll",
	{ "PGF", "PGC", "PGX", "", "", "", "", "", "", "" },
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



/*---------------------------------------------------------------------*/

static char *fileext(const char *filename)
{
	char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		PGF files Importer														*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *pal;
	char *fname;
	uint8_t *ziel;
	uint8_t index;
	uint8_t data;
	uint8_t BitsPerPixel;

	short i, k, dest;
	unsigned short width, height;
	long file_len;

	fname = smurf_struct->smurf_pic->filename;

	if (stricmp(fileext(fname), "PGF") != 0 &&
		stricmp(fileext(fname), "PGC") != 0 &&
		stricmp(fileext(fname), "PGX") != 0)
		return M_INVALID;

	file_len = smurf_struct->smurf_pic->file_len;

	if (stricmp(fileext(fname), "PGF") == 0 && file_len != 1920L)
	{
		smurf_struct->services->f_alert(ERROR1, "OK", NULL, NULL, 1);
		return M_PICERR;
	}

	buffer = smurf_struct->smurf_pic->pic_data;

	width = 240;
	height = 64;

	BitsPerPixel = 1;

	strcpy(smurf_struct->smurf_pic->format_name, "Portfolio Graphics");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	pal = smurf_struct->smurf_pic->palette;

	pal[0] = 255;
	pal[1] = 255;
	pal[2] = 255;
	pal[3] = 0;
	pal[4] = 0;
	pal[5] = 0;

	if (stricmp(fileext(fname), "PGC") == 0)
	{
		if (buffer[0] != 'P' || buffer[1] != 'G' || buffer[2] != 1)
		{
			smurf_struct->services->f_alert(ERROR4, "OK", NULL, NULL, 1);
			return M_PICERR;
		}
		strcpy(smurf_struct->smurf_pic->format_name, "Portfolio Graphics Compressed");

		ziel = (uint8_t *) smurf_struct->services->SMalloc(1920L);
		if (ziel == NULL)
			return M_MEMORY;

		i = 0;
		dest = 0;
		do
		{
			index = buffer[i + 3];
			if ((index & 0x80) != 0)	/* High Bit ist gesetzt                     */
			{							/* Gleiche Bytefolge                            */
				index -= 0x80;			/* Anzahl der Bytes                             */
				i++;
				data = buffer[i + 3];
				for (k = 0; k < index; k++)
					ziel[dest++] = data;
				i++;
			} else
			{							/* Unterschiedliche Bytefolge           */
				i++;
				for (k = 0; k < index; k++, i++)
					ziel[dest++] = buffer[i + 3];
			}
		} while (dest < 1920);
		smurf_struct->services->SMfree(buffer);

		smurf_struct->smurf_pic->pic_data = ziel;
	}

	if (stricmp(fileext(fname), "PGX") == 0)
	{
		if (buffer[0] != 'P' || buffer[1] != 'G' || buffer[2] != 'X')
		{
			smurf_struct->services->f_alert(ERROR3, "OK", NULL, NULL, 1);
			return M_PICERR;
		}
		strcpy(smurf_struct->smurf_pic->format_name, "Portfolio Graphics, PGX");

		i = 8;							/* Start der Daten                              */
		do
		{
			if (buffer[i] == 0xff)		/* Ende erreicht?                                   */
			{
				smurf_struct->services->f_alert(ERROR2, "OK", NULL, NULL, 1);
				return M_PICERR;
			}
			if (buffer[i] == 0x00)		/* PGC-Datei                                            */
			{
				ziel = (uint8_t *) smurf_struct->services->SMalloc(1920L);
				if (ziel == NULL)
					return M_MEMORY;

				i += 8;
				dest = 0;
				do
				{
					index = buffer[i];
					if ((index & 0x80) != 0)	/* High Bit ist gesetzt                     */
					{					/* Gleiche Bytefolge                            */
						index -= 0x80;	/* Anzahl der Bytes                             */
						i++;
						data = buffer[i];
						for (k = 0; k < index; k++)
							ziel[dest++] = data;
						i++;
					} else
					{					/* Unterschiedliche Bytefolge           */
						i++;
						for (k = 0; k < index; k++, i++)
							ziel[dest++] = buffer[i];
					}
				} while (dest < 1920);
				smurf_struct->services->SMfree(buffer);

				smurf_struct->smurf_pic->pic_data = ziel;
				break;
			}
			i += 8 + buffer[i + 1] + (buffer[i + 2] * 256);
		} while (i < file_len);
	}

	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	return M_PICDONE;
}
