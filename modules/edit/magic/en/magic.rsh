/*
 * GEM resource C output of magic
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

#include "magic.h"

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
#define NUM_STRINGS 70
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		23
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     45
#define NUM_TREE	1
#define NUM_UD		0
#endif


static char magic_string_0[] = "100";
static char magic_string_1[] = "maximum height: __._%";
static char magic_string_2[] = "999";
static char magic_string_3[] = "Type of calculation for";
static char magic_string_4[] = "";
static char magic_string_5[] = "";
static char magic_string_6[] = "objekttexture";
static char magic_string_7[] = "";
static char magic_string_8[] = "";
static char magic_string_9[] = "pixeltexture";
static char magic_string_10[] = "";
static char magic_string_11[] = "";
static char magic_string_12[] = "both";
static char magic_string_13[] = "";
static char magic_string_14[] = "";
static char magic_string_15[] = "<<<O";
static char magic_string_16[] = "";
static char magic_string_17[] = "";
static char magic_string_18[] = "<<O>";
static char magic_string_19[] = "";
static char magic_string_20[] = "";
static char magic_string_21[] = "<O>>";
static char magic_string_22[] = "";
static char magic_string_23[] = "";
static char magic_string_24[] = "O>>>";
static char magic_string_25[] = "";
static char magic_string_26[] = "";
static char magic_string_27[] = "<<<|";
static char magic_string_28[] = "";
static char magic_string_29[] = "";
static char magic_string_30[] = "<<|>";
static char magic_string_31[] = "";
static char magic_string_32[] = "";
static char magic_string_33[] = "<|>>";
static char magic_string_34[] = "";
static char magic_string_35[] = "";
static char magic_string_36[] = "|>>>";
static char magic_string_37[] = "";
static char magic_string_38[] = "";
static char magic_string_39[] = "<<<<";
static char magic_string_40[] = "";
static char magic_string_41[] = "";
static char magic_string_42[] = ">>>>";
static char magic_string_43[] = "";
static char magic_string_44[] = "";
static char magic_string_45[] = "go magic!";
static char magic_string_46[] = "mirroring";
static char magic_string_47[] = "";
static char magic_string_48[] = "";
static char magic_string_49[] = "increase height by";
static char magic_string_50[] = "";
static char magic_string_51[] = "";
static char magic_string_52[] = "tileing";
static char magic_string_53[] = "";
static char magic_string_54[] = "";
static char magic_string_55[] = "0100";
static char magic_string_56[] = "use width: ____";
static char magic_string_57[] = "9999";
static char magic_string_58[] = "columnwidth";
static char magic_string_59[] = "";
static char magic_string_60[] = "";
static char magic_string_61[] = "= texturewidth";
static char magic_string_62[] = "";
static char magic_string_63[] = "";
static char magic_string_64[] = "interpolate";
static char magic_string_65[] = "";
static char magic_string_66[] = "";
static char magic_string_67[] = "save memory";
static char magic_string_68[] = "";
static char magic_string_69[] = "";


TEDINFO rs_tedinfo[NUM_TI] = {
	{ magic_string_0, magic_string_1, magic_string_2, IBM, 6, TE_LEFT, 0x1180, 0x0, 0, 4,22 }, /* HOEHE_SW */
	{ magic_string_3, magic_string_4, magic_string_5, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 24,1 },
	{ magic_string_6, magic_string_7, magic_string_8, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 14,1 },
	{ magic_string_9, magic_string_10, magic_string_11, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 13,1 },
	{ magic_string_12, magic_string_13, magic_string_14, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_15, magic_string_16, magic_string_17, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_18, magic_string_19, magic_string_20, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_21, magic_string_22, magic_string_23, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_24, magic_string_25, magic_string_26, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_27, magic_string_28, magic_string_29, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_30, magic_string_31, magic_string_32, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_33, magic_string_34, magic_string_35, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_36, magic_string_37, magic_string_38, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_39, magic_string_40, magic_string_41, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_42, magic_string_43, magic_string_44, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 5,1 },
	{ magic_string_46, magic_string_47, magic_string_48, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 10,1 },
	{ magic_string_49, magic_string_50, magic_string_51, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 19,1 },
	{ magic_string_52, magic_string_53, magic_string_54, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 8,1 },
	{ magic_string_55, magic_string_56, magic_string_57, IBM, 6, TE_LEFT, 0x1180, 0x0, 0, 5,16 }, /* ANZSPALT */
	{ magic_string_58, magic_string_59, magic_string_60, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 12,1 },
	{ magic_string_61, magic_string_62, magic_string_63, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 15,1 },
	{ magic_string_64, magic_string_65, magic_string_66, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ magic_string_67, magic_string_68, magic_string_69, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 }
};


