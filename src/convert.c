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

/*  CONVERT.C                                                   */
/*  Konvertierungsroutinen fÅr SMURF Grafikkonverter            */
/*  Von allen Farbsystemen in alle Farbsysteme und von allen    */
/*  Farbtiefen in alle Farbtiefen aufwÑrts (fÅr abwÑrts werden  */
/*  die Ditherroutinen in DITHER.C benutzt).                    */
/*  Diese Routinen werden aufgerufen, wenn ein Edit- oder       */
/*  Exportmodul eine andere Farbtiefe / -system benîtigt, als   */
/*  die, in der das Bild vorliegt (MOD_ABILITY).                */
/*  Auûerdem werden hiermit alle importierten Bilder ggfs.      */
/*  nach RGB transformiert, da von jetzt ab intern nur noch RGB */
/*  verwaltet wird.                                             */
/*  18.4.96 Olaf                                                */
/*                                                              */
/*  Weitere Routs von Christian eingebaut und seinen Vorschlag  */
/*  zur Auswahl erweitert.  28.05.96							*/
/*                                                              */
/*	Olafs Routinen grîûtenteils ersetzt und umgeschrieben.		*/
/*	Christian :-)												*/
/*                                                              */
/*  8 Bit Graustufenzeugs eingebaut.  06.05.97                  */
/*	Christian													*/
/*                                                              */
/*  Standardformatroutinen auf zeilenweise Bearbeitung umgebaut */
/*	26.3.98 Christian											*/
/*                                                              */



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "import.h"
#include "demolib.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "globdefs.h"

#include "smurfobs.h"
#include "ext_obs.h"

#define TIMER 0


static void bgrtorgb(uint8_t *pic, long pixels)
{
	uint8_t c;

	do
	{
		c = pic[0];
		pic[0] = pic[2];
		pic += 2;
		*pic++ = c;
	} while (--pixels > 0);
}


