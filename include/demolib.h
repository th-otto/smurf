/* Standard Rezos */

#define TC_RGBrez 36
#define TC_VGArez 308 

#ifndef TC_W
#define TC_W 320l
#endif

#ifndef ASM_NAME
#ifdef __GNUC__
#  define ASM_NAME(x) __asm__(x)
#else
#  define ASM_NAME(x)
#endif
#endif

/* Typen */

typedef enum { False=0, True=!False } bool;
typedef enum { No_Mono, No_RGBTV_support, No_VGA_support, No_Memory } error;

typedef struct
{
	short x,y;
} coor;

/* Funktionen & globale Variablen */

/* ICE_UNPA */

void ice_decrunch(void *packed, void *pack_to) ASM_NAME("_ice_decrunch");

/* SCREENS */

extern int last_error;
extern unsigned short *screens[2];

#define tc_put_pixel(s,x,y,c) screens[s][(long)((long)(TC_W*(y))+(long)(x))]=c
#define tc_get_pixel(s,x,y) screens[s][(long)((long)(TC_W*(y))+(long)(x))]
#define tc_xor_pixel(s,x,y,c) screens[s][(long)((long)(TC_W*(y))+(long)(x))]^=c

void tc_line(int screen, int color, int x1, int y1, int x2, int y2);
void tc_line_adr(unsigned short *adr, int color, int x1, int y1, int x2, int y2);

void tc_screenshot(void *adr, int height);

#define tc_coor_line(s,c,p1,p2) tc_line(s,c,p1.x,p1.y,p2.x,p2.y)
#define tc_coor_line_adr(a,c,p1,p2) tc_line_adr(a,c,p1.x,p1.y,p2.x,p2.y)

coor between(coor p1, coor p2, short pos, short steps);

bool init_screens(bool doublebuffering, int modecode_rgb, int modecode_vga);
void exit_screens(void);
void set_screen(int nr);	/* supervisor-mode only */

void rand_black(void);		/* supervisor-mode only */
void rand_restore(void);	/* supervisor-mode only */

/* SUPRMODE */

void super_on(void);
void super_off(void);

/* PLAYER */

void in_pl(void) ASM_NAME("_in_pl");
void dein_pl(void) ASM_NAME("_dein_pl");
int startp(void *mod) ASM_NAME("_startp");
void stopp(void) ASM_NAME("stopp");

/* TIME */

void init_timer(void);
void wait_timer(long clicks);
long get_timer(void);
void wait_timer_and_do(long clicks, void (*func)(void));
