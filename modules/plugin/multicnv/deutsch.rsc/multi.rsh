/*
 * GEM resource C output of multi
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

#include "multi.h"

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
#define NUM_STRINGS 147
#define NUM_BB		0
#define NUM_IB		0
#define NUM_CIB     1
#define NUM_CIC     1
#define NUM_TI		47
#define NUM_FRSTR	0
#define NUM_FRIMG	0
#define NUM_OBS     77
#define NUM_TREE	2
#define NUM_UD		0
#endif


static char multi_string_0[] = "Start";
static char multi_string_1[] = "sichern als";
static char multi_string_2[] = "";
static char multi_string_3[] = "";
static char multi_string_4[] = "sichern als";
static char multi_string_5[] = "";
static char multi_string_6[] = "";
static char multi_string_7[] = "12345678901234567890123456";
static char multi_string_8[] = "";
static char multi_string_9[] = "";
static char multi_string_10[] = "2";
static char multi_string_11[] = "";
static char multi_string_12[] = "";
static char multi_string_13[] = "3";
static char multi_string_14[] = "";
static char multi_string_15[] = "";
static char multi_string_16[] = "4";
static char multi_string_17[] = "";
static char multi_string_18[] = "";
static char multi_string_19[] = "5";
static char multi_string_20[] = "";
static char multi_string_21[] = "";
static char multi_string_22[] = "6";
static char multi_string_23[] = "";
static char multi_string_24[] = "";
static char multi_string_25[] = "7";
static char multi_string_26[] = "";
static char multi_string_27[] = "";
static char multi_string_28[] = "8";
static char multi_string_29[] = "";
static char multi_string_30[] = "";
static char multi_string_31[] = "9";
static char multi_string_32[] = "";
static char multi_string_33[] = "";
static char multi_string_34[] = "Optionen...";
static char multi_string_35[] = "123456789012345678901234567890123456789012345";
static char multi_string_36[] = "";
static char multi_string_37[] = "";
static char multi_string_38[] = "Quellpfad";
static char multi_string_39[] = "";
static char multi_string_40[] = "";
static char multi_string_41[] = "Quellpfad";
static char multi_string_42[] = "";
static char multi_string_43[] = "";
static char multi_string_44[] = "123456789012345678901234567890123456789012345";
static char multi_string_45[] = "";
static char multi_string_46[] = "";
static char multi_string_47[] = "Zielpfad";
static char multi_string_48[] = "";
static char multi_string_49[] = "";
static char multi_string_50[] = "Zielpfad";
static char multi_string_51[] = "";
static char multi_string_52[] = "";
static char multi_string_53[] = "Wildcards";
static char multi_string_54[] = "";
static char multi_string_55[] = "";
static char multi_string_56[] = "Wildcards";
static char multi_string_57[] = "";
static char multi_string_58[] = "";
static char multi_string_59[] = "*_______________________________________________";
static char multi_string_60[] = "________________________________________________";
static char multi_string_61[] = "X";
static char multi_string_62[] = "";
static char multi_string_63[] = "24 Bit";
static char multi_string_64[] = "";
static char multi_string_65[] = "";
static char multi_string_66[] = "in Farbtiefe";
static char multi_string_67[] = "";
static char multi_string_68[] = "";
static char multi_string_69[] = "in Farbtiefe";
static char multi_string_70[] = "";
static char multi_string_71[] = "";
static char multi_string_72[] = "CMYK";
static char multi_string_73[] = "";
static char multi_string_74[] = "";
static char multi_string_75[] = "Farbsystem";
static char multi_string_76[] = "";
static char multi_string_77[] = "";
static char multi_string_78[] = "Farbsystem";
static char multi_string_79[] = "";
static char multi_string_80[] = "";
static char multi_string_81[] = "Floyd Steinberg";
static char multi_string_82[] = "";
static char multi_string_83[] = "";
static char multi_string_84[] = "ggfs. Rastern mit";
static char multi_string_85[] = "";
static char multi_string_86[] = "";
static char multi_string_87[] = "ggfs. Rastern mit";
static char multi_string_88[] = "";
static char multi_string_89[] = "";
static char multi_string_90[] = "feste Palette";
static char multi_string_91[] = "";
static char multi_string_92[] = "";
static char multi_string_93[] = "w.m. gleich";
static char multi_string_94[] = "";
static char multi_string_95[] = "";
static char multi_string_96[] = "w.m. gleich";
static char multi_string_97[] = "";
static char multi_string_98[] = "";
static char multi_string_99[] = "Palette laden";
static char multi_string_100[] = "Name+Farbtiefe";
static char multi_string_101[] = "";
static char multi_string_102[] = "";
static char multi_string_103[] = "Unterordner";
static char multi_string_104[] = "";
static char multi_string_105[] = "";
static char multi_string_106[] = "laden";
static char multi_string_107[] = "sichern";
static char multi_string_108[] = "GEMScript \003";
static char multi_string_109[] = "";
static char multi_string_110[] = "";
static char multi_string_111[] = "0.9";
static char multi_string_112[] = "";
static char multi_string_113[] = "";
static char multi_string_114[] = "0.9";
static char multi_string_115[] = "";
static char multi_string_116[] = "";
static char multi_string_117[] = "xxx von xxx Bildern konvertiert";
static char multi_string_118[] = "";
static char multi_string_119[] = "";
static char multi_string_120[] = "Kein Quellpfad!";
static char multi_string_121[] = "";
static char multi_string_122[] = "";
static char multi_string_123[] = "Kein Zielpfad!";
static char multi_string_124[] = "";
static char multi_string_125[] = "";
static char multi_string_126[] = "Zu wenig Speicher zum Laden einer Datei!";
static char multi_string_127[] = "";
static char multi_string_128[] = "";
static char multi_string_129[] = "Ein Bild konnte nicht importiert werden!";
static char multi_string_130[] = "";
static char multi_string_131[] = "";
static char multi_string_132[] = "PhotoCD-Modul nicht gefunden: \\\\SMURF\\\\MODULES\\\\PCD.SIM";
static char multi_string_133[] = "";
static char multi_string_134[] = "";
static char multi_string_135[] = "Lesefehler beim Lesen aus PCD-Datei!";
static char multi_string_136[] = "";
static char multi_string_137[] = "";
static char multi_string_138[] = "Fehler: keine Kommunikationsstruktur!";
static char multi_string_139[] = "";
static char multi_string_140[] = "";
static char multi_string_141[] = "Fehler: keine Basepage!";
static char multi_string_142[] = "";
static char multi_string_143[] = "";
static char multi_string_144[] = "Schreibfehler!";
static char multi_string_145[] = "";
static char multi_string_146[] = "";


/* mask of INFOICON */
static _UBYTE multi_RS0_MMASK[] = {
0x03, 0xFC, 0x00, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xFF, 0x80, 0x0F, 0xFF, 0xFF, 0xC0, 
0x0F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xE0, 0x3F, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF, 0xE0, 
0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 
0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 
0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 
0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 
0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF, 0xE0, 
0x0F, 0xFF, 0xFF, 0xE0, 0x01, 0xFF, 0xFF, 0xC0, 0x00, 0x1F, 0xFF, 0x00, 0x00, 0x01, 0xFE, 0x00};

