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
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "globdefs.h"
#include "plugin.h"
#include "popdefin.h"
#include "../../../src/smurf_f.h"
#include "../../../src/smurfobs.h"

#include "country.h"

#undef COUNTRY
#define COUNTRY 1

#if COUNTRY==1
#include "de/mrm.rsh"
#define TITLE           "Smurf ModComm-Protokoll Plugin"
#define WINTITLE        "Kommunikationsprotokoll"
#define AL_LOAD_MODULE  "Error loading module!"
#define AL_ADJUST_TPA   "Error adjusting module TPA!"
#define AL_START_MODULE "Error starting module!"
#define AL_MODULE_ID    "Error: invalid module ID!"
#define AL_LOAD_MODULES "Error loading modules."
#elif COUNTRY==0
#include "en/mrm.rsh"
#define TITLE           "Smurf ModComm-Protocol Plugin"
#define WINTITLE        "Communication protocol"
#define AL_LOAD_MODULE  "Error loading module!"
#define AL_ADJUST_TPA   "Error adjusting module TPA!"
#define AL_START_MODULE "Error starting module!"
#define AL_MODULE_ID    "Error: invalid module ID!"
#define AL_LOAD_MODULES "Error loading modules."
#elif COUNTRY==2
#include "en/mrm.rsh"		/* missing french resource */
#define TITLE           "Smurf ModComm-Protocol Plugin"
#define WINTITLE        "Communication protocol"
#define AL_LOAD_MODULE  "Error loading module!"
#define AL_ADJUST_TPA   "Error adjusting module TPA!"
#define AL_START_MODULE "Error starting module!"
#define AL_MODULE_ID    "Error: invalid module ID!"
#define AL_LOAD_MODULES "Error loading modules."
#else
#error "unsupported language"
#endif

static char const name_string[] = TITLE;

PLUGIN_INFO plugin_info = {
	name_string,
	0x100,								/* Plugin-Version 1.00 */
	SMURF_VERSION,						/* fuer Smurf-Version 1.06 */
	0									/* und nicht resident. */
};


#ifdef __PUREC__
void *_FilSysVec; /* missing in our startup code, needed by fopen() */
#endif

struct logline {
	char text[70];
};
#define NUM_LOGLINES 20
#if (COMM_LASTLINE - COMM_FIRSTLINE + 1) != NUM_LOGLINES
# error
#endif

static char const message_names[CONFIG_TRANSMIT - MTERM + 1][16] = {
	"MTERM",
	"MSTART",
	"MWRED",
	"MBEVT",
	"MEXEC",
	"MWMOVED",
	"MNEXTPIC",
	"MQUERY",
	"MPICTURE",
	"MMORE",
	"MEXTEND",
	"MMORECANC",
	"MCOLSYS",
	"MCROSSHAIR",
	"MCH_COORDS",
	"MPICS",
	"MCH_DEFCOO",
	"MDITHER_READY",
	"MPIC_UPDATE",
	"MKEVT",
	"DONE",
	"GETCONFIG",
	"CONFIG_TRANSMIT"
};

static char const result_names[M_WAITING - M_CONFSAVE + 1][16] = {
	"M_CONFSAVE",
	"M_SILENT_ERR",
	"M_CONFIG",
	"M_TERMINATED",
	"ALL_MSGS",
	"WINDEVENTS",
	"M_CH_OFF",
	"M_CH_COORDS",
	"M_PICTURE",
	"M_CH_COORDS",
	"M_CROSSHAIR",
	"M_COLSYS",
	"M_MOREOK",
	"M_EXTEND",
	"M_STARTED",
	"M_MODPIC",
	"M_UNKNOWN_TYPE",
	"M_PICERR",
	"M_PALERR",
	"M_DONEEXIT",
	"M_EXIT",
	"M_IOERROR",
	"M_MEMORY",
	"M_MODERR",
	"M_INVALID",
	"M_PICDONE",
	"M_WAITING"
};

static FILE *logfile = NULL;
static SERVICE_FUNCTIONS *services;
static PLUGIN_FUNCTIONS *smurf_functions;
static CLASS_MODULE *module_ptr;
static CLASS_MODULE module;
static SMURF_VARIABLES *smurf_vars;
static OBJECT *maintree;
static WINDOW win;
static short my_id;
struct logline logbuffer[NUM_LOGLINES + 1];
static void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);
static char logfile_name[256];

short start_imp_module(char *modpath, SMURF_PIC *imp_pic);
BASPAG *start_edit_module(char *modpath, BASPAG *edit_basepage, short mode, short mod_id, GARGAMEL *sm_struct);
long get_proclen(BASPAG *baspag);


/* get_proclen
   -------------------------------------------------
   Ermittelt die Gesamtlaenge des Prozesses mit der Basepage baspag.
   ----------------------------------------------------------------
 */
