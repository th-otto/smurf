start_imp_module:
[00010564] 48e7 183c                 movem.l    d3-d4/a2-a5,-(a7)
[00010568] 4fef ff4e                 lea.l      -178(a7),a7
[0001056c] 2648                      movea.l    a0,a3
[0001056e] 2f49 00ae                 move.l     a1,174(a7)
[00010572] 42af 00aa                 clr.l      170(a7)
[00010576] 42a7                      clr.l      -(a7)
[00010578] 43f9 0001 5f84            lea.l      $00015F84,a1
[0001057e] 7003                      moveq.l    #3,d0
[00010580] 4eb9 0001 3e0c            jsr        Pexec
[00010586] 584f                      addq.w     #4,a7
[00010588] 2800                      move.l     d0,d4
[0001058a] 4a80                      tst.l      d0
[0001058c] 6a48                      bpl.s      $000105D6
[0001058e] 4bef 0056                 lea.l      86(a7),a5
[00010592] 43f9 0001 5f86            lea.l      $00015F86,a1 "Error loading module!" AL_LOAD_MODULE
[00010598] 204d                      movea.l    a5,a0
[0001059a] 4eb9 0001 3442            jsr        strcpy
[000105a0] 705c                      moveq.l    #92,d0
[000105a2] 204b                      movea.l    a3,a0
[000105a4] 4eb9 0001 3426            jsr        strrchr
[000105aa] 2248                      movea.l    a0,a1
[000105ac] 5249                      addq.w     #1,a1
[000105ae] 204d                      movea.l    a5,a0
[000105b0] 4eb9 0001 33de            jsr        strcat
[000105b6] 42a7                      clr.l      -(a7)
[000105b8] 42a7                      clr.l      -(a7)
[000105ba] 7001                      moveq.l    #1,d0
[000105bc] 93c9                      suba.l     a1,a1
[000105be] 204d                      movea.l    a5,a0
[000105c0] 2679 0001 632c            movea.l    $0001632C,a3 services
[000105c6] 246b 004a                 movea.l    74(a3),a2 f_alert
[000105ca] 4e92                      jsr        (a2)
[000105cc] 504f                      addq.w     #8,a7
[000105ce] 363c ff01                 move.w     #$FF01,d3
[000105d2] 6000 00f4                 bra        $000106C8

[000105d6] 2a44                      movea.l    d4,a5
[000105d8] 204d                      movea.l    a5,a0
[000105da] 4eb9 0001 08f4            jsr        get_proclen
[000105e0] 2800                      move.l     d0,d4
[000105e2] 2204                      move.l     d4,d1
[000105e4] 204d                      movea.l    a5,a0
[000105e6] 4240                      clr.w      d0
[000105e8] 4eb9 0001 3de6            jsr        Mshrink
[000105ee] 2204                      move.l     d4,d1
[000105f0] e189                      lsl.l      #8,d1
[000105f2] 41f5 1800                 lea.l      0(a5,d1.l),a0
[000105f6] 2b48 0004                 move.l     a0,4(a5)
[000105fa] 4a40                      tst.w      d0
[000105fc] 671c                      beq.s      $0001061A
[000105fe] 42a7                      clr.l      -(a7)
[00010600] 42a7                      clr.l      -(a7)
[00010602] 7001                      moveq.l    #1,d0
[00010604] 93c9                      suba.l     a1,a1
[00010606] 41f9 0001 5f9c            lea.l      $00015F9C,a0 "Error adjusting module TPA!" AL_ADJUST_TPA
[0001060c] 2479 0001 632c            movea.l    $0001632C,a2 services
[00010612] 246a 004a                 movea.l    74(a2),a2 f_alert
[00010616] 4e92                      jsr        (a2)
[00010618] 504f                      addq.w     #8,a7
[0001061a] 266d 0008                 movea.l    8(a5),a3
[0001061e] 202b 0008                 move.l     8(a3),d0
[00010622] b0bc 5349 4d44            cmp.l      #$53494D44,d0
[00010628] 6706                      beq.s      $00010630
[0001062a] 70fe                      moveq.l    #-2,d0
[0001062c] 6000 009c                 bra        $000106CA
[00010630] 286b 000c                 movea.l    12(a3),a4
[00010634] 2eaf 00ae                 move.l     174(a7),(a7)
[00010638] 2f79 0001 632c 0052       move.l     $0001632C,82(a7) services
[00010640] 286b 000c                 movea.l    12(a3),a4
[00010644] 2054                      movea.l    (a4),a0
[00010646] 322f 0046                 move.w     70(a7),d1
[0001064a] 302f 002c                 move.w     44(a7),d0
[0001064e] 4eb9 0001 0234            jsr        x10234
[00010654] 42a7                      clr.l      -(a7)
[00010656] 224d                      movea.l    a5,a1
[00010658] 91c8                      suba.l     a0,a0
[0001065a] 7004                      moveq.l    #4,d0
[0001065c] 4eb9 0001 3e0c            jsr        Pexec
[00010662] 584f                      addq.w     #4,a7
[00010664] 4a80                      tst.l      d0
[00010666] 671c                      beq.s      $00010684
[00010668] 42a7                      clr.l      -(a7)
[0001066a] 42a7                      clr.l      -(a7)
[0001066c] 7001                      moveq.l    #1,d0
[0001066e] 93c9                      suba.l     a1,a1
[00010670] 41f9 0001 5fb8            lea.l      $00015FB8,a0 "Error starting module!" AL_START_MODULE
[00010676] 2479 0001 632c            movea.l    $0001632C,a2 services
[0001067c] 246a 004a                 movea.l    74(a2),a2 f_alert
[00010680] 4e92                      jsr        (a2)
[00010682] 504f                      addq.w     #8,a7
[00010684] 41eb 0004                 lea.l      4(a3),a0
[00010688] 2f48 00a6                 move.l     a0,166(a7)
[0001068c] 41d7                      lea.l      (a7),a0
[0001068e] 226f 00a6                 movea.l    166(a7),a1
[00010692] 4e91                      jsr        (a1)
[00010694] 3600                      move.w     d0,d3
[00010696] 2054                      movea.l    (a4),a0
[00010698] 72ff                      moveq.l    #-1,d1
[0001069a] 302f 002c                 move.w     44(a7),d0
[0001069e] 4eb9 0001 0234            jsr        x10234
[000106a4] 42a7                      clr.l      -(a7)
[000106a6] 224d                      movea.l    a5,a1
[000106a8] 206f 00ae                 movea.l    174(a7),a0
[000106ac] 7066                      moveq.l    #102,d0
[000106ae] 4eb9 0001 3e0c            jsr        Pexec
[000106b4] 584f                      addq.w     #4,a7
[000106b6] 206d 002c                 movea.l    44(a5),a0
[000106ba] 4eb9 0001 3dfc            jsr        Mfree
[000106c0] 204d                      movea.l    a5,a0
[000106c2] 4eb9 0001 3dfc            jsr        Mfree
[000106c8] 3003                      move.w     d3,d0
[000106ca] 4fef 00b2                 lea.l      178(a7),a7
[000106ce] 4cdf 3c18                 movem.l    (a7)+,d3-d4/a2-a5
[000106d2] 4e75                      rts

