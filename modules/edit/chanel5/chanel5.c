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
/*						Chanel No.5							*/
/* Version 0.1  --  26.01.97								*/
/*	  1-8, 24 Bit											*/
/* Version 0.2  --  30.01.97								*/
/*	  16 Bit - Achtung, wegen g von 6 auf 5 Bits nicht		*/
/*	  verlustfrei!											*/
/* Version 0.3  --  15.03.97								*/
/*	  Eigene Ressource										*/
/* Version 0.4  --  27.09.98								*/
/*	  šbermitteln und Empfangen von Konfigurationen ein-	*/
/*	  gebaut.												*/
/* Version 0.5  --  21.08.2000								*/
/*	  Laden und speichern von Konfigurationen eingebaut.	*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "globdefs.h"
#include "demolib.h"

#include "country.h"

#define TIMER 0

#if COUNTRY==1
#include "de/chanel5.rsh"
#elif COUNTRY==0
#include "en/chanel5.rsh"
#elif COUNTRY==2
#include "en/chanel5.rsh"				/* missing french resource */
#else
#error "Keine Sprache!"
#endif

typedef struct
{
	long version;
	uint8_t Folge;
} CONFIG;

static SERVICE_FUNCTIONS *services;

MOD_INFO module_info = {
	"Chanel No.5",
	0x0050,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 64 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	2, 4, 7, 8, 16, 24, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	0
};


static uint8_t Folge;
static short module_id;
static WINDOW window;
static OBJECT *win_form;


static void apply_setting(CONFIG *myConfig)
{
	Folge = myConfig->Folge;

	win_form[_RBG].ob_state &= ~OS_SELECTED;
	win_form[_GRB].ob_state &= ~OS_SELECTED;
	win_form[_GBR].ob_state &= ~OS_SELECTED;
	win_form[_BRG].ob_state &= ~OS_SELECTED;
	win_form[_BGR].ob_state &= ~OS_SELECTED;
	win_form[Folge].ob_state |= OS_SELECTED;

	services->redraw_window(&window, NULL, FORMAT_BOX, DRAWNOPICTURE);
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


static void write_setting(CONFIG * myConfig)
{
	myConfig->version = module_info.version;

	myConfig->Folge = Folge;
}


static void save_setting(void)
{
	char name[33];
	CONFIG myConfig;

	write_setting(&myConfig);

	services->mconfSave(&module_info, module_id, &myConfig, sizeof(CONFIG), name);
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Chanel No.5						*/
/*		1-8, 16 und 24 Bit							*/
/* Und wieder ein Modul mit so einem sch”nen Namen.	*/
/* Hiermit darf der User die drei Farbkan„le R, G	*/
/* und B eines Bildes verteilen wie er will.		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t t;
	uint8_t pixval;
	uint8_t BitsPerPixel;
	uint16_t *data16;
	uint16_t pixval16;
	unsigned short width, height;
	unsigned long length;
	CONFIG *config;
	CONFIG **pp;

	services = smurf_struct->services;

/* Wenn das Modul zum ersten Mal gestartet wurde */
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		win_form = rs_trindex[CHANEL5];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		module_id = smurf_struct->module_number;

		window.whandlem = 0;			/* evtl. Handle l”schen */
		window.module = smurf_struct->module_number;	/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X- ...       */
		window.wy = -1;					/* ... und Y-Pos        */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterh”he          */
		strcpy(window.wtitle, module_info.mod_name);		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* n„chstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		Folge = _RBG;					/* Default bestcken */

		if (smurf_struct->services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */
		break;

/* Buttonevent */
	case MBEVT:
		switch (smurf_struct->event_par[0])
		{
		case _RBG:
		case _GRB:
		case _GBR:
		case _BRG:
		case _BGR:
			Folge = smurf_struct->event_par[0];
			smurf_struct->module_mode = M_WAITING;
			break;
		case STARTIT:
			smurf_struct->module_mode = M_STARTED;
			break;
		case MCONF_LOAD:
			load_setting();
			break;
		case MCONF_SAVE:
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

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				data = smurf_struct->smurf_pic->pic_data;

				length = (unsigned long) width * (unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;

				length = SM_PALETTE_MAX;
			}

			switch (Folge)
			{
			case _RBG:
				do
				{
					data++;
					pixval = data[0];
					data[0] = data[1];
					data[1] = pixval;
					data += 2;
				} while (--length);
				break;
			case _GRB:
				do
				{
					pixval = data[0];
					data[0] = data[1];
					data[1] = pixval;
					data += 3;
				} while (--length);
				break;
			case _GBR:
				do
				{
					pixval = data[0];
					data[0] = data[1];
					data[1] = data[2];
					data[2] = pixval;
					data += 3;
				} while (--length);
				break;
			case _BRG:
				do
				{
					pixval = data[0];
					data[0] = data[2];
					data[2] = data[1];
					data[1] = pixval;
					data += 3;
				} while (--length);
				break;
			case _BGR:
				do
				{
					pixval = data[0];
					data[0] = data[2];
					data[2] = pixval;
					data += 3;
				} while (--length);
				break;
			default:
				break;
			}
		} else
		{
			data16 = (uint16_t *) smurf_struct->smurf_pic->pic_data;

			length = (unsigned long) width * (unsigned long) height;

			switch (Folge)
			{
			case _RBG:
				do
				{
					pixval16 = *data16;
					*data16++ = (pixval16 & 0xf800) | ((pixval16 & 0x7c0) >> 6) | ((pixval16 & 0x1f) << 6);
				} while (--length);
				break;
			case _GRB:
				do
				{
					pixval16 = *data16;
					*data16++ = ((pixval16 & 0xf800) >> 5) | ((pixval16 & 0x7c0) << 5) | (pixval16 & 0x1f);
				} while (--length);
				break;
			case _GBR:
				do
				{
					pixval16 = *data16;
					*data16++ = ((pixval16 & 0xf800) >> 11) | ((pixval16 & 0x7c0) << 5) | ((pixval16 & 0x1f) << 6);
				} while (--length);
				break;
			case _BRG:
				do
				{
					pixval16 = *data16;
					*data16++ = ((pixval16 & 0xf800) >> 5) | ((pixval16 & 0x7c0) >> 6) | ((pixval16 & 0x1f) << 11);
				} while (--length);
				break;
			case _BGR:
				do
				{
					pixval16 = *data16;
					*data16++ = ((pixval16 & 0xf800) >> 11) | (pixval16 & 0x7e0) | ((pixval16 & 0x1f) << 11);
				} while (--length);
				break;
			default:
				break;
			}
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
		pp = (CONFIG **)&smurf_struct->event_par[0];
		*pp = config;
		smurf_struct->event_par[2] = (short) sizeof(CONFIG);
		smurf_struct->module_mode = M_CONFIG;
		break;

	case CONFIG_TRANSMIT:
		pp = (CONFIG **)&smurf_struct->event_par[0];
		config = *pp;
		apply_setting(config);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
