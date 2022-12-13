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
/*                  Edsung Labs-Format						*/
/* Version 0.1  --  28.02.96                                */
/*	  xxx													*/
/*==========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#undef min
#define min(a,b)             ((a) < (b) ? (a) : (b))

#define TIMER 0

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Edsun Labs-Format",
	0x0010,
	"Christian Eyrich",
	{ "CEG", "", "", "", "", "", "", "", "", "" },
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
/*				Edsun Labs-Format (CEG)				*/
/*		24 Bit										*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	unsigned short x, y, width, height;

	unsigned long src_pos;
	unsigned long dst_pos;


#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	buffer = smurf_struct->smurf_pic->pic_data;

	if (strcmp((char *)buffer + 1, "EDSUN") != 0)
		return M_INVALID;
	BitsPerPixel = (*buffer + 0x0b) * 3;

	width = *(buffer + 0x67) + (*(buffer + 0x68) << 8);
	height = *(buffer + 0x69) + (*(buffer + 0x70) << 8);

	strcpy(smurf_struct->smurf_pic->format_name, "Edsun Labs .CEG");
	strncpy(smurf_struct->smurf_pic->infotext, (char *)buffer + 0x15, min(80, sizeof(smurf_struct->smurf_pic->infotext)) - 1);
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	smurf_struct->services->reset_busybox(128, "Edsun Labs 24 Bit");

	if ((ziel = smurf_struct->services->SMalloc((long) width * (long) height * 3)) == 0)
		return M_MEMORY;
	DatenOffset = 0x3f4;
	src_pos = DatenOffset;
	dst_pos = 0;

	y = 0;
	do							/* height */
	{
		x = 0;
		do						/* width */
		{
			ziel[dst_pos++] = buffer[src_pos++];
			ziel[dst_pos++] = buffer[src_pos++];
			ziel[dst_pos++] = buffer[src_pos++];
			x++;
		} while (x < width);
		y++;
	} while (y < height);

	smurf_struct->smurf_pic->pic_data = ziel;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu", get_timer());
	(void)Cnecin();
#endif

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
