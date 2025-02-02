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

/*-------------------------------------------------------------------------	*/
/*	 					Median Cut - Routinen								*/
/*		oder: "Was zum Geier ist eine Heckbert-Quantisierung?"				*/
/*	C-Routinen f�r Histogramm und variance-based Median Cut. Die dazu-		*/
/*	geh�rigen Assemblerroutinen sind im File medicut.s.						*/
/*																			*/
/*	Wer sich diesen Algorithmus ausgedacht hat, war entweder wahnsinnig 	*/
/*	oder ein absolutes Genie - oder beides.									*/
/*																			*/
/*-------------------------------------------------------------------------	*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "../smurf_f.h"

#include "../smurfobs.h"
#include "../ext_obs.h"

#define ComputeIndex3d(r,g,b)		(unsigned long)( ((uint16_t)r<<10) + ((uint16_t)g<<5) + ((uint16_t)b) )

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))

#define XEDGE	1
#define YEDGE	2
#define ZEDGE	3

long histogram_24bit(long *histogram, char *pdata, long pixlen) ASM_NAME("_histogram_24bit");


#define ASSEMBLER 0


/*------------- Median-Cut - BOX-Struktur ----*/
/* Zur Verwaltung der einzelnen Boxes im RGB-W�rfel */
typedef struct
{
	uint16_t xbox, ybox, zbox; 			/* Position der Box im W�rfel (x=r, y=g, z=b) */
	uint16_t boxwid, boxhgt, boxdepth;	/* Abmessungen der Box (wid=r, hgt=g depth=b) */
} MC_BOX;


static MC_BOX *boxes[SM_PALETTE_MAX];

unsigned long sumplane_xy(long *hist, MC_BOX *box, int16_t zposition) ASM_NAME("_sumplane_xy");
unsigned long sumplane_xz(long *hist, MC_BOX *box, int16_t yposition) ASM_NAME("_sumplane_xz");
unsigned long sumplane_yz(long *hist, MC_BOX *box, int16_t xposition) ASM_NAME("_sumplane_yz");

static unsigned long colnum;

/*--------------------------------------------------------------*/
/*							make_histogram						*/
/*	Ermittelt das Histogramm des Bildes in *picture und gibt es	*/
/*	als   long *3darray[32][32][32]   zur�ck.					*/
/*--------------------------------------------------------------*/
long *make_histogram(SMURF_PIC *picture)
{
	long *histogram;
	char *pdata;
	uint16_t *p16;
	unsigned long t;
	unsigned long pixlen;
	unsigned long x;
	unsigned long xc;
	unsigned long yc;
	uint16_t width, height;
	unsigned long index3d;
	uint8_t red, green, blue;
	uint8_t index8;
	uint8_t *curr_pal;
	uint8_t st_buf[17];
	long planelen;

	histogram = SMalloc(132000L);		/* speicher f�rs Histogramm */

	p16 = (uint16_t *) picture->pic_data;
	pdata = picture->pic_data;
	width = picture->pic_width;
	height = picture->pic_height;

	pixlen = (long) width *(long) height;

	Dialog.busy.reset(0, "Histogramm...");
	colnum = 0;
	t = pixlen;

	if (picture->format_type == FORM_PIXELPAK)
	{
		if (picture->depth == 24)
		{
			colnum = histogram_24bit(histogram, pdata, pixlen);
		} else if (picture->depth == 16)
		{
			while (t--)
			{
				if (!(t & 32767))
					Dialog.busy.draw((short) (20 - (t * 20L / pixlen)));
				red = *(p16) >> 11;
				green = (*(p16) >> 6) & 31;
				blue = *(p16++) & 31;
				index3d = ComputeIndex3d(red, green, blue);
				if (histogram[index3d] == 0)
					colnum++;
				histogram[index3d]++;
			}
		} else if (picture->depth == 8)
		{
			while (t--)
			{
				if (!(t & 32767))
					Dialog.busy.draw((short) (20 - (t * 20L / pixlen)));
				index8 = *(pdata++);
				curr_pal = (picture->palette + index8 + index8 + index8);
				red = *(curr_pal++) >> 3;
				green = *(curr_pal++) >> 3;
				blue = *(curr_pal) >> 3;
				index3d = ComputeIndex3d(red, green, blue);
				if (histogram[index3d] == 0)
					colnum++;
				histogram[index3d]++;
			}
		}
	} else if (picture->format_type == FORM_STANDARD)
	{
		planelen = ((width + 7) / 8) * height;
		x = ((width + 7) / 8);

		for (yc = 0; yc < height; yc++)
		{
			if (!(yc & 63))
				Dialog.busy.draw((short) (yc * 20L / height));
			pdata = (char *) (picture->pic_data) + (yc * x);
			for (xc = 0; xc < x; xc++)
			{
				get_standard_pix(pdata + xc, st_buf, picture->depth, planelen);
				for (t = 0; t < 16; t++)
				{
					index8 = st_buf[t];
					curr_pal = (picture->palette + index8 + index8 + index8);
					red = *(curr_pal++) >> 3;
					green = *(curr_pal++) >> 3;
					blue = *(curr_pal) >> 3;
					index3d = ComputeIndex3d(red, green, blue);
					if (histogram[index3d] == 0)
						colnum++;
					histogram[index3d]++;
				}
			}
		}
	}

	return histogram;
}


