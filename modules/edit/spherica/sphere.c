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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =========================================================*/
/*                  Spherical Image V0.5                    */
/* Version 0.1  --  25.04.97                                */
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "spherica.rsh"
#include "spherica.h"

#undef random
#define random( x ) (rand() % (x))

#define DEST_PIC    0
#define BUMP_PIC    1

#define TextCast    ob_spec.tedinfo->te_ptext

static SMURF_PIC *picture;
static SMURF_PIC *bump_pic;

static SMURF_PIC prev_pic;

static unsigned long spec_strn = 128;
static unsigned long spec_pow = 50;
static unsigned long spec_filt = 0;
static unsigned long roughness = 0;

static unsigned long bump_str = 0;

static unsigned long trans_strn = 0;

static unsigned long R_KUGEL = 255;
static unsigned long G_KUGEL = 0;
static unsigned long B_KUGEL = 128;

static unsigned long R_AMB = 0;
static unsigned long G_AMB = 32;
static unsigned long B_AMB = 64;

static long M[4];


static SERVICE_FUNCTIONS *service;

MOD_INFO module_info = {
#if defined(__68881__) || defined(_M68881)
	"Spherical Image¿ (FPU)",
#else
	"Spherical Image¿",
#endif
	0x0050,
	"Olaf Piesche",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ "", "", "", "" },
	{ "", "", "", "" },
	{ "", "", "", "" },
	{ { 0, 64 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	2,
	COMPILER_ID,
	{ "Zielbild", "Bumpmap", NULL, NULL, NULL, NULL }
};

MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0,	0, 0, 0,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};


static WINDOW my_window;
static OBJECT *maintree;

static SLIDER krsl;
static SLIDER kgsl;
static SLIDER kbsl;
static SLIDER spst;
static SLIDER sppw;
static SLIDER spfl;
static SLIDER arsl;
static SLIDER agsl;
static SLIDER absl;
static SLIDER rough;
static SLIDER bumpsl;
static SLIDER transp;


