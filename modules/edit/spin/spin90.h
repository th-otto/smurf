/*
 * resource set indices for spin90
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        10
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       3
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        5
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          290
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "spin90"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef spin90
#define RSC_ID spin90
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 10
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 3
#define NUM_OBS 5
#define NUM_TREE 1
#endif



#define SPIN90             0 /* form/dialog */
#define LEFT               2 /* BOXTEXT in tree SPIN90 */
#define RIGHT              3 /* BOXTEXT in tree SPIN90 */
#define TWO                4 /* BOXTEXT in tree SPIN90 */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD spin90_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD spin90_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD spin90_rsc_free(void);
#endif
