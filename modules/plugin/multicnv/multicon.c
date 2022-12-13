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

/*---------------------------------------------------------------------------
                                Multikonverter-Plugin
            FÅr SMURF Grafikkonverter, Olaf ab dem 20.03.98
            
    Das Plugin verwendet fÅr die Verwaltung der Exportmodule ausschlieûlich
    Smurf-interne Stukturen, so daû bei MMORE/MMORECANC/MMOREOK Smurf den
    gewohnten Weg geht und das Plugin nicht noch mit den Konfigurationen
    hantieren muû. Smurf merkt also in diesem Punkt gar nicht, daû er das
    Exportmodul gar nicht selber gestartet hat.
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurf_st.h"
#include "../../../src/rsc/en/smurf.h"
#include "smurfine.h"
#include "globdefs.h"
#include "plugin.h"
#include "popdefin.h"
#include "tools.h"
#include "multi.h"

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
#undef INFOICON							/* conflicts with smurf.h */
#undef DITHER_CB						/* conflicts with smurf.h */

#include "country.h"

#if COUNTRY==1
#include "de/multicon.rsh"
#elif COUNTRY==0
#include "en/multicon.rsh"
#elif COUNTRY==2
#include "en/multicon.rsh"		/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define TextCast ob_spec.tedinfo->te_ptext


static char const name_string[] = "Multikonverter";

PLUGIN_INFO plugin_info = {
	name_string,
	0x091,								/* Plugin-Version 1.00 */
	SMURF_VERSION,						/* fÅr Smurf-Version 1.06 */
	0,									/* und nicht resident. */
    COMPILER_ID
};

SERVICE_FUNCTIONS *services;

static short my_id;
static BOOLEAN dialog_open;
static BOOLEAN options_open;

WINDOW window;
OBJECT *dialog;
char **alerts;
static SMURF_PIC *pic_active;

static SMURF_PIC **smurf_picture;
GARGAMEL **smurf_struct;
BASPAG **edit_bp;
static short *active_pic;

PLUGIN_FUNCTIONS *smurf_functions;
SMURF_VARIABLES *smurf_vars;

static LIST_FIELD exporter_list;
static POP_UP depth_popup;
static POP_UP dither_popup;
static POP_UP palette_popup;

/*
*   Informationen des gewÑhlten Exporters: 
*/
EXPORT_CONFIG original_exp_conf;
EXPORT_CONFIG *exp_conf;				/* dies ist die Struktur aus Smurf, um ihn beim Dithern zu betrÅgen */
EXPORT_CONFIG my_exp_conf;
MOD_ABILITY expmabs;
char *export_path;
short mod_num;
WORD exp_depth;

WORD fix_red[256];
WORD fix_green[256];
WORD fix_blue[256];

static char Path[256] = "C:\\";


/* init_rsh -----------------------------------------------------
    Initialisiert die Resource 
    -----------------------------------------------------------*/
static void init_rsh(void)
{
	WORD t;

	/* Resource Umbauen */
	for (t = 0; t < NUM_OBS; t++)
		rsrc_obfix(rs_object, t);

	dialog = rs_trindex[MULTICON];		/* Resourcebaum holen */
	alerts = rs_frstr;
}



