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

*
*           FD FAST -   Dither
*
*                   68030 - Code
*
**********  "Die schnellsten Ditherroutinen der Welt".... ********
*
*   Pixelauslesen ist in einer externen Routine, die nach a5
*   gelegt und dann angesprungen wird.
*
*   F�r das Dithern von Standardformat-Quellen verwende ich einen
*   kleinen Trick: Die Ausleseroutine f�r 8 Bit wird auf jeden Fall
*   nach a5 gelegt. Vor der 16er-Schleife werden 16 StF-Pixel ausgelesen
*   und in ein 8bit-Pixelpacked-Format nach bu16 gewandelt. Die 
*   Ausleseroutine kommt dann her und liest aus dem 16-Byte-Puffer buf16
*   Byteweise aus, als w�re es ein normales 8Bit-Bild. Dadurch braucht
*   innerhalb der 16er-Schleife keine Unterscheidung mehr zwischen Standard-
*   oder Pixelpacked-Format gemacht zu werden.
*   Deshalb nennt man mich the Dodger ;-)
*
*   28.04.98: Hehehe, Pixelweise in die Ausleseroutine zu springen war ein
*               Fehler - jetzt existiert eine Zeilenditherroutine pro Quell-
*               farbtiefe und damit sind es nochmal 160% Geschwindigkeit.
*
*   (C) Therapy Seriouz Software, (Olaf Piesche)
*
*
*


	.include "../../../src/ditmacro.s"

/* imports */
	.globl fdd_24bit
	.globl fdd_16bit
	.globl fdd_8bit
	.globl get_standard_pix

	.globl seekcolor
	.globl red
	.globl grn
	.globl blu
	.globl max_col
	.globl line_ready
	.globl scalex_inc


	.globl read_ready

	.globl Palette
	.globl rgbtab


/* gcc cdecl entry point */
	.IFEQ PURE_C
	.globl  _floyd1624
_floyd1624:
	move.l 4(a7),a0
	.ENDC

	.globl floyd1624
floyd1624:
movem.l d2-d7/a2-a6,-(sp)


move.l  (a0)+,a1            /* Clip-Table */
move.l  (a0)+,a2            /* NC-table */
move.l  (a0)+,a3            /* plane-table */
move.l  (a0)+,a4            /* pxl-table */
move.l  (a0)+,rgbtab        /* Palettenbuffer */
move.l  (a0)+,plen          /* Planel�nge */
move.l  (a0)+,paddr         /* zielblock */
move.l  (a0)+,d0            /* width */
move.l  (a0)+,d1            /* height */
move.l  (a0)+,bips          /* Ziel-bitplanes */
move.l  (a0)+,busybox       /* busybox-Funktion */
move.l  (a0)+,d5            /* Farbtiefe des BILDES(!) in planes */
move.l  (a0)+,Palette       /* char *palette: -r-g-b-r-g-b-... je 8Bit */
move.l  (a0)+,SrcPlanelen   /* Standardformat-Flag und ggfs. Source-Planelength */
move.l  (a0)+,d2            /* Zoomfaktor */
move.l  (a0)+,set_pixels    /* setpix-Routine */
move.l  (a0)+,d3            /* not_in_nct */
    move.w  d3,not_in_nct
move.l  (a0)+,red           /* 15Bit-R-Palettenwerte */
move.l  (a0)+,grn           /* 15Bit-R-Palettenwerte */
move.l  (a0)+,blu           /* 15Bit-R-Palettenwerte */
move.l  (a0)+,d3
    move.w d3,destwidth     /* Zielblock-Breite */
move.l  (a0)+,d3
    move.w d3,destheight    /* Zielblock-H�he */
move.l  (a0)+,a0            /* picdata-memblock */


    move.w d2,zoom_faktor
    move.b d5,Depth
    move.w d0,orig_width


    /*---------- Ziel-Farbanzahl ausrechnen */
    moveq.l #1,d3
    move.l bips(pc),d5
    lsl.w   d5,d3
    clr.l d5
    move.b Depth,d5
    move.w d3,max_col


    move.l  a4,pixtab      /* Zielbuffer (PP) f�r geditherte Zeile */


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


