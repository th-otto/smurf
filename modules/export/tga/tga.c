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
/*						TGA-Format Exporter					*/
/*	Version 0.1 - xx.xx.96									*/
/*	  unkomprimierte TGAs									*/
/*	Version 0.2 - xx.xx.96									*/
/*	  nun auch komprimierte TGAs							*/
/*	Version 0.3 - 09.07.96									*/
/*	  Von Dale Åbernommen und Åberarbeitet.					*/
/*	  Komprimierte TGAs kînnen nun auch von anderen			*/
/*	  Programmen gelesen werden.							*/
/*	Version 0.4 - 12.07.96									*/
/*	  Verbesserte RLE-Komprimierung - unkomprimierbare		*/
/*	  Folgen Åber mehrere Pixel werden zusammengefasst.		*/
/*	Version 0.5 - 03.05.97									*/
/*	  Nochmal umgestrickt und auf meine Programmiernorm		*/
/*	  umgestellt. Unterroutinen in Funktionen gepackt.		*/
/*	  Eigenes Konfigurationsformular eingebaut.				*/
/* Version 0.6  --  12.12.97								*/
/*	  Monochrome TGA werden von anderen Programmen nun		*/
/*	  nicht mehr invertiert angezeigt.						*/
/* Version 0.7  --  17.10.98								*/
/*	  Fehler in Bedienung behoben. Bei Verlassen per Return	*/
/*	  wurde der Dialog zwar geschlossen, die Config aber	*/
/*	  nicht gespeichert.									*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY==1
#include "de/tga.rsh"
#elif COUNTRY==0
#include "en/tga.rsh"
#elif COUNTRY==2
#include "en/tga.rsh"					/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define TIMER 0


/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
#ifdef __PUREC__
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/*	Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"TGA Bitmap",
	0x0070,
	"Christian Eyrich, Dale Russell",
	{ "TGA", "", "", "", "", "", "", "", "", "" },
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
	1, 8, 16, 24, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	M_MORE | M_INFO									/* More und Infotext */
};





/* TGA-Headerstruktur schreiben */
static void write_header(uint8_t *ziel, char *comment, unsigned short width, unsigned short height, uint8_t comp, uint8_t BitsPerPixel)
{
	ziel[0] = strlen(comment) + 1;		/* LÑnge des Kommentarfeldes */
	ziel[2] = 0x0;

	if (comp)
		ziel[2] |= 0x08;				/* Komprimierung */

	if (BitsPerPixel == 4 || BitsPerPixel == 8)
	{
		ziel[1] = 1;					/* mit Farbpalette */
		ziel[2] |= 0x01;				/* Palettenbild */
		ziel[3] = 0;					/* Index des 1. Paletteneintr. */
		ziel[4] = 0;					/* im Intelformat */

		if (BitsPerPixel == 4)
		{
			ziel[5] = 0x30;				/* Anzahl PaletteneintrÑge */
			ziel[6] = 0x00;				/* im Intel-Format; 48 */
		} else
		{
			ziel[5] = 0x00;				/* Anzahl PaletteneintrÑge */
			ziel[6] = 0x01;				/* im Intel-Format; 256 */
		}

		ziel[7] = 24;					/* Bits pro Paletteneintrag */
	} else
	{
		ziel[1] = 0;					/* ohne Farbpalette */

		if (BitsPerPixel == 1)
			ziel[2] |= 0x03;			/* s/w-Bild */
		else
			ziel[2] |= 0x02;			/* RGB-Bild */

		ziel[3] = 0;					/* Index des 1. Paletteneintr. */
		ziel[4] = 0;					/* im Intelformat */
		ziel[5] = 0;					/* Anzahl PaletteneintrÑge */
		ziel[6] = 0;					/* im Intel-Format */
		ziel[7] = 0;					/* Bits pro Paletteneintrag */
	}

	ziel[8] = 0;						/* x-pos im */
	ziel[9] = 0;						/* Intel-Format */
	ziel[0x0a] = 0;						/* y-pos im */
	ziel[0x0b] = 0;						/* Intel-Format */
	ziel[0x0c] = (width & 0x00ff);		/* width im */
	ziel[0x0d] = (width & 0xff00) >> 8;	/* Intel-Format */
	ziel[0x0e] = (height & 0x00ff);		/* height im */
	ziel[0x0f] = (height & 0xff00) >> 8;	/* Intel-Format */
	ziel[0x10] = BitsPerPixel;			/* Farbtiefe */

	ziel[0x11] = 0x20;					/* Bildattribute: Aufbau */
	/* von oben nach unten */
	strcpy((char *)ziel + 0x12, comment);		/* Kommentar */
}


