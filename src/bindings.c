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

#ifdef __PUREC__
#include <aes.h>
#include <vdi.h>
#else
#include <gem.h>
#endif
#include <stdio.h>
#include "portab.h"
#include "smurf_st.h"
#include "bindings.h"
#include "nvdi5ras.h"


#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)
void vq_scrninfo(WORD handle, WORD *work_out)
{
	VDIPB vdipb;


	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = work_out;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 102;
	vdipb.contrl[1] = 0;
	vdipb.contrl[3] = 1;
	vdipb.contrl[5] = 1;
	vdipb.contrl[6] = handle;
	vdipb.intin[0] = 2;

	vdi(&vdipb);
}


int vq_ext_devinfo(int handle, int device, int *dev_exists, char *file_path, char *file_name, char *name)
{
	VDIPB vdipb;


	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsout = _VDIParBlk.ptsout;
	vdipb.intout = _VDIParBlk.intout;

	vdipb.contrl[0] = 248;
	vdipb.contrl[1] = 0;
	vdipb.contrl[3] = 7;
	vdipb.contrl[5] = 4242;
	vdipb.contrl[6] = handle;

	vdipb.intin[0] = device;
	*(char **)&vdipb.intin[1] = file_path;
	*(char **)&vdipb.intin[3] = file_name;
	*(char **)&vdipb.intin[5] = name;

	vdi(&vdipb);

	*dev_exists = vdipb.intout[0];

	return(vdipb.intout[1]);
}


/* v_ext_opnwk--------------------------------------------------
	Erweiterter v_opnwk, mit dem auch die erweiterten Parameter
	von NVDI unterstÅtzt werden.
	-----------------------------------------------------------*/
void v_ext_opnwk(WORD *work_in, WORD *handle, WORD *work_out)
{
	VDIPB vdipb;

	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = work_in;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = work_out;
	vdipb.ptsout = work_out+45;

	vdipb.contrl[0] = 1;
	vdipb.contrl[1] = 0;
	vdipb.contrl[2] = 6;
	vdipb.contrl[3] = 16;
	vdipb.contrl[4] = 45;
	vdipb.contrl[5] = 0;
	vdipb.contrl[6] = 0;

	vdi(&vdipb);
	
	*handle = vdipb.contrl[6];
}


/* vr_transfer_bits--------------------------------------------
	neue Rasterwandelroutine von NVDI 5
	-----------------------------------------------------------*/
void vr_transfer_bits(int handle, GCBITMAP *src_bm, GCBITMAP *dst_bm, int *src_rect, int *dst_rect, int mode)
{
	VDIPB vdipb;

	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = _VDIParBlk.intout;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 170;
	vdipb.contrl[1] = 4;
	vdipb.contrl[2] = 0;
	vdipb.contrl[3] = 4;
	vdipb.contrl[4] = 0;
	vdipb.contrl[5] = 0;
	vdipb.contrl[6] = handle;

	*(GCBITMAP **)&vdipb.contrl[7] = src_bm;
	*(GCBITMAP **)&vdipb.contrl[9] = dst_bm;
	*(GCBITMAP **)&vdipb.contrl[11] = 0L;

	vdipb.intin[0] = mode;
	vdipb.intin[1] = 0;
	vdipb.intin[2] = 0;
	vdipb.intin[3] = 0;

	vdipb.ptsin[0] = src_rect[0];
	vdipb.ptsin[1] = src_rect[1];
	vdipb.ptsin[2] = src_rect[2];
	vdipb.ptsin[3] = src_rect[3];

	vdipb.ptsin[4] = dst_rect[0];
	vdipb.ptsin[5] = dst_rect[1];
	vdipb.ptsin[6] = dst_rect[2];
	vdipb.ptsin[7] = dst_rect[3];

	vdi(&vdipb);
}


/* vq_px_format------------------------------------------------
	Liefer Pixelformat und Farbraum von handle zurÅck
	-----------------------------------------------------------*/
long vq_px_format(int handle, unsigned long *px_format)
{
	VDIPB vdipb;


	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = _VDIParBlk.intout;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 204;
	vdipb.contrl[1] = 0;
	vdipb.contrl[2] = 0;
	vdipb.contrl[3] = 0;
	vdipb.contrl[4] = 4;
	vdipb.contrl[5] = 3;
	vdipb.contrl[6] = handle;

	vdi(&vdipb);

	*px_format = *(long *)&vdipb.intout[2];

	return(	*(long *)&vdipb.intout[0]);
}


/* v_get_ctab_id-----------------------------------------------
	Fordert eine Farbtabellenid fÅr die Device vom System an
	-----------------------------------------------------------*/
long v_get_ctab_id(int handle)
{
	VDIPB vdipb;

	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = _VDIParBlk.intout;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 206;
	vdipb.contrl[1] = 0;
	vdipb.contrl[2] = 0;
	vdipb.contrl[3] = 0;
	vdipb.contrl[4] = 2;
	vdipb.contrl[5] = 6;
	vdipb.contrl[6] = handle;

	vdi(&vdipb);

	return(*(long *)&vdipb.intout[0]);
}


/* v_create_ctab-----------------------------------------------
	Legt eine Farbtabellenstruktur an und initialisiert sie
	-----------------------------------------------------------*/
COLOR_TAB *v_create_ctab(int handle, long color_space, unsigned long px_format)
{
	VDIPB vdipb;

	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = _VDIParBlk.intout;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 206;
	vdipb.contrl[1] = 0;
	vdipb.contrl[2] = 0;
	vdipb.contrl[3] = 4;
	vdipb.contrl[4] = 2;
	vdipb.contrl[5] = 8;
	vdipb.contrl[6] = handle;

	*(long *)&vdipb.intin[0] = color_space;
	*(unsigned long *)&vdipb.intin[2] = px_format;

	vdi(&vdipb);

	return(*(COLOR_TAB **)&vdipb.intout[0]);
}


