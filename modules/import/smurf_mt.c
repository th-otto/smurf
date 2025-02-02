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
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Speichertestmodul",
	0x0010,
	"Olaf Piesche",
	{ "", "", "", "", "", "", "", "", "", "" },
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

#define DEBUG           0

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*      Modul zum Testen der Speicherverwaltung     */
/*      nach PEXEC 3/4 ohne unsauberen Sprung       */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	void *buffer;
	long free_mem;

	(void) smurf_struct;
	Goto_pos(0, 0);
	free_mem = (long) Mxalloc(-1, 2);
	printf("\n-   Mod gestartet: %li Bytes.", free_mem);

	buffer = (void *)Malloc(100000l);
	free_mem = (long) Mxalloc(-1, 2);
	printf("\n-100K angefordert: %li Bytes.", free_mem);

	Mfree(buffer);
	free_mem = (long) Mxalloc(-1, 2);
	printf("\n- und freigegeben: %li Bytes.", free_mem);
	puts("\n\n Taste...");
	(void) Cnecin();

	return M_INVALID;
}
