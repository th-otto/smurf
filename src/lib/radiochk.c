/* ******************************************************************** */
/* Handler fÅr Userdefined Objects: Radiobuttons & Checkboxen. 			*/
/*																		*/
/* (C) MAI 1992 Stefan Piesche 											*/
/*		Umgeschrieben fÅr Farbicons: 29.07.95, Olaf.																	*/
/* ******************************************************************** */
/*																		*/
/* Die Routinen erwarten lediglich zwei Globale Parameter, die im 		*/
/* Hauptmodul definiert sein mÅssen.									*/
/* OBJECT *u_tree:	Zeiger auf den Objektbaum in dem die User-IMG sind.	*/
/* int	handle:		VDI-handle											*/
/*																		*/
/* Die Routinen installieren sich selbst, es muû allerdings vor jedem	*/
/* objc_draw der Dialogbox einmal (!) die Routine f_treewalk aufgerufen */
/* werden.	(Nur vor dem ersten objc_draw! Olaf.)						*/
/*																		*/
/* Die Objekte fÅr Radibuttons mÅssen folgendermaûen aufgebaut sein:	*/
/* Zuerst sollte der TEXT geschrieben werden.							*/
/* Dann wird Åber den Text eine transparente I-BOX gelegt, die links	*/
/* Genau 2 Zeichen breit Åber den Text hinaussteht (NICHT Adoptieren!).	*/
/* Der Status OS_SELECTED dieser I-BOXEN kann nachher ÅberprÅft werden	*/
/* Die I-BOXEN mÅssen definiert sein:									*/
/*																		*/
/* a) Radio-Buttons: OF_SELECTABLE,RADIOBUTTON; das obere Byte des Typs	*/
/*						ist 18 (0x12). Ein (!) Radiobutton muû bereits	*/
/*						als OS_SELECTED definiert sein.					*/
/*																		*/
/*																		*/
/* b) Check-Boxen:	OF_SELECTABLE; oberes Byte: 19 (0x13)				*/
/*																		*/
/*																		*/
/* ******************************************************************** */

/* Statusabfrage:	if( OBJEKTBAUM [BUTTON-OBJEKT].ob_state & OS_SELECTED) */
/*					-> 1=selektiert, 0=unselektiert						*/

#include <stdio.h>
#include "import.h"
#include "smurf.h"
#include "smurf_st.h"
#include "smurfine.h"
#include "smurf_f.h"
#include "ext_rsc.h"

#define RADIOBUTTON   	43				/* Oberes Byte.... */
#define CHECKBOX  		44				/* FÅr den Res-Editor */
#define CYCLEBUTTON		45				/* FÅr den Res-Editor */
#define CORNER_IMAGE	46				/* FÅr den Res-Editor */

static USERBLK check_user;
static USERBLK radio_user;
static USERBLK cycle_user;
static USERBLK corner_user;

#if 0
static USERBLK angle_user;
#endif



