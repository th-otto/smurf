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

*************************************************************
*
*   EnthÑlt eine Routine zum Lesen einer ganzen Bildzeile aus
*   dem Standardformat.
*
* ************************************************************



*---------------------------------------------------
*       howmany Pixel aus dem Standardformat lesen
*   Deklaration:    
*   void getpix_std_line(char *std, char *buf, int depth, long planelen, int howmany);
*                               a0         a1         d0             d1           d2
*   std:        Buffer, in dem sich die Standardformatzeile befindet
*   buf:        Zielspeicherbereich
*   depth:      Farbtiefe
*   planelen:   LÑnge einer Plane des Zielbildes in Bytes
*   howmany:    Wieviele Pixel sollen gelesen werden
*
*   Noch unoptimiert, nur zum Austesten
*---------------------------------------------------
/* gcc cdecl entry point */
	.IFEQ PURE_C
	.globl  _getpix_std_line
	move.l d2,-(a7)
_getpix_std_line:
	move.l 8(a7),a0
	move.l 12(a7),a1
#ifndef __MSHORT__
	move.w 18(a7),d0
	move.l 20(a7),d1
	move.w 26(a7),d2
#else
	move.w 16(a7),d0
	move.l 18(a7),d1
	move.w 22(a7),d2
#endif
	bsr.s getpix_std_line
	move.l (a7)+,d2
	rts
	.ENDC

	.globl  getpix_std_line
getpix_std_line:
    movem.l d3-d7/a2,-(sp)

    movea.l a1,a2
    move.w  d2,d7               /* (howmany */
    addq.w  #7,d7               /* + 7) */
    lsr.w   #3,d7               /* / 8 */
    sub.w   d7,d1               /* planelength um eine Zeile kÅrzen */
    subq.w  #1,d2
    move.w  d2,d5               /* howmany retten */
    subq.w  #1,d0               /* planes - 1 fÅr dbra */

    moveq.l #1,d6               /* sauberer als bset.l #0,d6 */
                                /* und kleiner und schneller ;-) */

looplane:
    looppix:
        /*-------------------------Schleifenanfang - 8 Pixel einer Plane fertigmachen */
        btst.b  #7,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_1       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_1:
        addq.l  #1,a1           /* und weiter */

        btst.b  #6,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_2       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_2:
        addq.l  #1,a1           /* und weiter */

        btst.b  #5,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_3       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_3:
        addq.l  #1,a1           /* und weiter */

        btst.b  #4,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_4       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_4:
        addq.l  #1,a1           /* und weiter */

        btst.b  #3,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_5       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_5:
        addq.l  #1,a1           /* und weiter */

        btst.b  #2,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_6       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_6:
        addq.l  #1,a1           /* und weiter */

        btst.b  #1,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_7       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_7:
        addq.l  #1,a1           /* und weiter */

        btst.b  #0,(a0)         /* Bit #Testbit gesetzt? */
        beq.b   looppix_8       /* Nein? Setzen Åberspringen */
        or.b    d6,(a1)         /* Bit in ziel setzen */
looppix_8:
        addq.l  #1,a1           /* und weiter */

        /*-------------------------zum Schleifenanfang */
        adda.l  #1,a0           /* buffer um eins weiter */
        subq.w  #8,d2           /* howmany runtersetzen */
        bpl looppix             /* nÑchste 8 Pixel */

    move.w  d5,d2               /* howmany zurÅck */
    movea.l a2,a1

    adda.l  d1,a0               /* nÑchste Plane */

    lsl.w   #1,d6               /* zu setzendes Bit verschieben */

    dbra    d0,looplane

    movem.l (sp)+,d3-d7/a2
    rts
    


*----------------- ein anderer Versuch, geht leider noch nicht, kînnte
*                           aber schneller sein
*
*   void getpix_std_line(char *std, char *buf, int depth, long planelen, int howmany);
* GEM-Std -> PackedPixel
* 18.08.98, Olaf
*   a0: Quelle (Std)
*   a1: Ziel
*   d0: Farbtiefe
*   d1: PlanelÑnge im Quellbild
*   d2: Pixelanzahl

	.IFNE 0
    movem.l d3-d4/a2,-(sp)  /* Register retten */
    moveq.l #7,d3
    subq.w #1,d0
    subq.w #1,d2
    move.w  d0,d4

loop:
    movea.l a0,a2              /* Quelladresse retten */

plane:
    btst.b d3,(a0)         /* Bit d3 testen */
    beq noset
    bset.b d0,(a1)     /* ggfs. d0 im Ziel setzen */
    noset:
    adda.l d1,a0           /* Quelle: eine Plane weiter */
    dbra d0,plane          /* und das fÅr d0 planes */

    movea.l a2,a0               /* Quelladresse restaurieren */
    addq.l #1,a1                /* Ziel um 1 Pixel weiter */
    move.b d4,d0                /* FarbtiefenzÑhler restaurieren */

    subq.b #1,d3                /* TestBit runterzÑhlen */
    bge no8
    moveq #7,d3                /* TestBit=7 */
    addq.l #1,a0               /* Quelladresse um ein Byte weiter */
    no8:

    dbra d2,loop               /* fÅr d2 Pixel */

    movem.l (sp)+,d3-d4/a2     /* Register restaurieren */
    rts                         /* und tschÅss */

	.ENDC