long get_proclen(BASPAG *baspag)
{
	/* BASEPAGE + Textsegment + Datensegment + BSS + Stack */
	return sizeof(*baspag) + baspag->p_tlen + baspag->p_dlen + baspag->p_blen + 1024L;
}


static void log_line(short message, short module_number, const char *mod_name)
{
	short i;
	WORD dummy;
	WORD mox;
	WORD moy;
	char buf[6];
	
	for (i = 0; i < NUM_LOGLINES - 1; i++)
	{
		strcpy(logbuffer[i].text, logbuffer[i + 1].text);
		maintree[i + COMM_FIRSTLINE].ob_spec.tedinfo->te_ptext = logbuffer[i].text;
	}
	if (module_number >= 0)
		strcpy(logbuffer[NUM_LOGLINES - 1].text, "  "); 
	else
		strcpy(logbuffer[NUM_LOGLINES - 1].text, "r ");
	strncat(logbuffer[NUM_LOGLINES - 1].text, mod_name, 34);
	strncat(logbuffer[NUM_LOGLINES - 1].text, "                                          ", 34 - strlen(mod_name));
	if (module_number >= 0)
	{
		itoa(module_number, buf, 10);
		strncat(logbuffer[NUM_LOGLINES - 1].text, buf, 3);
		strncat(logbuffer[NUM_LOGLINES - 1].text, "     ", 5 - strlen(buf));
		if (message >= MTERM && message <= CONFIG_TRANSMIT)
			strcat(logbuffer[NUM_LOGLINES - 1].text, message_names[message - MTERM]);
		else
			strcat(logbuffer[NUM_LOGLINES - 1].text, "<no msg>");
	} else
	{
		strncat(logbuffer[NUM_LOGLINES - 1].text, "     ", 5);
		if (message >= M_CONFSAVE && message <= M_WAITING)
			strcat(logbuffer[NUM_LOGLINES - 1].text, result_names[message - M_CONFSAVE]);
		else
			strcat(logbuffer[NUM_LOGLINES - 1].text, "<no msg>");
	}
	maintree[COMM_LASTLINE].ob_spec.tedinfo->te_ptext = logbuffer[NUM_LOGLINES - 1].text;
	for (i = 0; i <= NUM_LOGLINES - 1; i++)
	{
		if (logbuffer[i].text[0] == 'r')
		{
			maintree[i + COMM_FIRSTLINE].ob_spec.tedinfo->te_color &= 0xf0ff;
			maintree[i + COMM_FIRSTLINE].ob_spec.tedinfo->te_color |= 0x0b00;
		} else
		{
			maintree[i + COMM_FIRSTLINE].ob_spec.tedinfo->te_color &= 0xf0ff;
		}
	}
	
	redraw_window(&win, NULL, COMM_BOX, 0);
	if ((maintree[COMM_STEP].ob_state & OS_SELECTED) && logfile != NULL)
	{
		maintree[COMM_CONT].ob_state &= ~OS_DISABLED;
		redraw_window(&win, NULL, COMM_CONT, 0);
		do
		{
			evnt_button(1, 1, 1, &mox, &moy, &dummy, &dummy);
		} while (objc_find(maintree, ROOT, MAX_DEPTH, mox, moy) != COMM_CONT);
		maintree[COMM_CONT].ob_state &= ~OS_SELECTED;
		maintree[COMM_CONT].ob_state |= OS_DISABLED;
		redraw_window(&win, NULL, COMM_CONT, 0);
	}
	
	if ((maintree[COMM_LOGFILE].ob_state & OS_SELECTED) && logfile)
	{
		fprintf(logfile, "%s\n", logbuffer[NUM_LOGLINES - 1].text);
		fflush(logfile);
	}
}


static void init_rsh(void)
{
	WORD t;

	/* Resource Umbauen */
	for (t = 0; t < NUM_OBS; t++)
		rsrc_obfix(&rs_object[t], 0);

	maintree = rs_trindex[MODCOMM];		/* Resourcebaum holen */
}


static void init_windstruct(void)
{
	win.whandlem = 0;			/* evtl. Handle loeschen */
	win.module = my_id;
	win.wnum = 1;				/* Das wievielte Fenster des Moduls? */

	win.wx = -1;
	win.wy = -1;
	win.ww = maintree->ob_width;
	win.wh = maintree->ob_height;

	strcpy(win.wtitle, WINTITLE);	/* Fenstertitel */

	win.resource_form = maintree;	/* Modulresource-Formular */
	win.picture = NULL;			/* Zeigerfeld fuer Bild/Animation */
	win.editob = 0;
	win.nextedit = 0;
	win.editx = 0;
	win.pflag = 0;

	win.prev_window = NULL;	/* prev window (WINDOW*) */
	win.next_window = NULL;	/* next window (WINDOW*) */
}


