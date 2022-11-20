#include "import.h"
#include "bindings.h"
#include "wdialog.h"

#ifdef __GEMLIB__
#define contrl control /* different naming in VDIPB */
#endif

#if !defined(__GEMLIB__) && !defined(__PORTAES_H__)

WORD objc_sysvar(WORD ob_smode, WORD ob_swhich, WORD ob_sival1, WORD ob_sival2, WORD *ob_soval1, WORD *ob_soval2);


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


WORD vq_ext_devinfo(WORD handle, WORD device, WORD *dev_exists, char *file_path, char *file_name, char *name)
{
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

	return vdipb.intout[1];
}


WORD objc_sysvar(WORD ob_smode, WORD ob_swhich, WORD ob_sival1, WORD ob_sival2, WORD *ob_soval1, WORD *ob_soval2)
{
	AESPB aespb;

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

	return aespb.intout[0];
}


PRN_DIALOG *pdlg_create(WORD dialog_flags)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *)_GemParBlk.addrin;
	aespb.addrout = (void *)_GemParBlk.addrout;

	aespb.contrl[0] = 200;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 0;
	aespb.contrl[3] = 0;
	aespb.contrl[4] = 1;

	aespb.intin[0] = dialog_flags;

	_crystal(&aespb);

	return _GemParBlk.addrout[0];
}


WORD pdlg_delete(PRN_DIALOG *prn_dialog)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

	aespb.contrl[0] = 201;
	aespb.contrl[1] = 0;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 0;

	_GemParBlk.addrin[0] = prn_dialog;

	_crystal(&aespb);

	return aespb.intout[0];
}


WORD pdlg_open(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, const char *document_name, WORD option_flags, WORD x, WORD y)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

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

	return aespb.intout[0];
}


WORD pdlg_close(PRN_DIALOG *prn_dialog, WORD *x, WORD *y)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

	aespb.contrl[0] = 203;
	aespb.contrl[1] = 0;
	aespb.contrl[2] = 3;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 0;

	_GemParBlk.addrin[0] = prn_dialog;

	_crystal(&aespb);

	*x = aespb.intout[1];
	*y = aespb.intout[2];

	return aespb.intout[0];
}


WORD pdlg_update(PRN_DIALOG *prn_dialog, const char *name)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

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

	return aespb.intout[0];
}


PRN_SETTINGS *pdlg_new_settings(PRN_DIALOG *prn_dialog)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

	aespb.contrl[0] = 205;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 0;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 1;

	aespb.intin[0] = 5;

	_GemParBlk.addrin[0] = prn_dialog;

	_crystal(&aespb);

	return _GemParBlk.addrout[0];
}


WORD pdlg_free_settings(PRN_SETTINGS *prn_settings)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

	aespb.contrl[0] = 205;
	aespb.contrl[1] = 1;
	aespb.contrl[2] = 1;
	aespb.contrl[3] = 1;
	aespb.contrl[4] = 0;

	aespb.intin[0] = 6;

	_GemParBlk.addrin[0] = prn_settings;

	_crystal(&aespb);

	return aespb.intout[0];
}


WORD pdlg_evnt(PRN_DIALOG *prn_dialog, PRN_SETTINGS *prn_settings, EVNT *events, WORD *button)
{
	AESPB aespb;

	aespb.contrl = _GemParBlk.contrl;
	aespb.global = _GemParBlk.global;

	aespb.intin = _GemParBlk.intin;
	aespb.intout = _GemParBlk.intout;
	aespb.addrin = (void *) _GemParBlk.addrin;
	aespb.addrout = (void *) _GemParBlk.addrout;

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

	return aespb.intout[0];
}

#endif

/* v_ext_opnwk--------------------------------------------------
	Erweiterter v_opnwk, mit dem auch die erweiterten Parameter
	von NVDI unterstÅtzt werden.
	-----------------------------------------------------------*/
void v_ext_opnwk(WORD *work_in, WORD *handle, WORD *work_out)
{
	VDIPB vdipb;
	WORD contrl[15];
	WORD ptsin[8];

	contrl[0] = 1;
	contrl[1] = 0;
	contrl[2] = 6;
	contrl[3] = 16;
	contrl[4] = 45;
	contrl[5] = 0;
	contrl[6] = 0;

	vdipb.contrl = contrl;
	vdipb.intin = work_in;
	vdipb.ptsin = ptsin;
	vdipb.intout = work_out;
	vdipb.ptsout = work_out + 45;

	vdi(&vdipb);

	*handle = contrl[6];
}
