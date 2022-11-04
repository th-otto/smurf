/*
 * GEM resource C output of bumpmapq
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

#include "bumpmapq.h"

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
#define NUM_STRINGS 64
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		21
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     45
#define NUM_TREE	1
#define NUM_UD		0
#endif


static char bumpmapq_string_0[] = "Bump!";
static char bumpmapq_string_1[] = "St\204rke";
static char bumpmapq_string_2[] = "";
static char bumpmapq_string_3[] = "";
static char bumpmapq_string_4[] = "123";
static char bumpmapq_string_5[] = "";
static char bumpmapq_string_6[] = "";
static char bumpmapq_string_7[] = "H\224henwert von:";
static char bumpmapq_string_8[] = "";
static char bumpmapq_string_9[] = "";
static char bumpmapq_string_10[] = "Rot";
static char bumpmapq_string_11[] = "";
static char bumpmapq_string_12[] = "";
static char bumpmapq_string_13[] = "Gr\201n";
static char bumpmapq_string_14[] = "";
static char bumpmapq_string_15[] = "";
static char bumpmapq_string_16[] = "Blau";
static char bumpmapq_string_17[] = "";
static char bumpmapq_string_18[] = "";
static char bumpmapq_string_19[] = "Grau";
static char bumpmapq_string_20[] = "";
static char bumpmapq_string_21[] = "";
static char bumpmapq_string_22[] = "Bump-Mapping";
static char bumpmapq_string_23[] = "";
static char bumpmapq_string_24[] = "";
static char bumpmapq_string_25[] = "R";
static char bumpmapq_string_26[] = "";
static char bumpmapq_string_27[] = "";
static char bumpmapq_string_28[] = "St\204rke";
static char bumpmapq_string_29[] = "";
static char bumpmapq_string_30[] = "";
static char bumpmapq_string_31[] = "H\204rte";
static char bumpmapq_string_32[] = "";
static char bumpmapq_string_33[] = "";
static char bumpmapq_string_34[] = "G";
static char bumpmapq_string_35[] = "";
static char bumpmapq_string_36[] = "";
static char bumpmapq_string_37[] = "B";
static char bumpmapq_string_38[] = "";
static char bumpmapq_string_39[] = "";
static char bumpmapq_string_40[] = "123";
static char bumpmapq_string_41[] = "";
static char bumpmapq_string_42[] = "";
static char bumpmapq_string_43[] = "123";
static char bumpmapq_string_44[] = "";
static char bumpmapq_string_45[] = "";
static char bumpmapq_string_46[] = "123";
static char bumpmapq_string_47[] = "";
static char bumpmapq_string_48[] = "";
static char bumpmapq_string_49[] = "123";
static char bumpmapq_string_50[] = "";
static char bumpmapq_string_51[] = "";
static char bumpmapq_string_52[] = "123";
static char bumpmapq_string_53[] = "";
static char bumpmapq_string_54[] = "";
static char bumpmapq_string_55[] = "Material";
static char bumpmapq_string_56[] = "";
static char bumpmapq_string_57[] = "";
static char bumpmapq_string_58[] = "Diffuses Licht";
static char bumpmapq_string_59[] = "";
static char bumpmapq_string_60[] = "";
static char bumpmapq_string_61[] = "Glanzpunkt";
static char bumpmapq_string_62[] = "";
static char bumpmapq_string_63[] = "";


TEDINFO rs_tedinfo[NUM_TI] = {
	{ bumpmapq_string_1, bumpmapq_string_2, bumpmapq_string_3, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 7,1 },
	{ bumpmapq_string_4, bumpmapq_string_5, bumpmapq_string_6, SMALL, 6, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* BM_STRENGH_EDIT */
	{ bumpmapq_string_7, bumpmapq_string_8, bumpmapq_string_9, IBM, 6, TE_LEFT, 0x1100, 0x0, 0, 15,1 },
	{ bumpmapq_string_10, bumpmapq_string_11, bumpmapq_string_12, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 4,1 },
	{ bumpmapq_string_13, bumpmapq_string_14, bumpmapq_string_15, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ bumpmapq_string_16, bumpmapq_string_17, bumpmapq_string_18, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ bumpmapq_string_19, bumpmapq_string_20, bumpmapq_string_21, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ bumpmapq_string_22, bumpmapq_string_23, bumpmapq_string_24, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 13,1 },
	{ bumpmapq_string_25, bumpmapq_string_26, bumpmapq_string_27, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ bumpmapq_string_28, bumpmapq_string_29, bumpmapq_string_30, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 7,1 },
	{ bumpmapq_string_31, bumpmapq_string_32, bumpmapq_string_33, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 6,1 },
	{ bumpmapq_string_34, bumpmapq_string_35, bumpmapq_string_36, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ bumpmapq_string_37, bumpmapq_string_38, bumpmapq_string_39, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ bumpmapq_string_40, bumpmapq_string_41, bumpmapq_string_42, SMALL, 6, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_HARD_EDIT */
	{ bumpmapq_string_43, bumpmapq_string_44, bumpmapq_string_45, SMALL, 6, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_STRENGH_EDIT */
	{ bumpmapq_string_46, bumpmapq_string_47, bumpmapq_string_48, SMALL, 6, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_RED_EDIT */
	{ bumpmapq_string_49, bumpmapq_string_50, bumpmapq_string_51, SMALL, 6, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_GREEN_EDIT */
	{ bumpmapq_string_52, bumpmapq_string_53, bumpmapq_string_54, SMALL, 6, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_BLUE_EDIT */
	{ bumpmapq_string_55, bumpmapq_string_56, bumpmapq_string_57, IBM, 6, TE_CNTR, 0x1180, 0x0, 0, 9,1 },
	{ bumpmapq_string_58, bumpmapq_string_59, bumpmapq_string_60, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 15,1 },
	{ bumpmapq_string_61, bumpmapq_string_62, bumpmapq_string_63, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 11,1 }
};


OBJECT rs_object[NUM_OBS] = {
/* BUMPMAIN */

	{ -1, 1, 44, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 63,18 },
	{ 2, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(bumpmapq_string_0), 52,16, 8,1 }, /* DO_IT */
	{ 3, -1, -1, G_IBOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x10100L), 289,257, 28,2061 },
	{ 17, 4, 16, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 33,1, 28,2061 },
	{ 5, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 2,1, 6,1 },
	{ 7, 6, 6, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 4,2050, 1025,264 }, /* BM_STRENGH_FHR */
	{ 5, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2823, 1025,1536 }, /* BM_STRENGH_SLIDE */
	{ 8, -1, -1, G_BOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1027,11, 1026,2048 }, /* BM_STRENGH_EDIT */
	{ 9, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 1035,3, 14,1 },
	{ 10, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 15,5, 3,1 },
	{ 11, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 15,2054, 4,1 },
	{ 12, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 15,8, 4,1 },
	{ 13, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 15,2057, 4,1 },
	{ 14, -1, -1, (18<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 1036,5, 1029,1 }, /* HW_RADIO_RED */
	{ 15, -1, -1, (18<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 1036,2054, 1030,1 }, /* HW_RADIO_GREEN */
	{ 16, -1, -1, (18<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 1036,8, 1030,1 }, /* HW_RADIO_BLUE */
	{ 3, -1, -1, (18<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF2100L), 1036,2057, 1030,1 }, /* HW_RADIO_GREY */
	{ 18, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 1058,2048, 1036,1 },
	{ 19, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x10100L), 2,1, 1052,16 },
	{ 20, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 1793,3840, 1052,16 },
	{ 21, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 1796,1026, 769,1 },
	{ 22, -1, -1, G_TEXT, OF_SELECTABLE, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 784,1026, 6,1 },
	{ 23, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 1815,1026, 5,1 },
	{ 24, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 776,1026, 1025,1 },
	{ 25, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 1803,1026, 1025,1 },
	{ 27, 26, 26, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 793,3075, 1025,264 }, /* M_HARD_FHR */
	{ 25, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2823, 1025,1536 }, /* M_HARD_SLIDE */
	{ 28, -1, -1, G_BOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 1816,1036, 1026,2048 }, /* M_HARD_EDIT */
	{ 30, 29, 29, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 786,3075, 1025,264 }, /* M_STRENGH_FHR */
	{ 28, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2823, 1025,1536 }, /* M_STRENGH_SLIDE */
	{ 31, -1, -1, G_BOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 1809,1036, 1026,2048 }, /* M_STRENGH_EDIT */
	{ 33, 32, 32, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 1796,3075, 1025,264 }, /* M_RED_FHR */
	{ 31, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1172L), 0,2823, 1025,1536 }, /* M_RED_SLIDE */
	{ 35, 34, 34, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 776,3075, 1025,264 }, /* M_GREEN_FHR */
	{ 33, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1173L), 0,2823, 1025,1536 }, /* M_GREEN_SLIDE */
	{ 37, 36, 36, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 1803,3075, 1025,264 }, /* M_BLUE_FHR */
	{ 35, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1174L), 0,2823, 1025,1536 }, /* M_BLUE_SLIDE */
	{ 38, -1, -1, G_BOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 772,1036, 1026,2048 }, /* M_RED_EDIT */
	{ 39, -1, -1, G_BOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 1799,1036, 1026,2048 }, /* M_GREEN_EDIT */
	{ 40, -1, -1, G_BOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 779,1036, 1026,2048 }, /* M_BLUE_EDIT */
	{ 41, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 771,2048, 1032,1 },
	{ 42, -1, -1, G_TEXT, OF_RBUTTON, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 7,2061, 14,1 },
	{ 43, -1, -1, (18<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF2100L), 1028,2061, 1040,1 }, /* M_RADIO_DIFFUSE */
	{ 44, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 7,15, 10,1 },
	{ 0, -1, -1, (18<<8)+G_IBOX, 0x31, OS_NORMAL, C_UNION(0xFF2100L), 1028,15, 1036,1 } /* M_RADIO_GLANZ */
};


OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0] /* BUMPMAIN */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD bumpmapq_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD bumpmapq_rsc_load(wchar, hchar)
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
_WORD bumpmapq_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD bumpmapq_rsc_gaddr(type, idx, gaddr)
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
_WORD bumpmapq_rsc_free(void)
#else
_WORD bumpmapq_rsc_free()
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
_WORD rs_numstrings = 64;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 21;
_WORD rs_numobs = 45;
_WORD rs_numtree = 1;

char rs_name[] = "bumpmapq.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