static void compute_sphere(short wid, short hgt, uint8_t *addr, short crhx, short crhy, float rad)
{
	double gx, gy, gz;
	double nx, ny, nz;
	float lx, ly, lz, lx0, ly0, lz0;
	float nlx, nly, nlz;
	double l_abs, n_abs;
	float xst;
	float l_fak;

	float xb, yb;
	float div, spec_mult;
	short spr;
	short rk, bk, gk;
	short x, y;
	long bmp_xoff, bmp_yoff;
	long bmp_width = 0;
	long bmp_height = 0;
	long bmpxmin = 0;
	long bmpymin = 0;
	long bmpxmax = 0;
	long bmpymax = 0;
	float bxs = 0;
	float bys = 0;
	float bump_fak = 0;
	float radq;
	float zq, xq, yq;
	float pointx, pointy;
	uint8_t *bumpmap = 0;

	long xoff, yoff;
	long linelen;

	if (maintree[BUMP_CHECK].ob_state & OS_SELECTED)
	{
		service->reset_busybox(0, "Bump PreCalc...");
		bmp_width = bump_pic->pic_width;
		bmp_height = bump_pic->pic_height;
		bmpxmin = -(bmp_width / 2);
		bmpymin = -(bmp_height / 2);
		bmpxmax = bmp_width / 2;
		bmpymax = bmp_height / 2;
		bumpmap = ((uint8_t *) bump_pic->pic_data) + (bmpxmax) + (bmpymax * bmp_width);
		bxs = (float) (atol(maintree[BUMP_XSC].ob_spec.tedinfo->te_ptext)) / 1000.0F;
		bys = (float) (atol(maintree[BUMP_YSC].ob_spec.tedinfo->te_ptext)) / 1000.0F;
		bump_fak = (float) bump_str / 4096.0F;
	}

	service->reset_busybox(0, "Lights PreCalc...");

	spec_mult = (float) spec_pow / 8.0F;

	M[2] = 1600;
	radq = rad * rad;

	/* Lichtvektor */
	lx = -120;
	ly = -120;
	lz = 120;

	/* Lichtvektor normalisieren */
	l_abs = sqrt(lx * lx + ly * ly + lz * lz);
	lx0 = lx / l_abs;
	ly0 = ly / l_abs;
	lz0 = lz / l_abs;

	linelen = (long) wid *3L;

#if 0
	picdata = ((uint8_t *) addr) + ((long) (wid / 2L) * 3L) + ((long) (hgt / 2) * linelen);
#endif

	yoff = 0;

	service->reset_busybox(0, "Rumkugeln...");

	for (y = 0; y < hgt && !Bconstat(2); y++)
	{
		xoff = 0;
		if (!(y & 3))
			service->busybox((y << 7L) / hgt);

		for (x = 0; x < wid; x++)
		{
			pointx = +x - crhx;
			pointy = +y - crhy;
			xq = (pointx) * (pointx);
			yq = (pointy) * (pointy);

			if (xq + yq <= radq)
			{
				zq = (radq - xq - yq);
				gz = sqrt(zq);
				gx = pointx;
				gy = pointy;

				/*---------------------------- Normalenvektor des Punktes der Kugel berechnen */
				nx = pointx;
				ny = pointy;
				nz = gz;

				/*----------- normalisieren */
				n_abs = sqrt(nx * nx + ny * ny + nz * nz);
				nx /= n_abs;
				ny /= n_abs;
				nz /= n_abs;


#if 0
				printf("\n nx: %.4f   gx: %.4f", nx, gx);
				printf("\n ny: %.4f   gy: %.4f", ny, gy);
				printf("\n nz: %.4f   gz: %.4f", nz, gz);
				printf("\n---------------------------------------");
				evnt_keybd();
#endif

				/*--------------------------- Ambiente Helligkeit (0-255) */
				rk = (short) R_AMB;
				gk = (short) G_AMB;
				bk = (short) B_AMB;

				/*--------------------------- Roughness */
				if (maintree[ROUGH_CHECK].ob_state & OS_SELECTED)
				{
					nx += random(roughness + 1) / 256.0F;
					ny += random(roughness + 1) / 256.0F;
					nz += random(roughness + 1) / 256.0F;
				}

				/*--------------------------- Bumpmap einrechnen */
				if (maintree[BUMP_CHECK].ob_state & OS_SELECTED)
				{
					if (nz != 0.0)
						div = nx / nz;
					else
						div = 1;
					bmp_xoff = (long) ((div * (float) bmp_width) * bxs);

					if (nz != 0.0)
						div = ny / nz;
					else
						div = 1;
					bmp_yoff = (long) ((div * (float) bmp_height) * bys);

					if (bmp_xoff <= bmpxmin)
						bmp_xoff += bmp_width;
					else if (bmp_xoff >= bmpxmax)
						bmp_xoff -= bmp_width;
					if (bmp_yoff <= bmpymin)
						bmp_yoff += bmp_height;
					else if (bmp_yoff >= bmpymax)
						bmp_yoff -= bmp_height;

					bmp_xoff = bmp_xoff;
					bmp_yoff *= bmp_width;

					xb = *(bumpmap + bmp_xoff + bmp_yoff);
					xb -= *(bumpmap + bmp_xoff + bmp_yoff + 1);
					yb = *(bumpmap + bmp_xoff + bmp_yoff + 1);
					yb -= *(bumpmap + bmp_xoff + bmp_yoff + 1 + bmp_width);

					nx += (xb * bump_fak);
					ny += (yb * bump_fak);

					/*----------- Normalenvektor wieder normalisieren */
					n_abs = sqrt(nx * nx + ny * ny + nz * nz);
					nx /= n_abs;
					ny /= n_abs;
					nz /= n_abs;
				}

				/*--------------------------- Diffuses Licht */
				if (R_KUGEL || G_KUGEL || B_KUGEL)
				{
					l_fak = lx0 * nx + ly0 * ny + lz0 * nz;
					rk += ((float) R_KUGEL * l_fak);
					gk += ((float) G_KUGEL * l_fak);
					bk += ((float) B_KUGEL * l_fak);
				}

				/*--------------------------- Glanzpunkt */
				if (spec_strn)
				{
					nlx = lx - gx;
					nly = ly - gy;
					nlz = lz - gz;
					n_abs = sqrt(nlx * nlx + nly * nly + nlz * nlz);
					nlx /= n_abs;
					nly /= n_abs;
					nlz /= n_abs;

					xst = nx * nlx + ny * nly + nz * nlz;
					xst = (2.0F * xst * nz - nlz);
					if (xst < 0)
						xst = 0;
					spr = spec_strn * pow(xst, spec_mult);
					rk += spr;
					gk += spr;
					bk += spr;
				}
#if 0
				/*--------------------------- Textur einrechnen */
				if (maintree[TEXT_CHECK].ob_state & OS_SELECTED)
				{
					if (nz != 0.0)
						div = nx / nz;
					else
						div = 1;
					tex_xoff = (long) ((div * (float) tex_width) * txs);

					if (nz != 0.0)
						div = ny / nz;
					else
						div = 1;
					tex_yoff = (long) ((div * (float) tex_height) * tys);

					if (tex_xoff < texxmin)
						tex_xoff += tex_width;
					else if (tex_xoff > texxmax)
						tex_xoff -= tex_width;
					if (tex_yoff < texymin)
						tex_yoff += tex_height;
					else if (tex_yoff > texymax)
						tex_yoff -= tex_height;

					tex_xoff *= 3L;
					tex_yoff *= tex_width * 3L;

					rk = ((long) rk * (long) *(texture + tex_xoff + tex_yoff)) >> 8;
					gk = ((long) gk * (long) *(texture + tex_xoff + tex_yoff + 1)) >> 8;
					bk = ((long) bk * (long) *(texture + tex_xoff + tex_yoff + 2)) >> 8;
				}
#endif

#if 0
				/*
				 *   Transparenz 
				 */
				/*if(trans_strn) */
				{
					nz = -nz;
					/* n und g normieren */
					n_abs = sqrt(nx * nx + ny * ny + nz * nz);
					nx /= n_abs;
					ny /= n_abs;
					nz /= n_abs;

					gz += 1;
					n_abs = sqrt(gx * gx + gy * gy + gz * gz);
					gx /= n_abs;
					gy /= n_abs;
					gz /= n_abs;

#if 0
					if (!(x & 15))
					{
						printf("\n n: %.3f | %.3f | %.3f", nx, ny, nz);
						printf("\n g: %.3f | %.3f | %.3f", gx, gy, gz);
					}
#endif

					/* 2 2dimensionale Winkel (n->g) ausrechnen */
					frx = acos((gx * nx) * (gz * nz));
					fry = acos((gy * ny) * (gz * nz));

					/*  Winkel n->r ausrechnen */
					frx = 1.5 * (frx - 90.0);
					fry = 1.5 * (fry - 90.0);

					pic_xoff = (long) (cos(frx) * (float) pointx);
					pic_yoff = (long) (cos(fry) * (float) pointy);


					pic_xoff *= 3L;
					pic_yoff *= (long) wid *3L;

					rk = *(picdata + pic_xoff + pic_yoff);
					gk = *(picdata + pic_xoff + pic_yoff + 1);
					bk = *(picdata + pic_xoff + pic_yoff + 2);
				}
#endif

				/*--- Farbe Clippen ---*/
				if (rk < 0)
					rk = 0;
				else if (rk > 255)
					rk = 255;
				if (gk < 0)
					gk = 0;
				else if (gk > 255)
					gk = 255;
				if (bk < 0)
					bk = 0;
				else if (bk > 255)
					bk = 255;

				/*--- Pixel schreiben ---*/
				*(addr + xoff + yoff) = rk;
				*(addr + xoff + yoff + 1) = gk;
				*(addr + xoff + yoff + 2) = bk;
			}

			xoff += 3L;
		}								/* x-loop */

		yoff += linelen;
	}									/* y - loop */
}


