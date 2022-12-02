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
/*			Portable Bitmap Utilities Formats Encoder		*/
/* Version 0.1  --  01.02.97								*/
/*	  1, 8 und 24 Bit binÑr									*/
/* Version 0.2  --  22.03.97								*/
/*	  24 Bit ASCII											*/
/* Version 0.3  --  26.03.97								*/
/*	  1, 8 Bit ASCII										*/
/* Version 0.4  --  05.04.97								*/
/*	  Eigenes Konfigurationsformular eingebaut.				*/
/* Version 0.5  --  27.04.97								*/
/*	  Konfig kann Åbermittelt und empfangen werden.			*/
/* Version 0.6  --  17.10.98								*/
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
#include "de/ppm.rsh"
#elif COUNTRY==0
#include "en/ppm.rsh"
#elif COUNTRY==2
#include "en/ppm.rsh"				/* missing french resource */
#else
#error "Keine Sprache!"
#endif


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"PBM Format",
	0x0060,
	"Christian Eyrich",
	{ "PPM", "PGM", "PBM", "", "", "", "", "", "", "" },
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
	1, 8, 24, 0, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	M_MORE									/* More */
};


static uint8_t *ziel;
static uint8_t *oziel;

static unsigned short x, width;

static unsigned long in;


/* Schreibt den PPM-Header */
static void write_header(uint8_t mode, unsigned short width, unsigned short height, unsigned long *f_len, uint8_t BitsPerPixel)
{
	char dummy[6];
	size_t slen;

	/* Header schreiben */
	*ziel++ = 'P';
	if (BitsPerPixel == 1)
	{
		if (mode == FALSE)
			*ziel++ = '1';
		else
			*ziel++ = '4';
	} else if (BitsPerPixel == 8)
	{
		if (mode == FALSE)
			*ziel++ = '2';
		else
			*ziel++ = '5';
	} else if (mode == FALSE)
	{
		*ziel++ = '3';
	} else
	{
		*ziel++ = '6';
	}
	*ziel++ = 0x0a;
	*f_len += 3;

	itoa(width, dummy, 10);
	slen = strlen(dummy);
	strncpy(ziel, dummy, slen);
	ziel += slen;
	*f_len += slen;

	*ziel++ = ' ';

	(*f_len)++;
	itoa(height, dummy, 10);
	strncpy(ziel, dummy, slen);
	slen = strlen(dummy);
	ziel += slen;
	*f_len += slen;

	if (BitsPerPixel != 1)
	{
		*ziel++ = 0x0a;
		(*f_len)++;
		strncpy(ziel, "255", 3);
		ziel += 3;
		*f_len += 3;
	}
	*ziel++ = 0x0a;
	(*f_len)++;
}


static void putval_1Bit(uint8_t val)
{
	uint8_t pixval;
	uint8_t bits;

	bits = 7;
	do
	{
		pixval = (val >> bits) & 0x01;

		*ziel++ = pixval + '0';
		*ziel++ = ' ';

		x++;
	} while (bits-- && x < width);

	in += 16;
}


static void putval(uint8_t val)
{
	uint8_t pixval;
	uint8_t addval;
	uint8_t prevnotspace = 0;

	if ((pixval = (val / 100)) != 0)
	{
		*ziel++ = pixval + '0';
		prevnotspace = 1;
	} else
	{
		*ziel++ = ' ';
	}

	addval = pixval * 100;

	if ((pixval = (val / 10 - pixval * 10)) != 0 || prevnotspace)
		*ziel++ = pixval + '0';
	else
		*ziel++ = ' ';

	addval += pixval * 10;

	*ziel++ = (val - addval) + '0';

	*ziel++ = ' ';

	in += 4;
}


/* Schreibt ein ASCII-Bild */
static void write_ASCIIPic(uint8_t *buffer, unsigned short width, unsigned short height, unsigned long w, uint8_t BitsPerPixel)
{
	unsigned short y;

	if (BitsPerPixel == 1)
	{
		y = 0;
		do
		{
			x = 0;
			do
			{
				in = 0;					/* maximale ZeilenlÑnge 4 Pixel */
				do
				{
					putval_1Bit(*buffer++);
				} while (x < width && in < 64);
				*ziel++ = '\r';
				*ziel++ = '\n';
			} while (x < width);
		} while (++y < height);
	} else
	{
		y = 0;
		do
		{
			x = 0;
			do
			{
				in = 0;					/* maximale ZeilenlÑnge 17 Pixel */
				do
				{
					putval(*buffer++);
				} while (++x < w && in < 68);
				*ziel++ = '\r';
				*ziel++ = '\n';
			} while (x < w);
		} while (++y < height);
	}
}


