/*
 * resource set indices for bumpmapq
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        64
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       21
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        45
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          1888
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "bumpmapq"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef bumpmapq
#define RSC_ID bumpmapq
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 64
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 21
#define NUM_OBS 45
#define NUM_TREE 1
#endif



#define BUMPMAIN           0 /* form/dialog */
#define DO_IT              1 /* BUTTON in tree BUMPMAIN */
#define BM_STRENGH_FHR     5 /* BOX in tree BUMPMAIN */
#define BM_STRENGH_SLIDE   6 /* BOX in tree BUMPMAIN */
#define BM_STRENGH_EDIT    7 /* BOXTEXT in tree BUMPMAIN */
#define HW_RADIO_RED      13 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_GREEN    14 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_BLUE     15 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_GREY     16 /* IBOX in tree BUMPMAIN */
#define M_HARD_FHR        25 /* BOX in tree BUMPMAIN */
#define M_HARD_SLIDE      26 /* BOX in tree BUMPMAIN */
#define M_HARD_EDIT       27 /* BOXTEXT in tree BUMPMAIN */
#define M_STRENGH_FHR     28 /* BOX in tree BUMPMAIN */
#define M_STRENGH_SLIDE   29 /* BOX in tree BUMPMAIN */
#define M_STRENGH_EDIT    30 /* BOXTEXT in tree BUMPMAIN */
#define M_RED_FHR         31 /* BOX in tree BUMPMAIN */
#define M_RED_SLIDE       32 /* BOX in tree BUMPMAIN */
#define M_GREEN_FHR       33 /* BOX in tree BUMPMAIN */
#define M_GREEN_SLIDE     34 /* BOX in tree BUMPMAIN */
#define M_BLUE_FHR        35 /* BOX in tree BUMPMAIN */
#define M_BLUE_SLIDE      36 /* BOX in tree BUMPMAIN */
#define M_RED_EDIT        37 /* BOXTEXT in tree BUMPMAIN */
#define M_GREEN_EDIT      38 /* BOXTEXT in tree BUMPMAIN */
#define M_BLUE_EDIT       39 /* BOXTEXT in tree BUMPMAIN */
#define M_RADIO_DIFFUSE   42 /* IBOX in tree BUMPMAIN */
#define M_RADIO_GLANZ     44 /* IBOX in tree BUMPMAIN */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD bumpmapq_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD bumpmapq_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD bumpmapq_rsc_free(void);
#endif
