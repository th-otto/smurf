do_it:
[000103f8] 48e7 1f3c                 movem.l    d3-d7/a2-a5,-(a7)
[000103fc] 4fef ff74                 lea.l      -140(a7),a7
[00010400] 2f48 0074                 move.l     a0,116(a7)
[00010404] 2f79 0001 2f30 0070       move.l     $00012F30,112(a7)
[0001040c] 226f 0070                 movea.l    112(a7),a1
[00010410] 3f69 0136 006a            move.w     310(a1),106(a7)
[00010416] 3f69 0138 0068            move.w     312(a1),104(a7)
[0001041c] 2f69 012e 0060            move.l     302(a1),96(a7)
[00010422] 322f 006a                 move.w     106(a7),d1
[00010426] 48c1                      ext.l      d1
[00010428] 2001                      move.l     d1,d0
[0001042a] d080                      add.l      d0,d0
[0001042c] d081                      add.l      d1,d0
[0001042e] 2f40 0034                 move.l     d0,52(a7)
[00010432] 2f41 0030                 move.l     d1,48(a7)
[00010436] 7401                      moveq.l    #1,d2
[00010438] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[0001043e] c468 03b2                 and.w      946(a0),d2
[00010442] 672e                      beq.s      $00010472
[00010444] 2f79 0001 2f34 006c       move.l     $00012F34,108(a7)
[0001044c] 226f 006c                 movea.l    108(a7),a1
[00010450] 3f69 0136 0066            move.w     310(a1),102(a7)
[00010456] 3f69 0138 0064            move.w     312(a1),100(a7)
[0001045c] 2f69 012e 005c            move.l     302(a1),92(a7)
[00010462] 322f 0066                 move.w     102(a7),d1
[00010466] 48c1                      ext.l      d1
[00010468] 2001                      move.l     d1,d0
[0001046a] d080                      add.l      d0,d0
[0001046c] d081                      add.l      d1,d0
[0001046e] 2f40 002c                 move.l     d0,44(a7)
[00010472] 302f 006a                 move.w     106(a7),d0
[00010476] 48c0                      ext.l      d0
[00010478] 322f 006a                 move.w     106(a7),d1
[0001047c] 48c1                      ext.l      d1
[0001047e] 6100 1e28                 bsr        _lmul
[00010482] 2f00                      move.l     d0,-(a7)
[00010484] 302f 006c                 move.w     108(a7),d0
[00010488] 48c0                      ext.l      d0
[0001048a] 322f 006c                 move.w     108(a7),d1
[0001048e] 48c1                      ext.l      d1
[00010490] 6100 1e16                 bsr        _lmul
[00010494] d09f                      add.l      (a7)+,d0
[00010496] 41ef 0078                 lea.l      120(a7),a0
[0001049a] 6100 19f6                 bsr        _lxcnv
[0001049e] 41ef 0082                 lea.l      130(a7),a0
[000104a2] 2f20                      move.l     -(a0),-(a7)
[000104a4] 2f20                      move.l     -(a0),-(a7)
[000104a6] 3f20                      move.w     -(a0),-(a7)
[000104a8] 486f 0082                 pea.l      130(a7)
[000104ac] 6100 0d40                 bsr        sqrt
[000104b0] 4fef 000e                 lea.l      14(a7),a7
[000104b4] 41ef 0078                 lea.l      120(a7),a0
[000104b8] 6100 1998                 bsr        _xlcnv
[000104bc] 2000                      move.l     d0,d0
[000104be] 2e80                      move.l     d0,(a7)
[000104c0] 7264                      moveq.l    #100,d1
[000104c2] 4242                      clr.w      d2
[000104c4] 1439 0001 2f28            move.b     $00012F28,d2
[000104ca] 48c2                      ext.l      d2
[000104cc] 9282                      sub.l      d2,d1
[000104ce] 41ef 0078                 lea.l      120(a7),a0
[000104d2] 2001                      move.l     d1,d0
[000104d4] 6100 19bc                 bsr        _lxcnv
[000104d8] 41ef 0078                 lea.l      120(a7),a0
[000104dc] 43f9 0001 2e90            lea.l      $00012E90,a1
[000104e2] 6100 1166                 bsr        _xxmul
[000104e6] 41ef 0078                 lea.l      120(a7),a0
[000104ea] 43f9 0001 2e9a            lea.l      $00012E9A,a1
[000104f0] 6100 0f34                 bsr        _xxadd
[000104f4] 41ef 0078                 lea.l      120(a7),a0
[000104f8] 6100 1958                 bsr        _xlcnv
[000104fc] 2000                      move.l     d0,d0
[000104fe] 2f40 0028                 move.l     d0,40(a7)
[00010502] 7264                      moveq.l    #100,d1
[00010504] 4242                      clr.w      d2
[00010506] 1439 0001 2f29            move.b     $00012F29,d2
[0001050c] 48c2                      ext.l      d2
[0001050e] 9282                      sub.l      d2,d1
[00010510] 41ef 0078                 lea.l      120(a7),a0
[00010514] 2001                      move.l     d1,d0
[00010516] 6100 197a                 bsr        _lxcnv
[0001051a] 41ef 0078                 lea.l      120(a7),a0
[0001051e] 43f9 0001 2ea4            lea.l      $00012EA4,a1
[00010524] 6100 1356                 bsr        _xxdiv
[00010528] 41ef 0078                 lea.l      120(a7),a0
[0001052c] 43f9 0001 2eae            lea.l      $00012EAE,a1
[00010532] 6100 0ef2                 bsr        _xxadd
[00010536] 41ef 0078                 lea.l      120(a7),a0
[0001053a] 6100 1916                 bsr        _xlcnv
[0001053e] 2000                      move.l     d0,d0
[00010540] 2f40 0020                 move.l     d0,32(a7)
[00010544] 7000                      moveq.l    #0,d0
[00010546] 1039 0001 2f20            move.b     $00012F20,d0
[0001054c] 7200                      moveq.l    #0,d1
[0001054e] 1239 0001 2f26            move.b     $00012F26,d1
[00010554] 6100 1d52                 bsr        _lmul
[00010558] 7264                      moveq.l    #100,d1
[0001055a] 6100 1d96                 bsr        $000122F2
[0001055e] 1f40 0046                 move.b     d0,70(a7)
[00010562] 7000                      moveq.l    #0,d0
[00010564] 1039 0001 2f21            move.b     $00012F21,d0
[0001056a] 7200                      moveq.l    #0,d1
[0001056c] 1239 0001 2f26            move.b     $00012F26,d1
[00010572] 6100 1d34                 bsr        _lmul
[00010576] 7264                      moveq.l    #100,d1
[00010578] 6100 1d78                 bsr        $000122F2
[0001057c] 1f40 0045                 move.b     d0,69(a7)
[00010580] 7000                      moveq.l    #0,d0
[00010582] 1039 0001 2f22            move.b     $00012F22,d0
[00010588] 7200                      moveq.l    #0,d1
[0001058a] 1239 0001 2f26            move.b     $00012F26,d1
[00010590] 6100 1d16                 bsr        _lmul
[00010594] 7264                      moveq.l    #100,d1
[00010596] 6100 1d5a                 bsr        $000122F2
[0001059a] 2e00                      move.l     d0,d7
[0001059c] 7000                      moveq.l    #0,d0
[0001059e] 1039 0001 2f23            move.b     $00012F23,d0
[000105a4] 7200                      moveq.l    #0,d1
[000105a6] 1239 0001 2f27            move.b     $00012F27,d1
[000105ac] 6100 1cfa                 bsr        _lmul
[000105b0] 7264                      moveq.l    #100,d1
[000105b2] 6100 1d3e                 bsr        $000122F2
[000105b6] 1f40 0044                 move.b     d0,68(a7)
[000105ba] 7000                      moveq.l    #0,d0
[000105bc] 1039 0001 2f24            move.b     $00012F24,d0
[000105c2] 7200                      moveq.l    #0,d1
[000105c4] 1239 0001 2f27            move.b     $00012F27,d1
[000105ca] 6100 1cdc                 bsr        _lmul
[000105ce] 7264                      moveq.l    #100,d1
[000105d0] 6100 1d20                 bsr        $000122F2
[000105d4] 1f40 0043                 move.b     d0,67(a7)
[000105d8] 7000                      moveq.l    #0,d0
[000105da] 1039 0001 2f25            move.b     $00012F25,d0
[000105e0] 7200                      moveq.l    #0,d1
[000105e2] 1239 0001 2f27            move.b     $00012F27,d1
[000105e8] 6100 1cbe                 bsr        _lmul
[000105ec] 7264                      moveq.l    #100,d1
[000105ee] 6100 1d02                 bsr        $000122F2
[000105f2] 1f40 0042                 move.b     d0,66(a7)
[000105f6] 41ef 0078                 lea.l      120(a7),a0
[000105fa] 2017                      move.l     (a7),d0
[000105fc] 6100 1894                 bsr        _lxcnv
[00010600] 41ef 0078                 lea.l      120(a7),a0
[00010604] 43f9 0001 2eb8            lea.l      $00012EB8,a1
[0001060a] 6100 1270                 bsr        _xxdiv
[0001060e] 41ef 0082                 lea.l      130(a7),a0
[00010612] 3039 0001 2f2a            move.w     $00012F2A,d0
[00010618] 6100 1876                 bsr        _wxcnv
[0001061c] 41ef 0082                 lea.l      130(a7),a0
[00010620] 43f9 0001 2ec2            lea.l      $00012EC2,a1
[00010626] 6100 1254                 bsr        _xxdiv
[0001062a] 41ef 0078                 lea.l      120(a7),a0
[0001062e] 43ef 0082                 lea.l      130(a7),a1
[00010632] 6100 1016                 bsr        _xxmul
[00010636] 41ef 0078                 lea.l      120(a7),a0
[0001063a] 43f9 0001 2ecc            lea.l      $00012ECC,a1
[00010640] 6100 1008                 bsr        _xxmul
[00010644] 41ef 0078                 lea.l      120(a7),a0
[00010648] 6100 1808                 bsr        _xlcnv
[0001064c] 2000                      move.l     d0,d0
[0001064e] 2f40 0010                 move.l     d0,16(a7)
[00010652] 2017                      move.l     (a7),d0
[00010654] 222f 0028                 move.l     40(a7),d1
[00010658] 6100 1c4e                 bsr        _lmul
[0001065c] 7264                      moveq.l    #100,d1
[0001065e] 6100 1c92                 bsr        $000122F2
[00010662] 2f40 0024                 move.l     d0,36(a7)
[00010666] 222f 0024                 move.l     36(a7),d1
[0001066a] 6100 1c3c                 bsr        _lmul
[0001066e] 2a00                      move.l     d0,d5
[00010670] 2017                      move.l     (a7),d0
[00010672] 222f 0020                 move.l     32(a7),d1
[00010676] 6100 1c30                 bsr        _lmul
[0001067a] 7264                      moveq.l    #100,d1
[0001067c] 6100 1c74                 bsr        $000122F2
[00010680] 2f40 001c                 move.l     d0,28(a7)
[00010684] 7201                      moveq.l    #1,d1
[00010686] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[0001068c] c268 0382                 and.w      898(a0),d1
[00010690] 6730                      beq.s      $000106C2
[00010692] 3439 0001 2f2c            move.w     $00012F2C,d2
[00010698] 302f 006a                 move.w     106(a7),d0
[0001069c] 48c0                      ext.l      d0
[0001069e] 81fc 0002                 divs.w     #$0002,d0
[000106a2] 9440                      sub.w      d0,d2
[000106a4] 48c2                      ext.l      d2
[000106a6] 2f42 0018                 move.l     d2,24(a7)
[000106aa] 3279 0001 2f2e            movea.w    $00012F2E,a1
[000106b0] 322f 0068                 move.w     104(a7),d1
[000106b4] 48c1                      ext.l      d1
[000106b6] 83fc 0002                 divs.w     #$0002,d1
[000106ba] 92c1                      suba.w     d1,a1
[000106bc] 2f49 0014                 move.l     a1,20(a7)
[000106c0] 6018                      bra.s      $000106DA
[000106c2] 3039 0001 2f2c            move.w     $00012F2C,d0
[000106c8] 48c0                      ext.l      d0
[000106ca] 2f40 0018                 move.l     d0,24(a7)
[000106ce] 3239 0001 2f2e            move.w     $00012F2E,d1
[000106d4] 48c1                      ext.l      d1
[000106d6] 2f41 0014                 move.l     d1,20(a7)
[000106da] 222f 0024                 move.l     36(a7),d1
[000106de] 2001                      move.l     d1,d0
[000106e0] d080                      add.l      d0,d0
[000106e2] d081                      add.l      d1,d0
[000106e4] 6100 1c90                 bsr        Malloc
[000106e8] 2f48 0054                 move.l     a0,84(a7)
[000106ec] 673e                      beq.s      $0001072C
[000106ee] 222f 0024                 move.l     36(a7),d1
[000106f2] 2001                      move.l     d1,d0
[000106f4] d080                      add.l      d0,d0
[000106f6] d081                      add.l      d1,d0
[000106f8] 6100 1c7c                 bsr        Malloc
[000106fc] 2f48 0050                 move.l     a0,80(a7)
[00010700] 6722                      beq.s      $00010724
[00010702] 302f 006a                 move.w     106(a7),d0
[00010706] 48c0                      ext.l      d0
[00010708] 322f 0068                 move.w     104(a7),d1
[0001070c] 48c1                      ext.l      d1
[0001070e] 6100 1b98                 bsr        _lmul
[00010712] 6100 1c62                 bsr        Malloc
[00010716] 2f48 0058                 move.l     a0,88(a7)
[0001071a] 6616                      bne.s      $00010732
[0001071c] 206f 0050                 movea.l    80(a7),a0
[00010720] 6100 1c44                 bsr        Mfree
[00010724] 206f 0054                 movea.l    84(a7),a0
[00010728] 6100 1c3c                 bsr        Mfree
[0001072c] 70fd                      moveq.l    #-3,d0
[0001072e] 6000 0512                 bra        $00010C42
[00010732] 246f 0050                 movea.l    80(a7),a2
[00010736] 42af 0038                 clr.l      56(a7)
[0001073a] 6006                      bra.s      $00010742
[0001073c] 421a                      clr.b      (a2)+
[0001073e] 52af 0038                 addq.l     #1,56(a7)
[00010742] 222f 0024                 move.l     36(a7),d1
[00010746] 2001                      move.l     d1,d0
[00010748] d080                      add.l      d0,d0
[0001074a] d081                      add.l      d1,d0
[0001074c] 242f 0038                 move.l     56(a7),d2
[00010750] b480                      cmp.l      d0,d2
[00010752] 6de8                      blt.s      $0001073C
[00010754] 286f 0060                 movea.l    96(a7),a4
[00010758] 2f6f 0058 004c            move.l     88(a7),76(a7)
[0001075e] 4243                      clr.w      d3
[00010760] 6058                      bra.s      $000107BA
[00010762] 4244                      clr.w      d4
[00010764] 604c                      bra.s      $000107B2
[00010766] 4240                      clr.w      d0
[00010768] 101c                      move.b     (a4)+,d0
[0001076a] 4241                      clr.w      d1
[0001076c] 121c                      move.b     (a4)+,d1
[0001076e] d041                      add.w      d1,d0
[00010770] 4241                      clr.w      d1
[00010772] 121c                      move.b     (a4)+,d1
[00010774] d041                      add.w      d1,d0
[00010776] 7201                      moveq.l    #1,d1
[00010778] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[0001077e] c268 04a2                 and.w      1186(a0),d1
[00010782] 671a                      beq.s      $0001079E
[00010784] 143c 00ff                 move.b     #$FF,d2
[00010788] 3200                      move.w     d0,d1
[0001078a] 48c1                      ext.l      d1
[0001078c] 83fc 0003                 divs.w     #$0003,d1
[00010790] 9401                      sub.b      d1,d2
[00010792] 226f 004c                 movea.l    76(a7),a1
[00010796] 52af 004c                 addq.l     #1,76(a7)
[0001079a] 1282                      move.b     d2,(a1)
[0001079c] 6012                      bra.s      $000107B0
[0001079e] 3200                      move.w     d0,d1
[000107a0] 48c1                      ext.l      d1
[000107a2] 83fc 0003                 divs.w     #$0003,d1
[000107a6] 206f 004c                 movea.l    76(a7),a0
[000107aa] 52af 004c                 addq.l     #1,76(a7)
[000107ae] 1081                      move.b     d1,(a0)
[000107b0] 5244                      addq.w     #1,d4
[000107b2] b86f 006a                 cmp.w      106(a7),d4
[000107b6] 6dae                      blt.s      $00010766
[000107b8] 5243                      addq.w     #1,d3
[000107ba] b66f 0068                 cmp.w      104(a7),d3
[000107be] 6da2                      blt.s      $00010762
[000107c0] 2a6f 0054                 movea.l    84(a7),a5
[000107c4] 2f6f 0024 0038            move.l     36(a7),56(a7)
[000107ca] 604c                      bra.s      $00010818
[000107cc] 202f 0038                 move.l     56(a7),d0
[000107d0] 222f 0038                 move.l     56(a7),d1
[000107d4] 6100 1ad2                 bsr        _lmul
[000107d8] 2c00                      move.l     d0,d6
[000107da] 7000                      moveq.l    #0,d0
[000107dc] 102f 0046                 move.b     70(a7),d0
[000107e0] 2206                      move.l     d6,d1
[000107e2] 6100 1ac4                 bsr        _lmul
[000107e6] 2205                      move.l     d5,d1
[000107e8] 6100 1b08                 bsr        $000122F2
[000107ec] 1ac0                      move.b     d0,(a5)+
[000107ee] 7000                      moveq.l    #0,d0
[000107f0] 102f 0045                 move.b     69(a7),d0
[000107f4] 2206                      move.l     d6,d1
[000107f6] 6100 1ab0                 bsr        _lmul
[000107fa] 2205                      move.l     d5,d1
[000107fc] 6100 1af4                 bsr        $000122F2
[00010800] 1ac0                      move.b     d0,(a5)+
[00010802] 7000                      moveq.l    #0,d0
[00010804] 1007                      move.b     d7,d0
[00010806] 2206                      move.l     d6,d1
[00010808] 6100 1a9e                 bsr        _lmul
[0001080c] 2205                      move.l     d5,d1
[0001080e] 6100 1ae2                 bsr        $000122F2
[00010812] 1ac0                      move.b     d0,(a5)+
[00010814] 53af 0038                 subq.l     #1,56(a7)
[00010818] 202f 0038                 move.l     56(a7),d0
[0001081c] 6eae                      bgt.s      $000107CC
[0001081e] 7201                      moveq.l    #1,d1
[00010820] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010826] c268 03b2                 and.w      946(a0),d1
[0001082a] 6766                      beq.s      $00010892
[0001082c] 2a6f 0050                 movea.l    80(a7),a5
[00010830] 42af 0038                 clr.l      56(a7)
[00010834] 604e                      bra.s      $00010884
[00010836] 2c2f 001c                 move.l     28(a7),d6
[0001083a] 9caf 0038                 sub.l      56(a7),d6
[0001083e] 7000                      moveq.l    #0,d0
[00010840] 102f 0044                 move.b     68(a7),d0
[00010844] 2206                      move.l     d6,d1
[00010846] 6100 1a60                 bsr        _lmul
[0001084a] 222f 001c                 move.l     28(a7),d1
[0001084e] 6100 1aa2                 bsr        $000122F2
[00010852] 1ac0                      move.b     d0,(a5)+
[00010854] 7000                      moveq.l    #0,d0
[00010856] 102f 0043                 move.b     67(a7),d0
[0001085a] 2206                      move.l     d6,d1
[0001085c] 6100 1a4a                 bsr        _lmul
[00010860] 222f 001c                 move.l     28(a7),d1
[00010864] 6100 1a8c                 bsr        $000122F2
[00010868] 1ac0                      move.b     d0,(a5)+
[0001086a] 7000                      moveq.l    #0,d0
[0001086c] 102f 0042                 move.b     66(a7),d0
[00010870] 2206                      move.l     d6,d1
[00010872] 6100 1a34                 bsr        _lmul
[00010876] 222f 001c                 move.l     28(a7),d1
[0001087a] 6100 1a76                 bsr        $000122F2
[0001087e] 1ac0                      move.b     d0,(a5)+
[00010880] 52af 0038                 addq.l     #1,56(a7)
[00010884] 202f 0038                 move.l     56(a7),d0
[00010888] b0af 001c                 cmp.l      28(a7),d0
[0001088c] 6da8                      blt.s      $00010836
[0001088e] 6000 00a6                 bra        $00010936
[00010892] 2a6f 0054                 movea.l    84(a7),a5
[00010896] 42af 0038                 clr.l      56(a7)
[0001089a] 6000 008e                 bra        $0001092A
[0001089e] 2c2f 001c                 move.l     28(a7),d6
[000108a2] 9caf 0038                 sub.l      56(a7),d6
[000108a6] 4245                      clr.w      d5
[000108a8] 1a15                      move.b     (a5),d5
[000108aa] 3f05                      move.w     d5,-(a7)
[000108ac] 7000                      moveq.l    #0,d0
[000108ae] 102f 0046                 move.b     70(a7),d0
[000108b2] 2206                      move.l     d6,d1
[000108b4] 6100 19f2                 bsr        _lmul
[000108b8] 222f 001e                 move.l     30(a7),d1
[000108bc] 6100 1a34                 bsr        $000122F2
[000108c0] d05f                      add.w      (a7)+,d0
[000108c2] 3a00                      move.w     d0,d5
[000108c4] 4247                      clr.w      d7
[000108c6] 1e2d 0001                 move.b     1(a5),d7
[000108ca] 3f07                      move.w     d7,-(a7)
[000108cc] 7000                      moveq.l    #0,d0
[000108ce] 102f 0045                 move.b     69(a7),d0
[000108d2] 2206                      move.l     d6,d1
[000108d4] 6100 19d2                 bsr        _lmul
[000108d8] 222f 001e                 move.l     30(a7),d1
[000108dc] 6100 1a14                 bsr        $000122F2
[000108e0] d05f                      add.w      (a7)+,d0
[000108e2] 3e00                      move.w     d0,d7
[000108e4] 4240                      clr.w      d0
[000108e6] 102d 0002                 move.b     2(a5),d0
[000108ea] 3f00                      move.w     d0,-(a7)
[000108ec] 7000                      moveq.l    #0,d0
[000108ee] 102f 0044                 move.b     68(a7),d0
[000108f2] 2206                      move.l     d6,d1
[000108f4] 6100 19b2                 bsr        _lmul
[000108f8] 222f 001e                 move.l     30(a7),d1
[000108fc] 6100 19f4                 bsr        $000122F2
[00010900] d05f                      add.w      (a7)+,d0
[00010902] ba7c 00ff                 cmp.w      #$00FF,d5
[00010906] 6f04                      ble.s      $0001090C
[00010908] 3a3c 00ff                 move.w     #$00FF,d5
[0001090c] be7c 00ff                 cmp.w      #$00FF,d7
[00010910] 6f04                      ble.s      $00010916
[00010912] 3e3c 00ff                 move.w     #$00FF,d7
[00010916] b07c 00ff                 cmp.w      #$00FF,d0
[0001091a] 6f04                      ble.s      $00010920
[0001091c] 303c 00ff                 move.w     #$00FF,d0
[00010920] 1ac5                      move.b     d5,(a5)+
[00010922] 1ac7                      move.b     d7,(a5)+
[00010924] 1ac0                      move.b     d0,(a5)+
[00010926] 52af 0038                 addq.l     #1,56(a7)
[0001092a] 202f 0038                 move.l     56(a7),d0
[0001092e] b0af 001c                 cmp.l      28(a7),d0
[00010932] 6d00 ff6a                 blt        $0001089E
[00010936] 4243                      clr.w      d3
[00010938] 6000 02d8                 bra        $00010C12
[0001093c] 7007                      moveq.l    #7,d0
[0001093e] c043                      and.w      d3,d0
[00010940] 661c                      bne.s      $0001095E
[00010942] 3003                      move.w     d3,d0
[00010944] 48c0                      ext.l      d0
[00010946] ef88                      lsl.l      #7,d0
[00010948] 322f 0068                 move.w     104(a7),d1
[0001094c] 48c1                      ext.l      d1
[0001094e] 6100 19a2                 bsr        $000122F2
[00010952] 206f 0074                 movea.l    116(a7),a0
[00010956] 2268 0052                 movea.l    82(a0),a1
[0001095a] 2051                      movea.l    (a1),a0
[0001095c] 4e90                      jsr        (a0)
[0001095e] 3003                      move.w     d3,d0
[00010960] 48c0                      ext.l      d0
[00010962] 222f 0034                 move.l     52(a7),d1
[00010966] 6100 1940                 bsr        _lmul
[0001096a] 286f 0060                 movea.l    96(a7),a4
[0001096e] d9c0                      adda.l     d0,a4
[00010970] 3003                      move.w     d3,d0
[00010972] 48c0                      ext.l      d0
[00010974] 222f 0030                 move.l     48(a7),d1
[00010978] 6100 192e                 bsr        _lmul
[0001097c] 206f 0058                 movea.l    88(a7),a0
[00010980] d1c0                      adda.l     d0,a0
[00010982] 2f48 0048                 move.l     a0,72(a7)
[00010986] 7201                      moveq.l    #1,d1
[00010988] 2279 0001 2f3c            movea.l    $00012F3C,a1 main_form
[0001098e] c269 03b2                 and.w      946(a1),d1
[00010992] 6722                      beq.s      $000109B6
[00010994] 3e03                      move.w     d3,d7
[00010996] 48c7                      ext.l      d7
[00010998] 74ff                      moveq.l    #-1,d2
[0001099a] d46f 0064                 add.w      100(a7),d2
[0001099e] 8fc2                      divs.w     d2,d7
[000109a0] 4847                      swap       d7
[000109a2] 4245                      clr.w      d5
[000109a4] 3007                      move.w     d7,d0
[000109a6] 48c0                      ext.l      d0
[000109a8] 222f 002c                 move.l     44(a7),d1
[000109ac] 6100 18fa                 bsr        _lmul
[000109b0] 266f 005c                 movea.l    92(a7),a3
[000109b4] d7c0                      adda.l     d0,a3
[000109b6] 4244                      clr.w      d4
[000109b8] 6000 024e                 bra        $00010C08
[000109bc] 246f 0048                 movea.l    72(a7),a2
[000109c0] d4c4                      adda.w     d4,a2
[000109c2] 1012                      move.b     (a2),d0
[000109c4] 7201                      moveq.l    #1,d1
[000109c6] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[000109cc] c268 0472                 and.w      1138(a0),d1
[000109d0] 6606                      bne.s      $000109D8
[000109d2] 4a00                      tst.b      d0
[000109d4] 6700 01dc                 beq        $00010BB2
[000109d8] 4247                      clr.w      d7
[000109da] 3c07                      move.w     d7,d6
[000109dc] 72ff                      moveq.l    #-1,d1
[000109de] d26f 006a                 add.w      106(a7),d1
[000109e2] b841                      cmp.w      d1,d4
[000109e4] 6c0e                      bge.s      $000109F4
[000109e6] 4242                      clr.w      d2
[000109e8] 1400                      move.b     d0,d2
[000109ea] 4241                      clr.w      d1
[000109ec] 122a 0001                 move.b     1(a2),d1
[000109f0] 9441                      sub.w      d1,d2
[000109f2] de42                      add.w      d2,d7
[000109f4] 72ff                      moveq.l    #-1,d1
[000109f6] d26f 0068                 add.w      104(a7),d1
[000109fa] b641                      cmp.w      d1,d3
[000109fc] 6c12                      bge.s      $00010A10
[000109fe] 4242                      clr.w      d2
[00010a00] 1400                      move.b     d0,d2
[00010a02] 206f 0030                 movea.l    48(a7),a0
[00010a06] 4241                      clr.w      d1
[00010a08] 1232 8800                 move.b     0(a2,a0.l),d1
[00010a0c] 9441                      sub.w      d1,d2
[00010a0e] dc42                      add.w      d2,d6
[00010a10] 4241                      clr.w      d1
[00010a12] 1200                      move.b     d0,d1
[00010a14] 4242                      clr.w      d2
[00010a16] 142a ffff                 move.b     -1(a2),d2
[00010a1a] 9242                      sub.w      d2,d1
[00010a1c] 9e41                      sub.w      d1,d7
[00010a1e] 4241                      clr.w      d1
[00010a20] 1200                      move.b     d0,d1
[00010a22] 204a                      movea.l    a2,a0
[00010a24] 91ef 0030                 suba.l     48(a7),a0
[00010a28] 4242                      clr.w      d2
[00010a2a] 1410                      move.b     (a0),d2
[00010a2c] 9242                      sub.w      d2,d1
[00010a2e] 9c41                      sub.w      d1,d6
[00010a30] 3007                      move.w     d7,d0
[00010a32] 48c0                      ext.l      d0
[00010a34] 222f 0010                 move.l     16(a7),d1
[00010a38] 6100 186e                 bsr        _lmul
[00010a3c] 720a                      moveq.l    #10,d1
[00010a3e] e2a0                      asr.l      d1,d0
[00010a40] 3e00                      move.w     d0,d7
[00010a42] 3006                      move.w     d6,d0
[00010a44] 48c0                      ext.l      d0
[00010a46] 222f 0010                 move.l     16(a7),d1
[00010a4a] 6100 185c                 bsr        _lmul
[00010a4e] 720a                      moveq.l    #10,d1
[00010a50] e2a0                      asr.l      d1,d0
[00010a52] 3c00                      move.w     d0,d6
[00010a54] 7401                      moveq.l    #1,d2
[00010a56] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010a5c] c468 0382                 and.w      898(a0),d2
[00010a60] 671e                      beq.s      $00010A80
[00010a62] 226f 0018                 movea.l    24(a7),a1
[00010a66] 3007                      move.w     d7,d0
[00010a68] 48c0                      ext.l      d0
[00010a6a] 93c0                      suba.l     d0,a1
[00010a6c] 2f49 000c                 move.l     a1,12(a7)
[00010a70] 222f 0014                 move.l     20(a7),d1
[00010a74] 3406                      move.w     d6,d2
[00010a76] 48c2                      ext.l      d2
[00010a78] 9282                      sub.l      d2,d1
[00010a7a] 2f41 0008                 move.l     d1,8(a7)
[00010a7e] 6020                      bra.s      $00010AA0
[00010a80] 202f 0018                 move.l     24(a7),d0
[00010a84] 3207                      move.w     d7,d1
[00010a86] d244                      add.w      d4,d1
[00010a88] 48c1                      ext.l      d1
[00010a8a] 9081                      sub.l      d1,d0
[00010a8c] 2f40 000c                 move.l     d0,12(a7)
[00010a90] 242f 0014                 move.l     20(a7),d2
[00010a94] 3e06                      move.w     d6,d7
[00010a96] de43                      add.w      d3,d7
[00010a98] 48c7                      ext.l      d7
[00010a9a] 9487                      sub.l      d7,d2
[00010a9c] 2f42 0008                 move.l     d2,8(a7)
[00010aa0] 202f 000c                 move.l     12(a7),d0
[00010aa4] 222f 000c                 move.l     12(a7),d1
[00010aa8] 6100 17fe                 bsr        _lmul
[00010aac] 2f00                      move.l     d0,-(a7)
[00010aae] 202f 000c                 move.l     12(a7),d0
[00010ab2] 222f 000c                 move.l     12(a7),d1
[00010ab6] 6100 17f0                 bsr        _lmul
[00010aba] d09f                      add.l      (a7)+,d0
[00010abc] 41ef 0078                 lea.l      120(a7),a0
[00010ac0] 6100 13d0                 bsr        _lxcnv
[00010ac4] 41ef 0082                 lea.l      130(a7),a0
[00010ac8] 2f20                      move.l     -(a0),-(a7)
[00010aca] 2f20                      move.l     -(a0),-(a7)
[00010acc] 3f20                      move.w     -(a0),-(a7)
[00010ace] 486f 008c                 pea.l      140(a7)
[00010ad2] 6100 071a                 bsr        sqrt
[00010ad6] 4fef 000e                 lea.l      14(a7),a7
[00010ada] 41ef 0082                 lea.l      130(a7),a0
[00010ade] 6100 1372                 bsr        _xlcnv
[00010ae2] 2000                      move.l     d0,d0
[00010ae4] 2f40 0004                 move.l     d0,4(a7)
[00010ae8] b0af 0024                 cmp.l      36(a7),d0
[00010aec] 6d0a                      blt.s      $00010AF8
[00010aee] 72ff                      moveq.l    #-1,d1
[00010af0] d2af 0024                 add.l      36(a7),d1
[00010af4] 2f41 0004                 move.l     d1,4(a7)
[00010af8] 222f 0004                 move.l     4(a7),d1
[00010afc] 2001                      move.l     d1,d0
[00010afe] d080                      add.l      d0,d0
[00010b00] d081                      add.l      d1,d0
[00010b02] 2a6f 0054                 movea.l    84(a7),a5
[00010b06] dbc0                      adda.l     d0,a5
[00010b08] 7401                      moveq.l    #1,d2
[00010b0a] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010b10] c468 03b2                 and.w      946(a0),d2
[00010b14] 6700 0094                 beq        $00010BAA
[00010b18] 246f 0050                 movea.l    80(a7),a2
[00010b1c] d5c0                      adda.l     d0,a2
[00010b1e] 7000                      moveq.l    #0,d0
[00010b20] 1013                      move.b     (a3),d0
[00010b22] 7200                      moveq.l    #0,d1
[00010b24] 1215                      move.b     (a5),d1
[00010b26] 6100 1780                 bsr        _lmul
[00010b2a] e080                      asr.l      #8,d0
[00010b2c] 4241                      clr.w      d1
[00010b2e] 1212                      move.b     (a2),d1
[00010b30] d041                      add.w      d1,d0
[00010b32] 3f40 0040                 move.w     d0,64(a7)
[00010b36] 7000                      moveq.l    #0,d0
[00010b38] 102b 0001                 move.b     1(a3),d0
[00010b3c] 7200                      moveq.l    #0,d1
[00010b3e] 122d 0001                 move.b     1(a5),d1
[00010b42] 6100 1764                 bsr        _lmul
[00010b46] e080                      asr.l      #8,d0
[00010b48] 4241                      clr.w      d1
[00010b4a] 122a 0001                 move.b     1(a2),d1
[00010b4e] d041                      add.w      d1,d0
[00010b50] 3f40 003e                 move.w     d0,62(a7)
[00010b54] 7000                      moveq.l    #0,d0
[00010b56] 102b 0002                 move.b     2(a3),d0
[00010b5a] 7200                      moveq.l    #0,d1
[00010b5c] 122d 0002                 move.b     2(a5),d1
[00010b60] 6100 1746                 bsr        _lmul
[00010b64] e080                      asr.l      #8,d0
[00010b66] 4241                      clr.w      d1
[00010b68] 122a 0002                 move.b     2(a2),d1
[00010b6c] d041                      add.w      d1,d0
[00010b6e] 3f40 003c                 move.w     d0,60(a7)
[00010b72] 0c6f 00ff 0040            cmpi.w     #$00FF,64(a7)
[00010b78] 6306                      bls.s      $00010B80
[00010b7a] 3f7c 00ff 0040            move.w     #$00FF,64(a7)
[00010b80] 0c6f 00ff 003e            cmpi.w     #$00FF,62(a7)
[00010b86] 6306                      bls.s      $00010B8E
[00010b88] 3f7c 00ff 003e            move.w     #$00FF,62(a7)
[00010b8e] 0c6f 00ff 003c            cmpi.w     #$00FF,60(a7)
[00010b94] 6306                      bls.s      $00010B9C
[00010b96] 3f7c 00ff 003c            move.w     #$00FF,60(a7)
[00010b9c] 18ef 0041                 move.b     65(a7),(a4)+
[00010ba0] 18ef 003f                 move.b     63(a7),(a4)+
[00010ba4] 18ef 003d                 move.b     61(a7),(a4)+
[00010ba8] 6038                      bra.s      $00010BE2
[00010baa] 18dd                      move.b     (a5)+,(a4)+
[00010bac] 18dd                      move.b     (a5)+,(a4)+
[00010bae] 18dd                      move.b     (a5)+,(a4)+
[00010bb0] 6030                      bra.s      $00010BE2
[00010bb2] 7001                      moveq.l    #1,d0
[00010bb4] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010bba] c068 0442                 and.w      1090(a0),d0
[00010bbe] 670e                      beq.s      $00010BCE
[00010bc0] 18fc 00ff                 move.b     #$FF,(a4)+
[00010bc4] 18fc 00ff                 move.b     #$FF,(a4)+
[00010bc8] 18fc 00ff                 move.b     #$FF,(a4)+
[00010bcc] 6014                      bra.s      $00010BE2
[00010bce] 7001                      moveq.l    #1,d0
[00010bd0] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010bd6] c068 042a                 and.w      1066(a0),d0
[00010bda] 6706                      beq.s      $00010BE2
[00010bdc] 421c                      clr.b      (a4)+
[00010bde] 421c                      clr.b      (a4)+
[00010be0] 421c                      clr.b      (a4)+
[00010be2] 7001                      moveq.l    #1,d0
[00010be4] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010bea] c068 03b2                 and.w      946(a0),d0
[00010bee] 6716                      beq.s      $00010C06
[00010bf0] 5245                      addq.w     #1,d5
[00010bf2] 564b                      addq.w     #3,a3
[00010bf4] 564d                      addq.w     #3,a5
[00010bf6] 564a                      addq.w     #3,a2
[00010bf8] ba6f 0066                 cmp.w      102(a7),d5
[00010bfc] 6d08                      blt.s      $00010C06
[00010bfe] 9a6f 0066                 sub.w      102(a7),d5
[00010c02] 97ef 002c                 suba.l     44(a7),a3
[00010c06] 5244                      addq.w     #1,d4
[00010c08] b86f 006a                 cmp.w      106(a7),d4
[00010c0c] 6d00 fdae                 blt        $000109BC
[00010c10] 5243                      addq.w     #1,d3
[00010c12] b66f 0068                 cmp.w      104(a7),d3
[00010c16] 6d00 fd24                 blt        $0001093C
[00010c1a] 206f 0054                 movea.l    84(a7),a0
[00010c1e] 6100 1746                 bsr        Mfree
[00010c22] 206f 0050                 movea.l    80(a7),a0
[00010c26] 6100 173e                 bsr        Mfree
[00010c2a] 206f 0058                 movea.l    88(a7),a0
[00010c2e] 6100 1736                 bsr        Mfree
[00010c32] 206f 0074                 movea.l    116(a7),a0
[00010c36] 4268 002e                 clr.w      46(a0)
[00010c3a] 317c ffff 0030            move.w     #$FFFF,48(a0)
[00010c40] 4240                      clr.w      d0
[00010c42] 4fef 008c                 lea.l      140(a7),a7
[00010c46] 4cdf 3cf8                 movem.l    (a7)+,d3-d7/a2-a5
[00010c4a] 4e75                      rts
f_init_window
[00010c4c] 3f03                      move.w     d3,-(a7)
[00010c4e] 2f0a                      move.l     a2,-(a7)
[00010c50] 3600                      move.w     d0,d3
[00010c52] 45f9 0001 2f40            lea.l      $00012F40,a2 my_window
[00010c58] 7266                      moveq.l    #102,d1
[00010c5a] 4240                      clr.w      d0
[00010c5c] 2052                      movea.l    (a2),a0
[00010c5e] 6100 1588                 bsr        memset
[00010c62] 2052                      movea.l    (a2),a0
[00010c64] 4250                      clr.w      (a0)
[00010c66] 2052                      movea.l    (a2),a0
[00010c68] 3143 0002                 move.w     d3,2(a0)
[00010c6c] 2052                      movea.l    (a2),a0
[00010c6e] 317c 0001 0004            move.w     #$0001,4(a0)
[00010c74] 2052                      movea.l    (a2),a0
[00010c76] 317c ffff 0006            move.w     #$FFFF,6(a0)
[00010c7c] 2052                      movea.l    (a2),a0
[00010c7e] 317c ffff 0008            move.w     #$FFFF,8(a0)
[00010c84] 206a fffc                 movea.l    -4(a2),a0
[00010c88] 2252                      movea.l    (a2),a1
[00010c8a] 3368 0014 000a            move.w     20(a0),10(a1)
[00010c90] 206a fffc                 movea.l    -4(a2),a0
[00010c94] 2252                      movea.l    (a2),a1
[00010c96] 3368 0016 000c            move.w     22(a0),12(a1)
[00010c9c] 7028                      moveq.l    #40,d0
[00010c9e] 43f9 0001 2ed6            lea.l      $00012ED6,a1
[00010ca4] 2052                      movea.l    (a2),a0
[00010ca6] 41e8 000e                 lea.l      14(a0),a0
[00010caa] 6100 1308                 bsr        strncpy
[00010cae] 2052                      movea.l    (a2),a0
[00010cb0] 216a fffc 0038            move.l     -4(a2),56(a0)
[00010cb6] 2052                      movea.l    (a2),a0
[00010cb8] 42a8 003c                 clr.l      60(a0)
[00010cbc] 2052                      movea.l    (a2),a0
[00010cbe] 4268 0054                 clr.w      84(a0)
[00010cc2] 2052                      movea.l    (a2),a0
[00010cc4] 4268 004a                 clr.w      74(a0)
[00010cc8] 245f                      movea.l    (a7)+,a2
[00010cca] 361f                      move.w     (a7)+,d3
[00010ccc] 4e75                      rts
f_init_sliders:
[00010cce] 2f0a                      move.l     a2,-(a7)
[00010cd0] 2f0b                      move.l     a3,-(a7)
[00010cd2] 41f9 0001 2f46            lea.l      $00012F46,a0 red_slider
[00010cd8] 30bc 000c                 move.w     #$000C,(a0)
[00010cdc] 317c 000b 0002            move.w     #$000B,2(a0)
[00010ce2] 43e8 fff6                 lea.l      -10(a0),a1
[00010ce6] 2151 0004                 move.l     (a1),4(a0)
[00010cea] 317c 0006 0008            move.w     #$0006,8(a0)
[00010cf0] 42a8 000a                 clr.l      10(a0)
[00010cf4] 217c 0000 00ff 000e       move.l     #$000000FF,14(a0)
[00010cfc] 45e8 fffa                 lea.l      -6(a0),a2
[00010d00] 2152 0012                 move.l     (a2),18(a0)
[00010d04] 47e8 0016                 lea.l      22(a0),a3
[00010d08] 36bc 000a                 move.w     #$000A,(a3)
[00010d0c] 377c 0009 0002            move.w     #$0009,2(a3)
[00010d12] 2751 0004                 move.l     (a1),4(a3)
[00010d16] 377c 0005 0008            move.w     #$0005,8(a3)
[00010d1c] 42ab 000a                 clr.l      10(a3)
[00010d20] 277c 0000 00ff 000e       move.l     #$000000FF,14(a3)
[00010d28] 2752 0012                 move.l     (a2),18(a3)
[00010d2c] 47e8 002c                 lea.l      44(a0),a3
[00010d30] 36bc 0008                 move.w     #$0008,(a3)
[00010d34] 377c 0007 0002            move.w     #$0007,2(a3)
[00010d3a] 2751 0004                 move.l     (a1),4(a3)
[00010d3e] 377c 0004 0008            move.w     #$0004,8(a3)
[00010d44] 42ab 000a                 clr.l      10(a3)
[00010d48] 277c 0000 00ff 000e       move.l     #$000000FF,14(a3)
[00010d50] 2752 0012                 move.l     (a2),18(a3)
[00010d54] 47e8 0042                 lea.l      66(a0),a3
[00010d58] 36bc 000f                 move.w     #$000F,(a3)
[00010d5c] 377c 000e 0002            move.w     #$000E,2(a3)
[00010d62] 2751 0004                 move.l     (a1),4(a3)
[00010d66] 377c 000d 0008            move.w     #$000D,8(a3)
[00010d6c] 42ab 000a                 clr.l      10(a3)
[00010d70] 277c 0000 0064 000e       move.l     #$00000064,14(a3)
[00010d78] 2752 0012                 move.l     (a2),18(a3)
[00010d7c] 47e8 0058                 lea.l      88(a0),a3
[00010d80] 36bc 0012                 move.w     #$0012,(a3)
[00010d84] 377c 0011 0002            move.w     #$0011,2(a3)
[00010d8a] 2751 0004                 move.l     (a1),4(a3)
[00010d8e] 377c 0010 0008            move.w     #$0010,8(a3)
[00010d94] 42ab 000a                 clr.l      10(a3)
[00010d98] 277c 0000 0064 000e       move.l     #$00000064,14(a3)
[00010da0] 2752 0012                 move.l     (a2),18(a3)
[00010da4] 47e8 006e                 lea.l      110(a0),a3
[00010da8] 36bc 0020                 move.w     #$0020,(a3)
[00010dac] 377c 001f 0002            move.w     #$001F,2(a3)
[00010db2] 2751 0004                 move.l     (a1),4(a3)
[00010db6] 377c 0021 0008            move.w     #$0021,8(a3)
[00010dbc] 42ab 000a                 clr.l      10(a3)
[00010dc0] 277c 0000 03e7 000e       move.l     #$000003E7,14(a3)
[00010dc8] 2752 0012                 move.l     (a2),18(a3)
[00010dcc] 265f                      movea.l    (a7)+,a3
[00010dce] 245f                      movea.l    (a7)+,a2
[00010dd0] 4e75                      rts
f_default_sliders:
[00010dd2] 2f0a                      move.l     a2,-(a7)
[00010dd4] 45f9 0001 2fca            lea.l      $00012FCA,a2 set_slider
[00010dda] 157c 00ff ff56            move.b     #$FF,-170(a2)
[00010de0] 157c 00ff ff57            move.b     #$FF,-169(a2)
[00010de6] 157c 00ff ff58            move.b     #$FF,-168(a2)
[00010dec] 157c 0064 ff5c            move.b     #$64,-164(a2)
[00010df2] 157c 0046 ff5e            move.b     #$46,-162(a2)
[00010df8] 157c 00ff ff59            move.b     #$FF,-167(a2)
[00010dfe] 157c 00ff ff5a            move.b     #$FF,-166(a2)
[00010e04] 157c 00ff ff5b            move.b     #$FF,-165(a2)
[00010e0a] 157c 0064 ff5d            move.b     #$64,-163(a2)
[00010e10] 157c 0050 ff5f            move.b     #$50,-161(a2)
[00010e16] 357c 00c8 ff60            move.w     #$00C8,-160(a2)
[00010e1c] 4240                      clr.w      d0
[00010e1e] 102a ff56                 move.b     -170(a2),d0
[00010e22] 48c0                      ext.l      d0
[00010e24] 41ea ff7c                 lea.l      -132(a2),a0
[00010e28] 2252                      movea.l    (a2),a1
[00010e2a] 4e91                      jsr        (a1)
[00010e2c] 4240                      clr.w      d0
[00010e2e] 102a ff57                 move.b     -169(a2),d0
[00010e32] 48c0                      ext.l      d0
[00010e34] 41ea ff92                 lea.l      -110(a2),a0
[00010e38] 2252                      movea.l    (a2),a1
[00010e3a] 4e91                      jsr        (a1)
[00010e3c] 4240                      clr.w      d0
[00010e3e] 102a ff58                 move.b     -168(a2),d0
[00010e42] 48c0                      ext.l      d0
[00010e44] 41ea ffa8                 lea.l      -88(a2),a0
[00010e48] 2252                      movea.l    (a2),a1
[00010e4a] 4e91                      jsr        (a1)
[00010e4c] 4240                      clr.w      d0
[00010e4e] 102a ff5c                 move.b     -164(a2),d0
[00010e52] 48c0                      ext.l      d0
[00010e54] 41ea ffbe                 lea.l      -66(a2),a0
[00010e58] 2252                      movea.l    (a2),a1
[00010e5a] 4e91                      jsr        (a1)
[00010e5c] 4240                      clr.w      d0
[00010e5e] 102a ff5e                 move.b     -162(a2),d0
[00010e62] 48c0                      ext.l      d0
[00010e64] 41ea ffd4                 lea.l      -44(a2),a0
[00010e68] 2252                      movea.l    (a2),a1
[00010e6a] 4e91                      jsr        (a1)
[00010e6c] 302a ff60                 move.w     -160(a2),d0
[00010e70] 48c0                      ext.l      d0
[00010e72] 41ea ffea                 lea.l      -22(a2),a0
[00010e76] 2252                      movea.l    (a2),a1
[00010e78] 4e91                      jsr        (a1)
[00010e7a] 245f                      movea.l    (a7)+,a2
[00010e7c] 4e75                      rts
save_setting:
[00010e7e] 2f0a                      move.l     a2,-(a7)
[00010e80] 4fef ffca                 lea.l      -54(a7),a7
[00010e84] 41ef 0022                 lea.l      34(a7),a0
[00010e88] 6100 02a8                 bsr        write_setting
[00010e8c] 7221                      moveq.l    #33,d1
[00010e8e] 4240                      clr.w      d0
[00010e90] 41d7                      lea.l      (a7),a0
[00010e92] 6100 1354                 bsr        memset
[00010e96] 4857                      pea.l      (a7)
[00010e98] 7214                      moveq.l    #20,d1
[00010e9a] 43ef 0026                 lea.l      38(a7),a1
[00010e9e] 3039 0001 2f44            move.w     $00012F44,d0 module_id
[00010ea4] 41f9 0001 2be4            lea.l      module_info,a0
[00010eaa] 2479 0001 2fd2            movea.l    $00012FD2,a2
[00010eb0] 4e92                      jsr        (a2)
[00010eb2] 584f                      addq.w     #4,a7
[00010eb4] 43d7                      lea.l      (a7),a1
[00010eb6] 2079 0001 2f3c            movea.l    $00012F3C,a0 main_form
[00010ebc] 2468 04ec                 movea.l    1260(a0),a2
[00010ec0] 2052                      movea.l    (a2),a0
[00010ec2] 6100 10aa                 bsr        strcpy
[00010ec6] 4241                      clr.w      d1
[00010ec8] 7034                      moveq.l    #52,d0
[00010eca] 93c9                      suba.l     a1,a1
[00010ecc] 2079 0001 2f40            movea.l    $00012F40,a0 my_window
[00010ed2] 2479 0001 2fd6            movea.l    $00012FD6,a2
[00010ed8] 4e92                      jsr        (a2)
[00010eda] 4fef 0036                 lea.l      54(a7),a7
[00010ede] 245f                      movea.l    (a7)+,a2
[00010ee0] 4e75                      rts
load_setting:
[00010ee2] 2f0a                      move.l     a2,-(a7)
[00010ee4] 2f0b                      move.l     a3,-(a7)
[00010ee6] 4fef ffde                 lea.l      -34(a7),a7
[00010eea] 7221                      moveq.l    #33,d1
[00010eec] 4240                      clr.w      d0
[00010eee] 41d7                      lea.l      (a7),a0
[00010ef0] 6100 12f6                 bsr        memset
[00010ef4] 43d7                      lea.l      (a7),a1
[00010ef6] 3039 0001 2f44            move.w     $00012F44,d0 module_id
[00010efc] 41f9 0001 2be4            lea.l      module_info,a0
[00010f02] 2479 0001 2fce            movea.l    $00012FCE,a2
[00010f08] 4e92                      jsr        (a2)
[00010f0a] 2648                      movea.l    a0,a3
[00010f0c] 200b                      move.l     a3,d0
[00010f0e] 6736                      beq.s      $00010F46
[00010f10] 43d7                      lea.l      (a7),a1
[00010f12] 2479 0001 2f3c            movea.l    $00012F3C,a2 main_form
[00010f18] 206a 04ec                 movea.l    1260(a2),a0
[00010f1c] 2050                      movea.l    (a0),a0
[00010f1e] 6100 104e                 bsr        strcpy
[00010f22] 4241                      clr.w      d1
[00010f24] 7034                      moveq.l    #52,d0
[00010f26] 93c9                      suba.l     a1,a1
[00010f28] 2079 0001 2f40            movea.l    $00012F40,a0 my_window
[00010f2e] 2479 0001 2fd6            movea.l    $00012FD6,a2
[00010f34] 4e92                      jsr        (a2)
[00010f36] 204b                      movea.l    a3,a0
[00010f38] 6100 0016                 bsr.w      apply_setting
[00010f3c] 204b                      movea.l    a3,a0
[00010f3e] 2279 0001 2fda            movea.l    $00012FDA,a1
[00010f44] 4e91                      jsr        (a1)
[00010f46] 4fef 0022                 lea.l      34(a7),a7
[00010f4a] 265f                      movea.l    (a7)+,a3
[00010f4c] 245f                      movea.l    (a7)+,a2
[00010f4e] 4e75                      rts
apply_setting:
[00010f50] 48e7 0038                 movem.l    a2-a4,-(a7)
[00010f54] 2448                      movea.l    a0,a2
[00010f56] 47f9 0001 2fca            lea.l      $00012FCA,a3
[00010f5c] 176a 0004 ff56            move.b     4(a2),-170(a3)
[00010f62] 176a 0005 ff57            move.b     5(a2),-169(a3)
[00010f68] 176a 0006 ff58            move.b     6(a2),-168(a3)
[00010f6e] 176a 0007 ff59            move.b     7(a2),-167(a3)
[00010f74] 176a 0008 ff5a            move.b     8(a2),-166(a3)
[00010f7a] 176a 0009 ff5b            move.b     9(a2),-165(a3)
[00010f80] 176a 000a ff5c            move.b     10(a2),-164(a3)
[00010f86] 176a 000b ff5d            move.b     11(a2),-163(a3)
[00010f8c] 176a 000c ff5e            move.b     12(a2),-162(a3)
[00010f92] 176a 000d ff5f            move.b     13(a2),-161(a3)
[00010f98] 376a 000e ff60            move.w     14(a2),-160(a3)
[00010f9e] 302b ff60                 move.w     -160(a3),d0
[00010fa2] 48c0                      ext.l      d0
[00010fa4] 41eb ffea                 lea.l      -22(a3),a0
[00010fa8] 2253                      movea.l    (a3),a1
[00010faa] 4e91                      jsr        (a1)
[00010fac] 49eb ff72                 lea.l      -142(a3),a4
[00010fb0] 4240                      clr.w      d0
[00010fb2] 102a 0010                 move.b     16(a2),d0
[00010fb6] 2054                      movea.l    (a4),a0
[00010fb8] 3140 04a2                 move.w     d0,1186(a0)
[00010fbc] 4241                      clr.w      d1
[00010fbe] 122a 0013                 move.b     19(a2),d1
[00010fc2] 2054                      movea.l    (a4),a0
[00010fc4] 3141 03b2                 move.w     d1,946(a0)
[00010fc8] 2054                      movea.l    (a4),a0
[00010fca] 0268 fffe 042a            andi.w     #$FFFE,1066(a0)
[00010fd0] 2054                      movea.l    (a4),a0
[00010fd2] 0268 fffe 0442            andi.w     #$FFFE,1090(a0)
[00010fd8] 2054                      movea.l    (a4),a0
[00010fda] 0268 fffe 0472            andi.w     #$FFFE,1138(a0)
[00010fe0] 0c2a 002c 0011            cmpi.b     #$2C,17(a2)
[00010fe6] 660a                      bne.s      $00010FF2
[00010fe8] 2054                      movea.l    (a4),a0
[00010fea] 0068 0001 042a            ori.w      #$0001,1066(a0)
[00010ff0] 6022                      bra.s      $00011014
[00010ff2] 0c2a 002d 0011            cmpi.b     #$2D,17(a2)
[00010ff8] 660a                      bne.s      $00011004
[00010ffa] 2054                      movea.l    (a4),a0
[00010ffc] 0068 0001 0442            ori.w      #$0001,1090(a0)
[00011002] 6010                      bra.s      $00011014
[00011004] 0c2a 002f 0011            cmpi.b     #$2F,17(a2)
[0001100a] 6608                      bne.s      $00011014
[0001100c] 2054                      movea.l    (a4),a0
[0001100e] 0068 0001 0472            ori.w      #$0001,1138(a0)
[00011014] 4240                      clr.w      d0
[00011016] 102a 0012                 move.b     18(a2),d0
[0001101a] 2054                      movea.l    (a4),a0
[0001101c] 3140 0382                 move.w     d0,898(a0)
[00011020] 4241                      clr.w      d1
[00011022] 93c9                      suba.l     a1,a1
[00011024] 206b ff76                 movea.l    -138(a3),a0
[00011028] 246b 000c                 movea.l    12(a3),a2
[0001102c] 702c                      moveq.l    #44,d0
[0001102e] 4e92                      jsr        (a2)
[00011030] 4241                      clr.w      d1
[00011032] 702d                      moveq.l    #45,d0
[00011034] 93c9                      suba.l     a1,a1
[00011036] 206b ff76                 movea.l    -138(a3),a0
[0001103a] 246b 000c                 movea.l    12(a3),a2
[0001103e] 4e92                      jsr        (a2)
[00011040] 4241                      clr.w      d1
[00011042] 702f                      moveq.l    #47,d0
[00011044] 93c9                      suba.l     a1,a1
[00011046] 206b ff76                 movea.l    -138(a3),a0
[0001104a] 246b 000c                 movea.l    12(a3),a2
[0001104e] 4e92                      jsr        (a2)
[00011050] 4241                      clr.w      d1
[00011052] 7025                      moveq.l    #37,d0
[00011054] 93c9                      suba.l     a1,a1
[00011056] 206b ff76                 movea.l    -138(a3),a0
[0001105a] 246b 000c                 movea.l    12(a3),a2
[0001105e] 4e92                      jsr        (a2)
[00011060] 4241                      clr.w      d1
[00011062] 7027                      moveq.l    #39,d0
[00011064] 93c9                      suba.l     a1,a1
[00011066] 206b ff76                 movea.l    -138(a3),a0
[0001106a] 246b 000c                 movea.l    12(a3),a2
[0001106e] 4e92                      jsr        (a2)
[00011070] 4241                      clr.w      d1
[00011072] 7031                      moveq.l    #49,d0
[00011074] 93c9                      suba.l     a1,a1
[00011076] 206b ff76                 movea.l    -138(a3),a0
[0001107a] 246b 000c                 movea.l    12(a3),a2
[0001107e] 4e92                      jsr        (a2)
[00011080] 7001                      moveq.l    #1,d0
[00011082] 2054                      movea.l    (a4),a0
[00011084] c068 0262                 and.w      610(a0),d0
[00011088] 6752                      beq.s      $000110DC
[0001108a] 4240                      clr.w      d0
[0001108c] 102b ff56                 move.b     -170(a3),d0
[00011090] 48c0                      ext.l      d0
[00011092] 41eb ff7c                 lea.l      -132(a3),a0
[00011096] 2253                      movea.l    (a3),a1
[00011098] 4e91                      jsr        (a1)
[0001109a] 4240                      clr.w      d0
[0001109c] 102b ff57                 move.b     -169(a3),d0
[000110a0] 48c0                      ext.l      d0
[000110a2] 41eb ff92                 lea.l      -110(a3),a0
[000110a6] 2253                      movea.l    (a3),a1
[000110a8] 4e91                      jsr        (a1)
[000110aa] 4240                      clr.w      d0
[000110ac] 102b ff58                 move.b     -168(a3),d0
[000110b0] 48c0                      ext.l      d0
[000110b2] 41eb ffa8                 lea.l      -88(a3),a0
[000110b6] 2253                      movea.l    (a3),a1
[000110b8] 4e91                      jsr        (a1)
[000110ba] 4240                      clr.w      d0
[000110bc] 102b ff5c                 move.b     -164(a3),d0
[000110c0] 48c0                      ext.l      d0
[000110c2] 41eb ffbe                 lea.l      -66(a3),a0
[000110c6] 2253                      movea.l    (a3),a1
[000110c8] 4e91                      jsr        (a1)
[000110ca] 4240                      clr.w      d0
[000110cc] 102b ff5e                 move.b     -162(a3),d0
[000110d0] 48c0                      ext.l      d0
[000110d2] 41eb ffd4                 lea.l      -44(a3),a0
[000110d6] 2253                      movea.l    (a3),a1
[000110d8] 4e91                      jsr        (a1)
[000110da] 6050                      bra.s      $0001112C
[000110dc] 4240                      clr.w      d0
[000110de] 102b ff59                 move.b     -167(a3),d0
[000110e2] 48c0                      ext.l      d0
[000110e4] 41eb ff7c                 lea.l      -132(a3),a0
[000110e8] 2253                      movea.l    (a3),a1
[000110ea] 4e91                      jsr        (a1)
[000110ec] 4240                      clr.w      d0
[000110ee] 102b ff5a                 move.b     -166(a3),d0
[000110f2] 48c0                      ext.l      d0
[000110f4] 41eb ff92                 lea.l      -110(a3),a0
[000110f8] 2253                      movea.l    (a3),a1
[000110fa] 4e91                      jsr        (a1)
[000110fc] 4240                      clr.w      d0
[000110fe] 102b ff5b                 move.b     -165(a3),d0
[00011102] 48c0                      ext.l      d0
[00011104] 41eb ffa8                 lea.l      -88(a3),a0
[00011108] 2253                      movea.l    (a3),a1
[0001110a] 4e91                      jsr        (a1)
[0001110c] 4240                      clr.w      d0
[0001110e] 102b ff5d                 move.b     -163(a3),d0
[00011112] 48c0                      ext.l      d0
[00011114] 41eb ffbe                 lea.l      -66(a3),a0
[00011118] 2253                      movea.l    (a3),a1
[0001111a] 4e91                      jsr        (a1)
[0001111c] 4240                      clr.w      d0
[0001111e] 102b ff5f                 move.b     -161(a3),d0
[00011122] 48c0                      ext.l      d0
[00011124] 41eb ffd4                 lea.l      -44(a3),a0
[00011128] 2253                      movea.l    (a3),a1
[0001112a] 4e91                      jsr        (a1)
[0001112c] 4cdf 1c00                 movem.l    (a7)+,a2-a4
[00011130] 4e75                      rts
write_setting:
[00011132] 2f0a                      move.l     a2,-(a7)
[00011134] 20bc 0000 0100            move.l     #$00000100,(a0)
[0001113a] 43f9 0001 2f3c            lea.l      $00012F3C,a1 main_form
[00011140] 1169 ffe4 0004            move.b     -28(a1),4(a0)
[00011146] 1169 ffe5 0005            move.b     -27(a1),5(a0)
[0001114c] 1169 ffe6 0006            move.b     -26(a1),6(a0)
[00011152] 1169 ffe7 0007            move.b     -25(a1),7(a0)
[00011158] 1169 ffe8 0008            move.b     -24(a1),8(a0)
[0001115e] 1169 ffe9 0009            move.b     -23(a1),9(a0)
[00011164] 1169 ffea 000a            move.b     -22(a1),10(a0)
[0001116a] 1169 ffeb 000b            move.b     -21(a1),11(a0)
[00011170] 1169 ffec 000c            move.b     -20(a1),12(a0)
[00011176] 1169 ffed 000d            move.b     -19(a1),13(a0)
[0001117c] 3169 ffee 000e            move.w     -18(a1),14(a0)
[00011182] 7001                      moveq.l    #1,d0
[00011184] 2451                      movea.l    (a1),a2
[00011186] c06a 042a                 and.w      1066(a2),d0
[0001118a] 6708                      beq.s      $00011194
[0001118c] 117c 002c 0011            move.b     #$2C,17(a0)
[00011192] 6022                      bra.s      $000111B6
[00011194] 7001                      moveq.l    #1,d0
[00011196] 2451                      movea.l    (a1),a2
[00011198] c06a 0442                 and.w      1090(a2),d0
[0001119c] 6708                      beq.s      $000111A6
[0001119e] 117c 002d 0011            move.b     #$2D,17(a0)
[000111a4] 6010                      bra.s      $000111B6
[000111a6] 7001                      moveq.l    #1,d0
[000111a8] 2451                      movea.l    (a1),a2
[000111aa] c06a 0472                 and.w      1138(a2),d0
[000111ae] 6706                      beq.s      $000111B6
[000111b0] 117c 002f 0011            move.b     #$2F,17(a0)
[000111b6] 2451                      movea.l    (a1),a2
[000111b8] 116a 0383 0012            move.b     899(a2),18(a0)
[000111be] 2451                      movea.l    (a1),a2
[000111c0] 116a 03b3 0013            move.b     947(a2),19(a0)
[000111c6] 2451                      movea.l    (a1),a2
[000111c8] 116a 04a3 0010            move.b     1187(a2),16(a0)
[000111ce] 245f                      movea.l    (a7)+,a2
[000111d0] 4e75                      rts


	.data



