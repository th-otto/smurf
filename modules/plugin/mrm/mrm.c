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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurf_st.h"
#include "../../../src/rsc/en/smurf.h"
#include "smurfine.h"
#include "globdefs.h"
#include "plugin.h"
#include "popdefin.h"
#include "tools.h"
#include "multi.h"

#undef NUM_STRINGS
#undef NUM_FRSTR
#undef NUM_UD
#undef NUM_IMAGES
#undef NUM_BB
#undef NUM_FRIMG
#undef NUM_IB
#undef NUM_CIB
#undef NUM_TI
#undef NUM_OBS
#undef NUM_TREE

#include "country.h"

#if COUNTRY==1
#include "de/mrm.rsh"
#elif COUNTRY==0
#include "en/mrm.rsh"
#elif COUNTRY==2
#include "en/mrm.rsh"		/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define TextCast ob_spec.tedinfo->te_ptext

static char const name_string[] = "Smurf ModComm-Protokoll Plugin";

PLUGIN_INFO plugin_info = {
	name_string,
	0x100,								/* Plugin-Version 1.00 */
	SMURF_VERSION,						/* fÅr Smurf-Version 1.06 */
	0									/* und nicht resident. */
};

