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

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "fs.h"

/* Set 16 Pixel (Standard Format) Assembler-Rout */
short (*set_16_pixels)(uint8_t *source, uint8_t *dest, short depth, long planelen, short howmany);
SERVICE_FUNCTIONS *service;
short (*seek_nearest_col)(long *par, short maxcol);
short not_in_nct;




DITHER_MOD_INFO dith_module_info = {
	"Floyd-Steinberg",
	"Olaf Piesche",
	0x0100,								/* Schlumpfine-Version */
	0,									/* Konfigurierbar? */
	COMPILER_ID,
	ALLPAL								/* Palettenmodi */
};


/*  --------------------------------------------------------------  */
/*  --------------------- 8-24-Bit Bild -------------------------   */
/*  --------------------------------------------------------------  */
short dither_module_main(DITHER_DATA * dither)
{
	short back;

	service = dither->services;

	set_16_pixels = dither->set_16pixels;
	not_in_nct = dither->not_in_nct;

	seek_nearest_col = service->seek_nearest_col;

	back = floyd_steinberg68000(dither->picture, dither, dither->dest_mem);

	return back;
}
