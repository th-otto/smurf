#ifndef _sym_gem_h_
#define _sym_gem_h_

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

/*  ---------------------------------------------------------------*/
/*  -------------------------- Globales ---------------------------*/
/*  ---------------------------------------------------------------*/
/* Scrollbares Listenfeld */
/* Ein Listenfeld muû folgendermaûen aufgebaut sein:        */
/*  Zuerst ein Parentobjekt fÅr die ListeneintrÑge aufziehen,   */
/*  z.B. eine BOX. In dieses dann max_entries TEXT-Objekte ein- */
/*  fÅgen, in denen bereits Platz fÅr den String ist (also z.B. */
/*  mit '_' auffÅllen). Die Text-Objcs mÅssen RADIOBUTTONS und  */
/*  eines davon selektiert sein. Neben dieses Objekt dann eine  */
/*  BOX als Parentobjekt fÅr den Slider (INDICATOR, OS_SELECTED)   */
/*  aufziehen, darin eine BOX (INDICATOR) als Slider selbst.    */
/*  DarÅber und darunter je ein Boxchar (INDICATOR) mit einem   */
/*  Pfeil nach oben und nach unten setzen. Und das wars. Die    */
/*  Struktur mit den entsprechenden Werten fÅllen und bei Klick */
/*  auf eines der Objekte (Slider, Pfeile, Sliderparent, Listen-*/
/*  eintrag) dieses und die Struktur an f_listfield Åbergeben.  */
/*  Fertig. Die Funktion srollt und redrawed das Listenfeld     */
/*  selbstÑndig ud gibt den selektierten Listeneintrag          */
/*  (incl. Scrolloffset) zurÅck.                                */
typedef char**  LISTPTR;

#include "portab.h"
#ifdef __PUREC__
#include <aes.h>
#else
#include <gem.h>
extern short _app;
#endif

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif
#ifndef BOOLEAN
#define BOOLEAN int
#endif

#ifndef WF_ICONIFY
#define WF_ICONIFY			26
#define WF_UNICONIFY		27
#define WF_UNICONIFYXYWH	28
#endif

#ifndef WM_ICONIFY
#define WM_ICONIFY			34
#define WM_UNICONIFY		35
#define WM_ALLICONIFY		36
#endif

#ifndef SMALLER
#define SMALLER				0x4000
#endif

#ifndef OS_SELECTED
#define OS_SELECTED SELECTED
#define OS_DISABLED DISABLED
#define OS_SHADOWED SHADOWED

#define OF_SELECTABLE SELECTABLE
#define OF_DEFAULT    DEFAULT
#define OF_EDITABLE   EDITABLE
#define OF_RBUTTON    RBUTTON
#define OF_HIDETREE   HIDETREE
#endif

typedef struct
{
    WORD number_entries;    /* Anzahl an EintrÑgen          */
    char *entry;            /* Zeiger auf ein Feld von EintrÑgen */
    WORD max_entries;       /* Anzahl an maximal auf einmal darzustellenden EintrÑgen */
    WORD parent_obj;        /* Index des Vaterobjektes der ListeneintrÑge */
    WORD slider_parent;     /* Parentobjekt des Sliders     */
    WORD slider_obj;        /* Sliderobjekt                 */
    WORD slar_up;           /* Slider-Arrow hoch            */
    WORD slar_dn;           /* Slider-Arrow runter          */
    WORD scroll_offset;     /* Scrolloffset der Liste       */
    char *autolocator;      /* Zeiger auf Autolocator-String */
    short auto_len;
} LIST_FIELD;


/* Popup-Struktur zu Verwaltung von Popups durch f_pop */
typedef struct
{
    WORD popup_tree;        /* Objektbaum des Popups */
    WORD item;              /* Item (=letzter Eintrag)      */
    OBJECT *display_tree;   /* Tree, in dems dargestellt werden soll */
    WORD display_obj;       /* Objekt in display_tree, bei dems dargestellt werden soll */
    WORD Cyclebutton;       /* Index des Cyclebuttons */
} POP_UP;



/*  ---------------------------------------------------------------*/
/*  ------------------- SYM_GEM Functions -------------------------*/
/*  ---------------------------------------------------------------*/

#ifndef __GEMLIB__
WORD appl_xgetinfo (WORD type, WORD *out1, WORD *out2, WORD *out3, WORD *out4);
WORD objc_sysvar(WORD ob_smode, WORD ob_swhich, WORD ob_sival1, WORD ob_sival2, WORD *ob_soval1, WORD *ob_soval2);
#endif


WORD button_ev(OBJECT *tree);                                /* Evnt-button-Erweiterung. */

short f_numedit(WORD obj, OBJECT *tree, WORD deflt);            /* Editpopup zum einfacheren Editieren von FTEXTs. */
WORD f_schiebe(WORD regler, WORD fhr, OBJECT *tree);           
WORD f_pop(POP_UP *popup_struct, WORD mouseflag, WORD button, OBJECT *poptree); /* Popupmanager */
void f_drag(WORD obj, WORD parent, OBJECT *tree);          	/* Usergesteuertes Draggen von Objekten in einem Formular. */
void f_hidetree(OBJECT *tree, WORD object);               	/* Verstecken eines Objektes mit allen Kindern */
void f_showtree(OBJECT *tree, WORD object);                   /* Wiederanzeigen eines versteckten Objekts mit allen Kindern */

WORD get_selected_object(OBJECT *tree, WORD first_entry, WORD last_entry);

void f_generate_listfield(WORD uparrow, WORD dnarrow, WORD sliderparent, 
            WORD sliderobject, WORD listparent, char *listentries, 
            WORD num_entries, WORD max_entries, LIST_FIELD *listfield, WORD autoloc);

WORD f_listfield(long *pwindow, WORD klick_obj, WORD key_scancode, LIST_FIELD *lfstruct);

#endif
