#ifndef _smurf_import_h_
#define _smurf_import_h_

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

/*  Headerfile fÅr Module - SMURF                                   */
/*                  - Strukturdefinitionen -                        */
/*  Modulschnittstelle SCHLUMPFINE V0.6, 20.04.96 Olaf Piesche      */
/*  KOMMUNIKATION groûgeschrieben...                                */

#include "portab.h"
#include "stdint_.h"
#ifdef __PUREC__
#include <vdi.h>
#include <tos.h>
#else
#include <osbind.h>
#include <mintbind.h>
#include <gem.h>
#define BASPAG BASEPAGE
#define DTA _DTA
#define d_fname dta_name
#define cdecl
#include <support.h>
#define itoa(val, str, base) _itoa(val, str, base, 0)
#define ltoa(val, str, base) _itoa(val, str, base, 0)
#define ftoa(val, str, ndig, format, decpnt, sign) strcpy(str, (format == 0 ? ecvt : fcvt)(*(val), ndig, decpnt, sign))
#endif

typedef struct _window WINDOW;

#include "sym_gem.h"

#ifdef __GNUC__
#  define ASM_NAME(x) __asm__(x)
#else
#  define ASM_NAME(x)
#endif

#define CNFVERSION  0x04

/*  OS-defines  */
#define MINT        1
#define MTOS        2
#define NAES        4
#define GENEVA      8
#define MATSCHIG    16
#define WINX        32
#define MAG_PC      64

/*  OSFeatures - Defines */
#define GETINFO     1
#define COLICONS    2
#define BEVT        4
#define MPOPUP      8
#define PEXEC10x    16
#define AES3D       32
#define OSYSVAR     64


/*  Prozessortyp */
#define MC68000 1
#define MC68020 2
#define MC68030 4
#define MC68040 8
#define FPU     16
#define MC68060 32

/*--------- Struktur VECTOR zur Beschreibung eines Vektorobjektes -----*/

/* öber char *next_obj kann in ein Objekt eine Folge aufs nÑchste Objekt */
/* gesetzt werden. Dadurch kînnen komplexere Objekte mit Zeichenreihenfolge */
/* behandelt werden. */

typedef struct
{
    WORD    *points;            /* Anzahl an Punkten/Polygon */
    WORD    polygons;           /* Anzahl an Polygonen  */
    long    *x_coord;
    long    *y_coord;           /* Zeiger auf Arrays mit den Punktkoordinaten in mm */
    WORD    *polydef;           /* Zeiger auf Array mit Polygondefinitionen, -1 ist Endkennung fÅr ein Polygon  */
    char    *fillstyle;         /* FÅllstile (nach VDI-Mustern) */  
    char    *linestyle;         /* Linienstile der Polygone, VDI */
    char    *linet;             /* Liniendicke in mm */
    char    *linecol;           /* Linienfarben (wie fÅll) */
    char    *fillcolor;         /* FÅllfarben 3*char ccc mmm yyy, oder 4*char ccc mmm yyy kkk, 24Bittige Farbe  */  
    char    col_format;         /* CMYK oder RGB oder was?  */
    WORD    resolution;         /* Auflîsung bei gegebenen Koordinaten in dpi */

    char    *next_obj;          /* VECTOR-Zeiger aufs nÑchste Objekt in Reihenfolge (NULL = Ende) */
} VECTOR;


