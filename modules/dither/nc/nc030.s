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

*
*
*           Nearest Color   -   Dither
*
* Pixel Packed to Interleaved Bitplane - Transformer
* 13.11.95 Olaf Piesche, Dale Russell 
*
*
*   FÅr 68000 only angepaût am 21.4.96 (Olaf)
*   Auûerdem noch einige Buswaitstates gespart
*   -> 640*480TC -> 16 in 6.6s, Code nur noch 368 Bytes groû
*
*   8, 16 und 24 Bit Quellfarbtiefe verbunden
*
*   Yahoooo! Geschafft! Mit zwei kleinen Tricks (fÅr Auslesen von
*   Standardformat 8 Bit-Rout verwenden und Auslese-Routinen in
*   Subrouts legen) ist der Kram jetzt trotz allen Farbtiefen 
*   UND Standardformatdither schneller als je zuvor...!
*       30.10.96    Olaf
*
*
*                   SYSTEMPALETTE
*                   68000 - Code
*
*
**********  "Die schnellsten Ditherroutinen der Welt".... ********


	.include "../../../src/ditmacro.s"

/* imports */
	.globl get_standard_pix
	.globl read24bit
	.globl read16bit
	.globl read8bit

/* exports */
	.globl Palette
	.globl rgbtab



/* gcc cdecl entry point */
	.IFEQ PURE_C
	.globl  _nearest_color
_nearest_color:
	move.l 4(a7),a0
	.ENDC

	.globl nearest_color
nearest_color:
movem.l d2-d7/a2-a6,-(sp)

move.l  (a0)+,a1                /*normalized-tab */
move.l  (a0)+,a2                /*ac-table */
move.l  (a0)+,a3                /*plane-table */
move.l  (a0)+,a4                /*pxl-table */
move.l  (a0)+,a5                /*divwork */
move.l  (a0)+,rgbtab
move.l  (a0)+,plen          /* PlanelÑnge */
move.l  (a0)+,paddr         /* zielblock */
move.l  (a0)+,d0            /* width */
move.l  (a0)+,d1            /* height */
move.l  (a0)+,bips         /* bitplanes */
move.l  (a0)+,busybox       /* busybox-Funktion */
move.l  (a0)+,d5            /* Farbtiefe in planes */
move.l  (a0)+,Palette       /* char *palette: -r-g-b-r-g-b-... je 8Bit */
move.l  (a0)+,SrcPlanelen   /* Source-PlanelÑnge */
move.l  (a0)+,d2            /* Zoomfaktor */
move.l  (a0)+,set_pixels    /* set pixel - routine */
move.l  (a0)+,d3
move.l  (a0)+,a0            /*picdata-memblock */

    move.w d2,zoom_faktor
    move.b d5,Depth

    lea.l   pixtab(pc),a4      /*Zielbuffer (PP) */

    move.w  #16,pixels_set
    move.w  #42,a5

********************************************************************
*                   Standardformat-Vorbereitung
*   In STFDepth wird die wirkliche Farbtiefe des Bildes einge-
*   tragen, in Depth kommt 8, um die Auslese-Routinen in der
*   16er-Schleife dazu zu bringen, Byteweise aus buf16 auszulesen.
********************************************************************

    tst.l SrcPlanelen
    beq.b   PixelPacked
    
    move.b  d5,STFDepth
    move.b  #8,Depth

PixelPacked:

    move.w d1,height

    sub.l #1,bips


****************************************************************
*       Einsprungadresse fÅr Read-Routine
*       auf a5 legen
****************************************************************
    /* -------------------------Standardformat */
    tst.l SrcPlanelen
    beq.b   _PixelPacked
    lea.l read8bit(PC),a5
    bra _ok
    
_PixelPacked:
    /* ------------------------- 16 Bit */
    cmpi.b #16,Depth            
    bgt.b   _Bit24      
    blt.b   _Bit8       
    lea read16bit(PC),a5
    bra _ok

_Bit8:
    /* ------------------------- 8 Bit */
    lea read8bit(PC),a5
    bra _ok

_Bit24: 
    /* ------------------------- 24 Bit */
    lea read24bit(PC),a5
    
_ok:
    move.w  d1,-(sp)            /* Hîhe fÅr Schleife retten */



