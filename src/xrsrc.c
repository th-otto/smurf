/******************************************************************************
 * XRSRC.C
 *
 *			Extended Resource-Manager. RSC-Files can now have up to
 *			4294967295 bytes length.
 *			You can modify this source to handle more than one RSC-File
 *			by calling the MLOCAL-Functions
 *
 *				rs_load(pglobal, re_lpfname);
 *				rs_free(pglobal);
 *				rs_gaddr(pglobal, re_gtype, re_gindex, re_gaddr);
 *				rs_sadd(pglobal, re_stype, re_sindex, re_saddr);
 *
 *			with an integer-pointer to a 15 int array which will be
 *			handled as single global-arrays for each RSC-File.
 *
 *			This Source is copyrighted material by
 *					Oliver Groeger
 *					Graf-Konrad-Str.25
 *					8000 Munich 40
 *					Germany
 *
 * Version  :  1.00
 * Date     :  Aug 15th 1991
 * Author   :  Oliver Groeger
 *
 * Version  :  1.10
 * Datum    :  27 Sept. 1992
 * Autor    :  Olaf Meisiek
 * �nderung :  - xrsrc_load l�dt jetzt auch das alte Atari-RCS-Format
 *             - TOS-unabh�ngige Farbicon-Unterst�tzung hinzugef�gt
 *
 *
 * Anm.     : Um diese Routinen benutzen zu d�rfen, mu� man im
 *            Besitz eines Interface Originals sein!
 *            
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __PUREC__
#include <tos.h>
#include <vdi.h>
#include <aes.h>
#else
#include <osbind.h>
#include <gem.h>
#define DTA _DTA
#define d_length dta_size
#endif
#include "xrsrc.h"

#ifndef cdecl
#ifndef __PUREC__
#define cdecl
#endif
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef OS_SELECTED
#define OS_SELECTED SELECTED
#endif
#ifndef G_WHITE
#define G_WHITE			0
#define G_BLACK			1
#endif

/****** Compilerswitches ******************************************************/

#define COLOR_ICONS TRUE				/* Farbiconunterst�tzung?              */
#define SAVE_MEMORY FALSE				/* Speicherplatz bei Farbicons sparen? */

#ifdef __TURBOC__
#pragma warn -sig						/* Warnung "Conversion may loose significant digits" ausschalten */
#endif

/****** TYPES ****************************************************************/

#ifndef __MYDIAL__
typedef struct
{
	USERBLK ublk;
	UWORD old_type;
} OBBLK;
#endif

#ifndef R_IBPTEXT /* misspelled as R_IPBTEXT in original Pure-C headers */
#define R_IBPTEXT 13
#endif

typedef struct {
	OBBLK *userblks;
	CICON *coloricons;
	WORD num_coloricons;
	OBJECT **ap_ptree;
	RSXHDR *ap_rsc;
	WORD ap_rsclen;
} XRSRC_GLOBAL;

/****** VARIABLES ************************************************************/

WORD xrsrc_mustexist;

static WORD xgl_wbox;
static WORD xgl_hbox;
static GRECT xdesk;
static WORD xvdi_handle;

static XRSRC_GLOBAL *rs_global;
static RSXHDR *rs_hdr; /* real start of resource */
static RSXHDR *hdr_buf; /* converted RSHDR */

#if COLOR_ICONS == TRUE
static WORD farbtbl[256][32];
static ULONG farbtbl2[256];
static WORD is_palette;
static WORD rgb_palette[256][4];
static WORD xpixelbytes;
static WORD xscrn_planes;
#endif

/*****************************************************************************/

static void fix_chp(WORD *pcoord, WORD flag)
{
	WORD ncoord;

	ncoord = *pcoord & 0xff;

	if (!flag && ncoord == 0x50)
		ncoord = xdesk.g_w;				/* xdesk.w = Breite des Bildschirms in Pixel */
	else
		ncoord *= (flag ? xgl_hbox : xgl_wbox);	/* xgl_wbox, xgl_hbox = Zeichenbreite, Zeichenh�he in Pixel */

	if (((*pcoord >> 8) & 0xff) > 0x80)
		ncoord += (((*pcoord >> 8) & 0xff) | 0xff00);
	else
		ncoord += ((*pcoord >> 8) & 0xff);

	*pcoord = ncoord;
}

/*****************************************************************************/

static void rs_obfix(OBJECT *rs_otree, WORD rs_oobject)
{
	WORD *coord;
	WORD tmp = FALSE;
	WORD count = 0;

	coord = &rs_otree[rs_oobject].ob_x;

	while (count++ < 4)
	{
		fix_chp(coord++, tmp);
		tmp = tmp ? FALSE : TRUE;
	}
}

/*****************************************************************************/

static void rs_sglobal(WORD *base)
{
	rs_global = (XRSRC_GLOBAL *)base;
	hdr_buf = rs_global->ap_rsc;
	rs_hdr = &hdr_buf[1];
}

/*****************************************************************************/

static WORD rs_free(WORD *base)
{
#if COLOR_ICONS == TRUE
	WORD i;
	CICON *color_icn;
#endif

	rs_global = (XRSRC_GLOBAL *)base;

#if COLOR_ICONS == TRUE

	if ((color_icn = rs_global->coloricons) != NULL)
	{
#if SAVE_MEMORY == TRUE
		RSXHDR *rsxhdr;
		void *rs_start;
		void *rs_end;

		rsxhdr = rs_global->ap_rsc;
		rs_start = rsxhdr + 1;
		rs_end = (void *) (*(LONG *) (rsxhdr->rsh_rssize + (rsxhdr->rsh_rssize & 1L) + (LONG) rs_start) + (LONG) rs_start - 1L);

		for (i = 0; i < rs_global->num_coloricons; i++)
		{
			if (color_icn[i].num_planes > 1)
			{
				if (color_icn[i].col_data != NULL
					&& (color_icn[i].col_data < rs_start || color_icn[i].col_data > rs_end))
					free(color_icn[i].col_data);
				if (color_icn[i].sel_data != NULL
					&& (color_icn[i].sel_data < rs_start || color_icn[i].sel_data > rs_end))
					free(color_icn[i].sel_data);
				if (color_icn[i].sel_data == NULL && color_icn[i].sel_mask != NULL
					&& (color_icn[i].sel_mask < rs_start || color_icn[i].sel_mask > rs_end))
					free(color_icn[i].sel_mask);
			}
		}
#else
		for (i = 0; i < rs_global->num_coloricons; i++)
		{
			if (color_icn[i].num_planes > 1)
			{
				if (color_icn[i].col_data != NULL)
					free(color_icn[i].col_data);
				if (color_icn[i].sel_data != NULL)
					free(color_icn[i].sel_data);
				if (color_icn[i].sel_data == NULL && color_icn[i].sel_mask != NULL)
					free(color_icn[i].sel_mask);
			}
		}
#endif
		free(color_icn);
	}
	if (rs_global->userblks)
		free(rs_global->userblks);
#endif

	free(rs_global->ap_rsc);

	return TRUE;
}

