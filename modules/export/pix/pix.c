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
/*				Degas (Elite)-Image Encoder					*/
/* Version 0.1  --  28.03.96								*/
/*	  Prototyp meiner Exporter, P?1 - P?3					*/
/* Version 0.2  --  29.07.97								*/
/*	  Nachdem die 0.1 nie lief, komplette šberarbeitung des	*/
/*	  Exporters.											*/
/* Version 0.3  --  17.10.98								*/
/*	  Fehler in Bedienung behoben. Bei Verlassen per Return	*/
/*	  wurde der Dialog zwar geschlossen, die Config aber	*/
/*	  nicht gespeichert.									*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#define TIMER 0

#if COUNTRY==1
#include "de/pix.rsh"
#elif COUNTRY==0
#include "en/pix.rsh"
#elif COUNTRY==2
#include "en/pix.rsh"					/* missing french resource */
#else
#error "Keine Sprache!"
#endif


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Degas Elite",
	0x0030,
	"Christian Eyrich",
	{ "PI1", "PI2", "PI3", "PC1", "PC2", "PC3", "", "", "", "" },
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
	1, 2, 4, 0, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	M_MORE									/* More */
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				Degas-(Elite) Komprimierer			*/
/*		1, 2, 4 Bit, keine und RLE					*/
/* Gr”žere und kleinere Bildern als Degas zul„žt	*/
/* werden selbst„ndig abgeschnitten und aufgeblasen.*/
/* Leider ist es beim Aufblasen nicht immer m”glich,*/
/* den Rand weiž zu machen, da Weiž weder auf Index */
/* 0 (siehe memset()) liegen, noch berhaupt in der */
/* Bildpalette vorhanden sein muž.					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *ppal;
	uint8_t Planes;
	uint8_t BitsPerPixel;
	uint8_t res;
	uint8_t p;
	WORD t;
	char expmessag[21];

	uint16_t *pal;
	unsigned short i;
	unsigned short n;
	unsigned short x;
	unsigned short y;
	unsigned short bv;
	unsigned short zv;
	unsigned short w;
	unsigned short v1;
	unsigned short v2;
	unsigned short width, height;
	unsigned short runheight;

	unsigned long src_pos = 0;
	unsigned long dst_pos = 0;
	unsigned long plh;
	unsigned long plo;
	unsigned long f_len;

	typedef struct
	{
		uint8_t comp;
		uint8_t res;
		uint16_t pal[16];
	} head;

	typedef struct {
		uint8_t comp;
	} CONFIG;
	CONFIG **pp;

	head *pix_header;

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
			config.comp = FALSE;

		win_form = rs_trindex[PIX_EXPORT];	/* Resourcebaum holen */

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
		window.picture = NULL;			/* kein Bild.           */
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

		/* Extender wird vom Smurf erfragt */
	case MEXTEND:
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		switch (BitsPerPixel)
		{
		case 1:
			smurf_struct->event_par[0] = 3;
			break;
		case 2:
			smurf_struct->event_par[0] = 2;
			break;
		case 4:
			smurf_struct->event_par[0] = 1;
			break;
		}

		if (config.comp)
			smurf_struct->event_par[0] += 3;

		smurf_struct->module_mode = M_EXTEND;
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
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
		obuffer = buffer;

		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(*exp_pic));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

		switch (smurf_struct->smurf_pic->depth)
		{
		case 1:
			width = 640;
			height = 400;
			Planes = 1;
			res = 2;
			break;
		case 2:
			width = 640;
			height = 200;
			Planes = 2;
			res = 1;
			break;
		case 4:
			width = 320;
			height = 200;
			Planes = 4;
			res = 0;
			break;
		default:
			smurf_struct->module_mode = M_INVALID;
			smurf_struct->services->SMfree(exp_pic);
			return NULL;
		}

		f_len = width / 8 * height * Planes;

		if ((ziel = smurf_struct->services->SMalloc(sizeof(head) + f_len)) == 0)
		{
			smurf_struct->module_mode = M_MEMORY;
			smurf_struct->services->SMfree(exp_pic);
			return NULL;
		}

		oziel = ziel;

		pix_header = (head *) ziel;

		if (config.comp == FALSE)
			pix_header->comp = 0x0;
		else
			pix_header->comp = 0x80;

		pix_header->res = res;

		strcpy(expmessag, "Degas ");
		if (config.comp == FALSE)
			strcat(expmessag, "PI?");
		else
			strcat(expmessag, "PC?");
		strcpy(smurf_struct->smurf_pic->format_name, expmessag);

		ziel += sizeof(head);

		if (config.comp == FALSE)			/* unkomprimiert */
		{
			w = (smurf_struct->smurf_pic->pic_width + 7) / 8;
			if (w > width / 8)
				w = width / 8;
			/* wieviel Bildrest berspringen? */
			if (smurf_struct->smurf_pic->pic_width > width)
				bv = (smurf_struct->smurf_pic->pic_width + 7) / 8 - width / 8;
			else
				bv = 0;
			/* wieviel Zielrest berspringen? */
			if (width > smurf_struct->smurf_pic->pic_width)
				zv = width / 8 - (smurf_struct->smurf_pic->pic_width + 7) / 8;
			else
				zv = 0;

			runheight = smurf_struct->smurf_pic->pic_height;
			if (runheight > height)
				runheight = height;

			p = 0;
			do						/* Planes */
			{
				ziel = oziel + sizeof(head) + (p << 1);	/* Zieladresse der kodierten Scanline */

				y = 0;
				do					/* height */
				{
					x = 0;
					do				/* width */
					{
						*ziel++ = *buffer++;
						x++;
						if (x < w)
						{
							*ziel++ = *buffer++;
							x++;
						}
						ziel += (Planes - 1) << 1;	/* Zieladresse der kodierten Scanline */
					} while (x < w);	/* x */
					buffer += bv;	/* Bild berspringen */
					ziel += zv * Planes;	/* Ziel berspringen */
				} while (++y < runheight);	/* y */
			} while (++p < Planes);	/* p */
		} else						/* komprimiert */
		{
			w = (smurf_struct->smurf_pic->pic_width + 7) / 8;
			plh = w * height;		/* H”he einer Plane */
			dst_pos = 0;			/* Zieladresse der dekodierten Scanline */

			y = 0;
			do						/* height */
			{
				plo = y * w;		/* Offset vom Planeanfang in Bytes */

				p = 0;
				do					/* Plane */
				{
					src_pos = p * plh + plo;
					x = 0;
					do				/* width */
					{
						v1 = buffer[src_pos++];
						if ((v1 & 0x80) == 0x80)
						{
							n = (0x101 - v1);
							v2 = buffer[src_pos++];
							for (i = 0; i < n; i++)
								ziel[dst_pos++] = v2;
							x += n;
						} else
						{
							for (i = 0; i < v1 + 1; i++)
								ziel[dst_pos++] = buffer[src_pos++];
							x += (v1 + 1);
						}
					} while (x < w);	/* x */
				} while (++p < Planes);	/* p */
			} while (++y < height);	/* y */
		}							/* comp? */

		buffer = obuffer;
		ziel = oziel;

		exp_pic->pic_data = ziel;
		exp_pic->f_len = sizeof(head) + f_len;

		pal = pix_header->pal;
		ppal = smurf_struct->smurf_pic->palette;

		for (i = 16; i > 0; i--)
		{
			*pal = (*ppal++ >> 5) << 8;
			*pal |= (*ppal++ >> 5) << 4;
			*pal++ |= *ppal++ >> 5;
		}

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu", get_timer());
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
