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

/* max. number of import modules */
#define MAX_IMPORT_MODS 200

/*---------------- Strukturen -----------------*/

/* Wird sp�ter mit Dpathconf() f�r den Modulpfad selbst bestimmt und Runtime festgelegt. */
/* Falls Dpathconf() nicht existiert, wird 12 angenommen. */
#define NAME_MAX 255        /* maximale L�nge Dateiname ohne Pfad */

struct DIRENTRY
{
    char *modname;
    struct DIRENTRY *next;
};

/*  Displaymode - Dither- und Palettenmodi f�r 
    verschiedene Quellbild-Farbtiefen           */
typedef struct
{
    short dither_24, syspal_24;
    short dither_8, syspal_8;
    short dither_4, syspal_4;
    short palette_mode;
} DISPLAY_MODES;


/* Die Namen und Extensions aller Importmodule */
typedef struct
{
    char *imp_mod_list[MAX_IMPORT_MODS];
    char *mod_exts[10][MAX_IMPORT_MODS];
} IMPORT_LIST;


typedef struct
{
    WORD ascii_code;
    WORD scan_code;
    char shift;
    char ctrl;
    char alt;
} KINFO;


#ifndef _DOSVARS
#define _DOSVARS
typedef struct
{
    char    *in_dos;                 /* Adresse der DOS- Semaphore */
    short   *dos_time;               /* Adresse der DOS- Zeit      */
    short   *dos_date;               /* Adresse des DOS- Datums    */
    long    res1;                    /*                            */
    long    res2;                    /*                            */
    long    res3;                    /* ist 0L                     */
    void    *act_pd;                 /* Laufendes Programm         */
    long    res4;                    /*                            */
    short   res5;                    /*                            */
    void    *res6;                   /*                            */
    void    *res7;                   /* interne DOS- Speicherliste */
    void    (*resv_intmem)(void);    /* DOS- Speicher erweitern    */
    long    (*etv_critic)(void);     /* etv_critic des GEMDOS      */
    char    *((*err_to_str)(signed char e)); /* Umrechnung Code->Klartext  */
    long    res8;                    /*                            */
    long    res9;                    /*                            */
    long    res10;                   /*                            */
} DOSVARS;
#endif


#ifndef _AESVARS
#define _AESVARS
typedef struct
{
    long magic;                   /* mu� $87654321 sein         */
    void *membot;                 /* Ende der AES- Variablen    */
    void *aes_start;              /* Startadresse               */
    long magic2;                  /* ist 'MAGX'                 */
    long date;                    /* Erstelldatum               */
    void (*chgres)(short res, short txt);  /* Aufl�sung �ndern    */
    long (**shel_vector)(void);   /* residentes Desktop         */
    char *aes_bootdrv;            /* von hieraus wurde gebootet */
    short *vdi_device;            /* vom AES benutzter Treiber  */
    void *reservd1;               /* reserviert                 */
    void *reservd2;               /* reserviert                 */
    void *reservd3;               /* reserviert                 */
    short version;                /* Version ($0201 ist V2.1)   */
    short release;                /* 0=alpha..3=release         */
} AESVARS;
#endif


#ifndef _MAGX_COOKIE
#define _MAGX_COOKIE
typedef struct
{
    unsigned long config_status;
    DOSVARS *dosvars;
    AESVARS *aesvars;
} MAGX_COOKIE;
#endif

typedef struct
{
    short header_length;
    long data_type;
    long data_length;
    char data_name[65];
    char file_name[SM_PATH_MAX + 1];
} DD_HEADER;


/*------------- Fadenkreuz-Struktur ---------*/ 
typedef struct
{
    short anzahl;            /* Anzahl an Positionsmarkern - max. 6 */
    WORD xpos[6], ypos[6];   /* Position im Bild */
    short mod_pic[6];        /* Bildnummer (Modul, 1-6) f�r jeden Marker */
    short smurfpic[6];       /* Smurf-Bildnummer f�r jeden Marker */
} CROSSHAIR;

/*------------ Blockmodus-Konfiguration ------*/
typedef struct 
{
    short mode;
    long opacity;
    short transparent;    /* Bit 0=s, bit 1=w */
} BLOCKMODE;

/*------------AV_COMM-Struktur, Semaphoren f�r die Kommunikation
        bei D&D-Operationen (AV_WHAT_IZIT/THAT_IZIT) per AV-Protokoll.
        Damit der Messagehandler wei�, auf was f�r eine Aktion der 
        WHAT_IZIT gesendet wurde und der THAT_IZIT kommt. */

typedef struct
{
    short type;          /* Typ der gedraggten Daten (0= Bild, 1=Block) */
    WORD windowhandle;   /* Handle des Smurf-Fensters, aus dem die Daten kommen */
    WORD keystate;       /* Zustand der Sondertasten beim Ausl�sen der Aktion */
} AV_COMM;



typedef struct export_config
{
    short export_mod_num;             /* ID des laufenden Exporters */
    short exp_depth, exp_form, exp_align, exp_colsys;
    short exp_colred, exp_dither;     /* Dither/Palettenkonfiguration f�r den Export */
    WORD exp_fix_red[SM_PALETTE_MAX];           /* Palette f�r FIXPAL-Palettenmodus */
    WORD exp_fix_green[SM_PALETTE_MAX];
    WORD exp_fix_blue[SM_PALETTE_MAX];
} EXPORT_CONFIG;


typedef struct extend_module_config
{
    short id;                /* vollst�ndige Modul-ID (alle 16 Bits!) */
    short notify_types[10];  /* bis zu 10 Ereignisarten, �ber die das Modul informiert werden will */
    WORD windhandle;         /* windowhandle f�r Eventweiterleitung */
} EXT_MODCONF;
