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

*		Schnelle Punktkopierroutinen
*	2,4,16,256 Farben & TrueColor

	.text

*	šbergabeparameter :
*		A0 - Zeiger auf folgende Struktur :
*			L	Adresse
*			W	X
*			L	XY
*		A1 - Zeiger auf folgende Struktur :
*			L	Adresse
*			W	X
*			L	XY

	.IFEQ PURE_C
	.globl _fcopypoint2
_fcopypoint2:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypoint2
fcopypoint2:

	movem.l	D2-D3/A2-A3,-(sp)

	movea.l	(A0)+,A2
	move.w	(A0)+,D0
	move.w	D0,D1
	and.w	#0xFFF0,D0	
	and.w	#0xF,D1	
	lsr.w	#3,D0
	adda.w	D0,A2
	moveq.l	#15,D0
	sub.w	D1,D0

	movea.l	(A1)+,A3
	move.w	(A1)+,D2
	move.w	D2,D3
	and.w	#0xFFF0,D2	
	and.w	#0xF,D3	
	lsr.w	#3,D2
	adda.w	D2,A3
	moveq.l	#15,D2
	sub.w	D3,D2

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset
	bset.l	D2,D3
	bra.b	tsch
notset:
	bclr.l	D2,D3
tsch:
	move.w	D3,(A3)

	movem.l	(sp)+,D2-D3/A2-A3
	rts


	.IFEQ PURE_C
	.globl _fcopypoint4
_fcopypoint4:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypoint4
fcopypoint4:
	movem.l	D2-D3/A2-A3,-(sp)

	movea.l	(A0)+,A2
	move.w	(A0)+,D0
	move.w	D0,D1
	and.w	#0xFFF0,D0	
	and.w	#0xF,D1	
	lsr.w	#3,D0
	adda.w	D0,A2
	moveq.l	#15,D0
	sub.w	D1,D0

	movea.l	(A1)+,A3
	move.w	(A1)+,D2
	move.w	D2,D3
	and.w	#0xFFF0,D2	
	and.w	#0xF,D3	
	lsr.w	#3,D2
	adda.w	D2,A3
	moveq.l	#15,D2
	sub.w	D3,D2

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset4_1
	bset.l	D2,D3
	bra.b	tsch4_1
notset4_1:
	bclr.l	D2,D3
tsch4_1:
	move.w	D3,(A3)

	adda.l	(A0),A2
	adda.l	(A1),A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset4_2
	bset.l	D2,D3
	bra.b	tsch4_2
notset4_2:
	bclr.l	D2,D3
tsch4_2:
	move.w	D3,(A3)

	movem.l	(sp)+,D2-D3/A2-A3

	rts


	.IFEQ PURE_C
	.globl _fcopypoint16
_fcopypoint16:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypoint16
fcopypoint16:

	movem.l	D2-D5/A2-A3,-(sp)

	movea.l	(A0)+,A2
	move.w	(A0)+,D0
	move.l	(A0),D4
	move.w	D0,D1
	and.w	#0xFFF0,D0	
	and.w	#0xF,D1	
	lsr.w	#3,D0
	adda.w	D0,A2
	moveq.l	#15,D0
	sub.w	D1,D0

	movea.l	(A1)+,A3
	move.w	(A1)+,D2
	move.l	(A1),D5
	move.w	D2,D3
	and.w	#0xFFF0,D2	
	and.w	#0xF,D3	
	lsr.w	#3,D2
	adda.w	D2,A3
	moveq.l	#15,D2
	sub.w	D3,D2

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset16_1
	bset.l	D2,D3
	bra.b	tsch16_1
notset16_1:
	bclr.l	D2,D3
tsch16_1:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset16_2
	bset.l	D2,D3
	bra.b	tsch16_2
notset16_2:
	bclr.l	D2,D3
tsch16_2:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset16_3
	bset.l	D2,D3
	bra.b	tsch16_3
notset16_3:
	bclr.l	D2,D3
tsch16_3:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset16_4
	bset.l	D2,D3
	bra.b	tsch16_4
notset16_4:
	bclr.l	D2,D3
tsch16_4:
	move.w	D3,(A3)

	movem.l	(sp)+,D2-D5/A2-A3

	rts


	.IFEQ PURE_C
	.globl _fcopypoint256
_fcopypoint256:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypoint256
fcopypoint256:

	movem.l	D2-D5/A2-A3,-(sp)

	movea.l	(A0)+,A2
	move.w	(A0)+,D0
	move.l	(A0),D4
	move.w	D0,D1
	and.w	#0xFFF0,D0	
	and.w	#0xF,D1	
	lsr.w	#3,D0
	adda.w	D0,A2
	moveq.l	#15,D0
	sub.w	D1,D0

	movea.l	(A1)+,A3
	move.w	(A1)+,D2
	move.l	(A1),D5
	move.w	D2,D3
	and.w	#0xFFF0,D2	
	and.w	#0xF,D3	
	lsr.w	#3,D2
	adda.w	D2,A3
	moveq.l	#15,D2
	sub.w	D3,D2

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_1
	bset.l	D2,D3
	bra.b	tsch256_1
notset256_1:
	bclr.l	D2,D3
tsch256_1:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_2
	bset.l	D2,D3
	bra.b	tsch256_2
notset256_2:
	bclr.l	D2,D3
tsch256_2:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_3
	bset.l	D2,D3
	bra.b	tsch256_3
notset256_3:
	bclr.l	D2,D3
tsch256_3:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_4
	bset.l	D2,D3
	bra.b	tsch256_4
notset256_4:
	bclr.l	D2,D3
tsch256_4:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_5
	bset.l	D2,D3
	bra.b	tsch256_5
notset256_5:
	bclr.l	D2,D3
tsch256_5:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_6
	bset.l	D2,D3
	bra.b	tsch256_6
notset256_6:
	bclr.l	D2,D3
tsch256_6:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_7
	bset.l	D2,D3
	bra.b	tsch256_7
notset256_7:
	bclr.l	D2,D3
tsch256_7:
	move.w	D3,(A3)

	adda.l	D4,A2
	adda.l	D5,A3

	move.w	(A2),D1
	move.w	(A3),D3
	btst.l	D0,D1
	beq.b	notset256_8
	bset.l	D2,D3
	bra.b	tsch256_8
notset256_8:
	bclr.l	D2,D3
tsch256_8:
	move.w	D3,(A3)

	movem.l	(sp)+,D2-D5/A2-A3

	rts

	.IFEQ PURE_C
	.globl _fcopypoint256PP
_fcopypoint256PP:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypoint256PP
fcopypoint256PP:
	.IFEQ PURE_C
	move.l d2,-(a7)
	.ENDC
	move.l	(A0)+,D1
	move.w	(A0),D2
	movea.l	(A1)+,A0
	move.w	(A1),D0
	adda.w	D0,A0
	move.l	D1,A1
	adda.w	D2,A1
	move.b	(A1),(A0)
	.IFEQ PURE_C
	move.l (a7)+,d2
	.ENDC
	rts

	.IFEQ PURE_C
	.globl _fcopypointTC16
_fcopypointTC16:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypointTC16
fcopypointTC16:
	.IFEQ PURE_C
	move.l d2,-(a7)
	.ENDC
	move.l	(A0)+,D1
	move.w	(A0),D2
	movea.l	(A1)+,A0
	move.w	(A1),D0
	adda.w	D0,A0
	adda.w	D0,A0
	move.l	D1,A1
	adda.w	D2,A1
	adda.w	D2,A1
	move.w	(A1),(A0)
	.IFEQ PURE_C
	move.l (a7)+,d2
	.ENDC
	rts


	.IFEQ PURE_C
	.globl _fcopypointTC24
_fcopypointTC24:
	move.l 4(a7),a0
	move.l 8(a7),a1
	.ENDC

	.globl fcopypointTC24
fcopypointTC24:
	.IFEQ PURE_C
	move.l d2,-(a7)
	.ENDC
	move.l	(A0)+,D1
	move.w	(A0),D2
	movea.l	(A1)+,A0
	move.w	(A1),D0
	adda.w	D0,A0
	adda.w	D0,A0
	adda.w	D0,A0
	movea.l	D1,A1
	adda.w	D2,A1
	adda.w	D2,A1
	adda.w	D2,A1

	move.b	(A1)+,(A0)+
	move.b	(A1)+,(A0)+
	move.b	(A1),(A0)
	.IFEQ PURE_C
	move.l (a7)+,d2
	.ENDC
	rts
