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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "nvdi5ras.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "bindings.h"

#ifdef __PUREC__
/* Dies bastelt direkt ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

typedef struct
{
	char Intel;
	uint16_t skiph;
	unsigned long skipv;
	uint16_t v;
	uint16_t rs, gs, gm;
} SERVICE;

#define TIMER 0


static void d24_to_16(uint8_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, SERVICE *service);
static void d24_to_24(uint8_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, SERVICE *service);
static void d16_to_16(uint16_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, SERVICE *service);
static void d16_to_24(uint16_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, SERVICE *service);
static void d8pp_to_16(uint8_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, SERVICE *service, uint8_t *palette);
static void d8pp_to_24(uint8_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, SERVICE *service, uint8_t *palette);
static void dstd_to_16(uint8_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, uint16_t realwidth, uint16_t realheight, SERVICE *service, uint8_t *palette, uint8_t BitsPerPixel);
static void dstd_to_24(uint8_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, uint16_t realwidth, uint16_t realheight, SERVICE *service, uint8_t *palette, uint8_t BitsPerPixel);


#ifdef __GNUC__
static short call_eddi(short (*func)(short), short mode)
{
	/*
	 * The dispatcher uses Pure-C calling convention,
	 * and the mode parameter must be passed in D0
	 */
	register short ret __asm__("d0");
	__asm__ __volatile__(
		"\tmove.l %1,%%a0\n"
		"\tmove.w %2,%%d0\n"
		"\tjsr (%%a0)\n"
	: "=r"(ret)
	: "g"(func), "g"(mode)
	: "d1", "d2", "a0", "a1", "cc" AND_MEMORY);
	return ret;
}
#else
#define call_eddi(func, mode) func(mode)
#endif

/*--------------------------------------------------------------*/
/*			Konvertierung fÅr TC- und HC-Screenmodi				*/
/* Falls Bilder <= 16 Bit in einer 16 Bit Auflîsung, oder		*/
/* Bilder <= 24 Bit (also alle) in einer 24 Bit Auflîsung		*/
/* angezeigt werden sollen und nicht Systempalette ausgewÑhlt	*/
/* ist.															*/
/*--------------------------------------------------------------*/
void direct2screen(SMURF_PIC *picture, uint8_t *where_to, GRECT *part)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *palette;
	char depth;
	char bits;
	uint16_t width, height, endwid, endhgt;
	unsigned long offset;
	WORD work_out[273];
	short (*func)(short);
	SERVICE service;

#if 0
	vq_extnd(handle, 1, work_out);
	if (work_out[30] & 0x02)			/* Bit 1 gesetzt - neue Rasterfunktionen vorhanden? */
	{
		nvdi5_raster(picture, where_to, picture->zoom);
		return;
	}
