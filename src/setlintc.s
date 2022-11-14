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

*---------------------------------------------------
*       howmany Pixel ins Standardformat setzen
*       auf 1 Bit ausgerichtet!
*       TC-Formate in Planes ...
*   Deklaration: 
*   int setpix_std_line_TC(char *buf, char *dest, int depth, long planelen, int howmany);
*                                 a0          a1         d0              d1          d2
*   buf:        Buffer, in dem sich die Pixelpackedzeile befindet
*   dest:       Zielspeicherbereich
*   depth:      Farbtiefe
*   howmany:    Wieviele Pixel sollen gesetzt werden
*   planelen:   LÑnge einer Plane des Zielbildes in Bytes
*
*   Noch unoptimiert, nur zum Austesten
*---------------------------------------------------
	.IFEQ PURE_C
/* gcc cdecl entry point */
	.globl _setpix_std_line_TC
_setpix_std_line_TC:              /* Hier wird je nach Einsprungpunkt die Variable */
	move.l d2,-(a7)
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
	bsr.s setpix_std_line_TC
	move.l (a7)+,d2
	rts
	.ENDC


	.globl setpix_std_line_TC
setpix_std_line_TC:
    movem.l d3-d6,-(sp)

    clr.l d6
    move.w d2,d6                /* (howmany */
    addq.w #7,d6                /* + 7) */
    lsr.w #3,d6                 /* / 8 */
    move.w d6,return_val

    sub.l d6,d1                 /* planelength um eine Zeile kÅrzen */
    lsl.w #3,d6                 /* * 8 */

    subq.w #1,d2
    lsr.w #3,d2
    move.w d2,d5                /* howmany retten */

    clr.w d4
    subq.w #1,d0

looplaneTC:
    looppixTC:
        clr.w d3
        /*-------------------------Schleifenanfang - 8 Pixel einer Plane fertigmachen */

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_1  /* Nein? Setzen *berspringen */
        or.b #0x80,d3
looppixTC_1:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_2  /* Nein? Setzen *berspringen */
        or.b #0x40,d3
looppixTC_2:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_3  /* Nein? Setzen *berspringen */
        or.b #0x20,d3
looppixTC_3:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_4  /* Nein? Setzen *berspringen */
        or.b #0x10,d3
looppixTC_4:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_5  /* Nein? Setzen *berspringen */
        or.b #0x08,d3
looppixTC_5:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_6  /* Nein? Setzen *berspringen */
        or.b #0x04,d3
looppixTC_6:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_7  /* Nein? Setzen *berspringen */
        or.b #0x02,d3
looppixTC_7:

        btst.b d4,(a0)+    /* Bit d4 gesetzt? */
        beq.b looppixTC_8  /* Nein? Setzen *berspringen */
        or.b #0x01,d3
looppixTC_8:

        /*-------------------------zum Schleifenanfang */
        move.b d3,(a1)+         /* Byte schreiben und eins weiter */
        dbra d2,looppixTC

    move.w d5,d2                /* howmany zurÅck */

    suba.l d6,a0                /* pxl-tab zum Anfang */
    adda.l d1,a1                /* nÑchste Plane */

    addq.w #1,d4               /* getestetes Bit ++ */
    dbra d0,looplaneTC

    move.w return_val,d0

    movem.l (sp)+,d3-d6
    rts


return_val: ds.w 1
