*
* DSPMOD.S
*
*	in_pl
*
*	Sets interupts and plays some music. Supervisor only.
*	May return to user mode after the procedure.
*
* In	a0.l=Module adr.
*	(destroys a lot)
*
*	dein_pl
*
*	Stops playing the music and resets interupts.
*	Supervisor only.
*


* DSP MOD replay routine written by bITmASTER of BSW
* This is source code for PASM/gas

iera            = 0xfffffa07           /* Interrupt-Enable-Register A */
ierb            = 0xfffffa09           /*                           B */
imra            = 0xfffffa13
isra            = 0xfffffa0f
isrb            = 0xfffffa11
tacr            = 0xfffffa19
tbcr            = 0xfffffa1b
tadr            = 0xfffffa1f
tbdr            = 0xfffffa21
tccr            = 0xfffffa1d
tcdr            = 0xfffffa23
aer             = 0xfffffa03
STColor         = 0xffff8240
FColor          = 0xffff9800
d_vbl             = 0x70
timer_int       = 0x0120
timer_c_int     = 0x0114

ym_select       = 0xffff8800
ym_write        = 0xffff8802
ym_read         = 0xffff8800

vbaselow        = 0xffff820d
vbasemid        = 0xffff8203
vbasehigh       = 0xffff8201
vcountlow       = 0xffff8209
vcountmid       = 0xffff8207
vcounthigh      = 0xffff8205
linewid         = 0xffff820f
hscroll         = 0xffff8265

keyctl          = 0xfffffc00
keybd           = 0xfffffc02

DspHost         = 0xffffa200
HostIntVec      = 0x03fc

PCookies        = 0x05a0

d_hop             = 0xffff8a3a
op              = 0xffff8a3b
line_nr         = 0xffff8a3c
mode            = 0xffff8a3c
skew            = 0xffff8a3d
endmask1        = 0xffff8a28
endmask2        = 0xffff8a2a
endmask3        = 0xffff8a2c
x_count         = 0xffff8a36
y_count         = 0xffff8a38
dest_x_inc      = 0xffff8a2e
dest_y_inc      = 0xffff8a30
dest_adr        = 0xffff8a32
src_x_inc       = 0xffff8a20
src_y_inc       = 0xffff8a22
src_adr         = 0xffff8a24

mpx_src         = 0xffff8930
mpx_dst         = 0xffff8932


	.globl _in_pl
_in_pl:
	.globl in_pl
in_pl:
		moveq.l    #0,d0
		jsr        player+28  /* ein */
		pea.l      init
		move.w     #38,-(a7)  /* Supexec */
		trap       #14
		addq.l     #6,a7
		rts

	.globl _dein_pl
_dein_pl:
	.globl dein_pl
dein_pl:
		pea.l      off
		move.w     #38,-(a7) /* Supexec */
		trap       #14
		addq.l     #6,a7
		moveq.l    #2,d0
		jsr        player+28+4
		rts

	.globl _startp
_startp:
	move.l 4(a7),a0
	.globl startp
startp:
		jsr        player+28+8
		rts

	.globl _stopp
_stopp:
	.globl stopp
stopp:
		jsr        player+28+12
		rts

timer_b:
		movem.l    d0-d7/a0-a6,-(a7)
		jsr        player+28+16
		move.w     ticks,d0
		addq.w     #1,d0
		cmp.w      #50,d0
		bne.s      timer_b1
		clr.w      d0
timer_b1:
		move.w     d0,ticks
		movem.l    (a7)+,d0-d7/a0-a6
		bclr       #0,(isra).w
		addq.w     #1,hbls
		rte


init:
                lea     SaveArea,a0
                move.l  timer_int.w,(a0)+
                move.b  tbcr.w,(a0)+
                move.b  tbdr.w,(a0)+
                move.b  #246,tbdr.w
                move.b  #7,tbcr.w
                move.l  #timer_b,timer_int.w
                bset    #0,imra.w
                bset    #0,iera.w
                rts

off:
                bclr    #0,iera.w
                bclr    #0,imra.w
                lea     SaveArea,a0
                move.l  (a0)+,timer_int.w
                move.b  (a0)+,tbcr.w
                move.b  (a0)+,tbdr.w
                rts

waithbl:
		move.l     d0,-(a7)
		move.w     hbls,d0
		addq.w     #3,d0
waithbl1:
		cmp.w      hbls,d0
		bne.s      waithbl1
		move.l     (a7)+,d0
		rts


	.data
ticks: .dc.w 0

player:
	.include "player.s"

	.bss
hbls:           .ds.w 1
SaveArea:       .ds.b 6
