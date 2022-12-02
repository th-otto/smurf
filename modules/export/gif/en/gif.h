/*
 * resource set indices for gif
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        18
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       5
 * Number of Free Strings:   1
 * Number of Free Images:    0
 * Number of Objects:        15
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          662
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "gif"
#endif
#undef RSC_ID
#ifdef gif
#define RSC_ID gif
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 18
#define NUM_FRSTR 1
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 5
#define NUM_OBS 15
#define NUM_TREE 1
#endif



#define GIF_EXPORT         0 /* form/dialog */
#define OK                 1 /* BUTTON in tree GIF_EXPORT */
#define GIF87A             6 /* IBOX in tree GIF_EXPORT */
#define GIF89A             7 /* IBOX in tree GIF_EXPORT */
#define LINE_ARRANGE      10 /* IBOX in tree GIF_EXPORT */
#define SAVE              11 /* BUTTON in tree GIF_EXPORT */
#define TINDEX_BOX        12 /* IBOX in tree GIF_EXPORT */
#define TINDEX            13 /* FTEXT in tree GIF_EXPORT */
#define TINDEXOK          14 /* IBOX in tree GIF_EXPORT */

#define WRONG_IDX          0 /* Alert string */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD gif_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD gif_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD gif_rsc_free(void);
#endif
