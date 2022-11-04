/*
 * resource set indices for text
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        37
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       12
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        17
 * Number of Trees:          2
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          1254
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "text"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef text
#define RSC_ID text
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 37
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 12
#define NUM_OBS 17
#define NUM_TREE 2
#endif



#define TEXT_MAIN          0 /* form/dialog */
#define PREVIEW            1 /* BOX in tree TEXT_MAIN */
#define TEXT_OB            2 /* FTEXT in tree TEXT_MAIN */
#define TEXT_START         4 /* BUTTON in tree TEXT_MAIN */
#define ANTIALIAS          6 /* IBOX in tree TEXT_MAIN */
#define FONTSIZE           7 /* FTEXT in tree TEXT_MAIN */
#define FONTNAME          10 /* BOXTEXT in tree TEXT_MAIN */

#define ALERT_STRINGS      1 /* form/dialog */
#define NO_NVDI            1 /* TEXT in tree ALERT_STRINGS */
#define NO_FSEL            2 /* TEXT in tree ALERT_STRINGS */
#define ERROR_XFSL_FSEL    3 /* TEXT in tree ALERT_STRINGS */
#define ERROR_MAGIC_FSEL   4 /* TEXT in tree ALERT_STRINGS */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD text_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD text_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD text_rsc_free(void);
#endif
