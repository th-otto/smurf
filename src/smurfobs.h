#ifndef _smurfobs_h_
#define _smurfobs_h_

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

/* ----------------------- Smurf-Objekte -----------------------*/
/* Dieses File enthÑlt Definitionen, die eine Objektorientierung
    des Smurf-Kerns mîglich machen. Wenn man so will, gibt es
    "Pseudoklassen" fÅr jeden Bereich (Module, Dithering, etc.)
    mit Unterklassen, die eine Unterteilung fÅr spezielle Aufgaben
    (Modulkommunikation, Modul-GUI) darstellen. Die Unterklassen
    enthalten dann die jeweiligen Routinen.
    Modulstart wird dann z.B. Åber module.comm.start_edit() erledigt.
    Genauso verhÑlt es sich mit den Dialogen innerhalb Smurfs, die
    Adressen der ObjektbÑume werden Åber eine Struktur angesprochen,
    in der die jeweiligen BÑume von Plugins ersetzt werden.

    Die Objektorientierung ist nîtig, um der Pluginschnittstelle die
    geplante FunktionalitÑt zu ermîglichen. Die Pluginschnittstelle
    Åbernimmt dann das Ersetzen der Routinen in den Objekten durch
    die ggfs. vom Plugin Åbermittelten Routinen, so daû diese
    Smurfweit und auch in den Modulen zur VerfÅgung stehen.
    --------------------------------------------------------------*/


/* modComm ---------------------------------------------
    EnthÑlt alle Kommunikationsrelevanten Modulfunktionen.
    -----------------------------------------------------*/
typedef struct modComm
{
    short (*start_imp_module)(char *modpath, SMURF_PIC *imp_pic);
    BASPAG *(*start_edit_module)(char *modpath, BASPAG *edit_basepage, short mode, short mod_id, GARGAMEL *sm_struct);
    EXPORT_PIC *(*start_exp_module)(char *modpath, short message, SMURF_PIC *pic_to_export, BASPAG *exbase, GARGAMEL *sm_struct, short mod_id);
    BASPAG *(*start_dither_module)(short mode, short mod_id, DITHER_DATA *ditherdata);
    void (*checkTerminate)(short mode, short module_number);

    short (*analyzeMessage)(short module_ret, short picture_to_load);
    void (*handleMessage)(GARGAMEL *smurf_struct);

    SMURF_PIC *(*getPic)(WORD num, short mod_id, MOD_INFO *mod_info, WORD depth, short form, short col);
    short (*givePics)(MOD_INFO *mod_info, MOD_ABILITY *mod_abs, short module_number);

    short (*inform)(short message, SMURF_PIC *picture);
    void (*transmitAESmsg)(WORD *msgbuf);
} MODCOMM;


/* modGUI ---------------------------------------------
    EnthÑlt alle das GUI betreffenden Modulfunktionen.
    ----------------------------------------------------*/
typedef struct modGUI
{
    short (*openWindow)(WINDOW *module_window);
    short (*handleEvent)(short event_type, WINDOW *mod_window);
    void (*walkTree)(WINDOW *wind, WORD start);
    void (*initTree)(OBJECT *tree, WORD index);
    void (*convertIcon)(OBJECT *tree, WORD index);
    void (*makePreview)(WINDOW *wind);
} MODGUI;


/* moduleObj ---------------------------------------
    globale "Klasse" fÅr Module
    -------------------------------------------------*/
typedef struct moduleObj
{
    MODCOMM comm;
    MODGUI gui;

    BASPAG  *bp[MAX_MODS];        /* Basepages der Module */
    GARGAMEL *smStruct[MAX_MODS]; /* öbergabestrukturen der Edit-Module */
} CLASS_MODULE;


/* windowOBJ ----------------------------------------
    allgemeine Windowfunktionen
    --------------------------------------------------*/
