#ifndef _smurf_f_h_
#define _smurf_f_h_

/*
 * ***** BEGIN LICENSE BLOCK *****
 *
 * The contents of this file are subject to the GNU General Public License
 * Version 2 (the "License"); you may not use this file except in compliance
 * with the GPL. You may obtain a copy of the License at
 * http://www.gnu.org/copyleft/gpl.html or the file GPL.TXT from the program
 * or source code package.
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
 * the specific language governing rights and limitations under the GPL.
 *
 * The Original Code is Therapy Seriouz Software code.
 *
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include "portab.h"

#define MAX_PIC 25
#define MAX_MODS 21

#if !defined(__GNUC__) && !defined(__attribute__)
#  define __attribute__(x)
#endif

/* **********************************************************************/
/* ----------------------- Fileroutinen ------------------------------- */
/* Alles, was mit Dateizugriffen, Bild laden, etc zu tun hat            */
/* **********************************************************************/

/* Dateihandling mit Fileselector */
void file_load(char *ltext, char **dateien, int mode);
int file_save(char *stext, char *buf, long length);
int f_fsbox(char *Path, char *fbtext, char selectart);

/*  File laden */
char *fload(char *Path, int header);

/* Bild importieren */
void reload_pic(WINDOW *picwindow);
int f_import_pic(SMURF_PIC *smurf_picture, char *filename);
char *f_do_pcd(char *Path);


/*
 * config.c
 */
int load_config(void);
void save_config(void);
void f_analyze_system(void);
void GetSMPath(void);







/* **********************************************************************/
/* ----------------------- Event- und Messagefunktionen --------------- */
/* How to speak AES - Dialog mit dem Betriebssystem u.Ñ.                */
/* **********************************************************************/
void f_event(void);              /* Allg. Event-Funktion                 */
int  f_handle_message(void);     /* AES-Message-Handler                  */

void check_windclose(int windnum);   /* Auswerten von Windowdialog-AbbrÅchen */




/* **********************************************************************/
/* --------------------- Ditherroutinen u.Ñ. -------------------------- */
/* alles zur Bilddarstellung.                                           */
/* **********************************************************************/

extern int bplanes;
extern int dithermode;

/*---- Dither-Routinen - Maindispatcher ----*/
int f_dither(SMURF_PIC *dest, SYSTEM_INFO *sys_info, int pic_changed, GRECT *part, DISPLAY_MODES *displayOpt);

/*---- Histogrammberechnung + Median Cut ----*/
    long *make_histogram(SMURF_PIC *picture);
void median_cut(long *histogram, SMURF_PIC *picture, SYSTEM_INFO *sysinfo);

/*---- Dit-Routs C Subdispatcher ----*/
void f_dither_24_1(SYSTEM_INFO *sysinfo, SMURF_PIC *picture, char *where_to, int mode, char *nct, GRECT *part);
void f_transform_to_standard(SMURF_PIC *picture,char *paddr);
void f_align_standard(SMURF_PIC *picture,char *paddr, GRECT *part);

/* Anpassen der Palette */
char *fix_palette(SMURF_PIC *dest, SYSTEM_INFO *sys_info);

/* Nearest-Color-Suchfunktion (Assembler) */
int seek_nearest_col(long *par, int maxcol) ASM_NAME("_seek_nearest_col");

/* Routinen fÅr 16 und 24Bit-Bildschirmmodus */
void direct2screen(SMURF_PIC *picture, char *where_to, GRECT *part);

int export_dither_dispatcher(SMURF_PIC *dest, SYSTEM_INFO *sys_info, DISPLAY_MODES *display, int *fixpal_red, int *fixpal_green, int *fixpal_blue);
void make_tmp_nct(long *histogram, SMURF_PIC *pic, unsigned int maxc);
void makeNCT(long *par, long maxcol) ASM_NAME("_makeNCT");