/* data of INFOICON */
static _UBYTE multi_RS0_MDATA[] = {
0x01, 0xF0, 0x00, 0x00, 0x02, 0xFF, 0xE0, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x03, 0x00, 0x1E, 0xC0, 
0x0A, 0xD8, 0x00, 0xC0, 0x00, 0x66, 0x40, 0x40, 0x08, 0x00, 0x34, 0xC0, 0x3C, 0x00, 0x16, 0xC0, 
0x5F, 0xE0, 0x02, 0x60, 0x01, 0xBF, 0x06, 0xE0, 0x40, 0x1E, 0xF6, 0x60, 0x00, 0x00, 0xAA, 0xE0, 
0x40, 0x00, 0x1E, 0xE0, 0xC0, 0x00, 0x0B, 0xE0, 0x80, 0x00, 0x1F, 0xE0, 0xF0, 0x00, 0x1B, 0xE0, 
0xCE, 0x00, 0x0F, 0xE0, 0x87, 0xE0, 0x1F, 0xE0, 0x83, 0xFF, 0x1F, 0xE0, 0xA3, 0xDF, 0x5F, 0xE0, 
0xC3, 0xFA, 0xAF, 0xE0, 0xD5, 0xF7, 0x5F, 0xE0, 0xFE, 0xBA, 0xAF, 0xE0, 0xFF, 0x55, 0x5F, 0xE0, 
0xB9, 0xEA, 0xAF, 0xE0, 0xF7, 0xFF, 0xFF, 0xE0, 0xFF, 0xEF, 0xFF, 0xE0, 0x3F, 0xFF, 0xFF, 0xE0, 
0x03, 0xDB, 0xFF, 0xC0, 0x00, 0x7F, 0x7F, 0x80, 0x00, 0x07, 0xBE, 0x00, 0x00, 0x00, 0x7C, 0x00};