typedef struct generalWindow
{
    short (*open) (WINDOW *window);
    void (*redraw) (WINDOW *window, GRECT *mwind, WORD startob, WORD flags);
    void (*drawIconified) (WINDOW *window, WORD *vdiclip);

    int (*rcIntersect) (GRECT *r1, GRECT *r2, GRECT *r3);
    short (*myWindow) (WORD handle);
    WINDOW *(*myModuleWindow)(WORD handle);
    
    void (*windowToList) (WINDOW *window);
    void (*removeWindow) (WINDOW *window);
    void (*top) (WORD handle);
    void (*topNow) (WINDOW *window);
    void (*topHandle) (WORD handle);
    void (*close) (WORD handle);

    void (*cursorOn)(WINDOW *window);
    void (*cursorOff)(WINDOW *window);

    /* diese beiden sind Bindings - mÅssen vielleicht woanders hin
     */
    WORD (*windSet)(WORD wi_ghandle, WORD wi_gfield, WORD wi_gw1, WORD wi_gw2, WORD wi_gw3, WORD wi_gw4);
    WORD (*windGet)(WORD wi_ghandle, WORD wi_gfield, WORD *wi_gw1, WORD *wi_gw2, WORD *wi_gw3, WORD *wi_gw4);

    OBJECT *titles;
} GENERAL_WINDOW;


typedef struct unDo
{
    void (*saveBuffer)(SMURF_PIC *picture, int picnum);
    void (*swapBuffer)(SMURF_PIC *toPicture, int picNum);
} UNDO;


/* picwindowOBJ ------------------------------------
    Bildfenster betreffender Krempel
    -------------------------------------------------*/
typedef struct picWindow
{
    void (*drawBlock) (WINDOW *window, GRECT *picbox);
    void (*insertZoom) (WINDOW *window);
    void (*drawCrosshair) (WINDOW *window);
    void (*drawBlockbox) (WINDOW *window);
    int (*findCrosshair)(WINDOW *window);
    void (*arrowWindow) (int mode, WINDOW *wind,int amount);
    void (*slideWindow) (int pos, WINDOW *wind, int mode);
    void (*setSliders) (WINDOW *wind);
    void (*scrollRT)(WINDOW *window, int xamount, int yamount);

    void (*clipPicwin) (WINDOW *picw);
    void (*toggleAsterisk) (WINDOW *picwin, int onoff);
    void (*displayCoords)(WINDOW *pic_window, WORD mx, WORD my, BOOLEAN blockflag);
    void (*nullCoords)(WINDOW *picwindow);
    void (*removeBlock)(WINDOW *picwindow);
    void (*blockPop)(WINDOW *picwindow);

    UNDO undo;

    int topPicwin;
} IMAGEWINDOW;


/* picManager --------------------------------------
    Dialoghandling fÅr PicManager
    -------------------------------------------------*/
typedef struct picManager
{
    void (*handlePicman)(void);
    void (*insertPic)(short pic_to_insert);
    void (*showWH)(SMURF_PIC *pic);
    void (*cleanupList)(void);
    void (*navigateWindow)(void);
    BOOLEAN (*autoScroll)(WORD mx, WORD my);
    void (*makeThumbnail)(short picture_num);
    void (*drawPicmanBoxes) (void);

    OBJECT *tree;
    WINDOW *window;
    SMURF_PIC thumbnail;
    LIST_FIELD  pictureList;
    int picmanList[25];
    int selectedPic;
} PICTURE_MANAGER;


/* windowAlert -------------------------------------
    Handler fÅr die Windowalerts
    -------------------------------------------------*/
typedef struct windowAlert
{
    WORD (*openAlert)(char *alertstring, char *b1, char *b2, char *b3, WORD defbt);
    void (*closeAlert)(void);

    OBJECT  *alerts;
    BOOLEAN isTop;
    WORD winHandle;
} WIN_ALERT;


/* busyBox ---------------------------------
    Komplette Busybox mit allem was dazugehîrt
    -----------------------------------------*/
