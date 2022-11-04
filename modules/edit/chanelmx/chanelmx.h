/*
 * resource set indices for chanelmx
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        33
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       10
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        27
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          1072
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "chanelmx"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef chanelmx
#define RSC_ID chanelmx
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 33
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 10
#define NUM_OBS 27
#define NUM_TREE 1
#endif



#define CHANELMIXER        0 /* form/dialog */
#define START              3 /* BUTTON in tree CHANELMIXER */
#define S_F                6 /* BOX in tree CHANELMIXER */
#define S_S                7 /* BOX in tree CHANELMIXER */
#define S_E                8 /* BOXTEXT in tree CHANELMIXER */
#define DEST_BOX          10 /* IBOX in tree CHANELMIXER */
#define ZR                14 /* IBOX in tree CHANELMIXER */
#define ZG                15 /* IBOX in tree CHANELMIXER */
#define ZB                16 /* IBOX in tree CHANELMIXER */
#define SRC_BOX           18 /* IBOX in tree CHANELMIXER */
#define QB                22 /* IBOX in tree CHANELMIXER */
#define QG                23 /* IBOX in tree CHANELMIXER */
#define QR                24 /* IBOX in tree CHANELMIXER */
#define SAVE              25 /* BUTTON in tree CHANELMIXER */
#define LOAD              26 /* BUTTON in tree CHANELMIXER */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD chanelmx_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD chanelmx_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD chanelmx_rsc_free(void);
#endif
