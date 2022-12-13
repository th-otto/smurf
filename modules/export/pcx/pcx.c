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
/*					PCX Paintbrush File Encoder				*/
/* Version 0.1  --  13.10.97 - 14.10.97						*/
/*	  1-8 (aužer 2 Bit), 24 Bit								*/
/*	  Speichert knapp hinter Graftool die am besten kompri-	*/
/*	  mierten PCX.											*/
/* Version 0.2  --  17.10.98								*/
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

#define TIMER 0

#if COUNTRY==1
#include "de/pcx.rsh"
#elif COUNTRY==0
#include "en/pcx.rsh"
#elif COUNTRY==2
#include "en/pcx.rsh"					/* missing french resource */
#else
#error "Keine Sprache!"
#endif



/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = { "PCX Paintbrush",
	0x0020,
	"Christian Eyrich",
	{ "PCX", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 3, 4, 8, 24,	0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	M_MORE								/* More */
};




/* PCX-Headerstruktur schreiben */
static void write_header(uint8_t *ziel, unsigned short width, unsigned short height, long w, uint8_t comp, uint8_t BitsPerPixel, uint8_t *pal)
{
	uint8_t *ppal;
	uint8_t version;
	short i, cols;

	memset(ziel, 0, 0x80);			/* Header initialisieren */

	*ziel++ = 0x0a;						/* Magic */

	/* Version */
	if (BitsPerPixel == 1)
		version = 0;					/* langt fr s/w */
	else if (BitsPerPixel > 1 && BitsPerPixel < 8)
		version = 2;
	else
		version = 5;

	*ziel++ = version;

	if (comp == FALSE)
		*ziel++ = 0;					/* Compression */
	else
		*ziel++ = 1;					/* Compression */

	/* BitsPerPixel per Plane */
	if (BitsPerPixel == 1 || BitsPerPixel == 3 || BitsPerPixel == 4)
		*ziel++ = 1;
	else if (BitsPerPixel == 2)
		*ziel++ = 2;
	else /* if (BitsPerPixel == 8 || BitsPerPixel == 24) */
		*ziel++ = 8;

	*ziel++ = 0;	/* XStart */
	*ziel++ = 0;
	*ziel++ = 0;	/* YStart */
	*ziel++ = 0;
	*ziel++ = width - 1;	/* XEnd */
	*ziel++ = (width - 1) >> 8;
	*ziel++ = height - 1;	/* YEnd */
	*ziel++ = (height - 1) >> 8;
	*ziel++ = width;	/* HRes */
	*ziel++ = width >> 8;
	*ziel++ = height;	/* VRes */
	*ziel++ = height >> 8;

	/* Palette */
	if (BitsPerPixel <= 4)
	{
		ppal = ziel;

		if (BitsPerPixel == 1)
		{
			ppal[0] = 255;
			ppal[1] = 255;
			ppal[2] = 255;
			ppal[3] = 0;
			ppal[4] = 0;
			ppal[5] = 0;
		} else
		{
			cols = 1 << BitsPerPixel;
			for (i = 0; i < cols; i++)
			{
				*ppal++ = *pal++;
				*ppal++ = *pal++;
				*ppal++ = *pal++;
			}
		}
	}

	ziel += 0x30;

	*ziel++ = 0x0;						/* Reserved1 */

	/* Anzahl Planes */
	if (BitsPerPixel == 1 || BitsPerPixel == 2 || BitsPerPixel == 8)
		*ziel++ = 1;
	else if (BitsPerPixel == 3 || BitsPerPixel == 4)
		*ziel++ = BitsPerPixel;
	else /* if (BitsPerPixel == 24) */
		*ziel++ = 3;

	*ziel++ = w;	/* Bytes per Line */
	*ziel++ = w >> 8;
	
	*ziel++ = 1;	/* Palette Type */
	*ziel++ = 0;
	
	if (version >= 5)
	{
		*ziel++ = width;	/* horizontal Screensize */
		*ziel++ = width >> 8;
		*ziel++ = height;	/* vertikal Screensize */
		*ziel++ = height >> 8;
	}
}




/* Muž wegen der Invertierung leider in eigene Funktion */
static long write_1Bit(uint8_t *ziel, uint8_t *buffer, unsigned short w, unsigned short height)
{
	unsigned short x, y;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = ~*buffer++;
		} while (++x < w);
	} while (++y < height);

	return (unsigned long) w * (unsigned long) height;
}


