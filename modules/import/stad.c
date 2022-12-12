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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#define DEBUG           0

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Atari Stad-Grafik",
	0x0010,
	"Dale Russell",
	{ "PAC", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*      PAC Atari STAD Grafik                       */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	short width, height;
	short ID_Byte;
	short Pack_Byte;
	short Special_Byte;
	short xc, yc, x, c, n;
	uint8_t *buf;
	uint8_t *out;
	uint8_t *oout;
	short HorzMode;
	short ypos;
	short ytab[401];
	uint8_t *pal;

/***************************************************************/
/*      Kopfdaten analysieren                                  */
/***************************************************************/
	buffer = smurf_struct->smurf_pic->pic_data;
	if (strncmp((char *)buffer, "pM85", 4) == 0)
		HorzMode = TRUE;
	else if (strncmp((char *)buffer, "pM86", 4) == 0)
		HorzMode = FALSE;
	else
		return M_INVALID;

	smurf_struct->services->reset_busybox(128, "STAD 1 Bit");

/*************************************************************/
/*  Headerdaten auslesen                                     */
/*************************************************************/

	ID_Byte = *(buffer + 4);
	Pack_Byte = *(buffer + 5);
	Special_Byte = *(buffer + 6);

	strcpy(smurf_struct->smurf_pic->format_name, "Atari STAD Grafik");
 	pal = smurf_struct->smurf_pic->palette;
	pal[0] = pal[1] = pal[2] = 255;
	pal[3] = pal[4] = pal[5] = 0;

	width = 640;
	height = 400;
	buf = buffer + 0x07;				/* Datenbeginn */
	out = (uint8_t *)Malloc(32000L);
	if (!out)
		return M_MEMORY;
	memset(out, 0, 32000L);
	oout = out;
#if DEBUG>0
	(void) Cconws("\33E") printf("\n\n        -> Id-Byte:%i", ID_Byte);
	printf("\n      -> Pack-Byte:%i", Pack_Byte);
	printf("\n      -> Special-Byte:%i", Special_Byte);
	printf("\n      -> VMode:%i --- HMode:%i\n", VertMode, HorzMode);
#if DEBUG>1
	for (x = 0; x < 256; x++)
	{
		printf("\nPos.%x (=%i) = %x (=%i)", x, x, *(buffer + x), *(buffer + x));
		(void)Cnecin();
	}
#endif
#endif

/***************************************************************/
/*      Horizontale Dekodierung                                */
/***************************************************************/
	if (HorzMode)
	{
		yc = 0;
		while (yc++ < height)
		{
			xc = 0;
			while (xc < 80)
			{
				x = *(buf++);
				if (x == ID_Byte)
				{
					n = *(buf++) + 1;
					xc += n;
					while (n--)
						*(out++) = Pack_Byte;
				} else if (x == Special_Byte)
				{
					c = *(buf++);
					n = *(buf++) + 1;
					xc += n;
					while (n--)
						*(out++) = c;
				} else
				{
					*(out++) = x;
					xc++;
				}
			}
		}
	}
/***************************************************************/
/*      Vertikale Dekodierung                                  */
/***************************************************************/
	else
	{

/*  Y-Tabelle fllen */

		for (x = 0; x <= 400; x++)
			ytab[x] = (x << 6) + (x << 4);	/* ypos x 80 */

		xc = x = ypos = 0;
		yc = 0;
		while (xc < 80)
		{
			if (yc >= 400)
				yc -= 400;
			while (yc < 400)
			{
				x = *(buf++);
				if (x == ID_Byte)
				{
					n = *(buf++) + 1;
					yc += n;
					while (n--)
					{
						*(out + xc + ytab[ypos]) = Pack_Byte;
						ypos++;
						if (ypos > 399)
						{
							ypos = 0;
							xc++;
						}
					}
				} else if (x == Special_Byte)
				{
					c = *(buf++);
					n = *(buf++) + 1;
					yc += n;
					while (n--)
					{
						*(out + xc + ytab[ypos]) = c;
						ypos++;
						if (ypos > 399)
						{
							ypos = 0;
							xc++;
						}
					}
				} else
				{
					*(out + xc + ytab[ypos]) = x;
					ypos++;
					yc++;
					if (ypos > 399)
					{
						ypos = 0;
						xc++;
					}
				}
			}							/* y-loop */
		}								/* x-loop */
	}
/***************************************************************/
/*      Strukturen fllen                                      */
/***************************************************************/
	Mfree(buffer);
	smurf_struct->smurf_pic->pic_data = oout;
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;
	smurf_struct->smurf_pic->bp_pal = 0;
	smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	smurf_struct->smurf_pic->col_format = WURSCHT;
	return M_PICDONE;
}