/*--------------------- Transformers -----------------------*/
int f_convert(SMURF_PIC *picture, MOD_ABILITY *mod_abs, char modcolform, char mode, char automatic);
int tfm_std_to_std(SMURF_PIC *smurf_pic, char dst_depth, char mode);
int tfm_std_to_pp(SMURF_PIC *picture, char dst_depth, char mode);
int tfm_pp_to_std8(SMURF_PIC *picture, char mode);
int tfm_8_to_16(SMURF_PIC *picture, char mode);
int tfm_8_to_24(SMURF_PIC *picture, char mode);
int tfm_16_to_24(SMURF_PIC *picture, char mode);
int tfm_24_to_16(SMURF_PIC *picture, char mode);
int tfm_bgr_to_rgb(SMURF_PIC *picture, char mode);
int tfm_cmy_to_rgb(SMURF_PIC *picture, char mode);
int tfm_rgb_to_grey(SMURF_PIC *picture, char mode);


void transform_pic(void);        /* "Bild Wandeln" - Dialog */

/* **********************************************************************/
/* --------------------------- Bildanpassung -------------------------- */
/* CMYK nach YCBCR oder RGB, Intel nach Motorola, usw.                  */
/* **********************************************************************/
SMURF_PIC   *f_generate_newpic(int wid, int hgt, int depth);


/* **********************************************************************/
/* ------------------------ SMURF-Systemfunktionen -------------------- */
/* Was, Bin ich, wo bin ich, wieviele Farben hab' ich?                  */
/* **********************************************************************/
void f_scan_edit(void);                  /* Edit Modules scannen */
void f_scan_import(void);                                    /* Importmodule scannen */
int load_import_list(void);                                 /* Extensionsliste laden */
int seek_module(SMURF_PIC *picture, char *extension);       /* Modul aus Extensionsliste suchen */
void f_scan_export(void);                /* Edit Modules scannen */
void f_scan_dither(void);                /* Dither Modules scannen */
struct DIRENTRY *build_up_filelist(char *path, char *ext, int pathlen);
struct DIRENTRY *insert_entry(struct DIRENTRY *ende, char *string);
void destroy_filelist(struct DIRENTRY *begin);

int     f_init_system(void);                /* Systeminitialisierung - die Mutterfunktion */
void    f_init_palette(void);               /* Palette auslesen     */
char    *f_init_table(void);                /* NC-Tabelle init      */
void    f_init_bintable(OBJECT *rsc);       /* BinÑrwert-Table init */



int handle_keyboardevent(WINDOW *wind_s, int scancode, int *sel_object);
void change_object(WINDOW *window, int objct, int status, int redraw);


/*
 * resource.c
 */
extern int add_flags[40];

void f_init_popups(void);        /* Alle Popups initialisieren */
void f_init_sliders(void);   
void init_smurfrsc(char *rscpath);
void fix_rsc(void);              /* RSC-Anpassung an OS, Farbtiefe, AES, etc... */


/*
 * shortc2.c
 */
char set_menu_key(OBJECT *menu);
char get_menu_key(OBJECT *menu, KINFO *ki, int *title, int *item);
int scan_2_ascii(int scan, int state);




/*
 * menu.c
 */
void f_init_menu(void);          /* MenÅzeile her! */
void f_exit_menu(void);          /* MenÅzeile weg! */
int f_handle_menuevent(int *message);
void actualize_menu(void);       /* MenÅzeile aktualisieren */
void lock_Smurf(void);
void unlock_Smurf(void);




int blend_demopic(SMURF_PIC *picture);



void cursor_off(WINDOW *window);
void cursor_on(WINDOW *window);

void free_preview(void);

void make_thumbnail(SMURF_PIC *original_pic, SMURF_PIC *thumbnail, int dither);

void mr_message(void);
void protocol_message(int message, int id, char *modname);

int init_roto(void);
void roto(void);
void deinit_roto(void);

void gdps_main(void);


/* **********************************************************************/
/* ------------------- SMURF-Kommunikationsfunktionen ----------------- */
/* Bitte reden Sie mit mir, ich bin so alleine.                         */
/* **********************************************************************/
extern char *send_smurfid;

int get_dragdrop(WINDOW *window_to_handle, int *messagebuf);
int send_dragdrop(SMURF_PIC *picture, int dest_whandle, int mx, int my);
int dd_getheader(DD_HEADER *dd_header, int pipe_handle);
int dd_sendheader(int pipe_handle, long ext, long size, char *data_name, char *file_name);
void send_AESMessage(int dest_id, int msg, ...);                /* AES-Message an anderen Prozess schicken */
void update_dhst(char *path);
void bubble_init(void);
void bubble_exit(void);
void bubble_gem(int windownum, int xpos, int ypos, int modulemode);
void init_AVPROTO(void);
void get_avserv(void);
void call_stguide(int topwin_handle);