/*---------------------------------------------------------------------	*/
/*								sum_plane								*/
/*	Diese Routinen f�hren eine Aufsummierung aller H�ufigkeiten einer	*/
/*	"Scheibe" aus einem W�rfel durch und gibt die Summe (long) zur�ck.	*/
/*---------------------------------------------------------------------	*/
/* X-Y-Plane aufsummieren */
static unsigned long sum_xyplane(int16_t box_to_cut, int16_t zposition, long *histogram)
{
	int16_t x, y;
	int16_t yb, xb;
	int16_t w, h;
	unsigned long index3d;
	unsigned long sum = 0;

	xb = boxes[box_to_cut]->xbox;
	yb = boxes[box_to_cut]->ybox;
	w = boxes[box_to_cut]->boxwid;
	h = boxes[box_to_cut]->boxhgt;

	sum = 0;

	for (y = yb; y < yb + h; y++)
	{
		index3d = ComputeIndex3d(xb, y, zposition);
		x = w;
		while (x--)
		{
			sum += histogram[index3d];
			index3d += 1024;
		}
	}

	return sum;
}

/* X-Z-Plane aufsummieren */
static unsigned long sum_xzplane(int16_t box_to_cut, int16_t yposition, long *histogram)
{
	int16_t x, z;
	int16_t xb, zb;
	int16_t w, d;
	unsigned long index3d;
	unsigned long sum = 0;

	xb = boxes[box_to_cut]->xbox;
	zb = boxes[box_to_cut]->zbox;
	w = boxes[box_to_cut]->boxwid;
	d = boxes[box_to_cut]->boxdepth;

	sum = 0;

	for (z = zb; z < zb + d; z++)
	{
		index3d = ComputeIndex3d(xb, yposition, z);
		x = w;
		while (x--)
		{
			sum += histogram[index3d];
			index3d += 1024;
		}
	}

	return sum;
}


/* Y-Z-Plane aufsummieren */
static unsigned long sum_yzplane(int16_t box_to_cut, int16_t xposition, long *histogram)
{
	int16_t y, z;
	int16_t yb, zb;
	int16_t h, d;
	unsigned long index3d;
	unsigned long sum = 0;

	yb = boxes[box_to_cut]->ybox;
	zb = boxes[box_to_cut]->zbox;
	h = boxes[box_to_cut]->boxhgt;
	d = boxes[box_to_cut]->boxdepth;

	sum = 0;

	for (z = zb; z < zb + d; z++)
	{
		index3d = ComputeIndex3d(xposition, yb, z);
		y = h;
		while (y--)
		{
			sum += histogram[index3d];
			index3d += 32;
		}
	}

	return sum;
}