short start_imp_module(char *modpath, SMURF_PIC *imp_pic)
{
	const MODULE_START *module_start;
	short (*module_main)(GARGAMEL *smurf_struct);
	short module_return;
	short back;
	long ProcLen;
	long temp;
	long lback;
	char alstring[80];
	BASPAG *mod_basepage;
	const MOD_INFO *module_info;
	GARGAMEL sm_struct;

	/* Modul als Overlay laden und Basepage ermitteln */
	temp = Pexec(3, modpath, "", NULL);
	if (temp <= 0)
	{
		strcpy(alstring, AL_LOAD_MODULE);
		strcat(alstring, strrchr(modpath, '\\') + 1);
		services->f_alert(alstring, NULL, NULL, NULL, 1);
		module_return = M_STARTERR;
	} else
	{
		mod_basepage = (BASPAG *) temp;

		module_start = smurf_functions->get_module_start(mod_basepage);
		/* Zeiger auf Magic (muss MOD_MAGIC_IMPORT sein!) */
		if (module_start == NULL || module_start->magic != MOD_MAGIC_IMPORT)
			return M_MODERR;

		/* Laenge des gesamten Tochterprozesses ermitteln */
		ProcLen = get_proclen(mod_basepage);
		back = _Mshrink(mod_basepage, ProcLen);	/* Speicherblock verkuerzen */
		if (back != 0)
			services->f_alert(AL_ADJUST_TPA, NULL, NULL, NULL, 1);

		mod_basepage->p_hitpa = (void *) ((char *)mod_basepage + ProcLen);

		lback = Pexec(4, NULL, (char *) mod_basepage, NULL);
		if (lback != 0)
			services->f_alert(AL_START_MODULE, NULL, NULL, NULL, 1);

		module_info = module_start->info;	/* Zeiger auf Modulinfostruktur */
		sm_struct.smurf_pic = imp_pic;
		sm_struct.services = services;
		/*
		 * only valid mode for export modules,
		 * but set it nevertheless in case some module queries it.
		 */
		sm_struct.module_mode = MEXEC;

		/* BUG: module_number not initialized */
		log_line(sm_struct.module_mode, sm_struct.module_number, module_info->mod_name);
		
		module_main = (short (*)(GARGAMEL *)) (module_start->entry);
		module_return = module_main(&sm_struct);
		log_line(sm_struct.module_mode, -1, module_info->mod_name);

#if 0
		Pexec(102, NULL, mod_basepage, NULL);	/* Modul systemkonform toeten */
#endif
		services->SMfree(mod_basepage->p_env);
		services->SMfree(mod_basepage);			/* Modul-Basepage freigeben */
	}

	return module_return;
}


