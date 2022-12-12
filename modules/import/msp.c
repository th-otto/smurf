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
/*						Microsoft Paint						*/
/* Version 0.1  --  irgendwann Anfang 1996					*/
/*	  1 Bit, Version 1 und 2								*/
/* Version 0.2  --  31.01.97								*/
/*	  AbstÅrze bei manchen Bildern behoben und fÅr Block-	*/
/*	  routinen vorbereitet.									*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Microsoft Paint",
	0x0020,
	"Dale Russell, Christian Eyrich",
	{ "MSP", "", "", "", "", "", "", "", "", "" },
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
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Microsoft Paint					*/
/*		1 Bit, unkomprimiert, RLE					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint8_t version;
	int i;
	int vpos;
	uint8_t v1, v2;
	uint8_t n;

	uint16_t *Scanmap;
	uint16_t *Scanpoint;
	unsigned short width, height, x, y;
	uint16_t valid;
	unsigned long w;


	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (strncmp((char *)buffer, "DanM", 4) == 0)
		version = 1;
	else if (strncmp((char *)buffer, "LinS", 4) == 0)
		version = 2;
	else
		return M_INVALID;

	for (vpos = 0, valid = 0, i = 0; i < 13; i++)
	{
		valid = valid ^ *(uint16_t *) (buffer + vpos);
		vpos += 2;
	}

	if (valid != 0)
		return M_PICERR;

	width = swap_word(*(uint16_t *) (buffer + 0x04));
	height = swap_word(*(uint16_t *) (buffer + 0x06));

	strcpy(smurf_struct->smurf_pic->format_name, "Microsoft Paint");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;

	smurf_struct->services->reset_busybox(128, "Microsoft Paint 1 Bit");

	w = (unsigned long) (width + 7) / 8;
	if ((ziel = smurf_struct->services->SMalloc(w * height)) == 0)
		return M_MEMORY;

	if ((Scanmap = (uint16_t *) smurf_struct->services->SMalloc((unsigned long) height * 2)) == NULL)
	{
		smurf_struct->services->SMfree(ziel);
		return M_MEMORY;
	}

	Scanpoint = (uint16_t *) (buffer + 0x20);
	for (x = 0; x < height; x++)
		Scanmap[x] = swap_word(*Scanpoint++);	/* LÑngewerte Åbertragen */

	buffer += 0x20;
	if (version == 2)
		buffer += (unsigned long) height * 2L;

	oziel = ziel;

	y = 0;
	do
	{
		if (version == 1)
		{
			memcpy(ziel, buffer, w);
			ziel += w;
			buffer += w;
		} else
		{
			if (Scanmap[y])
			{
				x = 0;
				do
				{
					v1 = *buffer++;
					if (v1)
					{
						n = v1;

						x += n;
						while (n--)
							*ziel++ = *buffer++;
					} else
					{
						n = *buffer++;
						v2 = *buffer++;

						x += n;
						while (n--)
							*ziel++ = v2;
					}
				} while (x < Scanmap[y]);
			} else
			{
				memset(ziel, 0xff, w);
				ziel += w;
			}
		}							/* version */
	} while (++y < height);			/* y */

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	pal = smurf_struct->smurf_pic->palette;

	pal[0] = 255;
	pal[1] = 255;
	pal[2] = 255;
	pal[3] = 0;
	pal[4] = 0;
	pal[5] = 0;

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(Scanmap);

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
