/*
 * resource set indices for pac
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        11
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       3
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        10
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          426
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "pac"
#endif
#undef RSC_ID
#ifdef pac
#define RSC_ID pac
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 11
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 3
#define NUM_OBS 10
#define NUM_TREE 1
#endif



#define PAC_EXPORT         0 /* form/dialog */
#define OK                 1 /* BUTTON in tree PAC_EXPORT */
#define VERT               6 /* IBOX in tree PAC_EXPORT */
#define HOR                7 /* IBOX in tree PAC_EXPORT */
#define SAVE               9 /* BUTTON in tree PAC_EXPORT */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD pac_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD pac_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD pac_rsc_free(void);
#endif
