/*
 * resource set indices for print
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        215
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 3
 * Number of Color Icons:    3
 * Number of Tedinfos:       67
 * Number of Free Strings:   10
 * Number of Free Images:    0
 * Number of Objects:        91
 * Number of Trees:          4
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          11106
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "print"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef print
#define RSC_ID print
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 215
#define NUM_FRSTR 10
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 3
#define NUM_TI 67
#define NUM_OBS 91
#define NUM_TREE 4
#endif



#define PRINT_MAIN         0 /* form/dialog */
#define DRIVER_POPBUT      4 /* BOXTEXT in tree PRINT_MAIN */
#define DRIVER_CB          5 /* IBOX in tree PRINT_MAIN */
#define PAPER_POPBUT       7 /* BOXTEXT in tree PRINT_MAIN */
#define PAPER_CB           8 /* IBOX in tree PRINT_MAIN */
#define PAPER_REDRAW      10 /* BOX in tree PRINT_MAIN */
#define POSITION_PAPER    11 /* BOX in tree PRINT_MAIN */
#define POSITION_FRAME    12 /* BOX in tree PRINT_MAIN */
#define POSITION_BOX      13 /* BOX in tree PRINT_MAIN */
#define SIZE_BOX          14 /* BOX in tree PRINT_MAIN */
#define PRINT_START       15 /* BUTTON in tree PRINT_MAIN */
#define FORMAT_HOCH       19 /* BOX in tree PRINT_MAIN */
#define FORMAT_QUER       21 /* BOX in tree PRINT_MAIN */
#define ADJUST_PARENT     23 /* IBOX in tree PRINT_MAIN */
#define X_DOWN            24 /* BOXTEXT in tree PRINT_MAIN */
#define X_UP              25 /* BOXTEXT in tree PRINT_MAIN */
#define XPOS              26 /* FTEXT in tree PRINT_MAIN */
#define Y_DOWN            28 /* BOXTEXT in tree PRINT_MAIN */
#define Y_UP              29 /* BOXTEXT in tree PRINT_MAIN */
#define YPOS              30 /* FTEXT in tree PRINT_MAIN */
#define WIDTH_DOWN        32 /* BOXTEXT in tree PRINT_MAIN */
#define WIDTH_UP          33 /* BOXTEXT in tree PRINT_MAIN */
#define WIDTH             34 /* FTEXT in tree PRINT_MAIN */
#define HEIGHT_DOWN       36 /* BOXTEXT in tree PRINT_MAIN */
#define HEIGHT_UP         37 /* BOXTEXT in tree PRINT_MAIN */
#define HEIGHT            38 /* FTEXT in tree PRINT_MAIN */
#define UNIT_POPBUT       42 /* BOXTEXT in tree PRINT_MAIN */
#define UNIT_CB           43 /* IBOX in tree PRINT_MAIN */

#define DRIVERS            1 /* form/dialog */
#define DRIVER1            1 /* BOXTEXT in tree DRIVERS */

#define PAPERS             2 /* form/dialog */
#define PAPER1             1 /* BOXTEXT in tree PAPERS */
#define PAPER2             2 /* BOXTEXT in tree PAPERS */
#define PAPER3             3 /* BOXTEXT in tree PAPERS */
#define PAPER4             4 /* BOXTEXT in tree PAPERS */
#define PAPER5             5 /* BOXTEXT in tree PAPERS */
#define PAPER6             6 /* BOXTEXT in tree PAPERS */
#define PAPER7             7 /* BOXTEXT in tree PAPERS */
#define PAPER8             8 /* BOXTEXT in tree PAPERS */
#define PAPER9             9 /* BOXTEXT in tree PAPERS */
#define PAPER10           10 /* BOXTEXT in tree PAPERS */

#define UNITS              3 /* form/dialog */
#define UNIT_PIXELS        1 /* BOXTEXT in tree UNITS */
#define UNIT_MM            2 /* BOXTEXT in tree UNITS */
#define UNIT_INCH          3 /* BOXTEXT in tree UNITS */
#define UNIT_PERCENT       4 /* BOXTEXT in tree UNITS */

#define NO_PIC             0 /* Free string */

#define NO_INIT            1 /* Free string */

#define PAPERFORMAT        2 /* Free string */

#define NO_COLOUR          3 /* Free string */

#define NO_EIGHT           4 /* Free string */

#define NO_PRT_MEM         5 /* Free string */

#define NO_OPEN            6 /* Free string */

#define NO_PREP_MEM        7 /* Free string */

#define NO_GDOS            8 /* Free string */

#define NO_DRIVER          9 /* Free string */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD print_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD print_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD print_rsc_free(void);
#endif
