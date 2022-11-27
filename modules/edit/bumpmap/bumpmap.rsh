/*
 * GEM resource C output of bumpmap
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

#include "bumpmap.h"

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
#define NUM_FRSTR	1
#define NUM_FRIMG	0
#define NUM_OBS     64
#define NUM_TREE	1
#define NUM_UD		0
#endif


static char bumpmap_string_0[] = "Bump!";
static char bumpmap_string_1[] = "123";
static char bumpmap_string_2[] = "";
static char bumpmap_string_3[] = "";
static char bumpmap_string_4[] = "123";
static char bumpmap_string_5[] = "";
static char bumpmap_string_6[] = "";
static char bumpmap_string_7[] = "123";
static char bumpmap_string_8[] = "";
static char bumpmap_string_9[] = "";
static char bumpmap_string_10[] = "123";
static char bumpmap_string_11[] = "";
static char bumpmap_string_12[] = "";
static char bumpmap_string_13[] = "123";
static char bumpmap_string_14[] = "";
static char bumpmap_string_15[] = "";
static char bumpmap_string_16[] = "B";
static char bumpmap_string_17[] = "";
static char bumpmap_string_18[] = "";
static char bumpmap_string_19[] = "G";
static char bumpmap_string_20[] = "";
static char bumpmap_string_21[] = "";
static char bumpmap_string_22[] = "H\204rte";
static char bumpmap_string_23[] = "";
static char bumpmap_string_24[] = "";
static char bumpmap_string_25[] = "Helligkeit";
static char bumpmap_string_26[] = "";
static char bumpmap_string_27[] = "";
static char bumpmap_string_28[] = "R";
static char bumpmap_string_29[] = "";
static char bumpmap_string_30[] = "";
static char bumpmap_string_31[] = "Diffuses Licht";
static char bumpmap_string_32[] = "";
static char bumpmap_string_33[] = "";
static char bumpmap_string_34[] = "Glanzpunkt";
static char bumpmap_string_35[] = "";
static char bumpmap_string_36[] = "";
static char bumpmap_string_37[] = "Plastik";
static char bumpmap_string_38[] = "";
static char bumpmap_string_39[] = "";
static char bumpmap_string_40[] = "Metall";
static char bumpmap_string_41[] = "";
static char bumpmap_string_42[] = "";
static char bumpmap_string_43[] = "Material";
static char bumpmap_string_44[] = "";
static char bumpmap_string_45[] = "";
static char bumpmap_string_46[] = "Material";
static char bumpmap_string_47[] = "";
static char bumpmap_string_48[] = "";
static char bumpmap_string_49[] = "Preview";
static char bumpmap_string_50[] = "";
static char bumpmap_string_51[] = "";
static char bumpmap_string_52[] = "Setting";
static char bumpmap_string_53[] = "";
static char bumpmap_string_54[] = "";
static char bumpmap_string_55[] = "speichern";
static char bumpmap_string_56[] = "12345678901234567890123";
static char bumpmap_string_57[] = "";
static char bumpmap_string_58[] = "";
static char bumpmap_string_59[] = "H\224henwert von:";
static char bumpmap_string_60[] = "";
static char bumpmap_string_61[] = "";
static char bumpmap_string_62[] = "Rot";
static char bumpmap_string_63[] = "";
static char bumpmap_string_64[] = "";
static char bumpmap_string_65[] = "Blau";
static char bumpmap_string_66[] = "";
static char bumpmap_string_67[] = "";
static char bumpmap_string_68[] = "Gr\201n";
static char bumpmap_string_69[] = "";
static char bumpmap_string_70[] = "";
static char bumpmap_string_71[] = "Grau";
static char bumpmap_string_72[] = "";
static char bumpmap_string_73[] = "";
static char bumpmap_string_74[] = "123";
static char bumpmap_string_75[] = "";
static char bumpmap_string_76[] = "";
static char bumpmap_string_77[] = "Bump-H\224he";
static char bumpmap_string_78[] = "";
static char bumpmap_string_79[] = "";
static char bumpmap_string_80[] = "Licht parallel";
static char bumpmap_string_81[] = "";
static char bumpmap_string_82[] = "";
static char bumpmap_string_83[] = "mit Textur";
static char bumpmap_string_84[] = "";
static char bumpmap_string_85[] = "";
static char bumpmap_string_86[] = "Hintergrund schwarz";
static char bumpmap_string_87[] = "";
static char bumpmap_string_88[] = "";
static char bumpmap_string_89[] = "Hintergrund wei\236";
static char bumpmap_string_90[] = "";
static char bumpmap_string_91[] = "";
static char bumpmap_string_92[] = "Hintergrund normal";
static char bumpmap_string_93[] = "";
static char bumpmap_string_94[] = "";
static char bumpmap_string_95[] = "umkehren";
static char bumpmap_string_96[] = "";
static char bumpmap_string_97[] = "";
static char bumpmap_string_98[] = "Laden";


static char *rs_frstr[NUM_FRSTR] = {
	bumpmap_string_98
};


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ bumpmap_string_1, bumpmap_string_2, bumpmap_string_3, SMALL, 0, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_BLUE_EDIT */
	{ bumpmap_string_4, bumpmap_string_5, bumpmap_string_6, SMALL, 0, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_GREEN_EDIT */
	{ bumpmap_string_7, bumpmap_string_8, bumpmap_string_9, SMALL, 0, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_RED_EDIT */
	{ bumpmap_string_10, bumpmap_string_11, bumpmap_string_12, SMALL, 0, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_BRIGHTNESS_EDIT */
	{ bumpmap_string_13, bumpmap_string_14, bumpmap_string_15, SMALL, 0, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* M_HARD_EDIT */
	{ bumpmap_string_16, bumpmap_string_17, bumpmap_string_18, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ bumpmap_string_19, bumpmap_string_20, bumpmap_string_21, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ bumpmap_string_22, bumpmap_string_23, bumpmap_string_24, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 6,1 },
	{ bumpmap_string_25, bumpmap_string_26, bumpmap_string_27, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 11,1 },
	{ bumpmap_string_28, bumpmap_string_29, bumpmap_string_30, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 2,1 },
	{ bumpmap_string_31, bumpmap_string_32, bumpmap_string_33, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 15,1 },
	{ bumpmap_string_34, bumpmap_string_35, bumpmap_string_36, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 11,1 },
	{ bumpmap_string_37, bumpmap_string_38, bumpmap_string_39, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 8,1 },
	{ bumpmap_string_40, bumpmap_string_41, bumpmap_string_42, SMALL, 0, TE_CNTR, 0x1100, 0x0, -1, 7,1 },
	{ bumpmap_string_43, bumpmap_string_44, bumpmap_string_45, IBM, 0, TE_CNTR, 0x1080, 0x0, 0, 9,1 },
	{ bumpmap_string_46, bumpmap_string_47, bumpmap_string_48, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 9,1 },
	{ bumpmap_string_49, bumpmap_string_50, bumpmap_string_51, SMALL, 0, TE_LEFT, 0x1180, 0x0, -1, 8,1 },
	{ bumpmap_string_52, bumpmap_string_53, bumpmap_string_54, SMALL, 0, TE_LEFT, 0x1180, 0x0, -1, 8,1 },
	{ bumpmap_string_56, bumpmap_string_57, bumpmap_string_58, IBM, 0, TE_CNTR, 0x11F0, 0x0, -2, 24,1 }, /* LOAD_SET */
	{ bumpmap_string_59, bumpmap_string_60, bumpmap_string_61, SMALL, 6, TE_LEFT, 0x1100, 0x0, 0, 15,1 },
	{ bumpmap_string_62, bumpmap_string_63, bumpmap_string_64, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 4,1 },
	{ bumpmap_string_65, bumpmap_string_66, bumpmap_string_67, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ bumpmap_string_68, bumpmap_string_69, bumpmap_string_70, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ bumpmap_string_71, bumpmap_string_72, bumpmap_string_73, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 5,1 },
	{ bumpmap_string_74, bumpmap_string_75, bumpmap_string_76, SMALL, 0, TE_CNTR, 0x1178, 0x0, -1, 4,1 }, /* BM_STRENGTH_EDIT */
	{ bumpmap_string_77, bumpmap_string_78, bumpmap_string_79, SMALL, 0, TE_LEFT, 0x1180, 0x0, -1, 10,1 },
	{ bumpmap_string_80, bumpmap_string_81, bumpmap_string_82, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 15,1 },
	{ bumpmap_string_83, bumpmap_string_84, bumpmap_string_85, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 11,1 },
	{ bumpmap_string_86, bumpmap_string_87, bumpmap_string_88, IBM, 0, TE_LEFT, 0x1180, 0x0, -1, 20,1 },
	{ bumpmap_string_89, bumpmap_string_90, bumpmap_string_91, IBM, 0, TE_LEFT, 0x1180, 0x0, -1, 17,1 },
	{ bumpmap_string_92, bumpmap_string_93, bumpmap_string_94, IBM, 0, TE_LEFT, 0x1180, 0x0, -1, 19,1 },
	{ bumpmap_string_95, bumpmap_string_96, bumpmap_string_97, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 9,1 }
};


static OBJECT rs_object[NUM_OBS] = {
/* BUMPMAIN */

	{ -1, 1, 47, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 65,2069 },
	{ 2, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(bumpmap_string_0), 1076,2067, 1034,1 }, /* DO_IT */
	{ 3, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x10100L), 2,1, 287,3600 },
	{ 30, 4, 29, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 1793,3840, 287,3600 },
	{ 5, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 520,2062, 1026,2048 }, /* M_BLUE_EDIT */
	{ 6, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1540,2062, 1026,2048 }, /* M_GREEN_EDIT */
	{ 7, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 513,2062, 1026,2048 }, /* M_RED_EDIT */
	{ 9, 8, 8, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 9,6, 1,264 }, /* M_BLUE_FHR */
	{ 7, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1174L), 0,2311, 1,2048 }, /* M_BLUE_SLIDE */
	{ 11, 10, 10, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 1029,6, 1,264 }, /* M_GREEN_FHR */
	{ 9, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1173L), 0,2311, 1,2048 }, /* M_GREEN_SLIDE */
	{ 13, 12, 12, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 2,6, 1,264 }, /* M_RED_FHR */
	{ 11, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1172L), 0,2311, 1,2048 }, /* M_RED_SLIDE */
	{ 14, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 526,1806, 1026,2048 }, /* M_BRIGHTNESS_EDIT */
	{ 16, 15, 15, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 15,6, 1,264 }, /* M_BRIGHTNESS_FHR */
	{ 14, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* M_BRIGHTNESS_SLIDE */
	{ 17, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 534,1806, 1026,2048 }, /* M_HARD_EDIT */
	{ 19, 18, 18, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9178L), 23,6, 1,264 }, /* M_HARD_FHR */
	{ 17, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2311, 1,2048 }, /* M_HARD_SLIDE */
	{ 20, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 1544,5, 1025,1 },
	{ 21, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 517,5, 1025,1 },
	{ 22, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 1301,773, 1539,2048 },
	{ 23, -1, -1, G_TEXT, OF_SELECTABLE, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 12,1029, 1031,1536 },
	{ 24, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 1537,5, 769,1 },
	{ 25, -1, -1, G_TEXT, OF_RBUTTON, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 5,1, 14,1 },
	{ 26, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF2100L), 1026,1, 17,1 }, /* M_RADIO_DIFFUSE */
	{ 27, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 5,2050, 10,1 },
	{ 28, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 1026,2050, 13,1 }, /* M_RADIO_GLANZ */
	{ 29, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 281,6, 517,2048 },
	{ 3, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 793,1805, 1028,2048 },
	{ 31, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 1027,2048, 1801,1 },
	{ 32, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 260,2048, 1032,1 },
	{ 33, -1, -1, (47<<8)+G_BOX, OF_FL3DACT, OS_SELECTED, C_UNION(0xFF1100L), 1315,3851, 12,6 },
	{ 34, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 1315,1291, 517,1536 },
	{ 35, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 258,2322, 517,1536 },
	{ 36, -1, -1, G_BUTTON, 0x605, OS_NORMAL, C_UNION(bumpmap_string_55), 35,2067, 13,1 }, /* SAVE_SET */
	{ 37, -1, -1, G_BOXTEXT, 0x605, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 2,2067, 31,1 }, /* LOAD_SET */
	{ 47, 38, 46, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 48,11, 17,7 },
	{ 39, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 1,0, 14,1 },
	{ 40, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 3,1, 3,1 },
	{ 41, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 1,1, 6,1 }, /* HW_RADIO_RED */
	{ 42, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[21]), 11,1, 4,1 },
	{ 43, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 9,1, 1030,1 }, /* HW_RADIO_BLUE */
	{ 44, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[22]), 3,3, 4,1 },
	{ 45, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 1,3, 1030,1 }, /* HW_RADIO_GREEN */
	{ 46, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[23]), 11,3, 4,1 },
	{ 37, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF2100L), 9,3, 1030,1 }, /* HW_RADIO_GREY */
	{ 0, 48, 63, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 34,1, 31,10 },
	{ 50, 49, 49, G_BOX, OF_FL3DIND, OS_SELECTED, C_UNION(0xFE9179L), 1026,2560, 1025,264 }, /* BM_STRENGTH_FHR */
	{ 48, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1178L), 0,2823, 1025,1536 }, /* BM_STRENGTH_SLIDE */
	{ 51, -1, -1, G_BOXTEXT, OF_FL3DIND, OS_NORMAL, C_UNION(&rs_tedinfo[24]), 2,521, 1026,2048 }, /* BM_STRENGTH_EDIT */
	{ 52, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[25]), 1,0, 1542,1536 },
	{ 53, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[26]), 1034,2048, 1039,1 },
	{ 54, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0xFF2100L), 8,2048, 17,1 }, /* BM_PARALEL */
	{ 55, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[27]), 1034,2, 15,1 },
	{ 56, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0xFF2100L), 8,2, 13,1 }, /* BM_TEXTURE */
	{ 57, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[28]), 1034,2051, 20,1 },
	{ 58, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[29]), 1034,5, 18,1 },
	{ 59, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 8,2051, 22,1 }, /* BLACK_BAK */
	{ 60, -1, -1, (43<<8)+G_IBOX, 0x11, OS_NORMAL, C_UNION(0xFF2100L), 8,5, 19,1 }, /* WHITE_BAK */
	{ 61, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[30]), 1034,2054, 1043,1 },
	{ 62, -1, -1, (43<<8)+G_IBOX, 0x11, OS_SELECTED, C_UNION(0xFF2100L), 8,2054, 21,1 }, /* NORM_BAK */
	{ 63, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[31]), 1034,8, 1033,1 },
	{ 47, -1, -1, (44<<8)+G_IBOX, 0x21, OS_NORMAL, C_UNION(0xFF2100L), 8,8, 11,1 } /* BM_INVERT */
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0] /* BUMPMAIN */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD bumpmap_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD bumpmap_rsc_load(wchar, hchar)
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
_WORD bumpmap_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD bumpmap_rsc_gaddr(type, idx, gaddr)
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
_WORD bumpmap_rsc_free(void)
#else
_WORD bumpmap_rsc_free()
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
_WORD rs_numfrstr = 1;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 0;
_WORD rs_numti = 32;
_WORD rs_numobs = 64;
_WORD rs_numtree = 1;

char rs_name[] = "bumpmap.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
