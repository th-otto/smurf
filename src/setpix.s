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

* **********************************************************
*                   SET_PIXL.S          
* **********************************************************
*
*   EnthÑlt zwei Routinen zum setzen der Pixel ins Standard-
*   und eine fÅr Pixelpacked-Format.
*
* **********************************************************




*---------------------------------------------------
*       16 Pixel ins Standardformat setzen
*       auf 8 bit ausgerichtet!
*   Deklaration:    
*   int setpix_standard(char *buf16, char *dest, int depth, long planelen, int howmany);
*
*   buf16:  Buffer, in dem sich 16 Bytes (Pixel) Pixelpacked befinden
*   dest:   Zielspeicherbereich
*   depth:  Farbtiefe
*   howmany:    Wieviele Pixel sollen gesetzt werden (16>=howmany>=8!)
*   planelen:   LÑnge einer Plane des Zielbildes in Bytes
*
*   return: Anzahl an Bytes, die weitergegangen werden soll (1 oder 2)
*
*   Noch unoptimiert, nur zum austesten
*---------------------------------------------------
	.IFEQ PURE_C
/* gcc cdecl entry point */
	.globl _setpix_standard
_setpix_standard:
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
	bsr.s setpix_standard
	move.l (a7)+,d2
	rts
	.ENDC

	.globl setpix_standard
setpix_standard:
    movem.l d3-d6,-(sp)

    clr.w   d4
    subq.w  #1,d0

    move.w  #16,d5     /* Sprungoffset ausrechnen */
    sub.w   d2,d5
    mulu.w  #8,d5      /* 8 Bytes fÅr die 4 Befehle */

    moveq.l #0,d6
    
looplane:
    clr.w   d3
    bset.l  #15,d6

    jmp 2(pc,d5.w)

    /*-------------------------Schleifenanfang - 16 Pixel einer Plane fertigmachen */
    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_1    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_1:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_2    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_2:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_3    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_3:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_4    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_4:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_5    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_5:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_6    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_6:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_7    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_7:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_8    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_8:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_9    /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_9:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_10   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_10:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_11   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_11:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_12   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_12:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_13   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_13:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_14   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_14:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_15   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_15:
    lsr.w   #1,d6

    btst.b  d4,(a0)+        /* Bit d4 gesetzt? */
    beq.b   setpix_std_16   /* Nein? auch in d3 keins setzen. */
    or.w    d6,d3
setpix_std_16:
    lsr.w   #1,d6

    /*-------------------------zum Schleifenanfang */
    suba.w  d2,a0           /* pxl-tab zum Anfang */

    ror.w   #8,d3          /* obere 8 Bits holen */
    move.b  d3,(a1)         /* Byte schreiben */

    cmp.w   #8,d2
    ble nomorethan8
        rol.w   #8,d3
        move.b  d3,1(a1)            /* Byte schreiben */

nomorethan8:
    adda.l  d1,a1           /* nÑchste Plane */

    addq.w  #1,d4          /* getestetes Bit ++ */

    dbra d0,looplane

    /*----- Returncode schreiben */
    /*   wurden mehr als 8 Pixel geschrieben ist dieser 2, sonst 1 */
    move.w  #1,d0
    cmp.w   #8,d2
    ble.b return_1
    move.w  #2,d0

return_1:
    movem.l (sp)+,d3-d6
    rts
    


*---------------------------------------------------
* 16 Packed-Pixel aus a0 ins Standardformat nach 
* a1 schreiben, in d1 planes auf 16 Bit ausgerichtet
*
*   int setpix_standard_16(char *buf16, char *dest, int depth, long planelen, int howmany);
*
*   return: Anzahl an Bytes, die im Quellbild weitergegangen werden soll
*           (in diesem Fall immer 2)
*---------------------------------------------------
	.IFEQ PURE_C
/* gcc cdecl entry point */
	.globl _setpix_standard
_setpix_standard_16:
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
	bsr.s setpix_standard_16
	move.l (a7)+,d2
	rts
	.ENDC

	.globl setpix_standard_16
setpix_standard_16:
    movem.l     d3-d4/a2,-(sp)     /* save regs */
    subq.w  #1,d0
    moveq   #0,d4
    move.l  a0,a2

loopplane2:
    clr.w d3        /* clear Word to write */

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_1    /* nope! */
    ori.w   #0x8000,d3  /* yep! set d6th Bit in d7! */
setpix_1:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_2    /* nope! */
    ori.w   #0x4000,d3  /* yep! set d6th Bit in d7! */
setpix_2:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_3    /* nope! */
    ori.w   #0x2000,d3  /* yep! set d6th Bit in d7! */
setpix_3:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_4    /* nope! */
    ori.w   #0x1000,d3  /* yep! set d6th Bit in d7! */
setpix_4:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_5    /* nope! */
    ori.w   #0x0800,d3  /* yep! set d6th Bit in d7! */
setpix_5:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_6    /* nope! */
    ori.w   #0x0400,d3  /* yep! set d6th Bit in d7! */
setpix_6:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_7    /* nope! */
    ori.w   #0x0200,d3  /* yep! set d6th Bit in d7! */
setpix_7:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_8    /* nope! */
    ori.w   #0x0100,d3  /* yep! set d6th Bit in d7! */
setpix_8:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_9    /* nope! */
    ori.w   #0x0080,d3  /* yep! set d6th Bit in d7! */
setpix_9:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_10   /* nope! */
    ori.w   #0x0040,d3  /* yep! set d6th Bit in d7! */
setpix_10:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_11   /* nope! */
    ori.w   #0x0020,d3  /* yep! set d6th Bit in d7! */
setpix_11:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_12   /* nope! */
    ori.w   #0x0010,d3  /* yep! set d6th Bit in d7! */
setpix_12:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_13   /* nope! */
    ori.w   #0x0008,d3  /* yep! set d6th Bit in d7! */
setpix_13:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_14   /* nope! */
    ori.w   #0x0004,d3  /* yep! set d6th Bit in d7! */
setpix_14:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_15   /* nope! */
    ori.w   #0x0002,d3  /* yep! set d6th Bit in d7! */
setpix_15:

    btst.b  d4,(a0)+    /* is d2th Bit set in d1th Byte of buffer? */
    beq.b   setpix_16   /* nope! */
    ori.w   #0x0001,d3  /* yep! set d6th Bit in d7! */
setpix_16:

    movea.l a2,a0
    move.w  d3,(a1)     /* write finished word. */
    adda.l d1,a1       /* next plane */
    addq.w #1,d4
    dbra d0,loopplane2     /* do this for #d0 Planes. */

    moveq.l #2,d0          /* return: increment 2 Bytes! */

    movem.l (sp)+,d3-d4/a2
    rts




*---------------------------------------------------
*           16 Pixel pixelpacked setzen
*   Es wird immer von einer Farbtiefe von 8 Bit ausgegangen
*   Deklaration: dieselbe wie setpix_standard
*   int setpix_pp(char *buf16, char *dest, int depth, long planelen, int howmany);
*   Return: Pixelincrement im Quellbild (8Bit -> 16, 4 Bit->8)
*---------------------------------------------------
	.IFEQ PURE_C
/* gcc cdecl entry point */
	.globl _setpix_pp
_setpix_pp:
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
	bsr.s setpix_pp
	move.l (a7)+,d2
	rts
	.ENDC

	.globl setpix_pp
setpix_pp:
    move.w  #16,d1     /* Sprungoffset ausrechnen */
    sub.w   d2,d1
    lsl.w   #1,d1      /* 2 Bytes fÅr den move */

    jmp 2(pc,d1.w)
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+

    move.w  d2,d0
    rts
