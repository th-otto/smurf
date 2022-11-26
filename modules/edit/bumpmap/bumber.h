/*
 * resource set indices for bumber
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        83
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       27
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        53
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          2396
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "bumber"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef bumber
#define RSC_ID bumber
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 83
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 27
#define NUM_OBS 53
#define NUM_TREE 1
#endif



#define BUMPMAIN                           0 /* form/dialog */
#define DO_IT                              1 /* BUTTON in tree BUMPMAIN */
#define M_BLUE_EDIT                        4 /* BOXTEXT in tree BUMPMAIN */
#define M_GREEN_EDIT                       5 /* BOXTEXT in tree BUMPMAIN */
#define M_RED_EDIT                         6 /* BOXTEXT in tree BUMPMAIN */
#define M_BLUE_FHR                         7 /* BOX in tree BUMPMAIN */
#define M_BLUE_SLIDE                       8 /* BOX in tree BUMPMAIN */
#define M_GREEN_FHR                        9 /* BOX in tree BUMPMAIN */
#define M_GREEN_SLIDE                     10 /* BOX in tree BUMPMAIN */
#define M_RED_FHR                         11 /* BOX in tree BUMPMAIN */
#define M_RED_SLIDE                       12 /* BOX in tree BUMPMAIN */
#define M_BRIGHTNESS_EDIT                 13 /* BOXTEXT in tree BUMPMAIN */
#define M_BRIGHTNESS_FHR                  14 /* BOX in tree BUMPMAIN */
#define M_BRIGHTNESS_SLIDE                15 /* BOX in tree BUMPMAIN */
#define M_HARD_EDIT                       16 /* BOXTEXT in tree BUMPMAIN */
#define M_HARD_FHR                        17 /* BOX in tree BUMPMAIN */
#define M_HARD_SLIDE                      18 /* BOX in tree BUMPMAIN */
#define M_RADIO_DIFFUSE                   25 /* IBOX in tree BUMPMAIN */
#define M_RADIO_GLANZ                     27 /* IBOX in tree BUMPMAIN */
#define BM_STRENGTH_FHR                   31 /* BOX in tree BUMPMAIN */
#define BM_STRENGTH_SLIDE                 32 /* BOX in tree BUMPMAIN */
#define BM_STRENGTH_EDIT                  33 /* BOXTEXT in tree BUMPMAIN */
#define BM_PARALEL                        37 /* IBOX in tree BUMPMAIN */
#define BM_TEXTURE                        39 /* IBOX in tree BUMPMAIN */
#define BLACK_BAK                         44 /* IBOX in tree BUMPMAIN */
#define WHITE_BAK                         45 /* IBOX in tree BUMPMAIN */
#define NORM_BAK                          47 /* IBOX in tree BUMPMAIN */
#define BM_INVERT                         49 /* IBOX in tree BUMPMAIN */
#define SAVE_SET                          51 /* BUTTON in tree BUMPMAIN */
#define LOAD_SET                          52 /* BOXTEXT in tree BUMPMAIN */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD bumber_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD bumber_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD bumber_rsc_free(void);
#endif