static WORD cdecl f_do_radio(PARMBLK *parm)
{
	WORD pxy[8];
	WORD ob_x, ob_y;
	WORD color_index[2];
	WORD color_index2[2];
	WORD Radiobutton;
	WORD write_mode;
	CICONBLK *quell_img;				/* jetzt Farbicon (Struktur: siehe AES.H!) */
	USERBLK *buttonimg;
	MFDB source;
	MFDB dest;
	BOOLEAN coloricon;
	
	if (Sys_info.bitplanes > 8)
		write_mode = S_AND_D;
	else
		write_mode = S_OR_D;

	color_index[0] = 1;
	color_index[1] = 0;

	color_index2[0] = 0;
	color_index2[1] = 1;

	ob_x = parm->pb_x;
	ob_y = parm->pb_y;

	if (parm->pb_currstate & OS_SELECTED)
		Radiobutton = SelectedRadio;
	else
		Radiobutton = Radio;

	coloricon = FALSE;
	/* has it been converted in xrsrc.c? */
	if ((u_tree[Radiobutton].ob_type & 0xff) == G_USERDEF)
	{
		buttonimg = u_tree[Radiobutton].ob_spec.userblk;
		if (Sys_info.Max_col >= 4)
		{
			coloricon = TRUE;
			quell_img = (CICONBLK *) buttonimg->ub_parm;
		} else
		{
			quell_img = u_tree[Radiobutton].ob_spec.ciconblk;
		}
	} else
	{
		quell_img = u_tree[Radiobutton].ob_spec.ciconblk;
	}

	dest.fd_addr = NULL;
	source.fd_w = quell_img->monoblk.ib_wicon;					/* Radiobuttons/Checkboxen mit 16*16 */
	source.fd_h = quell_img->monoblk.ib_hicon;
	source.fd_wdwidth = 1;				/* 16 Pixel einer Plane = 1 Word */
	source.fd_stand = 0;

	/* AES-Hintergrundfarbe ermitteln und Box zeichnen */
	pxy[0] = ob_x;
	pxy[1] = ob_y;
	pxy[2] = ob_x + source.fd_w - 1;
	pxy[3] = ob_y + source.fd_h - 1;
	vsf_color(Sys_info.vdi_handle, Sys_info.AES_bgcolor);
	vswr_mode(Sys_info.vdi_handle, MD_REPLACE);
	vr_recfl(Sys_info.vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = source.fd_w - 1;
	pxy[3] = source.fd_h - 1;
	pxy[4] = ob_x;
	pxy[5] = ob_y;
	pxy[6] = ob_x + pxy[2];
	pxy[7] = ob_y + pxy[3];

	if (coloricon)
	{
		source.fd_nplanes = 1;			/* Bitplanes des Quellimages */
		source.fd_addr = quell_img->mainlist->col_mask;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		vrt_cpyfm(Sys_info.vdi_handle, S_AND_NOTD, pxy, &source, &dest, color_index2);	/* Farbe->Farbe - Rastercopy */

		source.fd_addr = quell_img->mainlist->col_data;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = quell_img->mainlist->num_planes;	/* Bitplanes des Quellimages */
		vro_cpyfm(Sys_info.vdi_handle, write_mode, pxy, &source, &dest);	/* Farbe->Farbe - Rastercopy */
	} else
	{
		source.fd_addr = quell_img->monoblk.ib_pdata;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = 1;			/* Bitplanes des Quellimages */
		vrt_cpyfm(Sys_info.vdi_handle, MD_REPLACE, pxy, &source, &dest, color_index);	/* s/w->Farbe - Rastercopy */
	}

	return parm->pb_currstate & ~OS_SELECTED;
}


WORD cdecl f_do_checkbox(PARMBLK *parm)
{
	WORD pxy[8];
	WORD ob_x, ob_y;
	WORD color_index[2];
	WORD color_index2[2];
	WORD Checkbox;
	WORD write_mode;
	CICONBLK *quell_img;
	USERBLK *buttonimg;
	MFDB source;
	MFDB dest;
	BOOLEAN coloricon;

	if (Sys_info.bitplanes > 8)
		write_mode = S_AND_D;
	else
		write_mode = S_OR_D;

	color_index[0] = 1;
	color_index[1] = 0;

	color_index2[0] = 0;
	color_index2[1] = 1;

	ob_x = parm->pb_x;
	ob_y = parm->pb_y;

	if (parm->pb_currstate & OS_SELECTED)
		Checkbox = SelectedCheck;
	else
		Checkbox = Check;

	coloricon = FALSE;
	/* has it been converted in xrsrc.c? */
	if ((u_tree[Checkbox].ob_type & 0xff) == G_USERDEF)
	{
		buttonimg = u_tree[Checkbox].ob_spec.userblk;
		if (Sys_info.Max_col >= 4)
		{
			coloricon = TRUE;
			quell_img = (CICONBLK *) buttonimg->ub_parm;
		} else
		{
			quell_img = u_tree[Checkbox].ob_spec.ciconblk;
		}
	} else
	{
		quell_img = u_tree[Checkbox].ob_spec.ciconblk;
	}

	dest.fd_addr = NULL;
	source.fd_w = quell_img->monoblk.ib_wicon;
	source.fd_h = quell_img->monoblk.ib_hicon;
	source.fd_wdwidth = 1;
	source.fd_stand = 0;

	pxy[0] = ob_x;
	pxy[1] = ob_y;
	pxy[2] = ob_x + source.fd_w - 1;
	pxy[3] = ob_y + source.fd_h - 1;
	vsf_color(Sys_info.vdi_handle, Sys_info.AES_bgcolor);
	vswr_mode(Sys_info.vdi_handle, MD_REPLACE);
	vr_recfl(Sys_info.vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = source.fd_w - 1;
	pxy[3] = source.fd_h - 1;
	pxy[4] = ob_x;
	pxy[5] = ob_y;
	pxy[6] = ob_x + pxy[2];
	pxy[7] = ob_y + pxy[3];

	if (coloricon)
	{
		source.fd_nplanes = 1;
		source.fd_addr = quell_img->mainlist->col_mask;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		vrt_cpyfm(Sys_info.vdi_handle, S_AND_NOTD, pxy, &source, &dest, color_index2);	/* Farb-Rastercopy */

		source.fd_addr = quell_img->mainlist->col_data;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = quell_img->mainlist->num_planes;	/* Bitplanes des Quellimages */
		vro_cpyfm(Sys_info.vdi_handle, write_mode, pxy, &source, &dest);	/* Farb-Rastercopy */
	} else
	{
		source.fd_addr = quell_img->monoblk.ib_pdata;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = 1;			/* Bitplanes des Quellimages */
		vrt_cpyfm(Sys_info.vdi_handle, MD_REPLACE, pxy, &source, &dest, color_index);	/* Farb-Rastercopy */
	}

	return parm->pb_currstate & ~OS_SELECTED;
}


WORD cdecl f_do_cycle(PARMBLK *parm)
{
	WORD pxy[8];
	WORD ob_x, ob_y;
	WORD color_index[2];
	WORD color_index2[2];
	WORD Cyclebutton;
	WORD write_mode;
	CICONBLK *quell_img;
	USERBLK *buttonimg;
	MFDB source;
	MFDB dest;
	BOOLEAN coloricon;

	if (Sys_info.bitplanes > 8)
		write_mode = S_AND_D;
	else
		write_mode = S_OR_D;

	color_index[0] = 1;
	color_index[1] = 0;
	color_index2[0] = 0;
	color_index2[1] = 1;

	ob_x = parm->pb_x;
	ob_y = parm->pb_y;

	if (parm->pb_currstate & OS_SELECTED)
		Cyclebutton = SelectedCycle;
	else
		Cyclebutton = Cycle;

	coloricon = FALSE;
	/* has it been converted in xrsrc.c? */
	if ((u_tree[Cyclebutton].ob_type & 0xff) == G_USERDEF)
	{
		buttonimg = u_tree[Cyclebutton].ob_spec.userblk;
		if (Sys_info.Max_col >= 4)
		{
			coloricon = TRUE;
			quell_img = (CICONBLK *) buttonimg->ub_parm;
		} else
		{
			quell_img = u_tree[Cyclebutton].ob_spec.ciconblk;
		}
	} else
	{
		quell_img = u_tree[Cyclebutton].ob_spec.ciconblk;
	}

	dest.fd_addr = NULL;
	source.fd_w = quell_img->monoblk.ib_wicon;
	source.fd_h = quell_img->monoblk.ib_hicon;
	source.fd_wdwidth = 2;
	source.fd_stand = 0;

	pxy[0] = ob_x;
	pxy[1] = ob_y;
	pxy[2] = ob_x + source.fd_w - 1;
	pxy[3] = ob_y + source.fd_h - 1;
	vsf_color(Sys_info.vdi_handle, Sys_info.AES_bgcolor);
	vswr_mode(Sys_info.vdi_handle, MD_REPLACE);
	vr_recfl(Sys_info.vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = source.fd_w - 1;
	pxy[3] = source.fd_h - 1;
	pxy[4] = ob_x;
	pxy[5] = ob_y;
	pxy[6] = ob_x + pxy[2];
	pxy[7] = ob_y + pxy[3];

	if (coloricon)
	{
		source.fd_nplanes = 1;
		source.fd_addr = quell_img->mainlist->col_mask;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		vrt_cpyfm(Sys_info.vdi_handle, S_AND_NOTD, pxy, &source, &dest, color_index2);	/* Farb-Rastercopy */

		source.fd_addr = quell_img->mainlist->col_data;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = quell_img->mainlist->num_planes;	/* Bitplanes des Quellimages */
		vro_cpyfm(Sys_info.vdi_handle, write_mode, pxy, &source, &dest);	/* Farb-Rastercopy */
	} else
	{
		source.fd_addr = quell_img->monoblk.ib_pdata;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = 1;			/* Bitplanes des Quellimages */
		vrt_cpyfm(Sys_info.vdi_handle, MD_REPLACE, pxy, &source, &dest, color_index);	/* Farb-Rastercopy */
	}

	return parm->pb_currstate & ~OS_SELECTED;
}


static WORD cdecl f_do_corner(PARMBLK *parm)
{
	WORD pxy[8];
	WORD ob_x, ob_y;
	WORD color_index[2];
	WORD color_index2[2];
	WORD write_mode;
	USERBLK *buttonimg;
	CICONBLK *quell_img;
	MFDB source;
	MFDB dest;
	BOOLEAN coloricon;

	if (Sys_info.bitplanes > 8)
		write_mode = S_AND_D;
	else
		write_mode = S_OR_D;

	pxy[0] = parm->pb_xc;
	pxy[1] = parm->pb_yc;
	pxy[2] = parm->pb_xc + parm->pb_wc - 1;
	pxy[3] = parm->pb_yc + parm->pb_hc - 1;
	vs_clip(Sys_info.vdi_handle, TRUE, pxy);

	color_index[0] = 1;
	color_index[1] = 0;
	color_index2[0] = 0;
	color_index2[1] = 1;

	ob_x = parm->pb_x;
	ob_y = parm->pb_y;

	coloricon = FALSE;
	/* has it been converted in xrsrc.c? */
	if ((u_tree[CORNER2].ob_type & 0xff) == G_USERDEF)
	{
		buttonimg = u_tree[CORNER2].ob_spec.userblk;
		if (Sys_info.Max_col >= 4)
		{
			coloricon = TRUE;
			quell_img = (CICONBLK *) buttonimg->ub_parm;
		} else
		{
			quell_img = u_tree[CORNER2].ob_spec.ciconblk;
		}
	} else
	{
		quell_img = u_tree[CORNER2].ob_spec.ciconblk;
	}

	dest.fd_addr = NULL;
	source.fd_w = quell_img->monoblk.ib_wicon;
	source.fd_h = quell_img->monoblk.ib_hicon;
	source.fd_wdwidth = 3;
	source.fd_stand = 0;

	ob_y = parm->pb_tree[ROOT].ob_y + parm->pb_tree[ROOT].ob_height - source.fd_h;

	pxy[0] = ob_x;
	pxy[1] = ob_y;
	pxy[2] = ob_x + source.fd_w - 1;
	pxy[3] = ob_y + source.fd_h - 1;
	vsf_color(Sys_info.vdi_handle, Sys_info.AES_bgcolor);
	vswr_mode(Sys_info.vdi_handle, MD_REPLACE);
	vr_recfl(Sys_info.vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = source.fd_w - 1;
	pxy[3] = source.fd_h - 1;
	pxy[4] = ob_x;
	pxy[5] = ob_y;
	pxy[6] = ob_x + pxy[2];
	pxy[7] = ob_y + pxy[3];

	if (coloricon)
	{
		source.fd_nplanes = 1;
		source.fd_addr = quell_img->mainlist->col_mask;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		vrt_cpyfm(Sys_info.vdi_handle, S_AND_NOTD, pxy, &source, &dest, color_index2);	/* Farb-Rastercopy */

		source.fd_addr = quell_img->mainlist->col_data;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = quell_img->mainlist->num_planes;	/* Bitplanes des Quellimages */
		vro_cpyfm(Sys_info.vdi_handle, write_mode, pxy, &source, &dest);	/* Farb-Rastercopy */
	} else
	{
		source.fd_addr = quell_img->monoblk.ib_pdata;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = 1;			/* Bitplanes des Quellimages */
		vrt_cpyfm(Sys_info.vdi_handle, MD_REPLACE, pxy, &source, &dest, color_index);	/* Farb-Rastercopy */
	}

	return parm->pb_currstate & ~OS_SELECTED;
}


#if 0
/* Winkelobjekt zeichnen
 * der eingestellte Winkel muû irgendwo in der OBJECT-Struktur
 * gespeichert werden - einen der bei IBOXes vorhandenen
 * Parameter muû ich also dazu miûbrauchen.
 */
static WORD cdecl drawAngleObject(PARMBLK *parm)
{
	CICONBLK *quell_img;
	MFDB source;
	MFDB dest;
	WORD pxy[8];
	WORD ob_x, ob_y;
	WORD color_index[2];
	WORD color_index2[2];
	USERBLK *buttonimg;
	WORD write_mode;
	BOOLEAN coloricon;

	if (Sys_info.bitplanes > 8)
		write_mode = S_AND_D;
	else
		write_mode = S_OR_D;

	color_index[0] = 1;
	color_index[1] = 0;
	color_index2[0] = 0;
	color_index2[1] = 1;

	ob_x = parm->pb_x;
	ob_y = parm->pb_y;

	coloricon = FALSE;
	/* has it been converted in xrsrc.c? */
	if ((u_tree[CORNER2].ob_type & 0xff) == G_USERDEF)
	{
		buttonimg = (USERBLK *) (u_tree[CORNER2].ob_spec.userblk);
		if (Sys_info.Max_col >= 4)
		{
			coloricon = TRUE;
			quell_img = (CICONBLK *) (buttonimg->ub_parm);
		} else
		{
			quell_img = u_tree[CORNER2].ob_spec.ciconblk;
		}
	} else
	{
		quell_img = u_tree[CORNER2].ob_spec.ciconblk;
	}

	dest.fd_addr = NULL;
	source.fd_w = 48;
	source.fd_h = 42;
	source.fd_wdwidth = 3;
	source.fd_stand = 0;

	pxy[0] = ob_x;
	pxy[1] = ob_y;
	pxy[2] = ob_x + source.fd_w - 1;
	pxy[3] = ob_y + source.fd_h - 1;
	vsf_color(Sys_info.vdi_handle, Sys_info.AES_bgcolor);
	vswr_mode(Sys_info.vdi_handle, MD_REPLACE);
	vr_recfl(Sys_info.vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = source.fd_w - 1;
	pxy[3] = source.fd_h - 1;
	pxy[4] = ob_x;
	pxy[5] = ob_y;
	pxy[6] = ob_x + pxy[2];
	pxy[7] = ob_y + pxy[3];

	if (coloricon)
	{
		source.fd_nplanes = 1;
		source.fd_addr = quell_img->mainlist->col_mask;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		vrt_cpyfm(Sys_info.vdi_handle, S_AND_NOTD, pxy, &source, &dest, color_index2);	/* Farb-Rastercopy */

		source.fd_addr = quell_img->mainlist->col_data;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = quell_img->mainlist->num_planes;	/* Bitplanes des Quellimages */
		vro_cpyfm(Sys_info.vdi_handle, write_mode, pxy, &source, &dest);	/* Farb-Rastercopy */
	} else
	{
		source.fd_addr = quell_img->monoblk.ib_pdata;	/* *mainlist: Zeiger auf CICON-Struktur, col_data sind die Bilddaten */
		source.fd_nplanes = 1;			/* Bitplanes des Quellimages */
		vrt_cpyfm(Sys_info.vdi_handle, MD_REPLACE, pxy, &source, &dest, color_index);	/* Farb-Rastercopy */
	}

	return parm->pb_currstate & ~OS_SELECTED;
}
#endif


static void f_init_tree(OBJECT *baum, WORD element)
{
	switch (baum[element].ob_type >> 8)
	{
	case CHECKBOX:
		baum[element].ob_type = (baum[element].ob_type & 0xff00) | G_USERDEF;
		baum[element].ob_spec.userblk = &check_user;
		check_user.ub_code = f_do_checkbox;
		break;
	case RADIOBUTTON:
		baum[element].ob_type = (baum[element].ob_type & 0xff00) | G_USERDEF;
		baum[element].ob_spec.userblk = &radio_user;
		radio_user.ub_code = f_do_radio;
		break;
	case CYCLEBUTTON:
		baum[element].ob_type = (baum[element].ob_type & 0xff00) | G_USERDEF;
		baum[element].ob_spec.userblk = &cycle_user;
		cycle_user.ub_code = f_do_cycle;
		break;
	case CORNER_IMAGE:
		baum[element].ob_type = (baum[element].ob_type & 0xff00) | G_USERDEF;
		baum[element].ob_spec.userblk = &corner_user;
		corner_user.ub_code = f_do_corner;
		break;
#if 0
	case ANGLE_OBJECT:
		baum[element].ob_type = (baum[element].ob_type & 0xff00) | G_USERDEF;
		baum[element].ob_spec.userblk = angle_user;
		angle_user.ub_code = drawAngleObject;
		break;
#endif
	}

	return;
}


void f_treewalk(OBJECT *tree, WORD start)
{
	WORD index = 0;

	for (index = tree[start].ob_head; index != -1 && index != start; index = tree[index].ob_next)
	{
		f_init_tree(tree, index);
		f_treewalk(tree, index);		/* rekursiv fÅr alle Kinder! */
	}
}
