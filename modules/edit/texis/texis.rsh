/*
 * GEM resource C output of texis
 *
 * created by ORCS 2.18
 */

#if !defined(__GNUC__) || !defined(__mc68000__)
#include <portab.h>
#endif

#ifndef __STDC__
# ifdef __PUREC__
#  define __STDC__ 1
# endif
#endif

#ifdef OS_WINDOWS
#  include <portaes.h>
#  define SHORT _WORD
#  ifdef __WIN32__
#    define _WORD signed short
#  else
#    define _WORD signed int
 #   pragma option -zE_FARDATA
#  endif
#else
#  ifdef __TURBOC__
#    include <aes.h>
#    define CP (_WORD *)
#  endif
#endif

#ifdef OS_UNIX
#  include <portaes.h>
#  define SHORT _WORD
#else
#  ifdef __GNUC__
#    ifndef __PORTAES_H__
#      if __GNUC__ < 4
#        include <aesbind.h>
#        ifndef _WORD
#          define _WORD int
#        endif
#        define CP (char *)
#      else
#        include <mt_gem.h>
#        ifndef _WORD
#          define _WORD short
#        endif
#        define CP (short *)
#      endif
#      define CW (short *)
#    endif
#  endif
#endif


#ifdef __SOZOBONX__
#  include <xgemfast.h>
#else
#  ifdef SOZOBON
#    include <aes.h>
#  endif
#endif

#ifdef MEGAMAX
#  include <gembind.h>
#  include <gemdefs.h>
#  include <obdefs.h>
#  define _WORD int
#  define SHORT int
#endif

#ifndef _VOID
#  define _VOID void
#endif

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef R_CICONBLK
#  define R_CICONBLK 17
#endif
#ifndef R_CICON
#  define R_CICON 18
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    ifdef __PUREC__
#      define _WORD int
#    else
#      define _WORD short
#    endif
#  endif
#endif

#ifndef _UBYTE
#  define _UBYTE char
#endif

#ifndef _BOOL
#  define _BOOL int
#endif

#ifndef _LONG
#  ifdef LONG
#    define _LONG LONG
#  else
#    define _LONG long
#  endif
#endif

#ifndef _ULONG
#  ifdef ULONG
#    define _ULONG ULONG
#  else
#    define _ULONG unsigned long
#  endif
#endif

#ifndef _LONG_PTR
#  define _LONG_PTR _LONG
#endif

#ifndef C_UNION
#ifdef __PORTAES_H__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __GEMLIB__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __PUREC__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __ALCYON__
#  define C_UNION(x) x
#endif
#endif
#ifndef C_UNION
#  define C_UNION(x) (_LONG_PTR)(x)
#endif

#ifndef SHORT
#  define SHORT short
#endif

#ifndef CP
#  define CP (SHORT *)
#endif

#ifndef CW
#  define CW (_WORD *)
#endif


#undef RSC_STATIC_FILE
#define RSC_STATIC_FILE 1

#include "texis.h"

#ifndef RSC_NAMED_FUNCTIONS
#  define RSC_NAMED_FUNCTIONS 0
#endif

#ifndef __ALCYON__
#undef defRSHInit
#undef defRSHInitBit
#undef defRSHInitStr
#ifndef RsArraySize
#define RsArraySize(array) (_WORD)(sizeof(array)/sizeof(array[0]))
#define RsPtrArraySize(type, array) (type *)array, RsArraySize(array)
#endif
#define defRSHInit( aa, bb ) RSHInit( aa, bb, RsPtrArraySize(OBJECT *, rs_trindex), RsArraySize(rs_object) )
#define defRSHInitBit( aa, bb ) RSHInitBit( aa, bb, RsPtrArraySize(BITBLK *, rs_frimg) )
#define defRSHInitStr( aa, bb ) RSHInitStr( aa, bb, RsPtrArraySize(_UBYTE *, rs_frstr) )
#endif

#ifdef __STDC__
#ifndef W_Cicon_Setpalette
extern _BOOL W_Cicon_Setpalette(_WORD *_palette);
#endif
#ifndef hrelease_objs
extern void hrelease_objs(OBJECT *_ob, _WORD _num_objs);
#endif
#ifndef hfix_objs
extern void *hfix_objs(RSHDR *_hdr, OBJECT *_ob, _WORD _num_objs);
#endif
#endif

