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
#include "log.h"

#include "country.h"

#undef COUNTRY
#define COUNTRY 1

#if COUNTRY==1
#include "de/mrm.h"
#define AL_LOAD_MODULE  "Error loading module!"
#define AL_LOAD_MODULES "Error loading modules."
#define AL_ADJUST_TPA   "Error adjusting module TPA!"
#define AL_START_MODULE "Error starting module!"
#define AL_MODULE_ID    "Error: invalid module ID!"
#elif COUNTRY==0
#include "en/mrm.h"
#elif COUNTRY==2
#include "en/mrm.h"		/* missing french resource */
#else
#error "unsupported language"
#endif


short start_imp_module(char *modpath, SMURF_PIC *imp_pic)
{
	char *textseg_begin;
	char *dummy = NULL;
	long mod_magic;
	short (*module_main)(GARGAMEL *smurf_struct);
	short module_return;
	short back;
	long ProcLen;
	long temp;
	long lback;
	char alstring[80];
	BASPAG *mod_basepage;
	MOD_INFO *module_info;
	GARGAMEL sm_struct;

	/* Modul als Overlay laden und Basepage ermitteln */
	temp = Pexec(3, modpath, "", NULL);
	if (temp < 0)
	{
		strcpy(alstring, AL_LOAD_MODULE);
		strcat(alstring, strrchr(modpath, '\\') + 1);
		services->f_alert(alstring, NULL, NULL, NULL, 1);
		module_return = M_STARTERR;
	} else
	{
		mod_basepage = (BASPAG *) temp;

		/* Laenge des gesamten Tochterprozesses ermitteln */
		ProcLen = get_proclen(mod_basepage);
		back = _Mshrink(mod_basepage, ProcLen);	/* Speicherblock verkuerzen */

		mod_basepage->p_hitpa = (void *) (mod_basepage + ProcLen); /* BUG: missing cast to char * */

		if (back != 0)
			services->f_alert(AL_ADJUST_TPA, NULL, NULL, NULL, 1);

#if 0
		mod_magic = get_modmagic(mod_basepage);	/* Zeiger auf Magic (muss MOD_MAGIC_IMPORT sein!) */
#else
		textseg_begin = mod_basepage->p_tbase;
		mod_magic = *((long *) (textseg_begin + MAGIC_OFFSET));
#endif
		if (mod_magic != MOD_MAGIC_IMPORT)
			return M_MODERR;

		module_info = *((MOD_INFO **) (textseg_begin + MOD_INFO_OFFSET));	/* Zeiger auf Modulinfostruktur */
		sm_struct.smurf_pic = imp_pic;
		sm_struct.services = services;
		module_info = *((MOD_INFO **) (textseg_begin + MOD_INFO_OFFSET));	/* Zeiger auf Modulinfostruktur */
		/* BUG: module_number & module_mode not initialized */
		log_line(sm_struct.module_mode, sm_struct.module_number, module_info->mod_name);
		
		lback = Pexec(4, NULL, (char *) mod_basepage, NULL);
		if (lback != 0)
			services->f_alert(AL_START_MODULE, NULL, NULL, NULL, 1);

		module_main = (short (*)(GARGAMEL *)) (textseg_begin + MAIN_FUNCTION_OFFSET);
		module_return = module_main(&sm_struct);
		log_line(sm_struct.module_mode, -1, module_info->mod_name);

#if 1 /* BUG: only valid for MagiC */
		Pexec(102, dummy, mod_basepage, NULL);	/* Modul systemkonform toeten */
#endif
		Mfree(mod_basepage->p_env);
		Mfree(mod_basepage);			/* Modul-Basepage freigeben */
	}

	return module_return;
}


