[000104a4] 48e7 1f3c                 movem.l    d3-d7/a2-a5,-(a7)
[000104a8] 4eb9 0001 3220            jsr        init_timer
[000104ae] 45f9 0001 3a2e            lea.l      $00013A2E,a2
[000104b4] 203c 0000 0400            move.l     #$00000400,d0
[000104ba] 206a ffe8                 movea.l    -24(a2),a0 SMalloc
[000104be] 4e90                      jsr        (a0)
[000104c0] 2548 fff8                 move.l     a0,-8(a2) 13a26
[000104c4] 223c 0000 0400            move.l     #$00000400,d1
[000104ca] 4240                      clr.w      d0
[000104cc] 206a fff8                 movea.l    -8(a2),a0 13a26
[000104d0] 4eb9 0001 2dd4            jsr        memset

[000104d6] 4243                      clr.w      d3
[000104d8] 4244                      clr.w      d4
[000104da] 3003                      move.w     d3,d0
[000104dc] 48c0                      ext.l      d0
[000104de] ef88                      lsl.l      #7,d0
[000104e0] 3204                      move.w     d4,d1
[000104e2] 48c1                      ext.l      d1
[000104e4] e589                      lsl.l      #2,d1
[000104e6] d081                      add.l      d1,d0
[000104e8] 35bc fffd 0800            move.w     #$FFFD,0(a2,d0.l)
[000104ee] 3003                      move.w     d3,d0
[000104f0] 48c0                      ext.l      d0
[000104f2] ef88                      lsl.l      #7,d0
[000104f4] 3204                      move.w     d4,d1
[000104f6] 48c1                      ext.l      d1
[000104f8] e589                      lsl.l      #2,d1
[000104fa] d081                      add.l      d1,d0
[000104fc] 15bc 0005 0802            move.b     #$05,2(a2,d0.l)
[00010502] 5244                      addq.w     #1,d4
[00010504] b87c 0020                 cmp.w      #$0020,d4
[00010508] 6dd0                      blt.s      $000104DA
[0001050a] 5243                      addq.w     #1,d3
[0001050c] b67c 0100                 cmp.w      #$0100,d3
[00010510] 6dc6                      blt.s      $000104D8
[00010512] 4243                      clr.w      d3

[00010514] 47f9 0001 3272            lea.l      $00013272,a3
[0001051a] 3003                      move.w     d3,d0
[0001051c] d040                      add.w      d0,d0
[0001051e] d043                      add.w      d3,d0
[00010520] d040                      add.w      d0,d0
[00010522] 0c73 0008 0002            cmpi.w     #$0008,2(a3,d0.w)
[00010528] 6e5e                      bgt.s      $00010588

[0001052a] 7a08                      moveq.l    #8,d5
[0001052c] 3003                      move.w     d3,d0
[0001052e] d040                      add.w      d0,d0
[00010530] d043                      add.w      d3,d0
[00010532] d040                      add.w      d0,d0
[00010534] 9a33 0003                 sub.b      3(a3,d0.w),d5
[00010538] 4244                      clr.w      d4
[0001053a] 3003                      move.w     d3,d0
[0001053c] d040                      add.w      d0,d0
[0001053e] d043                      add.w      d3,d0
[00010540] d040                      add.w      d0,d0
[00010542] 3c33 0000                 move.w     0(a3,d0.w),d6
[00010546] eb6e                      lsl.w      d5,d6
[00010548] 8c44                      or.w       d4,d6
[0001054a] 3003                      move.w     d3,d0
[0001054c] d040                      add.w      d0,d0
[0001054e] d043                      add.w      d3,d0
[00010550] d040                      add.w      d0,d0
[00010552] 3206                      move.w     d6,d1
[00010554] 48c1                      ext.l      d1
[00010556] e589                      lsl.l      #2,d1
[00010558] 206a fff8                 movea.l    -8(a2),a0 13a26
[0001055c] 31b3 0004 1800            move.w     4(a3,d0.w),0(a0,d1.l)
[00010562] 3003                      move.w     d3,d0
[00010564] d040                      add.w      d0,d0
[00010566] d043                      add.w      d3,d0
[00010568] d040                      add.w      d0,d0
[0001056a] 3206                      move.w     d6,d1
[0001056c] 48c1                      ext.l      d1
[0001056e] e589                      lsl.l      #2,d1
[00010570] 206a fff8                 movea.l    -8(a2),a0 13a26
[00010574] 11b3 0003 1802            move.b     3(a3,d0.w),2(a0,d1.l)
[0001057a] 7001                      moveq.l    #1,d0
[0001057c] eb68                      lsl.w      d5,d0
[0001057e] 5244                      addq.w     #1,d4
[00010580] b840                      cmp.w      d0,d4
[00010582] 6db6                      blt.s      $0001053A
[00010584] 6000 00be                 bra        $00010644

[00010588] 3003                      move.w     d3,d0
[0001058a] d040                      add.w      d0,d0
[0001058c] d043                      add.w      d3,d0
[0001058e] d040                      add.w      d0,d0
[00010590] 3e33 0000                 move.w     0(a3,d0.w),d7
[00010594] 70f8                      moveq.l    #-8,d0
[00010596] 3203                      move.w     d3,d1
[00010598] d241                      add.w      d1,d1
[0001059a] d243                      add.w      d3,d1
[0001059c] d241                      add.w      d1,d1
[0001059e] d033 1003                 add.b      3(a3,d1.w),d0
[000105a2] e067                      asr.w      d0,d7
[000105a4] 3007                      move.w     d7,d0
[000105a6] 48c0                      ext.l      d0
[000105a8] e588                      lsl.l      #2,d0
[000105aa] 206a fff8                 movea.l    -8(a2),a0 13a26
[000105ae] 31bc ffff 0800            move.w     #$FFFF,0(a0,d0.l)
[000105b4] 3007                      move.w     d7,d0
[000105b6] 48c0                      ext.l      d0
[000105b8] e588                      lsl.l      #2,d0
[000105ba] 206a fff8                 movea.l    -8(a2),a0 13a26
[000105be] 1187 0803                 move.b     d7,3(a0,d0.l)
[000105c2] 3007                      move.w     d7,d0
[000105c4] 48c0                      ext.l      d0
[000105c6] e588                      lsl.l      #2,d0
[000105c8] 206a fff8                 movea.l    -8(a2),a0 13a26
[000105cc] 11bc 0008 0802            move.b     #$08,2(a0,d0.l)
[000105d2] 7a05                      moveq.l    #5,d5
[000105d4] 3003                      move.w     d3,d0
[000105d6] d040                      add.w      d0,d0
[000105d8] d043                      add.w      d3,d0
[000105da] d040                      add.w      d0,d0
[000105dc] 1033 0003                 move.b     3(a3,d0.w),d0
[000105e0] d03c 00f8                 add.b      #$F8,d0
[000105e4] 9a00                      sub.b      d0,d5
[000105e6] 4244                      clr.w      d4