/* leider wird eine eigene Funktion benîtigt weil monochrome Bilder */
/* invertiert werden mÅssen */
static unsigned long write_1Bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	uint8_t v;
	unsigned short x, y;
	unsigned long w;

	w = (width + 7) / 8;
	v = (((width + 15) / 16) * 2 - w);

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = ~*buffer++;
		} while (++x < w);

		if (v)
			*ziel++ = 0xff;
	} while (++y < height);

	return 0;
}


static unsigned long write_8Bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	unsigned short x, y;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
		} while (++x < width);
	} while (++y < height);

	return 0;
}


static unsigned long write_16Bit(uint16_t *buffer, uint16_t *ziel, unsigned short width, unsigned short height)
{
	unsigned short x, y;
	uint16_t pixel16;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	y = 0;
	do
	{
		x = 0;
		do
		{
			pixel16 = *buffer++;
			/* Vorsicht: rrrrrggg|ggabbbbb nach arrrrrgg|gggbbbbb und dann noch nach Intel ... */
			*ziel++ = swap_word(((pixel16 & 0xffc0) >> 1) | (pixel16 & 0x001f));
		} while (++x < width);
	} while (++y < height);

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu\n", get_timer());
	(void)Cnecin();
#endif

	return 0;
}


static unsigned long write_24Bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	unsigned short x, y;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *(buffer++ + 2);
			*ziel++ = *buffer++;
			*ziel++ = *(buffer++ - 2);
		} while (++x < width);
	} while (++y < height);

	return 0;
}


/* leider wird eine eigene Funktion benîtigt weil monochrome Bilder */
/* invertiert werden mÅssen */
static unsigned long encode_1Bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	uint8_t pixel, v;
	unsigned short x, xx, y, counter;
	unsigned long w, dlen;

	w = (width + 7) / 8;
	v = (((width + 15) / 16) * 2 - w);

	dlen = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			counter = 0x80;
			pixel = *buffer++;
			x++;

			xx = x;
			while (*buffer == pixel && counter < 0xff && xx < w)
			{
				buffer++;
				xx++;
				counter++;
			}

			if (counter > 0x80)			/* es lohnt sich! */
			{
				*ziel++ = counter;
				*ziel++ = ~pixel;
				x = xx;
				dlen += 2;
			} else						/* ... aber leider nicht! */
			{
				ziel++;
				*ziel++ = ~pixel;
				counter = 1;
				while (*buffer != *(buffer + 1) && counter < 0x80 && xx < w)
				{
					*ziel++ = ~*buffer++;
					xx++;
					counter++;
				}
				*(ziel - counter - 1) = counter - 1;
				x = xx;
				dlen += counter + 1;
			}
		} while (x < w);
		/* Primitivlîsung, einfach mal unkomprimiert hinhÑngen */
		if (v)
		{
			*ziel++ = 0;
			*ziel++ = 0xff;
			dlen += 2;
		}
	} while (++y < height);

	return dlen;
}


static unsigned long encode_8Bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	uint8_t pixel;
	unsigned short x, xx, y, counter;
	unsigned long dlen;

	dlen = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			counter = 0x80;
			pixel = *buffer++;
			x++;

			xx = x;
			while (*buffer == pixel && counter < 0xff && xx < width)
			{
				buffer++;
				xx++;
				counter++;
			}

			if (counter > 0x80)			/* es lohnt sich! */
			{
				*ziel++ = counter;
				*ziel++ = pixel;
				x = xx;
				dlen += 2;
			} else						/* ... aber leider nicht! */
			{
				ziel++;
				*ziel++ = pixel;
				counter = 1;
				while (*buffer != *(buffer + 1) && counter < 0x80 && xx < width)
				{
					*ziel++ = *buffer++;
					xx++;
					counter++;
				}
				*(ziel - counter - 1) = counter - 1;
				x = xx;
				dlen += counter + 1;
			}
		} while (x < width);
	} while (++y < height);

	return dlen;
}