start_edit_module:
[000106d4] 48e7 1c3e                 movem.l    d3-d5/a2-a6,-(a7)
[000106d8] 4fef fff4                 lea.l      -12(a7),a7
[000106dc] 2f48 0008                 move.l     a0,8(a7)
[000106e0] 2849                      movea.l    a1,a4
[000106e2] 3a00                      move.w     d0,d5
[000106e4] 3601                      move.w     d1,d3
[000106e6] 2a6f 0030                 movea.l    48(a7),a5
[000106ea] 47f9 0001 5f84            lea.l      $00015F84,a3
[000106f0] 4a43                      tst.w      d3
[000106f2] 6b06                      bmi.s      $000106FA
[000106f4] b67c 0014                 cmp.w      #$0014,d3
[000106f8] 6f1a                      ble.s      $00010714
[000106fa] 42a7                      clr.l      -(a7)
[000106fc] 42a7                      clr.l      -(a7)
[000106fe] 7001                      moveq.l    #1,d0
[00010700] 93c9                      suba.l     a1,a1
[00010702] 41eb 004b                 lea.l      75(a3),a0 15fcf "Error: invalid module ID!" AL_MODULE_ID
[00010706] 2479 0001 632c            movea.l    $0001632C,a2 services
[0001070c] 246a 004a                 movea.l    74(a2),a2 f_alert
[00010710] 4e92                      jsr        (a2)
[00010712] 504f                      addq.w     #8,a7
[00010714] 200c                      move.l     a4,d0
[00010716] 6600 0082                 bne        $0001079A
[0001071a] 486b 0067                 pea.l      103(a3) 15feb
[0001071e] 43eb 0065                 lea.l      101(a3),a1 15fe9
[00010722] 206f 000c                 movea.l    12(a7),a0
[00010726] 7003                      moveq.l    #3,d0
[00010728] 4eb9 0001 3e0c            jsr        Pexec
[0001072e] 584f                      addq.w     #4,a7
[00010730] 2800                      move.l     d0,d4
[00010732] 4a80                      tst.l      d0
[00010734] 6a22                      bpl.s      $00010758
[00010736] 42a7                      clr.l      -(a7)
[00010738] 42a7                      clr.l      -(a7)
[0001073a] 93c9                      suba.l     a1,a1
[0001073c] 41eb 0068                 lea.l      104(a3),a0 15fec "Error loading modules." AL_LOAD_MODULES
[00010740] 2c79 0001 632c            movea.l    $0001632C,a6 services
[00010746] 2c6e 004a                 movea.l    74(a6),a6 f_alert
[0001074a] 7001                      moveq.l    #1,d0
[0001074c] 4e96                      jsr        (a6)
[0001074e] 504f                      addq.w     #8,a7
[00010750] 3b7c ff01 002c            move.w     #$FF01,44(a5)
[00010756] 6072                      bra.s      $000107CA
[00010758] 2844                      movea.l    d4,a4
[0001075a] 204c                      movea.l    a4,a0
[0001075c] 4eb9 0001 08f4            jsr        get_proclen
[00010762] 2800                      move.l     d0,d4
[00010764] 2204                      move.l     d4,d1
[00010766] 204c                      movea.l    a4,a0
[00010768] 4240                      clr.w      d0
[0001076a] 4eb9 0001 3de6            jsr        Mshrink
[00010770] 4a40                      tst.w      d0
[00010772] 671a                      beq.s      $0001078E
[00010774] 42a7                      clr.l      -(a7)
[00010776] 42a7                      clr.l      -(a7)
[00010778] 7001                      moveq.l    #1,d0
[0001077a] 93c9                      suba.l     a1,a1
[0001077c] 41eb 007f                 lea.l      127(a3),a0 16003 "Error adjusting module TPA!" AL_ADJUST_TPA
[00010780] 2c79 0001 632c            movea.l    $0001632C,a6 services
[00010786] 2c6e 004a                 movea.l    74(a6),a6 f_alert
[0001078a] 4e96                      jsr        (a6)
[0001078c] 504f                      addq.w     #8,a7
[0001078e] 2004                      move.l     d4,d0
[00010790] e188                      lsl.l      #8,d0
[00010792] 41f4 0800                 lea.l      0(a4,d0.l),a0
[00010796] 2948 0004                 move.l     a0,4(a4)
[0001079a] 200c                      move.l     a4,d0
[0001079c] 6700 014a                 beq        $000108E8
[000107a0] 246c 0008                 movea.l    8(a4),a2
[000107a4] 202a 0008                 move.l     8(a2),d0
[000107a8] b0bc 5345 4d44            cmp.l      #$53454D44,d0
[000107ae] 6720                      beq.s      $000107D0
[000107b0] 42a7                      clr.l      -(a7)
[000107b2] 42a7                      clr.l      -(a7)
[000107b4] 7001                      moveq.l    #1,d0
[000107b6] 93c9                      suba.l     a1,a1
[000107b8] 41eb 009b                 lea.l      155(a3),a0 1601f "Error loading module!"
[000107bc] 2c79 0001 632c            movea.l    $0001632C,a6 services
[000107c2] 2c6e 004a                 movea.l    74(a6),a6 f_alert
[000107c6] 4e96                      jsr        (a6)
[000107c8] 504f                      addq.w     #8,a7
[000107ca] 91c8                      suba.l     a0,a0
[000107cc] 6000 011c                 bra        $000108EA
[000107d0] 4a45                      tst.w      d5
[000107d2] 6604                      bne.s      $000107D8
[000107d4] 3b43 0046                 move.w     d3,70(a5)
[000107d8] 3b45 002c                 move.w     d5,44(a5)
[000107dc] 2b79 0001 632c 0052       move.l     $0001632C,82(a5) services
[000107e4] ba7c 0002                 cmp.w      #$0002,d5
[000107e8] 670c                      beq.s      $000107F6
[000107ea] ba7c 0012                 cmp.w      #$0012,d5
[000107ee] 6706                      beq.s      $000107F6
[000107f0] ba7c 00ff                 cmp.w      #$00FF,d5
[000107f4] 666e                      bne.s      $00010864
[000107f6] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[000107fc] 2268 00e0                 movea.l    224(a0),a1
[00010800] 3b51 0042                 move.w     (a1),66(a5)
[00010804] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[0001080a] 2268 00e4                 movea.l    228(a0),a1
[0001080e] 3b51 0044                 move.w     (a1),68(a5)
[00010812] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[00010818] 2268 00ec                 movea.l    236(a0),a1
[0001081c] 3b51 0048                 move.w     (a1),72(a5)
[00010820] ba7c 00ff                 cmp.w      #$00FF,d5
[00010824] 670e                      beq.s      $00010834
[00010826] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[0001082c] 2268 00f0                 movea.l    240(a0),a1
[00010830] 3b51 002e                 move.w     (a1),46(a5)
[00010834] ba7c 0012                 cmp.w      #$0012,d5
[00010838] 662a                      bne.s      $00010864
[0001083a] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[00010840] 2268 00f4                 movea.l    244(a0),a1
[00010844] 3b51 0030                 move.w     (a1),48(a5)
[00010848] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[0001084e] 2268 00f8                 movea.l    248(a0),a1
[00010852] 3b51 0032                 move.w     (a1),50(a5)
[00010856] 2079 0001 6420            movea.l    $00016420,a0 smurf_vars
[0001085c] 2268 00fc                 movea.l    252(a0),a1
[00010860] 3b51 0034                 move.w     (a1),52(a5)
[00010864] 246c 0008                 movea.l    8(a4),a2
[00010868] 41ea 0004                 lea.l      4(a2),a0
[0001086c] 2f48 0004                 move.l     a0,4(a7)
[00010870] ba7c 0006                 cmp.w      #$0006,d5
[00010874] 6772                      beq.s      $000108E8
[00010876] ba7c 0003                 cmp.w      #$0003,d5
[0001087a] 660a                      bne.s      $00010886
[0001087c] 7002                      moveq.l    #2,d0
[0001087e] 2057                      movea.l    (a7),a0
[00010880] 4eb9 0001 3e7e            jsr        graf_mouse
[00010886] 246a 000c                 movea.l    12(a2),a2
[0001088a] 2052                      movea.l    (a2),a0
[0001088c] 322d 0046                 move.w     70(a5),d1
[00010890] 302d 002c                 move.w     44(a5),d0
[00010894] 4eb9 0001 0234            jsr        x10234
[0001089a] 42a7                      clr.l      -(a7)
[0001089c] 224c                      movea.l    a4,a1
[0001089e] 91c8                      suba.l     a0,a0
[000108a0] 7004                      moveq.l    #4,d0
[000108a2] 4eb9 0001 3e0c            jsr        Pexec
[000108a8] 584f                      addq.w     #4,a7
[000108aa] 4a80                      tst.l      d0
[000108ac] 671a                      beq.s      $000108C8
[000108ae] 42a7                      clr.l      -(a7)
[000108b0] 42a7                      clr.l      -(a7)
[000108b2] 7001                      moveq.l    #1,d0
[000108b4] 93c9                      suba.l     a1,a1
[000108b6] 41eb 00b1                 lea.l      177(a3),a0 16035 "Error starting module!" AL_START_MODULE
[000108ba] 2c79 0001 632c            movea.l    $0001632C,a6 services
[000108c0] 2c6e 004a                 movea.l    74(a6),a6 f_alert
[000108c4] 4e96                      jsr        (a6)
[000108c6] 504f                      addq.w     #8,a7
[000108c8] 204d                      movea.l    a5,a0
[000108ca] 226f 0004                 movea.l    4(a7),a1
[000108ce] 4e91                      jsr        (a1)
[000108d0] 2052                      movea.l    (a2),a0
[000108d2] 72ff                      moveq.l    #-1,d1
[000108d4] 302d 002c                 move.w     44(a5),d0
[000108d8] 4eb9 0001 0234            jsr        x10234
[000108de] 2057                      movea.l    (a7),a0
[000108e0] 4240                      clr.w      d0
[000108e2] 4eb9 0001 3e7e            jsr        graf_mouse
[000108e8] 204c                      movea.l    a4,a0
[000108ea] 4fef 000c                 lea.l      12(a7),a7
[000108ee] 4cdf 7c38                 movem.l    (a7)+,d3-d5/a2-a6
[000108f2] 4e75                      rts