static short open_window(GARGAMEL *smurf_struct)
{
	WORD t;
	short back;

	/* Resource Umbauen */
	for (t = 0; t < NUM_OBS; t++)
		rsrc_obfix(rs_object, t);

	maintree = rs_trindex[SPH_MAIN];	/* Resourcebaum holen */

	my_window.whandlem = 0;				/* evtl. Handle l”schen */
	my_window.module = smurf_struct->module_number;
	strcpy(my_window.wtitle, module_info.mod_name);	/* Titel reinkopieren   */
	my_window.wnum = 1;					/* Fenster nummer 1...  */
	my_window.wx = -1;					/* Fenster X-...    */
	my_window.wy = -1;					/* ...und Y-Pos     */
	my_window.ww = maintree->ob_width;	/* Fensterbreite    */
	my_window.wh = maintree->ob_height;	/* Fensterh”he      */
	my_window.resource_form = maintree;	/* Resource         */
	my_window.picture = NULL;			/* kein Bild.       */
	my_window.pflag = 0;				/* kein Bild.       */
	my_window.editob = BUMP_XSC;
	my_window.nextedit = 0;
	smurf_struct->wind_struct = &my_window;	/* und die Fensterstruktur in die Gargamel */

	back = smurf_struct->services->f_module_window(&my_window);	/* Gib mir 'n Fenster! */

	return back;
}


