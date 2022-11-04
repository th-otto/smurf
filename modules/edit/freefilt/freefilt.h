/*
 * resource set indices for freefilt
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        99
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       32
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        54
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          2660
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "freefilt"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef freefilt
#define RSC_ID freefilt
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 99
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 32
#define NUM_OBS 54
#define NUM_TREE 1
#endif



#define FREE_FILTER        0 /* form/dialog */
#define FILTER_NAME        1 /* FTEXT in tree FREE_FILTER */
#define MATRIX_00          9 /* FTEXT in tree FREE_FILTER */
#define MATRIX_01         10 /* FTEXT in tree FREE_FILTER */
#define MATRIX_02         11 /* FTEXT in tree FREE_FILTER */
#define MATRIX_03         12 /* FTEXT in tree FREE_FILTER */
#define MATRIX_04         13 /* FTEXT in tree FREE_FILTER */
#define MATRIX_10         15 /* FTEXT in tree FREE_FILTER */
#define MATRIX_11         16 /* FTEXT in tree FREE_FILTER */
#define MATRIX_12         17 /* FTEXT in tree FREE_FILTER */
#define MATRIX_13         18 /* FTEXT in tree FREE_FILTER */
#define MATRIX_14         19 /* FTEXT in tree FREE_FILTER */
#define MATRIX_20         21 /* FTEXT in tree FREE_FILTER */
#define MATRIX_21         22 /* FTEXT in tree FREE_FILTER */
#define MATRIX_22         23 /* FTEXT in tree FREE_FILTER */
#define MATRIX_23         24 /* FTEXT in tree FREE_FILTER */
#define MATRIX_24         25 /* FTEXT in tree FREE_FILTER */
#define MATRIX_30         27 /* FTEXT in tree FREE_FILTER */
#define MATRIX_31         28 /* FTEXT in tree FREE_FILTER */
#define MATRIX_32         29 /* FTEXT in tree FREE_FILTER */
#define MATRIX_33         30 /* FTEXT in tree FREE_FILTER */
#define MATRIX_34         31 /* FTEXT in tree FREE_FILTER */
#define MATRIX_40         33 /* FTEXT in tree FREE_FILTER */
#define MATRIX_41         34 /* FTEXT in tree FREE_FILTER */
#define MATRIX_42         35 /* FTEXT in tree FREE_FILTER */
#define MATRIX_43         36 /* FTEXT in tree FREE_FILTER */
#define MATRIX_44         37 /* FTEXT in tree FREE_FILTER */
#define DO_IT             38 /* BUTTON in tree FREE_FILTER */
#define DIV               39 /* FTEXT in tree FREE_FILTER */
#define BIAS              40 /* FTEXT in tree FREE_FILTER */
#define LOAD_FILTER       41 /* BUTTON in tree FREE_FILTER */
#define SAVE_FILTER       42 /* BUTTON in tree FREE_FILTER */
#define CLIP              45 /* IBOX in tree FREE_FILTER */
#define INVERT            46 /* IBOX in tree FREE_FILTER */
#define STRENGTH_FHR      48 /* BOX in tree FREE_FILTER */
#define STRENGTH_SLIDE    49 /* BOX in tree FREE_FILTER */
#define STRENGTH_EDIT     53 /* TEXT in tree FREE_FILTER */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD freefilt_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD freefilt_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD freefilt_rsc_free(void);
#endif
