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
/*						GIF-Format Exporter					*/
/*	Version 0.1 - 07.09.97									*/
/*	  Skelett wird geschrieben								*/
/*	Version 0.2 - 15.8.98									*/
/*	  GIF wird korrekt aber ohne Kompression geschrieben	*/
/*	Version 0.3 - 2.10.98-3.10.98							*/
/*	  LZW-Kompression eingebaut								*/
/*	Version 0.4 - 17.10.98									*/
/*	  LZW-Kompressor Åberarbeitet und etwas beschleunigt;	*/
/*	  Transparenz ermîglicht.								*/
/*	Version 0.5 - 22.10.98									*/
/*	  Verarbeitung von 1-7 Bit Bildern ist jetzt mîglich.	*/
/*	Version 0.6 - 28.10.2000								*/
/*	  Bilder kînnen auch interlaced gespeichert werden.		*/
/*	Version 0.7 - 4.11.2000									*/
/*	  Speicherplatzverbrauch reduziert (im Falle von Bildern*/
/*	  < 8 Bit sogar drastisch). Vorher war immer noch die	*/
/*	  Berechnung auf Grund des "Expansionsalgorithmus"		*/
/*	  (8 auf 9 Bit pro Pixel auch bei 1-7 Bit) zum Testen	*/
/*	  der ersten Version drinnen.							*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "gif_lzw.h"

#include "country.h"

#if COUNTRY==1
#include "de/gif.rsh"
#elif COUNTRY==0
#include "en/gif.rsh"
#elif COUNTRY==2
#include "en/gif.rsh"					/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define TIMER 0

#define _LSCRDES	 7					/* ist wegen des AuffÅllens der Strukturen */
#define _IMAGE_DES	10					/* von Compilerseite her leider nîtig */

typedef struct
{
	uint8_t typ;						/* 87a oder 89a */
	uint8_t interlace;					/* interlaced oder nicht */
	short transparent;					/* transparenter Index */
	uint8_t tindexok;					/* transparenter Index gÅltig */
} CONFIG;

void *(*SMalloc)(long amount);

static SERVICE_FUNCTIONS *services;


/*	Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"GIF",
	0x0070,
	"Christian Eyrich",
	{ "GIF", "", "", "", "", "", "", "", "", "" },
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
	1, 2, 3, 4, 5, 6, 7, 8,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_PIXELPAK,
	M_MORE | M_INFO						/* More + Infotext */
};


short *lacetab;


/*-------------------------------------------------------------	*/
/*							Header schreiben					*/
/*-------------------------------------------------------------	*/
static uint8_t *write_header(uint8_t *file, uint8_t typ)
{
	if (typ == GIF87A)
		strncpy((char *)file, "GIF87a", 6);
	else
		strncpy((char *)file, "GIF89a", 6);

	return file + 6;
}


/*-------------------------------------------------------------	*/
/*				Logical Screen Descriptor schreiben				*/
/*-------------------------------------------------------------	*/
static uint8_t *write_lscrdes(uint8_t *file, uint8_t *pal, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	unsigned short coltab_size = 0;

	file[0] = width;	/* Breite */
	file[1] = width >> 8;
	file[2] = height;	/* Hîhe */
	file[3] = height >> 8;
	/* Packed Fields: 0-2 = Bits pro Pixel - 1 */
	/*                3 = 0 Sortierung nach Farbe */
	/*                4-6 = Bits pro Farbe - 1 (immer 7) */
	/*                7 = 1 globale Farbtabelle vorhanden */
	file[4] = (1 << 7) | (7 << 4) | (0 << 3) | (BitsPerPixel - 1);
	file[5] = 0;					/* Hintergrundfarbe */
	file[6] = 0;					/* PixelseitenverhÑltnis */

	/* Global Color Table? */
	if (file[4] & 0x80)
	{
		coltab_size = 1 << BitsPerPixel;

		memcpy(file + _LSCRDES, pal, 3 * coltab_size);

#if DEBUG
		printf("\n  Size: %d", coltab_size);
#endif
	}

	return file + _LSCRDES + 3 * coltab_size;
}