short f_convert(SMURF_PIC *picture, const MOD_ABILITY *mod_abs, uint8_t modcolform, uint8_t mode, uint8_t automatic)
{
	uint8_t picdepth, dstdepth, srcmode, dstmode, t;
	uint8_t moddepth[8];
	uint8_t modform[8];
	uint8_t piccolform;
	uint8_t g_ask = 0;
	uint8_t in_modcolform = modcolform;
	short back;
	SMURF_PIC *dummy_pic;

	picdepth = picture->depth;
	srcmode = picture->format_type;

	piccolform = picture->col_format;

	if (automatic)
	{
		dstdepth = mod_abs->depth1;
		dstmode = mod_abs->form1;
	} else
	{
		moddepth[0] = mod_abs->depth1;
		moddepth[1] = mod_abs->depth2;
		moddepth[2] = mod_abs->depth3;
		moddepth[3] = mod_abs->depth4;
		moddepth[4] = mod_abs->depth5;
		moddepth[5] = mod_abs->depth6;
		moddepth[6] = mod_abs->depth7;
		moddepth[7] = mod_abs->depth8;

		modform[0] = mod_abs->form1;
		modform[1] = mod_abs->form2;
		modform[2] = mod_abs->form3;
		modform[3] = mod_abs->form4;
		modform[4] = mod_abs->form5;
		modform[5] = mod_abs->form6;
		modform[6] = mod_abs->form7;
		modform[7] = mod_abs->form8;


		/* beste Zielfarbtiefe ermitteln */
		/* Sofern mîglich, wird eine Wandlung nach 16 Bit generell umgangen. */
		/* Weiterhin wird auch das Zielformat 8 Bit Graustufen umgangen wenn */
		/* das Quellbild kein Graustufenbild ist. */
		dummy_pic = alloc_smurfpic(NULL, FALSE);

		for (t = 0; t < 8 && moddepth[t] != 0; t++)
		{
			if (moddepth[t] >= picdepth)
			{
				if (in_modcolform == 255)
				{
					dummy_pic->depth = moddepth[t];
					dummy_pic->col_format = piccolform;
					start_exp_module(export_path, MCOLSYS, dummy_pic, module.bp[exp_conf.export_mod_num & 0xFF],
						module.smStruct[exp_conf.export_mod_num & 0xFF], exp_conf.export_mod_num);
					if (module.smStruct[exp_conf.export_mod_num & 0xFF]->module_mode == M_COLSYS)
						modcolform = module.smStruct[exp_conf.export_mod_num & 0xFF]->event_par[0];
				}

				if (moddepth[t] == 8 && modcolform == GREY && piccolform != GREY)
				{
					/* FIXME: translate */
					g_ask = Dialog.winAlert.openAlert("Das Bild muû fÅr das Modul in 8 Bit Graustufen gewandelt werden! Sind Sie damit einverstanden?", "Nein", "Ja", NULL, 1);
					if (g_ask == 1)		/* Nein */
						continue;
					else
						break;
				} else if (moddepth[t] == 16 && picdepth != 16)
					continue;
				else
					break;
			}
		}

		SMfree(dummy_pic);

		if (moddepth[t] == 0)
			dstdepth = moddepth[--t];
		else
			dstdepth = moddepth[t];
#if 0
		printf("\nsrcmode: %d, dstmode: %d, piccolform: %d, modcolform: %d, picdepth: %d, moddepth[%d]: %d",
			srcmode, modform[t], piccolform, modcolform, picdepth, t, moddepth[t]);
		(void)Cnecin();
#endif
		/* will der User das Bild vielleicht in eine hîhere Farbtiefe gewandelt haben? */
		if (g_ask)
		{
			if (g_ask == 1 && dstdepth == 8)	/* Nein und keine hîhere vorhanden */
				return -2;
			if (g_ask == 1 && dstdepth == 24)	/* Nein und hîhere vorhanden */
			{
				/* FIXME: translate */
				if (Dialog.winAlert.openAlert("Das Modul nimmt auch eine hîhere Farbtiefen als 8 Bit. Soll dahin gewandelt werden?", "Nein", " Ja ", NULL, 1) == 1)
					return -2;			/* Nein */
				else
					modcolform = 0;		/* Farbsystem fest auf RGB, muû spÑter erfragt werden */
			}
		}

		/* will der User das Bild wirklich nach 16 Bit gewandelt haben? */
		if (dstdepth == 16 && picdepth != 16)
		{
			/* FIXME: translate */
			if (Dialog.winAlert.openAlert("Bei der Wandlung nach 16 Bit wird das Bild an FarbqualitÑt verlieren! Wollen Sie das in Kauf nehmen?", "Nein", " Ja ", NULL, 1) == 1)
				return -2;
		}

		dstmode = modform[t];
	}


	if (picdepth == dstdepth && (srcmode == dstmode || dstmode == FORM_BOTH) && piccolform == modcolform)
		return 0;

	/*
	 * Farbtiefendispatcher
	 */
	if (piccolform != modcolform ||			/* Ganz verschieden ... */
		(picdepth == 24 && dstdepth == 8))	/* ... oder auch gleich aber 24 Bit->8 Bit */
	{
		if (modcolform == GREY)
		{
			back = tfm_rgb_to_grey(picture, mode);
			if (back == 0)
			{
				srcmode = FORM_PIXELPAK;
				picdepth = 8;
			} else
				return back;
		} else
		{
			picture->col_format = RGB;
			back = 0;
		}
	}

	if ((srcmode != dstmode && dstmode != FORM_BOTH) || picdepth != dstdepth)
	{
		if (srcmode == FORM_STANDARD)
		{
			if (dstmode == FORM_STANDARD)
				back = tfm_std_to_std(picture, dstdepth, mode);
			else
				back = tfm_std_to_pp(picture, dstdepth, mode);
		} else
		{
			switch (picdepth)
			{
			case 8:
				if (dstdepth == 8)
					back = tfm_pp_to_std8(picture, mode);
				else if (dstdepth == 16)
					back = tfm_8_to_16(picture, mode);
				else
					back = tfm_8_to_24(picture, mode);
				break;
			case 16:
				back = tfm_16_to_24(picture, mode);
				break;
			}
		}
	}

	return back;
}