static void init_windstruct(void)
{

	window.module = my_id;
	window.wnum = 1;					/* Das wievielte Fenster des Moduls? */
	window.wx = -1;
	window.wy = -1;

	strcpy(window.wtitle, "Multikonverter");	/* Fenstertitel */

	window.resource_form = dialog;		/* Modulresource-Formular */
	window.picture = NULL;				/* Zeigerfeld fÅr Bild/Animation */
	window.editob = WILDCARDS;
	window.nextedit = 0;
	window.editx = 0;
	window.pflag = 0;

	window.prev_window = NULL;			/* vorheriges Fenster (WINDOW*) */
	window.next_window = NULL;			/* nÑxtes Fenster (WINDOW*) */


	memcpy(&exporter_list, smurf_vars->export_list, sizeof(LIST_FIELD));
	exporter_list.max_entries = 9;
	exporter_list.parent_obj = EXPLIST_PARENT;
	exporter_list.slider_parent = EXPLIST_SB;	/* Parentobjekt des Sliders     */
	exporter_list.slider_obj = EXPLIST_SL;	/* Sliderobjekt                 */
	exporter_list.slar_up = EXPLIST_UP;	/* Slider-Arrow hoch            */
	exporter_list.slar_dn = EXPLIST_DN;	/* Slider-Arrow runter          */
	exporter_list.scroll_offset = 0;	/* Scrolloffset der Liste       */
	exporter_list.autolocator = NULL;	/* Zeiger auf Autolocator-String */
	exporter_list.auto_len = 0;

	memcpy(&depth_popup, &smurf_vars->pop_ups[POPUP_EXP_DEPTH], sizeof(POP_UP));
	depth_popup.item = 7;				/* Item (=letzter Eintrag)      */
	depth_popup.display_tree = dialog;	/* Tree, in dems dargestellt werden soll */
	depth_popup.display_obj = DEPTH_PB;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	depth_popup.Cyclebutton = DEPTH_CB;	/* Index des Cyclebuttons */

	memcpy(&palette_popup, &smurf_vars->pop_ups[POPUP_COLRED], sizeof(POP_UP));
	palette_popup.item = 1;				/* Item (=letzter Eintrag)      */
	palette_popup.display_tree = dialog;	/* Tree, in dems dargestellt werden soll */
	palette_popup.display_obj = PALMODE_PB;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	palette_popup.Cyclebutton = PALMODE_CB;	/* Index des Cyclebuttons */

	memcpy(&dither_popup, &smurf_vars->pop_ups[POPUP_DITHER], sizeof(POP_UP));
	dither_popup.item = 1;				/* Item (=letzter Eintrag)      */
	dither_popup.display_tree = dialog;	/* Tree, in dems dargestellt werden soll */
	dither_popup.display_obj = DITHER_PB;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	dither_popup.Cyclebutton = DITHER_CB;	/* Index des Cyclebuttons */
}



/* actualize_dialog ----------------------------------------------
    Aktualisiert den Dialog
    ------------------------------------------------------------*/
#if 0 /* unused */
static void actualize_dialog(void)
{
	services->redraw_window(&window, NULL, 0, 0);
}
#endif



/* open_exporter options ------------------------------------------------
    Bringt den geladenen Exporter dazu, seinen internen Optionsdialog zu
    îffnen.
    ---------------------------------------------------------------------*/
static void open_exporter_options(void)
{
	smurf_functions->start_exp_module(export_path, MMORE, NULL, edit_bp[mod_num & 0xFF], smurf_struct[mod_num & 0xFF], mod_num);
	options_open = 1;
}



/* give_free_module -------------------------------------------------------
    ermittelt eine freie Smurf-Modulstruktur in den Smurf-internen Arrays und
    gibt einen entsprechenden Index zurÅck, der fÅr smurf_struct verwendet werden kann.
    -----------------------------------------------------------------------*/
static short give_free_module(void)
{
	short mod;

	/* Freie Modulstruktur ermitteln */
	mod = 0;
	while (smurf_struct[mod] != NULL)
		mod++;
	if (mod > 20)
	{
		services->f_alert(smurf_vars->alerts[NO_MORE_MODULES].TextCast, 0, 0, 0, 1);
		mod = -1;
	}

	return mod;
}


/* analyze_module --------------------------------------------------------
    LÑdt das Modul mit dem Pfad export_path mit der ID mod_num und kopiert
    die Infostrukturen in expmabs.
    ---------------------------------------------------------------------*/
static void analyze_module(MOD_ABILITY *expmabs, char *export_path, short mod_num)
{
	MOD_ABILITY *export_mabs;

	mod_num = give_free_module();
	if (mod_num == -1)
	{
		services->f_alert(smurf_vars->alerts[MOD_LOAD_ERR].TextCast, 0, 0, 0, 1);
		return;
	}
	mod_num |= MOD_EXPORT;					/* als Exporter kennzeichnen */

	smurf_struct[mod_num & 0xFF] = malloc(sizeof(GARGAMEL));
	memset(smurf_struct[mod_num & 0xFF], 0x0, sizeof(GARGAMEL));

	/*
	 * Modul analysieren
	 */
	export_mabs = (MOD_ABILITY *) smurf_functions->start_exp_module(export_path, MQUERY, NULL, edit_bp[mod_num & 0xFF],
		smurf_struct[mod_num & 0xFF], mod_num);
	memcpy(expmabs, export_mabs, sizeof(MOD_ABILITY));

	if (expmabs->ext_flag & 0x02)
		dialog[EXPORTER_OPTIONS].ob_state &= ~OS_DISABLED;
	else
		dialog[EXPORTER_OPTIONS].ob_state |= OS_DISABLED;

	services->redraw_window(&window, NULL, EXPORTER_OPTIONS, 0);

	smurf_functions->check_and_terminate(MTERM, mod_num & 0xFF);	/* Modul wieder terminieren */
}


