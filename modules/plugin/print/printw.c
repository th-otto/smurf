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
#undef ALERT_STRINGS					/* conflicts with smurf.h */
#include "bindings.h"
#include "wdialog.h"
#include "gdos.h"

#include "country.h"

#if COUNTRY==1
#include "de/printw.rsh"
#define TEXT1 "Drucken..."
#elif COUNTRY==0
#include "en/printw.rsh"
#define TEXT1 "Print..."
#elif COUNTRY==2
#include "en/printw.rsh"		/* missing french resource */
#define TEXT1 "Print..."
#else
#error "Keine Sprache!"
#endif

static char const name_string[] = "GDOS Print-Plugin";

PLUGIN_INFO plugin_info = {
	name_string,
	0x0101,								/* Plugin-Version 1.01 */
	SMURF_VERSION,						/* f�r Smurf-Version 1.06 */
	0									/* und nicht resident. */
};

static short my_id;
static SMURF_PIC *pic_active;

SERVICE_FUNCTIONS *services;
SMURF_PIC **smurf_picture;
short *active_pic;
OBJECT *alerts;
SMURF_VARIABLES *smurf_vars;
PLUGIN_FUNCTIONS *smurf_functions;

static WORD pdlg_handle;
static PRN_DIALOG *prn_dialog;
static PRN_SETTINGS *prn_settings;


/* init_rsh -----------------------------------------------------
    Initialisiert die Resource 
    -----------------------------------------------------------*/
static void init_rsh(void)
{
	WORD t;

	/* Resource Umbauen */
	for (t = 0; t < NUM_OBS; t++)
		rsrc_obfix(rs_object, t);

	alerts = rs_trindex[ALERT_STRINGS];	/* Resourcebaum holen */
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
	}
}


static BOOLEAN handle_print_dialog(EVNT *events)
{
	WORD button;

	if (prn_dialog)
	{
		if (pdlg_evnt(prn_dialog, prn_settings, events, &button) == 0)	/* Dialog schlie�en? */
		{
			close_print_dialog(FALSE);		/* Dialog schlie�en */

			if (button == PDLG_OK)		/* "Drucken" angew�hlt? */
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


void plugin_main(PLUGIN_DATA *data)
{
	char *picname;
	WORD out1, out2, out3, out4;
	BOOLEAN closed = FALSE;
	static short notify_num = 0;
	EVNT events;

	/* Die Strukturen mit den Variablen und Funktionen holen */
	services = data->services;
	smurf_functions = data->smurf_functions;
	smurf_vars = data->smurf_vars;

	switch (data->message)
	{
		/* 
		 * Startup des Plugins: Men�eintrag anfordern 
		 */
	case MSTART:
		my_id = data->id;
		strcpy(data->plugin_name, TEXT1);
		init_rsh();
		data->event_par[0] = FILE_PRINT;
		data->message = MENU_ENTRY;		/* Men�eintrag anfordern */
		break;

	case PLG_STARTUP:
		data->message = M_WAITING;
		break;

		/*
		 * Plugin wurde aus dem Men� heraus aufgerufen 
		 */
	case PLGSELECTED:
		if (appl_find("?AGI") < 0 || appl_getinfo(7, &out1, &out2, &out3, &out4) != 1 || (out1 & 0x10) == 0)
		{
/*                              services->f_alert("Ben�tigtes Programm WDIALOG ist nicht installiert.", NULL, NULL, NULL, 1); */
			services->f_alert(alerts[NO_WDIALOG].TextCast, NULL, NULL, NULL, 1);
			data->message = M_EXIT;
		} else if (*(smurf_vars->picthere) == 0)
		{
/*                              services->f_alert("Kein Bild geladen!", NULL, NULL, NULL, 1); */
			services->f_alert(alerts[NO_PIC].TextCast, NULL, NULL, NULL, 1);
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
			 * Druckdialog kreiern, initialisieren und �ffnen 
			 */
			if ((prn_dialog = pdlg_create(PDLG_3D)) != NULL)
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
		}
		break;

	case DONE:
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
		} else
		{
			data->message = M_WAITING;
		}
		break;

	case MBEVT:
		events.mwhich = MU_BUTTON;
		events.mx = data->mousex;
		events.my = data->mousey;
		events.mclicks = data->klicks;
		events.mbutton = data->event_par[1];
		closed = handle_print_dialog(&events);
		data->message = M_WAITING;
		break;

	case MKEVT:
		events.mwhich = MU_KEYBD;
		events.kstate = data->event_par[3];
		events.key = data->event_par[1];
		closed = handle_print_dialog(&events);
		data->message = M_WAITING;
		break;

	case SMURF_AES_MESSAGE:
		events.mwhich = MU_MESAG;
		memcpy(events.msg, data->event_par, 16);
		closed = handle_print_dialog(&events);
		data->message = M_WAITING;
		break;

	case MPIC_UPDATE:
		pic_active = smurf_picture[data->event_par[0]];
		if ((picname = strchr(pic_active->filename, '\\')) != NULL && *(picname + 1) != '\0')
			picname++;
		else
			picname = pic_active->filename;
		pdlg_update(prn_dialog, picname);
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
