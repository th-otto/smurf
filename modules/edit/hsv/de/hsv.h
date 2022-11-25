/*
 * resource set indices for hsv
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        30
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       9
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        26
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          990
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "hsv"
#endif
#undef RSC_ID
#ifdef hsv
#define RSC_ID hsv
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 30
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 9
#define NUM_OBS 26
#define NUM_TREE 1
#endif



#define HSVS_MIR           0 /* form/dialog */
#define STARTIT            1 /* BUTTON in tree HSVS_MIR */
#define PREV_BOX           2 /* BOX in tree HSVS_MIR */
#define S4_E               4 /* BOXTEXT in tree HSVS_MIR */
#define S4_F               5 /* BOX in tree HSVS_MIR */
#define S4_S               6 /* BOX in tree HSVS_MIR */
#define COMPONENT1         7 /* TEXT in tree HSVS_MIR */
#define COMPONENT2         8 /* BOXTEXT in tree HSVS_MIR */
#define COMPONENT3         9 /* BOXTEXT in tree HSVS_MIR */
#define S5_F              10 /* BOX in tree HSVS_MIR */
#define S5_S              11 /* BOX in tree HSVS_MIR */
#define S6_F              12 /* BOX in tree HSVS_MIR */
#define S6_S              13 /* BOX in tree HSVS_MIR */
#define S5_E              14 /* BOXTEXT in tree HSVS_MIR */
#define S6_E              15 /* BOXTEXT in tree HSVS_MIR */
#define CHECK_S           16 /* BOXCHAR in tree HSVS_MIR */
#define CHECK_V           17 /* BOXCHAR in tree HSVS_MIR */
#define CHECK_H           18 /* BOXCHAR in tree HSVS_MIR */
#define MCONF_SAVE        19 /* BUTTON in tree HSVS_MIR */
#define MCONF_LOAD        20 /* BUTTON in tree HSVS_MIR */
#define SYSTEM_BOX        21 /* IBOX in tree HSVS_MIR */
#define _HSV              23 /* IBOX in tree HSVS_MIR */
#define _HLS              25 /* IBOX in tree HSVS_MIR */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD hsv_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD hsv_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD hsv_rsc_free(void);
#endif
