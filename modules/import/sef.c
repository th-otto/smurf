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
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"SEF-Importer",
	0x0050,
	"Dale Russell",
	{ "SEF", "", "", "", "", "", "", "", "", "" },
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
/*      SEF-Importer                                */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *Out;
	uint8_t *Merk;
	uint8_t *buf;
	uint8_t *m1;
	uint8_t *m2;
	unsigned short x, y, width, height;
	long off;
	char *filenam;
	char extend[4];

	buffer = smurf_struct->smurf_pic->pic_data;

	/*****************************************************/
	/*          MAGIC Code berprfen                    */
	/*****************************************************/
	filenam = smurf_struct->smurf_pic->filename;
	strcpy(extend, filenam + (strlen(filenam) - 3));
	if (stricmp(extend, "SEF") != 0)
		return M_INVALID;

	smurf_struct->services->reset_busybox(128, "SEF 24 Bit");

	width = (*(buffer + 286) << 8) + (*(buffer + 287));
	height = (*(buffer + 290) << 8) + (*(buffer + 291));

	Out = (uint8_t *)Malloc((unsigned long) width * height * 3);
	if (!Out)
		return M_MEMORY;
	Merk = Out;
	buf = buffer + 2258;
	off = (unsigned long) width * height;

	m1 = buf + off;
	m2 = m1 + off;

	/********* Make Interleaved Bitplanes for 24Bit mode ********/
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			*(Out++) = *(m2++);
			*(Out++) = *(m1++);
			*(Out++) = *(buf++);
		}
	}
	Mfree(buffer);
	strcpy(smurf_struct->smurf_pic->format_name, ".SEF (?)");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 24;
	smurf_struct->smurf_pic->bp_pal = 0;
	smurf_struct->smurf_pic->pic_data = Merk;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->col_format = BGR;
	return M_PICDONE;
}
