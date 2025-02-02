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

/*  ------------- SoftScale-Modul V1.21 --------------  */
/*          F�r SMURF Bildkonverter, 26.04.96           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/*--------------- Funktionen -------------------*/
/*----------------------------------------------*/
/*-----> Smurf --------*/
void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);


/*--------------- Globale Variablen --------------------------*/
static unsigned long busycount;
static unsigned long busymax;
static unsigned long busycall;

/*--------------- Infostruktur f�r Hauptprogramm -----*/
MOD_INFO module_info = {
	"Scale",							/* Name des Moduls */
	0x0130,
	"J. Dittmer/O. Piesche",			/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen f�r Importer */
/* 4 Slider�berschriften: max 8 */
	{ "X %", "Y %", "", "" },
/* 4 Checkbox�berschriften: */
	{ "use res", "Interpolate", "", "" },
/* 4 Edit-Objekt-�berschriften: */
	{ "X res", "Y res", "", "" },
/* min/max-Werte f�r Slider */
	{ { 50, 200 }, { 50, 200 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 1, 32768L }, { 1, 32768L }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte f�r Slider, Check und Edit */
	{ 100, 100, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 640, 480, 0, 0 },
	1,
	COMPILER_ID,
	{ "Image", NULL, NULL, NULL, NULL, NULL }
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




static void X_Scale(uint8_t *opic, uint8_t *npic, long obpl, long nbpl, short owidth, short height, short nwidth, GARGAMEL *smurf_struct)
{
	uint8_t *o_offset;
	uint8_t *n_offset;
	unsigned short basestep;
	unsigned short coldiv;
	unsigned long count;
	unsigned long lastcount;
	unsigned long lcount;
	unsigned long step;
	long red, green, blue;
	long sred, sgreen, sblue;
	long ered, egreen, eblue;
	unsigned short x;
	unsigned short y;
	unsigned short i;

	if (nwidth < owidth)
	{									/* -Extrapolieren */
		step = ((long) owidth << 16) / nwidth;
		basestep = (unsigned short) (step >> 16);
		step -= ((long) basestep << 16);
		basestep -= 1;
		coldiv = (short) ((((long) nwidth) << 16) / (owidth));

		if (step != 0)
		{
			for (y = 0; y < height; y++)
			{
				n_offset = npic + y * nbpl;
				o_offset = opic + y * obpl;
				count = 0;

				busycount++;
				if (!(busycount & busycall))
					smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

				sred = *(o_offset++);
				sgreen = *(o_offset++);
				sblue = *(o_offset++);

				lastcount = 0;

				for (x = 0; x < nwidth; x++)
				{
					count += step;
					if (count >= 0x10000L)
					{
						count -= 0x10000L;
						red = *(o_offset++);
						green = *(o_offset++);
						blue = *(o_offset++);

					} else
					{
						red = green = blue = 0;
					}

					i = 0;
					while (i++ < basestep)
					{
						red += *(o_offset++);
						green += *(o_offset++);
						blue += *(o_offset++);
					}

					ered = *(o_offset++);
					egreen = *(o_offset++);
					eblue = *(o_offset++);

					lcount = 0x10000L - lastcount;
					red += ((sred * lcount + ered * count) + 32767) >> 16;
					green += ((sgreen * lcount + egreen * count) + 32767) >> 16;
					blue += ((sblue * lcount + eblue * count) + 32767) >> 16;

					*(n_offset++) = (((red * coldiv) + 32767) >> 16);
					*(n_offset++) = (((green * coldiv) + 32767) >> 16);
					*(n_offset++) = (((blue * coldiv) + 32767) >> 16);

					sred = ered;
					sgreen = egreen;
					sblue = eblue;

					lastcount = count;
				}
			}
		} else	 /*--- Nur bei GANZER Scalierung (2,3,4....) ------*/
		{
			basestep++;
			for (y = 0; y < height; y++)
			{
				n_offset = npic + y * nbpl;
				o_offset = opic + y * obpl;

				busycount++;
				if (!(busycount & busycall))
					smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

				for (x = 0; x < nwidth; x++)
				{
					red = green = blue = 0;
					i = 0;
					while (i++ < basestep)
					{
						red += *(o_offset++);
						green += *(o_offset++);
						blue += *(o_offset++);
					}

					*(n_offset++) = (((red * coldiv) + 32767) >> 16);
					*(n_offset++) = (((green * coldiv) + 32767) >> 16);
					*(n_offset++) = (((blue * coldiv) + 32767) >> 16);
				}
			}
		}
	} else
	{									/* -Interpolieren */
		step = (((long) (owidth - 1) * 0xffff) / (nwidth - 1));

		for (y = 0; y < height; y++)
		{
			n_offset = npic + y * nbpl + (long) (nwidth - 1) * 3L;
			o_offset = opic + y * obpl + (long) (owidth - 2) * 3L;
			count = 0;
			busycount++;
			if (!(busycount & busycall))
				smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

			ered = *(o_offset++);
			egreen = *(o_offset++);
			eblue = *(o_offset++);
			sred = *(o_offset++);
			sgreen = *(o_offset++);
			sblue = *(o_offset);
			o_offset -= 8;

			for (x = 0; x < nwidth; x++)
			{
				lcount = 0xffff - count;
				*(n_offset++) = (((sred * lcount + ered * count) + 32767) >> 16);
				*(n_offset++) = (((sgreen * lcount + egreen * count) + 32767) >> 16);
				*(n_offset) = (((sblue * lcount + eblue * count) + 32767) >> 16);
				n_offset -= 5;

				count += step;
				if (count >= 0xffff)
				{
					count -= 0xffff;
					sred = ered;
					sgreen = egreen;
					sblue = eblue;
					ered = *(o_offset++);
					egreen = *(o_offset++);
					eblue = *(o_offset);
					o_offset -= 5;
				}
			}
		}
	}
}


static void Y_Scale(uint8_t *opic, uint8_t *npic, long obpl, long nbpl, short width, short oheight, short nheight, GARGAMEL *smurf_struct)
{
	uint8_t *o_offset;
	uint8_t *n_offset;
	unsigned short basestep;
	unsigned short coldiv;
	unsigned long count;
	unsigned long lastcount;
	unsigned long lcount;
	unsigned long step;
	long red, green, blue;
	long sred, sgreen, sblue;
	long ered, egreen, eblue;
	unsigned short x;
	unsigned short y;
	unsigned short i;

	if (nheight < oheight)
	{									/* -Extrapolieren */
		step = ((long) oheight << 16) / nheight;
		basestep = (unsigned short) (step >> 16);
		step -= ((long) basestep << 16);
		basestep -= 1;
		coldiv = (short) ((((long) nheight) << 16) / (oheight));

		if (step != 0)
		{
			for (x = 0; x < width; x++)
			{
				n_offset = npic + x + x + x;
				o_offset = opic + x + x + x;
				count = 0;

				busycount++;
				if (!(busycount & busycall))
					smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

				sred = *(o_offset++);
				sgreen = *(o_offset++);
				sblue = *(o_offset);
				o_offset += (obpl - 2);

				lastcount = 0;

				for (y = 0; y < nheight; y++)
				{
					count += step;
					if (count >= 0x10000L)
					{
						count -= 0x10000L;
						red = *(o_offset++);
						green = *(o_offset++);
						blue = *(o_offset);
						o_offset += (obpl - 2);
					} else
					{
						red = green = blue = 0;
					}

					i = 0;
					while (i++ < basestep)
					{
						red += *(o_offset++);
						green += *(o_offset++);
						blue += *(o_offset);
						o_offset += (obpl - 2);
					}

					ered = *(o_offset++);
					egreen = *(o_offset++);
					eblue = *(o_offset);
					o_offset += (obpl - 2);

					lcount = 0x10000L - lastcount;
					red += ((sred * lcount + ered * count) + 32767) >> 16;
					green += ((sgreen * lcount + egreen * count) + 32767) >> 16;
					blue += ((sblue * lcount + eblue * count) + 32767) >> 16;

					*(n_offset++) = (((red * coldiv) + 32767) >> 16);
					*(n_offset++) = (((green * coldiv) + 32767) >> 16);
					*(n_offset) = (((blue * coldiv) + 32767) >> 16);
					n_offset += (nbpl - 2);

					sred = ered;
					sgreen = egreen;
					sblue = eblue;

					lastcount = count;
				}
			}
		} else	 /*--- Nur bei GANZER Scalierung (2,3,4....) ------*/
		{
			basestep++;
			for (x = 0; x < width; x++)
			{
				n_offset = npic + x + x + x;
				o_offset = opic + x + x + x;

				busycount++;
				if (!(busycount & busycall))
					smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

				for (y = 0; y < nheight; y++)
				{
					red = green = blue = 0;
					i = 0;
					while (i++ < basestep)
					{
						red += *(o_offset++);
						green += *(o_offset++);
						blue += *(o_offset);
						o_offset += (obpl - 2);
					}

					*(n_offset++) = (((red * coldiv) + 32767) >> 16);
					*(n_offset++) = (((green * coldiv) + 32767) >> 16);
					*(n_offset) = (((blue * coldiv) + 32767) >> 16);
					n_offset += (nbpl - 2);
				}
			}
		}
	} else
	{									/* -Interpolieren */
		step = (((long) (oheight - 1) * 0xffff) / (nheight - 1));

		for (x = 0; x < width; x++)
		{
			n_offset = npic + (long) (nheight - 1) * nbpl + (long) x *3L;
			o_offset = opic + (long) (oheight - 2) * obpl + (long) x *3L;

			count = 0;
			busycount++;
			if (!(busycount & busycall))
				smurf_struct->services->busybox((short) ((busycount << 10) / busymax));

			ered = *(o_offset++);
			egreen = *(o_offset++);
			eblue = *(o_offset);
			o_offset += (obpl - 2L);
			sred = *(o_offset++);
			sgreen = *(o_offset++);
			sblue = *(o_offset);
			o_offset -= (obpl * 2 + 2L);

			for (y = 0; y < nheight; y++)
			{
				lcount = 0xffff - count;
				*(n_offset++) = (((sred * lcount + ered * count) + 32767) >> 16);
				*(n_offset++) = (((sgreen * lcount + egreen * count) + 32767) >> 16);
				*(n_offset) = (((sblue * lcount + eblue * count) + 32767) >> 16);
				n_offset -= (nbpl + 2L);

				count += step;
				if (count >= 0xffff)
				{
					count -= 0xffff;
					sred = ered;
					sgreen = egreen;
					sblue = eblue;
					ered = *(o_offset++);
					egreen = *(o_offset++);
					eblue = *(o_offset);
					o_offset -= (obpl + 2L);
				}
			}
		}
	}
}


static short do_interpolate(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short x_fak;
	short y_fak;
	short x_res;
	short y_res;
	short width;
	short height;
	short n_width;
	short n_height;
	uint8_t *pic;
	uint8_t *n_pic;
	long bpl;
	long n_bpl;

	/*--- Slider auslesen ---------------------- */

	x_fak = (short) smurf_struct->slide[0];
	y_fak = (short) smurf_struct->slide[1];
	x_res = (short) smurf_struct->edit[0];
	y_res = (short) smurf_struct->edit[1];

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	/*--- Faktor bestimmen -----------------------------*/

	if (smurf_struct->check[0] != 0)
	{
		n_width = x_res;
		n_height = y_res;
	} else
	{
		n_width = (short) ((long) x_fak * (long) width / 100);
		n_height = (short) ((long) y_fak * (long) height / 100);
	}

	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 31;

	/*--- Zu kleine Scalierung --> Beenden !!! --------------*/
	if (n_width < 8 && n_height < 8)
		return M_WAITING;

	/*--- Proportionalit�t anpassen --------------------------*/
	if (n_width < 8)
		n_width = width / ((float) height / n_height);
	if (n_height < 8)
		n_height = height / ((float) width / n_width);

	/*--- Keine Scalierung --> Beenden !!! ------------------*/
	if (width == n_width && height == n_height)
		return M_WAITING;

	/*--- Speicher f�r neues Bild ---------------------*/
	n_pic = (uint8_t *)Malloc((long) n_width * (long) n_height * 3L);

	if (n_pic == NULL)
		return M_MEMORY;				/*  Kein Speicher !!! */

	bpl = (long) width * 3L;
	n_bpl = (long) n_width * 3L;

	/*--- Hauptroutine --------------------*/

	if (n_width == width)
	{
		/*--- Nur Y-Scale -------*/
		busymax = ((long) width << 10) / 127;
		Y_Scale(pic, n_pic, bpl, n_bpl, width, height, n_height, smurf_struct);
	} else
	{
		if (n_height == height)
		{
			/*--- Nur X-Scale --------*/
			busymax = ((long) height << 10) / 127;
			X_Scale(pic, n_pic, bpl, n_bpl, width, height, n_width, smurf_struct);
		} else
		{
			/*--- X- und Y-Scale -------*/
			if (n_width < width)
			{
				/*--- Zuerst X-Scale ---*/
				busymax = (((long) height + (long) n_width) << 10) / 127;
				X_Scale(pic, pic, bpl, bpl, width, height, n_width, smurf_struct);
				Y_Scale(pic, n_pic, bpl, n_bpl, n_width, height, n_height, smurf_struct);
			} else
			{
				if (n_height < height)
				{
					/*--- Zuerst Y-Scale ---*/
					busymax = (((long) width + (long) n_height) << 10) / 127;
					Y_Scale(pic, pic, bpl, bpl, width, height, n_height, smurf_struct);
					X_Scale(pic, n_pic, bpl, n_bpl, width, n_height, n_width, smurf_struct);
				} else
				{
					/*--- Egal !!! (weil beide gr��er) ---*/
					busymax = (((long) height + (long) n_width) << 10) / 127;
					X_Scale(pic, n_pic, bpl, n_bpl, width, height, n_width, smurf_struct);
					Y_Scale(n_pic, n_pic, n_bpl, n_bpl, n_width, height, n_height, smurf_struct);
				}
			}
		}
	}


	/*--- Neue Bildwerte in Struktur schreiben -------*/
	picture->pic_width = n_width;
	picture->pic_height = n_height;
	picture->pic_data = n_pic;

	Mfree(pic);

	smurf_struct->event_par[0] = 0;
	smurf_struct->event_par[1] = -1;
	return M_PICDONE;
}

static void scale_line(uint8_t *dst, uint8_t *src, short nwidth, long width)
{
	long pos;
	short x;
	long offset;
	
	pos = 0;
	for (x = 0; x < nwidth; x++)
	{
		offset = (pos >> 10) * 3;
		*dst++ = src[offset + 0];
		*dst++ = src[offset + 1];
		*dst++ = src[offset + 2];
		pos += width;
	}
}

/*--------------- SCALING ----------------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short x_fak;
	short y_fak;
	short x_res;
	short y_res;
	short width;
	short height;
	short n_width;
	short n_height;
	short y;
	uint8_t *pic;
	uint8_t *n_pic;
	long bpl;
	long n_bpl;
	uint8_t *dst;
	uint8_t *src;
	
	/*--- Slider auslesen ---------------------- */

	x_fak = (short) smurf_struct->slide[0];
	y_fak = (short) smurf_struct->slide[1];
	x_res = (short) smurf_struct->edit[0];
	y_res = (short) smurf_struct->edit[1];

	/*--- Bilddaten auslesen --------------------*/

	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	pic = picture->pic_data;

	/*--- Faktor bestimmen -----------------------------*/

	if (smurf_struct->check[0] != 0)
	{
		n_width = x_res;
		n_height = y_res;
	} else
	{
		n_width = (short) ((long) x_fak * (long) width / 100);
		n_height = (short) ((long) y_fak * (long) height / 100);
	}

	/*--- BusyBox Vorberechnungen ----------------------*/
	busycount = 0;
	busycall = 31;

	/*--- Zu kleine Scalierung --> Beenden !!! --------------*/
	if (n_width < 8 && n_height < 8)
		return M_WAITING;

	/*--- Keine Scalierung --> Beenden !!! ------------------*/
	if (width == n_width && height == n_height)
		return M_WAITING;

	/*--- Proportionalit�t anpassen --------------------------*/
	if (n_width < 8)
		n_width = width / ((float) height / n_height);
	if (n_height < 8)
		n_height = height / ((float) width / n_width);

	/*--- Speicher f�r neues Bild ---------------------*/
	n_pic = (uint8_t *)Malloc((long) n_width * (long) n_height * 3L);

	if (n_pic == NULL)
		return M_MEMORY;				/*  Kein Speicher !!! */

	bpl = (long) width * 3L;
	n_bpl = (long) n_width * 3L;

	/*--- Hauptroutine --------------------*/
	dst = n_pic;
	src = pic;
	for (y = 0; y < n_height; y++)
	{
		scale_line(dst, src, n_width, ((long) width << 10) / n_width);
		dst += n_bpl;
		src = pic + (((long)y * height) / n_height) * bpl;
	}

	/*--- Neue Bildwerte in Struktur schreiben -------*/
	picture->pic_width = n_width;
	picture->pic_height = n_height;
	picture->pic_data = n_pic;

	Mfree(pic);

	smurf_struct->event_par[0] = 0;
	smurf_struct->event_par[1] = -1;
	return M_PICDONE;
}


/*-----> Modul --------*/

/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel gro�es Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview mu� in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* �bernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul mu� nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierf�r bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* f�r die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void)smurfpic;
	(void)preview;
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
		if (smurf_struct->check[1])
			smurf_struct->module_mode = do_interpolate(smurf_struct);
		else
			smurf_struct->module_mode = do_it(smurf_struct);
		break;

	case MTERM:
		/* Wenn das Modul sich verpissen soll */
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
