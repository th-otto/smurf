sed -i 's/ 006a/ 0070/;s/106(/112(/;s/ 0074/ 007a/;s/116(/122(/;s/ 007e/ 0084/;s/126(/132(/;s/ 0088/ 008e/;s/136(/142(/' x.s

do_colrun:
[000106c6] 48e7 1e3c                 movem.l    d3-d6/a2-a5,-(a7)
[000106ca] 4fef ff68                 lea.l      -152(a7),a7
[000106ce] 2f48 006c                 move.l     a0,108(a7)
[000106d2] 2449                      movea.l    a1,a2
[000106d4] 3800                      move.w     d0,d4
[000106d6] 426f 003e                 clr.w      62(a7)
[000106da] 266a 012e                 movea.l    302(a2),a3
[000106de] 3f6a 0136 0002            move.w     310(a2),2(a7)
[000106e4] 3eaa 0138                 move.w     312(a2),(a7)
[000106e8] 4246                      clr.w      d6
[000106ea] 49ef 0040                 lea.l      64(a7),a4
[000106ee] 600a                      bra.s      $000106FA
[000106f0] 3006                      move.w     d6,d0
[000106f2] d040                      add.w      d0,d0
[000106f4] 3986 0000                 move.w     d6,0(a4,d0.w)
[000106f8] 5246                      addq.w     #1,d6
[000106fa] bc7c 0010                 cmp.w      #$0010,d6
[000106fe] 6df0                      blt.s      $000106F0
[00010700] 4246                      clr.w      d6
[00010702] 6048                      bra.s      $0001074C
[00010704] 4240                      clr.w      d0
[00010706] 6036                      bra.s      $0001073E
[00010708] 3200                      move.w     d0,d1
[0001070a] d241                      add.w      d1,d1
[0001070c] 3434 1002                 move.w     2(a4,d1.w),d2
[00010710] 41f9 0001 4257            lea.l      $00014257,a0
[00010716] 1230 2000                 move.b     0(a0,d2.w),d1
[0001071a] 3400                      move.w     d0,d2
[0001071c] d442                      add.w      d2,d2
[0001071e] 3434 2000                 move.w     0(a4,d2.w),d2
[00010722] b230 2000                 cmp.b      0(a0,d2.w),d1
[00010726] 6414                      bcc.s      $0001073C
[00010728] 3202                      move.w     d2,d1
[0001072a] 3400                      move.w     d0,d2
[0001072c] d442                      add.w      d2,d2
[0001072e] 3600                      move.w     d0,d3
[00010730] d643                      add.w      d3,d3
[00010732] 39b4 2002 3000            move.w     2(a4,d2.w),0(a4,d3.w)
[00010738] 3981 2002                 move.w     d1,2(a4,d2.w)
[0001073c] 5240                      addq.w     #1,d0
[0001073e] 72ff                      moveq.l    #-1,d1
[00010740] d279 0001 37f4            add.w      num_datapoints,d1
[00010746] b041                      cmp.w      d1,d0
[00010748] 6dbe                      blt.s      $00010708
[0001074a] 5246                      addq.w     #1,d6
[0001074c] 70ff                      moveq.l    #-1,d0
[0001074e] d079 0001 37f4            add.w      num_datapoints,d0
[00010754] bc40                      cmp.w      d0,d6
[00010756] 6dac                      blt.s      $00010704

[00010758] 7201                      moveq.l    #1,d1
[0001075a] 2079 0001 3a4a            movea.l    $00013A4A,a0 maintree
[00010760] c268 0082                 and.w      130(a0),d1
[00010764] 6700 00f0                 beq        $00010856

[00010768] 41ef 0070                 lea.l      112(a7),a0
[0001076c] 3004                      move.w     d4,d0
[0001076e] 6100 243e                 bsr        _wxcnv
[00010772] 41ef 0070                 lea.l      112(a7),a0
[00010776] 43f9 0001 392e            lea.l      $0001392E,a1
[0001077c] 6100 1936                 bsr        _xxmul
[00010780] 41ef 0070                 lea.l      112(a7),a0
[00010784] 43f9 0001 3938            lea.l      $00013938,a1
[0001078a] 6100 1b5a                 bsr        __xxdiv
[0001078e] 41ef 007a                 lea.l      122(a7),a0
[00010792] 2f20                      move.l     -(a0),-(a7)
[00010794] 2f20                      move.l     -(a0),-(a7)
[00010796] 3f20                      move.w     -(a0),-(a7)
[00010798] 486f 007a                 pea.l      122(a7)
[0001079c] 6100 0ce6                 bsr        cos
[000107a0] 4fef 000e                 lea.l      14(a7),a7
[000107a4] 41ef 007a                 lea.l      122(a7),a0
[000107a8] 2f20                      move.l     -(a0),-(a7)
[000107aa] 2f20                      move.l     -(a0),-(a7)
[000107ac] 3f20                      move.w     -(a0),-(a7)
[000107ae] 486f 007a                 pea.l      122(a7)
[000107b2] 6100 1140                 bsr        fabs
[000107b6] 4fef 000e                 lea.l      14(a7),a7

[000107ba] 41ef 007a                 lea.l      122(a7),a0
[000107be] 3004                      move.w     d4,d0
[000107c0] 6100 23ec                 bsr        _wxcnv
[000107c4] 41ef 007a                 lea.l      122(a7),a0
[000107c8] 43f9 0001 3942            lea.l      $00013942,a1
[000107ce] 6100 18e4                 bsr        _xxmul
[000107d2] 41ef 007a                 lea.l      122(a7),a0
[000107d6] 43f9 0001 394c            lea.l      $0001394C,a1
[000107dc] 6100 1b08                 bsr        __xxdiv
[000107e0] 41ef 0084                 lea.l      132(a7),a0
[000107e4] 2f20                      move.l     -(a0),-(a7)
[000107e6] 2f20                      move.l     -(a0),-(a7)
[000107e8] 3f20                      move.w     -(a0),-(a7)
[000107ea] 486f 0084                 pea.l      132(a7)
[000107ee] 6100 0cd0                 bsr        sin
[000107f2] 4fef 000e                 lea.l      14(a7),a7
[000107f6] 41ef 0084                 lea.l      132(a7),a0
[000107fa] 2f20                      move.l     -(a0),-(a7)
[000107fc] 2f20                      move.l     -(a0),-(a7)
[000107fe] 3f20                      move.w     -(a0),-(a7)
[00010800] 486f 0084                 pea.l      132(a7)
[00010804] 6100 10ee                 bsr        fabs
[00010808] 4fef 000e                 lea.l      14(a7),a7

[0001080c] 41ef 0084                 lea.l      132(a7),a0
[00010810] 302a 0136                 move.w     310(a2),d0
[00010814] 6100 2398                 bsr        _wxcnv
[00010818] 41ef 0084                 lea.l      132(a7),a0
[0001081c] 43ef 0070                 lea.l      112(a7),a1
[00010820] 6100 1892                 bsr        _xxmul
[00010824] 41ef 008e                 lea.l      142(a7),a0
[00010828] 302a 0138                 move.w     312(a2),d0
[0001082c] 6100 2380                 bsr        _wxcnv
[00010830] 41ef 008e                 lea.l      142(a7),a0
[00010834] 43ef 007a                 lea.l      122(a7),a1
[00010838] 6100 187a                 bsr        _xxmul
[0001083c] 41ef 0084                 lea.l      132(a7),a0
[00010840] 43ef 008e                 lea.l      142(a7),a1
[00010844] 6100 164a                 bsr        __xxadd
[00010848] 41ef 0084                 lea.l      132(a7),a0
[0001084c] 6100 2322                 bsr        _xwcnv
[00010850] 2600                      move.l     d0,d3
[00010852] 6000 0190                 bra        $000109E4

[00010856] 4240                      clr.w      d0
[00010858] 9079 0001 4268            sub.w      mx,d0
[0001085e] 48c0                      ext.l      d0
[00010860] 4241                      clr.w      d1
[00010862] 9279 0001 4268            sub.w      mx,d1
[00010868] 48c1                      ext.l      d1
[0001086a] 6100 280c                 bsr        _lmul
[0001086e] 2f40 0008                 move.l     d0,8(a7)
[00010872] 4240                      clr.w      d0
[00010874] 9079 0001 426a            sub.w      my,d0
[0001087a] 48c0                      ext.l      d0
[0001087c] 4241                      clr.w      d1
[0001087e] 9279 0001 426a            sub.w      my,d1
[00010884] 48c1                      ext.l      d1
[00010886] 6100 27f0                 bsr        _lmul
[0001088a] 2f40 0004                 move.l     d0,4(a7)
[0001088e] 222f 0008                 move.l     8(a7),d1
[00010892] d280                      add.l      d0,d1
[00010894] 41ef 0070                 lea.l      112(a7),a0
[00010898] 2001                      move.l     d1,d0
[0001089a] 6100 2314                 bsr        _lxcnv
[0001089e] 41ef 007a                 lea.l      122(a7),a0
[000108a2] 2f20                      move.l     -(a0),-(a7)
[000108a4] 2f20                      move.l     -(a0),-(a7)
[000108a6] 3f20                      move.w     -(a0),-(a7)
[000108a8] 486f 0084                 pea.l      132(a7)
[000108ac] 6100 1078                 bsr        sqrt
[000108b0] 4fef 000e                 lea.l      14(a7),a7
[000108b4] 41ef 007a                 lea.l      122(a7),a0
[000108b8] 6100 22b6                 bsr        _xwcnv
[000108bc] 2000                      move.l     d0,d0
[000108be] 2f40 0010                 move.l     d0,16(a7)

