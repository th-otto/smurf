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

/* Effizienz des LZW-Encoders ist viel zu niedrig.
   Siehe WZ_LZW_SM.TIF vs. WZ_LZW_PS.TIF.
   Photoshop komprimiert 14,5% besser! */

/* =========================================================*/
/*							TIFF Exporter					*/
/* Version 0.1  --  23.04.98 - 24.04.98						*/
/*	  Header und 1 Bit unkomprimiert						*/
/* Version 0.2  --  25.04.98								*/
/*	  8 Bit Graustufen, Palettenbilder und 24 Bit eingebaut	*/
/* Version 0.3  --  02.05.98 und 18.5.98					*/
/*	  Fehler bei 8 Bit Export gefixt. Es wurde der Funktion	*/
/*	  fest zielbuf statt actziel als Ziel Åbergeben.		*/
/* Version 0.4  --  17.10.98								*/
/*	  Fehler in Bedienung behoben. Bei Verlassen per Return	*/
/*	  wurde der Dialog zwar geschlossen, die Config aber	*/
/*	  nicht gespeichert.									*/
/* Version 0.5  --  22.10.2000								*/
/*	  Schande, longborder hatte nie funktioniert (immer 0	*/
/*	  geliefert). D.h. Long-Tags in Intel-TIFF waren fast	*/
/*	  immer ungÅltig.										*/
/*	  Davon abgesehen wurden pStripOffsets und				*/
/*	  pStripByteCounts Åberhaupt nicht in Intelschreibweise	*/
/*	  gespeichert ...										*/
/* Version 0.6  --  4.11.2000								*/
/*	  4 Bit Bilder werden jetzt korrekt geschrieben wenn	*/
/*	  sie aus mehr als einem Streifen bestehen.				*/
/* Version 0.7  --  5.11.2000 - 6.11.2000					*/
/*	  LZW-Kodierung eingebaut								*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "tiff_lzw.h"

#define PRG		0
#define TIMER 0

#include "country.h"

#if COUNTRY == 1
#include "de/tiffrsc.rsh"
#elif COUNTRY == 0
#include "en/tiffrsc.rsh"
#elif COUNTRY == 2
#include "en/tiffrsc.rsh"				/* missing french resource */
#else
#error "Keine Sprache!"
#endif


#define BILEVEL 1
#define GREYSCALE	2
#define PALETTE 3
#define FULLCOLOR 4

#define II 0x4949
#define MM 0x4d4d

#define T_BYTE 1
#define T_ASCII 2
#define T_SHORT 3
#define T_LONG 4
#define T_RATIONAL 5

typedef struct
{
	uint8_t comp;						/* Kompression */
	uint8_t byteorder;					/* Byte Order */
} CONFIG;

#if !PRG
static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);
#else
static void *SMalloc(long amount);
static void SMfree(void *ptr);
#endif

/* Dies bastelt direkt ein rol.w #8,d0 inline ein. */
#ifdef __PUREC__
static unsigned short swap_word(unsigned short w) 0xE058;
static unsigned long swap_word2(unsigned long w) 0xE058;
static uint32_t swap_d0(uint32_t w) 0x4840;
#define swap_long(w) swap_word2(swap_d0(swap_word2(w)))
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
static uint32_t swap_long(uint32_t w)
{
	return ((w >> 24)) | ((w & 0xff0000L) >> 8) | ((w & 0xff00L) << 8) | ((w & 0xffL) << 24);
}
#endif

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"TIFF",
	0x0070,
	"Christian Eyrich",
	{ "TIFF", "", "", "", "", "", "", "", "", "" },
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


MOD_ABILITY module_ability = {
	1, 4, 8, 24, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	M_MORE									/* More */
};

static uint8_t Intel;
static uint8_t headwrite;
static unsigned long *pStripByteCounts;
static uint32_t *pStripOffsets;
static unsigned short StripsPerPic, RowsPerStrip;


/* TIFF mit 1 Bit schreiben */
static long writeTIFFdata_bi(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height)
{
	uint8_t *oziel;
	unsigned short x, y;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
		} while (++x < w);
	} while (++y < height);

	return ziel - oziel;
}


