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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/*	---------------------------------------------------------------------------	*/
/*                                                                              */
/*                           SMURF Grafikkonverter                              */
/*																				*/
/*  Copyright 1995-2000 Therapy Seriouz Software                                */
/*	                                                                            */
/*  Jedwede �hnlichkeit zu vorhandenen Schl�mpfen, Schlumpfinen oder Personen   */
/*  oder sonstigen, fiktiven oder realen, Lebewesen ist von uns, nicht in       */
/*  b�ser, sondern in guter Absicht, vors�tzlich eingebracht worden oder wurde  */
/*  oder wird eingebracht werden.                                               */
/*  Oder so ...                                                                 */
/*  --------------------------------------------------------------------------- */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __PUREC__
#include <ext.h>
#endif
#include <math.h>
#include <errno.h>
#include "import.h"
#include "demolib.h"
#include "startup.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "popdefin.h"
#include "plugin.h"
#include "smplugin.h"
#include "olgagems.h"
#include "smurfobs.h"
#include "ext_obs.h"
#include "debug.h"
#include "destruct.h"
#include "ext_rsc.h"

#include "xrsrc.h"
#include "globdefs.h"

#include "vaproto.h"

DISPLAY_MODES Display_Opt;
SERVICE_FUNCTIONS global_services;

#define BETA 0
#define LOW_POOL 1
#define HIGH_POOL 999

/* ------------------------------------------------------------*/
/*		 			Globale Variablendeklaration			   */
/* ------------------------------------------------------------*/
static unsigned char *mononct;			/* monochrome NCT (32KByte) */

WORD resource_global[100];
char **smurf_frstr;
BOOLEAN nullcoordset;
BOOLEAN syspalset;
WORD gl_hchar, gl_wchar, gl_hbox, gl_wbox;
short picwindthere;
short dialwindthere;
short picthere;

short openmode;							/* Dialog neu ge�ffnet (0) oder buttonevent? (!=0) */
BOOLEAN PCD;							/* PCD-Kennung f�r aktuell zu ladendes Bild */
WORD PCDwidth, PCDheight;				/* H�he und Breite des PCD-Bildes */

static WORD work_out[57];
WORD scrwd, scrht;						/* Screen Breite+H�he */
WORD mouse_xpos, mouse_ypos;			/* Mausposition */
WORD key_scancode;						/* Scancode beim letzten Keyboard-Event */
WORD key_ascii;
WORD obj;								/* Objekt beim loslassen des Buttons */

WORD mouse_button, key_at_event;

OBJECT *info_window;					/* Zeiger auf Resource  */
OBJECT *form_pop;						/* Zeiger auf Resource  */
OBJECT *col_pop;						/* Zeiger auf Resource-DITHERPOPUP  */
OBJECT *edit_pop;						/* Zeiger auf Resource  */
OBJECT *pic_form;						/* Pic-Window-Formular  */
OBJECT *pic_info_form;					/* Pic-Info-Formular    */
OBJECT *options_form;					/* Optionen-Formular    */
OBJECT *alert_form;						/* WindAlert-Formular   */
OBJECT *module_form;					/* Modul-Einstell-Formular  */
OBJECT *newpic_window;					/* Neues-Bild-Formular  */
OBJECT *export_form;					/* Export-Formular  */
OBJECT *exp_dp_popup;					/* Farbtiefen f�r Export    */
OBJECT *picorder_popup;
OBJECT *colred_popup;
OBJECT *blockpopup;
OBJECT *blockmode_window;
OBJECT *exmod_info;
OBJECT *image_order_window;
OBJECT *transform_window;
OBJECT *blocktype_window;

OBJECT *menu_tree;

OBJECT *u_tree;							/* Zeiger auf Radiobutton/Checkbox-Formular */

MFORM *lr_arrow, *ud_arrow, *lrud_arrow;
long f_len = 0x4b444100L;				/* L�nge des letzten geladenen Files 'KDA\0' */
WORD messagebuf[10];					/* Zeiger f�r Messageevents */
WORD klicks;							/* Anzahl Mausklicks beim letzten Buttonevent */

char *stpath;							/* Smurf-Standardpfad */
char loadpath[SM_PATH_MAX + 1];			/* voller Pfad der zuletzt geladenen Datei */
char savepath[SM_PATH_MAX + 1];			/* voller Pfad der zuletzt gespeicherten Datei */
char commpath[SM_PATH_MAX + 1];			/* voller Pfad der zuletzt �ber ein Protokoll empfangenen Datei */
char DraufschmeissBild = 0;

SYSTEM_INFO Sys_info;					/* Systemkonfiguration */
IMPORT_LIST Import_list;				/* Importmodul-Liste */

/* 15Bit-Systempalette aus NCT - werden in Sys_info eingeh�ngt!*/
static WORD red[SM_PALETTE_MAX];
static WORD grn[SM_PALETTE_MAX];
static WORD blu[SM_PALETTE_MAX];

/* die Original-Systempalette, wird bei Systemstart ausgelesen und
   danach nie mehr ver�ndert */
WORD orig_red[SM_PALETTE_MAX];
WORD orig_green[SM_PALETTE_MAX];
WORD orig_blue[SM_PALETTE_MAX];

uint8_t planetable[SM_PALETTE_MAX];

char *edit_modules[MAX_EDIT_MODS];				/* Pfade f�r bis zu 100 Edit-Module */
char *export_modules[MAX_EXPORT_MODS];			/* Pfade f�r bis zu 100 Export-Module */

char *export_cnfblock[50];				/* Konfigurationsbl�cke f�r die Exporter */
short export_cnflen[50];				/* L�nge des jeweiligen Blockes */

CROSSHAIR position_markers[20];			/* Positionsmarker f�r die Editmodule */

char *picnames[100];					/* BILDMANAGER: Namen f�r bis zu 100 Bilder */

int16_t anzahl_importmods = 0;			/* Anzahl an Import-Modulen */
int16_t anzahl_dithermods = 0;			/* Anzahl an Dither-Modulen */

long Name_Max;

/*----- Window-Variablen --------*/
WINDOW wind_s[25];
WINDOW picture_windows[MAX_PIC];

POP_UP popups[25];
SLIDER sliders[15];
unsigned char module_pics[MAX_MODS][7];

WORD Radio = RADIO;
WORD SelectedRadio = RADIO_SEL;
WORD Check = CHECK;
WORD SelectedCheck = CHECK_SEL;
WORD Cycle = CYCLE;
WORD SelectedCycle = CYCLE_SEL;
static long oldtim, tim;


GRECT screen;

WORD appl_id, menu_id;
SMURF_PIC *smurf_picture[MAX_PIC];
short active_pic;

BASPAG *Dithermod_Basepage[MAX_DITHER_MODS];			/* Basepages f�r Dithermodule */
const DITHER_MOD_INFO *ditmod_info[MAX_DITHER_MODS];		/* Ditherinfostrukturen f�r Dithermodule */

char *export_path;						/* Pfad des Export-Modules  */
short export_depth[8];
short export_format[8];

WORD TOOLBAR_HEIGHT;

char Smurf_locked;
char Startup;							/* Hochfahren des ganzen Monster-Systems */
static char startupdial_exist = FALSE;

short num_of_pics;

static long timer_fx_counter[10];
long timer_fx_max[10];
static void (*timer_fx_rout[10])(void);

WORD fix_red[SM_PALETTE_MAX];
WORD fix_blue[SM_PALETTE_MAX];
WORD fix_green[SM_PALETTE_MAX];

WORD sx, sy, sw, sh;
OBJECT *startrsc;
static WORD startuprsc_global[10];

/* --------------------------------------------------------------   */
/*  Funktion zum �ffnen einer virtuellen Bildschirmworkstation      */
/* --------------------------------------------------------------   */
static BOOLEAN open_vwork(void)
{
	WORD phys_handle;
	WORD work_in[16];
	int i;

	if ((appl_id = appl_init()) != -1)
	{
		for (i = 0; i < 16; work_in[i++] = 1)
			;
		work_in[10] = 2;
		work_in[11] = 0;
		work_in[12] = 0;
		work_in[13] = 0;
		work_in[14] = 0;
		work_in[15] = 0;
		phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
		Sys_info.vdi_handle = phys_handle;
		v_opnvwk(work_in, &Sys_info.vdi_handle, work_out);
		if (Sys_info.vdi_handle > 0)
			return TRUE;
	}
	return FALSE;
}


/*---- String im Startup-Dialog setzen und redrawen. Nur verwenden, wenn der Dialog offen ist! ---*/
void set_startupdial(const char *string)
{
	if (startupdial_exist)
	{
		strcpy(startrsc[STARTUP_TXT].TextCast, string);
		objc_draw(startrsc, STARTUP_TXT, 1, sx, sy, sw, sh);
	}
}



