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
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[1][Kein PCX Bild innerhalb|der CAS FAX Struktur][ Pech ]"
#define ERROR2 "[1][Es kann nur das erste Bild|angezeigt werden!][ Leider ]"
#define ERROR3 "[1][Eingebautes Bild hat|nicht das PCX Format!][ Pech ]"
#elif COUNTRY==0
#define ERROR1 "[1][No PCX image within the|CAS FAX structure!][Oh dear!]"
#define ERROR2 "[1][Only the first image|can be displayed!][ What a pity ] "
#define ERROR3 "[1][Incorporated image is|not in a PCX format!][ Pity ]"
#elif COUNTRY==2
#define ERROR1 "[1][No PCX image within the|CAS FAX structure!][Oh dear!]"
#define ERROR2 "[1][Only the first image|can be displayed!][ What a pity ] "
#define ERROR3 "[1][Incorporated image is|not in a PCX format!][ Pity ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#define NOMEM   ( (void*) 0L )
#define DEBUG           0

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"DCX-CAS-Fax Importer",
	0x0050,
	"Dale Russell",
	{ "DCX", "", "", "", "", "", "", "", "", "" },
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


/*************************************************************/
/*  PCX ggf. dekodieren                                      */
/*************************************************************/
static uint8_t *decodePCX(uint8_t *buffer, short BytePerLine, short height)
{
	short x, c, xc, yc;
	uint8_t *output;
	uint8_t *RetBuf;

	output = (uint8_t *)Malloc((long) BytePerLine * (long) height);
	if (!output)
		return NULL;
	RetBuf = output;
	yc = 0;
	while (yc++ < height)
	{
		xc = 0;
		while (xc < BytePerLine)
		{
			x = *(buffer++);
			if (x >= 192)
			{
				x -= 192;
				xc += x;
				c = *(buffer++);
				while (x--)
					*(output++) = c;
			} else
			{
				*(output++) = x;
				xc++;
			}
		}
	}
	return RetBuf;
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*      CAS FAX Standard                            */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *GetBuf;
	uint8_t *out2;
	short width;
	short height;
	short PicCnt;
	short x;
	long PicOffset[1024];
	short lBorder, rBorder, uBorder, dBorder;
	short BytePerLine, KompFlag;

	/***************************************************************/
	/*      Kopfdaten analysieren                                  */
	/***************************************************************/
	buffer = smurf_struct->smurf_pic->pic_data;

	smurf_struct->services->reset_busybox(128, "CAS Fax 1 Bit");

	if ((*(buffer) != 0xb1) || (*(buffer + 1) != 0x68) || (*(buffer + 2) != 0xde) || (*(buffer + 3) != 0x3a))
		return M_INVALID;

	PicCnt = 0;							/* BildzÑhler */
	for (x = 0; x < 1024; x++)			/* Eintragen der Offsets */
	{
		GetBuf = buffer + 0x04 + (x << 2);
		PicOffset[x] = (long) *(GetBuf) + (long) (*(GetBuf + 1) << 8) + (long)
			(*(GetBuf + 2) << 16L) + (long) (*(GetBuf + 3) << 24L);
		if (PicOffset[x] != 0L)
			PicCnt++;
	}
	if (!PicCnt)
	{
		form_alert(1, ERROR1);
		return M_PICERR;
	}
	if (PicCnt > 1)
		form_alert(1, ERROR2);

	strcpy(smurf_struct->smurf_pic->format_name, "CAS Fax Standard     ");

	/**********************************************************/
	/*  Auslesen der PCX Struktur                             */
	/**********************************************************/
	GetBuf = buffer + PicOffset[0];		/* 1. Bild */
	if (*(GetBuf) != 0x0a)
	{
		form_alert(1, ERROR3);
		return M_PICERR;
	}
	KompFlag = *(GetBuf + 2);
	lBorder = *(GetBuf + 4) + (*(GetBuf + 5) << 8);
	uBorder = *(GetBuf + 6) + (*(GetBuf + 7) << 8);
	rBorder = *(GetBuf + 8) + (*(GetBuf + 9) << 8);
	dBorder = *(GetBuf + 10) + (*(GetBuf + 11) << 8);
	BytePerLine = *(GetBuf + 0x42) + (*(GetBuf + 0x43) << 8);
	width = (rBorder - lBorder) + 1;
	height = (dBorder - uBorder) + 1;

	if (KompFlag)
	{
		out2 = decodePCX(GetBuf + 128, BytePerLine, height);
		if (out2 == NULL)
			return M_MEMORY;
		Mfree(buffer);
		buffer = out2;
	} else
	{
		buffer = GetBuf + 128;			/* Rohdaten */
	}

	/***************************************************************/
	/*      Strukturen fÅllen                                      */
	/***************************************************************/
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;
	smurf_struct->smurf_pic->bp_pal = 0;
	smurf_struct->smurf_pic->pic_data = buffer;
	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	return M_PICDONE;
}
