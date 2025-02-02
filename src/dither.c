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

/**********************************************************************	*/
/*	Smurf Dither-Routinen - von NC bis FS.								*/
/*		Dispatcher-Funktionen zum Aufrufen des eigentlichen Ditherings	*/
/*			(Danke, Stefan, nach dem Ausdruck hab ich gesucht!)			*/
/*																		*/
/*			Das Kraut-und-R�ben-Modul...								*/
/*	Sieht aus wie ein Acker und ist auch ungef�hr so effizient...		*/
/*																		*/
/*	Jetzt isses nur noch das R�ben-Modul, das Kraut habe ich			*/
/*	gr��tenteils entfernt... ;-)										*/
/*																		*/
/**********************************************************************	*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "import.h"
#include "demolib.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "globdefs.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "bindings.h"


#define TIMER 0

/*------------------ Globale Variablen ---------------------*/
int bplanes;
int dithermode;
static uint8_t *palette;
static uint8_t *pic;
static uint8_t *nc_table;
static uint8_t *plane_table;

static WORD width, height;
static WORD src_depth;
static WORD src_format;
static WORD pic_dmode;
static uint16_t w;

/* ----------------------------------------------------------------	*/
/* ----------------------------------------------------------------	*/
/*							Dither Dispatcher						*/
/* ----------------------------------------------------------------	*/
/* ----------------------------------------------------------------	*/
int f_dither(SMURF_PIC *dest, SYSTEM_INFO *sys_info, int pic_changed, GRECT *part, DISPLAY_MODES *displayOpt)
{
	MFDB *screenpic;
	MFDB *standard_image = NULL;
	uint8_t *paddr;
	uint8_t *sc_picture = NULL;
	uint8_t *z;
	WORD display_depth, handle, t;
	WORD endwid, endhgt, pic_pal_index;
	long *histogram, mem;
	uint8_t *tmp_nct;

	/*************************************/
	/*------- Struktur extrahieren ------*/
	/*************************************/
	handle = sys_info->vdi_handle;
	bplanes = sys_info->bitplanes;
	width = dest->pic_width;
	height = dest->pic_height;
	src_depth = dest->depth;
	pic = dest->pic_data;
	palette = dest->palette;
	nc_table = sys_info->nc_table;
	plane_table = sys_info->plane_table;
	src_format = dest->format_type;

	/*
	 * Breite und H�he nach Zoom - hier m�ssen die Ausschnittsabmessungen rein
	 */
	if (part == NULL)
	{
		endwid = width / (dest->zoom + 1);
		endhgt = height / (dest->zoom + 1);
	} else
	{
		endwid = part->g_w / (dest->zoom + 1);
		endhgt = part->g_h / (dest->zoom + 1);
	}

	if (endwid == 0 || endhgt == 0)
		return -1;

	/**************************************************/
	/*------- Speicher f�r die MFDBs anfordern -------*/
	/**************************************************/
	screenpic = (MFDB *)malloc(sizeof(MFDB));	/* Bildschirmdarstellung - MFDB */
	memset(screenpic, 0x0, sizeof(MFDB));

	if (bplanes <= 8 || src_depth == 1)
	{
		standard_image = (MFDB *)malloc(sizeof(MFDB));	/* Standardformat - MFDB */
		memset(standard_image, 0, sizeof(MFDB));
	}

	/***************************************************/
	/*--------- Darstellungsmodus ausw�hlen -----------*/
	/***************************************************/
	if (src_depth <= 24 && src_depth >= 16)
	{
		pic_dmode = displayOpt->syspal_24;
		dithermode = displayOpt->dither_24;
	} else if (src_depth <= 8 && src_depth >= 5)
	{
		pic_dmode = displayOpt->syspal_8;
		dithermode = displayOpt->dither_8;
	} else if (src_depth <= 4)
	{
		pic_dmode = displayOpt->syspal_4;
		dithermode = displayOpt->dither_4;
	}

	if (src_depth > 1)
		display_depth = bplanes;
	else
		display_depth = 1;

	/*
	 * Speicher f�r den Standardformat-Buffer anfordern
	 */
	w = ((endwid + 15) / 16) * 16;								/* Bildbreite ALIGN 16 */
	mem = (long)w * (long)endhgt * (long)display_depth / 8L;	/* Bildspeicher */
	if ((paddr = (uint8_t *)SMalloc(mem)) == NULL)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DISPLAY_NOMEM].TextCast, NULL, NULL, NULL, 1);
		free(screenpic);
		free(standard_image);
		return -1;
	}

	/*********************************************************/
	/*---- alte Bildschirmdarstellung (Daten) freigeben ---- */
	/*********************************************************/
	if (pic_changed)
	{
		if (dest->screen_pic != NULL)
		{
			SMfree(dest->screen_pic->fd_addr);
			free(dest->screen_pic);
		}
	}

	/* in HC/TC wird das Bild direkt f�r den Screen konvertiert, das brauchen wir also nicht... */
	/* ... zumindest nicht, wenn es kein Monochrombild ist. */

	/*
	 * Bildschirmdarstellung (ger�teabh�ngig) anfordern
	 */
	if (bplanes <= 8 || src_depth == 1)
	{
		if ((sc_picture = SMalloc(mem)) == NULL)
		{
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DISPLAY_NOMEM].TextCast, NULL, NULL, NULL, 1);
			free(screenpic);
			free(standard_image);
			SMfree(paddr);
			return -1;
		}

		memset(sc_picture, 0, mem);
	}

	graf_mouse(BUSYBEE, NULL);

	/****************************************/
	/*  Palette einf�gen                    */
	/****************************************/
	if (dest->changed != 255)			/* zu ditherndes Bild ist kein Block? */
	{
		if ((pic_dmode == CR_SYSPAL && bplanes <= 8) || src_depth == 1)
		{
			/* Ziel = Systempalette */
			for (t = 0; t <= sys_info->Max_col; t++)
			{
				dest->red[t] = (uint8_t)((255 * (long)sys_info->pal_red[t]) / 1000L);
				dest->grn[t] = (uint8_t)((255 * (long)sys_info->pal_green[t]) / 1000L);
				dest->blu[t] = (uint8_t)((255 * (long)sys_info->pal_blue[t]) / 1000L);
			}
		} else if (pic_dmode != CR_FILEPAL && pic_dmode != CR_SYSPAL &&
				   pic_dmode != CR_FIXPAL && src_depth <= bplanes && bplanes <= 8)
		{
			/* Ziel = MedianCut-Pal */
			z = dest->palette;
			for (t = 0; t <= sys_info->Max_col; t++)
			{
				pic_pal_index = plane_table[t] * 3;
				dest->red[t] = z[pic_pal_index];
				dest->grn[t] = z[pic_pal_index + 1];
				dest->blu[t] = z[pic_pal_index + 2];
			}
		} else if (pic_dmode == CR_FILEPAL)
		{
			/* Ziel = geladene Palette */
			for (t = 0; t <= sys_info->Max_col; t++)
			{
				pic_pal_index = t;
				dest->red[t] = fix_red[pic_pal_index];
				dest->grn[t] = fix_green[pic_pal_index];
				dest->blu[t] = fix_blue[pic_pal_index];
			}
		}
	}

	/***********************************************************************/
	/* --------------------- Pixelpacked Picturedata    ------------------- */
	/***********************************************************************/
	if (dest->format_type == FORM_PIXELPAK)
	{
		if (bplanes >= 16)
		{
			direct2screen(dest, paddr, part);
		} else
		{
			/*
			 * System- oder Filepalette
			 */
			if (pic_dmode == CR_SYSPAL || pic_dmode == CR_FIXPAL)
			{
				if (dest->local_nct)
				{
					SMfree(dest->local_nct);
					dest->local_nct = NULL;
				}

				f_dither_24_1(sys_info, dest, paddr, PLANTAB, nc_table, part);
			} else if (pic_dmode == CR_FILEPAL)
			{
				if (dest->changed != 255 && dest->local_nct == NULL)
				{
					histogram = make_histogram(dest);
					make_tmp_nct(histogram, dest, sys_info->Max_col + 1);
					SMfree(histogram);
				}

				f_dither_24_1(sys_info, dest, paddr, SCALEPAL | PLANTAB | LNCT, dest->local_nct, part);
			} else
			{
				/*
				 * Median Cut
				 */
				if (src_depth <= bplanes)
				{
					/*
					 * pp->Standard umbauen und Screenpalette anpassen
					 */
					f_transform_to_standard(dest, paddr);
				} else
				{
					/*
					 * oder Median Cut ausf�hren und Dithern
					 */
					if (dest->changed != 255 && dest->local_nct == NULL)
						tmp_nct = fix_palette(dest, sys_info);	/* Palette anpassen */
					else
						tmp_nct = dest->local_nct;
					f_dither_24_1(sys_info, dest, paddr, SCALEPAL | PLANTAB | LNCT, tmp_nct, part);
				}
			}

			standard_image->fd_addr = paddr;
		}
	} else
	{
		/***********************************************************************/
		/* --------------------- Standardformat Picturedata -------------------*/
		/***********************************************************************/
		/*
		 * Farbbild ...
		 */
		if (src_depth > 1)
		{
			/*
			 * Es mu� gedithert werden
			 */
			if (pic_dmode == CR_SYSPAL || pic_dmode == CR_FILEPAL || (pic_dmode != CR_SYSPAL && src_depth > bplanes))
			{
				if (bplanes <= 8)
				{
					if (pic_dmode == CR_FIXPAL || pic_dmode == CR_SYSPAL)
					{
						if (dest->local_nct)
						{
							SMfree(dest->local_nct);
							dest->local_nct = NULL;
						}

						f_dither_24_1(sys_info, dest, paddr, PLANTAB, nc_table, part);
					} else if (pic_dmode == CR_FILEPAL)
					{
						f_dither_24_1(sys_info, dest, paddr, PLANTAB | LNCT | SCALEPAL, nc_table, part);
					} else
					{
						if (dest->changed != 255 && dest->local_nct == NULL)
							tmp_nct = fix_palette(dest, sys_info);	/* Palette anpassen */
						else
							tmp_nct = dest->local_nct;
						f_dither_24_1(sys_info, dest, paddr, SCALEPAL | PLANTAB | LNCT, tmp_nct, part);
					}

					standard_image->fd_addr = paddr;
				} else
				{
					direct2screen(dest, paddr, part);
				}
			} else if (src_depth <= bplanes && pic_dmode != CR_SYSPAL && pic_dmode != CR_FIXPAL)
			{
				/*
				 * die Palette kann und darf angepa�t werden
				 */
				if (bplanes <= 8)
				{
					f_align_standard(dest, paddr, part);
					standard_image->fd_addr = paddr;
				} else
				{
					direct2screen(dest, paddr, part);
				}
			} else
			{
				if (dest->local_nct)
				{
					SMfree(dest->local_nct);
					dest->local_nct = NULL;
				}

				f_dither_24_1(sys_info, dest, paddr, PLANTAB, nc_table, part);
				standard_image->fd_addr = paddr;
			}
		} else
		{
			/*
			 * Monochrombild - kein Dithering...
			 */
			/* MFDBs vorbereiten */
			f_align_standard(dest, paddr, part);

			standard_image->fd_addr = paddr;
			standard_image->fd_w = endwid;
			standard_image->fd_h = endhgt;
			standard_image->fd_wdwidth = (endwid + 15) / 16;
			standard_image->fd_stand = 1;
			standard_image->fd_nplanes = 1;

			screenpic->fd_addr = sc_picture;
			screenpic->fd_w = endwid;
			screenpic->fd_h = endhgt;
			screenpic->fd_wdwidth = (endwid + 15) / 16;
			screenpic->fd_stand = 0;
			screenpic->fd_nplanes = 1;

			vr_trnfm(handle, standard_image, screenpic);
			SMfree(paddr);
		}
	}


	/*
	 * Transform f�r die Farbbilder
	 */
	if (src_depth > 1)
	{
		if (bplanes <= 8)
		{
			/*--------------- Quell-MFDB vobereiten */
			standard_image->fd_w = endwid;
			standard_image->fd_h = endhgt;
			standard_image->fd_wdwidth = (endwid + 15) / 16;
			standard_image->fd_stand = 1;
			standard_image->fd_nplanes = bplanes;

			/* ------ Ziel-MFDB vobereiten */
			screenpic->fd_addr = sc_picture;
			screenpic->fd_w = endwid;
			screenpic->fd_h = endhgt;
			screenpic->fd_wdwidth = (endwid + 15) / 16;
			screenpic->fd_stand = 0;
			screenpic->fd_nplanes = bplanes;

			/* -------- VDI-Transform */
			vr_trnfm(handle, standard_image, screenpic);
			SMfree(paddr);
		} else
		{
			/* ----------- HC/TC-Modus ------------ */
			screenpic->fd_addr = paddr;	/* Genug RAM - also schnell in anderen Speicherblock. */
			screenpic->fd_w = endwid;
			screenpic->fd_h = endhgt;
			screenpic->fd_wdwidth = (endwid + 15) / 16;
			screenpic->fd_stand = 1;
			screenpic->fd_nplanes = bplanes;
		}
	}

	graf_mouse(ARROW, NULL);
	if (bplanes <= 8 || src_depth == 1)
		free(standard_image);
	dest->screen_pic = screenpic;

	if (pic_dmode != CR_SYSPAL && src_depth > 1)
		dest->own_pal = 1;

	return 0;
}