/* ------------------------------------------------------------*/
/* ------------------------------------------------------------*/
/*	 					void MAIN(void)						   */
/* ------------------------------------------------------------*/
/* ------------------------------------------------------------*/
int main(int argc, const char *argv[])
{
	char timer[4] = "___";
	short t;
	short tt;
	short back;
	GRECT desk;

	/*
	 * Virtuelle Workstation �ffnen
	 */
	if (open_vwork() == FALSE)
	{
		form_alert(1, "[3][Fehler beim �ffnen der VDI-Workstation!][Ups]");	/* FIXME: translate */
		return 1;
	}

	Sys_info.screen_width = work_out[0] + 1;
	Sys_info.screen_height = work_out[1] + 1;

	/* ausf�hrlichen systemweiten Namen setzen */
	/* Aber nur wenn Smurf als Accessory gestartet wurde */
	/* oder in einer Multitaskingumgebung l�uft, sonst */
	/* bringt es SingleTOS durcheinander. */
	if (_app == 0 || _AESnumapps != 1)
		menu_register(appl_id, "  Smurf");

	/* ausf�hrlichen systemweiten Namen setzen */
	menu_register(-1, "SMURF");

	Pdomain(1);							/* Smurf in die MiNT-Domain setzen */

	/*
	 * evtl. �bergebenen Dateinamen kopieren und Parameter auswerten
	 */
	if (argc > 1)						/* mindestens ein Argument? */
		send_AESMessage(appl_id, AP_ARGSTART, argc, LONG2_2INT(argv), -1);	/* Message an sich selbst */

	/* Ermittelt das Smurflaufwerk */
	GetSMPath();

	/*
	 * Startup-Dialog initialisieren
	 */
	desk.g_x = 0;
	desk.g_y = 0;
	desk.g_w = Sys_info.screen_width;
	desk.g_h = Sys_info.screen_height;
	init_xrsrc(Sys_info.vdi_handle, &desk, gl_wchar, gl_hchar);
	xrsrc_mustexist = FALSE;
	back = xrsrc_load("startup.rsc", startuprsc_global);
	if (back == TRUE)
		startupdial_exist = TRUE;

	if (startupdial_exist)
	{
		xrsrc_gaddr(R_TREE, 0, &startrsc, startuprsc_global);
		form_center(startrsc, &sx, &sy, &sw, &sh);
		form_dial(FMD_START, sx, sy, sw, sh, sx, sy, sw, sh);
		wind_update(BEG_UPDATE);
		objc_draw(startrsc, 0, MAX_DEPTH, sx, sy, sw, sh);
	}


	/*
	 * Smurf Resource Init
	 */
	/* FIXME: translate */
	set_startupdial("Lade und initialisiere Resource...");
	init_smurfrsc("smurf.rsc");

	/*
	 * Busy-Window links oben hin
	 */
	Dialog.busy.busyTree->ob_x = 0;
	Dialog.busy.busyTree->ob_y = 0;
	wind_s[WIND_BUSY].wnum = WIND_BUSY;
	wind_s[WIND_BUSY].wx = Dialog.busy.busyTree->ob_x;
	wind_s[WIND_BUSY].wy = Dialog.busy.busyTree->ob_y;
	wind_s[WIND_BUSY].ww = Dialog.busy.busyTree->ob_width;
	wind_s[WIND_BUSY].wh = Dialog.busy.busyTree->ob_height;
	wind_s[WIND_BUSY].editob = 0;
	wind_s[WIND_BUSY].nextedit = 0;
	wind_s[WIND_BUSY].editx = 0;

	/* Display-Optionen  - Defaultwerte eintragen */
	/* FIXME: translate */
	set_startupdial("Lade Konfiguration...");

	if (Dialog.dispOpt.tree[PAL_MOUSE].ob_state & OS_SELECTED)
		Display_Opt.palette_mode = PAL_MOUSE;
	else if (Dialog.dispOpt.tree[PAL_TOPWIN].ob_state & OS_SELECTED)
		Display_Opt.palette_mode = PAL_TOPWIN;
	else if (Dialog.dispOpt.tree[PAL_SYSTEM].ob_state & OS_SELECTED)
		Display_Opt.palette_mode = PAL_SYSTEM;

	/*
	 * CNF laden oder Defaultwerte eintragen
	 */
	if (load_config() == -1)
	{
		/* FIXME: translate */
		set_startupdial("Keine Konfigurationsdatei!");
		Sys_info.center_dialog = 0;
		Sys_info.profi_mode = 0;
		Sys_info.outcol = G_BLACK;
		Sys_info.dither24 = DIT1;
		Sys_info.dither8 = DIT1;
		Sys_info.dither4 = DIT1;
		Sys_info.pal4 = CR_SYSPAL;
		Sys_info.pal8 = CR_SYSPAL;
		Sys_info.pal24 = CR_SYSPAL;
		Sys_info.environment = 1;
		Sys_info.Event_Timer = 50;
		Sys_info.PCD_Defsize = BASE_D4;
		Sys_info.PreviewDither = DIT1;
		Sys_info.PicmanDither = DIT1;
		Sys_info.PreviewMoveDither = DIT1;
		Sys_info.AutoconvDither = DIT1;
		Sys_info.AutoconvPalmode = 1;
		Sys_info.busybox_icon = BUSYICON_1;
		Sys_info.defaultExporter = -1;

		Display_Opt.dither_24 = Sys_info.dither24;
		Display_Opt.dither_8 = Sys_info.dither8;
		Display_Opt.dither_4 = Sys_info.dither4;
		Display_Opt.syspal_4 = Sys_info.pal4;
		Display_Opt.syspal_8 = Sys_info.pal8;
		Display_Opt.syspal_24 = Sys_info.pal24;
		Display_Opt.palette_mode = PAL_MOUSE;
	}

	free(stpath);

	/* FIXME: translate */
	set_startupdial("Initialisiere Oberfl�che...");
	Dialog.smurfOpt.tree[OPT_CENTER].ob_state = Sys_info.center_dialog;
	Dialog.smurfOpt.tree[OPT_WINDOWALERT].ob_state = Sys_info.window_alert;
	Dialog.smurfOpt.tree[OPT_PROFI].ob_state = Sys_info.profi_mode;
	Dialog.smurfOpt.tree[OPT_PREVS].ob_state = Sys_info.immed_prevs;
	Dialog.smurfOpt.tree[REALTIME_DITHER].ob_state = Sys_info.realtime_dither;
	Dialog.smurfOpt.tree[ORIGINAL_CLIP].ob_state |= OS_DISABLED;

	/* Oberfl�chenbuttons deselektieren */
	Dialog.smurfOpt.tree[ENV_STANDARD].ob_state &= ~OS_SELECTED;
	Dialog.smurfOpt.tree[ENV_SILLY].ob_state &= ~OS_SELECTED;
	Dialog.smurfOpt.tree[ENV_THERAPY].ob_state &= ~OS_SELECTED;

	/* Palettenmodi deselektieren */
	Dialog.dispOpt.tree[PAL_MOUSE].ob_state &= ~OS_SELECTED;
	Dialog.dispOpt.tree[PAL_TOPWIN].ob_state &= ~OS_SELECTED;
	Dialog.dispOpt.tree[PAL_SYSTEM].ob_state &= ~OS_SELECTED;

	/* und den richtigen selektieren */
	switch (Display_Opt.palette_mode)
	{
	case PAL_MOUSE:
		Dialog.dispOpt.tree[PAL_MOUSE].ob_state |= OS_SELECTED;
		break;
	case PAL_TOPWIN:
		Dialog.dispOpt.tree[PAL_TOPWIN].ob_state |= OS_SELECTED;
		break;
	case PAL_SYSTEM:
		Dialog.dispOpt.tree[PAL_SYSTEM].ob_state |= OS_SELECTED;
		break;
	}

	/* Busybox-Icon einh�ngen */
	Dialog.busy.busyTree[BW_ICON].ob_spec.userblk->ub_parm = u_tree[Sys_info.busybox_icon].ob_spec.userblk->ub_parm;

	/* Gr��e der Toolbar bestimmen */
	TOOLBAR_HEIGHT = pic_form[0].ob_height;

	itoa(Sys_info.Event_Timer, timer, 10);
	memcpy(Dialog.dispOpt.tree[PAL_TIMER].TextCast, timer, 3);

	/*
	 * Zeiger und Felder initialisieren
	 */
	/* FIXME: translate */
	set_startupdial("Initialisiere System...");
	for (t = 0; t < 25; t++)
	{
		smurf_picture[t] = NULL;
		wind_s[t].whandlem = -1;
		picture_windows[t].whandlem = -1;
		if (t < MAX_MODS)
			module.smStruct[t] = NULL;
		Dialog.picMan.picmanList[t] = -1;
	}

	for (t = 0; t < 100; t++)
	{
		edit_modules[t] = NULL;
		Dialog.emodList.modNames[t] = NULL;
		edit_cnfblock[t] = NULL;
	}

	for (t = 0; t < 50; t++)
	{
		export_modules[t] = NULL;
		Dialog.expmodList.modNames[t] = NULL;
		export_cnfblock[t] = NULL;
	}

	for (t = 0; t < 10; t++)
	{
		timer_fx_max[t] = 0;
		timer_fx_counter[t] = 0;
		timer_fx_rout[t] = NULL;

		position_markers[t].anzahl = 0;

		for (tt = 0; tt < 6; tt++)
		{
			position_markers[t].xpos[tt] = 10;
			position_markers[t].ypos[tt] = 10;
			position_markers[t].mod_pic[tt] = -1;
			position_markers[t].smurfpic[tt] = -1;
		}
	}

	graf_mouse(ARROW, NULL);

	Startup = 1;

	tt = f_init_system();				/* System initialisieren */

	if (tt == -1)
	{
		if (startupdial_exist)
			wind_update(END_UPDATE);
		form_dial(FMD_FINISH, sx, sy, sw, sh, sx, sy, sw, sh);
		xrsrc_free(startuprsc_global);

		shutdown_smurf(0);
		return 1;
	}

	f_analyze_system();					/* was bin ich? */


	/*
	 * Preview-Ditherpopup im Optionsdialog vorinitialisieren
	 */
	strncpy(Dialog.smurfOpt.tree[PREVDIT_PB].TextCast, ditmod_info[Sys_info.PreviewDither - 1]->algo_name, 15);
	strncpy(Dialog.smurfOpt.tree[PICMANDIT_PB].TextCast, ditmod_info[Sys_info.PicmanDither - 1]->algo_name, 15);
	strncpy(Dialog.smurfOpt.tree[MOVE_PREVDIT_PB].TextCast, ditmod_info[Sys_info.PreviewMoveDither - 1]->algo_name, 15);
	strncpy(Dialog.smurfOpt.tree[AUTODIT_PB].TextCast, ditmod_info[Sys_info.AutoconvDither - 1]->algo_name, 15);
	strncpy(Dialog.smurfOpt.tree[AUTOPAL_PB].TextCast, colred_popup[Sys_info.AutoconvPalmode].TextCast, 15);

	/*
	 * Export-Ditherpopup vorinitialisieren
	 */
	strncpy(export_form[EXP_DITHER].TextCast, ditmod_info[0]->algo_name, 15);
	exp_conf.exp_dither = DIT1;
	exp_conf.exp_colred = CR_SYSPAL;

	/* 
	 * richtigen Oberfl�chenbutton selektieren und Userdefs init
	 */
	switch (Sys_info.environment)
	{
	case 1:
		Dialog.smurfOpt.tree[ENV_STANDARD].ob_state |= OS_SELECTED;
		Radio = RADIO;
		SelectedRadio = RADIO_SEL;
		Check = CHECK;
		SelectedCheck = CHECK_SEL;
		Cycle = CYCLE;
		SelectedCycle = CYCLE_SEL;
		if (Sys_info.OS & MATSCHIG)
		{
			Cycle = CYCLE_MAGX;
			SelectedCycle = CYCLE_SEL_MAGX;
		}
		break;
	case 2:
		Dialog.smurfOpt.tree[ENV_SILLY].ob_state |= OS_SELECTED;
		Radio = RADIO2;
		SelectedRadio = RADIO2_SEL;
		Check = CHECKICN2;
		SelectedCheck = CHECKICN2_SEL;
		Cycle = CYCLE_2;
		SelectedCycle = CYCLE_2_SEL;
		if (Sys_info.OS & MATSCHIG)
		{
			Cycle = CYCLE_MAGX;
			SelectedCycle = CYCLE_SEL_MAGX;
		}
		break;
	case 3:
		Dialog.smurfOpt.tree[ENV_THERAPY].ob_state |= OS_SELECTED;
		Radio = RADIO3;
		SelectedRadio = RADIO3_SEL;
		Check = CHECKICN3;
		SelectedCheck = CHECKICN3_SEL;
		Cycle = CYCLE_3;
		SelectedCycle = CYCLE_3_SEL;
		if (Sys_info.OS & MATSCHIG)
		{
			Cycle = CYCLE_MAGX;
			SelectedCycle = CYCLE_SEL_MAGX;
		}
		break;
	}


	/*
	 * Standardpfad f�rs erste �ffnen der Fileselectorbox
	 */
	strcpy(loadpath, Sys_info.standard_path);
	strcat(loadpath, "\\*.*");
	strcpy(savepath, Sys_info.standard_path);
	strcat(savepath, "\\*.*");

	Window.redraw(&wind_s[WIND_BUSY], NULL, 0, 0);

	/*
	 * Resource an Farbaufl�sung anpassen
	 */
	fix_rsc();
	set_menu_key(menu_tree);			/* Shortcuts Init */

	/*
	 * Popus, Slider, etc. initialisieren
	 */
	f_init_popups();
	f_init_sliders();

	popups[POPUP_PREVDIT].item = Sys_info.PreviewDither;
	popups[POPUP_PICMANDIT].item = Sys_info.PicmanDither;
	popups[POPUP_MOVEDIT].item = Sys_info.PreviewMoveDither;
	popups[POPUP_AUTODITHER].item = Sys_info.AutoconvDither;
	popups[POPUP_AUTOPAL].item = Sys_info.AutoconvPalmode;

	popups[POPUP_KODAK].item = Sys_info.PCD_Defsize;
	f_pop(&popups[POPUP_KODAK], 0, F_REDRAW, NULL);

	/*
	 * als ge�ffnet gespeicherte Fenster �ffnen
	 */
	for (t = 2; t <= 18; t++)
	{
		if ((t == WIND_MODULES && Dialog.emodList.anzahl == 0) || (t == WIND_EXPORT && Dialog.expmodList.anzahl == 0))
			continue;

		if (Sys_info.dialog_opened[t])
			if (t != WIND_ALERT && t != WIND_MODFORM)
				Window.open(&wind_s[t]);
	}


	/*
	 * Environment auslesen und Protokolle initialisieren
	 */
	Comm.getAvserv();
	Comm.initAVPROTO();
	Comm.initOLGA();
	Comm.initbubbleGem();

	/*
	 * Importerliste initialisieren
	 */
	anzahl_importmods = -1;

	if (load_import_list() != 0)		/* keine da? */
	{
		/* FIXME: translate */
		t = Dialog.winAlert.openAlert("Keine Importmodul-Liste gefunden. Jetzt generieren?", " Ja ", "Nein", NULL, 1);
		if (t == 1)
			f_scan_import();
		else
			/* FIXME: translate */
			Dialog.winAlert.openAlert("Ohne eine Importerliste wird das Laden von Bildern deutlich l�nger dauern. Generieren Sie im Optionsdialog die Liste.", NULL, NULL, NULL, 1);
	}

	Startup = 0;

	Dialog.dispOpt.updateWindow(1, 1);
	Dialog.busy.dispRAM();

	if (!Sys_info.dialog_opened[WIND_BUSY])
		Dialog.close(WIND_BUSY);

	f_init_menu();
	actualize_menu();					/* und aktualisieren */


	f_event();							/* ----- Und los gehts! ----- */


	shutdown_smurf(0);

	return 0;
}


/* ----------------------------------------------------------------	*/
/* --------------------- Systempalette setzen ---------------------	*/
/* ----------------------------------------------------------------	*/
void f_set_syspal(void)
{
	short t;
	short maxcol;
	WORD rgb[3];

	if (!syspalset && Sys_info.bitplanes <= 8)
	{
		if (wind_update(BEG_UPDATE) == 0)
			return;

		maxcol = Sys_info.Max_col;

		for (t = 0; t <= maxcol; t++)
		{
			rgb[0] = Sys_info.pal_red[t];
			rgb[1] = Sys_info.pal_green[t];
			rgb[2] = Sys_info.pal_blue[t];
			vs_color(Sys_info.vdi_handle, t, rgb);
		}

		syspalset = TRUE;

		wind_update(END_UPDATE);
	}
}


/* ----------------------------------------------------------------	*/
/* --------------------- Bildpalette setzen -----------------------	*/
/* ----------------------------------------------------------------	*/
void f_set_picpal(SMURF_PIC * pic)
{
	short t;
	short maxcol;
	WORD rgb[3];

	if (Sys_info.bitplanes <= 8 && Display_Opt.palette_mode != PAL_SYSTEM)
	{
		if (wind_update(BEG_UPDATE) == 0)
			return;

		maxcol = Sys_info.Max_col;

		for (t = 0; t <= maxcol; t++)
		{
			rgb[0] = (WORD) ((long) pic->red[t] * 1000 / 255);
			rgb[1] = (WORD) ((long) pic->grn[t] * 1000 / 255);
			rgb[2] = (WORD) ((long) pic->blu[t] * 1000 / 255);
			vs_color(Sys_info.vdi_handle, t, rgb);
		}

		syspalset = FALSE;

		wind_update(END_UPDATE);
	}
}


/* ----------------------------------------------------------------	*/
/* ----------------------------- Bildinfo -------------------------	*/
/* ----------------------------------------------------------------	*/
void f_pic_info(void)
{
	char *file;
	char dummy[30] = "";
	short picdepth;
	static SMURF_PIC infoprev;
	SMURF_PIC *pic;
	OBJECT *infowindow;

	if (wind_s[WIND_PICINFO].whandlem == -1 && openmode)	/* wenn Infofenster zu, nix zu tun */
		return;

	if (smurf_picture[active_pic] != NULL)
	{
		infowindow = wind_s[WIND_PICINFO].resource_form;

		pic = picture_windows[active_pic].picture;

		memcpy(&infoprev, pic, sizeof(SMURF_PIC));
		infoprev.local_nct = NULL;
		infoprev.screen_pic = NULL;
		wind_s[WIND_PICINFO].picture = &infoprev;

		infoprev.zoom = compute_zoom(pic, infowindow[INFOPREV].ob_width, infowindow[INFOPREV].ob_height);

		wind_s[WIND_PICINFO].pic_xpos = infowindow[INFOPREV].ob_x;
		wind_s[WIND_PICINFO].pic_ypos = infowindow[INFOPREV].ob_y;
		wind_s[WIND_PICINFO].clipwid = pic->pic_width / (infoprev.zoom + 1);
		wind_s[WIND_PICINFO].cliphgt = pic->pic_height / (infoprev.zoom + 1);
		wind_s[WIND_PICINFO].xoffset = 0;
		wind_s[WIND_PICINFO].yoffset = 0;

		memset(infowindow[INFO_NAME].TextCast, 0x0, 25);
		memset(infowindow[INFO_FORMAT].TextCast, 0x0, 25);
		memset(infowindow[INFO_SIZE].TextCast, 0x0, 17);
		memset(infowindow[INFO_DEPTH].TextCast, 0x0, 6);
		memset(infowindow[INFO_FILESIZE].TextCast, 0x0, 10);
		memset(infowindow[INFO_RAMSIZE].TextCast, 0x0, 10);
		memset(infowindow[COMMENT1].TextCast, 0x0, 40);
		memset(infowindow[COMMENT2].TextCast, 0x0, 40);
		memset(infowindow[COMMENT3].TextCast, 0x0, 40);

		file = picture_windows[active_pic].wtitle + 12;
		strncpy(infowindow[INFO_NAME].TextCast, file, 25);

		strncpy(infowindow[INFO_FORMAT].TextCast, pic->format_name, 25);

		strcpy(infowindow[INFO_SIZE].TextCast, itoa(pic->pic_width, dummy, 10));
		strcat(infowindow[INFO_SIZE].TextCast, "*");
		strcat(infowindow[INFO_SIZE].TextCast, itoa(pic->pic_height, dummy, 10));
		strcat(infowindow[INFO_SIZE].TextCast, " Pixel");

		picdepth = pic->depth;

		strcpy(infowindow[INFO_DEPTH].TextCast, itoa(picdepth, dummy, 10));
		strcat(infowindow[INFO_DEPTH].TextCast, " Bit");

		if (picdepth < 16)
			strcpy(infowindow[INFO_COLORS].TextCast, itoa(1 << picdepth, dummy, 10));
		else if (picdepth == 16)
			strcpy(infowindow[INFO_COLORS].TextCast, "Muchos tausendos");	/* FIXME: translate */
		else if (picdepth == 24)
			strcpy(infowindow[INFO_COLORS].TextCast, "Muchas millionas");	/* FIXME: translate */

		if (pic->format_type == FORM_STANDARD)
			strcpy(infowindow[INFO_DATAFORM].TextCast, "Standardformat");	/* FIXME: translate */
		else
			strcpy(infowindow[INFO_DATAFORM].TextCast, "Pixel packed");	/* FIXME: translate */

		if (pic->col_format == RGB)
			strcpy(infowindow[INFO_COLSYS].TextCast, "RGB");
		else if (pic->col_format == BGR)
			strcpy(infowindow[INFO_COLSYS].TextCast, "BGR");
		else if (pic->col_format == CMY)
			strcpy(infowindow[INFO_COLSYS].TextCast, "CMY");
		else if (pic->col_format == YIQ)
			strcpy(infowindow[INFO_COLSYS].TextCast, "YIQ");
		else if (pic->col_format == GREY)
			strcpy(infowindow[INFO_COLSYS].TextCast, "Grau");	/* FIXME: translate */
		else if (pic->col_format == WURSCHT)
			strcpy(infowindow[INFO_COLSYS].TextCast, "?");

		strcpy(infowindow[INFO_FILESIZE].TextCast, ltoa(pic->file_len, dummy, 10));
		if (pic->format_type == FORM_PIXELPAK)
			ltoa((long) pic->pic_width * (long) pic->pic_height * (long) picdepth / 8L, dummy, 10);
		else
			ltoa(((long) pic->pic_width + 7) / 8L * (long) pic->pic_height * (long) picdepth, dummy, 10);
		strcpy(infowindow[INFO_RAMSIZE].TextCast, dummy);

		/*
		 * Infotext eintragen
		 */
		strncpy(infowindow[COMMENT1].TextCast, pic->infotext, 43);
		strncpy(infowindow[COMMENT2].TextCast, pic->infotext + 43, 43);
		strncpy(infowindow[COMMENT3].TextCast, pic->infotext + 43, 42);
	}

	/*
	 * Fenster noch nicht auf? Dann �ffnen, ansonsten nur redraw.
	 */
	if (wind_s[WIND_PICINFO].whandlem <= 0)
		Window.open(&wind_s[WIND_PICINFO]);
	else
	{
#if 0
		DISPLAY_MODES old;

		make_singular_display(&old, Sys_info.PreviewDither, CR_SYSPAL);
#endif
		Window.redraw(&wind_s[WIND_PICINFO], NULL, 0, 0);
#if 0
		restore_display(&old);
#endif
	}
}