/* ready_depth_popup ------------------------------------------------------------
    Paût die Farbtiefen im Smurf-Popup fÅr Exportfarbtiefen nach den ModulfÑhigkeiten
    in expmabs an.
    -----------------------------------------------------------------------------*/
static void ready_depth_popup(MOD_ABILITY *expmabs)
{
	short export_depth[10];
	short t;
	WORD ob;

	export_depth[0] = expmabs->depth1;
	export_depth[1] = expmabs->depth2;
	export_depth[2] = expmabs->depth3;
	export_depth[3] = expmabs->depth4;
	export_depth[4] = expmabs->depth5;
	export_depth[5] = expmabs->depth6;
	export_depth[6] = expmabs->depth7;
	export_depth[7] = expmabs->depth8;

	/*
	 *   Depth-Popup vorbereiten 
	 */
	ob = EXP_D1;
	for (t = 0; t <= 6; t++)
	{
		smurf_vars->exp_dp_popup[ob].ob_state |= OS_DISABLED;
		ob++;
	}
	for (t = 0; t < 8; t++)
	{
		switch (export_depth[t])
		{
		case 24:
			smurf_vars->exp_dp_popup[EXP_D24].ob_state &= ~OS_DISABLED;
			break;
		case 16:
			smurf_vars->exp_dp_popup[EXP_D16].ob_state &= ~OS_DISABLED;
			break;
		case 8:
			smurf_vars->exp_dp_popup[EXP_D8].ob_state &= ~OS_DISABLED;
			break;
		case 4:
			smurf_vars->exp_dp_popup[EXP_D4].ob_state &= ~OS_DISABLED;
			break;
		case 2:
			smurf_vars->exp_dp_popup[EXP_D2].ob_state &= ~OS_DISABLED;
			break;
		case 1:
			smurf_vars->exp_dp_popup[EXP_D1].ob_state &= ~OS_DISABLED;
			break;
		}
	}
}


/* init_exporter --------------------------------------------------------
    LÑdt den Exporter der globalen ID mod_num und startet diesen mit MSTART
    bei gleichzeitiger öbergabe seines Konfigurationsblocks.
    ----------------------------------------------------------------------*/
static void init_exporter(void)
{
	const MODULE_START *module_start;
	char module_name[33] = "";
	short t;
	const MOD_INFO *modinfo;
	void **pp;
	
	mod_num = give_free_module();
	if (mod_num == -1)
	{
		services->f_alert(smurf_vars->alerts[MOD_LOAD_ERR].TextCast, 0, 0, 0, 1);
		return;
	}
	mod_num |= MOD_EXPORT;					/* als Exporter kennzeichnen */

	smurf_struct[mod_num & 0xFF] = malloc(sizeof(GARGAMEL));
	memset(smurf_struct[mod_num & 0xFF], 0x0, sizeof(GARGAMEL));

	edit_bp[mod_num & 0xFF] = (BASPAG *) smurf_functions->start_exp_module(export_path, MSTART, NULL, edit_bp[mod_num & 0xFF], smurf_struct[mod_num & 0xFF], mod_num);
	smurf_struct[mod_num & 0xFF]->wind_struct = NULL;

	module_start = smurf_functions->get_module_start(edit_bp[mod_num & 0xFF]);
	modinfo = module_start->info;	/* Zeiger auf Modulinfostruktur */

	strncpy(module_name, modinfo->mod_name, 30);

	for (t = 0; t < smurf_vars->anzahl_exporter; t++)
	{
		if (strncmp(module_name, smurf_vars->export_module_names[t], strlen(module_name)) == 0)
			break;
	}
	pp = (void **)smurf_struct[mod_num & 0xFF]->event_par;
	*pp = smurf_vars->export_cnfblock[t];
	smurf_struct[mod_num & 0xFF]->event_par[2] = smurf_vars->export_cnflen[t];
}


