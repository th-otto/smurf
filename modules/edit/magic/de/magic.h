/*
 * resource set indices for magic
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        70
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       23
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        45
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          2062
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "magic"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef magic
#define RSC_ID magic
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 70
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 23
#define NUM_OBS 45
#define NUM_TREE 1
#endif



#define SM_MAGIC           0 /* form/dialog */
#define HOEHE_SW           1 /* FTEXT in tree SM_MAGIC */
#define RI10               8 /* IBOX in tree SM_MAGIC */
#define RI7               10 /* IBOX in tree SM_MAGIC */
#define RI8               12 /* IBOX in tree SM_MAGIC */
#define RI9               14 /* IBOX in tree SM_MAGIC */
#define RI6               16 /* IBOX in tree SM_MAGIC */
#define RI2               18 /* IBOX in tree SM_MAGIC */
#define RI4               20 /* IBOX in tree SM_MAGIC */
#define RI5               22 /* IBOX in tree SM_MAGIC */
#define RI1               24 /* IBOX in tree SM_MAGIC */
#define RI3               26 /* IBOX in tree SM_MAGIC */
#define GO                27 /* BUTTON in tree SM_MAGIC */
#define SHORTEN           28 /* IBOX in tree SM_MAGIC */
#define SPOSCH1           31 /* IBOX in tree SM_MAGIC */
#define SPOSCH2           34 /* IBOX in tree SM_MAGIC */
#define ANZSPALT          36 /* FTEXT in tree SM_MAGIC */
#define STRF2             37 /* IBOX in tree SM_MAGIC */
#define STRF1             40 /* IBOX in tree SM_MAGIC */
#define INTERPOL          42 /* IBOX in tree SM_MAGIC */
#define SPEICHER          44 /* IBOX in tree SM_MAGIC */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD magic_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD magic_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD magic_rsc_free(void);
#endif
