#include <string.h>
#include <stdlib.h>
#ifdef __PUREC__
#include <tos.h>
#if defined(_MINT_OSBIND_H)
#include <mint/falcon.h>
#endif
#else
#include <osbind.h>
#include <mintbind.h>
#endif
#include "demolib.h"

static long save_sp;

void super_on(void)
{
	save_sp = Super(NULL);
}


void super_off(void)
{
	Super((void *)save_sp);
}