/* **********************************************************************/
/* ---------------------------- Modulfunktionen ----------------------- */
/* Some of them want to use you, some of them want to get used by you...*/
/* **********************************************************************/
int handle_modevent(int event_type, WINDOW *mod_window);    /* Handling eines Events im Modulfenster */
void f_handle_modmessage(GARGAMEL *smurf_struct);
int analyze_message(int module_ret, int picture_to_load);   /* Analyse eines Modul-Returns */
int f_open_module_window(WINDOW *module_window);        /* Modulfenster îffnen */
int give_free_module(void);     /* Ermittelt Strukturindex fÅr freies Modul und gibt diesen zurÅck */
void check_and_terminate(int mode, int module_number);
void walk_module_tree(WINDOW *wind, int start);
void init_modtree(OBJECT *tree, int index);
void convert_icon(OBJECT *tree, int index);
SMURF_PIC *get_pic(int num, int mod_id, MOD_INFO *mod_info, int depth, int form, int col);
int f_give_pics(MOD_INFO *mod_info, MOD_ABILITY *mod_abs, int module_number);       /* mehrere Bilder ans Modul Åbergeben */
int inform_modules(int message, SMURF_PIC *picture);    /* Informiert alle Module und Plugins Åber message */
long get_proclen(BASPAG *baspag);
long get_modmagic(BASPAG *basepage);
void AESmsg_to_modules(int *msgbuf);
void make_modpreview(WINDOW *wind);


/*
 * prefdial.c
 */
extern int prev_zoom;
extern SMURF_PIC move_prev;

void f_module_prefs(MOD_INFO *infostruct, int mod_id);   /* Modulformular aufrufen */
void f_mpref_change(void);                               /* Eingabe im Modulformular */
void f_move_preview(WINDOW *window, SMURF_PIC *orig_pic, int redraw_object);
void copy_preview(SMURF_PIC *source_pic, SMURF_PIC *module_preview, WINDOW *prev_window);


/*
 * modconf.c
 */
extern void *edit_cnfblock[100];
extern OBJECT *confsave_dialog;
extern OBJECT *modconf_popup;

void *mconfLoad(MOD_INFO *modinfo, int mod_id, char *name);
void mconfSave(MOD_INFO *modinfo, int mod_id, void *confblock, long len, char *name);
void *load_from_modconf(MOD_INFO *modinfo, char *name, int *num, long type);
void memorize_emodConfig(BASPAG *modbase, GARGAMEL *smurf_struct);
void memorize_expmodConfig(BASPAG *modbase, GARGAMEL *smurf_struct, char save);
void transmitConfig(BASPAG *modbase, GARGAMEL *smurf_struct);


/*
 * undo.c
 */
void saveUndoBuffer(SMURF_PIC *picture, int picnum);
void swapUndoBuffer(SMURF_PIC *toPicture, int picNum);



/* **********************************************************************/
/* --------------------------- Windowfunktionen ----------------------- */
/* WINDOWSø sucks!                                                      */
/* **********************************************************************/
int f_open_window(WINDOW *wind);                        /* Fenster îffnen */
void f_redraw_window(WINDOW *window, GRECT *mwind, int startob, int flags);      /* Fenster Redraw */
void draw_iconified(WINDOW *window, int *vdiclip);
void draw_block(WINDOW *window, GRECT *picbox);
void insert_picwinzoom(WINDOW *window);
void f_draw_crosshair(WINDOW *window);       /* Positionierungskreuz in einem Bild zeichnen */
void f_draw_blockbox(WINDOW *window);
void draw_picmanboxes(void);
void clip_picw2screen(WINDOW *picw);
int f_rc_intersect( GRECT *r1, GRECT *r2, GRECT *r3);     /* Zum Redraw. */
int my_window(int handle);                              /* Fenster identifizieren */
WINDOW *my_module_window(int handle);                      /* Modulfenster identif. */
void f_setsliders(WINDOW *wind);                         /* Slider setzen */
void f_arrow_window(int mode, WINDOW *window, int amount);   /* Pfeilbedienung im Bildfenster */
void scrollWindowRT(WINDOW *window, int xamount, int yamount);
void f_slide_window(int pos, WINDOW *wind, int mode);    /* Bildfenster sliden */
void toggle_asterisk(WINDOW *picwin, int onoff);
void window_to_list(WINDOW *window);        /* Fenster in die Liste hÑngen */
void remove_window(WINDOW *window);
int find_crosshair(WINDOW *window);         /* findet eine Fadenkreuzstruktur zu einem Bildfenster */
void top_window(int handle);
void top_window_now(WINDOW *window);
void top_windowhandle(int handle);
void close_window(int handle);
int f_alert(char *alertstring, char *b1, char *b2, char *b3, int defbt);   /* Fenster-Alertfunktion */
void close_alert(void);
void f_pic_changed(WINDOW *window, int onoff);