/*-------------------------------------------------------------	*/
/*		Graphics Control Extension Block schreiben  [optional]	*/
/*-------------------------------------------------------------	*/
static uint8_t *write_graphctrl_ext(uint8_t *file, CONFIG *config, short delay)
{
	/* Wird momentan nur geschrieben wenn Transparent benîtigt wird */
	/* und auch verfÅgbar ist (nur GIF89a) */
	/* SpÑter auch bei Animationen */
	if (config->tindexok && config->typ == GIF89A)	/* Graphics Control Extension */
	{
		file[0] = 0x21;				/* Extension Introducer */
		file[1] = 0xf9;				/* Comment Label */
		file[2] = 4;				/* remain fields size */
		/* Packed Fields: 0 = Transparente Farbe? */
		/*                1 = User Input Flag */
		/*                2-4 = Disposal Method */
		/*                5-7 = reserviert */
		file[3] = (0 << 4) | (0 << 1) | (config->tindexok & 0x01);
		file[4] = delay;	/* Delay Time */
		file[5] = delay >> 8;
		file[6] = config->transparent;	/* Transparent Index */
		file[7] = 0;				/* Block Terminator */

		return file + 3 + file[2] + 1;
	}
	return file;
}


/*-------------------------------------------------------------	*/
/*			Comment Extension Block schreiben  [optional]		*/
/*-------------------------------------------------------------	*/
static uint8_t *write_comment_ext(uint8_t *file, CONFIG *config, char *comment)
{
	if (strlen(comment) && config->typ == GIF89A)	/* Comment Extension */
	{
		file[0] = 0x21;					/* Extension Introducer */
		file[1] = 0xfe;				/* Comment Label */
		file[2] = strlen(comment) + 1;	/* KommentarlÑnge */
		strcpy((char *)file + 3, comment);		/* Kommentar (nullterminiert) */
		file[3 + file[2]] = 0;	/* Block Terminator */

		return file + 3 + file[2] + 1;
	}
	return file;
}


/*-------------------------------------------------------------	*/
/*			Local Image Descriptor schreiben   [min. 1]			*/
/*-------------------------------------------------------------	*/
static uint8_t *write_image_descriptor(uint8_t *file, uint8_t *pal, unsigned short width, unsigned short height, uint8_t BitsPerPixel, uint8_t interlace)
{
	unsigned short coltab_size = 0;

	file[0] = 0x2c;
	file[1] = 0;	/* X-Pos */
	file[2] = 0;
	file[3] = 0;	/* Y-Pos */
	file[4] = 0;
	file[5] = width;	/* Breite */
	file[6] = width >> 8;
	file[7] = height;	/* Hîhe */
	file[8] = height >> 8;
	/* Packed Fields: 0-2 = Bits pro Pixel - 1 */
	/*                3-4 reserviert */
	/*                5 = 0 Sortierung nach Farbe */
	/*                6 = interlace */
	/*                7 = 0 lokale Farbtabelle nicht vorhanden */
	file[9] = (0 << 7) | (interlace << 6) | (0 << 5) | (BitsPerPixel - 1);

	/* local color table */
	if (file[9] & 0x80)
	{
		coltab_size = 1 << BitsPerPixel;

		memcpy(file + _LSCRDES, pal, 3 * coltab_size);
#if DEBUG
		printf("\n  Size: %d", coltab_size);
#endif
	}

	return file + _IMAGE_DES + 3 * coltab_size;
}


