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

/* =========================================================*/
/*			JPEG File Interchange Format Decoder			*/
/* Version 0.1  --  27.09.97								*/
/*	  8 und 24 Bit											*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

#undef LOCAL
#undef GLOBAL
#undef INLINE
#undef EXTERN

#include "jpeglib.h"
#include <setjmp.h>

#define TIMER 0

static uint8_t BitsPerPixel;
static unsigned short width, height;
static SERVICE_FUNCTIONS *Services;

long f_len_global;

struct my_error_mgr
{
	struct jpeg_error_mgr pub;			/* "public" fields */

	jmp_buf setjmp_buffer;				/* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"JPEG",
	0x0010,
	"Christian Eyrich",
	/* Extensionen */
	{ "JPG", "JPEG", "JFIF", "", "", "", "", "", "", "" },
	/* Slider */
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	/* Checkboxen */
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	/* Editfelder */
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",

	/* Minima + Maxima */
	/* Slider */
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	/* Edits */
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	/* Defaults */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};


/* Here's the routine that will replace the standard error_exit method: */
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}										/* my_error_exit */


/* exit() is referenced by error_exit() in jpeg.lib, which is overridden below */
void exit(int code)
{
	(void) code;
	for (;;);
}


/* getenv() will reference _BasPag, which is not available in our module */
char *getenv(const char *name)
{
	(void) name;
	return NULL;
}


static uint8_t *read_JPEG_file(uint8_t *inpic, unsigned long f_len)
{
	uint8_t *outpic;
	uint8_t *volatile ooutpic = NULL;
	char impmessag[21];
	char dummy[3];

	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;

	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct my_error_mgr jerr;

	/* More stuff */
	JSAMPARRAY buffer;					/* Output row buffer */
	long row_stride;					/* physical row width in output buffer */
	unsigned short y, bh, bl;

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		Mfree(ooutpic);
		return NULL;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	jpeg_mem_src(&cinfo, inpic, f_len);

	/* Step 3: read file parameters with jpeg_read_header() */
	(void) jpeg_read_header(&cinfo, TRUE);

	width = cinfo.image_width;
	height = cinfo.image_height;
	if (cinfo.num_components == 3)
		BitsPerPixel = 24;
	else
		BitsPerPixel = 8;
	strcpy(impmessag, "JFIF ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	Services->reset_busybox(0, impmessag);

	/* Step 4: set parameters for decompression */
	cinfo.dct_method = JDCT_IFAST;		/* fast integer dct */

	/* Step 5: Start decompressor */
	(void) jpeg_start_decompress(&cinfo);

	/* We may need to do some setup of our own at this point before reading
	 * the data.  After jpeg_start_decompress() we have the correct scaled
	 * output image dimensions available, as well as the output colormap
	 * if we asked for color quantization.
	 * In this example, we need to make an output work buffer of the right size.
	 */

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (JSAMPARRAY) malloc(sizeof(JSAMPROW));
	buffer[0] = (JSAMPROW) malloc(row_stride);

	if ((outpic = (uint8_t *) Malloc(row_stride * cinfo.output_height)) == 0)
	{
		jpeg_destroy_decompress(&cinfo);
		return (uint8_t *) M_MEMORY;
	}
	ooutpic = outpic;
	memset(outpic, 0, row_stride * cinfo.output_height);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	bh = height / 16;					/* busy-height */
	if (bh == 0)
		bh = height;
	bl = 0;								/* busy-length */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */

	y = 0;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		if ((y % bh) == 0)
		{
			Services->busybox(bl);
			bl += 8;
		}

		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		memcpy(outpic, buffer[0], row_stride);
		outpic += row_stride;

		y++;
	}

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);

	/* Step 8: Release JPEG decompression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	free(buffer[0]);
	free(buffer);

	return ooutpic;
}


static int is_jpeg(uint8_t *buffer)
{
#if 0
	long x = 0;
#endif


#if 0
	printf("Steht Marker 0xffd8 und anschliežendes 0xff am Fileanfang?\n");
#endif

	if (*(unsigned short *) buffer != 0xffd8 || *(buffer + 2) != 0xff)
	{
#if 0
		printf("Kombination steht nicht am Fileanfang\n");
#endif
		return -1;
	} else
	{
#if 0
		printf("Kombination steht am Fileanfang\n");
#endif
	}

#if 0
	printf("Suche jetzt bis zum Fileende nach 0xffe0\n");

	while (x < (f_len_global - 8) && !(*(uint16_t *) buffer == 0xffe0))
	{
		buffer++;
		x++;
	}

	if (*(unsigned long *) (buffer + 4) != 'JFIF')
	{
		printf("0xffe0 nicht gefunden\n");
		return -1;
	} else
	{
		printf("0xffe0 gefunden\n");
		return 0;
	}
#endif

	return 0;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		JPEG File Interchange Format (JPG)			*/
/*		8 und 24 Bit								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	unsigned short i;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	Services = smurf_struct->services;

	buffer = smurf_struct->smurf_pic->pic_data;

	f_len_global = smurf_struct->smurf_pic->file_len;

	if (is_jpeg(buffer) < 0)
	{
		return M_INVALID;
	} else
	{
		if ((ziel = read_JPEG_file(buffer, smurf_struct->smurf_pic->file_len)) == NULL)
			return M_PICERR;
		else if ((long) ziel == M_MEMORY)
			return M_MEMORY;

		strcpy(smurf_struct->smurf_pic->format_name, "JPEGs JFIF .JPG");

		smurf_struct->smurf_pic->pic_width = width;
		smurf_struct->smurf_pic->pic_height = height;
		smurf_struct->smurf_pic->depth = BitsPerPixel;

		Mfree(smurf_struct->smurf_pic->pic_data);
		smurf_struct->smurf_pic->pic_data = ziel;

		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

		if (BitsPerPixel == 8)
		{
			pal = smurf_struct->smurf_pic->palette;
			for (i = 0; i < 256; i++)
			{
				*pal++ = i;
				*pal++ = i;
				*pal++ = i;
			}

			smurf_struct->smurf_pic->col_format = GREY;
		} else
		{
			smurf_struct->smurf_pic->col_format = RGB;
		}
	}

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu", get_timer());
	getch();
#endif

	return M_PICDONE;
}