****************************************************************
*                   PicInc - Bytes pro Zeile
****************************************************************
    move.l  a0,oldpic           /*alte Bildadresse speichern */
    move.w  d0,d1               /*width copy */
    move.w  #1,bytes_per_pixel

    /*------------------------------ fÅr Standardformat */
    tst.l SrcPlanelen
    beq.b   Picinc8
    add.w   #7,d1
    lsr.w   #3,d1

Picinc8:
    /*------------------------------ fÅr 8 Bit */
    cmpi.b #8,Depth
    bgt picinc16
    bra picincdone


    /*------------------------------ fÅr 16 Bit */
picinc16:
    cmpi.b #16,Depth
    bgt picinc24
    lsl.w #1,d1
    move.w  #2,bytes_per_pixel

    bra picincdone

    /*-------------------------------fÅr 24 Bit */
picinc24:
    muls.w  #3,d1               /**3 */
    move.w  #3,bytes_per_pixel


picincdone:
    move.w  d1,picinc           /*speichern */
    add.w   d3,picinc           /* Skipbytes am Ende jeder Zeile */

    move.l  paddr,-(sp)



********************************************************************
*               Increments fÅr Scaling vorbereiten
********************************************************************
    /*-------------------------- Y-Increment */
    move.w  zoom_faktor(pc),d1
    muls.w  picinc(pc),d1
    move.l  d1,scaley_inc

    /*-------------------------- X-Increment */
    move.w  zoom_faktor(pc),d1
    muls.w  bytes_per_pixel(pc),d1
    move.w  d1,scalex_inc

****************************************************************
*           Breiten- und Hîhenwerte an Scaling anpassen
****************************************************************
    /*--------------------------  Hîhe */
    clr.l d1
    clr.l d2
    move.w  height(pc),d1
    move.w  zoom_faktor(pc),d2
    add.w   #1,d2
    divu.w  d2,d1
    move.w  d1,height
    move.w  d1,4(sp)

    /*--------------------------  Breite */
    clr.l d1
    clr.l d2
    move.w  d0,d1
    move.w  zoom_faktor(pc),d2
    add.w   #1,d2
    divu.w  d2,d1
    move.w  d1,d0



*******************************************************************
*           Breitenvariable auf 16 Bit ausrichten
*******************************************************************
    move.w  d0,width            /*speichern */


*******************************************************************
*******************************************************************
*           Start der y - Schleife
*******************************************************************
*******************************************************************
loopy:
    move.w 4(sp),d0        /* Y-Wert */
    andi.w #31,d0          /* undieren */
    bne.b nobusy            /* und wenn !=0 dann keine BB. */

    _Busybox

    nobusy:

    moveq   #0,d0          /*width counter init */


*********************************************************
*                       X-Loop
*********************************************************
loopx:


    *********************************************************
    *                   Standardformat-Bild 
    *   16 Pixel aus a0 in buf16 nach Packed Pixel wandeln
    *********************************************************
    tst.l SrcPlanelen
    beq no_standard_format


    movem.l d0-d1/a0-a1,-(sp)      /* Register retten */

    lea.l   buf16,a1               /* Ziel auf buf16 ausrichten */
    clr.l   d0
    move.b  STFDepth(pc),d0        /* Tiefe nach d0 */
    move.l  SrcPlanelen(pc),d1     /* Source Plane-LÑnge */
    bsr get_standard_pix           /* ...und auslesen. */

    movem.l (sp)+,d0-d1/a0-a1      /* Register restore */


    adda.l  #2,a0
    move.l  a0,-(sp)       /* alte Source speichern  */
    lea.l   buf16,a0       /*Source auf buf16 ausrichten */

no_standard_format:

    
    
***********************************************
***********************************************
*           Raster fÅr 16 Pixel
***********************************************
***********************************************
    move.l  rgbtab(pc),a6

    moveq   #0,d1           /*16 Pixel */

    clr.l   d2      /* Farbregister lîschen */
    clr.l   d3
    clr.l   d4

loop16:

    **********************
    *   Pixel auslesen
    **********************
    jmp (a5)
    
	.globl read_ready