/*---------------------------------------------------------------------	*/
/*								find_cutpos								*/
/*	findet die optimale Schnittposition f�r den W�rfel Nr. box_to_cut	*/
/*	an der Kante edge.													*/
/*	Die Routine l�uft von den beiden Seiten der zu schneidenden Kante	*/
/*	durch den W�rfel und ermittelt die Schnittstelle so, da� die H�ufig-*/
/*	keiten der Farben in den beiden resultierenden H�lften gleich sind.	*/
/*	Die Schnittposition ABSLOUT ZUM GESAMTW�RFEL wird zur�ckgegeben.	*/
/*---------------------------------------------------------------------	*/
static int16_t find_cutpos(int16_t box_to_cut, int16_t edge, long *histogram)
{
	int16_t left_position, right_position;
	unsigned long left_sum = 0;
	unsigned long all_sum = 0;

#if ASSEMBLER
	unsigned long (*sum_plane)(long *hist, MC_BOX * box, int16_t xposition);
#else
	unsigned long (*sum_plane)(int16_t box_to_cut, int16_t position, long *histo);
#endif
	int16_t all_pos;

	/*---------------- Welche Kante soll geschnitten werden? ---------*/
	if (edge == XEDGE)
	{
		left_position = boxes[box_to_cut]->xbox;
		right_position = boxes[box_to_cut]->xbox + boxes[box_to_cut]->boxwid - 1;
#if ASSEMBLER
		sum_plane = sumplane_yz;
#else
		sum_plane = sum_yzplane;
#endif
	} else if (edge == YEDGE)
	{
		left_position = boxes[box_to_cut]->ybox;
		right_position = boxes[box_to_cut]->ybox + boxes[box_to_cut]->boxhgt - 1;
#if ASSEMBLER
		sum_plane = sumplane_xz;
#else
		sum_plane = sum_xzplane;
#endif
	} else /* if (edge == ZEDGE) */
	{
		left_position = boxes[box_to_cut]->zbox;
		right_position = boxes[box_to_cut]->zbox + boxes[box_to_cut]->boxdepth - 1;
#if ASSEMBLER
		sum_plane = sumplane_xy;
#else
		sum_plane = sum_xyplane;
#endif
	}

	if (left_position == right_position)
		return left_position;
	if (right_position - left_position == 1)
		return left_position + 1;

	/*-------- Gesamtsumme ermitteln ---------*/
	all_pos = left_position;
	do
	{
#if ASSEMBLER
		all_sum += sum_plane(histogram, boxes[box_to_cut], all_pos);
#else
		all_sum += sum_plane(box_to_cut, all_pos, histogram);
#endif
	} while (all_pos++ < right_position);

	all_sum /= 2;


	/*---- von links durch den W�rfel laufen...	-----*/
	do
	{
		/*left_sum += sum_plane(histogram, boxes[box_to_cut], left_position); */
		left_sum += sum_plane(box_to_cut, left_position, histogram);
		left_position++;
	} while (left_sum < all_sum && left_position != right_position);

	return left_position;
}