/*---------------------------------------------------------------------------------	*/
/* insert_float																	*/
/*	f�gt eine 64Bit-Flie�kommazahl val als Text in den Objektbaum tree, TEXTobjekt	*/
/*	ein. Der String hat hinterher grunds�tzlich 9 Stellen, bei variabler Mantisse.	*/
/*---------------------------------------------------------------------------------	*/
static void insert_float(OBJECT * tree, WORD object, float val)
{
	char str[40];
	char str2[40];
	int dec, sign;
	double fval;

	memset(str, 0, sizeof(str));
	memset(str2, 0, sizeof(str2));
	fval = val;

	ftoa(&fval, str, 10, 1, &dec, &sign);

	if (dec > 0)
		strncpy(str2, str, dec);
	else
		strcpy(str2, "0");

	if (dec < 0)
		dec = 0;
	strcat(str2, ".");

	strcat(str2, str + dec);

	strncpy(tree[object].TextCast, str2, 9);
}



/*----------------------------------------------------------------------*/
/*						Formular "Neues Bild"							*/
/*----------------------------------------------------------------------*/
void f_newpic(WORD scancode)
{
	char strn[10];
	static WORD unit = UNIT_PIXELS;
	WORD button;
	WORD wid;
	WORD hgt;
	WORD dep;
	WORD dpi;
	WORD newunit;
	float oldwid;
	float oldhgt;
	float newwid;
	float newhgt;
	float conv_factor;
	OBJECT *new_form;

	new_form = wind_s[WIND_NEWPIC].resource_form;

	if (!openmode)
	{
		Window.open(&wind_s[WIND_NEWPIC]);
		button = 0;
	} else
		button = obj;

	if (scancode == -1)
	{
		if (button == NP_DEPTHPOP || button == NPDEPTH_CB)
		{
			f_pop(&popups[POPUP_NPDEPTH], 0, button, NULL);
			f_deselect_popup(&wind_s[WIND_NEWPIC], NP_DEPTHPOP, NPDEPTH_CB);
		}
	}

	if (button == DIALOG_EXIT || button == NEWPIC_MAKE)
	{
		dpi = atoi(new_form[NEWPIC_DPI].TextCast);
		newwid = atof(new_form[NEWPIC_WID].TextCast);
		newhgt = atof(new_form[NEWPIC_HGT].TextCast);

		if (unit != UNIT_PIXELS)
			conv_factor = convert_units(unit, UNIT_PIXELS, dpi);
		else
			conv_factor = 1;

		wid = newwid * conv_factor;
		hgt = newhgt * conv_factor;

		switch (popups[POPUP_NPDEPTH].item)
		{
		case DEPTH1:
			dep = 1;
			break;
		case DEPTH2:
			dep = 2;
			break;
		case DEPTH4:
			dep = 4;
			break;
		case DEPTH8:
			dep = 8;
			break;
		case DEPTH16:
			dep = 16;
			break;
		case DEPTH24:
			dep = 24;
			break;
		default:
			dep = 24;
			break;
		}

		change_object(&wind_s[WIND_NEWPIC], NEWPIC_MAKE, OS_UNSEL, 1);
		Dialog.close(WIND_NEWPIC);
		f_generate_newpic(wid, hgt, dep);
		Dialog.busy.ok();
	} else if (button == NP_UNITPB || button == NP_UNITCB)
	{
		dpi = atoi(new_form[NEWPIC_DPI].TextCast);
		oldwid = atof(new_form[NEWPIC_WID].TextCast);
		oldhgt = atof(new_form[NEWPIC_HGT].TextCast);

		newunit = f_pop(&popups[POPUP_UNIT], 0, button, NULL);
		f_deselect_popup(&wind_s[WIND_NEWPIC], NP_UNITPB, NP_UNITCB);

		if (newunit != -1 && newunit != unit)
		{
			/* neue Gr��e nach der Einheitenkonvertierung ausrechnen */
			conv_factor = convert_units(unit, newunit, dpi);
			newwid = oldwid * conv_factor;
			newhgt = oldhgt * conv_factor;

			unit = newunit;

			if (unit != UNIT_PIXELS)
			{
				insert_float(new_form, NEWPIC_WID, newwid);
				insert_float(new_form, NEWPIC_HGT, newhgt);
			} else
			{
				memset(strn, 0x0, 10);
				itoa(newwid, strn, 10);
				strcpy(new_form[NEWPIC_WID].TextCast, strn);

				memset(strn, 0x0, 10);
				itoa(newhgt, strn, 10);
				strcpy(new_form[NEWPIC_HGT].TextCast, strn);
			}

			Window.redraw(&wind_s[WIND_NEWPIC], NULL, NEWPIC_WID, 0);
			Window.redraw(&wind_s[WIND_NEWPIC], NULL, NEWPIC_HGT, 0);
		}
	}
}


/*----------------------------------------------------------------------*/
/*						Neues Bild generieren 							*/
/*----------------------------------------------------------------------*/
SMURF_PIC *f_generate_newpic(WORD wid, WORD hgt, WORD depth)
{
	uint8_t *palette;
	short pic_to_make = 1;
	short tt, index;
	short aligned_width;
	long PicLen;

	/* erstes freies Bild ermitteln */
	while (smurf_picture[pic_to_make] != NULL)
		pic_to_make++;
	if (pic_to_make > MAX_PIC)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_PIC_FREE].TextCast, NULL, NULL, NULL, 1);
		return 0;
	}

	if ((smurf_picture[pic_to_make] = alloc_smurfpic(NULL, FALSE)) == NULL)
		return NULL;

	if (depth >= 8)
	{
		PicLen = (long) ((long) wid * (long) hgt) * (long) depth / 8L;
	} else
	{
		aligned_width = (wid + 7) >> 3;
		PicLen = (long) ((long) aligned_width * (long) hgt * (long) depth);
	}

	if ((smurf_picture[pic_to_make]->pic_data = SMalloc(PicLen)) == NULL)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOMEM_NEWPIC].TextCast, NULL, NULL, NULL, 1);
		SMfree(smurf_picture[pic_to_make]);
		smurf_picture[pic_to_make] = NULL;
	} else
	{
		if (depth <= 8)
			memset(smurf_picture[pic_to_make]->pic_data, 0x0, PicLen);
		else
			memset(smurf_picture[pic_to_make]->pic_data, 0xff, PicLen);

		make_smurf_pic(pic_to_make, wid, hgt, depth, smurf_picture[pic_to_make]->pic_data);
		make_pic_window(pic_to_make, wid, hgt, "Namenlos"); /* FIXME: translate */

		smurf_picture[pic_to_make]->col_format = RGB;
		strcpy(smurf_picture[pic_to_make]->format_name, "-");
		strcpy(smurf_picture[pic_to_make]->filename, "C:\\Namenlos");

		/*
		 * pr�ventiv die Systempalette eintragen SMURF_PIC
		 */
		palette = smurf_picture[pic_to_make]->palette;
		if (depth == 1)
		{
			palette[0] = palette[1] = palette[2] = 255;
			palette[3] = palette[4] = palette[5] = 0;
		} else
		{
			for (tt = 0; tt <= Sys_info.Max_col; tt++)
			{
				index = planetable[tt];
				*(palette + 0 + index * 3) = (orig_red[tt] * 255L) / 1000L;
				*(palette + 1 + index * 3) = (orig_green[tt] * 255L) / 1000L;
				*(palette + 2 + index * 3) = (orig_blue[tt] * 255L) / 1000L;
			}
		}

		if (!Sys_info.realtime_dither)
			f_dither(smurf_picture[pic_to_make], &Sys_info, 0, NULL, &Display_Opt);

		Window.open(&picture_windows[pic_to_make]);
		imageWindow.clipPicwin(&picture_windows[pic_to_make]);
		Dialog.picMan.insertPic(pic_to_make);
		f_activate_pic(pic_to_make);

		picthere++;

		Dialog.busy.dispRAM();			/* wieviel Ram? */
		actualize_menu();				/* Men�eintr�ge ENABLEn / DISABLEn */
	}

	return smurf_picture[pic_to_make];
}



/*------------------------ Generieren und F�llen von Strukturen ---------------------------	*/
/*					WINDOW und SMURF_PIC f�r neues oder geladenes Bild						*/

void make_smurf_pic(short pic_to_make, WORD wid, WORD hgt, WORD depth, char *picdata)
{
	long PicLen;

	PicLen = ((long) wid * (long) hgt * (long) depth) / 8L;

	smurf_picture[pic_to_make]->pic_width = wid;
	smurf_picture[pic_to_make]->pic_height = hgt;
	smurf_picture[pic_to_make]->depth = depth;
	smurf_picture[pic_to_make]->pic_data = picdata;
	smurf_picture[pic_to_make]->file_len = PicLen;
	smurf_picture[pic_to_make]->zoom = 0;
	smurf_picture[pic_to_make]->block = NULL;
	smurf_picture[pic_to_make]->prev_picture = NULL;
	smurf_picture[pic_to_make]->next_picture = NULL;
	smurf_picture[pic_to_make]->local_nct = NULL;

	memset(smurf_picture[pic_to_make]->infotext, 0, sizeof(smurf_picture[pic_to_make]->infotext));

	if (depth < 8)
		smurf_picture[pic_to_make]->format_type = FORM_STANDARD;
	else
		smurf_picture[pic_to_make]->format_type = FORM_PIXELPAK;
}


/* make_pic_window ------------------------------------------------
	Erzeugt eine WINDOW-Struktur f�r das Bild mit der Nummer pic_to_make
	mit der Gr��e wid/hgt und dem Namen name. Ein Fenstertitel wird
	erzeugt, das Bild eingeh�ngt und pflag auf 1 gesetzt.
	----------------------------------------------------------------*/
void make_pic_window(short pic_to_make, WORD wid, WORD hgt, char *name)
{
	char widstr[6];
	char hgtstr[6];
	short whlen;
	WORD abx, aby, abw, abh;
	WORD flags;
	WORD dummy;

	picture_windows[pic_to_make].editob = 0;
	picture_windows[pic_to_make].editx = 0;
	picture_windows[pic_to_make].nextedit = 0;
	picture_windows[pic_to_make].module = 0;

	itoa(wid, widstr, 10);
	itoa(hgt, hgtstr, 10);
	strncpy(picture_windows[pic_to_make].wtitle, widstr, 5);
	strncat(picture_windows[pic_to_make].wtitle, "*", 1);
	strncat(picture_windows[pic_to_make].wtitle, hgtstr, 5);

	whlen = (short) strlen(picture_windows[pic_to_make].wtitle);
	strncat(picture_windows[pic_to_make].wtitle, "            ", 12 - whlen);
	strcat(picture_windows[pic_to_make].wtitle, shorten_name(name, 41 - (short) strlen(picture_windows[pic_to_make].wtitle)));
#if 0
	strncat(picture_windows[pic_to_make].wtitle, name, 40);
#endif

	Window.windGet(0, WF_WORKXYWH, &abx, &aby, &abw, &abh);

	picture_windows[pic_to_make].wnum = pic_to_make;
	picture_windows[pic_to_make].wx = abx + ((pic_to_make - 1) << 4);
	picture_windows[pic_to_make].wy = aby + ((pic_to_make - 1) << 4);

	/*
	 * aus den Bruttokoordinaten Nettokoordinaten machen
	 */
	flags = CLOSER | NAME | MOVER | SMALLER;
	flags |= FULLER | SIZER | UPARROW | DNARROW | LFARROW | RTARROW | VSLIDE | HSLIDE;
	wind_calc(WC_WORK, flags, picture_windows[pic_to_make].wx, picture_windows[pic_to_make].wy, 100, 100,
		&picture_windows[pic_to_make].wx, &picture_windows[pic_to_make].wy, &dummy, &dummy);

	picture_windows[pic_to_make].ww = wid;
	picture_windows[pic_to_make].wh = hgt + TOOLBAR_HEIGHT;

	picture_windows[pic_to_make].resource_form = pic_form;
	picture_windows[pic_to_make].picture = smurf_picture[pic_to_make];
	picture_windows[pic_to_make].xoffset = 0;
	picture_windows[pic_to_make].yoffset = 0;
	picture_windows[pic_to_make].pic_xpos = 0;
	picture_windows[pic_to_make].pic_ypos = TOOLBAR_HEIGHT;
	picture_windows[pic_to_make].clipwid = wid;
	picture_windows[pic_to_make].cliphgt = hgt;

	/*
	 * Block vernullen
	 */
	picture_windows[pic_to_make].picture->blockx = 0;
	picture_windows[pic_to_make].picture->blocky = 0;
	picture_windows[pic_to_make].picture->blockwidth = 0;
	picture_windows[pic_to_make].picture->blockheight = 0;
	picture_windows[pic_to_make].pflag = 1;
}



