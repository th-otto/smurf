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
int file_save(char *stext, uint8_t *buf, long length);
int f_fsbox(char *Path, char *fbtext, char selectart);

/*  File laden */
char *fload(char *Path, int header);

/* Bild importieren */
void reload_pic(WINDOW *picwindow);
short f_import_pic(SMURF_PIC *smurf_picture, char *filename);
uint8_t *f_do_pcd(const char *Path);


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
#define	AP_ARGSTART		0x5001

void f_event(void);              /* Allg. Event-Funktion                 */
WORD f_handle_message(void);     /* AES-Message-Handler                  */

void check_windclose(short windnum);   /* Auswerten von Windowdialog-AbbrÅchen */




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
void f_dither_24_1(SYSTEM_INFO *sysinfo, SMURF_PIC *picture, uint8_t *where_to, int mode, uint8_t *nct, GRECT *part);
void f_transform_to_standard(SMURF_PIC *picture, uint8_t *paddr);
void f_align_standard(SMURF_PIC *picture, uint8_t *paddr, GRECT *part);

/* Anpassen der Palette */
uint8_t *fix_palette(SMURF_PIC *dest, SYSTEM_INFO *sys_info);

/* Nearest-Color-Suchfunktion (Assembler) */
short seek_nearest_col(long *par, short maxcol) ASM_NAME("_seek_nearest_col");

/* Routinen fÅr 16 und 24Bit-Bildschirmmodus */
void direct2screen(SMURF_PIC *picture, uint8_t *where_to, GRECT *part);

int export_dither_dispatcher(SMURF_PIC *dest, SYSTEM_INFO *sys_info, DISPLAY_MODES *display, WORD *fixpal_red, WORD *fixpal_green, WORD *fixpal_blue);
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
SMURF_PIC *f_generate_newpic(WORD wid, WORD hgt, WORD depth);


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
struct DIRENTRY *build_up_filelist(char *path, char *ext, short pathlen);
struct DIRENTRY *insert_entry(struct DIRENTRY *ende, char *string);
void destroy_filelist(struct DIRENTRY *begin);

short f_init_system(void);                  /* Systeminitialisierung - die Mutterfunktion */
void    f_init_palette(void);               /* Palette auslesen     */
uint8_t *f_init_table(void);                /* NC-Tabelle init      */
void    f_init_bintable(OBJECT *rsc);       /* BinÑrwert-Table init */



WORD handle_keyboardevent(WINDOW *wind_s, WORD scancode, WORD *sel_object);
void change_object(WINDOW *window, WORD objct, WORD status, int redraw);


/*
 * resource.c
 */
extern short add_flags[40];

void f_init_popups(void);        /* Alle Popups initialisieren */
void f_init_sliders(void);   
void init_smurfrsc(const char *rscpath);
void fix_rsc(void);              /* RSC-Anpassung an OS, Farbtiefe, AES, etc... */


/*
 * shortc2.c
 */
char set_menu_key(OBJECT *menu);
char get_menu_key(OBJECT *menu, KINFO *ki, WORD *title, WORD *item);
int scan_2_ascii(int scan, int state);




/*
 * menu.c
 */
void f_init_menu(void);          /* MenÅzeile her! */
void f_exit_menu(void);          /* MenÅzeile weg! */
WORD f_handle_menuevent(WORD *message);
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

short get_dragdrop(WINDOW *window_to_handle, WORD *messagebuf);
short send_dragdrop(SMURF_PIC *picture, WORD dest_whandle, WORD mx, WORD my);
short dd_getheader(DD_HEADER *dd_header, WORD pipe_handle);
short dd_sendheader(WORD pipe_handle, long ext, long size, char *data_name, char *file_name);
void send_AESMessage(WORD dest_id, WORD msg, ...);                /* AES-Message an anderen Prozess schicken */
void update_dhst(char *path);
void bubble_init(void);
void bubble_exit(void);
void bubble_gem(WORD windownum, WORD xpos, WORD ypos, int modulemode);
void init_AVPROTO(void);
void get_avserv(void);
void call_stguide(WORD topwin_handle);



