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

/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
/*  Therapy Graphix Export Modul fr Smurf v1.0                     */
/*  Written by Dale Russell, April->August                          */
/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "trp_gfx.h"
#include "trp_gfx.rsh"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Therapy Grafix (TRP)",
	0x0030,
	"Dale Russell",
	{ "TRP", "", "", "", "", "", "", "", "", "" },
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
	8, 24, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};

/***********************************************************/
/*  Header                                                 */
/***********************************************************/
typedef struct
{
	char magic[8];						/* Erkennung: 'TRP_GFX!' */
	uint8_t type;						/* Typ(Bitfield; Gesetzt: vorhanden;
										   Gel”scht: nicht vorhanden) 
										   Bit 0: Bitmap enthalten
										   Bit 1: Animation enthalten
										   Bit 2: Vektorgraphik enthalten  */
	int32_t bitmap;						/* Offset auf Bildstruktur */
	int32_t animation;					/* Offset auf Animationsstruktur */
	int32_t vector;						/* Offset auf Vektorstruktur */
} trp_header;

/***********************************************************/
/*  Bildbeschreibungsstruktur                              */
/***********************************************************/
typedef struct
{
	int32_t data;						/* Offset auf Bilddaten,
										   relativ zum Dateianfang */
	int32_t width;						/* Bildbreite in Pixel */
	int32_t height;						/* Bildh”he in Pixel */
	uint8_t depth;						/* Farbtiefe in Bits */
	uint8_t planes;						/* Anzahl der Farbebenen */
	uint8_t mode;						/* Kompressionsart */
	uint8_t incoming;					/* current Bitstream(0) oder 
										   interlaced(1) */
	uint8_t coltable;					/* Farbpalette (0=Nein; 1=Ja; 
										   2=Graufstufen linear, 
										   nicht enthalten) */
	uint8_t table_depth;				/* Palettenfarbtiefe */
	int16_t colors;						/* Anzahl der Farben in der Palette */
	int32_t tableoffset;				/* Offset der Palettendaten,
										   relativ zum Dateianfang */
	uint8_t colormodel;					/* Farbmodell: (0=RGB; 1=YCbCr;
										   2=CMYK; 3=YUV; 4=YIQ) */
	char text[64];						/* Text, z.B. zu Beschreibung ect. */
} bitmap_struct;

static WINDOW my_window;
static OBJECT *main_form;
static SLIDER quality_slider;


/************* Header-Struktur fllen ****************/

static void header_fuellen(trp_header *header)
{
	strncpy((header->magic), "TRP_GFX!", 8);	/* MagicCode */
	header->type = 1;					/* Bitmap */
	header->bitmap = sizeof(trp_header);	/* Offset auf Bitmap-Struktur */
	header->animation = 0;				/* Keine Animation */
	header->vector = 0;					/* Keine Vektorgraphik */
}

/************* Bitmap-Struktur fllen ****************/

static void bitmapstruktur_fuellen(trp_header *header, bitmap_struct *bitmap, GARGAMEL *smurf_struct)
{
	if (smurf_struct->smurf_pic->depth == 8)
	{
		bitmap->colors = 256;
		bitmap->data = (header->bitmap) + sizeof(bitmap_struct) + (long) (bitmap->colors) * 3;
		bitmap->tableoffset = (header->bitmap) + sizeof(bitmap_struct);	/* Offset auf Palettendaten */
		bitmap->coltable = 1;			/* Farbpalette ist vorhanden */
		bitmap->table_depth = 24;	/* Palettenfarbtiefe in Bits */
	} else
	{
		bitmap->data = (header->bitmap) + sizeof(bitmap_struct);
		bitmap->tableoffset = 0;		/* Offset auf Palettendaten */
		bitmap->colors = 0;
		bitmap->coltable = 0;			/* Keine Farbpalette vorhanden */
		bitmap->table_depth = 0;		/* Palettenfarbtiefe in Bits */
	}
	bitmap->width = (long) smurf_struct->smurf_pic->pic_width;	/* Bildbreite */
	bitmap->height = (long) smurf_struct->smurf_pic->pic_height;	/* Bildbreite */
	bitmap->depth = smurf_struct->smurf_pic->depth;	/* Farbtiefe */
	bitmap->planes = 1;					/* Farbebenen */
	bitmap->mode = 1;					/* Kodierung: Dynamic RLE */
	bitmap->incoming = 0;				/* current Bitstream */
	bitmap->colormodel = 0;				/* RGB-Farbmodell */
	strcpy(bitmap->text, "This bitmap was saved with Smurf");
}

