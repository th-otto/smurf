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

/*  -------------   Kratzer Entfernen V0.5  --------------- */
/*              F�r SMURF Bildkonverter, 19.10.95           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#include "scratch.rsh"					/* missing */




void f_doit(GARGAMEL * smurfstruct);


/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Kratzer entfernen",
	0x0050,
	"Olaf Piesche",
/* 4 Sliderueberschriften: max 8 */
	{ NULL, NULL, NULL, NULL },
/* 4 Checkboxueberschriften: */
	{ NULL, NULL, NULL, NULL },
/* 4 Edit-Objekt-�berschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	0,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};



MOD_ABILITY module_ability = {
/* Farbtiefen, die vom Modul unterst�tzt werden:            */
	16, 24, 0, 0, 0, 0, 0, 0,
/*  Dazugeh�rige Datenformate (FORM_PIXELPAK/FORM_STANDARD/FORM_BOTH) */
	1, 1, 1, 1, 1, 1, 1, 1,
	0									/* Wird ein Preview unterst�tzt (Edit)? 1=ja, 0=nein */
};

SLIDER *SliderXmat;
SLIDER *SliderYmat;
SLIDER *SliderStrength;
OBJECT *main;
WINDOW *mwindow;
SMURF_PIC *picture;
int xmatrix,
 ymatrix,
 strength;

static void (*set_slider)(SLIDER *sliderstruct, long value);


/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	WORD(*popup) (POP_UP *popup_struct, WORD mouseflag, WORD button);	/* Funktion deklarieren */
	short (*slider)(SLIDER *slider_struct);	/* Funktion deklarieren */
	void (*generate_listfield)(WORD uparrow, WORD dnarrow, WORD sliderparent, WORD sliderobject,
							   WORD listparent, char *listentries, WORD num_entries, WORD max_entries,
							   LIST_FIELD * listfield);
	void (*listfield)(OBJECT * tree, int klick_obj, LIST_FIELD * lfstruct);
	int given_val,
	 t,
	 back;
	int mod_id;
	int wind_num = 1;
	int mx,
	 my,
	 mw,
	 mh;
	int SmurfMessage;
	char wt[] = "Kratzer entfernen";
	int Button,
	 Mousex,
	 Mousey,
	 next;


	SmurfMessage = smurf_struct->module_mode;

/* Hier werden die Funktionen aus der GARGAMEL-Struktur geholt. */
	slider = smurf_struct->services->slider;	/* Sliderfunktion   */
	set_slider = smurf_struct->services->set_slider;


	main = rs_trindex[SCRATCH];			/* Resourcebaum holen */

	picture = smurf_struct->smurf_pic;