/* **********************************************************************/
/* ---------------------------- Modulfunktionen ----------------------- */
/* Some of them want to use you, some of them want to get used by you...*/
/* **********************************************************************/
short handle_modevent(short event_type, WINDOW *mod_window);    /* Handling eines Events im Modulfenster */
void f_handle_modmessage(GARGAMEL *smurf_struct);
short analyze_message(short module_ret, short picture_to_load);   /* Analyse eines Modul-Returns */
short f_open_module_window(WINDOW *module_window);        /* Modulfenster îffnen */
short give_free_module(void);     /* Ermittelt Strukturindex fÅr freies Modul und gibt diesen zurÅck */
void check_and_terminate(short mode, short module_number);
void walk_module_tree(WINDOW *wind, WORD start);
void init_modtree(OBJECT *tree, WORD index);
void convert_icon(OBJECT *tree, WORD index);
SMURF_PIC *get_pic(WORD num, short mod_id, MOD_INFO *mod_info, WORD depth, short form, short col);
short f_give_pics(MOD_INFO *mod_info, MOD_ABILITY *mod_abs, short module_number);       /* mehrere Bilder ans Modul Åbergeben */
short inform_modules(short message, SMURF_PIC *picture);    /* Informiert alle Module und Plugins Åber message */
long get_proclen(BASPAG *baspag);
long get_modmagic(BASPAG *basepage);
void AESmsg_to_modules(WORD *msgbuf);
void make_modpreview(WINDOW *wind);


/*
 * prefdial.c
 */
extern int prev_zoom;
extern SMURF_PIC move_prev;

void f_module_prefs(MOD_INFO *infostruct, short mod_id);   /* Modulformular aufrufen */
void f_mpref_change(void);                               /* Eingabe im Modulformular */
void f_move_preview(WINDOW *window, SMURF_PIC *orig_pic, WORD redraw_object);
void copy_preview(SMURF_PIC *source_pic, SMURF_PIC *module_preview, WINDOW *prev_window);


/*
 * modconf.c
 */
extern void *edit_cnfblock[100];
extern OBJECT *confsave_dialog;
extern OBJECT *modconf_popup;

void *mconfLoad(MOD_INFO *modinfo, short mod_id, char *name);
void mconfSave(MOD_INFO *modinfo, short mod_id, void *confblock, long len, char *name);
void *load_from_modconf(MOD_INFO *modinfo, char *name, short *num, long type);
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
short f_open_window(WINDOW *wind);                        /* Fenster îffnen */
void f_redraw_window(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);      /* Fenster Redraw */
void draw_iconified(WINDOW *window, WORD *vdiclip);
void draw_block(WINDOW *window, GRECT *picbox);
void insert_picwinzoom(WINDOW *window);
void f_draw_crosshair(WINDOW *window);       /* Positionierungskreuz in einem Bild zeichnen */
void f_draw_blockbox(WINDOW *window);
void draw_picmanboxes(void);
void clip_picw2screen(WINDOW *picw);
int f_rc_intersect( GRECT *r1, GRECT *r2, GRECT *r3);     /* Zum Redraw. */
short my_window(WORD handle);                              /* Fenster identifizieren */
WINDOW *my_module_window(WORD handle);                      /* Modulfenster identif. */
void f_setsliders(WINDOW *wind);                         /* Slider setzen */
void f_arrow_window(int mode, WINDOW *window, int amount);   /* Pfeilbedienung im Bildfenster */
void scrollWindowRT(WINDOW *window, int xamount, int yamount);
void f_slide_window(int pos, WINDOW *wind, int mode);    /* Bildfenster sliden */
void toggle_asterisk(WINDOW *picwin, int onoff);
void window_to_list(WINDOW *window);        /* Fenster in die Liste hÑngen */
void remove_window(WINDOW *window);
int find_crosshair(WINDOW *window);         /* findet eine Fadenkreuzstruktur zu einem Bildfenster */
void top_window(WORD handle);
void top_window_now(WINDOW *window);
void top_windowhandle(WORD handle);
void close_window(WORD handle);
WORD f_alert(char *alertstring, char *b1, char *b2, char *b3, WORD defbt);   /* Fenster-Alertfunktion */
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
int save_block(EXPORT_PIC *pic_to_save, const char *path);

int block2clip(SMURF_PIC *picture, int mode, const char *path);
void clip2block(SMURF_PIC *picture, char *data, WORD mx, WORD my);

SMURF_PIC *previewBlock(SMURF_PIC *picture, GRECT *blockpart);
void *copyblock(SMURF_PIC *old_pic);
int insert_block(WINDOW *picwindow);
void block_dklick(WINDOW *picwindow);

