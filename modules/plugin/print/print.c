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
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurf_st.h"
#include "globdefs.h"
#include "smurfine.h"
#include "plugin.h"
#include "../../../src/rsc/en/smurf.h"
#undef NUM_STRINGS
#undef NUM_FRSTR
#undef NUM_UD
#undef NUM_IMAGES
#undef NUM_BB
#undef NUM_FRIMG
#undef NUM_IB
#undef NUM_CIB
#undef NUM_TI
#undef NUM_OBS
#undef NUM_TREE
#undef COL8								/* conflicts with smurf.h */
#include "bindings.h"
#include "wdialog.h"
#include "gdos.h"

#include "country.h"

#if COUNTRY==1
#include "de/print.rsh"
#elif COUNTRY==0
#include "en/print.rsh"
#elif COUNTRY==2
#include "en/print.rsh"		/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))

static void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);

static char const name_string[] = "GDOS Print-Plugin";

PLUGIN_INFO plugin_info = {
	name_string,
	0x0103,								/* Plugin-Version 1.03 */
	SMURF_VERSION,						/* fÅr Smurf-Version 1.06 */
	0									/* und nicht resident. */
};

static short my_id;
static BOOLEAN dialog_open;
static BOOLEAN popup_initialized;
static WORD unit = UNIT_PIXELS;

static POP_UP driver_popup;
static POP_UP paper_popup;
static POP_UP unit_popup;
static WINDOW print_window;
static OBJECT *print_dialog;
static OBJECT *drivpop_rsc;
static OBJECT *pappop_rsc;
static OBJECT *unitpop_rsc;
static SMURF_PIC *pic_active;

static WORD curr_deviceID = 91;
static float scale;
static float temp;

SERVICE_FUNCTIONS *services;
SMURF_PIC **smurf_picture;
short *active_pic;
char **alerts;
PLUGIN_FUNCTIONS *smurf_functions;
SMURF_VARIABLES *smurf_vars;

static WORD pdlg_handle;
static PRN_DIALOG *prn_dialog;
static PRN_SETTINGS *prn_settings;
static BOOLEAN pdlg_available;


static void init_windstruct(void)
{
	print_window.whandlem = 0;			/* evtl. Handle lîschen */
	print_window.module = my_id;
	print_window.wnum = 1;				/* Das wievielte Fenster des Moduls? */

	print_window.wx = -1;
	print_window.wy = -1;
	print_window.ww = print_dialog->ob_width;
	print_window.wh = print_dialog->ob_height;

	strcpy(print_window.wtitle, "Bild drucken");	/* Fenstertitel */

	print_window.resource_form = print_dialog;	/* Modulresource-Formular */
	print_window.picture = NULL;		/* Zeigerfeld fÅr Bild/Animation */
	print_window.editob = XPOS;
	print_window.nextedit = YPOS;
	print_window.editx = 0;
	print_window.pflag = 0;

	print_window.prev_window = NULL;	/* vorheriges Fenster (WINDOW*) */
	print_window.next_window = NULL;	/* nÑxtes Fenster (WINDOW*) */
}


/* init_rsh -----------------------------------------------------
    Initialisiert die Resource 
    -----------------------------------------------------------*/
static void init_rsh(void)
{
	WORD t;

	/* Resource Umbauen */
	for (t = 0; t < NUM_OBS; t++)
		rsrc_obfix(rs_object, t);

	print_dialog = rs_trindex[PRINT_MAIN];	/* Resourcebaum holen */
	drivpop_rsc = rs_trindex[DRIVERS];
	pappop_rsc = rs_trindex[PAPERS];
	unitpop_rsc = rs_trindex[UNITS];
	alerts = rs_frstr;
}


/* init_driver_popup ----------------------------------------------
    Bereitet das driver-Popup vor 
    -----------------------------------------------------------*/
static void init_driver_popup(void)
{
	short t, tt;
	WORD curr_obj;

	/*
	 * Driver-Popup vorbereiten (POP_UP)
	 */
	driver_popup.popup_tree = DRIVERS;	/* Objektbaum-Index des Popups */
	driver_popup.item = 0;				/* Item (=letzter Eintrag)      */
	driver_popup.display_tree = print_dialog;	/* Tree, in dems dargestellt werden soll */
	driver_popup.display_obj = DRIVER_POPBUT;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	driver_popup.Cyclebutton = DRIVER_CB;	/* Index des Cyclebuttons */

	curr_obj = DRIVER1;

	for (t = 0; t < dev_anzahl; t++)
	{
		strcpy(drivpop_rsc[curr_obj].ob_spec.tedinfo->te_ptext, DevInfo[t].devName);
		drivpop_rsc[curr_obj].ob_y = 3 + t * 20;
		curr_obj++;
	}

	for (tt = t; tt < 30; tt++)
	{
		objc_delete(drivpop_rsc, curr_obj);
		curr_obj++;
	}

	drivpop_rsc[0].ob_height = dev_anzahl * 20 + 3;

	for (t = 0; t < dev_anzahl; t++)
	{
		if (DevInfo[t].devID == curr_deviceID)
			break;
	}

	driver_popup.item = t + DRIVER1;

	/*
	 * Papier-Popup vorbereiten (POP_UP)
	 */
	paper_popup.popup_tree = PAPERS;	/* Objektbaum-Index des Popups */
	paper_popup.item = 1;				/* Item (=letzter Eintrag)      */
	paper_popup.display_tree = print_dialog;	/* Tree, in dems dargestellt werden soll */
	paper_popup.display_obj = PAPER_POPBUT;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	paper_popup.Cyclebutton = PAPER_CB;	/* Index des Cyclebuttons */

	/*
	 * Einheiten-Popup vorbereiten (POP_UP)
	 */
	unit_popup.popup_tree = UNITS;		/* Objektbaum-Index des Popups */
	unit_popup.item = 1;				/* Item (=letzter Eintrag)      */
	unit_popup.display_tree = print_dialog;	/* Tree, in dems dargestellt werden soll */
	unit_popup.display_obj = UNIT_POPBUT;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	unit_popup.Cyclebutton = UNIT_CB;	/* Index des Cyclebuttons */

	popup_initialized = 1;
}


