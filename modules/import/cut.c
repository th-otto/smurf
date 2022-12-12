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
/*			Dr.Halo Import-Format - Hallo Doktor!			*/
/*  Importformat fÅr das Programm Dr.Halo, da das Programm- */
/*  eigene Format nicht zur Speicherung von Bildern geeignet*/
/*  ist ...													*/
/* Version 0.1  --  07.01.96								*/
/*    Mehr schlecht als recht, eher zum kotzen				*/
/* Version 0.2  --  12.01.96								*/
/*    Da es ja irgendwann immer klappt, geht's nun			*/
/* Version 0.3  --  27.01.96								*/
/*	  Nun, ich hatte mich wohl geirrt, da hat nix geklappt.	*/
/*	  Leider ist nicht mehr eindeutig zu reproduzieren,		*/
/*	  weshalb ich mich zu so einer riskanten Aussage hin-	*/
/*	  reiûen lies. Apropos lies, Du, Christian, solltest das*/
/*	  nÑchste Mal Deine Kommentare etwas lesbarer schreiben.*/
/* Version 0.4  --  06.02.96								*/
/*	  In meinem Testbild war zwar was zu erkennen, aber das */
/*	  gelbe vom Ei war es nicht. Entgegen meiner Meinung, es*/
/*	  ginge eben nicht anders, geht es nun doch problemlos.	*/
/*	  Die Lîsung: Klaus hatte wiedermal Scheiûe erzÑhlt ...	*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[1][Die Farbpalette konnte nicht|gefunden werden!][ OK ]"
#define ERROR2 "[1][FÅr die Farbpalette steht nicht|genug Speicher zur VerfÅgung!][ OK ]"
#elif COUNTRY==0
#define ERROR1 "[1][The colour palette could|not be found!][ OK ]"
#define ERROR2 "[1][Not enough memory available for|the colour palette!][ OK ]"
#elif COUNTRY==2
#define ERROR1 "[1][The colour palette could|not be found!][ OK ]"
#define ERROR2 "[1][Not enough memory available for|the colour palette!][ OK ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Dr. Halo Import-Modul",
	0x0040,
	"Christian Eyrich",
	{ "CUT", "PAL", "", "", "", "", "", "", "", "" },
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

#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/* --- FLOAD --- */

static void *fload(const char *Path, short header)
{
	uint8_t *fil;
	int file;
	long f_len;

	if ((file = (int)Fopen(Path, FO_READ)) >= 0)	/* Datei îffnen */
	{
		f_len = Fseek(0, file, 2);		/* LÑnge ermitteln */

		if ((fil = (uint8_t *)Malloc((long) f_len - header)) == 0)
		{
			form_alert(1, ERROR2);
			fil = 0;
		} else
		{
			Fseek(header, file, 0);		/* Anfang suchen */
			Fread(file, f_len - header, fil);	/* Palette lesen */
		}
		Fclose(file);					/* Kanal schlieûen */
	} else
	{
		fil = NULL;
	}

	return fil;
}


static char *fileext(const char *filename)
{
	char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Dr.Halo (Pferdedoktor)  .CUT + .PAL			*/
/*		8 Bit, RLE									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint8_t *ppal;
	char *fname;
	uint8_t n;
	char name[257];

	unsigned short x, y;
	unsigned short i, v1, v2;
	unsigned short width, height;
	unsigned short maxr, maxg, maxb;
	unsigned short cols;

	unsigned long pal_pos = 0;


	buffer = smurf_struct->smurf_pic->pic_data;

	/* Header Check */
	fname = smurf_struct->smurf_pic->filename;
	if (stricmp(fileext(fname), "CUT") != 0 || *(uint16_t *) (buffer + 0x04) != 0)
		return M_INVALID;
	width = swap_word(*(uint16_t *) buffer);
	height = swap_word(*(uint16_t *) (buffer + 0x02));

	strcpy(smurf_struct->smurf_pic->format_name, "Dr. Halo Import .CUT");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 8;

	smurf_struct->services->reset_busybox(128, "Dr. Halo 8 Bit");

	/* Dr.Halo entpacken */
	if ((ziel = smurf_struct->services->SMalloc((long) width * (long) height)) == 0)
		return M_MEMORY;

	oziel = ziel;

	obuffer = buffer;
	buffer += 6;

	y = 0;
	do
	{
		x = *buffer++;
		x |= *buffer++ << 8;

		do
		{
			v1 = *buffer++;
			x--;

			if ((n = (v1 & 0x7f)) != 0)
			{
				if (v1 & 0x80)
				{
					v2 = *buffer++;
					x--;

					for (i = 0; i < n; i++)
						*ziel++ = v2;
				} else
				{
					for (i = 0; i < n; i++)
					{
						*ziel++ = *buffer++;
						x--;
					}
				}
			}
		} while (x > 0);
	} while (++y < height);

	ziel = oziel;
	buffer = obuffer;

	smurf_struct->smurf_pic->pic_data = ziel;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	/* Palettenname zusammenstÅckeln */
	strcpy(name, fname);
	strcpy(fileext(name), "pal");

	/* Palette laden */
	/* Vorsicht, Palette liegt in Blîcken vor */
	pal = smurf_struct->smurf_pic->palette;

	if ((ppal = fload(name, 0)) == 0)
	{
		form_alert(1, ERROR1);

		for (i = 0; i < 256; i++)
		{
			pal[pal_pos++] = i;
			pal[pal_pos++] = i;
			pal[pal_pos++] = i;
		}
	} else
	{
		cols = swap_word(*(uint16_t *) (ppal + 0x0c));
		maxr = swap_word(*(uint16_t *) (ppal + 0x0e));
		maxg = swap_word(*(uint16_t *) (ppal + 0x10));
		maxb = swap_word(*(uint16_t *) (ppal + 0x12));

		ppal += 40;
		for (i = 0; i < cols; i++)
		{
#if 0
			if (512 - i * 6 < 6)
				ppal += 512 - i * 6;
#endif
			*pal++ = ((ppal[0] + (ppal[1] << 8)) * 255) / maxr;
			*pal++ = ((ppal[2] + (ppal[3] << 8)) * 255) / maxg;
			*pal++ = ((ppal[4] + (ppal[5] << 8)) * 255) / maxb;
			ppal += 6;
		}
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}