/*----------------------------------------------------------------- */
/*              Transferroutine Std. -> Std. 1-8 Bit                */
/*                  hîher, schneller, weiter                        */
/*----------------------------------------------------------------- */
short tfm_std_to_std(SMURF_PIC *picture, uint8_t dst_depth, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t BitsPerPixel;
	unsigned short width, height;
	unsigned long planelength, w;

	/* Struktur auslesen */
	buffer = picture->pic_data;
	width = picture->pic_width;
	height = picture->pic_height;
	BitsPerPixel = picture->depth;

	w = (width + 7) / 8;
	planelength = w * (long)height;

	/* Speicher fÅr neues Bild mit dst_depth Planes anfordern ... */
	if ((ziel = SMalloc(planelength * dst_depth)) == 0)
		return M_MEMORY;

	/* ... und ausnullen da die neuen Planes leer sein sollten */
	memset(ziel, 0x0, planelength * dst_depth);

	/* alte Planes in den neuen Speicher vor die zusÑtzlichen */
	/* Planes kopieren ... */
	memcpy(ziel, buffer, planelength * BitsPerPixel);

	/* ... und altes Bild freigeben */
	if (mode == SAME)
		SMfree(buffer);

	picture->pic_data = ziel;
	picture->depth = dst_depth;

	return 0;
}


/*----------------------------------------------------------------- */
/*		Transformation von 1-8 Bit Standard nach 8, 16 und 24 Bit	*/
/*		PP-Format													*/
/*----------------------------------------------------------------- */
short tfm_std_to_pp(SMURF_PIC *picture, uint8_t dst_depth, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *palette;
	uint8_t *pal;
	uint8_t *pixbuf;
	uint8_t *opixbuf;
	uint8_t BitsPerPixel;
	uint8_t Planes, val;
	unsigned short x, y, width, height;
	unsigned long planelength, w;

	Dialog.busy.reset(128, "StF -> PP");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	palette = picture->palette;
	width = picture->pic_width;
	height = picture->pic_height;
	BitsPerPixel = picture->depth;

	/* Zielbild anfordern */
	if ((ziel = SMalloc((((long)width * (long)height * dst_depth) >> 3) + 7)) == 0)
		return M_MEMORY;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	w = (width + 7) / 8;
	planelength = w * (unsigned long)height;	/* LÑnge einer Plane in Bytes */

	Planes = BitsPerPixel;

	obuffer = buffer;
	oziel = ziel;

	if (dst_depth == 8)
	{
		y = 0;
		do
		{
			memset(ziel, 0x0, width);
			getpix_std_line(buffer, ziel, Planes, planelength, width);
			buffer += w;
			ziel += width;
		} while (++y < height);
	} else
	{
		pixbuf = (uint8_t *)malloc(width + 7);
		opixbuf = pixbuf;

		if (dst_depth == 24)
		{
			y = 0;
			do
			{
				pixbuf = opixbuf;

				memset(pixbuf, 0x0, width);
				getpix_std_line(buffer, pixbuf, Planes, planelength, width);
				buffer += w;

				x = 0;
				do
				{
					val = *pixbuf++;
					pal = palette + val + val + val;

					*ziel++ = *pal++;
					*ziel++ = *pal++;
					*ziel++ = *pal;
				} while (++x < width);
			} while (++y < height);
		} else
		{
			y = 0;
			do
			{
				pixbuf = opixbuf;

				memset(pixbuf, 0x0, width);
				getpix_std_line(buffer, pixbuf, Planes, planelength, width);
				buffer += w;

				x = 0;
				do
				{
					val = *pixbuf++;
					pal = palette + val + val + val;

					*((unsigned short *)ziel) = ((*pal & 0xf8) << 8) | ((*(pal + 1) & 0xfc) << 3) | (*(pal + 2) >> 3);
					ziel += 2;
				} while (++x < width);
			} while (++y < height);
		}

		pixbuf = opixbuf;
		free(pixbuf);
	}

	buffer = obuffer;
	ziel = oziel;

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu\n", get_timer());
	(void)Cnecin();
#endif

	if (mode == SAME)
		SMfree(buffer);					/* StF-Bild freigeben */

	picture->pic_data = ziel;
	picture->depth = dst_depth;
	picture->format_type = FORM_PIXELPAK;

	Dialog.busy.ok();

	return 0;
}


