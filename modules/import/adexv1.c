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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#define DEBUG       0
#define W_PIX       255
#define B_PIX       0

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Adex ChromaGraph Bitmap",
	0x0010,
	"Dale Russell",
	{ "IMG", "RLE", "", "", "", "", "", "", "", "" },
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
/*  ADEX ChromaGraph Bitmap                         */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer = smurf_struct->smurf_pic->pic_data;
	short BitsPerPixel = 0;
	short width = 0;
	short height = 0;
	short CodeFlag = 0;

#if DEBUG>0
	short Planes = 0;
	short Colors;
#endif
	uint8_t *retbuf;
	uint8_t *output;
	uint8_t *buf2;
	uint8_t *paldat;
	short bperz;
	short n;
	short c;
	short xc, yc;
	short ColCount;
	char dummy[3];
	char impmessag[17];

	/*****************************************************/
	/*          MAGIC Code �berpr�fen                    */
	/*****************************************************/
	if (strncmp((char *)buffer, "PICT", 4) != 0)
		return M_INVALID;

	/*****************************************************/
	/*      Kopfdaten auslesen                           */
	/*****************************************************/
	CodeFlag = *(buffer + 4);
	BitsPerPixel = *(buffer + 5);
	width = *(buffer + 6) + (*(buffer + 7) << 8);
	height = *(buffer + 8) + (*(buffer + 9) << 8);
#if DEBUG>0
	Colors = *(buffer + 10) + (*(buffer + 11) << 8);
#endif

	strcpy(impmessag, "ChromaGraph ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

#if DEBUG>0
	printf("\n  Width: %i", width);
	printf("\n  Height: %i", height);
	printf("\n  Depth: %i", BitsPerPixel);
	printf("\n  Planes: %i", Planes);
	printf("\n  Colors: %i", Colors);
	(void)Cnecin();
#endif
/*****************************************************/
/*      Palette auslesen                             */
/*****************************************************/
	if (BitsPerPixel == 8)
		ColCount = 256;
	else
		ColCount = 16;

	paldat = buffer + 0x0e;
	output = smurf_struct->smurf_pic->palette;
	for (xc = 0; xc < ColCount; xc++)
	{
		*(output++) = (*(paldat++) << 2);	/* r */
		*(output++) = (*(paldat++) << 2);	/* g */
		*(output++) = (*(paldat++) << 2);	/* b */

	}
	bperz = (width + ((8 / BitsPerPixel) - 1)) / (8 / BitsPerPixel);

/*****************************************************/
/*      Bilddaten ggf. dekodieren                    */
/*****************************************************/
	if (CodeFlag == TRUE)
	{
		output = (uint8_t *)Malloc((long) bperz * (long) height);
		if (output == NULL)
			return M_MEMORY;
		retbuf = output;
		yc = xc = 0;
		while (yc++ < height)
		{
			while (xc < bperz)
			{
				n = *(buffer++);
				c = *(buffer++);
				xc += n;
				while (n--)
					*(output++) = c;
			}
			xc -= bperz;
		}
		Mfree(smurf_struct->smurf_pic->pic_data);
	} else
	{
		retbuf = buffer;
	}

	if (BitsPerPixel == 4)
	{
		buf2 = (uint8_t *)Malloc((long) bperz * (long) height * 2L);
		if (buf2 == NULL)
			return M_MEMORY;
		buffer = buf2;
		output = retbuf;

		for (yc = 0; yc < height; yc++)
		{
			for (xc = 0; xc < bperz; xc++)
			{
				*(buf2++) = (*(retbuf) & 0xf0) >> 4;
				*(buf2++) = (*(retbuf++) & 0x0f);
			}
		}
		Mfree(output);
	} else
	{
		buffer = retbuf;
	}
	strcpy(smurf_struct->smurf_pic->format_name, "Adex ChromaGraph");
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->pic_data = buffer;
	smurf_struct->smurf_pic->depth = BitsPerPixel;
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	return M_PICDONE;
}