get_proclen:
[000108f4] 2f03                      move.l     d3,-(a7)
[000108f6] 2028 000c                 move.l     12(a0),d0
[000108fa] 2228 001c                 move.l     28(a0),d1
[000108fe] 2428 0014                 move.l     20(a0),d2
[00010902] 2600                      move.l     d0,d3
[00010904] d681                      add.l      d1,d3
[00010906] d682                      add.l      d2,d3
[00010908] d6bc 0000 04e9            add.l      #$000004E9,d3
[0001090e] 2403                      move.l     d3,d2
[00010910] 2002                      move.l     d2,d0
[00010912] 261f                      move.l     (a7)+,d3
[00010914] 4e75                      rts


	.data
[00013ec8]                           dc.b $00
[00013ec9]                           dc.b $00

[00015a74]                           dc.b 'Smurf ModComm-Protokoll Plugin',0
[00015a93]                           dc.b $00

plugin_info:
[00015a94] 00015a74                  dc.l $00015a74
[00015a98]                           dc.w $0100
[00015a9a]                           dc.w $0106
[00015a9c]                           dc.b $00
[00015a9d]                           dc.b $00

[00015a9e]                           dc.b 'MTERM',0
[00015aa4]                           dc.b $00
[00015aa5]                           dc.b $00
[00015aa6]                           dc.b $00
[00015aa7]                           dc.b $00
[00015aa8]                           dc.b $00
[00015aa9]                           dc.b $00
[00015aaa]                           dc.b $00
[00015aab]                           dc.b $00
[00015aac]                           dc.b $00
[00015aad]                           dc.b $00
[00015aae]                           dc.b 'MSTART',0
[00015ab5]                           dc.b $00
[00015ab6]                           dc.b $00
[00015ab7]                           dc.b $00
[00015ab8]                           dc.b $00
[00015ab9]                           dc.b $00
[00015aba]                           dc.b $00
[00015abb]                           dc.b $00
[00015abc]                           dc.b $00
[00015abd]                           dc.b $00
[00015abe]                           dc.b 'MWRED',0
[00015ac4]                           dc.b $00
[00015ac5]                           dc.b $00
[00015ac6]                           dc.b $00
[00015ac7]                           dc.b $00
[00015ac8]                           dc.b $00
[00015ac9]                           dc.b $00
[00015aca]                           dc.b $00
[00015acb]                           dc.b $00
[00015acc]                           dc.b $00
[00015acd]                           dc.b $00
[00015ace]                           dc.b 'MBEVT',0
[00015ad4]                           dc.b $00
[00015ad5]                           dc.b $00
[00015ad6]                           dc.b $00
[00015ad7]                           dc.b $00
[00015ad8]                           dc.b $00
[00015ad9]                           dc.b $00
[00015ada]                           dc.b $00
[00015adb]                           dc.b $00
[00015adc]                           dc.b $00
[00015add]                           dc.b $00
[00015ade]                           dc.b 'MEXEC',0
[00015ae4]                           dc.b $00
[00015ae5]                           dc.b $00
[00015ae6]                           dc.b $00
[00015ae7]                           dc.b $00
[00015ae8]                           dc.b $00
[00015ae9]                           dc.b $00
[00015aea]                           dc.b $00
[00015aeb]                           dc.b $00
[00015aec]                           dc.b $00
[00015aed]                           dc.b $00
[00015aee]                           dc.b 'MWMOVED',0
[00015af6]                           dc.b $00
[00015af7]                           dc.b $00
[00015af8]                           dc.b $00
[00015af9]                           dc.b $00
[00015afa]                           dc.b $00
[00015afb]                           dc.b $00
[00015afc]                           dc.b $00
[00015afd]                           dc.b $00
[00015afe]                           dc.b 'MNEXTPIC',0
[00015b07]                           dc.b $00
[00015b08]                           dc.b $00
[00015b09]                           dc.b $00
[00015b0a]                           dc.b $00
[00015b0b]                           dc.b $00
[00015b0c]                           dc.b $00
[00015b0d]                           dc.b $00
[00015b0e]                           dc.b 'MQUERY',0
[00015b15]                           dc.b $00
[00015b16]                           dc.b $00
[00015b17]                           dc.b $00
[00015b18]                           dc.b $00
[00015b19]                           dc.b $00
[00015b1a]                           dc.b $00
[00015b1b]                           dc.b $00
[00015b1c]                           dc.b $00
[00015b1d]                           dc.b $00
[00015b1e]                           dc.b 'MPICTURE',0
[00015b27]                           dc.b $00
[00015b28]                           dc.b $00
[00015b29]                           dc.b $00
[00015b2a]                           dc.b $00
[00015b2b]                           dc.b $00
[00015b2c]                           dc.b $00
[00015b2d]                           dc.b $00
[00015b2e]                           dc.b 'MMORE',0
[00015b34]                           dc.b $00
[00015b35]                           dc.b $00
[00015b36]                           dc.b $00
[00015b37]                           dc.b $00
[00015b38]                           dc.b $00
[00015b39]                           dc.b $00
[00015b3a]                           dc.b $00
[00015b3b]                           dc.b $00
[00015b3c]                           dc.b $00
[00015b3d]                           dc.b $00
[00015b3e]                           dc.b 'MEXTEND',0
[00015b46]                           dc.b $00
[00015b47]                           dc.b $00
[00015b48]                           dc.b $00
[00015b49]                           dc.b $00
[00015b4a]                           dc.b $00
[00015b4b]                           dc.b $00
[00015b4c]                           dc.b $00
[00015b4d]                           dc.b $00
[00015b4e]                           dc.b 'MMORECANC',0
[00015b58]                           dc.b $00
[00015b59]                           dc.b $00
[00015b5a]                           dc.b $00
[00015b5b]                           dc.b $00
[00015b5c]                           dc.b $00
[00015b5d]                           dc.b $00
[00015b5e]                           dc.b 'MCOLSYS',0
[00015b66]                           dc.b $00
[00015b67]                           dc.b $00
[00015b68]                           dc.b $00
[00015b69]                           dc.b $00
[00015b6a]                           dc.b $00
[00015b6b]                           dc.b $00
[00015b6c]                           dc.b $00
[00015b6d]                           dc.b $00
[00015b6e]                           dc.b 'MCROSSHAIR',0
[00015b79]                           dc.b $00
[00015b7a]                           dc.b $00
[00015b7b]                           dc.b $00
[00015b7c]                           dc.b $00
[00015b7d]                           dc.b $00
[00015b7e]                           dc.b 'MCH_COORDS',0
[00015b89]                           dc.b $00
[00015b8a]                           dc.b $00
[00015b8b]                           dc.b $00
[00015b8c]                           dc.b $00
[00015b8d]                           dc.b $00
[00015b8e]                           dc.b 'MPICS',0
[00015b94]                           dc.b $00
[00015b95]                           dc.b $00
[00015b96]                           dc.b $00
[00015b97]                           dc.b $00
[00015b98]                           dc.b $00
[00015b99]                           dc.b $00
[00015b9a]                           dc.b $00
[00015b9b]                           dc.b $00
[00015b9c]                           dc.b $00
[00015b9d]                           dc.b $00
[00015b9e]                           dc.b 'MCH_DEFCOO',0
[00015ba9]                           dc.b $00
[00015baa]                           dc.b $00
[00015bab]                           dc.b $00
[00015bac]                           dc.b $00
[00015bad]                           dc.b $00
[00015bae]                           dc.b 'MDITHER_READY',0
[00015bbc]                           dc.b $00
[00015bbd]                           dc.b $00
[00015bbe]                           dc.b 'MPIC_UPDATE',0
[00015bca]                           dc.b $00
[00015bcb]                           dc.b $00
[00015bcc]                           dc.b $00
[00015bcd]                           dc.b $00
[00015bce]                           dc.b 'MKEVT',0
[00015bd4]                           dc.b $00
[00015bd5]                           dc.b $00
[00015bd6]                           dc.b $00
[00015bd7]                           dc.b $00
[00015bd8]                           dc.b $00
[00015bd9]                           dc.b $00
[00015bda]                           dc.b $00
[00015bdb]                           dc.b $00
[00015bdc]                           dc.b $00
[00015bdd]                           dc.b $00
[00015bde]                           dc.b 'DONE',0
[00015be3]                           dc.b $00
[00015be4]                           dc.b $00
[00015be5]                           dc.b $00
[00015be6]                           dc.b $00
[00015be7]                           dc.b $00
[00015be8]                           dc.b $00
[00015be9]                           dc.b $00
[00015bea]                           dc.b $00
[00015beb]                           dc.b $00
[00015bec]                           dc.b $00
[00015bed]                           dc.b $00
[00015bee]                           dc.b 'GETCONFIG',0
[00015bf8]                           dc.b $00
[00015bf9]                           dc.b $00
[00015bfa]                           dc.b $00
[00015bfb]                           dc.b $00
[00015bfc]                           dc.b $00
[00015bfd]                           dc.b $00
[00015bfe]                           dc.b 'CONFIG_TRANSMIT',0