/******************************************************************/
/*      8Bit "Dynamic-RLE"-Kodierung                              */
/******************************************************************/
static long encoding8Bit_trp(bitmap_struct *bitmap, GARGAMEL *smurf_struct, uint8_t *expdata, int quality)
{
	uint8_t ID_Byte_Run;
	uint8_t *output;
	uint8_t *input;
	uint8_t *cptr;
	uint8_t *scan1;
	uint8_t *scan2;
	int pixel, counter, xgo, same, typ, MaxRun;
	long x, y, gox, goy, length;
	long id_case[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t *pal;
	uint8_t *getpal;
	int w, r1, g1, b1, r2, g2, b2, gquality;

	/* ------------- */
	/* Quantisierung */
	/* ------------- */
	gox = bitmap->width;
	goy = bitmap->height;
	scan1 = smurf_struct->smurf_pic->pic_data;
	scan2 = scan1 + 1;
	pal = smurf_struct->smurf_pic->palette;
	if (quality > 0)
	{
		gquality = quality - 1;
		y = 0;
		while (y++ < goy)
		{
			x = 0;
			while (x++ < gox)
			{
				w = *(scan1);
				getpal = pal + w * 3;
				r1 = *(getpal++);
				g1 = *(getpal++);
				b1 = *(getpal++);
				w = *(scan2);
				getpal = pal + w * 3;
				r2 = *(getpal++);
				g2 = *(getpal++);
				b2 = *(getpal++);

				if (abs(r1 - r2) < quality)
					if (abs(g1 - g2) < gquality)
						if (abs(b1 - b2) < quality)
							*(scan2) = *(scan1);

				scan1++;
				scan2++;
			}
		}
	}

	/* ---------------- */
	/*  Byte Run suchen */
	/* ---------------- */
	scan1 = smurf_struct->smurf_pic->pic_data;
	x = y = 0;
	while (y < goy)
	{
		xgo = TRUE;
		while (xgo == TRUE)
		{
			pixel = *(scan1++);
			cptr = scan1;
			counter = 0x00;
			same = TRUE;
			x++;
			for (;;)
			{
				if (same == FALSE || counter >= 0x7f)
					break;
				counter++;
				if (*(cptr++) != pixel)
					same = FALSE;
			}


			typ = 0;
			if (counter >= 2 && counter < 16)
				typ = 1;
			else if (counter >= 16 && counter < 32)
				typ = 2;
			else if (counter >= 32 && counter < 64)
				typ = 3;
			else if (counter >= 64 && counter < 128)
				typ = 4;
			id_case[typ] += counter;
			scan1 += (counter - 1);
			x += (counter - 1);
			if (x >= gox)
			{
				x -= gox;
				y++;
				if (y >= goy)
					xgo = FALSE;
			}
		}
	}
	x = id_case[1];
	counter = 1;
	for (xgo = 2; xgo <= 4; xgo++)
	{
		if (id_case[xgo] >= x)
		{
			x = id_case[xgo];
			counter = xgo;
		}
	}
	switch (counter)
	{
	case 0x01:
		ID_Byte_Run = 0xf0;
		break;
	case 0x02:
		ID_Byte_Run = 0xe0;
		break;
	case 0x03:
		ID_Byte_Run = 0xc0;
		break;
	case 0x04:
		ID_Byte_Run = 0x80;
		break;
	}

	/* ------------------ */
	/*  Kodierung         */
	/* ------------------ */
	MaxRun = 0xff - ID_Byte_Run;
	input = smurf_struct->smurf_pic->pic_data;
	output = expdata + (bitmap->data);
	*(output++) = ID_Byte_Run;
	length = 1;
	x = y = 0;

	while (y < goy)
	{
		xgo = TRUE;
		while (xgo == TRUE)
		{
			pixel = *(input++);
			cptr = input;
			counter = 0x00;
			same = TRUE;
			x++;

			for (;;)
			{
				if (same == FALSE || counter >= MaxRun)
					break;
				counter++;
				if (*(cptr++) != pixel)
					same = FALSE;
			}

			if (counter <= 0x01)
			{
				if (pixel >= ID_Byte_Run)
				{
					*(output++) = ID_Byte_Run + 1;
					*(output++) = pixel;
					length += 2;
				} else
				{
					*(output++) = pixel;
					length++;
				}
			} else
			{
				*(output++) = counter + ID_Byte_Run;
				*(output++) = pixel;
				x += (counter - 1);
				input += (counter - 1);
				length += 2;
			}

			if (x >= gox)
			{
				x -= gox;
				y++;
				if (y >= goy)
					xgo = FALSE;
			}
		}
	}
	return length;
}


/******************************************************************/
/*      24Bit "Smurfy's Law"-Kodierung                            */
/******************************************************************/
static long encoding24Bit_trp(bitmap_struct *bitmap, GARGAMEL *smurf_struct, uint8_t *expdata, int quality)
{
	long xc, yc;
	long xmax, ymax;
	uint8_t *getdata;
	uint8_t *putdata;
	uint8_t r, g, b;
	long len;

	(void) quality;
	xmax = bitmap->width;
	ymax = bitmap->height;
	getdata = smurf_struct->smurf_pic->pic_data;
	putdata = expdata + (bitmap->data);
	yc = len = 0;

	while (yc++ < xmax)
	{
		xc = 0;
		while (xc++ < ymax)
		{
			r = *(getdata++);
			g = *(getdata++);
			b = *(getdata++);
			*(putdata++) = r;
			*(putdata++) = g;
			*(putdata++) = b;
			len += 3;
		}
	}

	return len;
}


/***********************************************************/
/*  Exporter MAIN                                          */
/***********************************************************/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	trp_header header;
	bitmap_struct bitmap;
	char *expdata;						/* Output Arrays */
	long org_length;
	long datalength;					/* L„nge der Rckgabedaten in Bytes */
	int quality;
	WORD t;

	main_form = rs_trindex[THERAPY_GFX];

	for (t = 0; t < NUM_OBS; t++)
		rsrc_obfix(rs_object, t);

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* WINDOW-Struktur vorbereiten */
		my_window.whandlem = 0;
		my_window.module = smurf_struct->module_number;
		my_window.wnum = 1;
		my_window.wx = -1;
		my_window.wy = -1;
		my_window.ww = -1;
		my_window.wh = -1;
		strcpy(my_window.wtitle, rs_frstr[WINDOW_TITLE]);
		my_window.resource_form = main_form;
		my_window.picture = 0;
		my_window.shaded = 0;
		my_window.editob = 0;
		my_window.nextedit = 0;
		my_window.editx = 0;

		/* SLIDER-Struktur vorbereiten */
		quality_slider.regler = QUAL_SLID;
		quality_slider.schiene = QUAL_FHR;
		quality_slider.rtree = main_form;
		quality_slider.txt_obj = QUAL_TXT;
		quality_slider.min_val = 0;
		quality_slider.max_val = 30;

		smurf_struct->services->f_module_window(&my_window);
		smurf_struct->services->set_slider(&quality_slider, 0);
		smurf_struct->module_mode = M_WAITING;
		return NULL;

	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case QUAL_SLID:
			quality = smurf_struct->services->slider(&quality_slider);
			break;
		case STARTIT:
			smurf_struct->module_mode = MEXEC;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

	case MEXEC:
		org_length = (long) smurf_struct->smurf_pic->pic_width * (long) smurf_struct->smurf_pic->pic_height * (long) (smurf_struct->smurf_pic->depth / 8);
		exp_pic = (EXPORT_PIC *)Malloc(sizeof(EXPORT_PIC));
		expdata = (uint8_t *)Malloc(org_length + 16000l);
		if (exp_pic == NULL || expdata == NULL)
		{
			Mfree(expdata);
			Mfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

		/***************************************************/
		/*  Header in Output Array kopieren                */
		/***************************************************/
		header_fuellen(&header);
		memcpy(expdata, &header, sizeof(header));
		/***************************************************/
		/*  Bildstruktur in Output Array kopieren          */
		/***************************************************/
		bitmapstruktur_fuellen(&header, &bitmap, smurf_struct);
		memcpy(expdata + sizeof(trp_header), &bitmap, sizeof(bitmap_struct));
		/***************************************************/
		/*  Farbpalette kopieren (bitmap->coltable==1)     */
		/***************************************************/
		if ((bitmap.coltable) == 1)
			memcpy(expdata + (bitmap.tableoffset), smurf_struct->smurf_pic->palette, (long) ((bitmap.data) - (bitmap.tableoffset)));
		/***************************************************/
		/*  8Bit D-RLE  Kodierung                          */
		/***************************************************/
		if (bitmap.depth == 8)
		{
			datalength = encoding8Bit_trp(&bitmap, smurf_struct, expdata, quality);
		} else /* if (bitmap.depth == 24) */
		{
			datalength = encoding24Bit_trp(&bitmap, smurf_struct, expdata, quality);
		}
		datalength += bitmap.data;
		/***************************************************/
		/*  smurf-struct fllen                            */
		/***************************************************/
		exp_pic->pic_data = expdata;
		exp_pic->f_len = datalength;
		smurf_struct->module_mode = M_DONEEXIT;

		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

	case MTERM:
		/* exp_pic wird von smurf freigegeben */
		smurf_struct->module_mode = M_EXIT;
		return NULL;
	}

	return NULL;
}