/* TIFF mit 8 Bit Graustufen schreiben */
static long writeTIFFdata_grey(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height)
{
	uint8_t *oziel;
	unsigned short x, y;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
		} while (++x < w);
	} while (++y < height);

	return ziel - oziel;
}


/* TIFF 4 Bit Palettenbilder schreiben - muû leider wegen 2 Pixel in ein Byte in eigene Funktion */
static long writeTIFFdata_pal4(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short realheight, unsigned short height)
{
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t *line;
	unsigned short x, y;
	unsigned long planelength;
	unsigned long w;
	unsigned long realwidth;

	oziel = ziel;

	realwidth = (width + 7) / 8;
	w = (width + 1) / 2;

	planelength = realwidth * realheight;

	pixbuf = SMalloc(width + 7);

	y = 0;
	do
	{
		memset(pixbuf, 0, width);
		getpix_std_line(buffer, pixbuf, 4, planelength, width);
		buffer += realwidth;
		line = pixbuf;

		x = 0;
		do
		{
			*ziel++ = (*line++ << 4) | *line++;
		} while (++x < w);
	} while (++y < height);

	SMfree(pixbuf);

	return ziel - oziel;
}


/* TIFF 8 Bit Palettenbilder schreiben */
static long writeTIFFdata_pal8(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height)
{
	uint8_t *oziel;
	unsigned short x, y;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
		} while (++x < w);
	} while (++y < height);

	return ziel - oziel;
}


/* TIFF 24 Bit Fullcolorbilder schreiben */
static long writeTIFFdata_rgb(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height)
{
	uint8_t *oziel;
	unsigned short x, y;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
			*ziel++ = *buffer++;
			*ziel++ = *buffer++;
		} while (++x < width);
	} while (++y < height);

	return ziel - oziel;
}



/* TIFF mit 1, 8 und 24 Bit PackBits kodieren */
static long encode_RLE(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height)
{
	uint8_t *oziel;
	uint8_t pixel;
	uint8_t counter;
	uint8_t nextenc;
	unsigned short x, y, xx;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			counter = 1;
			pixel = *buffer++;
			x++;

			xx = x;
			while (*buffer == pixel && counter < 0x7f && xx < w)
			{
				buffer++;
				xx++;
				counter++;
			}

			if (counter >= 2)			/* es lohnt sich! */
			{
				*ziel++ = 0x101 - counter;
				*ziel++ = pixel;
				x = xx;
			} else						/* ... aber leider nicht! */
			{
				ziel++;					/* Header fÅr Literal Run schreiben */
				*ziel++ = pixel;

				do
				{
					nextenc = 1;

					while (*buffer != *(buffer + 1) && counter < 0x7f && xx < w)
					{
						*ziel++ = *buffer++;
						xx++;
						counter++;
					}

					while (*buffer == *(buffer + 1) && nextenc < 4 && counter < 0x7f && xx < w)
					{
						*ziel++ = *buffer++;
						xx++;
						counter++;
						nextenc++;
					}
				} while (nextenc < 4 && counter < 0x7f && xx < w);

				if (nextenc > 3)
				{
					counter -= nextenc - 1;
					xx -= nextenc - 1;
					buffer -= nextenc - 1;
					ziel -= nextenc - 1;
				}
#if 0
				if (counter < 4)
				{
					if (counter == 1)
					{
						ziel -= 2;
						*ziel++ = 1;
						*ziel++ = pixel;
					} else
					{
						ziel -= 4;
						*ziel++ = 1;
						*ziel++ = pixel;
						*ziel++ = 1;
						*ziel++ = *(buffer - 1);
					}
				} else
#endif
				*(ziel - counter - 1) = counter - 1;

				x = xx;
			}
		} while (x < w);
	} while (++y < height);

	return ziel - oziel;
}


/* Schreibt je nach Byteformat Motorola oder Intel ints */
static unsigned short intborder(unsigned short input)
{
	if (Intel)
		input = swap_word(input);
	return input;
}


/* Schreibt je nach Byteformat Motorola oder Intel longs */
static unsigned long longborder(unsigned long input)
{
	if (Intel)
		input = swap_long(input);
	return input;
}