/*----------------------------------------------------------------- */
/*		Transformation vom PP-Format nach 8 Bit Standardformat		*/
/*----------------------------------------------------------------- */
short tfm_pp_to_std8(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t v;
	unsigned short y, width, height;
	unsigned long planelength, w;

	Dialog.busy.reset(128, "PP -> StF");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	width = picture->pic_width;
	height = picture->pic_height;

	/* Zielbild anfordern */
	w = (width + 7) / 8;
	planelength = w * (unsigned long)height;	/* LÑnge einer Plane in Bytes */

	if ((ziel = SMalloc(planelength * 8L)) == 0)
		return M_MEMORY;
	memset(ziel, 0x0, planelength * 8L);

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	obuffer = buffer;
	oziel = ziel;

	v = (width + 7) / 8;

	y = 0;
	do
	{
		setpix_std_line(buffer, ziel, 8, planelength, width);
		buffer += width;
		ziel += v;						/* ziel weiter */
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu\n", get_timer());
	(void)Cnecin();
#endif

	if (mode == SAME)					/* wenn ersetzen, */
		SMfree(buffer);					/* PP-Bild freigeben */

	picture->pic_data = ziel;
	picture->format_type = FORM_STANDARD;

	Dialog.busy.ok();

	return 0;
}


/*----------------------------------------------------------------- */
/*						8 Bit PP nach 16 Bit						*/
/*----------------------------------------------------------------- */
short tfm_8_to_16(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *palette;
	uint8_t *pal;
	uint8_t val;
	uint16_t *ziel;
	uint16_t *oziel;
	uint16_t width, height;
	unsigned long length;

	Dialog.busy.reset(128, "8 Bit->16 Bit");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	palette = picture->palette;
	width = picture->pic_width;
	height = picture->pic_height;

	length = (long)width * (long)height;

	/* 16 Bit-Bild anfordern */
	if ((ziel = (uint16_t *)SMalloc(length * 2L)) == 0)
		return M_MEMORY;

	obuffer = buffer;
	oziel = ziel;

	/* Farbtiefe konvertieren */
	do
	{
		val = *buffer++;
		pal = palette + val + val + val;

		*ziel++ = ((*pal & 0xf8) << 8) | ((*(pal + 1) & 0xfc) << 3) | (*(pal + 2) >> 3);
	} while (--length);

	buffer = obuffer;
	ziel = oziel;

	if (mode == SAME)
		SMfree(buffer);					/* 8 Bit-Bild freigeben */

	picture->pic_data = ziel;
	picture->depth = 16;

	Dialog.busy.ok();

	return 0;
}


/*----------------------------------------------------------------- */
/*                      8 Bit PP nach 24 Bit                        */
/*----------------------------------------------------------------- */
short tfm_8_to_24(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *palette;
	uint8_t *pal;
	uint8_t val;
	uint16_t width, height;
	unsigned long length;

	/* Struktur auslesen */
	buffer = picture->pic_data;
	palette = picture->palette;
	width = picture->pic_width;
	height = picture->pic_height;

	length = (long)width * (long)height;

	/* 24 Bit-Bild anfordern */
	if ((ziel = SMalloc(length * 3L)) == 0)
		return M_MEMORY;

	obuffer = buffer;
	oziel = ziel;

	Dialog.busy.reset(0, "8 Bit->24 Bit");

	/* Farbtiefe konvertieren */
	do
	{
		val = *buffer++;
		pal = palette + val + val + val;

		*ziel++ = *pal;
		*ziel++ = *(pal + 1);
		*ziel++ = *(pal + 2);
	} while (--length);

	buffer = obuffer;
	ziel = oziel;

	if (mode == SAME)
		SMfree(buffer);					/* 8 Bit-Bild freigeben */

	picture->pic_data = ziel;
	picture->depth = 24;

	Dialog.busy.ok();

	return 0;
}


/*----------------------------------------------------------------- */
/*                        16 Bit nach 24 Bit                        */
/*----------------------------------------------------------------- */
short tfm_16_to_24(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint16_t *buffer;
	uint16_t *obuffer;
	uint16_t width, height, val;
	long length;

	Dialog.busy.reset(0, "16 Bit->24 Bit");

	/* Struktur auslesen */
	buffer = (uint16_t *)picture->pic_data;
	width = picture->pic_width;
	height = picture->pic_height;

	length = (long)width * (long)height;

	/* 24 Bit-Bild anfordern */
	if ((ziel = SMalloc(length * 3L)) == 0)
		return M_MEMORY;

	obuffer = buffer;
	oziel = ziel;

	/* Farbtiefe konvertieren */
	do
	{
		val = *buffer++;

		*ziel++ = ((val & 0xf800) >> 8);
		*ziel++ = ((val & 0x07e0) >> 3);
		*ziel++ = ((val & 0x001f) << 3);
	} while (--length);

	buffer = obuffer;
	ziel = oziel;

	if (mode == SAME)
		SMfree(buffer);					/* 16Bit-Bild freigeben */

	picture->pic_data = ziel;
	picture->depth = 24;

	Dialog.busy.ok();

	return 0;
}


/*----------------------------------------------------------------- */
/*                        24 Bit nach 16 Bit                        */
/*		Eigentlich ein Exportdither, gefÑllt mir hier aber besser	*/
/*----------------------------------------------------------------- */
short tfm_24_to_16(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint16_t *ziel;
	uint16_t *oziel;
	uint16_t x, y, width, height;

	Dialog.busy.reset(0, "24 Bit->16 Bit");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	obuffer = buffer;
	width = picture->pic_width;
	height = picture->pic_height;

	if (mode == NEW)
	{
		if ((ziel = (uint16_t *)SMalloc((long)width * (long)height * 2L)) == 0)
			return M_MEMORY;
	} else
		ziel = (uint16_t *)buffer;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = ((buffer[0] & 0xf8) << 8) | ((buffer[1] & 0xf8) << 3) | (buffer[2] >> 3);
			buffer += 3;
		} while (++x < width);
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

	if (mode == NEW)
		SMfree(buffer);
	else
		_Mshrink(ziel, (long)width * (long)height * 2L);

	picture->pic_data = ziel;
	picture->depth = 16;

	Dialog.busy.ok();

	return 0;
}



