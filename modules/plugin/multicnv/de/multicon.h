/*
 * resource set indices for multicon
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        129
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 1
 * Number of Color Icons:    1
 * Number of Tedinfos:       38
 * Number of Free Strings:   9
 * Number of Free Images:    0
 * Number of Objects:        67
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          6734
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "multicon"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef multicon
#define RSC_ID multicon
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 129
#define NUM_FRSTR 9
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 1
#define NUM_TI 38
#define NUM_OBS 67
#define NUM_TREE 1
#endif



#define MULTICON           0 /* form/dialog */
#define GO                 1 /* BUTTON in tree MULTICON */
#define _BOX               6 /* BOX in tree MULTICON */
#define EXPLIST_PARENT     7 /* IBOX in tree MULTICON */
#define EXPLIST_1          8 /* TEXT in tree MULTICON */
#define EXPLIST_9         16 /* TEXT in tree MULTICON */
#define EXPLIST_UP        17 /* BOXCHAR in tree MULTICON */
#define EXPLIST_SB        18 /* BOX in tree MULTICON */
#define EXPLIST_SL        19 /* BOX in tree MULTICON */
#define EXPLIST_DN        20 /* BOXCHAR in tree MULTICON */
#define EXPORTER_OPTIONS  21 /* BUTTON in tree MULTICON */
#define SOURCE_PATH       22 /* BOXTEXT in tree MULTICON */
#define DEST_PATH         25 /* BOXTEXT in tree MULTICON */
#define WILDCARDS         30 /* FTEXT in tree MULTICON */
#define INFOICON          32 /* CICON in tree MULTICON */
#define DEPTH_PB          35 /* BOXTEXT in tree MULTICON */
#define DEPTH_CB          36 /* IBOX in tree MULTICON */
#define COLSYS_PB         39 /* BOXTEXT in tree MULTICON */
#define COLSYS_CB         40 /* IBOX in tree MULTICON */
#define DITHER_PB         43 /* BOXTEXT in tree MULTICON */
#define DITHER_CB         44 /* IBOX in tree MULTICON */
#define PALMODE_PB        47 /* BOXTEXT in tree MULTICON */
#define PALMODE_CB        48 /* IBOX in tree MULTICON */
#define KEEP_DEPTH        50 /* IBOX in tree MULTICON */
#define KEEP_COLSYS       52 /* IBOX in tree MULTICON */
#define LOADPAL           53 /* BUTTON in tree MULTICON */
#define PROC_BACK         64 /* BOX in tree MULTICON */
#define PROC_BAR          65 /* BOX in tree MULTICON */
#define PROC_TEXT         66 /* TEXT in tree MULTICON */

#define NO_SOURCE          0 /* Free string */

#define NO_DEST            1 /* Free string */

#define NO_LOADMEM         2 /* Free string */

#define IMPORT_ERROR       3 /* Free string */

#define NO_PCD             4 /* Free string */

#define PCD_READ_ERROR     5 /* Free string */

#define NO_COM_STRUCT      6 /* Free string */

#define NO_BASEPAGE        7 /* Free string */

#define WRITE_ERROR        8 /* Free string */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD multicon_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD multicon_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD multicon_rsc_free(void);
#endif
