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

/*  ------------- Farbverlauf-Modul ----------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../import.h"
#include "../../../src/smurfine.h"
#include "country.h"

#if COUNTRY == 1
#define TEXT1 "Farbverlauf"
#elif COUNTRY == 0
#define TEXT1 "Color gradient"
#else
#define TEXT1 "Color gradient"
#endif


#define TIMER 0

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	TEXT1,
	0x0100,
	"Olaf Piesche",
	/* Extensionen */
	{ "", "", "", "", "", "", "", "", "", "" },
	/* Slider */
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	/* Checkboxen */
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	/* Editfelder */
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",

	/* Minima + Maxima */
	/* Slider */
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	/* Edits */
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	/* Defaults */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	/* how many pics? */
	1,
	/* description for pictures */
	"", "", "", "", "", ""
};

/*--------------------- Was kann ich ? ----------------------*/
MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,    /* Farbtiefen */
	/* Grafikmodi: */
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	0 /* Extra-Flag */ 
};

short x137f4 = 2;
short x137f6 = 0;
short x137f8 = 90;
short x137fa = 5;

void edit_module_main(GARGAMEL *smurf_struct)
{
}