/*---------------------------------------------------------------------	*/
/*								cut_box									*/
/*	Sucht variance-based die Stelle, an der der W�rfel mit der Nummer	*/
/*	box_to_cut an der Kante edge geschnitten werden kann, schnippelt	*/
/*	den W�rfel auseinander,	erzeugt eine neue MC_BOX-Struktur und pa�t	*/
/*	die alte an.														*/
/*---------------------------------------------------------------------	*/
static void cut_box(int16_t num_of_boxes, int16_t box_to_cut, int16_t edge, long *histogram)
{
	int16_t cut_position;
	int16_t old_len;

#if 0
	Goto_pos(0,0);
	printf("\n Teile Box Nr %i ", box_to_cut);
	if (edge == XEDGE)
		puts(" an X   ");
	else if (edge == YEDGE)
		puts(" an Y   ");
	else if (edge == ZEDGE)
		puts(" an Z   ");

	printf("\n Urbox: %i-%i-%i, %i-%i-%i   ",
		boxes[box_to_cut]->xbox, boxes[box_to_cut]->ybox, boxes[box_to_cut]->zbox,
		boxes[box_to_cut]->boxwid, boxes[box_to_cut]->boxhgt, boxes[box_to_cut]->boxdepth);
#endif

	/*---------- Skalpell ansetzen (Ermitteln der Schnittposition) */
	cut_position = find_cutpos(box_to_cut, edge, histogram);

	/*
	   printf("\n   Cut-Position %i     ", cut_position);
	 */

	/*---- und ein Schnitt durch die Kante der Coloris Boxus... ----*/
	if (edge == XEDGE)
	{
		old_len = boxes[box_to_cut]->boxwid;
		boxes[box_to_cut]->boxwid = cut_position - boxes[box_to_cut]->xbox;	/* alte Box anpassen */
		boxes[num_of_boxes]->xbox = cut_position;	/* und neue generieren */
		boxes[num_of_boxes]->ybox = boxes[box_to_cut]->ybox;
		boxes[num_of_boxes]->zbox = boxes[box_to_cut]->zbox;
		boxes[num_of_boxes]->boxwid = old_len - boxes[box_to_cut]->boxwid;
		boxes[num_of_boxes]->boxhgt = boxes[box_to_cut]->boxhgt;
		boxes[num_of_boxes]->boxdepth = boxes[box_to_cut]->boxdepth;
	} else if (edge == YEDGE)
	{
		old_len = boxes[box_to_cut]->boxhgt;
		boxes[box_to_cut]->boxhgt = cut_position - boxes[box_to_cut]->ybox;	/* alte Box anpassen */
		boxes[num_of_boxes]->xbox = boxes[box_to_cut]->xbox;	/* und neue generieren */
		boxes[num_of_boxes]->ybox = cut_position;
		boxes[num_of_boxes]->zbox = boxes[box_to_cut]->zbox;
		boxes[num_of_boxes]->boxwid = boxes[box_to_cut]->boxwid;
		boxes[num_of_boxes]->boxhgt = old_len - boxes[box_to_cut]->boxhgt;
		boxes[num_of_boxes]->boxdepth = boxes[box_to_cut]->boxdepth;
	} else if (edge == ZEDGE)
	{
		old_len = boxes[box_to_cut]->boxdepth;
		boxes[box_to_cut]->boxdepth = cut_position - boxes[box_to_cut]->zbox;	/* alte Box anpassen */
		boxes[num_of_boxes]->xbox = boxes[box_to_cut]->xbox;	/* und neue generieren */
		boxes[num_of_boxes]->ybox = boxes[box_to_cut]->ybox;
		boxes[num_of_boxes]->zbox = cut_position;
		boxes[num_of_boxes]->boxwid = boxes[box_to_cut]->boxwid;
		boxes[num_of_boxes]->boxhgt = boxes[box_to_cut]->boxhgt;
		boxes[num_of_boxes]->boxdepth = old_len - boxes[box_to_cut]->boxdepth;
	}
#if 0
	printf("\n Box 1 (Nr %i): %i-%i-%i, %i-%i-%i   ", box_to_cut,
		   boxes[box_to_cut].xbox, boxes[box_to_cut].ybox, boxes[box_to_cut].zbox,
		   boxes[box_to_cut].boxwid, boxes[box_to_cut].boxhgt, boxes[box_to_cut].boxdepth);

	printf("\n Box 2 (Nr. %i): %i-%i-%i, %i-%i-%i   ", num_of_boxes,
		   boxes[num_of_boxes].xbox, boxes[num_of_boxes].ybox, boxes[num_of_boxes].zbox,
		   boxes[num_of_boxes].boxwid, boxes[num_of_boxes].boxhgt, boxes[num_of_boxes].boxdepth);
	(void)Cnecin();
#endif
}







