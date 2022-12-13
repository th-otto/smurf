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
 * The Initial Developer of the Original Code is
 * Christian Eyrich
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include "import.h"
#include <stddef.h>
#include "fontsel.h"

#ifndef FALSE
#define FALSE	0
#define TRUE	1
#endif


int get_cookie(unsigned long cookie, unsigned long *value)
{
	unsigned long *cookiejar;

	/* ansonsten value auf 0L */
	*value = 0;
	/* Zeiger auf Cookie Jar holen */
	cookiejar = (unsigned long *) Setexc(0x5a0 / 4, (void (*)()) -1);

	/* Cookie Jar installiert? */
	if (cookiejar == NULL)
		return FALSE;

	/* Keksdose nach cookie durchsuchen */
	while (*cookiejar != 0)
	{
		if (*cookiejar++ == cookie)
		{
			/* wenn cookie gefunden wurde, value auf Cookiewert setzen, */
			*value = *cookiejar;
			return TRUE;
		}
		cookiejar += 1;
	}

	return FALSE;
}
