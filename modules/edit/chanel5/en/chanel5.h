/*
 * resource set indices for chanel5
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        24
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       7
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        20
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          792
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "chanel5"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef chanel5
#define RSC_ID chanel5
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 24
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 7
#define NUM_OBS 20
#define NUM_TREE 1
#endif



#define CHANEL5            0 /* form/dialog */
#define START              3 /* BUTTON in tree CHANEL5 */
#define FORMAT_BOX         7 /* IBOX in tree CHANEL5 */
#define _RBG               9 /* IBOX in tree CHANEL5 */
#define _GRB              11 /* IBOX in tree CHANEL5 */
#define _GBR              13 /* IBOX in tree CHANEL5 */
#define _BRG              15 /* IBOX in tree CHANEL5 */
#define _BGR              17 /* IBOX in tree CHANEL5 */
#define MCONF_SAVE        18 /* BUTTON in tree CHANEL5 */
#define MCONF_LOAD        19 /* BUTTON in tree CHANEL5 */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD chanel5_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD chanel5_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD chanel5_rsc_free(void);
#endif
