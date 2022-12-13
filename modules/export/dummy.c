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

/* =========================================================*/
/*                      Dummy-Exporter                      */
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

char comp1[] = "Dies ist ein";
char comp2[] = "Dummy-Export";
char comp3[] = " Zum Testen ";
char comp4[] = "des Export- ";
char comp5[] = " Parts von  ";
char comp6[] = "SCHLUMPFINE ";


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Dummy-Exporter",
	0x0010,
	"Olaf",
	{ "XXX", "YYY", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	0
};


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*              8 Bit, unkomprimiert                */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic = NULL;

	(void)smurf_struct;
	return exp_pic;					/* Hier macht Smurf nix. */
}
