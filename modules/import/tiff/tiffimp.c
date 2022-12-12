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
/*							TIFF-Importer					*/
/* Version 1.0  --  25.01.97								*/
/*	  xxx													*/
/* Version 1.1  --  15.03.97 - 26.04.98						*/
/*	  Dutzende von Bugfixes und Sicherheitsabfragen ein-	*/
/*	  gebaut. Besonders LZW-Decoding verbessert.			*/
/*	  Die TIC-Erkennung funktioniert nun auch endlich.		*/
/* Version 1.2  --  11.05.98								*/
/*	  min-is-black und min-is-white wurde zwar beim			*/
/*	  Erstellen der Farbpalette beachtet, jedoch wurde das	*/
/*	  Bild (1 Bit) nicht invertiert wenn min-is-black kam.	*/
/* Version 1.3  --  13.05.98 - 17.05.98						*/
/*	  LZW-Dekoder etwas geÑndert, damit auch Bilder von En-	*/
/*	  codern gelesen werden kînnen, die nicht schon bei		*/
/*	  ftable >= LCC - 1 auf die nÑchste Bittiefe gehen,		*/
/*	  sondern erst bei ftable >= LCC.						*/
/* Version 1.4  --  6.11.99									*/
/*	  Fehler, daû LZW-gepackte TIFF im Standardformat		*/
/*	  Probleme machen wenn sie nicht aligned sind, behoben.	*/
/* Version 1.5  --  2.11.2000								*/
/*	  Fehler daû fÅr 32 Bit-Bilder weder im LZW- noch im	*/
/*	  Packbitsdecoder die richtige LÑnge berechnet wurde	*/
/*	  und daû schluûendlich nur ein leerer 24 Bit Speicher-	*/
/*	  block an Smurf Åbergeben wurde behoben. Auûerdem		*/
/*	  eine 32 Bit-Variante des Difference-Decoders hinzuge-	*/
/*	  fÅgt.													*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define DIRENTRYS     "[1][ Getting a ZERO | as IFD-counter (?)][ Crash! ]"
#define DATAARRAY     "[ Can't allocate | RAM for DataRows][Sorry!]"
#define WIDTH         "[1][ * Missing:|'Width/Height-Tag'][ Stop! ]"
#define PLANARCONFIG  "[1][ * Missing:|'Configuration-Tags'][ Stop! ]"
#define PHOTO4        "[1][ Photometric-Set | 'Transparency mask' | isn't supported! ][ Oh-No! ]"
#define PHOTO6        "[1][ Photometric-Set | 'YCbCr-Colorsystem' | isn't supported! ][ Oh-No! ]"
#define PHOTO8        "[1][ Photometric-Set | 'CIE LAB-Colorsystem' | isn't supported! ][ Oh-No! ]"
#define PICTURE_DEPTH "[1][ Can't compute | Picture-Depth ][ Oh-No! ]"
#define UNKNOWN_ALG   "[1][Unknown | compression algorithm][ What? ]"

#define ERROR1        "[1][ Fehler beim Entpacken ][ Abbruch ]"
#define ERROR2        "[1][ Bildstruktur defekt ][ Abbruch ]"

#elif COUNTRY==0

#define DIRENTRYS     "[1][ Getting a ZERO | as IFD-counter (?)][ Crash! ]"
#define DATAARRAY     "[ Can't allocate | RAM for DataRows][Sorry!]"
#define WIDTH         "[1][ * Missing:|'Width/Height-Tag'][ Stop! ]"
#define PLANARCONFIG  "[1][ * Missing:|'Configuration-Tags'][ Stop! ]"
#define PHOTO4        "[1][ Photometric-Set | 'Transparency mask' | isn't supported! ][ Oh-No! ]"
#define PHOTO6        "[1][ Photometric-Set | 'YCbCr-Colorsystem' | isn't supported! ][ Oh-No! ]"
#define PHOTO8        "[1][ Photometric-Set | 'CIE LAB-Colorsystem' | isn't supported! ][ Oh-No! ]"
#define PICTURE_DEPTH "[1][ Can't compute | Picture-Depth ][ Oh-No! ]"
#define UNKNOWN_ALG   "[1][Unknown | compression algorithm][ What? ]"

#define ERROR1        "[1][ Error while decoding ][ Abort ]"
#define ERROR2        "[1][ Bildstruktur defekt ][ Abort ]"

#elif COUNTRY==2

#define DIRENTRYS     "[1][ Getting a ZERO | as IFD-counter (?)][ Crash! ]"
#define DATAARRAY     "[ Can't allocate | RAM for DataRows][Sorry!]"
#define WIDTH         "[1][ * Missing:|'Width/Height-Tag'][ Stop! ]"
#define PLANARCONFIG  "[1][ * Missing:|'Configuration-Tags'][ Stop! ]"
#define PHOTO4        "[1][ Photometric-Set | 'Transparency mask' | isn't supported! ][ Oh-No! ]"
#define PHOTO6        "[1][ Photometric-Set | 'YCbCr-Colorsystem' | isn't supported! ][ Oh-No! ]"
#define PHOTO8        "[1][ Photometric-Set | 'CIE LAB-Colorsystem' | isn't supported! ][ Oh-No! ]"
#define PICTURE_DEPTH "[1][ Can't compute | Picture-Depth ][ Oh-No! ]"
#define UNKNOWN_ALG   "[1][Unknown | compression algorithm][ What? ]"

#define ERROR1        "[1][ Error while decoding ][ Abort ]"
#define ERROR2        "[1][ Bildstruktur defekt ][ Abort ]"

#else
#error "Keine Sprache!"

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"
#include "tiffimp.h"

void *(*SMalloc)(long amount);
void (*SMfree)(void *ptr);


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"TIFF-Importer",
	0x0150,
	"Dale Russell, Christian Eyrich",
	{ "TIF", "TIFF", "TIC", "TIH", "TIM", "TIP", "", "", "", "" },
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




static short check_tiff(uint8_t *buffer)
{
	short v1, v2, format;
	unsigned short magic;

	/* Motorola oder Intel ? */

	v1 = buffer[0];
	v2 = buffer[1];

	if (v1 == 0x49 && v2 == 0x49)
		format = INTEL;
	else if (v1 == 0x4d && v2 == 0x4d)
		format = MOTOROLA;
	else
		format = -1;

	/* Header checken (Magic = 0x002a) */
	magic = *(uint16_t *) (buffer + 2);
	if (format != -1 && (magic == 0x2a00 || magic == 0x002a))
		return format;
	else
		return -1;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			Taged Image File Format (TIF)			*/