[000108c2] 3017                      move.w     (a7),d0
[000108c4] 9079 0001 426a            sub.w      my,d0
[000108ca] 48c0                      ext.l      d0
[000108cc] 3217                      move.w     (a7),d1
[000108ce] 9279 0001 426a            sub.w      my,d1
[000108d4] 48c1                      ext.l      d1
[000108d6] 6100 27a0                 bsr        _lmul
[000108da] 2f40 0004                 move.l     d0,4(a7)
[000108de] 222f 0008                 move.l     8(a7),d1
[000108e2] d280                      add.l      d0,d1
[000108e4] 41ef 0070                 lea.l      112(a7),a0
[000108e8] 2001                      move.l     d1,d0
[000108ea] 6100 22c4                 bsr        _lxcnv
[000108ee] 41ef 007a                 lea.l      122(a7),a0
[000108f2] 2f20                      move.l     -(a0),-(a7)
[000108f4] 2f20                      move.l     -(a0),-(a7)
[000108f6] 3f20                      move.w     -(a0),-(a7)
[000108f8] 486f 0084                 pea.l      132(a7)
[000108fc] 6100 1028                 bsr        sqrt
[00010900] 4fef 000e                 lea.l      14(a7),a7
[00010904] 41ef 007a                 lea.l      122(a7),a0
[00010908] 6100 2266                 bsr        _xwcnv
[0001090c] 2a00                      move.l     d0,d5
[0001090e] 302f 0002                 move.w     2(a7),d0
[00010912] 9079 0001 4268            sub.w      mx,d0
[00010918] 48c0                      ext.l      d0
[0001091a] 322f 0002                 move.w     2(a7),d1
[0001091e] 9279 0001 4268            sub.w      mx,d1
[00010924] 48c1                      ext.l      d1
[00010926] 6100 2750                 bsr        _lmul
[0001092a] 2f40 0008                 move.l     d0,8(a7)
[0001092e] 4240                      clr.w      d0
[00010930] 9079 0001 426a            sub.w      my,d0
[00010936] 48c0                      ext.l      d0
[00010938] 4241                      clr.w      d1
[0001093a] 9279 0001 426a            sub.w      my,d1
[00010940] 48c1                      ext.l      d1
[00010942] 6100 2734                 bsr        _lmul
[00010946] 2f40 0004                 move.l     d0,4(a7)
[0001094a] 222f 0008                 move.l     8(a7),d1
[0001094e] d280                      add.l      d0,d1
[00010950] 41ef 0070                 lea.l      112(a7),a0
[00010954] 2001                      move.l     d1,d0
[00010956] 6100 2258                 bsr        _lxcnv
[0001095a] 41ef 007a                 lea.l      122(a7),a0
[0001095e] 2f20                      move.l     -(a0),-(a7)
[00010960] 2f20                      move.l     -(a0),-(a7)
[00010962] 3f20                      move.w     -(a0),-(a7)
[00010964] 486f 0084                 pea.l      132(a7)
[00010968] 6100 0fbc                 bsr        sqrt
[0001096c] 4fef 000e                 lea.l      14(a7),a7

[00010970] 41ef 007a                 lea.l      122(a7),a0
[00010974] 6100 21fa                 bsr        _xwcnv
[00010978] 2000                      move.l     d0,d0
[0001097a] 2f40 000c                 move.l     d0,12(a7)
[0001097e] 3017                      move.w     (a7),d0
[00010980] 9079 0001 426a            sub.w      my,d0
[00010986] 48c0                      ext.l      d0
[00010988] 3217                      move.w     (a7),d1
[0001098a] 9279 0001 426a            sub.w      my,d1
[00010990] 48c1                      ext.l      d1
[00010992] 6100 26e4                 bsr        _lmul
[00010996] 2f40 0004                 move.l     d0,4(a7)
[0001099a] 222f 0008                 move.l     8(a7),d1
[0001099e] d280                      add.l      d0,d1
[000109a0] 41ef 0070                 lea.l      112(a7),a0
[000109a4] 2001                      move.l     d1,d0
[000109a6] 6100 2208                 bsr        _lxcnv
[000109aa] 41ef 007a                 lea.l      122(a7),a0
[000109ae] 2f20                      move.l     -(a0),-(a7)
[000109b0] 2f20                      move.l     -(a0),-(a7)
[000109b2] 3f20                      move.w     -(a0),-(a7)
[000109b4] 486f 0084                 pea.l      132(a7)
[000109b8] 6100 0f6c                 bsr        sqrt
[000109bc] 4fef 000e                 lea.l      14(a7),a7
[000109c0] 41ef 007a                 lea.l      122(a7),a0
[000109c4] 6100 21aa                 bsr        _xwcnv
[000109c8] 2200                      move.l     d0,d1

[000109ca] 262f 0010                 move.l     16(a7),d3
[000109ce] b685                      cmp.l      d5,d3
[000109d0] 6c02                      bge.s      $000109D4
[000109d2] 2605                      move.l     d5,d3
[000109d4] b6af 000c                 cmp.l      12(a7),d3
[000109d8] 6c04                      bge.s      $000109DE
[000109da] 262f 000c                 move.l     12(a7),d3
[000109de] b681                      cmp.l      d1,d3
[000109e0] 6c02                      bge.s      $000109E4
[000109e2] 2601                      move.l     d1,d3

[000109e4] 41f9 0001 3956            lea.l      $00013956,a0
[000109ea] 43ef 007a                 lea.l      122(a7),a1
[000109ee] 22d8                      move.l     (a0)+,(a1)+
[000109f0] 22d8                      move.l     (a0)+,(a1)+
[000109f2] 32d8                      move.w     (a0)+,(a1)+
[000109f4] 41ef 007a                 lea.l      122(a7),a0
[000109f8] 2003                      move.l     d3,d0
[000109fa] 6100 13ae                 bsr        _xldiv

[000109fe] 2003                      move.l     d3,d0
[00010a00] d080                      add.l      d0,d0
[00010a02] d083                      add.l      d3,d0
[00010a04] d0bc 0000 0800            add.l      #$00000800,d0
[00010a0a] 206f 006c                 movea.l    108(a7),a0
[00010a0e] 2268 0052                 movea.l    82(a0),a1
[00010a12] 2069 0038                 movea.l    56(a1),a0
[00010a16] 4e90                      jsr        (a0)

[00010a18] 2a48                      movea.l    a0,a5
[00010a1a] 2f48 0062                 move.l     a0,98(a7)

[00010a1e] 4246                      clr.w      d6
[00010a20] 601a                      bra.s      $00010A3C
[00010a22] 1bb9 0001 4224 6000       move.b     datapoint_red,0(a5,d6.w)
[00010a2a] 1bb9 0001 4235 6001       move.b     datapoint_green,1(a5,d6.w)
[00010a32] 1bb9 0001 4246 6002       move.b     datapoint_blue,2(a5,d6.w)
[00010a3a] 5646                      addq.w     #3,d6
[00010a3c] 3006                      move.w     d6,d0
[00010a3e] 48c0                      ext.l      d0
[00010a40] 2f00                      move.l     d0,-(a7)
[00010a42] 2f01                      move.l     d1,-(a7)
[00010a44] 2003                      move.l     d3,d0
[00010a46] 7202                      moveq.l    #2,d1
[00010a48] 6100 2678                 bsr        _ldiv
[00010a4c] 221f                      move.l     (a7)+,d1
[00010a4e] 2200                      move.l     d0,d1
[00010a50] d281                      add.l      d1,d1
[00010a52] d280                      add.l      d0,d1
[00010a54] 241f                      move.l     (a7)+,d2
[00010a56] b481                      cmp.l      d1,d2
[00010a58] 6dc8                      blt.s      $00010A22

[00010a5a] 2c01                      move.l     d1,d6
[00010a5c] 6038                      bra.s      $00010A96
[00010a5e] 3039 0001 37f4            move.w     num_datapoints,d0
[00010a64] 41f9 0001 4224            lea.l      datapoint_red,a0
[00010a6a] 1bb0 00ff 6000            move.b     -1(a0,d0.w),0(a5,d6.w)
[00010a70] 3039 0001 37f4            move.w     num_datapoints,d0
[00010a76] 43f9 0001 4235            lea.l      datapoint_green,a1
[00010a7c] 1bb1 00ff 6001            move.b     -1(a1,d0.w),1(a5,d6.w)
[00010a82] 3039 0001 37f4            move.w     num_datapoints,d0
[00010a88] 41f9 0001 4246            lea.l      datapoint_blue,a0
[00010a8e] 1bb0 00ff 6002            move.b     -1(a0,d0.w),2(a5,d6.w)
[00010a94] 5646                      addq.w     #3,d6
[00010a96] 3006                      move.w     d6,d0
[00010a98] 48c0                      ext.l      d0
[00010a9a] 2203                      move.l     d3,d1
[00010a9c] d281                      add.l      d1,d1
[00010a9e] d283                      add.l      d3,d1
[00010aa0] d2bc 0000 0400            add.l      #$00000400,d1
[00010aa6] b081                      cmp.l      d1,d0
[00010aa8] 6db4                      blt.s      $00010A5E