/*
 * lib/radiochk.c
 */
extern USERBLK cycle_user;

void f_treewalk(OBJECT *tree,int start);                 /* Userdefined Objects eintragen */
int	cdecl f_do_checkbox(PARMBLK *parm);
int	cdecl f_do_cycle(PARMBLK *parm);


/* **********************************************************************/
/*                              Blockfunktionen                         */
/*  Alles was mit den Blîcken zu tun hat, oder:                         */
/*      "Um den nÑchsten Block und dann links abbiegen..."              */
/* **********************************************************************/

void block_freistellen(WINDOW *pwindow);
void block_type_in(void);
void blockmode(void);
void blockfunctions_off(void);
void blockfunctions_on(void);
void block_over_all(WINDOW *window);
void insert_blockcoords(SMURF_PIC *pic);

int encode_block(SMURF_PIC *picture, EXPORT_PIC **pic_to_save);
int save_block(EXPORT_PIC *pic_to_save, char *path);

int block2clip(SMURF_PIC *picture, int mode, char *path);
void clip2block(SMURF_PIC *picture, char *data, int mx, int my);

SMURF_PIC *previewBlock(SMURF_PIC *picture, GRECT *blockpart);
void *copyblock(SMURF_PIC *old_pic);
int insert_block(WINDOW *picwindow);
void block_dklick(WINDOW *picwindow);

/* **********************************************************************/
/* ------------------------------ Statusbox --------------------------- */
/* "Picard an Maschinenraum! Statusbericht!"                            */
/* **********************************************************************/
void reset_busybox(int lft,  char *txt);     /* Busybox setzen und Text einfÅgen */
void ok_busybox(void);                       /* Busybox auf 128/"OK" setzen */
int draw_busybox(int lft);                  /* Busybox setzen */
void actualize_ram(void);                    /* RAM anzeigen */
void disable_busybox(void);
void enable_busybox(void);
void fulldisable_busybox(void);


/* **********************************************************************/
/* ------------------------- Bedienungsfunktionen --------------------- */
/* Alles, was dem Benutzer das Leben einfach und dem Programmierer      */
/*  schwer macht...                                                     */
/* **********************************************************************/

/*----- Smurf-Systemspezifisches ----*/
/*----- Smurf-Systemspezifisches ----*/
extern DISPLAY_MODES Display_Opt;
extern char *edit_modules[100];			/* Pfade fÅr bis zu 100 Edit-Module */
extern SERVICE_FUNCTIONS global_services;

extern WORD resource_global[100];
extern int nullcoordset;
extern int syspalset;
extern int gl_hchar, gl_wchar, gl_hbox, gl_wbox;
extern int picwindthere, dialwindthere, picthere;
extern int openmode;					/* Dialog neu geîffnet (0) oder buttonevent? (!=0) */
extern int PCD;							/* PCD-Kennung fÅr aktuell zu ladendes Bild */
extern int PCDwidth, PCDheight;			/* Hîhe und Breite des PCD-Bildes */
extern int handle;						/* VDI-Hendl */
extern int scrwd, scrht;				/* Screen Breite+Hîhe */
extern int mouse_xpos, mouse_ypos;		/* Mausposition */
extern int key_scancode;				/* Scancode beim letzten Keyboard-Event */
extern int key_ascii;
extern int obj;							/* Objekt beim loslassen des Buttons */
extern int mouse_button, key_at_event;