#ifndef RLOCAL
#  if RSC_STATIC_FILE
#    ifdef LOCAL
#      define RLOCAL LOCAL
#    else
#      define RLOCAL static
#    endif
#  else
#    define RLOCAL
#  endif
#endif


#ifndef N_
#  define N_(x)
#endif


#if RSC_STATIC_FILE
#undef NUM_STRINGS
#undef NUM_BB
#undef NUM_IB
#undef NUM_CIB
#undef NUM_CIC
#undef NUM_TI
#undef NUM_FRSTR
#undef NUM_FRIMG
#undef NUM_OBS
#undef NUM_TREE
#undef NUM_UD
#define NUM_STRINGS 156
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		50
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     80
#define NUM_TREE	4
#define NUM_UD		0
#endif


static char texis_string_0[] = "Laden";
static char texis_string_1[] = "XXXXXXXX.MAP";
static char texis_string_2[] = "";
static char texis_string_3[] = "";
static char texis_string_4[] = "Map";
static char texis_string_5[] = "";
static char texis_string_6[] = "";
static char texis_string_7[] = "Map";
static char texis_string_8[] = "";
static char texis_string_9[] = "";
static char texis_string_10[] = "00000";
static char texis_string_11[] = "";
static char texis_string_12[] = "";
static char texis_string_13[] = "11111";
static char texis_string_14[] = "";
static char texis_string_15[] = "";
static char texis_string_16[] = "22222";
static char texis_string_17[] = "";
static char texis_string_18[] = "";
static char texis_string_19[] = "Laden";
static char texis_string_20[] = "XXXXXXXX.SMT";
static char texis_string_21[] = "";
static char texis_string_22[] = "";
static char texis_string_23[] = "Textur";
static char texis_string_24[] = "";
static char texis_string_25[] = "";
static char texis_string_26[] = "Textur";
static char texis_string_27[] = "";
static char texis_string_28[] = "";
static char texis_string_29[] = "Sichern";
static char texis_string_30[] = "Start";
static char texis_string_31[] = "55555";
static char texis_string_32[] = "";
static char texis_string_33[] = "";
static char texis_string_34[] = "Turb.";
static char texis_string_35[] = "";
static char texis_string_36[] = "";
static char texis_string_37[] = "Turb.";
static char texis_string_38[] = "";
static char texis_string_39[] = "";
static char texis_string_40[] = "44444";
static char texis_string_41[] = "";
static char texis_string_42[] = "";
static char texis_string_43[] = "Y-Scale";
static char texis_string_44[] = "";
static char texis_string_45[] = "";
static char texis_string_46[] = "Y-Scale";
static char texis_string_47[] = "";
static char texis_string_48[] = "";
static char texis_string_49[] = "00";
static char texis_string_50[] = "";
static char texis_string_51[] = "";
static char texis_string_52[] = "Farben";
static char texis_string_53[] = "";
static char texis_string_54[] = "";
static char texis_string_55[] = "Farben";
static char texis_string_56[] = "";
static char texis_string_57[] = "";
static char texis_string_58[] = " scharf ";
static char texis_string_59[] = "";
static char texis_string_60[] = "";
static char texis_string_61[] = "Map";
static char texis_string_62[] = "";
static char texis_string_63[] = "";
static char texis_string_64[] = "Map";
static char texis_string_65[] = "";
static char texis_string_66[] = "";
static char texis_string_67[] = "R";
static char texis_string_68[] = "";
static char texis_string_69[] = "";
static char texis_string_70[] = "R";
static char texis_string_71[] = "";
static char texis_string_72[] = "";
static char texis_string_73[] = "G";
static char texis_string_74[] = "";
static char texis_string_75[] = "";
static char texis_string_76[] = "G";
static char texis_string_77[] = "";
static char texis_string_78[] = "";
static char texis_string_79[] = "B";
static char texis_string_80[] = "";
static char texis_string_81[] = "";
static char texis_string_82[] = "B";
static char texis_string_83[] = "";
static char texis_string_84[] = "";
static char texis_string_85[] = "Laden";
static char texis_string_86[] = "Sichern";
static char texis_string_87[] = " keine ";
static char texis_string_88[] = "";
static char texis_string_89[] = "";
static char texis_string_90[] = "Turbulenz";
static char texis_string_91[] = "";
static char texis_string_92[] = "";
static char texis_string_93[] = "Turbulenz";
static char texis_string_94[] = "";
static char texis_string_95[] = "";
static char texis_string_96[] = "33333";
static char texis_string_97[] = "";
static char texis_string_98[] = "";
static char texis_string_99[] = "X-Scale";
static char texis_string_100[] = "";
static char texis_string_101[] = "";
static char texis_string_102[] = "X-Scale";
static char texis_string_103[] = "";
static char texis_string_104[] = "";
static char texis_string_105[] = " normal ";
static char texis_string_106[] = "";
static char texis_string_107[] = "";
static char texis_string_108[] = "Modus";
static char texis_string_109[] = "";
static char texis_string_110[] = "";
static char texis_string_111[] = "Modus";
static char texis_string_112[] = "";
static char texis_string_113[] = "";
static char texis_string_114[] = "keine";
static char texis_string_115[] = "";
static char texis_string_116[] = "";
static char texis_string_117[] = "Zoom";
static char texis_string_118[] = "";
static char texis_string_119[] = "";
static char texis_string_120[] = "Sinus";
static char texis_string_121[] = "";
static char texis_string_122[] = "";
static char texis_string_123[] = "Rotate";
static char texis_string_124[] = "";
static char texis_string_125[] = "";
static char texis_string_126[] = "16 Bit";
static char texis_string_127[] = "";
static char texis_string_128[] = "";
static char texis_string_129[] = "24 Bit";
static char texis_string_130[] = "";
static char texis_string_131[] = "";
static char texis_string_132[] = "scharf";
static char texis_string_133[] = "";
static char texis_string_134[] = "";
static char texis_string_135[] = "linear";
static char texis_string_136[] = "";
static char texis_string_137[] = "";
static char texis_string_138[] = "soft";
static char texis_string_139[] = "";
static char texis_string_140[] = "";
static char texis_string_141[] = "Xtrasoft";
static char texis_string_142[] = "";
static char texis_string_143[] = "";
static char texis_string_144[] = "normal";
static char texis_string_145[] = "";
static char texis_string_146[] = "";
static char texis_string_147[] = "Holz x";
static char texis_string_148[] = "";
static char texis_string_149[] = "";
static char texis_string_150[] = "Holz y";
static char texis_string_151[] = "";
static char texis_string_152[] = "";
static char texis_string_153[] = "Marmor";
static char texis_string_154[] = "";
static char texis_string_155[] = "";


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ texis_string_1, texis_string_2, texis_string_3, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 13,1 }, /* MAPNAME */
	{ texis_string_4, texis_string_5, texis_string_6, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 4,1 },
	{ texis_string_7, texis_string_8, texis_string_9, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 4,1 },
	{ texis_string_10, texis_string_11, texis_string_12, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* RS_A */
	{ texis_string_13, texis_string_14, texis_string_15, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* GS_A */
	{ texis_string_16, texis_string_17, texis_string_18, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* BS_A */
	{ texis_string_20, texis_string_21, texis_string_22, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 13,1 }, /* TEXNAME */
	{ texis_string_23, texis_string_24, texis_string_25, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 7,1 },
	{ texis_string_26, texis_string_27, texis_string_28, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 7,1 },
	{ texis_string_31, texis_string_32, texis_string_33, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* TS_A */
	{ texis_string_34, texis_string_35, texis_string_36, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 6,1 },
	{ texis_string_37, texis_string_38, texis_string_39, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 },
	{ texis_string_40, texis_string_41, texis_string_42, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* YS_A */
	{ texis_string_43, texis_string_44, texis_string_45, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 8,1 },
	{ texis_string_46, texis_string_47, texis_string_48, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 8,1 },
	{ texis_string_49, texis_string_50, texis_string_51, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 3,1 }, /* COL_NUMBER */
	{ texis_string_52, texis_string_53, texis_string_54, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 7,1 },
	{ texis_string_55, texis_string_56, texis_string_57, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 7,1 },
	{ texis_string_58, texis_string_59, texis_string_60, IBM, 0, TE_CNTR, 0x1100, 0x0, -1, 9,1 }, /* MODE_PB */
	{ texis_string_61, texis_string_62, texis_string_63, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 4,1 },
	{ texis_string_64, texis_string_65, texis_string_66, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 4,1 },
	{ texis_string_67, texis_string_68, texis_string_69, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 2,1 },
	{ texis_string_70, texis_string_71, texis_string_72, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 2,1 },
	{ texis_string_73, texis_string_74, texis_string_75, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 2,1 },
	{ texis_string_76, texis_string_77, texis_string_78, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 2,1 },
	{ texis_string_79, texis_string_80, texis_string_81, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 2,1 },
	{ texis_string_82, texis_string_83, texis_string_84, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 2,1 },
	{ texis_string_87, texis_string_88, texis_string_89, IBM, 0, TE_CNTR, 0x1100, 0x0, -1, 8,1 }, /* TURB_PB */
	{ texis_string_90, texis_string_91, texis_string_92, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 10,1 },
	{ texis_string_93, texis_string_94, texis_string_95, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 10,1 },
	{ texis_string_96, texis_string_97, texis_string_98, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* XS_A */
	{ texis_string_99, texis_string_100, texis_string_101, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 8,1 },
	{ texis_string_102, texis_string_103, texis_string_104, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 8,1 },
	{ texis_string_105, texis_string_106, texis_string_107, IBM, 0, TE_CNTR, 0x1100, 0x0, -1, 9,1 },
	{ texis_string_108, texis_string_109, texis_string_110, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 6,1 },
	{ texis_string_111, texis_string_112, texis_string_113, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 },
	{ texis_string_114, texis_string_115, texis_string_116, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 6,1 }, /* TURB_NONE */
	{ texis_string_117, texis_string_118, texis_string_119, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 5,1 }, /* TURB_ZOOM */
	{ texis_string_120, texis_string_121, texis_string_122, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 6,1 }, /* TURB_SIN */
	{ texis_string_123, texis_string_124, texis_string_125, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 }, /* TURB_ROTATE */
	{ texis_string_126, texis_string_127, texis_string_128, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 }, /* TURB_X */
	{ texis_string_129, texis_string_130, texis_string_131, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 }, /* TURB_Y */
	{ texis_string_132, texis_string_133, texis_string_134, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 }, /* MAP_SHARP */
	{ texis_string_135, texis_string_136, texis_string_137, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 }, /* MAP_LIN */
	{ texis_string_138, texis_string_139, texis_string_140, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 5,1 }, /* MAP_SOFT */
	{ texis_string_141, texis_string_142, texis_string_143, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 9,1 }, /* MAP_XSOFT */
	{ texis_string_144, texis_string_145, texis_string_146, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 },
	{ texis_string_147, texis_string_148, texis_string_149, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 },
	{ texis_string_150, texis_string_151, texis_string_152, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 },
	{ texis_string_153, texis_string_154, texis_string_155, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 7,1 }
};


static OBJECT rs_object[NUM_OBS] = {
/* TEX_MAIN */

	{ -1, 1, 62, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x11100L), 0,0, 1080,25 },
	{ 2, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1141L), 798,3857, 1036,1030 },
	{ 3, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(texis_string_0), 1552,274, 8,1 }, /* LOADMAP */
	{ 4, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 1538,274, 12,1 }, /* MAPNAME */
	{ 5, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1538,2321, 514,2048 },
	{ 6, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 1282,2065, 514,2048 },
	{ 8, 7, 7, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1795,1282, 1,264 }, /* RS_PAR */
	{ 6, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1172L), 0,2311, 1,2048 }, /* RED_SLIDER */
	{ 9, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 1026,3850, 1539,3328 }, /* RS_A */
	{ 11, 10, 10, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1032,1282, 1,264 }, /* GS_PAR */
	{ 9, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1173L), 0,2311, 1,2048 }, /* GREEN_SLIDER */
	{ 12, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 263,3850, 1539,3328 }, /* GS_A */
	{ 14, 13, 13, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 13,1282, 1,264 }, /* BS_PAR */
	{ 12, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1174L), 0,2311, 1,2048 }, /* BLUE_SLIDER */
	{ 15, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 1291,3850, 1539,3328 }, /* BS_A */
	{ 16, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(texis_string_19), 1296,3860, 8,1 }, /* LOADTEX */
	{ 17, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 1282,3860, 12,1 }, /* TEXNAME */
	{ 18, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 1282,1812, 1028,2048 },
	{ 19, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 1026,1556, 1028,2048 },
	{ 20, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(texis_string_29), 1296,3606, 8,1 }, /* SAVETEX */
	{ 21, -1, -1, G_BUTTON, OF_SELECTABLE|OF_DEFAULT|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(texis_string_30), 46,23, 8,1 }, /* START */
	{ 23, 22, 22, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 563,2306, 1,264 }, /* TS_PAR */
	{ 21, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* TURB_SLIDER */
	{ 24, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 1841,267, 1539,3328 }, /* TS_A */
	{ 25, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 50,2561, 1539,2816 },
	{ 26, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 1841,2305, 1539,2816 },
	{ 28, 27, 27, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 45,2306, 1,264 }, /* YS_PAR */
	{ 26, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* YS_SLIDER */
	{ 29, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 1323,267, 1539,3328 }, /* YS_A */
	{ 30, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 43,2561, 517,2816 },
	{ 31, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 1834,2305, 517,2816 },
	{ 32, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1141L), 533,1284, 4,2 },
	{ 33, -1, -1, (21<<8)+G_BOXCHAR, OF_FL3DACT, OS_NORMAL, C_UNION(0x1001100L), 533,1283, 4,1 }, /* COL_UP */
	{ 34, -1, -1, (21<<8)+G_BOXCHAR, OF_FL3DACT, OS_NORMAL, C_UNION(0x2001100L), 533,1286, 4,1 }, /* COL_DN */
	{ 35, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 530,3332, 2,1 }, /* COL_NUMBER */
	{ 36, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 1042,1282, 1028,2816 },
	{ 37, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 786,1026, 1028,2816 },
	{ 38, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_SHADOWED, C_UNION(&rs_tedinfo[18]), 1577,3853, 778,1 }, /* MODE_PB */
	{ 39, -1, -1, (45<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0x12100L), 1076,3085, 3,2049 }, /* MODE_CB */
	{ 40, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 809,13, 514,2816 },
	{ 41, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 553,3852, 514,2816 },
	{ 42, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[21]), 4,1537, 1536,2816 },
	{ 43, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[22]), 1795,1281, 1536,2816 },
	{ 44, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[23]), 1544,1537, 1536,2816 },
	{ 45, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[24]), 1288,1281, 1536,2816 },
	{ 46, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[25]), 525,1537, 1536,2816 },
	{ 47, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[26]), 269,1281, 1536,2816 },
	{ 48, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(texis_string_85), 786,1288, 10,1 }, /* LOAD_COLS */
	{ 49, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(texis_string_86), 786,1290, 10,1 }, /* SAVE_COLS */
	{ 50, -1, -1, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0x100L), 514,3856, 820,256 },
	{ 51, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_SHADOWED, C_UNION(&rs_tedinfo[27]), 282,3853, 778,1 }, /* TURB_PB */
	{ 52, -1, -1, (45<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0x12100L), 1828,3085, 3,2049 }, /* TURB_CB */
	{ 53, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[28]), 1305,13, 1542,2816 },
	{ 54, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[29]), 1049,3852, 1542,2816 },
	{ 56, 55, 55, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 807,2306, 1,264 }, /* XS_PAR */
	{ 54, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* XS_SLIDER */
	{ 57, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[30]), 38,267, 1539,3328 }, /* XS_A */
	{ 58, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[31]), 293,2561, 517,2816 },
	{ 59, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[32]), 37,2305, 517,2816 },
	{ 60, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_SHADOWED, C_UNION(&rs_tedinfo[33]), 266,14, 778,1 },
	{ 61, -1, -1, (45<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0x12100L), 1812,3341, 3,2049 },
	{ 62, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[34]), 1289,269, 1539,2816 },
	{ 0, -1, -1, G_TEXT, OF_LASTOB, OS_NORMAL, C_UNION(&rs_tedinfo[35]), 1033,13, 1539,2816 },

