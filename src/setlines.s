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

* ************************************************************
*     SETLINES.S   derived from SETLINES.SO for some reasone
* ************************************************************
*
*   EnthÑlt eine Routine zum Schreiben einer ganzen Bildzeile
*   ins Standardformat.
*
* ************************************************************



*---------------------------------------------------
*  howmany Pixel ins Standardformat setzen
*  auf 8 Bit ausgerichtet!
* Deklaration: 
* int setpix_std_line(char *buf, char *dest, int depth, long planelen, int howmany);
*                            a0          a1         d0             d1           d2
*   buf:        Buffer, in dem sich die Pixelpackedzeile befindet
*   dest:       Zielspeicherbereich
*   depth:  Farbtiefe
*   howmany:    Wieviele Pixel sollen gesetzt werden
*   planelen:   LÑnge einer Plane des Zielbildes in Bytes
*
*   Noch unoptimiert, nur zum Austesten
*---------------------------------------------------
/* gcc cdecl entry point */
	.globl _setpix_std_line
_setpix_std_line:
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
	bsr.s setpix_std_line
	move.l (a7)+,d2
	rts

	.globl setpix_std_line
setpix_std_line:
    movem.l d3-d7,-(sp)

    clr.l d6
    move.w d2,d6                /* (howmany */
    addq.w #7,d6                /* + 7) */
    lsr.w #3,d6                 /* / 8 */
    move.w d6,return_val

    sub.l d6,d1                 /* planelength um eine Zeile kÅrzen */

    lsl.w #3,d6                 /* * 8 */

    subq.w #1,d2
    lsr.w #3,d2
    move.w d2,d5               /* howmany retten */

    clr.w d3
    subq.w #1,d0
    move.w d0,planeCount

    move.l #0x01010101,d4  /* Bitmaske */

looplane:
    looppix:
        clr.w d0

        /*
         * Optimierversuch Nr. 3
         * Rechnerisch 82 Taktzyklen zum Zusammenbau von einem Byte im Vergleich zu
         * 128 in der Originalroutine (40 Ticks bei 455*634*4bit auf 32MHz Falcon gespart)
         *
         * Bits 4-7 im Zielbyte zusammenbasteln
         */
        move.l (a0)+,d7        /* long holen 5 */
        and.l d4,d7            /* je 1 bit pro byte ausmaskieren 2 */
        lsr.l d3,d7            /* Ergebnis wieder zurueck schieben, damit die Testbits auf bit 1 liegen 6 */

        lsl.l #4,d7            /* bit 0 Byte 1 -> Bit 4 byte 1 4 */
        or.b d7,d0             /* 2 */

        lsr.l #7,d7            /* Bit 0 Byte 2 -> bit 5 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 3 -> bit 6 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 4 -> bit 7 Byte 1 */
        or.b d7,d0

        /* Bits 0-3 im Zielbyte zusammenbasteln */
        
        move.l (a0)+,d7        /* long holen */
        and.l d4,d7            /* je 1 bit pro byte ausmaskieren */
        lsr.l d3,d7            /* Ergebnis wieder zurueck schieben, damit die Testbits auf bit 1 liegen */

        or.b d7,d0             /* Bit 0 Byte 1 -> bit 0 Byte 1 */

        lsr.l #7,d7            /* Bit 0 Byte 2 -> bit 1 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 3 -> bit 2 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 4 -> bit 3 Byte 1 */
        or.b d7,d0

        move.b d0,(a1)+         /* Byte schreiben und eins weiter */
    dbra d2,looppix

    move.w d5,d2               /* howmany zurÅck */
    suba.l d6,a0               /* pxl-tab zum Anfang */
    adda.l d1,a1               /* nÑchste Plane */

    addq.w #1,d3               /* Testbit ++ */
    lsl.l #1,d4                /* Bitmaske verschieben (naextes Testbit) */

    sub.w #1,planeCount
    bpl looplane

 move.w return_val,d0
 movem.l (sp)+,d3-d7
 rts



*---------------------------------------------------
*       howmany Pixel ins Standardformat setzen
*       auf 16 Bit ausgerichtet!
*   Deklaration: 
*   int setpix_std_line116(char *buf, char *dest, int depth, long planelen, int howmany);
*                                 a0          a1         d0             d1           d2
*   buf:        Buffer, in dem sich die Pixelpackedzeile befindet
*   dest:       Zielspeicherbereich
*   depth:      Farbtiefe
*   howmany:    Wieviele Pixel sollen gesetzt werden
*   planelen:   LÑnge einer Plane des Zielbildes in Bytes
*
*   Noch unoptimiert, nur zum Austesten
*---------------------------------------------------
/* gcc cdecl entry point */
	.globl _setpix_std_line16
