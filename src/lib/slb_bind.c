/*
*
* Binding for the use of a "shared library"
*
* Andreas Kromke
* 22.10.97
*
*/

#ifdef __PUREC__
#include <tos.h>
#include <slb.h>
#else
#include <osbind.h>
#include <mint/slb.h>
#endif

/*****************************************************************
*
* Opens a "shared lib".
*
* Input:
*     name             Name of the library, including extension
*     path             Search path with '\', optional
*     min_ver          Minimum required version number
* Returns:
*     sl               Library descriptor
*     fn               Function for calling a library function
*     <ret>            Actual version number, or error code
*
*****************************************************************/

#ifndef _MINT_SLB_H
LONG Slbopen( char *name, char *path, LONG min_ver, SLB_HANDLE *sl, SLB_EXEC *fn )

{
     return(gemdos(0x16, name, path, min_ver, sl, fn ));
}


/*****************************************************************
*
* Closes a "shared lib".
*
* Returns:
*     <ret>          EACCDN, if library not opened
*
*****************************************************************/

LONG Slbclose( SLB_HANDLE sl )
{
     return(gemdos(0x17, sl));
}
#endif