/* TURB_POPUP */

	{ -1, 1, 6, G_BOX, OF_FL3DBAK, OS_SHADOWED, C_UNION(0x11100L), 0,0, 11,2567 },
	{ 2, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[36]), 768,768, 778,1 }, /* TURB_NONE */
	{ 3, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[37]), 768,1793, 778,1 }, /* TURB_ZOOM */
	{ 4, -1, -1, G_BOXTEXT, OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[38]), 768,2818, 778,1 }, /* TURB_SIN */
	{ 5, -1, -1, G_BOXTEXT, OF_FL3DACT, OS_NORMAL, C_UNION(&rs_tedinfo[39]), 768,3843, 778,1 }, /* TURB_ROTATE */
	{ 6, -1, -1, G_BOXTEXT, OF_FL3DACT, OS_NORMAL, C_UNION(&rs_tedinfo[40]), 768,773, 778,1 }, /* TURB_X */
	{ 0, -1, -1, G_BOXTEXT, OF_LASTOB|OF_FL3DACT, OS_NORMAL, C_UNION(&rs_tedinfo[41]), 768,1798, 778,1 }, /* TURB_Y */

/* MAP_POPUP */

	{ -1, 1, 4, G_BOX, OF_FL3DBAK, OS_SHADOWED, C_UNION(0x11100L), 0,0, 267,517 },
	{ 2, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[42]), 768,768, 778,1 }, /* MAP_SHARP */
	{ 3, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[43]), 768,1793, 778,1 }, /* MAP_LIN */
	{ 4, -1, -1, G_BOXTEXT, OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[44]), 768,2818, 778,1 }, /* MAP_SOFT */
	{ 0, -1, -1, G_BOXTEXT, OF_LASTOB|OF_FL3DACT, OS_NORMAL, C_UNION(&rs_tedinfo[45]), 768,3843, 778,1 }, /* MAP_XSOFT */

