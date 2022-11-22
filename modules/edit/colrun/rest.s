[00011092] 48e7 1f3e                 movem.l    d3-d7/a2-a6,-(a7)
[00011096] 4fef ff9a                 lea.l      -102(a7),a7
[0001109a] 3600                      move.w     d0,d3
[0001109c] 6100 0338                 bsr        x113d6
[000110a0] 47f9 0001 3a4a            lea.l      maintree,a3
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
[0001399c]                           dc.b '[1][Es kînnen nur 16 StÅtzpunkte|gesetzt werden!][ Ok ]',0
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
