/* Headerdatei zum Fontselektoraufruf */


/* Smurfstruktur */
typedef struct
{
	WORD index;
	WORD ID;
	WORD size;
	WORD FX;
} FONT_INFO;


struct xfsl_input_args {
	WORD vdihandle;
	unsigned short fontflags;
	const char *headline;
	WORD *id;
	WORD *size;
};


/* Strukturen, Funktionen und Deklarationen */
/* fÅr xFSL-Fontselektoren */
typedef struct
{
	unsigned long xfsl;			/* Magic 'xFSL' */
	unsigned int  revision;		/* Schnittstellen-Revision */
	unsigned long product;		/* Kennung des Fontselektors */
	unsigned int  version;		/* Version des Fontselektors */
#if defined(__PUREC__) || (defined(__GNUC__) && defined(__MSHORT__))
    WORD cdecl (*xfsl_input)(WORD vdihandle, unsigned short fontflags, const char *headline, WORD *id, WORD *size);
#else
    WORD cdecl (*xfsl_input)(struct xfsl_input_args args);
#endif
	WORD cdecl (*xfsl_init)(WORD vdihandle, void *xpar);				/* Init-Aufruf */
	WORD cdecl (*xfsl_event)(WORD handle, void *event);			/* Event-Aufruf */
	WORD cdecl (*xfsl_exit)(WORD handle);				/* Exit-Aufruf */
	WORD cdecl (*xfsl_info)(void);				/* Info-Aufruf */
} xFSL;

#if !defined(__XFNT_INFO) && !defined(__GEMLIB__)
#define __XFNT_INFO
typedef struct
{
	long size;					/* LÑnge der Struktur, muû vor vqt_xfntinfo() gesetzt werden */
	WORD format;				/* Fontformat, z.B. 4 fÅr TrueType */
	WORD id;					/* Font-ID, z.B. 6059 */
	WORD index;					/* Index */
	char font_name[50];			/* vollstÑndiger Fontname, z.B. "Century 725 Italic BT" */
	char family_name[50];		/* Name der Fontfamilie, z.B. "Century725 BT" */
	char style_name[50];		/* Name des Fontstils, z.B. "Italic" */
	char file_name1[200];		/* Name der 1. Fontdatei, z.B. "C:\FONTS\TT1059M_.TTF" */
	char file_name2[200];		/* Name der optionalen 2. Fontdatei */
	char file_name3[200];		/* Name der optionalen 3. Fontdatei */
	WORD pt_cnt;				/* Anzahl der Punkthîhen fÅr vst_point(), z.B. 10 */
	WORD pt_sizes[64];			/* verfÅgbare Punkthîhen,*/
} XFNT_INFO;
#endif


/* Strukturen, Funktionen und Deklarationen */
/* fÅr den MagiC-Fontselektor */

/* Beschreibung von <font_flags>: */
#define FNTS_BTMP	1			/* Bitmapfonts anzeigen */
#define FNTS_OUTL	2			/* Vektorfonts anzeigen */
#define FNTS_MONO	4			/* Ñquidistante Fonts anzeigen */
#define FNTS_PROP	8			/* proportionale Fonts anzeigen */

/* Beschreibung von <dialog_flags>: */
#define  FNTS_3D		1		/* Auswahl im 3D-Look anzeigen */

/* Beschreibung von <button_flags>: */
#define FNTS_SNAME		0x01	/* Checkbox fÅr die Namen selektieren */
#define FNTS_SSTYLE		0x02	/* Checkbox fÅr die Stile selektieren */
#define FNTS_SSIZE		0x04	/* Checkbox fÅr die Hîhe selektieren */
#define FNTS_SRATIO		0x08	/* Checkbox fÅr das VerhÑltnis
								   Breite/Hîhe selektieren */

#define FNTS_CHNAME		0x0100	/* Checkbox fÅr die Namen anzeigen */
#define FNTS_CHSTYLE	0x0200	/* Checkbox fÅr die Stile anzeigen */
#define FNTS_CHSIZE		0x0400	/* Checkbox fÅr die Hîhe anzeigen */
#define FNTS_CHRATIO	0x0800	/* Checkbox fÅr das VerhÑltnis
								   Breite/Hîhe anzeigen */
#define FNTS_RATIO		0x1000	/* VerhÑltnis Breite/Hîhe ein-
								   stellbar */
#define FNTS_BSET		0x2000	/* Button "setzen" anwÑhlbar */
#define FNTS_BMARK		0x4000	/* Button "markieren" anwÑhlbar */

#define BUTTON_FLAGS	0

/* Beschreibung von fnts_do-RÅckgabe: */
#define	FNTS_CANCEL		1		/* "Abbruch" wurde angewÑhlt */
#define	FNTS_OK			2		/* "OK" wurde gedrÅckt */
#define	FNTS_SET		3		/* "setzen" wurde angewÑhlt */
#define	FNTS_MARK		4		/* "markieren" wurde betÑtigt */
#define	FNTS_OPT		5		/* der applikationseigene Button wurde ausgewÑhlt */

#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)

typedef	void	*FNT_DIALOG;

FNT_DIALOG *fnts_create(WORD vdi_handle, WORD no_fonts, WORD font_flags, WORD dialog_flags, const char *sample, const char *opt_button);

WORD fnts_delete(FNT_DIALOG *fnt_dialog, WORD handle);
WORD fnts_do(FNT_DIALOG *fnt_dialog, WORD flags, long id_in, long pt_in, long ratio_in, WORD *check_boxes, long *id, long *pt, long *ratio);


long vst_arbpt32(int handle, long height, int *char_width, int *char_height, int *cell_width, int *cell_height);
void v_opnbm(int *work_in, MFDB * bitmap, int *handle, int *work_out);
void v_clsbm(int handle);
int vqt_xfntinfo(int handle, int flags, int id, int index, XFNT_INFO * info);
void vqt_real_extent(int handle, int x, int y, const char *string, int *extent);
#endif


extern WORD work_in[25];
extern WORD work_out[57];
extern OBJECT *alerts;
extern SERVICE_FUNCTIONS *services;

int get_cookie(unsigned long cookie, unsigned long *value);
WORD call_fontsel(WORD  handle, FONT_INFO *fontinfo);