/* MODE_POPUP */

	{ -1, 1, 4, G_BOX, OF_FL3DBAK, OS_SHADOWED, C_UNION(0x11100L), 0,0, 523,2826 },
	{ 2, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[46]), 768,768, 778,1 },
	{ 3, -1, -1, G_BOXTEXT, OF_SELECTABLE|OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[47]), 768,1793, 778,1 },
	{ 4, -1, -1, G_BOXTEXT, OF_FL3DACT, OS_DRAW3D, C_UNION(&rs_tedinfo[48]), 768,2818, 778,1 },
	{ 0, -1, -1, G_BOXTEXT, OF_LASTOB|OF_FL3DACT, OS_NORMAL, C_UNION(&rs_tedinfo[49]), 768,3843, 778,1 }
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0], /* TEX_MAIN */
	&rs_object[63], /* TURB_POPUP */
	&rs_object[70], /* MAP_POPUP */
	&rs_object[75] /* MODE_POPUP */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD texis_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD texis_rsc_load(wchar, hchar)
_WORD wchar;
_WORD wchar;
#endif
{
#ifndef RSC_HAS_PALETTE
#  define RSC_HAS_PALETTE 0
#endif
#ifndef RSC_USE_PALETTE
#  define RSC_USE_PALETTE 0
#endif
#if RSC_HAS_PALETTE || RSC_USE_PALETTE
	W_Cicon_Setpalette(&rgb_palette[0][0]);
#endif
#if NUM_OBS != 0
	{
		_WORD Obj;
		OBJECT *tree;
		for (Obj = 0, tree = rs_object; Obj < NUM_OBS; Obj++, tree++)
		{
			tree->ob_x = wchar * (tree->ob_x & 0xff) + (tree->ob_x >> 8);
			tree->ob_y = hchar * (tree->ob_y & 0xff) + (tree->ob_y >> 8);
			tree->ob_width = wchar * (tree->ob_width & 0xff) + (tree->ob_width >> 8);
			tree->ob_height = hchar * (tree->ob_height & 0xff) + (tree->ob_height >> 8);
		}
		hfix_objs(NULL, rs_object, NUM_OBS);
	}
#endif
	return 1;
}