/*----------------------------------------------------------------- */
/* convert_units_from                                               */
/*  Konvertiert Pixel in Pixel, Millimeter, Zoll und Prozent        */
/*  RÅckgabewert ist ein Flieûkommawert zur Ausgabe.                */
/*----------------------------------------------------------------- */
static float convert_units_from(WORD unit, WORD mode)
{
	float val;

	switch (mode)
	{
	case WIDTH:
		switch (unit)
		{
		case UNIT_PIXELS:
			val = (float) OutParam.picwidth;
			break;
		case UNIT_MM:
			val = (float) OutParam.picwidth * 25.4 / (float) DevParam.horres;
			break;
		case UNIT_INCH:
			val = (float) OutParam.picwidth / (float) DevParam.horres;
			break;
		case UNIT_PERCENT:
			val = (float) OutParam.picwidth * 100.0 / (float) pic_active->pic_width;
			break;
		}
		break;

	case HEIGHT:
		switch (unit)
		{
		case UNIT_PIXELS:
			val = (float) OutParam.picheight;
			break;
		case UNIT_MM:
			val = (float) OutParam.picheight * 25.4 / (float) DevParam.verres;
			break;
		case UNIT_INCH:
			val = (float) OutParam.picheight / (float) DevParam.verres;
			break;
		case UNIT_PERCENT:
			val = (float) OutParam.picheight * 100.0 / (float) pic_active->pic_height;
			break;
		}
		break;

	case XPOS:
		switch (unit)
		{
		case UNIT_PIXELS:
			val = (float) OutParam.zx;
			break;
		case UNIT_MM:
			val = (float) OutParam.zx * 25.4 / (float) DevParam.verres;
			break;
		case UNIT_INCH:
			val = (float) OutParam.zx / (float) DevParam.verres;
			break;
		case UNIT_PERCENT:
			val = (float) OutParam.zx * 100.0 / (float) DevParam.prtwidth;
			break;
		}
		break;

	case YPOS:
		switch (unit)
		{
		case UNIT_PIXELS:
			val = (float) OutParam.zy;
			break;
		case UNIT_MM:
			val = (float) OutParam.zy * 25.4 / (float) DevParam.verres;
			break;
		case UNIT_INCH:
			val = (float) OutParam.zy / (float) DevParam.verres;
			break;
		case UNIT_PERCENT:
			val = (float) OutParam.zy * 100.0 / (float) DevParam.prtheight;
			break;
		}
		break;
	}

	return val;
}


/*----------------------------------------------------------------- */
/* convert_units_to                                                 */
/*  Konvertiert Pixel, Millimeter, Zoll und Prozent in Pixel        */
/*  RÅckgabewert ist ein Flieûkommawert zur Ausgabe.                */
/*----------------------------------------------------------------- */
static WORD convert_units_to(WORD unit, WORD mode)
{
	float val;

	switch (mode)
	{
	case WIDTH:
		val = atof(print_dialog[WIDTH].TextCast);

		switch (unit)
		{
		case UNIT_MM:
			val = val * (float) DevParam.horres / 25.4;
			break;
		case UNIT_INCH:
			val = val * (float) DevParam.horres;
			break;
		case UNIT_PERCENT:
			val = val * (float) pic_active->pic_width / 100.0;
			break;
		}
		break;

	case HEIGHT:
		val = atof(print_dialog[HEIGHT].TextCast);

		switch (unit)
		{
		case UNIT_MM:
			val = val * (float) DevParam.verres / 25.4;
			break;
		case UNIT_INCH:
			val = val * (float) DevParam.verres;
			break;
		case UNIT_PERCENT:
			val = val * (float) pic_active->pic_height / 100.0;
			break;
		}
		break;


	case XPOS:
		val = atof(print_dialog[XPOS].TextCast);

		switch (unit)
		{
		case UNIT_MM:
			val = val * (float) DevParam.horres / 25.4;
			break;
		case UNIT_INCH:
			val = val * (float) DevParam.horres;
			break;
		case UNIT_PERCENT:
			val = val * (float) DevParam.prtwidth / 100.0;
			break;
		}
		break;

	case YPOS:
		val = atof(print_dialog[YPOS].TextCast);

		switch (unit)
		{
		case UNIT_MM:
			val = val * (float) DevParam.verres / 25.4;
			break;
		case UNIT_INCH:
			val = val * (float) DevParam.verres;
			break;
		case UNIT_PERCENT:
			val = val * (float) DevParam.prtheight / 100.0;
			break;
		}
		break;
	}

	return val + 0.5;
}