/*****************************************************************************/

static void *get_sub(WORD index, LONG offset, WORD size)
{
	UBYTE *ptr = (UBYTE *) rs_hdr;

	ptr += offset;
	ptr += ((LONG) index * (LONG) size);

	return (void *) ptr;
}

/*****************************************************************************/

static void *get_address(WORD type, WORD index)
{
	void *the_addr = NULL;
	union
	{
		void *dummy;
		BYTE *string;
		OBJECT **dpobject;
		OBJECT *object;
		TEDINFO *tedinfo;
		ICONBLK *iconblk;
		BITBLK *bitblk;
	} all_ptr;

	switch (type)
	{
	case R_TREE:
		all_ptr.dpobject = rs_global->ap_ptree;
		the_addr = all_ptr.dpobject[index];
		break;

	case R_OBJECT:
		the_addr = get_sub(index, hdr_buf->rsh_object, sizeof(OBJECT));
		break;

	case R_TEDINFO:
	case R_TEPTEXT:
		the_addr = get_sub(index, hdr_buf->rsh_tedinfo, sizeof(TEDINFO));
		break;

	case R_ICONBLK:
	case R_IBPMASK:
		the_addr = get_sub(index, hdr_buf->rsh_iconblk, sizeof(ICONBLK));
		break;

	case R_BITBLK:
	case R_BIPDATA:
		the_addr = get_sub(index, hdr_buf->rsh_bitblk, sizeof(BITBLK));
		break;

	case R_OBSPEC:
		all_ptr.object = get_address(R_OBJECT, index);
		the_addr = &all_ptr.object->ob_spec;
		break;

	case R_TEPVALID:
	case R_TEPTMPLT:
		all_ptr.tedinfo = get_address(R_TEDINFO, index);
		if (type == R_TEPVALID)
			the_addr = &all_ptr.tedinfo->te_pvalid;
		else
			the_addr = &all_ptr.tedinfo->te_ptmplt;
		break;

	case R_IBPDATA:
	case R_IBPTEXT:
		all_ptr.iconblk = get_address(R_ICONBLK, index);
		if (type == R_IBPDATA)
			the_addr = &all_ptr.iconblk->ib_pdata;
		else
			the_addr = &all_ptr.iconblk->ib_ptext;
		break;

	case R_STRING:
		the_addr = get_sub(index, hdr_buf->rsh_frstr, sizeof(BYTE *));
		the_addr = *((void **)the_addr);
		break;

	case R_IMAGEDATA:
		the_addr = get_sub(index, hdr_buf->rsh_frimg, sizeof(BYTE *));
		the_addr = *((void **)the_addr);
		break;

	case R_FRIMG:
		the_addr = get_sub(index, hdr_buf->rsh_frimg, sizeof(BYTE *));
		break;

	case R_FRSTR:
		the_addr = get_sub(index, hdr_buf->rsh_frstr, sizeof(BYTE *));
		break;
	}

	return the_addr;
}

/*****************************************************************************/

static WORD rs_gaddr(WORD *base, WORD re_gtype, WORD re_gindex, OBJECT ** re_gaddr)
{
	rs_sglobal(base);

	*re_gaddr = get_address(re_gtype, re_gindex);

	if (*re_gaddr == (OBJECT *) NULL)
		return FALSE;

	return TRUE;
}

/*****************************************************************************/

static WORD rs_sadd(WORD *base, WORD rs_stype, WORD rs_sindex, OBJECT *re_saddr)
{
	OBJECT *old_addr;

	rs_sglobal(base);

	old_addr = get_address(rs_stype, rs_sindex);

	if (old_addr == (OBJECT *) NULL)
		return FALSE;

	*old_addr = *re_saddr;

	return TRUE;
}

/*****************************************************************************/

static WORD fix_long(LONG *lptr)
{
	LONG base;

	base = *lptr;
	if (base == 0L)
		return FALSE;

	base += (LONG) rs_hdr;

	*lptr = base;

	return TRUE;
}

/*****************************************************************************/

static void fix_nptr(LONG index, WORD ob_type)
{
	while (index >= 0)
		fix_long(get_address(ob_type, index--));
}

/*****************************************************************************/

static WORD fix_ptr(WORD type, LONG index)
{
	return fix_long(get_address(type, index));
}

/*****************************************************************************/

static void fix_treeindex(void)
{
	OBJECT **adr;
	LONG count;

	count = hdr_buf->rsh_ntree - 1;

	adr = get_sub(0, hdr_buf->rsh_trindex, sizeof(OBJECT *));

	rs_global->ap_ptree = adr;

	while (count >= 0)
	{
		fix_long((LONG *) (count * sizeof(OBJECT *) + (LONG) adr));
		count--;
	}
}

/*****************************************************************************/

static void fix_tedinfo(void)
{
	LONG count;
	TEDINFO *tedinfo;

	count = hdr_buf->rsh_nted - 1;

	while (count >= 0)
	{
		tedinfo = get_address(R_TEDINFO, count);

		if (fix_ptr(R_TEPTEXT, count))
			tedinfo->te_txtlen = strlen(tedinfo->te_ptext) + 1;

		if (fix_ptr(R_TEPTMPLT, count))
			tedinfo->te_tmplen = strlen(tedinfo->te_ptmplt) + 1;

		fix_ptr(R_TEPVALID, count);

		count--;
	}

	return;
}

/*****************************************************************************/

