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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			SoftImage Format (PIC)					*/
/*		1, 4, 8 Bit, unkomprimiert					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

/* info structure for main module */
MOD_INFO module_info = {
	"Softimage",				/* Name of module */
	0x0100,						/* version */
	"Dodger",					/* author */
	/* 10 Extensionen fuer Importer */
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
	/* description for pictures */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

struct header
{
	uint32_t magic;               /* PIC_MAGIC_NUMBER 0x5380f634 */
	int32_t /* float */ version;  /* Version of format       */
	char comment[80];             /* Prototype description   */
	char id[4];                   /* "PICT"                  */
	short width;                  /* Image width, in pixels  */
	short height;                 /* Image height, in pixels */
	int32_t /* float */ ratio;    /* Pixel aspect ratio      */
	short fields;                 /* Picture field type      */
	short padding;                /* Unused                  */
};

/* Data type */
#define PIC_UNSIGNED_INTEGER	0x00
#define PIC_SIGNED_INTEGER		0x10	/* XXX: Not implemented */
#define PIC_SIGNED_FLOAT		0x20	/* XXX: Not implemented */


/* Compression type */
#define PIC_UNCOMPRESSED		0x00
#define PIC_PURE_RUN_LENGTH		0x01
#define PIC_MIXED_RUN_LENGTH	0x02

/* Channel types (OR'd) */
#define PIC_RED_CHANNEL			0x80
#define PIC_GREEN_CHANNEL		0x40
#define PIC_BLUE_CHANNEL		0x20
#define PIC_ALPHA_CHANNEL		0x10
#define PIC_SHADOW_CHANNEL		0x08	/* XXX: Not implemented */
#define PIC_DEPTH_CHANNEL		0x04	/* XXX: Not implemented */
#define PIC_AUXILIARY_1_CHANNEL	0x02	/* XXX: Not implemented */
#define PIC_AUXILIARY_2_CHANNEL	0x01	/* XXX: Not implemented */

short imp_module_main(GARGAMEL *smurf_struct)
{
	struct header *pic_data;
	uint16_t width, height;
	long data_len;
	long header_len;
	
	pic_data = smurf_struct->smurf_pic->pic_data;
	if (strncmp(pic_data->id, "PICT", 4) != 0)
		return M_INVALID;
	strcpy(smurf_struct->smurf_pic->format_name, "Softimage");
	width = pic_data->width;
	height = pic_data->height;
	data_len = ((unsigned long)width * height) * 3;
	if (smurf_struct->smurf_pic->file_len < data_len)
		return M_UNKNOWN_TYPE;
	header_len = smurf_struct->smurf_pic->file_len - data_len;

	/*
	 * That seems to be wrong.
	 * See http://paulbourke.net/dataformats/softimagepic/readpic.c
	 */
	memmove(pic_data, (uint8_t *)pic_data + header_len, data_len);

	_Mshrink(pic_data, data_len);
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 24;
	
	return M_PICDONE;
}