/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/*							PAM - Dispatcher					*/
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */

uint8_t *fix_palette(SMURF_PIC *dest, SYSTEM_INFO *sys_info)
{
	long *histogram;

	histogram = make_histogram(dest);
	median_cut(histogram, dest, sys_info);
	make_tmp_nct(histogram, dest, sys_info->Max_col + 1);
	SMfree(histogram);

	return dest->local_nct;
}



/* -----------------------------------------------------------------*/
/*				nach Standardformat wandeln							*/
/* wandelt ein 8 Bit PP-Bild in ein in ein auf 16 Pixel				*/
/* ausgerichtetes Standardformatbild 								*/
/* -----------------------------------------------------------------*/
void f_transform_to_standard(SMURF_PIC *picture, uint8_t *paddr)
{
	uint8_t *buffer;
	uint16_t y, bh, bl;
	long planelength;


#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	buffer = picture->pic_data;

	planelength = (long)(((long)(width + 15) / 16 * 2) * (long)height);	/* L�nge einer Plane in Bytes (Bytegrenzen) */

	bh = height / 10;					/* busy-height */
	bl = 0;								/* busy-length */

	Dialog.busy.reset(0, "Transform...");

	y = 0;
	do
	{
		if ((y % bh) == 0)
		{
			Dialog.busy.draw(bl);
			bl += 12;
		}

		paddr += setpix_std_line16(buffer, paddr, bplanes, planelength, width);
		buffer += width;
	} while (++y < height);

#if TIMER
	/* wie schnell waren wir? */
	printf("timer: %lu\n", get_timer());
	(void)Cnecin();
#endif
}