static void do_rsfix(ULONG size)
{
	rs_global->ap_rsc = hdr_buf;
	rs_global->ap_rsclen = (UWORD) size;

	fix_treeindex();
	fix_tedinfo();

	fix_nptr(hdr_buf->rsh_nib - 1, R_IBPMASK);
	fix_nptr(hdr_buf->rsh_nib - 1, R_IBPDATA);
	fix_nptr(hdr_buf->rsh_nib - 1, R_IBPTEXT);

	fix_nptr(hdr_buf->rsh_nbb - 1, R_BIPDATA);
	fix_nptr(hdr_buf->rsh_nstring - 1, R_FRSTR);
	fix_nptr(hdr_buf->rsh_nimages - 1, R_FRIMG);
}

#if COLOR_ICONS == TRUE

/*****************************************************************************/
/* std_to_byte wandelt eine Grafik im Standardformat direkt ins ger�te-      */
/* abh�ngige Format (in Aufl�sungen mit >= 16 Planes)                        */
/*****************************************************************************/

static void std_to_byte(UWORD *col_data, LONG len, WORD old_planes, ULONG *farbtbl2, MFDB *s)
{
	LONG x;
	LONG i;
	LONG mul[32];
	LONG pos;
	UWORD np;
	UWORD *new_data;
	UWORD pixel;
	UWORD color;
	UWORD back[32];
	WORD memflag = FALSE;
	UBYTE *p1;
	UBYTE *p2;
	ULONG colback;

	if (s->fd_addr == col_data)
	{
		if ((col_data = malloc(len * 2 * s->fd_nplanes)) == NULL)
		{
			form_error(-(-39) - 31);
			return;
		}
		memcpy(col_data, s->fd_addr, len * 2 * s->fd_nplanes);
		memflag = TRUE;
	}
	new_data = (UWORD *) s->fd_addr;
	p1 = (UBYTE *) new_data;

	if (old_planes < 8)
	{
		colback = farbtbl2[(1 << old_planes) - 1];
		farbtbl2[(1 << old_planes) - 1] = farbtbl2[255];
	}

	for (i = 0; i < old_planes; i++)
		mul[i] = i * len;

	pos = 0;

	for (x = 0; x < len; x++)
	{
		for (np = 0; np < old_planes; np++)
			back[np] = col_data[mul[np] + x];

		for (pixel = 0; pixel < 16; pixel++)
		{
			color = 0;
			for (np = 0; np < old_planes; np++)
			{
				color |= ((back[np] & 0x8000) >> (15 - np));
				back[np] <<= 1;
			}

			switch (xpixelbytes)
			{
			case 2:
				new_data[pos++] = *(UWORD *) & farbtbl2[color];
				break;

			case 3:
				p2 = (UBYTE *) & farbtbl2[color];
				*(p1++) = *(p2++);
				*(p1++) = *(p2++);
				*(p1++) = *(p2++);
				break;

			case 4:
				((ULONG *) new_data)[pos++] = farbtbl2[color];
				break;
			}
		}
	}

	if (old_planes < 8)
		farbtbl2[(1 << old_planes) - 1] = colback;

	if (memflag)
		free(col_data);
}

/*****************************************************************************/
/* Icon an aktuelle Grafikaufl�sung anpassen                                 */
/* (z.B. 4 Plane Icon an 24 Plane TrueColor)                                 */
/*****************************************************************************/

static void xfix_cicon(UWORD *col_data, LONG len, WORD old_planes, WORD new_planes, MFDB *s)
{
	LONG x;
	LONG i;
	LONG old_len;
	LONG rest_len;
	LONG mul[32];
	LONG pos;
	UWORD np;
	UWORD *new_data;
	UWORD mask;
	UWORD pixel;
	UWORD bit;
	UWORD color;
	UWORD back[32];
	UWORD old_col[32];
	UWORD maxcol;
	WORD got_mem = FALSE;
	MFDB d;

	len >>= 1;

	if (old_planes == new_planes)
	{
		if (s != NULL)
		{
			if (new_planes == xscrn_planes)
			{
				d = *s;
				d.fd_stand = FALSE;
				s->fd_addr = col_data;
				if (d.fd_addr == s->fd_addr)
				{
					if ((d.fd_addr = malloc(len * 2 * new_planes)) == NULL)
						d.fd_addr = s->fd_addr;
					else
						got_mem = TRUE;
				}

				vr_trnfm(xvdi_handle, s, &d);
				if (d.fd_addr != s->fd_addr && got_mem == TRUE)
				{
					memcpy(s->fd_addr, d.fd_addr, len * 2 * new_planes);
					free(d.fd_addr);
				}
			} else
			{
				memcpy(s->fd_addr, col_data, len * 2 * new_planes);
			}
		}
		return;
	}

	old_len = old_planes * len;
	rest_len = new_planes * len - old_len;

	if (new_planes <= 8)
	{
		if (s != NULL)
		{
			new_data = &((UWORD *) s->fd_addr)[old_len];
			memset(new_data, 0, rest_len * 2);
			memcpy(s->fd_addr, col_data, old_len * 2);
			col_data = s->fd_addr;
		} else
		{
			new_data = (UWORD *) & col_data[old_len];
		}

		for (x = 0; x < len; x++)
		{
			mask = 0xffff;

			for (i = 0; i < old_len; i += len)
				mask &= (UWORD) col_data[x + i];

			if (mask)
				for (i = 0; i < rest_len; i += len)
					new_data[x + i] |= mask;
		}

		if (s != NULL)					/* ins ger�teabh�ngige Format konvertieren */
		{
			d = *s;
			d.fd_stand = 0;
			if ((d.fd_addr = malloc(len * 2 * new_planes)) == NULL)
				d.fd_addr = s->fd_addr;

			vr_trnfm(xvdi_handle, s, &d);
			if (d.fd_addr != s->fd_addr)
			{
				memcpy(s->fd_addr, d.fd_addr, len * 2 * new_planes);
				free(d.fd_addr);
			}
		}
	} else								/* TrueColor, bzw RGB-orientierte Pixelwerte */
	{
		if (!xpixelbytes || s == NULL)
		{
			for (i = 0; i < new_planes; i++)
				mul[i] = i * len;

			maxcol = 0;
			if (old_planes < 8)
			{
				maxcol = (1 << old_planes) - 1;
				memcpy(old_col, farbtbl[maxcol], new_planes * sizeof(WORD));
				memset(farbtbl[maxcol], 0, new_planes * sizeof(WORD));
			}

			if (s != NULL)
			{
				new_data = &((UWORD *) s->fd_addr)[old_len];
				memset(new_data, 0, rest_len * 2);
				memcpy(s->fd_addr, col_data, old_len * 2);
				col_data = s->fd_addr;
			}

			for (x = 0; x < len; x++)
			{
				bit = 1;
				for (np = 0; np < old_planes; np++)
					back[np] = col_data[mul[np] + x];

				for (pixel = 0; pixel < 16; pixel++)
				{
					color = 0;
					for (np = 0; np < old_planes; np++)
					{
						color += ((back[np] & 1) << np);
						back[np] >>= 1;
					}

					for (np = 0; np < new_planes; np++)
					{
						pos = mul[np] + x;
						col_data[pos] = (col_data[pos] & ~bit) | (farbtbl[color][np] & bit);
					}

					bit <<= 1;
				}
			}
			if (old_planes < 8)
				memcpy(farbtbl[maxcol], old_col, new_planes * sizeof(WORD));

			if (s != NULL)				/* ins ger�teabh�ngige Format konvertieren */
			{
				d = *s;
				d.fd_stand = 0;
				if ((d.fd_addr = malloc(len * 2 * new_planes)) == NULL)
					d.fd_addr = s->fd_addr;

				vr_trnfm(xvdi_handle, s, &d);
				if (d.fd_addr != s->fd_addr)
				{
					memcpy(s->fd_addr, d.fd_addr, len * 2 * new_planes);
					free(d.fd_addr);
				}
			}
		} else
		{
			std_to_byte(col_data, len, old_planes, farbtbl2, s);
		}
	}
}