/* Struktur SMURF_PIC - Standard-Bildinformationsstruktur von SMURF */
typedef struct smurfpic
{
    /*    0 */ char filename[257];     /* Zeiger auf Dateiname incl. Pfad des Bildes - von Smurf */

    /*  258 */ short changed;          /* Bitvektor fÅr BildÑnderungen: 
                               Bit 0: neue NCT wird benîtigt,
                               Bit 1: Redither ist notwendig
                               = 255: Bildstruktur enthÑlt einen Block! */
                                                
    /*  260 */ char format_name[41];   /* Zeiger auf Formatname - vom Modul */

    /*  302 */ void *pic_data;         /* Zeiger auf rohe Bilddaten */
    /*  306 */ void *mask;             /* Zeiger auf 8Bit-Maske, gekocht */

    /*  310 */ WORD pic_width;         /* Breite des Bildes in Pixels, gebraten */
    /*  312 */ WORD pic_height;        /* Hîhe des Bildes in Pixels, gedÅnstet */
    /*  314 */ WORD depth;             /* Farbtiefe in BIT (1-24), 30 min. bei 200C im Backofen... */
    /*  316 */ MFDB *screen_pic;       /* Zeiger auf MFDB fÅr Bildschirmdarstellung */
    /*  320 */ uint8_t *palette;       /* Zeiger auf Palette */
    /*  324 */ uint8_t bp_pal;         /* Farbtiefe pro Paletteneintrag (BITs) */
    /*  326 */ long file_len;          /* DateilÑnge des Bildes */
    /*  330 */ uint8_t format_type;    /* 0=Pixel Packed, 1=Standardformat */
    /*  331 */ uint8_t col_format;     /* Farbsystem, in dem das Bild vom Modul zurÅckkommt */
    /*  332 */ char infotext[97];      /* Infotext fÅr weitere Bildinformationen */

    /*  430 */ WORD red[256];          /* Palette, nach der die */
    /*  942 */ WORD grn[256];          /* Bildschirmdarstellung */
    /* 1454 */ WORD blu[256];          /* gedithert wurde. (15Bit-Format) */


    /* 1966 */ WORD blockx, blocky, blockwidth, blockheight;    /* Koordinaten fÅr den Blockrahmen */

    /* 1974 */ struct smurfpic *block;

    /* 1978 */ WORD zoom;               /* Zoomfaktor des Bildes */ 
    /* 1980 */ short image_type;        /* Was ist drin in der Datei? */

    /* 1982 */ uint8_t own_pal;         /* 1: hier herrscht eine eigene Palette */
    /* 1984 */ unsigned char *local_nct;       /* lokale NCT fÅr die aktuelle Palette (wenn != Syspal!)    */
    /* 1988 */ short not_in_nct;               /* Kennung fÅr nicht in der NCT enthaltene Farben           */
    
    /* 1990 */ struct smurfpic *prev_picture;  /* Zeiger auf vorheriges Bild (SMURF_PIC*) */
    /* 1994 */ struct smurfpic *next_picture;  /* Zeiger auf nÑxtes Bild (SMURF_PIC*) */
    /* 1998 */
} SMURF_PIC;


/* Allgemeine Windowstruktur zur internen Verwaltung von Fenstern */
/*  101 Bytes */
struct _window
{
    WORD whandlem;          /* AES-Handle des Windows */
    short module;           /* Modul, dem das Fenster gehîrt */
    short wnum;             /* das wievielte Fenster des Moduls? */
    WORD wx, wy, ww, wh;    /* X/Y, Breite, Hîhe */ 
    char wtitle[41];        /* Fenstertitel */
    OBJECT *resource_form;  /* Modulresource-Formular */
    WORD dialog_num;		/* Dialognummer in der RSC fuer BGH */
    SMURF_PIC *picture;     /* Zeigerfeld fÅr Bild/Animation */
    WORD xoffset, yoffset;  /* Scrolloffsets in Pixels */
    short shaded;           /* Shaded-Flag */
    WORD pic_xpos, pic_ypos; /* X/Y-Position fÅr 'picture' im Fenster */
    WORD editob, nextedit, editx;

    WORD clipwid, cliphgt;  /* Breite und Hîhe des Ausschnittes von *picture */
    short pflag;            /* reines Bildfenster: 1, ansonsten 0 */

    WINDOW *prev_window;     /* vorheriges Fenster */
    WINDOW *next_window;     /* nÑxtes Fenster */
    WORD fullx,fully,fullw,fullh;    /* zum RÅckspeichern der Koordinaten bei WM_FULLED */
};


/* Slider-Struktur zur Verwaltung von Slidern durch f_rslid */
typedef struct
{
    WORD regler;
    WORD schiene; 
    OBJECT *rtree;
    WORD txt_obj;
    long min_val;
    long max_val;
    WINDOW *window;
} SLIDER;


/*---- Struktur MOD_INFO zur öbergabe von pars ans Hauptprogramm:   */
/* Zeiger darauf: bei 8(a0), wobei a0 = Textsegment                 */
/* 225 Bytes */
typedef struct
{
    char *mod_name;                 /* erweiterter Modulname    */
    short version;                  /* Modulversion alc BCD     */ 
    char *autor;                    /* Autor des Moduls         */
    char *ext[10];                  /* bis zu 10 Extensionen, deren Formate das Modul unterstÅtzt */
    char *slide1;                   /* Funktion Slider 1        */
    char *slide2;                   /* Funktion Slider 2        */
    char *slide3;                   /* Funktion Slider 3        */
    char *slide4;                   /* Funktion Slider 4        */
    char *check1;                   /* Funktion Checkbox 1      */
    char *check2;                   /* Funktion Checkbox 2      */
    char *check3;                   /* Funktion Checkbox 3      */
    char *check4;                   /* Funktion Checkbox 4      */
    char *edit1;                    /* Funktion Ftext 1         */
    char *edit2;                    /* Funktion Ftext 2         */
    char *edit3;                    /* Funktion Ftext 3         */
    char *edit4;                    /* Funktion Ftext 4         */
    long smin1, smax1;              /* min/max-Werte Slider 1   */
    long smin2, smax2;              /* min/max-Werte Slider 2   */
    long smin3, smax3;              /* min/max-Werte Slider 3   */
    long smin4, smax4;              /* min/max-Werte Slider 4   */
    long emin1, emax1;              /* min/max-Werte Ftext 1    */
    long emin2, emax2;              /* min/max-Werte Ftext 2    */
    long emin3, emax3;              /* min/max-Werte Ftext 3    */
    long emin4, emax4;              /* min/max-Werte Ftext 4    */
    long sdef1,sdef2,sdef3,sdef4;   /* Defaultwerte fÅr Slider */
    uint8_t cdef1,cdef2,cdef3,cdef4;   /* Defaultwerte fÅr Checkboxes */
    long edef1,edef2,edef3,edef4;   /* Defaultwerte fÅr Edit-Obs */

    uint8_t how_many_pix;           /* Wieviele Bilder braucht das EDITModul? */
    char *pic_descr1,               /* Bildbeschreibungen fÅr die einzelnen Bilder */
         *pic_descr2,
         *pic_descr3,
         *pic_descr4,
         *pic_descr5,
         *pic_descr6;
} MOD_INFO;


/*----------------- SERVICE_FUNCTIONS - die Dienstfunktionen */
typedef struct
{
    /*   0 */ short (*busybox)(short lft);
    /*   4 */ void (*reset_busybox)(short lft, const char *string);

    /*   8 */ short (*f_module_window)(WINDOW *mod_window);
    /*  12 */ void (*f_module_prefs)(MOD_INFO *infostruct, short mod_id);

    /*  16 */ WORD (*popup)(POP_UP *popup_struct, WORD mouseflag, WORD button, OBJECT *poptree);
    /*  20 */ void (*deselect_popup)(WINDOW *wind, WORD ob1, WORD ob2);

    /*  24 */ short (*slider)(SLIDER *slider_struct);
    /*  28 */ void (*set_slider)(SLIDER *sliderstruct, long value);

    /*  32 */ WORD (*listfield)(WINDOW *window, WORD klick_obj, WORD keyscan, LIST_FIELD *lfstruct);
    /*  36 */ void (*generate_listfield)(WORD uparrow, WORD dnarrow, WORD sliderparent, WORD sliderobject,
                WORD listparent, char *listentries, WORD num_entries, WORD max_entries, LIST_FIELD *listfield, WORD autoloc);

    /*  40 */ SMURF_PIC *(*new_pic)(WORD wid, WORD hgt, WORD depth);

    /*  44 */ void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);

    /*  48 */ void (*f_move_preview)(WINDOW *window, SMURF_PIC *orig_pic, WORD redraw_object);
    /*  52 */ void (*copy_preview)(SMURF_PIC *source_pic, SMURF_PIC *module_preview, WINDOW *prev_window);

    /*  56 */ void *(*SMalloc)(long amount);
    /*  60 */ void (*SMfree)(void *ptr);
    
    /*  64 */ short CPU_type;
    /*  66 */ short (*seek_nearest_col)(long *par, short maxcol);
    /*  70 */ SMURF_PIC* (*get_pic)(WORD num, short mod_id, MOD_INFO *mod_info, WORD depth, short form, short col);

    /*  74 */ WORD (*f_alert)(char *alertstring, char *b1, char *b2, char *b3, WORD defbt);
    /*  78 */ BOOLEAN (*f_fsbox)(char *Path, const char *fbtext, uint8_t selectart);
    /*  82 */ float (*convert_units)(short oldunit, short newunit, float dpi);

    /*  86 */ void *(*mconfLoad)(MOD_INFO *modinfo, short mod_id, char *name);
    /*  90 */ void (*mconfSave)(MOD_INFO *modinfo, short mod_id, void *confblock, long len, char *name);
    /* 100 */ 
} SERVICE_FUNCTIONS;


/*------ Struktur GARGAMEL zur öbergabe an das Modul: */
typedef struct
{
    /*  0 */ SMURF_PIC *smurf_pic;                          /* Zeiger auf Bildstruktur, in die das Bild gelegt werden soll - Smurf                          */
    /*  4 */ WINDOW *wind_struct;                           /* Zeiger auf Bildstruktur, in die das Bild gelegt werden soll - Smurf                          */
    /*  8 */ long slide1, slide2, slide3, slide4;           /* öbergabewerte aus Einstellformular  (nach min/maxwerten aus MOD_INFO-Struktur  -  Smurf      */
    /* 24 */ uint8_t check1, check2, check3, check4;        /* öbergabewerte aus Einstellformular  (0 oder 1) - Smurf   */
    /* 28 */ long edit1, edit2, edit3, edit4;               /* öbergabewerte aus Einstellformular  (nach min/maxwerten aus MOD_INFO-Struktur  -  Smurf      */
    /* 44 */ short module_mode;                             /* Message */

    /* 46 */ WORD event_par[10];                            /* Beim Event betroffenes Objekt */

    /* 66 */ WORD mousex,mousey;                            /* Mauspos beim Event           */
    /* 70 */ short module_number;                           /* ID des Modules, von Smurf Åbergeben */
    /* 72 */ WORD klicks;                                   /* Mausklicks beim Event        */
    /* 74 */ WORD picwind_x, picwind_y, picwind_w, picwind_h;     /* Betroffenes Bildfenster - Abmessungen und Pos. */

    /* 82 */ SERVICE_FUNCTIONS *services;
    /* 86 */
} GARGAMEL;


/* SYSTEM-Struktur fÅr Systeminformationen */
/* Muû beim Start von Smurf gefÅllt werden (globale variable) */
/*  1857 Bytes */
typedef struct
{
    WORD app_id;
    uint8_t bitplanes;       /* aktuelle Planetiefe */
    unsigned short Max_col; /* maximale Farben momentan */
    WORD screen_width;      /* Bildschirmbreite */
    WORD screen_height;     /* Bildschirmhîhe */
    WORD vdi_handle;
    uint8_t *nc_table;      /* Standard-Nearest-Color-Table. Muû von Smurf anfangs geladen / erzeugt werden! */
    uint8_t *plane_table;   /* Standard-Binary-Palette-Table. Muû von Smurf anfangs erzeugt werden! */

    WORD *red, *grn, *blu;  /* Zeiger auf Systempalette - mÅssen von Smurf anfangs gefÅllt werden! */


    char standard_path[256];    /* Smurf-Startpfad */
    char home_path[256];        /* Zeiger auf $HOME-Verzeichnis - kann auf Standard_path zeigen! */

    short DSP;              /* DSP-existant-Flag */

    WORD pal_red[257],      /* Systempalette (die echte im VDI-Format!)*/
        pal_green[257], 
        pal_blue[257];
    
	/* Variablen fÅr die Konfiguration */
    short center_dialog;      
    short window_alert;       
    short profi_mode;     
    short busybox_oc;
    short harddisk_swap;
    short immed_prevs;
    short realtime_dither;
    short dither24;
    short dither8;
    short dither4;
    short pal24,pal8,pal4;
    short environment;
    short busybox_icon;
    short outcol;
    WORD AES_version;
    short OS;
    short OSFeatures;
    WORD dialog_xposition[25];
    WORD dialog_yposition[25];
    uint8_t dialog_opened[25];

    WORD ENV_avserver;
    WORD olgaman_ID;

    WORD Event_Timer;
    
    short PCD_Defsize;
    short PreviewDither;
    short PreviewMoveDither;
    short PicmanDither;
    short AutoconvDither, AutoconvPalmode;

    WORD AES_bgcolor;
    WORD keyevents;
    short defaultExporter;
    char *scrp_path;
} SYSTEM_INFO;


/* ---------------- Definition der Modul-FÑhigkeiten -------------- */
/* In dieser Struktur legen Edit- und Exportmodule Werte ab, die    */
/*  bestimmen, in welchen Farbtiefen, Datenformaten, Farbsystemen,  */
/*  etc. das Bild verarbeitet werden kann.                          */
/*  Eine Struktur dieses Typs muû in jedem Edit- und Exportmodul    */
/*  global definiert sein und wird vom Startupcode in den Modul-    */
/*  header eingehÑngt.                                              */
/*  --------------------------------------------------------------  */
/*  74 Bytes */
typedef struct
{
/*----------Die ersten 3 Parameter werden von Smurf zur automatischen   */
/*          Transformierung benutzt.                                    */
    /* Farbtiefen, die vom Modul unterstÅtzt werden: */
    uint8_t depth1, depth2, depth3, depth4, depth5, depth6, depth7, depth8;
    /* Dazugehîrige Datenformate (FORM_PIXELPAK/FORM_STANDARD/FORM_BOTH) */
    uint8_t form1, form2, form3, form4, form5, form6, form7, form8;

    uint8_t ext_flag;      /* Ext-Flag: Bit 0=Preview, 1=MMORE, 2=Infotext mîglich */
} MOD_ABILITY;
#define M_PREVIEW 0x01
#define M_MORE    0x02
#define M_INFO    0x04

/*--------------- RÅckgabestruktur vom Exportmodul ---------------- */
/*  Diese Struktur muû vom Exportmodul nach Kodierung des Bildes an */
/*  Smurf zurÅckgegeben werden. Dieser speichert den fertigen Block */
/*  dann ab.                                                        */
/* ---------------------------------------------------------------- */
/*  8 Bytes */
typedef struct
{
    void *pic_data;     /* Daten des zu speichernden Blockes    */
    long f_len;         /* LÑnge des Blockes in Bytes           */
} EXPORT_PIC;


#define MOD_MAGIC_DITHER 0x53444d44L /* 'SDMD' */
#define MOD_MAGIC_EDIT   0x53454d44L /* 'SEMD' */
#define MOD_MAGIC_EXPORT 0x53584d44L /* 'SXMD' */
#define MOD_MAGIC_IMPORT 0x53494d44L /* 'SIMD' */
#define MOD_MAGIC_PLUGIN 0x53504c47L /* 'SPLG' */

/*-------- Pixel Packed -> Standardformat - Routinen    ----------------*/
short setpix_standard(uint8_t *buf16, uint8_t *dest, short depth, long planelen, short howmany) ASM_NAME("_setpix_standard");
short setpix_pp(uint8_t *buf16, uint8_t *dest, short depth, long planelen, short howmany) ASM_NAME("_setpix_pp");
short setpix_standard_16(uint8_t *buf16, uint8_t *dest, short depth, long planelen, short howmany) ASM_NAME("_setpix_standard_16");
void get_standard_pix(void *st_pic, void *buf16, short planes, long planelen) ASM_NAME("_get_standard_pix");
void getpix_std_1(uint8_t *std, short *pixval, short depth, long planelen, short which) ASM_NAME("_getpix_std_1");
short setpix_std_line(uint8_t *buf, uint8_t *dest, short depth, long planelen, short howmany) ASM_NAME("_setpix_std_line");
void getpix_std_line(uint8_t *std, uint8_t *buf, short depth, long planelen, short howmany) ASM_NAME("_getpix_std_line");
short setpix_std_line16(uint8_t *buf, uint8_t *dest, short depth, long planelen, short howmany) ASM_NAME("_setpix_std_line16");
void rearrange_line2(uint8_t *src, uint8_t *dst, long bytes, unsigned short pixels) ASM_NAME("_rearrange_line2");


short f_rslid(SLIDER *slider_struct);
void setslider(SLIDER *sliderstruct, long value);
void f_txtinsert(short num, OBJECT *tree, WORD txt_obj, WINDOW *ws); /* EinfÅgen von Zahlen in Textobjekte */

extern MOD_INFO module_info ASM_NAME("module_info");
extern MOD_ABILITY module_ability ASM_NAME("module_ability");

EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct) ASM_NAME("exp_module_main");
short imp_module_main(GARGAMEL *smurf_struct) ASM_NAME("imp_module_main");
void edit_module_main(GARGAMEL *smurf_struct) ASM_NAME("edit_module_main");


#endif
