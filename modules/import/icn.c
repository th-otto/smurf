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
/*						GEM Icon Format						*/
/* Version 0.1  --  10.09.96								*/
/*	  monochrome Icons										*/
/* Version 0.2  --  17.11.96								*/
/*	  Nun auch farbige Icons								*/
/* Version 0.3  --  26.04.98								*/
/*	  Nun auch 256farbige Icons								*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "icn.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"C-Source",
	0x0030,
	"Christian Eyrich",
	{ "ICN", "", "", "", "", "", "", "", "", "" },
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



#if 0 /* unused */
/* --- STRSRCHR --- */
/* Diese Funktion sucht von hinten her nach dem ersten Vorkommen des */
/* Åbergebenen Zeichen und gibt die Position oder -1 zurÅck */

static int strsrchr(char *s, char c)
{
	int i;

	for (i = (int) strlen(s) - 1; i >= 0 && s[i] != c; i--)
		;

	return i;
}
#endif


#if 0 /* unused */
/* --- STRSRCHL --- */
/* Diese Funktion sucht von vorne her nach dem ersten Vorkommen des */
/* Åbergebenen Zeichen und gibt die Position oder -1 zurÅck */

static int strsrchl(char *s, char c)
{
	int i, len;

	len = (int) strlen(s);

	for (i = 0; i < len && s[i] != c; i++)
		;

	if (i == len)
		i = -1;
	return i;
}
#endif


static const char *fileext(const char *filename)
{
	const char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				GEM Icon-Format (ICN)				*/
/*		1, 2, 4, 8 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	char *buffer;
	char *obuffer;
	char *helpstr;
	uint8_t *help;
	uint8_t *pal;
	char *fname;
	char *name_and_type;
	uint8_t BitsPerPixel;
	uint8_t Planes;
	uint8_t hexTable[256];
#define end '}'
	char *strvalue;
	char dummy[3];
	char impmessag[21];
	uint8_t dimension;

	uint16_t *ziel;
	uint16_t *oziel;
	unsigned short i;
	unsigned short width = 0;
	unsigned short height = 0;
	uint16_t w;

	unsigned long Datasize = 0;
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

	fname = smurf_struct->smurf_pic->filename;

	/* Header Check */
	if (stricmp(fileext(fname), "ICN") != 0)
		return M_INVALID;

	i = 0;
	do
	{
		if ((helpstr = strstr(buffer, "#define")) == NULL)
			return M_PICERR;
			
		name_and_type = helpstr + 8;		/* "#define" Åbergehen */
		if ((strvalue = strchr(name_and_type, ' ')) == NULL)
			return M_PICERR;
		*strvalue++ = '\0';

		help = (uint8_t *)strrchr(name_and_type, '_');
		if (help != NULL && *(help + 1) != '\0')
			dimension = *(help + 1);
		else if (i == 0)			/* erster define */
			dimension = 'w';
		else if (i == 1)			/* zweiter define */
			dimension = 'h';
		else
			dimension = ' ';

		if (dimension == 'w' || dimension == 'W')
		{
			width = (unsigned short)strtol(strvalue, NULL, 0);
		} else if (dimension == 'h' || dimension == 'H')
		{
			height = (unsigned short)strtol(strvalue, NULL, 0);
		} else if (strnicmp(name_and_type, "DATASIZE", 8) == 0)
		{
			Datasize = (unsigned short)strtol(strvalue, NULL, 0);
		}
		buffer = strvalue;
	} while (++i < 3 && (width == 0 || height == 0 || Datasize == 0));

	if (i == 3 && (width == 0 || height == 0 || Datasize == 0))
		return M_PICERR;
	helpstr = strchr(buffer, '{');
	if (helpstr ==  NULL)
		return M_PICERR;

	w = (width + 15) / 16;

	BitsPerPixel = (char) (Datasize / (w * (unsigned long) height));

	Planes = BitsPerPixel;
	if (Planes != 1 && Planes != 2 && Planes != 4 && Planes != 8)
		return M_PICERR;

	strcpy(smurf_struct->smurf_pic->format_name, "GEM Icon .ICN");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "GEM Icon ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if ((ziel = smurf_struct->services->SMalloc(((w * 16) * (unsigned long) height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;

	oziel = ziel;

	length = (unsigned long) w * height *Planes;

	while (length--)
	{
		while (*helpstr != end && *helpstr != 'x' && *helpstr != 'X')
			helpstr++;
		if (*helpstr++ == end)
			break;

		*ziel = (hexTable[(unsigned char)*helpstr++] << 12);
		*ziel |= (hexTable[(unsigned char)*helpstr++] << 8);
		*ziel |= (hexTable[(unsigned char)*helpstr++] << 4);
		*ziel++ |= hexTable[(unsigned char)*helpstr++];
	}

	buffer = obuffer;
	ziel = oziel;

	pal = smurf_struct->smurf_pic->palette;
	switch (BitsPerPixel)
	{
	case 1:
		for (i = 0; i < 2 * 2; i++)
			*pal++ = stdpal1[i];
		break;
	case 2:
		for (i = 0; i < 4 * 3; i++)
			*pal++ = stdpal2[i];
		break;
	case 4:
		for (i = 0; i < 16 * 3; i++)
			*pal++ = stdpal4[i];
		break;
	case 8:
		for (i = 0; i < 256 * 3; i++)
			*pal++ = stdpal8[i];
		break;
	}

	smurf_struct->smurf_pic->pic_data = ziel;
	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
