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
/*					Atari STAD Grafik (PAC)					*/
/* Version 0.1  --  18.11.98								*/
/*	  1 Bit, ID-, Pack- und Special-Byte noch fest			*/
/* Version 0.2  --  20.11.98								*/
/*	  ID-, Pack- und Special-Byte werden jetzt aufwendig	*/
/*	  mit Hilfe eines Histogramms ausgesucht. 				*/
/*	  Kommt jetzt auch mit Sourcebildern anderer Gr��en als	*/
/*	  640*400 zurecht.										*/
/* Version 0.3  --  22.11.98								*/
/*	  Jetzt auch vertikale (pm86) Kompression.				*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY==1
#include "de/pac.rsh"
#elif COUNTRY==0
#include "en/pac.rsh"
#elif COUNTRY==2
#include "en/pac.rsh"				/* missing french resource */
#else
#error "Keine Sprache!"
#endif


#define	DEBUG	0
#define TIMER 0

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"STAD-Grafik",
	0x0030,
	"Christian Eyrich",
	{ "PAC", "", "", "", "", "", "", "", "", "" },
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
	1, 0, 0, 0, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};


static uint8_t ID_Byte;
static uint8_t Pack_Byte;
static uint8_t Special_Byte;



static void write_header(uint8_t *ziel, uint8_t comp)
{
	if (comp == HOR)
		*(uint32_t *) ziel = 0x704d3835L; /* 'pM85' */
	else
		*(uint32_t *) ziel = 0x704d3836L; /* 'pM86' */

	*(ziel + 4) = ID_Byte;
	*(ziel + 5) = Pack_Byte;
	*(ziel + 6) = Special_Byte;
}



/* horizontale Kompression */
static unsigned long encode_pM85(uint8_t *buffer, uint8_t *ziel, unsigned short sheight, unsigned short sw)
{
	uint8_t counter;
	uint8_t pixel;
	unsigned short x, xx, y;
	unsigned short dw, dheight, minwidth, minheight;
	unsigned long len, dlen;

	dw = 80;
	dheight = 400;

	if (sw < dw)
		minwidth = sw;
	else
		minwidth = dw;

	if (sheight < dheight)
		minheight = sheight;
	else
		minheight = dheight;

	dlen = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			counter = 0;
			pixel = *buffer++;
			x++;

			xx = x;
			while (*buffer == pixel && counter < 0xff && xx < minwidth)
			{
				buffer++;
				xx++;
				counter++;
			}

			if (counter > 1 ||			/* es lohnt sich */
				pixel == ID_Byte || pixel == Special_Byte)	/* oder ID- oder Spezial-Byte mu� */
			{							/* direkt abgespeichert werden */
				if (pixel == Pack_Byte)
				{
					*ziel++ = ID_Byte;
					*ziel++ = counter;
					dlen += 2;
				} else
				{
					*ziel++ = Special_Byte;
					*ziel++ = pixel;
					*ziel++ = counter;
					dlen += 3;
				}

				x = xx;
			} else						/* lohnt sich leider nicht! */
			{
				do
				{
					*ziel++ = pixel;
					dlen++;
				} while (counter--);

				while ((*buffer != *(buffer + 1) || *buffer != *(buffer + 2)) && *buffer != ID_Byte
					   && *buffer != Special_Byte && xx < minwidth)
				{
					*ziel++ = *buffer++;
					xx++;
					dlen++;
				}

				x = xx;
			}
		} while (x < minwidth);

		if (sw < dw)					/* Quellbreite < Zielbreite -> Ziel mit Wei� auff�llen */
		{
			if (Pack_Byte == 0x0)
			{
				*ziel++ = ID_Byte;
				*ziel++ = dw - sw - 1;
				dlen += 2;
			} else
			{
				*ziel++ = Special_Byte;
				*ziel++ = 0x0;
				*ziel++ = dw - sw - 1;
				dlen += 3;
			}
		} else							/* Quellbreite >= Zielbreite -> �berz�hlige Quellpixel �bergehen */
		{
			buffer += sw - dw;
		}
	} while (++y < minheight);

	if (sheight < dheight)				/* Quellh�he < Zielh�he -> Ziel mit Wei� auff�llen */
	{
#if 0
		do
		{
			if (Pack_Byte == 0x0)
			{
				*ziel++ = ID_Byte;
				*ziel++ = 79;
				dlen += 2;
			}
			else
			{
				*ziel++ = Special_Byte;
				*ziel++ = 0x0;
				*ziel++ = 79;
				dlen += 3;
			}
		} while(++y < dheight);
#endif
		len = (long) (dheight - sheight) * 80L;

		do
		{
			if (Pack_Byte == 0x0)
			{
				*ziel++ = ID_Byte;
				*ziel++ = 255;
				dlen += 2;
			} else
			{
				*ziel++ = Special_Byte;
				*ziel++ = 0x0;
				*ziel++ = 255;
				dlen += 3;
			}

			len -= 256;
		} while (len >= 255);

		if (Pack_Byte == 0x0)
		{
			*ziel++ = ID_Byte;
			*ziel++ = len - 1;
			dlen += 2;
		} else
		{
			*ziel++ = Special_Byte;
			*ziel++ = 0x0;
			*ziel++ = len - 1;
			dlen += 3;
		}
	}


	return dlen;
}