/* actualize_dialog ----------------------------------------------
    Aktualisiert den Druckdialog (Previews)
    ------------------------------------------------------------*/
static void actualize_dialog(WORD start_object)
{
	WORD pagewidth, pageheight;						/* Page in Realmaûen (Pixel) */
	WORD val;

	pagewidth = DevParam.prtwidth + DevParam.leftBorder + DevParam.rightBorder;
	pageheight = DevParam.prtheight + DevParam.upperBorder + DevParam.lowerBorder;

	if (OutParam.zx + OutParam.picwidth > DevParam.leftBorder + DevParam.prtwidth)
	{
		OutParam.picwidth = DevParam.prtwidth - OutParam.zx;

		val = convert_units_from(unit, WIDTH);
		itoa(val, print_dialog[WIDTH].TextCast, 10);
		redraw_window(&print_window, NULL, WIDTH, 0);
	}

	if (OutParam.zy + OutParam.picheight > DevParam.upperBorder + DevParam.prtheight)
	{
		OutParam.picheight = DevParam.prtheight - OutParam.zy;

		val = convert_units_from(unit, HEIGHT);
		itoa(val, print_dialog[HEIGHT].TextCast, 10);
		redraw_window(&print_window, NULL, HEIGHT, 0);
	}

	scale = (float) pagewidth / (float) (print_dialog[PAPER_REDRAW].ob_width - 6);
	temp = (float) pageheight / (float) (print_dialog[PAPER_REDRAW].ob_height - 6);
	if (temp > scale)
		scale = temp;
	scale += 0.001;						/* zur Sicherheit etwas grîûer machen */
	print_dialog[POSITION_PAPER].ob_width = pagewidth / scale + 0.5;
	print_dialog[POSITION_PAPER].ob_height = pageheight / scale + 0.5;

	print_dialog[POSITION_FRAME].ob_x = DevParam.leftBorder / scale + 0.5;
	print_dialog[POSITION_FRAME].ob_y = DevParam.upperBorder / scale + 0.5;
	print_dialog[POSITION_FRAME].ob_width = DevParam.prtwidth / scale + 0.5;
	print_dialog[POSITION_FRAME].ob_height = DevParam.prtheight / scale + 0.5;

	print_dialog[POSITION_BOX].ob_x = OutParam.zx / scale + 0.5;
	print_dialog[POSITION_BOX].ob_y = OutParam.zy / scale + 0.5;
	print_dialog[POSITION_BOX].ob_width = OutParam.picwidth / scale + 0.5;
	print_dialog[POSITION_BOX].ob_height = OutParam.picheight / scale + 0.5;

	print_dialog[SIZE_BOX].ob_x =
		/*print_dialog[POSITION_BOX].ob_x + */ print_dialog[POSITION_BOX].ob_width - print_dialog[SIZE_BOX].ob_width;
	print_dialog[SIZE_BOX].ob_y =
		/*print_dialog[POSITION_BOX].ob_y + */ print_dialog[POSITION_BOX].ob_height - print_dialog[SIZE_BOX].ob_height;

	if (print_dialog[SIZE_BOX].ob_x < 0)
		print_dialog[SIZE_BOX].ob_x = 0;
	if (print_dialog[SIZE_BOX].ob_y < 0)
		print_dialog[SIZE_BOX].ob_y = 0;

	if (print_dialog[POSITION_BOX].ob_x < print_dialog[POSITION_FRAME].ob_x)
		print_dialog[POSITION_BOX].ob_x = print_dialog[POSITION_FRAME].ob_x;
	if (print_dialog[POSITION_BOX].ob_y < print_dialog[POSITION_FRAME].ob_y)
		print_dialog[POSITION_BOX].ob_y = print_dialog[POSITION_FRAME].ob_y;

	redraw_window(&print_window, NULL, start_object, 0);

	return;
}



/* get_devinfo ------------------------------------------------
    Sucht aus den bis zu 30 DevInfoS-Strukturen die zu ID passende raus.
    ----------------------------------------------------------*/
static DevInfoS *get_devinfo(WORD ID)
{
	int t;

	for (t = 0; t < 30; t++)
	{
		if (DevInfo[t].devID == ID)
			return &DevInfo[t];
	}

	return NULL;
}



/*--------------------------------------------------------------------------------- */
/* insert_32_float                                                                  */
/*  fÅgt eine 64Bit-Flieûkommazahl val als Text in den Objektbaum tree, TEXTobjekt  */
/*  ein. Der String hat hinterher grundsÑtzlich 5 Stellen, bei variabler Mantisse.  */
/*--------------------------------------------------------------------------------- */
static void insert_float(OBJECT *tree, WORD object, float val)
{
	char str[10];
	char str2[10];
	int dec;
	int sign;
	double fval;

	memset(str, 0, 10);
	memset(str2, 0, 10);
	fval = (double) val;

	ftoa(&fval, str, 10, 1, &dec, &sign);

	if (dec > 0)
		strncpy(str2, str, dec);
	else
		strcpy(str2, "0");

	if (dec < 0)
		dec = 0;
	strcat(str2, ".");

	strcat(str2, str + dec);

	strncpy(tree[object].TextCast, str2, 5);

	redraw_window(&print_window, NULL, object, 0);
}


