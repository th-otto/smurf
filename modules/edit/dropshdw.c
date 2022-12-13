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

/*  ----------- Drop Shadow-Modul V1.21 ---------------  */
/*          FÅr SMURF Bildkonverter, 26.04.96           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY == 1
#define STR_STRENGTH "StÑrke"
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

/*--------------- Funktionen -------------------*/
/*----------------------------------------------*/
/*-----> Smurf --------*/
void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);

/*--------------- Globale Variablen --------------------------*/
static unsigned long busycount;
static unsigned long busymax;
static unsigned long busycall;


/*--------------- Infostruktur fÅr Hauptprogramm -----*/
MOD_INFO module_info = {
	"Drop Shadow",						/* Name des Moduls */
	0x0200,
	"J\224rg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen fÅr Importer */
/* 4 SliderÅberschriften: max 8 */
	{ STR_STRENGTH, STR_SOFT, STR_SIZE, "" },
/* 4 CheckboxÅberschriften: */
	{ STR_ROUNDED, "", "", "" },
/* 4 Edit-Objekt-öberschriften: */
	{ STR_XDIST, STR_YDIST, "", "" },
/* min/max-Werte fÅr Slider */
	{ { 1, 255 }, { 1, 20 }, { -64, 64 }, { 0, 0 } },
/* min/max fÅr Editobjekte */
	{ { -4096, 4096 }, { -4096, 4096 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fÅr Slider, Check und Edit */
	{ 128, 3, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 5, 5, 0, 0 },
	1,
	COMPILER_ID,
	{ "", NULL, NULL, NULL, NULL, NULL }
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




/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel groûes Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview muû in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* Åbernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul muû nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierfÅr bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* fÅr die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}



/*--------------- Soft-Shadow ----------------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short width, height;
	short strength;
	short softness;
	uint8_t *pic;
	uint8_t *greypic;
	uint8_t *tcyoffset;
	uint8_t *tcoffset;
	uint8_t *offset;
	uint8_t *moffset;
	uint8_t *mxoffset;
	long bpl;
	short x;
	short y;
	short mx, my;
	short mgx_o, mgy_o;
	short x_start;
	short x_end;
	short y_start;
	short y_end;
	short mw, mh;
	long shadowcount;
	uint8_t mred, mgreen, mblue;

	short radius;
	short x_quad, y_quad;

	short mx_s[50];
	short mx_e[50];
	short mx_ce;
	short my_cs;
	short x_o, y_o;
	uint8_t *redbuf;
	uint8_t *greenbuf;
	uint8_t *bluebuf;
	short mx_cs;
	short size;
	long shadowfak;

	/*--- Slider auslesen ---------------------- */

	strength = (short) smurf_struct->slide[0];
	if (strength == 0)
		return M_PICDONE;				   /*--- Keine énderung --> BEENDEN --------*/

	softness = (short) smurf_struct->slide[1];

	size = (short) smurf_struct->slide[2];
	x_o = (short) smurf_struct->edit[0];
	y_o = (short) smurf_struct->edit[1];

	mw = mh = softness * 2 + 1;

	if (smurf_struct->check[0])
	{
		/* rounded */
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
			} while (radius <= softness && x <= mw);
			mx_e[y] = x - 1;
			shadowcount += mx_e[y] - mx_s[y];
		}
	} else
	{
		/*--- Radius-Maske generieren ------------------------*/
		shadowcount = 0;
		for (y = 0; y < mh; y++)
		{
			mx_s[y] = 0;
			mx_e[y] = mw - 1;
			shadowcount += mw;
		}
	}

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	bpl = width * 3L;

	/*---GRAU-BILD generieren ---------------------------------*/
	mred = 255;
	mgreen = 255;
	mblue = 255;

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
				*(moffset++) = 1;
			} else
			{
				*(moffset++) = 0;
			}
			offset += 3;
		}
	}

	redbuf = (uint8_t *)Malloc((shadowcount + 1) * 3L * sizeof(*redbuf));
	if (redbuf == NULL)
	{
		Mfree(greypic);
		return M_MEMORY;
	}
	greenbuf = redbuf + shadowcount + 1;
	bluebuf = greenbuf + shadowcount + 1;

	/*--- Pre-Calcs ---------------------------------------*/

	{
		double strengthfak;
	
		strengthfak = strength / (shadowcount / 2.0);
		shadowfak = (1.0 - (size + 64.0) / 128.0) * (shadowcount / 2.0);
		for (x = 0; x <= shadowcount; x++)
		{
			short xx = (x - (double)shadowfak) * strengthfak;
			if (xx < 0)
				xx = 0;
			if (xx > strength)
				xx = strength;
			redbuf[x] = 255 - xx;
			greenbuf[x] = 255 - xx;
			bluebuf[x] = 255 - xx;
		}
	}

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
	busycall = 31;
	busymax = y_end - y_start;
	if (busymax > 32)
		busycall = 15;
	if (busymax > 256)
		busycall = 1;
	busymax = (busymax << 10) / 127;

	/*--- Hauptroutine --------------------*/

	for (y = y_start; y < y_end; y++)
	{
		busycount++;
		if ((busycount & busycall) == 0)
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		/*--- main-part -----------------*/
		tcyoffset = pic + (long) y * bpl;
		offset = greypic + (long) y * width + (long) x_start;

		for (x = x_start; x < x_end; x++)
		{
			if (*(offset++) == 0)
			{
				mgx_o = x - x_o - softness;
				mgy_o = y - y_o - softness;
				shadowcount = 0;

				for (my = 0; my < mh; my++)
				{
					mxoffset = greypic + mgx_o + (long) (mgy_o + my) * width;
					for (mx = mx_s[my]; mx < mx_e[my]; mx++)
					{
						shadowcount += *(mxoffset + mx);
					}
				}

				tcoffset = tcyoffset + x * 3L;

				*(tcoffset++) = redbuf[shadowcount];
				*(tcoffset++) = redbuf[shadowcount];
				*(tcoffset++) = redbuf[shadowcount];
			}
		}

		/*--- left-part -------------------------------------*/
		tcyoffset = pic + (long) y * bpl;
		offset = greypic + (long) y * width;

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

						mxoffset = greypic + mgx_o + (long) (mgy_o + my) * width;
						for (mx = mx_cs; mx < mx_e[my]; mx++)
						{
							shadowcount += *(mxoffset + mx);
						}
					}
				}

				tcoffset = tcyoffset + x * 3L;

				*(tcoffset++) = redbuf[shadowcount];
				*(tcoffset++) = redbuf[shadowcount];
				*(tcoffset++) = redbuf[shadowcount];
			}
		}

		/*--- right-part ------------------------------------*/
		tcyoffset = pic + (long) y * bpl;
		offset = greypic + (long) y * width + x_end;

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

						mxoffset = greypic + mgx_o + (long) (mgy_o + my) * width;
						for (mx = mx_s[my]; mx < mx_cs; mx++)
						{
							shadowcount += *(mxoffset + mx);
						}
					}
				}

				tcoffset = tcyoffset + x * 3L;

				*(tcoffset++) = redbuf[shadowcount];
				*(tcoffset++) = redbuf[shadowcount];
				*(tcoffset++) = redbuf[shadowcount];

			}
		}

	} /*--- ende Y-schleife ----------------*/


	/*--- upper-part ---------------------------------------*/
	for (y = 0; y < y_start; y++)
	{
		busycount++;
		if ((busycount & busycall) == 0)
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		mgy_o = y - y_o - softness;
		if ((mgy_o + mh) > 0)
		{
			tcyoffset = pic + (long) y * bpl;
			offset = greypic + (long) y * width;

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
						if ((mgx_o + mx_e[my]) > 0 || (mgx_o + mx_s[my]) < width)
						{
							mx_cs = mx_s[my];
							if ((mgx_o + mx_cs) < 0)
								mx_cs -= (mgx_o + mx_cs);
							mx_ce = mx_e[my];
							if ((mgx_o + mx_ce) > width)
								mx_ce += width - (mgx_o + mx_ce);

							mxoffset = greypic + mgx_o + (long) (mgy_o + my) * width;
							for (mx = mx_cs; mx < mx_ce; mx++)
							{
								shadowcount += *(mxoffset + mx);
							}
						}
					}

					tcoffset = tcyoffset + x * 3L;

					*(tcoffset++) = redbuf[shadowcount];
					*(tcoffset++) = greenbuf[shadowcount];
					*(tcoffset++) = bluebuf[shadowcount];
				}
			}
		} else
		{
			tcyoffset = pic + (long) y * bpl;
			offset = greypic + (long) y * width;

			for (x = 0; x < width; x++)
			{
				if (*(offset++) == 0)
				{
					*tcyoffset++ = 255;
					*tcyoffset++ = 255;
					*tcyoffset++ = 255;
				} else
				{
					tcyoffset += 3;
				}
			}
		}
	}									/* Y-schleife ---------------- */


	/*--- lower-part ---------------------------------------*/
	for (y = y_end; y < height; y++)
	{
		busycount++;
		if ((busycount & busycall) == 0)
			smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

		mgy_o = y - y_o - softness;
		if (mgy_o < height)
		{
			tcyoffset = pic + (long) y * bpl;
			offset = greypic + (long) y * width;

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
						if ((mgx_o + mx_e[my]) > 0 || (mgx_o + mx_s[my]) < width)
						{
							mx_cs = mx_s[my];
							if ((mgx_o + mx_cs) < 0)
								mx_cs -= (mgx_o + mx_cs);
							mx_ce = mx_e[my];
							if ((mgx_o + mx_ce) > width)
								mx_ce += width - (mgx_o + mx_ce);

							mxoffset = greypic + mgx_o + (long) (mgy_o + my) * width;
							for (mx = mx_cs; mx < mx_ce; mx++)
							{
								shadowcount += *(mxoffset + mx);
							}
						}
					}

					tcoffset = tcyoffset + x * 3L;

					*(tcoffset++) = redbuf[shadowcount];
					*(tcoffset++) = greenbuf[shadowcount];
					*(tcoffset++) = bluebuf[shadowcount];
				}
			}
		} else
		{
			tcyoffset = pic + (long) y * bpl;
			offset = greypic + (long) y * width;

			for (x = 0; x < width; x++)
			{
				if (*(offset++) == 0)
				{
					*tcyoffset++ = 255;
					*tcyoffset++ = 255;
					*tcyoffset++ = 255;
				} else
				{
					tcyoffset += 3;
				}
			}
		}
	}

	Mfree(redbuf);
	Mfree(greypic);

	return M_PICDONE;
}


/*-----------------------  FUNCTION MAIN --------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* Wenn das Modul aufgerufen wurde, */
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		/* Wenn das Modul gestartet wurde */
		smurf_struct->module_mode = do_it(smurf_struct);
		break;

	case MTERM:
		/* Wenn das Modul sich verpissen soll */
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