/*---------------------------------------------------------------------	*/
/*								find_edge								*/
/*	Sucht den W�rfel mit der l�ngsten Kante �berhaupt und liefert die	*/
/*	Nummer des w�rfels mit return() und die Kante in *edge zur�ck.		*/
/*---------------------------------------------------------------------	*/
static int16_t find_edge(int16_t *edge, int16_t num_of_boxes)
{
	int16_t t;
	int16_t w, h, d;
	int16_t boxreturn = 0;
	int16_t edgereturn = 0;
	int16_t curedge = 0;
	int16_t maxlen = 0;
	int16_t curmaxlen = 0;

	for (t = 0; t < num_of_boxes; t++)
	{
		w = boxes[t]->boxwid;
		h = boxes[t]->boxhgt;
		d = boxes[t]->boxdepth;

		/*----------------- L�ngste Kante im W�rfel t finden */
		if (w > h)
		{
			curedge = XEDGE;
			curmaxlen = w;
		} else
		{
			curedge = YEDGE;
			curmaxlen = h;
		}

		if ((curedge == XEDGE && d > w) || (curedge == YEDGE && d > h))
		{
			curedge = ZEDGE;
			curmaxlen = d;
		}

		/*--------------------- L�nger als die l�ngste zuvor gefundene Kante? */
		if (curmaxlen > maxlen)
		{
			edgereturn = curedge;
			boxreturn = t;
			maxlen = curmaxlen;
		}
	}

	*edge = edgereturn;
	return boxreturn;
}



/*---------------------------------------------------------------------	*/
/*								shrink_box								*/
/* Schrumpft die MC_BOX mit der Nummer num_of_box auf ihre Extrema ein	*/
/*---------------------------------------------------------------------	*/
static void shrink_box(int16_t num_of_box, long *histogram)
{
	int16_t t;
	uint16_t diff;
	int16_t w, h, d;
	int16_t x, y, z;
	unsigned long sum;

	w = boxes[num_of_box]->boxwid;
	h = boxes[num_of_box]->boxhgt;
	d = boxes[num_of_box]->boxdepth;
	x = boxes[num_of_box]->xbox;
	y = boxes[num_of_box]->ybox;
	z = boxes[num_of_box]->zbox;


	/*--- von oben -----*/
	for (t = y; t < y + h; t++)
	{
#if ASSEMBLER
		sum = sumplane_xz(histogram, boxes[num_of_box], t);
#else
		sum = sum_xzplane(num_of_box, t, histogram);
#endif
		if (sum != 0)
			break;
	}
	diff = t - y;
	boxes[num_of_box]->boxhgt -= diff;
	boxes[num_of_box]->ybox = t;
	/*--- von unten -----*/
	t = boxes[num_of_box]->boxhgt;
	y = boxes[num_of_box]->ybox;
	do
	{
		t--;
#if ASSEMBLER
		sum = sumplane_xz(histogram, boxes[num_of_box], t + y);
#else
		sum = sum_xzplane(num_of_box, t + y, histogram);
#endif
	} while (sum == 0 && t > 1);
	boxes[num_of_box]->boxhgt = t + 1;

	/*--- von links -----*/
	for (t = x; t < x + w; t++)
	{
#if ASSEMBLER
		sum = sumplane_yz(histogram, boxes[num_of_box], t);
#else
		sum = sum_yzplane(num_of_box, t, histogram);
#endif
		if (sum != 0)
			break;
	}
	diff = t - x;
	boxes[num_of_box]->boxwid -= diff;
	boxes[num_of_box]->xbox = t;
	/*--- von rechts -----*/
	t = boxes[num_of_box]->boxwid;
	x = boxes[num_of_box]->xbox;
	do
	{
		t--;
#if ASSEMBLER
		sum = sumplane_yz(histogram, boxes[num_of_box], t + x);
#else
		sum = sum_yzplane(num_of_box, t + x, histogram);
#endif
	} while (sum == 0 && t > 1);
	boxes[num_of_box]->boxwid = t + 1;

	/*--- von vorne -----*/
	for (t = z; t < z + d; t++)
	{
#if ASSEMBLER
		sum = sumplane_xy(histogram, boxes[num_of_box], t);
#else
		sum = sum_xyplane(num_of_box, t, histogram);
#endif
		if (sum != 0)
			break;
	}
	diff = t - z;
	boxes[num_of_box]->boxdepth -= diff;
	boxes[num_of_box]->zbox = t;
	/*--- von hinten -----*/
	t = boxes[num_of_box]->boxdepth;
	z = boxes[num_of_box]->zbox;
	do
	{
		t--;
#if ASSEMBLER
		sum = sumplane_xy(histogram, boxes[num_of_box], t + z);
#else
		sum = sum_xyplane(num_of_box, t + z, histogram);
#endif
	} while (sum == 0 && t > 1);
	boxes[num_of_box]->boxdepth = t + 1;
}



