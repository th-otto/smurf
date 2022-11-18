/* Funktionsprototypen */

#ifdef __PUREC__
#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)
void vq_scrninfo(WORD handle, WORD *work_out);
int vq_ext_devinfo(WORD handle, WORD device, WORD *dev_exists, char *file_path, char *file_name, char *name);
void v_clsbm(WORD handle);
void v_opnbm(WORD *work_in, MFDB *bitmap, WORD *handle, WORD *work_out);
#endif
#else
#include <gemx.h>
#endif

#ifndef _MINT_MINTBIND_H
#define Ssystem(mode, arg1, arg2)   gemdos(0x154, (int)mode, (long)arg1, (long)arg2)
#endif

/* Konstanten fÅr Ssystem() */
# define OSNAME     	0
# define OSXNAME  		1
# define OSVERSION    	2
# define TOSHEADER    	3
# define OS_BUILD_DATE  4
# define OS_BUILD_TIME  5
# define COMPILE_TYPE 	6

#ifndef FEATURES
# define FEATURES 		7
#endif

#ifndef GETCOOKIE
# define GETCOOKIE    	8
#endif

# define SETCOOKIE    	9
# define GET_LVAL 		10
# define GET_WVAL    	11
# define GET_BVAL    	12
# define SET_LVAL		13
# define SET_WVAL    	14
# define SET_BVAL    	15
# define SYS_SLEVEL  	16
# define RUN_LEVEL      17  /* currently disabled, reserved */
# define TSLICE		    18
# define FORCEFASTLOAD  19
# define SYNC_TIME   	20
# define RESERVED_21 	21  /* reserved, use Dcntl instead */
# define CLOCK_UTC    	100

# define FEAT_MP   		1   /* memory protection enabled */
# define FEAT_VM    	2   /* virtual memory manager enabled */
# define GCJARSIZE 0

void v_ext_opnwk(WORD *work_in, WORD *handle, WORD *work_out);
WORD SM_wind_set(WORD wi_ghandle, WORD wi_gfield, WORD wi_gw1, WORD wi_gw2, WORD wi_gw3, WORD wi_gw4);
WORD SM_wind_get(WORD wi_ghandle, WORD wi_gfield, WORD *wi_gw1, WORD *wi_gw2, WORD *wi_gw3, WORD *wi_gw4);