[00012e8a]                           dc.b 'Laden',0

[00012e90]                           dc.w $4001
[00012e92]                           dc.w $c000
[00012e94]                           dc.b $00
[00012e95]                           dc.b $00
[00012e96]                           dc.b $00
[00012e97]                           dc.b $00
[00012e98]                           dc.b $00
[00012e99]                           dc.b $00
[00012e9a]                           dc.w $4004
[00012e9c]                           dc.w $c800
[00012e9e]                           dc.b $00
[00012e9f]                           dc.b $00
[00012ea0]                           dc.b $00
[00012ea1]                           dc.b $00
[00012ea2]                           dc.b $00
[00012ea3]                           dc.b $00
[00012ea4]                           dc.w $4000
[00012ea6]                           dc.w $828f
[00012ea8]                           dc.w $5c28
[00012eaa]                           dc.w $f5c2
[00012eac]                           dc.w $8f5c
[00012eae]                           dc.w $3fff
[00012eb0]                           dc.w $8000
[00012eb2]                           dc.b $00
[00012eb3]                           dc.b $00
[00012eb4]                           dc.b $00
[00012eb5]                           dc.b $00
[00012eb6]                           dc.b $00
[00012eb7]                           dc.b $00
[00012eb8]                           dc.w $4007
[00012eba]                           dc.w $b400
[00012ebc]                           dc.b $00
[00012ebd]                           dc.b $00
[00012ebe]                           dc.b $00
[00012ebf]                           dc.b $00
[00012ec0]                           dc.b $00
[00012ec1]                           dc.b $00
[00012ec2]                           dc.w $4005
[00012ec4]                           dc.w $a000
[00012ec6]                           dc.b $00
[00012ec7]                           dc.b $00
[00012ec8]                           dc.b $00
[00012ec9]                           dc.b $00
[00012eca]                           dc.b $00
[00012ecb]                           dc.b $00
[00012ecc]                           dc.w $4009
[00012ece]                           dc.w $8000
[00012ed0]                           dc.b $00
[00012ed1]                           dc.b $00
[00012ed2]                           dc.b $00
[00012ed3]                           dc.b $00
[00012ed4]                           dc.b $00
[00012ed5]                           dc.b $00
[00012ed6]                           dc.b 'Bump It Up  V0.1',0
[00012ee7]                           dc.b $00
[00012ee8]                           dc.b $00
[00012ee9]                           dc.b $00
[00012eea]                           dc.b $00
[00012eeb]                           dc.b $00
[00012eec]                           dc.b $00
[00012eed]                           dc.b $00
[00012eee]                           dc.b $00
[00012eef]                           dc.b $00
[00012ef0]                           dc.b $00
[00012ef1]                           dc.b $00
[00012ef2]                           dc.b $00
[00012ef3]                           dc.b $00
[00012ef4]                           dc.b $00
[00012ef5]                           dc.b $00
[00012ef6]                           dc.b $00
[00012ef7]                           dc.b $00
[00012ef8]                           dc.b $00
[00012ef9]                           dc.b $00
[00012efa]                           dc.b $00
[00012efb]                           dc.b $00
[00012efc]                           dc.b $00
[00012efd]                           dc.b $00
[00012efe]                           dc.b $00
[00012eff]                           dc.b $00
[00012f00]                           dc.b $00
[00012f01]                           dc.b $00
[00012f02]                           dc.b $00
[00012f03]                           dc.b $00
[00012f04]                           dc.b $00
[00012f05]                           dc.b $00
[00012f06]                           dc.b $00
[00012f07]                           dc.b $00
[00012f08] 00012fe2                  dc.l $00012fe2
[00012f0c] 00013000                  dc.l $00013000
[00012f10] 0001301e                  dc.l $0001301e
[00012f14] 00013126                  dc.l $00013126
[00012f18] 0001323e                  dc.l $0001323e
[00012f1c] 0001327e                  dc.l $0001327e

12f20: red
12f21: green
12f22: blue
12f23: gred
12f24: ggreen
12f25: gblue
12f26: diffuse_strength
12f27: glanz_hard
12f28: diffuse_hard
12f29: glanz_hard
12f2a: bm_strength

12f2c: kreuzz_x
12f2e: kreuz_y

12f30: input_pictures
12f3c: main_form
12f40: my_window
12f44: module_id
12f46: red_slider
12f5c: green_slider
12f72: blue_slider
12f88: m_str_slider
12f9e: m_hard_slider
12fb4: bm_str_slider

12FCA: set_slider
12fce: mconfLoad
13002: mconfSave
13006: redraw_window
1300a: SMfree