/* Sortiert ein Bild von normaler Zeilenreihenfolge */
/* in place in interlaced Zeilenreihenfolge um */
static void interlace(unsigned short height, CONFIG *config)
{
	unsigned short k, l;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	if (config->interlace)
	{
		services->reset_busybox(128, "interlace GIF");

		for (l = 0, k = 0; l < height; l += 8, k++)
			lacetab[k] = l;
		for (l = 4; l < height; l += 8, k++)
			lacetab[k] = l;
		for (l = 2; l < height; l += 4, k++)
			lacetab[k] = l;
		for (l = 1; l < height; l += 2, k++)
			lacetab[k] = l;
	} else
	{
		for (k = 0; k < height; k++)
			lacetab[k] = k;
	}

#if TIMER
	/* wie schnell waren wir? */
	printf("\nZeit: %lu", get_timer());
	(void)Cnecin();
#endif
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					GIF - Exporter (GIF)			*/
/*		1-8 Bit, LZW								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	char *comment;
	uint8_t *file;
	uint8_t *current_file;
	uint8_t BitsPerPixel;
	WORD t;

	unsigned short width, height;
	unsigned short headlen;

	unsigned long f_len, length;

	static WINDOW window;
	static OBJECT *win_form;
	static CONFIG config;
	CONFIG **pp;
	
	services = smurf_struct->services;

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
			config.typ = GIF87A;
			config.interlace = 0;
			config.transparent = 0;
			config.tindexok = 0;
		}

		win_form = rs_trindex[GIF_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		smurf_struct->module_mode = M_WAITING;
		break;

	case MMORE:
		/* Ressource aktualisieren */
		if (config.interlace)
			win_form[LINE_ARRANGE].ob_state |= OS_SELECTED;
		else
			win_form[LINE_ARRANGE].ob_state &= ~OS_SELECTED;

		if (config.tindexok)
		{
			win_form[TINDEX].ob_state &= ~OS_DISABLED;
			win_form[TINDEXOK].ob_state |= OS_SELECTED;
		} else
		{
			win_form[TINDEX].ob_state |= OS_DISABLED;
			win_form[TINDEXOK].ob_state &= ~OS_SELECTED;
		}

		if (config.typ == GIF87A)
		{
			win_form[GIF87A].ob_state |= OS_SELECTED;
			win_form[GIF89A].ob_state &= ~OS_SELECTED;
			win_form[TINDEX].ob_state |= OS_DISABLED;
			win_form[TINDEXOK].ob_state |= OS_DISABLED;
		} else
		{
			win_form[GIF87A].ob_state &= ~OS_SELECTED;
			win_form[GIF89A].ob_state |= OS_SELECTED;
			if (config.tindexok)
				win_form[TINDEX].ob_state &= ~OS_DISABLED;
			win_form[TINDEXOK].ob_state &= ~OS_DISABLED;
		}

		itoa(config.transparent, win_form[TINDEX].ob_spec.tedinfo->te_ptext, 10);

		window.whandlem = 0;			/* evtl. Handle lîschen */
		window.module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X-...        */
		window.wy = -1;					/* ...und Y-Pos         */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterhîhe          */
		strcpy(window.wtitle, "GIF Exporter");		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = TINDEX;			/* erstes Editobjekt    */
		window.nextedit = TINDEX;		/* nÑchstes Editobjekt  */
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
			config = **pp;
		} else
		{
			config.typ = GIF87A;
			config.interlace = 0;
			config.transparent = 0;
			config.tindexok = 0;
		}

		smurf_struct->module_mode = M_WAITING;

		break;

		/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case OK:
			config.transparent = atoi(win_form[TINDEX].ob_spec.tedinfo->te_ptext);
			if (config.transparent < 0 || config.transparent > 255)
			{
				form_alert(1, rs_frstr[WRONG_IDX]);
				smurf_struct->module_mode = M_WAITING;
			} else
			{
				/* Konfig Åbergeben */
				pp = (CONFIG **) &smurf_struct->event_par[0];
				*pp = &config;
				smurf_struct->event_par[2] = (short) sizeof(CONFIG);
	
				smurf_struct->module_mode = M_MOREOK;
			}
			break;

		case SAVE:
			config.transparent = atoi(win_form[TINDEX].ob_spec.tedinfo->te_ptext);

			/* Konfig Åbergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);

			smurf_struct->module_mode = M_CONFSAVE;
			break;
		
		case LINE_ARRANGE:
			config.interlace ^= 1;
			smurf_struct->module_mode = M_WAITING;
			break;

		case GIF87A:
		case GIF89A:
			config.typ = smurf_struct->event_par[0];

			if (config.typ == GIF87A)
			{
				win_form[TINDEX].ob_state |= OS_DISABLED;
				win_form[TINDEXOK].ob_state |= OS_DISABLED;
			} else
			{
				if (config.tindexok)
					win_form[TINDEX].ob_state &= ~OS_DISABLED;
				win_form[TINDEXOK].ob_state &= ~OS_DISABLED;
			}
			smurf_struct->services->redraw_window(&window, NULL, TINDEX_BOX, 0);
			smurf_struct->module_mode = M_WAITING;
			break;

		case TINDEXOK:
			config.tindexok ^= 1;

			if (config.tindexok)
				win_form[TINDEX].ob_state &= ~OS_DISABLED;
			else
				win_form[TINDEX].ob_state |= OS_DISABLED;

			smurf_struct->services->redraw_window(&window, NULL, TINDEX, 0);
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
			config.transparent = atoi(win_form[TINDEX].ob_spec.tedinfo->te_ptext);
			if (config.transparent < 0 || config.transparent > 255)
			{
				form_alert(1, rs_frstr[WRONG_IDX]);
				smurf_struct->module_mode = M_WAITING;
			} else
			{
				/* Konfig Åbergeben */
				pp = (CONFIG **) &smurf_struct->event_par[0];
				*pp = &config;
				smurf_struct->event_par[2] = (short) sizeof(CONFIG);
	
				smurf_struct->module_mode = M_MOREOK;
			}
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

		/* Extender wird vom Smurf erfragt */
	case MEXTEND:
		smurf_struct->event_par[0] = 1;
		smurf_struct->module_mode = M_EXTEND;
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		smurf_struct->event_par[0] = RGB;
		smurf_struct->module_mode = M_COLSYS;
		break;

	case MEXEC:
		SMalloc = smurf_struct->services->SMalloc;

		/* Zeiger initialisieren */
		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));
		buffer = smurf_struct->smurf_pic->pic_data;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;
		comment = smurf_struct->smurf_pic->infotext;

		/* Header + Screen Descriptor + globale Farbtabelle + Graphic Control Extension + */
		/* Comment Extension + Image Descriptor + Trailer */
		headlen = 6 + _LSCRDES + (1 << BitsPerPixel) * 3 + 8 + (4 + (short) strlen(comment) + 1) + _IMAGE_DES + 1;

		if (BitsPerPixel == 8)
			length = (long) width * (long) height * 2 / 3;	/* sollte an sich ausreichen */
		else
			length = (long) ((width + 7) / 8 * BitsPerPixel) * (long) height;

		file = ziel = (uint8_t *) SMalloc(headlen + length /* + (length / 250) * 2 */ );
		if (ziel == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		}

		current_file = write_header(file, config.typ);
		current_file = write_lscrdes(current_file, smurf_struct->smurf_pic->palette, width, height, BitsPerPixel);
		current_file = write_graphctrl_ext(current_file, &config, 0);
		/* Bildblock */
		current_file = write_comment_ext(current_file, &config, comment);
		current_file = write_image_descriptor(current_file, smurf_struct->smurf_pic->palette, width, height, BitsPerPixel, config.interlace);

		lacetab = (short *) SMalloc(height * sizeof(*lacetab));
		if (lacetab == NULL)
		{
			smurf_struct->services->SMfree(ziel);
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		}
		interlace(height, &config);

		if (BitsPerPixel == 8)
			current_file = encode_lzw_8bit(buffer, current_file, width, height);
		else
			current_file = encode_lzw_17bit(buffer, current_file, width, height, BitsPerPixel);

		if (current_file == NULL)
		{
			smurf_struct->services->SMfree(file);
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		}
		/* Trailer */
		*current_file++ = 0x3b;

		smurf_struct->services->SMfree(lacetab);

		f_len = current_file - file;

		_Mshrink(ziel, f_len);
		exp_pic->pic_data = ziel;
		exp_pic->f_len = f_len;
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