typedef struct busyBox
{
    short (*draw)(short lft);
    void (*reset)(short lft, const char *txt);
    void (*ok)(void);
    void (*dispRAM)(void);

    void (*disable)(void);
    void (*enable)(void);
    void (*fullDisable)(void);

    int noEvents;
    long resetClock;
    char disabled;
    OBJECT *busyTree;
} BUSYBOX;


/* editmodList ---------------------------------
    Editmodul-Listfenster mit allem dazugehîrigen Schnickschnack
    ---------------------------------------------*/
typedef struct editmodList
{
    void (*handleList)(void);
    void (*infoOff)(void);
    void (*infoOn)(short mod_index);

    char *modNames[100];            /* Namen fÅr bis zu 100 Edit-Module */
    LIST_FIELD  modList;
    OBJECT *tree;
    OBJECT *infoTree;
    int anzahl;
} EDITMOD_LIST;


/* exportList ----------------------------------
    Exportmodul-Listfenster
    ---------------------------------------------*/
typedef struct exportList
{
    void (*handleList)(void);
    void (*infoOn)(short mod_index);
    void (*infoOff)(void);

    char *modNames[100];            /* Namen fÅr bis zu 100 Edit-Module */
    LIST_FIELD  modList;
    OBJECT *tree;
    OBJECT *infoTree;
    int anzahl;
} EXPMOD_LIST;


/* smurfOptions ---------------------------------
    Optionsdialog
    ----------------------------------------------*/
typedef struct smurfOptions
{
    void (*options)(void);
    void (*setEnvconfig)(void);

    OBJECT *tree;   
} OPTIONS_DIALOG;


/* displayOptions ----------------------------------
    Displayoptions-Dialog
    -------------------------------------------------*/
typedef struct displayOptions
{
    void (*displayOptions)(void);
    void (*updateWindow)(int mode, int redraw);

    OBJECT *tree;
} DISPOPTS_DIALOG;


/* internalGuiOBJ -----------------------------------
    Smurf-interne GUI-Funktionen
    enthÑlt z.B. die Strukturen aller einzelnen Dialoge
    --------------------------------------------------*/
typedef struct dialogHandlers
{
    PICTURE_MANAGER picMan;
    BUSYBOX busy;
    WIN_ALERT winAlert;
    EDITMOD_LIST emodList;
    EXPMOD_LIST expmodList;
    OPTIONS_DIALOG smurfOpt;
    DISPOPTS_DIALOG dispOpt;

    WORD topDialog;

    WORD (*init)(short DialogNumber, WORD DialogOK);
    void (*close)(short windnum);
} DIALOGS;


typedef struct commHandlers
{
    short (*initOLGA)(void);
    short (*deinitOLGA)(void);
    short (*updateOLGA)(char *filename);
    short (*renameOLGA)(char *oldname, char *newname);
    
    short (*getDragdrop)(WINDOW *window_to_handle, WORD *messagebuf);
    short (*sendDragdrop)(SMURF_PIC *picture, WORD dest_whandle, WORD mx, WORD my);
    short (*ddGetheader)(DD_HEADER *dd_header, WORD pipe_handle);
    short (*ddSendheader)(WORD pipe_handle, long ext, long size, char *data_name, char *file_name);
    
    void (*updateDhst)(char *path);

    void (*getAvserv)(void);
    void (*initAVPROTO)(void);

    void (*initbubbleGem)(void);
    void (*deinitbubbleGem)(void);
    void (*bubbleGem)(WORD windownum, WORD xpos, WORD ypos, int modulemode);


    void (*sendAESMsg)(WORD dest_id, WORD msg, ...);

    AV_COMM avComm;
} COMMUNICATION;


/*
 * die dazugehîrigen Funktionen zum Initialisieren der Strukturen
 */
void init_moduleObs(void);          /* Modulobjekte initialisieren */
void init_GuiObs(void);             /* GUI-Objekte initialisieren */
void init_MiscObs(void);            /* allgemeine Objekte initialisieren */

#endif