OBJECT rs_object[NUM_OBS] = {
/* SM_MAGIC */

	{ -1, 1, 44, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 49,2060 },
	{ 2, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 5,9, 21,1 }, /* HOEHE_SW */
	{ 27, 3, 26, G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1100L), 1,2048, 28,2055 },
	{ 4, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 2,2048, 23,1 },
	{ 5, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 1,2049, 13,1 },
	{ 6, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 15,2049, 12,1 },
	{ 7, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 12,3, 4,1 },
	{ 8, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 4,3, 6,1 },
	{ 9, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1,3, 8,1 }, /* RI10 */
	{ 10, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 4,4, 6,1 },
	{ 11, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1,4, 8,1 }, /* RI7 */
	{ 12, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 4,5, 6,1 },
	{ 13, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1,5, 8,1 }, /* RI8 */
	{ 14, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 4,6, 6,1 },
	{ 15, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1,6, 8,1 }, /* RI9 */
	{ 16, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 22,3, 6,1 },
	{ 17, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 19,3, 8,1 }, /* RI6 */
	{ 18, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 22,4, 6,1 },
	{ 19, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 19,4, 8,1 }, /* RI2 */
	{ 20, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 22,5, 6,1 },
	{ 21, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 19,5, 8,1 }, /* RI4 */
	{ 22, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 22,6, 6,1 },
	{ 23, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 19,6, 8,1 }, /* RI5 */
	{ 24, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 13,2052, 6,1 },
	{ 25, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 10,2052, 8,1 }, /* RI1 */
	{ 26, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 13,2053, 6,1 },
	{ 2, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF1100L), 10,2053, 8,1 }, /* RI3 */
	{ 28, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(magic_string_45), 37,10, 10,2049 }, /* GO */
	{ 29, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_SELECTED, C_UNION(0xFF1100L), 2,9, 17,1 }, /* SHORTEN */
	{ 35, 30, 34, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 30,5, 19,2051 },
	{ 31, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 4,2049, 10,1 },
	{ 32, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF1100L), 1,2049, 11,1 }, /* SPOSCH1 */
	{ 33, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 1,0, 18,1 },
	{ 34, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 4,2050, 9,1 },
	{ 29, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1,2050, 11,1 }, /* SPOSCH2 */
	{ 41, 36, 40, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 30,2048, 19,4 },
	{ 37, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 4,2050, 15,1 }, /* ANZSPALT */
	{ 38, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1,2050, 13,1 }, /* STRF2 */
	{ 39, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 1,0, 13,1 },
	{ 40, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 4,2049, 15,1 },
	{ 35, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF1100L), 1,2049, 17,1 }, /* STRF1 */
	{ 42, -1, -1, G_TEXT, OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[21]), 5,10, 11,1 },
	{ 43, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_DISABLED, C_UNION(0xFF1100L), 2,10, 14,1 }, /* INTERPOL */
	{ 44, -1, -1, G_TEXT, OF_FL3DBAK, OS_DISABLED, C_UNION(&rs_tedinfo[22]), 5,11, 11,1 },
	{ 0, -1, -1, (44<<8)+G_IBOX, 0x21, OS_DISABLED, C_UNION(0xFF1100L), 2,11, 14,1 } /* SPEICHER */
};


OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0] /* SM_MAGIC */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD magic_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD magic_rsc_load(wchar, hchar)
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
_WORD magic_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD magic_rsc_gaddr(type, idx, gaddr)
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
		*((_LONG **)gaddr) = &rs_object[idx].ob_spec.index;
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
_WORD magic_rsc_free(void)
#else
_WORD magic_rsc_free()
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
_WORD rs_numstrings = 70;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 23;
_WORD rs_numobs = 45;
_WORD rs_numtree = 1;

char rs_name[] = "magic.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