BASPAG *start_edit_module(char *modpath, BASPAG *edit_basepage, short mode, short mod_id, GARGAMEL *sm_struct)
{
	void (*module_main)(GARGAMEL *smurf_struct);
	char *textseg_begin;
	short back;
	long ProcLen;
	long temp;
	long lback;
	long mod_magic;
	MOD_ABILITY *mod_abs;

	if (mod_id < 0 || mod_id >= MAX_MODS - 1)
		services->f_alert(AL_MODULE_ID, NULL, NULL, NULL, 1);

	/*
	 * Modul als Overlay laden und Basepage ermitteln
	 */
	if (edit_basepage == NULL)			/* Modul wurde noch nicht gestartet! */
	{
		temp = Pexec(3, modpath, "\0", ""); /* BUG: empty env */
		if (temp < 0)
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

			edit_basepage->p_hitpa = (void *) (edit_basepage + ProcLen); /* BUG: missing cast to char * */
			if (edit_basepage == NULL) /* BUG: too late */
				return edit_basepage;
#if 0
			mod_magic = get_modmagic(edit_basepage);	/* Zeiger auf Magic (muss MOD_MAGIC_EDIT sein!) */
#else
			textseg_begin = edit_basepage->p_tbase;
			mod_magic = *((long *) (textseg_begin + MAGIC_OFFSET));
#endif
			if (mod_magic != MOD_MAGIC_EDIT)
			{
				services->f_alert(AL_LOAD_MODULE, NULL, NULL, NULL, 1);
				return NULL;
			}

			lback = Pexec(4, NULL, (char *) edit_basepage, NULL);
			if (lback < 0)
				services->f_alert(AL_START_MODULE, NULL, NULL, NULL, 1);
		}
	}

	if (edit_basepage > 0)
	{
		textseg_begin = (char *) (edit_basepage->p_tbase);

		/* Modulkennung (als wievieltes Modul gestartet?) */
		if (mode == MSTART)
			sm_struct->module_number = mod_id;

		/* Message von Smurf */
		sm_struct->module_mode = mode;	/* 0=laden und Starten, 1=Redraw ausfuehren , -1 = Beenden, 2=aktivieren */

		/* Funktionen einhaengen */
		sm_struct->services = &global_services;

		/* EVENT im Modulfenster ! */
		if (mode == MBEVT || mode == MKEVT || mode == SMURF_AES_MESSAGE)
		{
			sm_struct->mousex = mouse_xpos;
			sm_struct->mousey = mouse_ypos;
			sm_struct->klicks = klicks;

			if (mode != SMURF_AES_MESSAGE)
				sm_struct->event_par[0] = obj;

			if (mode == MKEVT)
			{
				sm_struct->event_par[1] = key_scancode;
				sm_struct->event_par[2] = key_ascii;
				sm_struct->event_par[3] = key_at_event;
			}
		}

		if (mode != MQUERY)
		{
			if (mode == MEXEC)
				graf_mouse(BUSYBEE, dummy_ptr);

			module_main = (void (*)(GARGAMEL *smurf_struct)) (textseg_begin + MAIN_FUNCTION_OFFSET);
			module_main(sm_struct);

			graf_mouse(ARROW, dummy_ptr);
		}

		if (mode == MQUERY)
		{
			module.bp[mod_id & 0xFF] = edit_basepage;
			mod_abs = *((MOD_ABILITY **) (textseg_begin + MOD_ABS_OFFSET));	/* Module Abilities */
			return ((BASPAG *) mod_abs);
		}
	}

	return edit_basepage;
}


/* get_proclen
   -------------------------------------------------
   Ermittelt die Gesamtlaenge des Prozesses mit der Basepage baspag.
   ----------------------------------------------------------------
 */
long get_proclen(BASPAG *baspag)
{
	long TextLen;
	long BSSLen;
	long DataLen;
	long ProcLen;

	TextLen = baspag->p_tlen;
	BSSLen = baspag->p_blen;
	DataLen = baspag->p_dlen;
	/* BASEPAGE + Textsegment + Datensegment + BSS + Stack */
	ProcLen = sizeof(*baspag) + TextLen + DataLen + BSSLen + 1001L; /* WTF? why 1001? */

	return ProcLen;
}
