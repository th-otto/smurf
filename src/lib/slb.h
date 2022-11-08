/*
*
* Binding for the use of "shared libraries"
*
* Andreas Kromke
* 22.10.97
* Last change 19.2.99  - SLB_EXEC with cdecl corrected
*
*/

#ifndef LONG
#include <portab.h>
#endif

#ifndef _MINT_SLB_H
typedef void *SLB_HANDLE;

typedef LONG cdecl (*SLB_EXEC)( SLB_HANDLE sl, LONG fn, WORD nargs, ... );

extern LONG Slbopen( char *name, char *path, LONG min_ver, SLB_HANDLE *sl, SLB_EXEC *fn );
extern LONG Slbclose( SLB_HANDLE sl );
#endif