/* ------------------------------------------------------------------------	*/
/*					Buttonevent im Modulfenster behandeln					*/
/*	Verarbeitet B/K-Events in Modulfenstern. MBEVT/MKEVT wird ans Modul		*/
/*	�bergeben, bei Return von M_STARTED werden die vom Modul ben�tigten		*/
/*	Bilder ans Modul �bergeben bzw. konvertiert und das Modul gestartet.	*/
/* ------------------------------------------------------------------------	*/
static WORD do_MBEVT(short module_number, WINDOW *mod_win, int mode)
{
	uint8_t *picture;
	long mod_magic;
	WORD back;
	WORD oldtop = 0;
	short how_many_pix;
	short picnum;
	short mod_index;
	long len;
	EXPORT_PIC *pic_to_save;
	SMURF_PIC *edited_pic;
	const MOD_INFO *mod_info;
	const MOD_ABILITY *mod_abs;

	mod_index = module_number & MOD_ID_MASK;

	/*
	 * Modulanalyse
	 */
	if (!(module_number & MOD_PLUGIN))		/* kein Plugin? */
	{
		const MODULE_START *module_start;

		module_start = get_module_start(module.bp[mod_index]);
		mod_magic = get_modmagic(module.bp[mod_index]);
		mod_info = module_start->info;
		mod_abs = module_start->ability;
		how_many_pix = mod_info->how_many_pix;

		module.smStruct[mod_index]->smurf_pic = smurf_picture[active_pic];

		if (smurf_picture[active_pic])
			if (smurf_picture[active_pic]->block != NULL)
				module.smStruct[mod_index]->smurf_pic = smurf_picture[active_pic]->block;
	} else
	{
		mod_magic = get_modmagic(plugin_bp[mod_index]);
		mod_info = NULL;
		mod_abs = NULL;
		how_many_pix = 0;
	}


	/*
	 * Edit-Modul?
	 */
	if (mod_magic == MOD_MAGIC_EDIT)
	{
		/* Buttonevent �bergeben --- */
		module.comm.start_edit_module("", module.bp[mod_index], mode, mod_index, module.smStruct[mod_index]);

		if (module.smStruct[mod_index]->module_mode == M_STARTED && (picthere || mod_info->how_many_pix == 0))
		{
			/*
			 * Default-Konfigurationsblock anfordern und merken
			 */
			module.comm.start_edit_module(edit_modules[mod_index], module.bp[mod_index], GETCONFIG, mod_index, module.smStruct[mod_index]);
			if (module.smStruct[mod_index]->module_mode == M_CONFIG)
				memorize_emodConfig(module.bp[mod_index], module.smStruct[mod_index]);

			module.smStruct[mod_index]->event_par[0] = position_markers[module_number & MOD_ID_MASK].xpos[0];
			module.smStruct[mod_index]->event_par[1] = position_markers[module_number & MOD_ID_MASK].ypos[0];
			module.comm.start_edit_module("", module.bp[mod_index], MCH_COORDS, mod_index, module.smStruct[mod_index]);

			back = 0;
			if (mod_info->how_many_pix > 1)
				back = f_give_pics(mod_info, mod_abs, module_number);
			else
				f_convert(smurf_picture[active_pic], mod_abs, RGB, SAME, 0);

			if (back == 0)
			{
				/*
				 * Bilder �bergeben, -> karierte Flagge f�rs Modul
				 */
				Window.windGet(0, WF_TOP, &oldtop, 0, 0, 0);
				Window.top_window_now(&wind_s[WIND_BUSY]);
				Window.redraw(&wind_s[WIND_BUSY], NULL, 0, 0);
				Dialog.busy.reset(0, mod_win->wtitle);
				module.smStruct[mod_index]->event_par[0] = 0;
				module.comm.start_edit_module(edit_modules[mod_index], module.bp[mod_index], MEXEC, mod_index, module.smStruct[mod_index]);
			}
		}

		back = 0;
	}

	/*
	 * Export-Modul?
	 */
	else if (mod_magic == MOD_MAGIC_EXPORT)
	{
		pic_to_save = module.comm.start_exp_module("", mode, smurf_picture[active_pic], module.bp[mod_index], module.smStruct[mod_index], module_number);

		if (module.smStruct[mod_index]->module_mode == M_DONEEXIT ||
			module.smStruct[mod_index]->module_mode == M_PICDONE)
		{

			picture = pic_to_save->pic_data;
			len = pic_to_save->f_len;
			file_save("Bild Speichern", picture, len); /* FIXME: translate */
			SMfree(pic_to_save);
			SMfree(picture);
			Dialog.busy.dispRAM();
		}
	}

	/*
	 * Plugin?
	 */
	else if (mod_magic == MOD_MAGIC_PLUGIN)
	{
		plg_data[mod_index]->event_par[0] = obj;

		if (mode == MBEVT)
		{
			plg_data[mod_index]->mousex = mouse_xpos;
			plg_data[mod_index]->mousey = mouse_ypos;
			plg_data[mod_index]->klicks = klicks;
			plg_data[mod_index]->event_par[1] = mouse_button;
		}

		if (mode == MKEVT)
		{
			plg_data[mod_index]->event_par[1] = key_scancode;
			plg_data[mod_index]->event_par[2] = key_ascii;
		}

		plg_data[mod_index]->event_par[3] = key_at_event;

		return start_plugin(plugin_bp[mod_index], mode, mod_index | MOD_PLUGIN, plg_data[mod_index]);
	}

	if ((mod_magic == MOD_MAGIC_EDIT || mod_magic == MOD_MAGIC_EXPORT) && module.smStruct[mod_index])
		f_handle_modmessage(module.smStruct[mod_index]);	/* handlen von MBEVT-Antworten */

	/*
	 * das Edit-Modul ist fertig mit dem Bildbearbeiten und Smurf mu� entsprechend reagieren
	 */
	if (mod_magic == MOD_MAGIC_EDIT &&
		(module.smStruct[mod_index]->module_mode == M_PICDONE ||
		 module.smStruct[mod_index]->module_mode == M_DONEEXIT))
	{
		Dialog.busy.dispRAM();

		/*
		 * wenn mehrere Bilder �bergeben wurden, welches wurde bearbeitet?
		 */
		if (how_many_pix > 1)
		{
			picnum = module.smStruct[mod_index]->event_par[0];
			picnum = module_pics[mod_index][picnum];
			edited_pic = smurf_picture[picnum];
		} else
		{
			picnum = active_pic;
			edited_pic = smurf_picture[active_pic];
		}

		if (edited_pic->block != NULL)
		{
			/*
			 * f�r den Fall, da� das Modul einen Block erzeugt oder bearbeitet hat: 
			 */
			edited_pic->blockwidth = edited_pic->block->pic_width;
			edited_pic->blockheight = edited_pic->block->pic_height;
			edited_pic->block->zoom = edited_pic->zoom;
			edited_pic->block->changed = 255;

			edited_pic = edited_pic->block;
		}

		Window.top_window_now(&wind_s[WIND_BUSY]);
		Window.redraw(&wind_s[WIND_BUSY], NULL, 0, 0);

		imageWindow.clipPicwin(&picture_windows[picnum]);
		picture_windows[picnum].clipwid = smurf_picture[picnum]->pic_width;
		picture_windows[picnum].cliphgt = smurf_picture[picnum]->pic_height;

		/* damit bei Bild�nderungen im Fall von Median-Cut */
		/* auch die Palette neu berechnet wird und sowieso */
		if (edited_pic->local_nct)
		{
			SMfree(edited_pic->local_nct);
			edited_pic->local_nct = NULL;
		}

		if (!Sys_info.realtime_dither)
			if (f_dither(edited_pic, &Sys_info, 1, NULL, &Display_Opt) == -1)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DIT_ERROR].TextCast, NULL, NULL, NULL, 1);

		Window.redraw(&picture_windows[picnum], NULL, 0, 0);
		f_pic_changed(&picture_windows[picnum], 1);
		Dialog.busy.dispRAM();
		inform_modules(MPIC_UPDATE, smurf_picture[active_pic]);
	}

	if ((mod_magic == MOD_MAGIC_EDIT || mod_magic == MOD_MAGIC_EXPORT) && module.smStruct[mod_index])
		check_and_terminate(module.smStruct[mod_index]->module_mode, mod_index);

	Window.top_windowhandle(oldtop);
	Dialog.busy.dispRAM();
	return 0;
}



