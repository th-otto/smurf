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
/*						GIF-Format Exporter					*/
/*	Version 0.1 - 04.09.97									*/
/*	  87a und 89a, normal und interlaced. Es werden zwar	*/
/*	  alle Farbtiefen gelesen, aber 1-7 Bit noch in PP ge-	*/
/*	  liefert. Bei mehreren Bildern wird nur das erste ge-	*/
/*	  lesen.												*/
/*	Version 0.2 - 23.09.97									*/
/*	  1-7 Bit Bilder werden nun im Standardformat geliefert.*/
/*	Version 0.3 - 14.11.98									*/
/*	  Verkraftet auch Extension-Blocks mit Sub-Blocks und	*/
/*	  Auftreten von Extension-Blocks bei GIF 87a.			*/
/*	Version 0.35 - 15.1.99									*/
/*	  if(pCount == 0)-Abfrage in decode_lzw_normal() auf	*/
/*	  Stefan Hillers Report Åber AbstÅrze mit DEFEKT.GIF	*/
/*	  hin erweitert. Short-Read-Fehler sollten damit ohne	*/
/*	  schwere Auswirkungen bleiben.	Ist das in 8-Bit-Routine*/
/*	  auch nîtig?											*/
/*	Version 0.36 - 25.10.2000								*/
/*	  Einen (im Graphical Extension Block, hoffentlich der	*/
/*	  einzige) Integerzugriff gegen zwei Bytezugriffe		*/
/*	  ersetzt da der wegen seiner Lage auf 68000er zum		*/
/*	  Absturz fÅhrte.										*/
/*	Version 0.37 - 4.11.2000								*/
/*	  "invertiert" hereinkommende 1 Bit GIF werden jetzt	*/
/*	  deinvertiert und mit dazupassener Palette versehen.	*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "gif.h"
#include "gif_2lzw.h"
#include "demolib.h"

#define _LSCRDES	7					/* ist wegen des auffÅllens der Strukturen */
#define _IMAGE_DES	10					/* von Compilerseite her leider nîtig */

#define TIMER 0

#define DEBUG	0
#define TIMER	0

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);
static void (*reset_busybox)(short lft, const char *string);

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"GIF",
	0x0037,
	"Christian Eyrich, Olaf Piesche",
	{ "GIF", "", "", "", "", "", "", "", "", "" },
	/* Objekttitel */
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
	/* Objektgrenzwerte */
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	/* Slider-Defaultwerte */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,
	NULL, NULL, NULL, NULL, NULL, NULL
};


long filelen;
short PROCESSOR = 0;

static SMURF_PIC *sm_pic;
static uint8_t inter;

static LSCRDES l_screen;
static IMAGE_DES image_descriptor;
static GC_EXTENSION gcon_ext;
static COM_EXTENSION com_ext;
static COLTAB coltab;
static uint8_t *dest;




/*-------------------------------------------------------------	*/
/*						Headercheck 							*/
/*-------------------------------------------------------------	*/
static uint8_t *check_header(uint8_t *file, short *version)
{
	if (strncmp(file, "GIF", 3) != 0)
		return NULL;

	if (strncmp(file + 3, "87a", 3) == 0)
	{
		strcpy(sm_pic->format_name, "GIF 87a");
		*version = 87;
	} else if (strncmp(file + 3, "89a", 3) == 0)
	{
		strcpy(sm_pic->format_name, "GIF 89a");
		*version = 89;
	} else
	{
		return NULL;
	}

	return file + 6;
}


/*-------------------------------------------------------------	*/
/*				Logical Screen Descriptor auslesen				*/
/*-------------------------------------------------------------	*/
static uint8_t *read_lscrdes(uint8_t *file)
{
	l_screen.width = (file[1] << 8) | file[0];
	l_screen.height = (file[3] << 8) | file[2];
	l_screen.packed_fields = file[4];
	l_screen.background_color = file[5];
	l_screen.aspect_ratio = file[6];

#if DEBUG
	printf("\n\n--------------read Logical Screen Descriptor      ");
	printf("\n  width: %u    ", l_screen.width);
	printf("\n  height: %u   ", l_screen.height);
	printf("\n  packed: %d   ", l_screen.packed_fields);
	printf("\n  back col: %d ", l_screen.background_color);
	printf("\n  aspect: %d   ", l_screen.aspect_ratio);
#endif

	/* Global Color Table? */
	coltab.size = 0;
	if (l_screen.packed_fields & 0x80)
	{
#if DEBUG
		printf("\n  Global Color Table found!");
#endif

		coltab.depth = (l_screen.packed_fields & 0x07) + 1;
		coltab.size = 1 << coltab.depth;
		memcpy(coltab.rgb_values, file + _LSCRDES, 3 * coltab.size);

#if DEBUG
		printf("\n  Size: %d", coltab.size);
#endif
	}

	return file + _LSCRDES + 3 * coltab.size;
}


