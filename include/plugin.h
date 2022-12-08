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

/*-----------------------------------------------------------------	*/
/*	Strukturdefinitionen fÅr die PlugIn - Schnittstelle				*/
/*	fÅr SMURF Grafikkonverter (der immer mehr zu einem 				*/
/*	Betriebssytem wird...), 21.09.1997 22:55, Olaf Piesche			*/
/*																	*/
/*	Plugins Funktionsweise:											*/
/*		Bei SMURF-Programmstart werden die Plugins hochgeladen und	*/
/*		mit der Message MSTART zum Initialisieren wie Module		*/
/*		aufgerufen.	Dann kînnen sie sich in eine oder mehrere der	*/
/*		Funktionen in den Åbergebenen Strukturen PLUGIN_FUNCTIONS	*/
/*		und ADD_FUNCTIONS einhÑngen. 								*/
/*		Hierzu gibt es zwei Mîglichkeiten: Entweder ersetzt ein 	*/
/*		Plugin eine Funktion komplett, oder es hÑngt sich "hinten"	*/
/*		dran. Vergleichbar mit dem XBRA-Verfahren wird das Plugin	*/
/*		dazu in eine Liste eingetragen, Åber die es nach dem Aufruf	*/
/*		der eigentlichen Funktion aufgerufen wird. Wenn sich		*/
/*		mehrere Module an dieselbe Funktion anhÑngen wollen, werden	*/
/*		sie in der Reihenfolge aufgerufen, die sich aus der 		*/
/*		PrioritÑt in plugin_flag in PLUGIN_DATA ergibt.	Das setzt	*/
/*		Ñuûerst sorgfÑltige Programmierung und Verwaltung voraus!	*/
/*		Der Modulfunktion, die eingehÑngt 							*/
/*		wird, werden immer dieselben Parameter Åbergeben, wie der	*/
/*		eigentlichen Funktion. öber den Parameter plugin_flag kann	*/
/*		das Plugin seine gewÅnschte Position unabhÑngig von seiner	*/
/*		physikalischen Dateiposition angeben, an die es in der 		*/
/*		Branch-Liste fÅr die entsprechende Funktion gerne mîchte.	*/
/*		Bei SMURF-Programmende werden die Plugins nacheinander mit	*/
/*		MTERM aufgerufen, um sich zu deinitialisieren und 			*/
/*		terminieren.												*/
/*	Die dritte der zwei Mîglichkeiten ;-) :							*/
/*		Mit der Message MENU_ENTRY kann sich ein Plugin unter dem	*/
/*		erweiterten Namen ins Smurf-MenÅ "Plugin" eintragen lassen. */
/*		Bei Klick auf den entsprechenden MenÅeintrag wird die		*/
/*		Message PLGSELECTED an das entsprechende Plugin geschickt,	*/
/*		und es kann Loslegen (z.B. Fenster îffnen, etc.). Fenster-	*/
/*		Handling und Events (MBEVT, MWRED) laufen wie bei den 		*/
/*		Modulen ab, beim Fensterschlieûen wird allerdings nicht 	*/
/*		MTERM sondern MWINDCLOSED geschickt.						*/
/*		FIXED_PLUGIN ist das Zeichen fÅr Smurf, daû das Plugin fest */
/*		verdrahtet ist, wie z.B. Multikonvert und Print.			*/
/*																	*/
/*	Die Plugin-Schnittstelle wird nicht îffentlich dokumentiert.	*/
/*	Basta. Das ist einfach zuviel Arbeit fÅr den Programmierer und	*/
/*	vor allem fÅr mich, der ich die Dokumentation dazu schreiben	*/
/*	mÅûte ;-)														*/
/*-----------------------------------------------------------------	*/

/*----- Messages fÅr Plugins von Smurf*/
#define	MWINDCLOSED	257				/* Beim Fensterschlieûen kann nicht MTERM geschickt werden,
										weil das Plugin ja trotzdem weiterlaufen soll.	*/
#define	PLGSELECTED 258
#define PLG_STARTUP	259				/* fÅr die Startup-Kommunikation */

