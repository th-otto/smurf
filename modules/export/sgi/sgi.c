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
/*						SGI Image File Encoder				*/
/* Version 0.1  --  03.03.97								*/
/*	  8, 24 Bit												*/
/* Version 0.2  --  03.05.97								*/
/*	  Eigenes Konfigurationsformular eingebaut.				*/
/* Version 0.3  --  17.10.98								*/
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
#include "de/sgi.rsh"
#elif COUNTRY==0
#include "en/sgi.rsh"
#elif COUNTRY==2
#include "en/sgi.rsh"				/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define	TextCast	ob_spec.tedinfo->te_ptext


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"SGI Image File",
	0x0030,
	"Christian Eyrich",
	{ "SGI", "RLE", "", "", "", "", "", "", "", "" },
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
	M_MORE | M_INFO									/* More + Infotext */
};


/* Kodiert Bilder mit einem und drei Bytes pro Pixel */
static unsigned long write_1bpc(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height, uint8_t Planes)
{
	uint8_t *obuffer;
	uint8_t p;
	unsigned short x, y;
	unsigned long len;

	obuffer = buffer;
	ziel += 0x200;

	len = 0;

	p = 0;
	do
	{
		buffer = obuffer + p;

		y = height;
		while (y--)						/* y absteigend, damit die Tabelle von unten her geschrieben wird - */
		{								/* so wird ein flip bei komprimierten Bildern ÅberflÅssig */
			x = 0;
			do							/* width */
			{
				x++;
				*ziel++ = *buffer;
				buffer += Planes;

			} while (x < width);		/* x */

			len += width;
		}								/* y */
	} while (++p < Planes);

	return len;
}


/* Kodiert Bilder mit einem und drei Bytes pro Pixel */
static unsigned long encode_1bpc(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height, unsigned long tablen, uint8_t Planes)
{
	uint8_t *zielo;
	uint8_t *obuffer;
	uint8_t pixel;
	uint8_t p;
	uint8_t counter;
	unsigned short x, xx, y;

	unsigned long *starttab;
	unsigned long *lengthtab;
	unsigned long len;
	unsigned long llen;
	unsigned long tablen3;

	obuffer = buffer;
	zielo = ziel;
	ziel += 0x200;

	starttab = (unsigned long *) ziel;
	lengthtab = starttab + (height * Planes);
	ziel += tablen;

	llen = len = 0;

	p = 0;
	do
	{
		tablen3 = height * p;

		buffer = obuffer + p;

		y = height;
		while (y--)						/* y absteigend, damit die Tabelle von unten her geschrieben wird - */
		{								/* so wird ein flip bei komprimierten Bildern ÅberflÅssig */
			starttab[tablen3 + y] = (unsigned long) (ziel - zielo);

			x = 0;
			do							/* width */
			{
				counter = 1;
				pixel = *buffer;
				buffer += Planes;
				x++;

				xx = x;
				while (*buffer == pixel && counter < 0x7f && xx < width)
				{
					buffer += Planes;
					xx++;
					counter++;
				}

				if (counter > 1)		/* es lohnt sich ... */
				{
					*ziel++ = counter;
					*ziel++ = pixel;
					len += 2;
				} else					/* ... leider nicht! */
				{
					ziel++;
					*ziel++ = pixel;
					while ((*buffer != *(buffer + Planes) || *buffer != *(buffer + Planes + Planes)) && counter < 0x7f
						   && xx < width)
					{
						*ziel++ = *buffer;
						buffer += Planes;
						counter++;
						xx++;
					}
					*(ziel - counter - 1) = 0x80 | counter;
					len += counter + 1;
				}
				x = xx;
			} while (x < width);		/* x */

			lengthtab[tablen3 + y] = len - llen;
			llen = len;
		}								/* y */
	} while (++p < Planes);

	*ziel++ = 0;						/* Zuletzt Null-Run */
	len++;
	len += tablen;

	return len;
}


