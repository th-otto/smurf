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

* seekcol.s
*
* Sucht aus Palette (a0), 4(a0), 8(a0) die �hnlichste
* Farbe zu (12(a0), 16(a0) und 20(a0) (jeweils r,g,b) aus
* d0 Farben.
*
*   int seek_nearest_col(long *par, int maxcol);
*

	.IFEQ PURE_C
/* gcc cdecl entry point */
	.globl _seek_nearest_col
_seek_nearest_col:
	move.l 4(a7),a0
#ifndef __MSHORT__
	move.w 10(a7),d0
#else
	move.w 8(a7),d0
#endif
	.ENDC

	.globl seek_nearest_col
seek_nearest_col:
    movem.l a2-a4/d2-d7,-(sp)

    move.l (a0)+,a1        /* rpal */
    move.l (a0)+,a2        /* gpal   */
    move.l (a0)+,a3        /* bpal */
    move.l (a0)+,d1
    move.l (a0)+,d2
    move.l (a0)+,d3

    subq.w  #1,d0          /* wg. dbra */
    move.w  d0,maxcol      /* maxcol speichern */

    move.l #32767,d7
    
loopcol:
    move.w (a1)+,d4        /* Rot-differenz */
    sub.w   d1,d4
    bpl.b rok1
    neg.w d4
rok1:

    move.w (a2)+,d5        /* Gr�n-Differenz */
    sub.w   d2,d5
    bpl.b gok1
    neg.w d5
gok1:

    move.w (a3)+,d6        /* Blau-Differenz */
    sub.w   d3,d6
    bpl.b bok1
    neg.w d6
bok1:

    add.w   d6,d5          /* Gesamtdifferenz */
    add.w   d5,d4
    
    cmp.w   d7,d4          /* Speichern? */
    bge coldiffok           /* coldiffok */
        move.w  d4,d7
        move.w  d0,a4
    coldiffok:

    dbra d0,loopcol

    
    move.w  maxcol,d0      /* R�ckgabe-Index ausrechnen  */
    sub.w   a4,d0
    
    movem.l (sp)+,a2-a4/d2-d7
    rts








*************************************************************
* makeNCT
*       Erzeugt eine NCT in 12(a0)
*
* Sucht aus Palette (a0), 4(a0), 8(a0) die �hnlichste
*   Farbe zu 32768 15 Bit-Farben aus d0 Palettenfarben
*
*   void makeNCT(long *par, long maxcol);
*
*************************************************************
/* gcc cdecl entry point */
	.globl   _makeNCT
_makeNCT:
	move.l 4(a7),a0
	move.l 8(a7),d0

	.globl   makeNCT
makeNCT:
    movem.l d2-d7/a2-a6,-(sp)

    move.l (a0)+,a1        /* rpal */
    move.l (a0)+,a2        /* gpal   */
    move.l (a0)+,a3        /* bpal */
    move.l (a0)+,a0        /* nct-buffer */

    clr.l   d1
    clr.l   d2
    clr.l   d3


    move.w  d0,a6
    subq.w  #1,a6          /* wg. dbra */
    move.l  #0,a5

    andi.l #0xffff,d0
    lsl.w   #1,d0

    move.l  d0,maxcol
    add.l d0,maxcol
    add.l d0,maxcol

loop32:
    move.w  a5,d1      /* rot ausrechnen */
    rol.w   #6,d1
    and.w   #31,d1

    move.w  a5,d2      /* gr�n ausrechnen */
    lsr.w   #5,d2
    and.w   #31,d2

    move.w  a5,d3      /* blau ausrechnen */
    and.w   #31,d3

    move.w  a6,d0

    moveq.l #127,d7
    
loopcoln:
    movem.w (a1)+,d4-d6

    sub.w   d1,d4  /* R-Diff */
    bpl.b rok2
    neg.w d4
rok2:

    sub.w   d2,d5  /* G-Diff */
    bpl.b gok2
    neg.w d5
gok2:

    sub.w   d3,d6  /* B-Diff */
    bpl.b bok2
    neg.w d6
bok2:

    add.w   d6,d5              /* Gesamtdifferenz */
    add.w   d5,d4
    
    cmp.w   d7,d4              /* Speichern? */
    bge.b coldiffokn
        move.w  d4,d7
        movea.w d0,a4
coldiffokn:

    dbra    d0,loopcoln

    suba.l  maxcol,a1

    move.w  a6,d1      /* R�ckgabe-Index ausrechnen  */
    sub.w   a4,d1
    move.b  d1,(a0)+           /* Index eintragen */

    addq.l  #1,a5
    cmp.l   #32768,a5
    blt loop32

    movem.l (sp)+,d2-d7/a2-a6
    rts

.BSS
.EVEN
maxcol:     ds.l    1