/* Wenn das Modul zum ersten Mal gestartet wurde */
	if (SmurfMessage == MSTART)
	{

		/* Resource Umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(&rs_object[t], 0);

		/* Speicher f�r die Sliderstrukturen anfordern */
		SliderXmat = Malloc(sizeof(SLIDER));
		SliderYmat = Malloc(sizeof(SLIDER));
		SliderStrength = Malloc(sizeof(SLIDER));

		mod_id = smurf_struct->module_number;	/* Welche ID habe ich?  */
		mwindow = (WINDOW *) Malloc(sizeof(WINDOW) + 5);
		mwindow->whandlem = 0;			/* evtl. Handle l�schen */
		mwindow->module = mod_id;		/* ID in die Fensterstruktur eintragen  */
		strcpy(mwindow->wtitle, wt);	/* Titel reinkopieren   */
		mwindow->wnum = wind_num;		/* Fenster nummer 1...  */
		mwindow->wx = 50;				/* Fenster X-...    */
		mwindow->wy = 50;				/* ...und Y-Pos     */
		mwindow->ww = main->ob_width;	/* Fensterbreite    */
		mwindow->wh = main->ob_height;	/* Fensterh�he      */
		mwindow->resource_form = main;	/* Resource         */
		mwindow->picture = 0;			/* kein Bild.       */
		mwindow->editob = 0;

		smurf_struct->wind_struct = mwindow;	/* und die Fensterstruktur in die Gargamel */

		/* Sliderstrukturen f�llen */
		SliderXmat->regler = XSLIDER;
		SliderXmat->schiene = XSLIDER_FHR;
		SliderXmat->rtree = main;
		SliderXmat->txt_obj = XSLIDER_TEXT;
		SliderXmat->min_val = 2;
		SliderXmat->max_val = 32;
		SliderXmat->window = mwindow;

		SliderYmat->regler = YSLIDER;
		SliderYmat->schiene = YSLIDER_FHR;
		SliderYmat->rtree = main;
		SliderYmat->txt_obj = YSLIDER_TEXT;
		SliderYmat->min_val = 2;
		SliderYmat->max_val = 32;
		SliderYmat->window = mwindow;

		SliderStrength->regler = SSLIDER;
		SliderStrength->schiene = SSLIDER_FHR;
		SliderStrength->rtree = main;
		SliderStrength->txt_obj = SSLIDER_TEXT;
		SliderStrength->min_val = 1;
		SliderStrength->max_val = 255;
		SliderStrength->window = mwindow;

		back = smurf_struct->services->f_module_window(mwindow);	/* Gib mir 'n Fenster! */

		/* Slider auf Defaults setzen */
		set_slider(SliderStrength, 90);
		set_slider(SliderXmat, 2);
		set_slider(SliderYmat, 2);

		if (back == -1)
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */

		return;
	}

	/* Buttonevent im Modulfenster */
	else if (SmurfMessage == MBEVT)
	{
		Button = smurf_struct->event_par[0];
		Mousex = smurf_struct->mousex;
		Mousey = smurf_struct->mousey;

		if (Button != -1 && Button != 0)
			form_button(main, Button, 1, &next);

		if (Button == XSLIDER)
			xmatrix = slider(SliderXmat);
		else if (Button == YSLIDER)
			ymatrix = slider(SliderYmat);
		else if (Button == SSLIDER)
			strength = slider(SliderStrength);
		else if (Button == STARTIT)
		{
			f_doit(smurf_struct);
			smurf_struct->module_mode = M_PICDONE;	/* Ich warte... */
		} else
			smurf_struct->module_mode = M_WAITING;	/* Ich warte... */

		return;
	}


	/* Mterm empfangen - Speicher freigeben und beenden */
	else if (SmurfMessage == MTERM)
	{
		Mfree(mwindow);
		Mfree(SliderXmat);
		Mfree(SliderYmat);
		Mfree(SliderStrength);
		smurf_struct->module_mode = M_EXIT;
		return;
	}



}