[00015c0e]                           dc.b 'M_CONFIG',0
[00015c17]                           dc.b $00
[00015c18]                           dc.b $00
[00015c19]                           dc.b $00
[00015c1a]                           dc.b $00
[00015c1b]                           dc.b $00
[00015c1c]                           dc.b $00
[00015c1d]                           dc.b $00
[00015c1e]                           dc.b 'M_TERMINATED',0
[00015c2b]                           dc.b $00
[00015c2c]                           dc.b $00
[00015c2d]                           dc.b $00
[00015c2e]                           dc.b 'ALL_MSGS',0
[00015c37]                           dc.b $00
[00015c38]                           dc.b $00
[00015c39]                           dc.b $00
[00015c3a]                           dc.b $00
[00015c3b]                           dc.b $00
[00015c3c]                           dc.b $00
[00015c3d]                           dc.b $00
[00015c3e]                           dc.b 'WINDEVENTS',0
[00015c49]                           dc.b $00
[00015c4a]                           dc.b $00
[00015c4b]                           dc.b $00
[00015c4c]                           dc.b $00
[00015c4d]                           dc.b $00
[00015c4e]                           dc.b 'M_CH_OFF',0
[00015c57]                           dc.b $00
[00015c58]                           dc.b $00
[00015c59]                           dc.b $00
[00015c5a]                           dc.b $00
[00015c5b]                           dc.b $00
[00015c5c]                           dc.b $00
[00015c5d]                           dc.b $00
[00015c5e]                           dc.b 'M_CH_COORDS',0
[00015c6a]                           dc.b $00
[00015c6b]                           dc.b $00
[00015c6c]                           dc.b $00
[00015c6d]                           dc.b $00
[00015c6e]                           dc.b 'M_PICTURE',0
[00015c78]                           dc.b $00
[00015c79]                           dc.b $00
[00015c7a]                           dc.b $00
[00015c7b]                           dc.b $00
[00015c7c]                           dc.b $00
[00015c7d]                           dc.b $00
[00015c7e]                           dc.b 'M_CH_COORDS',0
[00015c8a]                           dc.b $00
[00015c8b]                           dc.b $00
[00015c8c]                           dc.b $00
[00015c8d]                           dc.b $00
[00015c8e]                           dc.b 'M_CROSSHAIR',0
[00015c9a]                           dc.b $00
[00015c9b]                           dc.b $00
[00015c9c]                           dc.b $00
[00015c9d]                           dc.b $00
[00015c9e]                           dc.b 'M_COLSYS',0
[00015ca7]                           dc.b $00
[00015ca8]                           dc.b $00
[00015ca9]                           dc.b $00
[00015caa]                           dc.b $00
[00015cab]                           dc.b $00
[00015cac]                           dc.b $00
[00015cad]                           dc.b $00
[00015cae]                           dc.b 'M_MOREOK',0
[00015cb7]                           dc.b $00
[00015cb8]                           dc.b $00
[00015cb9]                           dc.b $00
[00015cba]                           dc.b $00
[00015cbb]                           dc.b $00
[00015cbc]                           dc.b $00
[00015cbd]                           dc.b $00
[00015cbe]                           dc.b 'M_EXTEND',0
[00015cc7]                           dc.b $00
[00015cc8]                           dc.b $00
[00015cc9]                           dc.b $00
[00015cca]                           dc.b $00
[00015ccb]                           dc.b $00
[00015ccc]                           dc.b $00
[00015ccd]                           dc.b $00
[00015cce]                           dc.b 'M_STARTED',0
[00015cd8]                           dc.b $00
[00015cd9]                           dc.b $00
[00015cda]                           dc.b $00
[00015cdb]                           dc.b $00
[00015cdc]                           dc.b $00
[00015cdd]                           dc.b $00
[00015cde]                           dc.b 'M_MODPIC',0
[00015ce7]                           dc.b $00
[00015ce8]                           dc.b $00
[00015ce9]                           dc.b $00
[00015cea]                           dc.b $00
[00015ceb]                           dc.b $00
[00015cec]                           dc.b $00
[00015ced]                           dc.b $00
[00015cee]                           dc.b 'M_UNKNOWN_TYPE',0
[00015cfd]                           dc.b $00
[00015cfe]                           dc.b 'M_PICERR',0
[00015d07]                           dc.b $00
[00015d08]                           dc.b $00
[00015d09]                           dc.b $00
[00015d0a]                           dc.b $00
[00015d0b]                           dc.b $00
[00015d0c]                           dc.b $00
[00015d0d]                           dc.b $00
[00015d0e]                           dc.b 'M_PALERR',0
[00015d17]                           dc.b $00
[00015d18]                           dc.b $00
[00015d19]                           dc.b $00
[00015d1a]                           dc.b $00
[00015d1b]                           dc.b $00
[00015d1c]                           dc.b $00
[00015d1d]                           dc.b $00
[00015d1e]                           dc.b 'M_DONEEXIT',0
[00015d29]                           dc.b $00
[00015d2a]                           dc.b $00
[00015d2b]                           dc.b $00
[00015d2c]                           dc.b $00
[00015d2d]                           dc.b $00
[00015d2e]                           dc.b 'M_EXIT',0
[00015d35]                           dc.b $00
[00015d36]                           dc.b $00
[00015d37]                           dc.b $00
[00015d38]                           dc.b $00
[00015d39]                           dc.b $00
[00015d3a]                           dc.b $00
[00015d3b]                           dc.b $00
[00015d3c]                           dc.b $00
[00015d3d]                           dc.b $00
[00015d3e]                           dc.b 'M_IOERROR',0
[00015d48]                           dc.b $00
[00015d49]                           dc.b $00
[00015d4a]                           dc.b $00
[00015d4b]                           dc.b $00
[00015d4c]                           dc.b $00
[00015d4d]                           dc.b $00
[00015d4e]                           dc.b 'M_MEMORY',0
[00015d57]                           dc.b $00
[00015d58]                           dc.b $00
[00015d59]                           dc.b $00
[00015d5a]                           dc.b $00
[00015d5b]                           dc.b $00
[00015d5c]                           dc.b $00
[00015d5d]                           dc.b $00
[00015d5e]                           dc.b 'M_MODERR',0
[00015d67]                           dc.b $00
[00015d68]                           dc.b $00
[00015d69]                           dc.b $00
[00015d6a]                           dc.b $00
[00015d6b]                           dc.b $00
[00015d6c]                           dc.b $00
[00015d6d]                           dc.b $00
[00015d6e]                           dc.b 'M_INVALID',0
[00015d78]                           dc.b $00
[00015d79]                           dc.b $00
[00015d7a]                           dc.b $00
[00015d7b]                           dc.b $00
[00015d7c]                           dc.b $00
[00015d7d]                           dc.b $00
[00015d7e]                           dc.b 'M_PICDONE',0
[00015d88]                           dc.b $00
[00015d89]                           dc.b $00
[00015d8a]                           dc.b $00
[00015d8b]                           dc.b $00
[00015d8c]                           dc.b $00
[00015d8d]                           dc.b $00
[00015d8e]                           dc.b 'M_WAITING',0
[00015d98]                           dc.b $00
[00015d99]                           dc.b $00
[00015d9a]                           dc.b $00
[00015d9b]                           dc.b $00
[00015d9c]                           dc.b $00
[00015d9d]                           dc.b $00