static void drag_posbox(WORD mousex, WORD mousey, WORD object)
{
	WORD inx, iny, inw, inh;
	WORD outx, outy, outw, outh;
	WORD endx, endy;
	WORD papx, papy;
	WORD button;
	WORD key;
	WORD oldx, oldy;
	float val;
	float aspect;

	aspect = (float) pic_active->pic_width / (float) pic_active->pic_height;

	objc_offset(print_dialog, POSITION_PAPER, &papx, &papy);

	objc_offset(print_dialog, POSITION_BOX, &inx, &iny);
	inw = print_dialog[POSITION_BOX].ob_width;
	inh = print_dialog[POSITION_BOX].ob_height;

	objc_offset(print_dialog, POSITION_FRAME, &outx, &outy);
	outw = print_dialog[POSITION_FRAME].ob_width;
	outh = print_dialog[POSITION_FRAME].ob_height;

	oldx = mousex;
	oldy = mousey;

	/*
	 * Verkleinern/Vergrîûern?
	 */
	if (object == SIZE_BOX)
	{
		do
		{
			graf_mkstate(&endx, &endy, &button, &key);

			if (oldx == endx && oldy == endy)
				continue;

			oldx = endx;
			oldy = endy;

			if (endx < outx + outw && endx > inx)
			{
				print_dialog[POSITION_BOX].ob_width = endx - inx;

				OutParam.picwidth = (endx - inx) * scale;

				val = convert_units_from(unit, WIDTH);
				insert_float(print_dialog, WIDTH, val);
			}

			if (endy < outy + outh && endy > iny)
			{
				print_dialog[POSITION_BOX].ob_height = endy - iny;

				if (key == KEY_CTRL || key & KEY_SHIFT)
					OutParam.picheight = OutParam.picwidth / aspect;
				else
					OutParam.picheight = (endy - iny) * scale;

				val = convert_units_from(unit, HEIGHT);
				insert_float(print_dialog, HEIGHT, val);
			}

			print_dialog[SIZE_BOX].ob_x = print_dialog[POSITION_BOX].ob_width - print_dialog[SIZE_BOX].ob_width;
			print_dialog[SIZE_BOX].ob_y = print_dialog[POSITION_BOX].ob_height - print_dialog[SIZE_BOX].ob_height;

			actualize_dialog(POSITION_PAPER);
		} while (button != 0);
	}

	/*
	 * Draggen?
	 */
	else
	{
		graf_dragbox(inw, inh, inx, iny, outx, outy, outw, outh, &endx, &endy);

		print_dialog[POSITION_BOX].ob_x = endx - papx;
		print_dialog[POSITION_BOX].ob_y = endy - papy;
		print_dialog[SIZE_BOX].ob_x =
			(endx - papx) + print_dialog[POSITION_BOX].ob_width - print_dialog[SIZE_BOX].ob_width;
		print_dialog[SIZE_BOX].ob_y =
			(endx - papx) + print_dialog[POSITION_BOX].ob_height - print_dialog[SIZE_BOX].ob_height;

		OutParam.zx = (endx - papx) * scale + 0.5;
		if (OutParam.zx + OutParam.picwidth > DevParam.leftBorder + DevParam.prtwidth)
			OutParam.zx = DevParam.leftBorder + DevParam.prtwidth - OutParam.picwidth;
		else if (print_dialog[POSITION_BOX].ob_x <= print_dialog[POSITION_FRAME].ob_x)
			OutParam.zx = DevParam.leftBorder;

		OutParam.zy = (endy - papy) * scale + 0.5;
		if (OutParam.zy + OutParam.picheight > DevParam.upperBorder + DevParam.prtheight)
			OutParam.zy = DevParam.upperBorder + DevParam.prtheight - OutParam.picheight;
		else if (print_dialog[POSITION_BOX].ob_y <= print_dialog[POSITION_FRAME].ob_y)
			OutParam.zy = DevParam.upperBorder;

		val = convert_units_from(unit, XPOS);
		insert_float(print_dialog, XPOS, val);
		val = convert_units_from(unit, YPOS);
		insert_float(print_dialog, YPOS, val);

		actualize_dialog(POSITION_PAPER);
	}
}


/* Clippt XPOS, YPOS, WIDTH und HEIGHT damit das Bild aufs Blatt paût */
static BOOLEAN clip_values(void)
{
	BOOLEAN changed = FALSE;

	if (OutParam.zx + OutParam.picwidth > DevParam.leftBorder + DevParam.prtwidth)
	{
		OutParam.zx = DevParam.leftBorder + DevParam.prtwidth - OutParam.picwidth;
		changed = TRUE;
	}

	if (OutParam.zx < 0)
	{
		OutParam.zx = 0;
		changed = TRUE;
	}

	if (OutParam.zy + OutParam.picheight > DevParam.upperBorder + DevParam.prtheight)
	{
		OutParam.zy = DevParam.upperBorder + DevParam.prtheight - OutParam.picheight;
		changed = TRUE;
	}

	if (OutParam.zy < 0)
	{
		OutParam.zy = 0;
		changed = TRUE;
	}

	if (OutParam.zx + OutParam.picwidth > DevParam.leftBorder + DevParam.prtwidth)
	{
		OutParam.picwidth = DevParam.prtwidth - OutParam.zx;
		changed = TRUE;
	}

	if (OutParam.zy + OutParam.picheight > DevParam.upperBorder + DevParam.prtheight)
	{
		OutParam.picheight = DevParam.prtheight - OutParam.zy;
		changed = TRUE;
	}

	return changed;
}