#if 0
/*-------------- Main-Function fÅr GIF-Programm zum Debuggen */
int main(void)
{
	int filehandle;
	GARGAMEL sm_struct;
	SMURF_PIC pic;
	uint8_t *file;
	long dummy, len;
	EXPORT_PIC *exp_pic;

	filehandle = (int)Fopen("AMBER.RAW", FO_READ);
	if (filehandle < 0)
		return 1;

	len = Fseek(0L, filehandle, 2);
	Fseek(0L, filehandle, 0);

	file = Malloc(len);

	Fread(filehandle, len, file);
	Fclose(filehandle);

	pic.pic_width = 320;
	pic.pic_height = 200;
	pic.depth = 8;
	sm_struct.smurf_pic = &pic;

	sm_struct.module_mode = MSTART;
	exp_pic = exp_module_main(&sm_struct);
	pic.pic_data = file + 768;
	pic.palette = (uint8_t *) malloc(1024);
	memcpy(pic.palette, file, 768);
	sm_struct.module_mode = MEXEC;
	exp_pic = exp_module_main(&sm_struct);

	Mfree(file);

	filehandle = (int) Fcreate("AMBER_.GIF", 0);
	Fwrite(filehandle, exp_pic->f_len, exp_pic->pic_data);
	Fclose(filehandle);

	return 0;
}
#endif