[00015d9e]                           dc.b $00
[00015d9f]                           dc.b $00
[00015da0]                           dc.b $00
[00015da1]                           dc.b $00

[00015e70]                           dc.b 'Mr. Message',0
[00015e7c]                           dc.b '\modcomm.log',0
[00015e89]                           dc.b $77
[00015e8a]                           dc.b $00
[00015e8b]                           dc.b $0a,$0a,'Logfile opened: ',0
[00015e9e]                           dc.w $2573
[00015ea0]                           dc.b $00
[00015ea1]                           dc.b '   Module                          ID     Message',$0a,0
[00015ed4]                           dc.b '--------------------------------------------------',$0a,0
[00015f08]                           dc.b $0a,'Logfile closed: ',0
[00015f1a]                           dc.b '%s',$0a,0
[00015f1e]                           dc.w $2020
[00015f20]                           dc.b $00
[00015f21]                           dc.b $00
[00015f22]                           dc.w $7220
[00015f24]                           dc.b $00
[00015f25]                           dc.b $00
[00015f26]                           dc.b '                                          ',0
[00015f51]                           dc.b '     ',0
[00015f57]                           dc.b '     ',0
[00015f5d]                           dc.b '<no msg>',0
[00015f66]                           dc.b '%s',$0a,0
[00015f6a]                           dc.w $6100
[00015f6c]                           dc.b 'Kommunikationsprotokoll',0
[00015f84]                           dc.b $00
[00015f85]                           dc.b $00
[00015f86]                           dc.b 'Error loading module!',0
[00015f9c]                           dc.b 'Error adjusting module TPA!',0
[00015fb8]                           dc.b 'Error starting module!',0
[00015fcf]                           dc.b 'Error: invalid module ID!',0
[00015fe9]                           dc.b $00
[00015fea]                           dc.b $00
[00015feb]                           dc.b $00
[00015fec]                           dc.b 'Error loading modules.',0
[0001604c]                           dc.b $00
[0001604d]                           dc.b $00
[0001604e]                           dc.b $00
[0001604f]                           dc.b $00
[00016050]                           dc.b $00
[00016051]                           dc.b $00
[00016052]                           dc.b $00
[00016053]                           dc.b $00
[00016054]                           dc.b $00
[00016055]                           dc.b $00
[00016056]                           dc.b $00
[00016057]                           dc.b $00
[00016058]                           dc.b $00
[00016059]                           dc.b $00
[0001605a]                           dc.b $00
[0001605b]                           dc.b $00
[0001605c]                           dc.b $00
[0001605d]                           dc.b $00
[0001605e]                           dc.b $00
[0001605f]                           dc.b $00
[00016060]                           dc.b $00
[00016061]                           dc.b $00
[00016062]                           dc.b $00
[00016063]                           dc.b $00
[00016064]                           dc.b '%s %s %02d %02d:%02d:%02d %4d',$0a,0
[00016083]                           dc.b $00



