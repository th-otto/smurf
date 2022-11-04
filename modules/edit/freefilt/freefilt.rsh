/*
 * GEM resource C output of freefilt
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

#include "freefilt.h"

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
#define NUM_STRINGS 99
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		32
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     54
#define NUM_TREE	1
#define NUM_UD		0
#endif


static char freefilt_string_0[] = "Noname\0\0\0\0\0\0\0\0\0\0\0\0\0";
static char freefilt_string_1[] = "___________________";
static char freefilt_string_2[] = "XXXXXXXXXXXXXXXXXXX";
static char freefilt_string_3[] = "+00";
static char freefilt_string_4[] = "___";
static char freefilt_string_5[] = "X9";
static char freefilt_string_6[] = "+00";
static char freefilt_string_7[] = "___";
static char freefilt_string_8[] = "X9";
static char freefilt_string_9[] = "+00";
static char freefilt_string_10[] = "___";
static char freefilt_string_11[] = "X9";
static char freefilt_string_12[] = "+00";
static char freefilt_string_13[] = "___";
static char freefilt_string_14[] = "X9";
static char freefilt_string_15[] = "+00";
static char freefilt_string_16[] = "___";
static char freefilt_string_17[] = "X9";
static char freefilt_string_18[] = "+00";
static char freefilt_string_19[] = "___";
static char freefilt_string_20[] = "X9";
static char freefilt_string_21[] = "+00";
static char freefilt_string_22[] = "___";
static char freefilt_string_23[] = "X9";
static char freefilt_string_24[] = "+00";
static char freefilt_string_25[] = "___";
static char freefilt_string_26[] = "X9";
static char freefilt_string_27[] = "+00";
static char freefilt_string_28[] = "___";
static char freefilt_string_29[] = "X9";
static char freefilt_string_30[] = "+00";
static char freefilt_string_31[] = "___";
static char freefilt_string_32[] = "X9";
static char freefilt_string_33[] = "+00";
static char freefilt_string_34[] = "___";
static char freefilt_string_35[] = "X9";
static char freefilt_string_36[] = "+00";
static char freefilt_string_37[] = "___";
static char freefilt_string_38[] = "X99";
static char freefilt_string_39[] = "+01";
static char freefilt_string_40[] = "___";
static char freefilt_string_41[] = "X9";
static char freefilt_string_42[] = "+00";
static char freefilt_string_43[] = "___";
static char freefilt_string_44[] = "X9";
static char freefilt_string_45[] = "+00";
static char freefilt_string_46[] = "___";
static char freefilt_string_47[] = "X9";
static char freefilt_string_48[] = "+00";
static char freefilt_string_49[] = "___";
static char freefilt_string_50[] = "X9";
static char freefilt_string_51[] = "+00";
static char freefilt_string_52[] = "___";
static char freefilt_string_53[] = "X99";
static char freefilt_string_54[] = "+00";
static char freefilt_string_55[] = "___";
static char freefilt_string_56[] = "X9";
static char freefilt_string_57[] = "+00";
static char freefilt_string_58[] = "___";
static char freefilt_string_59[] = "X9";
static char freefilt_string_60[] = "+00";
static char freefilt_string_61[] = "___";
static char freefilt_string_62[] = "X9";
static char freefilt_string_63[] = "+00";
static char freefilt_string_64[] = "___";
static char freefilt_string_65[] = "X99";
static char freefilt_string_66[] = "+00";
static char freefilt_string_67[] = "___";
static char freefilt_string_68[] = "X9";
static char freefilt_string_69[] = "+00";
static char freefilt_string_70[] = "___";
static char freefilt_string_71[] = "X9";
static char freefilt_string_72[] = "+00";
static char freefilt_string_73[] = "___";
static char freefilt_string_74[] = "X9";
static char freefilt_string_75[] = "+00";
static char freefilt_string_76[] = "___";
static char freefilt_string_77[] = "X9";
static char freefilt_string_78[] = "OK";
static char freefilt_string_79[] = "+1\0\0";
static char freefilt_string_80[] = "Div.: ____";
static char freefilt_string_81[] = "X999";
static char freefilt_string_82[] = "+0\0\0";
static char freefilt_string_83[] = "Bias: ____";
static char freefilt_string_84[] = "X999";
static char freefilt_string_85[] = "load WF5";
static char freefilt_string_86[] = "save WF5";
static char freefilt_string_87[] = "Clip";
static char freefilt_string_88[] = "";
static char freefilt_string_89[] = "";
static char freefilt_string_90[] = "Invert";
static char freefilt_string_91[] = "";
static char freefilt_string_92[] = "";
static char freefilt_string_93[] = "St\204rke";
static char freefilt_string_94[] = "";
static char freefilt_string_95[] = "";
static char freefilt_string_96[] = "123";
static char freefilt_string_97[] = "";
static char freefilt_string_98[] = "";


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ freefilt_string_0, freefilt_string_1, freefilt_string_2, IBM, 6, TE_CNTR, 0x1180, 0x0, 0, 20,20 }, /* FILTER_NAME */
	{ freefilt_string_3, freefilt_string_4, freefilt_string_5, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_00 */
	{ freefilt_string_6, freefilt_string_7, freefilt_string_8, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_01 */
	{ freefilt_string_9, freefilt_string_10, freefilt_string_11, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_02 */
	{ freefilt_string_12, freefilt_string_13, freefilt_string_14, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_03 */
	{ freefilt_string_15, freefilt_string_16, freefilt_string_17, IBM, 6, TE_LEFT, 0x1180, 0x0, 0, 4,4 }, /* MATRIX_04 */
	{ freefilt_string_18, freefilt_string_19, freefilt_string_20, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_10 */
	{ freefilt_string_21, freefilt_string_22, freefilt_string_23, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_11 */
	{ freefilt_string_24, freefilt_string_25, freefilt_string_26, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_12 */
	{ freefilt_string_27, freefilt_string_28, freefilt_string_29, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_13 */
	{ freefilt_string_30, freefilt_string_31, freefilt_string_32, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_14 */
	{ freefilt_string_33, freefilt_string_34, freefilt_string_35, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_20 */
	{ freefilt_string_36, freefilt_string_37, freefilt_string_38, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_21 */
	{ freefilt_string_39, freefilt_string_40, freefilt_string_41, IBM, 6, TE_LEFT, 0x1A80, 0x0, -1, 4,4 }, /* MATRIX_22 */
	{ freefilt_string_42, freefilt_string_43, freefilt_string_44, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_23 */
	{ freefilt_string_45, freefilt_string_46, freefilt_string_47, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_24 */
	{ freefilt_string_48, freefilt_string_49, freefilt_string_50, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_30 */
	{ freefilt_string_51, freefilt_string_52, freefilt_string_53, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_31 */
	{ freefilt_string_54, freefilt_string_55, freefilt_string_56, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_32 */
	{ freefilt_string_57, freefilt_string_58, freefilt_string_59, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_33 */
	{ freefilt_string_60, freefilt_string_61, freefilt_string_62, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_34 */
	{ freefilt_string_63, freefilt_string_64, freefilt_string_65, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_40 */
	{ freefilt_string_66, freefilt_string_67, freefilt_string_68, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_41 */
	{ freefilt_string_69, freefilt_string_70, freefilt_string_71, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_42 */
	{ freefilt_string_72, freefilt_string_73, freefilt_string_74, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_43 */
	{ freefilt_string_75, freefilt_string_76, freefilt_string_77, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 4,4 }, /* MATRIX_44 */
	{ freefilt_string_79, freefilt_string_80, freefilt_string_81, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,11 }, /* DIV */
	{ freefilt_string_82, freefilt_string_83, freefilt_string_84, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 5,11 }, /* BIAS */
	{ freefilt_string_87, freefilt_string_88, freefilt_string_89, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ freefilt_string_90, freefilt_string_91, freefilt_string_92, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 7,1 },
	{ freefilt_string_93, freefilt_string_94, freefilt_string_95, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 7,1 },
	{ freefilt_string_96, freefilt_string_97, freefilt_string_98, IBM, 6, TE_CNTR, 0x100, 0x0, 0, 4,1 } /* STRENGTH_EDIT */
};


static OBJECT rs_object[NUM_OBS] = {
/* FREE_FILTER */

	{ -1, 1, 52, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 36,2065 },
	{ 2, -1, -1, G_FTEXT, 0x608, OS_SELECTED, C_UNION(&rs_tedinfo[0]), 1538,257, 1811,1 }, /* FILTER_NAME */
	{ 38, 3, 3, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 2,2, 21,2056 },
	{ 2, 4, 32, G_BOX, OF_FL3DACT, OS_SELECTED, C_UNION(0x1100L), 1536,1536, 1811,3335 },
	{ 5, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x10100L), 512,512, 1043,2567 },
	{ 6, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 256,2561, 1043,2305 },
	{ 7, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 256,515, 1043,2305 },
	{ 8, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 256,2564, 1043,2305 },
	{ 14, 9, 13, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 256,256, 4,2567 },
	{ 10, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1024,1536, 3,1 }, /* MATRIX_00 */
	{ 11, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 1024,3585, 3,1 }, /* MATRIX_01 */
	{ 12, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 1024,1539, 3,1 }, /* MATRIX_02 */
	{ 13, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 1024,3588, 3,1 }, /* MATRIX_03 */
	{ 8, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 1024,1542, 3,1 }, /* MATRIX_04 */
	{ 20, 15, 19, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 4,256, 4,2567 },
	{ 16, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 1024,1536, 3,1 }, /* MATRIX_10 */
	{ 17, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 1024,3585, 3,1 }, /* MATRIX_11 */
	{ 18, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 1024,1539, 3,1 }, /* MATRIX_12 */
	{ 19, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 1024,3588, 3,1 }, /* MATRIX_13 */
	{ 14, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 1024,1542, 3,1 }, /* MATRIX_14 */
	{ 26, 21, 25, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 1799,256, 4,2567 },
	{ 22, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 1024,1536, 3,1 }, /* MATRIX_20 */
	{ 23, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 1024,3585, 3,1 }, /* MATRIX_21 */
	{ 24, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 1024,1539, 3,1 }, /* MATRIX_22 */
	{ 25, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 1024,3588, 3,1 }, /* MATRIX_23 */
	{ 20, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 1024,1542, 3,1 }, /* MATRIX_24 */
	{ 32, 27, 31, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 1547,256, 4,2567 },
	{ 28, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 1024,1536, 3,1 }, /* MATRIX_30 */
	{ 29, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 1024,3585, 3,1 }, /* MATRIX_31 */
	{ 30, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 1024,1539, 3,1 }, /* MATRIX_32 */
	{ 31, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 1024,3588, 3,1 }, /* MATRIX_33 */
	{ 26, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 1024,1542, 3,1 }, /* MATRIX_34 */
	{ 3, 33, 37, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 1295,256, 4,2567 },
	{ 34, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[21]), 1024,1536, 3,1 }, /* MATRIX_40 */
	{ 35, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[22]), 1024,3585, 3,1 }, /* MATRIX_41 */
	{ 36, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[23]), 1024,1539, 3,1 }, /* MATRIX_42 */
	{ 37, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[24]), 1024,3588, 3,1 }, /* MATRIX_43 */
	{ 32, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[25]), 1024,1542, 3,1 }, /* MATRIX_44 */
	{ 39, -1, -1, G_BUTTON, 0x605, OS_NORMAL, C_UNION(freefilt_string_78), 26,2063, 8,1 }, /* DO_IT */
	{ 40, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[26]), 1026,11, 10,1 }, /* DIV */
	{ 41, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[27]), 1026,2060, 10,1 }, /* BIAS */
	{ 42, -1, -1, G_BUTTON, 0x601, OS_NORMAL, C_UNION(freefilt_string_85), 3,2063, 8,1 }, /* LOAD_FILTER */
	{ 43, -1, -1, G_BUTTON, 0x601, OS_NORMAL, C_UNION(freefilt_string_86), 14,2063, 8,1 }, /* SAVE_FILTER */
	{ 44, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[28]), 1042,11, 4,1 },
	{ 45, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[29]), 1042,2060, 6,1 },
	{ 46, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_SELECTED, C_UNION(0xFF2100L), 1039,11, 7,1 }, /* CLIP */
	{ 47, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0xFF2100L), 1039,2060, 1033,1 }, /* INVERT */
	{ 48, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[30]), 1819,257, 6,1 },
	{ 50, 49, 49, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 1821,2306, 1025,264 }, /* STRENGTH_FHR */
	{ 48, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2823, 1025,1536 }, /* STRENGTH_SLIDE */
	{ 51, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x9179L), 514,1038, 1823,256 },
	{ 52, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x9170L), 770,1294, 1823,256 },
	{ 0, 53, 53, G_BOX, OF_FL3DACT, OS_SELECTED, C_UNION(0x1100L), 1564,779, 1027,1 },
	{ 52, -1, -1, G_TEXT, 0x420, OS_NORMAL, C_UNION(&rs_tedinfo[31]), 0,0, 1027,1 } /* STRENGTH_EDIT */
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0] /* FREE_FILTER */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD freefilt_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD freefilt_rsc_load(wchar, hchar)
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
_WORD freefilt_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD freefilt_rsc_gaddr(type, idx, gaddr)
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
_WORD freefilt_rsc_free(void)
#else
_WORD freefilt_rsc_free()
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
_WORD rs_numstrings = 99;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 32;
_WORD rs_numobs = 54;
_WORD rs_numtree = 1;

char rs_name[] = "freefilt.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