_setpix_std_line16:              /* Hier wird je nach Einsprungpunkt die Variable */
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
	bsr.s setpix_std_line16
	move.l (a7)+,d2
	rts


	.globl setpix_std_line16
setpix_std_line16:              /* Hier wird je nach Einsprungpunkt die Variable */
    movem.l d3-d6,-(sp)

    clr.l d6
    move.w d2,d6                /* (howmany */
    add.w #15,d6                /* + 15) */
    lsr.w #4,d6                 /* / 16 */
    lsl.w #1,d6                 /* *2 */
    move.w d6,return_val

    sub.l d6,d1                 /* planelength um eine Zeile kÅrzen */
    lsl.w #3,d6                 /* * 8 */

    subq.w #1,d2
    lsr.w #4,d2
    move.w d2,d5                /* howmany/16 retten */

    clr.w d3

    subq.w #1,d0
    move.w d0,planeCount


    move.l #0x01010101,d4  /* Bitmaske */

looplane16:
    looppix16:
        /*-------------------------Schleifenanfang - 8 Pixel einer Plane fertigmachen */
        clr.w d0

        /* Bits 4-7 im Zielbyte zusammenbasteln */
        
        move.l (a0)+,d7        /* long holen 5 */
        and.l d4,d7            /* je 1 bit pro byte ausmaskieren 2 */
        lsr.l d3,d7            /* Ergebnis wieder zurueck schieben, damit die Testbits auf bit 1 liegen 6 */

        lsl.l #4,d7            /* bit 0 Byte 1 -> Bit 4 byte 1 4 */
        or.b d7,d0             /* 2 */

        lsr.l #7,d7            /* Bit 0 Byte 2 -> bit 5 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* Bit 0 Byte 3 -> bit 6 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* Bit 0 Byte 4 -> bit 7 Byte 1 */
        or.b d7,d0

        /* Bits 0-3 im Zielbyte zusammenbasteln */
        
        move.l (a0)+,d7        /* long holen */
        and.l d4,d7            /* je 1 bit pro byte ausmaskieren */
        lsr.l d3,d7            /* Ergebnis wieder zurueck schieben, damit die Testbits auf bit 1 liegen */

        or.b d7,d0             /* Bit 0 Byte 1 -> bit 0 Byte 1 */

        lsr.l #7,d7            /* Bit 0 Byte 2 -> bit 1 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 3 -> bit 2 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 4 -> bit 3 Byte 1 */
        or.b d7,d0

        move.b d0,(a1)+         /* Byte schreiben und eins weiter */


        clr.w d0

        /* Bits 4-7 im Zielbyte zusammenbasteln */
        
        move.l (a0)+,d7        /* long holen 5 */
        and.l d4,d7            /* je 1 bit pro byte ausmaskieren 2 */
        lsr.l d3,d7            /* Ergebnis wieder zurueck schieben, damit die Testbits auf bit 1 liegen 6 */

        lsl.l #4,d7            /* bit 0 Byte 1 -> Bit 4 byte 1 4 */
        or.b d7,d0             /* 2 */

        lsr.l #7,d7            /* Bit 0 Byte 2 -> bit 5 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* Bit 0 Byte 3 -> bit 6 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* Bit 0 Byte 4 -> bit 7 Byte 1 */
        or.b d7,d0

        /* Bits 0-3 im Zielbyte zusammenbasteln */
        
        move.l (a0)+,d7        /* long holen */
        and.l d4,d7            /* je 1 bit pro byte ausmaskieren */
        lsr.l d3,d7            /* Ergebnis wieder zurueck schieben, damit die Testbits auf bit 1 liegen */

        or.b d7,d0             /* Bit 0 Byte 1 -> bit 0 Byte 1 */

        lsr.l #7,d7            /* Bit 0 Byte 2 -> bit 1 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 3 -> bit 2 Byte 1 */
        or.b d7,d0

        lsr.l #7,d7            /* bit 0 Byte 4 -> bit 3 Byte 1 */
        or.b d7,d0

        move.b d0,(a1)+         /* Byte schreiben und eins weiter */

        /*-------------------------zum Schleifenanfang */
    dbra d2,looppix16

    move.w d5,d2               /* howmany zurÅck */
    suba.l d6,a0               /* pxl-tab zum Anfang */
    adda.l d1,a1               /* nÑchste Plane */

    addq.w #1,d3               /* getestetes Bit ++ */
    lsl.l #1,d4                /* Bitmaske verschieben */

    sub.w #1,planeCount
    bpl looplane16

    move.w return_val,d0

    movem.l (sp)+,d3-d6
    rts



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
planeCount: ds.w 1