/*------------------------------------------------------------------*/
/*				Transferroutine BGR->RGB und andersherum			*/
/*------------------------------------------------------------------*/
short tfm_bgr_to_rgb(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *palette;
	uint16_t *ziel16;
	uint16_t memwidth;
	uint16_t width, height, depth, pixel, red, green, blue;
	long length;

	Dialog.busy.reset(0, "BGR->RGB");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	palette = picture->palette;
	width = picture->pic_width;
	height = picture->pic_height;
	depth = picture->depth;

	if (mode == NEW)
	{
		if (picture->format_type == FORM_STANDARD)
			memwidth = (width + 15) / 16 * 16;
		else
			memwidth = width;

		if ((ziel = (uint8_t *)SMalloc(((long)memwidth * (long)height * depth) >> 3)) == 0)
			return M_MEMORY;
		memcpy(ziel, buffer, ((long)memwidth * (long)height * depth) >> 3);
	} else
		ziel = buffer;

	length = (long)width * (long)height;

	switch (depth)
	{
	/*----- 16 BIT -----*/
	case 16:
		ziel16 = (uint16_t *)ziel;
		do
		{
			pixel = *ziel16;
			red = pixel & 0xf800;
			green = pixel & 0x07e0;
			blue = pixel & 0x001f;

			*ziel16++ = (red >> 11) | green | (blue << 11);
		} while (--length);
		break;
	/*----- 24 BIT -----*/
	case 24:
		bgrtorgb(ziel, length);
		break;
	default:
		bgrtorgb(palette, SM_PALETTE_MAX);
		break;
	}

	if (mode == NEW)
		picture->pic_data = ziel;

	picture->col_format = RGB;

	Dialog.busy.ok();

	return 0;
}