/*-------------------------------------------------------------------------------------	*/
/*										MEDIAN CUT										*/
/*	Farbquantisierung nach Heckbert aus dem Histogramm *histogram des Bildes *picture	*/
/*-------------------------------------------------------------------------------------	*/
void median_cut(long *histogram, SMURF_PIC *picture, SYSTEM_INFO *sysinfo)
{
	int16_t currentbox = 0;
	int16_t edge;
	int16_t num_of_boxes = 1;				/*Initial 1 Box (der gesamte W�rfel) */
	int16_t dest_colors, t;
	int16_t x, y, z;
	int16_t w, h, d;
	int16_t cx, cy, cz;
	unsigned long index3d;
	unsigned long rcol[SM_PALETTE_MAX];
	unsigned long gcol[SM_PALETTE_MAX];
	unsigned long bcol[SM_PALETTE_MAX];
	unsigned long divide[SM_PALETTE_MAX];
	long frequency;
	uint8_t red, green, blue;
	WORD *pr;
	WORD *pg;
	WORD *pb;

	/*
	 * Die Anzahl an Farben ist entweder die an Screenfarben oder, wenn weniger, 
	 * die Anzahl, die im Bild �berhaupt vorhanden ist.
	 */
	dest_colors = sysinfo->Max_col + 1;
	if ((int16_t) colnum < dest_colors)
		dest_colors = (int16_t) colnum;

	for (t = 0; t < dest_colors + 1; t++)
	{
		boxes[t] = malloc(sizeof(MC_BOX));
		memset(boxes[t], 0, sizeof(MC_BOX));
	}

	/*
	 *  Box 0 ist erstmal der Gesamte W�rfel...
	 */
	Dialog.busy.reset(20, "Variance-MedianCut");

	boxes[0]->xbox = 0;
	boxes[0]->ybox = 0;
	boxes[0]->zbox = 0;
	boxes[0]->boxwid = 32;
	boxes[0]->boxhgt = 32;
	boxes[0]->boxdepth = 32;
	shrink_box(0, histogram);


	/*
	 *  und jetzt immer wieder zerschneiden, bis dest_col boxes da sind... 
	 */
	do
	{
		/* Die l�ngste Kante und deren W�rfel suchen.... */
		currentbox = find_edge(&edge, num_of_boxes);

		/* Schnipp-Schnapp! */
		cut_box(num_of_boxes, currentbox, edge, histogram);

		shrink_box(currentbox, histogram);
		shrink_box(num_of_boxes, histogram);

		if (!(num_of_boxes & 31))
			Dialog.busy.draw((num_of_boxes << 5) / dest_colors + 20);
	} while (++num_of_boxes < dest_colors);


	/*
	 *  Jetzt mu� aus den Boxes noch die Palette ermittelt werden.
	 */
	Dialog.busy.reset(64, "erzeuge Palette");
	for (t = 0; t < num_of_boxes; t++)
	{
		x = boxes[t]->xbox;
		y = boxes[t]->ybox;
		z = boxes[t]->zbox;
		w = boxes[t]->boxwid;
		h = boxes[t]->boxhgt;
		d = boxes[t]->boxdepth;

		divide[t] = rcol[t] = gcol[t] = bcol[t] = 0;

		for (cz = z; cz < z + d; cz++)
		{
			for (cy = y; cy < y + h; cy++)
			{
				for (cx = x; cx < x + w; cx++)
				{
					index3d = ComputeIndex3d(cx, cy, cz);
					frequency = histogram[index3d];
					divide[t] += frequency;
					rcol[t] += (long) cx *frequency;
					gcol[t] += (long) cy *frequency;
					bcol[t] += (long) cz *frequency;
				}
			}
		}
	}

	for (t = 0; t < dest_colors + 1; t++)
		free(boxes[t]);

	/*
	 * So. Jetzt hab ich 'nen Eimer voll 15 Bit-Werte und die m�ssen in die Bildpalette.
	 * Zuerst wird da die Systempalette eingef�gt und dann die ermittelten Farben dr�ber-
	 * geschrieben, damit so viel von der Syspal erhalten bleibt, wie m�glich.
	 */
	pr = picture->red;
	pg = picture->grn;
	pb = picture->blu;

	for (t = 0; t < sysinfo->Max_col + 1; t++)
	{
		pr[t] = sysinfo->pal_red[t] * 0.255;
		pg[t] = sysinfo->pal_green[t] * 0.255;
		pb[t] = sysinfo->pal_blue[t] * 0.255;
	}

	for (t = 0; t < num_of_boxes; t++)
	{
		red = (uint8_t) (rcol[t] / divide[t]);
		green = (uint8_t) (gcol[t] / divide[t]);
		blue = (uint8_t) (bcol[t] / divide[t]);
		pr[t] = red << 3;
		pg[t] = green << 3;
		pb[t] = blue << 3;
	}

}


