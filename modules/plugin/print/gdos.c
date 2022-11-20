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

/* Ausdruck von Bildern */
/* UnterstÅtzt wird nur der Druck Åber GDOS-Treiber */
/* Farbausdruck nur Åber entsprechende Treiber, d.h ab NVDI 4 */
/* Skalierung muû ebenfalls vom Treiber unterstÅtzt werden */

#define _GNU_SOURCE /* for pow10() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurf_st.h"
#include "globdefs.h"
#include "smurfine.h"
#include "plugin.h"
#include "bindings.h"
#include "wdialog.h"
#include "gdos.h"

#include "country.h"

#if COUNTRY==1
#include "de/print.h"
#elif COUNTRY==0
#include "en/print.h"
#elif COUNTRY==2
#include "en/print.h"			/* missing french resource */
#endif

short dev_anzahl = 0;
DevInfoS DevInfo[30];
DevParamS DevParam;
OutParamS OutParam;

static WORD work_out[57];

static uint16_t swidth, sheight, dwidth, dheight;


/* Gedruckt wird immer das aktive Bild. */
short print_with_GDOS(void)
{
	uint8_t *srcpic;
	BOOLEAN can_scale;
	WORD work_in[16] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0 };
	WORD gdos_handle = 0;
	WORD pxy[8];
	WORD clip[4];
	WORD colindex[2] = { 1, 0 };
	WORD SCALE;
	uint16_t height;
	uint16_t stripoffset;
	unsigned long lstripheight;
	uint16_t stripheight;
	uint16_t bh, bl;
	long mem;
	unsigned long w, dy;
	MFDB srcform;
	MFDB dstform;

	/* Daten extrahieren */
	swidth = smurf_picture[*active_pic]->pic_width;
	sheight = smurf_picture[*active_pic]->pic_height;

	/* einloggen */
	work_in[0] = DevParam.devID;		/* Nummer des GDOS-Treibers, siehe ASSIGN.SYS */
	v_opnwk(work_in, &gdos_handle, work_out);
	if (!gdos_handle)
	{
		services->f_alert(alerts[NO_OPEN], NULL, NULL, NULL, 1);
		return -1;
	}

	/* FIXME: translate */
	services->reset_busybox(128, "Druck vorbereiten");

	memset(&srcform, 0, sizeof(MFDB));
	memset(&dstform, 0, sizeof(MFDB));

	can_scale = DevParam.can_scale;

	if (can_scale)
	{
		SCALE = 0x8000;					/* fÅr vr?_cpyfm() */
		dwidth = OutParam.picwidth;
		dheight = OutParam.picheight;
	} else
	{
		SCALE = 0x0;					/* fÅr vr?_cpyfm() */
		dwidth = swidth;
		dheight = sheight;
	}

	pxy[0] = 0;							/* linke obere Ecke des Quellblocks */
	pxy[1] = 0;							/* linke obere Ecke des Quellblocks */
	pxy[2] = swidth - 1;				/* rechte untere Ecke des Quellblocks */
	pxy[3] = sheight - 1;				/* rechte untere Ecke des Quellblocks */
	pxy[4] = OutParam.zx;				/* linke obere Ecke des Zielblocks */
	pxy[5] = OutParam.zy;				/* linke obere Ecke des Zielblocks */
	pxy[6] = OutParam.zx + dwidth - 1;	/* rechte untere Ecke des Zielblocks */
	pxy[7] = OutParam.zy + dheight - 1;	/* rechte untere Ecke des Zielblocks */

	clip[0] = pxy[4];					/* linke obere Ecke des Clipping-Rechtecks */
	clip[1] = pxy[5];					/* linke obere Ecke des Clipping-Rechtecks */
	clip[2] = pxy[6];					/* rechte untere Ecke des Clipping-Rechtecks */

	if ((mem = (long) Malloc(-1)) < 128000L)
	{
		services->f_alert(alerts[NO_PRT_MEM], NULL, NULL, NULL, 1);
		return -1;
	}
	lstripheight = ((mem - 6535L) / ((((swidth + 15) / 16 * 16) * DevParam.depth) >> 3));
	if (lstripheight < 2)
	{
		services->f_alert(alerts[NO_PRT_MEM], NULL, NULL, NULL, 1);
		return -1;
	}
	stripheight = lstripheight;
	if (lstripheight > sheight)
		stripheight = sheight;

	w = ((swidth + 15) / 16) * 16;
	if ((srcpic = (uint8_t *) Malloc((lstripheight * w * DevParam.depth) >> 3)) == 0)
	{
		services->f_alert(alerts[NO_PREP_MEM], NULL, NULL, NULL, 1);
		return -1;
	}

	/*  printf("mem: %lu, stripheight: %lu\n", mem, lstripheight); */

	set_MFDB(&srcform, &dstform, srcpic, DevParam.depth);

