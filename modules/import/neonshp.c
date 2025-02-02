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

/* ******************************************** */
/*                                              */
/*                                              */
/* ******************************************** */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "import.h"
#include "smurfine.h"

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))

MOD_INFO module_info = {
	"NeoN-Shapefile-Importer",
	0x0010,
	"Olaf Piesche",
/* Extensionen */
	{ "SHP", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};




static long number_of_points;					/* Punkte im ganzen File? */
static long number_of_polys;					/* Wieviele Polygone? */




/*------------------------------------------------------------- */
/*  Liest alle Punkte eines Polygons aus dem File aus.          */
/*  Zur�ckgeliefert wird ein Zeiger auf das Ende des Polygons,  */
/*  poly_point_number und poly_defs werden gef�llt.             */
/*------------------------------------------------------------- */
static char *get_poly_points(char *data_pos, int points, long *pointx, long *pointy, long *poly_point_number, long *poly_defs)
{
	char string[32];
	int points_read;
	char *endpos;
	long xposition, yposition;

	long len_of_number;
	double orig_number;
	int t;

	number_of_polys++;					/* ein Polygon mehr... */
	poly_point_number[number_of_polys] = points;	/* Punktzahl eintragen */

/*printf("\n Polygon %li, %i Punkte...", number_of_polys, points);*/

	for (t = 0; t < points; t++)
	{
		endpos = strchr(data_pos, '\t');	/* Ende der Zahl suchen */
		len_of_number = endpos - data_pos;	/* L�nge der Zahl ermitteln */
		strncpy(string, data_pos, len_of_number);
		orig_number = atof(string);
		pointx[number_of_points] = (long) (orig_number * (long) (2L << 20L));

/*printf("\nXpos: %.8f", orig_number);*/
		data_pos += len_of_number;
		data_pos += 1;					/* zur Y-Pos gehen */


		endpos = strchr(data_pos, '\t');	/* Ende der Zahl suchen */
		len_of_number = endpos - data_pos;	/* L�nge der Zahl ermitteln */
		strncpy(string, data_pos, len_of_number);
		orig_number = atof(string);
		pointy[number_of_points] = (long) (orig_number * (long) (2L << 20L));

/*printf("\tYpos: %.8f", orig_number);*/
		data_pos += len_of_number;
		data_pos += 4;					/* tab, sp-Flag und \n �berspringen */

		number_of_points++;
	}

	return data_pos;
}





/*------------------------------------------------------------- */
/*  sucht nach einem "numpoints"-Tag, schreibt numpoints nach   */
/*  *points und liefert einen Zeiger auf die X-Koordinate des   */
/*  ersten Punktes zur�ck.                                      */
/*------------------------------------------------------------- */
static char *find_numpoints(char *data_pos, int *points)
{
	char pts[6];
	char *endpos;
	int len_of_number;

	data_pos = strstr(data_pos, "numpoints");

	if (data_pos == NULL)
		return NULL;

	endpos = strstr(data_pos, "\n");	/* Ende der Zahl suchen */
	data_pos += 10;						/* data_pos auf die Zahl ausrichten */

	len_of_number = endpos - data_pos;	/* L�nge der Zahl ermitteln */

	strncpy(pts, data_pos, len_of_number);
	*(points) = atoi(pts);				/* Zahl �bernehmen */

	data_pos += len_of_number;			/* ans Ende der Zahl */
	data_pos += 4;						/* 3 Returns+1 Tab �berspringen. */

	return data_pos;
}


short imp_module_main(GARGAMEL *smurf_struct)
{
	long *pointx;						/* X-Positionen */
	long *pointy;						/* Y-Positionen */

	long *poly_point_number;			/* Wieviele Punkte im Polygon n? */
	long *poly_defs;					/* Polygondefinition */

	char *file_data;
	char *data_pos;
	char *dp2;

	int points;

	pointx = (long *)Malloc(10000l * sizeof(*pointx));
	pointy = (long *)Malloc(10000l * sizeof(*pointy));
	poly_point_number = (long *)Malloc(5000 * sizeof(*poly_point_number));


	file_data = smurf_struct->smurf_pic->pic_data;

	/* Kennung(en) pr�fen */
	if (strncmp((char *)file_data, "RIP-2D-Shape(ascii)", 19) != 0)
		return M_INVALID;
#if 0
	if (strncmp((char *)file_data + 22, "Splineversion", 13) != 0)
		return M_INVALID;
	if (strncmp((char *)file_data + 37, "Params: ", 8) != 0)
		return M_INVALID;
#endif


/*Goto_pos(1,1);*/

	/* Und los. */
	while (data_pos != NULL)
	{
/*      printf("\nSuche nach numpoints...");    */
		dp2 = data_pos;				/* Das ist n�tig, sonst funktionierts in der Schleife nicht */
		data_pos = find_numpoints(dp2, &points);	/* Poly-Punktzahl suchen */
/*      printf("\nPoly mit %i points gefunden, datapos=%li", points, data_pos); */

		if (data_pos == NULL)
			break;

		dp2 = data_pos;				/* Das ist n�tig, sonst funktionierts in der Schleife nicht */
		data_pos = get_poly_points(dp2, points, pointx, pointy, poly_point_number, poly_defs);
	}

	_Mshrink(poly_point_number, number_of_polys * sizeof(*poly_point_number));
	_Mshrink(pointx, number_of_points * sizeof(*pointx));
	_Mshrink(pointy, number_of_points * sizeof(*pointy));

#if 0
	printf("\n\n Polygone gesamt: %li", number_of_polys);
	printf("\n Punkte gesamt: %li", number_of_points);
	(void)Cnecin();
#endif


	smurf_struct->smurf_pic->pic_data = NULL;
	smurf_struct->smurf_pic->depth = 16;

	strcpy(smurf_struct->smurf_pic->format_name, "NeoN Shape");
	smurf_struct->smurf_pic->pic_width = 0;
	smurf_struct->smurf_pic->pic_height = 0;
	smurf_struct->smurf_pic->x_coord = pointx;
	smurf_struct->smurf_pic->y_coord = pointy;
	smurf_struct->smurf_pic->number_of_polys = number_of_polys;
	smurf_struct->smurf_pic->poly_point_number = poly_point_number;
	smurf_struct->smurf_pic->this_is_a_vector = 1;

	return M_PICDONE;					/* das wars. */
}