/* **********************************************************************/
/* ------------------------------ Statusbox --------------------------- */
/* "Picard an Maschinenraum! Statusbericht!"                            */
/* **********************************************************************/
void reset_busybox(short lft, const char *txt);     /* Busybox setzen und Text einfÅgen */
void ok_busybox(void);                       /* Busybox auf 128/"OK" setzen */
short draw_busybox(short lft);                  /* Busybox setzen */
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
extern BOOLEAN nullcoordset;
extern BOOLEAN syspalset;
extern WORD gl_hchar, gl_wchar, gl_hbox, gl_wbox;
extern short picwindthere, dialwindthere, picthere;
extern short openmode;					/* Dialog neu geîffnet (0) oder buttonevent? (!=0) */
extern BOOLEAN PCD;						/* PCD-Kennung fÅr aktuell zu ladendes Bild */
extern WORD PCDwidth, PCDheight;		/* Hîhe und Breite des PCD-Bildes */
extern WORD handle;						/* VDI-Hendl */
extern WORD scrwd, scrht;				/* Screen Breite+Hîhe */
extern WORD mouse_xpos, mouse_ypos;		/* Mausposition */
extern WORD key_scancode;				/* Scancode beim letzten Keyboard-Event */
extern WORD key_ascii;
extern WORD obj;						/* Objekt beim loslassen des Buttons */
extern WORD mouse_button, key_at_event;

extern MFORM *dummy_ptr;				/* Dummymouse fÅr Maus-Form */
extern MFORM lr_arrow, ud_arrow, lrud_arrow;
extern long f_len;						/* LÑnge des letzten geladenen Files */
extern WORD *messagebuf;
extern WORD klicks;						/* Anzahl Mausklicks beim letzten Buttonevent */
extern char *stpath;					/* Smurf-Standardpfad */
extern char loadpath[257];				/* voller Pfad der zuletzt geladenen Datei */
extern char savepath[257];				/* voller Pfad der zuletzt gespeicherten Datei */
extern char commpath[257];				/* voller Pfad der zuletzt Åber ein Protokoll empfangenen Datei */
extern char DraufschmeissBild;
extern SYSTEM_INFO Sys_info;			/* Systemkonfiguration */
extern IMPORT_LIST Import_list;			/* Importmodul-Liste */

extern WORD orig_red[256];
extern WORD orig_green[256];
extern WORD orig_blue[256];
extern uint8_t planetable[260];

/*--------- Exporterspezifisches ------*/
extern char *edit_modules[100];			/* Pfade fÅr bis zu 100 Edit-Module */
extern char *export_modules[100];		/* Pfade fÅr bis zu 100 Export-Module */
extern char *export_cnfblock[50];		/* Konfigurationsblîcke fÅr die Exporter */
extern short export_cnflen[50];			/* LÑnge des jeweiligen Blockes */

extern CROSSHAIR position_markers[20];	/* Positionsmarker fÅr die Editmodule */
extern char *picnames[100];				/* BILDMANAGER: Namen fÅr bis zu 100 Bilder */

extern int16_t anzahl_importmods;			/* Anzahl an Import-Modulen */
extern int16_t anzahl_dithermods;			/* Anzahl an Dither-Modulen */

extern long Name_Max;

/*----- Window-Variablen --------*/
extern WINDOW wind_s[25];
extern WINDOW picture_windows[MAX_PIC];
extern POP_UP popups[25];
extern SLIDER sliders[15];
extern char module_pics[MAX_MODS][7];

extern WORD Radio, SelectedRadio;
extern WORD Check, SelectedCheck;
extern WORD Cycle, SelectedCycle;

extern GRECT screen;					/* globales Screen-GRECT */

extern WORD appl_id, menu_id;
extern SMURF_PIC *smurf_picture[25];
extern short active_pic;
extern BASPAG *Dithermod_Basepage[10];			/* Basepages fÅr Dithermodule */

extern long sx1, sx2, sx3, sx4;			/* Maxima */
extern long sn1, sn2, sn3, sn4;			/* Minima */
extern short sy1, sy2, sy3, sy4;				/* eingestellte Sliderwerte */
extern short edit_mod_num;				/* Modul-ID des Moduls, das das Einstellformular benutzt */
extern char *export_path;					/* Pfad des Export-Modules	*/
extern short export_depth[8];
extern short export_format[8];
extern WORD TOOLBAR_HEIGHT;

extern char Smurf_locked;
extern char Startup;				/* Hochfahren des ganzen Monster-Systems */
extern char startupdial_exist;
extern short num_of_pics;

extern long timer_fx_max[10];

extern WORD fix_red[256];
extern WORD fix_blue[256];
extern WORD fix_green[256];

extern WORD sx, sy, sw, sh;

extern OBJECT *startrsc;