static unsigned long encode_16Bit(uint16_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	uint16_t pixel16;
	unsigned short x, xx, y, counter;
	unsigned long dlen;

	dlen = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			counter = 0x80;
			pixel16 = *buffer++;
			x++;

			xx = x;
			while (*buffer == pixel16 && counter < 0xff && xx < width)
			{
				buffer++;
				xx++;
				counter++;
			}

			if (counter > 0x80)			/* es lohnt sich! */
			{
				*ziel++ = counter;
				/* Vorsicht: rrrrrggg|ggabbbbb nach arrrrrgg|gggbbbbb und dann noch nach Intel ... */
				pixel16 = ((pixel16 & 0xffc0) >> 1) | (pixel16 & 0x001f);
				*ziel++ = pixel16;
				*ziel++ = pixel16 >> 8;
				x = xx;
				dlen += 3;
			} else						/* ... aber leider nicht! */
			{
				*ziel++ = 0;
				/* Vorsicht: rrrrrggg|ggabbbbb nach arrrrrgg|gggbbbbb und dann noch nach Intel ... */
				pixel16 = ((pixel16 & 0xffc0) >> 1) | (pixel16 & 0x001f);
				*ziel++ = pixel16;
				*ziel++ = pixel16 >> 8;
				counter = 1;
				while (*buffer != *(buffer + 1) && counter < 0x80 && xx < width)
				{
					/* Vorsicht: rrrrrggg|ggabbbbb nach arrrrrgg|gggbbbbb und dann noch nach Intel ... */
					pixel16 = ((*buffer & 0xffc0) >> 1) | (*buffer++ & 0x001f);
					*ziel++ = pixel16;
					*ziel++ = pixel16 >> 8;
					xx++;
					counter++;
				}
				*(ziel - (counter * 2) - 1) = counter - 1;
				x = xx;
				dlen += (counter * 2) + 1;
			}
		} while (x < width);
	} while (++y < height);

	return dlen;
}


static unsigned long encode_24Bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height)
{
	uint8_t pixel[3];
	unsigned short x, xx, y, counter;
	unsigned long dlen;

	dlen = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			counter = 0x80;
			pixel[0] = *buffer++;
			pixel[1] = *buffer++;
			pixel[2] = *buffer++;
			x++;

			xx = x;
			while (counter < 0xff && xx < width)
			{
				if (*buffer != pixel[0] || *(buffer + 1) != pixel[1] || *(buffer + 2) != pixel[2])
					break;
				buffer += 3;
				xx++;
				counter++;
			}

			if (counter > 0x80)			/* es lohnt sich! */
			{
				*ziel++ = counter;
				*ziel++ = pixel[2];
				*ziel++ = pixel[1];
				*ziel++ = pixel[0];
				x = xx;
				dlen += 4;
			} else						/* ... aber leider nicht! */
			{
				ziel++;
				*ziel++ = pixel[2];
				*ziel++ = pixel[1];
				*ziel++ = pixel[0];
				counter = 1;
				while (counter < 0x80 && xx < width)
				{
					if (*buffer != *(buffer + 3) || *(buffer + 1) != *(buffer + 4) || *(buffer + 2) != *(buffer + 5))
					{
						*ziel++ = *(buffer++ + 2);
						*ziel++ = *buffer++;
						*ziel++ = *(buffer++ - 2);
						xx++;
						counter++;
					} else
						break;
				}
				*(ziel - (counter * 3) - 1) = counter - 1;
				x = xx;
				dlen += (counter * 3) + 1;
			}
		} while (x < width);
	} while (++y < height);

	return dlen;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				Targa Bitmap - (TGA)				*/