#endif

	buffer = picture->pic_data;
	width = picture->pic_width;
	height = picture->pic_height;
	depth = picture->depth;
	ziel = where_to;
	palette = picture->palette;

	if (get_cookie(0x45644449L, (unsigned long *)&func) != 0 && call_eddi(func, 0) >= 0x0100)	/* 'EdDI' */
	{
		vq_scrninfo(Sys_info.vdi_handle, work_out);

		/* Anzahl benutzter Bits fÅr Farben */
		if ((((unsigned long)(UWORD)work_out[3] << 16) | (UWORD)work_out[4]) == 32768L)
			bits = 15;
		else
			bits = work_out[2];

		/* Organisations-Dispatcher */
		switch (work_out[16])
		{
			/* VGA 24 Bit Intel oder */
			/* VGA 32 Bit Intel xbgr */
		case 0:
			service.Intel = TRUE;
			if (bits == 24)
			{
				service.rs = 0;		/* wird hier miûbraucht und unten als xfirst benutzt */
				service.gs = 0;		/* wird hier miûbraucht und unten als jump benutzt */
			} else
			{
				service.rs = 1;		/* wird hier miûbraucht und unten als xfirst benutzt */
				service.gs = 1;		/* wird hier miûbraucht und unten als jump benutzt */
			}
			service.gm = 0xf8;
			break;
			/* VGA 15 Bit Intel */
		case 2:
			service.Intel = TRUE;
			service.rs = 7;
			service.gs = 2;
			service.gm = 0xf8;
			break;
			/* VGA 16 Bit Intel */
		case 3:
			service.Intel = TRUE;
			service.rs = 8;
			service.gs = 3;
			service.gm = 0xfc;
			break;
			/* VGA 32 Bit Intel bgrx */
		case 8:
			service.Intel = TRUE;
			service.rs = 0;			/* wird hier miûbraucht und unten als xfirst benutzt */
			service.gs = 1;			/* wird hier miûbraucht und unten als jump benutzt */
			service.gm = 0xfc;
			break;
			/* VGA 15 Bit Motorola */
		case 10:
			service.Intel = FALSE;
			service.rs = 7;
			service.gm = 0xf8;
			service.gs = 2;
			break;
		case 11:
			service.Intel = FALSE;
			if (bits == 15)
			{
				/* Falcon */
				service.rs = 8;
				service.gs = 3;
				service.gm = 0xf8;
			} else
			{
				/* VGA 16 Bit Motorola */
				/* Wird zwar gefÅllt, aber eigentlich nie gebraucht */
				service.rs = 8;
				service.gs = 3;
				service.gm = 0xfc;
			}
			break;
			/* VGA 24 Bit Motorola oder */
			/* VGA 32 Bit Motorola xrgb */
		case 16:
			service.Intel = FALSE;
			if (bits == 24)
			{
				service.rs = 0;		/* wird hier miûbraucht und unten als xfirst benutzt */
				service.gs = 0;		/* wird hier miûbraucht und unten als jump benutzt */
			} else
			{
				service.rs = 1;		/* wird hier miûbraucht und unten als xfirst benutzt */
				service.gs = 1;		/* wird hier miûbraucht und unten als jump benutzt */
			}
			service.gm = 0xf8;
			break;
			/* VGA 32 Bit Motorola rgbx */
		case 24:
			service.Intel = FALSE;
			service.rs = 0;			/* wird hier miûbraucht und unten als xfirst benutzt */
			service.gs = 1;			/* wird hier miûbraucht und unten als jump benutzt */
			service.gm = 0xf8;
			break;
		default:
			return;
		}
	} else
	{
		/* Falcon */
		service.Intel = FALSE;
		service.rs = 8;
		service.gs = 3;
		service.gm = 0xf8;
	}

	if (depth == 24)
	{
		/* nach jedem Pixel Åberspringen */
		service.skiph = picture->zoom * 3;

		/* nach jeder Zeile Åberspringen */
		service.skipv = (unsigned long)picture->zoom * (unsigned long)width * 3L;

		if (part != NULL)
		{
			/* Offset zum Bildanfang */
			offset = ((long)part->g_y * (long)width + (long)part->g_x) * 3L;
			buffer += offset;

			service.skipv += (width - part->g_w) * 3L;
			width = part->g_w;
			height = part->g_h;
		}

		/* unter den Tisch gefallene Bytes nachholen */
		service.skipv += (width % (picture->zoom + 1)) * 3;
	} else
	{
		/* nach jedem Pixel Åberspringen */
		service.skiph = picture->zoom;

		/* nach jeder Zeile Åberspringen */
		if (picture->format_type == FORM_PIXELPAK)
			service.skipv = (unsigned long)picture->zoom * (unsigned long)width;
		else
			service.skipv = (unsigned long)picture->zoom * ((unsigned long)(width + 7) / 8);

		if (part != NULL)
		{
			/* Offset zum Bildanfang */
			if (picture->format_type == FORM_PIXELPAK)
				offset = (long)part->g_y * (long)width + (long)part->g_x;
			else
				offset = (long)part->g_y * (long)((width + 7) / 8) + (long)(part->g_x + 7) / 8;
			buffer += offset;

			if (picture->format_type == FORM_PIXELPAK)
				service.skipv += width - part->g_w;
			width = part->g_w;
			height = part->g_h;
		}

		/* unter den Tisch gefallene Bytes nachholen */
		if (picture->format_type == FORM_PIXELPAK)
			service.skipv += width % (picture->zoom + 1);
	}

	endwid = width / (picture->zoom + 1);
	endhgt = height / (picture->zoom + 1);

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	/* 16 Bit Bildschirmmodus */
	if (bplanes == 16)
	{
		if (!part)
			Dialog.busy.reset(0, "-> 16 Bit");

		service.v = ((endwid + 15) / 16) * 16L - endwid;	/* Padpixel um auf Vielfaches von 16 zu kommen */

		if (picture->format_type == FORM_STANDARD)
			dstd_to_16(buffer, (uint16_t *)ziel, endwid, endhgt, picture->pic_width, picture->pic_height, &service, palette, depth);
		else if (depth == 8)
			d8pp_to_16(buffer, (uint16_t *)ziel, endwid, endhgt, &service, palette);
		else if (depth == 16)
			d16_to_16((uint16_t *)buffer, (uint16_t *)ziel, endwid, endhgt, &service);
		else if (depth == 24)
			d24_to_16(buffer, (uint16_t *)ziel, endwid, endhgt, &service);
	} else
	{
		if (!part)
		{
			if (bplanes == 24)
				Dialog.busy.reset(0, "-> 24 Bit");
			else
				Dialog.busy.reset(0, "-> 32 Bit");
		}

		service.v = (((endwid + 15) / 16) * 16L - endwid) * (bplanes >> 3);	/* Padpixel um auf Vielfaches von 16 zu kommen */

		if (picture->format_type == FORM_STANDARD)
			dstd_to_24(buffer, ziel, endwid, endhgt, picture->pic_width, picture->pic_height, &service, palette, depth);
		else if (depth == 8)
			d8pp_to_24(buffer, ziel, endwid, endhgt, &service, palette);
		else if (depth == 16)
			d16_to_24((uint16_t *)buffer, ziel, endwid, endhgt, &service);
		else if (depth == 24)
			d24_to_24(buffer, ziel, endwid, endhgt, &service);
	}


