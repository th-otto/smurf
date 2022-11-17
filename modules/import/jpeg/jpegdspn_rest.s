[00010014] 48e7 1c3c                 movem.l    d3-d5/a2-a5,-(a7)
[00010018] 2448                      movea.l    a0,a2
[0001001a] 2052                      movea.l    (a2),a0
[0001001c] 2868 012e                 movea.l    302(a0),a4
[00010020] 47f9 0001 0144            lea.l      pic_data,a3
[00010026] 0c2c 004a 0006            cmpi.b     #$4A,6(a4)
[0001002c] 6660                      bne.s      $0001008E
[0001002e] 0c2c 0046 0007            cmpi.b     #$46,7(a4)
[00010034] 6658                      bne.s      $0001008E
[00010036] 0c2c 0049 0008            cmpi.b     #$49,8(a4)
[0001003c] 6650                      bne.s      $0001008E
[0001003e] 0c2c 0046 0009            cmpi.b     #$46,9(a4)
[00010044] 6648                      bne.s      $0001008E
[00010046] 43f9 0001 06d8            lea.l      $000106D8,a1 /* "JFIF Format" */
[0001004c] 2052                      movea.l    (a2),a0
[0001004e] 41e8 0104                 lea.l      260(a0),a0
[00010052] 4eb9 0001 031e            jsr        strcpy
[00010058] 41f9 0001 06e4            lea.l      $000106E4,a0 /* "DSP Brainstorming" */
[0001005e] 7043                      moveq.l    #67,d0
[00010060] 226a 0052                 movea.l    82(a2),a1
[00010064] 2269 0004                 movea.l    4(a1),a1
[00010068] 4e91                      jsr        (a1)
[0001006a] 2052                      movea.l    (a2),a0
[0001006c] 2028 0146                 move.l     326(a0),d0
[00010070] 204c                      movea.l    a4,a0
[00010072] 4eb9 0001 014a            jsr        $0001014A
[00010078] 4a40                      tst.w      d0
[0001007a] 6712                      beq.s      $0001008E
[0001007c] 2013                      move.l     (a3),d0
[0001007e] 6614                      bne.s      $00010094
[00010080] 2039 0001 0140            move.l     image_size,d0
[00010086] 6706                      beq.s      $0001008E
[00010088] 70fd                      moveq.l    #-3,d0
[0001008a] 6000 00a8                 bra        $00010134
[0001008e] 70ff                      moveq.l    #-1,d0
[00010090] 6000 00a2                 bra        $00010134
[00010094] 204c                      movea.l    a4,a0
[00010096] 4eb9 0001 05ae            jsr        Mfree
[0001009c] 3039 0001 013c            move.w     pixel_wdwidth,d0
[000100a2] b079 0001 013a            cmp.w      pixel_width,d0
[000100a8] 675a                      beq.s      $00010104
[000100aa] 7000                      moveq.l    #0,d0
[000100ac] 3039 0001 013c            move.w     pixel_wdwidth,d0
[000100b2] 2600                      move.l     d0,d3
[000100b4] d683                      add.l      d3,d3
[000100b6] d680                      add.l      d0,d3
[000100b8] 7000                      moveq.l    #0,d0
[000100ba] 3039 0001 013a            move.w     pixel_width,d0
[000100c0] 2800                      move.l     d0,d4
[000100c2] d884                      add.l      d4,d4
[000100c4] d880                      add.l      d0,d4
[000100c6] 2853                      movea.l    (a3),a4
[000100c8] 2a53                      movea.l    (a3),a5
[000100ca] 4245                      clr.w      d5
[000100cc] 6012                      bra.s      $000100E0
[000100ce] 2004                      move.l     d4,d0
[000100d0] 224c                      movea.l    a4,a1
[000100d2] 204d                      movea.l    a5,a0
[000100d4] 4eb9 0001 0364            jsr        memcpy
[000100da] d9c3                      adda.l     d3,a4
[000100dc] dbc4                      adda.l     d4,a5
[000100de] 5245                      addq.w     #1,d5
[000100e0] ba79 0001 013e            cmp.w      pixel_height,d5
[000100e6] 65e6                      bcs.s      $000100CE
[000100e8] 2004                      move.l     d4,d0
[000100ea] 7200                      moveq.l    #0,d1
[000100ec] 3239 0001 013e            move.w     pixel_height,d1
[000100f2] 4eb9 0001 054e            jsr        _lmul
[000100f8] 2200                      move.l     d0,d1
[000100fa] 2053                      movea.l    (a3),a0
[000100fc] 4240                      clr.w      d0
[000100fe] 4eb9 0001 0598            jsr        MShrink
[00010104] 2052                      movea.l    (a2),a0
[00010106] 3179 0001 013a 0136       move.w     pixel_width,310(a0)
[0001010e] 2052                      movea.l    (a2),a0
[00010110] 3179 0001 013e 0138       move.w     pixel_height,312(a0)
[00010118] 2052                      movea.l    (a2),a0
[0001011a] 317c 0018 013a            move.w     #$0018,314(a0)
[00010120] 2052                      movea.l    (a2),a0
[00010122] 4228 014b                 clr.b      331(a0)
[00010126] 2052                      movea.l    (a2),a0
[00010128] 4228 014a                 clr.b      330(a0)
[0001012c] 2052                      movea.l    (a2),a0
[0001012e] 2153 012e                 move.l     (a3),302(a0)
[00010132] 4240                      clr.w      d0
[00010134] 4cdf 3c38                 movem.l    (a7)+,d3-d5/a2-a5
[00010138] 4e75                      rts

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
11bac: InPointer
11bb0: InSize
*/
