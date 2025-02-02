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
/*					DA's Layout DIP-Preview					*/
/* Version 0.1  --  08.10.97								*/
/*	  24 Bit												*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

#undef WHITE							/* is used in AES.H */
#define WHITE	1
#define NWHITE	2
#define BANFANG	1
#define BMITTE	2
#define ZANFANG	1
#define ZMITTE	2

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"DA's Layout Preview",
	0x0010,
	"Christian Eyrich",
	{ "DIP", "", "", "", "", "", "", "", "", "" },
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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				DA's Layout DIP-Preview				*/
/*		24 Bit, eigene Kompression					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	uint8_t align;
	uint8_t status;
	uint8_t pos;
	uint8_t pix;
	uint8_t run;
	uint8_t pixel;
	char dummy[3];
	char impmessag[21];

	unsigned short x, y;
	unsigned short width, height;
	unsigned short realwidth;


	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	/* kein DIP oder kein Preview enthalten */
	if (strncmp((char *)buffer, "DALAYOUT", 8) != 0 || *(uint16_t *) (buffer + 0x0a) == 0x001e)
		return M_INVALID;
	BitsPerPixel = 24;

	width = *(uint16_t *) (buffer + 0x1c);
	realwidth = width * 3;
	height = *(uint16_t *) (buffer + 0x1e);

	DatenOffset = 0x24;

	strcpy(smurf_struct->smurf_pic->format_name, "DA's DIP-Preview");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "DIP-Preview ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if ((ziel = smurf_struct->services->SMalloc((long) width * (long) height * 3L)) == 0)
		return M_MEMORY;
	oziel = ziel;

	buffer += DatenOffset;

	y = 0;
	do
	{
		align = BANFANG;
		status = WHITE;
		pos = ZANFANG;

		x = 0;
		do
		{
			pix = *buffer++;

			/* restliche Pixel der Zeile wei� machen, also �berspringen */
			if (pos == ZMITTE)
			{
				ziel += (width - x) * 3;
				buffer--;		/* n�chstes Statement nochmal */
				break;
			}

			/* ganze Zeile wei�, also �berspringen */
			if (pix == width)
			{
				ziel += realwidth;

				x = width;
			} else
			{
				/* pix Pixel wei�, also �berspringen */
				if (status == WHITE)
				{
					ziel += pix * 3;

					status = NWHITE;
				} else
				{
					run = pix;
					while (run--)
					{
						if (align == BANFANG)
						{
							pixel = *buffer++;
							*ziel++ = pixel & 0xf0;
							*ziel++ = pixel << 4;
							pixel = *buffer++;
							*ziel++ = pixel & 0xf0;
							align = BMITTE;
						} else
						{
							*ziel++ = pixel << 4;
							pixel = *buffer++;
							*ziel++ = pixel & 0xf0;
							*ziel++ = pixel << 4;
							align = BANFANG;
						}
					}

					pos = ZMITTE;
				}

				x += pix;
			}
		} while (x < width);
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	smurf_struct->smurf_pic->col_format = CMY;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