/*----------------------------------------------------------------------*/
/*								Eventhandler							*/
/*	Das Hirn des Hauptprogramms. Hier werden alle Events aufgenommen,	*/
/*	Aktionen wie Radiobuttonbehandlung, UDOs, etc. ausgef�hrt und die	*/
/*	Events an die jeweiligen Dialogfunktionen oder Module 				*/
/*	weitergeleitet.														*/
/*----------------------------------------------------------------------*/
void f_event(void)
{
	BOOLEAN itsok;
	WORD bb_icon;
	WORD back;
	WORD message_back = 0;
	WORD newedit;
	WORD topwin;
	short windownum;
	WORD klickobj = 0;
	short windnum = 0;
	WORD windh = 0;
	WORD topw_num;
	WORD ev_flags;
	WORD old_mx = -1;
	WORD old_my = -1;
	WORD mex = 0;
	WORD mey = 0;
	WORD mex2 = 0;
	WORD mey2 = 0;
	short module_number;
	WORD title;
	WORD item;
	WORD keyboard_back;
	WORD key_object;
	WORD klickhandle;
	WORD timer_event;
	short t;
	short tt;
	WORD mbutton;
	WORD dummy;
	BOOLEAN pm_to_redraw = FALSE;

	void (*timerrout)(void);

	KINFO ki;
	OBJECT *ob;
	WINDOW *picwin;
	WINDOW *mod_win = NULL;
	static WINDOW *oldpicwin;
	static short oldwindnum;
	short mod_desire;
	short mod_index;

	DEBUG_MSG(("f_event...\n"));

	key_scancode = 0;
	obj = 0;

	ob = wind_s[WIND_DOPT].resource_form;
	timer_event = atoi(ob[PAL_TIMER].TextCast);
	if (timer_event == 0)
		timer_event = 20;

	/* Main-Schleife bis PRG_CLOSED */
	do
	{
		Window.windGet(0, WF_TOP, &topwin, &dummy, &dummy, &dummy);	/* TOPWIN ermitteln */

		ev_flags = MU_BUTTON | MU_MESAG | MU_KEYBD;
		ev_flags |= MU_TIMER;			/* Timerevent einschalten! */

		/*
		 * Bildfenster?
		 */
		if ((windownum = Window.myWindow(topwin)) < 0)
		{
			picwin = &picture_windows[-windownum];
			mex = picwin->wx;
			mey = picwin->wy;
			mex2 = picwin->ww;
			mey2 = picwin->wh;
		}

		/**/
		/*		EVENT-Schleife 			*/
		/**/
		do
		{
			back = evnt_multi(ev_flags, 0x102, 0x3, 0x0, 0, mex, mey, mex2, mey2, 1, mex, mey, mex2, mey2,
				messagebuf,
				EVNT_TIME(Sys_info.Event_Timer),
				&mouse_xpos, &mouse_ypos, &mouse_button, &key_at_event, &key_scancode, &klicks);

			klickhandle = wind_find(mouse_xpos, mouse_ypos);

			/*
			 * Timerevent - Palettenwechsel, Koo-Anzeige, FX, etc
			 */
			if (back & MU_TIMER)
			{
				for (t = 0; t < 10; t++)
				{
					if (timer_fx_max[t] > 0)
					{
						timer_fx_counter[t]++;
						if (timer_fx_counter[t] >= timer_fx_max[t])
						{
							timer_fx_counter[t] = 0;
							timerrout = timer_fx_rout[t];
							timerrout();
						}
					}
				}

				/*--- Bildmanagerredraw (Bildausschnittsbox) ----*/
				if (pm_to_redraw)
				{
					graf_mkstate(&dummy, &dummy, &mbutton, &dummy);
					if (mbutton == 0)
					{
						Window.redraw(Dialog.picMan.window, NULL, PM_PREVBOX, 0);
						pm_to_redraw = 0;
					}
				}

				/*---- Hat sich die Maus bewegt? */
				if (old_mx != mouse_xpos || old_my != mouse_ypos)
				{
					windh = wind_find(mouse_xpos, mouse_ypos);

					windnum = Window.myWindow(windh);

					/*
					 * Maus �ber Dialogfenster oder anderem oder Desktop 
					 */
					if (windnum >= 0)
					{
						if (oldwindnum < 0)	/* war vorheriges ein Bildfenster? */
						{
							if (Display_Opt.palette_mode == PAL_MOUSE)
								f_set_syspal();
							graf_mouse(ARROW, NULL);
							if (!nullcoordset)
								imageWindow.nullCoords(oldpicwin);
						}
					}
					/*
					 * Maus �ber Bildfenster 
					 */
					else
					{
						if (picture_windows[-windnum].shaded == 0)
							f_pic_event(&picture_windows[-windnum], MU_TIMER, windnum);

						oldpicwin = &picture_windows[-windnum];
					}
				}

				oldwindnum = windnum;
				old_mx = mouse_xpos;
				old_my = mouse_ypos;
			}
		} while (back == MU_TIMER);

		/* 
		 * ggfs. Messagehandler aufrufen 
		 */
		if (back & MU_MESAG)
		{
			key_scancode = key_ascii = obj = 0;	/* letztes angeklicktes Objekt l�schen */

			if ((messagebuf[0] == WM_SIZED || messagebuf[0] == WM_FULLED || messagebuf[0] == WM_ARROWED ||
				 messagebuf[0] == WM_HSLID || messagebuf[0] == WM_VSLID))
				pm_to_redraw = 1;

			message_back = f_handle_message();
		}

		/*
		 * Messages an angemeldete Plugins weiterleiten
		 */
		if (back != MU_TIMER)
		{
			for (t = 0; t < 20; t++)
			{
				if (modconfs[t] != NULL)
				{
					for (tt = 0; tt < 10; tt++)
					{
						mod_desire = modconfs[t]->notify_types[tt];
						mod_index = modconfs[t]->id & MOD_ID_MASK;

						if ((modconfs[t]->id & MOD_PLUGIN) && plugin_bp[mod_index] != NULL)	/* Plugin? */
						{
							if ((back & MU_MESAG) && (mod_desire == ALL_MSGS || mod_desire == messagebuf[0]))
							{
								memcpy(plg_data[mod_index]->event_par, messagebuf, 16);
								start_plugin(plugin_bp[mod_index], SMURF_AES_MESSAGE, 0, plg_data[mod_index]);
							} else if ((back & MU_BUTTON) && mod_desire == MBEVT && modconfs[t]->windhandle == windh)
							{
								/* FIXME: windh only set on MU_TIMER event */
								back = do_MBEVT(modconfs[t]->id, NULL, MBEVT);
							} else if ((back & MU_KEYBD) && mod_desire == MKEVT && modconfs[t]->windhandle == topwin)
							{
								back = do_MBEVT(modconfs[t]->id, NULL, MKEVT);
							}
							if (back == M_TERMINATED)
								break;
						}
					}
				}
			}
		}

		/* 
		 * Fensternummern ermitteln und andere Events behandeln
		 */
		if (back != MU_MESAG)
		{
			windnum = Window.myWindow(klickhandle);	/* Fensternummer ermitteln */
			if (!windnum)				/* nicht meins und Bildfenster ausgeschlossen */
				mod_win = Window.myModuleWindow(klickhandle);

			/*
			 * Keyboardevents --------------------
			 */
			if (back & MU_KEYBD)
			{
#if 0
				if (Dialog.winAlert.isTop)
				{
					keyboard_back = handle_keyboardevent(&wind_s[WIND_ALERT], key_scancode, &key_object);
					if (keyboard_back == 0)
						Dialog.winAlert.closeAlert();

					back = 0;
				} else
#endif
				if ((key_scancode >> 8) == KEY_UNDO)
				{
					Window.windGet(0, WF_TOP, &topwin, &dummy, &dummy, &dummy);

					if (Window.myWindow(topwin) > 0)	/* Dialogfenster? */
						Window.close(topwin);
				} else if ((key_scancode >> 8) == KEY_HELP)
				{
					call_stguide(topwin);
				} else
				{
					obj = 0;
					key_ascii = scan_2_ascii(key_scancode, key_at_event);
					Window.windGet(0, WF_TOP, &topwin, &dummy, &dummy, &dummy);
					topw_num = Window.myWindow(topwin);
					mod_win = Window.myModuleWindow(topwin);

					/* Control gedr�ckt? */
					if ((key_at_event & KEY_CTRL) ||
						((key_at_event & KEY_ALT) && (key_scancode >> 8) != SCAN_RETURN && (key_scancode >> 8) != SCAN_ENTER))
					{
						ki.ascii_code = key_ascii;
						ki.scan_code = key_scancode;
						ki.ctrl = key_at_event & 0x004;
						ki.alt = key_at_event & 0x008;
						ki.shift = key_at_event & 0x003;

						if ((itsok = get_menu_key(menu_tree, &ki, &title, &item)) != FALSE)
						{
							messagebuf[3] = title;
							messagebuf[4] = item;
							back = f_handle_menuevent(messagebuf);
							if (back == -1)
								message_back = PRG_CLOSED;
						} else
						{
							Comm.sendAESMsg(Sys_info.ENV_avserver, AV_SENDKEY, key_at_event, key_scancode);
						}
						key_at_event = obj = key_scancode = 0;
					} else
						itsok = 0;

					/* Dialogfenster oder Modulfenster */
					if (itsok == 0 &&
						((topw_num > 0 && wind_s[topw_num].shaded == 0) || (mod_win != NULL && mod_win->shaded == 0)))
					{
						if (wind_s[topw_num].whandlem != -1 && topw_num > 0)	/* Dialog */
						{
							keyboard_back = handle_keyboardevent(&wind_s[topw_num], key_scancode, &key_object);
							openmode = 1;
							if (keyboard_back == 0)
								obj = DIALOG_EXIT;
							else
								obj = key_object;

							CallDialog(topwin);
						} else if (mod_win != NULL)	/* Modulfenster */
						{
							keyboard_back = handle_keyboardevent(mod_win, key_scancode, &key_object);
							obj = key_object;
							do_MBEVT(mod_win->module, mod_win, MKEVT);
							if (keyboard_back == 0)
								change_object(mod_win, obj, OS_UNSEL, 1);
						}
					}

					/* Bildfenster */
					else if (topw_num < 0)
					{
						keyboard_back = 0;
						picwin = &picture_windows[-topw_num];
						picwin_keyboard(key_scancode, key_at_event, picwin);
					}
				}

				key_scancode = 0;		/* Scancode l�schen */
				key_ascii = 0;			/* Scancode l�schen */
			}

			/*
			 * Buttonevents f�r Bilder, Dialoge und Module -------------------
			 */
			else if (back & MU_BUTTON)
			{
				if (Dialog.winAlert.isTop)
				{
					klickobj = objc_find(wind_s[WIND_ALERT].resource_form, 0, MAX_DEPTH, mouse_xpos, mouse_ypos);
					obj = UDO_or_not(&wind_s[WIND_ALERT], klickobj);
					if (obj == ALBUTTON1 || obj == ALBUTTON2 || obj == ALBUTTON3)
						Dialog.winAlert.closeAlert();
					back = 0;
				} else
				{
					key_scancode = key_ascii = 0;

					/*
					 * Bildfenster ------------------------
					 */
					if (windnum < 0 && picture_windows[-windnum].shaded == 0)
					{
						klickobj = -1;
						f_pic_event(&picture_windows[-windnum], MU_BUTTON, windnum);
					}

					/*
					 * Dialogfenster ------------------------
					 */
					else if (windnum > 0 && wind_s[windnum].shaded == 0)
					{
						ob = wind_s[windnum].resource_form;
						klickobj = objc_find(ob, 0, MAX_DEPTH, mouse_xpos, mouse_ypos);

						if (mouse_button == 0x02 && ob[klickobj].ob_type != (CYCLEBUTTON | G_USERDEF))
						{
							Comm.bubbleGem(windnum, mouse_xpos, mouse_ypos, 0);
						} else
						{
							/*------- Fenster toppen? --------*/
							if (klickobj == 0 || ob[klickobj].ob_type == G_FTEXT || IsDisabled(ob[klickobj])
								/* || !IsSelectable(ob[klickobj]) && (ob[klickobj].ob_type & 0xff00) == 0 */ )
							{
								Window.top_window(wind_s[windnum].whandlem);
								back = 0;
							} else
								if (klickobj != -1 && IsSelectable(ob[klickobj]) &&
									!IsDisabled(ob[klickobj]) && !(ob[klickobj].ob_flags & OF_RBUTTON))
							{
								if (!ob[klickobj].ob_state & OS_SELECTED)
									change_object(&wind_s[windnum], klickobj, OS_SELECTED, 1);
								else
									change_object(&wind_s[windnum], klickobj, OS_UNSEL, 1);
							}

							obj = UDO_or_not(&wind_s[windnum], klickobj);	/* UDOs behandeln */
							if (obj == -1)
								continue;

							/* Abbruch-Button */
							if ((ob[obj].ob_type >> 8) == CANCELBUTTON)
							{
								Window.close(wind_s[windnum].whandlem);
								change_object(&wind_s[windnum], obj, OS_UNSEL, 1);
							} else
							{
								f_handle_editklicks(&wind_s[windnum], klickobj);

								if (IsDisabled(ob[obj]))
									klickobj = -1;
							}
						}
					}

					/*
					 * Modulfenster ----------------------------------
					 */
					else if (!windnum && mod_win != NULL)
					{
						/* Das Modul mu� den angeklickten Button evtl. wieder deselektieren!    */
						/* -> change_object in die GARGAMEL!!!                                  */

						klickobj = objc_find(mod_win->resource_form, 0, MAX_DEPTH, mouse_xpos, mouse_ypos);

						if (mouse_button == 0x02 && klickobj != 0 &&
							mod_win->resource_form[klickobj].ob_type != (CYCLEBUTTON | G_USERDEF))
						{
							Comm.bubbleGem(klickhandle, mouse_xpos, mouse_ypos, 1);
						} else if (mod_win != NULL && mod_win->shaded == 0)
						{
							module_number = mod_win->module;
							klickobj = handle_modevent(MU_BUTTON, mod_win);

							/* Objekt ist im Baum und nicht 0? */
							if (klickobj > 0)
							{
								obj = UDO_or_not(mod_win, klickobj);

								/* Abbruch-Button? */
								if (obj == klickobj && (mod_win->resource_form[obj].ob_type >> 8) == CANCELBUTTON)
								{
									Window.close(mod_win->whandlem);
									change_object(mod_win, obj, OS_UNSEL, 1);
								} else
								{
									f_handle_editklicks(mod_win, klickobj);

									/* Radiobutton - noch keine Extrawurst */
									if (mod_win->resource_form[klickobj].ob_flags & OF_RBUTTON)
									{
										Window.top_window_now(mod_win);
										form_button(mod_win->resource_form, klickobj, klicks, &newedit);
									}

									if (mod_win->resource_form[klickobj].ob_type == MPIC_INFORMATION)
									{
										ready_modpics_popup(mod_win);
										f_pop(&popups[POPUP_PICORDER], 1, 0, NULL);	/* Bildreihenfolge-popup �ffnen */
									}

									if (obj == klickobj && klickobj > 0)
										do_MBEVT(module_number, mod_win, MBEVT);

									/* Erste Frage mu� sein da das Module ggf. bei do_MBEVT terminiert */
									/* bzw. sein Fenster geschlossen werden kann, mod_win aber weil schon */
									/* oben geholt noch auf den nun ung�ltigen Speicher zeigt. */
									if (module.smStruct[module_number & MOD_ID_MASK] &&
										module.smStruct[module_number & MOD_ID_MASK]->wind_struct &&
										(mod_win->resource_form[klickobj].ob_type & 0x00ff) != G_USERDEF &&
										!(mod_win->resource_form[klickobj].ob_flags & OF_RBUTTON) &&
										(mod_win->resource_form[klickobj].ob_type >> 8) != UDO &&
										(mod_win->resource_form[klickobj].ob_flags & OF_SELECTABLE))
										change_object(mod_win, klickobj, OS_UNSEL, 1);
								}
							}
						}
					}

					/*--------------- Busybalken umschalten? */
					if (klickhandle == wind_s[WIND_BUSY].whandlem)
					{
						klickobj = objc_find(Dialog.busy.busyTree, 0, MAX_DEPTH, mouse_xpos, mouse_ypos);
						if (klickobj == BW_ICON)
						{
							switch (Sys_info.busybox_icon)
							{
							case BUSYICON_1:
								bb_icon = BUSYICON_2;
								break;
							case BUSYICON_2:
								bb_icon = BUSYICON_3;
								break;
							case BUSYICON_3:
								bb_icon = BUSYICON_4;
								break;
							case BUSYICON_4:
								bb_icon = BUSYICON_5;
								break;
							case BUSYICON_5:
								bb_icon = BUSYICON_6;
								break;
							case BUSYICON_6:
								bb_icon = BUSYICON_7;
								break;
							default:
								bb_icon = BUSYICON_1;
								break;
							}

							Dialog.busy.busyTree[BW_ICON].ob_spec.userblk->ub_parm =
								u_tree[bb_icon].ob_spec.userblk->ub_parm;
							Sys_info.busybox_icon = bb_icon;

							Window.redraw(&wind_s[WIND_BUSY], NULL, BW_ICON, 0);
						}
					}

					/*
					 * Buttonevents an die Dialoge weiterleiten
					 */
					if (!Dialog.winAlert.isTop && (back & MU_BUTTON) && windnum > 0 && wind_s[windnum].shaded == 0 &&
						obj == klickobj)
					{
						openmode = 1;
						CallDialog(klickhandle);
					}

				}

			}

		}

	} while (message_back != PRG_CLOSED);
}



/* --------------------------------------------------------------------	*/
/*					Dialogdispatcher									*/
/* --------------------------------------------------------------------	*/
BOOLEAN CallDialog(WORD topwin)
{
	short wnum;

	wnum = Window.myWindow(topwin);

	if (wind_s[wnum].whandlem != -1 && wind_s[wnum].whandlem != 0)
	{
		if (topwin == wind_s[WIND_DOPT].whandlem)
			Dialog.dispOpt.displayOptions();
		else if (topwin == wind_s[WIND_OPTIONS].whandlem)
			Dialog.smurfOpt.options();
		else if (topwin == wind_s[WIND_MODFORM].whandlem)
			f_mpref_change();
		else if (topwin == wind_s[WIND_MODULES].whandlem)
			Dialog.emodList.handleList();
		else if (topwin == wind_s[WIND_NEWPIC].whandlem)
			f_newpic(-1);
		else if (topwin == wind_s[WIND_BTYPEIN].whandlem)
			block_type_in();
		else if (topwin == wind_s[WIND_EXPORT].whandlem)
			Dialog.expmodList.handleList();
		else if (topwin == wind_s[WIND_BUSY].whandlem)
			Dialog.busy.dispRAM();
		else if (topwin == wind_s[WIND_PICMAN].whandlem)
			Dialog.picMan.handlePicman();
		else if (topwin == wind_s[FORM_EXPORT].whandlem)
			f_export_formular();
		else if (topwin == wind_s[WIND_INFO].whandlem)
			f_info();
		else if (topwin == wind_s[WIND_TRANSFORM].whandlem)
			transform_pic();
		else if (topwin == wind_s[WIND_BLOCKMODE].whandlem)
			blockmode();
		else
			return FALSE;
	}

	return TRUE;
}



/* ----------------------------------------------------------------	*/
/*							Info-Monolog							*/
/* -----------------------------------------------------------------*/
void f_info(void)
{
	WORD button;
	OBJECT *ob;

	ob = wind_s[WIND_INFO].resource_form;

	f_set_syspal();
	button = Dialog.init(WIND_INFO, -1);

	BCD2string(ob[VERSION].TextCast, SMURF_VERSION);
#if BETA
	strcat(ob[VERSION].TextCast, "B");
	itoa(BETA, ob[VERSION].TextCast + strlen(ob[VERSION].TextCast), 10);
#endif

#ifdef DEBUG
	strcat(ob[VERSION].TextCast, " Debug");
#endif

	strcpy(ob[INFO_DATE].TextCast, __DATE__);

	if (button == INFO_YEAH && timer_fx_max[0] == 0)
	{
		init_roto();
		timer_fx_max[0] = 1;
		timer_fx_counter[0] = 0;
		timer_fx_rout[0] = roto;
	} else if (button == INFO_YEAH)
	{
		change_object(&wind_s[WIND_INFO], INFO_YEAH, OS_UNSEL, 1);
		timer_fx_max[0] = 0;
		timer_fx_counter[0] = 0;
		timer_fx_rout[0] = NULL;
		deinit_roto();
	}
}