#if TIMER
	/* wie schnell waren wir? */
	printf("\n%lu", get_timer());
	(void)Cnecin();
#endif
}



/* <= 8 Bit Standardformat -> 16 Bit */
static void dstd_to_16(uint8_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, uint16_t realwidth, uint16_t realheight, SERVICE *service, uint8_t *palette, uint8_t BitsPerPixel)
{
	uint8_t *line;
	uint8_t *pixbuf;
	uint8_t *pal;
	char Intel;
	uint16_t rs, gs, gm;
	uint16_t skiph;
	uint16_t v;
	uint16_t *convtab;
	uint16_t *oconvtab;
	uint16_t x;
	uint16_t y;
	uint16_t count;
	unsigned long w, planelength, skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	rs = service->rs;
	gs = service->gs;
	gm = service->gm;

	count = 1 << BitsPerPixel;
	convtab = (uint16_t *)calloc(1, count << 1);
	oconvtab = convtab;

	/* Vorberechnung */
	x = 0;
	pal = palette;
	do
	{
		if (Intel)
			*convtab++ = swap_word(((pal[0] & 0xf8) << rs) | ((pal[1] & gm) << gs) | (pal[2] >> 3));
		else
			*convtab++ = ((pal[0] & 0xf8) << rs) | ((pal[1] & gm) << gs) | (pal[2] >> 3);
		pal += 3;
	} while (++x < count);

	convtab = oconvtab;


	w = (realwidth + 7) / 8;
	planelength = w * (unsigned long)realheight;	/* LÑnge einer Plane in Bytes */

	pixbuf = (uint8_t *)SMalloc(realwidth + 7);

	/* Anwendung */
	y = 0;
	do
	{
		memset(pixbuf, 0, realwidth);
		getpix_std_line(buffer, pixbuf, BitsPerPixel, planelength, realwidth);
		buffer += w;
		buffer += skipv;
		line = pixbuf;

		x = 0;
		do
		{
			*ziel++ = convtab[*line++];

			line += skiph;
		} while (++x < width);

		ziel += v;
	} while (++y < height);

	free(convtab);
	SMfree(pixbuf);
}


