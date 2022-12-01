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
 * The Initial Developer of the Original Code is
 * Bjoern Spruck
 *
 * This code is a module for the programm SMURF developed by
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *         
 * Contributor(s):
 *         
 *
 * ***** END LICENSE BLOCK *****
 */

/*
		Smurf Ausbeulen
*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Beule"
#define TEXT2 "Interpolieren"
#define TEXT3 "Rand schwarz"
#define TEXT4 "H”he:"
#define TEXT5 "Radius:"
#define TEXT6 "ausbeulen..."
#elif COUNTRY==0
#define TEXT1 "Beule"
#define TEXT2 "Interpolieren"
#define TEXT3 "Rand schwarz"
#define TEXT4 "Height:"
#define TEXT5 "Radius:"
#define TEXT6 "ausbeulen..."
#elif COUNTRY==2
#define TEXT1 "Beule"
#define TEXT2 "Interpolieren"
#define TEXT3 "Rand schwarz"
#define TEXT4 "Height:"
#define TEXT5 "Radius:"
#define TEXT6 "ausbeulen..."
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "import.h"
#include "smurfine.h"
#include "beule.h"

/* Infostruktur fr Hauptmodul */

MOD_INFO module_info = { TEXT1,
	0x0080, "Bj”rn Spruck",
	{ "", "", "", "", "", "", "", "", "", "" },
	/* Objekttitel */
	"", "", "", "",
	TEXT2, TEXT3, "", "",
	TEXT4, TEXT5, "", "",

	/* Min/Max Slider */
	0, 128, 0, 128, 0, 128, 0, 128,
	/* Min/Max Editfelder */
	-100, 100, 10, 10000, 0, 10, 0, 10,
	/* Slider-Defaultwerte */
	0, 0, 0, 0,
	0, 0, 0, 0,
	15, 150, 0, 0,
	/* Anzahl Bilder */
	1,
	/* Bildbeschreibung */
	NULL, NULL, NULL, NULL, NULL, NULL
};

MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	0
};

SERVICE_FUNCTIONS *services;

#define BANG_INTER static short bang_inter
#define BANG static short bang
#define BUSYCALL 7

#include "bangcalc.c"

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			Beule (by Sage) 						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short my_id;
	static short mx, my;

	services = smurf_struct->services;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		my_id = smurf_struct->module_number;
		smurf_struct->services->f_module_prefs(&module_info, my_id);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MCROSSHAIR:
		smurf_struct->event_par[0] = 1;
		smurf_struct->event_par[1] = 0;
		smurf_struct->module_mode = M_CROSSHAIR;
		break;

	case MCH_DEFCOO:					/* Smurf fragt: und die Default-Koordinaten? */
		mx = (smurf_struct->smurf_pic->pic_width) >> 1;
		my = (smurf_struct->smurf_pic->pic_height) >> 1;
		smurf_struct->event_par[0] = mx;
		smurf_struct->event_par[1] = my;
		smurf_struct->module_mode = M_CHDEFCOO;
		break;

	case MCH_COORDS:					/* Smurf sagt: hier sind die eingestellten Fadenkreuzkoordinaten */
		mx = smurf_struct->event_par[0];
		my = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		{
			short hoehe, radius, rands;
			short back;
			
			hoehe = (short) smurf_struct->edit1;
			radius = (short) smurf_struct->edit2;
			rands = smurf_struct->check2;
			smurf_struct->services->reset_busybox(0, TEXT6);
			if (smurf_struct->services->CPU_type & FPU)
			{
				if (smurf_struct->check1)
				{						/* Interpolieren */
					back = bang_inter_FPU(smurf_struct->smurf_pic, mx, my, hoehe, radius, rands);
				} else
				{						/* nicht Interpolieren */
					back = bang_FPU(smurf_struct->smurf_pic, mx, my, hoehe, radius, rands);
				}
			} else
			{
				if (smurf_struct->check1)
				{						/* Interpolieren */
					back = bang_inter(smurf_struct->smurf_pic, mx, my, hoehe, radius, rands);
				} else
				{						/* nicht Interpolieren */
					back = bang(smurf_struct->smurf_pic, mx, my, hoehe, radius, rands);
				}
			}
			if (back == 0)
			{
				smurf_struct->module_mode = M_MEMORY;
			} else
			{
				smurf_struct->module_mode = M_PICDONE;
			}
		}
		break;

	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