/*-------------------------------------------------------------------------	*/
/*	Window-Close-Handler													*/
/*	Diese Funktion handled das Schlie�en eines Dialogs mit dem Closer		*/
/*	(was in Smurf einem Abbruch-Button gleichkommt) und setzt dessen		*/
/*	eventuelle Optionen zur�ck.												*/
/*-------------------------------------------------------------------------	*/
void check_windclose(short windnum)
{
	char str[4];
	short module_number;
	WORD whandle;
	WORD sel_option;
	WINDOW *window_to_handle;
	OBJECT *rsc;
	OBJECT *rsc2;

	switch (windnum)
	{
		/*
		 * Optionsdialog
		 */
	case WIND_OPTIONS:
		rsc = wind_s[WIND_OPTIONS].resource_form;

		rsc[OPT_CENTER].ob_state &= ~OS_SELECTED;
		rsc[OPT_PROFI].ob_state &= ~OS_SELECTED;
		rsc[OPT_WINDOWALERT].ob_state &= ~OS_SELECTED;
		rsc[OPT_PREVS].ob_state &= ~OS_SELECTED;

		rsc[OPT_CENTER].ob_state |= Sys_info.center_dialog;
		rsc[OPT_PROFI].ob_state |= Sys_info.profi_mode;
		rsc[OPT_WINDOWALERT].ob_state |= Sys_info.window_alert;
		rsc[OPT_PREVS].ob_state |= Sys_info.immed_prevs;

		rsc[ENV_STANDARD].ob_state &= ~OS_SELECTED;
		rsc[ENV_SILLY].ob_state &= ~OS_SELECTED;
		rsc[ENV_THERAPY].ob_state &= ~OS_SELECTED;

		itoa(Sys_info.outcol, str, 10);
		strncpy(rsc[OUTCOL].TextCast, str, 2);
		xrsrc_gaddr(R_TREE, KODAK_POPUP, &rsc2, resource_global);
		strncpy(rsc[PCD_DEF_PB].TextCast, rsc2[Sys_info.PCD_Defsize].TextCast, 9);

		switch (Sys_info.environment)
		{
		case 1:
			rsc[ENV_STANDARD].ob_state |= OS_SELECTED;
			break;
		case 2:
			rsc[ENV_SILLY].ob_state |= OS_SELECTED;
			break;
		case 3:
			rsc[ENV_THERAPY].ob_state |= OS_SELECTED;
			break;
		}
		break;

		/*
		 * Display-Options
		 */
	case WIND_DOPT:
		rsc = wind_s[WIND_DOPT].resource_form;

		/* Palettenmodi deselektieren */
		rsc[PAL_MOUSE].ob_state &= ~OS_SELECTED;
		rsc[PAL_TOPWIN].ob_state &= ~OS_SELECTED;
		rsc[PAL_SYSTEM].ob_state &= ~OS_SELECTED;

		/* und den richtigen Selektieren */
		switch (Display_Opt.palette_mode)
		{
		case PAL_MOUSE:
			rsc[PAL_MOUSE].ob_state |= OS_SELECTED;
			break;
		case PAL_TOPWIN:
			rsc[PAL_TOPWIN].ob_state |= OS_SELECTED;
			break;
		case PAL_SYSTEM:
			rsc[PAL_SYSTEM].ob_state |= OS_SELECTED;
			break;
		}

		Sys_info.pal24 = Display_Opt.syspal_24;
		Sys_info.pal8 = Display_Opt.syspal_8;
		Sys_info.pal4 = Display_Opt.syspal_4;

		Sys_info.dither24 = Display_Opt.dither_24;
		Sys_info.dither8 = Display_Opt.dither_8;
		Sys_info.dither4 = Display_Opt.dither_4;

		/* Timer neu eintragen */
		itoa(Sys_info.Event_Timer, rsc[PAL_TIMER].TextCast, 10);
		break;

		/*
		 * Exportformular
		 */
	case FORM_EXPORT:
		module_number = exp_conf.export_mod_num & MOD_ID_MASK;

		window_to_handle = module.smStruct[module_number]->wind_struct;

		/* ggfs. More-Dialog schlie�en */
		if (window_to_handle)
		{
			whandle = window_to_handle->whandlem;
			Window.cursorOff(window_to_handle);
			window_to_handle->shaded = 0;
			wind_close(whandle);
			wind_delete(whandle);
			window_to_handle->whandlem = -1;
		}

		/* Export-Modul terminieren */
		check_and_terminate(MTERM, exp_conf.export_mod_num & MOD_ID_MASK);

		/* damit an der exp_conf festgestellt werden kann, ob �berhaupt ein 
		 * Exporter im Exportdialog l�uft. Wenn nicht, werden z.B. bei der
		 * Schlie�ung von exportereigenen Dialogen (MMORE) die Exporter
		 * gleich wieder terminiert. 0 funktioniert, weil die kleinste Exporter-ID
		 * 0x100 (256) ist.
		 */
		exp_conf.export_mod_num = 0;
		break;

		/*
		 * Blockmodusformular
		 */
	case WIND_BLOCKMODE:
		rsc = wind_s[WIND_BLOCKMODE].resource_form;
		sel_option = get_selected_object(rsc, BCONF_REPLACE, BCONF_MULT);
		rsc[sel_option].ob_state &= ~OS_SELECTED;
		rsc[blockmode_conf.mode].ob_state |= OS_SELECTED;

		setslider(&sliders[BLOCK_SLIDER], blockmode_conf.opacity);
		break;

	case WIND_MODULES:
		Dialog.emodList.infoOff();
		break;

	case WIND_EXPORT:
		Dialog.expmodList.infoOff();
		break;

		/*
		 * Smurf-Info
		 */
	case WIND_INFO:
		timer_fx_max[0] = 0;
		timer_fx_counter[0] = 0;
		timer_fx_rout[0] = NULL;
		deinit_roto();
		break;
	}
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
/* -------		 			Bild laden						---- */
/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
short f_loadpic(uint8_t *pic, char *picpath)
{
	char PCDmodpath[SM_PATH_MAX + 1];
	char *namename;
	char *nameext;
	char ext[5] = "";
	short module_ret;
	short init_zoom;
	WORD dummy;
	WORD key;
	short picture_to_load = 1;
	short free_pic = 1;
	short back = 0;

	strcpy(PCDmodpath, Sys_info.standard_path);
	strcat(PCDmodpath, "\\modules\\pcd.sim");

	while (free_pic <= MAX_PIC && smurf_picture[free_pic] != NULL)
		free_pic++;

	if (free_pic > MAX_PIC)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_PIC_FREE].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}

	/*
	 * erstes freies Bild ermitteln
	 */
	while (picture_to_load < MAX_PIC && smurf_picture[picture_to_load] != NULL)
		picture_to_load++;

	if ((smurf_picture[picture_to_load] = alloc_smurfpic(NULL, FALSE)) == NULL)
		return -1;
	smurf_picture[picture_to_load]->pic_data = pic;

	graf_mouse(BUSYBEE, NULL);

	strcpy(smurf_picture[picture_to_load]->filename, picpath);

	/*
	 * can't use init_timer() here,
	 * because it would clobber its static variable,
	 * which may also be used by the plugins
	 */
	oldtim = clock();

	namename = strrchr(picpath, '\\') + 1;	/* Dateinamen abtrennen */

	if (PCD)
	{
		PCD = 0;
		smurf_picture[picture_to_load]->pic_width = PCDwidth;
		smurf_picture[picture_to_load]->pic_height = PCDheight;
		smurf_picture[picture_to_load]->file_len = f_len;

		if ((module_ret = module.comm.start_imp_module(PCDmodpath, smurf_picture[picture_to_load])) == -1)
		{
			/* FIXME: translate */
			Dialog.winAlert.openAlert("PhotoCD-Modul nicht gefunden: \\SMURF\\MODULES\\PCD.SIM", NULL, NULL, NULL, 1);
			module_ret = M_MODERR;
			Dialog.busy.reset(128, "Error!");
		} else
		{
			Dialog.busy.ok();
		}
	} else
	{
		if ((nameext = strrchr(namename, '.')) != NULL && nameext > namename)	/* Extender abtrennen */
		{
			strncpy(ext, nameext + 1, 4);
			strupr(ext);
		} else
		{
			strcpy(ext, "");
		}
		module_ret = f_import_pic(smurf_picture[picture_to_load], ext);
	}

	smurf_picture[picture_to_load]->zoom = 0;

	graf_mkstate(&dummy, &dummy, &dummy, &key);
	if (key == KEY_ALT)
	{
		back = f_pop(&popups[POPUP_ZOOM], 1, 0, NULL);
		switch (back)
		{
		case DURCH2:
			init_zoom = 1;
			break;
		case DURCH3:
			init_zoom = 3;
			break;
		case DURCH4:
			init_zoom = 9;
			break;
		case DURCH5:
			init_zoom = 19;
			break;
		case DURCH6:
			init_zoom = 29;
			break;
		case ZOOM1:
			init_zoom = 0;
			break;
		default:
			init_zoom = 0;
			break;
		}

		smurf_picture[picture_to_load]->zoom = init_zoom;
	}

	if ((back = f_formhandle(picture_to_load, module_ret, namename)) == 0)
	{
		picture_windows[picture_to_load].clipwid = smurf_picture[picture_to_load]->pic_width;
		picture_windows[picture_to_load].cliphgt = smurf_picture[picture_to_load]->pic_height;
#if 0
		Window.redraw(&picture_windows[picture_to_load], NULL, 0, 0);
#endif
		f_activate_pic(picture_to_load);
		picthere++;
	}

	Dialog.busy.dispRAM();				/* Wieviel RAM? */
	actualize_menu();					/* Men�eintr�ge ENABLEn / DISABLEn */

	graf_mouse(ARROW, NULL);

	/*---- BBOK - Timer einschalten ---*/
	timer_fx_max[1] = 30;
	timer_fx_counter[1] = 0;
	timer_fx_rout[1] = Dialog.busy.ok;

	return back;						/* und zur�ck */
}


/* --------------------------------------------------------------- */
/* -------		 			Formaterkennung					------ */
/* --------------------------------------------------------------- */
short f_formhandle(short picture_to_load, short module_ret, char *namename)
{
	char times[20];
	char stats[21];
	uint8_t *palette;
	short tt;
	int dec;
	int sign;
	short back = 0;
	BOOLEAN picerror = 0;					/* Fehler in Bildabmessungen, Farbtiefe, etc. */
	double timer_val;

	/*-- Auswertung der Modulmessage --*/
	picerror = analyze_message(module_ret, picture_to_load);

	/* Ist ein Fehler aufgetreten, bei dem die n�chsten */
	/* Bilder auch Probleme bekommen k�nnen? */
	if (picerror && (module_ret == M_INVALID || module_ret == M_IOERROR || module_ret == M_STARTERR))
		back = -1;

	if (picerror || module_ret == M_INVALID)
	{
		if (smurf_picture[picture_to_load] != NULL)
		{
			if (smurf_picture[picture_to_load]->pic_data != NULL)
				SMfree(smurf_picture[picture_to_load]->pic_data);
			smurf_picture[picture_to_load]->pic_data = NULL;
				SMfree(smurf_picture[picture_to_load]);
			smurf_picture[picture_to_load] = NULL;
		}
		back = -1;
	}

			/*------ Bild wurde einwandfrei geladen -> Dithern und Fenster auf -----*/
	if (!picerror && (module_ret == M_PICDONE || module_ret == M_DONEEXIT))
	{
		/* Farbsystem transformieren */
		if (smurf_picture[picture_to_load]->col_format == BGR)
			tfm_bgr_to_rgb(smurf_picture[picture_to_load], SAME);
		else if (smurf_picture[picture_to_load]->col_format == CMY)
			tfm_cmy_to_rgb(smurf_picture[picture_to_load], SAME);

		/* pr�ventiv die Systempalette eintragen SMURF_PIC */
		for (tt = 0; tt <= Sys_info.Max_col; tt++)
		{
			smurf_picture[picture_to_load]->red[tt] = (char) ((255 * (long) Sys_info.pal_red[tt]) / 1000L);
			smurf_picture[picture_to_load]->grn[tt] = (char) ((255 * (long) Sys_info.pal_green[tt]) / 1000L);
			smurf_picture[picture_to_load]->blu[tt] = (char) ((255 * (long) Sys_info.pal_blue[tt]) / 1000L);
		}

		palette = smurf_picture[picture_to_load]->palette;
		if (palette[0] == palette[3] && palette[1] == palette[4] && palette[2] == palette[5] &&
			smurf_picture[picture_to_load]->depth == 1)
		{
			palette[0] = palette[1] = palette[2] = 255;
			palette[3] = palette[4] = palette[5] = 0;
		}

		/*
		 * Wenn realtime-dither aus ist, Bild vordithern, ansonsten
		 * gleich weiter.
		 */
		if (Sys_info.realtime_dither & OS_SELECTED)
		{
			smurf_picture[picture_to_load]->screen_pic = NULL;
			back = 0;
		} else
			back = f_dither(smurf_picture[picture_to_load], &Sys_info, 0, NULL, &Display_Opt);

		if (back != -1)
		{
			num_of_pics++;

			/*
			 * Zeit in Statusbox eintragen
			 */
			tim = clock();
			timer_val = (double) ((float) (tim - oldtim) / 200.0F);
			memset(stats, 0, sizeof(stats));
			memset(times, 0, sizeof(times));
			strcpy(stats, "OK (");
			ftoa(&timer_val, times, 10, 1, &dec, &sign);
			if (dec > 0)
				strncat(stats, times, dec);
			else
				strcat(stats, "0");
			if (dec < 0)
				dec = 0;
			strncat(stats, ".", 1);
			strncat(stats, times + dec, 2);
			strncat(stats, " s)", 3);
			Dialog.busy.reset(128, stats);

			/* Bildstruktur aufbauen */
			make_pic_window(picture_to_load, smurf_picture[picture_to_load]->pic_width, smurf_picture[picture_to_load]->pic_height, namename);

			/*------------ Bildnummer in den Fenstertitel - zum Entk�fern ---- */
#if 0
			itoa(picture_to_load, help_str, 10);
			strcat(picture_windows[picture_to_load].wtitle, " - Nr. ");
			strcat(picture_windows[picture_to_load].wtitle, help_str);
#endif

			Window.open(&picture_windows[picture_to_load]);
			imageWindow.clipPicwin(&picture_windows[picture_to_load]);

			Dialog.picMan.insertPic(picture_to_load);
		} else
		{
			back = -1;					/* wahrscheinlich Speicherfehler */
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DIT_ERROR].TextCast, NULL, NULL, NULL, 1);
			SMfree(smurf_picture[picture_to_load]->pic_data);
			free(smurf_picture[picture_to_load]->screen_pic);
			SMfree(smurf_picture[picture_to_load]->local_nct);
			SMfree(smurf_picture[picture_to_load]);
			smurf_picture[picture_to_load] = NULL;
		}
	}

	return back;
}


/* ************************************************************** */
/* ************************************************************** */
short f_import_pic(SMURF_PIC *smurf_picture, char *extension)
{
	char *ice_depack_buf;
	char no_ext = 0;
	char *modpath;						/* voller Modulpfad, Original */
	char *mod_path;						/* voller Modulpfad, editable */
	char alertstr[128];
	short module_ret = M_INVALID;
	WORD alertback = 0;
	short pathlen;
	long oldmem;
	long newmem;
	long ice_unpack_len;
	long *icebuf;
	struct DIRENTRY *filelist;
	struct DIRENTRY *actual;

	if (extension == NULL || strlen(extension) == 0)
		no_ext = 1;

	/* ICE-Packed? */
	if (strncmp(smurf_picture->pic_data, "ICE!", 4) == 0)
	{
		icebuf = (long *) smurf_picture->pic_data;
		ice_unpack_len = *(icebuf + 2);
#if 0
		strncpy(len_str, "ICE! L�nge: ", 12);
		ltoa(ice_unpack_len, help_str, 10);
		strncat(len_str, help_str, 8);
		Dialog.busy.reset(128, len_str);
#endif
		if ((ice_depack_buf = SMalloc(ice_unpack_len)) == 0)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOMEM_ICE].TextCast, NULL, NULL, NULL, 1);
		} else
		{
			Dialog.busy.reset(128, "ICE-Decrunching...");

			ice_decrunch(icebuf, ice_depack_buf);
			SMfree(smurf_picture->pic_data);
#if 0
			smurf_picture->pic_data = SMalloc(ice_unpack_len);
			memcpy(smurf_picture->pic_data, ice_depack_buf, ice_unpack_len);
			if (SMfree(ice_depack_buf) != 0)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[ICE_MFREE_ERR].TextCast, NULL, NULL, NULL, 1);
