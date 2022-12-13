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

/* **************************************************** */
/*                                                      */
/* Highlight PC-Format - Importer                       */
/*      Format von: wei�ichnich                         */
/*      Importer von Olaf.                              */
/*                                                      */
/* Dieses Format braucht man, wie so viele andere,      */
/* ungef�hr so dringend wie eine Kugel zwischen den     */
/* Augen. Wahrlich kein Highlight. Und der Erfinder war */
/* mit Sicherheit auch keine Leuchte...                 */
/*                                                      */
/*  Nummer 4 der Unterrichtsgenerierten Importer.       */
/*   Mit Dale die ganze verdammte Nacht gecoded, dann   */
/*   fr�h um 8... naja, halb neun in die Schule und     */
/*   unwahrscheinlich tot gewesen. F�hl mich wie ne     */
/*   Leiche auf Urlaub, nur da� die wahrscheinlich      */
/*   nicht so m�de w�re.                                */
/*   Lt. Holtorf m��te der Import funktionieren - aber  */
/*   wer wei� das bei dem und seinem Buch schon...      */
/*                                                      */
/* **************************************************** */



#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"


MOD_INFO module_info = {
	"Highlight - Importer",
	0x0010,
	"Olaf Piesche",
/* Extensionen */
	{ "PIC", "", "", "", "", "", "", "", "", "" },
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



short imp_module_main(GARGAMEL *smurf_struct)
{
	short width;
	short height;
	short depth;
	uint8_t *picdata;
	uint16_t *pd;
	uint16_t *picd;
	long Len24, Len16, filelen;
	uint16_t p15;
	short x, y;
	short red, green, blue;

	picdata = smurf_struct->smurf_pic->pic_data;
	pd = (uint16_t *) picdata;
	filelen = smurf_struct->smurf_pic->file_len;

	width = *(pd);
	height = *(pd + 1);

/* Die spitzenm�ige Kennung ermitteln... */
	Len24 = (long) width * (long) height * 3;
	Len16 = (long) width * (long) height * 2;

	if (filelen - 4 == Len16)
		depth = 16;
	else if (filelen - 4 == Len24)
		depth = 24;
	else
		return M_INVALID;

	picd = pd;
	pd += 2;

	if (depth == 16)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				p15 = *(pd++);
				red = p15 & 0xf800;		/* RGB ausmaskieren */
				green = p15 & 0x07e0;
				blue = p15 & 0x003f;
				green >>= 1;			/* und R und B vertauschen. Ich wei� noch nicht, */
				red <<= 10;				/* ob das notwendig ist, da Klaus etwas mi�ver- */
				blue >>= 11;			/* st�ndliche Angaben macht (RGB oder BGR?) */
				*(picd++) = red | green | blue;
			}
		}

		_Mshrink(picdata, Len16);
	} else
	{
		memmove(picd, pd, Len24);
		_Mshrink(picd, Len24);
	}

	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = depth;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	strcpy(smurf_struct->smurf_pic->format_name, "Highlight PC-Format");

	return M_DONEEXIT;
}