/*		1, 8 und 24 Bit, unkomprimiert und RLE		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;

	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t *ppal;
	char *comment;
	uint8_t BitsPerPixel;
	WORD t;
	short headlen;
	short palcols;
	unsigned short width, height, w;
	unsigned long backlen;
	unsigned long len;

	typedef struct {
		uint8_t comp;
	} CONFIG;

	static WINDOW window;
	static OBJECT *win_form;
	static CONFIG config;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* falls Åbergeben, Konfig Åbernehmen */
		if (*((void **) &smurf_struct->event_par[0]) != 0)
			memcpy(&config, *((void **) &smurf_struct->event_par[0]), sizeof(CONFIG));
		else
			config.comp = TRUE;

		win_form = rs_trindex[TGA_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		smurf_struct->module_mode = M_WAITING;
		break;

	case MMORE:
		/* Ressource aktualisieren */
		if (config.comp == FALSE)
		{
			win_form[KEINE].ob_state |= OS_SELECTED;
			win_form[RLE].ob_state &= ~OS_SELECTED;
		} else
		{
			win_form[KEINE].ob_state &= ~OS_SELECTED;
			win_form[RLE].ob_state |= OS_SELECTED;
		}

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
			memcpy(&config, *((void **) &smurf_struct->event_par[0]), sizeof(config));
		else
			config.comp = TRUE;
		smurf_struct->module_mode = M_WAITING;
		break;

/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			/* Konfig Åbergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short)sizeof(config);
			smurf_struct->module_mode = M_MOREOK;
			break;
		case SAVE:
			/* Konfig Åbergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short)sizeof(config);
			smurf_struct->module_mode = M_CONFSAVE;
			break;
		case KEINE:
			config.comp = FALSE;
			smurf_struct->module_mode = M_WAITING;
			break;
		case RLE:
			config.comp = TRUE;
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
		smurf_struct->event_par[0] = RGB;
		smurf_struct->module_mode = M_COLSYS;
		break;

	case MEXEC:
		/* Vielleicht ist noch eine 256 Byte-Tabelle nîtig mit der monochrome
		   Bilder invertiert werden kînnen. Die bisher abgespeicherten werden
		   nÑmlich invertiert angezeigt. */

		/* Zeiger initialisieren */
		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}
		buffer = smurf_struct->smurf_pic->pic_data;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		switch (BitsPerPixel)
		{
		case 1:
			w = (width + 15) / 16;
			len = (long) w * 2L * (long) height;
			break;
		case 4:
			w = (width + 3) / 4;
			len = (long) w * 2L * (long) height;
			break;
		default:
			len = (((long) width * (long) height * (long) BitsPerPixel) >> 3L);
			break;
		}

		if (BitsPerPixel == 4 || BitsPerPixel == 8)
			palcols = 0x01 << BitsPerPixel;
		else
			palcols = 0;

		/* Hier wird die LÑnge des Kommentars ermittelt */
		/* Der Kommentar sollte lt. Buch nullterminiert sein! */
		comment = smurf_struct->smurf_pic->infotext;
		/* HeaderlÑnge (0x12) + KommentarlÑnge */
		headlen = 0x12 + (short) strlen(comment) + 1;
		/* len: BildlÑnge, palcols: Anzahl Palettenfarben */
		if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(((long) headlen + len + (long) palcols * 3) + 16384)) == NULL)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

		write_header(ziel, comment, width, height, config.comp, BitsPerPixel);

		if (BitsPerPixel > 1 && BitsPerPixel < 16)
		{
			pal = smurf_struct->smurf_pic->palette;
			ppal = ziel + headlen;
			for (palcols = 0; palcols < 256; palcols++)
			{
				*ppal++ = *(pal++ + 2);	/* B */
				*ppal++ = *pal++;		/* G */
				*ppal++ = *(pal++ - 2);	/* R */
			}

			headlen += 768;
		}

		exp_pic->pic_data = ziel;
		ziel += headlen;
		if (config.comp == FALSE)				/* unkomprimiert */
		{
			if (BitsPerPixel == 1)
				write_1Bit(buffer, ziel, width, height);
			else if (BitsPerPixel == 8)
				write_8Bit(buffer, ziel, width, height);
			else if (BitsPerPixel == 16)
				write_16Bit((uint16_t *) buffer, (uint16_t *) ziel, width, height);
			else /* if (BitsPerPixel == 24) */
				write_24Bit(buffer, ziel, width, height);

			backlen = len;
		} else if (BitsPerPixel == 1) /* komprimiert */
		{
			backlen = encode_1Bit(buffer, ziel, width, height);
		} else if (BitsPerPixel == 8)
		{
			backlen = encode_8Bit(buffer, ziel, width, height);
		} else if (BitsPerPixel == 16)
		{
			backlen = encode_16Bit((uint16_t *) buffer, ziel, width, height);
		} else /* if (BitsPerPixel == 24) */
		{
			backlen = encode_24Bit(buffer, ziel, width, height);
		}
		exp_pic->f_len = backlen + headlen;
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