/*****************************************************************************/

static void xrect2array(const GRECT *rect, WORD *array)
{
	*array++ = rect->g_x;
	*array++ = rect->g_y;
	*array++ = rect->g_x + rect->g_w - 1;
	*array = rect->g_y + rect->g_h - 1;
}

/*****************************************************************************/
/* Zeichnet Farb-Icon                                                        */
/*****************************************************************************/

static void draw_bitblk(WORD *p, WORD x, WORD y, WORD w, WORD h, WORD num_planes, WORD mode, WORD *index)
{
	WORD pxy[8];
	MFDB s;
	MFDB d;

	d.fd_addr = NULL;					/* screen */
	s.fd_addr = (void *) p;
	s.fd_w = w;
	s.fd_h = h;
	s.fd_wdwidth = w >> 4;
	s.fd_stand = FALSE;
	s.fd_nplanes = num_planes;

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = s.fd_w - 1;
	pxy[3] = s.fd_h - 1;

	pxy[4] = x;
	pxy[5] = y;
	pxy[6] = pxy[4] + pxy[2];
	pxy[7] = pxy[5] + pxy[3];

	if (num_planes > 1)
		vro_cpyfm(xvdi_handle, mode, pxy, &s, &d);
	else
		vrt_cpyfm(xvdi_handle, mode, pxy, &s, &d, index);	/* copy it */
}

/*****************************************************************************/

static WORD cdecl xdraw_cicon(PARMBLK *pb)
{
	WORD ob_x;
	WORD ob_y;
	WORD x;
	WORD y;
	WORD dummy;
	WORD pxy[4];
	WORD m_mode;
	WORD i_mode;
	WORD mskcol;
	WORD icncol;
	LONG ob_spec;
	ICONBLK *iconblk;
	CICON *cicn;
	WORD *mask;
	WORD *data;
	WORD *dark = NULL;
	char letter[2];
	WORD selected;
	WORD mindex[2];
	WORD iindex[2];
	WORD buf;
	WORD xy[4];
	WORD invert = FALSE;

	selected = pb->pb_currstate & OS_SELECTED;

	xrect2array((GRECT *) & pb->pb_xc, xy);
	vs_clip(xvdi_handle, TRUE, xy);		/* Setze Rechteckausschnitt */

	ob_spec = pb->pb_parm;
	ob_x = pb->pb_x;
	ob_y = pb->pb_y;

	iconblk = (ICONBLK *) ob_spec;
	cicn = ((CICONBLK *) ob_spec)->mainlist;
	m_mode = MD_TRANS;

	if (selected)						/* it was an objc_change */
	{
		if (cicn->sel_data != NULL)
		{
			mask = cicn->sel_mask;
			data = cicn->sel_data;
			if (cicn->num_planes > 1)
			{
				if (cicn->num_planes > 8)	/* TrueColor, bzw RGB-orientierte Grafikkarte? */
					i_mode = S_AND_D;
				else
					i_mode = S_OR_D;
			} else
				i_mode = MD_TRANS;
		} else
		{
			mask = cicn->col_mask;
			data = cicn->col_data;

			if (cicn->num_planes > 1)
			{
				if (cicn->num_planes > 8)
					i_mode = S_AND_D;
				else
					i_mode = S_OR_D;
				dark = cicn->sel_mask;
			} else
				invert = TRUE;
		}
	} else
	{
		mask = cicn->col_mask;
		data = cicn->col_data;

		if (cicn->num_planes > 1)
		{
			if (cicn->num_planes > 8)
				i_mode = S_AND_D;
			else
				i_mode = S_OR_D;
		} else
			i_mode = MD_TRANS;
	}

	mindex[0] = ((iconblk->ib_char & 0x0f00) != 0x0100) ? (iconblk->ib_char & 0x0f00) >> 8 : G_WHITE;
	mindex[1] = G_WHITE;

	icncol = iindex[0] = (WORD) (((UWORD) iconblk->ib_char & 0xf000U) >> 12U);
	iindex[1] = G_WHITE;

	mskcol = (iconblk->ib_char & 0x0f00) >> 8;

	x = ob_x + iconblk->ib_xicon;
	y = ob_y + iconblk->ib_yicon;

	if (invert)
	{
		buf = iindex[0];
		iindex[0] = mindex[0];
		mindex[0] = buf;
		i_mode = MD_TRANS;
	}
	if (selected)
	{
		buf = icncol;
		icncol = mskcol;
		mskcol = buf;
	}

	draw_bitblk(mask, x, y, iconblk->ib_wicon, iconblk->ib_hicon, 1, m_mode, mindex);
	draw_bitblk(data, x, y, iconblk->ib_wicon, iconblk->ib_hicon, cicn->num_planes, i_mode, iindex);

	if (dark)
	{
		mindex[0] = G_BLACK;
		mindex[1] = G_WHITE;
		draw_bitblk(dark, x, y, iconblk->ib_wicon, iconblk->ib_hicon, 1, MD_TRANS, mindex);
	}

	if (iconblk->ib_ptext[0])
	{
		x = ob_x + iconblk->ib_xtext;
		y = ob_y + iconblk->ib_ytext;

		pxy[0] = x;
		pxy[1] = y;
		pxy[2] = x + iconblk->ib_wtext - 1;
		pxy[3] = y + iconblk->ib_htext - 1;

		vswr_mode(xvdi_handle, MD_REPLACE);	/* Textbox zeichnen */
		vsf_color(xvdi_handle, mskcol);
		vsf_interior(xvdi_handle, FIS_SOLID);
		vsf_perimeter(xvdi_handle, FALSE);
		v_bar(xvdi_handle, pxy);
	}

	vswr_mode(xvdi_handle, MD_TRANS);
	vst_font(xvdi_handle, 1);			/* Systemfont */
	vst_height(xvdi_handle, 4, &dummy, &dummy, &dummy, &dummy);
	vst_color(xvdi_handle, icncol);
	vst_effects(xvdi_handle, 0 /*TXT_NORMAL */ );
	vst_alignment(xvdi_handle, 0 /*ALI_LEFT */ , 5 /*ALI_TOP */ , &dummy, &dummy);
	vst_rotation(xvdi_handle, 0);

	if (iconblk->ib_ptext[0])
	{
		x += (iconblk->ib_wtext - strlen(iconblk->ib_ptext) * 6) / 2;
		y += (iconblk->ib_htext - 6) / 2;

		v_gtext(xvdi_handle, x, y, iconblk->ib_ptext);
	}

	letter[0] = iconblk->ib_char & 0xff;
	if (letter[0])
	{
		letter[1] = '\0';
		x = ob_x + iconblk->ib_xicon + iconblk->ib_xchar;
		y = ob_y + iconblk->ib_yicon + iconblk->ib_ychar;

		v_gtext(xvdi_handle, x, y, letter);
	}

	vs_clip(xvdi_handle, FALSE, xy);

	return pb->pb_currstate & ~OS_SELECTED;
}										/* draw_userdef */

