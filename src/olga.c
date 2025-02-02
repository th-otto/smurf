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

#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "import.h"
#include "smurfine.h"
#include "globdefs.h"
#include "smurf_st.h"
#include "smurf.h"
#include "smurf_f.h"
#include "plugin.h"
#include "ext_rsc.h"
#include "olga.h"
#include "olgagems.h"
#include "bindings.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "debug.h"


/*-------------------------------------------------------------------
	Initialisieren des OLGA-Protokolls, wie von Th. Much beschrieben.
	----------------------------------------------------------------*/
short init_OLGA(void)
{
	char *path;
	char *path_name;
	char *dot;
	char name[9] = "";
	char old[SM_PATH_MAX];
	WORD id;

	DEBUG_MSG(("init_OLGA...\n"));

	Dialog.busy.reset(128, "OLGA Init");

	Sys_info.olgaman_ID = -1;


	/*
	 * Erstmal versuchen, den Olgamanager zu finden
	 */
	id = appl_find("OLGA    ");

	if (id < 0)
	{
		if ((path = getenv("OLGAMANAGER")) != NULL)
		{
			path_name = strrchr(path, '\\');
			if (path_name != 0)
				strcpy(name, path_name);
			else
				strncpy(name, path, 8);
			dot = strrchr(name, '.');
			if (dot)
				*dot = 0;

			strupr(name);

			strncat(name, "         ", 8 - strlen(name));
			id = appl_find(name);

			/*
			 * er l�uft nicht, also nachstarten
			 */
			if (id < 0)
			{
				if ((Sys_info.OS & MINT) || (Sys_info.OS & MATSCHIG))
				{
					if (Fattrib(path, 0, 0) >= 0)
					{
						get_path(old, 0);
						set_path(path);
						if (Sys_info.OS & MINT)
							id = shel_write(0, 1, 0, path, "");
						else if (Sys_info.OS & MATSCHIG)
							id = shel_write(1, 1, 100, path, "");
						set_path(old);

						if (id == 0)
							id = -1;
					}
				}
			}
		}
	}


	if (id < 0)
	{
		DEBUG_MSG(("init_OLGA... Ende ohne OLGA-Manager\n"));
		return -1;
	}

	/* Die ID wird hier negativ eingetragen, daran erkennen die Funktionen
	   sp�ter, da� Olga nicht gekommen ist. Wenn vom Olga-Manager die Message
	   OLGA_INIT ohne Fehler ankommt, hei�t das, die Show kann beginnen und
	   die ID wird vom Smurf-Messagehandler wieder verpositivt. Das ganze
	   Spielchen nur, damit Smurf nicht mit Olga was anf�ngt, obwohl ihr
	   Manager noch gar nicht bereit ist, alles mitzuschneiden. ;-) */

	Sys_info.olgaman_ID = -id;

	/*
	 * und jetzt OLE_INIT
	 */
	Comm.sendAESMsg(-Sys_info.olgaman_ID, OLE_INIT, OL_SERVER, 0, 0, 0, 0x5247, -1);

	DEBUG_MSG(("init_OLGA... Ende \n"));

	return 0;
}



/*-------------------------------------------------------------------
	Und hiermit verabschieden wir uns wieder von Olga:
	----------------------------------------------------------------*/
short deinit_OLGA(void)
{
	if (Sys_info.olgaman_ID >= 0)
		Comm.sendAESMsg(Sys_info.olgaman_ID, OLE_EXIT, -1);

	return 0;
}


/*-------------------------------------------------------------------
	OLGA_UPDATE verschicken. Der Dateiname wird kopiert, falls sich *filename
	�ndert, bevor die Clients die Update-Message vom Manager kriegen.
	----------------------------------------------------------------*/
short update_OLGA(char *filename)
{
	char *new_filename;

	if (Sys_info.olgaman_ID < 0)
		return -1;

	/*-------- Block wenn m�glich als global anfordern? */
	if (Ssystem(FEATURES, 0L, 0L) != EINVFN || (Sys_info.OS & MINT) || (Sys_info.OS & MATSCHIG))
		new_filename = (char *) Mxalloc(strlen(filename) + 1, 0x20);	/* plus ein Byte f�rs Nullbyte */
	else
		new_filename = (char *) SMalloc(strlen(filename) + 1);	/* plus ein Byte f�rs Nullbyte */

	strcpy(new_filename, filename);

	Comm.sendAESMsg(Sys_info.olgaman_ID, OLGA_UPDATE, LONG2_2INT(new_filename), 0, -1);

	/* auf keinen Fall hier schon new_filename freigeben! */
	/* das wird nach Antwort von Olga durch OLGA_ACK gemacht */

	return 0;
}


/*-------------------------------------------------------------------
	OLGA_RENAME verschicken, wenn eine Datei unter einem anderen Namen
	gespeichert wurde. Danach mu� noch ein OLGA_UPDATE mit newname folgen
	wenn der Dateiinhalt ver�ndert wurde!
	-------------------------------------------------------------------*/
short rename_OLGA(char *oldname, char *newname)
{
	char *old_filename;
	char *new_filename;

	if (Sys_info.olgaman_ID < 0)
		return -1;


	/*-------- Block wenn m�glich als global anfordern? */
	if (Ssystem(FEATURES, 0L, 0L) != EINVFN || (Sys_info.OS & MINT) || (Sys_info.OS & MATSCHIG))
	{
		old_filename = (char *) Mxalloc(strlen(oldname) + 1, 0x20);	/* plus ein Byte f�rs Nullbyte */
		new_filename = (char *) Mxalloc(strlen(newname) + 1, 0x20);	/* plus ein Byte f�rs Nullbyte */
	} else
	{
		old_filename = (char *) SMalloc(strlen(oldname) + 1);	/* plus ein Byte f�rs Nullbyte */
		new_filename = (char *) SMalloc(strlen(newname) + 1);	/* plus ein Byte f�rs Nullbyte */
	}

	strcpy(old_filename, oldname);
	strcpy(new_filename, newname);

	Comm.sendAESMsg(Sys_info.olgaman_ID, OLGA_RENAME, LONG2_2INT(old_filename), LONG2_2INT(new_filename), 0, -1);

	/* auf keinen Fall hier schon old_filename oder new_filename freigeben! */
	/* das wird nach Antwort von Olga durch OLGA_ACK gemacht */

	return 0;
}