/* <= 8 Bit Standardformat -> 24 Bit */
static void dstd_to_24(uint8_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, uint16_t realwidth, uint16_t realheight, SERVICE *service, uint8_t *palette, uint8_t BitsPerPixel)
{
	uint8_t *line;
	uint8_t *pixbuf;
	uint8_t *pal;
	char Intel;
	uint8_t val;
	uint16_t skiph;
	uint16_t v;
	uint8_t xfirst;
	uint16_t jump;
	uint16_t x;
	uint16_t y;
	unsigned long w, planelength, skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	xfirst = service->rs;
	jump = service->gs;

	w = (realwidth + 7) / 8;
	planelength = w * (unsigned long)realheight;	/* LÑnge einer Plane in Bytes */

	pixbuf = (uint8_t *)SMalloc(realwidth + 7);

	if (xfirst)
		ziel++;

	y = 0;
	do
	{
		memset(pixbuf, 0x0, realwidth);
		getpix_std_line(buffer, pixbuf, BitsPerPixel, planelength, realwidth);
		buffer += w;
		buffer += skipv;
		line = pixbuf;

		x = 0;
		do
		{
			val = *line++;
			pal = palette + val + val + val;

			if (Intel)
			{
				*ziel++ = *(pal + 2);
				*ziel++ = *(pal + 1);
				*ziel++ = *pal;
			} else
			{
				*ziel++ = *pal++;
				*ziel++ = *pal++;
				*ziel++ = *pal;
			}

			ziel += jump;				/* 32 Bit? */

			line += skiph;
		} while (++x < width);

		ziel += v;
	} while (++y < height);

	SMfree(pixbuf);
}


/* 8 Bit pixelpacked -> 16 Bit */
static void d8pp_to_16(uint8_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, SERVICE *service, uint8_t *palette)
{
	uint8_t *pal;
	char Intel;
	uint16_t rs, gs, gm;
	uint16_t v;
	uint16_t skiph;
	uint16_t *convtab;
	uint16_t *oconvtab;
	uint16_t x;
	uint16_t y;
	unsigned long skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	rs = service->rs;
	gs = service->gs;
	gm = service->gm;

	convtab = (uint16_t *)calloc(1, SM_PALETTE_MAX * sizeof(*convtab));
	oconvtab = convtab;

	/* Vorberechnung */
	x = 0;
	pal = palette;
	do
	{
		if (Intel)
			*convtab++ = swap_word(((pal[0] & 0xf8) << rs) | ((pal[1] & gm) << gs) | (pal[2] >> 3));
		else
			*convtab++ = ((pal[0] & 0xf8) << rs) | ((pal[1] & gm) << gs) | (pal[2] >> 3);
		pal += 3;
	} while (++x < SM_PALETTE_MAX);

	convtab = oconvtab;

	/* Anwendung */
	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = convtab[*buffer++];

			buffer += skiph;
		} while (++x < width);

		buffer += skipv;
		ziel += v;
	} while (++y < height);

	free(convtab);
}


/* 8 Bit pixelpacked -> 24 Bit */
static void d8pp_to_24(uint8_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, SERVICE *service, uint8_t *palette)
{
	uint8_t *pal;
	char Intel;
	uint16_t skiph;
	uint16_t v;
	uint8_t val, xfirst;
	uint16_t jump;
	uint16_t x, y;
	unsigned long skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	xfirst = service->rs;
	jump = service->gs;

	if (xfirst)
		ziel++;

	y = 0;
	do
	{
		x = 0;
		do
		{
			val = *buffer++;
			pal = palette + val + val + val;

			if (Intel)
			{
				*ziel++ = *(pal++ + 2);
				*ziel++ = *pal++;
				*ziel++ = *(pal++ - 2);
			} else
			{
				*ziel++ = *pal++;
				*ziel++ = *pal++;
				*ziel++ = *pal++;
			}

			ziel += jump;				/* 32 Bit? */

			buffer += skiph;
		} while (++x < width);

		buffer += skipv;
		ziel += v;
	} while (++y < height);
}