void f_set_syspal(void);				/* Systempalette setzen */
void f_set_picpal(SMURF_PIC *pic);   /* Bildpalette setzen */
void f_pic_info(void);               /* Bildinfo anzeigen */
void f_newpic(WORD scancode);
void make_smurf_pic(short pic_to_make, WORD wid, WORD hgt, WORD depth, char *picdata);
void make_pic_window(short pic_to_make, WORD wid, WORD hgt, char *name);
BOOLEAN CallDialog(WORD topwin);     /* Dialogdispatcher */
void f_info(void);
short f_loadpic(char *pic, char *picpath);
short f_formhandle(short picture_to_load, short module_ret, char *namename);
WORD init_dialog(short DialogNumber, WORD DialogOK);
void close_dialog(short windnum);
void shutdown_smurf(char while_startup);
short duplicate_pic(WINDOW *window);


/*
 * options.c
 */
void f_options(void);                /* SMURF - Optionen */
void f_set_envconfig(void);          /* Environment-Konfiguration Åbernehmen */


/*
 * edit.c
 */
void f_edit_pop(void);               /* Edit-Modul-Fenster */
void emod_info_on(short mod_index);
void emod_info_off(void);


/*
 * export.c
 */
extern EXPORT_CONFIG exp_conf;

void f_export_pic(void);                                 /* Bild exportieren */
int f_save_pic(MOD_ABILITY *export_mabs);
void save_file(void);
short dither_for_export(MOD_ABILITY *mod_abs, short max_expdepth, short dest_format, SMURF_PIC *converted_pic);
void init_exmod_info(short mod_index);
void exmod_info_off(void);
short loadNCT(short loadplanes, SYSTEM_INFO *sysinfo);


/*
 * objct.c
 */
void f_handle_radios(OBJECT *tree, WORD klickobj, short windnum);
void f_handle_editklicks(WINDOW *window, WORD object);
WORD UDO_or_not(WINDOW *wind, WORD klickobj);
void f_deselect_popup(WINDOW *wind, WORD ob1, WORD ob2);


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

void f_pic_event(WINDOW *picwindow, short event_type, short windnum);            /* Auswertung von Events im Bildfenster */
void f_block_popup(WINDOW *picwindow);
void set_nullcoord(WINDOW *picwindow);
void f_display_coords(WINDOW *pic_window, WORD mx, WORD my, BOOLEAN blockflag);
void f_display_bwh(WINDOW *pic_window);
void reload_pic(WINDOW *picwindow);
void f_activate_pic(short windnum);
void picwin_keyboard(WORD key_scancode, WORD key_at_event, WINDOW *picwin);


/*
 * dispopts.c
 */
void f_display_opt(void);            /* Display-Optionen */
void f_update_dwindow(int mode, int redraw);


/*
 * picman.c
 */
void f_picman(void);
void insert_to_picman(short pic_to_insert);
void show_picman_wh(SMURF_PIC *pic);
BOOLEAN pm_autoscroll(WORD mx, WORD my);
void f_resort_piclist(void);
MOD_INFO *ready_modpics_popup(WINDOW *mwindow);     /* Modul-Bildreihenfolge-Popup vorbereiten */
void make_picman_thumbnail(short picture_num);
void picman_windowmove(void);
WORD compute_zoom(SMURF_PIC *picture, WORD twid, WORD thgt);



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
void exported_SMfree(void *ptr);
void set_startupdial(char *string);
float convert_units(short oldunit, short newunit, float dpi);
char *load_palfile(char *path, int *red, int *green, int *blue, int max_cols);
long get_maxnamelen(char *path);
void BCD2string(char *string, int bcd);
char *strrpbrk(char *s1beg, char *s1, char *s2);
void make_singular_display(DISPLAY_MODES *old, int Dither, int Pal);
void restore_display(DISPLAY_MODES *old);
char *shorten_name(char *string, char newlen);
int get_path(char *path, char drive);
int set_path(char *path);
void f_drag_object(WINDOW *wind, WORD objct, WORD *dex, WORD *dey, BOOLEAN call, BOOLEAN (*call_me)(WORD mx, WORD my));



/* **********************************************************************/
void color_choose(void);

/* **********************************************************************/
/* ---------------------- Bindings neuer Funktionen ------------------- */
/* Denn kein Betriebssystem ist vollkommen.                             */
/* **********************************************************************/
#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)
int cdecl fsel_boxinput(char *path, char *name, int *button, char *label, void *callback);
#endif
WORD SM_wind_set(WORD wi_ghandle, WORD wi_gfield, WORD wi_gw1, WORD wi_gw2, WORD wi_gw3, WORD wi_gw4);
WORD SM_wind_get(WORD wi_ghandle, WORD wi_gfield, WORD *wi_gw1, WORD *wi_gw2, WORD *wi_gw3, WORD *wi_gw4);

#endif
