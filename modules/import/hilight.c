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

/* **************************************************** */
/*                                                      */
/* Highlight PC-Format - Importer                       */     
/*      Format von: wei�ichnich                         */
/*      Importer von Olaf.                              */
/*                                                      */
/* Dieses Format braucht man, wie so viele andere,      */
/* ungef�hr so dringend wie eine Kugel zwischen den     */
/* Augen. Wahrlich kein Highlight. Und der Erfinder war */
/* mit Sicherheit auch keine Leuchte...                 */
/*                                                      */
/*  Nummer 4 der Unterrichtsgenerierten Importer.       */
/*   Mit Dale die ganze verdammte Nacht gecoded, dann   */
/*   fr�h um 8... naja, halb neun in die Schule und     */
/*   unwahrscheinlich tot gewesen. F�hl mich wie ne     */
/*   Leiche auf Urlaub, nur da� die wahrscheinlich      */   
/*   nicht so m�de w�re.                                */
/*   Lt. Holtorf m��te der Import funktionieren - aber  */
/*   wer wei� das bei dem und seinem Buch schon...      */
/*                                                      */
/* **************************************************** */



#include <tos.h>
#include <stdio.h>
#include <string.h>
#include <ext.h>

#include "..\import.h"
#include "..\..\src\smurfine.h"


MOD_INFO module_info=
{
    "Highlight - Importer",
    0x0010,
    "Olaf Piesche",
/* Extensionen */
    "PIC","","","","","","","","","",
/* Slider */
    "","","","",
/* Editfelder */
    "","","","",
/* Checkboxen */
    "","","","",
/* Minima + Maxima */
/* Slider */
    0,0,
    0,0,
    0,0,
    0,0,
/* Edits */
    0,0,
    0,0,
    0,0,
    0,0,
/* Defaults */
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};



int     imp_module_main(GARGAMEL *smurf_struct)
{
int width=0, height=0, depth=0;
char *picdata;
int *pd, *picd;
long Len24, Len16, filelen;
int p15;
int x,y, red, green, blue;


if(smurf_struct->module_mode==MTERM) return(M_EXIT);


picdata=smurf_struct->smurf_pic->pic_data;
pd = (int *)picdata;
filelen=smurf_struct->smurf_pic->file_len;

width=*(pd);
height=*(pd+1);

/* Die spitzenm�ige Kennung ermitteln... */
Len24=(long)width*(long)height*3L;
Len16=(long)width*(long)height*2L;

if(filelen-4 == Len16) depth=16;
else if(filelen-4 == Len24) depth=24;

picd=pd;
pd+=2;

if(depth==16)
{

 for(y=0; y<height; y++)  
   {
      for(x=0; x<width; x++)     
      {
     p15=*(pd++);
     red = p15 & 0xf800;   /* RGB ausmaskieren */
     green=p15 & 0x07e0;
     blue =p15 & 0x003f;
     green>>=1; /* und R und B vertauschen. Ich wei� noch nicht, */
     red<<=10;  /* ob das notwendig ist, da Klaus etwas mi�ver- */
     blue>>=11; /* st�ndliche Angaben macht (RGB oder BGR?) */
     *(picd++)=red|green|blue;
      }
   }

   _Mshrink(picdata, Len16);

}
else if(depth==24)
{
   memcpy(picd, picdata, Len24);
   _Mshrink(picd, Len24);
}

smurf_struct->smurf_pic->pic_width=width;
smurf_struct->smurf_pic->pic_height=height;
smurf_struct->smurf_pic->depth=depth;
strncpy(smurf_struct->smurf_pic->format_name, "Highlight PC-Format   ", 21);

return(M_DONEEXIT);
}