#if 0
	printf("Start Scheibenausgabe\n");
#endif
#if 0
    /* lt. Behnes unsinnig und fÅr Leerseite vor Ausdruck verantwortlich */
	/* Druckstart: Workstation updaten und Puffer lîschen */
	v_updwk(gdos_handle);
	v_clear_disp_list(gdos_handle);
#endif
	bh = 128 / (sheight / stripheight);	/* busy-height */
	bl = 0;								/* busy-length */

	services->reset_busybox(0, "Seite aufbauen");

	stripoffset = 0;
	height = sheight;
	while (height)
	{
		services->busybox(bl);
		bl += bh;

		if (height < stripheight)
			stripheight = height;

		f_d_dither(smurf_picture[*active_pic], srcpic, stripoffset, stripheight, DevParam.depth);

		dy = stripoffset + stripheight - 1;	/* letzte einzulesende Zeile */
		dy *= dheight;
		if (sheight < dheight)			/* Quellbild kleiner als das Zielbild? */
			dy += dheight - 1;
		dy /= sheight;

		clip[3] = OutParam.zy + (WORD) dy;	/* rechte untere Ecke des Clipping-Rechtecks */

		vs_clip(gdos_handle, TRUE, clip);	/* Clipping-Rechteck setzen */

		srcform.fd_h = stripheight;

		if (DevParam.depth == 1)
			vrt_cpyfm(gdos_handle, MD_REPLACE | SCALE, pxy, &srcform, &dstform, colindex);
		else
			vro_cpyfm(gdos_handle, S_ONLY | SCALE, pxy, &srcform, &dstform);

		pxy[1] -= stripheight;
		pxy[3] -= stripheight;

		clip[1] = clip[3] + 1;			/* erste auszugebende Zeile fÅr den nÑchsten Durchgang */

		stripoffset += stripheight;

		height -= stripheight;
	}

	services->reset_busybox(128, "Druck ausfÅhren");

	/* ausloggen */
	v_updwk(gdos_handle);
	if (OutParam.advance)
		v_form_adv(gdos_handle);
	v_clswk(gdos_handle);

	services->reset_busybox(128, "OK");

	Mfree(srcpic);

	return 0;
}


short scan_devs(void)
{
	short i;

	if (dev_anzahl > 0)
		return 0;

	services->reset_busybox(128, "Scanne Drucker");

	if (!vq_gdos())
	{
		services->f_alert(alerts[NO_GDOS], NULL, NULL, NULL, 1);
		return -1;
	}

	dev_anzahl = 0;
	for (i = 21; i <= 30; i++)			/* GDOS-Device Drucker-Treiber */
		if (get_DevInfo(i, &DevInfo[dev_anzahl]))
			dev_anzahl++;
	for (i = 81; i <= 90; i++)			/* GDOS-Device FAX-Treiber */
		if (get_DevInfo(i, &DevInfo[dev_anzahl]))
			dev_anzahl++;
	for (i = 91; i <= 100; i++)			/* GDOS-Device IMG-Treiber */
		if (get_DevInfo(i, &DevInfo[dev_anzahl]))
			dev_anzahl++;

	services->reset_busybox(128, "OK");

	if (dev_anzahl == 0)
	{
		services->f_alert(alerts[NO_DRIVER], NULL, NULL, NULL, 1);
		return -1;
	}
	return 0;
}


/* Funktion um Cookie auf Anwesenheit zu testen */
/* Christian Eyrich irgendwann im 20. Jahrhundert */
static BOOLEAN get_cookie(unsigned long cookie, unsigned long *value)
{
	unsigned long *cookiejar;

	/* ansonsten value auf 0L */
	*value = 0;
	/* Zeiger auf Cookie Jar holen */
	cookiejar = (unsigned long *) Setexc(0x5a0 / 4, (void (*)()) -1);

	/* Cookie Jar installiert? */
	if (cookiejar == NULL)
		return FALSE;

	/* Keksdose nach cookie durchsuchen */
	while (*cookiejar++ != 0)
	{
		if (*cookiejar++ == cookie)
		{
			/* wenn cookie gefunden wurde, value auf Cookiewert setzen, */
			*value = *cookiejar;
			return TRUE;
		}
		cookiejar += 1;
	}

	return FALSE;
}