/* éndert die Orientierung der Bilder */
static short switch_orient(uint8_t *buffer, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *buffero;
	uint8_t *bufferu;
	uint8_t *temp;
	unsigned short t;

	unsigned long w;
	unsigned long len;
	unsigned long planelength;

	w = (unsigned long) width;

	len = (w * BitsPerPixel) >> 3;
	planelength = len * height;

	if ((temp = (uint8_t *)Malloc(len)) == 0)
		return -1;

	buffero = buffer;
	bufferu = buffero + planelength - len;

	t = height >> 1;
	while (t--)
	{
		memcpy(temp, buffero, len);
		memcpy(buffero, bufferu, len);
		memcpy(bufferu, temp, len);

		buffero += len;
		bufferu -= len;
	}

	Mfree(temp);

	return 0;
}


/* SGI-Headerstruktur schreiben */
static void write_header(uint8_t *ziel, uint8_t *comment, unsigned short width, unsigned short height, uint8_t comp, uint8_t ColorMap, uint8_t Planes, uint8_t Dim)
{
	*((uint16_t *) ziel) = 0x01da;	/* Magic */
	ziel += 2;
	
	if (comp == FALSE)
		*ziel++ = 0;					/* Compression */
	else
		*ziel++ = 1;					/* Compression */

	/* 2 Bpc wird es nicht geben, da es keineswegs 16 Bit-Bilder sind, sondern weiterhin */
	/* Bilder mit drei KanÑlen, jedoch 16 Bit Genauigkeit pro RGB-Kanal ... */
	*ziel++ = 1;						/* Bpc */
	*((uint16_t *) ziel) = Dim;		/* Dimension */
	ziel += 2;
	
	*((uint16_t *) ziel) = width;	/* XSize */
	ziel += 2;
	*((uint16_t *) ziel) = height;	/* YSize */
	ziel += 2;
	*((uint16_t *) ziel) = Planes;	/* ZSize */
	ziel += 2;

	*((uint32_t *) ziel) = 0x0;	/* PixMin */
	ziel += 4;
	*((uint32_t *) ziel) = 0xff;	/* PixMax */
	ziel += 4;

	*((uint32_t *) ziel) = 0;		/* Dummy */
	ziel += 4;

	strncpy(ziel, comment, 79);			/* Image Name */
	ziel[79] = '\0';
	ziel += 80;

	*((uint32_t *) ziel) = ColorMap;	/* ColorMap */
	ziel += 4;
	memset(ziel, 0, 404);				/* Not used */
	ziel += 404;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				SGI Image Format (SGI)				*/
/*		8, 24 Bit, unkomprimiert, RLE				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t BitsPerPixel;
	uint8_t Planes;
	uint8_t Dim;
	uint8_t ColorMap;
	WORD t;
	unsigned short width, height;
	unsigned short headsize;

	unsigned long tablen, f_len;

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

		win_form = rs_trindex[SGI_EXPORT];	/* Resourcebaum holen */

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
		strcpy(window.wtitle, rs_frstr[WINDOW_TITLE]);	/* Titel reinkopieren   */
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
		if (smurf_struct->smurf_pic->depth == 8)
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

		/* 1 = BW, ditherd, colormapped; 3 = RGB; 4 = RGB + alpha channel */
		if (BitsPerPixel == 24)
			Planes = 3;
		else
			Planes = 1;

		if (BitsPerPixel == 24)
			Dim = 3;
		else
			Dim = 2;

		/* 0x00 - Normal pixel values: 1 Channel Greyscale, 3 Channels RGB */
		/* 0x01 - Dithered Images: (8 BPP 3+3+2) - obsolete */
		/* 0x02 - single Channel Images - obsolete */
		/* 0x03 - color Map */
		ColorMap = 0;

		headsize = 0x200;
		f_len = ((long) width * (long) height * Planes * BitsPerPixel) >> 3;

		if (config.comp)
			tablen = (long) height * (long) Planes * 4 * 2;
		else
			tablen = 0;

		if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(headsize + f_len + tablen)) == 0)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;

			write_header(ziel, smurf_struct->smurf_pic->infotext, width, height, config.comp, ColorMap, Planes, Dim);

			if (config.comp == FALSE)
			{
				if (switch_orient(buffer, width, height, BitsPerPixel) != 0)
					smurf_struct->services->f_alert(rs_frstr[ORIENTCORR], NULL, NULL, NULL, 1);
				f_len = write_1bpc(oziel, buffer, width, height, Planes);
			} else
			{
				f_len = encode_1bpc(oziel, buffer, width, height, tablen, Planes);
			}
			ziel = oziel;

			exp_pic->pic_data = ziel;
			exp_pic->f_len = headsize + f_len;
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