/*------------------------------------------------------------------*/
/*					Standardformat ausrichten						*/
/* richtet ein beliebiges nach 8 Bit ausgerichtetes Standardformat-	*/
/* bild aus *picture nach 16 Bit in *paddr f�r Bildschirmdarstellung*/
/* aus. Evtl. Zoomen wird auch hier erledigt.						*/
/* Ge�ndert am 27.07. zwecks Konvertieren von Ausschnitten.			*/
/* Beim Ausschnittdithern ist kein Realigning n�tig, da die Werte in*/
/* *part bereits aligned sind. Beim Darstellen des Ausschnitts wird */
/* dann nur der darzustellende Bereich herauskopiert.				*/
/*------------------------------------------------------------------*/
void f_align_standard(SMURF_PIC *picture, uint8_t *paddr, GRECT *part)
{
	uint8_t *buffer;
	uint8_t *linebuf;
	uint8_t *src;
	uint8_t *dest;
	uint8_t p;
	uint8_t Planes;
	long planelen, dest_planelen, bufwid;
	WORD x, y, bytes_per_line, bytes_aligned, bytes_copy, xbegin;
	WORD endwid, endhgt;

	buffer = picture->pic_data;
	Planes = picture->depth;
	endwid = width / (picture->zoom + 1);
	endhgt = height / (picture->zoom + 1);
	bufwid = width;

	bytes_copy = bytes_per_line = (width + 7) / 8;	/* L�nge einer Zeile im Bild */
	planelen = (long)bytes_per_line * (long)picture->pic_height;

	bytes_aligned = (WORD)((endwid + 15) / 16) * 2;	/* L�nge einer Zeile im aligneden Bild */
	dest_planelen = (long)bytes_aligned * (long)endhgt;

	if (!part && (long)width * (long)height > 64000L)
		Dialog.busy.reset(0, "Bild anpassen..."); /* FIXME: translate */

	/*
	 * Ausschnitt?
	 */
	if (part && part->g_w > 0 && part->g_h > 0)
	{
		bytes_copy = (part->g_w + 7) / 8;
		xbegin = (part->g_x - 7) / 8;
		buffer += ((long)part->g_y * (long)bytes_per_line) + xbegin;
		height = part->g_h;

		endwid = part->g_w / (picture->zoom + 1);
		endhgt = part->g_h / (picture->zoom + 1);
		bufwid = part->g_w;

		bytes_aligned = (WORD)((endwid + 15) / 16) * 2;	/* L�nge einer Zeile im aligneden Bild */
	}

	p = 0;

	/*
	 * ohne Zoom
	 */
	if (picture->zoom == 0)
	{
		do
		{
			Dialog.busy.draw((p << 7) / Planes);

			y = 0;
			do
			{
				memcpy(paddr, buffer, bytes_copy);
				paddr += bytes_aligned;
				buffer += bytes_per_line;
			} while (++y < height);
		} while (++p < Planes);
	}

	/*
	 * mit Zoom
	 */
	else
	{
		linebuf = malloc(bufwid + 32);

		y = 0;
		do
		{
			if ((y & 63) == 0)
				Dialog.busy.draw((y << 7) / endhgt);

			memset(linebuf, 0x0, bufwid + 32);
			getpix_std_line(buffer, linebuf, Planes, planelen, bufwid);

			src = linebuf;
			dest = linebuf;
			x = 0;
			do
			{
				*dest++ = *src++;
				src += picture->zoom;
			} while (++x < endwid);

			setpix_std_line16(linebuf, paddr, Planes, dest_planelen, endwid);

			paddr += bytes_aligned;
			buffer += (long)bytes_per_line * (long)(picture->zoom + 1);
		} while (++y < endhgt);

		free(linebuf);
	}
}



