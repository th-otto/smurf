/*
 * resource set indices for tiffrsc
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        27
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       8
 * Number of Free Strings:   1
 * Number of Free Images:    0
 * Number of Objects:        21
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          880
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "tiffrsc"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef tiffrsc
#define RSC_ID tiffrsc
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 27
#define NUM_FRSTR 1
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 8
#define NUM_OBS 21
#define NUM_TREE 1
#endif



#define TIFF_EXPORT        0 /* form/dialog */
#define OK                 1 /* BUTTON in tree TIFF_EXPORT */
#define KEINE              6 /* IBOX in tree TIFF_EXPORT */
#define RLE                7 /* IBOX in tree TIFF_EXPORT */
#define HUFFMAN            9 /* IBOX in tree TIFF_EXPORT */
#define LZW               11 /* IBOX in tree TIFF_EXPORT */
#define MOTO              17 /* IBOX in tree TIFF_EXPORT */
#define INTEL             18 /* IBOX in tree TIFF_EXPORT */
#define SAVE              20 /* BUTTON in tree TIFF_EXPORT */

#define WINDOW_TITLE       0 /* Free string */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD tiffrsc_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD tiffrsc_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD tiffrsc_rsc_free(void);
#endif