/* 16 Bit -> 16 Bit */
static void d16_to_16(uint16_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, SERVICE *service)
{
	char Intel;
	uint16_t rs, gs, gm;
	uint16_t skiph;
	uint16_t v;
	uint8_t r, g, b;
	uint16_t x, y, val;
	unsigned long skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	rs = service->rs;
	gs = service->gs;
	gm = service->gm;

	if (rs == 8 && !Intel)
	{
		/* Falcon oder VGA 16 Bit Motorola */
		y = 0;
		do
		{
			x = 0;
			do
			{
				*ziel++ = *buffer++;

				buffer += skiph;
			} while (++x < width);
			buffer += skipv;
			ziel += v;
		} while (++y < height);
	} else
	{
		/* VGA 15 Bit Motorola und Intel sowie VGA 16 Bit Intel */
		y = 0;
		do
		{
			x = 0;
			do
			{
				val = *buffer++;
				r = (val & 0xf800) >> 8;
				g = (val & 0x7e0) >> 3;
				b = (val & 0x1f) << 3;

				if (Intel)
					*ziel++ = swap_word(((r & 0xf8) << rs) | ((g & gm) << gs) | (b >> 3));
				else
					*ziel++ = ((r & 0xf8) << rs) | ((g & gm) << gs) | (b >> 3);

				buffer += skiph;
			} while (++x < width);

			buffer += skipv;
			ziel += v;
		} while (++y < height);
	}
}


/* 16 Bit -> 24 Bit */
static void d16_to_24(uint16_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, SERVICE *service)
{
	char Intel;
	uint16_t skiph;
	uint16_t v;
	uint8_t xfirst;
	uint16_t jump;
	uint16_t x, y, val;
	unsigned long skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	xfirst = service->rs;
	jump = service->gs;

	if (xfirst)
		ziel++;

	y = 0;
	do
	{
		x = 0;
		do
		{
			val = *buffer++;
			if (Intel)
			{
				*ziel++ = (val & 0x1f) << 3;
				*ziel++ = (val & 0x7e0) >> 3;
				*ziel++ = (val & 0xf800) >> 8;
			} else
			{
				*ziel++ = (val & 0xf800) >> 8;
				*ziel++ = (val & 0x7e0) >> 3;
				*ziel++ = (val & 0x1f) << 3;
			}

			ziel += jump;				/* 32 Bit? */

			buffer += skiph;
		} while (++x < width);

		buffer += skipv;
		ziel += v;
	} while (++y < height);
}


/* 24 Bit -> 16 Bit */
static void d24_to_16(uint8_t *buffer, uint16_t *ziel, uint16_t width, uint16_t height, SERVICE *service)
{
	char Intel;
	uint16_t skiph;
	uint16_t rs, gs, gm;
	uint16_t v;
	uint16_t x, y;
	unsigned long skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	rs = service->rs;
	gs = service->gs;
	gm = service->gm;

	y = 0;
	do
	{
		x = 0;
		do
		{
			if (Intel)
				*ziel++ = swap_word(((buffer[0] & 0xf8) << rs) | ((buffer[1] & gm) << gs) | (buffer[2]>> 3));
			else
				*ziel++ = ((buffer[0] & 0xf8) << rs) | ((buffer[1] & gm) << gs) | (buffer[2] >> 3);

			buffer += skiph;
		} while (++x < width);

		buffer += skipv;
		ziel += v;
	} while (++y < height);
}


/* 24 Bit -> 24 Bit */
static void d24_to_24(uint8_t *buffer, uint8_t *ziel, uint16_t width, uint16_t height, SERVICE *service)
{
	char Intel;
	uint16_t skiph;
	uint16_t v;
	uint8_t xfirst;
	uint16_t jump;
	uint16_t x, y;
	unsigned long skipv;

	Intel = service->Intel;
	skiph = service->skiph;
	skipv = service->skipv;
	v = service->v;
	xfirst = service->rs;
	jump = service->gs;

	if (xfirst)
		ziel++;

	y = 0;
	do
	{
		x = 0;
		do
		{
			if (Intel)
			{
				*ziel++ = *(buffer++ + 2);
				*ziel++ = *buffer++;
				*ziel++ = *(buffer++ - 2);
			} else
			{
				*ziel++ = *buffer++;
				*ziel++ = *buffer++;
				*ziel++ = *buffer++;
			}

			ziel += jump;				/* 32 Bit? */

			buffer += skiph;
		} while (++x < width);

		buffer += skipv;
		ziel += v;
	} while (++y < height);
}