/*-------------------------------------------------------------	*/
/*			Control Extension Blocks auslesen  [optional]		*/
/*-------------------------------------------------------------	*/
static uint8_t *read_extblks(uint8_t *file, short version)
{
	short blocksize = 0;

#if DEBUG
	printf("\n");
#endif

	/* Extension Blocks aufsammeln */
	while (*file == 0x21)
	{
		blocksize = 3 + *(file + 2);

		switch (*(file + 1))
		{
			/* Graphical Extension Block */
		case 0xf9:
#if DEBUG
			printf("\n  Graphical Extension Block found");
#endif
			if (version == 89)
			{
				gcon_ext.packed_fields = file[3];
				/* nîtig da sonst Zugriff auf ungerade Adresse */
				gcon_ext.delay_time = (file[5] << 8) | file[4];
				gcon_ext.transparent_color = file[6];
			}

			file += blocksize;
			break;

			/* Plain Text Extension Block */
		case 0x01:
#if DEBUG
			printf("\n  Plain Text Extension Block found");
#endif
			file += blocksize;

			/* LÑnge der variablen PlainTextData-Strings dazurechnen */
			while (*file)
				file += 1 + *file;

			break;

			/* Application Extension Block */
		case 0xff:
#if DEBUG
			printf("\n  Application Extension Block found");
#endif
			file += blocksize;

			/* LÑnge der variablen ApplicationData-Strings dazurechnen */
			while (*file)
				file += 1 + *file;

			break;

			/* Comment Extension Block */
		case 0xfe:
#if DEBUG
			printf("\n  Comment Extension Block found");
#endif
			if (version == 89)
			{
				com_ext.commentlength = *(file + 2);
				com_ext.comment = (char *)file + 3;
			}

			file += blocksize;

			/* LÑnge der variablen CommentData-Strings dazurechnen */
			while (*file)
				file += 1 + *file;

			break;

			/* Other Extension Blocks */
		default:
#if DEBUG
			printf("\n  unknown Extension Block found");
#endif
			file += blocksize;

			break;
		}

		/* Terminator Åbergehen */
		file++;
	}

#if DEBUG
	printf("\n");
#endif

	return file;
}


/*-------------------------------------------------------------	*/
/*			Local Image Descriptor auslesen   [min. 1]			*/
/*-------------------------------------------------------------	*/
static uint8_t *read_image_descriptor(uint8_t *file)
{
	/* vorhanden? */
	if (*file != 0x2c)
		return NULL;
#if DEBUG
	printf("\n---------Image Descriptor found!");
#endif

	/* auslesen */
	image_descriptor.left_pos = (file[2] << 8) | file[1];
	image_descriptor.top_pos = (file[4] << 8) | file[3];
	image_descriptor.image_width = (file[6] << 8) | file[5];
	image_descriptor.image_height = (file[8] << 8) | file[7];
	image_descriptor.packed_fields = file[9];
	inter = (image_descriptor.packed_fields & 0x40) >> 6;

#if DEBUG
	printf("\n  lpos: %u", image_descriptor.left_pos);
	printf("\n  tpos: %u", image_descriptor.top_pos);
	printf("\n  image width: %u", image_descriptor.image_width);
	printf("\n  image height: %u", image_descriptor.image_height);
	printf("\n  packed field: %d", image_descriptor.packed_fields);
	printf("\n  interlaced: %d", inter);
#endif

	/* local color table? */
	coltab.size = 0;
	if (image_descriptor.packed_fields & 0x80)
	{
#if DEBUG
		printf("\n  --------Local Color Table found!");
#endif

		coltab.depth = (image_descriptor.packed_fields & 0x07) + 1;
		coltab.size = 1 << coltab.depth;
		memcpy(coltab.rgb_values, file + _IMAGE_DES, 3 * coltab.size);

#if DEBUG
		printf("\n  Size: %d", coltab.size);
#endif
	}

	return file + _IMAGE_DES + 3 * coltab.size;
}


