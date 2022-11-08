#include <string.h>
#include <stdlib.h>
#include <time.h>
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

static clock_t startclock;


void init_timer(void)
{
	startclock = clock();
}


void wait_timer(long clicks)
{
	while (clock() - startclock < clicks)
		;
}


long get_timer(void)
{
	return clock() - startclock;
}


void wait_timer_and_do(long clicks, void (*func)(void))
{
	while (clock() - startclock < clicks)
		func();
}