extern MFORM *dummy_ptr;				/* Dummymouse fÅr Maus-Form */
extern MFORM lr_arrow, ud_arrow, lrud_arrow;
extern long f_len;						/* LÑnge des letzten geladenen Files */
extern int *messagebuf;
extern int klicks;						/* Anzahl Mausklicks beim letzten Buttonevent */
extern char *stpath;					/* Smurf-Standardpfad */
extern char loadpath[257];				/* voller Pfad der zuletzt geladenen Datei */
extern char savepath[257];				/* voller Pfad der zuletzt gespeicherten Datei */
extern char commpath[257];				/* voller Pfad der zuletzt Åber ein Protokoll empfangenen Datei */
extern char DraufschmeissBild;
extern SYSTEM_INFO Sys_info;			/* Systemkonfiguration */
extern IMPORT_LIST Import_list;			/* Importmodul-Liste */

extern int orig_red[256];
extern int orig_green[256];
extern int orig_blue[256];
extern char planetable[260];

/*--------- Exporterspezifisches ------*/
extern char *edit_modules[100];			/* Pfade fÅr bis zu 100 Edit-Module */
extern char *export_modules[100];		/* Pfade fÅr bis zu 100 Export-Module */
extern char *export_cnfblock[50];		/* Konfigurationsblîcke fÅr die Exporter */
extern int export_cnflen[50];			/* LÑnge des jeweiligen Blockes */

extern CROSSHAIR position_markers[20];	/* Positionsmarker fÅr die Editmodule */
extern char *picnames[100];				/* BILDMANAGER: Namen fÅr bis zu 100 Bilder */

extern int anzahl_importmods;			/* Anzahl an Import-Modulen */
extern int anzahl_dithermods;			/* Anzahl an Dither-Modulen */

extern long Name_Max;

/*----- Window-Variablen --------*/
extern WINDOW wind_s[25];
extern WINDOW picture_windows[MAX_PIC];
extern POP_UP popups[25];
extern SLIDER sliders[15];
extern char module_pics[MAX_MODS][7];

extern int Radio, SelectedRadio;
extern int Check, SelectedCheck;
extern int Cycle, SelectedCycle;

extern GRECT screen;					/* globales Screen-GRECT */

extern int appl_id, menu_id;
extern SMURF_PIC *smurf_picture[25];
extern int active_pic;
extern BASPAG *Dithermod_Basepage[10];			/* Basepages fÅr Dithermodule */

extern long sx1, sx2, sx3, sx4;			/* Maxima */
extern long sn1, sn2, sn3, sn4;			/* Minima */
extern int sy1, sy2, sy3, sy4;				/* eingestellte Sliderwerte */
extern int edit_mod_num;				/* Modul-ID des Moduls, das das Einstellformular benutzt */
extern char *export_path;					/* Pfad des Export-Modules	*/
extern int export_depth[8];
extern int export_format[8];
extern int TOOLBAR_HEIGHT;

extern char Smurf_locked;
extern char Startup;				/* Hochfahren des ganzen Monster-Systems */
extern char startupdial_exist;
extern int num_of_pics;

extern long timer_fx_max[10];

extern int fix_red[256];
extern int fix_blue[256];
extern int fix_green[256];

extern int sx, sy, sw, sh;

extern OBJECT *startrsc;

void f_set_syspal(void);				/* Systempalette setzen */
void f_set_picpal(SMURF_PIC *pic);   /* Bildpalette setzen */
void f_pic_info(void);               /* Bildinfo anzeigen */
void f_newpic(int scancode);
void make_smurf_pic(int pic_to_make, int wid, int hgt, int depth, char *picdata);
void make_pic_window(int pic_to_make, int wid, int hgt, char *name);
int CallDialog(int topwin);     /* Dialogdispatcher */
void f_info(void);
int f_loadpic(char *pic, char *picpath);
int f_formhandle(int picture_to_load, int module_ret, char *namename);
int init_dialog(int DialogNumber, int DialogOK);
void close_dialog(int windnum);
void shutdown_smurf(char while_startup);
int duplicate_pic(WINDOW *window);


/*
 * options.c
 */
void f_options(void);                /* SMURF - Optionen */
void f_set_envconfig(void);          /* Environment-Konfiguration Åbernehmen */


/*
 * edit.c
 */
