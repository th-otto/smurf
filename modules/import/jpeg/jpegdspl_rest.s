[0001003c] 48e7 1c3c                 movem.l    d3-d5/a2-a5,-(a7)
[00010040] 2448                      movea.l    a0,a2
[00010042] 2250                      movea.l    (a0),a1
[00010044] 2869 012e                 movea.l    302(a1),a4
[00010048] 47f9 0001 015c            lea.l      pic_data,a3
[0001004e] 0c2c 004a 0006            cmpi.b     #$4A,6(a4)
[00010054] 665c                      bne.s      $000100B2
[00010056] 0c2c 0046 0007            cmpi.b     #$46,7(a4)
[0001005c] 6654                      bne.s      $000100B2
[0001005e] 0c2c 0049 0008            cmpi.b     #$49,8(a4)
[00010064] 664c                      bne.s      $000100B2
[00010066] 0c2c 0046 0009            cmpi.b     #$46,9(a4)
[0001006c] 6644                      bne.s      $000100B2
[0001006e] 43f9 0001 073e            lea.l      $0001073E,a1 /* "JFIF Format" */
[00010074] 2052                      movea.l    (a2),a0
[00010076] 41e8 0104                 lea.l      260(a0),a0
[0001007a] 6100 02e6                 bsr        strcpy
[0001007e] 41f9 0001 07ef            lea.l      $000107EF,a0 /* "DSP Brainstorming" */
[00010084] 7043                      moveq.l    #67,d0
[00010086] 226a 0052                 movea.l    82(a2),a1
[0001008a] 2269 0004                 movea.l    4(a1),a1
[0001008e] 4e91                      jsr        (a1)
[00010090] 2052                      movea.l    (a2),a0
[00010092] 2028 0146                 move.l     326(a0),d0
[00010096] 204c                      movea.l    a4,a0
[00010098] 6100 00c8                 bsr        $00010162
[0001009c] 4a40                      tst.w      d0
[0001009e] 6712                      beq.s      $000100B2
[000100a0] 2013                      move.l     (a3),d0
[000100a2] 6614                      bne.s      $000100B8
[000100a4] 2239 0001 0158            move.l     image_size,d1
[000100aa] 6706                      beq.s      $000100B2
[000100ac] 70fd                      moveq.l    #-3,d0
[000100ae] 6000 009c                 bra        $0001014C
[000100b2] 70ff                      moveq.l    #-1,d0
[000100b4] 6000 0096                 bra        $0001014C
[000100b8] 204c                      movea.l    a4,a0
[000100ba] 6100 0536                 bsr        Mfree
[000100be] 3039 0001 0154            move.w     pixel_wdwidth,d0
[000100c4] b079 0001 0152            cmp.w      pixel_width,d0
[000100ca] 6750                      beq.s      $0001011C
[000100cc] 7200                      moveq.l    #0,d1
[000100ce] 3200                      move.w     d0,d1
[000100d0] 2601                      move.l     d1,d3
[000100d2] d683                      add.l      d3,d3
[000100d4] d681                      add.l      d1,d3
[000100d6] 7400                      moveq.l    #0,d2
[000100d8] 3439 0001 0152            move.w     pixel_width,d2
[000100de] 2802                      move.l     d2,d4
[000100e0] d884                      add.l      d4,d4
[000100e2] d882                      add.l      d2,d4
[000100e4] 2853                      movea.l    (a3),a4
[000100e6] 2a4c                      movea.l    a4,a5
[000100e8] 4245                      clr.w      d5
[000100ea] 6010                      bra.s      $000100FC
[000100ec] 2004                      move.l     d4,d0
[000100ee] 224c                      movea.l    a4,a1
[000100f0] 204d                      movea.l    a5,a0
[000100f2] 6100 02b4                 bsr        memcpy
[000100f6] d9c3                      adda.l     d3,a4
[000100f8] dbc4                      adda.l     d4,a5
[000100fa] 5245                      addq.w     #1,d5
[000100fc] ba79 0001 0156            cmp.w      pixel_height,d5
[00010102] 65e8                      bcs.s      $000100EC
[00010104] 2004                      move.l     d4,d0
[00010106] 7200                      moveq.l    #0,d1
[00010108] 3239 0001 0156            move.w     pixel_height,d1
[0001010e] 6100 0482                 bsr        _lmul
[00010112] 2200                      move.l     d0,d1
[00010114] 2053                      movea.l    (a3),a0
[00010116] 4240                      clr.w      d0
[00010118] 6100 04c2                 bsr        Mshrink
[0001011c] 2052                      movea.l    (a2),a0
[0001011e] 3179 0001 0152 0136       move.w     pixel_width,310(a0)
[00010126] 2052                      movea.l    (a2),a0
[00010128] 3179 0001 0156 0138       move.w     pixel_height,312(a0)
[00010130] 2052                      movea.l    (a2),a0
[00010132] 317c 0018 013a            move.w     #$0018,314(a0)
[00010138] 2052                      movea.l    (a2),a0
[0001013a] 4228 014b                 clr.b      331(a0)
[0001013e] 2052                      movea.l    (a2),a0
[00010140] 4228 014a                 clr.b      330(a0)
[00010144] 2052                      movea.l    (a2),a0
[00010146] 2153 012e                 move.l     (a3),302(a0)
[0001014a] 4240                      clr.w      d0
[0001014c] 4cdf 3c38                 movem.l    (a7)+,d3-d5/a2-a5
[00010150] 4e75                      rts

START_DRIVER = 1

	.text

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
		beq.s      jpg_convert
		st         jpgd_present
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
		move.l     InPointer(pc),0(a0)
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
		move.l     #jpg_user,60(a0) /* UserRoutine *
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

/*
11cd0: InPointer
11cd4: InSize
*/
