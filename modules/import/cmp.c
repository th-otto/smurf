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

/* =========================================================*/
/*				CMP Atari Public Painter Monochrom			*/
/* Version 0.1  --  irgendwann 1996							*/
/*	  1 Bit, binÑre Version									*/
/* Version 0.2  --  18.11.98								*/
/*	  sicherer gegen kaputte CMP gemacht (EndeÅberprÅfung	*/
/*	  eingebaut)											*/
/* =========================================================*/

#include <tos.h>
#include <ext.h>
#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "..\import.h"
#include "..\..\src\smurfine.h"

void *(*SMalloc)(long amount);
int	(*SMfree)(void *ptr);

char *fileext(char *filename);

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {"Atari Public Painter",
						0x0020,
						"Christian Eyrich, Dale Russell",
						"CMP", "", "", "", "",
						"", "", "", "", "",
						"Slider 1",
						"Slider 2",
						"Slider 3",
						"Slider 4",
						"Checkbox 1",
						"Checkbox 2",
						"Checkbox 3",
						"Checkbox 4",
						"Edit 1",
						"Edit 2",
						"Edit 3",
						"Edit 4",
						0,128,
						0,128,
						0,128,
						0,128,
						0,10,
						0,10,
						0,10,
						0,10,
						0,0,0,0,
						0,0,0,0,
						0,0,0,0,
						0
						};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Atari Public Painter Monochrom (CMP)		*/
/*		1 Bit, RLE									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
int imp_module_main(GARGAMEL *smurf_struct)
{
	char *buffer, *obuffer, *ziel, *oziel, *pal, *fname,
		 S_Byte, v1, v2;

	unsigned int width, height, w, x, y, n;

	unsigned long maxlen;

	
	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;
	
	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	fname = smurf_struct->smurf_pic->filename;
	if(stricmp(fileext(fname), "CMP") != 0)
		return(M_INVALID);
	else
	{
		S_Byte = *buffer;
		buffer += 2;

		width = 640;
		w = 80;
		height = 400;

		strncpy(smurf_struct->smurf_pic->format_name, "Atari Public Painter", 21);
		smurf_struct->smurf_pic->pic_width = width;
		smurf_struct->smurf_pic->pic_height = height;
		smurf_struct->smurf_pic->depth = 1;

		smurf_struct->services->reset_busybox(128, "Public Painter 1 Bit");

		if((ziel = SMalloc(32000L)) == 0)
			return(M_MEMORY);
		else
		{
			oziel = ziel;
			memset(ziel, 0x0, 32000L);

			maxlen = 32000L;

			y = 0;
			do
			{
				x = 0;
				do
				{
					v1 = *buffer++;
					if(v1 == S_Byte)
					{
						n = *buffer++ + 1;
						v2 = *buffer++;

						x += n;

						while(n--)
						{
							*ziel++ = v2;
							if(!--maxlen)
								goto end;
						}
					}
					else
					{
						*ziel++ = v1;
						x++;
						if(!--maxlen)
							goto end;
					}	
				} while(x < w);
			} while(++y < width);

end:

			buffer = obuffer;
			ziel = oziel;

			smurf_struct->smurf_pic->pic_data = ziel;

			SMfree(buffer);

			smurf_struct->smurf_pic->format_type = FORM_STANDARD;

			pal = smurf_struct->smurf_pic->palette;
			pal[0] = 255;
			pal[1] = 255;
			pal[2] = 255;
			pal[3] = 0;
			pal[4] = 0;
			pal[5] = 0;
		} /* Malloc */
	} /* Erkennung */
	
	return(M_PICDONE);
}


char *fileext(char *filename)
{
	char *extstart;


	if((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');
	
	return(extstart);
} /* fileext */