[00010aaa] 4bed 0400                 lea.l      1024(a5),a5
[00010aae] 426f 003e                 clr.w      62(a7)
[00010ab2] 6000 0162                 bra        $00010C16
[00010ab6] 302f 003e                 move.w     62(a7),d0
[00010aba] d040                      add.w      d0,d0
[00010abc] 3f74 0000 003c            move.w     0(a4,d0.w),60(a7)
[00010ac2] 3a34 0002                 move.w     2(a4,d0.w),d5

[00010ac6] 322f 003c                 move.w     60(a7),d1
[00010aca] 41f9 0001 4224            lea.l      datapoint_red,a0
[00010ad0] 7400                      moveq.l    #0,d2
[00010ad2] 1430 1000                 move.b     0(a0,d1.w),d2
[00010ad6] 7c0a                      moveq.l    #10,d6
[00010ad8] edaa                      lsl.l      d6,d2
[00010ada] 2f42 0038                 move.l     d2,56(a7)

[00010ade] 43f9 0001 4235            lea.l      datapoint_green,a1
[00010ae4] 7c00                      moveq.l    #0,d6
[00010ae6] 1c31 1000                 move.b     0(a1,d1.w),d6
[00010aea] 700a                      moveq.l    #10,d0
[00010aec] e1ae                      lsl.l      d0,d6

[00010aee] 41f9 0001 4246            lea.l      datapoint_blue,a0
[00010af4] 7400                      moveq.l    #0,d2
[00010af6] 1430 1000                 move.b     0(a0,d1.w),d2
[00010afa] e1aa                      lsl.l      d0,d2
[00010afc] 2f42 0034                 move.l     d2,52(a7)

[00010b00] 43f9 0001 4257            lea.l      $00014257,a1
[00010b06] 4242                      clr.w      d2
[00010b08] 1431 1000                 move.b     0(a1,d1.w),d2
[00010b0c] 41ef 0070                 lea.l      112(a7),a0
[00010b10] 3002                      move.w     d2,d0
[00010b12] 6100 209a                 bsr        _wxcnv
[00010b16] 41ef 0070                 lea.l      112(a7),a0
[00010b1a] 43ef 007a                 lea.l      122(a7),a1
[00010b1e] 6100 17c6                 bsr        __xxdiv
[00010b22] 41ef 0070                 lea.l      112(a7),a0
[00010b26] 6100 2048                 bsr        _xwcnv
[00010b2a] 3000                      move.w     d0,d0
[00010b2c] 3f40 0068                 move.w     d0,104(a7)

[00010b30] 41f9 0001 4257            lea.l      $00014257,a0
[00010b36] 4241                      clr.w      d1
[00010b38] 1230 5000                 move.b     0(a0,d5.w),d1
[00010b3c] 41ef 0070                 lea.l      112(a7),a0
[00010b40] 3001                      move.w     d1,d0
[00010b42] 6100 206a                 bsr        _wxcnv
[00010b46] 41ef 0070                 lea.l      112(a7),a0
[00010b4a] 43ef 007a                 lea.l      122(a7),a1
[00010b4e] 6100 1796                 bsr        __xxdiv
[00010b52] 41ef 0070                 lea.l      112(a7),a0
[00010b56] 6100 2018                 bsr        _xwcnv
[00010b5a] 3000                      move.w     d0,d0
[00010b5c] 3f40 0066                 move.w     d0,102(a7)

[00010b60] 41f9 0001 4224            lea.l      datapoint_red,a0
[00010b66] 7000                      moveq.l    #0,d0
[00010b68] 1030 5000                 move.b     0(a0,d5.w),d0
[00010b6c] 720a                      moveq.l    #10,d1
[00010b6e] e3a8                      lsl.l      d1,d0
[00010b70] 90af 0038                 sub.l      56(a7),d0
[00010b74] 322f 0066                 move.w     102(a7),d1
[00010b78] 926f 0068                 sub.w      104(a7),d1
[00010b7c] 48c1                      ext.l      d1
[00010b7e] 6100 2542                 bsr        _ldiv
[00010b82] 2f40 0030                 move.l     d0,48(a7)

[00010b86] 41f9 0001 4235            lea.l      datapoint_green,a0
[00010b8c] 7000                      moveq.l    #0,d0
[00010b8e] 1030 5000                 move.b     0(a0,d5.w),d0
[00010b92] 720a                      moveq.l    #10,d1
[00010b94] e3a8                      lsl.l      d1,d0
[00010b96] 9086                      sub.l      d6,d0
[00010b98] 322f 0066                 move.w     102(a7),d1
[00010b9c] 926f 0068                 sub.w      104(a7),d1
[00010ba0] 48c1                      ext.l      d1
[00010ba2] 6100 251e                 bsr        _ldiv
[00010ba6] 2f40 002c                 move.l     d0,44(a7)

[00010baa] 41f9 0001 4246            lea.l      datapoint_blue,a0
[00010bb0] 7000                      moveq.l    #0,d0
[00010bb2] 1030 5000                 move.b     0(a0,d5.w),d0
[00010bb6] 720a                      moveq.l    #10,d1
[00010bb8] e3a8                      lsl.l      d1,d0
[00010bba] 90af 0034                 sub.l      52(a7),d0
[00010bbe] 322f 0066                 move.w     102(a7),d1
[00010bc2] 926f 0068                 sub.w      104(a7),d1
[00010bc6] 48c1                      ext.l      d1
[00010bc8] 6100 24f8                 bsr        _ldiv
[00010bcc] 2200                      move.l     d0,d1

[00010bce] 3f6f 0068 006a            move.w     104(a7),106(a7)
[00010bd4] 6032                      bra.s      $00010C08
[00010bd6] 202f 0038                 move.l     56(a7),d0
[00010bda] 740a                      moveq.l    #10,d2
[00010bdc] e4a0                      asr.l      d2,d0
[00010bde] 1a80                      move.b     d0,(a5)
[00010be0] 2a06                      move.l     d6,d5
[00010be2] e4a5                      asr.l      d2,d5
[00010be4] 1b45 0001                 move.b     d5,1(a5)
[00010be8] 202f 0034                 move.l     52(a7),d0
[00010bec] e4a0                      asr.l      d2,d0
[00010bee] 1b40 0002                 move.b     d0,2(a5)
[00010bf2] 564d                      addq.w     #3,a5
[00010bf4] 2a2f 0030                 move.l     48(a7),d5
[00010bf8] dbaf 0038                 add.l      d5,56(a7)
[00010bfc] dcaf 002c                 add.l      44(a7),d6
[00010c00] d3af 0034                 add.l      d1,52(a7)
[00010c04] 526f 006a                 addq.w     #1,106(a7)
[00010c08] 302f 006a                 move.w     106(a7),d0
[00010c0c] b06f 0066                 cmp.w      102(a7),d0
[00010c10] 6dc4                      blt.s      $00010BD6

[00010c12] 526f 003e                 addq.w     #1,62(a7)
[00010c16] 70ff                      moveq.l    #-1,d0
[00010c18] d079 0001 37f4            add.w      num_datapoints,d0
[00010c1e] 322f 003e                 move.w     62(a7),d1
[00010c22] b240                      cmp.w      d0,d1
[00010c24] 6d00 fe90                 blt        $00010AB6

[00010c28] 2f02                      move.l     d2,-(a7)
[00010c2a] 2003                      move.l     d3,d0
[00010c2c] 7202                      moveq.l    #2,d1
[00010c2e] 6100 2492                 bsr        _ldiv
[00010c32] 221f                      move.l     (a7)+,d1
[00010c34] 2200                      move.l     d0,d1
[00010c36] d281                      add.l      d1,d1
[00010c38] d280                      add.l      d0,d1
[00010c3a] 2a6f 0062                 movea.l    98(a7),a5
[00010c3e] dbc1                      adda.l     d1,a5
[00010c40] 4bed 0400                 lea.l      1024(a5),a5