/*****************************************************************************/
/* Icon ins ger�teabh�ngige Format wandeln und ggf an andere Aufl�sungen     */
/* anpassen                                                                  */
/*****************************************************************************/

static WORD xadd_cicon(CICONBLK *cicnblk, OBJECT *obj, WORD nub)
{
	WORD x;
	WORD y;
	WORD line;
	WORD xmax;
	WORD best_planes;
	WORD find_planes;
	CICON *cicn;
	CICON *color_icn;
	CICON *best_icn = NULL;
	LONG len;
	CICON **next;
	MFDB d;
	OBBLK *ub;

#if SAVE_MEMORY == TRUE
	CICON *max_icn = NULL;
#endif

	len = cicnblk->monoblk.ib_wicon / 8 * cicnblk->monoblk.ib_hicon;

	color_icn = &rs_global->coloricons[nub];

	best_planes = 1;
	if (xscrn_planes > 8)
		find_planes = 4;
	else
		find_planes = xscrn_planes;

	cicn = cicnblk->mainlist;
	next = &cicnblk->mainlist;

	while (cicn != NULL)
	{
		*next = cicn;
		next = &cicn->next_res;

#if SAVE_MEMORY == TRUE
		if (cicn->num_planes > xscrn_planes)
			max_icn = cicn;
#endif
		if (cicn->num_planes >= best_planes && cicn->num_planes <= find_planes)
		{
			best_planes = cicn->num_planes;
			best_icn = cicn;
		}
		cicn = cicn->next_res;
	}

	if (best_icn == NULL)				/* kein passendes Farbicon gefunden */
		return FALSE;
	else
		*color_icn = *best_icn;

	if (best_planes > 1)
		color_icn->num_planes = xscrn_planes;
	else
		color_icn->num_planes = 1;

	/* Platz f�r das ger�teabh�ngige Format allozieren */
	if ((color_icn->col_data = malloc(len * color_icn->num_planes)) == NULL)
	{
		form_error(-(-39) - 31);
		return FALSE;
	}
	if (color_icn->sel_data)
	{
		if ((color_icn->sel_data = malloc(len * color_icn->num_planes)) == NULL)
		{
			free(color_icn->col_data);
			form_error(-(-39) - 31);
			return FALSE;
		}
	}

	if (best_planes > 1)
	{
		if (best_icn->sel_data == NULL)
		{
			/* Selected-Maske vorbereiten */
			if ((color_icn->sel_mask = malloc(len)) == NULL)
			{
				free(color_icn->col_data);
				if (color_icn->sel_data)
					free(color_icn->sel_data);
				form_error(-(-39) - 31);
				return FALSE;
			}

			xmax = cicnblk->monoblk.ib_wicon / 16;

			for (y = 0; y < cicnblk->monoblk.ib_hicon; y++)
			{
				line = y * xmax;

				for (x = 0; x < xmax; x++)
				{
					if (y & 1)
						color_icn->sel_mask[line + x] = best_icn->col_mask[line + x] & 0xaaaa;
					else
						color_icn->sel_mask[line + x] = best_icn->col_mask[line + x] & 0x5555;
				}
			}
		}

		d.fd_addr = color_icn->col_data;
		d.fd_w = cicnblk->monoblk.ib_wicon;
		d.fd_h = cicnblk->monoblk.ib_hicon;
		d.fd_wdwidth = d.fd_w >> 4;
		d.fd_stand = TRUE;
		d.fd_nplanes = xscrn_planes;

		xfix_cicon((UWORD *) best_icn->col_data, len, best_planes, xscrn_planes, &d);
		if (best_icn->sel_data)
		{
			d.fd_addr = color_icn->sel_data;
			xfix_cicon((UWORD *) best_icn->sel_data, len, best_planes, xscrn_planes, &d);
		}
	} else
	{
		memcpy(color_icn->col_data, best_icn->col_data, len);
		memcpy(color_icn->sel_data, best_icn->sel_data, len);
	}

#if SAVE_MEMORY == TRUE
	if (best_icn->num_planes < color_icn->num_planes && max_icn != NULL)
	{
		if (best_icn->sel_data == NULL || max_icn->sel_data != NULL)
		{
			best_icn->col_data = max_icn->col_data;
			best_icn->sel_data = max_icn->sel_data;
			best_icn->num_planes = xscrn_planes;
		}
	}

	if (best_icn->num_planes == color_icn->num_planes)
	{
		memcpy(best_icn->col_data, color_icn->col_data, len * color_icn->num_planes);
		free(color_icn->col_data);
		color_icn->col_data = NULL;
		if (best_icn->sel_data != NULL)
		{
			memcpy(best_icn->sel_data, color_icn->sel_data, len * color_icn->num_planes);
			free(color_icn->sel_data);
			color_icn->sel_data = NULL;
		} else
		{
			memcpy(cicnblk->monoblk.ib_pmask, color_icn->sel_mask, len);
			best_icn->sel_mask = cicnblk->monoblk.ib_pmask;
			free(color_icn->sel_mask);
			color_icn->sel_mask = NULL;
		}
	} else
	{
		if (best_icn->sel_data == NULL)
		{
			memcpy(cicnblk->monoblk.ib_pmask, color_icn->sel_mask, len);
			free(color_icn->sel_mask);
			color_icn->sel_mask = NULL;
			best_icn->col_data = color_icn->col_data;
			best_icn->sel_mask = cicnblk->monoblk.ib_pmask;
			best_icn->num_planes = color_icn->num_planes;
		} else
			*best_icn = *color_icn;
	}
	cicnblk->mainlist = best_icn;
#else
	color_icn->next_res = cicnblk->mainlist;
	cicnblk->mainlist = color_icn;
#endif
	ub = rs_global->userblks;
	ub[nub].old_type = G_CICON;
	ub[nub].ublk.ub_parm = obj->ob_spec.index;
	ub[nub].ublk.ub_code = xdraw_cicon;
	obj->ob_spec.userblk = &ub[nub].ublk;
	obj->ob_type = (obj->ob_type & 0xff00) | G_USERDEF;

	return TRUE;
}

