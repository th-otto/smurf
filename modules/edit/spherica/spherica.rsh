/*
 * GEM resource C output of spherica
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

#include "spherica.h"

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
#define NUM_STRINGS 189
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		62
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     111
#define NUM_TREE	1
#define NUM_UD		0
#endif


static char spherica_string_0[] = "r";
static char spherica_string_1[] = "";
static char spherica_string_2[] = "";
static char spherica_string_3[] = "_____";
static char spherica_string_4[] = "";
static char spherica_string_5[] = "";
static char spherica_string_6[] = "g";
static char spherica_string_7[] = "";
static char spherica_string_8[] = "";
static char spherica_string_9[] = "_____";
static char spherica_string_10[] = "";
static char spherica_string_11[] = "";
static char spherica_string_12[] = "b";
static char spherica_string_13[] = "";
static char spherica_string_14[] = "";
static char spherica_string_15[] = "_____";
static char spherica_string_16[] = "";
static char spherica_string_17[] = "";
static char spherica_string_18[] = " Textur";
static char spherica_string_19[] = "";
static char spherica_string_20[] = "";
static char spherica_string_21[] = "_____";
static char spherica_string_22[] = "";
static char spherica_string_23[] = "";
static char spherica_string_24[] = "xScale %.";
static char spherica_string_25[] = "";
static char spherica_string_26[] = "";
static char spherica_string_27[] = "yscale %.";
static char spherica_string_28[] = "";
static char spherica_string_29[] = "";
static char spherica_string_30[] = "xOff Pixel";
static char spherica_string_31[] = "";
static char spherica_string_32[] = "";
static char spherica_string_33[] = "yOff Pixel";
static char spherica_string_34[] = "";
static char spherica_string_35[] = "";
static char spherica_string_36[] = "001000";
static char spherica_string_37[] = "______";
static char spherica_string_38[] = "999999";
static char spherica_string_39[] = "001000";
static char spherica_string_40[] = "______";
static char spherica_string_41[] = "999999";
static char spherica_string_42[] = "00000";
static char spherica_string_43[] = "_____";
static char spherica_string_44[] = "99999";
static char spherica_string_45[] = "00000";
static char spherica_string_46[] = "_____";
static char spherica_string_47[] = "99999";
static char spherica_string_48[] = "->Bump";
static char spherica_string_49[] = "";
static char spherica_string_50[] = "";
static char spherica_string_51[] = " rauh";
static char spherica_string_52[] = "";
static char spherica_string_53[] = "";
static char spherica_string_54[] = "_____";
static char spherica_string_55[] = "";
static char spherica_string_56[] = "";
static char spherica_string_57[] = " Transparenz";
static char spherica_string_58[] = "";
static char spherica_string_59[] = "";
static char spherica_string_60[] = "_____";
static char spherica_string_61[] = "";
static char spherica_string_62[] = "";
static char spherica_string_63[] = "1000";
static char spherica_string_64[] = "_.___";
static char spherica_string_65[] = "9999";
static char spherica_string_66[] = "Idx";
static char spherica_string_67[] = "";
static char spherica_string_68[] = "";
static char spherica_string_69[] = "_____";
static char spherica_string_70[] = "";
static char spherica_string_71[] = "";
static char spherica_string_72[] = "xScale %.";
static char spherica_string_73[] = "";
static char spherica_string_74[] = "";
static char spherica_string_75[] = "yscale %.";
static char spherica_string_76[] = "";
static char spherica_string_77[] = "";
static char spherica_string_78[] = "xOff Pixel";
static char spherica_string_79[] = "";
static char spherica_string_80[] = "";
static char spherica_string_81[] = "yOff Pixel";
static char spherica_string_82[] = "";
static char spherica_string_83[] = "";
static char spherica_string_84[] = "000400";
static char spherica_string_85[] = "______";
static char spherica_string_86[] = "9";
static char spherica_string_87[] = "000400";
static char spherica_string_88[] = "______";
static char spherica_string_89[] = "9";
static char spherica_string_90[] = "00000";
static char spherica_string_91[] = "_____";
static char spherica_string_92[] = "99999";
static char spherica_string_93[] = "00000";
static char spherica_string_94[] = "_____";
static char spherica_string_95[] = "99999";
static char spherica_string_96[] = "Soft (1-3)";
static char spherica_string_97[] = "";
static char spherica_string_98[] = "";
static char spherica_string_99[] = "1";
static char spherica_string_100[] = "_";
static char spherica_string_101[] = "9";
static char spherica_string_102[] = "Persp.";
static char spherica_string_103[] = "";
static char spherica_string_104[] = "";
static char spherica_string_105[] = " Bumpmapping";
static char spherica_string_106[] = "";
static char spherica_string_107[] = "";
static char spherica_string_108[] = "0030";
static char spherica_string_109[] = "X: ____";
static char spherica_string_110[] = "9999";
static char spherica_string_111[] = "0030";
static char spherica_string_112[] = "Y: ____";
static char spherica_string_113[] = "9999";
static char spherica_string_114[] = "0030";
static char spherica_string_115[] = "Z: ____";
static char spherica_string_116[] = "9999";
static char spherica_string_117[] = " Glanzpunkt ";
static char spherica_string_118[] = "";
static char spherica_string_119[] = "";
static char spherica_string_120[] = "Los!";
static char spherica_string_121[] = " Ambient";
static char spherica_string_122[] = "";
static char spherica_string_123[] = "";
static char spherica_string_124[] = "laden";
static char spherica_string_125[] = "sichern";
static char spherica_string_126[] = "An";
static char spherica_string_127[] = "";
static char spherica_string_128[] = "";
static char spherica_string_129[] = "Bumpmapping";
static char spherica_string_130[] = "";
static char spherica_string_131[] = "";
static char spherica_string_132[] = "Textur";
static char spherica_string_133[] = "";
static char spherica_string_134[] = "";
static char spherica_string_135[] = "Roughness";
static char spherica_string_136[] = "";
static char spherica_string_137[] = "";
static char spherica_string_138[] = "Transparenz";
static char spherica_string_139[] = "";
static char spherica_string_140[] = "";
static char spherica_string_141[] = "Parameter";
static char spherica_string_142[] = "";
static char spherica_string_143[] = "";
static char spherica_string_144[] = "r";
static char spherica_string_145[] = "";
static char spherica_string_146[] = "";
static char spherica_string_147[] = "_____";
static char spherica_string_148[] = "";
static char spherica_string_149[] = "";
static char spherica_string_150[] = "g";
static char spherica_string_151[] = "";
static char spherica_string_152[] = "";
static char spherica_string_153[] = "_____";
static char spherica_string_154[] = "";
static char spherica_string_155[] = "";
static char spherica_string_156[] = "b";
static char spherica_string_157[] = "";
static char spherica_string_158[] = "";
static char spherica_string_159[] = "_____";
static char spherica_string_160[] = "";
static char spherica_string_161[] = "";
static char spherica_string_162[] = "Str";
static char spherica_string_163[] = "";
static char spherica_string_164[] = "";
static char spherica_string_165[] = "_____";
static char spherica_string_166[] = "";
static char spherica_string_167[] = "";
static char spherica_string_168[] = "Pow";
static char spherica_string_169[] = "";
static char spherica_string_170[] = "";
static char spherica_string_171[] = "_____";
static char spherica_string_172[] = "";
static char spherica_string_173[] = "";
static char spherica_string_174[] = "Filt";
static char spherica_string_175[] = "";
static char spherica_string_176[] = "";
static char spherica_string_177[] = "_____";
static char spherica_string_178[] = "";
static char spherica_string_179[] = "";
static char spherica_string_180[] = " Kugel";
static char spherica_string_181[] = "";
static char spherica_string_182[] = "";
static char spherica_string_183[] = "Radius";
static char spherica_string_184[] = "";
static char spherica_string_185[] = "";
static char spherica_string_186[] = "Preview";
static char spherica_string_187[] = "";
static char spherica_string_188[] = "";


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ spherica_string_0, spherica_string_1, spherica_string_2, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 }, /* _TIT */
	{ spherica_string_3, spherica_string_4, spherica_string_5, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* KR_A */
	{ spherica_string_6, spherica_string_7, spherica_string_8, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ spherica_string_9, spherica_string_10, spherica_string_11, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* KG_A */
	{ spherica_string_12, spherica_string_13, spherica_string_14, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ spherica_string_15, spherica_string_16, spherica_string_17, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* KB_A */
	{ spherica_string_18, spherica_string_19, spherica_string_20, SMALL, 6, TE_LEFT, 0x1100, 0x0, 1, 8,1 },
	{ spherica_string_21, spherica_string_22, spherica_string_23, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* TEXT_A */
	{ spherica_string_24, spherica_string_25, spherica_string_26, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 10,1 },
	{ spherica_string_27, spherica_string_28, spherica_string_29, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 10,1 },
	{ spherica_string_30, spherica_string_31, spherica_string_32, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 11,1 },
	{ spherica_string_33, spherica_string_34, spherica_string_35, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 11,1 },
	{ spherica_string_36, spherica_string_37, spherica_string_38, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 7,7 }, /* TEX_XSC */
	{ spherica_string_39, spherica_string_40, spherica_string_41, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 7,7 }, /* TEX_YSC */
	{ spherica_string_42, spherica_string_43, spherica_string_44, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 6,6 }, /* TEX_XOFF */
	{ spherica_string_45, spherica_string_46, spherica_string_47, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 6,6 }, /* TEX_YOFF */
	{ spherica_string_48, spherica_string_49, spherica_string_50, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 7,1 },
	{ spherica_string_51, spherica_string_52, spherica_string_53, SMALL, 6, TE_LEFT, 0x1100, 0x0, 1, 6,1 },
	{ spherica_string_54, spherica_string_55, spherica_string_56, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* ROUGH_A */
	{ spherica_string_57, spherica_string_58, spherica_string_59, SMALL, 6, TE_LEFT, 0x1100, 0x0, 1, 13,1 },
	{ spherica_string_60, spherica_string_61, spherica_string_62, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* TRANSP_A */
	{ spherica_string_63, spherica_string_64, spherica_string_65, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 5,6 }, /* TRAN_IDX */
	{ spherica_string_66, spherica_string_67, spherica_string_68, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 4,1 },
	{ spherica_string_69, spherica_string_70, spherica_string_71, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* BUMP_A */
	{ spherica_string_72, spherica_string_73, spherica_string_74, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 10,1 },
	{ spherica_string_75, spherica_string_76, spherica_string_77, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 10,1 },
	{ spherica_string_78, spherica_string_79, spherica_string_80, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 11,1 },
	{ spherica_string_81, spherica_string_82, spherica_string_83, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 11,1 },
	{ spherica_string_84, spherica_string_85, spherica_string_86, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 7,7 }, /* BUMP_XSC */
	{ spherica_string_87, spherica_string_88, spherica_string_89, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 7,7 }, /* BUMP_YSC */
	{ spherica_string_90, spherica_string_91, spherica_string_92, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 6,6 }, /* BUMP_XOFF */
	{ spherica_string_93, spherica_string_94, spherica_string_95, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 6,6 }, /* BUMP_YOFF */
	{ spherica_string_96, spherica_string_97, spherica_string_98, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 11,1 },
	{ spherica_string_99, spherica_string_100, spherica_string_101, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 2,2 },
	{ spherica_string_102, spherica_string_103, spherica_string_104, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 7,1 },
	{ spherica_string_105, spherica_string_106, spherica_string_107, SMALL, 6, TE_LEFT, 0x1100, 0x0, 1, 13,1 },
	{ spherica_string_108, spherica_string_109, spherica_string_110, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 5,8 }, /* XRAD */
	{ spherica_string_111, spherica_string_112, spherica_string_113, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 5,8 },
	{ spherica_string_114, spherica_string_115, spherica_string_116, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 5,8 },
	{ spherica_string_117, spherica_string_118, spherica_string_119, SMALL, 6, TE_LEFT, 0x1180, 0x0, 1, 13,1 },
	{ spherica_string_121, spherica_string_122, spherica_string_123, SMALL, 6, TE_LEFT, 0x1100, 0x0, 1, 9,1 },
	{ spherica_string_126, spherica_string_127, spherica_string_128, SMALL, 6, TE_RIGHT, 0x1180, 0x0, -1, 3,1 },
	{ spherica_string_129, spherica_string_130, spherica_string_131, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ spherica_string_132, spherica_string_133, spherica_string_134, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 7,1 },
	{ spherica_string_135, spherica_string_136, spherica_string_137, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 10,1 },
	{ spherica_string_138, spherica_string_139, spherica_string_140, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ spherica_string_141, spherica_string_142, spherica_string_143, SMALL, 6, TE_RIGHT, 0x1180, 0x0, -1, 10,1 },
	{ spherica_string_144, spherica_string_145, spherica_string_146, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ spherica_string_147, spherica_string_148, spherica_string_149, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* AR_A */
	{ spherica_string_150, spherica_string_151, spherica_string_152, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ spherica_string_153, spherica_string_154, spherica_string_155, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* AG_A */
	{ spherica_string_156, spherica_string_157, spherica_string_158, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ spherica_string_159, spherica_string_160, spherica_string_161, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* AB_A */
	{ spherica_string_162, spherica_string_163, spherica_string_164, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 4,1 },
	{ spherica_string_165, spherica_string_166, spherica_string_167, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* SSTR_A */
	{ spherica_string_168, spherica_string_169, spherica_string_170, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 4,1 },
	{ spherica_string_171, spherica_string_172, spherica_string_173, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* SPOW_A */
	{ spherica_string_174, spherica_string_175, spherica_string_176, SMALL, 6, TE_CNTR, 0x1100, 0x0, -1, 5,1 },
	{ spherica_string_177, spherica_string_178, spherica_string_179, SMALL, 6, TE_CNTR, 0x1180, 0x0, -1, 6,1 }, /* SFLT_A */
	{ spherica_string_180, spherica_string_181, spherica_string_182, SMALL, 6, TE_LEFT, 0x1180, 0x0, 1, 7,1 },
	{ spherica_string_183, spherica_string_184, spherica_string_185, SMALL, 6, TE_LEFT, 0x1180, 0x0, 1, 7,1 },
	{ spherica_string_186, spherica_string_187, spherica_string_188, SMALL, 6, TE_CNTR, 0x1100, 0x0, 1, 8,1 }
};


static OBJECT rs_object[NUM_OBS] = {
/* SPH_MAIN */

	{ -1, 1, 110, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 68,2072 },
	{ 14, 2, 13, G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1026,1, 16,11 },
	{ 3, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 1026,2048, 1536,2048 }, /* _TIT */
	{ 5, 4, 4, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,1, 1,264 }, /* KR_F */
	{ 3, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* KR_R */
	{ 6, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1,2057, 1539,3328 }, /* KR_A */
	{ 7, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 1031,2048, 1536,2048 },
	{ 9, 8, 8, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1031,1, 1,264 }, /* KG_F */
	{ 7, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* KG_R */
	{ 10, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 6,2057, 1539,3328 }, /* KG_A */
	{ 11, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 1036,2048, 1536,2048 },
	{ 13, 12, 12, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1036,1, 1,264 }, /* KB_F */
	{ 11, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* KB_R */
	{ 1, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 11,2057, 1539,3328 }, /* KB_A */
	{ 29, 15, 28, G_BOX, OF_HIDETREE|OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1052,2048, 16,2059 }, /* TEXTURE_BOX */
	{ 16, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 0,0, 16,2048 },
	{ 17, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 1,10, 1539,3328 }, /* TEXT_A */
	{ 19, 18, 18, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,2049, 1,264 }, /* TEXT_F */
	{ 17, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* TEXT_R */
	{ 20, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 6,2049, 1542,2048 },
	{ 21, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 1030,3, 1542,2048 },
	{ 22, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 6,5, 1031,2048 },
	{ 23, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 6,2054, 1031,2048 },
	{ 24, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 8,2, 6,1 }, /* TEX_XSC */
	{ 25, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 8,2051, 6,1 }, /* TEX_YSC */
	{ 26, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 9,2053, 5,1 }, /* TEX_XOFF */
	{ 27, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 9,7, 5,1 }, /* TEX_YOFF */
	{ 28, -1, -1, G_TEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 9,2056, 6,1 },
	{ 14, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0xFF1100L), 1030,2056, 9,1 },
	{ 34, 30, 32, G_BOX, OF_HIDETREE|OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1052,2048, 16,2059 }, /* ROUGH_BOX */
	{ 31, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 0,0, 16,2048 },
	{ 32, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 1,10, 1539,3328 }, /* ROUGH_A */
	{ 29, 33, 33, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,2049, 1,264 }, /* ROUGH_F */
	{ 32, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* ROUGH_R */
	{ 41, 35, 40, G_BOX, OF_HIDETREE|OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1052,2048, 16,2059 }, /* TRANSP_BOX */
	{ 36, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 0,0, 16,2048 },
	{ 37, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 1,10, 1539,3328 }, /* TRANSP_A */
	{ 39, 38, 38, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,2049, 1,264 }, /* TRANSP_F */
	{ 37, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* TRANSP_R */
	{ 40, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[21]), 1032,2, 5,1 }, /* TRAN_IDX */
	{ 34, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[22]), 7,2049, 514,2048 },
	{ 58, 42, 57, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x11100L), 1052,2048, 16,2059 }, /* BUMP_BOX */
	{ 43, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[23]), 1,10, 1539,3328 }, /* BUMP_A */
	{ 45, 44, 44, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,2049, 1,264 }, /* BUMP_F */
	{ 43, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* BUMP_R */
	{ 46, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[24]), 1029,2049, 1542,2048 },
	{ 47, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[25]), 1029,3, 1542,2048 },
	{ 48, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[26]), 1029,5, 1031,2048 },
	{ 49, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[27]), 1029,2054, 1031,2048 },
	{ 50, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[28]), 1031,2, 6,1 }, /* BUMP_XSC */
	{ 51, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[29]), 1031,2051, 6,1 }, /* BUMP_YSC */
	{ 52, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[30]), 1032,2053, 5,1 }, /* BUMP_XOFF */
	{ 53, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[31]), 1032,7, 5,1 }, /* BUMP_YOFF */
	{ 54, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[32]), 6,2056, 1031,2048 },
	{ 55, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[33]), 1036,9, 1,1 },
	{ 56, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[34]), 1033,10, 6,1 },
	{ 57, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE|OF_FL3DBAK, OS_SELECTED|OS_DISABLED, C_UNION(0xFF1100L), 7,10, 1032,1 }, /* BUMP_PERSP */
	{ 41, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[35]), 0,0, 16,2048 },
	{ 62, 59, 61, G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1042,1, 10,11 },
	{ 60, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[36]), 1025,1, 7,1 }, /* XRAD */
	{ 61, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[37]), 1025,2050, 7,1 },
	{ 58, -1, -1, G_FTEXT, OF_EDITABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[38]), 1025,4, 7,1 },
	{ 63, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[39]), 1026,2060, 16,2048 },
	{ 64, -1, -1, G_BUTTON, OF_SELECTABLE|OF_DEFAULT|OF_EXIT|OF_FL3DACT, OS_NORMAL, C_UNION(spherica_string_120), 57,2070, 7,1 }, /* LOS */
	{ 65, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[40]), 1042,2060, 16,2048 },
	{ 66, -1, -1, G_BOX, OF_FL3DACT, OS_SELECTED, C_UNION(0x11101L), 36,13, 12,6 }, /* PREV_BOX */
	{ 67, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_DISABLED, C_UNION(spherica_string_124), 1060,20, 1035,1 }, /* CONF_LOAD */
	{ 68, -1, -1, G_BUTTON, OF_SELECTABLE|OF_EXIT|OF_FL3DACT, OS_DISABLED, C_UNION(spherica_string_125), 1060,22, 1035,1 }, /* CONF_SAVE */
	{ 69, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[41]), 305,3328, 1025,2048 },
	{ 70, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[42]), 1078,2049, 11,1 },
	{ 71, -1, -1, (43<<8)+G_IBOX, OF_SELECTABLE|OF_RBUTTON|OF_FL3DBAK, OS_SELECTED, C_UNION(0xFF1100L), 52,2049, 14,1 }, /* BUMP_RADIO */
	{ 72, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[43]), 1078,3, 6,1 },
	{ 73, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[44]), 1078,2052, 9,1 },
	{ 74, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[45]), 1078,6, 11,1 },
	{ 75, -1, -1, (43<<8)+G_IBOX, OF_SELECTABLE|OF_RBUTTON|OF_FL3DBAK, OS_NORMAL, C_UNION(0xFF1100L), 52,3, 1032,1 }, /* TEXT_RADIO */
	{ 76, -1, -1, (43<<8)+G_IBOX, OF_SELECTABLE|OF_RBUTTON|OF_FL3DBAK, OS_NORMAL, C_UNION(0xFF1100L), 52,2052, 1035,1 }, /* ROUGH_RADIO */
	{ 77, -1, -1, (43<<8)+G_IBOX, OF_SELECTABLE|OF_RBUTTON|OF_FL3DBAK, OS_NORMAL, C_UNION(0xFF1100L), 52,6, 14,1 }, /* TRANSP_RADIO */
	{ 78, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE|OF_FL3DBAK, OS_SELECTED, C_UNION(0xFF1100L), 49,2049, 2,1 }, /* BUMP_CHECK */
	{ 79, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE|OF_FL3DBAK, OS_DISABLED, C_UNION(0xFF1100L), 49,3, 2,1 }, /* TEXT_CHECK */
	{ 80, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(0xFF1100L), 49,2052, 2,1 }, /* ROUGH_CHECK */
	{ 81, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE|OF_FL3DBAK, OS_NORMAL, C_UNION(0xFF1100L), 49,6, 2,1 }, /* TRANS_CHECK */
	{ 82, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[46]), 52,3328, 1542,2048 },
	{ 95, 83, 94, G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1042,13, 16,11 },
	{ 84, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[47]), 1026,2048, 1536,2048 },
	{ 86, 85, 85, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,1, 1,264 }, /* AR_F */
	{ 84, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* AR_R */
	{ 87, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[48]), 1,2057, 1539,3328 }, /* AR_A */
	{ 88, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[49]), 1031,2048, 1536,1536 },
	{ 90, 89, 89, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1031,1, 1,264 }, /* AG_F */
	{ 88, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* AG_R */
	{ 91, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[50]), 6,2057, 1539,3328 }, /* AG_A */
	{ 92, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[51]), 1036,2048, 1536,2048 },
	{ 94, 93, 93, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1036,1, 1,264 }, /* AB_F */
	{ 92, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* AB_R */
	{ 82, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[52]), 11,2057, 1539,3328 }, /* AB_A */
	{ 108, 96, 107, G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0x11100L), 1026,13, 16,11 },
	{ 97, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[53]), 2,2048, 514,1536 },
	{ 99, 98, 98, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1026,1, 1,264 }, /* SSTR_F */
	{ 97, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* SSTR_R */
	{ 100, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[54]), 1,2057, 1539,3328 }, /* SSTR_A */
	{ 101, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[55]), 7,2048, 514,2048 },
	{ 103, 102, 102, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1031,1, 1,264 }, /* SPOW_F */
	{ 101, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* SPOW_R */
	{ 104, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[56]), 6,2057, 1539,3328 }, /* SPOW_A */
	{ 105, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[57]), 1035,2048, 3,2048 },
	{ 107, 106, 106, (21<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE8178L), 1036,1, 1,264 }, /* SFLT_F */
	{ 105, -1, -1, (21<<8)+G_BOX, OF_TOUCHEXIT|OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* SFLT_R */
	{ 95, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[58]), 11,2057, 1539,3328 }, /* SFLT_A */
	{ 109, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[59]), 1026,2048, 16,2048 },
	{ 110, -1, -1, G_BOXTEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[60]), 1042,2048, 10,2048 },
	{ 0, -1, -1, G_BOXTEXT, OF_LASTOB|OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[61]), 36,2060, 12,2048 }
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0] /* SPH_MAIN */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD spherica_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD spherica_rsc_load(wchar, hchar)
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
_WORD spherica_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD spherica_rsc_gaddr(type, idx, gaddr)
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
_WORD spherica_rsc_free(void)
#else
_WORD spherica_rsc_free()
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
_WORD rs_numstrings = 189;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 62;
_WORD rs_numobs = 111;
_WORD rs_numtree = 1;

char rs_name[] = "spherica.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