/* Sortiert ein Bild von interlaced Zeilenreihenfolge */
/* in place in normale Zeilenreihenfolge um */
static void deinterlace(uint8_t *buffer, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *lacebuf;
	uint8_t p, Planes;
	short *lacetabf;
	short *lacetabb;
	short i, j, k, l, x, y;
	unsigned long realwidth, planelength;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	reset_busybox(128, "deinterlace GIF");

	if (BitsPerPixel == 8)
	{
		Planes = 1;
		realwidth = width;
	} else
	{
		Planes = BitsPerPixel;
		realwidth = (width + 7) / 8;
		planelength = realwidth * height;
	}

	lacebuf = (uint8_t *) Malloc(realwidth * sizeof(*lacebuf) + height * sizeof(*lacetabf) * 2);
	lacetabf = (short *) (lacebuf + realwidth);
	lacetabb = lacetabf + height;

	for (p = 0; p < Planes; p++)
	{
		for (l = 0, k = 0; l < height; l += 8, k++)
			lacetabf[k] = l;
		for (l = 4; l < height; l += 8, k++)
			lacetabf[k] = l;
		for (l = 2; l < height; l += 4, k++)
			lacetabf[k] = l;
		for (l = 1; l < height; l += 2, k++)
			lacetabf[k] = l;

		i = 1;
		do
		{
			if (lacetabf[i])
			{
				j = 0;
				y = i;
				while ((x = lacetabf[y]) != i)
				{
					lacetabf[y] = 0;
					y = lacetabb[j++] = x;
				}
				lacetabf[y] = 0;

				memcpy(lacebuf, buffer + x * realwidth, realwidth);
				while (j--)
				{
					memcpy(buffer + x * realwidth, buffer + lacetabb[j] * realwidth, realwidth);
					x = lacetabb[j];
				}
				memcpy(buffer + x * realwidth, lacebuf, realwidth);
			}
		} while (++i < height);

		buffer += planelength;
	}

#if TIMER
	/* wie schnell waren wir? */
	printf("\nZeit: %lu", get_timer());
	(void)Cnecin();
#endif

	Mfree(lacebuf);
}


/*---------------------------------------------------------	*/
/*				'Table Based' Image Reader					*/
/*---------------------------------------------------------	*/
static uint8_t *read_tbase_image(uint8_t *file)
{
	uint8_t *data;
	char dummy[3];
	char impmessag[17];
	unsigned long w, memwidth;
	short back;

	/* Local Image Descriptor */
	if ((data = read_image_descriptor(file)) == NULL)
		return NULL;

	if (coltab.depth == 8)
	{
		memwidth = image_descriptor.image_width;
	} else
	{
		w = ((unsigned long) image_descriptor.image_width + 7) / 8;	/* Auf volle Byte gerundete ZeilenlÑnge in Byte */
		memwidth = w * 8;
	}
	/* Die 256 am Ende sind notwendig um Fehler in manchen Bildern */
	/* (nachgewiesenermaûen in den Bildern, nicht im Smurf) abzufangen. */
	/* Sonst wÅrde durch einen zu langen Code am Schluû Åbers Ende geschrieben. */
	if ((dest = (uint8_t *) SMalloc(((memwidth * image_descriptor.image_height * coltab.depth) >> 3) + 256)) == NULL)
		return (uint8_t *) M_MEMORY;
	memset(dest, 0, ((memwidth * image_descriptor.image_height * coltab.depth) >> 3) + 256);

	strcpy(impmessag, "GIF ");
	strcat(impmessag, itoa(coltab.depth, dummy, 10));
	strcat(impmessag, " Bit");

	reset_busybox(128, impmessag);

	if (coltab.depth == 8)
		back = decode_lzw_fast(data, dest);
	else
		back = decode_lzw_normal(data, dest, image_descriptor.image_width, image_descriptor.image_height, coltab.depth);
	if (back < 0)
	{
		Mfree(dest);
		return (uint8_t *) M_MEMORY;
	}
	
	if (inter)
		deinterlace(dest, image_descriptor.image_width, image_descriptor.image_height, coltab.depth);

	return data;
}