static short compute_preview(GARGAMEL *smurf_struct)
{
	prev_pic.pic_width = maintree[PREV_BOX].ob_width;
	prev_pic.pic_height = maintree[PREV_BOX].ob_height;
	prev_pic.depth = 24;
	prev_pic.format_type = FORM_PIXELPAK;
	prev_pic.col_format = RGB;
	prev_pic.zoom = 0;

	/*--------- Wenn Bumpmapping selektiert ist, versuchen die Bumpmap frs Preview zu holen */
	if (maintree[BUMP_CHECK].ob_state & OS_SELECTED)
	{
		bump_pic = service->get_pic(BUMP_PIC, smurf_struct->module_number, &module_info, 8, FORM_PIXELPAK, GREY);

		if (bump_pic == NULL)
			return -1;
	}

	my_window.picture = &prev_pic;
	my_window.pic_xpos = maintree[PREV_BOX].ob_x;
	my_window.pic_ypos = maintree[PREV_BOX].ob_y;
	my_window.clipwid = maintree[PREV_BOX].ob_width;
	my_window.cliphgt = maintree[PREV_BOX].ob_height;

	prev_pic.pic_data = service->SMalloc(my_window.clipwid * my_window.cliphgt * 3L);

	graf_mouse(BUSYBEE, NULL);
	compute_sphere(prev_pic.pic_width, prev_pic.pic_height, prev_pic.pic_data, prev_pic.pic_width / 2, prev_pic.pic_height / 2, 40);
	graf_mouse(ARROW, NULL);

	return 0;
}


static short handle_bevt(GARGAMEL *smurf_struct)
{
	WORD button;
	short ret = M_WAITING;

	button = smurf_struct->event_par[0];

	switch (button)
	{
	case LOS:
		return M_STARTED;

	case KR_F:
	case KR_R:
		R_KUGEL = service->slider(&krsl);
		break;
	case KG_F:
	case KG_R:
		G_KUGEL = service->slider(&kgsl);
		break;
	case KB_F:
	case KB_R:
		B_KUGEL = service->slider(&kbsl);
		break;

	case SSTR_F:
	case SSTR_R:
		spec_strn = service->slider(&spst);
		break;
	case SPOW_F:
	case SPOW_R:
		spec_pow = service->slider(&sppw);
		break;
	case SFLT_F:
	case SFLT_R:
		spec_filt = service->slider(&spfl);
		break;

	case AR_F:
	case AR_R:
		R_AMB = service->slider(&arsl);
		break;
	case AG_F:
	case AG_R:
		G_AMB = service->slider(&agsl);
		break;
	case AB_F:
	case AB_R:
		B_AMB = service->slider(&absl);
		break;

	case BUMP_F:
	case BUMP_R:
		bump_str = service->slider(&bumpsl);
		break;

	case ROUGH_F:
	case ROUGH_R:
		roughness = service->slider(&rough);
		break;


	case TRANSP_F:
	case TRANSP_R:
		trans_strn = service->slider(&transp);
		break;

	case PREV_BOX:
		if (compute_preview(smurf_struct) == 0)
			ret = M_MODPIC;
		break;
	}

	return ret;
}




