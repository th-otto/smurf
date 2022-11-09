#ifndef __STDINT__H__
#define __STDINT__H__

/*
 * wrapper for stdint.h, for compilers that lack it
 */
#if defined(__PUREC__) && !defined(__int8_t_defined)

#define _STDINT_H

typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef short int               int16_t;
typedef unsigned short int      uint16_t;
typedef long int                int32_t;
typedef unsigned long int       uint32_t;

#else

#include <stdint.h>

#endif

#endif /* __STDINT__H__ */
