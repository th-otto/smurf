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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vaproto.h"
#include "import.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "popdefin.h"
#include "plugin.h"

#include "xrsrc.h"
#include "globdefs.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "bindings.h"
#ifdef __PUREC__
#include "slb.h"
#else
#include <mint/slb.h>
#endif

#if !defined(__PUREC__) && !defined(__MSHORT__)
/* must pass structure by value, because otherwise stack layout is wrong */
struct bgh_load_args {
	const char *name;
	short Mode;
};
struct bgh_gethelp_args {
	ULONG BGH_Handle;
	short section;
	short Gruppe;
	short Index;
};

typedef struct
{
	short Version;
	char *Info;
	unsigned long cdecl (*Load) (struct bgh_load_args args);
	void cdecl (*Free) (ULONG BGH_Handle);
	char *cdecl (*GetHelpString) (struct bgh_gethelp_args args);
} BGH_Cookie;

#else

typedef struct
{
	short Version;
	char *Info;
	unsigned long cdecl (*Load) (const char *Name, short Mode);
	void cdecl (*Free) (ULONG BGH_Handle);
	char *cdecl (*GetHelpString) (ULONG BGH_Handle, short Section, short Guppe, short Index);
} BGH_Cookie;

#endif

#define BGH_DIAL  1
#define BGH_ALERT 2
#define BGH_USER  3

#define BUBBLEGEM_SHOW	0xBABB
#define BUBBLEGEM_ACK	0xBABC

static char BGHI_Exist;					/* BGHI_Cokkie vorhanden = TRUE         */

static ULONG Help;						/* Zeiger auf die Bubble-Help-Texte     */
static BGH_Cookie *BGHI_Cookie;
static char BGHI_Slb;					/* BGHI als shared library = TRUE       */
static SLB_EXEC slbexec;
static SLB_HANDLE slb;
/* Dialognummer in der RSC fuer BGH */
static WORD const dialog_nums[] = {
	-1,
	-1,
	DISPLAY_OPTIONS, /* WIND_DOPT */
	NEWPIC_FORM,     /* WIND_PIC */
	IMAGE_INFO,      /* WIND_PICINFO */
	SMURF_OPTIONS,   /* WIND_OPTIONS */
	WINDOW_ALERT,    /* WIND_ALERT */
	MOD_FORM,        /* WIND_MODFORM */
	BUSY_WINDOW,     /* WIND_BUSY */
	MODULES,         /* WIND_MODULES */
	SMURF_INFO,      /* WIND_INFO */
	NEWPIC_FORM,     /* WIND_NEWPIC */
	BLOCK_TYPE,      /* WIND_BTYPEIN */
	EXPORT_MODS,     /* WIND_EXPORT */
	PIC_MANAGER,     /* WIND_PICMAN */
	EXPORT_FORM,     /* FORM_EXPORT */
	TRANSFORM_PIC,   /* WIND_TRANSFORM */
	-1,              /* WIND_PICORDER */
	BLOCK_CONF       /* WIND_BLOCKMODE */
};


void bubble_init(void)
{
	long err;

	BGHI_Exist = TRUE;
	BGHI_Slb = FALSE;
	if (!get_cookie(0x42474849L, (unsigned long *)&BGHI_Cookie))	/* 'BGHI' Cookie suchen */
	{
		err = Slbopen("BGH.SLB", NULL, /*0x0101L */ 2L, &slb, &slbexec);
		if (err == -64L)				/* Versionsnummer zu klein ?                */
		{
#if 0
			ShowArrow();
			Note(ALERT_NORM,1,BUBBLE_HELP);
#endif
		}
		if (err >= 0)					/* šber shared library versuchen        */
		{
			BGHI_Cookie = (BGH_Cookie *) (*slbexec) (slb, 0L, 0);
			if (BGHI_Cookie >= 0)
				BGHI_Slb = TRUE;
			else
				BGHI_Exist = FALSE;
		} else
			BGHI_Exist = FALSE;
	}
}


void bubble_exit(void)
{
	if (BGHI_Slb)
		Slbclose(slb);
}


