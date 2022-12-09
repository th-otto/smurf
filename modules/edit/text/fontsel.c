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
 * The Initial Developer of the Original Code is
 * Christian Eyrich
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <stddef.h>
#include "import.h"
#include "fontsel.h"
#include "de/text.h"
#include "bindings.h"
#ifdef __PORTAES_H__
#include <wdlgfslx.h>
#endif

#ifndef _AESversion
#define _AESversion _GemParBlk.global[0]
#endif

#define	TextCast	ob_spec.tedinfo->te_ptext


static WORD call_magic_fsl(WORD handle, FONT_INFO *fontinfo)
{
	WORD back;
	WORD dummy_handle;
	long new_id, new_size, ratio;
	FNT_DIALOG *fnt_dialog;
	WORD check_boxes;
	
	(void) handle;
	new_id = fontinfo->ID;
	new_size = (long) fontinfo->size << 16;
	ratio = 1L << 16;

	/* Workaround weil das bescheuerte MagiC die Attribute */
	/* der bergebenen Workstation verstellt ... */
	dummy_handle = 0;
	v_opnvwk(work_in, &dummy_handle, work_out);
	fnt_dialog = fnts_create(dummy_handle, 0, 0xf, FNTS_3D, "The quick blue Smurf ...", 0L);

	if (fnt_dialog)
	{
		back = fnts_do(fnt_dialog, 0, new_id, new_size, ratio, &check_boxes, &new_id, &new_size, &ratio);

		if (back == FNTS_OK || back == FNTS_SET)
		{
			fontinfo->ID = (WORD) new_id;
			fontinfo->size = (WORD) (new_size >> 16);
		}

		fnts_delete(fnt_dialog, 0);
	} else
	{
#if 0
		form_alert(1, "[1][Fehler beim Aufruf des|MagiC-Fontselectors!][ Hoppla ]");
#endif
#if 0
/* 1.06 Style */
		services->f_alert(alerts[ERROR_MAGIC_FSEL].TextCast, NULL, NULL, NULL, 1);
#endif
		back = -1;
	}

	v_clsvwk(dummy_handle);

	return back;
}



static WORD call_xfsl(WORD handle, FONT_INFO *fontinfo, xFSL *fontsel_struct)
{
	WORD new_id;
	WORD new_size;
	WORD back;

	(void) handle;
	new_id = fontinfo->ID;
	new_size = fontinfo->size;

#if defined(__PUREC__) || (defined(__GNUC__) && defined(__MSHORT__))
	back = fontsel_struct->xfsl_input(0, 0x00FE, NULL, &new_id, &new_size);
#else
	{
		struct xfsl_input_args args;
		
		args.vdihandle = 0;
		args.fontflags = 0x00FE;
		args.headline = NULL;
		args.id = &new_id;
		args.size = &new_size;
		back = fontsel_struct->xfsl_input(args);
	}
#endif

	if (back > 0)
	{
		fontinfo->size = new_size;
		fontinfo->ID = new_id;
	} else if (back < 0)
	{
		services->f_alert(alerts[ERROR_XFSL_FSEL].TextCast, NULL, NULL, NULL, 1);
	}
	
	return back;
}


WORD call_fontsel(WORD handle, FONT_INFO *fontinfo)
{
	WORD back;
	WORD idummy;
	xFSL *fontsel_struct;

	if (get_cookie(0x7846534CL, (unsigned long *) &fontsel_struct))	/* 'xFSL' */
	{
		back = call_xfsl(handle, fontinfo, fontsel_struct);
	} else if ((_AESversion >= 0x400 || appl_find("?AGI") >= 0) &&
			 appl_getinfo(7, &back, &idummy, &idummy, &idummy) && (back & 0x04))
	{
		back = call_magic_fsl(handle, fontinfo);
	} else
	{
		services->f_alert(alerts[NO_FSEL].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}

	return back;
}