static long write_3and4Bit(uint8_t *ziel, uint8_t *buffer, unsigned short w, unsigned short height, uint8_t Planes)
{
	uint8_t *obuffer;
	uint8_t p;
	unsigned short x, y;
	unsigned long offset;
	unsigned long planelength;

	offset = w;
	planelength = (unsigned long) w * (unsigned long) height;

	obuffer = buffer;

	y = 0;
	do
	{
		p = 0;
		do
		{
			buffer = obuffer + p * planelength;

			x = 0;
			do
			{
				*ziel++ = *buffer++;
			} while (++x < w);
		} while (++p < Planes);
		obuffer += offset;
	} while (++y < height);

	return planelength * Planes;
}


static long write_8and24Bit(uint8_t *ziel, uint8_t *buffer, unsigned short w, unsigned short height, uint8_t Planes)
{
	uint8_t *obuffer;
	uint8_t p;
	unsigned short x, y;
	unsigned long offset;

	offset = (unsigned long) w * Planes;

	obuffer = buffer;

	y = 0;
	do
	{
		p = 0;
		do
		{
			buffer = obuffer + p;

			x = 0;
			do
			{
				*ziel++ = *buffer;
				buffer += Planes;
			} while (++x < w);
		} while (++p < Planes);
		obuffer += offset;
	} while (++y < height);

	return (unsigned long) w * (unsigned long) height * Planes;
}


/* Muž wegen der Invertierung leider in eigene Funktion */
static long encode_1Bit(uint8_t *ziel, uint8_t *buffer, unsigned short w, unsigned short height)
{
	uint8_t n;
	uint8_t pixel;
	unsigned short x, y;
	unsigned long len = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			n = 1;
			pixel = *buffer++;
			x++;
			while (*buffer == pixel && n < 0x3f && x < w)
			{
				buffer++;
				x++;
				n++;
			}

			pixel = ~pixel;
			if (n > 1 || (pixel & 0xc0) == 0xc0)
			{
				*ziel++ = (n | 0xc0);
				*ziel++ = pixel;
				len += 2;
			} else
			{
				*ziel++ = pixel;
				len++;
			}
		} while (x < w);
	} while (++y < height);

	return len;
}


static long encode_3and4Bit(uint8_t *ziel, uint8_t *buffer, unsigned short w, unsigned short height, uint8_t Planes)
{
	uint8_t *obuffer;
	uint8_t n;
	uint8_t p;
	uint8_t pixel;
	unsigned short x, y;
	unsigned long offset;
	unsigned long planelength;
	unsigned long len = 0;

	offset = w;
	planelength = (unsigned long) w * (unsigned long) height;

	obuffer = buffer;

	y = 0;
	do
	{
		p = 0;
		do
		{
			buffer = obuffer + p * planelength;

			x = 0;
			do
			{
				n = 1;
				pixel = *buffer++;
				x++;
				while (*buffer == pixel && n < 0x3f && x < w)
				{
					buffer++;
					x++;
					n++;
				}

				if (n > 1 || (pixel & 0xc0) == 0xc0)
				{
					*ziel++ = (n | 0xc0);
					*ziel++ = pixel;
					len += 2;
				} else
				{
					*ziel++ = pixel;
					len++;
				}
			} while (x < w);
		} while (++p < Planes);
		obuffer += offset;
	} while (++y < height);

	return len;
}


