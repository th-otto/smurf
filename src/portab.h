/*****************************************************************************/
/*                                                                           */
/* PORTAB.H                                                                  */
/*                                                                           */
/* Use of this file may make your code compatible with all C compilers       */
/* listed.                                                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* ENVIRONMENT                                                               */
/*****************************************************************************/

#ifndef __PORTAB__
#define __PORTAB__

#define M68000     1                          /* Motorola Processing Unit    */
#define I8086      0                          /* Intel Processing Unit       */

#define GEM1       0x0001                     /* ATARI GEM version           */
#define GEM2       0x0002                     /* MSDOS GEM 2.x versions      */
#define GEM3       0x0004                     /* MSDOS GEM/3 version         */
#define XGEM       0x0100                     /* OS/2,FlexOS X/GEM version   */

#ifndef GEM
#define GEM        GEM1                       /* GEMDOS default is GEM1      */
#endif /* GEM */

/*****************************************************************************/
/* STANDARD TYPE DEFINITIONS                                                 */
/*****************************************************************************/

#define BYTE    signed char                   /* Signed byte                 */
#define UBYTE   unsigned char                 /* Unsigned byte               */

#ifdef __PUREC__
#define WORD    int                           /* Signed word (16 bits)       */
#define UWORD   unsigned int                  /* Unsigned word               */
#else
#define WORD    short                         /* Signed word (16 bits)       */
#define UWORD   unsigned short                /* Unsigned word               */
#endif

#define LONG    long                          /* Signed long (32 bits)       */
#define ULONG   unsigned long                 /* Unsigned long               */

#define FLOAT   float                         /* Single precision float      */
#define DOUBLE  double                        /* Double precision float      */

#define INT     int                           /* A machine dependent int     */
#define UINT    unsigned int                  /* A machine dependent uint    */

#define REG     register                      /* Register variable           */
#define AUTO    auto                          /* Local to function           */
#define EXTERN  extern                        /* External variable           */
#define LOCAL   static                        /* Local to module             */
#define MLOCAL  LOCAL                         /* Local to module             */
#define GLOBAL                                /* Global variable             */

/*****************************************************************************/
/* COMPILER DEPENDENT DEFINITIONS                                            */
/*****************************************************************************/

#define ADR(A) (WORD)((LONG)A >> 16), (WORD)((LONG)A & 0xFFFF)

#define ANSI 1
#define _(params) params                      /* Parameter checking          */

#define CONST    const
#define VOLATILE volatile
#define CDECL    cdecl
#define PASCAL   pascal

#define SIZE_T   size_t

#ifndef VOID
#define VOID     void
#endif

/*****************************************************************************/
/* MISCELLANEOUS DEFINITIONS                                                 */
/*****************************************************************************/

#define FAILURE (-1)                          /* Function failure return val */
#define SUCCESS 0                             /* Function success return val */
#define FOREVER for (;;)                      /* Infinite loop declaration   */
#define EOS     '\0'                          /* End of string value         */

#ifndef EOF
#define EOF     (-1)                          /* EOF value                   */
#endif

#endif /* __PORTAB__ */