/*----- Messages fÅr Smurf von Plugins*/
#define	MENU_ENTRY		256			/* Plugin will ins Plugin-MenÅ */
#define	FIXED_PLUGIN	257			/* Plugin ist ein festverdrahtetes welches */
#define	INVISIBLE		258			/* Plugin ist nicht fÅr den User sichtbar */




/*------------------------------------------------------------------------------
	Struktur fÅr die Funktionen, die von den Plugins ersetzt oder aufgerufen
	werden kînnen. Dies sind sÑmtlich Funktionen aus dem SMURF-
	Hauptprogramm. Zum Ersetzen trÑgt das Plugin einfach die Adresse seiner Funktion
	in den entsprechenden Zeiger der Funktion ein.
	----------------------------------------------------------------------------*/
typedef struct
{
	/*
	 *	Funktionen zum Starten der Module 
	 */
	short		(*start_imp_module)(char *modpath, SMURF_PIC *imp_pic);
	EXPORT_PIC* (*start_exp_module)(char *modpath, short message, SMURF_PIC *pic_to_export, BASPAG *exbase, GARGAMEL *sm_struct, short mod_id);
	BASPAG*		(*start_edit_module)(char *modpath, BASPAG *edit_basepage, short mode, short mod_anz, GARGAMEL *smurf_struct);
	BASPAG*		(*start_dither_module)(short mode, short mod_id, DITHER_DATA *ditherdata);
	void 		(*check_and_terminate)(short mode, short module_number);
	const void *(*get_module_start)(BASPAG *basepage);

	/*
	 *	Transformers und Ditherroutinen
	 */
	short	(*tfm_std_to_std)(SMURF_PIC *smurf_pic, uint8_t dst_depth, uint8_t mode);
	short	(*tfm_std_to_pp)(SMURF_PIC *picture, uint8_t dst_depth, uint8_t mode);
	short	(*tfm_pp_to_std8)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_8_to_16)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_8_to_24)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_16_to_24)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_24_to_16)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_bgr_to_rgb)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_cmy_to_rgb)(SMURF_PIC *picture, uint8_t mode);
	short	(*tfm_rgb_to_grey)(SMURF_PIC *picture, uint8_t mode);

	short	(*dither_for_export)(const MOD_ABILITY *mod_abs, short max_expdepth, short dest_format, SMURF_PIC *converted_pic);
/*	short	(*dither)(SMURF_PIC *dest, SYSTEM_INFO *sys_info, short pic_changed); */
	short 	(*f_convert)(SMURF_PIC *picture, const MOD_ABILITY *mod_abs, uint8_t modcolform, uint8_t mode, uint8_t automatic);


	/*
	* Systemfunktionen (Eventhandler u.Ñ.)
	*/
	void	(*f_event)(void);				/* Allg. Event-Funktion */
	WORD	(*f_handle_message)(void);		/* AES-Message-Handler	*/
	void 	(*f_pic_event)(WINDOW *picwindow, short event_type, short windnum);

	/*
	* Dialogfunktionen
	*/
	void	(*check_windclose)(short windnum);	/* Auswerten von Windowdialog-AbbrÅchen */
	BOOLEAN	(*CallDialog)(WORD topwin);			/* Dialogdispatcher */
	WORD	(*init_dialog)(short DialogNumber, WORD DialogOK);
	void	(*close_dialog)(short windnum);

	void	(*f_module_prefs)(MOD_INFO *infostruct, short mod_id);	/* Modulformular aufrufen */
	void	(*f_mpref_change)(void);								/* Eingabe im Modulformular */
	void 	(*transform_pic)(void);
	void	(*close_window)(WORD handle);

	/*
	* Resource-Funktionen
	*/
	void	(*init_smurfrsc)(const char *rscpath);
	void	(*fix_rsc)(void);				/* RSC-Anpassung an OS, Farbtiefe, AES, etc... */
	void	(*f_init_popups)(void);		/* Alle Popups initialisieren */
	void	(*f_init_sliders)(void);	

	/*
	* Zeichenfunktionen (Blockbox, Fadenkreuz, etc.)
	*/
	void	(*f_draw_blockbox)(WINDOW *window);
	void	(*f_draw_crosshair)(WINDOW *window);		/* Positionierungskreuz in einem Bild zeichnen */


	/*
	* Fileroutinen
	*/
	struct DIRENTRY*	(*build_up_filelist)(char *path, char *ext, short pathlen);
	struct DIRENTRY*	(*insert_entry)(struct DIRENTRY *ende, char *string);
	void				(*destroy_filelist)(struct DIRENTRY *begin);

	/*
	*	Dateihandling mit Fileselector 
	*/
	BOOLEAN	(*file_save)(char *stext, uint8_t *buf, long length);
	void	(*file_load)(char *ltext, char **dateien, short mode);
	BOOLEAN (*f_fsbox)(char *Path, const char *fbtext, uint8_t selectart);
	char *	(*fload)(char *Path, short header);

	/* 
	*	Bild importieren und was dazugehîrt
	*/
	short (*f_import_pic)(SMURF_PIC *smurf_picture, char *filename);
	short (*f_formhandle)(short picture_to_load, short module_ret, char *namename);
	uint8_t *(*f_do_pcd)(const char *Path);

	/*
	*	Konfiguration und Smurf-Dateien
	*/
	void	(*save_config)(void);
	short	(*load_config)(void);

	void	(*destroy_smurfpic)(SMURF_PIC *pic);
} PLUGIN_FUNCTIONS;