/* Schreibt ein binÑres Bild */
static void write_BINPic(uint8_t *buffer, unsigned short height, unsigned long w)
{
	unsigned short y;

	y = 0;
	do
	{
		memcpy(ziel, buffer, w);
		buffer += w;
		ziel += w;
	} while (++y < height);
}



/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			PBMPlus Picture Format (PPM)			*/
/*		1, 8, 24 Bit, unkomprimiert,				*/
/*		Speicherform binÑr und ASCII				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t BitsPerPixel;
	uint8_t zeichen;
	WORD t;
	unsigned short height;
	unsigned short headsize;
	unsigned short ret;
	unsigned long w, memwidth, f_len;

	typedef struct {
		uint8_t mode;
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
			config.mode = TRUE;

		win_form = rs_trindex[PPM_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		smurf_struct->module_mode = M_WAITING;
		break;

	case MMORE:
		/* Ressource aktualisieren */
		if (config.mode)
		{
			win_form[BIN].ob_state |= OS_SELECTED;
			win_form[ASCII].ob_state &= ~OS_SELECTED;
		} else
		{
			win_form[BIN].ob_state &= ~OS_SELECTED;
			win_form[ASCII].ob_state |= OS_SELECTED;
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
			config.mode = TRUE;

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
		case BIN:
			config.mode = TRUE;
			smurf_struct->module_mode = M_WAITING;
			break;
		case ASCII:
			config.mode = FALSE;
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

		/* Extender wird vom Smurf erfragt */
	case MEXTEND:
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		switch (BitsPerPixel)
		{
		case 1:
			smurf_struct->event_par[0] = 3;
			break;
		case 8:
			smurf_struct->event_par[0] = 2;
			break;
		case 24:
			smurf_struct->event_par[0] = 1;
			break;
		}
		smurf_struct->module_mode = M_EXTEND;
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		if (smurf_struct->smurf_pic->depth == 8)
			smurf_struct->event_par[0] = GREY;
		else
			smurf_struct->event_par[0] = RGB;

		smurf_struct->module_mode = M_COLSYS;

		break;

		/* Und losexportieren */
	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(*exp_pic));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (BitsPerPixel == 1)
		{
			w = (unsigned long) (width + 7) / 8;
			memwidth = w * 8;
		} else
		{
			if (BitsPerPixel == 24)
				w = (unsigned long) width * 3;
			else
				w = (unsigned long) width;
			memwidth = (unsigned long) width;
		}

		headsize = 3 + 6 + 6 + 4;

		if (config.mode == FALSE)				/* ASCII */
		{
			if (BitsPerPixel == 1)
			{
				/* 4 Werte passen in eine Zeile zu 70 Stellen */
				/* ret Returnzeichen zusÑtzlich werden benîtigt */
				ret = (((w + 3) / 4) * 2);
				zeichen = 2;

				f_len = ((unsigned long) zeichen * width + ret) * height;
			} else
			{
				/* 17 Werte passen in eine Zeile zu 70 Stellen */
				/* ret Returnzeichen zusÑtzlich werden benîtigt */
				ret = (((w + 16) / 17) * 2);
				zeichen = 4;

				f_len = ((unsigned long) zeichen * w + ret) * height;
			}
		} else							/* binÑr */
		{
			f_len = (memwidth * height * BitsPerPixel) >> 3;
		}

		if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(headsize + f_len)) == 0)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;

			write_header(config.mode, width, height, &f_len, BitsPerPixel);	/* Header schreiben */

			if (config.mode == FALSE)
				write_ASCIIPic(buffer, width, height, w, BitsPerPixel);
			else
				write_BINPic(buffer, height, w);

			ziel = oziel;

			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
		}								/* Malloc */

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
	}

	return NULL;
}


