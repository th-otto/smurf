#include "demolib.h"

#pragma warn -stv

int main(void)
{
	coor p1, p2;
	
	init_screens(0, 0, 0);
	exit_screens();
	set_screen(0);
	tc_line_adr(0, 0, 0, 0, 0, 0);
	tc_line(0, 0, 0, 0, 0, 0);
	p1.x = 0;
	p2.x = 0;
	between(p1, p2, 0, 0);
	rand_black();
	rand_restore();
	tc_screenshot(0, 0);

	ice_decrunch(0, 0);
	
	super_on();
	super_off();
	
	in_pl();
	dein_pl();
	startp(0);
	stopp();
	
	init_timer();
	wait_timer(0);
	get_timer();
	wait_timer_and_do(0, 0);
	
	return 0;
}


long setmode(int mode)
{
	return mode;
}


int mon_type(void)
{
	return 0;
}