/*****************************************************************************/
/* Testen wieviel Bytes pro Pixel im ger�teabh�ngigen Format verwendet werden*/
/*****************************************************************************/

static WORD test_rez(void)
{
	WORD i;
	WORD np;
	WORD color;
	WORD pxy[8];
	WORD rgb[3];
	WORD bpp = 0;
	UWORD backup[32];
	UWORD test[32];
	WORD black[3] = { 0, 0, 0 };
	WORD white[3] = { 1000, 1000, 1000 };
	MFDB screen;
	MFDB pixel = { NULL, 16, 1, 1, 0, 1, 0, 0, 0 };
	MFDB stdfm = { NULL, 16, 1, 1, 1, 1, 0, 0, 0 };

	if (xscrn_planes >= 8)
	{
		stdfm.fd_nplanes = pixel.fd_nplanes = xscrn_planes;

		if (xscrn_planes == 8)
		{
			color = 0xff;
			memset(test, 0, xscrn_planes * sizeof(WORD));
			for (np = 0; np < xscrn_planes; np++)
				test[np] = (color & (1 << np)) << (15 - np);

			pixel.fd_addr = stdfm.fd_addr = test;
			vr_trnfm(xvdi_handle, &stdfm, &pixel);

			for (i = 1; i < xscrn_planes; i++)
				if (test[i])
					break;

			if (i >= xscrn_planes && !(test[0] & 0x00ff))
				bpp = 1;
		} else
		{
			xrect2array(&xdesk, pxy);
			vs_clip(xvdi_handle, FALSE, pxy);
			screen.fd_addr = NULL;

			memset(backup, 0, sizeof(backup));

			vswr_mode(xvdi_handle, MD_REPLACE);
			vsl_ends(xvdi_handle, 0, 0);
			vsl_type(xvdi_handle, 1);
			vsl_width(xvdi_handle, 1);
			memset(pxy, 0, sizeof(pxy));

			graf_mouse(M_OFF, NULL);

			pixel.fd_addr = backup;		/* Punkt retten */
			vro_cpyfm(xvdi_handle, S_ONLY, pxy, &screen, &pixel);

			/* Alte Farbe retten */
			vq_color(xvdi_handle, 15, 1, rgb);

			/* Ger�teabh�ngiges Format testen */
			pixel.fd_addr = test;
			vsl_color(xvdi_handle, 15);
			vs_color(xvdi_handle, 15, white);
			v_pline(xvdi_handle, 2, pxy);

			memset(test, 0, xscrn_planes * sizeof(WORD));
			vro_cpyfm(xvdi_handle, S_ONLY, pxy, &screen, &pixel);

			for (i = (xscrn_planes + 15) / 16 * 2; i < xscrn_planes; i++)
				if (test[i])
					break;

			if (i >= xscrn_planes)
			{
				vs_color(xvdi_handle, 15, black);
				v_pline(xvdi_handle, 2, pxy);

				memset(test, 0, xscrn_planes * sizeof(WORD));
				vro_cpyfm(xvdi_handle, S_ONLY, pxy, &screen, &pixel);

				for (i = (xscrn_planes + 15) / 16 * 2; i < xscrn_planes; i++)
					if (test[i])
						break;

				if (i >= xscrn_planes)
					bpp = (xscrn_planes + 7) / 8;
			}

			/* Alte Farbe restaurieren */
			vs_color(xvdi_handle, 15, rgb);

			pixel.fd_addr = backup;		/* Punkt restaurieren */
			vro_cpyfm(xvdi_handle, S_ONLY, pxy, &pixel, &screen);

			graf_mouse(M_ON, NULL);
		}
	}

	return bpp;
}

/*****************************************************************************/
/* Unter TrueColor Pixelwerte der RGB-Palette ermitteln                      */
/*****************************************************************************/