static void actualize_xywh(void)
{
	float val;

	val = convert_units_from(unit, WIDTH);
	insert_float(print_dialog, WIDTH, val);

	val = convert_units_from(unit, HEIGHT);
	insert_float(print_dialog, HEIGHT, val);

	val = convert_units_from(unit, XPOS);
	insert_float(print_dialog, XPOS, val);

	val = convert_units_from(unit, YPOS);
	insert_float(print_dialog, YPOS, val);
}


static void close_print_dialog(BOOLEAN with_free)
{
	WORD lastx, lasty;

	if (prn_dialog)
	{
		pdlg_close(prn_dialog, &lastx, &lasty);
		pdlg_delete(prn_dialog);
		if (with_free)
			pdlg_free_settings(prn_settings);
		prn_dialog = 0;
		pdlg_handle = 0;
	}
}


/* handle_print_dialog ----------------------------------------
    öbernimmt die Userbedienung des Print-Dialogs
    ----------------------------------------------------------*/
static void handle_print_dialog(PLUGIN_DATA *data)
{
	unsigned char new_paperform;
	WORD button;
	WORD back;
	float val;

#if 0
	Goto_pos(1, 0);
#endif

	button = data->event_par[0];

	if (data->message == MBEVT)
	{
		switch (button)
		{
		case DRIVER_POPBUT:
		case DRIVER_CB:
			back = services->popup(&driver_popup, 0, button, drivpop_rsc);
			if (back != -1)
			{
				curr_deviceID = DevInfo[back - DRIVER1].devID;

#if 0
				printf("Drucker gewechselt\n");
#endif
				back = actualize_DevParam(curr_deviceID, &DevParam);
#if 0
				printf("Druckerinfo aktualisiert\n");
#endif
				if (back == -1)
				{
					services->f_alert(alerts[NO_INIT], NULL, NULL, NULL, 1);
				} else
				{
					clip_values();

					actualize_dialog(PAPER_REDRAW);

					back = DevParam.paperform;
					if (back >= PAGE_DEFAULT && back <= PAGE_B5)
						paper_popup.item = back + PAPER1;
					else
						paper_popup.item = back + PAPER6 - 16;

					strcpy(print_dialog[PAPER_POPBUT].TextCast, pappop_rsc[paper_popup.item].TextCast);
					services->deselect_popup(&print_window, PAPER_POPBUT, PAPER_CB);
				}

				if (DevParam.can_scale == 0)
				{
					print_window.editob = XPOS;
					print_dialog[WIDTH_UP].ob_state |= OS_DISABLED;
					print_dialog[WIDTH_DOWN].ob_state |= OS_DISABLED;
					print_dialog[WIDTH].ob_state |= OS_DISABLED;
					print_dialog[HEIGHT_UP].ob_state |= OS_DISABLED;
					print_dialog[HEIGHT_DOWN].ob_state |= OS_DISABLED;
					print_dialog[HEIGHT].ob_state |= OS_DISABLED;
				} else
				{
					print_dialog[WIDTH_UP].ob_state &= ~OS_DISABLED;
					print_dialog[WIDTH_DOWN].ob_state &= ~OS_DISABLED;
					print_dialog[WIDTH].ob_state &= ~OS_DISABLED;
					print_dialog[HEIGHT_UP].ob_state &= ~OS_DISABLED;
					print_dialog[HEIGHT_DOWN].ob_state &= ~OS_DISABLED;
					print_dialog[HEIGHT].ob_state &= ~OS_DISABLED;
				}

				redraw_window(&print_window, NULL, ADJUST_PARENT, 0);
			}

			services->deselect_popup(&print_window, DRIVER_POPBUT, DRIVER_CB);
			actualize_xywh();
			break;

		case PAPER_POPBUT:
		case PAPER_CB:
			back = services->popup(&paper_popup, 0, button, pappop_rsc);
			if (back != -1)
			{
				if (back >= PAPER1 && back <= PAPER5)
					new_paperform = back - PAPER1;
				else
					new_paperform = back - PAPER6 + 16;
				DevParam.paperform = new_paperform;

				back = actualize_DevParam(curr_deviceID, &DevParam);
				if (back == -1)
				{
					services->f_alert(alerts[NO_INIT], NULL, NULL, NULL, 1);
				} else
				{
					clip_values();

					actualize_dialog(PAPER_REDRAW);
				}

				if (DevParam.paperform != new_paperform)
				{
					services->f_alert(alerts[PAPERFORMAT], NULL, NULL, NULL, 1);
					back = DevParam.paperform;
					if (back >= PAGE_DEFAULT && back <= PAGE_B5)
						paper_popup.item = back + PAPER1;
					else
						paper_popup.item = back + PAPER6 - 16;

					strcpy(print_dialog[PAPER_POPBUT].TextCast, pappop_rsc[paper_popup.item].TextCast);
				}
			}

			services->deselect_popup(&print_window, PAPER_POPBUT, PAPER_CB);
			actualize_xywh();
			break;

		case PRINT_START:
			if (DevParam.depth == 1 && smurf_picture[*active_pic]->depth > 1)
			{
				services->f_alert(alerts[NO_COLOUR], NULL, NULL, NULL, 1);
			} else if (DevParam.depth == 3)
			{
				services->f_alert(alerts[NO_EIGHT], NULL, NULL, NULL, 1);
			} else
			{
				OutParam.advance = 1;	/* Vorschub nach Seite? */
				print_with_GDOS();
			}
			break;

		case X_UP:
		case X_DOWN:
			val = atof(print_dialog[XPOS].TextCast);
			if (button == X_UP)
				val++;
			else
				val--;
			insert_float(print_dialog, XPOS, val);
			break;

		case Y_UP:
		case Y_DOWN:
			val = atof(print_dialog[YPOS].TextCast);
			if (button == Y_UP)
				val++;
			else
				val--;
			insert_float(print_dialog, YPOS, val);
			break;

		case WIDTH_UP:
		case WIDTH_DOWN:
			val = atof(print_dialog[WIDTH].TextCast);
			if (button == WIDTH_UP)
				val++;
			else
				val--;
			insert_float(print_dialog, WIDTH, val);
			break;

		case HEIGHT_UP:
		case HEIGHT_DOWN:
			val = atof(print_dialog[HEIGHT].TextCast);
			if (button == HEIGHT_UP)
				val++;
			else
				val--;
			insert_float(print_dialog, HEIGHT, val);
			break;

		case SIZE_BOX:
		case POSITION_BOX:
			drag_posbox(data->mousex, data->mousey, button);
			break;

		case UNIT_POPBUT:
		case UNIT_CB:
			back = services->popup(&unit_popup, 0, button, unitpop_rsc);
			if (back != -1)
			{
				unit = back;

				strcpy(print_dialog[UNIT_POPBUT].TextCast, unitpop_rsc[unit_popup.item].TextCast);

				actualize_xywh();
			}

			services->deselect_popup(&print_window, UNIT_POPBUT, UNIT_CB);
			break;
		}
	}

	if (data->message == MKEVT)
	{
		if (button == PRINT_START)
		{
			if (DevParam.depth == 3)
				services->f_alert(alerts[NO_EIGHT], NULL, NULL, NULL, 1);
			else
			{
				OutParam.advance = 1;	/* Vorschub nach Seite? */
				print_with_GDOS();
			}
		}
	}

	/* Breite leer? */
	if (*(print_dialog[WIDTH].TextCast) == '\0' && button != WIDTH)
	{
		val = (float) pic_active->pic_width * (float) OutParam.picheight / (float) pic_active->pic_height;
		OutParam.picwidth = val;
		val = convert_units_from(unit, WIDTH);
		insert_float(print_dialog, WIDTH, val);
	}

	/* Hîhe leer? */
	if (*(print_dialog[HEIGHT].TextCast) == '\0' && button != HEIGHT)
	{
		val = (float) pic_active->pic_height * (float) OutParam.picwidth / (float) pic_active->pic_width;
		OutParam.picheight = val;
		val = convert_units_from(unit, HEIGHT);
		insert_float(print_dialog, HEIGHT, val);
	}

	/*
	 * Position geÑndert?
	 */
	if (button == X_UP || button == X_DOWN || button == XPOS)
	{
		button = XPOS;
		OutParam.zx = convert_units_to(unit, XPOS);
	} else if (button == Y_UP || button == Y_DOWN || button == YPOS)
	{
		button = YPOS;
		OutParam.zy = convert_units_to(unit, YPOS);
	} else if (button == WIDTH_UP || button == WIDTH_DOWN || button == WIDTH)
	{
		button = WIDTH;
		OutParam.picwidth = convert_units_to(unit, WIDTH);
	} else if (button == HEIGHT_UP || button == HEIGHT_DOWN || button == HEIGHT)
	{
		button = HEIGHT;
		OutParam.picheight = convert_units_to(unit, HEIGHT);
	}

	if (clip_values())
	{
		val = convert_units_from(unit, button);
		insert_float(print_dialog, button, val);
		redraw_window(&print_window, NULL, button, 0);
	}

	actualize_dialog(POSITION_PAPER);
}