/* vertikale Kompression */
static unsigned long encode_pM86(uint8_t *buffer, uint8_t *ziel, unsigned short sheight, unsigned short sw)
{
	uint8_t *obuffer;
	uint8_t counter;
	uint8_t pixel;
	unsigned short x, y, yy;
	unsigned short dw, dheight, minwidth, minheight;
	unsigned long len, dlen;

	dw = 80;
	dheight = 400;

	if (sw < dw)
		minwidth = sw;
	else
		minwidth = dw;

	if (sheight < dheight)
		minheight = sheight;
	else
		minheight = dheight;

	obuffer = buffer;

	dlen = 0;

	x = 0;
	do
	{
		buffer = obuffer + x;

		y = 0;
		do
		{
			counter = 0;
			pixel = *buffer;
			buffer += sw;
			y++;

			yy = y;
			while (*buffer == pixel && counter < 0xff && yy < minheight)
			{
				buffer += sw;
				yy++;
				counter++;
			}

			if (counter > 1 ||			/* es lohnt sich */
				pixel == ID_Byte || pixel == Special_Byte)	/* oder ID- oder Spezial-Byte mu� */
			{							/* direkt abgespeichert werden */
				if (pixel == Pack_Byte)
				{
					*ziel++ = ID_Byte;
					*ziel++ = counter;
					dlen += 2;
				} else
				{
					*ziel++ = Special_Byte;
					*ziel++ = pixel;
					*ziel++ = counter;
					dlen += 3;
				}

				y = yy;
			} else						/* lohnt sich leider nicht! */
			{
				do
				{
					*ziel++ = pixel;
					dlen++;
				} while (counter--);

				while ((*buffer != *(buffer + sw) || *buffer != *(buffer + sw + sw)) && *buffer != ID_Byte
					   && *buffer != Special_Byte && yy < minheight)
				{
					*ziel++ = *buffer;
					buffer += sw;
					yy++;
					dlen++;
				}

				y = yy;
			}
		} while (y < minheight);

		if (sheight < dheight)			/* Quellh�he < Zielh�he -> Ziel mit Wei� auff�llen */
		{
			if (Pack_Byte == 0x0)
			{
				*ziel++ = ID_Byte;
				*ziel++ = dheight - sheight - 1;
				dlen += 2;
			} else
			{
				*ziel++ = Special_Byte;
				*ziel++ = 0x0;
				*ziel++ = dheight - sheight - 1;
				dlen += 3;
			}
		}
	} while (++x < minwidth);

	if (sw < dw)						/* Quellbreite < Zielbreite -> Ziel mit Wei� auff�llen */
	{
		len = (long) (dw - sw) * 400L;

		do
		{
			if (Pack_Byte == 0x0)
			{
				*ziel++ = ID_Byte;
				*ziel++ = 255;
				dlen += 2;
			} else
			{
				*ziel++ = Special_Byte;
				*ziel++ = 0x0;
				*ziel++ = 255;
				dlen += 3;
			}

			len -= 256;
		} while (len >= 255);

		if (Pack_Byte == 0x0)
		{
			*ziel++ = ID_Byte;
			*ziel++ = len - 1;
			dlen += 2;
		} else
		{
			*ziel++ = Special_Byte;
			*ziel++ = 0x0;
			*ziel++ = len - 1;
			dlen += 3;
		}
	}


	return dlen;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				Atari STAD Grafik (PAC)				*/
/*		1 Bit, RLE in zwei Richtungen				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL * smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *oziel;
	WORD t;
	uint8_t highest, lowest;

	unsigned short sw, sheight;

	unsigned long histo[256];
	unsigned long headsize;
	unsigned long len;
	unsigned long f_len;
	unsigned long highestcount;
	unsigned long lowestcount;
	unsigned long x;

	typedef struct {
		uint8_t comp;
	} CONFIG;
	static WINDOW window;
	static OBJECT *win_form;
	static CONFIG config;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* falls �bergeben, Konfig �bernehmen */
		if (*((void **) &smurf_struct->event_par[0]) != 0)
			memcpy(&config, *((void **) &smurf_struct->event_par[0]), sizeof(CONFIG));
		else
			config.comp = VERT;

		win_form = rs_trindex[PAC_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		smurf_struct->module_mode = M_WAITING;

		break;

	case MMORE:
		/* Ressource aktualisieren */
		if (config.comp == VERT)
		{
			win_form[VERT].ob_state |= OS_SELECTED;
			win_form[HOR].ob_state &= ~OS_SELECTED;
		} else
		{
			win_form[VERT].ob_state &= ~OS_SELECTED;
			win_form[HOR].ob_state |= OS_SELECTED;
		}

		window.whandlem = 0;			/* evtl. Handle l�schen */
		window.module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X-...        */
		window.wy = -1;					/* ...und Y-Pos         */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterh�he          */
		strcpy(window.wtitle, "STAD Exporter");		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* n�chstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		if (smurf_struct->services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */

		break;

/* Closer geklickt, Default wieder her */
	case MMORECANC:
		/* falls �bergeben, Konfig �bernehmen */
		if (*((void **) &smurf_struct->event_par[0]) != 0)
			memcpy(&config, *((void **) &smurf_struct->event_par[0]), sizeof(config));
		else
			config.comp = VERT;
		smurf_struct->module_mode = M_WAITING;
		break;

/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			/* Konfig �bergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short)sizeof(config);

			smurf_struct->module_mode = M_MOREOK;
			break;
		case SAVE:
			/* Konfig �bergeben */
			*((void **) &smurf_struct->event_par[0]) = &config;
			smurf_struct->event_par[2] = (short)sizeof(config);

			smurf_struct->module_mode = M_CONFSAVE;
			break;
		case VERT:
		case HOR:
			config.comp = smurf_struct->event_par[0];
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
			/* Konfig �bergeben */
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
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		headsize = 7;

		sw = (smurf_struct->smurf_pic->pic_width + 7) / 8;
		sheight = smurf_struct->smurf_pic->pic_height;

		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(EXPORT_PIC));

		if (exp_pic == NULL || (ziel = (uint8_t *) smurf_struct->services->SMalloc(headsize + 32000L)) == NULL)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;

			f_len = 0;

			/* "Histogramm" aufbauen */
			for (t = 0; t < 256; t++)
				histo[t] = 0;
			len = (long) sw * (long) sheight;
			x = 0;
			do
			{
				histo[buffer[x]]++;
			} while (++x < len);

			if (sw < 80)				/* Quellbreite < Zielbreite -> Ziel mit Wei� auff�llen */
				histo[0] += (long) (80 - sw) * sheight;
			if (sheight < 400)			/* Quellh�he < Zielh�he -> Ziel mit Wei� auff�llen */
				histo[0] += (long) (400 - sheight) * 80;

			/* seltenstes und h�ufigestes Byte herausfinden */
			lowest = 0;
			highestcount = 0;
			lowestcount = len;
			x = 0;
			do
			{
				if (histo[x] >= highestcount)
				{
					highest = x;
					highestcount = histo[x];
				} else if (histo[x] <= lowestcount)
				{
					lowest = x;
					lowestcount = histo[x];
				}
			} while (++x < 256);

			Pack_Byte = highest;
			ID_Byte = lowest;

			/* zweitseltenstes herausfinden */
			lowest = 0;
			lowestcount = len;
			x = 0;
			do
			{
				if (histo[x] <= lowestcount && x != ID_Byte)
				{
					lowest = x;
					lowestcount = histo[t];
				}
			} while (++x < 256);

			Special_Byte = lowest;

			write_header(ziel, config.comp);
			ziel += headsize;

			if (config.comp == HOR)
				f_len = encode_pM85(buffer, ziel, sheight, sw);
			else
				f_len = encode_pM86(buffer, ziel, sheight, sw);

			ziel = oziel;

			smurf_struct->smurf_pic->pic_width = 640;
			smurf_struct->smurf_pic->pic_height = 400;

			f_len += headsize;
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
