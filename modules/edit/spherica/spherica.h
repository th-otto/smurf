/*
 * resource set indices for spherica
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        189
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       62
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        111
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          5134
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "spherica"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef spherica
#define RSC_ID spherica
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 189
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 62
#define NUM_OBS 111
#define NUM_TREE 1
#endif



#define SPH_MAIN           0 /* form/dialog */
#define _TIT               2 /* TEXT in tree SPH_MAIN */
#define KR_F               3 /* BOX in tree SPH_MAIN */
#define KR_R               4 /* BOX in tree SPH_MAIN */
#define KR_A               5 /* BOXTEXT in tree SPH_MAIN */
#define KG_F               7 /* BOX in tree SPH_MAIN */
#define KG_R               8 /* BOX in tree SPH_MAIN */
#define KG_A               9 /* BOXTEXT in tree SPH_MAIN */
#define KB_F              11 /* BOX in tree SPH_MAIN */
#define KB_R              12 /* BOX in tree SPH_MAIN */
#define KB_A              13 /* BOXTEXT in tree SPH_MAIN */
#define TEXTURE_BOX       14 /* BOX in tree SPH_MAIN */
#define TEXT_A            16 /* BOXTEXT in tree SPH_MAIN */
#define TEXT_F            17 /* BOX in tree SPH_MAIN */
#define TEXT_R            18 /* BOX in tree SPH_MAIN */
#define TEX_XSC           23 /* FTEXT in tree SPH_MAIN */
#define TEX_YSC           24 /* FTEXT in tree SPH_MAIN */
#define TEX_XOFF          25 /* FTEXT in tree SPH_MAIN */
#define TEX_YOFF          26 /* FTEXT in tree SPH_MAIN */
#define ROUGH_BOX         29 /* BOX in tree SPH_MAIN */
#define ROUGH_A           31 /* BOXTEXT in tree SPH_MAIN */
#define ROUGH_F           32 /* BOX in tree SPH_MAIN */
#define ROUGH_R           33 /* BOX in tree SPH_MAIN */
#define TRANSP_BOX        34 /* BOX in tree SPH_MAIN */
#define TRANSP_A          36 /* BOXTEXT in tree SPH_MAIN */
#define TRANSP_F          37 /* BOX in tree SPH_MAIN */
#define TRANSP_R          38 /* BOX in tree SPH_MAIN */
#define TRAN_IDX          39 /* FTEXT in tree SPH_MAIN */
#define BUMP_BOX          41 /* BOX in tree SPH_MAIN */
#define BUMP_A            42 /* BOXTEXT in tree SPH_MAIN */
#define BUMP_F            43 /* BOX in tree SPH_MAIN */
#define BUMP_R            44 /* BOX in tree SPH_MAIN */
#define BUMP_XSC          49 /* FTEXT in tree SPH_MAIN */
#define BUMP_YSC          50 /* FTEXT in tree SPH_MAIN */
#define BUMP_XOFF         51 /* FTEXT in tree SPH_MAIN */
#define BUMP_YOFF         52 /* FTEXT in tree SPH_MAIN */
#define BUMP_PERSP        56 /* IBOX in tree SPH_MAIN */
#define XRAD              59 /* FTEXT in tree SPH_MAIN */
#define LOS               63 /* BUTTON in tree SPH_MAIN */
#define PREV_BOX          65 /* BOX in tree SPH_MAIN */
#define CONF_LOAD         66 /* BUTTON in tree SPH_MAIN */
#define CONF_SAVE         67 /* BUTTON in tree SPH_MAIN */
#define BUMP_RADIO        70 /* IBOX in tree SPH_MAIN */
#define TEXT_RADIO        74 /* IBOX in tree SPH_MAIN */
#define ROUGH_RADIO       75 /* IBOX in tree SPH_MAIN */
#define TRANSP_RADIO      76 /* IBOX in tree SPH_MAIN */
#define BUMP_CHECK        77 /* IBOX in tree SPH_MAIN */
#define TEXT_CHECK        78 /* IBOX in tree SPH_MAIN */
#define ROUGH_CHECK       79 /* IBOX in tree SPH_MAIN */
#define TRANS_CHECK       80 /* IBOX in tree SPH_MAIN */
#define AR_F              84 /* BOX in tree SPH_MAIN */
#define AR_R              85 /* BOX in tree SPH_MAIN */
#define AR_A              86 /* BOXTEXT in tree SPH_MAIN */
#define AG_F              88 /* BOX in tree SPH_MAIN */
#define AG_R              89 /* BOX in tree SPH_MAIN */
#define AG_A              90 /* BOXTEXT in tree SPH_MAIN */
#define AB_F              92 /* BOX in tree SPH_MAIN */
#define AB_R              93 /* BOX in tree SPH_MAIN */
#define AB_A              94 /* BOXTEXT in tree SPH_MAIN */
#define SSTR_F            97 /* BOX in tree SPH_MAIN */
#define SSTR_R            98 /* BOX in tree SPH_MAIN */
#define SSTR_A            99 /* BOXTEXT in tree SPH_MAIN */
#define SPOW_F           101 /* BOX in tree SPH_MAIN */
#define SPOW_R           102 /* BOX in tree SPH_MAIN */
#define SPOW_A           103 /* BOXTEXT in tree SPH_MAIN */
#define SFLT_F           105 /* BOX in tree SPH_MAIN */
#define SFLT_R           106 /* BOX in tree SPH_MAIN */
#define SFLT_A           107 /* BOXTEXT in tree SPH_MAIN */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD spherica_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD spherica_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD spherica_rsc_free(void);
#endif