#ifdef __STDC__
_WORD texis_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD texis_rsc_gaddr(type, idx, gaddr)
_WORD type;
_WORD idx;
void *gaddr;
#endif
{
	switch (type)
	{
#if NUM_TREE != 0
	case R_TREE:
		if (idx < 0 || idx >= NUM_TREE)
			return 0;
		*((OBJECT **)gaddr) = rs_trindex[idx];
		break;
#endif
#if NUM_OBS != 0
	case R_OBJECT:
		if (idx < 0 || idx >= NUM_OBS)
			return 0;
		*((OBJECT **)gaddr) = &rs_object[idx];
		break;
#endif
#if NUM_TI != 0
	case R_TEDINFO:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((TEDINFO **)gaddr) = &rs_tedinfo[idx];
		break;
#endif
#if NUM_IB != 0
	case R_ICONBLK:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((ICONBLK **)gaddr) = &rs_iconblk[idx];
		break;
#endif
#if NUM_BB != 0
	case R_BITBLK:
		if (idx < 0 || idx >= NUM_BB)
			return 0;
		*((BITBLK **)gaddr) = &rs_bitblk[idx];
		break;
#endif
#if NUM_FRSTR != 0
	case R_STRING:
		if (idx < 0 || idx >= NUM_FRSTR)
			return 0;
		*((char **)gaddr) = (char *)(rs_frstr[idx]);
		break;
#endif
#if NUM_FRIMG != 0
	case R_IMAGEDATA:
		if (idx < 0 || idx >= NUM_FRIMG)
			return 0;
		*((BITBLK **)gaddr) = rs_frimg[idx];
		break;
#endif
#if NUM_OBS != 0
	case R_OBSPEC:
		if (idx < 0 || idx >= NUM_OBS)
			return 0;
		*((_LONG_PTR **)gaddr) = &rs_object[idx].ob_spec.index;
		break;
#endif
#if NUM_TI != 0
	case R_TEPTEXT:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_tedinfo[idx].te_ptext);
		break;
#endif
#if NUM_TI != 0
	case R_TEPTMPLT:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_tedinfo[idx].te_ptmplt);
		break;