/* color data of INFOICON */
static _UBYTE multi_RS0_4CDATA[] = {
0x00, 0xF0, 0x00, 0x00, 0x01, 0xFF, 0xE0, 0x00, 0x05, 0x1F, 0xFF, 0x40, 0x0F, 0x00, 0x3F, 0x40, 
0x17, 0xF8, 0x13, 0x40, 0x15, 0x7F, 0xC1, 0x40, 0x14, 0x0F, 0xF9, 0x40, 0x40, 0x00, 0x4B, 0x40, 
0x20, 0x00, 0x0B, 0x60, 0x25, 0x40, 0x0B, 0x60, 0x27, 0xE0, 0x0B, 0x60, 0x20, 0x01, 0x0B, 0xE0, 
0x20, 0x00, 0x2B, 0xE0, 0xA0, 0x00, 0x2B, 0xE0, 0xA0, 0x00, 0x2B, 0xE0, 0xF0, 0x00, 0x2B, 0xE0, 
0xFE, 0x00, 0x2B, 0xE0, 0xFF, 0xE0, 0x2F, 0xE0, 0xFF, 0xFF, 0x2F, 0xE0, 0xFB, 0xFF, 0x5F, 0xE0, 
0xFF, 0xFA, 0xBF, 0xE0, 0xFF, 0xF7, 0x7F, 0xE0, 0xFF, 0xFA, 0xFF, 0xE0, 0xFF, 0x55, 0xFF, 0xE0, 
0xFE, 0xEB, 0xFF, 0xE0, 0xF5, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF, 0xE0, 0x3F, 0xFF, 0xFF, 0xE0, 
0x03, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0x80, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x7C, 0x00, 
0x00, 0x70, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x10, 0x01, 0x40, 0x0C, 0x00, 0x21, 0x40, 
0x14, 0x00, 0x13, 0x40, 0x15, 0x00, 0x01, 0x40, 0x14, 0x08, 0x01, 0x40, 0x5C, 0x00, 0x4B, 0x40, 
0x3F, 0xE0, 0x0B, 0x60, 0x25, 0xDF, 0x0B, 0x20, 0x27, 0xFE, 0xEB, 0x40, 0x20, 0x01, 0x0B, 0xA0, 
0x20, 0x00, 0x2B, 0x40, 0xA0, 0x00, 0x2A, 0xA0, 0xA0, 0x00, 0x2B, 0x40, 0x80, 0x00, 0x2A, 0xA0, 
0xB2, 0x00, 0x2B, 0x40, 0xF8, 0x00, 0x2A, 0xA0, 0xFC, 0x02, 0x2D, 0x40, 0xF8, 0x00, 0x00, 0xE0, 
0xFE, 0x00, 0x01, 0x60, 0x98, 0x00, 0x02, 0xA0, 0x80, 0x40, 0x07, 0x40, 0x80, 0x00, 0x02, 0xA0, 
0x84, 0x00, 0x05, 0x60, 0x80, 0x00, 0x02, 0xA0, 0x00, 0x00, 0x05, 0x60, 0x3E, 0x00, 0x02, 0xE0, 
0x03, 0x80, 0x05, 0x40, 0x00, 0x78, 0x07, 0x80, 0x00, 0x07, 0x84, 0x00, 0x00, 0x00, 0x7C, 0x00, 
0x00, 0x70, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x10, 0x01, 0x40, 0x0F, 0x00, 0x21, 0x40, 
0x17, 0xE8, 0x13, 0x40, 0x15, 0x7F, 0xC1, 0x40, 0x14, 0x0F, 0xF9, 0x40, 0x40, 0x00, 0x4B, 0x40, 
0x20, 0x00, 0x0B, 0x60, 0x24, 0x00, 0x0B, 0x20, 0x27, 0xE0, 0x0B, 0x40, 0x20, 0x01, 0x0B, 0xA0, 
0x20, 0x00, 0x2B, 0x40, 0xA0, 0x00, 0x2A, 0xA0, 0xA0, 0x00, 0x2B, 0x40, 0x80, 0x00, 0x2A, 0xA0, 
0xB6, 0x00, 0x2B, 0x40, 0xA0, 0x00, 0x2A, 0xA0, 0x85, 0x02, 0x2D, 0x40, 0xE0, 0x20, 0x00, 0xE0, 
0xA6, 0x40, 0x11, 0x60, 0xBE, 0x01, 0x62, 0xA0, 0x85, 0x40, 0x77, 0x40, 0xA2, 0x00, 0xE2, 0xA0, 
0xC6, 0x41, 0x75, 0x60, 0x80, 0x00, 0x02, 0xA0, 0x00, 0x10, 0x05, 0x60, 0x3E, 0x08, 0x02, 0xE0, 
0x03, 0xB4, 0x05, 0x40, 0x00, 0x7A, 0xA7, 0x80, 0x00, 0x07, 0xC4, 0x00, 0x00, 0x00, 0x7C, 0x00, 
0x01, 0xF0, 0x00, 0x00, 0x02, 0xFB, 0xE0, 0x00, 0x06, 0x00, 0x1F, 0x80, 0x03, 0x00, 0x1E, 0xC0, 
0x1B, 0xF8, 0x00, 0xC0, 0x18, 0xE7, 0x40, 0xC0, 0x18, 0x00, 0x3C, 0xC0, 0x3C, 0x00, 0x36, 0xC0, 
0x43, 0xE0, 0x06, 0xE0, 0x43, 0x7F, 0x06, 0xE0, 0x40, 0x01, 0xF6, 0xE0, 0x40, 0x00, 0xFE, 0xE0, 
0x40, 0x00, 0x1E, 0xE0, 0xC0, 0x00, 0x1F, 0xE0, 0xC0, 0x00, 0x1F, 0xFC, 0xF0, 0x00, 0x1F, 0xFF, 
0xCE, 0x00, 0x1F, 0xFF, 0x87, 0xE0, 0x1F, 0xFE, 0x83, 0xFF, 0x1F, 0xFE, 0x83, 0xFE, 0xAF, 0xFE, 
0xC3, 0xFF, 0xDF, 0xFC, 0xC7, 0xF9, 0xDF, 0xFC, 0xFF, 0x85, 0xBF, 0xF8, 0xFF, 0xFF, 0x7F, 0xF8, 
0xFB, 0x7E, 0xBF, 0xF8, 0xFE, 0xFF, 0xFF, 0xF0, 0x7F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xE0, 
0x03, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0x80, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x7C, 0x00};

