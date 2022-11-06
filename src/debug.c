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

/************************************************************/
/* Routinen, die es erleichtern, ein Debugfile ausgeben zu	*/
/* lassen.													*/
/************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "debug.h"

#define SCREEN 1

void write_debug(const char *message)
{
	FILE *file;


	if(message == NULL)
	{
		file = fopen("debug.log", "w");
		message = "Smurf Debug";
	}
	else
		file = fopen("debug.log", "a");

	if(file != NULL)
	{
		fputs(message, file);
		fputs("\n", file);

		fclose(file);
	}
		
}


void DebugMsg(const char *Arg, ...)
{
#ifdef DEBUG
	char Puf[512];
	va_list	ap;
	FILE *DebugHandle;
	int len;
	
	va_start( ap, Arg );
	DebugHandle = fopen( "smurf.log", "a+" );

	if( DebugHandle )
	{
		len = vsprintf( Puf, Arg, ap);
		fwrite( Puf, sizeof( char ), len, DebugHandle );
		fclose( DebugHandle );
	}
	
	va_end ( ap );
#endif
}