static void xfill_farbtbl(void)
{
	WORD np;
	WORD color;
	WORD pxy[8];
	WORD backup[32];
	WORD rgb[3];
	MFDB screen;
	MFDB pixel = { NULL, 16, 1, 1, 0, 1, 0, 0, 0 };
	MFDB stdfm = { NULL, 16, 1, 1, 1, 1, 0, 0, 0 };
	WORD pixtbl[16] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 16 };

	if (xscrn_planes >= 8)
	{
		if (xscrn_planes > 8)
		{
			if (is_palette == FALSE)	/* Keine Palette in der Resource */
			{
				for (color = 0; color < 255; color++)
				{
					if (color < 16)
					{
						vq_color(xvdi_handle, pixtbl[color], 1, rgb_palette[color]);
						rgb_palette[color][3] = pixtbl[color];
					} else
					{
						vq_color(xvdi_handle, color + 1, 1, rgb_palette[color]);
						rgb_palette[color][3] = color + 1;
					}
				}
				vq_color(xvdi_handle, 1, 1, rgb_palette[255]);
				rgb_palette[255][3] = 1;
				is_palette = TRUE;
			}

			vs_clip(xvdi_handle, FALSE, pxy);
			graf_mouse(M_OFF, NULL);

			memset(backup, 0, sizeof(backup));
			memset(farbtbl, 0, 32 * 256 * sizeof(WORD));
			screen.fd_addr = NULL;
			stdfm.fd_nplanes = pixel.fd_nplanes = xscrn_planes;

			vswr_mode(xvdi_handle, MD_REPLACE);
			vsl_ends(xvdi_handle, 0, 0);
			vsl_type(xvdi_handle, 1);
			vsl_width(xvdi_handle, 1);
			memset(pxy, 0, sizeof(pxy));

			pixel.fd_addr = backup;		/* Punkt retten */
			vro_cpyfm(xvdi_handle, S_ONLY, pxy, &screen, &pixel);

			/* Alte Farbe retten */
			vq_color(xvdi_handle, 15, 1, rgb);

			for (color = 0; color < 256; color++)
			{
				vs_color(xvdi_handle, 15, rgb_palette[color]);
				vsl_color(xvdi_handle, 15);
				v_pline(xvdi_handle, 2, pxy);

				stdfm.fd_addr = pixel.fd_addr = farbtbl[color];

				/* vro_cpyfm, weil v_get_pixel nicht mit TrueColor (>=24 Planes) funktioniert */
				vro_cpyfm(xvdi_handle, S_ONLY, pxy, &screen, &pixel);

				if (farbtbl2 != NULL && xpixelbytes)
				{
					farbtbl2[color] = 0L;
					memcpy(&farbtbl2[color], pixel.fd_addr, xpixelbytes);
				}

				vr_trnfm(xvdi_handle, &pixel, &stdfm);
				for (np = 0; np < xscrn_planes; np++)
					if (farbtbl[color][np])
						farbtbl[color][np] = 0xffff;
			}

			/* Alte Farbe restaurieren */
			vs_color(xvdi_handle, 15, rgb);

			pixel.fd_addr = backup;		/* Punkt restaurieren */
			vro_cpyfm(xvdi_handle, S_ONLY, pxy, &pixel, &screen);

			graf_mouse(M_ON, NULL);
		} else
		{
			if (farbtbl2 != NULL && xpixelbytes)
				for (color = 0; color < 256; color++)
					*(UBYTE *) & farbtbl2[color] = color;
		}
	}
}

#endif /* COLOR_ICONS */

/*****************************************************************************/
/* Pointer f�r Farbicons in der Resource initialisieren                      */
/*****************************************************************************/

static WORD fill_cicon_liste(LONG *cicon_liste, ULONG header, RSXHDR *rsxhdr)
{
	WORD i;
	WORD i2;
	WORD num = 0;
	BYTE *p;
	LONG iclen;
	LONG num_cicon;
	LONG ob;
	LONG p2;
	CICONBLK *cblk;
	CICON *cicon;
	CICON *cold;
	OBJECT *pobject;

	while (!cicon_liste[num])
		num++;

	if (cicon_liste[num] != -1L)
		return -1;

	cblk = (CICONBLK *) & cicon_liste[num + 1];

	for (i = 0; i < num; i++)
	{
		cicon_liste[i] = (LONG) cblk;
		p = (BYTE *) & cblk[1];
		cblk->monoblk.ib_pdata = (WORD *) p;
		iclen = cblk->monoblk.ib_wicon / 8 * cblk->monoblk.ib_hicon;
		p += iclen;
		cblk->monoblk.ib_pmask = (WORD *) p;
		p += iclen;
		p2 = (LONG) cblk->monoblk.ib_ptext;
		if (!p2 || header + p2 == (ULONG) p || p2 < (LONG)rsxhdr->rsh_string || p2 > (LONG)rsxhdr->rsh_rssize)
			cblk->monoblk.ib_ptext = (char *) p;
		else
			cblk->monoblk.ib_ptext = (char *)header + (LONG) cblk->monoblk.ib_ptext;

		cicon = (CICON *) & p[12];
		p += 12L;
		cold = cicon;
		if ((num_cicon = (LONG) cblk->mainlist) > 0)
		{
			cblk->mainlist = cicon;

			for (i2 = 0; i2 < num_cicon; i2++)
			{
				p = (BYTE *) & cicon[1];
				cicon->col_data = (WORD *) p;
				p += iclen * cicon->num_planes;
				cicon->col_mask = (WORD *) p;
				p += iclen;
				if (cicon->sel_data != NULL)
				{
					cicon->sel_data = (WORD *) p;
					p += iclen * cicon->num_planes;
					cicon->sel_mask = (WORD *) p;
					p += iclen;
				}
				cicon->next_res = (CICON *) p;
				cold = cicon;
				cicon = (CICON *) p;
			}
			cold->next_res = NULL;
		}
		cblk = (CICONBLK *) p;
	}

	if (num != -1)
	{
		pobject = (OBJECT *) (header + rsxhdr->rsh_object);

		for (ob = 0; ob < (LONG)rsxhdr->rsh_nobs; ob++)
			if ((pobject[ob].ob_type & 0xff) == G_CICON)
			{
				pobject[ob].ob_spec.index = cicon_liste[pobject[ob].ob_spec.index];
			}

		if ((rs_global->userblks = malloc(num * sizeof(OBBLK))) != NULL)
			memset(rs_global->userblks, 0, num * sizeof(OBBLK));
		if ((rs_global->coloricons = malloc(num * sizeof(CICON))) != NULL)
			memset(rs_global->coloricons, 0, num * sizeof(CICON));
		rs_global->num_coloricons = num;
	}

	return num;
}

/*****************************************************************************/
/* Farbicons f�r aktuelle Aufl�sung initialisieren                           */
/*****************************************************************************/

