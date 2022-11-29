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
/*						Channelmixer						*/
/* Version 0.1  --  13.04.98								*/
/*	  24 Bit, Bilder mÅssen noch gleich groû sein!			*/
/* Version 0.2  --  20.04.98								*/
/*	  Bilder kînnen jetzt auch in unterschiedlicher Grîûe	*/
/*	  vorliegen.											*/
/* Version 0.3  --  27.09.98								*/
/*	  öbermitteln und Empfangen sowie Laden und Speichern	*/
/*	  von Konfigurationen eingebaut.						*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"
#include "globdefs.h"

#include "country.h"

#if COUNTRY == 1
#define TEXT1 "Zielbild"
#define TEXT2 "Quellbild"
#elif COUNTRY == 0
#define TEXT1 "Source"
#define TEXT2 "Destination"
#else
#define TEXT1 "Source"
#define TEXT2 "Destination"
#endif

#include "chanelmx.rsh"


#define TIMER 0

#define SRC_PIC		0
#define DEST_PIC	1

typedef struct
{
	long version;
	uint8_t srcchanel;
	uint8_t destchanel;
	short sdeck;
} CONFIG;

static SERVICE_FUNCTIONS *services;

MOD_INFO module_info = {
	"Channelmixer",
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
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
	0, 64,
	0, 64,
	0, 64,
	0, 64,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	2,
	TEXT1, TEXT2, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0,	0, 0, 0,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};

static uint8_t srcchanel;
static uint8_t destchanel;

static short module_id;
static short sdeck;

static SLIDER decksl;

static WINDOW window;
static OBJECT *win_form;


static void apply_setting(CONFIG *myConfig)
{
	srcchanel = myConfig->srcchanel;
	destchanel = myConfig->destchanel;

	win_form[ZR].ob_state &= ~OS_SELECTED;
	win_form[ZG].ob_state &= ~OS_SELECTED;
	win_form[ZB].ob_state &= ~OS_SELECTED;
	win_form[destchanel].ob_state |= OS_SELECTED;

	win_form[QR].ob_state &= ~OS_SELECTED;
	win_form[QG].ob_state &= ~OS_SELECTED;
	win_form[QB].ob_state &= ~OS_SELECTED;
	win_form[srcchanel].ob_state |= OS_SELECTED;

	services->redraw_window(&window, NULL, DEST_BOX, DRAWNOPICTURE);
	services->redraw_window(&window, NULL, SRC_BOX, DRAWNOPICTURE);

	services->set_slider(&decksl, myConfig->sdeck);
}


static void load_setting(void)
{
	char name[33];
	CONFIG *myConfig;

	if ((myConfig = services->mconfLoad(&module_info, module_id, name)) != NULL)
	{
		apply_setting(myConfig);
		services->SMfree(myConfig);
	}
}


static void write_setting(CONFIG *myConfig)
{
	myConfig->version = 0x0030;

	myConfig->srcchanel = srcchanel;
	myConfig->destchanel = destchanel;

	myConfig->sdeck = sdeck;
}



static void save_setting(void)
{
	char name[33];
	CONFIG myConfig;

	write_setting(&myConfig);

	services->mconfSave(&module_info, module_id, &myConfig, sizeof(CONFIG), name);
}




static void make_sliders(void)
{
	decksl.regler = S_S;
	decksl.schiene = S_F;
	decksl.rtree = win_form;
	decksl.txt_obj = S_E;
	decksl.min_val = 1;
	decksl.max_val = 100;
	decksl.window = &window;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Channelmixer					*/
/* Ein Kanal (R, G oder B) wird in einen anderen	*/
/* Kanal eines anderen Bildes eingerechnet (Modus	*/
/* ersetzen mit StÑrke von 1-100).					*/
/*		24 Bit										*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL * smurf_struct)
{
	static uint8_t *buffer;
	static uint8_t *ziel;
	uint8_t t;
	uint8_t pixval;

	unsigned short sx, sy, dsx, ddx;
	unsigned short bh, bl;
	unsigned short deckung;
	static unsigned short swidth, sheight, dwidth, dheight;

	CONFIG *config;

	services = smurf_struct->services;

/* Wenn das Modul zum ersten Mal gestartet wurde */
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		win_form = rs_trindex[CHANELMIXER];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		module_id = smurf_struct->module_number;

		window.whandlem = 0;			/* evtl. Handle lîschen */
		window.module = smurf_struct->module_number;	/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X- ...       */
		window.wy = -1;					/* ... und Y-Pos        */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterhîhe          */
		strcpy(window.wtitle, module_info.mod_name);		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* nÑchstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		/* Defaults bestÅcken */
		srcchanel = QR;
		destchanel = ZR;
		sdeck = 100;
		make_sliders();
		services->set_slider(&decksl, 100);

		if (services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */

		break;

/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case QR:
		case QG:
		case QB:
			srcchanel = smurf_struct->event_par[0];
			smurf_struct->module_mode = M_WAITING;
			break;
		case ZR:
		case ZG:
		case ZB:
			destchanel = smurf_struct->event_par[0];
			smurf_struct->module_mode = M_WAITING;
			break;
		case S_S:
			sdeck = (short) services->slider(&decksl);
			break;
		case STARTIT:
			smurf_struct->module_mode = M_STARTED;
			break;
		case LOAD:
			load_setting();
			break;
		case SAVE:
			save_setting();
			break;
		}
		break;