****************************************************************
*       Einsprungadresse f�r Read-Routine
*       auf a5 legen
****************************************************************
    /* -------------------------Standardformat */
    tst.l SrcPlanelen
    beq.b   _PixelPacked
    lea fdd_8bit(PC),a5
    bra _ok
    
_PixelPacked:           /* 16 Bit? */
    cmpi.b #16,Depth            
    bgt.b   _Bit24
    blt.b   _Bit8
    lea fdd_16bit(PC),a5
    bra _ok
_Bit8:                  /* 8Bit? */
    lea fdd_8bit(PC),a5
    bra _ok
_Bit24:                 /* 24Bit? */
    lea fdd_24bit(PC),a5
    
_ok:
    move.w  d1,-(sp)            /* H�he f�r Schleife retten */




****************************************************************
*                   PicInc - Bytes pro Zeile
****************************************************************
    move.l  a0,oldpic           /*alte Bildadresse speichern */
    move.w  d0,d1               /*width copy */
    move.w  #1,bytes_per_pixel

*------------------------------ f�r Standardformat
    tst.l SrcPlanelen
    beq.b   Picinc8
    add.w   #7,d1
    lsr.w   #3,d1
Picinc8:                /* 8 Bit */
    cmpi.b #8,Depth
    bgt picinc16
    bra picincdone
picinc16:               /* 16 Bit */
    cmpi.b #16,Depth
    bgt picinc24
    lsl.w #1,d1
    move.w  #2,bytes_per_pixel
    bra picincdone
picinc24:               /* 24 Bit */
    muls.w  #3,d1
    move.w  #3,bytes_per_pixel

picincdone:
    move.w  d1,picinc           /*speichern */
    move.l  paddr(pc),-(sp)


    ********************************************************************
    *   Vorbereitung f�r evtl. Scaling
    ********************************************************************
    /*-------------------------- Y-Increment */
    move.w  zoom_faktor(pc),d1
    muls.w  picinc(pc),d1
    move.l  d1,scaley_inc

    /*-------------------------- X-Increment */
    move.w  zoom_faktor(pc),d1
    muls.w  bytes_per_pixel(pc),d1
    move.w  d1,scalex_inc


    /*Breiten- und H�henwerte an Scaling anpassen */

    /*--------------------------  H�he */
    clr.l d1
    clr.l d2
    move.w  destheight(pc),d1
    move.w  zoom_faktor(pc),d2
    add.w   #1,d2
    divu.w  d2,d1
    move.w  d1,destheight
    move.w  d1,4(sp)

    /*--------------------------  Breite */
    clr.l d1
    clr.l d2
    move.w  destwidth(pc),d1
    move.w  zoom_faktor(pc),d2
    add.w   #1,d2
    divu.w  d2,d1
    move.w  d1,d0
    move.w  d0,destwidth            /*speichern */
    
    /* Zeilenl�nge im Zielblock (Bytes) berechnen */
    move.l  plen(pc),d3
    divu.w  destheight(pc),d3
    ext.l   d3
    move.l  d3,DestLineLen     /* Zeilenl�nge (Bytes)im Zielblock */