/*-----------------------------------------------------------------------------
	Struktur fÅr Funktionen, an die sich Plugins anhÑngen kînnen. In die Zeiger
	mÅssen von den Plugins die Adressen der eigenen Funktionen eingetragen werden.
	Die Parameteranzahl und -Typ dÅrfen sich nicht unterscheiden, egal ob die 
	Parameter benîtigt werden oder nicht, sonst gibt's Stackrprobleme!
	----------------------------------------------------------------------------*/
typedef struct
{
	/*------- Funktionen zum Starten der Module ----------------*/
	short		(*start_imp_module)(char *modpath, SMURF_PIC *imp_pic);
	EXPORT_PIC*	(*start_exp_module)(char *modpath, short message, SMURF_PIC *pic_to_export, BASPAG *exbase, GARGAMEL *sm_struct, short mod_id);
	BASPAG*		(*start_edit_module)(char *modpath, BASPAG *edit_basepage, short mode, short mod_anz, GARGAMEL *smurf_struct);
	BASPAG*		(*start_dither_module)(short mode, short mod_id, DITHER_DATA *ditherdata);
} ADD_FUNCTIONS;




/*----------------------------------------------------------------------
	Struktur fÅr Smurf-Variablen, zum Zugriff oder éndern durch Plugins.
	------------------------------------------------------------------*/