[00010c44] 41ef 0070                 lea.l      112(a7),a0
[00010c48] 3004                      move.w     d4,d0
[00010c4a] 6100 1f62                 bsr        _wxcnv
[00010c4e] 41ef 0070                 lea.l      112(a7),a0
[00010c52] 43f9 0001 3960            lea.l      $00013960,a1
[00010c58] 6100 145a                 bsr        _xxmul
[00010c5c] 41ef 0070                 lea.l      112(a7),a0
[00010c60] 43f9 0001 396a            lea.l      $0001396A,a1
[00010c66] 6100 167e                 bsr        __xxdiv
[00010c6a] 41ef 007a                 lea.l      122(a7),a0
[00010c6e] 2f20                      move.l     -(a0),-(a7)
[00010c70] 2f20                      move.l     -(a0),-(a7)
[00010c72] 3f20                      move.w     -(a0),-(a7)
[00010c74] 486f 0084                 pea.l      132(a7)
[00010c78] 6100 080a                 bsr        cos
[00010c7c] 4fef 000e                 lea.l      14(a7),a7
[00010c80] 41ef 007a                 lea.l      122(a7),a0
[00010c84] 43f9 0001 3974            lea.l      $00013974,a1
[00010c8a] 6100 1428                 bsr        _xxmul
[00010c8e] 41ef 007a                 lea.l      122(a7),a0
[00010c92] 6100 1edc                 bsr        _xwcnv
[00010c96] 2000                      move.l     d0,d0
[00010c98] 2f40 0028                 move.l     d0,40(a7)

[00010c9c] 41ef 0070                 lea.l      112(a7),a0
[00010ca0] 3004                      move.w     d4,d0
[00010ca2] 6100 1f0a                 bsr        _wxcnv
[00010ca6] 41ef 0070                 lea.l      112(a7),a0
[00010caa] 43f9 0001 397e            lea.l      $0001397E,a1
[00010cb0] 6100 1402                 bsr        _xxmul
[00010cb4] 41ef 0070                 lea.l      112(a7),a0
[00010cb8] 43f9 0001 3988            lea.l      $00013988,a1
[00010cbe] 6100 1626                 bsr        __xxdiv
[00010cc2] 41ef 007a                 lea.l      122(a7),a0
[00010cc6] 2f20                      move.l     -(a0),-(a7)
[00010cc8] 2f20                      move.l     -(a0),-(a7)
[00010cca] 3f20                      move.w     -(a0),-(a7)
[00010ccc] 486f 0084                 pea.l      132(a7)
[00010cd0] 6100 07ee                 bsr        sin
[00010cd4] 4fef 000e                 lea.l      14(a7),a7
[00010cd8] 41ef 007a                 lea.l      122(a7),a0
[00010cdc] 43f9 0001 3992            lea.l      $00013992,a1
[00010ce2] 6100 13d0                 bsr        _xxmul
[00010ce6] 41ef 007a                 lea.l      122(a7),a0
[00010cea] 6100 1e84                 bsr        _xwcnv
[00010cee] 2000                      move.l     d0,d0
[00010cf0] 2f40 0024                 move.l     d0,36(a7)

[00010cf4] 7200                      moveq.l    #0,d1
[00010cf6] 2f41 001c                 move.l     d1,28(a7)
[00010cfa] 2f41 0020                 move.l     d1,32(a7)
[00010cfe] 202f 0028                 move.l     40(a7),d0
[00010d02] 4480                      neg.l      d0
[00010d04] 322a 0136                 move.w     310(a2),d1
[00010d08] 48c1                      ext.l      d1
[00010d0a] 6100 236c                 bsr        _lmul
[00010d0e] e280                      asr.l      #1,d0
[00010d10] 2f40 0014                 move.l     d0,20(a7)

[00010d14] 202f 0024                 move.l     36(a7),d0
[00010d18] 4480                      neg.l      d0
[00010d1a] 322a 0138                 move.w     312(a2),d1
[00010d1e] 48c1                      ext.l      d1
[00010d20] 6100 2356                 bsr        _lmul
[00010d24] e280                      asr.l      #1,d0
[00010d26] 2f40 0020                 move.l     d0,32(a7)

[00010d2a] 720a                      moveq.l    #10,d1
[00010d2c] e2a0                      asr.l      d1,d0
[00010d2e] 2c00                      move.l     d0,d6
[00010d30] dc86                      add.l      d6,d6
[00010d32] dc80                      add.l      d0,d6
[00010d34] 7401                      moveq.l    #1,d2
[00010d36] 2079 0001 3a4a            movea.l    $00013A4A,a0 maintree
[00010d3c] c468 0082                 and.w      130(a0),d2
[00010d40] 6700 00c2                 beq        $00010E04

[00010d44] 426f 006a                 clr.w      106(a7)
[00010d48] 6000 00aa                 bra        $00010DF4
[00010d4c] 701f                      moveq.l    #31,d0
[00010d4e] c06f 006a                 and.w      106(a7),d0
[00010d52] 661e                      bne.s      $00010D72
[00010d54] 302f 006a                 move.w     106(a7),d0
[00010d58] 48c0                      ext.l      d0
[00010d5a] ef88                      lsl.l      #7,d0
[00010d5c] 322a 0138                 move.w     312(a2),d1
[00010d60] 48c1                      ext.l      d1
[00010d62] 6100 235e                 bsr        _ldiv
[00010d66] 206f 006c                 movea.l    108(a7),a0
[00010d6a] 2268 0052                 movea.l    82(a0),a1
[00010d6e] 2051                      movea.l    (a1),a0
[00010d70] 4e90                      jsr        (a0)
[00010d72] 2f6f 0014 001c            move.l     20(a7),28(a7)

[00010d78] 2f00                      move.l     d0,-(a7)
[00010d7a] 302f 006e                 move.w     110(a7),d0
[00010d7e] 48c0                      ext.l      d0
[00010d80] 322a 0136                 move.w     310(a2),d1
[00010d84] 48c1                      ext.l      d1
[00010d86] 6100 22f0                 bsr        _lmul
[00010d8a] 221f                      move.l     (a7)+,d1
[00010d8c] 2200                      move.l     d0,d1
[00010d8e] d281                      add.l      d1,d1
[00010d90] d280                      add.l      d0,d1
[00010d92] 2f41 0018                 move.l     d1,24(a7)

[00010d96] 4245                      clr.w      d5
[00010d98] 603a                      bra.s      $00010DD4
[00010d9a] 3005                      move.w     d5,d0
[00010d9c] 48c0                      ext.l      d0
[00010d9e] 2600                      move.l     d0,d3
[00010da0] d683                      add.l      d3,d3
[00010da2] d680                      add.l      d0,d3
[00010da4] d6af 0018                 add.l      24(a7),d3
[00010da8] 222f 001c                 move.l     28(a7),d1
[00010dac] 740a                      moveq.l    #10,d2
[00010dae] e4a1                      asr.l      d2,d1
[00010db0] 2801                      move.l     d1,d4
[00010db2] d884                      add.l      d4,d4
[00010db4] d881                      add.l      d1,d4
[00010db6] d886                      add.l      d6,d4
[00010db8] 17b5 4800 3800            move.b     0(a5,d4.l),0(a3,d3.l)
[00010dbe] 17b5 4801 3801            move.b     1(a5,d4.l),1(a3,d3.l)
[00010dc4] 17b5 4802 3802            move.b     2(a5,d4.l),2(a3,d3.l)
[00010dca] 202f 0028                 move.l     40(a7),d0
[00010dce] d1af 001c                 add.l      d0,28(a7)
[00010dd2] 5245                      addq.w     #1,d5
[00010dd4] ba6a 0136                 cmp.w      310(a2),d5
[00010dd8] 6dc0                      blt.s      $00010D9A
[00010dda] 202f 0024                 move.l     36(a7),d0
[00010dde] d1af 0020                 add.l      d0,32(a7)
[00010de2] 222f 0020                 move.l     32(a7),d1
[00010de6] 740a                      moveq.l    #10,d2
[00010de8] e4a1                      asr.l      d2,d1
[00010dea] 2c01                      move.l     d1,d6
[00010dec] dc86                      add.l      d6,d6
[00010dee] dc81                      add.l      d1,d6
[00010df0] 526f 006a                 addq.w     #1,106(a7)
[00010df4] 302f 006a                 move.w     106(a7),d0
[00010df8] b06a 0138                 cmp.w      312(a2),d0
[00010dfc] 6d00 ff4e                 blt        $00010D4C
[00010e00] 6000 00fa                 bra        $00010EFC

[00010e04] 2a6f 0062                 movea.l    98(a7),a5
[00010e08] 4bed 0400                 lea.l      1024(a5),a5
[00010e0c] 426f 006a                 clr.w      106(a7)
[00010e10] 6000 00de                 bra        $00010EF0
[00010e14] 700f                      moveq.l    #15,d0
[00010e16] c06f 006a                 and.w      106(a7),d0
[00010e1a] 661e                      bne.s      $00010E3A
[00010e1c] 302f 006a                 move.w     106(a7),d0
[00010e20] 48c0                      ext.l      d0
[00010e22] ef88                      lsl.l      #7,d0
[00010e24] 322a 0138                 move.w     312(a2),d1
[00010e28] 48c1                      ext.l      d1
[00010e2a] 6100 2296                 bsr        _ldiv
[00010e2e] 206f 006c                 movea.l    108(a7),a0
[00010e32] 2268 0052                 movea.l    82(a0),a1
[00010e36] 2051                      movea.l    (a1),a0
[00010e38] 4e90                      jsr        (a0)