static void check_depth(void)
{
	WORD enablemode;

	if (exp_depth == EXP_D1)
	{
		strcpy(dialog[PALMODE_PB].TextCast, "s/w");
		dialog[PALMODE_CB].ob_state |= OS_DISABLED;
		dialog[PALMODE_PB].ob_state |= OS_DISABLED;
		my_exp_conf.exp_colred = CR_SYSPAL;
	} else
	{
		strcpy(dialog[PALMODE_PB].TextCast, smurf_vars->colred_popup[my_exp_conf.exp_colred].TextCast);
		dialog[PALMODE_CB].ob_state &= ~OS_DISABLED;
		dialog[PALMODE_PB].ob_state &= ~OS_DISABLED;
	}

	services->deselect_popup(&window, PALMODE_PB, PALMODE_CB);


	if (exp_depth != EXP_D16 && exp_depth > EXP_D1)
	{
		if (smurf_vars->ditmod_info[my_exp_conf.exp_dither - 1]->pal_mode == FIXPAL)
		{
			palette_popup.item = CR_FIXPAL;
			my_exp_conf.exp_colred = CR_FIXPAL;
			strncpy(dialog[PALMODE_PB].TextCast, smurf_vars->colred_popup[my_exp_conf.exp_colred].TextCast, 15);
			enablemode = OS_DISABLED;
		} else
		{
			palette_popup.item = CR_SYSPAL;
			my_exp_conf.exp_colred = CR_SYSPAL;
			strncpy(dialog[PALMODE_PB].TextCast, smurf_vars->colred_popup[my_exp_conf.exp_colred].TextCast, 15);
			enablemode = OS_ENABLED;
		}

		if (enablemode == OS_DISABLED)
		{
			dialog[PALMODE_PB].ob_state |= OS_DISABLED;
			dialog[PALMODE_CB].ob_state |= OS_DISABLED;
		} else
		{
			dialog[PALMODE_PB].ob_state &= ~OS_DISABLED;
			dialog[PALMODE_CB].ob_state &= ~OS_DISABLED;
		}
		services->deselect_popup(&window, PALMODE_PB, PALMODE_CB);
	}
}


static WORD depth2popbutton(short depth)
{
	WORD exp_depth;

	if (depth < 2)
		exp_depth = EXP_D1;
	else if (depth < 4)
		exp_depth = EXP_D2;
	else if (depth < 8)
		exp_depth = EXP_D4;
	else if (depth < 16)
		exp_depth = EXP_D8;
	else if (depth < 24)
		exp_depth = EXP_D16;
	else
		exp_depth = EXP_D24;

	return exp_depth;
}


static void init_colpop(void)
{
	exp_depth = depth2popbutton(expmabs.depth1);

	if (expmabs.depth2 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth2);
	if (expmabs.depth3 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth3);
	if (expmabs.depth4 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth4);
	if (expmabs.depth5 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth5);
	if (expmabs.depth6 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth6);
	if (expmabs.depth7 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth7);
	if (expmabs.depth8 > expmabs.depth1)
		exp_depth = depth2popbutton(expmabs.depth8);

	strcpy(dialog[DEPTH_PB].TextCast, smurf_vars->exp_dp_popup[exp_depth].TextCast);
	services->deselect_popup(&window, DEPTH_PB, DEPTH_CB);
	depth_popup.item = exp_depth;

	check_depth();
}


static BOOLEAN load_palfile(char *path, WORD *red, WORD *green, WORD *blue, short max_cols, char *pal_loadpath)
{
	short fsback;
	WORD *palbuf;
	WORD *palcpy;
	short max_count;
	short t;

	strcpy(pal_loadpath, path);
	/* FIXME: translate */
	fsback = smurf_functions->f_fsbox(pal_loadpath, "Palette laden", 0);

	if (fsback != FALSE)
	{
		palbuf = (WORD *) fload(pal_loadpath, 0);
		max_count = (short) (f_len / 6);
		if (max_count > max_cols)
		{
			services->f_alert(smurf_vars->alerts[PAL_DEPTHERR].TextCast, 0, 0, 0, 1);
			return FALSE;
		} else
		{
			palcpy = palbuf;
			/* Mit der ersten Farbe im File die Palette ausnullen */
			for (t = 0; t < 256; t++)
			{
				red[t] = (WORD) (255L * (long) palcpy[0] / 1000L);
				green[t] = (WORD) (255L * (long) palcpy[1] / 1000L);
				blue[t] = (WORD) (255L * (long) palcpy[2] / 1000L);
			}

			/* und Åbertragen */
			for (t = 0; t < max_count; t++)
			{
				red[t] = (WORD) (255L * (long) palcpy[t * 3] / 1000L);
				green[t] = (WORD) (255L * (long) palcpy[t * 3 + 1] / 1000L);
				blue[t] = (WORD) (255L * (long) palcpy[t * 3 + 2] / 1000L);
			}
		}
		Mfree(palbuf);
		return TRUE;
	}

	return FALSE;
}