#endif
			smurf_picture->pic_data = ice_depack_buf;
			f_len = ice_unpack_len;

			Dialog.busy.reset(128, "Decrunched...");
		}
	}


	smurf_picture->file_len = f_len;

	/* Pic-Defaults einstellen */
	smurf_picture->format_type = FORM_PIXELPAK;
	smurf_picture->col_format = RGB;


	/* ---------- Importer nach Extension in Import_list suchen */
	oldtim = clock();

	if (!no_ext)
		module_ret = seek_module(smurf_picture, extension);

	/* ---------- alle Importer in Import_list durchsuchen */
	if (no_ext || (module_ret != M_PICDONE && module_ret != M_DONEEXIT && module_ret != M_MEMORY &&
				   module_ret != M_PICERR && module_ret != M_PALERR))
	{
		if (!no_ext)
		{
			Dialog.busy.reset(0, strrchr(smurf_picture->filename, '\\') + 1);
			alertback = Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_ILIST_MATCH].TextCast, "Nein", "Ja", NULL, 2);
		}

		if (alertback == 2 || no_ext)
		{
			if (anzahl_importmods > 0)
				module_ret = seek_module(smurf_picture, "*");
			else
			{
				modpath = (char *) calloc(1, strlen(Sys_info.standard_path) + strlen("\\modules\\import\\") + 1);
				strcpy(modpath, Sys_info.standard_path);
				strcat(modpath, "\\modules\\import\\");

				Name_Max = get_maxnamelen(modpath);

				pathlen = (short) (strlen(modpath) + Name_Max);
				mod_path = (char *) calloc(1, pathlen + 1);

				filelist = build_up_filelist(modpath, "sim", pathlen);

				actual = filelist;
				while (actual != NULL)
				{
					/* Pic-Defaults einstellen */
					smurf_picture->format_type = FORM_PIXELPAK;
					smurf_picture->col_format = RGB;

					/*-------- N�chsten Modulpfad ermitteln */
					strcpy(mod_path, modpath);
					strcat(mod_path, actual->modname);

					Dialog.busy.reset(128, actual->modname);

					/*-------- Modul starten */
					oldmem = (long) Malloc(-1);
					oldtim = clock();
					if ((module_ret = module.comm.start_imp_module(mod_path, smurf_picture)) == M_STARTERR)
						Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MOD_LOAD_ERR].TextCast, NULL, NULL, NULL, 1);

					newmem = (long) Malloc(-1);

					if (oldmem != newmem && module_ret != M_PICDONE && module_ret != M_DONEEXIT)
					{
						/* FIXME: translate */
						sprintf(alertstr, "[3][ Modul %s | hat %ld | Bytes Speicher verbraten!][ Oh! ]", actual->modname, oldmem - newmem);
						form_alert(1, alertstr);
					}

					/*-------- Return-Message pr�fen */
					if (module_ret == M_PICDONE || module_ret == M_DONEEXIT)
					{
						/* FIXME: translate */
						Dialog.busy.reset(128, "loaded...");
						break;
					}

					actual = actual->next;
				}

				if (filelist == NULL)
				{
					Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_IMODS_FOUND].TextCast, NULL, NULL, NULL, 1);
					module_ret = M_INVALID;
				}

				destroy_filelist(filelist);

				free(modpath);
				free(mod_path);
			}
		} else if (alertback == 1)
		{
			module_ret = M_SILENT_ERR;	/* Meldung "Bild bitte an uns schicken" vermeiden */
		}
	}

	return module_ret;
}


/* ******************************************************************/
/*		Initialisierung der Bedienung eines Dialogs					*/
/*	Handled Radiobuttons und setzt globale Variable button beim		*/
/*	Dr�cken von RETURN, �ffnet oder toppt das Dialogfenster.		*/
/* ******************************************************************/
WORD init_dialog(short DialogNumber, WORD DialogOK)
{
	WORD button;

	if (!openmode)
	{
		button = 0;
		Window.open(&wind_s[DialogNumber]);
	} else
		button = obj;

	if (obj == DIALOG_EXIT)
		button = DialogOK;

	if (openmode)
		f_handle_radios(wind_s[DialogNumber].resource_form, button, DialogNumber);

	return button;
}


/* ******************************************************************/
/*						Dialog schlie�en							*/
/*	Fenster schlie�en und l�schen, aus der Liste nehmen Cursor aus, */
/* ******************************************************************/
void close_dialog(short windnum)
{
	WINDOW *wind;

	wind = &wind_s[windnum];

	if (wind->whandlem != -1)
	{
		Window.cursorOff(wind);
		wind_close(wind->whandlem);
		wind_delete(wind->whandlem);
		wind->whandlem = -1;
		wind->shaded = 0;
		Sys_info.dialog_opened[windnum] = 0;
		dialwindthere--;
	}
}



/* ******************************************************************/
/* ******************************************************************/
/*						SYSTEM-INITIALISIERUNG						*/
/* ******************************************************************/
/* ******************************************************************/




/* ----------------------------------------------------------------	*/
/*	System-Init: Farbtabellen, Systemkonfigurationsvariablen, etc.	*/
/* ----------------------------------------------------------------	*/
short f_init_system(void)
{
	OBJECT *tree;
	WORD pxy[8] = { 0, 0, 0, 0 };

	scrwd = Sys_info.screen_width;
	scrht = Sys_info.screen_height;
	screen.g_x = 0;
	screen.g_y = 0;
	screen.g_w = scrwd;
	screen.g_h = scrht;

	/* alles systemgegebene ermitteln */
	Sys_info.AES_version = _AESversion;
	Sys_info.Max_col = work_out[13] - 1;
	Sys_info.app_id = appl_id;

	vq_extnd(Sys_info.vdi_handle, 1, work_out);
	Sys_info.bitplanes = work_out[4];

	/* FIXME: translate */
	set_startupdial("initialisiere Objekte ...");
	init_moduleObs();
	init_GuiObs();
	init_MiscObs();

	DEBUG_MSG(("----------------- Smurf Start -----------------\n"));
	DEBUG_MSG(("Smurf-Pfad: %s\n", Sys_info.standard_path));
	DEBUG_MSG(("AES Version %#x\n", Sys_info.AES_version));
	DEBUG_MSG(("Screen: %iX%i\n", Sys_info.screen_width, Sys_info.screen_height));
	DEBUG_MSG(("  Farben: %i\n", Sys_info.Max_col));
	DEBUG_MSG(("  Bitplanes: %i\n", Sys_info.bitplanes));
	DEBUG_MSG(("Smurf VDI-Handle: %i\n", Sys_info.vdi_handle));
	DEBUG_MSG(("Smurf AES-ID: %i\n", Sys_info.app_id));

	/*----------------- Globale Dienstfunktionen f�r Module einh�ngen --------------*/
	/* FIXME: translate */
	set_startupdial("Moduldienste");
	global_services.busybox = Dialog.busy.draw;
	global_services.reset_busybox = Dialog.busy.reset;
	global_services.f_module_window = f_open_module_window;
	global_services.f_module_prefs = f_module_prefs;
	global_services.popup = f_pop;
	global_services.deselect_popup = f_deselect_popup;
	global_services.slider = f_rslid;
	global_services.set_slider = setslider;
	global_services.listfield = f_listfield;
	global_services.generate_listfield = f_generate_listfield;
	global_services.new_pic = f_generate_newpic;
	global_services.redraw_window = Window.redraw;
	global_services.f_move_preview = f_move_preview;
	global_services.copy_preview = copy_preview;
	global_services.SMalloc = SMalloc;
	global_services.SMfree = exported_SMfree;
	global_services.seek_nearest_col = seek_nearest_col;
	global_services.get_pic = get_pic;
	global_services.f_alert = Dialog.winAlert.openAlert;
	global_services.f_fsbox = f_fsbox;
	global_services.convert_units = convert_units;
	global_services.mconfSave = mconfSave;
	global_services.mconfLoad = mconfLoad;

	f_init_palette();

	/*----------------------- Module suchen und Modullisten aufbauen -------------------*/
	/* FIXME: translate */
	set_startupdial("Lade Dithermodule...");

	f_scan_dither();
	if (anzahl_dithermods == 0)
	{
		DEBUG_MSG(("***** Keine Dithermodule!\n"));
		/* FIXME: translate */
		Dialog.winAlert.openAlert("Keine Dithermodule gefunden! �berpr�fen Sie die Pfade, ohne Dithermodule kann Smurf nicht arbeiten. Smurf wird deshalb beendet", NULL, NULL, NULL, 1);
		return -1;
	}

	DEBUG_MSG(("Dithermodule: %i\n", anzahl_dithermods));
	DEBUG_MSG(("  %s, %s, %s, %s, %s \n", col_pop[1].TextCast, col_pop[2].TextCast,
		col_pop[3].TextCast, col_pop[4].TextCast, col_pop[5].TextCast));

	/* FIXME: translate */
	set_startupdial("Lade Editmodule...");
	f_scan_edit();
	f_generate_listfield(UP_MODULE, DN_MODULE, SBACK_MODULE, SLID_MODULE, ENTRYPARENT,
		(char *) Dialog.emodList.modNames, Dialog.emodList.anzahl, 9, &Dialog.emodList.modList, 24);
	if (Dialog.emodList.anzahl == 0)
	{
		DEBUG_MSG(("***** Keine Editmodule!\n"));
		form_alert(1, "[1][Keine Editmodule gefunden!][ Oh! ]");	/* FIXME: translate */
		change_object(&wind_s[WIND_MODULES], START_MODULE, OS_DISABLED, 1);
		change_object(&wind_s[WIND_MODULES], INFO_MODULE, OS_DISABLED, 1);

		xrsrc_gaddr(R_TREE, PIC_POPUP, &tree, resource_global);
		tree[PIC_EDIT].ob_state |= OS_DISABLED;
	}

	DEBUG_MSG(("Editmodule: %i\n", Dialog.emodList.anzahl));

	/* FIXME: translate */
	set_startupdial("Lade Exportmodule...");
	f_scan_export();
	f_generate_listfield(EMOD_UP, EMOD_DN, EMODSL_PAR, EMOD_SLIDER, EMODULES_BOX,
		(char *) Dialog.expmodList.modNames, Dialog.expmodList.anzahl, 9, &Dialog.expmodList.modList, 24);
	if (Dialog.expmodList.anzahl == 0)
	{
		DEBUG_MSG(("***** Keine Exportmodule!\n"));

		Dialog.winAlert.openAlert("Keine Exportmodule gefunden!", NULL, NULL, NULL, 1);
		change_object(&wind_s[WIND_EXPORT], START_EMOD, OS_DISABLED, 1);
		change_object(&wind_s[WIND_EXPORT], EXMOD_INFO, OS_DISABLED, 1);

		xrsrc_gaddr(R_TREE, BLOCK_POPUP, &tree, resource_global);
		tree[BLOCK_EXPORT].ob_state |= OS_DISABLED;
		xrsrc_gaddr(R_TREE, PIC_POPUP, &tree, resource_global);
		tree[PIC_EXPORT].ob_state |= OS_DISABLED;
	}

	DEBUG_MSG(("Exportmodule: %i\n", Dialog.expmodList.anzahl));

	/* Plugins Scannen und initialisieren */
	/* FIXME: translate */
	set_startupdial("Lade Plugins...");
	scan_plugins();

	DEBUG_MSG(("Listfeldinit: Bildmanager, "));

	/* FIXME: translate */
	set_startupdial("Initialisiere Listen...");
	f_generate_listfield(PM_UP, PM_DN, PMSL_PAR, PM_SLIDER, PM_BOX,
		(char *) picnames, 0, 8, &Dialog.picMan.pictureList, -1);

	if (startupdial_exist)
	{
		wind_update(END_UPDATE);
		form_dial(FMD_FINISH, sx, sy, sw, sh, sx, sy, sw, sh);
		xrsrc_free(startuprsc_global);

		/* Clipping das vom objc_draw() des Startup-Dialogs gesetzt */
		/* wurde weil XRSRC-Routinen benutzt werden wieder ausschalten */
		vs_clip(Sys_info.vdi_handle, FALSE, pxy);
	}

	DEBUG_MSG(("VDI-Tabelle...\n"));

	f_init_bintable(menu_tree);

	DEBUG_MSG(("Editmodule, "));

	/* Listfelder Initialisieren    */
	f_listfield(&wind_s[WIND_MODULES], F_REDRAW, 0, &Dialog.emodList.modList);
	DEBUG_MSG(("Exporter, "));
	f_listfield(&wind_s[WIND_EXPORT], F_REDRAW, 0, &Dialog.expmodList.modList);
	DEBUG_MSG(("Bildmanager 2.\n"));
	f_listfield(&wind_s[WIND_PICMAN], F_REDRAW, 0, &Dialog.picMan.pictureList);

	DEBUG_MSG(("Statusfenster\n"));

	Window.open(&wind_s[WIND_BUSY]);	/* erstmal Statusfenster �ffnen! */

	/*--------------------Farbtabellen-Initialisierung-*/
	Dialog.busy.reset(80, "BPT init");
	DEBUG_MSG(("BPT Init\n"));
	Sys_info.plane_table = planetable;

	Dialog.busy.reset(64, "NCT init");
	DEBUG_MSG(("NCT Init\n"));
	Sys_info.nc_table = f_init_table();	/* NC-Tabelle laden oder erzeugen */

	Sys_info.red = red;
	Sys_info.grn = grn;
	Sys_info.blu = blu;

	DEBUG_MSG(("Systeminitialisierung abgeschlossen.\n"));

	Dialog.busy.ok();
	return 0;
}


/* ----------------------------------------------------------------	*/
/* ------------------------- Palette auslesen ---------------------	*/
/* ----------------------------------------------------------------	*/
void f_init_palette(void)
{
	WORD rgb[3];
	WORD t;

	for (t = 0; t <= Sys_info.Max_col; t++)
	{
		vq_color(Sys_info.vdi_handle, t, 0, rgb);
		orig_red[t] = rgb[0];
		orig_green[t] = rgb[1];
		orig_blue[t] = rgb[2];
		Sys_info.pal_red[t] = rgb[0];
		Sys_info.pal_green[t] = rgb[1];
		Sys_info.pal_blue[t] = rgb[2];

		red[t] = (WORD) (rgb[0] * 31L / 1000L);
		fix_red[t] = (WORD) (rgb[0] * 255L / 1000L);

		grn[t] = (WORD) (rgb[1] * 31L / 1000L);
		fix_green[t] = (WORD) (rgb[1] * 255L / 1000L);

		blu[t] = (WORD) (rgb[2] * 31L / 1000L);
		fix_blue[t] = (WORD) (rgb[2] * 255L / 1000L);
	}
}


