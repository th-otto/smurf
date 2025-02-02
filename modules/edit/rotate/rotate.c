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
/*					freies Drehen-Modul						*/
/*					three shear method						*/
/* Version 0.1  --  13.04.96/14.04.96						*/
/*	  8, 16 und 24 Bit ohne Antialiasing					*/
/* Version 0.2  --  14.04.96								*/
/*	  24 Bit mit Antialiasing								*/
/* Version 0.3  --  16.07.98								*/
/*	  Speicherschie�er behoben								*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

short (*busybox)(short pos);

void rotate_xschleife(long *par, int width) ASM_NAME("_rotate_xschleife");
void rotate_yschleife(long *par, int height) ASM_NAME("_rotate_yschleife");

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

MOD_INFO module_info = {
	"freies Drehen",
	0x0030,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ "", "", "", "" },
	{ "interpolierend", "", "", "" },
	{ "Winkel", "", "", "" },
	{ { 0, 64 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { -45, 45 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0,
	0, 0, 0,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};




/* dreht ohne Antialiasing */
static uint8_t *rotate_normal(uint8_t *buffer, unsigned short width, unsigned short height, unsigned short newwidth,
	unsigned short newheight, unsigned short tempwidth, float angle, float xshearfac, float yshearfac,
	short xshearjunk, short yshearjunk, uint8_t bytes)
{
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	short intnew0, new;
	unsigned short x, y;
	unsigned short bh, bl;
	unsigned long realwidth, realtempwidth, realnewwidth, off;

	realwidth = width * bytes;
	realtempwidth = tempwidth * bytes;
	realnewwidth = newwidth * bytes;


	obuffer = buffer;

	/* In X-Richtung scheren */

	if ((ziel = SMalloc((long) tempwidth * (long) height * bytes)) == 0)
		return NULL;

	oziel = ziel;
	memset(ziel, 0, (long) tempwidth * (long) height * bytes);

	if ((bh = height / 8) == 0)			/* busy-height */
		bh = height;
	bl = 0;								/* busy-length */

	y = 0;
	do
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 5;
		}

		if (angle > 0)
			intnew0 = y * xshearfac;
		else
			intnew0 = (height - y) * xshearfac;

		memcpy(ziel + (intnew0 * bytes), buffer, realwidth);
		buffer += realwidth;
		ziel += realtempwidth;
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

	SMfree(buffer);
	buffer = ziel;


	obuffer = buffer;

	/* In Y-Richtung scheren */

	if ((ziel = SMalloc((long) tempwidth * (long) newheight * bytes)) == 0)
		return NULL;

	oziel = ziel;
	memset(ziel, 0, (long) tempwidth * (long) newheight * bytes);

	bh = tempwidth / 8;					/* busy-height */

	x = 0;
	do
	{
		buffer = obuffer + x * bytes;
		ziel = oziel + x * bytes;

		if (!(x % bh))
		{
			busybox(bl);
			bl += 5;
		}

		if (angle > 0)
			intnew0 = (tempwidth - x) * yshearfac;
		else
			intnew0 = x * yshearfac;

		intnew0 -= yshearjunk;

		for (y = 0; y < height; y++)
		{
			new = y + intnew0;
			if (new >= 0 && new < newheight)
			{
				off = new * realtempwidth;
				*(ziel + off) = *buffer;
				if (bytes > 1)
				{
					*(ziel + off + 1) = *(buffer + 1);
					if (bytes > 2)
						*(ziel + off + 2) = *(buffer + 2);
				}
			}

			buffer += realtempwidth;
		}
		while (++y < height) ;
	} while (++x < tempwidth);

	buffer = obuffer;
	ziel = oziel;

	SMfree(buffer);
	buffer = ziel;


	obuffer = buffer;

	/* Zur Korrektur wieder in X-Richtung scheren */

	if ((ziel = SMalloc((long) newwidth * (long) newheight * bytes + 6)) == 0)
		return NULL;

	oziel = ziel;
	memset(ziel, 0, (long) newwidth * (long) newheight * bytes);


	if ((bh = newheight / 8) == 0)		/* busy-height */
		bh = newheight;

	y = 0;
	do
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 5;
		}

		if (angle > 0)
			intnew0 = y * xshearfac;
		else
			intnew0 = (newheight - y) * xshearfac;

		intnew0 -= xshearjunk;

		if (intnew0 < 0)
			memcpy(ziel, buffer + -intnew0 * bytes, realnewwidth);
		else
			memcpy(ziel + intnew0 * bytes, buffer, realnewwidth - intnew0 * bytes);
		buffer += realtempwidth;
		ziel += realnewwidth;
	} while (++y < newheight);

	buffer = obuffer;
	ziel = oziel;

	SMfree(buffer);

	return ziel;
}