#endif
#if NUM_TI != 0
	case R_TEPVALID:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_tedinfo[idx].te_pvalid);
		break;
#endif
#if NUM_IB != 0
	case R_IBPMASK:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_iconblk[idx].ib_pmask);
		break;
#endif
#if NUM_IB != 0
	case R_IBPDATA:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_iconblk[idx].ib_pdata);
		break;
#endif
#if NUM_IB != 0
	case R_IBPTEXT:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_iconblk[idx].ib_ptext);
		break;
#endif
#if NUM_BB != 0
	case R_BIPDATA:
		if (idx < 0 || idx >= NUM_BB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_bitblk[idx].bi_pdata);
		break;
#endif
#if NUM_FRSTR != 0
	case R_FRSTR:
		if (idx < 0 || idx >= NUM_FRSTR)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_frstr[idx]);
		break;
#endif
#if NUM_FRIMG != 0
	case R_FRIMG:
		if (idx < 0 || idx >= NUM_FRIMG)
			return 0;
		*((BITBLK ***)gaddr) = &rs_frimg[idx];
		break;
#endif
	default:
		return 0;
	}
	return 1;
}


#ifdef __STDC__
_WORD texis_rsc_free(void)
#else
_WORD texis_rsc_free()
#endif
{
#if NUM_OBS != 0
	hrelease_objs(rs_object, NUM_OBS);
#endif
	return 1;
}

#endif /* RSC_NAMED_FUNCTIONS */

#else /* !RSC_STATIC_FILE */
#if 0
_WORD rs_numstrings = 156;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 50;
_WORD rs_numobs = 80;
_WORD rs_numtree = 4;

char rs_name[] = "texis.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
