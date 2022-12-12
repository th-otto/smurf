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
/*						GEM-Image Encoder					*/
/* Version 0.1  --  04.06.96 - 09.06.96						*/
/*	  Nur XIMG.	Und nur Bilder im korrekten Format.			*/
/* Version 0.2  --  10.01.97 - 20.01.97						*/
/*	  Kompressionstheorem Åberarbeitet und Bugs gefixt		*/
/*	  wegen denen in anderen Speicher geschreiben wurde.	*/
/* Version 0.3  --  28.04.97								*/
/*	  Eigenes Konfigurationsformular eingebaut.				*/
/* Version 0.4  --  24.09.98								*/
/*	  1 Bit Bilder jetzt auch als IMG.						*/
/* Version 0.5  --  17.11.98								*/
/*	  Nun auch Bilder bis <= 8 Bit als IMG.					*/
/* Version 0.6  --  xx.xx.98								*/
/*	  CMY-IMGs kînnen nun erstellt werden.					*/
/* Version 0.7  --  xx.xx.22								*/
/*	  Fix for calculating header size of true-color images.	*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY==1
#include "de/gem_img.rsh"
#elif COUNTRY==0
#include "en/gem_img.rsh"
#elif COUNTRY==2
#include "en/gem_img.rsh"		/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define TIMER 0


#define SOLID0 1
#define SOLID1 2
#define PATRUN 3
#define BITSTR 4

#define IMG		0
#define XIMG	1

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"GEM-(X)Image",
	0x0070,
	"Christian Eyrich",
	{ "IMG", "", "", "", "", "", "", "", "", "" },
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
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 2, 3, 4, 6, 7, 8, 24,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_PIXELPAK,
	M_MORE
};

static void *(*SMalloc)(long amount);


static unsigned long write_RGB(uint8_t *buffer, uint8_t *ziel, unsigned long w, uint8_t pad, unsigned short height, unsigned short strings)
{
	uint8_t *obuffer;
	uint8_t *vbuffer;
	uint8_t vrc;
	uint8_t v3[3];
	uint8_t count;
	uint8_t run;
	unsigned short x, y, k;
	unsigned long plo;
	unsigned long complen;
	unsigned long complenp;
	unsigned long uncomplen;
	unsigned long left;

	complen = 0;
	complenp = 0;
	uncomplen = (unsigned long) strings * 2L + w;

	obuffer = buffer;

	y = 0;
	do									/* height */
	{
		plo = y * w;					/* Offset vom Planeanfang in Bytes */

		/* vrc mîglich? */
		vrc = 1;
		while (y + vrc < height && vrc < 255)
		{
			vbuffer = obuffer + plo;

			if (memcmp(vbuffer, vbuffer + w, w) != 0)
				goto whileoff;
			plo += w;
			vrc++;
		}

	  whileoff:
		if (vrc > 1)					/* Bringt's der if? */
		{
			y += vrc - 1;
			plo = y * w;				/* Offset vom Planeanfang in Bytes */

			*ziel++ = 0x0;
			*ziel++ = 0x0;
			*ziel++ = 0xff;
			*ziel++ = vrc;
			complen += 4;
		}

		buffer = obuffer + plo;			/* Quelladresse der zu kodierenden Scanline */

		x = 0;
		do								/* width */
		{
			count = 1;

			v3[0] = *buffer++;
			v3[1] = *buffer++;
			v3[2] = *buffer++;
			x += 3;

			while (x + 3u <= w && count < 255)
			{
				if (*buffer != v3[0] || *(buffer + 1) != v3[1] || *(buffer + 2) != v3[2])
					break;
				buffer += 3;
				x += 3;
				count++;
			}

			if (count > 1)				/* PATRUN */
			{
				*ziel++ = 0x0;
				*ziel++ = count;
				*ziel++ = v3[0];
				*ziel++ = v3[1];
				*ziel++ = v3[2];
				complenp += 5;
			} else						/* BITSTR */
			{
				*ziel++ = 0x80;
				ziel++;
				*ziel++ = v3[0];
				*ziel++ = v3[1];
				*ziel++ = v3[2];
				count = 3;
				while (x < w && count < 253)
				{
					if (x + 6u > w ||
						*buffer != *(buffer + 3) ||
						*(buffer + 1) != *(buffer + 4) ||
						*(buffer + 2) != *(buffer + 5))
					{
						*ziel++ = *buffer++;
						*ziel++ = *buffer++;
						*ziel++ = *buffer++;
						x += 3;
						count += 3;
					} else
						break;
				}
				*(ziel - count - 1) = count;
				complenp += 2 + count;
			}
		} while (x < w);				/* x */
		if (complenp > uncomplen)
		{
			ziel -= complenp;
			buffer -= w;
			left = w;
			k = 0;
			do
			{
				if (left >= 255)
					run = 255;
				else
					run = left;
				*ziel++ = 0x80;
				*ziel++ = run;
				memcpy(ziel, buffer, run);
				ziel += run;
				buffer += run;
				complen += run + 2;
				left -= run;
			} while (++k < strings);
		} else
		{
			complen += complenp;
		}

		complenp = 0;

		/* noch pad Bytes als SOLID0 dazu */
		if (pad)
		{
			*ziel++ = pad;
			complen++;
		}
	} while (++y < height);

	return (complen);
}