[00010e3a] 2f00                      move.l     d0,-(a7)
[00010e3c] 302f 006e                 move.w     110(a7),d0
[00010e40] 48c0                      ext.l      d0
[00010e42] 322a 0136                 move.w     310(a2),d1
[00010e46] 48c1                      ext.l      d1
[00010e48] 6100 222e                 bsr        _lmul
[00010e4c] 221f                      move.l     (a7)+,d1
[00010e4e] 2200                      move.l     d0,d1
[00010e50] d281                      add.l      d1,d1
[00010e52] d280                      add.l      d0,d1
[00010e54] 2f41 0018                 move.l     d1,24(a7)

[00010e58] 302f 006a                 move.w     106(a7),d0
[00010e5c] 9079 0001 426a            sub.w      my,d0
[00010e62] 48c0                      ext.l      d0
[00010e64] 322f 006a                 move.w     106(a7),d1
[00010e68] 9279 0001 426a            sub.w      my,d1
[00010e6e] 48c1                      ext.l      d1
[00010e70] 6100 2206                 bsr        _lmul
[00010e74] 2f40 0004                 move.l     d0,4(a7)
[00010e78] 3c39 0001 4268            move.w     mx,d6
[00010e7e] 48c6                      ext.l      d6
[00010e80] 4245                      clr.w      d5
[00010e82] 6062                      bra.s      $00010EE6
[00010e84] 5386                      subq.l     #1,d6
[00010e86] 2006                      move.l     d6,d0
[00010e88] 2206                      move.l     d6,d1
[00010e8a] 6100 21ec                 bsr        _lmul
[00010e8e] 2f40 0008                 move.l     d0,8(a7)
[00010e92] d0af 0004                 add.l      4(a7),d0
[00010e96] 41ef 0070                 lea.l      112(a7),a0
[00010e9a] 6100 1d14                 bsr        _lxcnv
[00010e9e] 41ef 007a                 lea.l      122(a7),a0
[00010ea2] 2f20                      move.l     -(a0),-(a7)
[00010ea4] 2f20                      move.l     -(a0),-(a7)
[00010ea6] 3f20                      move.w     -(a0),-(a7)
[00010ea8] 486f 0084                 pea.l      132(a7)
[00010eac] 6100 0a78                 bsr        sqrt
[00010eb0] 4fef 000e                 lea.l      14(a7),a7

[00010eb4] 41ef 007a                 lea.l      122(a7),a0
[00010eb8] 6100 1cb6                 bsr        _xwcnv
[00010ebc] 2000                      move.l     d0,d0
[00010ebe] 2800                      move.l     d0,d4
[00010ec0] d884                      add.l      d4,d4
[00010ec2] d880                      add.l      d0,d4
[00010ec4] 3005                      move.w     d5,d0
[00010ec6] 48c0                      ext.l      d0
[00010ec8] 2600                      move.l     d0,d3
[00010eca] d683                      add.l      d3,d3
[00010ecc] d680                      add.l      d0,d3
[00010ece] d6af 0018                 add.l      24(a7),d3
[00010ed2] 17b5 4800 3800            move.b     0(a5,d4.l),0(a3,d3.l)
[00010ed8] 17b5 4801 3801            move.b     1(a5,d4.l),1(a3,d3.l)
[00010ede] 17b5 4802 3802            move.b     2(a5,d4.l),2(a3,d3.l)
[00010ee4] 5245                      addq.w     #1,d5
[00010ee6] ba6a 0136                 cmp.w      310(a2),d5
[00010eea] 6d98                      blt.s      $00010E84
[00010eec] 526f 006a                 addq.w     #1,106(a7)
[00010ef0] 302f 006a                 move.w     106(a7),d0
[00010ef4] b06a 0138                 cmp.w      312(a2),d0
[00010ef8] 6d00 ff1a                 blt        $00010E14

[00010efc] 206f 0062                 movea.l    98(a7),a0
[00010f00] 226f 006c                 movea.l    108(a7),a1
[00010f04] 2469 0052                 movea.l    82(a1),a2
[00010f08] 226a 003c                 movea.l    60(a2),a1
[00010f0c] 4e91                      jsr        (a1)
[00010f0e] 4fef 0098                 lea.l      152(a7),a7
[00010f12] 4cdf 3c78                 movem.l    (a7)+,d3-d6/a2-a5
[00010f16] 4e75                      rts

compute_preview:
[00010f18] 48e7 1f3c                 movem.l    d3-d7/a2-a5,-(a7)
[00010f1c] 4fef ffce                 lea.l      -50(a7),a7
[00010f20] 426f 000a                 clr.w      10(a7)
[00010f24] 2468 012e                 movea.l    302(a0),a2
[00010f28] 4243                      clr.w      d3
[00010f2a] 47ef 000c                 lea.l      12(a7),a3
[00010f2e] 600a                      bra.s      $00010F3A
[00010f30] 3003                      move.w     d3,d0
[00010f32] d040                      add.w      d0,d0
[00010f34] 3783 0000                 move.w     d3,0(a3,d0.w)
[00010f38] 5243                      addq.w     #1,d3
[00010f3a] b67c 0010                 cmp.w      #$0010,d3
[00010f3e] 6df0                      blt.s      $00010F30
[00010f40] 4243                      clr.w      d3
[00010f42] 49f9 0001 4257            lea.l      $00014257,a4
[00010f48] 4bf9 0001 37f4            lea.l      num_datapoints,a5
[00010f4e] 603a                      bra.s      $00010F8A
[00010f50] 4244                      clr.w      d4
[00010f52] 602c                      bra.s      $00010F80
[00010f54] 3004                      move.w     d4,d0
[00010f56] d040                      add.w      d0,d0
[00010f58] 3233 0002                 move.w     2(a3,d0.w),d1
[00010f5c] 1434 1000                 move.b     0(a4,d1.w),d2
[00010f60] 3233 0000                 move.w     0(a3,d0.w),d1
[00010f64] b434 1000                 cmp.b      0(a4,d1.w),d2
[00010f68] 6414                      bcc.s      $00010F7E
[00010f6a] 3001                      move.w     d1,d0
[00010f6c] 3404                      move.w     d4,d2
[00010f6e] d442                      add.w      d2,d2
[00010f70] 3204                      move.w     d4,d1
[00010f72] d241                      add.w      d1,d1
[00010f74] 37b3 2002 1000            move.w     2(a3,d2.w),0(a3,d1.w)
[00010f7a] 3780 1002                 move.w     d0,2(a3,d1.w)
[00010f7e] 5244                      addq.w     #1,d4
[00010f80] 70ff                      moveq.l    #-1,d0
[00010f82] d055                      add.w      (a5),d0
[00010f84] b840                      cmp.w      d0,d4
[00010f86] 6dcc                      blt.s      $00010F54
[00010f88] 5243                      addq.w     #1,d3
[00010f8a] 70ff                      moveq.l    #-1,d0
[00010f8c] d055                      add.w      (a5),d0
[00010f8e] b640                      cmp.w      d0,d3
[00010f90] 6dbe                      blt.s      $00010F50
[00010f92] 426f 000a                 clr.w      10(a7)
[00010f96] 6000 00e2                 bra        $0001107A
[00010f9a] 302f 000a                 move.w     10(a7),d0
[00010f9e] d040                      add.w      d0,d0
[00010fa0] 3033 0000                 move.w     0(a3,d0.w),d0
[00010fa4] 322f 000a                 move.w     10(a7),d1
[00010fa8] d241                      add.w      d1,d1
[00010faa] 3f73 1002 0008            move.w     2(a3,d1.w),8(a7)
[00010fb0] 7a00                      moveq.l    #0,d5
[00010fb2] 1a34 00cd                 move.b     -51(a4,d0.w),d5
[00010fb6] 740a                      moveq.l    #10,d2
[00010fb8] e5ad                      lsl.l      d2,d5
[00010fba] 7c00                      moveq.l    #0,d6
[00010fbc] 1c34 00de                 move.b     -34(a4,d0.w),d6
[00010fc0] e5ae                      lsl.l      d2,d6
[00010fc2] 7e00                      moveq.l    #0,d7
[00010fc4] 1e34 00ef                 move.b     -17(a4,d0.w),d7
[00010fc8] e5af                      lsl.l      d2,d7
[00010fca] 4243                      clr.w      d3
[00010fcc] 1634 0000                 move.b     0(a4,d0.w),d3
[00010fd0] 3f43 0030                 move.w     d3,48(a7)
[00010fd4] 302f 0008                 move.w     8(a7),d0
[00010fd8] 4244                      clr.w      d4
[00010fda] 1834 0000                 move.b     0(a4,d0.w),d4
[00010fde] 3f44 002e                 move.w     d4,46(a7)
[00010fe2] 7200                      moveq.l    #0,d1
[00010fe4] 1234 00cd                 move.b     -51(a4,d0.w),d1
[00010fe8] 2001                      move.l     d1,d0
[00010fea] e5a8                      lsl.l      d2,d0
[00010fec] 9085                      sub.l      d5,d0
[00010fee] 3204                      move.w     d4,d1
[00010ff0] 9243                      sub.w      d3,d1
[00010ff2] 48c1                      ext.l      d1
[00010ff4] 6100 20cc                 bsr        _ldiv
[00010ff8] 2f40 0004                 move.l     d0,4(a7)
[00010ffc] 322f 0008                 move.w     8(a7),d1
[00011000] 7000                      moveq.l    #0,d0
[00011002] 1034 10de                 move.b     -34(a4,d1.w),d0
[00011006] 740a                      moveq.l    #10,d2
[00011008] e5a8                      lsl.l      d2,d0
[0001100a] 9086                      sub.l      d6,d0
[0001100c] 322f 002e                 move.w     46(a7),d1
[00011010] 926f 0030                 sub.w      48(a7),d1
[00011014] 48c1                      ext.l      d1
[00011016] 6100 20aa                 bsr        _ldiv
[0001101a] 2e80                      move.l     d0,(a7)
[0001101c] 322f 0008                 move.w     8(a7),d1
[00011020] 7000                      moveq.l    #0,d0
[00011022] 1034 10ef                 move.b     -17(a4,d1.w),d0
[00011026] 740a                      moveq.l    #10,d2
[00011028] e5a8                      lsl.l      d2,d0
[0001102a] 9087                      sub.l      d7,d0
[0001102c] 322f 002e                 move.w     46(a7),d1
[00011030] 926f 0030                 sub.w      48(a7),d1
[00011034] 48c1                      ext.l      d1
[00011036] 6100 208a                 bsr        _ldiv
[0001103a] 382f 0030                 move.w     48(a7),d4
[0001103e] 6030                      bra.s      $00011070
[00011040] 4243                      clr.w      d3
[00011042] 601c                      bra.s      $00011060
[00011044] 2205                      move.l     d5,d1
[00011046] 740a                      moveq.l    #10,d2
[00011048] e4a1                      asr.l      d2,d1
[0001104a] 1481                      move.b     d1,(a2)
[0001104c] 2206                      move.l     d6,d1
[0001104e] e4a1                      asr.l      d2,d1
[00011050] 1541 0001                 move.b     d1,1(a2)
[00011054] 2207                      move.l     d7,d1
[00011056] e4a1                      asr.l      d2,d1
[00011058] 1541 0002                 move.b     d1,2(a2)
[0001105c] 564a                      addq.w     #3,a2
[0001105e] 5243                      addq.w     #1,d3
[00011060] b67c 0010                 cmp.w      #$0010,d3
[00011064] 6dde                      blt.s      $00011044
[00011066] daaf 0004                 add.l      4(a7),d5
[0001106a] dc97                      add.l      (a7),d6
[0001106c] de80                      add.l      d0,d7
[0001106e] 5244                      addq.w     #1,d4
[00011070] b86f 002e                 cmp.w      46(a7),d4
[00011074] 6dca                      blt.s      $00011040
[00011076] 526f 000a                 addq.w     #1,10(a7)
[0001107a] 70ff                      moveq.l    #-1,d0
[0001107c] d055                      add.w      (a5),d0
[0001107e] 322f 000a                 move.w     10(a7),d1
[00011082] b240                      cmp.w      d0,d1
[00011084] 6d00 ff14                 blt        $00010F9A
[00011088] 4fef 0032                 lea.l      50(a7),a7
[0001108c] 4cdf 3cf8                 movem.l    (a7)+,d3-d7/a2-a5
[00011090] 4e75                      rts