/*------------------------------------------------------------------*/
/*				Transferroutine CMY->RGB und andersherum			*/
/*------------------------------------------------------------------*/
short tfm_cmy_to_rgb(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint16_t *buffer16;
	uint16_t *ziel16;
	uint16_t memwidth, width, height, depth;
	long length;

	Dialog.busy.reset(0, "CMY->RGB");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	pal = picture->palette;
	width = picture->pic_width;
	height = picture->pic_height;
	depth = picture->depth;

	if (mode == NEW)
	{
		if (picture->format_type == FORM_STANDARD)
			memwidth = (width + 15) / 16 * 16;
		else
			memwidth = width;

		if ((ziel = (uint8_t *)SMalloc(((long)memwidth * (long)height * depth) >> 3)) == 0)
			return M_MEMORY;
		oziel = ziel;
	} else
	{
		ziel = buffer;
		oziel = ziel;
	}

	length = (long)width * (long)height;

	switch (depth)
	{
	/*----- 16 BIT -----*/
	case 16:
		buffer16 = (uint16_t *)buffer;
		ziel16 = (uint16_t *)ziel;
		do
		{
			*ziel16++ = ~*buffer16++;
		} while (--length);
		break;

	/*----- 24 BIT -----*/
	case 24:
		do
		{
			*ziel++ = ~*buffer++;
			*ziel++ = ~*buffer++;
			*ziel++ = ~*buffer++;
		} while (--length);
		break;
	default:
		length = SM_PALETTE_MAX;
		do
		{
			uint8_t v;
			v = ~*pal;
			*pal++ = v;
			v = ~*pal;
			*pal++ = v;
			v = ~*pal;
			*pal++ = v;
		} while (--length);
		break;
	}

	ziel = oziel;

	if (mode == NEW)
		picture->pic_data = ziel;

	picture->col_format = RGB;

	Dialog.busy.ok();

	return 0;
}