/* handle_dialog ----------------------------------------------
    öbernimmt die Userbedienung des Dialogs
    ----------------------------------------------------------*/
static void handle_dialog(PLUGIN_DATA *data)
{
	WORD button;
	WORD back;
	WORD exp_index;
	short t;
	char module_name[33] = "";
	const MODULE_START *module_start;
	const MOD_INFO *minfo;
	char pal_loadpath[256];
	void **pp;

	if (data->message == MBEVT)
	{

		button = data->event_par[0];
		switch (button)
		{
		case DEPTH_PB:
		case DEPTH_CB:
			back = services->popup(&depth_popup, 0, button, NULL);
			services->deselect_popup(&window, DEPTH_PB, DEPTH_CB);
			if (back > 0)
				exp_depth = back;
			check_depth();
			break;

		case DITHER_PB:
		case DITHER_CB:
			back = services->popup(&dither_popup, 0, button, NULL);
			services->deselect_popup(&window, DITHER_PB, DITHER_CB);
			if (back > 0)
				my_exp_conf.exp_dither = back;
			check_depth();
			break;

		case PALMODE_PB:
		case PALMODE_CB:
			back = services->popup(&palette_popup, 0, button, NULL);
			services->deselect_popup(&window, PALMODE_PB, PALMODE_CB);
			if (back > 0)
				my_exp_conf.exp_colred = back;

			if (my_exp_conf.exp_colred == CR_FILEPAL)
				change_object(&window, LOADPAL, OS_ENABLED, 1);
			else
				change_object(&window, LOADPAL, OS_DISABLED, 1);
			break;

		case EXPORTER_OPTIONS:
			open_exporter_options();
			break;

		case SOURCE_PATH:
			back = services->f_fsbox(Path, "Quellpfad", 6);
			strncpy(dialog[SOURCE_PATH].TextCast, Path, 53);
			dialog[SOURCE_PATH].TextCast[54] = 0;
			services->redraw_window(&window, NULL, SOURCE_PATH, 0);
			break;

		case DEST_PATH:
			back = services->f_fsbox(Path, "Zielpfad", 6);
			strncpy(dialog[DEST_PATH].TextCast, Path, 53);
			dialog[DEST_PATH].TextCast[54] = 0;
			services->redraw_window(&window, NULL, DEST_PATH, 0);
			break;

		case LOADPAL:
			if (load_palfile(smurf_vars->Sys_info->standard_path,
										fix_red, fix_green, fix_blue, 1 << depth_button2depth(exp_depth), pal_loadpath))
			{
				strcpy(smurf_vars->colred_popup[CR_FILEPAL].TextCast, strrchr(pal_loadpath, '\\') + 1);
				strcpy(dialog[PALMODE_PB].TextCast, strrchr(pal_loadpath, '\\') + 1);
			}
			change_object(&window, DITHER_PB, OS_UNSEL, 1);
			change_object(&window, LOADPAL, OS_UNSEL, 1);
			break;

		case GO:
			start_conversion();
			break;
		}

		/*
		 * Hat sich der gewÑhlte Exporter geÑndert? -> Farbtiefenpopup neu initialisieren
		 */
		if ((button >= EXPLIST_1 && button <= EXPLIST_9) ||
			button == EXPLIST_UP || button == EXPLIST_DN || button == EXPLIST_SL || button == EXPLIST_SB)
		{
			/*
			 * ist der Exporter schon durch Fensterschlieûen
			 * terminiert worden? Dann darf hier nix mehr gemacht werden
			 */
			if (edit_bp[mod_num & 0xFF] != NULL)
			{
				/*
				 * bisherigen Optionsdialog schlieûen
				 */
				if (options_open)
				{
					options_open = 0;

					module_start = smurf_functions->get_module_start(edit_bp[mod_num & 0xFF]);
					minfo = module_start->info;	/* Zeiger auf Modulinfostruktur */
					strncpy(module_name, minfo->mod_name, 30);

					for (t = 0; t < smurf_vars->anzahl_exporter; t++)
					{
						if (strncmp(module_name, smurf_vars->export_module_names[t], strlen(module_name)) == 0)
							break;
					}

					pp = (void **)smurf_struct[mod_num & 0xFF]->event_par;
					*pp = smurf_vars->export_cnfblock[t];
					smurf_struct[mod_num & 0xFF]->event_par[2] = smurf_vars->export_cnflen[t];

					smurf_functions->start_exp_module(export_path, MMORECANC, NULL, edit_bp[mod_num & 0xFF],
													  smurf_struct[mod_num & 0xFF], mod_num);
					wind_close(smurf_struct[mod_num & 0xFF]->wind_struct->whandlem);
					wind_delete(smurf_struct[mod_num & 0xFF]->wind_struct->whandlem);
					smurf_struct[mod_num & 0xFF]->wind_struct->whandlem = -1;
				}

				/* momentanen Exporter wieder terminieren
				 */
				smurf_functions->check_and_terminate(MTERM, mod_num & 0xFF);
			}

			exp_index = services->listfield(&window, button, 0, &exporter_list);
			export_path = smurf_vars->export_modules[exp_index];
			analyze_module(&expmabs, export_path, mod_num);
			ready_depth_popup(&expmabs);
			init_exporter();
			init_colpop();
		}
	}

}