static void write_IFDEntry(uint8_t *ziel, short *dcount, unsigned short tag, unsigned short type, unsigned long count, unsigned long value, unsigned short *ifds)
{
	if (headwrite)						/* Nicht bei Pass 1 ausfÅhren */
	{
		ziel += *dcount;

		*(uint16_t *) ziel = intborder(tag);
		*(uint16_t *) (ziel + 2) = intborder(type);
		*(uint32_t *) (ziel + 4) = longborder(count);

		if (type == T_BYTE)
			*(ziel + 8) = (uint8_t) value;
		else if (type == T_SHORT && count == 1)
			*(uint16_t *) (ziel + 8) = intborder((uint16_t) value);
		else if (type == T_LONG || type == T_RATIONAL || count > 1)
			*(uint32_t *) (ziel + 8) = longborder(value);
	}

	*dcount += 12;
	(*ifds)++;
}



/* TIFF-Headerstruktur schreiben */
static unsigned short write_header(uint8_t *ziel, SMURF_PIC *smurf_pic, CONFIG *config, uint8_t format, unsigned short headsize, long w)
{
	uint8_t *pal;
	uint8_t BitsPerPixel;
	short i, cols, dcount, bps;
	uint16_t *ppal;
	uint16_t comp;
	uint16_t ifds = 0;
	uint16_t pallen;

	unsigned long PalOffset;
	unsigned long StripOffsets;
	unsigned long StripByteCounts;
	unsigned long help;


	BitsPerPixel = smurf_pic->depth;

	if (config->comp == KEINE)
		comp = 1;
	else if (config->comp == HUFFMAN)
		comp = 2;
	else if (config->comp == LZW)
		comp = 5;
	else if (config->comp == RLE)
		comp = 32773U;

	cols = 1 << BitsPerPixel;
	if (BitsPerPixel > 1 && BitsPerPixel <= 8 && smurf_pic->col_format != GREY)
		pallen = 6 * cols;
	else
		pallen = 0;

	if (headwrite)						/* nicht bei Pass 1 ausfÅhren */
	{
		if (config->byteorder == MOTO)
			*(uint16_t *) ziel = MM;
		else
			*(uint16_t *) ziel = II;

		*(uint16_t *) (ziel + 2) = intborder(42);

		*(uint32_t *) (ziel + 4) = longborder(28);

		strcpy(ziel + 8, "Smurf TIFF-Exporter");
	}

	dcount = 30;
	write_IFDEntry(ziel, &dcount, 0xfe, T_LONG, 1, 0, &ifds);	/* New Subfile Type */
	write_IFDEntry(ziel, &dcount, 0x100, T_SHORT, 1, smurf_pic->pic_width, &ifds);	/* Bildbreite */
	write_IFDEntry(ziel, &dcount, 0x101, T_SHORT, 1, smurf_pic->pic_height, &ifds);	/* Bildhîhe */
	if (BitsPerPixel >= 1 && BitsPerPixel <= 8)
	{
		bps = 0;
		write_IFDEntry(ziel, &dcount, 0x102, T_SHORT, 1, BitsPerPixel, &ifds);	/* BitsPerSample */
	}
	if (BitsPerPixel == 24)
	{
		bps = 6;
		help = headsize - bps - pallen;
		write_IFDEntry(ziel, &dcount, 0x102, T_SHORT, 3, help, &ifds);	/* BitsPerSample */
		if (headwrite)					/* nicht bei Pass 1 ausfÅhren */
		{
			*(uint16_t *) (ziel + help) = intborder(0x08);
			*(uint16_t *) (ziel + help + 2) = intborder(0x08);
			*(uint16_t *) (ziel + help + 4) = intborder(0x08);
		}
	}

	write_IFDEntry(ziel, &dcount, 0x103, T_SHORT, 1, comp, &ifds);	/* Kompression */

	switch (format)
	{
	case BILEVEL:
		write_IFDEntry(ziel, &dcount, 0x106, T_SHORT, 1, 0, &ifds);	/* min is white */
		break;
	case GREYSCALE:
		write_IFDEntry(ziel, &dcount, 0x106, T_SHORT, 1, 1, &ifds);	/* min is black */
		break;
	case PALETTE:
		write_IFDEntry(ziel, &dcount, 0x106, T_SHORT, 1, 3, &ifds);	/* palette color image */
		break;
	case FULLCOLOR:
		write_IFDEntry(ziel, &dcount, 0x106, T_SHORT, 1, 2, &ifds);	/* RGB full color image */
		break;
	}

	RowsPerStrip = (unsigned short) (32768L / w);	/* 32 KB pro Streifen durch Bytes pro Zeile */
	if (RowsPerStrip > smurf_pic->pic_height)
		RowsPerStrip = smurf_pic->pic_height;
	StripsPerPic = (smurf_pic->pic_height + RowsPerStrip - 1) / RowsPerStrip;	/* mit Rundung */

	if (StripsPerPic > 1)
		StripOffsets = headsize - StripsPerPic * 8L - bps - pallen;
	else
		StripOffsets = headsize;
	pStripOffsets = (uint32_t *) (ziel + StripOffsets);
	write_IFDEntry(ziel, &dcount, 0x111, T_LONG, StripsPerPic, StripOffsets, &ifds);	/* StripOffsets */

	if (BitsPerPixel == 24)
		write_IFDEntry(ziel, &dcount, 0x115, T_SHORT, 1, 3, &ifds);	/* SamplesPerPixel */
	write_IFDEntry(ziel, &dcount, 0x116, T_SHORT, 1, RowsPerStrip, &ifds);	/* RowsPerStrip */

	if (StripsPerPic > 1)
		StripByteCounts = headsize - StripsPerPic * 4L - bps - pallen;
	else
		StripByteCounts = dcount + 8;
	pStripByteCounts = (unsigned long *) (ziel + StripByteCounts);	/* Adresse merken */
	write_IFDEntry(ziel, &dcount, 0x117, T_LONG, StripsPerPic, StripByteCounts, &ifds);	/* StripByteCounts */

	if (StripsPerPic == 1)
		help = headsize - 16 - bps - pallen;
	else
		help = headsize - 16 - StripsPerPic * 8L - bps - pallen;
	write_IFDEntry(ziel, &dcount, 0x11a, T_RATIONAL, 1, help, &ifds);	/* x-resolution */
	if (headwrite)
	{
		*(uint32_t *) (ziel + help) = longborder(0x48);	/* 72 */
		*(uint32_t *) (ziel + help + 4) = longborder(0x01);	/* durch 1 */
	}
	write_IFDEntry(ziel, &dcount, 0x11b, T_RATIONAL, 1, help + 8, &ifds);	/* y-resolution */
	if (headwrite)
	{
		*(uint32_t *) (ziel + help + 8) = longborder(0x48);	/* 72 */
		*(uint32_t *) (ziel + help + 12) = longborder(0x01);	/* durch 1 */
	}
	write_IFDEntry(ziel, &dcount, 0x128, T_SHORT, 1, 2, &ifds);	/* dpi */

	/* öbertragen der Palette */
	if (BitsPerPixel > 1 && BitsPerPixel <= 8 && smurf_pic->col_format != GREY)
	{
		PalOffset = headsize - pallen;
		write_IFDEntry(ziel, &dcount, 0x140, T_SHORT, 3 * cols, PalOffset, &ifds);	/* ColorMap */

		if (headwrite)					/* Nicht bei Pass 1 ausfÅhren */
		{
			pal = smurf_pic->palette;
			ppal = (uint16_t *) (ziel + PalOffset);

			for (i = 0; i < cols; i++)
			{
				*ppal = *pal++ << 8;
				*(ppal + cols) = *pal++ << 8;
				*(ppal++ + cols + cols) = *pal++ << 8;
			}
		}
	}

	if (headwrite)						/* nicht bei Pass 1 ausfÅhren */
		*(uint32_t *) (ziel + dcount) = longborder(0x0);	/* Offset of next IFD */

	if (headwrite)						/* nicht bei Pass 1 ausfÅhren */
		*(uint16_t *) (ziel + 28) = intborder(ifds);	/* Number of entrys in this IFD */

	if (StripsPerPic == 1)
		return dcount + 4 + bps + 16 + pallen;
	else
		return dcount + 4 + bps + 16 + StripsPerPic * 8L + pallen;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			Taged Image File Format (TIFF)			*/
/*		1, 4, 8, 24 Bit, 							*/
/*		unkomprimiert, RLE, Huffman und LZW			*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL * smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *actziel;
	uint8_t *zielbuf;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t BitsPerPixel;
	uint8_t format;
	unsigned short width, height, heightinv, headsize;
	WORD t;
	unsigned long f_len;
	unsigned long w;
	unsigned long ws;
	unsigned long datalen;

	static WINDOW window;
	static OBJECT *win_form;
	static CONFIG config;


	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* falls Åbergeben, Konfig Åbernehmen */
		if (*((void **) &smurf_struct->event_par[0]) != 0)
		{
			memcpy(&config, *((void **) &smurf_struct->event_par[0]), sizeof(config));
		} else
		{
			config.comp = KEINE;
			config.byteorder = MOTO;
		}

		win_form = rs_trindex[TIFF_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MMORE:
		/* Ressource aktualisieren */
		win_form[KEINE].ob_state &= ~OS_SELECTED;
		win_form[RLE].ob_state &= ~OS_SELECTED;
		win_form[HUFFMAN].ob_state &= ~OS_SELECTED;
		win_form[LZW].ob_state &= ~OS_SELECTED;
		win_form[config.comp].ob_state |= OS_SELECTED;

		win_form[MOTO].ob_state &= ~OS_SELECTED;
		win_form[INTEL].ob_state &= ~OS_SELECTED;
		win_form[config.byteorder].ob_state |= OS_SELECTED;

		window.whandlem = 0;			/* evtl. Handle lîschen */
		window.module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X-...        */
		window.wy = -1;					/* ...und Y-Pos         */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterhîhe          */
		strcpy(window.wtitle, rs_frstr[WINDOW_TITLE]);		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* nÑchstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		if (smurf_struct->services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */

		break;

		/* Closer geklickt, Default wieder her */
	case MMORECANC:
		/* falls Åbergeben, Konfig Åbernehmen */
		if (*((void **) &smurf_struct->event_par[0]) != 0)
		{
			memcpy(&config, *((void ***) &smurf_struct->event_par[0]), sizeof(config));
		} else
		{
			config.comp = KEINE;
			config.byteorder = MOTO;
		}
		smurf_struct->module_mode = M_WAITING;
		break;

		/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case KEINE:
		case RLE:
		case HUFFMAN:
		case LZW:
			config.comp = smurf_struct->event_par[0];
			smurf_struct->module_mode = M_WAITING;
			break;
		case MOTO:
		case INTEL:
			config.byteorder = smurf_struct->event_par[0];
			smurf_struct->module_mode = M_WAITING;
			break;
		case OK:
			/* Konfig Åbergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short) sizeof(config);
			smurf_struct->module_mode = M_MOREOK;
			break;
		case SAVE:
			/* Konfig Åbergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);
			smurf_struct->module_mode = M_CONFSAVE;
			break;
		}
		break;

		/* Keyboardevent */
	case MKEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			/* Konfig Åbergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short)sizeof(config);
			smurf_struct->module_mode = M_MOREOK;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		if (smurf_struct->smurf_pic->depth == 8 && smurf_struct->smurf_pic->col_format == GREY)
			smurf_struct->event_par[0] = GREY;
		else
			smurf_struct->event_par[0] = RGB;
		smurf_struct->module_mode = M_COLSYS;
		break;

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
#if !PRG
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;
#endif

		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (config.byteorder == MOTO)
			Intel = 0;
		else
			Intel = 1;

		switch (BitsPerPixel)
		{
		case 1:
			ws = w = (width + 7) / 8;
			break;
		case 4:
			w = (width + 1) / 2;
			ws = (width + 7) / 8;
			break;
		case 8:
			ws = w = width;
			break;
		case 24:
			ws = w = width * 3L;
			break;
		}

		if (BitsPerPixel == 1)
			format = BILEVEL;
		if (BitsPerPixel >= 2 && BitsPerPixel <= 8 && smurf_struct->smurf_pic->col_format != GREY)
			format = PALETTE;
		if (BitsPerPixel == 8 && smurf_struct->smurf_pic->col_format == GREY)
			format = GREYSCALE;
		if (BitsPerPixel == 24)
			format = FULLCOLOR;

		headwrite = 0;
		headsize = write_header(NULL, smurf_struct->smurf_pic, &config, format, 0, w);

		f_len = w * (long) height;
		if (config.comp == RLE)
			f_len += f_len * 10 / 100;	/* plus 10% Sicherheitsreserve */
		else if (config.comp == LZW && BitsPerPixel > 1)
			f_len = f_len;				/* sparen geht nicht, dafÅr ist die Effizienz nicht hoch genug */

		if ((ziel = (char *) SMalloc(headsize + f_len)) == 0)
		{
			SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;
			memset(ziel, 0, headsize + f_len);

			if (config.comp != KEINE)
			{
				if ((zielbuf = (uint8_t *) Malloc(32770L)) == 0)	/* Zwischenpuffer */
				{
					SMfree(exp_pic);
					smurf_struct->module_mode = M_MEMORY;
					return NULL;
				}

				actziel = zielbuf;
			}

			headwrite = 1;
			f_len = write_header(ziel, smurf_struct->smurf_pic, &config, format, headsize, w);
			ziel += f_len;

			heightinv = height;
			do
			{
				*pStripOffsets++ = longborder(f_len);

				if (heightinv < RowsPerStrip)
					RowsPerStrip = heightinv;

				if (config.comp == KEINE)	/* wieder auf aktuelles ziel synchronisieren */
					actziel = ziel;

				switch (format)
				{
				case BILEVEL:
					datalen = writeTIFFdata_bi(actziel, buffer, w, RowsPerStrip);
					break;
				case GREYSCALE:
					datalen = writeTIFFdata_grey(actziel, buffer, w, RowsPerStrip);
					break;
				case PALETTE:
					if (BitsPerPixel == 4)
						datalen = writeTIFFdata_pal4(actziel, buffer, width, height, RowsPerStrip);
					else
						datalen = writeTIFFdata_pal8(actziel, buffer, w, RowsPerStrip);
					break;
				case FULLCOLOR:
					datalen = writeTIFFdata_rgb(actziel, buffer, width, RowsPerStrip);
					break;
				}

				if (config.comp == RLE)
					datalen = encode_RLE(ziel, zielbuf, w, RowsPerStrip);
				else if (config.comp == LZW)
					datalen = encode_LZW(ziel, zielbuf, w, RowsPerStrip, smurf_struct);

				heightinv -= RowsPerStrip;

				f_len += datalen;
				ziel += datalen;

				buffer += ws * RowsPerStrip;
				*pStripByteCounts++ = longborder(datalen);
			} while (--StripsPerPic);

			ziel = oziel;

			_Mshrink(ziel, f_len);

			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
		}								/* Malloc */

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif
		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		/* exp_pic wird von smurf freigegeben */
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;
		break;
	}

	return NULL;
}


#if PRG
int main(void)
{
	int filehandle;
	GARGAMEL sm_struct;
	SMURF_PIC pic;
	char *file;
	long len;
	EXPORT_PIC *exp_pic;

	filehandle = (int)Fopen("K:\\AMBER.RAW", FO_READ);
	if (filehandle < 0)
		return 1;

	len = Fseek(0L, filehandle, 2);
	Fseek(0L, filehandle, 0L);

	file = Malloc(len);

	Fread(filehandle, len, file);
	Fclose(filehandle);

	pic.pic_width = 320;
	pic.pic_height = 200;
	pic.depth = 8;
	sm_struct.smurf_pic = &pic;

	sm_struct.module_mode = MSTART;
	exp_pic = exp_module_main(&sm_struct);
	pic.pic_data = file + 768;
	pic.palette = (char *) malloc(1024);
	memcpy(pic.palette, file, 768);
	sm_struct.module_mode = MEXEC;
	exp_pic = exp_module_main(&sm_struct);

	Mfree(file);

	filehandle = (int) Fcreate("K:\\AMBER_.TIF", 0);
	Fwrite(filehandle, exp_pic->f_len, exp_pic->pic_data);
	Fclose(filehandle);

	return 0;
}


static void *SMalloc(long amount)
{
	void *buffer;

	buffer = (void *) Malloc(amount);
	if (buffer)
		memset(buffer, 0, amount);

	return buffer;
}


/* --- Funktion zum Freigeben von Speicher + Kontrolle ------- */
static void SMfree(void *ptr)
{
	if (ptr == NULL)
		return;

	Mfree(ptr);
}
#endif