/*		1, 2, 4, 8, 24, 32 Bit,						*/
/*		unkomprimiert, Pixelpacked (Byte & Word),	*/
/*		LZW, 32773-Packbits, CCITT3-Huffman,		*/
/*		Min-is-black/whit, RGB, CMY, CMYK			*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *Palette24;
	uint8_t *NewBuf;
	char *help;
	uint8_t invertTIC;
	short format;
	short cnt, shl1, shl2, shl3, shl4, ahl1, ahl2;

#if DEBUG
	short is_WORD;
#endif
	short is_LONG, Depth, meth, maxR, W1, W2;
	short DirEntrys, TagId, TagFormat;
	short PaletteColors = 0;
	short errcode, Difference;
	long HeaderOffset;
	long MapOffset;
	long DataOffset;
	long TagOff;
	long Compression;
	long TagCount;
	long PlanarConfig;
	long BitsPerSample;
	long Photo = 0;
	long SamplePerPixel;
	long Width;
	long Height;
	long BakOffset;
	long RowsPerStrip;
	long tc;
	long *DataArray;
	long DataCount;
	long FillOrder;
	long NewLength;

	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	/* Header checken */
	buffer = smurf_struct->smurf_pic->pic_data;
	if ((format = check_tiff(buffer)) < 0)
		return M_INVALID;

	/* Header Check */
	help = strrchr(smurf_struct->smurf_pic->filename, '.');
	if (help && stricmp(help + 1, "TIC") == 0)
		invertTIC = 1;
	else
		invertTIC = 0;

	/* Defaultwerte einsetzen */
	if (format == INTEL)
	{
		shl1 = 24;
		shl2 = 16;
		shl3 = 8;
		shl4 = 0;
		ahl1 = 8;
		ahl2 = 0;
		W1 = 8;
		W2 = 9;
	} else
	{
		shl1 = 0;
		shl2 = 8;
		shl3 = 16;
		shl4 = 24;
		ahl1 = 0;
		ahl2 = 8;
		W1 = 10;
		W2 = 11;
	}

	DataArray = NULL;
	DataOffset = MapOffset = RowsPerStrip = 0;
	Width = Height = BitsPerSample = SamplePerPixel = PlanarConfig = 0;
	Difference = 1;
	DataCount = 1;
	FillOrder = 1;
	Compression = 0;

	/************************************************************/
	/*	Tag-Identification v0.1									*/
	/************************************************************/
	if ((Palette24 = SMalloc(1024L)) == NULL)
		return M_MEMORY;
	memset(Palette24, 0x0, 1024L);

	HeaderOffset = (long) buffer[4] << shl4;
	HeaderOffset += (long) buffer[5] << shl3;
	HeaderOffset += (long) buffer[6] << shl2;
	HeaderOffset += (long) buffer[7] << shl1;
	DirEntrys = buffer[HeaderOffset] << ahl2;
	DirEntrys += buffer[HeaderOffset + 1] << ahl1;

	if (DirEntrys == 0)
	{
		form_alert(1, DIRENTRYS);
		SMfree(Palette24);
		return M_PICERR;
	}

	TagOff = HeaderOffset + 2;
	for (cnt = 0; cnt < DirEntrys; cnt++)
	{
#if DEBUG
		is_WORD = FALSE;
#endif
		is_LONG = FALSE;
		TagId = buffer[TagOff + 0] << ahl2;
		TagId += buffer[TagOff + 1] << ahl1;
		TagFormat = buffer[TagOff + 2] << ahl2;
		TagFormat += buffer[TagOff + 3] << ahl1;

		switch (TagFormat)
		{
		case 1:
			break;
		case 3:
#if DEBUG
			is_WORD = TRUE;
#endif
			break;
		case 4:
			is_LONG = TRUE;
			break;
		default:
#if DEBUG
			printf("\n Tag-Format is unknown (%i)", TagFormat);
#endif
			break;
		}

		TagCount = (long) buffer[TagOff + 4] << shl4;
		TagCount += (long) buffer[TagOff + 5] << shl3;
		TagCount += (long) buffer[TagOff + 6] << shl2;
		TagCount += (long) buffer[TagOff + 7] << shl1;

		switch (TagId)
		{
		case 0x0100:
			if (is_LONG)				/*  Width */
			{
				Width = (long) buffer[TagOff + 8] << shl4;
				Width += (long) buffer[TagOff + 9] << shl3;
				Width += (long) buffer[TagOff + 10] << shl2;
				Width += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				Width = (long) buffer[TagOff + 8] << ahl2;
				Width += (long) buffer[TagOff + 9] << ahl1;
			}
			break;

		case 0x0101:
			if (is_LONG)				/*  Height */
			{
				Height = (long) buffer[TagOff + 8] << shl4;
				Height += (long) buffer[TagOff + 9] << shl3;
				Height += (long) buffer[TagOff + 10] << shl2;
				Height += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				Height = (long) buffer[TagOff + 8] << ahl2;
				Height += (long) buffer[TagOff + 9] << ahl1;
			}
			break;

		case 0x0102:
			if (TagCount == 1)
			{
				BakOffset = TagOff + 8;
			} else
			{
				BakOffset = (long) buffer[TagOff + 8] << shl4;
				BakOffset += (long) buffer[TagOff + 9] << shl3;
				BakOffset += (long) buffer[TagOff + 10] << shl2;
				BakOffset += (long) buffer[TagOff + 11] << shl1;
			}

			if (is_LONG)				/* BitsPerSample */
			{
				BitsPerSample = (long) buffer[BakOffset] << shl4;
				BitsPerSample += (long) buffer[BakOffset + 1] << shl3;
				BitsPerSample += (long) buffer[BakOffset + 2] << shl2;
				BitsPerSample += (long) buffer[BakOffset + 3] << shl1;
			} else
			{
				BitsPerSample = (long) buffer[BakOffset] << ahl2;
				BitsPerSample += (long) buffer[BakOffset + 1] << ahl1;
			}
			break;

		case 0x0111:
			if (is_LONG)				/* Strip Offsets */
			{
				DataOffset = (long) buffer[TagOff + 8] << shl4;
				DataOffset += (long) buffer[TagOff + 9] << shl3;
				DataOffset += (long) buffer[TagOff + 10] << shl2;
				DataOffset += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				DataOffset = (long) buffer[TagOff + W1] << ahl2;
				DataOffset += (long) buffer[TagOff + W2] << ahl1;
			}

			if (TagCount > 1)
			{
				BakOffset = DataOffset;
				DataArray = SMalloc(TagCount * 4L);
				if (DataArray == NULL)
				{
					form_alert(1, DATAARRAY);
					SMfree(Palette24);
					return M_PICERR;
				}

				DataCount = TagCount;

				for (tc = 0; tc < TagCount; tc++)
				{
					if (is_LONG)
					{
						DataOffset = (long) buffer[BakOffset + 0] << shl4;
						DataOffset += (long) buffer[BakOffset + 1] << shl3;
						DataOffset += (long) buffer[BakOffset + 2] << shl2;
						DataOffset += (long) buffer[BakOffset + 3] << shl1;
						BakOffset += 4;
					} else
					{
						DataOffset = (long) buffer[BakOffset + 0] << ahl2;
						DataOffset += (long) buffer[BakOffset + 1] << ahl1;
						BakOffset += 2;
					}

					DataArray[tc] = DataOffset;
				}
			}
			break;

		case 0x013d:
			Difference = buffer[TagOff + 8] << ahl2;
			Difference += buffer[TagOff + 9] << ahl1;
			break;

		case 0x0117:
			break;						/* Strip Counts, not necessary for decoding... */

		case 0x0106:
			if (is_LONG)				/* Photometric Interpretation */
			{
				Photo = (long) buffer[TagOff + 8] << shl4;
				Photo += (long) buffer[TagOff + 9] << shl3;
				Photo += (long) buffer[TagOff + 10] << shl2;
				Photo += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				Photo = (long) buffer[TagOff + 8] << ahl2;
				Photo += (long) buffer[TagOff + 9] << ahl1;
			}
			break;
		case 0x0140:					/* Colormap Offset */
#if DEBUG
			printf("\nColormap-Tag found (Counter:%li) Word=%i Long=%i", TagCount, is_WORD, is_LONG);
#endif
			PaletteColors = TagCount / 3;
			MapOffset = (long) buffer[TagOff + 8] << shl4;
			MapOffset += (long) buffer[TagOff + 9] << shl3;
			MapOffset += (long) buffer[TagOff + 10] << shl2;
			MapOffset += (long) buffer[TagOff + 11] << shl1;
			break;

		case 0x0152:
#if 0
			ExtraSamples = buffer[TagOff + 8];
#endif
			break;

		case 0x0115:
			if (is_LONG)				/* SamplePerPixel */
			{
				SamplePerPixel = (long) buffer[TagOff + 8] << shl4;
				SamplePerPixel += (long) buffer[TagOff + 9] << shl3;
				SamplePerPixel += (long) buffer[TagOff + 10] << shl2;
				SamplePerPixel += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				SamplePerPixel = (long) buffer[TagOff + 8] << ahl2;
				SamplePerPixel += (long) buffer[TagOff + 9] << ahl1;
			}
			break;

		case 0x011c:					/* Plane Configuration */
			PlanarConfig = (long) buffer[TagOff + 8] << ahl2;
			PlanarConfig += (long) buffer[TagOff + 9] << ahl1;
			break;

		case 0x010a:					/* Fill Order */
			FillOrder = (long) buffer[TagOff + 8] << ahl2;
			FillOrder += (long) buffer[TagOff + 9] << ahl1;
			break;

		case 0x0103:
			if (is_LONG)				/* Compression */
			{
				Compression = (long) buffer[TagOff + 8] << shl4;
				Compression += (long) buffer[TagOff + 9] << shl3;
				Compression += (long) buffer[TagOff + 10] << shl2;
				Compression += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				Compression = (long) buffer[TagOff + 8] << ahl2;
				Compression += (long) buffer[TagOff + 9] << ahl1;
			}
			break;

		case 0x0116:
			if (is_LONG)				/* Rows per Strip */
			{
				RowsPerStrip = (long) buffer[TagOff + 8] << shl4;
				RowsPerStrip += (long) buffer[TagOff + 9] << shl3;
				RowsPerStrip += (long) buffer[TagOff + 10] << shl2;
				RowsPerStrip += (long) buffer[TagOff + 11] << shl1;
			} else
			{
				RowsPerStrip = (long) buffer[TagOff + 8] << ahl2;
				RowsPerStrip += (long) buffer[TagOff + 9] << ahl1;
			}
			break;

		default:
#if DEBUG
			printf("\n	Unknown or unsupported tag #%i ($%X)", TagId, TagId);
#endif
			break;
		}
		TagOff += 12;					/* Next Tag, please */
	}

