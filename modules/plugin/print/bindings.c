#include <aes.h>
#include <vdi.h>
#include "wdialog.h"

void vq_scrninfo(int handle, int *work_out);
int vq_ext_devinfo(int handle, int device, int *dev_exists, char *name);

int	objc_sysvar( int ob_smode, int ob_swhich, int ob_sival1,
				int ob_sival2, int *ob_soval1, int *ob_soval2);


AESPB aespb;


void vq_scrninfo(int handle, int *work_out)
{
	VDIPB vdipb;


	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.intout = work_out;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.ptsout = _VDIParBlk.ptsout;

	vdipb.contrl[0] = 102;
	vdipb.contrl[1] = 0;
	vdipb.contrl[3] = 1;
	vdipb.contrl[5] = 1;
	vdipb.contrl[6] = handle;
	vdipb.intin[0] = 2;

	vdi(&vdipb);

	return;
} /* vq_scrninfo */


/* leicht ver�ndertes Binding, file_path und file_name werden nicht durchgereicht */
int vq_ext_devinfo(int handle, int device, int *dev_exists, char *name)
{
	char file_name[33], file_path[257];

	VDIPB vdipb;


	vdipb.contrl = _VDIParBlk.contrl;
	vdipb.ptsin = _VDIParBlk.ptsin;
	vdipb.intin = _VDIParBlk.intin;
	vdipb.ptsout = _VDIParBlk.ptsout;
	vdipb.intout = _VDIParBlk.intout;

	vdipb.intin[0] = device;
	*(char **)&vdipb.intin[1] = file_path;
	*(char **)&vdipb.intin[3] = file_name;
	*(char **)&vdipb.intin[5] = name;

	vdipb.contrl[0] = 248;
	vdipb.contrl[1] = 0;
	vdipb.contrl[3] = 7;
	vdipb.contrl[5] = 4242;
	vdipb.contrl[6] = handle;

	vdi(&vdipb);

	*dev_exists = vdipb.intout[0];

	return(vdipb.intout[1]);
} /* vq_ext_devinfo */


/* v_ext_opnwk--------------------------------------------------
	Erweiterter v_opnwk, mit dem auch die erweiterten Parameter
	von NVDI unterst�tzt werden.
	-----------------------------------------------------------*/
void v_ext_opnwk(int *work_in, int *handle, int *work_out)
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

	return;
} /* v_ext_opnwk */


int	objc_sysvar(int ob_smode, int ob_swhich, int ob_sival1,
				int ob_sival2, int *ob_soval1, int *ob_soval2)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

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
} /* objc_sysvar */


int	SM_wind_set(int wi_ghandle, int wi_gfield, int wi_gw1,
				int wi_gw2, int wi_gw3, int wi_gw4)
{
	AESPB aespb;


	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (int *)_GemParBlk.addrin;
	aespb.addrout = (int *)_GemParBlk.addrout;

	aespb.contrl[0] = 105;
	aespb.contrl[1] = 6;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 0;
	aespb.contrl[4] = 0;

	aespb.intin[0] = wi_ghandle;
	aespb.intin[1] = wi_gfield;
	aespb.intin[2] = wi_gw1;
	aespb.intin[3] = wi_gw2;
	aespb.intin[4] = wi_gw3;
	aespb.intin[5] = wi_gw4;

	_crystal(&aespb);

	return(aespb.intout[0]);
} /* wind_set */


PRN_DIALOG *pdlg_create(int dialog_flags)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (int *)_GemParBlk.addrin;
	aespb.addrout = (int *)_GemParBlk.addrout;

	aespb.contrl[0] = 200;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 0;
	aespb.contrl[3] = 0;
	aespb.contrl[4] = 1;

	aespb.intin[0] = dialog_flags;

	_crystal(&aespb);

	return(_GemParBlk.addrout[0]);
} /* pdlg_create */


int pdlg_delete(PRN_DIALOG *prn_dialog)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 201;
	aespb.contrl[1] = 0;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 0;

	_GemParBlk.addrin[0] = prn_dialog;

	_crystal(&aespb);

	return(aespb.intout[0]);
} /* pdlg_delete */


int pdlg_open(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings,
			  char *document_name, int option_flags, int x, int y)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 202;
	aespb.contrl[1] = 3;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 3;
	aespb.contrl[4] = 0;

	aespb.intin[0] = option_flags;
	aespb.intin[1] = x;
	aespb.intin[2] = y;

	_GemParBlk.addrin[0] = prn_dialog;
	_GemParBlk.addrin[1] = settings;
	_GemParBlk.addrin[2] = document_name;

	_crystal(&aespb);

	return(aespb.intout[0]);
} /* pdlg_open */


int pdlg_close(PRN_DIALOG *prn_dialog, int *x, int *y)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 203;
	aespb.contrl[1] = 0;
	aespb.contrl[2] = 3;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 0;

	_GemParBlk.addrin[0] = prn_dialog;

	_crystal(&aespb);

	*x = aespb.intout[1];
	*y = aespb.intout[2];

	return(aespb.intout[0]);
} /* pdlg_close */


int pdlg_update(PRN_DIALOG *prn_dialog, char *name)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 205;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 3;
	aespb.contrl[4] = 0;

	aespb.intin[0] = 2;

	_GemParBlk.addrin[0] = prn_dialog;
	_GemParBlk.addrin[1] = 0;
	_GemParBlk.addrin[2] = name;

	_crystal(&aespb);

	return(aespb.intout[0]);
} /* pdlg_update */


PRN_SETTINGS *pdlg_new_settings(PRN_DIALOG *prn_dialog)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 205;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 0;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 1;

	aespb.intin[0] = 5;

	_GemParBlk.addrin[0] = prn_dialog;

	_crystal(&aespb);

	return(_GemParBlk.addrout[0]);
} /* pdlg_new_settings */


int pdlg_free_settings(PRN_SETTINGS *prn_settings)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 205;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 0;

	aespb.intin[0] = 6;

	_GemParBlk.addrin[0] = prn_settings;

	_crystal(&aespb);

	return(aespb.intout[0]);
} /* pdlg_free_settings */


int pdlg_evnt(PRN_DIALOG *prn_dialog, PRN_SETTINGS *prn_settings,
			  EVNT *events, int *button)
{
	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;
	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 206;
	aespb.contrl[1] = 0;
	aespb.contrl[2] = 2;
	aespb.contrl[3] = 3;
	aespb.contrl[4] = 0;

	_GemParBlk.addrin[0] = prn_dialog;
	_GemParBlk.addrin[1] = prn_settings;
	_GemParBlk.addrin[2] = events;

	_crystal(&aespb);

	*button = aespb.intout[1];

	return(aespb.intout[0]);
} /* pdlg_event */