/* Keyboardevent */
	case MKEVT:
		switch (smurf_struct->event_par[0])
		{
		case STARTIT:
			smurf_struct->module_mode = M_STARTED;
			break;
		}
		break;

	case MPICS:
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			smurf_struct->event_par[0] = 24;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = RGB;
			smurf_struct->module_mode = M_PICTURE;
			break;

		case SRC_PIC:
			smurf_struct->event_par[0] = 24;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = RGB;
			smurf_struct->module_mode = M_PICTURE;
			break;

		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		return;


	case MPICTURE:
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			ziel = smurf_struct->smurf_pic->pic_data;
			dwidth = smurf_struct->smurf_pic->pic_width;
			dheight = smurf_struct->smurf_pic->pic_height;
			break;
		case SRC_PIC:
			buffer = smurf_struct->smurf_pic->pic_data;
			swidth = smurf_struct->smurf_pic->pic_width;
			sheight = smurf_struct->smurf_pic->pic_height;
			break;
		}
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		deckung = sdeck * 64 / 100;

		if (swidth > dwidth)
		{
			dsx = (swidth - dwidth) * 3;
			ddx = 0;
			swidth = dwidth;
		} else
		{
			ddx = (dwidth - swidth) * 3;
			dsx = 0;
			dwidth = swidth;
		}

		if (sheight > dheight)
			sheight = dheight;
		else
			dheight = sheight;

		if ((bh = sheight / 10) == 0)	/* busy-height */
			bh = sheight;
		bl = 0;							/* busy-length */

		if (deckung == 64)
		{
			sy = 0;
			do
			{
				if (!(sy % bh))
				{
					services->busybox(bl);
					bl += 12;
				}

				sx = 0;
				do
				{
					if (srcchanel == QR)
						pixval = *buffer;
					else if (srcchanel == QG)
						pixval = *(buffer + 1);
					else
						pixval = *(buffer + 2);
					buffer += 3;

					if (destchanel == ZR)
						*ziel = pixval;
					else if (destchanel == ZG)
						*(ziel + 1) = pixval;
					else
						*(ziel + 2) = pixval;
					ziel += 3;
				} while (++sx < swidth);
				buffer += dsx;
				ziel += ddx;
			} while (++sy < sheight);
		} else
		{
			sy = 0;
			do
			{
				if (!(sy % bh))
				{
					services->busybox(bl);
					bl += 12;
				}

				sx = 0;
				do
				{
					if (srcchanel == QR)
						pixval = *buffer;
					else if (srcchanel == QG)
						pixval = *(buffer + 1);
					else
						pixval = *(buffer + 2);
					buffer += 3;

					if (destchanel == ZR)
						*ziel = (char) ((pixval * deckung + *ziel * (64 - deckung)) >> 8);
					else if (destchanel == ZG)
						*(ziel + 1) = (char) ((pixval * deckung + *(ziel + 1) * (64 - deckung)) >> 8);
					else
						*(ziel + 2) = (char) ((pixval * deckung + *(ziel + 2) * (64 - deckung)) >> 8);
					ziel += 3;
				} while (++sx < swidth);
				buffer += dsx;
				ziel += ddx;
			} while (++sy < sheight);
		}

#if TIMER
		/* wie schnell waren wir? */
		printf("\n%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_PICDONE;
		break;

	case GETCONFIG:
		config = smurf_struct->services->SMalloc(sizeof(CONFIG));
		write_setting(config);

		*((CONFIG **)&smurf_struct->event_par[0]) = config;
		smurf_struct->event_par[2] = (short) sizeof(CONFIG);
		smurf_struct->module_mode = M_CONFIG;
		break;

	case CONFIG_TRANSMIT:
		config = *((CONFIG **)&smurf_struct->event_par[0]);
		smurf_struct->module_mode = M_WAITING;
		break;

/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
