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

/*
      /---------------- Soft-Tex-Shadow-Modul V1.1 ---------------\
      |                                                           |
      |               F�r SMURF Bildkonverter, 26.04.96           |    
      |                                                           |
      |     Wirft einen weichen Schatten auf ein beliebiges       |
      |       Hintergrundbild (wenn zu klein wird es gekachelt).  |
      |                                                           |
      |       last change: 3.04.97                                |
      \___________________________________________________________/
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY == 1
#define STR_STRENGTH "St�rke"
#define STR_SOFT "Weich"
#define STR_SIZE "Gr\224\236e"
#define STR_ROUNDED "Abgerundet"
#define STR_XDIST "X-Abstand"
#define STR_YDIST "Y-Abstand"
#elif COUNTRY == 0
#define STR_STRENGTH "Strength"
#define STR_SOFT "Soft"
#define STR_SIZE "Size"
#define STR_ROUNDED "Rounded"
#define STR_XDIST "X-dist"
#define STR_YDIST "Y-dist"
#else
#define STR_STRENGTH "Strength"
#define STR_SOFT "Soft"
#define STR_SIZE "Size"
#define STR_ROUNDED "Rounded"
#define STR_XDIST "X-dist"
#define STR_YDIST "Y-dist"
#endif

/*------------------------ Funktionen -----------------------*/
/*-----------------------------------------------------------*/
/*-----> Smurf                            */

/*-----> Modul                            */


/*--------------- Globale Variablen --------------------------*/
/*               -------------------                                         */
static unsigned long busycount;
static unsigned long  busymax;
static unsigned long busycall;

static SMURF_PIC *input_pictures[3];

static char p1string[] = "Shadowpic";
static char p2string[] = "Background";

/*--------------- Infostruktur f�r Hauptprogramm --------------*/
/*               --------------------------------                */
MOD_INFO module_info = {
	"Drop Tex-Shadow",					/* Name des Moduls */
	0x0110,
	"J�rg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen f�r Importer */
/* 4 Slider�berschriften: max 8 */
	{ STR_STRENGTH, STR_SOFT, STR_XDIST, STR_YDIST },
/* 4 Checkbox�berschriften: */
	{ "", "", "", "" },
/* 4 Edit-Objekt-�berschriften: */
	{ "", "", "", "" },
/* min/max-Werte f�r Slider */
	{ { 1, 255 }, { 1, 20 }, { -30, 30 }, { -30, 30 } },
/* min/max f�r Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte f�r Slider, Check und Edit */
	{ 128, 5, 10, 10 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	2,
	COMPILER_ID,
	{ p1string, p2string, NULL, NULL, NULL, NULL }
};


/*--------------------- Was kann ich ? ----------------------*/
MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,			/* Farbtiefen */
	/* Grafikmodi: */
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0									/* Extra-Flag */
};





/*--------------- Soft-Shadow ----------------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	SMURF_PIC *texture;
	short width, height;
	short strengh;
	short softness;
	uint8_t *pic;
	uint8_t *greypic;
	uint8_t *tcyoffset;
	uint8_t *tcoffset;
	uint8_t *offset;
	uint8_t *moffset;
	uint8_t *mxoffset;
	short twidth, theight;
	uint8_t *texpic;
	long bpl, tbpl;
	short x, y;
	short mx, my;
	short tx, ty;
	short mgx_o, mgy_o;
	short x_start, x_end, y_start, y_end;
	short mw, mh;
	long shadowfak, shadowcount;
	uint8_t grey, mred, mgreen, mblue;
	uint8_t v;

	short radius;
	short x_quad, y_quad;

	short mx_s[50];
	short mx_e[50];
	short mx_cs, mx_ce, my_cs;
	short x_o, y_o;

	/*--- Slider auslesen ---------------------- */

	strengh = (short) smurf_struct->slide[0];
	if (strengh == 0)
		return M_PICDONE;				   /*--- Keine �nderung --> BEENDEN --------*/

	softness = (short) smurf_struct->slide[1];

	x_o = (short) smurf_struct->slide[2];
	y_o = (short) smurf_struct->slide[3];

	mw = mh = softness * 2 + 1;


	/*--- Radius-Maske generieren ------------------------*/
	shadowcount = 0;
	for (y = 0; y < mh; y++)
	{
		y_quad = (y - softness) * (y - softness);
		x = 0;
		do
		{
			x_quad = (x - softness) * (x - softness);
			radius = 0.5 + sqrt(x_quad + y_quad);
			x++;
		} while (radius > softness);
		mx_s[y] = x - 1;

		do
		{
			x_quad = (x - softness) * (x - softness);
			radius = 0.5 + sqrt(x_quad + y_quad);
			x++;
		} while ((radius <= softness) && (x <= mw));
		mx_e[y] = x - 1;
		shadowcount += mx_e[y] - mx_s[y];
	}

	shadowfak = 65535L / shadowcount;


	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	bpl = width * 3L;

	texture = input_pictures[1];
	twidth = texture->pic_width;
	theight = texture->pic_height;
	texpic = texture->pic_data;

	tbpl = twidth * 3L;