typedef struct
{
	/*------------------ die komplette Resource -------------------*/
	/*   0 */ OBJECT 	*info_window;				/* Smurf-Info	*/
	/*   4 */ OBJECT 	*form_pop;					/* Farbtiefen-Popup	*/
	/*   8 */ OBJECT 	*edit_pop;					/* 	*/
	/*  12 */ OBJECT 	*col_pop;					/* Dithermodul-popup	*/
	/*  16 */ OBJECT 	*display_opt;				/* Display Options*/
	/*  20 */ OBJECT	*pic_form;					/* Pic-Window-Formular	*/
	/*  24 */ OBJECT	*pic_info_form;				/* Pic-Info-Formular	*/
	/*  28 */ OBJECT	*options_form;				/* Smurf-Options-Formular	*/
	/*  32 */ OBJECT	*alert_form;				/* WindAlert-Formular	*/
	/*  36 */ OBJECT	*module_form;				/* Modul-Einstell-Formular	*/
	/*  40 */ OBJECT	*busy_window;				/* Busywindow-Formular	*/
	/*  44 */ OBJECT	*modules_window;			/* Edit-Modullisten-Formular	*/
	/*  48 */ OBJECT	*newpic_window;				/* Neues-Bild-Formular	*/
	/*  52 */ OBJECT	*exmod_window;				/* Expormodul-Formular */
	/*  56 */ OBJECT	*form_picmanager;			/* Bild-Manager		*/
	/*  60 */ OBJECT	*export_form;				/* Export-Formular	*/
	/*  64 */ OBJECT	*exp_dp_popup;				/* Farbtiefen fÅr Export	*/
	/*  68 */ OBJECT	*picorder_popup;			/* Bildreihenfolge-Popup	*/
	/*  72 */ OBJECT	*colred_popup;				/* Palettenmodus-Popup */
	/*  76 */ OBJECT	*blockpopup;				/* Blockpopup */
	/*  80 */ OBJECT	*mod_information;			/* Edit-Modulinformation - Dialog */
	/*  84 */ OBJECT	*exmod_info;				/* Exportmodul - Info */
	/*  88 */ OBJECT	*image_order_window;		/* Bildreihenfolge-Window */
	/*  92 */ OBJECT	*transform_window;			/* Bild wandeln */
	/*  96 */ OBJECT	*blocktype_window;			/* Block eingeben */

	/* 100 */ OBJECT	*menu_tree;					/* MenÅleiste */
	/* 104 */ OBJECT 	*u_tree;					/* Radiobutton/Checkbox-Formular	*/
	/* 108 */ OBJECT	*alerts;					/* Alert-Strings */

	/* 112 */ char *loadpath;				/* voller Pfad der zuletzt geladenen Datei (256 Bytes) */
	/* 116 */ char *savepath;				/* voller Pfad der zuletzt gespeicherten Datei (256 Bytes) */
	/* 120 */ char *commpath;				/* voller Pfad der zuletzt Åber ein Protokoll empfangenen Datei (256 Bytes)*/
	/* 124 */ char *DraufschmeissBild;	/* D&D-Flag (1 Byte!) */

	/* 128 */ SYSTEM_INFO *Sys_info;			/* Systemkonfiguration */

	/* 132 */ WINDOW *wind_s;
	/* 136 */ WINDOW *picture_windows;
	/* 140 */ SMURF_PIC **smurf_picture;
	/* 144 */ GARGAMEL **smurf_struct;			/* öbergabestrukturen der Edit-Module */
	/* 148 */ BASPAG **edit_bp;

	/* 152 */ short *picwindthere;
	/* 156 */ short *dialwindthere;
	/* 160 */ short *picthere;
	/* 164 */ short *active_pic;
	
	/* 168 */ LIST_FIELD *export_list;
	/* 172 */ LIST_FIELD *edit_list;
	/* 176 */ LIST_FIELD *picture_list;
	/* 180 */ POP_UP *pop_ups;

	/* 184 */ const DITHER_MOD_INFO **ditmod_info;

	/* 188 */ EXPORT_CONFIG *exp_conf;
	/* 192 */ short anzahl_importmods;
	/* 194 */ short anzahl_editmods;
	/* 196 */ short anzahl_exporter;
	/* 198 */ short anzahl_dithermods;

	/* 200 */ char **edit_modules;			/* Pfade fÅr bis zu 100 Edit-Module */
	/* 204 */ char **edit_module_names;		/* Namen fÅr bis zu 100 Edit-Module */
	/* 208 */ char **export_modules;			/* Pfade fÅr bis zu 100 Export-Module */
	/* 212 */ char **export_module_names;		/* Namen fÅr bis zu 100 Export-Module */

	/* 216 */ char **export_cnfblock;			/* Konfigurationsblîcke fÅr die Exporter */
	/* 220 */ short *export_cnflen;			/* LÑnge des jeweiligen Blockes */

	/* 224 */ WORD *mouse_xpos;
	/* 228 */ WORD *mouse_ypos;
	/* 232 */ WORD *mouse_button;
	/* 236 */ WORD *klicks;
	/* 240 */ WORD *obj;
	/* 244 */ WORD *key_scancode;
	/* 248 */ WORD *key_ascii;
	/* 252 */ WORD *key_at_event;
	/* 256 */ 
} SMURF_VARIABLES;



/*--------------------------------------------------------------------
	Struktur fÅr die DatenÅbergabe und Kommunikation mit Plugins
	Die Funktionszeiger in den Strukturen replace und call sind von Smurf
	sÑmtlich initialisiert, das Modul muû nur noch die Funktionen,
	die es einhÑngen will, in die entsprechenden Zeiger eintragen. 
	Wichtig: Nur das Ñndern, wo auch wirklich was eingehÑngt werden soll!
	FÅr alle call-Funktionen gilt die Branch-Position in plugin_flag.
	Bei MSTART muû vom Plugin die Åbergebene Struktur in den Variablen
	plugin_name und ggfs. plugin_flag initialisiert werden.
	-----------------------------------------------------------------------*/