/* color mask of INFOICON */
static _UBYTE multi_RS0_4CMASK[] = {
0x01, 0xF0, 0x00, 0x00, 0x03, 0xFF, 0xE0, 0x00, 0x07, 0xFF, 0xFF, 0xC0, 0x0F, 0xFF, 0xFF, 0xC0, 
0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xC0, 0x7F, 0xFF, 0xFF, 0xC0, 
0x7F, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF, 0xE0, 
0x7F, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xF8, 
0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xF0, 0x7F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xE0, 
0x03, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0x80, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x7C, 0x00};

static CICON rs_cicon[] = {
	{ 4, (_WORD *) multi_RS0_4CDATA, (_WORD *) multi_RS0_4CMASK, 0, 0, 0 }
};


static CICONBLK rs_ciconblk[] = {
	{ { (_WORD *) multi_RS0_MMASK, (_WORD *) multi_RS0_MDATA, multi_string_62, 4096,24,23, 0,0,32,32, 13,13,6,8 }, &rs_cicon[0] } /* INFOICON */
};



#define RSC_HAS_PALETTE 1
#ifndef RSC_USE_PALETTE
#  define RSC_USE_PALETTE 0
#endif
#if (RSC_STATIC_FILE && RSC_NAMED_FUNCTIONS) || RSC_USE_PALETTE
#if RSC_USE_PALETTE && !RSC_NAMED_FUNCTIONS && RSC_STATIC_FILE
_WORD rgb_palette[][4] = {
#else
static _WORD rgb_palette[][4] = {
#endif
	{ 0x03E8, 0x03E8, 0x03E8, 0x0000 },
	{ 0x03E8, 0x0000, 0x0000, 0x0002 },
	{ 0x0000, 0x03E8, 0x0000, 0x0003 },
	{ 0x03E8, 0x03E8, 0x0000, 0x0006 },
	{ 0x0000, 0x0000, 0x03E8, 0x0004 },
	{ 0x03E8, 0x0000, 0x03E8, 0x0007 },
	{ 0x0000, 0x03E8, 0x03E8, 0x0005 },
	{ 0x0329, 0x0329, 0x0329, 0x0008 },
	{ 0x020B, 0x020B, 0x020B, 0x0009 },
	{ 0x029A, 0x0000, 0x0000, 0x000A },
	{ 0x0000, 0x029A, 0x0000, 0x000B },
	{ 0x029A, 0x029A, 0x0000, 0x000E },
	{ 0x0000, 0x0000, 0x029A, 0x000C },
	{ 0x029A, 0x0000, 0x029A, 0x000F },
	{ 0x0000, 0x029A, 0x029A, 0x000D },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0001 }
};
#endif


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ multi_string_1, multi_string_2, multi_string_3, SMALL, 6, TE_LEFT, 0x1080, 0x0, -1, 12,1 },
	{ multi_string_4, multi_string_5, multi_string_6, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 12,1 },
	{ multi_string_7, multi_string_8, multi_string_9, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 27,1 }, /* EXPLIST_1 */
	{ multi_string_10, multi_string_11, multi_string_12, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_13, multi_string_14, multi_string_15, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_16, multi_string_17, multi_string_18, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_19, multi_string_20, multi_string_21, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_22, multi_string_23, multi_string_24, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_25, multi_string_26, multi_string_27, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_28, multi_string_29, multi_string_30, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 },
	{ multi_string_31, multi_string_32, multi_string_33, SMALL, 6, TE_LEFT, 0x1180, 0x0, -1, 2,1 }, /* EXPLIST_9 */
	{ multi_string_35, multi_string_36, multi_string_37, IBM, 6, TE_CNTR, 0x1180, 0x0, -2, 46,1 }, /* SOURCE_PATH */
	{ multi_string_38, multi_string_39, multi_string_40, SMALL, 6, TE_LEFT, 0x1080, 0x0, -1, 10,1 },
	{ multi_string_41, multi_string_42, multi_string_43, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 10,1 },
	{ multi_string_44, multi_string_45, multi_string_46, IBM, 6, TE_CNTR, 0x1180, 0x0, -2, 46,1 }, /* DEST_PATH */
	{ multi_string_47, multi_string_48, multi_string_49, SMALL, 6, TE_LEFT, 0x1080, 0x0, -1, 9,1 },
	{ multi_string_50, multi_string_51, multi_string_52, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 9,1 },
	{ multi_string_53, multi_string_54, multi_string_55, SMALL, 6, TE_LEFT, 0x1080, 0x0, -1, 10,1 },
	{ multi_string_56, multi_string_57, multi_string_58, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 10,1 },
	{ multi_string_59, multi_string_60, multi_string_61, IBM, 6, TE_CNTR, 0x1180, 0x0, -1, 49,49 }, /* WILDCARDS */
	{ multi_string_63, multi_string_64, multi_string_65, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 7,1 }, /* DEPTH_PB */
	{ multi_string_66, multi_string_67, multi_string_68, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 13,1 },
	{ multi_string_69, multi_string_70, multi_string_71, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 13,1 },
	{ multi_string_72, multi_string_73, multi_string_74, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 5,1 }, /* COLSYS_PB */
	{ multi_string_75, multi_string_76, multi_string_77, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 11,1 },
	{ multi_string_78, multi_string_79, multi_string_80, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 11,1 },
	{ multi_string_81, multi_string_82, multi_string_83, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 16,1 }, /* DITHER_PB */
	{ multi_string_84, multi_string_85, multi_string_86, SMALL, 6, TE_LEFT, 0x1000, 0x0, -1, 18,1 },
	{ multi_string_87, multi_string_88, multi_string_89, SMALL, 6, TE_LEFT, 0x1100, 0x0, -1, 18,1 },
	{ multi_string_90, multi_string_91, multi_string_92, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 14,1 }, /* PALMODE_PB */
	{ multi_string_93, multi_string_94, multi_string_95, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ multi_string_96, multi_string_97, multi_string_98, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ multi_string_100, multi_string_101, multi_string_102, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 15,1 },
	{ multi_string_103, multi_string_104, multi_string_105, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ multi_string_108, multi_string_109, multi_string_110, IBM, 0, TE_LEFT, 0x1180, 0x0, -1, 12,1 },
	{ multi_string_111, multi_string_112, multi_string_113, IBM, 6, TE_LEFT, 0x1000, 0x0, -1, 4,1 },
	{ multi_string_114, multi_string_115, multi_string_116, IBM, 6, TE_LEFT, 0x1100, 0x0, -1, 4,1 },
	{ multi_string_117, multi_string_118, multi_string_119, IBM, 6, TE_CNTR, 0x1100, 0x0, -1, 32,1 }, /* PROC_TEXT */
	{ multi_string_120, multi_string_121, multi_string_122, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 16,1 }, /* NO_SOURCE */
	{ multi_string_123, multi_string_124, multi_string_125, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 15,1 }, /* NO_DEST */
	{ multi_string_126, multi_string_127, multi_string_128, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 41,1 }, /* NO_LOADMEM */
	{ multi_string_129, multi_string_130, multi_string_131, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 41,1 }, /* IMPORT_ERROR */
	{ multi_string_132, multi_string_133, multi_string_134, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 56,1 }, /* NO_PCD */
	{ multi_string_135, multi_string_136, multi_string_137, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 37,1 }, /* PCD_READ_ERROR */
	{ multi_string_138, multi_string_139, multi_string_140, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 38,1 }, /* NO_COM_STRUCT */
	{ multi_string_141, multi_string_142, multi_string_143, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 24,1 }, /* NO_BASEPAGE */
	{ multi_string_144, multi_string_145, multi_string_146, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 15,1 } /* WRITE_ERROR */
};