/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
/*                      EXPORT Dither Dispatcher                    */
/*                  Aufruf mit gefakter Sysinfo                     */
/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
int export_dither_dispatcher(SMURF_PIC *dest, SYSTEM_INFO *sys_info, DISPLAY_MODES *display, WORD *fixpal_red, WORD *fixpal_green, WORD *fixpal_blue)
{
	uint8_t *paddr;
	uint8_t *tmp_nct;
	uint8_t *z;
	short oldzoom;
	WORD display_depth, t;
	WORD endwid, endhgt, pic_pal_index;
	long *histogram, mem;
	MFDB *standard_image;

	/**************************************************/
	/*------- Speicher f�r die MFDBs anfordern -------*/
	/**************************************************/
	standard_image = (MFDB *)malloc(sizeof(MFDB));	/* Bildschirmdarstellung - MFDB */
	memset(standard_image, 0x0, sizeof(MFDB));

	/*************************************/
	/*------- Struktur extrahieren ------*/
	/*************************************/
	bplanes = sys_info->bitplanes;
	nc_table = sys_info->nc_table;
	plane_table = sys_info->plane_table;

	pic = dest->pic_data;
	palette = dest->palette;
	width = dest->pic_width;
	height = dest->pic_height;
	src_depth = dest->depth;
	src_format = dest->format_type;

	/***************************************************/
	/*--------- Darstellungsmodus ausw�hlen -----------*/
	/***************************************************/
	if (src_depth <= 24 && src_depth >= 16)
	{
		dithermode = display->dither_24;
		pic_dmode = display->syspal_24;
	} else if (src_depth <= 8 && src_depth >= 5)
	{
		dithermode = display->dither_8;
		pic_dmode = display->syspal_8;
	} else if (src_depth <= 4)
	{
		dithermode = display->dither_4;
		pic_dmode = display->syspal_4;
	}

	if (src_depth > 1)
		display_depth = bplanes;
	else
		display_depth = 1;

	/*************************************************/
	/*------- Speicher f�r das Bild anfordern -------*/
	/*************************************************/
	w = ((width + 15) / 16) * 16;		/* Bildbreite ALIGN 16 */
	mem = (long)w * (long)height * (long)display_depth / 8L;	/* Bildspeicher */
	if ((paddr = SMalloc(mem)) == NULL)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DIT_NOMEM].TextCast, NULL, NULL, NULL, 1);
		free(standard_image);
		return -1;
	}

	graf_mouse(BUSYBEE, NULL);

	/*
	 * Zoom sichern und auf Null setzen
	 */
	oldzoom = dest->zoom;
	dest->zoom = 0;

	/****************************************/
	/*  Palette einf�gen                    */
	/****************************************/
	if ((pic_dmode == CR_SYSPAL && bplanes <= 8) || src_depth == 1)
	{
		for (t = 0; t <= sys_info->Max_col; t++)
		{
			dest->red[t] = (uint8_t)((255 * (long)sys_info->pal_red[t]) / 1000L);
			dest->grn[t] = (uint8_t)((255 * (long)sys_info->pal_green[t]) / 1000L);
			dest->blu[t] = (uint8_t)((255 * (long)sys_info->pal_blue[t]) / 1000L);
		}
	} else if (pic_dmode != CR_FILEPAL && pic_dmode != CR_SYSPAL && pic_dmode != CR_FIXPAL && src_depth <= bplanes && bplanes <= 8)
	{
		z = dest->palette;
		for (t = 0; t <= sys_info->Max_col; t++)
		{
			pic_pal_index = plane_table[t] * 3;
			dest->red[t] = z[pic_pal_index];
			dest->grn[t] = z[pic_pal_index + 1];
			dest->blu[t] = z[pic_pal_index + 2];
		}
	} else if (pic_dmode == CR_FILEPAL)
	{
		for (t = 0; t <= sys_info->Max_col; t++)
		{
			pic_pal_index = t;
			dest->red[t] = fixpal_red[pic_pal_index];
			dest->grn[t] = fixpal_green[pic_pal_index];
			dest->blu[t] = fixpal_blue[pic_pal_index];
		}
	}


	/***********************************************************************/
	/* --------------------- Pixelpacked Picturedata    -------------------*/
	/***********************************************************************/
	if (dest->format_type == FORM_PIXELPAK)
	{
		/* ------- mit Median-Cut ------- */
		/* der Teil Bildtiefe < Exportertiefe wird schon in f_save() abgefangen */
		if (pic_dmode != CR_SYSPAL && pic_dmode != CR_FIXPAL && pic_dmode != CR_FILEPAL)
		{
			tmp_nct = fix_palette(dest, sys_info);	/* Palette anpassen */
			f_dither_24_1(sys_info, dest, paddr, LNCT | STAND8 | SCALEPAL, tmp_nct, NULL);
		} else if (pic_dmode != CR_FIXPAL && pic_dmode != CR_FILEPAL)	/* SysPal */
		{
			f_dither_24_1(sys_info, dest, paddr, PLANTAB | STAND8, nc_table, NULL);
			for (t = 0; t <= sys_info->Max_col; t++)
			{
				pic_pal_index = sys_info->plane_table[t];
				dest->red[pic_pal_index] = (uint8_t)((255L * (long)(sys_info->pal_red[t])) / 1000L);
				dest->grn[pic_pal_index] = (uint8_t)((255L * (long)(sys_info->pal_green[t])) / 1000L);
				dest->blu[pic_pal_index] = (uint8_t)((255L * (long)(sys_info->pal_blue[t])) / 1000L);
			}
		} else if (pic_dmode == CR_FILEPAL)	/* FilePal */
		{
			histogram = make_histogram(dest);
			make_tmp_nct(histogram, dest, sys_info->Max_col + 1);
			SMfree(histogram);
			f_dither_24_1(sys_info, dest, paddr, LNCT | STAND8 | SCALEPAL, dest->local_nct, NULL);
		} else						/* FixPal */
		{
			f_dither_24_1(sys_info, dest, paddr, STAND8, nc_table, NULL);
		}

		/*------------- Breite und H�he nach Zoom ------------*/
		endwid = width;
		endhgt = height;

		/*--------------- MFDB vobereiten --------------*/
		standard_image->fd_addr = paddr;
		standard_image->fd_w = endwid;
		standard_image->fd_h = endhgt;
		standard_image->fd_wdwidth = (endwid + 15) / 16;
		standard_image->fd_stand = 1;
		standard_image->fd_nplanes = bplanes;
	}

	/***********************************************************************/
	/* --------------------- Standardformat Picturedata -------------------*/
	/***********************************************************************/
	else
	{
		/* Syspal, Fixpal oder Median Cut? */
		if (pic_dmode == CR_SYSPAL || pic_dmode == CR_FIXPAL || (pic_dmode == CR_MEDIAN && src_depth > bplanes))
		{
			if (pic_dmode == CR_SYSPAL)
			{
				for (t = 0; t <= sys_info->Max_col; t++)
				{
					pic_pal_index = sys_info->plane_table[t];
					dest->red[pic_pal_index] = (uint8_t)((long)sys_info->pal_red[t] * 255L / 1000L);
					dest->grn[pic_pal_index] = (uint8_t)((long)sys_info->pal_green[t] * 255L / 1000L);
					dest->blu[pic_pal_index] = (uint8_t)((long)sys_info->pal_blue[t] * 255L / 1000L);
				}
			}

			if (bplanes <= 8)
			{
				if (pic_dmode == CR_SYSPAL)
				{
					f_dither_24_1(sys_info, dest, paddr, PLANTAB | STAND8, nc_table, NULL);
				} else if (pic_dmode == CR_FIXPAL)
				{
					f_dither_24_1(sys_info, dest, paddr, STAND8, nc_table, NULL);
				} else
				{
					tmp_nct = fix_palette(dest, sys_info);	/* Palette anpassen */
					f_dither_24_1(sys_info, dest, paddr, SCALEPAL | LNCT | STAND8, tmp_nct, NULL);
				}

				standard_image->fd_addr = paddr;
			}
		}
		/* Filepal? */
		else
		{
			histogram = make_histogram(dest);
			make_tmp_nct(histogram, dest, sys_info->Max_col + 1);
			SMfree(histogram);
			f_dither_24_1(sys_info, dest, paddr, STAND8 | LNCT | SCALEPAL, dest->local_nct, NULL);
		}

		standard_image->fd_addr = paddr;
		standard_image->fd_w = width;
		standard_image->fd_h = height;
		standard_image->fd_wdwidth = (width + 15) / 16;
		standard_image->fd_stand = 1;
		standard_image->fd_nplanes = bplanes;
	}

	dest->zoom = oldzoom;

	graf_mouse(ARROW, NULL);

	dest->screen_pic = standard_image;

	return 0;
}
