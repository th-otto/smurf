/*
 * resource set indices for texis
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        156
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       50
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        80
 * Number of Trees:          4
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          3830
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "texis"
#endif
#undef RSC_ID
#ifdef texis
#define RSC_ID texis
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 156
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 50
#define NUM_OBS 80
#define NUM_TREE 4
#endif



#define TEX_MAIN           0 /* form/dialog */
#define LOADMAP            2 /* BUTTON in tree TEX_MAIN */
#define MAPNAME            3 /* TEXT in tree TEX_MAIN */
#define RS_PAR             6 /* BOX in tree TEX_MAIN */
#define RED_SLIDER         7 /* BOX in tree TEX_MAIN */
#define RS_A               8 /* BOXTEXT in tree TEX_MAIN */
#define GS_PAR             9 /* BOX in tree TEX_MAIN */
#define GREEN_SLIDER      10 /* BOX in tree TEX_MAIN */
#define GS_A              11 /* BOXTEXT in tree TEX_MAIN */
#define BS_PAR            12 /* BOX in tree TEX_MAIN */
#define BLUE_SLIDER       13 /* BOX in tree TEX_MAIN */
#define BS_A              14 /* BOXTEXT in tree TEX_MAIN */
#define LOADTEX           15 /* BUTTON in tree TEX_MAIN */
#define TEXNAME           16 /* TEXT in tree TEX_MAIN */
#define SAVETEX           19 /* BUTTON in tree TEX_MAIN */
#define STARTIT           20 /* BUTTON in tree TEX_MAIN */
#define TS_PAR            21 /* BOX in tree TEX_MAIN */
#define TURB_SLIDER       22 /* BOX in tree TEX_MAIN */
#define TS_A              23 /* BOXTEXT in tree TEX_MAIN */
#define YS_PAR            26 /* BOX in tree TEX_MAIN */
#define YS_SLIDER         27 /* BOX in tree TEX_MAIN */
#define YS_A              28 /* BOXTEXT in tree TEX_MAIN */
#define COL_UP            32 /* BOXCHAR in tree TEX_MAIN */
#define COL_DN            33 /* BOXCHAR in tree TEX_MAIN */
#define COL_NUMBER        34 /* TEXT in tree TEX_MAIN */
#define MODE_PB           37 /* BOXTEXT in tree TEX_MAIN */
#define MODE_CB           38 /* IBOX in tree TEX_MAIN */
#define LOAD_COLS         47 /* BUTTON in tree TEX_MAIN */
#define SAVE_COLS         48 /* BUTTON in tree TEX_MAIN */
#define TURB_PB           50 /* BOXTEXT in tree TEX_MAIN */
#define TURB_CB           51 /* IBOX in tree TEX_MAIN */
#define XS_PAR            54 /* BOX in tree TEX_MAIN */
#define XS_SLIDER         55 /* BOX in tree TEX_MAIN */
#define XS_A              56 /* BOXTEXT in tree TEX_MAIN */

#define TURB_POPUP         1 /* form/dialog */
#define TURB_NONE          1 /* BOXTEXT in tree TURB_POPUP */
#define TURB_ZOOM          2 /* BOXTEXT in tree TURB_POPUP */
#define TURB_SIN           3 /* BOXTEXT in tree TURB_POPUP */
#define TURB_ROTATE        4 /* BOXTEXT in tree TURB_POPUP */
#define TURB_X             5 /* BOXTEXT in tree TURB_POPUP */
#define TURB_Y             6 /* BOXTEXT in tree TURB_POPUP */

#define MAP_POPUP          2 /* form/dialog */
#define MAP_SHARP          1 /* BOXTEXT in tree MAP_POPUP */
#define MAP_LIN            2 /* BOXTEXT in tree MAP_POPUP */
#define MAP_SOFT           3 /* BOXTEXT in tree MAP_POPUP */
#define MAP_XSOFT          4 /* BOXTEXT in tree MAP_POPUP */

#define MODE_POPUP         3 /* form/dialog */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD texis_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD texis_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD texis_rsc_free(void);
#endif
