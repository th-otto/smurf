/*
 * resource set indices for trp_gfx
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        19
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       6
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        14
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          602
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "trp_gfx"
#endif
#undef RSC_ID
#ifdef trp_gfx
#define RSC_ID trp_gfx
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 19
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 6
#define NUM_OBS 14
#define NUM_TREE 1
#endif



#define THERAPY_GFX        0 /* form/dialog */
#define QUAL_FHR           1 /* BOX in tree THERAPY_GFX */
#define QUAL_SLID          2 /* BOX in tree THERAPY_GFX */
#define QUAL_TXT           3 /* BOXTEXT in tree THERAPY_GFX */
#define ROT_CB             8 /* IBOX in tree THERAPY_GFX */
#define GRUEN_CB           9 /* IBOX in tree THERAPY_GFX */
#define BLAU_CB           10 /* IBOX in tree THERAPY_GFX */
#define HUFF_CB           12 /* IBOX in tree THERAPY_GFX */
#define STARTIT           13 /* BUTTON in tree THERAPY_GFX */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD trp_gfx_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD trp_gfx_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD trp_gfx_rsc_free(void);
#endif