static void do_ciconfix(LONG header, RSXHDR *rsxhdr, LONG rs_len)
{
	LONG *cicon_liste;
	WORD i;
	OBJECT *obj;

	cicon_liste = (LONG *) (*(LONG *) (rsxhdr->rsh_rssize + (rsxhdr->rsh_rssize & 1L) + header + sizeof(LONG)) + header);
	if ((LONG) cicon_liste - header > (LONG)rsxhdr->rsh_rssize && (LONG) cicon_liste - header < rs_len)
	{
		if (fill_cicon_liste(cicon_liste, header, rsxhdr) != -1)
		{
#if COLOR_ICONS == TRUE
			WORD nub = 0;
			WORD work_out[57];
			WORD *palette;

			if (rs_global->userblks && rs_global->coloricons)
			{
				vq_extnd(xvdi_handle, TRUE, work_out);	/* Anzahl der Planes ermitteln */
				xscrn_planes = work_out[4];

				xpixelbytes = test_rez();
				palette = (WORD *) * (LONG *) (rsxhdr->rsh_rssize + (rsxhdr->rsh_rssize & 1L) + header + 2 * sizeof(LONG));
				if (palette != NULL)
				{
					palette = (WORD *)((char *)palette + header);
					memcpy(rgb_palette, palette, sizeof(rgb_palette));
					is_palette = TRUE;
				} else
				{
					is_palette = FALSE;
				}

				xfill_farbtbl();

				for (i = 0; i < (WORD)rsxhdr->rsh_nobs; i++)
				{
					obj = &((OBJECT *) (rsxhdr->rsh_object + header))[i];
					if ((obj->ob_type & 0xff) == G_CICON)
					{
						if (xadd_cicon(obj->ob_spec.ciconblk, obj, nub++) == FALSE)
						{
							memset(&rs_global->coloricons[nub - 1], 0, sizeof(CICON));
							obj->ob_type = (obj->ob_type & 0xff00) | G_ICON;
						}
						obj->ob_spec.index -= header; /* will be added back in fix_object */
					}
				}
			} else
#endif
			{
				for (i = 0; i < (WORD)rsxhdr->rsh_nobs; i++)
				{
					obj = &((OBJECT *) (rsxhdr->rsh_object + header))[i];
					if ((obj->ob_type & 0xff) == G_CICON)
					{
						obj->ob_type = (obj->ob_type & 0xff00) | G_ICON;
						obj->ob_spec.index -= header; /* will be added back in fix_object */
					}
				}
			}
		}
	}
}

/*****************************************************************************/

static WORD rs_read(WORD *global, const char *fname)
{
	WORD i;
	WORD fh;
	char tmpnam[128];
	DTA dta;
	DTA *old_dta;
	LONG size;
	WORD ret = TRUE;

	strcpy(tmpnam, fname);

	if (!shel_find(tmpnam))
	{
		if (xrsrc_mustexist)
			form_error(-(-33) - 31);
		return FALSE;
	}

	rs_global = (XRSRC_GLOBAL *)global;

	old_dta = Fgetdta();
	Fsetdta(&dta);
	if (Fsfirst(tmpnam, 0x10) == 0)
		size = dta.d_length;
	else
		size = 0;
	Fsetdta(old_dta);

	if (size > (LONG)sizeof(RSHDR) && (fh = Fopen(tmpnam, 0)) > 0)
	{
		if ((hdr_buf = (RSXHDR *) malloc(size + sizeof(RSXHDR))) != NULL)
		{
			rs_hdr = (RSXHDR *)((char *) hdr_buf + sizeof(RSXHDR));

			if (Fread(fh, size, rs_hdr) == size)
			{
				if ((rs_hdr->rsh_vrsn & 3) == 3)
				{
					memcpy(hdr_buf, rs_hdr, sizeof(RSXHDR));
				} else
				{
					/* convert the RSHDR to RSXHDR */
					for (i = 0; i < (WORD)(sizeof(RSXHDR) / sizeof(LONG)); i++)
						((ULONG *) hdr_buf)[i] = ((UWORD *) rs_hdr)[i];
				}

				do_rsfix(hdr_buf->rsh_rssize);

				if (size > (LONG)hdr_buf->rsh_rssize + 72L)	/* Farbicons in der Resource? */
					do_ciconfix((ULONG) rs_hdr, hdr_buf, size);
			} else
				ret = FALSE;
		} else
		{
			form_error(-(-39) - 31);
			ret = FALSE;
		}

		Fclose(fh);
	} else
	{
		ret = FALSE;
	}

	return ret;
}

/*****************************************************************************/

static void fix_object(void)
{
	WORD count;
	OBJECT *obj;

	count = hdr_buf->rsh_nobs - 1;

	while (count >= 0)
	{
		obj = get_address(R_OBJECT, count);
		rs_obfix(obj, 0);
		if ((obj->ob_type & 0xff) != G_BOX && (obj->ob_type & 0xff) != G_IBOX && (obj->ob_type & 0xff) != G_BOXCHAR)
			fix_long(&obj->ob_spec.index);

		count--;
	}
}

/*****************************************************************************/

static void rs_fixindex(WORD *global)
{
	rs_sglobal(global);

	fix_object();
}

/*****************************************************************************/

WORD init_xrsrc(WORD vdi_handle, GRECT *desk, WORD gl_wbox, WORD gl_hbox)
{
	xvdi_handle = vdi_handle;
	xdesk = *desk;
	xgl_wbox = gl_wbox;
	xgl_hbox = gl_hbox;

	return TRUE;
}

/*****************************************************************************/

void term_xrsrc(void)
{
}

/*****************************************************************************/

WORD xrsrc_load(const char *fname, WORD *global)
{
	if (!rs_read(global, fname))
		 return FALSE;

	rs_fixindex(global);

	return TRUE;
}

/*****************************************************************************/

WORD xrsrc_free(WORD *pglobal)
{
	return rs_free(pglobal);
}

/*****************************************************************************/

WORD xrsrc_gaddr(WORD re_gtype, WORD re_gindex, void *re_gaddr, WORD *pglobal)
{
	return rs_gaddr(pglobal, re_gtype, re_gindex, re_gaddr);
}

/*****************************************************************************/

WORD xrsrc_saddr(WORD re_stype, WORD re_sindex, void *re_saddr, WORD *pglobal)
{
	return rs_sadd(pglobal, re_stype, re_sindex, re_saddr);
}

/*****************************************************************************/

WORD xrsrc_obfix(OBJECT *re_otree, WORD re_oobject)
{
	rs_obfix(re_otree, re_oobject);

	return TRUE;
}