static void make_sliders(void)
{
	krsl.regler = KR_R;
	krsl.schiene = KR_F;
	krsl.rtree = maintree;
	krsl.txt_obj = KR_A;
	krsl.min_val = 0;
	krsl.max_val = 255;
	krsl.window = &my_window;
	service->set_slider(&krsl, R_KUGEL);

	kgsl.regler = KG_R;
	kgsl.schiene = KG_F;
	kgsl.rtree = maintree;
	kgsl.txt_obj = KG_A;
	kgsl.min_val = 0;
	kgsl.max_val = 255;
	kgsl.window = &my_window;
	service->set_slider(&kgsl, G_KUGEL);

	kbsl.regler = KB_R;
	kbsl.schiene = KB_F;
	kbsl.rtree = maintree;
	kbsl.txt_obj = KB_A;
	kbsl.min_val = 0;
	kbsl.max_val = 255;
	kbsl.window = &my_window;
	service->set_slider(&kbsl, B_KUGEL);

	spst.regler = SSTR_R;
	spst.schiene = SSTR_F;
	spst.rtree = maintree;
	spst.txt_obj = SSTR_A;
	spst.min_val = 0;
	spst.max_val = 255;
	spst.window = &my_window;
	service->set_slider(&spst, spec_strn);

	sppw.regler = SPOW_R;
	sppw.schiene = SPOW_F;
	sppw.rtree = maintree;
	sppw.txt_obj = SPOW_A;
	sppw.min_val = 0;
	sppw.max_val = 255;
	sppw.window = &my_window;
	service->set_slider(&sppw, spec_pow);

	spfl.regler = SFLT_R;
	spfl.schiene = SFLT_F;
	spfl.rtree = maintree;
	spfl.txt_obj = SFLT_A;
	spfl.min_val = 0;
	spfl.max_val = 255;
	spfl.window = &my_window;
	service->set_slider(&spfl, spec_filt);

	arsl.regler = AR_R;
	arsl.schiene = AR_F;
	arsl.rtree = maintree;
	arsl.txt_obj = AR_A;
	arsl.min_val = 0;
	arsl.max_val = 255;
	arsl.window = &my_window;
	service->set_slider(&arsl, R_AMB);

	agsl.regler = AG_R;
	agsl.schiene = AG_F;
	agsl.rtree = maintree;
	agsl.txt_obj = AG_A;
	agsl.min_val = 0;
	agsl.max_val = 255;
	agsl.window = &my_window;
	service->set_slider(&agsl, G_AMB);

	absl.regler = AB_R;
	absl.schiene = AB_F;
	absl.rtree = maintree;
	absl.txt_obj = AB_A;
	absl.min_val = 0;
	absl.max_val = 255;
	absl.window = &my_window;
	service->set_slider(&absl, B_AMB);

	bumpsl.regler = BUMP_R;
	bumpsl.schiene = BUMP_F;
	bumpsl.rtree = maintree;
	bumpsl.txt_obj = BUMP_A;
	bumpsl.min_val = 0;
	bumpsl.max_val = 255;
	bumpsl.window = &my_window;
	service->set_slider(&bumpsl, bump_str);

	rough.regler = ROUGH_R;
	rough.schiene = ROUGH_F;
	rough.rtree = maintree;
	rough.txt_obj = ROUGH_A;
	rough.min_val = 0;
	rough.max_val = 64;
	rough.window = &my_window;
	service->set_slider(&rough, roughness);

	transp.regler = TRANSP_R;
	transp.schiene = TRANSP_F;
	transp.rtree = maintree;
	transp.txt_obj = TRANSP_A;
	transp.min_val = 0;
	transp.max_val = 100;
	transp.window = &my_window;
	service->set_slider(&transp, trans_strn);
}