/* dreht mit Antialiasing */
static uint8_t *rotate_antialias(uint8_t *buffer, unsigned short width, unsigned short height, unsigned short newwidth,
	unsigned short newheight, unsigned short tempwidth, float angle, float xshearfac, float yshearfac,
	short xshearjunk, short yshearjunk)
{
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;

	short intnew0, new;
	unsigned short x, y;
	unsigned short bh, bl;

	long par[6];
	unsigned long realwidth, realtempwidth,	 realnewwidth, off;
	unsigned long fracnew0;
	unsigned long omfracnew0;
	unsigned long prevr;
	unsigned long prevg;
	unsigned long prevb;

	float new0;


	realwidth = (long) width *3L;

	realtempwidth = tempwidth * 3;
	realnewwidth = newwidth * 3;

	obuffer = buffer;

	/* In X-Richtung scheren */

	if ((ziel = SMalloc((long) tempwidth * (long) height * 3 + 3)) == 0)
		return NULL;

	oziel = ziel;
	memset(ziel, 0, (long) tempwidth * (long) height * 3);

	if ((bh = height / 5) == 0)			/* busy-height */
		bh = height;
	bl = 0;								/* busy-length */

	y = 0;
	do
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 8;
		}

		if (angle > 0.0)
			new0 = y * xshearfac;
		else
			new0 = (height - y) * xshearfac;

		intnew0 = new0;

		fracnew0 = (new0 - intnew0) * 4096L;
		omfracnew0 = 4096L - fracnew0;

		par[0] = (long) fracnew0;
		par[1] = (long) omfracnew0;
		par[2] = (long) (ziel + (intnew0 * 3));
		par[3] = (long) buffer;
		rotate_xschleife(par, width);

		buffer += realwidth;
		ziel += realtempwidth;
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

	SMfree(buffer);
	buffer = ziel;


	obuffer = buffer;

	/* In Y-Richtung scheren */

	if ((ziel = SMalloc((long) tempwidth * (long) newheight * 3)) == 0)
		return NULL;

	oziel = ziel;
	memset(ziel, 0, (long) tempwidth * (long) newheight * 3);

	bh = tempwidth / 5;					/* busy-height */

	x = 0;
	do
	{
		buffer = obuffer + x * 3;
		ziel = oziel + x * 3;

		if (!(x % bh))
		{
			busybox(bl);
			bl += 8;
		}

		if (angle > 0)
			new0 = (tempwidth - x) * yshearfac;
		else
			new0 = x * yshearfac;

		intnew0 = new0;

		fracnew0 = (new0 - intnew0) * 4096L;
		omfracnew0 = 4096L - fracnew0;

		intnew0 -= yshearjunk;

		off = intnew0 * realwidth;
#if 0
		par[0] = (long)fracnew0;
		par[1] = (long)omfracnew0;
		par[2] = (long)realtempwidth;
		par[3] = (long)realtempwidth;
		par[4] = (long)(ziel + off);
		par[5] = (long)buffer;
		rotate_yschleife(par, height);
#endif

		prevr = prevg = prevb = 0x0;

		y = 0;
		do
		{
			new = y + intnew0;
			if (new >= 0 && new < newheight)
			{
				off = new * realtempwidth;
				*(ziel + off + 0) = ((prevr * fracnew0 + (unsigned long) *buffer * omfracnew0 + 2048) >> 12);
				*(ziel + off + 1) = ((prevg * fracnew0 + (unsigned long) *(buffer + 1) * omfracnew0 + 2048) >> 12);
				*(ziel + off + 2) = ((prevb * fracnew0 + (unsigned long) *(buffer + 2) * omfracnew0 + 2048) >> 12);

				prevr = (unsigned long) *buffer;
				prevg = (unsigned long) *(buffer + 1);
				prevb = (unsigned long) *(buffer + 2);
			}

			buffer += realtempwidth;
		} while (++y < height);

		if (fracnew0 && intnew0 + height < newheight)
		{
			ziel += off + realtempwidth;
			*ziel++ = ((prevr * fracnew0 + 0x0 * omfracnew0 + 2048) >> 12);
			*ziel++ = ((prevg * fracnew0 + 0x0 * omfracnew0 + 2048) >> 12);
			*ziel++ = ((prevb * fracnew0 + 0x0 * omfracnew0 + 2048) >> 12);
		}

	} while (++x < tempwidth);

	buffer = obuffer;
	ziel = oziel;

	SMfree(buffer);
	buffer = ziel;


	obuffer = buffer;

	/* Zur Korrektur wieder in X-Richtung scheren */

	if ((ziel = SMalloc((long) newwidth * (long) newheight * 3)) == 0)
		return NULL;

	oziel = ziel;
	memset(ziel, 0, (long) newwidth * (long) newheight * 3);


	if ((bh = newheight / 5) == 0)		/* busy-height */
		bh = newheight;

	y = 0;
	do
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 8;
		}

		if (angle > 0)
			new0 = y * xshearfac;
		else
			new0 = (newheight - y) * xshearfac;

		intnew0 = new0;

		fracnew0 = (new0 - intnew0) * 4096L;
		omfracnew0 = 4096L - fracnew0;

		intnew0 -= xshearjunk;

		prevr = prevg = prevb = 0x0;

		x = 0;
		do
		{
			new = intnew0 + x;

			if (new >= 0 && new < newwidth)
			{
				off = new * 3;
				/* newxelzeile ist newwidth lang */
				*(ziel + off + 0) = ((prevr * fracnew0 + (unsigned long) *buffer * omfracnew0 + 2048) >> 12);
				*(ziel + off + 1) = ((prevg * fracnew0 + (unsigned long) *(buffer + 1) * omfracnew0 + 2048) >> 12);
				*(ziel + off + 2) = ((prevb * fracnew0 + (unsigned long) *(buffer + 2) * omfracnew0 + 2048) >> 12);

				prevr = (unsigned long) *buffer++;
				prevg = (unsigned long) *buffer++;
				prevb = (unsigned long) *buffer++;
			} else
				buffer += 3;
		} while (++x < tempwidth);

		if (fracnew0 && intnew0 + tempwidth < newwidth)
		{
			off += new;
			*(ziel + off + 0) = ((prevr * fracnew0 + 0x0 * omfracnew0 + 2048) >> 12);
			*(ziel + off + 1) = ((prevg * fracnew0 + 0x0 * omfracnew0 + 2048) >> 12);
			*(ziel + off + 2) = ((prevb * fracnew0 + 0x0 * omfracnew0 + 2048) >> 12);
		}

		ziel += realnewwidth;
	} while (++y < newheight);


	buffer = obuffer;
	ziel = oziel;

	SMfree(buffer);

	return ziel;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				freies Drehen-Modul					*/