/* versucht einen Treiber zu îffnen und stellt bei Erfolg Infos in die Dev-Struktur */
short get_DevInfo(WORD devID, DevInfoS *DevInfo)
{
	char name[128];
	char file_path[33];
	char file_name[33];
	WORD work_in[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0 };
	WORD gdos_handle = 0;
	WORD devExist = 0;
	NVDI_STR *nvdi_str = NULL;

	work_in[0] = devID;
	v_opnwk(work_in, &gdos_handle, work_out);
	if (gdos_handle == 0)
		return 0;

	/* Vorsicht, vq_ext_devinfo existiert nur mit NVDI >= 3.0 */
	get_cookie(0x4e564449L, (unsigned long *)&nvdi_str); /* 'NVDI' */

	if (nvdi_str && nvdi_str->nvdi_version >= 0x0300)
	{
		vq_ext_devinfo(gdos_handle, devID, &devExist, file_path, file_name, name);
	} else
	{
		memset(name, 0, sizeof(name));			/* die Strings von vqt_devinfo() sind nicht nullterminiert */
#if defined(__GEMLIB__) || defined(__PORTAES_H__)
		vqt_devinfo(gdos_handle, devID, &devExist, name, name + 64);
#else
		vqt_devinfo(gdos_handle, devID, &devExist, name);
#endif
	}

	if (devExist)						/* nicht nur > 0, denn kann auch -1 sein */
	{
		DevInfo->devID = devID;
		DevInfo->devName[0] = '\0';		/* damit der cat was zum drancaten hat */
		strncat(DevInfo->devName, name, 35);
	}

	v_clswk(gdos_handle);

	return devExist;
}


/* Dispatcher um ein Bild von beliebigem Format und beliebiger Farbtiefe ins Zielformat */
/* (auf 16 Pixel aligned) und in die vom Treiber erwartete Farbtiefe (s. dest_mode) zu bringen. */
/* MONO = monochromes Bild s/w  (1 Bit Input) */
/* HALF = Halbtonbild, Graustufen, Treiber rastert selbst (32 Bit Input) */
/* COL8 = 8 Farben Bild, feste Palette: Weiû, Schwarz, Rot, GrÅn, Blau, Gelb, Cyan, Magenta (3 Bit Input) */
/* TC = Truecolorbild, Treiber rastert selbst (32 Bit Input) */
/* In BitsPerPixel wird dann auch gleich die tatsÑchliche Farbtiefe zurÅckgegeben */
short f_d_dither(SMURF_PIC *smurf_pic, uint8_t *ziel, uint16_t stripoffset, uint16_t stripheight, unsigned char dest_depth)
{
	uint8_t *buffer;
	uint8_t *palette;
	uint8_t *pal;
	uint8_t *pixbuf;
	uint8_t v;
	uint8_t BitsPerPixel;
	uint16_t *buffer16;
	uint16_t x, y, dw, sw;
	uint16_t pix;
	unsigned long planelength;

	buffer = smurf_pic->pic_data;
	buffer16 = (uint16_t *)buffer;
	BitsPerPixel = smurf_pic->depth;

	dw = ((swidth + 15) / 16) * 16;

	if (smurf_pic->format_type == FORM_PIXELPAK)
		buffer += ((unsigned long) swidth * stripoffset * BitsPerPixel) >> 3;
	else
		buffer += ((unsigned long) (swidth + 7) / 8 * stripoffset * BitsPerPixel) >> 3;

	memset(ziel, 0, ((unsigned long) dw * stripheight * dest_depth) >> 3);

	planelength = (unsigned long) (swidth + 7) / 8 * sheight;	/* LÑnge einer Plane in Bytes */

	palette = smurf_pic->palette;

	sw = (swidth + 7) / 8;

	if (dest_depth == 1)				/* Hier muû eine Ditherroutine rein */
	{
		if (BitsPerPixel == 1)
		{
			v = dw / 8 - sw;

			y = 0;
			do
			{
				x = 0;
				do
				{
					*ziel++ = *buffer++;
				} while (++x < sw);

				ziel += v;
			} while (++y < stripheight);
		}
	} else if (dest_depth == 32)
	{
		pixbuf = (uint8_t *) Malloc(swidth + 7);

		v = (dw - swidth) * 4;

		y = 0;
		do
		{
			if (BitsPerPixel == 24)
			{
				x = 0;
				do
				{
					ziel++;
					*ziel++ = *buffer++;
					*ziel++ = *buffer++;
					*ziel++ = *buffer++;
				} while (++x < swidth);
			} else if (BitsPerPixel == 16)
			{
				x = 0;
				do
				{
					pix = *buffer16++;

					ziel++;
					*ziel++ = (pix & 0xf800) >> 8;
					*ziel++ = (pix & 0x7e0) >> 3;
					*ziel++ = (pix & 0x1f) << 3;
				} while (++x < swidth);
			} else if (BitsPerPixel == 8 && smurf_pic->format_type == FORM_PIXELPAK)
			{
				x = 0;
				do
				{
					pix = *buffer++;
					pal = palette + pix + pix + pix;

					ziel++;
					*ziel++ = *pal;
					*ziel++ = *(pal + 1);
					*ziel++ = *(pal + 2);
				} while (++x < swidth);
			} else if (smurf_pic->format_type == FORM_STANDARD)
			{
				memset(pixbuf, 0x0, swidth);
				getpix_std_line(buffer, pixbuf, BitsPerPixel, planelength, swidth);

				x = 0;
				do
				{
					pix = pixbuf[x];
					pal = palette + pix + pix + pix;

					ziel++;
					*ziel++ = *pal;
					*ziel++ = *(pal + 1);
					*ziel++ = *(pal + 2);
				} while (++x < swidth);

				buffer += sw;
			}
			ziel += v;
		} while (++y < stripheight);

		Mfree(pixbuf);
	}

	return 0;
}


