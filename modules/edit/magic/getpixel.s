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
 * The Initial Developer of the Original Code is
 * Bjoern Spruck
 *
 * This code is a module for the programm SMURF developed by
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *         
 * Contributor(s):
 *         
 *
 * ***** END LICENSE BLOCK *****
 */

*		Schnelle Punktinformationsroutinen
*		2, 4, 16, 256 Farben & 16 / 24 Bit TrueColor


.DATA

gp_fak2 : 	dc.l 10

	.BSS
	.globl fuck_tab
fuck_tab:	ds.b	256

	.TEXT

*	öbergabeparameter :
*		A0 - Adresse
*		A1 - max Y
*		D0	max X
*		D1	X
*		D2	Y
*	RÅckgabe in D0
*

	.IFEQ PURE_C
	.globl _get_point2
_get_point2:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_point2
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_point2
get_point2:
	mulu.w	D0,D2
	move.w	D1,D0
	and.l	#0xFFF0,D1	
	and.l	#0xF,D0
	lsr.w	#3,D1	
	add.l	D1,D2
	adda.l	D2,A0	
	moveq.l	#15,D2
	sub.w	D0,D2

	move.w	(A0),D1
	btst.l	D2,D1
	beq.b	.notset2
	moveq.l	#1,D0
	rts
.notset2:
	moveq.l	#0,D0
	rts


	.IFEQ PURE_C
	.globl _get_point2_fak
_get_point2_fak:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_point2_fak
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_point2_fak
get_point2_fak:
	mulu.w	D0,D2
	move.w	D1,D0
	and.l	#0xFFF0,D1	
	and.l	#0xF,D0
	lsr.w	#3,D1	
	add.l	D1,D2
	adda.l	D2,A0	
	moveq.l	#15,D2
	sub.w	D0,D2

	move.w	(A0),D1
	btst.l	D2,D1
	beq.b	.notset2_fak
	move.l	gp_fak2,D0
	rts
.notset2_fak:
	moveq.l	#0,D0
	rts


	.IFEQ PURE_C
	.globl _get_point4
_get_point4:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_point4
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_point4
get_point4:
	move.l	D3,-(sp)

	mulu.w	D0,D2
	move.w	D1,D3
	and.l	#0xFFF0,D1	
	and.l	#0xF,D3
	lsr.w	#3,D1	
	add.l	D1,D2
	adda.l	D2,A0	
	moveq.l	#15,D0
	sub.w	D3,D0
	moveq.l	#0,D3

	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset4
	bset.l	#0,D3
.notset4:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset4_2
	bset.l	#1,D3
.notset4_2:
	move.w	D3,D0

	move.l	(sp)+,D3
	rts


	.IFEQ PURE_C
	.globl _get_point16
_get_point16:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_point16
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_point16
get_point16:
	move.l	D3,-(sp)

	mulu.w	D0,D2
	move.w	D1,D3
	and.l	#0xFFF0,D1	
	and.l	#0xF,D3
	lsr.w	#3,D1	
	add.l	D1,D2
	adda.l	D2,A0	
	moveq.l	#15,D0
	sub.w	D3,D0
	moveq.l	#0,D3

	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset16_1
	bset.l	#0,D3
.notset16_1:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset16_2
	bset.l	#1,D3
.notset16_2:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset16_3
	bset.l	#2,D3
.notset16_3:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset16_4
	bset.l	#3,D3
.notset16_4:
	move.w	D3,D0

	move.l	(sp)+,D3
	rts


	.IFEQ PURE_C
	.globl _get_point256
_get_point256:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_point256
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_point256
get_point256:
	move.l	D3,-(sp)

	mulu.w	D0,D2
	move.w	D1,D3
	and.l	#0xFFF0,D1	
	and.l	#0xF,D3
	lsr.w	#3,D1	
	add.l	D1,D2
	adda.l	D2,A0	
	moveq.l	#15,D0
	sub.w	D3,D0
	moveq.l	#0,D3

	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256
	bset.l	#0,D3
.notset256:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_2
	bset.l	#1,D3
.notset256_2:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_3
	bset.l	#2,D3
.notset256_3:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_4
	bset.l	#3,D3
.notset256_4:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_5
	bset.l	#4,D3
.notset256_5:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_6
	bset.l	#5,D3
.notset256_6:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_7
	bset.l	#6,D3
.notset256_7:
	adda.l	A1,A0
	move.w	(A0),D1
	btst.l	D0,D1
	beq.b	.notset256_8
	bset.l	#7,D3
.notset256_8:
	move.w	D3,D0

	move.l	(sp)+,D3
	rts


	.IFEQ PURE_C
	.globl _get_point256PP
_get_point256PP:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_point256PP
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_point256PP
get_point256PP:
	mulu.w	D0,D2
	adda.l	D2,A0
	lea		fuck_tab(pc),A1
	adda.w	D1,A0
	moveq.l	#0,D0
	move.b	(A0),D0
	move.b	(A1,D0.w),D0 /* Skaliertabelle */
	rts


	.IFEQ PURE_C
	.globl _get_pointTC16
_get_pointTC16:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_pointTC16
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_pointTC16
get_pointTC16: /**** hier wird nur das letzte Byte Åbergeben !!! */
	lsl.w	#1,D0
	mulu.w	D0,D2
	adda.l	D2,A0
	add.w	D1,D1
	adda.w	D1,A0
	moveq.l	#0,D0
	move.w	(A0),D0
*	and.w	#0xFF,D0 /*  !!! nur das letzte Byte */
	rts


	.IFEQ PURE_C
	.globl _get_pointTC24
_get_pointTC24:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_pointTC24
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_pointTC24
get_pointTC24:
	move.w	D0,-(sp)
	lsl.w	#1,D0
	add.w	(sp)+,D0
	
	mulu.w	D0,D2
	adda.l	D2,A0
	move.w	D1,D0
	add.w	D1,D1
	add.w	D0,D1
	adda.w	D1,A0

	moveq.l	#0,D0
	move.b	(A0)+,D0
	lsl.l	#8,D0
	add.b	(A0)+,D0
	lsl.l	#8,D0
	add.b	(A0),D0
	rts


	.IFEQ PURE_C
	.globl _get_pointTC16_2
_get_pointTC16_2:
	move.l d2,-(a7)
	move.l 8(a7),a0
#ifndef __MSHORT__
	move.w 14(a7),d0
	move.w 18(a7),d1
	move.w 22(a7),d2
	move.l 24(a7),a1
#else
	move.w 12(a7),d0
	move.w 14(a7),d1
	move.w 16(a7),d2
	move.l 18(a7),a1
#endif
	bsr.s get_pointTC16_2
	move.l (a7)+,d2
	rts
	.ENDC

	.globl get_pointTC16_2
get_pointTC16_2:	/**** hier werden die FARBEN wie bei 24Bit Åbergeben !!! */
						/**** long = 0,ROT,GRöN,BLAU */
	lsl.w	#1,D0
	mulu.w	D0,D2
	adda.l	D2,A0
	add.w	D1,D1
	adda.w	D1,A0
	move.w	(A0),D2
	clr.l	D0

	move.w	D2,D1
	andi.w	#0x1F,D1 /* Blauwert isolieren */
	or.w	D1,D0

	lsl.l	#2,D2
	move.w	D2,D1
	andi.w	#0x1F00,D1 /* GrÅnwert isolieren */
	or.w	D1,D0

	lsl.l	#3,D2
	andi.l	#0x1F0000,D2 /* Rotwert isolieren */
	or.l	D2,D0

	rts