[00011092] 48e7 1f3e                 movem.l    d3-d7/a2-a6,-(a7)
[00011096] 4fef ff9a                 lea.l      -102(a7),a7
[0001109a] 3600                      move.w     d0,d3
[0001109c] 6100 0338                 bsr        x113d6
[000110a0] 47f9 0001 3a4a            lea.l      $00013A4A,a3 maintree
[000110a6] 486f 005e                 pea.l      94(a7)
[000110aa] 43ef 0064                 lea.l      100(a7),a1
[000110ae] 7015                      moveq.l    #21,d0
[000110b0] 2053                      movea.l    (a3),a0
[000110b2] 6100 20e8                 bsr        objc_offset
[000110b6] 584f                      addq.w     #4,a7
[000110b8] 2053                      movea.l    (a3),a0
[000110ba] 3028 020c                 move.w     524(a0),d0
[000110be] 3028 020e                 move.w     526(a0),d0
[000110c2] 486f 005a                 pea.l      90(a7)
[000110c6] 43ef 0060                 lea.l      96(a7),a1
[000110ca] 3003                      move.w     d3,d0
[000110cc] 6100 20ce                 bsr        objc_offset
[000110d0] 584f                      addq.w     #4,a7
[000110d2] 3203                      move.w     d3,d1
[000110d4] 48c1                      ext.l      d1
[000110d6] 2001                      move.l     d1,d0
[000110d8] d080                      add.l      d0,d0
[000110da] d081                      add.l      d1,d0
[000110dc] e788                      lsl.l      #3,d0
[000110de] 2053                      movea.l    (a3),a0
[000110e0] 3f70 0814 0058            move.w     20(a0,d0.l),88(a7)
[000110e6] 3f70 0816 0056            move.w     22(a0,d0.l),86(a7)
[000110ec] 3c2f 005a                 move.w     90(a7),d6
[000110f0] 3a30 0812                 move.w     18(a0,d0.l),d5
[000110f4] 3e05                      move.w     d5,d7
[000110f6] 49ef 0064                 lea.l      100(a7),a4
[000110fa] 4bef 0062                 lea.l      98(a7),a5
[000110fe] b67c 0016                 cmp.w      #$0016,d3
[00011102] 6706                      beq.s      $0001110A
[00011104] b67c 0017                 cmp.w      #$0017,d3
[00011108] 6616                      bne.s      $00011120
[0001110a] 4854                      pea.l      (a4)
[0001110c] 4855                      pea.l      (a5)
[0001110e] 224c                      movea.l    a4,a1
[00011110] 204c                      movea.l    a4,a0
[00011112] 6100 204e                 bsr        graf_mkstate
[00011116] 504f                      addq.w     #8,a7
[00011118] 3015                      move.w     (a5),d0
[0001111a] 66ee                      bne.s      $0001110A
[0001111c] 6000 018a                 bra        $000112A8
[00011120] 45ef 0052                 lea.l      82(a7),a2
[00011124] 4854                      pea.l      (a4)
[00011126] 4855                      pea.l      (a5)
[00011128] 224a                      movea.l    a2,a1
[0001112a] 41ef 005c                 lea.l      92(a7),a0
[0001112e] 6100 2032                 bsr        graf_mkstate
[00011132] 504f                      addq.w     #8,a7
[00011134] 3015                      move.w     (a5),d0
[00011136] 6700 0170                 beq        $000112A8
[0001113a] 3812                      move.w     (a2),d4
[0001113c] 986f 005a                 sub.w      90(a7),d4
[00011140] 41d7                      lea.l      (a7),a0
[00011142] 7003                      moveq.l    #3,d0
[00011144] 6100 2040                 bsr        graf_mouse
[00011148] 4854                      pea.l      (a4)
[0001114a] 4855                      pea.l      (a5)
[0001114c] 224a                      movea.l    a2,a1
[0001114e] 41ef 005c                 lea.l      92(a7),a0
[00011152] 6100 200e                 bsr        graf_mkstate
[00011156] 504f                      addq.w     #8,a7
[00011158] bc52                      cmp.w      (a2),d6
[0001115a] 6604                      bne.s      $00011160
[0001115c] 3015                      move.w     (a5),d0
[0001115e] 66e8                      bne.s      $00011148
[00011160] 3012                      move.w     (a2),d0
[00011162] 906f 005e                 sub.w      94(a7),d0
[00011166] 9044                      sub.w      d4,d0
[00011168] 3403                      move.w     d3,d2
[0001116a] 48c2                      ext.l      d2
[0001116c] 2202                      move.l     d2,d1
[0001116e] d281                      add.l      d1,d1
[00011170] d282                      add.l      d2,d1
[00011172] e789                      lsl.l      #3,d1
[00011174] 2053                      movea.l    (a3),a0
[00011176] 3180 1812                 move.w     d0,18(a0,d1.l)
[0001117a] 4a40                      tst.w      d0
[0001117c] 6a08                      bpl.s      $00011186
[0001117e] 2053                      movea.l    (a3),a0
[00011180] 4270 1812                 clr.w      18(a0,d1.l)
[00011184] 601c                      bra.s      $000111A2
[00011186] 3203                      move.w     d3,d1
[00011188] 48c1                      ext.l      d1
[0001118a] 2001                      move.l     d1,d0
[0001118c] d080                      add.l      d0,d0
[0001118e] d081                      add.l      d1,d0
[00011190] e788                      lsl.l      #3,d0
[00011192] 2053                      movea.l    (a3),a0
[00011194] 0c70 00ff 0812            cmpi.w     #$00FF,18(a0,d0.l)
[0001119a] 6f06                      ble.s      $000111A2
[0001119c] 31bc 00ff 0812            move.w     #$00FF,18(a0,d0.l)
[000111a2] 4241                      clr.w      d1
[000111a4] 3003                      move.w     d3,d0
[000111a6] 93c9                      suba.l     a1,a1
[000111a8] 206b 0004                 movea.l    4(a3),a0 13a4e win
[000111ac] 2c6b 0826                 movea.l    2086(a3),a6 14270 redraw_window
[000111b0] 4e96                      jsr        (a6)
[000111b2] 70ff                      moveq.l    #-1,d0
[000111b4] d06f 005c                 add.w      92(a7),d0
[000111b8] 3f40 004a                 move.w     d0,74(a7)
[000111bc] 7202                      moveq.l    #2,d1
[000111be] d26f 0058                 add.w      88(a7),d1
[000111c2] 3f41 004e                 move.w     d1,78(a7)
[000111c6] 3405                      move.w     d5,d2
[000111c8] d46f 005e                 add.w      94(a7),d2
[000111cc] 5b42                      subq.w     #5,d2
[000111ce] 3f42 004c                 move.w     d2,76(a7)
[000111d2] 700a                      moveq.l    #10,d0
[000111d4] d06f 0056                 add.w      86(a7),d0
[000111d8] 3f40 0050                 move.w     d0,80(a7)
[000111dc] 43ef 004a                 lea.l      74(a7),a1
[000111e0] 206b 0004                 movea.l    4(a3),a0 13a4e win
[000111e4] 2c6b 0826                 movea.l    2086(a3),a6 14270 redraw_window
[000111e8] 7015                      moveq.l    #21,d0
[000111ea] 4241                      clr.w      d1
[000111ec] 4e96                      jsr        (a6)
[000111ee] 3c12                      move.w     (a2),d6
[000111f0] 3203                      move.w     d3,d1
[000111f2] 48c1                      ext.l      d1
[000111f4] 2001                      move.l     d1,d0
[000111f6] d080                      add.l      d0,d0
[000111f8] d081                      add.l      d1,d0
[000111fa] e788                      lsl.l      #3,d0
[000111fc] 2053                      movea.l    (a3),a0 13a4a maintree
[000111fe] 3a30 0812                 move.w     18(a0,d0.l),d5
[00011202] 43eb 07f7                 lea.l      2039(a3),a1 14241
[00011206] 13b0 0813 3000            move.b     19(a0,d0.l),0(a1,d3.w)
[0001120c] 3415                      move.w     (a5),d2
[0001120e] 6600 ff38                 bne        $00011148
[00011212] 41d7                      lea.l      (a7),a0
[00011214] 4240                      clr.w      d0
[00011216] 6100 1f6e                 bsr        graf_mouse
[0001121a] 4241                      clr.w      d1
[0001121c] 7015                      moveq.l    #21,d0
[0001121e] 93c9                      suba.l     a1,a1
[00011220] 206b 0004                 movea.l    4(a3),a0 13a4e win
[00011224] 246b 0826                 movea.l    2086(a3),a2 14270 redraw_window
[00011228] 4e92                      jsr        (a2)
[0001122a] 41eb 07f7                 lea.l      2039(a3),a0 14241
[0001122e] 4244                      clr.w      d4
[00011230] 1830 3000                 move.b     0(a0,d3.w),d4
[00011234] 4a44                      tst.w      d4
[00011236] 6706                      beq.s      $0001123E
[00011238] b87c 00ff                 cmp.w      #$00FF,d4
[0001123c] 6624                      bne.s      $00011262
[0001123e] 3203                      move.w     d3,d1
[00011240] 48c1                      ext.l      d1
[00011242] 2001                      move.l     d1,d0
[00011244] d080                      add.l      d0,d0
[00011246] d081                      add.l      d1,d0
[00011248] e788                      lsl.l      #3,d0
[0001124a] 2053                      movea.l    (a3),a0 13a4a maintree
[0001124c] 3187 0812                 move.w     d7,18(a0,d0.l)
[00011250] 41eb 07f7                 lea.l      2039(a3),a0 14241
[00011254] 1187 3000                 move.b     d7,0(a0,d3.w)
[00011258] 70ea                      moveq.l    #-22,d0
[0001125a] d043                      add.w      d3,d0
[0001125c] 6100 00de                 bsr        x1133c
[00011260] 6042                      bra.s      $000112A4
[00011262] 4245                      clr.w      d5
[00011264] 6036                      bra.s      $0001129C
[00011266] 70ea                      moveq.l    #-22,d0
[00011268] d043                      add.w      d3,d0
[0001126a] ba40                      cmp.w      d0,d5
[0001126c] 672c                      beq.s      $0001129A
[0001126e] 41eb 080d                 lea.l      2061(a3),a0 14257
[00011272] 4241                      clr.w      d1
[00011274] 1230 5000                 move.b     0(a0,d5.w),d1
[00011278] b841                      cmp.w      d1,d4
[0001127a] 661e                      bne.s      $0001129A
[0001127c] 3c03                      move.w     d3,d6
[0001127e] 48c6                      ext.l      d6
[00011280] 2406                      move.l     d6,d2
[00011282] d482                      add.l      d2,d2
[00011284] d486                      add.l      d6,d2
[00011286] e78a                      lsl.l      #3,d2
[00011288] 2253                      movea.l    (a3),a1 13a4a maintree
[0001128a] 3387 2812                 move.w     d7,18(a1,d2.l)
[0001128e] 43eb 07f7                 lea.l      2039(a3),a1 14241
[00011292] 1387 3000                 move.b     d7,0(a1,d3.w)
[00011296] 6100 00a4                 bsr        x1133c
[0001129a] 5245                      addq.w     #1,d5
[0001129c] ba79 0001 37f4            cmp.w      num_datapoints,d5
[000112a2] 6dc2                      blt.s      $00011266
[000112a4] 7001                      moveq.l    #1,d0
[000112a6] 6002                      bra.s      $000112AA
[000112a8] 4240                      clr.w      d0
[000112aa] 4fef 0066                 lea.l      102(a7),a7
[000112ae] 4cdf 7cf8                 movem.l    (a7)+,d3-d7/a2-a6
[000112b2] 4e75                      rts

