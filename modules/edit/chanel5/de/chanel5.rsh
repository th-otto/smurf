/*
 * GEM resource C output of chanel5
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

#include "chanel5.h"

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
#define NUM_STRINGS 24
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		7
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     20
#define NUM_TREE	1
#define NUM_UD		0
#endif


static char chanel5_string_0[] = "Preview";
static char chanel5_string_1[] = "";
static char chanel5_string_2[] = "";
static char chanel5_string_3[] = "Start";
static char chanel5_string_4[] = "Format";
static char chanel5_string_5[] = "";
static char chanel5_string_6[] = "";
static char chanel5_string_7[] = "->RBG";
static char chanel5_string_8[] = "";
static char chanel5_string_9[] = "";
static char chanel5_string_10[] = "->GRB";
static char chanel5_string_11[] = "";
static char chanel5_string_12[] = "";
static char chanel5_string_13[] = "->GBR";
static char chanel5_string_14[] = "";
static char chanel5_string_15[] = "";
static char chanel5_string_16[] = "->BRG";
static char chanel5_string_17[] = "";
static char chanel5_string_18[] = "";
static char chanel5_string_19[] = "->BGR";
static char chanel5_string_20[] = "";
static char chanel5_string_21[] = "";
static char chanel5_string_22[] = "laden";
static char chanel5_string_23[] = "speichern";


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ chanel5_string_0, chanel5_string_1, chanel5_string_2, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 8,1 },
	{ chanel5_string_4, chanel5_string_5, chanel5_string_6, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 7,1 },
	{ chanel5_string_7, chanel5_string_8, chanel5_string_9, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 },
	{ chanel5_string_10, chanel5_string_11, chanel5_string_12, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 },
	{ chanel5_string_13, chanel5_string_14, chanel5_string_15, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 },
	{ chanel5_string_16, chanel5_string_17, chanel5_string_18, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 },
	{ chanel5_string_19, chanel5_string_20, chanel5_string_21, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 6,1 }
};


static OBJECT rs_object[NUM_OBS] = {
/* CHANEL5 */

	{ -1, 1, 19, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 1059,14 },
	{ 2, -1, -1, (47<<8)+G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFF1178L), 1041,1, 16,8 },
	{ 3, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 17,2057, 7,1 },
	{ 4, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(chanel5_string_3), 24,12, 1033,1 }, /* STARTIT */
	{ 5, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 2,1, 6,1 },
	{ 6, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1170L), 2,1794, 256,1031 },
	{ 7, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1171L), 1793,1538, 256,1031 },
	{ 18, 8, 17, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 3,2, 9,8 }, /* FORMAT_BOX */
	{ 9, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 3,2048, 5,1 },
	{ 10, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF1100L), 1024,2048, 8,1 }, /* _RBG */
	{ 11, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 3,2, 5,1 },
	{ 12, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1024,2, 8,1 }, /* _GRB */
	{ 13, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 3,2051, 5,1 },
	{ 14, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1024,2051, 8,1 }, /* _GBR */
	{ 15, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 3,5, 5,1 },
	{ 16, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1024,5, 8,1 }, /* _BRG */
	{ 17, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 3,2054, 5,1 },
	{ 7, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF1100L), 1024,2054, 8,1 }, /* _BGR */
	{ 19, -1, -1, G_BUTTON, 0x605, OS_NORMAL, C_UNION(chanel5_string_22), 1025,12, 1033,1 }, /* MCONF_LOAD */
	{ 0, -1, -1, G_BUTTON, 0x625, OS_NORMAL, C_UNION(chanel5_string_23), 1036,12, 1033,1 } /* MCONF_SAVE */
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0] /* CHANEL5 */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD chanel5_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD chanel5_rsc_load(wchar, hchar)
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
_WORD chanel5_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD chanel5_rsc_gaddr(type, idx, gaddr)
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
_WORD chanel5_rsc_free(void)
#else
_WORD chanel5_rsc_free()
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
_WORD rs_numstrings = 24;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 7;
_WORD rs_numobs = 20;
_WORD rs_numtree = 1;

char rs_name[] = "chanel5.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