read_ready:                 /* RÅckkehr aus der Read-Routine */

    ***********************************************
    *       Nearest Color table offset
    *       (16Bit-Wert aus 3 5Bit-Werten machen)
    ***********************************************
    move.w  d3,d5           /*grÅn kopieren */
    move.w  d4,d6           /*rot kopieren */
    ror.w   #6,d6
    lsl.w   #5,d5           /*green<<=5 */
    add.w   d2,d6           /*red+=blue */
    add.w   d5,d6           /*red+=green */

    ***********************************************
    *   Index aus NC-Table holen und BinÑrwert eintragen
    ***********************************************
    clr.w   d7
    move.b  (d6.w,a2),d7          /*Index des Screenwerts (NCT) */
    move.b  (d7.w,a3),(a4)+       /*Screenwert eintragen (Planetable) */

    adda.w  scalex_inc(pc),a0            /* X-Scaling - adda.W ist hier schneller!! (eigenartig.) */


    addq.w  #1,d1      /* 16er-Counter erhîhen */
    addq.w  #1,d0      /* X-Counter erhîhen */

    cmp.w   width(pc),d0
    bge     raushier

    cmp.w   #16,d1
    blt     loop16

raushier:
    move.w  d1,pixels_set


    tst.l SrcPlanelen
    beq no_standard_format2
    move.l (sp)+,a0

no_standard_format2:

    lea.l   pixtab(pc),a4      /*pixtab zum Anfang */
    /*
     ********** <=16 Pixel ins Standardformat setzen 
     */
    /*int set_16pixels(char *buf16, char *dest, int depth, long planelen, int howmany); */
    movem.l d0-d2/a0-a1,-(sp)
    move.l  bips(pc),d0
    addq.l  #1,d0          /* Farbtiefe nach d0 */
    movea.l a4,a0          /* -src-Adresse */
    move.l  d1,d2          /* -Pixelanzahl */
    move.l  plen,d1      /* -Planelength */
    movea.l paddr,a1      /* -dest */
    movea.l set_pixels(pc),a6
    .IFEQ PURE_C
#ifndef __MSHORT__
	move.l d2,-(a7)
	move.l d1,-(a7)
	move.l d0,-(a7)
	move.l a1,-(a7)
	move.l a0,-(a7)
    jsr (a6)
	lea 20(a7),a7
#else
	move.w d2,-(a7)
	move.l d1,-(a7)
	move.w d0,-(a7)
	move.l a1,-(a7)
	move.l a0,-(a7)
    jsr (a6)
	lea 16(a7),a7
#endif
    .ELSE
    jsr (a6)
    .ENDC
    add.l   d0,paddr.l     /* paddr um d0 weiter */
    movem.l (sp)+,d0-d2/a0-a1

    
    /*********************   loopx schlieûen */
    cmp.w   width(pc),d0
    blt loopx

    move.l  oldpic(pc),a0      /* alte Position restoren,... */
    adda.w  picinc(pc),a0      /* ...nÑchste Zeile hernehmen... */
    move.l  a0,oldpic      /* ...und als alte Pos speichern */

    ************ Y-Scaling
    add.l   scaley_inc(pc),a0   
    move.l  a0,oldpic      /* ...und als alte Pos speichern */

    subq.w  #1,4(sp)
    bgt.w   loopy           /*loop schlieûen */



    *******************************************************
    *         Ende - Stack und Register restaurieren
    *                   und RÅcksprung
    *******************************************************
    addq.w  #6,sp
    movem.l (sp)+,d2-d7/a2-a6
    rts







*******************************************************
*******************************************************
*                   BSS Segment
*                   alle Symbole
*******************************************************
*******************************************************
picinc: ds.w    1
width:  ds.w    1
oldpic: ds.l    1
plen:   ds.l    1
paddr:  ds.l    1
rgbtab: ds.l    1
c_grn:  ds.l    1
c_blu:  ds.l    1
bips:   ds.l    1
Palette:    ds.l    1
ColFormat:  ds.l    1
busybox:    ds.l    1
height:     ds.l    1
busymax:    ds.l    1
busycount:  ds.l    1
Depth:      ds.b    1
pixtab:     ds.b    64
pixels_set: ds.w    1

zoom_faktor:    ds.w    1

scaley_counter: ds.l    1
scaley_inc:     ds.l    1

scalex_inc:     ds.w    1

bytes_per_pixel:    ds.w    1

SrcPlanelen:    ds.l    1
buf16:          ds.b    32
STFDepth:       ds.b    1
set_pixels:     ds.l    1
