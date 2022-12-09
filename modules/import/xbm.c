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
/*						X BitMap Format						*/
/* Version 0.1  --  31.08.96								*/
/*	  Momentan nur X11-XBMs									*/
/* Version 0.2  --  17.11.96								*/
/*	  Nun kînnen auch XBMs von X10 geladen werden			*/
/* Version 0.3  --  19.05.98								*/
/*	  Komplett Åberarbeitet, Speicheranforderung korrigiert	*/
/*	  (war nicht auf 8 Bit aligned -> Åberschriebener		*/
/*	  Speicher).											*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define START	'{'
#define END		'}'

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"X BitMap-Format",
	0x0030,
	"Christian Eyrich",
	{ "XBM", "", "", "", "", "", "", "", "", "" },
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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				X BitMap-Format (XBM)				*/
/*		Bilder 1 Bit unkomprimiert					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	char *buffer;
	char *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	char *helpstr;
	char *fname;
	char *name_and_type;
	char *strvalue;
	uint8_t BitsPerPixel;
	uint8_t version;
	uint8_t hexTable[256];
	uint8_t translate[256];

	unsigned short i;
	unsigned short width = 0;
	unsigned short height = 0;
	unsigned short value;
	unsigned short w;
	unsigned long length;


	i = 0;
	do
	{
		hexTable[i] = 0;
	} while (++i < 256);
	hexTable['0'] = 0;
	hexTable['1'] = 1;
	hexTable['2'] = 2;
	hexTable['3'] = 3;
	hexTable['4'] = 4;
	hexTable['5'] = 5;
	hexTable['6'] = 6;
	hexTable['7'] = 7;
	hexTable['8'] = 8;
	hexTable['9'] = 9;
	hexTable['A'] = 10;
	hexTable['B'] = 11;
	hexTable['C'] = 12;
	hexTable['D'] = 13;
	hexTable['E'] = 14;
	hexTable['F'] = 15;
	hexTable['a'] = 10;
	hexTable['b'] = 11;
	hexTable['c'] = 12;
	hexTable['d'] = 13;
	hexTable['e'] = 14;
	hexTable['f'] = 15;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	/* Header Check */
	fname = smurf_struct->smurf_pic->filename;
	if (stricmp(fileext(fname), "XBM") != 0)
		return M_INVALID;

	smurf_struct->services->reset_busybox(128, "X BitMap 1 Bit");

	BitsPerPixel = 1;

	if (strstr(buffer, "char") == NULL && strstr(buffer, "CHAR") == NULL)
		version = 6;
	else
		version = 7;

	i = 0;
	do
	{
		translate[i] = (i & 0x01) << 7;
		translate[i] += (i & 0x02) << 5;
		translate[i] += (i & 0x04) << 3;
		translate[i] += (i & 0x08) << 1;
		translate[i] += (i & 0x10) >> 1;
		translate[i] += (i & 0x20) >> 3;
		translate[i] += (i & 0x40) >> 5;
		translate[i] += (i & 0x80) >> 7;
	} while (++i < 256);

	i = 0;
	do
	{
		if ((helpstr = strstr(buffer, "#define")) == NULL)
			return M_PICERR;
		name_and_type = helpstr + 8;		/* hinter "#define " stellen */
		if ((strvalue = strchr(name_and_type, ' ')) == NULL)
			return M_PICERR;
		*strvalue++ = '\0';
		value = (unsigned short)strtol(strvalue, NULL, 0);

		if (strstr(name_and_type, "width") != NULL || strstr(name_and_type, "WIDTH") != NULL)
			width = value;
		else if (strstr(name_and_type, "height") != NULL || strstr(name_and_type, "HEIGHT") != NULL)
			height = value;
		buffer = strvalue;
	} while (++i < 4 && (width == 0 || height == 0));

	if (i == 4 && (width == 0 || height == 0))
		return M_PICERR;

	strcpy(smurf_struct->smurf_pic->format_name, "X BitMap .XBM");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	if ((helpstr = strchr(buffer, START)) == NULL)
		return M_PICERR;

	w = (width + 7) / 8;

	if ((ziel = smurf_struct->services->SMalloc((unsigned long) w * height)) == 0)
		return M_MEMORY;

	oziel = ziel;

	length = (unsigned long) w * height;

	while (length--)
	{
		while (*helpstr != END && *helpstr != 'x' && *helpstr != 'X')
			helpstr++;
		if (*helpstr++ == END)
			break;
		if (version == 6)
			helpstr += 2;
		i = hexTable[(unsigned char)*helpstr++] << 4;
		i |= hexTable[(unsigned char)*helpstr++];
		*ziel++ = translate[i];
	}

	buffer = obuffer;
	ziel = oziel;

	pal = smurf_struct->smurf_pic->palette;
	pal[0] = 255;
	pal[1] = 255;
	pal[2] = 255;
	pal[3] = 0;
	pal[4] = 0;
	pal[5] = 0;

	smurf_struct->smurf_pic->pic_data = ziel;
	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
