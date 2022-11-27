/*
 * resource set indices for bumpmap
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        99
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       32
 * Number of Free Strings:   1
 * Number of Free Images:    0
 * Number of Objects:        64
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          2844
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "bumpmap"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef bumpmap
#define RSC_ID bumpmap
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 99
#define NUM_FRSTR 1
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 32
#define NUM_OBS 64
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
#define SAVE_SET                          35 /* BUTTON in tree BUMPMAIN */
#define LOAD_SET                          36 /* BOXTEXT in tree BUMPMAIN */
#define HW_RADIO_RED                      40 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_BLUE                     42 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_GREEN                    44 /* IBOX in tree BUMPMAIN */
#define HW_RADIO_GREY                     46 /* IBOX in tree BUMPMAIN */
#define BM_STRENGTH_FHR                   48 /* BOX in tree BUMPMAIN */
#define BM_STRENGTH_SLIDE                 49 /* BOX in tree BUMPMAIN */
#define BM_STRENGTH_EDIT                  50 /* BOXTEXT in tree BUMPMAIN */
#define BM_PARALEL                        53 /* IBOX in tree BUMPMAIN */
#define BM_TEXTURE                        55 /* IBOX in tree BUMPMAIN */
#define BLACK_BAK                         58 /* IBOX in tree BUMPMAIN */
#define WHITE_BAK                         59 /* IBOX in tree BUMPMAIN */
#define NORM_BAK                          61 /* IBOX in tree BUMPMAIN */
#define BM_INVERT                         63 /* IBOX in tree BUMPMAIN */

#define LOAD_SET_STR                       0 /* Free string */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD bumpmap_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD bumpmap_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD bumpmap_rsc_free(void);
#endif