BASPAG *start_edit_module(char *modpath, BASPAG *edit_basepage, short mode, short mod_id, GARGAMEL *sm_struct)
{
	void (*module_main)(GARGAMEL *smurf_struct);
	const MODULE_START *module_start;
	short back;
	long ProcLen;
	long temp;
	long lback;
	const MOD_ABILITY *mod_abs;
	const MOD_INFO *module_info;

	if (mod_id < 0 || mod_id > MAX_MODS - 1)
		services->f_alert(AL_MODULE_ID, NULL, NULL, NULL, 1);

	/*
	 * Modul als Overlay laden und Basepage ermitteln
	 */
	if (edit_basepage == NULL)			/* Modul wurde noch nicht gestartet! */
	{
		temp = Pexec(3, modpath, "", NULL);
		if (temp <= 0)
		{
			services->f_alert(AL_LOAD_MODULES, NULL, NULL, NULL, 1);
			sm_struct->module_mode = M_STARTERR;
			return NULL;
		} else
		{
			edit_basepage = (BASPAG *) temp;

			/* Laenge des gesamten Tochterprozesses ermitteln */
			ProcLen = get_proclen(edit_basepage);
			back = _Mshrink(edit_basepage, ProcLen);	/* Speicherblock verkuerzen */
			if (back != 0)
				services->f_alert(AL_ADJUST_TPA, NULL, NULL, NULL, 1);

			edit_basepage->p_hitpa = (void *) ((char *)edit_basepage + ProcLen);

			lback = Pexec(4, NULL, (char *) edit_basepage, NULL);
			if (lback != 0)
				services->f_alert(AL_START_MODULE, NULL, NULL, NULL, 1);
		}
	}

	if (edit_basepage != NULL)
	{
		/* Zeiger auf Magic (muss MOD_MAGIC_EDIT sein!) */
		module_start = smurf_functions->get_module_start(edit_basepage);
		if (module_start == NULL || module_start->magic != MOD_MAGIC_EDIT)
		{
			services->f_alert(AL_LOAD_MODULE, NULL, NULL, NULL, 1);
			return NULL;
		}

		/* Modulkennung (als wievieltes Modul gestartet?) */
		if (mode == MSTART)
			sm_struct->module_number = mod_id;
		/* Message von Smurf */
		sm_struct->module_mode = mode;	/* 0=laden und Starten, 1=Redraw ausfuehren , -1 = Beenden, 2=aktivieren */
		
		/* Funktionen einhaengen */
		sm_struct->services = services;

		/* EVENT im Modulfenster ! */
		if (mode == MBEVT || mode == MKEVT || mode == SMURF_AES_MESSAGE)
		{
			sm_struct->mousex = *smurf_vars->mouse_xpos;
			sm_struct->mousey = *smurf_vars->mouse_ypos;
			sm_struct->klicks = *smurf_vars->klicks;

			if (mode != SMURF_AES_MESSAGE)
				sm_struct->event_par[0] = *smurf_vars->obj;

			if (mode == MKEVT)
			{
				sm_struct->event_par[1] = *smurf_vars->key_scancode;
				sm_struct->event_par[2] = *smurf_vars->key_ascii;
				sm_struct->event_par[3] = *smurf_vars->key_at_event;
			}
		}

		module_main = (void (*)(GARGAMEL *smurf_struct)) (module_start->entry);

		if (mode != MQUERY)
		{
			if (mode == MEXEC)
				graf_mouse(BUSYBEE, NULL);

			module_info = module_start->info;
			log_line(sm_struct->module_mode, sm_struct->module_number, module_info->mod_name);

			lback = Pexec(4, NULL, (char *) edit_basepage, NULL);
			if (lback != 0)
				services->f_alert(AL_START_MODULE, NULL, NULL, NULL, 1);

			module_main(sm_struct);

			log_line(sm_struct->module_mode, -1, module_info->mod_name);

			graf_mouse(ARROW, NULL);
		}

		if (mode == MQUERY)
		{
			module.bp[mod_id & 0xFF] = edit_basepage;
			smurf_vars->edit_bp[mod_id & 0xFF] = edit_basepage;
			mod_abs = module_start->ability;	/* Module Abilities */
			return (BASPAG *) mod_abs;
		}
	}

	return edit_basepage;
}


void plugin_main(PLUGIN_DATA *data)
{
	time_t t;

	services = data->services;
	smurf_functions = data->smurf_functions;
	redraw_window = services->redraw_window;
	
	switch (data->message)
	{
	case MSTART:
		my_id = data->id;
		strcpy(data->plugin_name, "Mr. Message");
		module_ptr = data->module_object;
		memcpy(&module, module_ptr, sizeof(*module_ptr));
		init_rsh();
		init_windstruct();
		data->wind_struct = &win;
		/* install plugin in plugin menu */
		data->event_par[0] = 0;
		data->message = MENU_ENTRY;
		break;
	
	case PLGSELECTED:
		data->message = M_WAITING;
		if (services->f_module_window(&win) < 0)
			data->message = M_EXIT;
		smurf_vars = data->smurf_vars;
		memcpy(&module, module_ptr, sizeof(*module_ptr));
		module_ptr->comm.start_imp_module = start_imp_module;
		module_ptr->comm.start_edit_module = start_edit_module;
		strcpy(logfile_name, smurf_vars->Sys_info->standard_path);
		strcat(logfile_name, "\\modcomm.log");
		break;
	
	case MWINDCLOSED:
		win.whandlem = -1;
		data->message = M_WAITING;
		memcpy(module_ptr, &module, sizeof(*module_ptr));
		if (logfile != NULL)
		{
			fclose(logfile);
			logfile = NULL;
		}
		break;
	
	case MTERM:
		data->message = M_TERMINATED;
		memcpy(module_ptr, &module, sizeof(*module_ptr));
		if (logfile != NULL)
		{
			fclose(logfile);
			logfile = NULL;
		}
		break;
	
	case MBEVT:
		if (data->event_par[0] == COMM_LOGFILE)
		{
			if (maintree[COMM_LOGFILE].ob_state & OS_SELECTED)
			{
				logfile = fopen(logfile_name, "w");
				if (logfile != NULL)
				{
					fprintf(logfile, "\n\nLogfile opened: ");
					t = time(NULL);
					fprintf(logfile, "%s", ctime(&t));
					fprintf(logfile, "   Module                          ID     Message\n");
					fprintf(logfile, "--------------------------------------------------\n");
					fflush(logfile);
				}
			} else
			{
				if (logfile != NULL)
				{
					fprintf(logfile, "\nLogfile closed: ");
					t = time(NULL);
					fprintf(logfile, "%s\n", ctime(&t));
					fclose(logfile);
					logfile = NULL;
				}
			}
		}
		break;
	
	default:
		data->message = M_WAITING;
		break;
	}
}