static unsigned long write_Plane(uint8_t *buffer, uint8_t *ziel, unsigned long w, unsigned short height, uint8_t BitsPerPixel, unsigned short strings)
{
	uint8_t *obuffer;
	uint8_t *vbuffer;
	uint8_t vrc;
	uint8_t v1;
	uint8_t count;
	uint8_t run;
	uint8_t Planes;
	uint8_t p;
	uint8_t mode;
	unsigned short x, y, k;

	unsigned long pla;
	unsigned long plh;
	unsigned long plo;
	unsigned long complen;
	unsigned long complenp;
	unsigned long uncomplen;
	unsigned long left;

	Planes = BitsPerPixel;

	plh = (unsigned long) height * w;	/* Hîhe einer Plane in Byte */

	complen = 0;
	complenp = 0;
	uncomplen = (unsigned long) strings *2L + w;

	obuffer = buffer;

	y = 0;
	do
	{
		plo = y * w;					/* Offset vom Planeanfang in Bytes */

		/* vrc mîglich? */
		vrc = 1;
		while (y + vrc < height && vrc < 255)
		{
			p = 0;
			do
			{
				vbuffer = obuffer + p * plh + plo;

				if (memcmp(vbuffer, vbuffer + w, w) != 0)
					goto whileoff;
			} while (++p < Planes);
			plo += w;
			vrc++;
		}

	  whileoff:
		if (vrc > 1)					/* Bringt's der if? */
		{
			y += vrc - 1;
			plo = y * w;				/* Offset vom Planeanfang in Bytes */

			*ziel++ = 0x0;
			*ziel++ = 0x0;
			*ziel++ = 0xff;
			*ziel++ = vrc;
			complen += 4;
		}

		for (p = 0; p < Planes; p++)	/* Planes */
		{
			pla = plh * p;				/* Abstand dieser Plane vom Bildanfang */
			buffer = obuffer + pla + plo;	/* Quelladresse der zu kodierenden Scanline */

			x = 0;
			do
			{
				count = 1;

				v1 = *buffer++;
				x++;

				if (v1 == 0x0)
					mode = SOLID0;
				else if (v1 == 0xff)
					mode = SOLID1;
				else if (x + 1u < w && *buffer == v1 && *(buffer + 1) == v1)
					mode = PATRUN;
				else
					mode = BITSTR;

				switch (mode)
				{
				case SOLID0:
					while (*buffer == 0x0 && x < w && count < 127)
					{
						buffer++;
						x++;
						count++;
					}
					*ziel++ = count;
					complenp++;
					break;
				case SOLID1:
					while (*buffer == 0xff && x < w && count < 127)
					{
						buffer++;
						x++;
						count++;
					}
					*ziel++ = count | 0x80;
					complenp++;
					break;
				case PATRUN:
					while (*buffer == v1 && x < w && count < 255)
					{
						buffer++;
						x++;
						count++;
					}
					*ziel++ = 0x0;
					*ziel++ = count;
					*ziel++ = v1;
					complenp += 3;
					break;
				case BITSTR:
					*ziel++ = 0x80;
					ziel++;
					*ziel++ = v1;
					while (x < w && count < 255)
					{
						if (x + 3u > w ||
							((!(*buffer == 0x0 && *(buffer + 1) == 0x0 && *(buffer + 2) == 0x0) &&
							  !(*buffer == 0xff && *(buffer + 1) == 0xff && *(buffer + 2) == 0xff) &&
							  !(*buffer == *(buffer + 1))) ||
							*buffer == *(buffer + 2)))
						{
							*ziel++ = *buffer++;
							x++;
							count++;
						} else
							break;
					}
					*(ziel - count - 1) = count;
					complenp += 2 + count;
					break;
				}
			} while (x < w);
			if (complenp > uncomplen)
			{
				ziel -= complenp;
				buffer -= w;
				left = w;
				k = 0;
				do
				{
					if (left >= 255)
						run = 255;
					else
						run = left;
					*ziel++ = 0x80;
					*ziel++ = run;
					memcpy(ziel, buffer, run);
					ziel += run;
					buffer += run;
					complen += run + 2;
					left -= run;
				} while (++k < strings);
			} else
			{
				complen += complenp;
			}

			complenp = 0;
		}
	} while (++y < height);

	return complen;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			GEM-(X)IMG Komprimierer (IMG)			*/
/*		1-8 , 24 Bit, eigene Komprimierungen		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t pad;
	uint8_t BitsPerPixel;
	uint8_t Planes;
	uint8_t PattLength;
	WORD t;

	uint16_t *ppal;
	unsigned short k;
	unsigned short width, height;
	unsigned short strings;
	unsigned short Header;
	unsigned short img_headsize;
	unsigned short ximg_headsize;
	unsigned short Palette;
	unsigned short cols;
	unsigned long w, memwidth;
	unsigned long f_len;

	static WINDOW window;
	static OBJECT *win_form;

	typedef struct
	{
		uint16_t version;
		uint16_t headsize;
		uint16_t BitsPerPixel;
		uint16_t patlen;
		uint16_t pixwidth;
		uint16_t pixheight;
		uint16_t width;
		uint16_t height;
	} IMG_HEAD;

	typedef enum
	{
		RGB_,
		CMY_,
		HLS_,
		PANTONE_
	} XIMG_COLMODEL;

	typedef struct
	{
		unsigned long imgtype;
		XIMG_COLMODEL colmodel;
	} XIMG_HEAD;

	typedef struct
	{
		uint8_t format;					/* IMG oder XIMG */
		uint8_t mode;					/* GVW oder Pixart */
	} CONFIG;

	CONFIG **pp;

	IMG_HEAD *img_header;
	XIMG_HEAD *ximg_header;
	static CONFIG config;


	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* falls Åbergeben, Konfig Åbernehmen */
		pp = (CONFIG **) &smurf_struct->event_par[0];
		if (*pp != NULL)
		{
			config = **pp;
		} else
		{
			config.format = XIMG;
			config.mode = GVW;
		}

		win_form = rs_trindex[XIMG_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		smurf_struct->module_mode = M_WAITING;
		break;

	case MMORE:
		/* Ressource aktualisieren */
		if (config.mode == GVW)
		{
			win_form[GVW].ob_state |= OS_SELECTED;
			win_form[PIXA].ob_state &= ~OS_SELECTED;
		} else
		{
			win_form[GVW].ob_state &= ~OS_SELECTED;
			win_form[PIXA].ob_state |= OS_SELECTED;
		}

		if (config.format == XIMG)
			win_form[FORMAT].ob_state |= OS_SELECTED;
		else
			win_form[FORMAT].ob_state &= ~OS_SELECTED;

		window.whandlem = 0;			/* evtl. Handle lîschen */
		window.module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X-...        */
		window.wy = -1;					/* ...und Y-Pos         */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterhîhe          */
		strcpy(window.wtitle, "GEM-(X)IMG Exporter");		/* Titel reinkopieren   */
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
		pp = (CONFIG **) &smurf_struct->event_par[0];
		if (*pp != NULL)
		{
			memcpy(&config, *pp, sizeof(CONFIG));
		} else
		{
			config.format = XIMG;
			config.mode = GVW;
		}

		smurf_struct->module_mode = M_WAITING;
		break;

		/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			/* Konfig Åbergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);
			smurf_struct->module_mode = M_MOREOK;
			break;
		case SAVE:
			/* Konfig Åbergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);
			smurf_struct->module_mode = M_CONFSAVE;
			break;
		case FORMAT:
			config.format ^= 1;		/* Format togglen */
			smurf_struct->module_mode = M_WAITING;
			break;
		case GVW:
		case PIXA:
			config.mode = smurf_struct->event_par[0];
			smurf_struct->module_mode = M_WAITING;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

		/* Keyboardevent */
	case MKEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			smurf_struct->module_mode = M_MOREOK;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		smurf_struct->event_par[0] = RGB;
		smurf_struct->module_mode = M_COLSYS;
		break;

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		SMalloc = smurf_struct->services->SMalloc;

		img_headsize = (unsigned short) sizeof(IMG_HEAD);

		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));

		/* allgemeine Variablen fÅllen */
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		BitsPerPixel = smurf_struct->smurf_pic->depth;
		if (BitsPerPixel == 24 && config.mode == GVW)
		{
			Planes = 1;
			PattLength = 3;
		} else
		{
			Planes = BitsPerPixel;
			PattLength = 1;
		}

		Header = img_headsize;
		if (config.format == XIMG && BitsPerPixel != 1)
		{
			if (BitsPerPixel == 24)
				Palette = 0;
			else
				Palette = (1 << BitsPerPixel) * 6;
			ximg_headsize = (unsigned short) sizeof(XIMG_HEAD);
			Header += (unsigned short) (ximg_headsize + Palette);
		} else
		{
			ximg_headsize = 0;
			Palette = 0;
		}

		if (BitsPerPixel == 24)
		{
			w = (unsigned long) width * 3;

			memwidth = (unsigned long) width;
			pad = (((width + 15) / 16) * 16 - width) * 3;
		} else
		{
			w = (width + 7) / 8;		/* Auf volle Byte gerundete ZeilenlÑnge in Byte */
			memwidth = w * 8;
			pad = 0;
		}

		strings = (unsigned short) (w / 255 + 1);

		/* Daten + Bit Strings im worst case + 1 Padbyte gepackt als Solid Run */
		f_len = ((memwidth * (long) height * BitsPerPixel) >> 3) + ((unsigned long) strings * 2L + 1) * (long) height * (long) Planes;

		if ((ziel = (uint8_t *) SMalloc(Header + f_len)) == 0)
		{
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		} else
		{
			img_header = (IMG_HEAD *) ziel;

			/* Header fÅllen */
			img_header->version = 1;
			img_header->headsize = Header >> 1;
			img_header->BitsPerPixel = BitsPerPixel;
			img_header->patlen = PattLength;
			img_header->pixwidth = 0x174;
			img_header->pixheight = 0x174;
			img_header->width = width;
			img_header->height = height;

			/* erweiterten Header fÅllen */
			if (ximg_headsize != 0)
			{
				ximg_header = (XIMG_HEAD *) (ziel + img_headsize);
				ximg_header->imgtype = 0x58494d47L; /* 'XIMG' */
				ximg_header->colmodel = RGB_;
			}

			strcpy(smurf_struct->smurf_pic->format_name, "GEM-(X)IMG .IMG");

			/* Farbpalette fÅllen */
			if (Palette != 0)
			{
				ppal = (uint16_t *) (ziel + img_headsize + ximg_headsize);

				pal = smurf_struct->smurf_pic->palette;
				cols = 1 << BitsPerPixel;
				for (k = 0; k < cols; k++)
				{
					*ppal++ = (uint16_t) (((unsigned long) *pal++ * 1000L) / 255);
					*ppal++ = (uint16_t) (((unsigned long) *pal++ * 1000L) / 255);
					*ppal++ = (uint16_t) (((unsigned long) *pal++ * 1000L) / 255);
				}
			}

			f_len = Header;

			if (BitsPerPixel == 24 && config.mode == GVW)
				f_len += write_RGB(buffer, ziel + Header, w, pad, height, strings);
			else
				f_len += write_Plane(buffer, ziel + Header, w, height, BitsPerPixel, strings);

			_Mshrink(ziel, f_len);
			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
		}

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
