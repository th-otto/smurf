#define	PDLG_CANCEL	1						/* "Abbruch" wurde angewÑhlt */
#define	PDLG_OK		2

#ifndef PDLG_3D
#define	PDLG_3D		1						/* Dialog im 3D-Stil anzeigen */
#define	PDLG_PREFS	0						/* Einstelldialog anzeigen */
#define	PDLG_PRINT	1						/* Druckdialog anzeigen */
#endif

#ifndef _MT_GEMLIB_X_H_
typedef struct
{
	long	magic;							/* 'pset' */
	long	length;							/* StrukturlÑnge */
	long	format;							/* Strukturtyp */
	long	reserved;

	long	page_flags;						/* Flags, u.a. gerade Seiten, ungerade Seiten */
	short	first_page;						/* erste zu druckende Seite */
	short	last_page;						/* letzte zu druckende Seite */
	short	no_copies;						/* Anzahl der Kopien */
	short	orientation;					/* Drehung */
	long	scale;							/* Skalierung: 0x10000L entspricht 100% */

	short	driver_id;						/* VDI-GerÑtenummer */
	short	driver_type;					/* Typ des eingestellten Treibers */
	long	driver_mode;					/* Flags, u.a. fÅr Hintergrunddruck */
	long	reserved1;
	long	reserved2;
	
	long	printer_id;						/* Druckernummer */
	long	mode_id;						/* Modusnummer */
	short	mode_hdpi;						/* horizontale Auflîsung in dpi */
	short	mode_vdpi;						/* vertikale Auflîsung in dpi */
	long	quality_id;						/* Druckmodus (hardwÑremÑûige QualitÑt, z.B. Microweave oder Econofast) */

	long	color_mode;						/* Farbmodus */
	long	plane_flags;					/* Flags fÅr auszugebende Farbebenen (z.B. nur cyan) */
	long	dither_mode;					/* Rasterverfahren */
	long	dither_value;					/* Parameter fÅr das Rasterverfahren */

	long	size_id;						/* Papierformat */
	long	type_id;						/* Papiertyp (normal, glossy) */
	long	input_id;						/* Papiereinzug */
	long	output_id;						/* Papierauswurf */

	long	contrast;						/* Kontrast: 0x10000L entspricht Normaleinstellung */
	long	brightness;						/* Helligkeit: 0x1000L entspricht Normaleinstellung */
	long	reserved3;
	long	reserved4;

	long	reserved5;
	long	reserved6;
	long	reserved7;
	long	reserved8;

	char	device[128];					/* Dateiname fÅr den Ausdruck */

	struct
	{
		char	inside[120];
	} mac_settings;
} PRN_SETTINGS;

typedef	void *PRN_DIALOG;

#ifndef __EVNT
#define __EVNT
typedef struct																/* Ereignisstruktur fÅr EVNT_multi(), Fensterdialoge, etc. */
{
	short mwhich;
	short mx;
	short my;
	short mbutton;
	short kstate;
	short key;
	short mclicks;
	short reserved[9];
	short msg[16];
} EVNT;
#endif


PRN_DIALOG *pdlg_create(WORD dialog_flags);
WORD pdlg_delete(PRN_DIALOG *prn_dialg);
WORD pdlg_open(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, const char *document_name, WORD option_flags, WORD x, WORD y);
WORD pdlg_close(PRN_DIALOG *prn_dialog, WORD *x, WORD *y);
WORD pdlg_update(PRN_DIALOG *prn_dialog, const char *document_name);
PRN_SETTINGS *pdlg_new_settings(PRN_DIALOG *prn_dialog);
WORD pdlg_free_settings(PRN_SETTINGS *settings);
WORD pdlg_evnt(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, EVNT *events, WORD *button);
void EVNT_multi(WORD evtypes, WORD nclicks, WORD bmask, WORD bstate, const MOBLK *m1, const MOBLK *m2, unsigned long ms, EVNT *event);

#endif