static OBJECT rs_object[NUM_OBS] = {
/* MULTICON */

	{ -1, 1, 64, G_BOX, OF_FL3DBAK, OS_NORMAL, C_UNION(0x1100L), 0,0, 1083,22 },
	{ 2, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(multi_string_0), 1070,20, 1034,1 }, /* GO */
	{ 3, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x10100L), 513,2823, 1049,8 },
	{ 22, 4, 21, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 257,2567, 1049,8 },
	{ 5, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 1792,1792, 520,2304 },
	{ 6, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1536,1792, 520,2048 },
	{ 17, 7, 7, G_BOX, OF_FL3DIND, OS_NORMAL, C_UNION(0x11101L), 769,513, 1044,2052 }, /* _BOX */
	{ 6, 8, 16, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0,0, 1044,2052 }, /* EXPLIST_PARENT */
	{ 9, -1, -1, (21<<8)+G_TEXT, 0x415, OS_SELECTED, C_UNION(&rs_tedinfo[2]), 0,0, 1044,2048 }, /* EXPLIST_1 */
	{ 10, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 0,2048, 1044,2048 },
	{ 11, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 0,1, 1044,2048 },
	{ 12, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 0,2049, 1044,2048 },
	{ 13, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 0,2, 1044,2048 },
	{ 14, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 0,2050, 1044,2048 },
	{ 15, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 0,3, 1044,2048 },
	{ 16, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 0,2051, 1044,2048 },
	{ 7, -1, -1, G_TEXT, 0x415, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 0,4, 1044,2048 }, /* EXPLIST_9 */
	{ 18, -1, -1, (21<<8)+G_BOXCHAR, OF_FL3DACT, OS_NORMAL, C_UNION(0x1FF1100L), 1558,769, 1537,1 }, /* EXPLIST_UP */
	{ 20, 19, 19, (21<<8)+G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x111F9L), 1046,1282, 514,2563 }, /* EXPLIST_SB */
	{ 18, -1, -1, (21<<8)+G_BOX, OF_FL3DACT, OS_NORMAL, C_UNION(0xFF1100L), 512,2048, 1537,2050 }, /* EXPLIST_SL */
	{ 21, -1, -1, (21<<8)+G_BOXCHAR, OF_FL3DACT, OS_NORMAL, C_UNION(0x2FF1100L), 1558,262, 1537,1 }, /* EXPLIST_DN */
	{ 3, -1, -1, G_BUTTON, 0x601, OS_NORMAL, C_UNION(multi_string_34), 1025,262, 532,1 }, /* EXPORTER_OPTIONS */
	{ 23, -1, -1, G_BOXTEXT, 0x605, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 2,1281, 48,1 }, /* SOURCE_PATH */
	{ 24, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 1281,2048, 1330,2304 },
	{ 25, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 1025,2048, 1072,2048 },
	{ 26, -1, -1, G_BOXTEXT, 0x605, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 2,2307, 48,1 }, /* DEST_PATH */
	{ 27, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 1281,3074, 6,2304 },
	{ 28, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 1025,3074, 6,2048 },
	{ 29, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 1537,517, 1542,2304 },
	{ 30, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 1281,517, 1542,2048 },
	{ 31, -1, -1, G_FTEXT, 0x408, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 2,3589, 48,1 }, /* WILDCARDS */
	{ 32, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x117DL), 53,2817, 1030,1 },
	{ 33, -1, -1, G_CICON, OF_NONE, OS_NORMAL, C_UNION(&rs_ciconblk[0]), 54,1536, 8192,8192 }, /* INFOICON */
	{ 34, -1, -1, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x10100L), 1563,2823, 30,2059 },
	{ 54, 35, 53, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 1307,2567, 30,2059 },
	{ 36, -1, -1, G_BOXTEXT, 0x645, OS_SHADOWED, C_UNION(&rs_tedinfo[20]), 1,257, 1034,1 }, /* DEPTH_PB */
	{ 37, -1, -1, (45<<8)+G_IBOX, 0x401, OS_NORMAL, C_UNION(0x12100L), 1803,3584, 3,2049 }, /* DEPTH_CB */
	{ 38, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[21]), 1792,1536, 9,2048 },
	{ 39, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[22]), 1536,1280, 9,2048 },
	{ 40, -1, -1, G_BOXTEXT, 0x645, 0x28, C_UNION(&rs_tedinfo[23]), 1,2307, 1034,1 }, /* COLSYS_PB */
	{ 41, -1, -1, (45<<8)+G_IBOX, 0x401, OS_DISABLED, C_UNION(0x12100L), 1803,1539, 3,2049 }, /* COLSYS_CB */
	{ 42, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[24]), 1,3586, 1031,2048 },
	{ 43, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[25]), 1792,3330, 1031,2048 },
	{ 44, -1, -1, G_BOXTEXT, 0x645, OS_SHADOWED, C_UNION(&rs_tedinfo[26]), 1,1286, 16,1 }, /* DITHER_PB */
	{ 45, -1, -1, (45<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0x12100L), 785,518, 3,2049 }, /* DITHER_CB */
	{ 46, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[27]), 1792,2053, 1548,2048 },
	{ 47, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[28]), 1536,1797, 1548,2048 },
	{ 48, -1, -1, G_BOXTEXT, 0x645, OS_SHADOWED, C_UNION(&rs_tedinfo[29]), 1,1288, 16,1 }, /* PALMODE_PB */
	{ 49, -1, -1, (45<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0x12100L), 785,520, 3,2049 }, /* PALMODE_CB */
	{ 50, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[30]), 18,513, 1035,1 },
	{ 51, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_NORMAL, C_UNION(0xFF1100L), 1039,513, 1037,1 }, /* KEEP_DEPTH */
	{ 52, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[31]), 18,3075, 1035,1 },
	{ 53, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_DISABLED, C_UNION(0xFF1100L), 1039,3075, 1037,1 }, /* KEEP_COLSYS */
	{ 34, -1, -1, G_BUTTON, 0x605, OS_DISABLED, C_UNION(multi_string_99), 1793,266, 1295,1 }, /* LOADPAL */
	{ 55, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[32]), 1028,2064, 1039,1 },
	{ 56, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[33]), 1028,18, 1036,1 },
	{ 57, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_DISABLED, C_UNION(0xFF1100L), 2,2064, 17,1 },
	{ 58, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_DISABLED, C_UNION(0xFF1100L), 2,18, 14,1 },
	{ 59, -1, -1, G_BUTTON, 0x605, OS_DISABLED, C_UNION(multi_string_106), 30,20, 9,1 },
	{ 60, -1, -1, G_BUTTON, 0x605, OS_DISABLED, C_UNION(multi_string_107), 18,20, 10,1 },
	{ 61, -1, -1, G_TEXT, OF_FL3DBAK, OS_NORMAL, C_UNION(&rs_tedinfo[34]), 1028,20, 12,1 },
	{ 62, -1, -1, (44<<8)+G_IBOX, OF_SELECTABLE, OS_DISABLED, C_UNION(0xFF1100L), 2,20, 14,1 },
	{ 63, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[35]), 54,2050, 3,1 },
	{ 64, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[36]), 1845,1794, 3,1 },
	{ 0, 65, 66, G_BOX, 0x680, OS_SELECTED, C_UNION(0xFF1100L), 513,3347, 1067,2049 }, /* PROC_BACK */
	{ 66, -1, -1, G_BOX, OF_FL3DIND, OS_NORMAL, C_UNION(0x1117AL), 0,0, 23,2049 }, /* PROC_BAR */
	{ 64, -1, -1, G_TEXT, OF_LASTOB, OS_NORMAL, C_UNION(&rs_tedinfo[37]), 0,1024, 1067,1 }, /* PROC_TEXT */