/* v_delete_ctab-----------------------------------------------
	Lîscht eine Farbtabellenstruktur
	-----------------------------------------------------------*/
int v_delete_ctab(int handle, COLOR_TAB *ctab)
{
	VDIPB vdipb;

	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intout = _VDIParBlk.intout;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 206;
	vdipb.contrl[1] = 0;
	vdipb.contrl[2] = 0;
	vdipb.contrl[3] = 2;
	vdipb.contrl[4] = 1;
	vdipb.contrl[5] = 9;
	vdipb.contrl[6] = handle;

	*(COLOR_TAB **)&vdipb.intin[0] = ctab;

	vdi(&vdipb);

	return(vdipb.intout[0]);
}


WORD	objc_sysvar(WORD ob_smode, WORD ob_swhich, WORD ob_sival1, WORD ob_sival2, WORD *ob_soval1, WORD *ob_soval2)
{
	AESPB aespb;


	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (int *)_GemParBlk.addrin;
	aespb.addrout = (int *)_GemParBlk.addrout;

	aespb.contrl[0] = 48;
	aespb.contrl[1] = 4;
	aespb.contrl[2] = 3;
	aespb.contrl[3] = 0;
	aespb.contrl[4] = 0;

	aespb.intin[0] = ob_smode;
	aespb.intin[1] = ob_swhich;
	aespb.intin[2] = ob_sival1;
	aespb.intin[3] = ob_sival2;

	_crystal(&aespb);

	*ob_soval1 = aespb.intout[1];
	*ob_soval2 = aespb.intout[2];

	return(aespb.intout[0]);
}


/*
 * Ein alternatives Binding fÅr fsel_exinput() (Die Funktionsnummer ist
 * identisch!), das die öbergabe der Callback-Adresse erlaubt. Andere
 * Fileselectoren sollten den ÅberzÑhligen Parameter einfach ignorieren.
 */
int cdecl fsel_boxinput(char *path, char *name, int *button, char *label, void *callback)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (int *)_GemParBlk.addrin;
	aespb.addrout = (int *)_GemParBlk.addrout;

	aespb.contrl[0] = 91;
	aespb.contrl[1] = 0;
	aespb.contrl[2] = 2;
	aespb.contrl[3] = 4;
	aespb.contrl[4] = 0;

	_GemParBlk.addrin[0] = path;
	_GemParBlk.addrin[1] = name;
	_GemParBlk.addrin[2] = label;
	_GemParBlk.addrin[3] = callback;

	_crystal(&aespb);

	*button = aespb.intout[1];

	return(aespb.intout[0]);
}
#endif


int	SM_wind_get(int wi_ghandle, int wi_gfield, int *wi_gw1, int *wi_gw2, int *wi_gw3, int *wi_gw4)
{
	return wind_get(wi_ghandle, wi_gfield, wi_gw1, wi_gw2, wi_gw3, wi_gw4);
}


int	SM_wind_set(int wi_ghandle, int wi_gfield, int wi_gw1, int wi_gw2, int wi_gw3, int wi_gw4)
{
	return wind_set(wi_ghandle, wi_gfield, wi_gw1, wi_gw2, wi_gw3, wi_gw4);
}


#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)
/* Binding fÅr die neue VDI-Funktion v_opnbm(), Offscreen-Bitmap îffnen */
/* VDI 100, 1 */
void v_opnbm(int *work_in, MFDB *bitmap, int *handle, int *work_out)
{
    /* Wenn work_in[15..19] 0 enthalten, wird eine Bitmap im gerÑtespezifischen
       Format oder mit nur 1 Ebene erzeugt (hÑngt vom MFDB ab). Anderfalls wird
       versucht eine Bitmap mit der Farbanzahl <work_in[15..16]>, <work_in[17]>
       Ebenen, dem Pixelformat <work_in[18]> und der Bitreihenfolge
		<work_in[19]> anzulegen. Falls kein passender Offscreen-Treiber vorhanden 
		ist, kann die Bitmap nicht geîffnet werden. */

    VDIPB vdipb;


    vdipb.contrl = _VDIParBlk.contrl;
    vdipb.intin = work_in;
    vdipb.ptsin = _VDIParBlk.ptsin;
    vdipb.intout = work_out;
    vdipb.ptsout = work_out + 45;

    vdipb.contrl[0] = 100;
    vdipb.contrl[1] = 0;
    vdipb.contrl[3] = 20;
    vdipb.contrl[5] = 1;
    *(MFDB **)&vdipb.contrl[7] = bitmap;

    vdi(&vdipb);

    *handle = vdipb.contrl[6];
}


/* Binding fÅr die neue VDI-Funktion v_clsbm(), Offscreen-Bitmap schlieûen */
/* VDI 101, 1 */
void v_clsbm(int handle)
{
    VDIPB vdipb;


    vdipb.contrl = _VDIParBlk.contrl;
    vdipb.intin = _VDIParBlk.intin;
    vdipb.ptsin = _VDIParBlk.ptsin;
    vdipb.intout = _VDIParBlk.intout;
    vdipb.ptsout = _VDIParBlk.ptsout;

    vdipb.contrl[0] = 101;
    vdipb.contrl[1] = 0;
    vdipb.contrl[3] = 0;
    vdipb.contrl[5] = 1;
    vdipb.contrl[6] = handle;

    vdi(&vdipb);
}
#endif
