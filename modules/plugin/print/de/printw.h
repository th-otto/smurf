/*
 * resource set indices for printw
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        12
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       4
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        5
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          450
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "printw"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef printw
#define RSC_ID printw
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 12
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 4
#define NUM_OBS 5
#define NUM_TREE 1
#endif



#define ALERT_STRINGS      0 /* form/dialog */
#define NO_PIC             1 /* TEXT in tree ALERT_STRINGS */
#define NO_OPEN            2 /* TEXT in tree ALERT_STRINGS */
#define NO_PRT_MEM         3 /* TEXT in tree ALERT_STRINGS */
#define NO_PREP_MEM        4 /* TEXT in tree ALERT_STRINGS */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD printw_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD printw_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD printw_rsc_free(void);
#endif
