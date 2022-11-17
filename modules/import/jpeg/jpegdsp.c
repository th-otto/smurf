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
/*			JPEG File Interchange Format Decoder			*/
/* Version 0.1  --  27.09.97								*/
/*	  8 und 24 Bit											*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../import.h"
#include "../../../src/smurfine.h"
#include "country.h"

#undef LOCAL
#undef GLOBAL
#undef INLINE
#undef EXTERN

#include "jpeglib.h"
#include <setjmp.h>
#include "jpgdh.h"

#define TIMER 0
#define START_DRIVER 1

#if COUNTRY==1
#define MSG1 "[3][Brainstorm JPEG DSP Driver|nicht gefunden. Nachstarten?|(Vorsicht Crashgefahr wenn|DSP mehrfach benutzt!)][Starten|Abbruch]"
#elif COUNTRY==0
#define MSG1 "[3][Brainstorm JPEG DSP Driver|nicht gefunden. Nachstarten?|(Vorsicht Crashgefahr wenn|DSP mehrfach benutzt!)][Starten|Abbruch]"
#elif COUNTRY==2
#define MSG1 "[3][Brainstorm JPEG DSP Driver|nicht gefunden. Nachstarten?|(Vorsicht Crashgefahr wenn|DSP mehrfach benutzt!)][Starten|Abbruch]"
#endif

extern JPGDDRV_STRUCT *jpgd_driver ASM_NAME("jpgd_driver");
extern uint16_t pixel_width ASM_NAME("pixel_width");
extern uint16_t pixel_wdwidth ASM_NAME("pixel_wdwidth");
extern uint16_t pixel_height ASM_NAME("pixel_height");
extern unsigned long image_size ASM_NAME("image_size");
JPGD_STRUCT jpgd_struct ASM_NAME("jpgd_struct");


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"JFIF Format",
	0x0130,
	"Bj\224rn Spruck",
	/* Extensionen */
	{ "JPG", "JPEG", "JFIF", "", "", "", "", "", "", "" },
	/* Slider */
	"",
	"",
	"",
	"",
	/* Checkboxen */
	"",
	"",
	"",
	"",
	/* Editfelder */
	"",
	"",
	"",
	"",

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


#if START_DRIVER
void start_driver(void) ASM_NAME("start_driver");

void start_driver(void)
{
	if (form_alert(1, MSG1) == 1)
		shel_write(1, 1, 0, "JPEGD.PRG", "");
}
#endif

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		JPEG File Interchange Format (JPG)			*/
/*		8 und 24 Bit								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	(void)smurf_struct;
	start_driver();
	return M_PICDONE;
}