/* Invertiert 1 Bit GIF deren Bits im vom GEM abweichenden */
/* Format vorliegen und erstellt eine dazu passende Palette */
static void invert_gif(uint8_t *buffer, unsigned short width, unsigned short height, uint8_t *pal)
{
	unsigned short x, y;
	unsigned long realwidth;

	realwidth = (width + 7) / 8;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*buffer++ = ~*buffer;
		} while (++x < realwidth);
	} while (++y < height);


	pal[0] = 0xff;
	pal[1] = 0xff;
	pal[2] = 0xff;
	pal[3] = 0x0;
	pal[4] = 0x0;
	pal[5] = 0x0;
}



/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					GIF Importer (GIF)				*/
/*		1-8 Bit, LZW								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *file;
	uint8_t *current_file;
	short version;

	PROCESSOR = smurf_struct->services->CPU_type;

	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	sm_pic = smurf_struct->smurf_pic;
	file = smurf_struct->smurf_pic->pic_data;
	filelen = smurf_struct->smurf_pic->file_len;

	/* Header prÅfen */
	if ((current_file = check_header(file, &version)) == NULL)
		return M_INVALID;

	reset_busybox = smurf_struct->services->reset_busybox;

#if DEBUG
	Goto_pos(0, 1);
	printf("\n%s", sm_pic->format_name);
#endif

	com_ext.commentlength = 0;
	com_ext.comment = NULL;

	/* Logical Screen Descriptor lesen */
	current_file = read_lscrdes(current_file);

	/* Control Extension Blocks lesen */
	current_file = read_extblks(current_file, version);

#if DEBUG
	(void)Cnecin();
#endif

	/* Table Based Image lesen */
	if ((current_file = read_tbase_image(current_file)) == NULL)
		return M_PICERR;				/* kein Image Descriptor */
	if (current_file == (uint8_t *) M_MEMORY)
		return M_MEMORY;				/* nicht genug Speicher */

#if DEBUG
	printf("\n\nFarbtiefe: %d", coltab.depth);
	printf("\nBreite: %d", image_descriptor.image_width);
	printf("\nHîhe: %d", image_descriptor.image_height);
#endif

	/* Control Extension Blocks lesen */
	current_file = read_extblks(current_file, version);

	SMfree(smurf_struct->smurf_pic->pic_data);

	/* Achtung bei sehr langen Kommentaren! */
	if (com_ext.commentlength != 0)
		strncpy(smurf_struct->smurf_pic->infotext, com_ext.comment, com_ext.commentlength);

	if (coltab.depth == 1 && coltab.rgb_values[0] == 0x0 && coltab.rgb_values[3] == 0xff)
		invert_gif(dest, image_descriptor.image_width, image_descriptor.image_height, coltab.rgb_values);

	memcpy(smurf_struct->smurf_pic->palette, coltab.rgb_values, 256 * 3);
	smurf_struct->smurf_pic->col_format = RGB;

	if (coltab.depth == 8)
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	else
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	smurf_struct->smurf_pic->pic_data = dest;
	smurf_struct->smurf_pic->pic_width = image_descriptor.image_width;
	smurf_struct->smurf_pic->pic_height = image_descriptor.image_height;
	smurf_struct->smurf_pic->depth = coltab.depth;

	return M_PICDONE;
}


#ifdef PRG
/*-------------- Main-Function fÅr GIF-Programm zum Debuggen */
int main(void)
{
	int filehandle;
	GARGAMEL sm_struct;
	SMURF_PIC pic;
	uint8_t *file;
	long len;

	filehandle = (int)Fopen("amber.gif", FO_READ);
	if (filehandle < 0)
		return 1;

	len = Fseek(0L, filehandle, 2);
	Fseek(0L, filehandle, 0);

	file = (uint8_t *)Malloc(len);

	Fread(filehandle, len, file);
	Fclose(filehandle);

	pic.pic_data = file;
	sm_struct.smurf_pic = &pic;

	imp_module_main(&sm_struct);

	return 0;
}
#endif