[000112b4] 48e7 1030                 movem.l    d3/a2-a3,-(a7)
[000112b8] 594f                      subq.w     #4,a7
[000112ba] 3600                      move.w     d0,d3
[000112bc] 45f9 0001 37f4            lea.l      num_datapoints,a2
[000112c2] 0c52 0010                 cmpi.w     #$0010,(a2)
[000112c6] 660e                      bne.s      $000112D6
[000112c8] 41f9 0001 399c            lea.l      $0001399C,a0
[000112ce] 7001                      moveq.l    #1,d0
[000112d0] 6100 1f04                 bsr        form_alert
[000112d4] 605e                      bra.s      $00011334
[000112d6] 47f9 0001 3a4a            lea.l      $00013A4A,a3
[000112dc] 4857                      pea.l      (a7)
[000112de] 43ef 0006                 lea.l      6(a7),a1
[000112e2] 7015                      moveq.l    #21,d0
[000112e4] 2053                      movea.l    (a3),a0 13a4a maintree
[000112e6] 6100 1eb4                 bsr        objc_offset
[000112ea] 584f                      addq.w     #4,a7
[000112ec] 1003                      move.b     d3,d0
[000112ee] 902f 0001                 sub.b      1(a7),d0
[000112f2] 3212                      move.w     (a2),d1
[000112f4] 41eb 080d                 lea.l      2061(a3),a0 14257
[000112f8] 1180 1000                 move.b     d0,0(a0,d1.w)
[000112fc] 3203                      move.w     d3,d1
[000112fe] 9257                      sub.w      (a7),d1
[00011300] 3012                      move.w     (a2),d0
[00011302] 48c0                      ext.l      d0
[00011304] 2400                      move.l     d0,d2
[00011306] d482                      add.l      d2,d2
[00011308] d480                      add.l      d0,d2
[0001130a] e78a                      lsl.l      #3,d2
[0001130c] 2253                      movea.l    (a3),a1 13a4a maintree
[0001130e] d3c2                      adda.l     d2,a1
[00011310] 3341 0222                 move.w     d1,546(a1)
[00011314] 3412                      move.w     (a2),d2
[00011316] 48c2                      ext.l      d2
[00011318] 2002                      move.l     d2,d0
[0001131a] d080                      add.l      d0,d0
[0001131c] d082                      add.l      d2,d0
[0001131e] e788                      lsl.l      #3,d0
[00011320] 2253                      movea.l    (a3),a1 13a4a maintree
[00011322] d3c0                      adda.l     d0,a1
[00011324] 0269 ff7f 0218            andi.w     #$FF7F,536(a1)
[0001132a] 7016                      moveq.l    #22,d0
[0001132c] d052                      add.w      (a2),d0
[0001132e] 6100 00a6                 bsr        x113d6
[00011332] 5252                      addq.w     #1,(a2)
[00011334] 584f                      addq.w     #4,a7
[00011336] 4cdf 0c08                 movem.l    (a7)+,d3/a2-a3
[0001133a] 4e75                      rts

