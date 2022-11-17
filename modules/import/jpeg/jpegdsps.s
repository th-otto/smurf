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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =========================================================*/
/*			JPEG File Interchange Format Decoder			*/
/* DSP version, assembler part                              */
/* =========================================================*/

START_DRIVER = 0

	.text

/*
 * external references
 */
	.globl jpgd_struct
	.globl InPointer
	.globl InSize

/*
 * our globals
 */
	.globl pixel_width
pixel_width: .dc.w 0
	.globl pixel_wdwidth
pixel_wdwidth: .dc.w 0
	.globl pixel_height
pixel_height: .dc.w 0
	.globl image_size
image_size: .dc.l 0
	.globl pic_data
pic_data: .dc.l 0
jpgd_present: .dc.b 0
dsp_present: .dc.b 0

	.globl jpg_convert
jpg_convert:
		movem.l    d2-d7/a2-a6,-(a7)
		move.l     a0,InPointer
		move.l     d0,InSize
		clr.l      pic_data
		clr.l      jpgd_driver
		sf         jpgd_present
jpg_convert1:
		pea.l      find_jpgd(pc)
		move.w     #38,-(a7) /* Supexec */
		trap       #14
		addq.l     #6,a7
		tst.l      jpgd_driver
		bne.s      jpg_convert3
	.IFNE START_DRIVER
		tst.b      jpgd_present
		bne.s      jpg_convert2
		sf         dsp_present
		pea.l      find_snd(pc)
		move.w     #38,-(a7) /* Supexec */
		trap       #14
		addq.l     #6,a7
		tst.b      dsp_present(pc) /* 68020+ only */
		beq.s      jpg_convert2
		st         jpgd_present
	.globl start_driver
		jsr        start_driver(pc)
		bra.s      jpg_convert1
	.ENDC
jpg_convert2:
		moveq.l    #0,d0
		movem.l    (a7)+,d2-d7/a2-a6
		rts
jpg_convert3:
		pea.l      run_driver(pc)
		move.w     #38,-(a7) /* Supexec */
		trap       #14
		addq.l     #6,a7
		moveq.l    #-1,d0
		movem.l    (a7)+,d2-d7/a2-a6
		rts

find_snd:
		movea.l    (0x000005A0).w,a0
find_snd1:
		tst.l      (a0)
		beq.s      find_snd2
		cmpi.l     #0x5F534E44,(a0) /* '_SND' */
		beq.s      find_snd3
		addq.l     #8,a0
		bra.s      find_snd1
find_snd2:
		rts
find_snd3:
		move.l     4(a0),d0
		btst       #3,d0
		sne        dsp_present
		rts

run_driver:
		lea.l      jpgd_struct(pc),a0
		moveq.l    #1,d0 /* JPGDOpenDriver */
		bsr        call_driver
		tst.l      d0
		lea.l      jpgd_struct(pc),a0
		move.l     InPointer(pc),ZERO(a0)
		move.l     InSize(pc),8(a0)
		move.w     #3,18(a0) /* OutComponents */
		move.w     #3,20(a0) /* OutPixelSize */
		move.w     #(8*3)/2,mfdb_div
		bsr.w      init_output
		moveq.l    #4,d0 /* JPGDGetImageInfo */
		bsr        call_driver
		move.w     86(a0),d0 /* MFDBPixelWidth */
		move.w     d0,pixel_width
		add.w      #15,d0
		andi.w     #-16,d0
		move.w     d0,pixel_wdwidth
		move.w     88(a0),d1 /* MFDBPixelHeight */
		move.w     d1,pixel_height
		mulu.w     d0,d1
		moveq.l    #3,d0
		mulu.l     d0,d1 /* 68020+ only */
		move.l     d1,image_size
		movem.l    d2-d7/a0-a6,-(a7)
		move.l     d1,-(a7)
		move.w     #72,-(a7) /* Malloc */
		trap       #1
		addq.l     #6,a7
		movem.l    (a7)+,d2-d7/a0-a6
		tst.l      d0
		beq.s      run_driver1
		move.l     d0,pic_data
		move.l     d0,OutPtr
		moveq.l    #5,d0 /* JPGDGetImageSize */
		bsr.w      call_driver
		moveq.l    #6,d0 /* JPGDDecodeImage */
		bsr.w      call_driver
		tst.l      d0
run_driver1:
		lea.l      jpgd_struct(pc),a0
		moveq.l    #2,d0 /* JPGDCloseDriver */
		bsr.w      call_driver
		tst.l      d0
		rts

init_output:
		move.w     #-1,22(a0) /* OutFlag */
		move.l     #jpg_create,28(a0) /* Create */
		move.l     #jpg_write,32(a0) /* Write */
		move.l     #jpg_close,36(a0) /* Close */
		move.l     #jpg_sigterm,40(a0) /* SigTerm */
		move.l     #jpg_user,60(a0) /* UserRoutine */
		rts

find_jpgd:
		movea.l    (0x000005A0).w,a0
find_jpgd1:
		tst.l      (a0)
		beq.s      find_jpgd2
		cmpi.l     #0x5F4A5044,(a0) /* '_JPD' */
		beq.s      find_jpgd3
		addq.l     #8,a0
		bra.s      find_jpgd1
find_jpgd2:
		moveq.l    #-1,d0
		rts
find_jpgd3:
		movea.l    4(a0),a1
		move.l     a1,jpgd_driver
		rts

call_driver:
		movea.l    jpgd_driver,a1
		movea.l    0(a1,d0.w*4),a1 /* 68020+ only */
		jsr        (a1)
		rts

jpg_write:
		movem.l    d1-d7/a0-a6,-(a7)
		movea.l    a0,a6
		movea.l    64(a6),a0 /* OutTmpPointer */
		movea.l    OutPtr,a1
		moveq.l    #0,d0
		moveq.l    #0,d1
		moveq.l    #0,d6
		move.w     90(a6),d6 /* MFDBWordSize */
		divu.w     mfdb_div,d6
		subq.w     #1,d6
		move.w     70(a6),-(a7) /* OutTmpHeight */
jpg_write1:
		move.w     d6,d4
		moveq.l    #1,d7
jpg_write2:
		moveq.l    #8-1,d5 /* copy 8 pixels */
jpg_write3:
		move.b     (a0)+,(a1)+
		move.b     (a0)+,(a1)+
		move.b     (a0)+,(a1)+
		dbf        d5,jpg_write3
		dbf        d4,jpg_write2
		subq.w     #1,(a7)
		bne.s      jpg_write1
		addq.w     #2,a7
		move.l     a1,OutPtr
		moveq.l    #0,d0
		movem.l    (a7)+,d1-d7/a0-a6
		rts

jpg_create:
jpg_close:
jpg_user:
		moveq.l    #0,d0
jpg_sigterm:
		rts

	.data

	.globl jpgd_driver
jpgd_driver: .dc.l 0
mfdb_div: .dc.w 0
	.dc.l 0 /* unused */
OutPtr: .dc.l 1

	.bss

ZERO = 0
