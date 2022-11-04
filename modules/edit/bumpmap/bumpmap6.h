/*
 * resource set indices for bumpmap6
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        67
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       22
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        47
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          1976
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "bumpmap6"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef bumpmap6
#define RSC_ID bumpmap6
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 67
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 22
#define NUM_OBS 47
#define NUM_TREE 1
#endif



#define BUMPMAIN           0 /* form/dialog */
#define DO_IT              1 /* BUTTON in tree BUMPMAIN */
#define BM_STRENGH_FHR     5 /* BOX in tree BUMPMAIN */
#define BM_STRENGH_SLIDE   6 /* BOX in tree BUMPMAIN */
#define BM_STRENGH_EDIT    7 /* BOXTEXT in tree BUMPMAIN */
#define HW_RADIO_RED      10 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_GREEN    12 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_BLUE     14 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_GREY     16 /* IBOX in tree BUMPMAIN */
#define BM_PARALEL        18 /* IBOX in tree BUMPMAIN */
#define M_HARD_FHR        27 /* BOX in tree BUMPMAIN */
#define M_HARD_SLIDE      28 /* BOX in tree BUMPMAIN */
#define M_HARD_EDIT       29 /* BOXTEXT in tree BUMPMAIN */
#define M_STRENGH_FHR     30 /* BOX in tree BUMPMAIN */
#define M_STRENGH_SLIDE   31 /* BOX in tree BUMPMAIN */
#define M_STRENGH_EDIT    32 /* BOXTEXT in tree BUMPMAIN */
#define M_RED_FHR         33 /* BOX in tree BUMPMAIN */
#define M_RED_SLIDE       34 /* BOX in tree BUMPMAIN */
#define M_GREEN_FHR       35 /* BOX in tree BUMPMAIN */
#define M_GREEN_SLIDE     36 /* BOX in tree BUMPMAIN */
#define M_BLUE_FHR        37 /* BOX in tree BUMPMAIN */
#define M_BLUE_SLIDE      38 /* BOX in tree BUMPMAIN */
#define M_RED_EDIT        39 /* BOXTEXT in tree BUMPMAIN */
#define M_GREEN_EDIT      40 /* BOXTEXT in tree BUMPMAIN */
#define M_BLUE_EDIT       41 /* BOXTEXT in tree BUMPMAIN */
#define M_RADIO_DIFFUSE   44 /* IBOX in tree BUMPMAIN */
#define M_RADIO_GLANZ     46 /* IBOX in tree BUMPMAIN */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD bumpmap6_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD bumpmap6_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD bumpmap6_rsc_free(void);
#endif