void f_edit_pop(void);               /* Edit-Modul-Fenster */
void emod_info_on(int mod_index);
void emod_info_off(void);


/*
 * export.c
 */
extern EXPORT_CONFIG exp_conf;

void f_export_pic(void);                                 /* Bild exportieren */
int f_save_pic(MOD_ABILITY *export_mabs);
void save_file(void);
int dither_for_export(MOD_ABILITY *mod_abs, int max_expdepth, int dest_format, SMURF_PIC *converted_pic);
void init_exmod_info(int mod_index);
void exmod_info_off(void);
int loadNCT(int loadplanes, SYSTEM_INFO *sysinfo);


/*
 * objct.c
 */
void f_handle_radios(OBJECT *tree, int klickobj, int windnum);
void f_handle_editklicks(WINDOW *window, int object);
int UDO_or_not(WINDOW *wind, int klickobj);
void f_deselect_popup(WINDOW *wind, int ob1, int ob2);


/*
 * exp_form.c
 */
extern	MOD_ABILITY export_mod_ability;

void f_export_formular(void);
void prepare_depthpopup(void);


/*
 * picevent.c
 */
extern BLOCKMODE blockmode_conf;

void f_pic_event(WINDOW *picwindow, int event_type, int windnum);            /* Auswertung von Events im Bildfenster */
void f_block_popup(WINDOW *picwindow);
void set_nullcoord(WINDOW *picwindow);
void f_display_coords(WINDOW *pic_window, int mx, int my, char blockflag);
void f_display_bwh(WINDOW *pic_window);
void reload_pic(WINDOW *picwindow);
void f_activate_pic(int windnum);
void picwin_keyboard(int key_scancode, int key_at_event, WINDOW *picwin);


/*
 * dispopts.c
 */
void f_display_opt(void);            /* Display-Optionen */
void f_update_dwindow(int mode, int redraw);


/*
 * picman.c
 */
void f_picman(void);
void insert_to_picman(int pic_to_insert);
void show_picman_wh(SMURF_PIC *pic);
int pm_autoscroll(int mx, int my);
void f_resort_piclist(void);
MOD_INFO *ready_modpics_popup(WINDOW *mwindow);     /* Modul-Bildreihenfolge-Popup vorbereiten */
void make_picman_thumbnail(int picture_num);
void picman_windowmove(void);
int compute_zoom(SMURF_PIC *picture, int twid, int thgt);



/* **********************************************************************/
/* ---------------------------- Toolfunktionen ------------------------ */
/* **********************************************************************/
char *quote_arg(char *s);
char *unquote_arg(char *s);
char *strargvtok(char *s);
char *mystrtok(char *s, char c);
int strsrchl(char *s, char c);
int strsrchr(char *s, char c);
void get_tmp_dir(char *tmpdir);
int get_cookie(unsigned long cookie, unsigned long *value);
void *SMalloc(long amount);
int SMfree(void *ptr);
void set_startupdial(char *string);
float convert_units(int oldunit, int newunit, float dpi);
char *load_palfile(char *path, int *red, int *green, int *blue, int max_cols);
long get_maxnamelen(char *path);
void BCD2string(char *string, int bcd);
char *strrpbrk(char *s1beg, char *s1, char *s2);
void make_singular_display(DISPLAY_MODES *old, int Dither, int Pal);
void restore_display(DISPLAY_MODES *old);
char *shorten_name(char *string, char newlen);
int get_path(char *path, char drive);
int set_path(char *path);
void f_drag_object ( WINDOW *wind, int objct, int *dex, int *dey, int call, int (*call_me)(int mx, int my) );



/* **********************************************************************/
void color_choose(void);

/* **********************************************************************/
/* ---------------------- Bindings neuer Funktionen ------------------- */
/* Denn kein Betriebssystem ist vollkommen.                             */
/* **********************************************************************/
#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)
int cdecl fsel_boxinput(char *path, char *name, int *button, char *label, void *callback);
#endif
int SM_wind_set(int wi_ghandle, int wi_gfield, int wi_gw1, int wi_gw2, int wi_gw3, int wi_gw4);
int SM_wind_get(int wi_ghandle, int wi_gfield, int *wi_gw1, int *wi_gw2, int *wi_gw3, int *wi_gw4);

#endif