void bubble_gem(WORD windownum, WORD xpos, WORD ypos, BOOLEAN modulemode)
{
	WORD klickobj;
	char helppath[SM_PATH_MAX];
	char helpname[64];
	char *HelpString;

#if 0
	char *helpstring;
#endif
	short Section;
	short TreeId;
	char num_byte[5];
	WORD bubble_id;
	short general_info = 0;
	WINDOW *window = 0;

	char mod_name_string[30];
	char *cmp_string;
	char *textseg_begin;
	MOD_INFO *mod_info;
	short mod_index;
	WORD t;
	long mod_magic;

	if (BGHI_Exist == FALSE)
		return;

	Section = 0;
	TreeId = 0;
	klickobj = 0;
	obj = 0;							/* globale Objektvariable l”schen */

	/* 
	 * Fensterstruktur ermitteln
	 */
	if (modulemode == 0)
	{
		if (windownum > 0)
			window = &wind_s[windownum];
		else if (windownum < 0)
			window = &picture_windows[-windownum];
	} else
	{
		window = Window.myModuleWindow(windownum);
	}

	if (window == NULL || window->shaded)
		return;
	klickobj = objc_find(window->resource_form, 0, MAX_DEPTH, xpos, ypos);
	if (klickobj < 0)
		return;

	if (windownum == WIND_MODULES && !(Dialog.emodList.tree[INFO_MODULE].ob_state & OS_SELECTED))
		if (klickobj >= MODULE1 && klickobj <= MODULE9)
			general_info = 1;

	if (windownum == WIND_MODFORM)
		modulemode = 1;

	/*
	 * allgemeine Editmodulinfo?
	 */
	if (general_info)
	{
		mod_index = klickobj - MODULE1 + Dialog.emodList.modList.scroll_offset;
		strcpy(helpname, strrchr(edit_modules[mod_index], '\\'));
		cmp_string = strrchr(helpname, '.');
		strcpy(cmp_string, ".hlp");
		Section = BGH_DIAL;
		TreeId = 0;
		klickobj = -1;
	} else
	{
		/*
		 * fest definierte Dialognummern
		 */
		if (!modulemode && windownum != WIND_MODFORM)
		{
			if (windownum < 0)			/* Bildfenster */
			{
				Section = BGH_USER;
				TreeId = BILDFENSTER;
			} else if (windownum == WIND_MODULES)
			{
				if (Dialog.emodList.tree[INFO_MODULE].ob_state & OS_SELECTED)	/* Editmodul-Info */
				{
					Section = BGH_USER;
					TreeId = EDITMODULE_INFO;
				} else
				{
					Section = BGH_DIAL;
					TreeId = MODULES;
				}
			} else if (windownum == WIND_EXPORT)	/* Exportmodul-Info */
			{
				if (Dialog.expmodList.tree[EXMOD_INFO].ob_state & OS_SELECTED)
				{
					Section = BGH_USER;
					TreeId = EXPORTMODUL_INFO;
				} else
				{
					Section = BGH_DIAL;
					TreeId = EXPORT_MODS;
				}
			} else
			{
				Section = BGH_DIAL;
				if (window->wnum < 0 ||
					window->wnum >= (int)(sizeof(dialog_nums) / sizeof(dialog_nums[0])) ||
					(TreeId = dialog_nums[window->wnum]) < 0)
					return;
			}

			strcpy(helpname, "\\smurf.bgh");
		} else
		{
			/*
			 * Modulfenster
			 */
			char *modpath;

			mod_index = window->module & 0xff;
			strcpy(helpname, "\\");

			if (window->module & 0x200)
				mod_magic = get_modmagic(plugin_bp[mod_index]);
			else
				mod_magic = get_modmagic(module.bp[mod_index]);

			if (mod_magic == MOD_MAGIC_EDIT || mod_magic == MOD_MAGIC_EXPORT)
			{
				textseg_begin = module.bp[mod_index]->p_tbase;
				mod_info = *((MOD_INFO **) (textseg_begin + MOD_INFO_OFFSET));

				/*---- Modulname kopieren und mit Leerzeichen auffllen */
				memset(mod_name_string, ' ', 30);
				strncpy(mod_name_string, mod_info->mod_name, 28);
				if (strlen(mod_name_string) < 28)
					strncat(mod_name_string, "                           ", 28 - strlen(mod_name_string));

				for (t = 0; t < 100; t++)
				{
					cmp_string = Dialog.emodList.modNames[t];
					if (strcmp(cmp_string, mod_name_string) == 0)
						break;
				}

				if (t >= 99)
					return;
				modpath = edit_modules[t];
			} else if (mod_magic == MOD_MAGIC_PLUGIN)
			{
				textseg_begin = plugin_bp[mod_index]->p_tbase;

				modpath = plugin_paths[mod_index];
			} else
			{
				return;
			}

			/*---- Modulpfad kopieren, Name isolieren und .hlp dranh„ngen. */
			strcat(helpname, strrchr(modpath, '\\') + 1);
			cmp_string = strrchr(helpname, '.');
			strcpy(cmp_string, ".hlp");
			Section = BGH_DIAL;
			TreeId = 0;
		}
	}

	strcpy(helppath, Sys_info.standard_path);
	strcat(helppath, "\\bubble");
	strcat(helppath, helpname);
	memset(num_byte, 0, 5);
/* printf ("BGH: %s, %i, %i, %i\r\n", helppath, Section, TreeId, klickobj ); */

#if 0
	/*-------- Block wenn m”glich als global anfordern? */
	if (Ssystem(FEATURES, 0L, 0L) != EINVFN || (Sys_info.OS & MINT) || (Sys_info.OS & MATSCHIG))
		helpstring = (char *) Mxalloc(SM_PATH_MAX + 1, 0x20);
	else
		helpstring = (char *) SMalloc(SM_PATH_MAX + 1);
#endif

#if !defined(__PUREC__) && !defined(__MSHORT__)
	{
		struct bgh_load_args args;
		args.name = helppath;
		args.Mode = 0;
		Help = BGHI_Cookie->Load(args);
	}
#else
	Help = BGHI_Cookie->Load(helppath, 0);
#endif
	if (!Help)
		return;
#if !defined(__PUREC__) && !defined(__MSHORT__)
	{
		struct bgh_gethelp_args args;
		args.BGH_Handle = Help;
		args.section = Section;
		args.Gruppe = TreeId;
		args.Index = klickobj;
		HelpString = BGHI_Cookie->GetHelpString(args);
	}
#else
	HelpString = BGHI_Cookie->GetHelpString(Help, Section, TreeId, klickobj);
#endif
	BGHI_Cookie->Free(Help);

#if 0
	{
		short found = 0;
		short end = 0;
		char *end_of_string;
		char *search_pos;
		char *help_file;

		help_file = fload(helppath, 0);
		if (help_file == NULL)
			return;

		search_pos = help_file;

		if (general_info)
		{
			search_pos = strstr(help_file, "#GINF");
			if (search_pos == NULL)
			{
				SMfree(help_file);
				SMfree(helpstring);
				return;
			}

			search_pos += 5 + 2;		/* #GINF und 1 CRLF weiter */
			end_of_string = strchr(search_pos, '@');
			memset(helpstring, 0, SM_PATH_MAX);
			strncpy(helpstring, search_pos, end_of_string - search_pos);
		} else
		{
			/*------ "Directory-Eintrag" suchen */
			if (!modulemode)
			{
				do
				{
					search_pos = strchr(search_pos, '#');
					memcpy(num_byte, search_pos + 1, 3);
					if (atoi(num_byte) == windownum)
						found = 1;
					else if (atoi(num_byte) == 0)
						end = 1;
					search_pos++;
				} while (!found && !end);

				if (end)
				{
					SMfree(helpstring);
					SMfree(help_file);
					return;
				}
			}

			/*------ "Subdirectory-Eintrag" suchen */
			end = found = 0;
			do
			{
				search_pos = strchr(search_pos, '-');
				if (search_pos != NULL)
				{
					memset(num_byte, 0x0, 5);
					memcpy(num_byte, search_pos + 1, 3);
					if (atoi(num_byte) == klickobj)
						found = 1;
					else if (atoi(num_byte) == -1)
						end = 1;
					else
						search_pos = strchr(search_pos, '@');

					search_pos++;
				} else
					end = 1;
			} while (!found && !end);

			if (end)
			{
				SMfree(helpstring);
				SMfree(help_file);
				return;
			}

			search_pos += 5;			/* 3 Ziffern und 1 CRLF weiter */
			end_of_string = strchr(search_pos, '@');

			memset(helpstring, 0, SM_PATH_MAX);
			strncpy(helpstring, search_pos, end_of_string - search_pos);
		}
	
		/* auf keinen Fall hier schon helpstring freigeben! */
		/* das wird nach Antwort von Bubble-GEM durch BUBBLEGEM_ACK gemacht */
		SMfree(help_file);
	}
#endif

	/*------ Message an BubbleGEM schicken -----------*/
	if ((bubble_id = appl_find("BUBBLE  ")) > 0)
		Comm.sendAESMsg(bubble_id, BUBBLEGEM_SHOW, xpos, ypos, LONG2_2INT(HelpString), -1);
}