void make_tmp_nct(long *histogram, SMURF_PIC * pic, unsigned short maxc)
{
	long tt;
	long par[10];
	uint8_t *nct;
	WORD rpal[SM_PALETTE_MAX], gpal[SM_PALETTE_MAX], bpal[SM_PALETTE_MAX];
	long curr_freq, lowest_freq;
	unsigned short lowfreq_idx;
	short not_in_nct;
	short idx;

	/*--------------- Palette auf 15 Bit skalieren ------------*/
	for (tt = 0; tt < SM_PALETTE_MAX; tt++)
	{
		rpal[tt] = pic->red[tt] >> 3;
		gpal[tt] = pic->grn[tt] >> 3;
		bpal[tt] = pic->blu[tt] >> 3;
	}


	/*--------------- die am wenigsten h�ufigste Farbe finden ----*/
	lowfreq_idx = 0;
	lowest_freq = curr_freq = histogram[0];
	for (tt = 0; tt < 32768L; tt++)
	{
		curr_freq = histogram[tt];

		if (curr_freq < lowest_freq)
		{
			lowest_freq = curr_freq;
			lowfreq_idx = (unsigned short) tt;
		}
	}

	/* Den Palettenindex dazu ermitteln */
	par[0] = (long) (pic->red);
	par[1] = (long) (pic->grn);
	par[2] = (long) (pic->blu);
	par[3] = (lowfreq_idx >> 10) & 31;
	par[4] = (lowfreq_idx >> 5) & 31;
	par[5] = lowfreq_idx & 31;
	not_in_nct = seek_nearest_col(par, maxc);

	pic->not_in_nct = not_in_nct;		/* und in die Bildstruktur eintragen */

	/*-------------- Histogrammbasierte NCT erzeugen -------------*/
	if (pic->local_nct != NULL)
		SMfree(pic->local_nct);
	nct = pic->local_nct = SMalloc(32769L);
	memset(nct, not_in_nct, 32769L);
	Dialog.busy.reset(65, "generiere NCT...");

	par[0] = (long) rpal;
	par[1] = (long) gpal;
	par[2] = (long) bpal;

	for (tt = 0; tt < 32768L; tt++)
	{
		if (histogram[tt])
		{
			par[3] = (tt >> 10) & 31;
			par[4] = (tt >> 5) & 31;
			par[5] = tt & 31;
			idx = seek_nearest_col(par, maxc);
			*(nct + tt) = idx;
		} else
		{
			nct[tt] = (unsigned char) not_in_nct;
		}

		if (!(tt & 8191))
			Dialog.busy.draw((short) (64 + (tt >> 9)));
	}


#if 0
	for (tt = 0; tt < SM_PALETTE_MAX; tt++)
	{
		cred[cnt] = pic->red[tt] >> 3;
		cred[cnt + 1] = pic->grn[tt] >> 3;
		cred[cnt + 2] = pic->blu[tt] >> 3;
		cnt += 3;
	}

	par[0] = (long) &cred;
	par[3] = (long) nct;
	makeNCT(par, maxc);
#endif
}