void f_doit(GARGAMEL *smurfstruct)
{
	SMURF_PIC *picture;
	int width,
	 height;
	int x,
	 y,
	 div;
	long xm,
	 ym;
	char red1,
	 red2,
	 red3;
	char green1,
	 green2,
	 green3;
	char blue1,
	 blue2,
	 blue3;
	long pix1,
	 pix2,
	 pix3,
	 diff;
	unsigned char *pic_data,
	*pdcopy;
	long redd,
	 greend,
	 blued;
	long lr,
	 lg,
	 lb;
	long rr,
	 rg,
	 rb;
	long ur,
	 ug,
	 ub;
	long dr,
	 dg,
	 db;
	long bperz,
	 yoff,
	 count = 0L;
	int widthcontrol;
	int heigthcontrol;


	picture = smurfstruct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;

	pic_data = pdcopy = picture->pic_data;

	bperz = (long) width *3L;


	/* Vertikalen Kratzer weginterpolieren */
	for (y = 0; y < height - 1; y++)
	{
		count += 64;
		if (!(y & 31))
			smurfstruct->busybox(count / (long) height);

		for (x = 0; x < width - 1; x++)
		{
			/* Farbwerte rausholen */
			red1 = *(pic_data);
			green1 = *(pic_data + 1);
			blue1 = *(pic_data + 2);

			red2 = *(pic_data + 3);
			green2 = *(pic_data + 4);
			blue2 = *(pic_data + 5);

			/* Pixeldurchschnitt ausrechnen */
			pix1 = (red1 + green1 + blue1) / 3L;
			pix2 = (red2 + green2 + blue2) / 3L;

			div = 0;
			/*  linker Rand (RPixel ist wei�) */
			if (labs(pix2 - pix1) > strength && pix1 < pix2)	/* Grenzwert �berschritten ? */
			{
/*          /* Breite kontrollieren */
				widthcontrol = 0;
				pdcopy = pic_data + 3;
				do
				{
					red2 = *(pdcopy);
					green2 = *(pdcopy + 1);
					blue2 = *(pdcopy + 2);
					widthcontrol++;
					pdcopy += 3;
				} while (labs(pix1 - (red2 + green2 + blue2) / 3L) > strength && widthcontrol < xmatrix);

				if (widthcontrol > xmatrix)
					break;
				*/redd = greend = blued = 0;
				for (xm = -12; xm < -3; xm += 3)
				{
					redd += *(pic_data + xm);
					greend += *(pic_data + xm + 1);
					blued += *(pic_data + xm + 2);
				}

				*(pic_data + 3) = (redd + red1) / 4L;
				*(pic_data + 4) = (greend + green1) / 4L;
				*(pic_data + 5) = (blued + blue1) / 4L;
			}



			/*  rechter Rand (MPixel ist wei�) */
			if (labs(pix1 - pix2) > strength && pix1 > pix2)	/* Grenzwert �berschritten ? */
			{

/*          widthcontrol=0;
            pdcopy=pic_data;
            do{
                red2=*(pdcopy);
                green2=*(pdcopy+1);
                blue2=*(pdcopy+2);
                widthcontrol++;
                pdcopy-=3;
            } while( labs( pix1 - (red2+green2+blue2)/3L) > strength && widthcontrol<xmatrix );

            if(widthcontrol>xmatrix) break; */

				redd = greend = blued = 0;
				for (xm = 12; xm > 3; xm -= 3)
				{
					redd += *(pic_data + xm);
					greend += *(pic_data + xm + 1);
					blued += *(pic_data + xm + 2);
				}

				*(pic_data) = (redd + red1) / 4L;
				*(pic_data + 1) = (greend + green1) / 4L;
				*(pic_data + 2) = (blued + blue1) / 4L;
			}

			pic_data += 3;

		}

		pic_data += 3;					/* Zeilenende */
	}


	pic_data = pdcopy = picture->pic_data;


	/* Horizontalen Kratzer weginterpolieren */
	for (y = 0; y < height - 1; y++)
	{
		count += 64;
		if (!(y & 31))
			smurfstruct->busybox(count / (long) height);

		for (x = 0; x < width - 1; x++)
		{
			/* Farbwerte rausholen */
			red1 = *(pic_data);
			green1 = *(pic_data + 1);
			blue1 = *(pic_data + 2);

			red3 = *(pic_data + bperz);
			green3 = *(pic_data + 1 + bperz);
			blue3 = *(pic_data + 2 + bperz);

			/* Pixeldurchschnitt ausrechnen */
			pix1 = (red1 + green1 + blue1) / 3L;
			pix3 = (red3 + green3 + blue3) / 3L;

			div = 0;
			/*  oberer Rand (UPixel ist wei�) */
			if (labs(pix1 - pix3) > strength && pix1 < pix3)	/* Grenzwert �berschritten ? */
			{
				redd = greend = blued = 0;
				for (ym = -4; ym < -1; ym++)
				{
					yoff = ym * bperz;
					redd += *(pic_data + yoff);
					greend += *(pic_data + yoff + 1);
					blued += *(pic_data + yoff + 2);
				}
				*(pic_data + bperz) = (redd + red1) / 4L;
				*(pic_data + bperz + 1) = (greend + green1) / 4L;
				*(pic_data + bperz + 2) = (blued + blue1) / 4L;
			}



			/*  unterer Rand (MPixel ist wei�) */
			if (labs(pix3 - pix1) > strength && pix1 > pix3)	/* Grenzwert �berschritten ? */
			{
				redd = greend = blued = 0;
				for (ym = 4; ym > 1; ym--)
				{
					yoff = ym * bperz;
					redd += *(pic_data + yoff);
					greend += *(pic_data + yoff + 1);
					blued += *(pic_data + yoff + 2);
				}
				*(pic_data) = (redd + red1) / 4L;
				*(pic_data + 1) = (greend + green1) / 4L;
				*(pic_data + 2) = (blued + blue1) / 4L;
			}



			pic_data += 3;

		}

		pic_data += 3;					/* Zeilenende */
	}
}
