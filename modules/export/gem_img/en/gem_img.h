/*
 * resource set indices for gem_img
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        14
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       4
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        12
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          506
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "gem_img"
#endif
#undef RSC_ID
#ifdef gem_img
#define RSC_ID gem_img
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 14
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 4
#define NUM_OBS 12
#define NUM_TREE 1
#endif



#define XIMG_EXPORT        0 /* form/dialog */
#define OK                 1 /* BUTTON in tree XIMG_EXPORT */
#define GVW                6 /* IBOX in tree XIMG_EXPORT */
#define PIXA               7 /* IBOX in tree XIMG_EXPORT */
#define FORMAT            10 /* IBOX in tree XIMG_EXPORT */
#define SAVE              11 /* BUTTON in tree XIMG_EXPORT */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD gem_img_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD gem_img_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD gem_img_rsc_free(void);
#endif
