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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../import.h"
#include "../../src/smurfine.h"


/* info structure for main module */
MOD_INFO module_info = {
	"QFax",
	0x0010,
	"Christian Eyrich",
	/* Extensionen */
	{ "F01", "F02", "F03", "F04", "F05", "F06", "F07", "F08", "F09", "F10" },

	/* Slider */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* Checkboxen */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* Editfelder */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",

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
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};

char x137f0[2] = "";

short imp_module_main(GARGAMEL *smurf_struct)
{
	return M_PICDONE;					/* das wars. */
}