/* handle_aesmsg ----------------------------------------------
    kÅmmert sich um von Smurf weitergeleitete AES-Messages
    ----------------------------------------------------------*/
static short handle_aesmsg(WORD *msgbuf)
{
	(void) msgbuf;
	return M_WAITING;
}


void plugin_main(PLUGIN_DATA *data)
{
	WORD exp_index;

	/* Die Strukturen mit den Variablen und Funktionen holen */
	services = data->services;
	smurf_functions = data->smurf_functions;
	smurf_vars = data->smurf_vars;
	smurf_struct = smurf_vars->smurf_struct;
	smurf_picture = smurf_vars->smurf_picture;
	edit_bp = smurf_vars->edit_bp;
	active_pic = smurf_vars->active_pic;
	exp_conf = smurf_vars->exp_conf;

	switch (data->message)
	{
		/* 
		 * Startup des Plugins: MenÅeintrag anfordern 
		 */
	case MSTART:
		my_id = data->id;
		init_rsh();
		strcpy(data->plugin_name, "Multikonvert...");
		init_windstruct();
		data->wind_struct = &window;
		data->message = MENU_ENTRY;		/* MenÅeintrag anfordern */
		/* plugin responds to this menu entry */
		data->event_par[0] = FILE_FOLDER;
		break;

		/*
		 * Plugin wurde aus dem MenÅ heraus aufgerufen 
		 */
	case PLGSELECTED:
		pic_active = smurf_picture[*active_pic];

		/*
		 * Fenster îffnen und Dialog initialisieren
		 */
		my_exp_conf.exp_dither = DIT1;
		my_exp_conf.exp_colred = CR_SYSPAL;

		strcpy(dialog[DITHER_PB].TextCast, smurf_vars->col_pop[DIT1].TextCast);
		strcpy(dialog[PALMODE_PB].TextCast, smurf_vars->colred_popup[CR_SYSPAL].TextCast);

		services->f_module_window(&window);
		dialog_open = TRUE;

		services->listfield(&window, 99, 99, &exporter_list);

		exp_index = services->listfield(&window, 0, 0, &exporter_list);
		export_path = smurf_vars->export_modules[exp_index];
		analyze_module(&expmabs, export_path, mod_num);
		ready_depth_popup(&expmabs);
		init_exporter();

		init_colpop();

		strcpy(dialog[SOURCE_PATH].TextCast, "");
		strcpy(dialog[DEST_PATH].TextCast, "");
		strcpy(dialog[WILDCARDS].TextCast, "*");

		data->message = M_WAITING;
		break;

		/*
		 * Fenster wurde geschlossen
		 */
	case MWINDCLOSED:
		smurf_functions->check_and_terminate(MTERM, mod_num & 0xFF);
		dialog_open = FALSE;
		break;

		/*
		 * Keyboardevent/Buttonevent
		 */
	case MKEVT:
	case MBEVT:
		handle_dialog(data);
		data->message = M_WAITING;
		break;

	case MTERM:
		data->message = M_TERMINATED;
		break;

	case SMURF_AES_MESSAGE:
		data->message = handle_aesmsg(data->event_par);
		break;

	default:
		data->message = M_WAITING;
		break;
	}
}
