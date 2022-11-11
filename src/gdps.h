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

/* Allgemeine GDPS-Treiber-Struktur */
typedef struct
{
    void *next;             /* Zeiger auf nÑchsten Treiber oder 0L */
    long magic;             /* Magic, muû 'GDPS' (0x47445053) sein */
    short version;          /* Treiberversion * 100 */
    short type;             /* Treibertyp, interessant ist 0x000-0x0ff fÅr graphische EingabegerÑte */
    char *info;             /* Zeiger auf Treiberinfo, 0-terminierter String mit max. 32 Zeichen */
    char *info2;            /* Zeiger auf Programmierer/Copyright, 0-terminierter String mit max. 32 Zeichen */
    short desc;             /* Scannerbeschreibung */
    short cols;             /* Anzahl Farben */
    short depth;            /* Mîgliche Multi-Value Bittiefen */
    short free;             /* Flag ob Scanner frei ist */
    short command;          /* Kommando an Scanner */
    void *comstruct;        /* Zeiger auf Kommandostruktur */
} GENERAL_GDPS;


/* Smurf GDPS-Treiber-Struktur */
typedef struct
{
    short version;          /* Treiberversion * 100 */
    char info[33];          /* Zeiger auf Treiberinfo, 0-terminierter String mit max. 32 Zeichen */
    short desc;             /* Scannerbeschreibung */
    short cols;             /* Anzahl Farben */
    short depth;            /* Mîgliche Multi-Value Bittiefen */
} SMURF_GDPS;


/* Kommandostruktur fÅr GDPI-Scanner */
typedef struct
{
    unsigned short result;  /* Ergebnis das Treiber meldet */
    short modes;            /* erlaubte Scanmodi */
    short depth;            /* Tiefe des Bildes in Bit/Pixel */
    void *memory;           /* dahin soll das Bild gehen */
    long memlen;            /* verfÅgbarer Speicher */
    short width_byte;       /* Breite einer Zeile in Bytes */
    short height;           /* Hîhe des Bildes in Zeilen */
    short mmwidth;          /* Breite in 1/10 Millimetern */
    short mmheight;         /* Hîhe in 1/10 Millimetern */
    short xdpi;             /* Auflîsung in x-Richtung in dpi */
    short ydpi;             /* Auflîsung in y-Richtung in dpi */
    short modulo;           /* 2 => Bild wird WordbÅndig */
    short start_x;          /* linke obere Ecke x in 1/10mm */
    short start_y;          /* linke obere Ecke y in 1/10mm */
    long ser_nr;            /* Seriennummer */

/**** 
der nun folgende Teil ist nur beim Aufruf des Scanners mit
den Befehlen der 0x2XX Serie definiert
******/
    short add_bits;         /* Angabe, wieviele Bits zusÑtzlich */
                            /* z.B. besitzt CRANACH Studio */
                            /* zu jedem Bild 2 Masken. Wird  */
                            /* eine Bitmap gescant, so benîtigt */
                            /* das Programm nicht nur 1 BIT pro */
                            /* Pixel, sonder drei Bit */
                            /* Analog dazu mÅûten bei einem */
                            /* Grautonbild 8 + 2 = 10 Pixel */
                            /* berechnet werden. Sollte dieser */
                            /* Wert nicht beachtet werden, so */
                            /* kann es nach dem Scannen passieren */
                            /* daû CRANACH Studio das Fenster */
                            /* nicht îffnen kann, da zwar fÅr */
                            /* das gescannte Bild Speicher vor- */
                            /* handen ist, nicht aber fÅr die */
                            /* notwendigen Masken.             */
                            /* gebraucht werden, z.B. zwei */
                            /* zusÑtzliche Bits fÅr die Maske */
    void *Dchange_pointer;  /* Zeiger auf diese Funktion */
    void *Dupdate;          /* Zeiger auf jene Funktion */
    short read;             /* Lesebufferindex virtuell */
    short write;            /* Schreibbufferindex virtuell */
    short virt_flag;        /* flag, ob virtuell gearbeitet wird */
} COMSTRUCT;
