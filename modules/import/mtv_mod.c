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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#define DEBUGMODE   0

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"MTV Raytracer",
	0x0010,
	"Dale Russell",
	{ "MTV", "", "", "", "", "", "", "", "", "" },
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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*      MTV Raytracer                               */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *retbuf;
	unsigned short width, height;
	char extend[4];
	char wid[10];
	char hei[10];
	char *filenam;
	int i;

	filenam = smurf_struct->smurf_pic->filename;
	strcpy(extend, filenam + (strlen(filenam) - 3));
	buffer = smurf_struct->smurf_pic->pic_data;
	if (stricmp(extend, "MTV") != 0)
		return M_INVALID;

	smurf_struct->services->reset_busybox(128, "MTV Raytracer 24 Bit");

	memset(&wid[0], 0, sizeof(wid));
	memset(&hei[0], 0, sizeof(hei));

	buffer = smurf_struct->smurf_pic->pic_data;

/* Width ASCII */

	retbuf = buffer;
	for (i = 0; i < 8; i++)
		if (*(retbuf) != ' ')
			wid[i] = *(retbuf++);

	retbuf++;

/* Height ASCII */

	for (i = 0; i < 8; i++)
		if (*(retbuf) != 0x0a)
			hei[i] = *(retbuf++);

	width = atoi(wid);
	height = atoi(hei);

	retbuf = (uint8_t *)Malloc((unsigned long) width * height * 3);
	if (retbuf == NULL)
		return M_MEMORY;
	memcpy(retbuf, buffer + 8, (unsigned long) width * height * 3);
	Mfree(buffer);

	smurf_struct->smurf_pic->depth = 24;
	smurf_struct->smurf_pic->pic_data = retbuf;
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	strcpy(smurf_struct->smurf_pic->format_name, "MTV Raytracer (MTV)");
	return M_PICDONE;
}