/* handle_aesmsg ----------------------------------------------
    kÅmmert sich um von Smurf weitergeleitete AES-Messages
    ----------------------------------------------------------*/
static short handle_aesmsg(WORD *msgbuf)
{
	WORD driver_id;
	DevInfoS *info;
	short t;

	if (msgbuf[0] == 82)				/* Treiber wurde geÑndert! */
	{
		driver_id = msgbuf[3];

		info = get_devinfo(driver_id);
		if (info != NULL)
		{
			get_DevInfo(driver_id, info);

			for (t = 0; t < dev_anzahl; t++)
				if (DevInfo[t].devID == driver_id)
					break;

			strcpy(drivpop_rsc[t + DRIVER1].TextCast, DevInfo[t].devName);

			if (driver_popup.item == t + DRIVER1)
				strcpy(print_dialog[DRIVER_POPBUT].TextCast, DevInfo[t].devName);

			services->deselect_popup(&print_window, DRIVER_POPBUT, DRIVER_CB);
		}

		if (driver_id == curr_deviceID)
			actualize_DevParam(curr_deviceID, &DevParam);

		actualize_dialog(PAPER_REDRAW);
	}

	return M_WAITING;
}


/* KÅrzt fÅr beliebige Strings zu lange Filenamen durch */
/* entfernen von Text in der Mitte und ersetzen durch "...". */
/* Aus "Dies ist ein zu langer Filename.img" wÅrde bei KÅrzung */
/* auf 27 Zeichen "Dies ist ein...Filename.img" */
static char *shorten_name(char *string, short newlen, char *temp)
{
	/* nichts tun wenn String sowieso passend */
	if (strlen(string) <= newlen)
		return string;

	strncpy(temp, string, newlen / 2 - 1);	/* auf die HÑlfte und eines weniger */
	strcat(temp, "...");				/* LÅckenfÅller rein */
	strcat(temp, string + strlen(string) - (newlen - newlen / 2 - 3));	/* und bis newlen LÑnge mit Originalstring affÅllen */

	return temp;
}



/* handle_print_dialog ----------------------------------------
    öbernimmt die Userbedienung des Print-Dialogs
    ----------------------------------------------------------*/
static BOOLEAN handle_pdlg_dialog(EVNT *events)
{
	WORD button;

	if (prn_dialog)
	{
		if (pdlg_evnt(prn_dialog, prn_settings, events, &button) == 0)	/* Dialog schlieûen? */
		{
			close_print_dialog(FALSE);		/* Dialog schlieûen */

			if (button == PDLG_OK)		/* "Drucken" angewÑhlt? */
			{
				actualize_DevParam(prn_settings->driver_id, &DevParam);
				print_with_GDOS();
			}

			pdlg_free_settings(prn_settings);

			return TRUE;
		}
	}

	return FALSE;
}


static void make_windowtitle(void)
{
	char *picname;
	char temp[257];

	if ((picname = strrchr(pic_active->filename, '\\')) != NULL && *(picname + 1) != '\0')
		picname++;
	else
		picname = pic_active->filename;
	strcpy(print_window.wtitle, alerts[PRINT_TEXT2]);
	strcat(print_window.wtitle, " \"");
	memset(temp, 0, sizeof(temp));
	strcat(print_window.wtitle, shorten_name(picname, (short)(sizeof(print_window.wtitle) - 3 - strlen(print_window.wtitle)), temp));
	strcat(print_window.wtitle, "\"");
#if defined(__GEMLIB__) || defined(__PORTAES_H__)
	wind_set_str(print_window.whandlem, WF_NAME, print_window.wtitle);
#else
	wind_set(print_window.whandlem, WF_NAME, LONG2_2INT(print_window.wtitle), 0, 0);
#endif
}