/*  Dreht eine Bitmap um bis zu 360 Grad - die		*/
/*  eigentliche Engine dreht allerdings nur im		*/
/*  Bereich -45 bis +45 Grad, alles andere 			*/
/*  wird vorher verlustfrei und in-place zurecht-	*/
/*  geflippt.										*/
/*  Die Engine l�uft mit Alan Paeth' "three-shear	*/
/*  method" aus "A fast algorithm for general raster*/
/*  rotation", Graphics Interface '86 S. 77-81.		*/
/*  Das Schei�teil fri�t leider den Speicher: zwei	*/
/*  Puffer, die gr��er als das Original sind, plus	*/
/*  Output und Input - und das auch noch in 24 Bit	*/
/*		8, 16 und 24 Bit							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t BitsPerPixel;
	uint8_t antialias;

	unsigned short width, height;
	unsigned short tempwidth;
	unsigned short tempheight;
	unsigned short newwidth, newheight;
	short xshearjunk, yshearjunk;
	float angle, xshearfac, yshearfac;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde, */
	/* ein Einstellformular anfordern....             */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Einstellformular wurde mit START verlassen - Funktion ausf�hren */
	case MEXEC:
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;
		busybox = smurf_struct->services->busybox;

#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		antialias = smurf_struct->check[0];
		angle = (float) smurf_struct->edit[0];

		BitsPerPixel = smurf_struct->smurf_pic->depth;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		buffer = smurf_struct->smurf_pic->pic_data;

		if (antialias && BitsPerPixel != 24)
		{
			smurf_struct->module_mode = M_WAITING;
			return;
		}

		angle = angle * M_PI / 180.0;	/* nach Radiant konvertieren */

		xshearfac = fabs(tan(angle / 2.0));
		yshearfac = fabs(sin(angle));

		tempwidth = height * xshearfac + width;
		yshearjunk = (tempwidth - width) * yshearfac;
		tempheight = tempwidth * yshearfac + height;
		xshearjunk = (tempheight - height - yshearjunk) * xshearfac;
		newheight = tempheight - 2 * yshearjunk;
		newwidth = newheight * xshearfac + tempwidth - 2 * xshearjunk;

		smurf_struct->services->reset_busybox(0, "durchdrehen ...");

		if (antialias)
			ziel = rotate_antialias(buffer, width, height, newwidth, newheight, tempwidth, angle, xshearfac, yshearfac, xshearjunk, yshearjunk);
		else
			ziel = rotate_normal(buffer, width, height, newwidth, newheight, tempwidth, angle, xshearfac, yshearfac, xshearjunk, yshearjunk, BitsPerPixel >> 3);

		if (ziel == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}

		smurf_struct->smurf_pic->pic_width = newwidth;
		smurf_struct->smurf_pic->pic_height = newheight;
		smurf_struct->smurf_pic->pic_data = ziel;


#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_PICDONE;
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