static long encode_8and24Bit(uint8_t *ziel, uint8_t *buffer, unsigned short w, unsigned short height, uint8_t Planes)
{
	uint8_t *obuffer;
	uint8_t n;
	uint8_t p;
	uint8_t pixel;
	unsigned short x, y;
	unsigned long offset;
	unsigned len = 0;

	offset = (unsigned long) w * Planes;

	obuffer = buffer;

	y = 0;
	do
	{
		p = 0;
		do
		{
			buffer = obuffer + p;

			x = 0;
			do
			{
				n = 1;
				pixel = *buffer;
				buffer += Planes;
				x++;
				while (*buffer == pixel && n < 0x3f && x < w)
				{
					buffer += Planes;
					x++;
					n++;
				}

				if (n > 1 || (pixel & 0xc0) == 0xc0)
				{
					*ziel++ = (n | 0xc0);
					*ziel++ = pixel;
					len += 2;
				} else
				{
					*ziel++ = pixel;
					len++;
				}
			} while (x < w);
		} while (++p < Planes);
		obuffer += offset;
	} while (++y < height);

	return len;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				PCX Image Format (PCX)				*/
/*		1-8, 24 Bit, unkomprimiert, RLE				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t *ppal;
	uint8_t BitsPerPixel;

	unsigned short width, height;
	unsigned short headsize;
	unsigned short pallen;
	unsigned short cols;
	unsigned short w;
	WORD t;

	unsigned long f_len, memwidth;

	typedef struct {
		uint8_t comp;
	} CONFIG;
	CONFIG **pp;
	
	static WINDOW window;
	static OBJECT *win_form;
	static CONFIG config;


	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* falls bergeben, Konfig bernehmen */
		pp = (CONFIG **) &smurf_struct->event_par[0];
		if (*pp != NULL)
			config = **pp;
		else
			config.comp = TRUE;

		win_form = rs_trindex[PCX_EXPORT];	/* Resourcebaum holen */

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

		window.whandlem = 0;			/* evtl. Handle l”schen */
		window.module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X-...        */
		window.wy = -1;					/* ...und Y-Pos         */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterh”he          */
		strcpy(window.wtitle, rs_frstr[WINDOW_TITLE]);		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild            */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* n„chstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		if (smurf_struct->services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */
		break;

	/* Closer geklickt, Default wieder her */
	case MMORECANC:
		/* falls bergeben, Konfig bernehmen */
		pp = (CONFIG **) &smurf_struct->event_par[0];
		if (*pp != NULL)
			config = **pp;
		else
			config.comp = TRUE;

		smurf_struct->module_mode = M_WAITING;
		break;

	/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			/* Konfig bergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short)sizeof(config);
			smurf_struct->module_mode = M_MOREOK;
			break;
		case SAVE:
			/* Konfig bergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
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
			/* Konfig bergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
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
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		headsize = 0x80;

		if (BitsPerPixel < 8)
		{
			w = (width + 7) / 8;
			memwidth = w * 8;
		} else
		{
			w = width;
			memwidth = width;
		}

		f_len = ((long) memwidth * (long) height * BitsPerPixel) >> 3;
		if (config.comp)
			f_len += f_len * 15 / 100;	/* plus 15% Sicherheitsreserve */

		if (BitsPerPixel == 8)
			pallen = 769;
		else
			pallen = 0;

		pal = smurf_struct->smurf_pic->palette;

		if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(headsize + f_len + pallen)) == 0)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			write_header(ziel, width, height, w, config.comp, BitsPerPixel, pal);

			if (config.comp == FALSE)
			{
				switch (BitsPerPixel)
				{
				case 1:
					f_len = write_1Bit(ziel + headsize, buffer, w, height);
					break;
#if 0
				case 2:
					f_len = write_2Bit(ziel, buffer);
					break;
#endif
				case 3:
				case 4:
					f_len = write_3and4Bit(ziel + headsize, buffer, w, height, BitsPerPixel);
					break;
				case 8:
				case 24:
					f_len = write_8and24Bit(ziel + headsize, buffer, w, height, BitsPerPixel >> 3);
					break;
				}
			} else
			{
				switch (BitsPerPixel)
				{
				case 1:
					f_len = encode_1Bit(ziel + headsize, buffer, w, height);
					break;
#if 0
				case 2:
					f_len = encode_2Bit(ziel, buffer);
					break;
#endif
				case 3:
				case 4:
					f_len = encode_3and4Bit(ziel + headsize, buffer, w, height, BitsPerPixel);
					break;
				case 8:
				case 24:
					f_len = encode_8and24Bit(ziel + headsize, buffer, w, height, BitsPerPixel >> 3);
					break;
				}
			}
		
			/* Palette */
			if (BitsPerPixel == 8)
			{
				ppal = ziel + headsize + f_len;
				*ppal++ = 0x0c;
				cols = 1 << BitsPerPixel;
				for (t = 0; t < cols; t++)
				{
					*ppal++ = *pal++;
					*ppal++ = *pal++;
					*ppal++ = *pal++;
				}
			}

			exp_pic->pic_data = ziel;
			exp_pic->f_len = headsize + f_len + pallen;
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