/* call_stguide
   --------------------------------------------------
   Ruft den ST-Guide mit dem zum Fensterhandle topwin_handle passenden
   Hypertext auf der passenden Seite auf, Module und Plugins inklusive.
   -------------------------------------------------------------
*/
void call_stguide(WORD topwin_handle)
{
	WORD STG_id;
	WORD wnum;
	short t;
	short mod_index;
	WINDOW *window;
	MOD_INFO *mod_info;
	char *wtitle;
	char *modpath;
	char *textseg_begin;
	char *cmp_string;
	char hypname[128] = "*:\\smurf.hyp ";
	char mod_name_string[30];
	long mod_magic;

	/*
	 *  ST-Guide suchen
	 */
	STG_id = appl_find("ST-GUIDE");
	if (STG_id < 0)
	{
		Dialog.winAlert.openAlert("ST-Guide nicht gefunden!", NULL, NULL, NULL, 1);
		return;
	}

	/*
	 *  HELP in einem SMURF-Dialogfenster?
	 */
	wnum = Window.myWindow(topwin_handle);
	if (wnum > 0 && wnum != WIND_MODFORM)
	{
		wtitle = wind_s[wnum].wtitle;
		if (topwin_handle == wind_s[FORM_EXPORT].whandlem)
			strcat(hypname, "Exportdialog");
		else
			strcat(hypname, wtitle);
		Comm.sendAESMsg(STG_id, VA_START, LONG2_2INT(hypname), -1);
	} else if (wnum < 0)				/* Bildfenster! */
	{
		strcat(hypname, "Bildfenster");
		Comm.sendAESMsg(STG_id, VA_START, LONG2_2INT(hypname), -1);
	}

	/*
	 *  oder in einem Modulfenster?
	 */
	else if (wnum == 0)
	{
		window = Window.myModuleWindow(topwin_handle);
		if (window == NULL)
			return;

		strcpy(hypname, "*:\\");

		mod_index = (window->module) & 0xFF;

		if (window->module & 0x200)
			mod_magic = get_modmagic(plugin_bp[mod_index]);
		else
			mod_magic = get_modmagic(module.bp[mod_index]);


		if (mod_magic == MOD_MAGIC_EDIT || mod_magic == MOD_MAGIC_EXPORT)
		{
			textseg_begin = module.bp[mod_index]->p_tbase;
			mod_info = *((MOD_INFO **)(textseg_begin + MOD_INFO_OFFSET));

			/*---- Modulname kopieren und mit Leerzeichen auffllen */
			memset(mod_name_string, ' ', 30);
			strncpy(mod_name_string, mod_info->mod_name, 28);
			if (strlen(mod_name_string) < 28)
				strncat(mod_name_string, "                           ", 28 - strlen(mod_name_string));

			for (t = 0; t < 100; t++)
			{
				cmp_string = Dialog.emodList.modNames[t];
				if (strcmp(cmp_string, mod_name_string) == 0)
					break;
			}

			if (t >= 99)
				return;

			modpath = edit_modules[t];
		} else if (mod_magic == MOD_MAGIC_PLUGIN)
		{
			textseg_begin = plugin_bp[mod_index]->p_tbase;

			modpath = plugin_paths[mod_index];
		}

		/*---- Modulpfad kopieren, Name isolieren und .HYP dranh„ngen. */
		strcat(hypname, strrchr(modpath, '\\') + 1);
		cmp_string = strrchr(hypname, '.');
		*cmp_string = 0;
		strcat(hypname, ".hyp");

		Comm.sendAESMsg(STG_id, VA_START, LONG2_2INT(hypname), -1);
	}
}