/* Setzt MFDBs auf benîtigte Werte */
void set_MFDB(MFDB *srcform, MFDB *dstform, void *srcpic, unsigned char BitsPerPixel)
{
	srcform->fd_addr = srcpic;
	srcform->fd_w = swidth;
	srcform->fd_h = sheight;
	srcform->fd_wdwidth = (swidth + 15) / 16;
	srcform->fd_stand = 0;
	srcform->fd_nplanes = BitsPerPixel;

	dstform->fd_addr = NULL;
	/* Mehr braucht nicht */
}


/* fÅllt die Treiberinfostruktur */
short actualize_DevParam(WORD gdos_dev, DevParamS *DevParam)
{
	WORD work_in[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0 };
	WORD gdos_handle;
	float res;

	/* DevParamS-Struktur initialisieren */
	work_in[0] = gdos_dev;
	work_in[11] = DevParam->paperform;	/* Papierformat setzen */

#if 0
	printf("Drucker îffnen\n");
#endif
	memset(DevParam, 0, sizeof(DevParamS));
	v_ext_opnwk(work_in, &gdos_handle, work_out);
	if (!gdos_handle)
	{
		services->f_alert(alerts[NO_OPEN], NULL, NULL, NULL, 1);
		return -1;
	}

	DevParam->devID = gdos_dev;			/* GDOS-GerÑte-ID */
	vq_extnd(gdos_handle, 0, work_out);	/* Modus v_opnwk */
	DevParam->prtwidth = work_out[0];	/* bedruckbare Breite in Pixeln */
	DevParam->prtheight = work_out[1];	/* bedruckbare Hîhe in Pixeln */
	DevParam->pixwidth = work_out[3];	/* Pixelgrîûe in Êm */
	DevParam->pixheight = work_out[4];	/* Pixelgrîûe in Êm */

	vq_extnd(gdos_handle, 1, work_out);	/* Modus vq_extnd */
	DevParam->depth = work_out[4];		/* Farbtiefe in Bit */
	DevParam->can_scale = work_out[30] & 0x01;	/* Bit 0 gesetzt - kann der Treiber skalieren? */
	DevParam->paperform = work_out[44];	/* Papierformat */
	if (work_out[20] != 0)
	{
		/* work_out[20]: 1=1/10, 2=1/100, 3=1/1000 Êm */
		res = pow10(work_out[20]);
		DevParam->pixwidth = work_out[21] / res;	/* Pixelwidth in Êm */
		DevParam->pixheight = work_out[22] / res;	/* Pixelheight in Êm */
		DevParam->horres = work_out[23];	/* horizontale Auflîsung in dpi */
		DevParam->verres = work_out[24];	/* vertikale Auflîsung in dpi */
		DevParam->leftBorder = work_out[40];
		DevParam->upperBorder = work_out[41];
		DevParam->rightBorder = work_out[42];
		DevParam->lowerBorder = work_out[43];
	} else
	{
		DevParam->leftBorder = 0;
		DevParam->upperBorder = 0;
		DevParam->rightBorder = 0;
		DevParam->lowerBorder = 0;
	}

	DevParam->pixwidth /= 1000;			/* aus Êm mm machen */
	DevParam->pixwidth /= 1000;			/* aus Êm mm machen */

	v_clswk(gdos_handle);

	return 0;
}