/*------------------------------------------------------------------*/
/*				Transferroutine RGB->Grau und nur so rum			*/
/*			Alles mîgliche nach 8 Bit Graustufen pixelpacked.		*/
/*------------------------------------------------------------------*/
short tfm_rgb_to_grey(SMURF_PIC *picture, uint8_t mode)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *palette;
	uint8_t *pal;
	uint8_t *pixbuf;
	uint8_t BitsPerPixel;
	uint8_t val;
	uint16_t *buffer16;
	uint16_t i, x, y, width, height, val16;
	unsigned long planelength, length, w;

	/* FIXME: translate */
	Dialog.busy.reset(0, "RGB->Graustufen");

	/* Struktur auslesen */
	buffer = picture->pic_data;
	width = picture->pic_width;
	height = picture->pic_height;
	BitsPerPixel = picture->depth;

	length = (long)width * (long)height;

	if ((ziel = (uint8_t *)SMalloc(length)) == 0)
		return M_MEMORY;

	obuffer = buffer;
	oziel = ziel;

	if (BitsPerPixel == 24)
	{
		do
		{
			uint32_t val32;

			val32 = (uint32_t)*buffer++ * 871L;
			val32 += (uint32_t)*buffer++ * 2929L;
			val32 += (uint32_t)*buffer++ * 295L;
			val32 >>= 12;
			*ziel++ = val32;
		} while (--length);
	} else if (BitsPerPixel == 16)
	{
		buffer16 = (uint16_t *)buffer;

		do
		{
			val16 = *buffer16++;

			*ziel++ = (((long)((val16 & 0xf800) >> 8) * 871L) + ((long) ((val16 & 0x7e0) >> 3) * 2929L) + ((long) ((val16 & 0x1f) << 3) * 295L)) >> 12;
		} while (--length);
	} else
	{
		palette = picture->palette;

		if (picture->format_type == FORM_PIXELPAK)
		{
			do
			{
				uint32_t val32;

				val = *buffer++;
				pal = palette + val + val + val;

				val32 = (uint32_t)*pal++ * 871L;
				val32 += (uint32_t)*pal++ * 2929L;
				val32 += (uint32_t)*pal * 295L;
				val32 >>= 12;
				*ziel++ = val32;
			} while (--length);
		} else
		{
			pixbuf = malloc(width);

			if (pixbuf != NULL)
			{
				w = (width + 7) / 8;
				planelength = w * (unsigned long)height;	/* LÑnge einer Plane in Bytes */
	
				y = 0;
				do
				{
					memset(pixbuf, 0, width);
					getpix_std_line(buffer, pixbuf, BitsPerPixel, planelength, width);
	
					x = 0;
					do
					{
						uint32_t val32;
	
						val = pixbuf[x];
						pal = palette + val + val + val;
	
						val32 = (uint32_t)*pal++ * 871L;
						val32 += (uint32_t)*pal++ * 2929L;
						val32 += (uint32_t)*pal * 295L;
						val32 >>= 12;
						*ziel++ = val32;
					} while (++x < width);
	
					buffer += w;
				} while (++y < height);
				free(pixbuf);
			}
		}
	}

	buffer = obuffer;
	ziel = oziel;

	pal = picture->palette;

	for (i = 0; i < SM_PALETTE_MAX; i++)
	{
		*pal++ = i;
		*pal++ = i;
		*pal++ = i;
	}

	/* und altes Bild freigeben */
	if (mode == SAME)
		SMfree(buffer);

	picture->pic_data = ziel;

	picture->depth = 8;
	picture->format_type = FORM_PIXELPAK;
	picture->col_format = GREY;

	Dialog.busy.ok();

	return 0;
}


/*------------------------------------------------------------------*/
/*		Testet, ob das Åbergebene Bild ein Graustufenbild ist		*/
/*------------------------------------------------------------------*/
#if 0									/* unused */
BOOLEAN test_if_grey(SMURF_PIC *picture)
{
	uint8_t *data;
	uint8_t BitsPerPixel;
	uint8_t r, g, b;
	uint16_t *data16;
	uint16_t width, height, pixel16;
	unsigned long length;

	if (picture->col_format == GREY)
		return TRUE;

	width = picture->pic_width;
	height = picture->pic_height;
	BitsPerPixel = picture->depth;

	if (BitsPerPixel <= 8)
	{
		data = picture->palette;

		length = SM_PALETTE_MAX;
	} else
	{
		if (BitsPerPixel == 24)
			data = picture->pic_data;
		else
			data16 = (uint16_t *)picture->pic_data;

		length = (long)width * (long)height;
	}

	if (BitsPerPixel != 16)
	{
		do
		{
			r = *data++;
			g = *data++;
			b = *data++;
		} while (r == g && r == b && g == b && --length);
	} else
	{
		do
		{
			pixel16 = *data16++;

			r = ((pixel16 & 0xf800) >> 8);
			g = ((pixel16 & 0x7e0) >> 3);
			b = ((pixel16 & 0x1f) << 3);
		} while (r == g && r == b && g == b && --length);
	}

	if (r == g && r == b && g == b)
	{
		picture->col_format = GREY;
		return 1;
	}
	return 0;
}
#endif