/* ALERT_STRINGS */

	{ -1, 1, 9, G_BOX, OF_NONE, OS_SHADOWED, C_UNION(0x11100L), 0,0, 1082,2062 },
	{ 2, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[38]), 2,2048, 15,1 }, /* NO_SOURCE */
	{ 3, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[39]), 2,2, 14,1 }, /* NO_DEST */
	{ 4, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[40]), 2,2051, 40,1 }, /* NO_LOADMEM */
	{ 5, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[41]), 2,5, 40,1 }, /* IMPORT_ERROR */
	{ 6, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[42]), 2,2054, 55,1 }, /* NO_PCD */
	{ 7, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[43]), 2,8, 36,1 }, /* PCD_READ_ERROR */
	{ 8, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[44]), 2,2057, 37,1 }, /* NO_COM_STRUCT */
	{ 9, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[45]), 2,11, 23,1 }, /* NO_BASEPAGE */
	{ 0, -1, -1, G_TEXT, OF_LASTOB, OS_NORMAL, C_UNION(&rs_tedinfo[46]), 2,2060, 14,1 } /* WRITE_ERROR */
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0], /* MULTICON */
	&rs_object[67] /* ALERT_STRINGS */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD multi_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD multi_rsc_load(wchar, hchar)
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
_WORD multi_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD multi_rsc_gaddr(type, idx, gaddr)
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
_WORD multi_rsc_free(void)
#else
_WORD multi_rsc_free()
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
_WORD rs_numstrings = 147;
_WORD rs_numfrstr = 0;

_WORD rs_nuser = 0;
_WORD rs_numimages = 0;
_WORD rs_numbb = 0;
_WORD rs_numfrimg = 0;
_WORD rs_numib = 0;
_WORD rs_numcib = 1;
_WORD rs_numti = 47;
_WORD rs_numobs = 77;
_WORD rs_numtree = 2;

char rs_name[] = "multi.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