[0001133c] 48e7 103e                 movem.l    d3/a2-a6,-(a7)
[00011340] 3600                      move.w     d0,d3
[00011342] 45f9 0001 4257            lea.l      $00014257,a2
[00011348] 47ea ffcd                 lea.l      -51(a2),a3 14224 datapoint_red
[0001134c] 49ea ffde                 lea.l      -34(a2),a4 14235 datapoint_green
[00011350] 4bea ffef                 lea.l      -17(a2),a5 14246 datapoint_blue
[00011354] 4df9 0001 37f4            lea.l      num_datapoints,a6
[0001135a] 6038                      bra.s      $00011394
[0001135c] 15b2 3001 3000            move.b     1(a2,d3.w),0(a2,d3.w)
[00011362] 17b3 3001 3000            move.b     1(a3,d3.w),0(a3,d3.w)
[00011368] 19b4 3001 3000            move.b     1(a4,d3.w),0(a4,d3.w)
[0001136e] 1bb5 3001 3000            move.b     1(a5,d3.w),0(a5,d3.w)
[00011374] 3203                      move.w     d3,d1
[00011376] 48c1                      ext.l      d1
[00011378] 2001                      move.l     d1,d0
[0001137a] d080                      add.l      d0,d0
[0001137c] d081                      add.l      d1,d0
[0001137e] e788                      lsl.l      #3,d0
[00011380] 206a f7f3                 movea.l    -2061(a2),a0 13a4a maintree
[00011384] d1c0                      adda.l     d0,a0
[00011386] 226a f7f3                 movea.l    -2061(a2),a1 13a4a maintree
[0001138a] d3c0                      adda.l     d0,a1
[0001138c] 3368 023a 0222            move.w     570(a0),546(a1)
[00011392] 5243                      addq.w     #1,d3
[00011394] b656                      cmp.w      (a6),d3
[00011396] 6fc4                      ble.s      $0001135C
[00011398] 3016                      move.w     (a6),d0
[0001139a] 4232 00ff                 clr.b      -1(a2,d0.w)
[0001139e] 3016                      move.w     (a6),d0
[000113a0] 4233 00ff                 clr.b      -1(a3,d0.w)
[000113a4] 3016                      move.w     (a6),d0
[000113a6] 4234 00ff                 clr.b      -1(a4,d0.w)
[000113aa] 3016                      move.w     (a6),d0
[000113ac] 4235 00ff                 clr.b      -1(a5,d0.w)
[000113b0] 3216                      move.w     (a6),d1
[000113b2] 48c1                      ext.l      d1
[000113b4] 2001                      move.l     d1,d0
[000113b6] d080                      add.l      d0,d0
[000113b8] d081                      add.l      d1,d0
[000113ba] e788                      lsl.l      #3,d0
[000113bc] 206a f7f3                 movea.l    -2061(a2),a0 13a4a maintree
[000113c0] d1c0                      adda.l     d0,a0
[000113c2] 0068 0080 0200            ori.w      #$0080,512(a0)
[000113c8] 5356                      subq.w     #1,(a6)
[000113ca] 7016                      moveq.l    #22,d0
[000113cc] 6100 0008                 bsr.w      $000113D6
[000113d0] 4cdf 7c08                 movem.l    (a7)+,d3/a2-a6
[000113d4] 4e75                      rts

[000113d6] 48e7 1830                 movem.l    d3-d4/a2-a3,-(a7)
[000113da] 3800                      move.w     d0,d4
[000113dc] 7616                      moveq.l    #22,d3
[000113de] 45f9 0001 3a4a            lea.l      $00013A4A,a2
[000113e4] 6016                      bra.s      $000113FC
[000113e6] 3203                      move.w     d3,d1
[000113e8] 48c1                      ext.l      d1
[000113ea] 2001                      move.l     d1,d0
[000113ec] d080                      add.l      d0,d0
[000113ee] d081                      add.l      d1,d0
[000113f0] e788                      lsl.l      #3,d0
[000113f2] 2052                      movea.l    (a2),a0 13a4a maintree
[000113f4] 0270 fffe 080a            andi.w     #$FFFE,10(a0,d0.l)
[000113fa] 5243                      addq.w     #1,d3
[000113fc] b67c 0025                 cmp.w      #$0025,d3
[00011400] 6de4                      blt.s      $000113E6
[00011402] 3204                      move.w     d4,d1
[00011404] 48c1                      ext.l      d1
[00011406] 2001                      move.l     d1,d0
[00011408] d080                      add.l      d0,d0
[0001140a] d081                      add.l      d1,d0
[0001140c] e788                      lsl.l      #3,d0
[0001140e] 2052                      movea.l    (a2),a0 13a4a maintree
[00011410] 0070 0001 080a            ori.w      #$0001,10(a0,d0.l)
[00011416] 93c9                      suba.l     a1,a1
[00011418] 206a 0004                 movea.l    4(a2),a0 13a4e win
[0001141c] 266a 0826                 movea.l    2086(a2),a3 14270 redraw_window
[00011420] 7015                      moveq.l    #21,d0
[00011422] 4241                      clr.w      d1
[00011424] 4e93                      jsr        (a3)
[00011426] 47f9 0001 37f6            lea.l      $000137F6,a3
[0001142c] 70ea                      moveq.l    #-22,d0
[0001142e] d044                      add.w      d4,d0
[00011430] 3680                      move.w     d0,(a3)
[00011432] 41ea 07da                 lea.l      2010(a2),a0 14224 datapoint_red
[00011436] 4241                      clr.w      d1
[00011438] 1230 0000                 move.b     0(a0,d0.w),d1
[0001143c] 3001                      move.w     d1,d0
[0001143e] 48c0                      ext.l      d0
[00011440] 41ea ffbe                 lea.l      -66(a2),a0 13a08
[00011444] 226a 0822                 movea.l    2082(a2),a1 1426c set_slider
[00011448] 4e91                      jsr        (a1)
[0001144a] 3013                      move.w     (a3),d0
[0001144c] 41ea 07eb                 lea.l      2027(a2),a0 14235 datapoint_green
[00011450] 4241                      clr.w      d1
[00011452] 1230 0000                 move.b     0(a0,d0.w),d1
[00011456] 3001                      move.w     d1,d0
[00011458] 48c0                      ext.l      d0
[0001145a] 41ea ffd4                 lea.l      -44(a2),a0 13a1e
[0001145e] 226a 0822                 movea.l    2082(a2),a1 1426c set_slider
[00011462] 4e91                      jsr        (a1)
[00011464] 3013                      move.w     (a3),d0
[00011466] 41ea 07fc                 lea.l      2044(a2),a0 14246 datapoint_blue
[0001146a] 4241                      clr.w      d1
[0001146c] 1230 0000                 move.b     0(a0,d0.w),d1
[00011470] 3001                      move.w     d1,d0
[00011472] 48c0                      ext.l      d0
[00011474] 41ea ffea                 lea.l      -22(a2),a0 13a34
[00011478] 226a 0822                 movea.l    2082(a2),a1 1426c set_slider
[0001147c] 4e91                      jsr        (a1)
[0001147e] 4cdf 0c18                 movem.l    (a7)+,d3-d4/a2-a3
[00011482] 4e75                      rts


	.data
[0001399c]                           dc.b '[1][Es k�nnen nur 16 St�tzpunkte|gesetzt werden!][ Ok ]',0
[000139d4]                           dc.b $00
[000139d5]                           dc.b $00
[000139d6]                           dc.b $00
[000139d7]                           dc.b $00
[000139d8]                           dc.b $00
[000139d9]                           dc.b $00
[000139da]                           dc.b $00
[000139db]                           dc.b $00
[000139dc]                           dc.b $00
[000139dd]                           dc.b $00
[000139de]                           dc.b $00
[000139df]                           dc.b $00
[000139e0]                           dc.b $00
[000139e1]                           dc.b $00
[000139e2]                           dc.b $00
[000139e3]                           dc.b $00
[000139e4]                           dc.b $00
[000139e5]                           dc.b $00
[000139e6]                           dc.b $00
[000139e7]                           dc.b $00
[000139e8]                           dc.b $00
[000139e9]                           dc.b $00
[000139ea]                           dc.b $00
[000139eb]                           dc.b $00
aespb
[000139ec] 00014274                  dc.l $00014274
[000139f0] 00014292                  dc.l $00014292
[000139f4] 000142b0                  dc.l $000142b0
[000139f8] 000143b8                  dc.l $000143b8
[000139fc] 000144d0                  dc.l $000144d0
[00013a00] 00014510                  dc.l $00014510

137f8 colrun_direction
137fa colrun_type

13a04 config
13a08 red_slider
13a1e green_slider
13a34 blue_slider
13a4a maintree
13a4e win
13a52 smurf_pic
13a56 preview;
13b58 

14268 mx
1426a my
1426c set_slider
14270 redraw_window
