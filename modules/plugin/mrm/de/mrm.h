/*
 * resource set indices for mrm
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        74
 * Number of Bitblks:        3
 * Number of Iconblks:       0
 * Number of Color Iconblks: 1
 * Number of Color Icons:    1
 * Number of Tedinfos:       24
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        39
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         3
 * Total file size:          5238
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "mrm"
#endif
#undef RSC_ID
#ifdef mrm
#define RSC_ID mrm
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 74
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 3
#define NUM_BB 3
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 1
#define NUM_TI 24
#define NUM_OBS 39
#define NUM_TREE 1
#endif



#define TREE001                            0 /* unknown form */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD mrm_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD mrm_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD mrm_rsc_free(void);
#endif