typedef struct
{
	/*  0 */ WINDOW *wind_struct;					/* evtl. Fenster des Plugins */
	/*  4 */ SERVICE_FUNCTIONS *services;			/* die Åblichen Dienstfunktionen... */

	/* vom Plugin Ñnderbarer Kram: */
	/*  8 */ PLUGIN_FUNCTIONS *replace;				/* Funktionen zum Ersetzen */
	/* 12 */ ADD_FUNCTIONS *call;					/* Funktionen zum DranhÑngen */
	/* 16 */ SMURF_VARIABLES *smurf_vars;			/* Smurf-Variablen fÅrs Modul */

	/* nur fÅr Lesezugriff durch Plugin: */
	/* 20 */ PLUGIN_FUNCTIONS *smurf_functions;	/* Smurf-Funktionen zum Aufrufen fÅrs Plugin */


	/* Kommunikation und Information fÅr Smurf */
	/* 24 */ short message;							/* Message */
	/* 26 */ WORD event_par[10];					/* Beim Event betroffenes Objekt und weitere Daten*/
	/* 46 */ WORD mousex,mousey;					/* Mauspos beim Event 			*/
	/* 50 */ WORD klicks;							/* Mausklicks beim Event 		*/

	/* 52 */ char	plugin_name[25];				/* erweiterter Plugin-Name f. MenÅtitel */
	/* 78 */ short	plugin_flag;					/* Bit 0+1: GewÅnschte Position in evtl.
														Branch-Liste:	%_0: egal,
														sonst			%01: ganz vorne,
																		%11: ganz hinten
														falls sich mehrere Plugins an dieselbe
														Funktion anhÑngen wollen.
													 */
	/* 80 */ short id;
	/* 82 */ void /* struct moduleObj */ *module_object;
	/* 86 */
} PLUGIN_DATA;



/*--------------------------------------------------------------
	Diese Struktur hÑngt im Plugin-Header und ermîglicht folgendes:
	Åber for_smurf_version kann Smurf einfach feststellen, ob ein Plugin Åberhaupt 
	fÅr ihn gedacht ist. öber name und plugin_version kann er feststellen, ob ein 
	Plugin zweimal in unterschiedlichen	Versionsnummern vorhanden ist, und ggfs. 
	das veraltete gleich abmelden. Diese beiden Punkte sind hier extrem wichtig, weil
	Plugins direkt in Smurf eingreifen. 
	Auûerdem wird es eine Dienstfunktion geben, mit der ein Plugin alle anderen aktiven 
	Plugins abfragen kann. Damit kann ein Plugin sich individuell auf andere einstellen 
	und sogar andere Plugins aufrufen (!).
	Somit ist diese Schnittstelle die flexibelste von allen, und das alles mit dieser
	kleinen Struktur...
	-----------------------------------------------------------------*/
typedef struct plginfo
{
	const char *name;
	int16_t	plugin_version;
	int16_t for_smurf_version;
	uint8_t resident;			/* Resident halten oder nicht? */
} PLUGIN_INFO;


/*
 * structures at start of text segments of plugins/modules
 */

typedef struct {
	unsigned short trap[2];  /* Pterm(0) */
	unsigned short entry[2]; /* bra.w plugin_main */
	unsigned long magic;
	const PLUGIN_INFO *info;
	unsigned long interface_version;
} PLUGIN_START;

typedef struct {
	unsigned short trap[2];  /* Pterm(0) */
	unsigned short entry[2]; /* bra.w dither_module_main */
	unsigned long magic;
	const DITHER_MOD_INFO *info;
	unsigned long interface_version;
} DITHER_START;

typedef struct {
	unsigned short trap[2];  /* Pterm(0) */
	unsigned short entry[2]; /* bra.w edit_module_main/imp_module_main/exp_module_main */
	unsigned long magic;
	const MOD_INFO *info;
	const MOD_ABILITY *ability;
	unsigned long interface_version;
} MODULE_START;

extern PLUGIN_INFO plugin_info ASM_NAME("plugin_info");

void plugin_main(PLUGIN_DATA *data) ASM_NAME("plugin_main");