[00016244] 0001625a                  dc.l $0001625a
[00016248] 0001625a                  dc.l $0001625a
[0001624c] 0001625a                  dc.l $0001625a
[00016250] 000162aa                  dc.l $000162aa
[00016254]                           dc.b $00
[00016255]                           dc.b $00
[00016256]                           dc.w $0100
[00016258]                           dc.b $00
[00016259]                           dc.b $00
[0001625a]                           dc.b $00
[0001625b]                           dc.b $00
[0001625c]                           dc.b $00
[0001625d]                           dc.b $00
[0001625e]                           dc.b $00
[0001625f]                           dc.b $00
[00016260]                           dc.b $00
[00016261]                           dc.b $00
[00016262]                           dc.b $00
[00016263]                           dc.b $00
[00016264]                           dc.b $00
[00016265]                           dc.b $00
[00016266]                           dc.b $00
[00016267]                           dc.b $00
[00016268]                           dc.b $00
[00016269]                           dc.b $00
[0001626a]                           dc.b $00
[0001626b]                           dc.b $00
[0001626c]                           dc.b $00
[0001626d]                           dc.b $00
[0001626e]                           dc.b $00
[0001626f]                           dc.b $00
[00016270]                           dc.b $00
[00016271]                           dc.b $00
[00016272]                           dc.b $00
[00016273]                           dc.b $00
[00016274]                           dc.b $00
[00016275]                           dc.b $00
[00016276]                           dc.b $00
[00016277]                           dc.b $00
[00016278]                           dc.b $00
[00016279]                           dc.b $00
[0001627a]                           dc.b $00
[0001627b]                           dc.b $00
[0001627c]                           dc.b $00
[0001627d]                           dc.b $00
[0001627e]                           dc.b $00
[0001627f]                           dc.b $00
[00016280]                           dc.b $00
[00016281]                           dc.b $00
[00016282]                           dc.b $00
[00016283]                           dc.b $00
[00016284]                           dc.b $00
[00016285]                           dc.b $00
[00016286]                           dc.b $00
[00016287]                           dc.b $00
[00016288]                           dc.b $00
[00016289]                           dc.b $00
[0001628a]                           dc.b $00
[0001628b]                           dc.b $00
[0001628c]                           dc.b $00
[0001628d]                           dc.b $00
[0001628e]                           dc.b $00
[0001628f]                           dc.b $00
[00016290]                           dc.b $00
[00016291]                           dc.b $00
[00016292]                           dc.b $00
[00016293]                           dc.b $00
[00016294]                           dc.b $00
[00016295]                           dc.b $00
[00016296]                           dc.b $00
[00016297]                           dc.b $00
[00016298]                           dc.b $00
[00016299]                           dc.b $00
[0001629a]                           dc.b $00
[0001629b]                           dc.b $00
[0001629c]                           dc.b $00
[0001629d]                           dc.b $00
[0001629e]                           dc.b $00
[0001629f]                           dc.b $00
[000162a0]                           dc.b $00
[000162a1]                           dc.b $00
[000162a2]                           dc.b $00
[000162a3]                           dc.b $00
[000162a4]                           dc.b $00
[000162a5]                           dc.b $00
[000162a6]                           dc.b $00
[000162a7]                           dc.b $00
[000162a8]                           dc.b $00
[000162a9]                           dc.b $00
[000162aa]                           dc.b $00
[000162ab]                           dc.b $00
[000162ac]                           dc.b $00
[000162ad]                           dc.b $00
[000162ae]                           dc.b $00
[000162af]                           dc.b $00
[000162b0]                           dc.b $00
[000162b1]                           dc.b $00
[000162b2]                           dc.b $00
[000162b3]                           dc.b $00
[000162b4]                           dc.b $00
[000162b5]                           dc.b $00
[000162b6]                           dc.b $00
[000162b7]                           dc.b $00
[000162b8]                           dc.b $00
[000162b9]                           dc.b $00
[000162ba]                           dc.b $00
[000162bb]                           dc.b $01
[000162bc]                           dc.w $0600
[000162be]                           dc.b $00
[000162bf]                           dc.b $00
[000162c0]                           dc.b $00
[000162c1]                           dc.b $00
[000162c2]                           dc.b $00
[000162c3]                           dc.b $00
[000162c4]                           dc.b $00
[000162c5]                           dc.b $00
[000162c6]                           dc.b $00
[000162c7]                           dc.b $00
[000162c8]                           dc.b $00
[000162c9]                           dc.b $00
[000162ca]                           dc.b $00
[000162cb]                           dc.b $00
[000162cc]                           dc.b $00
[000162cd]                           dc.b $00
[000162ce]                           dc.b $00
[000162cf]                           dc.b $00
[000162d0]                           dc.w $ffff
[000162d2]                           dc.w $0600
[000162d4]                           dc.b $00
[000162d5]                           dc.b $00
[000162d6]                           dc.b $00
[000162d7]                           dc.b $00
[000162d8]                           dc.b $00
[000162d9]                           dc.b $00
[000162da]                           dc.b $00
[000162db]                           dc.b $00
[000162dc]                           dc.b $00
[000162dd]                           dc.b $00
[000162de]                           dc.b $00
[000162df]                           dc.b $00
[000162e0]                           dc.b $00
[000162e1]                           dc.b $00
[000162e2]                           dc.b $00
[000162e3]                           dc.b $00
[000162e4]                           dc.b $00
[000162e5]                           dc.b $00
[000162e6]                           dc.b $00
[000162e7]                           dc.b $02
[000162e8]                           dc.w $0700
[000162ea]                           dc.b $00
[000162eb]                           dc.b $00
[000162ec]                           dc.b $00
[000162ed]                           dc.b $00
[000162ee]                           dc.b $00
[000162ef]                           dc.b $00
[000162f0]                           dc.b $00
[000162f1]                           dc.b $00
[000162f2]                           dc.b $00
[000162f3]                           dc.b $00
[000162f4]                           dc.b $00
[000162f5]                           dc.b $00
[000162f6]                           dc.b $00
[000162f7]                           dc.b $00
[000162f8]                           dc.b $00
[000162f9]                           dc.b $00
[000162fa]                           dc.b $00
[000162fb]                           dc.b $00
[000162fc]                           dc.b $00
[000162fd]                           dc.b $03
[000162fe]                           dc.w $0600
[00016300]                           dc.b $00
[00016301]                           dc.b $00
[00016302]                           dc.b $00
[00016303]                           dc.b $00
[00016304]                           dc.b $00
[00016305]                           dc.b $00
[00016306]                           dc.b $00
[00016307]                           dc.b $00
[00016308]                           dc.b $00
[00016309]                           dc.b $00
[0001630a]                           dc.b $00
[0001630b]                           dc.b $00
[0001630c]                           dc.b $00
[0001630d]                           dc.b $00
[0001630e]                           dc.b $00
[0001630f]                           dc.b $00

15aae: message_names

15d9e: logfile

16328: FilSysVec
1632c: services
16330: module_ptr
16334: module
16420: smurf_vars
16424: maintree
16428: win
1648e: my_id
16490-16a4e logbuffer
16a4e: redraw_window
16a52: logfile_name
