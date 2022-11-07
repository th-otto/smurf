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

/* IS_MENU_KEY - eine Funktion um den gedr�ckten Shortcut festzustellen */

#include <string.h>
#include <stdio.h>
#include "../modules/import.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "debug.h"

#ifndef __PUREC__
#define KEYTAB _KEYTAB
#endif


#define CTRL_CHAR	'^'					/* Men�-Control-Buchstabe */
#define ALT_CHAR	0x07				/* Men�-Alternate-Buchstabe */
#define SHIFT_CHAR	0x01				/* Men�-Shifttaste */

#define THEBAR		1					/* Objekt-Nummer der Men�zeile */
#define THEACTIVE	2					/* Objekt-Nummer der aktiven Men�s */
#define THEFIRST	3					/* Objekt-Nummer des ersten Men�s */


/* Fehlt noch die Dimensionierung */
static CUTTAB cuttab[128];


/* SET_MENU_KEY */
/* menu: Zeiger auf den Men�baum */

char set_menu_key(OBJECT *menu)
{
	char *s, j=0, k;
	int i, ltitle, menu_start;
	int invisible_box, t, cutpos=0;

	DEBUG_MSG (( "Set Menu Key...\n" ));

	for(k=0; k<128; k++) strncpy(cuttab[k].cut, "   ", 3);


	if(menu != 0L)
	{

		for(t=3; ; t++)	if(menu[t].ob_type==G_IBOX) break;

		invisible_box=t;
		

		menu_start=invisible_box+1;
		
		j=menu_start+1;

		ltitle=3;
		do
		{
			do
			{
				if((menu[j].ob_type & 0xff) == G_STRING)
				{
					s = menu[j].ob_spec.free_string;			/* Men� */
					i = (int)strlen(s);
					if (i > 0 && s[i - 1] == ' ')
						i--;
					if (i > 0 && s[i - 1] == ' ')
						i--;
					for(; i >= 0 && s[i] != ' '; i--)	/* von hinten her nach dem letzten Leerzeichen suchen */
						;

					if(i >= 2 && (s[i + 1] == SHIFT_CHAR || s[i + 1] == CTRL_CHAR || s[i + 1] == ALT_CHAR))
					{
						k = 0;
						if(s[i + 1] != SHIFT_CHAR)
							k++;
						while(s[++i] != ' ')
							cuttab[cutpos].cut[k++] = s[i];
						cuttab[cutpos].ltitle = ltitle;
						cuttab[cutpos].litem = j;
						cutpos++;
					} /* if */
				} /* if */

				j++;					/* N�chster Eintrag */
			} while(menu[j].ob_type==G_STRING);

			if(menu[j+1].ob_type==G_STRING) j++;
			ltitle++;
			
		} while(menu[j].ob_type == G_STRING);

	} /* if */

	return(0);
}


static char is_state(OBJECT *tree, int obj, unsigned int state)
{
	return((tree[obj].ob_state & state) != 0);
}


/* GET_MENU_KEY */
/* is_menu_key: TRUE wenn die Taste auf ein Men� zutrifft, sonst FALSE */
/* ki: Zeiger auf eine Struktur die die aktuell bet�tigte Taste beschreibt */
/* title: Zeiger auf den Index des Titels, falls "is_menu_key" TRUE ist */
/* item: Zeiger auf den Index des Men�eintrages, falls "is_menu_key" TRUE ist */

char get_menu_key(OBJECT *menu, KINFO *ki, int *title, int *item)
{
	char j = 0,	search[3] = "   ";
	int sc_found;

	if(ki->shift)
		search[0] = SHIFT_CHAR;
	if(ki->ctrl)
		search[1] = CTRL_CHAR;
	else
		if(ki->alt)
			search[1] = ALT_CHAR;
	search[2] = ki->ascii_code;

	j = 0;
	sc_found=0;
	while(strnicmp(cuttab[j].cut, "   ", 3) != 0)
	{
		if(strnicmp(cuttab[j].cut, search, 3) == 0)
		{
			*title = cuttab[j].ltitle;
			*item = cuttab[j].litem;
			sc_found=1;
			break;
		}
		else
			j++;
	}

	if(!is_state(menu, *item, OS_DISABLED) && sc_found)
	/* R�ckgabe der ausgew�hlten Werte */
		return(1);
	else
		return(0);
}


int scan_2_ascii(int scan,int state)
{
	KEYTAB *keytab = Keytbl((void *) -1l,(void *) -1l,(void *) -1l);

	if (state)
	{
		scan >>=8;
		if ((scan>=120) && (scan<=131))
			scan -= 118;
			
		if (state & 3)
			scan = (int) *(keytab->shift+scan);
		else
			scan = (int) *(keytab->unshift+scan);
	}

	scan &= 0xff;
	if (scan>='a')
	{
		if (scan<='z')
			scan -= 32;
		else if (scan=='�')
			scan='�';
		else if (scan=='�')
			scan='�';
		else if (scan=='�')
			scan='�';
	}
	return(scan);
}