#if DEBUG
	printf("\n\n	PHOTOMETRIC:%li", Photo);
	printf("\n	DataOffset:%li", DataOffset);
	printf("\n	MapOffset:%li", MapOffset);
	printf("\n	SamplePerPixel:%li", SamplePerPixel);
	printf("\n	BitsPerSample:%li", BitsPerSample);
	printf("\n	Width in Pixel:%li", Width);
	printf("\n	Height in Pixel:%li", Height);
	printf("\n	Planar Config:%li", PlanarConfig);
	printf("\n	Differencing Predictor:%i", Difference);
	printf("\n	Kompression:%li", Compression);
	printf("\n	RowsPerStrip:%li", RowsPerStrip);
	printf("\n	Fill-Order:%li", FillOrder);
	(void)Cnecin();
#endif

	if (Width == 0 || Height == 0)
	{
		form_alert(1, WIDTH);
		SMfree(Palette24);
		return M_PICERR;
	}

	if (PlanarConfig == 0 && BitsPerSample == 0 && SamplePerPixel == 0)
	{
		form_alert(1, PLANARCONFIG);
		SMfree(Palette24);
		return M_PICERR;
	}

	/****************************************************************/
	/*																*/
	/*		   		Welches TIFF hÑttens denn gern?					*/
	/*																*/
	/****************************************************************/

	if (Photo == 4 || Photo == 6)
	{
		if (Photo == 4)
			form_alert(1, PHOTO4);
		else if (Photo == 6)
			form_alert(1, PHOTO6);
		else if (Photo == 8)
			form_alert(1, PHOTO8);
		SMfree(Palette24);
		SMfree(DataArray);
		return M_PALERR;
	}

	if (DataCount > 1 || DataArray != NULL)
	{
		DataOffset = DataArray[0];
	} else if (DataOffset != 0)
	{
		DataArray = SMalloc(4L);
		DataArray[0] = DataOffset;
	}

	if (Compression <= 0)
		Compression = 1;

	if (PlanarConfig <= 0)
		PlanarConfig = 1;

	if (BitsPerSample <= 0)
		BitsPerSample = 0;
	else if (BitsPerSample > 8)
		BitsPerSample = 8;

	if (SamplePerPixel <= 0)
		SamplePerPixel = 1;
	else if (SamplePerPixel > 4)
		SamplePerPixel = 4;

	if (RowsPerStrip == 0 || RowsPerStrip > Height)
		RowsPerStrip = Height;

	if (SamplePerPixel == 4 && BitsPerSample == 0 && Photo == 5)	/* 32 Bit CMYK-TIFs */
		BitsPerSample = 8;
	if (SamplePerPixel == 3 && BitsPerSample == 0)	/* 24 Bit TIFs */
		BitsPerSample = 8;

	Depth = BitsPerSample * SamplePerPixel;

	if (Depth > 32)
	{
		if (SamplePerPixel == 3 || Photo == 2)
			Depth = 24;
		else if (SamplePerPixel == 1 || Photo == 3 || Photo == 0 || Photo == 1)
			Depth = 8;
	}

	if (Depth == 0)
	{
		if (SamplePerPixel == 0 || SamplePerPixel == 1)
		{
			switch ((int) BitsPerSample)
			{
			case 8:
				Depth = 256;
				break;
			case 4:
				Depth = 16;
				break;
			case 2:
				Depth = 4;
				break;
			case 1:
				Depth = 2;
				break;
			default:
				break;
			}
		} else if (SamplePerPixel == 3)
		{
			Depth = 24;
		}
		if (Depth == 0)
		{
			form_alert(1, PICTURE_DEPTH);
			SMfree(Palette24);
			SMfree(DataArray);
			return M_PICERR;
		}
	}

	if (Photo == 0 || Photo == 1 || Photo == 3)
	{
		switch (Depth)
		{
		case 8:
			maxR = 256;
			break;
		case 4:
			maxR = 16;
			break;
		case 2:
			maxR = 4;
			break;
		case 1:
			maxR = 2;
			break;
		default:
			maxR = 256;
			break;
		}

		if (PaletteColors < 1 || PaletteColors > maxR)
			PaletteColors = maxR;
	}

	if (PaletteColors < 1 || PaletteColors > 256)
		PaletteColors = 1 << Depth;

	if (Photo == 0 || Photo == 1 || Photo == 3)
	{
		if (Depth == 1)
		{
			if (!MapOffset)
			{
				Palette24[0] = 255;
				Palette24[1] = 255;
				Palette24[2] = 255;
				Palette24[3] = 0;
				Palette24[4] = 0;
				Palette24[5] = 0;
			}
		} else
		{
			if (Photo == 3)
			{
				if (MapOffset == 0)
					tiff24_min_is_black(Palette24, Depth);
				else
					tiff24_colormap((uint16_t *) (buffer + MapOffset), Palette24, (long) PaletteColors, format);
			} else if (Photo == 0)
			{
				tiff24_min_is_white(Palette24, Depth);
			} else if (Photo == 1)
			{
				tiff24_min_is_black(Palette24, Depth);
			}
		}
	}

	switch ((unsigned int) Compression)
	{
	case 0x01:
		meth = 0;						/* No compression or pixelpacked: is_BYTE */
		break;
	case 0x02:
		meth = 1;						/* CCITT Group 3: 1-dimensional Huffman */
		break;
	case 0x03:
		meth = 2;						/* CCITT Group 3/T.4: Facsimile 1/2-dimensional Huffmann */
		break;
	case 0x04:
		meth = 3;						/* CCITT Group 4/T.6: S/W Compression */
		break;
	case 0x05:
		meth = 4;						/* Lempel, Ziv and Welch (LZW) Compression */
		break;
	case 0x06:
		meth = 5;						/* Joint Photographic Experts Group (JPeG) Compression */
		break;
	case 32773U:
		meth = 6;						/* RLE: 32773 Packbits Compression */
		break;
	case 32771U:
		meth = 0;						/* No compression or pixelpacked: is_WORD */
		break;
	default:
		form_alert(1, UNKNOWN_ALG);
		SMfree(Palette24);
		SMfree(DataArray);
		return M_UNKNOWN_TYPE;
	}

