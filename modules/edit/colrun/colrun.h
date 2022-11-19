/*
 * resource set indices for colrun
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
 * Number of Objects:        40
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          1450
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "colrun"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef colrun
#define RSC_ID colrun
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
#define NUM_OBS 40
#define NUM_TREE 1
#endif



#define COLRUN_MAIN                        0 /* form/dialog */
#define COLRUN_LINEAR                      5 /* IBOX in tree COLRUN_MAIN */
#define COLRUN_RADIAL                      6 /* IBOX in tree COLRUN_MAIN */
#define M_RED_FHR                          8 /* BOX in tree COLRUN_MAIN */
#define M_RED_SLIDE                        9 /* BOX in tree COLRUN_MAIN */
#define M_RED_EDIT                        10 /* BOXTEXT in tree COLRUN_MAIN */
#define M_BLUE_FHR                        12 /* BOX in tree COLRUN_MAIN */
#define M_BLUE_SLIDE                      13 /* BOX in tree COLRUN_MAIN */
#define M_BLUE_EDIT                       14 /* BOXTEXT in tree COLRUN_MAIN */
#define M_GREEN_FHR                       16 /* BOX in tree COLRUN_MAIN */
#define M_GREEN_SLIDE                     17 /* BOX in tree COLRUN_MAIN */
#define M_GREEN_EDIT                      18 /* BOXTEXT in tree COLRUN_MAIN */
#define START                             19 /* BUTTON in tree COLRUN_MAIN */
#define PREVIEW                           20 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_BOX                     21 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_FIRST                   22 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_LAST                    23 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_00                      24 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_01                      25 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_02                      26 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_03                      27 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_04                      28 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_05                      29 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_06                      30 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_07                      31 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_08                      32 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_09                      33 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_10                      34 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_11                      35 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_12                      36 /* BOX in tree COLRUN_MAIN */
#define DATAPOINT_13                      37 /* BOX in tree COLRUN_MAIN */
#define COLRUN_DIR                        39 /* FTEXT in tree COLRUN_MAIN */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD colrun_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD colrun_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD colrun_rsc_free(void);
#endif