static void init_rsc(void)
{
#if 0
	maintree[TRAN_IDX].ob_state |= OS_DISABLED;
	maintree[TEX_XSC].ob_state |= OS_DISABLED;
	maintree[TEX_YSC].ob_state |= OS_DISABLED;
	maintree[TEX_XOFF].ob_state |= OS_DISABLED;
	maintree[TEX_YOFF].ob_state |= OS_DISABLED;
#endif
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*              Spherical Image¿ V0.5               */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	static short crhx, crhy;

	service = smurf_struct->services;

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
#if defined(__68881__) || defined(_M68881)
		if (!(smurf_struct->services->CPU_type & FPU))
		{
			smurf_struct->services->f_alert("Dieses Modul l„uft nur mit einem Fliežkommaprozessor.", NULL, NULL, NULL, 1);
			smurf_struct->module_mode = M_EXIT;
			return;
		}
#endif

		if (open_window(smurf_struct) != -1)
			smurf_struct->module_mode = M_WAITING;
		else
			smurf_struct->module_mode = M_MODERR;

		make_sliders();
		init_rsc();
		break;

	/*------------------------------------------------------------ loslegen? */
	case MEXEC:
		M[0] = 0;
		M[1] = 0;
		M[2] = 0;

		compute_sphere(picture->pic_width, picture->pic_height, picture->pic_data, crhx, crhy, atoi(maintree[XRAD].TextCast));

		smurf_struct->event_par[0] = 0;
		smurf_struct->module_mode = M_PICDONE;
		break;

	/*----------------------------------------------- Smurf will die Bilder bergeben */
	case MPICS:
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			smurf_struct->event_par[0] = 24;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = RGB;
			smurf_struct->module_mode = M_PICTURE;
			break;

		case BUMP_PIC:
			if (maintree[BUMP_CHECK].ob_state & OS_SELECTED)
			{
				smurf_struct->event_par[0] = 8;
				smurf_struct->event_par[1] = FORM_PIXELPAK;
				smurf_struct->event_par[2] = GREY;
				smurf_struct->module_mode = M_PICTURE;
			} else
			{
				smurf_struct->module_mode = M_WAITING;
			}
			break;

		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

	/*------------------------------------------------ Bilder von Smurf abholen */
	case MPICTURE:
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			picture = smurf_struct->smurf_pic;
			break;
		case BUMP_PIC:
			bump_pic = smurf_struct->smurf_pic;
			break;
		}
		smurf_struct->module_mode = M_WAITING;
		break;

	/*------------------------------------------------------------ Buttonevent kommt */
	case MBEVT:
		smurf_struct->module_mode = handle_bevt(smurf_struct);
		break;

	/*---- Smurf fragt: Fadenkreuz? */
	case MCROSSHAIR:
		smurf_struct->event_par[0] = 1;
		smurf_struct->event_par[1] = 0;
		smurf_struct->module_mode = M_CROSSHAIR;
		break;

	/*--- Smurf fragt: und die Start-Koordinaten? */
	case MCH_DEFCOO:
		smurf_struct->event_par[0] = smurf_struct->smurf_pic->pic_width / 2;
		smurf_struct->event_par[1] = smurf_struct->smurf_pic->pic_height / 2;
		smurf_struct->module_mode = M_CHDEFCOO;
		break;

	/*--- Smurf sagt: hier sind die eingestellten Koordinaten */
	case MCH_COORDS:
		crhx = smurf_struct->event_par[0];
		crhy = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_WAITING;
		break;

	/*--- Smurf sagt: dein Bild ist fertig gedithert. */
	case MDITHER_READY:
		service->redraw_window(&my_window, NULL, PREV_BOX, 0);
		smurf_struct->module_mode = M_WAITING;
		service->SMfree(prev_pic.pic_data);
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
