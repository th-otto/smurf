/*
 * resource set indices for multi
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        147
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 1
 * Number of Color Icons:    1
 * Number of Tedinfos:       47
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        77
 * Number of Trees:          2
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          7216
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "multi"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef multi
#define RSC_ID multi
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 147
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 1
#define NUM_TI 47
#define NUM_OBS 77
#define NUM_TREE 2
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

#define ALERT_STRINGS      1 /* form/dialog */
#define NO_SOURCE          1 /* TEXT in tree ALERT_STRINGS */
#define NO_DEST            2 /* TEXT in tree ALERT_STRINGS */
#define NO_LOADMEM         3 /* TEXT in tree ALERT_STRINGS */
#define IMPORT_ERROR       4 /* TEXT in tree ALERT_STRINGS */
#define NO_PCD             5 /* TEXT in tree ALERT_STRINGS */
#define PCD_READ_ERROR     6 /* TEXT in tree ALERT_STRINGS */
#define NO_COM_STRUCT      7 /* TEXT in tree ALERT_STRINGS */
#define NO_BASEPAGE        8 /* TEXT in tree ALERT_STRINGS */
#define WRITE_ERROR        9 /* TEXT in tree ALERT_STRINGS */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD multi_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD multi_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD multi_rsc_free(void);
#endif