#if DEBUG
	printf("\n\nPHOTOMETRIC: %li", Photo);
	printf("\nDataOffset: %li", DataOffset);
	printf("\nMapOffset: %li", MapOffset);
	printf("\nSamplePerPixel: %li", SamplePerPixel);
	printf("\nBitsPerSample: %li", BitsPerSample);
	printf("\nWidth in Pixel: %li", Width);
	printf("\nHeight in Pixel: %li", Height);
	printf("\nPlanar Config: %li", PlanarConfig);
	printf("\nDifferencing Predictor:%i", Difference);
	printf("\nKompression: %li", Compression);
	printf("\nRowsPerStrip: %li", RowsPerStrip);
	printf("\nFill-Order: %li", FillOrder);
	(void)Cnecin();
#endif

	if (meth != 0 && meth != 1 && meth != 4 && meth != 6)
	{
/*		form_alert(1,"[1][Nicht unterstÅtzter|Packalgorithmus!][Mist]"); */
		SMfree(Palette24);
		SMfree(DataArray);
		return M_UNKNOWN_TYPE;
	}


/********************************************************/
/*		Decrunch-Dispatcher								*/
/********************************************************/
	if (Depth >= 8)
	{
		NewLength = Width * Height * Depth / 8 + 1024;
		NewBuf = (uint8_t *) SMalloc(NewLength);
	} else
	{
		NewLength = (Width + 7) / 8 * Height * Depth + 1024;
		NewBuf = (uint8_t *) SMalloc(NewLength);
	}

	if (NewBuf == NULL)
	{
		SMfree(Palette24);
		SMfree(NewBuf);
		SMfree(DataArray);
		return M_MEMORY;
	}

	if (meth > 0)
	{
		errcode = 0;
		switch ((int) meth)
		{
		case 0x01:
			errcode = tiffCCITT3_depack(smurf_struct, buffer + DataOffset, NewBuf, Width, Height, Depth);
			Depth = 8;
			PlanarConfig = 1;
			PaletteColors = 2;
			if ((Depth == 24 && PlanarConfig == 1 && Photo == 2) ||
				(Depth == 8 && PlanarConfig == 1) ||
				(Depth == 1 && PlanarConfig == 1))
				break;
			else
			{
				SMfree(buffer);
				DataOffset = 0;
				buffer = NewBuf;
				NewBuf = SMalloc(NewLength);
				break;
			}
		case 0x04:
			errcode = tiffLZW_depack(smurf_struct, buffer, NewBuf, Width, Height, Depth, DataCount, DataArray, RowsPerStrip, Difference);
			if ((Depth == 24 && PlanarConfig == 1 && Photo == 2) ||
				(Depth == 8 && PlanarConfig == 1) ||
				(Depth == 1 && PlanarConfig == 1))
				break;
			else
			{
				SMfree(buffer);
				DataOffset = 0;
				buffer = NewBuf;
				NewBuf = SMalloc(NewLength);
				break;
			}
		case 0x06:
			errcode = tiff32773_depack(smurf_struct, buffer, NewBuf, Width, Height, Depth, RowsPerStrip, DataArray);
			if ((Depth == 24 && PlanarConfig == 1 && Photo == 2) ||
				(Depth == 8 && PlanarConfig == 1) ||
				(Depth == 1 && PlanarConfig == 1))
				break;
			else
			{
				SMfree(buffer);
				DataOffset = 0;
				buffer = NewBuf;
				NewBuf = SMalloc(NewLength);
				break;
			}
		default:
			break;
		}

		if (errcode < 0)
		{
			form_alert(1, ERROR1);
			SMfree(Palette24);
			SMfree(NewBuf);
			SMfree(DataArray);
			return M_PICERR;
		}
	}

	if (Depth == 24 && PlanarConfig == 1 && Photo == 2)
	{
		/*  ------- 24 Bit, RGB  -------- */
		if (meth != 0x01 && meth != 0x04 && meth != 0x06)
			memcpy(NewBuf, buffer + DataOffset, NewLength);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		smurf_struct->smurf_pic->depth = 24;
		if (invertTIC)
			smurf_struct->smurf_pic->col_format = CMY;
		else
			smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	} else if (Depth == 24 && PlanarConfig == 2 && Photo == 2)
	{
		/*  ------- 24 Bit, RGB-Planes   -------- */
		tiff24Planes_24Normal(smurf_struct, buffer, NewBuf, Width, Height, DataCount, DataArray);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		smurf_struct->smurf_pic->depth = 24;
		if (invertTIC)
			smurf_struct->smurf_pic->col_format = CMY;
		else
			smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	} else if (Depth >= 24 && PlanarConfig == 1 && Photo == 5)
	{
		/*  ------- 32/24 Bit, CMYK  -------- */
		tiff32CMYK_24Normal(smurf_struct, buffer, NewBuf, Width, Height, DataOffset);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		/* solange Smurf nur 24 Bit versteht */
		smurf_struct->smurf_pic->depth = 24;
		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	} else if (Depth == 8 && PlanarConfig == 1)
	{
		/*  ------- 8 Bit, RGB-Palette   -------- */
		if (meth != 0x01 && meth != 0x04 && meth != 0x06)
			memcpy(NewBuf, buffer + DataOffset, NewLength);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		smurf_struct->smurf_pic->depth = 8;
		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
		memcpy(smurf_struct->smurf_pic->palette, Palette24, 1024L);
	} else if (Depth == 4 && PlanarConfig == 1)
	{
		/*  ------- 4 Bit, RGB-Palette   -------- */
		tiff4_to_stf(smurf_struct, buffer + DataOffset, NewBuf, Width, Height);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		smurf_struct->smurf_pic->depth = 4;
		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
		memcpy(smurf_struct->smurf_pic->palette, Palette24, 1024L);
	} else if (Depth == 2 && PlanarConfig == 1)
	{
		/*  ------- 2 Bit, RGB-Palette   -------- */
		tiff2_to_stf(smurf_struct, buffer + DataOffset, NewBuf, Width, Height);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		smurf_struct->smurf_pic->depth = 2;
		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
		memcpy(smurf_struct->smurf_pic->palette, Palette24, 1024L);
	} else if (Depth == 1 && PlanarConfig == 1)
	{
		/*  ------- 1 Bit, RGB-Palette   -------- */
		if (Photo == 1)
			invert_1Bit(NewBuf, NewLength);
		if (meth != 0x01 && meth != 0x04 && meth != 0x06)
			memcpy(NewBuf, buffer + DataOffset, NewLength);
		smurf_struct->smurf_pic->pic_data = NewBuf;
		smurf_struct->smurf_pic->depth = 1;
		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
		memcpy(smurf_struct->smurf_pic->palette, Palette24, 1024L);
	} else
	{
		form_alert(1, ERROR2);
		SMfree(Palette24);
		SMfree(NewBuf);
		SMfree(DataArray);
		return M_PICERR;
	}

	smurf_struct->smurf_pic->pic_width = (short) Width;
	smurf_struct->smurf_pic->pic_height = (short) Height;
	SMfree(buffer);
	SMfree(Palette24);

	return M_PICDONE;
}


#if PRG
/*-------------- Main-Function fÅr TIFF-Programm zum Debuggen */


static void *smalloc(long amount)
{
	void *buffer;

	buffer = (void *) Malloc(amount);
	memset(buffer, 0, amount);

	return buffer;
}


/* --- Funktion zum Freigeben von Speicher + Kontrolle ------- */
static void smfree(void *ptr)
{
	if (ptr == NULL)
		return;

	Mfree(ptr);
}


int main(void)
{
	int filehandle;
	GARGAMEL sm_struct;
	SMURF_PIC pic;
	char *file;
	long len;

	filehandle = (int)Fopen("ambert.tif", FO_READ);
	if (filehandle < 0)
		return 1;

	len = Fseek(0L, filehandle, 2);
	Fseek(0L, filehandle, 0);

	file = Malloc(len);

	Fread(filehandle, len, file);
	Fclose(filehandle);

	pic.pic_data = file;
	pic.file_len = len;
	sm_struct.smurf_pic = &pic;
	sm_struct.SMalloc = smalloc;
	sm_struct.SMfree = smfree;

	imp_module_main(&sm_struct);

	return 0;
}
#endif