*******************************************************************
*           Start der y - Schleife
*******************************************************************
loopy:
    move.w 4(sp),d0        /* Y-Wert */
    andi.w #31,d0         /* undieren */
    bne.b nobusy            /* und wenn !=0 dann keine BB. */
    _Busybox
    nobusy:



    /*
     *    Standardformat-Bild: Pixel auslesen
     *   16 Pixel aus a0 werden nach buf16 zu 8bit Pixelpacked gewandelt.
     *   Danach wird die source auf buf16 ausgerichtet, die Farbtiefe
     *   steht auf 8 bit. Dadurch merkt die eigentliche Ditherroutine nix
     *   davon, ob sie ein Standardformatbild dithert oder nicht
     */
    tst.l SrcPlanelen
    beq no_standard_format
    movem.l d0-d1/a0-a1,-(sp)      /* Register retten */

    move.w  orig_width(pc),d3          /* ungezoomte Breite */
    add.w   #15,d3                  /* + Rand */
    lsr.w   #4,d3                  /* -> Anzahl 16er-Bl�cke */
    subq.w  #1,d3
    
    move.l  SrcPlanelen(pc),d1
    movea.l pixtab(pc),a1           /* Ziel auf pixtab ausrichten */
    
    getst_loop:
        move.b  STFDepth,d0            /* Tiefe nach d0 */
        bsr get_standard_pix
        adda.l  #2,a0                  /* 16 Pixel im Quellbild weitergehen... */
    dbra    d3,getst_loop

    movem.l (sp)+,d0-d1/a0-a1      /* Register restore */
    
    movea.l pixtab(pc),a0          /*Source auf buf16 ausrichten */
no_standard_format:

    
    /*
     *   Vorbereitung f�r 1 Zeile Dither */
     */
    moveq.l #0,d2              /* Farbregister l�schen */
    moveq.l #0,d3
    moveq.l #0,d4
    moveq.l #0,d7              /* Fehlerregister l�schen */
    moveq.l #0,d6
    moveq.l #0,d5
    
    move.l  rgbtab(pc),a6           /* Palette (3*512 Bytes) */

    move.w  destwidth(pc),d0            /*width counter init */
    subq.w  #1,d0
    move.w  not_in_nct(pc),d1


    clr.l d7
    
    jmp (a5)                /* Zeile Dithern */
line_ready:


*
*   Und jetzt mu� die Zeile ins Standardformat gewandelt werden.
*   
    movem.l a1-a3,-(sp)
    
    move.l  bips(pc),d0    /* Farbtiefe */
    move.l  plen(pc),d1    /* Planelength */
    move.w  destwidth(pc),d2
    move.l  pixtab(pc),a0
    move.l  paddr(pc),a1
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
    movem.l (sp)+,a1-a3

    move.l  DestLineLen(pc),d0
    add.l   d0,paddr

    movea.l pixtab(pc),a4      /*pixtab zum Anfang */

    move.l  oldpic(pc),a0      /* alte Position restoren,... */
    adda.w  picinc(pc),a0      /* ...n�chste Zeile hernehmen... */
    add.l   scaley_inc(pc),a0  /* ...Y-Skalieren... */
    move.l  a0,oldpic      /* ...und als alte Pos speichern */

    subq.w  #1,4(sp)
    bgt.w   loopy           /**********loopy Ende */



    *         Ende - Stack und Register restaurieren
    *                   und R�cksprung
    addq.w  #6,sp
    movem.l (sp)+,d2-d7/a2-a6
    rts




*******************************************************
*******************************************************
*                   BSS Segment
*                   alle Symbole
*******************************************************
*******************************************************
.EVEN
scaley_inc:     ds.l    1
scalex_inc:     ds.w    1

paddr:      ds.l    1
rgbtab:     ds.l    1

picinc:     ds.w    1
oldpic:     ds.l    1
plen:       ds.l    1
bips:       ds.l    1
Palette:    ds.l    1
busybox:    ds.l    1
width:      ds.w    1
height:     ds.w    1
orig_width: ds.w    1
Depth:      ds.b    1
.EVEN
max_col:    ds.w    1
not_in_nct: ds.w    1
red:        ds.l    1
grn:        ds.l    1
blu:        ds.l    1

pixtab:     ds.l    1

zoom_faktor:    ds.w    1

bytes_per_pixel:    ds.w    1

SrcPlanelen:    ds.l    1
DestLineLen:    ds.l    1
STFDepth:       ds.b    1
.EVEN
set_pixels:     ds.l    1
destwidth:      ds.w    1
destheight:     ds.w    1