/*---GRAU-BILD generieren ---------------------------------*/

	mred = *(pic);
	mgreen = *(pic + 1);
	mblue = *(pic + 2);

	greypic = (uint8_t *)Malloc((long) width * height);
	if (greypic == NULL)
	{
		return M_MEMORY;
	}

	offset = pic;
	moffset = greypic;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (offset[0] != mred || offset[1] != mgreen || offset[2] != mblue)
			{
				*(moffset++) = strengh;
			} else
			{
				*(moffset++) = 0;
			}
			offset += 3;
		}
	}

/*--- Texture auf Schattenbild mappen -----------------------*/

	for (y = 0; y < height; y++)
	{
		ty = y % theight;
		offset = pic + bpl * y;
		tcoffset = texpic + tbpl * ty;
		moffset = greypic + (long) y *width;

		tx = 0;
		for (x = 0; x < width; x++)
		{
			if (tx == twidth)
			{
				tx = 0;
				tcoffset = texpic + tbpl * ty;
			}

			if (*(moffset++) == 0)
			{
				*(offset++) = *(tcoffset++);
				*(offset++) = *(tcoffset++);
				*(offset++) = *(tcoffset++);
			} else
			{
				offset += 3;
				tcoffset += 3;
			}


			tx++;
		}
	}


	/*--- Pre-Calcs ---------------------------------------*/

	x_start = softness + x_o;
	if (x_start < 0)
		x_start = 0;
	x_end = width - softness + x_o;
	if (x_end > width)
		x_end = width;

	y_start = softness + y_o;
	if (y_start < 0)
		y_start = 0;
	y_end = height - softness + y_o;
	if (y_end > height)
		y_end = height;



	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 1;
	busymax = height;
	if (busymax > 32)
		busycall = 3;
	if (busymax > 256)
		busycall = 7;
	busymax = (busymax << 10) / 127;


	/*--- Hauptroutine --------------------*/

	for (y = y_start; y < y_end; y++)
	{
		busycount++;
		if (!(busycount & busycall))
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		/*--- main-part -----------------*/
		tcyoffset = pic + (long) y *bpl;
		offset = greypic + (long) y *width + (long) x_start;

		for (x = x_start; x < x_end; x++)
		{
			if (*(offset++) == 0)
			{
				mgx_o = x - x_o - softness;
				mgy_o = y - y_o - softness;
				shadowcount = 0;
				for (my = 0; my < mh; my++)
				{
					mxoffset = greypic + (long) (mgy_o + my) * width;
					for (mx = mx_s[my]; mx < mx_e[my]; mx++)
					{
						shadowcount += *(mxoffset + (long) (mgx_o + mx));
					}
				}

				grey = 255 - ((shadowcount * shadowfak) >> 16);

				tcoffset = tcyoffset + x * 3L;

				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;
				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;
				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;

			}
		}

		/*--- left-part -------------------------------------*/
		tcyoffset = pic + (long) y *bpl;
		offset = greypic + (long) y *width;

		for (x = 0; x < x_start; x++)
		{
			if (*(offset++) == 0)
			{
				shadowcount = 0;
				mgx_o = x - x_o - softness;
				mgy_o = y - y_o - softness;
				for (my = 0; my < mh; my++)
				{
					if ((mgx_o + mx_e[my]) > 0)
					{
						mx_cs = mx_s[my];
						if ((mgx_o + mx_cs) < 0)
							mx_cs -= (mgx_o + mx_cs);

						mxoffset = greypic + (long) (mgy_o + my) * width;
						for (mx = mx_cs; mx < mx_e[my]; mx++)
						{
							shadowcount += *(mxoffset + (long) (mgx_o + mx));
						}
					}
				}

				grey = 255 - ((shadowcount * shadowfak) >> 16);

				tcoffset = tcyoffset + x * 3L;

				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;
				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;
				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;

			}
		}

		/*--- right-part ------------------------------------*/
		tcyoffset = pic + (long) y *bpl;
		offset = greypic + (long) y *width + x_end;

		for (x = x_end; x < width; x++)
		{
			if (*(offset++) == 0)
			{
				shadowcount = 0;
				mgx_o = x - x_o - softness;
				mgy_o = y - y_o - softness;
				for (my = 0; my < mh; my++)
				{
					if ((mgx_o + mx_s[my]) < width)
					{
						mx_cs = mx_e[my];
						if ((mgx_o + mx_cs) > width)
							mx_cs -= (width - (mgx_o + mx_cs));

						mxoffset = greypic + (long) (mgy_o + my) * width;
						for (mx = mx_s[my]; mx < mx_cs; mx++)
						{
							shadowcount += *(mxoffset + (long) (mgx_o + mx));
						}
					}
				}

				grey = 255 - ((shadowcount * shadowfak) >> 16);

				tcoffset = tcyoffset + x * 3L;

				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;
				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;
				v = ((*(tcoffset) * grey) >> 8);
				*(tcoffset++) = v;

			}
		}

	} /*--- ende Y-schleife ----------------*/


	/*--- upper-part ---------------------------------------*/
	for (y = 0; y < y_start; y++)
	{
		busycount++;
		if (!(busycount & busycall))
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		mgy_o = y - y_o - softness;
		if ((mgy_o + mh) > 0)
		{
			tcyoffset = pic + (long) y *bpl;
			offset = greypic + (long) y *width;

			for (x = 0; x < width; x++)
			{
				if (*(offset++) == 0)
				{
					shadowcount = 0;
					mgx_o = x - x_o - softness;

					my_cs = 0;
					if ((mgy_o + my_cs) < 0)
						my_cs -= (mgy_o + my_cs);

					for (my = my_cs; my < mh; my++)
					{
						if (((mgx_o + mx_e[my]) > 0) || ((mgx_o + mx_s[my]) < width))
						{
							mx_cs = mx_s[my];
							if ((mgx_o + mx_cs) < 0)
								mx_cs -= (mgx_o + mx_cs);
							mx_ce = mx_e[my];
							if ((mgx_o + mx_ce) > width)
								mx_ce += width - (mgx_o + mx_ce);

							mxoffset = greypic + (long) (mgy_o + my) * width;
							for (mx = mx_cs; mx < mx_ce; mx++)
							{
								shadowcount += *(mxoffset + (long) (mgx_o + mx));
							}
						}
					}

					grey = 255 - ((shadowcount * shadowfak) >> 16);

					tcoffset = tcyoffset + x * 3L;

					v = ((*(tcoffset) * grey) >> 8);
					*(tcoffset++) = v;
					v = ((*(tcoffset) * grey) >> 8);
					*(tcoffset++) = v;
					v = ((*(tcoffset) * grey) >> 8);
					*(tcoffset++) = v;

				}
			}

		}

	}									/* Y-schleife ---------------- */



	/*--- lower-part ---------------------------------------*/
	for (y = y_end; y < height; y++)
	{
		busycount++;
		if (!(busycount & busycall))
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		mgy_o = y - y_o - softness;
		if (mgy_o < height)
		{
			tcyoffset = pic + (long) y *bpl;
			offset = greypic + (long) y *width;

			for (x = 0; x < width; x++)
			{
				if (*(offset++) == 0)
				{
					shadowcount = 0;
					mgx_o = x - x_o - softness;

					my_cs = mh;
					if ((mgy_o + my_cs) > height)
						my_cs += height - (mgy_o + my_cs);

					for (my = 0; my < my_cs; my++)
					{
						if (((mgx_o + mx_e[my]) > 0) || ((mgx_o + mx_s[my]) < width))
						{
							mx_cs = mx_s[my];
							if ((mgx_o + mx_cs) < 0)
								mx_cs -= (mgx_o + mx_cs);
							mx_ce = mx_e[my];
							if ((mgx_o + mx_ce) > width)
								mx_ce += width - (mgx_o + mx_ce);

							mxoffset = greypic + (long) (mgy_o + my) * width;
							for (mx = mx_cs; mx < mx_ce; mx++)
							{
								shadowcount += *(mxoffset + (long) (mgx_o + mx));
							}
						}
					}

					grey = 255 - ((shadowcount * shadowfak) >> 16);

					tcoffset = tcyoffset + x * 3L;

					v = ((*(tcoffset) * grey) >> 8);
					*(tcoffset++) = v;
					v = ((*(tcoffset) * grey) >> 8);
					*(tcoffset++) = v;
					v = ((*(tcoffset) * grey) >> 8);
					*(tcoffset++) = v;

				}
			}

		}

	}									/* Y-schleife ---------------- */



	Mfree(greypic);

	smurf_struct->event_par[0] = 0;
	smurf_struct->event_par[1] = -1;

	return M_PICDONE;
}


/*-----------------------  FUNCTION MAIN --------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul aufgerufen wurde, */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;
	case MPICTURE:
		/* Bild holen */
		input_pictures[smurf_struct->event_par[0]] = smurf_struct->smurf_pic;

		/* und weiter warten */
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Wenn das Modul gestartet wurde */
	case MEXEC:
		smurf_struct->module_mode = do_it(smurf_struct);
		break;

	/* Wenn das Modul sich verpissen soll */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}


/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel gro�es Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview mu� in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* �bernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul mu� nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierf�r bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* f�r die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);
void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}
