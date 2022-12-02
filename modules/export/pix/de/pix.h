/*
 * resource set indices for pix
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        14
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       3
 * Number of Free Strings:   1
 * Number of Free Images:    0
 * Number of Objects:        12
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          486
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "pix"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef pix
#define RSC_ID pix
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 14
#define NUM_FRSTR 1
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 3
#define NUM_OBS 12
#define NUM_TREE 1
#endif



#define PIX_EXPORT         0 /* form/dialog */
#define SAVE               1 /* BUTTON in tree PIX_EXPORT */
#define KEINE              6 /* IBOX in tree PIX_EXPORT */
#define RLE                7 /* IBOX in tree PIX_EXPORT */
#define OK                11 /* BUTTON in tree PIX_EXPORT */

#define WINDOW_TITLE       0 /* Free string */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD pix_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD pix_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD pix_rsc_free(void);
#endif