void plugin_main(PLUGIN_DATA *data)
{
	char *picname;
	WORD out1, out2, out3, out4;
	BOOLEAN closed = FALSE;
	static short notify_num = 0;
	EVNT events;
	WORD back, i;
	DevInfoS *devinfo;

	/* Die Strukturen mit den Variablen und Funktionen holen */
	services = data->services;
	smurf_functions = data->smurf_functions;
	smurf_vars = data->smurf_vars;

	redraw_window = services->redraw_window;	/* Redrawfunktion */

	switch (data->message)
	{
		/* 
		 * Startup des Plugins: MenÅeintrag anfordern 
		 */
	case MSTART:
		my_id = data->id;
		init_rsh();
		strcpy(data->plugin_name, alerts[PRINT_TEXT1]);
		init_windstruct();
		data->wind_struct = &print_window;
		/* plugin responds to this menu entry */
		data->event_par[0] = FILE_PRINT;
		data->message = MENU_ENTRY;		/* MenÅeintrag anfordern */
		break;

	case PLG_STARTUP:
		data->message = M_WAITING;
		break;

		/*
		 * Plugin wurde aus dem MenÅ heraus aufgerufen 
		 */
	case PLGSELECTED:
		pdlg_available = appl_find("?AGI") >= 0 && appl_getinfo(7, &out1, &out2, &out3, &out4) != 0 && (out1 & 0x10) != 0;
		if (*(smurf_vars->picthere) == 0)
		{
			services->f_alert(alerts[NO_PIC], NULL, NULL, NULL, 1);
			data->message = M_EXIT;
		} else
		{
			smurf_picture = smurf_vars->smurf_picture;
			active_pic = smurf_vars->active_pic;

			pic_active = smurf_picture[*active_pic];

			OutParam.zx = 0;
			OutParam.zy = 0;
			OutParam.picwidth = pic_active->pic_width;
			OutParam.picheight = pic_active->pic_height;

			/*
			 * Druckdialog kreiern, initialisieren und îffnen 
			 */
			if (pdlg_available && (prn_dialog = pdlg_create(PDLG_3D)) != NULL)
			{
				prn_settings = pdlg_new_settings(prn_dialog);

				if ((picname = strrchr(pic_active->filename, '\\')) != NULL && *(picname + 1) != '\0')
					picname++;
				else
					picname = pic_active->filename;

				if ((pdlg_handle = pdlg_open(prn_dialog, prn_settings, picname, PDLG_PRINT, -1, -1)) == 0)
				{
					pdlg_delete(prn_dialog);
					prn_dialog = 0;
				} else if (notify_num == 0)
				{
					data->event_par[0] = pdlg_handle;
					data->message = ALL_MSGS;
					notify_num++;
					return;
				}
			}

			data->message = M_WAITING;

			if (pdlg_handle == 0)
			{
				strcpy(print_dialog[XPOS].TextCast, "0");
				strcpy(print_dialog[YPOS].TextCast, "0");
				itoa(OutParam.picwidth, print_dialog[WIDTH].TextCast, 10);
				itoa(OutParam.picheight, print_dialog[HEIGHT].TextCast, 10);
	
				/*
				 * Devices scannen und Infos Åber aktuelles GerÑt holen 
				 */
				if (scan_devs() == -1)
				{
					data->message = M_EXIT;
					return;
				}
	
				for (i = 0; i < dev_anzahl; i++)
				{
					if (DevInfo[i].devID == 91)
					{
						curr_deviceID = 91;
						break;
					}
				}
	
				if (curr_deviceID != 91)
					curr_deviceID = DevInfo[0].devID;
	
				if (actualize_DevParam(curr_deviceID, &DevParam) != -1)
					actualize_dialog(PAPER_REDRAW);
	
				/*
				 *  Fenster îffnen und Dialog initialisieren
				 */
				pic_active = smurf_picture[*active_pic];
				make_windowtitle();
				if (services->f_module_window(&print_window) < 0)
				{
					data->message = M_EXIT;
				} else
				{
					dialog_open = TRUE;
		
					if (popup_initialized == 0)
						init_driver_popup();
		
					/*printf("\nDriver Preselection"); */
					devinfo = get_devinfo(curr_deviceID);
					if (devinfo != NULL)
						strcpy(print_dialog[DRIVER_POPBUT].ob_spec.tedinfo->te_ptext, devinfo->devName);
		
					/*printf("\nPaper Preselection"); */
					/* Papierformat ermitteln */
					back = DevParam.paperform;
					/*printf("\nvoreingestelltes Papierformat: %i", back); */
					if (back >= PAGE_DEFAULT && back <= PAGE_B5)
						paper_popup.item = back + PAPER1;
					else
						paper_popup.item = back + PAPER6 - 16;
					strcpy(print_dialog[PAPER_POPBUT].TextCast, pappop_rsc[paper_popup.item].TextCast);
		
					strcpy(print_dialog[UNIT_POPBUT].TextCast, unitpop_rsc[UNIT_PIXELS].TextCast);
				}
			}
		}
		break;

	case DONE:
		data->message = M_WAITING;
		if (pdlg_handle != 0)
		{
			if (notify_num == 1)
			{
				data->event_par[0] = pdlg_handle;
				data->message = MBEVT;
				notify_num++;
			} else if (notify_num == 2)
			{
				data->event_par[0] = pdlg_handle;
				data->message = MKEVT;
				notify_num++;
			}
		}
		break;

		/*
		 * Fenster wurde geschlossen
		 */
	case MWINDCLOSED:
		dialog_open = FALSE;
		data->message = M_WAITING;
		break;

		/*
		 * button event
		 */
	case MBEVT:
		if (pdlg_handle != 0)
		{
			events.mwhich = MU_BUTTON;
			events.mx = data->mousex;
			events.my = data->mousey;
			events.mclicks = data->klicks;
			events.mbutton = data->event_par[1];
			closed = handle_pdlg_dialog(&events);
		} else
		{
			handle_print_dialog(data);
		}
		data->message = M_WAITING;
		break;

		/*
		 * key event
		 */
	case MKEVT:
		if (pdlg_handle != 0)
		{
			events.mwhich = MU_KEYBD;
			events.kstate = data->event_par[3];
			events.key = data->event_par[1];
			closed = handle_pdlg_dialog(&events);
		} else
		{
			handle_print_dialog(data);
		}
		data->message = M_WAITING;
		break;

	case SMURF_AES_MESSAGE:
		if (pdlg_handle)
		{
			events.mwhich = MU_MESAG;
			memcpy(events.msg, data->event_par, 16);
			closed = handle_pdlg_dialog(&events);
			data->message = M_WAITING;
		} else
		{
			data->message = handle_aesmsg(data->event_par);
		}
		break;

		/*
		 * aktives Bild hat sich geÑndert
		 */
	case MPIC_UPDATE:
		pic_active = smurf_picture[data->event_par[0]];
		if ((picname = strchr(pic_active->filename, '\\')) != NULL && *(picname + 1) != '\0')
			picname++;
		else
			picname = pic_active->filename;
		if (dialog_open)
		{
			actualize_dialog(POSITION_PAPER);
			make_windowtitle();
		} else if (pdlg_handle)
		{
			pdlg_update(prn_dialog, picname);
		}
		data->message = M_WAITING;
		break;

	case MTERM:
		close_print_dialog(TRUE);
		data->message = M_TERMINATED;
		break;

	default:
		data->message = M_WAITING;
		break;
	}

	if (closed)
		data->message = M_EXIT;
}