/* ----------------------------------------------------------------	*/
/*--------------- NC-Table laden oder ggfs. generieren -------------*/
/* ----------------------------------------------------------------	*/
uint8_t *f_init_table(void)
{
	char tablename[SM_PATH_MAX];
	char bpstring[8];
	uint8_t *palbuf;
	uint8_t *access_table;
	uint8_t *pltab;
	WORD redcomp[SM_PALETTE_MAX];
	WORD grncomp[SM_PALETTE_MAX];
	WORD blucomp[SM_PALETTE_MAX];
	WORD rgb[3];
	WORD *palette;
	WORD *pal_control;
	short t;
	short idx;
	short different = 0;
	int fil = 0;
	short maxc;
	WORD button = 0;
	short loadplanes;
	unsigned short tt;
	unsigned short ttcount = 0;
	long write_len;
	long par[6];
	WORD monopal[6];
	WORD monopal_nct[6];

	DEBUG_MSG(("f_init_table...\n"));

	/*
	 * Erst mal die monochrome NCT.
	 * Diese kann immer erzeugt werden, da ich die Farben immer wei�.
	 */
	/* Tabellen-Filename zusammenbasteln */
	strcpy(tablename, Sys_info.home_path);
	strcat(tablename, "\\smp.1");

	/* mit einem Trick testen, ob die Datei vorhanden ist */
	if (Fattrib(tablename, 0, 0) < 0)
	{
		Dialog.busy.reset(128, "monochrome NCT");

		monopal[0] = monopal[1] = monopal[2] = 1000;
		monopal[3] = monopal[4] = monopal[5] = 0;

		monopal_nct[0] = monopal_nct[1] = monopal_nct[2] = 31;
		monopal_nct[3] = monopal_nct[4] = monopal_nct[5] = 0;

		mononct = (unsigned char *) SMalloc((2 * 6) + SM_PALETTE_MAX + 32768L);

		palette = (WORD *) mononct;
		for (t = 0; t < 6; t++)			/* Palette */
			palette[t] = monopal[t];
		*(mononct + 12) = 0;			/* VDI-Umsetzung (in diesem Fall keine...) */
		*(mononct + 13) = 1;

		par[0] = (long) &monopal_nct;
		par[3] = (long) mononct + (2 * 6) + SM_PALETTE_MAX;
		makeNCT(par, 2);

		if ((fil = (int) Fcreate(tablename, 0)) >= 0)
		{
			write_len = Fwrite(fil, (2 * 6) + SM_PALETTE_MAX + 32768L, mononct);	/* Tabelle schreiben */
			Fclose(fil);				/* Datei schlie�en */
			if (write_len != (2 * 6) + SM_PALETTE_MAX + 32768L)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NCT_SAVEERROR].TextCast, NULL, NULL, NULL, 1);
		} else
		{
			/* FIXME: translate */
			Dialog.winAlert.openAlert("Fehler beim Anlegen der monochromen NCT-Datei! Das Dithering auf s/w wird u.U. nicht richtig funktionieren.", NULL, NULL, NULL, 1);
		}

		SMfree(mononct);
	}


	/*
	 * und jetzt die aktuelle NCT
	 */
	maxc = Sys_info.Max_col + 1;
	loadplanes = Sys_info.bitplanes;

	if (loadplanes > 8)
		return NULL;

	/* Tabellen-Filename zusammenbasteln */
	strcpy(tablename, Sys_info.home_path);
	strcat(tablename, "\\smp.");
	itoa(loadplanes, bpstring, 10);
	strcat(tablename, bpstring);

	/* mit einem Trick testen, ob die Datei vorhanden ist */
	if (Fattrib(tablename, 0, 0) >= 0)
	{
		if ((palbuf = (uint8_t *)fload(tablename, 0)) == NULL)
			return NULL;

		access_table = palbuf + maxc * 6 + SM_PALETTE_MAX;
		pal_control = (WORD *) palbuf;

		for (t = 0; t < maxc; t++)
		{
			redcomp[t] = *pal_control++;
			grncomp[t] = *pal_control++;
			blucomp[t] = *pal_control++;

			if (abs(Sys_info.pal_red[t] - redcomp[t]) > 2 ||
				abs(Sys_info.pal_green[t] - grncomp[t]) > 2 ||
				abs(Sys_info.pal_blue[t] - blucomp[t]) > 2)
				different = 1;
		}

		if (different)
		{
			/* FIXME: translate */
			button = Dialog.winAlert.openAlert(Dialog.winAlert.alerts[PAL_CHANGED].TextCast, "Zur�ck", "Aktuelle", NULL, 1);
		}

		if (!different || button == 1)
		{
			/* Systempalette an gespeicherte anpassen */
			for (t = 0; t < maxc; t++)
			{
				red[t] = (WORD) (redcomp[t] * 31L / 1000L);
				grn[t] = (WORD) (grncomp[t] * 31L / 1000L);
				blu[t] = (WORD) (blucomp[t] * 31L / 1000L);
				Sys_info.pal_red[t] = rgb[0] = redcomp[t];
				Sys_info.pal_green[t] = rgb[1] = grncomp[t];
				Sys_info.pal_blue[t] = rgb[2] = blucomp[t];

				vs_color(Sys_info.vdi_handle, t, rgb);
			}

			return access_table;
		}
	} else
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NCT_GENERATE].TextCast, NULL, NULL, NULL, 1);

		/* Tabelle mu� neu generiert werden! */
		Window.open(&wind_s[WIND_BUSY]);

		access_table = (uint8_t *) SMalloc(32768L);
		palbuf = (uint8_t *) SMalloc(maxc * 6 + SM_PALETTE_MAX);

		Dialog.busy.reset(0, "NCT erzeugen");

		/* Palette schreiben */
		palette = (WORD *) palbuf;
		for (t = 0; t < maxc; t++)
		{
			*palette++ = Sys_info.pal_red[t];
			*palette++ = Sys_info.pal_green[t];
			*palette++ = Sys_info.pal_blue[t];
		}

		/* Planetable schreiben */
		pltab = (uint8_t *) palette;
		for (t = 0; t < SM_PALETTE_MAX; t++)
			*pltab++ = planetable[t];

		par[0] = (long) red;
		par[1] = (long) grn;
		par[2] = (long) blu;

		for (tt = 0; tt < 32768L; tt++)
		{
			par[3] = (tt >> 10) & 31;
			par[4] = (tt >> 5) & 31;
			par[5] = tt & 31;
			idx = seek_nearest_col(par, maxc);

			*(access_table + tt) = idx;

			if (!(tt & 1023))
				Dialog.busy.draw(ttcount >> 8);
			ttcount++;
		}

		Dialog.busy.reset(128, "NCT speichern...");

		if ((fil = (int) Fcreate(tablename, 0)) >= 0)
		{
			Fwrite(fil, (maxc * 6) + SM_PALETTE_MAX, palbuf);	/* Palette und Planetable schreiben */
			write_len = Fwrite(fil, 32768L, access_table);	/* NCT schreiben */
			Fclose(fil);				/* Datei schlie�en */
			if (write_len != 32768L)
				Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NCT_SAVEERROR].TextCast, NULL, NULL, NULL, 1);
		} else
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NCT_SAVEERROR].TextCast, NULL, NULL, NULL, 1);

		SMfree(palbuf);

		Dialog.busy.reset((int) (tt * 128L >> 15L), "OK"); /* FIXME: translate */
	}

	DEBUG_MSG(("f_init_table...Ende\n"));

	return access_table;
}


/* ----------------------------------------------------------------	*/
/* ------ Bin�rwerttabelle f�r Palettenindex-Wert Umsetzung ------	*/
/*	Vorgehensweise: Erst wird ein Rechteck mit Palettenfarbe x auf	*/
/*	den Screen gemalt. Dieses wird dann mit vro_cpyfm in einen MFDB	*/
/*	kopiert und dieser dann ins Standardformat transferiert. Aus	*/
/*	diesem Standardblock wird dann der Wert bin�r ausgelesen. Das	*/
/*	ganze jetzt noch f�r jede Palettenfarbe, einmal kr�ftig um-		*/
/*	r�hren - voila, fertig ist die Tabelle.							*/
/* ----------------------------------------------------------------	*/
void f_init_bintable(OBJECT *rsc)
{
	WORD col;
	WORD maxc;
	WORD pel;
	WORD index;
	WORD pxy[4] = { 0, 0, 0, 0 };

	graf_mouse(M_OFF, NULL);

	objc_offset(rsc, 0, &pxy[0], &pxy[1]);
	pxy[2] = pxy[0];
	pxy[3] = pxy[1];

	maxc = Sys_info.Max_col + 1;
	if (maxc > SM_PALETTE_MAX)
		maxc = SM_PALETTE_MAX;

	for (col = 0; col < SM_PALETTE_MAX; col++)
		planetable[col] = col;			/* planetable vorinitialisieren */

	for (col = 0; col < maxc; col++)
	{
		vsf_color(Sys_info.vdi_handle, col);			/* Farbe einstellen */
		vr_recfl(Sys_info.vdi_handle, pxy);			/* Rechteck malen */

		v_get_pixel(Sys_info.vdi_handle, pxy[0], pxy[1], &pel, &index);	/* und dann Farb- und Pixelwert holen */
		planetable[col] = pel;
	}

	graf_mouse(M_ON, NULL);
}


void shutdown_smurf(char while_startup)
{
	WORD t;
	WORD maxcol;
	WORD rgb[4];
	long mod_magic;

	Comm.sendAESMsg(Sys_info.ENV_avserver, AV_EXIT, Sys_info.app_id, -1);

	Comm.deinitOLGA();

	Comm.deinitbubbleGem();

	if (while_startup)
		goto End_Only;

	Dialog.busy.reset(0, "See you...");

	/*
	 * originale Systempalette wieder einstellen
	 */
	maxcol = Sys_info.Max_col + 1;
	for (t = 0; t < maxcol; t++)
	{
		rgb[0] = orig_red[t];
		rgb[1] = orig_green[t];
		rgb[2] = orig_blue[t];
		vs_color(Sys_info.vdi_handle, t, rgb);
	}


	/*
	 * alle Module terminieren
	 */
	for (t = 0; t < 21; t++)
	{
		if (module.bp[t])
		{
			mod_magic = get_modmagic(module.bp[t]);

			if (mod_magic == MOD_MAGIC_EDIT)
				module.comm.start_edit_module("", module.bp[t], MTERM, t, module.smStruct[t]);

			check_and_terminate(MTERM, t);
		}
	}

	/* 
	 * residente Plugins terminieren...
	 */
	for (t = 0; t < anzahl_plugins; t++)
	{
		if (plugin_bp[t] && plg_info[t]->resident != 0)
		{
			start_plugin(plugin_bp[t], MTERM, t, plg_data[t]);

#if 0
			Pexec(102, NULL, plugin_bp[t], NULL);
#endif
			SMfree(plugin_bp[t]->p_env);
			SMfree(plugin_bp[t]);
			plugin_bp[t] = NULL;
			free(plg_data[t]);
			plg_data[t] = NULL;
			free(plg_info[t]);
			plg_info[t] = NULL;
			free(modconfs[t]);
			modconfs[t] = NULL;
		}
	}


	/*
	 * reservierten Speicher wieder freigeben
	 * der erste Modulname zeigt auch auf den Anfang des Speicherblocks
	 */
	free(Import_list.imp_mod_list[0]);

	Dialog.busy.reset(64, "later...");

	for (t = 0; t < 100; t++)
	{
		free(Dialog.emodList.modNames[t]);
		free(edit_modules[t]);
		free(Dialog.expmodList.modNames[t]);
		free(export_modules[t]);
	}

	Dialog.busy.reset(128, "Alligator!");

	for (t = 0; t < 25; t++)
	{
		if (wind_s[t].whandlem != -1)
		{
			wind_close(wind_s[t].whandlem);
			wind_delete(wind_s[t].whandlem);
		}

		if (picture_windows[t].whandlem != -1)
		{
			wind_close(picture_windows[t].whandlem);
			wind_delete(picture_windows[t].whandlem);
		}

		if (smurf_picture[t])
		{
			if (smurf_picture[t]->screen_pic)
			{
				SMfree(smurf_picture[t]->screen_pic->fd_addr);
				free(smurf_picture[t]->screen_pic);
			}

			SMfree(smurf_picture[t]->pic_data);
			SMfree(smurf_picture[t]);
		}

	}


  End_Only:

	/* -----  Resource schlie�en, AES freigeben, virWS schlie�en  -- */
	f_exit_menu();						/* Men� deinstallieren */
	xrsrc_free(resource_global);
	v_clsvwk(Sys_info.vdi_handle);
	appl_exit();
}


/* kopiert eine in picture �bergebene SMURF_PIC-Struktur. */
/* Achtung, ein eventuell enthaltener Block wird nicht mitkopiert! */
static short copy_smurfpic(SMURF_PIC *picture, SMURF_PIC **new_pic)
{
	uint8_t BitsPerPixel;
	unsigned short width, height;
	long data_len;

	if ((*new_pic = alloc_smurfpic(picture, TRUE)) == NULL)
		return -1;

	strcpy((*new_pic)->filename, "C:\\Namenlos");

	/* Bild */
	width = picture->pic_width;
	height = picture->pic_height;
	BitsPerPixel = picture->depth;

	if (picture->format_type == FORM_STANDARD)
		data_len = (long) (width + 7) / 8 * (long) height * BitsPerPixel;
	else
		data_len = ((long) width * (long) height * BitsPerPixel) >> 3;
	if (((*new_pic)->pic_data = (char *) SMalloc(data_len)) == NULL)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOMEM_NEWPIC].TextCast, NULL, NULL, NULL, 1);
		return -1;
	} else
	{
		memcpy((*new_pic)->pic_data, picture->pic_data, data_len);
	}

	/*
	 * Bildschirmdarstellung kopieren wenn das Bild mit Realtimedither
	 * verarbeitet wird, gibt es keine Bildschirmdarstellung. Dann mu�
	 * ein Nullzeiger in die Kopie geh�ngt werden.
	 */
	if (picture->screen_pic != NULL && picture->screen_pic->fd_addr != NULL)
	{
		width = picture->screen_pic->fd_wdwidth;
		height = picture->screen_pic->fd_h;
		BitsPerPixel = picture->screen_pic->fd_nplanes;

		(*new_pic)->screen_pic = (MFDB *) malloc(sizeof(MFDB));
		memcpy((*new_pic)->screen_pic, picture->screen_pic, sizeof(MFDB));
		data_len = (long) width * 2L * (long) height * BitsPerPixel;

		if (((*new_pic)->screen_pic->fd_addr = (char *) SMalloc(data_len)) == NULL)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DISPLAY_NOMEM].TextCast, NULL, NULL, NULL, 1);
			return -1;
		} else
		{
			memcpy((*new_pic)->screen_pic->fd_addr, picture->screen_pic->fd_addr, data_len);
		}
	} else
	{
		(*new_pic)->screen_pic = NULL;
	}

	/*
	 * Block kopieren
	 */
	if (picture->block)
	{
		(*new_pic)->block = NULL;
		(*new_pic)->blockx = 0;
		(*new_pic)->blocky = 0;
		(*new_pic)->blockwidth = 0;
		(*new_pic)->blockheight = 0;
	}

	if (picture->local_nct)
	{
		if (((*new_pic)->local_nct = (uint8_t *) SMalloc(32768L)) == NULL)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NOMEM_NEWPIC].TextCast, NULL, NULL, NULL, 1);
			return -1;
		}

		memcpy((*new_pic)->local_nct, picture->local_nct, 32768L);
	}

	(*new_pic)->prev_picture = NULL;
	(*new_pic)->next_picture = NULL;

	return 0;
}


/* Legt eine Kopie des �bergebenen Bildes an */
short duplicate_pic(WINDOW * window)
{
	short pic_to_make = 1;

	/* erstes freies Bild ermitteln */
	while (smurf_picture[pic_to_make] != NULL)
		pic_to_make++;
	if (pic_to_make > MAX_PIC)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[NO_PIC_FREE].TextCast, NULL, NULL, NULL, 1);
		return 0;
	}

	if (copy_smurfpic(window->picture, &smurf_picture[pic_to_make]) == -1)
	{
		destroy_smurfpic(smurf_picture[pic_to_make]);
		smurf_picture[pic_to_make] = NULL;
		return -1;
	}

	make_pic_window(pic_to_make, window->picture->pic_width, window->picture->pic_height, "Namenlos");
	picture_windows[pic_to_make].wtitle[11] = '*';

	Window.open(&picture_windows[pic_to_make]);
	imageWindow.clipPicwin(&picture_windows[pic_to_make]);
	imageWindow.setSliders(&picture_windows[pic_to_make]);
	Dialog.picMan.insertPic(pic_to_make);
	f_activate_pic(pic_to_make);

	picthere++;

	Dialog.busy.dispRAM();				/* Wieviel Ram? */
	actualize_menu();					/* Men�eintr�ge ENABLEn / DISABLEn */

	active_pic = pic_to_make;

	return 0;
}