[000105e8] 3003                      move.w     d3,d0
[000105ea] d040                      add.w      d0,d0
[000105ec] d043                      add.w      d3,d0
[000105ee] d040                      add.w      d0,d0
[000105f0] 3c33 0000                 move.w     0(a3,d0.w),d6
[000105f4] eb6e                      lsl.w      d5,d6
[000105f6] 8c44                      or.w       d4,d6
[000105f8] cc6b ffd6                 and.w      -42(a3),d6 /* 13248
[000105fc] 3003                      move.w     d3,d0
[000105fe] d040                      add.w      d0,d0
[00010600] d043                      add.w      d3,d0
[00010602] d040                      add.w      d0,d0
[00010604] 3207                      move.w     d7,d1
[00010606] 48c1                      ext.l      d1
[00010608] ef89                      lsl.l      #7,d1
[0001060a] 3406                      move.w     d6,d2
[0001060c] 48c2                      ext.l      d2
[0001060e] e58a                      lsl.l      #2,d2
[00010610] d282                      add.l      d2,d1
[00010612] 35b3 0004 1800            move.w     4(a3,d0.w),0(a2,d1.l)
[00010618] 3003                      move.w     d3,d0
[0001061a] d040                      add.w      d0,d0
[0001061c] d043                      add.w      d3,d0
[0001061e] d040                      add.w      d0,d0
[00010620] 1033 0003                 move.b     3(a3,d0.w),d0
[00010624] d03c 00f8                 add.b      #$F8,d0
[00010628] 3207                      move.w     d7,d1
[0001062a] 48c1                      ext.l      d1
[0001062c] ef89                      lsl.l      #7,d1
[0001062e] 3406                      move.w     d6,d2
[00010630] 48c2                      ext.l      d2
[00010632] e58a                      lsl.l      #2,d2
[00010634] d282                      add.l      d2,d1
[00010636] 1580 1802                 move.b     d0,2(a2,d1.l)
[0001063a] 7001                      moveq.l    #1,d0
[0001063c] eb68                      lsl.w      d5,d0
[0001063e] 5244                      addq.w     #1,d4
[00010640] b840                      cmp.w      d0,d4
[00010642] 6da4                      blt.s      $000105E8
[00010644] 5243                      addq.w     #1,d3
[00010646] b67c 0040                 cmp.w      #$0040,d3
[0001064a] 6d00 fec8                 blt        $00010514

[0001064e] 4243                      clr.w      d3

[00010650] 49eb 0180                 lea.l      384(a3),a4
[00010654] 3003                      move.w     d3,d0
[00010656] d040                      add.w      d0,d0
[00010658] d043                      add.w      d3,d0
[0001065a] d040                      add.w      d0,d0
[0001065c] 0c74 0008 0002            cmpi.w     #$0008,2(a4,d0.w)
[00010662] 6e5e                      bgt.s      $000106C2
[00010664] 7a08                      moveq.l    #8,d5
[00010666] 3003                      move.w     d3,d0
[00010668] d040                      add.w      d0,d0
[0001066a] d043                      add.w      d3,d0
[0001066c] d040                      add.w      d0,d0
[0001066e] 9a34 0003                 sub.b      3(a4,d0.w),d5
[00010672] 4244                      clr.w      d4
[00010674] 3003                      move.w     d3,d0
[00010676] d040                      add.w      d0,d0
[00010678] d043                      add.w      d3,d0
[0001067a] d040                      add.w      d0,d0
[0001067c] 3c34 0000                 move.w     0(a4,d0.w),d6
[00010680] eb6e                      lsl.w      d5,d6
[00010682] 8c44                      or.w       d4,d6
[00010684] 3003                      move.w     d3,d0
[00010686] d040                      add.w      d0,d0
[00010688] d043                      add.w      d3,d0
[0001068a] d040                      add.w      d0,d0
[0001068c] 3206                      move.w     d6,d1
[0001068e] 48c1                      ext.l      d1
[00010690] e589                      lsl.l      #2,d1
[00010692] 206a fff8                 movea.l    -8(a2),a0 13a26
[00010696] 31b4 0004 1800            move.w     4(a4,d0.w),0(a0,d1.l)
[0001069c] 3003                      move.w     d3,d0
[0001069e] d040                      add.w      d0,d0
[000106a0] d043                      add.w      d3,d0
[000106a2] d040                      add.w      d0,d0
[000106a4] 3206                      move.w     d6,d1
[000106a6] 48c1                      ext.l      d1
[000106a8] e589                      lsl.l      #2,d1
[000106aa] 206a fff8                 movea.l    -8(a2),a0 13a26
[000106ae] 11b4 0003 1802            move.b     3(a4,d0.w),2(a0,d1.l)
[000106b4] 7001                      moveq.l    #1,d0
[000106b6] eb68                      lsl.w      d5,d0
[000106b8] 5244                      addq.w     #1,d4
[000106ba] b840                      cmp.w      d0,d4
[000106bc] 6db6                      blt.s      $00010674
[000106be] 6000 00be                 bra        $0001077E

[000106c2] 3003                      move.w     d3,d0
[000106c4] d040                      add.w      d0,d0
[000106c6] d043                      add.w      d3,d0
[000106c8] d040                      add.w      d0,d0
[000106ca] 3e34 0000                 move.w     0(a4,d0.w),d7
[000106ce] 70f8                      moveq.l    #-8,d0
[000106d0] 3203                      move.w     d3,d1
[000106d2] d241                      add.w      d1,d1
[000106d4] d243                      add.w      d3,d1
[000106d6] d241                      add.w      d1,d1
[000106d8] d034 1003                 add.b      3(a4,d1.w),d0
[000106dc] e067                      asr.w      d0,d7
[000106de] 3007                      move.w     d7,d0
[000106e0] 48c0                      ext.l      d0
[000106e2] e588                      lsl.l      #2,d0
[000106e4] 206a fff8                 movea.l    -8(a2),a0 13a26
[000106e8] 31bc ffff 0800            move.w     #$FFFF,0(a0,d0.l)
[000106ee] 3007                      move.w     d7,d0
[000106f0] 48c0                      ext.l      d0
[000106f2] e588                      lsl.l      #2,d0
[000106f4] 206a fff8                 movea.l    -8(a2),a0 13a26
[000106f8] 1187 0803                 move.b     d7,3(a0,d0.l)
[000106fc] 3007                      move.w     d7,d0
[000106fe] 48c0                      ext.l      d0
[00010700] e588                      lsl.l      #2,d0
[00010702] 206a fff8                 movea.l    -8(a2),a0 13a26
[00010706] 11bc 0008 0802            move.b     #$08,2(a0,d0.l)
[0001070c] 7a05                      moveq.l    #5,d5
[0001070e] 3003                      move.w     d3,d0
[00010710] d040                      add.w      d0,d0
[00010712] d043                      add.w      d3,d0
[00010714] d040                      add.w      d0,d0
[00010716] 1034 0003                 move.b     3(a4,d0.w),d0
[0001071a] d03c 00f8                 add.b      #$F8,d0
[0001071e] 9a00                      sub.b      d0,d5
[00010720] 4244                      clr.w      d4

[00010722] 3003                      move.w     d3,d0
[00010724] d040                      add.w      d0,d0
[00010726] d043                      add.w      d3,d0
[00010728] d040                      add.w      d0,d0
[0001072a] 3c34 0000                 move.w     0(a4,d0.w),d6
[0001072e] eb6e                      lsl.w      d5,d6
[00010730] 8c44                      or.w       d4,d6
[00010732] cc6b ffd6                 and.w      -42(a3),d6
[00010736] 3003                      move.w     d3,d0
[00010738] d040                      add.w      d0,d0
[0001073a] d043                      add.w      d3,d0
[0001073c] d040                      add.w      d0,d0
[0001073e] 3207                      move.w     d7,d1
[00010740] 48c1                      ext.l      d1
[00010742] ef89                      lsl.l      #7,d1
[00010744] 3406                      move.w     d6,d2
[00010746] 48c2                      ext.l      d2
[00010748] e58a                      lsl.l      #2,d2
[0001074a] d282                      add.l      d2,d1
[0001074c] 35b4 0004 1800            move.w     4(a4,d0.w),0(a2,d1.l)
[00010752] 3003                      move.w     d3,d0
[00010754] d040                      add.w      d0,d0
[00010756] d043                      add.w      d3,d0
[00010758] d040                      add.w      d0,d0
[0001075a] 1034 0003                 move.b     3(a4,d0.w),d0
[0001075e] d03c 00f8                 add.b      #$F8,d0
[00010762] 3207                      move.w     d7,d1
[00010764] 48c1                      ext.l      d1
[00010766] ef89                      lsl.l      #7,d1
[00010768] 3406                      move.w     d6,d2
[0001076a] 48c2                      ext.l      d2
[0001076c] e58a                      lsl.l      #2,d2
[0001076e] d282                      add.l      d2,d1
[00010770] 1580 1802                 move.b     d0,2(a2,d1.l)
[00010774] 7001                      moveq.l    #1,d0
[00010776] eb68                      lsl.w      d5,d0
[00010778] 5244                      addq.w     #1,d4
[0001077a] b840                      cmp.w      d0,d4
[0001077c] 6da4                      blt.s      $00010722
[0001077e] 5243                      addq.w     #1,d3
[00010780] b67c 001c                 cmp.w      #$001C,d3
[00010784] 6d00 feca                 blt        $00010650

[00010788] 4eb9 0001 322e            jsr        get_timer
[0001078e] 2f00                      move.l     d0,-(a7)
[00010790] 41f9 0001 3911            lea.l      $00013911,a0 "Zeit: %lu"
[00010796] 4eb9 0001 264e            jsr        printf
[0001079c] 584f                      addq.w     #4,a7
[0001079e] 4eb9 0001 3220            jsr        init_timer

[000107a4] 203c 0000 0400            move.l     #$00000400,d0
[000107aa] 206a ffe8                 movea.l    -24(a2),a0 SMalloc
[000107ae] 4e90                      jsr        (a0)
[000107b0] 2548 fffc                 move.l     a0,-4(a2) 13a2a
[000107b4] 223c 0000 0400            move.l     #$00000400,d1
[000107ba] 4240                      clr.w      d0
[000107bc] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000107c0] 4eb9 0001 2dd4            jsr        memset

[000107c6] 4243                      clr.w      d3
[000107c8] 49f9 0001 ba2e            lea.l      $0001BA2E,a4
[000107ce] 4244                      clr.w      d4
[000107d0] 3003                      move.w     d3,d0
[000107d2] 48c0                      ext.l      d0
[000107d4] ef88                      lsl.l      #7,d0
[000107d6] 3204                      move.w     d4,d1
[000107d8] 48c1                      ext.l      d1
[000107da] e589                      lsl.l      #2,d1
[000107dc] d081                      add.l      d1,d0
[000107de] 39bc fffd 0800            move.w     #$FFFD,0(a4,d0.l)
[000107e4] 3003                      move.w     d3,d0
[000107e6] 48c0                      ext.l      d0
[000107e8] ef88                      lsl.l      #7,d0
[000107ea] 3204                      move.w     d4,d1
[000107ec] 48c1                      ext.l      d1
[000107ee] e589                      lsl.l      #2,d1
[000107f0] d081                      add.l      d1,d0
[000107f2] 19bc 0005 0802            move.b     #$05,2(a4,d0.l)
[000107f8] 5244                      addq.w     #1,d4
[000107fa] b87c 0020                 cmp.w      #$0020,d4
[000107fe] 6dd0                      blt.s      $000107D0
[00010800] 5243                      addq.w     #1,d3
[00010802] b67c 0100                 cmp.w      #$0100,d3
[00010806] 6dc0                      blt.s      $000107C8

[00010808] 4243                      clr.w      d3
[0001080a] 4beb 0228                 lea.l      552(a3),a5
[0001080e] 3003                      move.w     d3,d0
[00010810] d040                      add.w      d0,d0
[00010812] d043                      add.w      d3,d0
[00010814] d040                      add.w      d0,d0
[00010816] 0c75 0008 0002            cmpi.w     #$0008,2(a5,d0.w)
[0001081c] 6e5e                      bgt.s      $0001087C
[0001081e] 7a08                      moveq.l    #8,d5
[00010820] 3003                      move.w     d3,d0
[00010822] d040                      add.w      d0,d0
[00010824] d043                      add.w      d3,d0
[00010826] d040                      add.w      d0,d0
[00010828] 9a35 0003                 sub.b      3(a5,d0.w),d5
[0001082c] 4244                      clr.w      d4
[0001082e] 3003                      move.w     d3,d0
[00010830] d040                      add.w      d0,d0
[00010832] d043                      add.w      d3,d0
[00010834] d040                      add.w      d0,d0
[00010836] 3c35 0000                 move.w     0(a5,d0.w),d6
[0001083a] eb6e                      lsl.w      d5,d6
[0001083c] 8c44                      or.w       d4,d6
[0001083e] 3003                      move.w     d3,d0
[00010840] d040                      add.w      d0,d0
[00010842] d043                      add.w      d3,d0
[00010844] d040                      add.w      d0,d0
[00010846] 3206                      move.w     d6,d1
[00010848] 48c1                      ext.l      d1
[0001084a] e589                      lsl.l      #2,d1
[0001084c] 206a fffc                 movea.l    -4(a2),a0 13a2a
[00010850] 31b5 0004 1800            move.w     4(a5,d0.w),0(a0,d1.l)
[00010856] 3003                      move.w     d3,d0
[00010858] d040                      add.w      d0,d0
[0001085a] d043                      add.w      d3,d0
[0001085c] d040                      add.w      d0,d0
[0001085e] 3206                      move.w     d6,d1
[00010860] 48c1                      ext.l      d1
[00010862] e589                      lsl.l      #2,d1
[00010864] 206a fffc                 movea.l    -4(a2),a0 13a2a
[00010868] 11b5 0003 1802            move.b     3(a5,d0.w),2(a0,d1.l)
[0001086e] 7001                      moveq.l    #1,d0
[00010870] eb68                      lsl.w      d5,d0
[00010872] 5244                      addq.w     #1,d4
[00010874] b840                      cmp.w      d0,d4
[00010876] 6db6                      blt.s      $0001082E
[00010878] 6000 00be                 bra        $00010938
[0001087c] 3003                      move.w     d3,d0
[0001087e] d040                      add.w      d0,d0
[00010880] d043                      add.w      d3,d0
[00010882] d040                      add.w      d0,d0
[00010884] 3e35 0000                 move.w     0(a5,d0.w),d7
[00010888] 70f8                      moveq.l    #-8,d0
[0001088a] 3203                      move.w     d3,d1
[0001088c] d241                      add.w      d1,d1
[0001088e] d243                      add.w      d3,d1
[00010890] d241                      add.w      d1,d1
[00010892] d035 1003                 add.b      3(a5,d1.w),d0
[00010896] e067                      asr.w      d0,d7
[00010898] 3007                      move.w     d7,d0
[0001089a] 48c0                      ext.l      d0
[0001089c] e588                      lsl.l      #2,d0
[0001089e] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000108a2] 31bc ffff 0800            move.w     #$FFFF,0(a0,d0.l)
[000108a8] 3007                      move.w     d7,d0
[000108aa] 48c0                      ext.l      d0
[000108ac] e588                      lsl.l      #2,d0
[000108ae] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000108b2] 1187 0803                 move.b     d7,3(a0,d0.l)
[000108b6] 3007                      move.w     d7,d0
[000108b8] 48c0                      ext.l      d0
[000108ba] e588                      lsl.l      #2,d0
[000108bc] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000108c0] 11bc 0008 0802            move.b     #$08,2(a0,d0.l)
[000108c6] 7a05                      moveq.l    #5,d5
[000108c8] 3003                      move.w     d3,d0
[000108ca] d040                      add.w      d0,d0
[000108cc] d043                      add.w      d3,d0
[000108ce] d040                      add.w      d0,d0
[000108d0] 1035 0003                 move.b     3(a5,d0.w),d0
[000108d4] d03c 00f8                 add.b      #$F8,d0
[000108d8] 9a00                      sub.b      d0,d5
[000108da] 4244                      clr.w      d4
[000108dc] 3003                      move.w     d3,d0
[000108de] d040                      add.w      d0,d0
[000108e0] d043                      add.w      d3,d0
[000108e2] d040                      add.w      d0,d0
[000108e4] 3c35 0000                 move.w     0(a5,d0.w),d6
[000108e8] eb6e                      lsl.w      d5,d6
[000108ea] 8c44                      or.w       d4,d6
[000108ec] cc6b ffd6                 and.w      -42(a3),d6
[000108f0] 3003                      move.w     d3,d0
[000108f2] d040                      add.w      d0,d0
[000108f4] d043                      add.w      d3,d0
[000108f6] d040                      add.w      d0,d0
[000108f8] 3207                      move.w     d7,d1
[000108fa] 48c1                      ext.l      d1
[000108fc] ef89                      lsl.l      #7,d1
[000108fe] 3406                      move.w     d6,d2
[00010900] 48c2                      ext.l      d2
[00010902] e58a                      lsl.l      #2,d2
[00010904] d282                      add.l      d2,d1
[00010906] 39b5 0004 1800            move.w     4(a5,d0.w),0(a4,d1.l)
[0001090c] 3003                      move.w     d3,d0
[0001090e] d040                      add.w      d0,d0
[00010910] d043                      add.w      d3,d0
[00010912] d040                      add.w      d0,d0
[00010914] 1035 0003                 move.b     3(a5,d0.w),d0
[00010918] d03c 00f8                 add.b      #$F8,d0
[0001091c] 3207                      move.w     d7,d1
[0001091e] 48c1                      ext.l      d1
[00010920] ef89                      lsl.l      #7,d1
[00010922] 3406                      move.w     d6,d2
[00010924] 48c2                      ext.l      d2
[00010926] e58a                      lsl.l      #2,d2
[00010928] d282                      add.l      d2,d1
[0001092a] 1980 1802                 move.b     d0,2(a4,d1.l)
[0001092e] 7001                      moveq.l    #1,d0
[00010930] eb68                      lsl.w      d5,d0
[00010932] 5244                      addq.w     #1,d4
[00010934] b840                      cmp.w      d0,d4
[00010936] 6da4                      blt.s      $000108DC
[00010938] 5243                      addq.w     #1,d3
[0001093a] b67c 0040                 cmp.w      #$0040,d3
[0001093e] 6d00 feca                 blt        $0001080A

[00010942] 4243                      clr.w      d3
[00010944] 4beb 03a8                 lea.l      936(a3),a5
[00010948] 3003                      move.w     d3,d0
[0001094a] d040                      add.w      d0,d0
[0001094c] d043                      add.w      d3,d0
[0001094e] d040                      add.w      d0,d0
[00010950] 0c75 0008 0002            cmpi.w     #$0008,2(a5,d0.w)
[00010956] 6e5e                      bgt.s      $000109B6
[00010958] 7a08                      moveq.l    #8,d5
[0001095a] 3003                      move.w     d3,d0
[0001095c] d040                      add.w      d0,d0
[0001095e] d043                      add.w      d3,d0
[00010960] d040                      add.w      d0,d0
[00010962] 9a35 0003                 sub.b      3(a5,d0.w),d5
[00010966] 4244                      clr.w      d4
[00010968] 3003                      move.w     d3,d0
[0001096a] d040                      add.w      d0,d0
[0001096c] d043                      add.w      d3,d0
[0001096e] d040                      add.w      d0,d0
[00010970] 3c35 0000                 move.w     0(a5,d0.w),d6
[00010974] eb6e                      lsl.w      d5,d6
[00010976] 8c44                      or.w       d4,d6
[00010978] 3003                      move.w     d3,d0
[0001097a] d040                      add.w      d0,d0
[0001097c] d043                      add.w      d3,d0
[0001097e] d040                      add.w      d0,d0
[00010980] 3206                      move.w     d6,d1
[00010982] 48c1                      ext.l      d1
[00010984] e589                      lsl.l      #2,d1
[00010986] 206a fffc                 movea.l    -4(a2),a0 13a2a
[0001098a] 31b5 0004 1800            move.w     4(a5,d0.w),0(a0,d1.l)
[00010990] 3003                      move.w     d3,d0
[00010992] d040                      add.w      d0,d0
[00010994] d043                      add.w      d3,d0
[00010996] d040                      add.w      d0,d0
[00010998] 3206                      move.w     d6,d1
[0001099a] 48c1                      ext.l      d1
[0001099c] e589                      lsl.l      #2,d1
[0001099e] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000109a2] 11b5 0003 1802            move.b     3(a5,d0.w),2(a0,d1.l)
[000109a8] 7001                      moveq.l    #1,d0
[000109aa] eb68                      lsl.w      d5,d0
[000109ac] 5244                      addq.w     #1,d4
[000109ae] b840                      cmp.w      d0,d4
[000109b0] 6db6                      blt.s      $00010968
[000109b2] 6000 00be                 bra        $00010A72
[000109b6] 3003                      move.w     d3,d0
[000109b8] d040                      add.w      d0,d0
[000109ba] d043                      add.w      d3,d0
[000109bc] d040                      add.w      d0,d0
[000109be] 3e35 0000                 move.w     0(a5,d0.w),d7
[000109c2] 70f8                      moveq.l    #-8,d0
[000109c4] 3203                      move.w     d3,d1
[000109c6] d241                      add.w      d1,d1
[000109c8] d243                      add.w      d3,d1
[000109ca] d241                      add.w      d1,d1
[000109cc] d035 1003                 add.b      3(a5,d1.w),d0
[000109d0] e067                      asr.w      d0,d7
[000109d2] 3007                      move.w     d7,d0
[000109d4] 48c0                      ext.l      d0
[000109d6] e588                      lsl.l      #2,d0
[000109d8] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000109dc] 31bc ffff 0800            move.w     #$FFFF,0(a0,d0.l)
[000109e2] 3007                      move.w     d7,d0
[000109e4] 48c0                      ext.l      d0
[000109e6] e588                      lsl.l      #2,d0
[000109e8] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000109ec] 1187 0803                 move.b     d7,3(a0,d0.l)
[000109f0] 3007                      move.w     d7,d0
[000109f2] 48c0                      ext.l      d0
[000109f4] e588                      lsl.l      #2,d0
[000109f6] 206a fffc                 movea.l    -4(a2),a0 13a2a
[000109fa] 11bc 0008 0802            move.b     #$08,2(a0,d0.l)
[00010a00] 7a05                      moveq.l    #5,d5
[00010a02] 3003                      move.w     d3,d0
[00010a04] d040                      add.w      d0,d0
[00010a06] d043                      add.w      d3,d0
[00010a08] d040                      add.w      d0,d0
[00010a0a] 1035 0003                 move.b     3(a5,d0.w),d0
[00010a0e] d03c 00f8                 add.b      #$F8,d0
[00010a12] 9a00                      sub.b      d0,d5
[00010a14] 4244                      clr.w      d4
[00010a16] 3003                      move.w     d3,d0
[00010a18] d040                      add.w      d0,d0
[00010a1a] d043                      add.w      d3,d0
[00010a1c] d040                      add.w      d0,d0
[00010a1e] 3c35 0000                 move.w     0(a5,d0.w),d6
[00010a22] eb6e                      lsl.w      d5,d6
[00010a24] 8c44                      or.w       d4,d6
[00010a26] cc6b ffd6                 and.w      -42(a3),d6
[00010a2a] 3003                      move.w     d3,d0
[00010a2c] d040                      add.w      d0,d0
[00010a2e] d043                      add.w      d3,d0
[00010a30] d040                      add.w      d0,d0
[00010a32] 3207                      move.w     d7,d1
[00010a34] 48c1                      ext.l      d1
[00010a36] ef89                      lsl.l      #7,d1
[00010a38] 3406                      move.w     d6,d2
[00010a3a] 48c2                      ext.l      d2
[00010a3c] e58a                      lsl.l      #2,d2
[00010a3e] d282                      add.l      d2,d1
[00010a40] 39b5 0004 1800            move.w     4(a5,d0.w),0(a4,d1.l)
[00010a46] 3003                      move.w     d3,d0
[00010a48] d040                      add.w      d0,d0
[00010a4a] d043                      add.w      d3,d0
[00010a4c] d040                      add.w      d0,d0
[00010a4e] 1035 0003                 move.b     3(a5,d0.w),d0
[00010a52] d03c 00f8                 add.b      #$F8,d0
[00010a56] 3207                      move.w     d7,d1
[00010a58] 48c1                      ext.l      d1
[00010a5a] ef89                      lsl.l      #7,d1
[00010a5c] 3406                      move.w     d6,d2
[00010a5e] 48c2                      ext.l      d2
[00010a60] e58a                      lsl.l      #2,d2
[00010a62] d282                      add.l      d2,d1
[00010a64] 1980 1802                 move.b     d0,2(a4,d1.l)
[00010a68] 7001                      moveq.l    #1,d0
[00010a6a] eb68                      lsl.w      d5,d0
[00010a6c] 5244                      addq.w     #1,d4
[00010a6e] b840                      cmp.w      d0,d4
[00010a70] 6da4                      blt.s      $00010A16
[00010a72] 5243                      addq.w     #1,d3
[00010a74] b67c 001c                 cmp.w      #$001C,d3
[00010a78] 6d00 feca                 blt        $00010944
[00010a7c] 4eb9 0001 322e            jsr        get_timer
[00010a82] 2f00                      move.l     d0,-(a7)
[00010a84] 41f9 0001 391c            lea.l      $0001391C,a0 "Zeit: %lu"
[00010a8a] 4eb9 0001 264e            jsr        printf
[00010a90] 584f                      addq.w     #4,a7
[00010a92] 4cdf 3cf8                 movem.l    (a7)+,d3-d7/a2-a5
[00010a96] 4e75                      rts


[00010a98] 2f0a                      move.l     a2,-(a7)
[00010a9a] 2f0b                      move.l     a3,-(a7)
[00010a9c] 2448                      movea.l    a0,a2
[00010a9e] 702e                      moveq.l    #46,d0
[00010aa0] 204a                      movea.l    a2,a0
[00010aa2] 4eb9 0001 2bfe            jsr        strrchr
[00010aa8] 2648                      movea.l    a0,a3
[00010aaa] 200b                      move.l     a3,d0
[00010aac] 6704                      beq.s      $00010AB2
[00010aae] 524b                      addq.w     #1,a3
[00010ab0] 600c                      bra.s      $00010ABE
[00010ab2] 4240                      clr.w      d0
[00010ab4] 204a                      movea.l    a2,a0
[00010ab6] 4eb9 0001 2bfe            jsr        strrchr
[00010abc] 2648                      movea.l    a0,a3
[00010abe] 204b                      movea.l    a3,a0
[00010ac0] 265f                      movea.l    (a7)+,a3
[00010ac2] 245f                      movea.l    (a7)+,a2
[00010ac4] 4e75                      rts

_PrintF:
_OutFlt:
fcvt:
ftoa:
pow10:
printf:
_OutIntD:
_OutCarD:
_OutCarH:
_OutCarO:
_OutChr:
_OutStr:
OutZero:
OutBlank:
fwrite:
_FlshBuf:
write:
lseek:
_XltErr:
itoa:

strtoul:
ultoa:
strnicmp:
memset:


init_timer:
[00013220] 4eb9 0001 2636            jsr        $00012636
[00013226] 23c0 0002 3d36            move.l     d0,$00023D36
[0001322c] 4e75                      rts
get_timer:
[0001322e] 4eb9 0001 2636            jsr        $00012636
[00013234] 90b9 0002 3d36            sub.l      $00023D36,d0
[0001323a] 4e75                      rts

	.data
[0001323c]                           dc.b $0000

[0001323e]                           dc.b $0000
[00013240]                           dc.b $0001
[00013242]                           dc.b $0003
[00013244]                           dc.b $0007
[00013246]                           dc.b $000f
[00013248]                           dc.b $001f
[0001324a]                           dc.b $003f
[0001324c]                           dc.b $007f
[0001324e]                           dc.b $00ff

[00013250]                           dc.b $0080
[00013252]                           dc.b $0040
[00013254]                           dc.b $0020
[00013256]                           dc.b $0010
[00013258]                           dc.b $0008
[0001325a]                           dc.b $0004
[0001325c]                           dc.b $0002
[0001325e]                           dc.b $0001
[00013260]                           dc.b $0000

[00013262]                           dc.w $8000
[00013264]                           dc.w $c000
[00013266]                           dc.w $e000
[00013268]                           dc.w $f000
[0001326a]                           dc.w $f800
[0001326c]                           dc.w $fc00
[0001326e]                           dc.w $fe00
[00013270]                           dc.w $ff00
[00013272]                           dc.b $0035
[00013274]                           dc.b $0008
[00013276]                           dc.b $0000
[00013278]                           dc.b $0007
[0001327a]                           dc.b $0006
[0001327c]                           dc.b $0001
[0001327e]                           dc.b $0007
[00013280]                           dc.b $0004
[00013282]                           dc.b $0002
[00013284]                           dc.b $0008
[00013286]                           dc.b $0004
[00013288]                           dc.b $0003
[0001328a]                           dc.b $000b
[0001328c]                           dc.b $0004
[0001328e]                           dc.b $0004
[00013290]                           dc.b $000c
[00013292]                           dc.b $0004
[00013294]                           dc.b $0005
[00013296]                           dc.b $000e
[00013298]                           dc.b $0004
[0001329a]                           dc.b $0006
[0001329c]                           dc.b $000f
[0001329e]                           dc.b $0004
[000132a0]                           dc.b $0007
[000132a2]                           dc.b $0013
[000132a4]                           dc.b $0005
[000132a6]                           dc.b $0008
[000132a8]                           dc.b $0014
[000132aa]                           dc.b $0005
[000132ac]                           dc.b $0009
[000132ae]                           dc.b $0007
[000132b0]                           dc.b $0005
[000132b2]                           dc.b $000a
[000132b4]                           dc.b $0008
[000132b6]                           dc.b $0005
[000132b8]                           dc.b $000b
[000132ba]                           dc.b $0008
[000132bc]                           dc.b $0006
[000132be]                           dc.b $000c
[000132c0]                           dc.b $0003
[000132c2]                           dc.b $0006
[000132c4]                           dc.b $000d
[000132c6]                           dc.b $0034
[000132c8]                           dc.b $0006
[000132ca]                           dc.b $000e
[000132cc]                           dc.b $0035
[000132ce]                           dc.b $0006
[000132d0]                           dc.b $000f
[000132d2]                           dc.b $002a
[000132d4]                           dc.b $0006
[000132d6]                           dc.b $0010
[000132d8]                           dc.b $002b
[000132da]                           dc.b $0006
[000132dc]                           dc.b $0011
[000132de]                           dc.b $0027
[000132e0]                           dc.b $0007
[000132e2]                           dc.b $0012
[000132e4]                           dc.b $000c
[000132e6]                           dc.b $0007
[000132e8]                           dc.b $0013
[000132ea]                           dc.b $0008
[000132ec]                           dc.b $0007
[000132ee]                           dc.b $0014
[000132f0]                           dc.b $0017
[000132f2]                           dc.b $0007
[000132f4]                           dc.b $0015
[000132f6]                           dc.b $0003
[000132f8]                           dc.b $0007
[000132fa]                           dc.b $0016
[000132fc]                           dc.b $0004
[000132fe]                           dc.b $0007
[00013300]                           dc.b $0017
[00013302]                           dc.b $0028
[00013304]                           dc.b $0007
[00013306]                           dc.b $0018
[00013308]                           dc.b $002b
[0001330a]                           dc.b $0007
[0001330c]                           dc.b $0019
[0001330e]                           dc.b $0013
[00013310]                           dc.b $0007
[00013312]                           dc.b $001a
[00013314]                           dc.b $0024
[00013316]                           dc.b $0007
[00013318]                           dc.b $001b
[0001331a]                           dc.b $0018
[0001331c]                           dc.b $0007
[0001331e]                           dc.b $001c
[00013320]                           dc.b $0002
[00013322]                           dc.b $0008
[00013324]                           dc.b $001d
[00013326]                           dc.b $0003
[00013328]                           dc.b $0008
[0001332a]                           dc.b $001e
[0001332c]                           dc.b $001a
[0001332e]                           dc.b $0008
[00013330]                           dc.b $001f
[00013332]                           dc.b $001b
[00013334]                           dc.b $0008
[00013336]                           dc.b $0020
[00013338]                           dc.b $0012
[0001333a]                           dc.b $0008
[0001333c]                           dc.b $0021
[0001333e]                           dc.b $0013
[00013340]                           dc.b $0008
[00013342]                           dc.b $0022
[00013344]                           dc.b $0014
[00013346]                           dc.b $0008
[00013348]                           dc.b $0023
[0001334a]                           dc.b $0015
[0001334c]                           dc.b $0008
[0001334e]                           dc.b $0024
[00013350]                           dc.b $0016
[00013352]                           dc.b $0008
[00013354]                           dc.b $0025
[00013356]                           dc.b $0017
[00013358]                           dc.b $0008
[0001335a]                           dc.b $0026
[0001335c]                           dc.b $0028
[0001335e]                           dc.b $0008
[00013360]                           dc.b $0027
[00013362]                           dc.b $0029
[00013364]                           dc.b $0008
[00013366]                           dc.b $0028
[00013368]                           dc.b $002a
[0001336a]                           dc.b $0008
[0001336c]                           dc.b $0029
[0001336e]                           dc.b $002b
[00013370]                           dc.b $0008
[00013372]                           dc.b $002a
[00013374]                           dc.b $002c
[00013376]                           dc.b $0008
[00013378]                           dc.b $002b
[0001337a]                           dc.b $002d
[0001337c]                           dc.b $0008
[0001337e]                           dc.b $002c
[00013380]                           dc.b $0004
[00013382]                           dc.b $0008
[00013384]                           dc.b $002d
[00013386]                           dc.b $0005
[00013388]                           dc.b $0008
[0001338a]                           dc.b $002e
[0001338c]                           dc.b $000a
[0001338e]                           dc.b $0008
[00013390]                           dc.b $002f
[00013392]                           dc.b $000b
[00013394]                           dc.b $0008
[00013396]                           dc.b $0030
[00013398]                           dc.b $0052
[0001339a]                           dc.b $0008
[0001339c]                           dc.b $0031
[0001339e]                           dc.b $0053
[000133a0]                           dc.b $0008
[000133a2]                           dc.b $0032
[000133a4]                           dc.b $0054
[000133a6]                           dc.b $0008
[000133a8]                           dc.b $0033
[000133aa]                           dc.b $0055
[000133ac]                           dc.b $0008
[000133ae]                           dc.b $0034
[000133b0]                           dc.b $0024
[000133b2]                           dc.b $0008
[000133b4]                           dc.b $0035
[000133b6]                           dc.b $0025
[000133b8]                           dc.b $0008
[000133ba]                           dc.b $0036
[000133bc]                           dc.b $0058
[000133be]                           dc.b $0008
[000133c0]                           dc.b $0037
[000133c2]                           dc.b $0059
[000133c4]                           dc.b $0008
[000133c6]                           dc.b $0038
[000133c8]                           dc.b $005a
[000133ca]                           dc.b $0008
[000133cc]                           dc.b $0039
[000133ce]                           dc.b $005b
[000133d0]                           dc.b $0008
[000133d2]                           dc.b $003a
[000133d4]                           dc.b $004a
[000133d6]                           dc.b $0008
[000133d8]                           dc.b $003b
[000133da]                           dc.b $004b
[000133dc]                           dc.b $0008
[000133de]                           dc.b $003c
[000133e0]                           dc.b $0032
[000133e2]                           dc.b $0008
[000133e4]                           dc.b $003d
[000133e6]                           dc.b $0033
[000133e8]                           dc.b $0008
[000133ea]                           dc.b $003e
[000133ec]                           dc.b $0034
[000133ee]                           dc.b $0008
[000133f0]                           dc.b $003f
[000133f2]                           dc.b $001b
[000133f4]                           dc.b $0005
[000133f6]                           dc.b $0040
[000133f8]                           dc.b $0012
[000133fa]                           dc.b $0005
[000133fc]                           dc.b $0080
[000133fe]                           dc.b $0017
[00013400]                           dc.b $0006
[00013402]                           dc.b $00c0
[00013404]                           dc.b $0037
[00013406]                           dc.b $0007
[00013408]                           dc.w $0100
[0001340a]                           dc.b $0036
[0001340c]                           dc.b $0008
[0001340e]                           dc.w $0140
[00013410]                           dc.b $0037
[00013412]                           dc.b $0008
[00013414]                           dc.w $0180
[00013416]                           dc.b $0064
[00013418]                           dc.b $0008
[0001341a]                           dc.w $01c0
[0001341c]                           dc.b $0065
[0001341e]                           dc.b $0008
[00013420]                           dc.w $0200
[00013422]                           dc.b $0068
[00013424]                           dc.b $0008
[00013426]                           dc.w $0240
[00013428]                           dc.b $0067
[0001342a]                           dc.b $0008
[0001342c]                           dc.w $0280
[0001342e]                           dc.b $00cc
[00013430]                           dc.b $0009
[00013432]                           dc.w $02c0
[00013434]                           dc.b $00cd
[00013436]                           dc.b $0009
[00013438]                           dc.w $0300
[0001343a]                           dc.b $00d2
[0001343c]                           dc.b $0009
[0001343e]                           dc.w $0340
[00013440]                           dc.b $00d3
[00013442]                           dc.b $0009
[00013444]                           dc.w $0380
[00013446]                           dc.b $00d4
[00013448]                           dc.b $0009
[0001344a]                           dc.w $03c0
[0001344c]                           dc.b $00d5
[0001344e]                           dc.b $0009
[00013450]                           dc.w $0400
[00013452]                           dc.b $00d6
[00013454]                           dc.b $0009
[00013456]                           dc.w $0440
[00013458]                           dc.b $00d7
[0001345a]                           dc.b $0009
[0001345c]                           dc.w $0480
[0001345e]                           dc.b $00d8
[00013460]                           dc.b $0009
[00013462]                           dc.w $04c0
[00013464]                           dc.b $00d9
[00013466]                           dc.b $0009
[00013468]                           dc.w $0500
[0001346a]                           dc.b $00da
[0001346c]                           dc.b $0009
[0001346e]                           dc.w $0540
[00013470]                           dc.b $00db
[00013472]                           dc.b $0009
[00013474]                           dc.w $0580
[00013476]                           dc.b $0098
[00013478]                           dc.b $0009
[0001347a]                           dc.w $05c0
[0001347c]                           dc.b $0099
[0001347e]                           dc.b $0009
[00013480]                           dc.w $0600
[00013482]                           dc.b $009a
[00013484]                           dc.b $0009
[00013486]                           dc.w $0640
[00013488]                           dc.b $0018
[0001348a]                           dc.b $0006
[0001348c]                           dc.w $0680
[0001348e]                           dc.b $009b
[00013490]                           dc.b $0009
[00013492]                           dc.w $06c0
[00013494]                           dc.b $0001
[00013496]                           dc.b $000c
[00013498]                           dc.w $fffe
[0001349a]                           dc.b $0037
[0001349c]                           dc.b $000a
[0001349e]                           dc.b $0000
[000134a0]                           dc.b $0002
[000134a2]                           dc.b $0003
[000134a4]                           dc.b $0001
[000134a6]                           dc.b $0003
[000134a8]                           dc.b $0002
[000134aa]                           dc.b $0002
[000134ac]                           dc.b $0002
[000134ae]                           dc.b $0002
[000134b0]                           dc.b $0003
[000134b2]                           dc.b $0003
[000134b4]                           dc.b $0003
[000134b6]                           dc.b $0004
[000134b8]                           dc.b $0003
[000134ba]                           dc.b $0004
[000134bc]                           dc.b $0005
[000134be]                           dc.b $0002
[000134c0]                           dc.b $0004
[000134c2]                           dc.b $0006
[000134c4]                           dc.b $0003
[000134c6]                           dc.b $0005
[000134c8]                           dc.b $0007
[000134ca]                           dc.b $0005
[000134cc]                           dc.b $0006
[000134ce]                           dc.b $0008
[000134d0]                           dc.b $0004
[000134d2]                           dc.b $0006
[000134d4]                           dc.b $0009
[000134d6]                           dc.b $0004
[000134d8]                           dc.b $0007
[000134da]                           dc.b $000a
[000134dc]                           dc.b $0005
[000134de]                           dc.b $0007
[000134e0]                           dc.b $000b
[000134e2]                           dc.b $0007
[000134e4]                           dc.b $0007
[000134e6]                           dc.b $000c
[000134e8]                           dc.b $0004
[000134ea]                           dc.b $0008
[000134ec]                           dc.b $000d
[000134ee]                           dc.b $0007
[000134f0]                           dc.b $0008
[000134f2]                           dc.b $000e
[000134f4]                           dc.b $0018
[000134f6]                           dc.b $0009
[000134f8]                           dc.b $000f
[000134fa]                           dc.b $0017
[000134fc]                           dc.b $000a
[000134fe]                           dc.b $0010
[00013500]                           dc.b $0018
[00013502]                           dc.b $000a
[00013504]                           dc.b $0011
[00013506]                           dc.b $0008
[00013508]                           dc.b $000a
[0001350a]                           dc.b $0012
[0001350c]                           dc.b $0067
[0001350e]                           dc.b $000b
[00013510]                           dc.b $0013
[00013512]                           dc.b $0068
[00013514]                           dc.b $000b
[00013516]                           dc.b $0014
[00013518]                           dc.b $006c
[0001351a]                           dc.b $000b
[0001351c]                           dc.b $0015
[0001351e]                           dc.b $0037
[00013520]                           dc.b $000b
[00013522]                           dc.b $0016
[00013524]                           dc.b $0028
[00013526]                           dc.b $000b
[00013528]                           dc.b $0017
[0001352a]                           dc.b $0017
[0001352c]                           dc.b $000b
[0001352e]                           dc.b $0018
[00013530]                           dc.b $0018
[00013532]                           dc.b $000b
[00013534]                           dc.b $0019
[00013536]                           dc.b $00ca
[00013538]                           dc.b $000c
[0001353a]                           dc.b $001a
[0001353c]                           dc.b $00cb
[0001353e]                           dc.b $000c
[00013540]                           dc.b $001b
[00013542]                           dc.b $00cc
[00013544]                           dc.b $000c
[00013546]                           dc.b $001c
[00013548]                           dc.b $00cd
[0001354a]                           dc.b $000c
[0001354c]                           dc.b $001d
[0001354e]                           dc.b $0068
[00013550]                           dc.b $000c
[00013552]                           dc.b $001e
[00013554]                           dc.b $0069
[00013556]                           dc.b $000c
[00013558]                           dc.b $001f
[0001355a]                           dc.b $006a
[0001355c]                           dc.b $000c
[0001355e]                           dc.b $0020
[00013560]                           dc.b $006b
[00013562]                           dc.b $000c
[00013564]                           dc.b $0021
[00013566]                           dc.b $00d2
[00013568]                           dc.b $000c
[0001356a]                           dc.b $0022
[0001356c]                           dc.b $00d3
[0001356e]                           dc.b $000c
[00013570]                           dc.b $0023
[00013572]                           dc.b $00d4
[00013574]                           dc.b $000c
[00013576]                           dc.b $0024
[00013578]                           dc.b $00d5
[0001357a]                           dc.b $000c
[0001357c]                           dc.b $0025
[0001357e]                           dc.b $00d6
[00013580]                           dc.b $000c
[00013582]                           dc.b $0026
[00013584]                           dc.b $00d7
[00013586]                           dc.b $000c
[00013588]                           dc.b $0027
[0001358a]                           dc.b $006c
[0001358c]                           dc.b $000c
[0001358e]                           dc.b $0028
[00013590]                           dc.b $006d
[00013592]                           dc.b $000c
[00013594]                           dc.b $0029
[00013596]                           dc.b $00da
[00013598]                           dc.b $000c
[0001359a]                           dc.b $002a
[0001359c]                           dc.b $00db
[0001359e]                           dc.b $000c
[000135a0]                           dc.b $002b
[000135a2]                           dc.b $0054
[000135a4]                           dc.b $000c
[000135a6]                           dc.b $002c
[000135a8]                           dc.b $0055
[000135aa]                           dc.b $000c
[000135ac]                           dc.b $002d
[000135ae]                           dc.b $0056
[000135b0]                           dc.b $000c
[000135b2]                           dc.b $002e
[000135b4]                           dc.b $0057
[000135b6]                           dc.b $000c
[000135b8]                           dc.b $002f
[000135ba]                           dc.b $0064
[000135bc]                           dc.b $000c
[000135be]                           dc.b $0030
[000135c0]                           dc.b $0065
[000135c2]                           dc.b $000c
[000135c4]                           dc.b $0031
[000135c6]                           dc.b $0052
[000135c8]                           dc.b $000c
[000135ca]                           dc.b $0032
[000135cc]                           dc.b $0053
[000135ce]                           dc.b $000c
[000135d0]                           dc.b $0033
[000135d2]                           dc.b $0024
[000135d4]                           dc.b $000c
[000135d6]                           dc.b $0034
[000135d8]                           dc.b $0037
[000135da]                           dc.b $000c
[000135dc]                           dc.b $0035
[000135de]                           dc.b $0038
[000135e0]                           dc.b $000c
[000135e2]                           dc.b $0036
[000135e4]                           dc.b $0027
[000135e6]                           dc.b $000c
[000135e8]                           dc.b $0037
[000135ea]                           dc.b $0028
[000135ec]                           dc.b $000c
[000135ee]                           dc.b $0038
[000135f0]                           dc.b $0058
[000135f2]                           dc.b $000c
[000135f4]                           dc.b $0039
[000135f6]                           dc.b $0059
[000135f8]                           dc.b $000c
[000135fa]                           dc.b $003a
[000135fc]                           dc.b $002b
[000135fe]                           dc.b $000c
[00013600]                           dc.b $003b
[00013602]                           dc.b $002c
[00013604]                           dc.b $000c
[00013606]                           dc.b $003c
[00013608]                           dc.b $005a
[0001360a]                           dc.b $000c
[0001360c]                           dc.b $003d
[0001360e]                           dc.b $0066
[00013610]                           dc.b $000c
[00013612]                           dc.b $003e
[00013614]                           dc.b $0067
[00013616]                           dc.b $000c
[00013618]                           dc.b $003f
[0001361a]                           dc.b $000f
[0001361c]                           dc.b $000a
[0001361e]                           dc.b $0040
[00013620]                           dc.b $00c8
[00013622]                           dc.b $000c
[00013624]                           dc.b $0080
[00013626]                           dc.b $00c9
[00013628]                           dc.b $000c
[0001362a]                           dc.b $00c0
[0001362c]                           dc.b $005b
[0001362e]                           dc.b $000c
[00013630]                           dc.w $0100
[00013632]                           dc.b $0033
[00013634]                           dc.b $000c
[00013636]                           dc.w $0140
[00013638]                           dc.b $0034
[0001363a]                           dc.b $000c
[0001363c]                           dc.w $0180
[0001363e]                           dc.b $0035
[00013640]                           dc.b $000c
[00013642]                           dc.w $01c0
[00013644]                           dc.b $006c
[00013646]                           dc.b $000d
[00013648]                           dc.w $0200
[0001364a]                           dc.b $006d
[0001364c]                           dc.b $000d
[0001364e]                           dc.w $0240
[00013650]                           dc.b $004a
[00013652]                           dc.b $000d
[00013654]                           dc.w $0280
[00013656]                           dc.b $004b
[00013658]                           dc.b $000d
[0001365a]                           dc.w $02c0
[0001365c]                           dc.b $004c
[0001365e]                           dc.b $000d
[00013660]                           dc.w $0300
[00013662]                           dc.b $004d
[00013664]                           dc.b $000d
[00013666]                           dc.w $0340
[00013668]                           dc.b $0072
[0001366a]                           dc.b $000d
[0001366c]                           dc.w $0380
[0001366e]                           dc.b $0073
[00013670]                           dc.b $000d
[00013672]                           dc.w $03c0
[00013674]                           dc.b $0074
[00013676]                           dc.b $000d
[00013678]                           dc.w $0400
[0001367a]                           dc.b $0075
[0001367c]                           dc.b $000d
[0001367e]                           dc.w $0440
[00013680]                           dc.b $0076
[00013682]                           dc.b $000d
[00013684]                           dc.w $0480
[00013686]                           dc.b $0077
[00013688]                           dc.b $000d
[0001368a]                           dc.w $04c0
[0001368c]                           dc.b $0052
[0001368e]                           dc.b $000d
[00013690]                           dc.w $0500
[00013692]                           dc.b $0053
[00013694]                           dc.b $000d
[00013696]                           dc.w $0540
[00013698]                           dc.b $0054
[0001369a]                           dc.b $000d
[0001369c]                           dc.w $0580
[0001369e]                           dc.b $0055
[000136a0]                           dc.b $000d
[000136a2]                           dc.w $05c0
[000136a4]                           dc.b $005a
[000136a6]                           dc.b $000d
[000136a8]                           dc.w $0600
[000136aa]                           dc.b $005b
[000136ac]                           dc.b $000d
[000136ae]                           dc.w $0640
[000136b0]                           dc.b $0064
[000136b2]                           dc.b $000d
[000136b4]                           dc.w $0680
[000136b6]                           dc.b $0065
[000136b8]                           dc.b $000d
[000136ba]                           dc.w $06c0
[000136bc]                           dc.b $0001
[000136be]                           dc.b $000c
[000136c0]                           dc.w $fffe
[000136c2]                           dc.b $0008
[000136c4]                           dc.b $000b
[000136c6]                           dc.w $0700
[000136c8]                           dc.b $000c
[000136ca]                           dc.b $000b
[000136cc]                           dc.w $0740
[000136ce]                           dc.b $000d
[000136d0]                           dc.b $000b
[000136d2]                           dc.w $0780
[000136d4]                           dc.b $0012
[000136d6]                           dc.b $000c
[000136d8]                           dc.w $07c0
[000136da]                           dc.b $0013
[000136dc]                           dc.b $000c
[000136de]                           dc.w $0800
[000136e0]                           dc.b $0014
[000136e2]                           dc.b $000c
[000136e4]                           dc.w $0840
[000136e6]                           dc.b $0015
[000136e8]                           dc.b $000c
[000136ea]                           dc.w $0880
[000136ec]                           dc.b $0016
[000136ee]                           dc.b $000c
[000136f0]                           dc.w $08c0
[000136f2]                           dc.b $0017
[000136f4]                           dc.b $000c
[000136f6]                           dc.w $0900
[000136f8]                           dc.b $001c
[000136fa]                           dc.b $000c
[000136fc]                           dc.w $0940
[000136fe]                           dc.b $001d
[00013700]                           dc.b $000c
[00013702]                           dc.w $0980
[00013704]                           dc.b $001e
[00013706]                           dc.b $000c
[00013708]                           dc.w $09c0
[0001370a]                           dc.b $001f
[0001370c]                           dc.b $000c
[0001370e]                           dc.w $0a00




[00013906]                           dc.b 'Zeit: %lu',$0a,0
[00013911]                           dc.b 'Zeit: %lu',$0a,0
[0001391c]                           dc.b 'Zeit: %lu',$0a,0
[00013927]                           dc.b $00
[00013928]                           dc.b $00
[00013929]                           dc.b $00
[0001392a]                           dc.b $00
[0001392b]                           dc.b $00
[0001392c]                           dc.b $00
[0001392d]                           dc.b $00
[0001392e]                           dc.b $00
[0001392f]                           dc.b $00
[00013930]                           dc.b $00
[00013931]                           dc.b $00
[00013932]                           dc.b $00
[00013933]                           dc.b $00
[00013934]                           dc.b $00
[00013935]                           dc.b $00
[00013936]                           dc.b $00
[00013937]                           dc.b $00
[00013938]                           dc.b $00
[00013939]                           dc.b $00
[0001393a]                           dc.b $00
[0001393b]                           dc.b $00
[0001393c]                           dc.b $00
[0001393d]                           dc.b $00
[0001393e]                           dc.b $00
[0001393f]                           dc.b $00
[00013940] 00013956                  dc.l $00013956
[00013944] 00013956                  dc.l $00013956
[00013948] 00013956                  dc.l $00013956
[0001394c] 000139a6                  dc.l _StdOutF
[00013950]                           dc.b $00
[00013951]                           dc.b $00
[00013952]                           dc.w $0100
[00013954]                           dc.b $00
[00013955]                           dc.b $00
[00013956]                           dc.b $00
[00013957]                           dc.b $00
[00013958]                           dc.b $00
[00013959]                           dc.b $00
[0001395a]                           dc.b $00
[0001395b]                           dc.b $00
[0001395c]                           dc.b $00
[0001395d]                           dc.b $00
[0001395e]                           dc.b $00
[0001395f]                           dc.b $00
[00013960]                           dc.b $00
[00013961]                           dc.b $00
[00013962]                           dc.b $00
[00013963]                           dc.b $00
[00013964]                           dc.b $00
[00013965]                           dc.b $00
[00013966]                           dc.b $00
[00013967]                           dc.b $00
[00013968]                           dc.b $00
[00013969]                           dc.b $00
[0001396a]                           dc.b $00
[0001396b]                           dc.b $00
[0001396c]                           dc.b $00
[0001396d]                           dc.b $00
[0001396e]                           dc.b $00
[0001396f]                           dc.b $00
[00013970]                           dc.b $00
[00013971]                           dc.b $00
[00013972]                           dc.b $00
[00013973]                           dc.b $00
[00013974]                           dc.b $00
[00013975]                           dc.b $00
[00013976]                           dc.b $00
[00013977]                           dc.b $00
[00013978]                           dc.b $00
[00013979]                           dc.b $00
[0001397a]                           dc.b $00
[0001397b]                           dc.b $00
[0001397c]                           dc.b $00
[0001397d]                           dc.b $00
[0001397e]                           dc.b $00
[0001397f]                           dc.b $00
[00013980]                           dc.b $00
[00013981]                           dc.b $00
[00013982]                           dc.b $00
[00013983]                           dc.b $00
[00013984]                           dc.b $00
[00013985]                           dc.b $00
[00013986]                           dc.b $00
[00013987]                           dc.b $00
[00013988]                           dc.b $00
[00013989]                           dc.b $00
[0001398a]                           dc.b $00
[0001398b]                           dc.b $00
[0001398c]                           dc.b $00
[0001398d]                           dc.b $00
[0001398e]                           dc.b $00
[0001398f]                           dc.b $00
[00013990]                           dc.b $00
[00013991]                           dc.b $00
[00013992]                           dc.b $00
[00013993]                           dc.b $00
[00013994]                           dc.b $00
[00013995]                           dc.b $00
[00013996]                           dc.b $00
[00013997]                           dc.b $00
[00013998]                           dc.b $00
[00013999]                           dc.b $00
[0001399a]                           dc.b $00
[0001399b]                           dc.b $00
[0001399c]                           dc.b $00
[0001399d]                           dc.b $00
[0001399e]                           dc.b $00
[0001399f]                           dc.b $00
[000139a0]                           dc.b $00
[000139a1]                           dc.b $00
[000139a2]                           dc.b $00
[000139a3]                           dc.b $00
[000139a4]                           dc.b $00
[000139a5]                           dc.b $00

_StdOutF:
[000139a6]                           dc.b $00
[000139a7]                           dc.b $00
[000139a8]                           dc.b $00
[000139a9]                           dc.b $00
[000139aa]                           dc.b $00
[000139ab]                           dc.b $00
[000139ac]                           dc.b $00
[000139ad]                           dc.b $00
[000139ae]                           dc.b $00
[000139af]                           dc.b $00
[000139b0]                           dc.b $00
[000139b1]                           dc.b $00
[000139b2]                           dc.b $00
[000139b3]                           dc.b $00
[000139b4]                           dc.b $00
[000139b5]                           dc.b $00
[000139b6]                           dc.b $00
[000139b7]                           dc.b $01
[000139b8]                           dc.w $0600
[000139ba]                           dc.b $00
[000139bb]                           dc.b $00
[000139bc]                           dc.b $00
[000139bd]                           dc.b $00
[000139be]                           dc.b $00
[000139bf]                           dc.b $00
[000139c0]                           dc.b $00
[000139c1]                           dc.b $00
[000139c2]                           dc.b $00
[000139c3]                           dc.b $00
[000139c4]                           dc.b $00
[000139c5]                           dc.b $00
[000139c6]                           dc.b $00
[000139c7]                           dc.b $00
[000139c8]                           dc.b $00
[000139c9]                           dc.b $00
[000139ca]                           dc.b $00
[000139cb]                           dc.b $00
[000139cc]                           dc.w $ffff
[000139ce]                           dc.w $0600
[000139d0]                           dc.b $00
[000139d1]                           dc.b $00
[000139d2]                           dc.b $00
[000139d3]                           dc.b $00
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
[000139e3]                           dc.b $02
[000139e4]                           dc.w $0700
[000139e6]                           dc.b $00
[000139e7]                           dc.b $00
[000139e8]                           dc.b $00
[000139e9]                           dc.b $00
[000139ea]                           dc.b $00
[000139eb]                           dc.b $00
[000139ec]                           dc.b $00
[000139ed]                           dc.b $00
[000139ee]                           dc.b $00
[000139ef]                           dc.b $00
[000139f0]                           dc.b $00
[000139f1]                           dc.b $00
[000139f2]                           dc.b $00
[000139f3]                           dc.b $00
[000139f4]                           dc.b $00
[000139f5]                           dc.b $00
[000139f6]                           dc.b $00
[000139f7]                           dc.b $00
[000139f8]                           dc.b $00
[000139f9]                           dc.b $03
[000139fa]                           dc.w $0600
[000139fc]                           dc.b $00
[000139fd]                           dc.b $00


1323e: bitmask
13710: module_info
137f0: BitsAvail

13a16: SMalloc
13a1a: SMfree
13a1e: 
13a22:
13a24: global_y
13a26:
13a2a:
13a2e:
1ba2e:
