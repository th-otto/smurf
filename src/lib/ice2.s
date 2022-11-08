* Pack Ice version 2.40 depack routines, taken from ST Format
* Cover disk 39 (October 1992), added to extension 18/09/1992
* By N.Halliday
*
* Input  : a0 - Address of packed data
*          a1 - Address of unpacked data
* Output : None

	.globl _ice_decrunch
_ice_decrunch:
	move.l  4(a7),a0
	move.l  8(a7),a1

	.globl ice_decrunch
ice_decrunch:
	movem.L	d0-a6,-(sp)
	bsr.s	.getinfo
	cmpi.L	#0x49434521,d0 /* 'ICE!' */
	bne.s	.not_packed
	bsr.s	.getinfo
	lea.L	-8(a0,d0.L),a5
	bsr.s	.getinfo
	move.L	d0,(sp)
	movea.l a1,a4
	movea.l a1,a6
	adda.L	d0,a6
	movea.l a6,a3
	move.b	-(a5),d7
	bsr.w	.normal_bytes
	move.L	a3,a6
	bsr	.get_1_bit
	bcc.s	.no_picture
	move.W	#0x0f9f,d7
	bsr	.get_1_bit
	bcc.s	.ice_00
	moveq	#15,d0
	bsr	.get_d0_bits
	move.W	d1,d7
.ice_00:
	moveq	#3,d6
.ice_01:
	move.W	-(a3),d4
	moveq	#3,d5
.ice_02:
	add.W	d4,d4
	addx.W	d0,d0
	add.W	d4,d4
	addx.W	d1,d1
	add.W	d4,d4
	addx.W	d2,d2
	add.W	d4,d4
	addx.W	d3,d3
	dbra	d5,.ice_02
	dbra	d6,.ice_01
	movem.W	d0-d3,(a3)
	dbra	d7,.ice_00
.no_picture:
.not_packed:
	movem.L	(sp)+,d0-a6
	rts

.getinfo:
	moveq	#3,d1
.getbytes:
	lsl.L	#8,d0
	move.B	(a0)+,d0
	dbf	d1,.getbytes
	rts
.normal_bytes:
	bsr.s	.get_1_bit
	bcc.s	.test_if_end
	moveq.L	#0,d1
	bsr.s	.get_1_bit
	bcc.s	.copy_direkt
	lea.L	.direkt_tab+20(pc),a1
	moveq.L	#4,d3
.nextgb:
	move.L	-(a1),d0
	bsr.s	.get_d0_bits
	swap.W	d0
	cmp.W	d0,d1
	dbne	d3,.nextgb
.no_more:
 add.L	20(a1),d1
.copy_direkt:
	move.B	-(a5),-(a6)
	dbf	d1,.copy_direkt
.test_if_end:
	cmpa.L	a4,a6
	bgt.s	.strings
	rts
.get_1_bit:
	add.B	d7,d7
	bne.s	.Bitfound
	move.B	-(a5),d7
	addx.B	d7,d7
.Bitfound:
	rts
.get_d0_bits:
	moveq.L	#0,d1
.hole_bit_loop:
	add.B	d7,d7
	bne.s	.on_d0
	move.B	-(a5),d7
	addx.B	d7,d7
.on_d0:
	addx.W	d1,d1
	dbf	d0,.hole_bit_loop
	rts
.strings:	lea.L	.Length_tab(pc),a1
	moveq.L	#3,d2
.get_length_bit:
	bsr.s	.get_1_bit
	dbcc	d2,.get_length_bit
.no_length_bit:
	moveq.L	#0,d4
	moveq.L	#0,d1
	move.B	1(a1,d2.W),d0
	ext.W	d0
	bmi.s	.no_ueber
.get_ueber:
	bsr.s	.get_d0_bits
.no_ueber:
	move.B	6(a1,d2.W),d4
	add.W	d1,d4
	beq.s	.get_offset_2
	lea.L	.more_offset(pc),a1
	moveq.L	#1,d2
.getoffs:
	bsr.s	.get_1_bit
	dbcc	d2,.getoffs
	moveq.L	#0,d1
	move.B	1(a1,d2.W),d0
	ext.W	d0
	bsr.s	.get_d0_bits
	add.W	d2,d2
	add.W	6(a1,d2.W),d1
	bpl.s	.depack_bytes
	sub.W	d4,d1
	bra.s	.depack_bytes
.get_offset_2:
	moveq.L	#0,d1
	moveq.L	#5,d0
	moveq.L	#-1,d2
	bsr.s	.get_1_bit
	bcc.s	.Less_40
	moveq.L	#8,d0
	moveq.L	#0x3f,d2
.Less_40:
	bsr.s	.get_d0_bits
	add.W	d2,d1
.depack_bytes:
	lea.L	2(a6,d4.W),a1
	adda.W	d1,a1
	move.B	-(a1),-(a6)
.dep_b:
	move.B	-(a1),-(a6)
	dbf	d4,.dep_b
	bra	.normal_bytes
.direkt_tab:
	dc.L	0x7fff000e,0x00ff0007,0x00070002,0x00030001,0x00030001
	dc.L     270-1,	15-1,	 8-1,	 5-1,	 2-1
.Length_tab:
	dc.B	9,1,0,-1,-1
	dc.B	8,4,2,1,0
.more_offset:
	dc.B	  11,   4,   7,  0	/* Bits lesen */
	dc.W	0x11f,  -1, 0x1f	/* Standard Offset */
ende_ice_decrunch_2:

