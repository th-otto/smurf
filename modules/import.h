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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/*  Headerfile f�r Module - SMURF                                   */
/*                  - Strukturdefinitionen -                        */
/*  Modulschnittstelle SCHLUMPFINE V0.6, 20.04.96 Olaf Piesche      */
/*  KOMMUNIKATION gro�geschrieben...                                */

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

/* �ber char *next_obj kann in ein Objekt eine Folge aufs n�chste Objekt */
/* gesetzt werden. Dadurch k�nnen komplexere Objekte mit Zeichenreihenfolge */
/* behandelt werden. */

typedef struct
{
    WORD    *points;            /* Anzahl an Punkten/Polygon */
    WORD    polygons;           /* Anzahl an Polygonen  */
    long    *x_coord, 
            *y_coord;           /* Zeiger auf Arrays mit den Punktkoordinaten in mm */
    WORD    *polydef;           /* Zeiger auf Array mit Polygondefinitionen, -1 ist Endkennung f�r ein Polygon  */
    char    *fillstyle;         /* F�llstile (nach VDI-Mustern) */  
    char    *linestyle;         /* Linienstile der Polygone, VDI */
    char    *linet;             /* Liniendicke in mm */
    char    *linecol;           /* Linienfarben (wie f�ll) */
    char    *fillcolor;         /* F�llfarben 3*char ccc mmm yyy, oder 4*char ccc mmm yyy kkk, 24Bittige Farbe  */  
    char    col_format;         /* CMYK oder RGB oder was?  */
    WORD    resolution;         /* Aufl�sung bei gegebenen Koordinaten in dpi */

    char    *next_obj;          /* VECTOR-Zeiger aufs n�chste Objekt in Reihenfolge (NULL = Ende) */
} VECTOR;


/* Struktur SMURF_PIC - Standard-Bildinformationsstruktur von SMURF */
/* 3 KB */
typedef struct smurfpic
{
    char filename[257];     /* Zeiger auf Dateiname incl. Pfad des Bildes - von Smurf */

    short changed;          /* Bitvektor f�r Bild�nderungen: 
                               Bit 0: neue NCT wird ben�tigt,
                               Bit 1: Redither ist notwendig
                               = 255: Bildstruktur enth�lt einen Block! */
                                                
    char format_name[41];   /* Zeiger auf Formatname - vom Modul */

    void *pic_data;         /* Zeiger auf rohe Bilddaten */
    void *mask;             /* Zeiger auf 8Bit-Maske, gekocht */

    WORD pic_width;         /* Breite des Bildes in Pixels, gebraten */
    WORD pic_height;        /* H�he des Bildes in Pixels, ged�nstet */
    WORD depth;             /* Farbtiefe in BIT (1-24), 30 min. bei 200C im Backofen... */
    MFDB *screen_pic;       /* Zeiger auf MFDB f�r Bildschirmdarstellung */
    char *palette;          /* Zeiger auf Palette */
    char bp_pal;            /* Farbtiefe pro Paletteneintrag (BITs) */
    long file_len;          /* Dateil�nge des Bildes */
    char format_type;       /* 0=Pixel Packed, 1=Standardformat */
    char col_format;        /* Farbsystem, in dem das Bild vom Modul zur�ckkommt */
    char infotext[97];      /* Infotext f�r weitere Bildinformationen */

    WORD red[256],          /* Palette, nach der die */
         grn[256],          /* Bildschirmdarstellung */
         blu[256];          /* gedithert wurde. (15Bit-Format) */


    WORD blockx, blocky, blockwidth, blockheight;    /* Koordinaten f�r den Blockrahmen */

    struct smurfpic *block;

    WORD zoom;               /* Zoomfaktor des Bildes */ 
    short image_type;        /* Was ist drin in der Datei? */

    char own_pal;           /* 1: hier herrscht eine eigene Palette */
    unsigned char *local_nct;       /* lokale NCT f�r die aktuelle Palette (wenn != Syspal!)    */
    short not_in_nct;               /* Kennung f�r nicht in der NCT enthaltene Farben           */
    
    struct smurfpic *prev_picture;  /* Zeiger auf vorheriges Bild (SMURF_PIC*) */
    struct smurfpic *next_picture;  /* Zeiger auf n�xtes Bild (SMURF_PIC*) */
} SMURF_PIC;


/* Allgemeine Windowstruktur zur internen Verwaltung von Fenstern */
/*  101 Bytes */
typedef struct
{
    WORD whandlem;          /* AES-Handle des Windows */
    short module;           /* Modul, dem das Fenster geh�rt */
    short wnum;             /* das wievielte Fenster des Moduls? */
    WORD wx, wy, ww, wh;    /* X/Y, Breite, H�he */ 
    char wtitle[41];        /* Fenstertitel */
    OBJECT *resource_form;  /* Modulresource-Formular */
    WORD dialog_num;		/* Dialognummer in der RSC fuer BGH */
    SMURF_PIC *picture;     /* Zeigerfeld f�r Bild/Animation */
    WORD xoffset, yoffset;  /* Scrolloffsets in Pixels */
    short shaded;           /* Shaded-Flag */
    WORD pic_xpos, pic_ypos; /* X/Y-Position f�r 'picture' im Fenster */
    WORD editob, nextedit, editx;

    WORD clipwid, cliphgt;  /* Breite und H�he des Ausschnittes von *picture */
    short pflag;            /* reines Bildfenster: 1, ansonsten 0 */

    void *prev_window;      /* vorheriges Fenster (WINDOW*) */
    void *next_window;      /* n�xtes Fenster (WINDOW*) */
    WORD fullx,fully,fullw,fullh;    /* zum R�ckspeichern der Koordinaten bei WM_FULLED */
} WINDOW;


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


/*---- Struktur MOD_INFO zur �bergabe von pars ans Hauptprogramm:   */
/* Zeiger darauf: bei 8(a0), wobei a0 = Textsegment                 */
/* 225 Bytes */
typedef struct
{
    char *mod_name;                 /* erweiterter Modulname    */
    short version;                  /* Modulversion alc BCD     */ 
    char *autor;                    /* Autor des Moduls         */
    char *ext[10];                  /* bis zu 10 Extensionen, deren Formate das Modul unterst�tzt */
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
    long sdef1,sdef2,sdef3,sdef4;   /* Defaultwerte f�r Slider */
    char cdef1,cdef2,cdef3,cdef4;   /* Defaultwerte f�r Checkboxes */
    long edef1,edef2,edef3,edef4;   /* Defaultwerte f�r Edit-Obs */

    char how_many_pix;              /* Wieviele Bilder braucht das EDITModul? */
    char *pic_descr1,               /* Bildbeschreibungen f�r die einzelnen Bilder */
         *pic_descr2,
         *pic_descr3,
         *pic_descr4,
         *pic_descr5,
         *pic_descr6;
} MOD_INFO;


/*----------------- SERVICE_FUNCTIONS - die Dienstfunktionen */
typedef struct
{
    int (*busybox)(int lft);
    void (*reset_busybox)(int lft, const char *string);

    int (*f_module_window)(WINDOW *mod_window);
    void (*f_module_prefs)(MOD_INFO *infostruct, short mod_id);

    int     (*popup)(POP_UP *popup_struct, int mouseflag, WORD button, OBJECT *poptree);
    void    (*deselect_popup)(WINDOW *wind, WORD ob1, WORD ob2);

    int     (*slider)(SLIDER *slider_struct);
    void    (*set_slider)(SLIDER *sliderstruct, long value);

    int     (*listfield)(long *window, WORD klick_obj, int keyscan, LIST_FIELD *lfstruct);
    void    (*generate_listfield)(WORD uparrow, WORD dnarrow, WORD sliderparent, WORD sliderobject,
                WORD listparent, char *listentries, WORD num_entries, WORD max_entries, LIST_FIELD *listfield, WORD autoloc);

    SMURF_PIC * (*new_pic)(WORD wid, WORD hgt, WORD depth);

    void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, int flags);

    void (*f_move_preview)(WINDOW *window, SMURF_PIC *orig_pic, WORD redraw_object);
    void (*copy_preview)(SMURF_PIC *source_pic, SMURF_PIC *module_preview, WINDOW *prev_window);

    void *  (*SMalloc)(long amount);
    int (*SMfree)(void *ptr);
    
    int CPU_type;
    int (*seek_nearest_col)(long *par, int maxcol);
    SMURF_PIC* (*get_pic)(WORD num, short mod_id, MOD_INFO *mod_info, WORD depth, int form, int col);

    int (*f_alert)(char *alertstring, char *b1, char *b2, char *b3, WORD defbt);
    int (*f_fsbox)(char *Path, char *fbtext, char selectart);
    float (*convert_units)(int oldunit, int newunit, float dpi);

    void *(*mconfLoad)(MOD_INFO *modinfo, short mod_id, char *name);
    void (*mconfSave)(MOD_INFO *modinfo, short mod_id, void *confblock, long len, char *name);
} SERVICE_FUNCTIONS;


/*------ Struktur GARGAMEL zur �bergabe an das Modul: */
typedef struct
{
    /*  0 */ SMURF_PIC *smurf_pic;                          /* Zeiger auf Bildstruktur, in die das Bild gelegt werden soll - Smurf                          */
    /*  4 */ WINDOW *wind_struct;                           /* Zeiger auf Bildstruktur, in die das Bild gelegt werden soll - Smurf                          */
    /*  8 */ long slide1, slide2, slide3, slide4;           /* �bergabewerte aus Einstellformular  (nach min/maxwerten aus MOD_INFO-Struktur  -  Smurf      */
    /* 24 */ char check1, check2, check3, check4;           /* �bergabewerte aus Einstellformular  (0 oder 1) - Smurf   */
    /* 28 */ long edit1, edit2, edit3, edit4;               /* �bergabewerte aus Einstellformular  (nach min/maxwerten aus MOD_INFO-Struktur  -  Smurf      */
    /* 44 */ short module_mode;                             /* Message */

    /* 46 */ WORD event_par[10];                            /* Beim Event betroffenes Objekt */

    /* 66 */ WORD mousex,mousey;                            /* Mauspos beim Event           */
    /* 70 */ short module_number;                           /* ID des Modules, von Smurf �bergeben */
    /* 72 */ WORD klicks;                                   /* Mausklicks beim Event        */
    /* 74 */ WORD picwind_x, picwind_y, picwind_w, picwind_h;     /* Betroffenes Bildfenster - Abmessungen und Pos. */

    /* 82 */ SERVICE_FUNCTIONS *services;
    /* 86 */
} GARGAMEL;


/* SYSTEM-Struktur f�r Systeminformationen */
/* Mu� beim Start von Smurf gef�llt werden (globale variable) */
/*  1857 Bytes */
typedef struct
{
    WORD app_id;
    char bitplanes;         /* aktuelle Planetiefe */
    unsigned short Max_col; /* maximale Farben momentan */
    WORD screen_width;      /* Bildschirmbreite */
    WORD screen_height;     /* Bildschirmh�he */
    WORD vdi_handle;
    char *nc_table;         /* Standard-Nearest-Color-Table. Mu� von Smurf anfangs geladen / erzeugt werden! */
    char *plane_table;      /* Standard-Binary-Palette-Table. Mu� von Smurf anfangs erzeugt werden! */

    WORD *red, *grn, *blu;  /* Zeiger auf Systempalette - m�ssen von Smurf anfangs gef�llt werden! */


    char standard_path[256];    /* Smurf-Startpfad */
    char home_path[256];        /* Zeiger auf $HOME-Verzeichnis - kann auf Standard_path zeigen! */

    short DSP;              /* DSP-existant-Flag */

    WORD pal_red[257],      /* Systempalette (die echte im VDI-Format!)*/
        pal_green[257], 
        pal_blue[257];
    
	/* Variablen f�r die Konfiguration */
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
    char dialog_opened[25];

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


/* ---------------- Definition der Modul-F�higkeiten -------------- */
/* In dieser Struktur legen Edit- und Exportmodule Werte ab, die    */
/*  bestimmen, in welchen Farbtiefen, Datenformaten, Farbsystemen,  */
/*  etc. das Bild verarbeitet werden kann.                          */
/*  Eine Struktur dieses Typs mu� in jedem Edit- und Exportmodul    */
/*  global definiert sein und wird vom Startupcode in den Modul-    */
/*  header eingeh�ngt.                                              */
/*  --------------------------------------------------------------  */
/*  74 Bytes */
typedef struct
{
/*----------Die ersten 3 Parameter werden von Smurf zur automatischen   */
/*          Transformierung benutzt.                                    */
    /* Farbtiefen, die vom Modul unterst�tzt werden: */
    char depth1, depth2, depth3, depth4, depth5, depth6, depth7, depth8;
    /* Dazugeh�rige Datenformate (FORM_PIXELPAK/FORM_STANDARD/FORM_BOTH) */
    char form1, form2, form3, form4, form5, form6, form7, form8;

    char ext_flag;      /* Ext-Flag: Bit 0=Preview, 1=MMORE, 2=Infotext m�glich */
} MOD_ABILITY;


/*--------------- R�ckgabestruktur vom Exportmodul ---------------- */
/*  Diese Struktur mu� vom Exportmodul nach Kodierung des Bildes an */
/*  Smurf zur�ckgegeben werden. Dieser speichert den fertigen Block */
/*  dann ab.                                                        */
/* ---------------------------------------------------------------- */
/*  8 Bytes */
typedef struct
{
    void *pic_data;     /* Daten des zu speichernden Blockes    */
    long f_len;         /* L�nge des Blockes in Bytes           */
} EXPORT_PIC;


#define MOD_MAGIC_DITHER 0x53444d44L /* 'SDMD' */
#define MOD_MAGIC_EDIT   0x53454d44L /* 'SEMD' */
#define MOD_MAGIC_EXPORT 0x53584d44L /* 'SXMD' */
#define MOD_MAGIC_IMPORT 0x53494d44L /* 'SIMD' */
#define MOD_MAGIC_PLUGIN 0x53504c47L /* 'SPLG' */

/*-------- Pixel Packed -> Standardformat - Routinen    ----------------*/
int setpix_standard(char *buf16, char *dest, int depth, long planelen, int howmany) ASM_NAME("_setpix_standard");
int setpix_pp(char *buf16, char *dest, int depth, long planelen, int howmany) ASM_NAME("_setpix_pp");
int setpix_standard_16(char *buf16, char *dest, int depth, long planelen, int howmany) ASM_NAME("_setpix_standard_16");
void get_standard_pix(void *st_pic, void *buf16, int planes, long planelen) ASM_NAME("_get_standard_pix");
void getpix_std_1(char *std, short *pixval, int depth, long planelen, int which) ASM_NAME("_getpix_std_1");
int setpix_std_line(char *buf, char *dest, int depth, long planelen, int howmany) ASM_NAME("_setpix_std_line");
void getpix_std_line(char *std, char *buf, int depth, long planelen, int howmany) ASM_NAME("_getpix_std_line");
int setpix_std_line16(char *buf, char *dest, int depth, long planelen, int howmany) ASM_NAME("_setpix_std_line16");
void rearrange_line2(char *src, char *dst, long bytes, unsigned int pixels) ASM_NAME("_rearrange_line2");


int f_rslid(SLIDER *slider_struct);
void setslider(SLIDER *sliderstruct, long value);
void f_txtinsert(int num, OBJECT *tree, int txt_obj, WINDOW *ws); /* Einf�gen von Zahlen in Textobjekte */

#endif
