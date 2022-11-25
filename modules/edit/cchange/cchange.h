/*
 * resource set indices for cchange
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        38
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       12
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        21
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          986
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "cchange"
#endif
#undef RSC_ID
#ifdef cchange
#define RSC_ID cchange
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 38
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 12
#define NUM_OBS 21
#define NUM_TREE 1
#endif



#define COL_CHANGE         0 /* form/dialog */
#define STARTIT            1 /* BUTTON in tree COL_CHANGE */
#define SRC                2 /* IBOX in tree COL_CHANGE */
#define DST                3 /* IBOX in tree COL_CHANGE */
#define DST_SMP            4 /* BOX in tree COL_CHANGE */
#define SRC_SMP            5 /* BOX in tree COL_CHANGE */
#define DRAHMEN            7 /* IBOX in tree COL_CHANGE */
#define EDIT_DR            8 /* FTEXT in tree COL_CHANGE */
#define EDIT_DG            9 /* FTEXT in tree COL_CHANGE */
#define EDIT_DB           10 /* FTEXT in tree COL_CHANGE */
#define SRAHMEN           14 /* IBOX in tree COL_CHANGE */
#define EDIT_SR           15 /* FTEXT in tree COL_CHANGE */
#define EDIT_SG           16 /* FTEXT in tree COL_CHANGE */
#define EDIT_SB           17 /* FTEXT in tree COL_CHANGE */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD cchange_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD cchange_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD cchange_rsc_free(void);
#endif
