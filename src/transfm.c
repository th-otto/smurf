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

/*-----------------------------------------------------------------	*/
/*							TRANSFM.C								*/
/*	Dialog "Bild wandeln" zum destruktiven expandieren oder redu-	*/
/*	zieren eines Bildes. Benutzt werden die CONVERT.C-Routinen		*/
/*	�ber eine MOD_ABILITY-Struktur.									*/
/*		05.06.1997 Olaf												*/
/*-----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "import.h"
#include "smurfine.h"
#include "globdefs.h"
#include "popdefin.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurf.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "ext_rsc.h"


/*------------ globales f�r die Konvertierung ------*/
static short conv_depth = 24;
static short conv_dither = DIT1;
static short conv_pal = CR_SYSPAL;

static WORD tfm_fix_red[SM_PALETTE_MAX];
static WORD tfm_fix_blue[SM_PALETTE_MAX];
static WORD tfm_fix_green[SM_PALETTE_MAX];



/**************************************************************	*/
/*			Aktualisieren der Popups im Wandeln-Dialog			*/
/**************************************************************	*/
static void actualize_convpopups(short dither_algo, short depth, short pal)
{
	WORD mode_pal;
	WORD mode_dit;
	WORD mode_load;
	OBJECT *res;

	res = wind_s[WIND_TRANSFORM].resource_form;

	if (ditmod_info[dither_algo - 1]->pal_mode == FIXPAL)
	{
		conv_pal = CR_FIXPAL;
		strncpy(res[CONV_PALPB].TextCast, colred_popup[CR_FIXPAL].TextCast, 15);
		mode_pal = OS_DISABLED;
		mode_dit = OS_ENABLED;
		mode_load = OS_DISABLED;
	}

	if (depth > smurf_picture[active_pic]->depth || depth == 16 || depth == 24)
	{
		strcpy(res[CONV_PALPB].TextCast, "-");
		strcpy(res[CONV_DITHPB].TextCast, "-");
		mode_pal = OS_DISABLED;
		mode_dit = OS_DISABLED;
		mode_load = OS_DISABLED;
	} else if (depth <= smurf_picture[active_pic]->depth)
	{
		strcpy(res[CONV_DITHPB].TextCast, col_pop[dither_algo].TextCast);
		mode_dit = OS_ENABLED;

		if (ditmod_info[dither_algo - 1]->pal_mode != FIXPAL)
		{
			strcpy(res[CONV_PALPB].TextCast, colred_popup[pal].TextCast);
			mode_pal = OS_ENABLED;
		}

		if (pal == CR_FILEPAL)
			mode_load = OS_ENABLED;
		else
			mode_load = OS_DISABLED;
	}

	if (depth == 1)
	{
		strcpy(res[CONV_PALPB].TextCast, "s/w");
		mode_pal = OS_DISABLED;
	}

	change_object(&wind_s[WIND_TRANSFORM], CONV_PALPB, mode_pal, 1);
	change_object(&wind_s[WIND_TRANSFORM], CONV_PALCB, mode_pal, 1);
	change_object(&wind_s[WIND_TRANSFORM], CONV_DITHPB, mode_dit, 1);
	change_object(&wind_s[WIND_TRANSFORM], CONV_DITHCB, mode_dit, 1);
	change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, mode_load, 1);
}


/**************************************************************	*/
/*		Ditherdispatcher f�r destruktives Dithern 				*/
/*	Der einzige Teil von Smurf, der nicht konstruktiv ist ...	*/
/**************************************************************	*/
static short dither_destruktiv(short dest_depth, short dest_dither, short dest_pal)
{
	uint8_t *dest_palette;
	uint8_t dest_form;
	short t;
	SMURF_PIC *convpic;
	MOD_ABILITY old_export_modabs;
	DISPLAY_MODES new_display;
	SYSTEM_INFO Cheat_sysinfo;

	convpic = smurf_picture[active_pic];

	if (dest_depth < 8)
		dest_form = FORM_STANDARD;
	else
		dest_form = FORM_PIXELPAK;

	/*----------- neue Strukturen zum Dithern generieren bzw. alte retten -----------*/
	memcpy(&old_export_modabs, &export_mod_ability, sizeof(MOD_ABILITY));
	memcpy(&new_display, &Display_Opt, sizeof(DISPLAY_MODES));
	memcpy(&Cheat_sysinfo, &Sys_info, sizeof(SYSTEM_INFO));

	/*----- sind wir in einer anderen Farbtiefe (mu� eine andere NCT geladen werden)? -----*/
	if (dest_pal == CR_SYSPAL && dest_depth != Sys_info.bitplanes)
	{
		if (loadNCT(dest_depth, &Cheat_sysinfo) != 0)
		{
			/* FIXME: translate */
			Dialog.winAlert.openAlert("Kein NCT-File f�r die gew�hlte Farbtiefe gefunden. Dithering nicht m�glich.", NULL, NULL, NULL, 1);
			return -1;
		}
	}

	/*---------------- neue Strukturen zurechtbeschei�en -----*/
	Cheat_sysinfo.bitplanes = dest_depth;
	Cheat_sysinfo.Max_col = (1 << dest_depth) - 1;

	memset(&export_mod_ability, 0x0, sizeof(MOD_ABILITY));
	export_mod_ability.depth1 = dest_depth;
	export_mod_ability.form1 = dest_form;

	exp_conf.exp_depth = dest_depth;
	exp_conf.exp_form = dest_form;

	new_display.syspal_4 = dest_pal;
	new_display.syspal_8 = dest_pal;
	new_display.syspal_24 = dest_pal;
	new_display.dither_4 = dest_dither;
	new_display.dither_8 = dest_dither;
	new_display.dither_24 = dest_dither;

	/* destruktiv dithern */
	export_dither_dispatcher(convpic, &Cheat_sysinfo, &new_display, tfm_fix_red, tfm_fix_green, tfm_fix_blue);

	/*---------------- Bilddaten aus dem MFDB �bertragen */
	SMfree(convpic->pic_data);
	convpic->pic_data = convpic->screen_pic->fd_addr;
	convpic->depth = Cheat_sysinfo.bitplanes;
	convpic->format_type = dest_form;

	free(convpic->screen_pic);			/* MFDB freigeben, um das Dithering */
	convpic->screen_pic = NULL;			/* problemlos zu erm�glichen */

	/*---------------- Palette �bertragen */
	dest_palette = convpic->palette;
	memset(dest_palette, 0, SM_PALETTE_SIZE);
	for (t = 0; t < SM_PALETTE_MAX; t++)
	{
		*dest_palette++ = convpic->red[t];
		*dest_palette++ = convpic->grn[t];
		*dest_palette++ = convpic->blu[t];
	}

	/*---------------- und jetzt noch konvertieren, dann sind wir fertig. */
	/* Wird wohl in Zukunft wegfallen k�nnen */
	/* f_convert(convpic, mod_abs, dest_colsys, SAME, 0); */

	/*---------- alles aus der Cheat_sysinfo wegwerfen, -----*/
	/*---------- was in der load_nct angefordert wurde ------*/
	if (dest_pal == CR_SYSPAL && dest_depth != Sys_info.bitplanes)
	{
		free(Cheat_sysinfo.red);
		free(Cheat_sysinfo.grn);
		free(Cheat_sysinfo.blu);
		free(Cheat_sysinfo.plane_table);
		SMfree(Cheat_sysinfo.nc_table);
	}

	/* und jetzt Screendisplay neu dithern. */
	if (!Sys_info.realtime_dither)
		f_dither(convpic, &Sys_info, 0, NULL, &Display_Opt);

	Window.redraw(&picture_windows[active_pic], NULL, 0, 0);

	Dialog.busy.ok();

	return 0;
}



/**************************************************************	*/
/*						Bild wandeln							*/
/**************************************************************	*/
static void do_transform(short conv_depth, short conv_dither, short conv_pal)
{
	short picdepth;
	MOD_ABILITY abs;

	memset(&abs, 0, sizeof(MOD_ABILITY));	/* l�schen... */

	abs.depth1 = conv_depth;

	if (conv_depth < 8)
		abs.form1 = FORM_STANDARD;
	else
		abs.form1 = FORM_PIXELPAK;

	picdepth = smurf_picture[active_pic]->depth;

	/*
	 * 24 nach 16 Bit
	 */
	if (picdepth == 24 && conv_depth == 16)
		tfm_24_to_16(smurf_picture[active_pic], SAME);
	else
		/* raufw�rts ... */
	if (conv_depth > picdepth || (conv_depth == 24 && picdepth == 24))
		f_convert(smurf_picture[active_pic], &abs, RGB, SAME, 0);
	else
		/* ... und runterw�rts */
		dither_destruktiv(conv_depth, conv_dither, conv_pal);
}


static short autoreduce_image(void)
{
	uint8_t *rt, *gt, *bt;
	uint8_t *data;
	uint8_t R, G, B;
	uint16_t idx;
	uint16_t *intdata;
	short depth;
	BOOLEAN found = TRUE;
	short ddepth;
	long pixlen;
	long t;
	long colcount = 0;
	long find = 0;
	SMURF_PIC *convpic;

	/* zu aller erst mal die Farben im Bild z�hlen */
	rt = SMalloc(32769L * 3 * sizeof(*rt));
	if (rt == NULL)
		return M_MEMORY;
	gt = rt + 32769L;
	bt = gt + 32769L;

	Dialog.busy.reset(0, "z�hle Farben...");

	convpic = smurf_picture[active_pic];
	pixlen = (long) convpic->pic_width * (long) convpic->pic_height;
	depth = convpic->depth;
	data = convpic->pic_data;
	intdata = (uint16_t *) data;

	for (t = 0; t < pixlen; t++)
	{
		if (!(t & 2047))
			Dialog.busy.draw((short) ((long) t * 128L / pixlen));
		if (convpic->format_type == FORM_PIXELPAK)
		{
			/*--------- RGB-Werte holen----- */
			switch (depth)
			{
			case 24:
				R = *data++;
				G = *data++;
				B = *data++;
				break;

			case 16:
				idx = *intdata++;
				R = (idx >> 11) & 31;
				G = (idx >> 6) & 31;
				B = idx & 31;
				break;

			case 8:
				idx = *data++;
				R = *(convpic->palette + idx * 3 + 0);
				G = *(convpic->palette + idx * 3 + 1);
				B = *(convpic->palette + idx * 3 + 2);
				break;
			
			default:
				return 0; /* should not happen */
			}

			/*----------- Farbe in der Tabelle suchen */
			found = FALSE;
			find = colcount;
			while (find--)
			{
				if (rt[find] == R && gt[find] == G && bt[find] == B)
				{
					found = TRUE;
					break;
				}
			}

			if (found == FALSE)				/* NICHT gefunden? -> Eintragen und Farbz�hler erh�hen */
			{
				colcount++;
				rt[colcount] = R;
				gt[colcount] = G;
				bt[colcount] = B;
			}
		}

		if (colcount > 32768L)
			break;
	}

	/* Such-Tabellen freigeben */
	SMfree(rt);

	/* Bild reduzieren */
	if (colcount > 32768L)
		return 0;
	else if (colcount <= 2)
		ddepth = 1;
	else if (colcount <= 4)
		ddepth = 2;
	else if (colcount <= 16)
		ddepth = 4;
	else if (colcount <= 256)
		ddepth = 8;
	else if (colcount <= 32768L)
	{
		if (smurf_picture[active_pic]->depth == 24)
		{
			/* FIXME: translate */
			if (Dialog.winAlert.openAlert("Das Bild kann nach 16 Bit Farbtiefe gewandelt werden. Folge kann aber ein Qualit�tsverlust sein!", "Abbruch", " OK ", NULL, 1) == 2)
				tfm_24_to_16(smurf_picture[active_pic], NEW);
		}
		return 0;
	}

	if (ddepth != depth)
		dither_destruktiv(ddepth, DIT1, CR_MEDIAN);

	return 0;
}


void transform_pic(void)
{
	static char tfm_filepal_name[32] = "feste Palette"; /* FIXME: translate */
	char *name;
	char str[4];
	char *pal_loadpath;
	WORD button;
	WORD popbut;
	WORD enablemode;
	WORD dbutton;
	WORD back;
	OBJECT *resource;

	button = Dialog.init(WIND_TRANSFORM, CONV_START);

	resource = wind_s[WIND_TRANSFORM].resource_form;

	if (openmode == 0 || openmode == 2)
	{
		conv_depth = smurf_picture[active_pic]->depth;
		switch (conv_depth)
		{
		case 1:
			dbutton = DEPTH1;
			break;
		case 2:
			dbutton = DEPTH2;
			break;
		case 3:
			dbutton = DEPTH4;
			break;
		case 4:
			dbutton = DEPTH4;
			break;
		case 5:
			dbutton = DEPTH8;
			break;
		case 6:
			dbutton = DEPTH8;
			break;
		case 7:
			dbutton = DEPTH8;
			break;
		case 8:
			dbutton = DEPTH8;
			break;
		case 9:
			dbutton = DEPTH16;
			break;
		case 16:
			dbutton = DEPTH16;
			break;
		case 24:
			dbutton = DEPTH24;
			break;
		default:
			return;
		}

		popups[POPUP_CONVDEPTH].item = dbutton;
		strcpy(resource[CONV_DEPTHPB].TextCast, form_pop[dbutton].TextCast);
		change_object(&wind_s[WIND_TRANSFORM], CONV_DEPTHPB, OS_UNSEL, 1);
		actualize_convpopups(conv_dither, conv_depth, conv_pal);

		if (dbutton == DEPTH1)
			conv_depth = 1;
		else if (dbutton == DEPTH2)
			conv_depth = 2;
		else if (dbutton == DEPTH4)
			conv_depth = 4;
		else if (dbutton == DEPTH8)
			conv_depth = 8;
		else if (dbutton == DEPTH16)
			conv_depth = 16;
		else if (dbutton == DEPTH24)
			conv_depth = 24;
	}

	switch (button)
	{
	case CONV_DEPTHPB:
	case CONV_DEPTHCB:
		popbut = f_pop(&popups[POPUP_CONVDEPTH], 0, button, NULL);
		f_deselect_popup(&wind_s[WIND_TRANSFORM], CONV_DEPTHCB, CONV_DEPTHPB);

		switch (popbut)
		{
		case DEPTH1:
			conv_depth = 1;
			break;
		case DEPTH2:
			conv_depth = 2;
			break;
		case DEPTH4:
			conv_depth = 4;
			break;
		case DEPTH8:
			conv_depth = 8;
			break;
		case DEPTH16:
			conv_depth = 16;
			break;
		case DEPTH24:
			conv_depth = 24;
			break;
		default:
			return;
		}

		actualize_convpopups(conv_dither, conv_depth, conv_pal);

		if (conv_dither > 0 && conv_depth <= smurf_picture[active_pic]->depth && conv_depth != 16 && conv_depth != 24)
		{
			if (ditmod_info[conv_dither - 1]->pal_mode == FIXPAL)
			{
				conv_pal = CR_FIXPAL;
				popups[POPUP_CONVPAL].item = conv_pal;
				strncpy(resource[CONV_PALPB].TextCast, colred_popup[conv_pal].TextCast, 15);
				enablemode = OS_DISABLED;
				change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);
			} else
			{
				if (conv_depth == 1)
				{
					strcpy(resource[CONV_PALPB].TextCast, "s/w");
					enablemode = OS_DISABLED;
					change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);
				} else
				{
					conv_pal = CR_SYSPAL;
					popups[POPUP_CONVPAL].item = conv_pal;
					strncpy(resource[CONV_PALPB].TextCast, colred_popup[conv_pal].TextCast, 15);
					enablemode = OS_ENABLED;
					change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);
				}
			}

			change_object(&wind_s[WIND_TRANSFORM], CONV_PALPB, enablemode, 1);
			change_object(&wind_s[WIND_TRANSFORM], CONV_PALCB, enablemode, 1);
		}

		break;

	case CONV_DITHPB:
	case CONV_DITHCB:
		popbut = f_pop(&popups[POPUP_CONVDITHER], 0, button, NULL);
		f_deselect_popup(&wind_s[WIND_TRANSFORM], CONV_DITHCB, CONV_DITHPB);

		if (popbut > 0)
		{
			conv_dither = popbut;

			if (ditmod_info[conv_dither - 1]->pal_mode == FIXPAL)
			{
				conv_pal = CR_FIXPAL;
				strncpy(resource[CONV_PALPB].TextCast, colred_popup[conv_pal].TextCast, 15);
				enablemode = OS_DISABLED;
				change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);
			} else
			{
				conv_pal = CR_SYSPAL;
				popups[POPUP_CONVPAL].item = CR_SYSPAL;
				strncpy(resource[CONV_PALPB].TextCast, colred_popup[conv_pal].TextCast, 15);
				enablemode = OS_ENABLED;
				change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);
			}

			if (conv_depth == 1)
			{
				strcpy(resource[CONV_PALPB].TextCast, "s/w");
				enablemode = OS_DISABLED;
				change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);
			}

			change_object(&wind_s[WIND_TRANSFORM], CONV_PALPB, enablemode, 1);
			change_object(&wind_s[WIND_TRANSFORM], CONV_PALCB, enablemode, 1);
		}

		break;

	case CONV_PALPB:
	case CONV_PALCB:
		strcpy(colred_popup[CR_FILEPAL].TextCast, tfm_filepal_name);
		popbut = f_pop(&popups[POPUP_CONVPAL], 0, button, NULL);
		if (popbut > 0)
			conv_pal = popbut;

		if (conv_pal == CR_FILEPAL)
			change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_ENABLED, 1);
		else
			change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_DISABLED, 1);

		f_deselect_popup(&wind_s[WIND_TRANSFORM], CONV_PALCB, CONV_PALPB);
		break;

	case CONV_START:
		change_object(&wind_s[WIND_TRANSFORM], CONV_START, OS_UNSEL, 1);
		Dialog.close(WIND_TRANSFORM);
		do_transform(conv_depth, conv_dither, conv_pal);
		f_pic_changed(&picture_windows[active_pic], 1);
		break;

		/*--------------------- feste Palette laden -----------------------*/
	case TFM_LOADPAL:
		pal_loadpath =
			load_palfile(Sys_info.standard_path, tfm_fix_red, tfm_fix_green, tfm_fix_blue, (1 << conv_depth));

		if (pal_loadpath != NULL)
		{
			strcpy(tfm_filepal_name, strrchr(pal_loadpath, '\\') + 1);
			strcpy(colred_popup[CR_FILEPAL].TextCast, tfm_filepal_name);
			strcpy(resource[CONV_PALPB].TextCast, tfm_filepal_name);
		}

		change_object(&wind_s[WIND_TRANSFORM], CONV_PALPB, OS_UNSEL, 1);
		change_object(&wind_s[WIND_TRANSFORM], TFM_LOADPAL, OS_UNSEL, 1);
		break;


	case TRANSFM_REDUCE:
		change_object(&wind_s[WIND_TRANSFORM], TRANSFM_REDUCE, OS_UNSEL, 1);
		Dialog.close(WIND_TRANSFORM);
		back = autoreduce_image();
		if (back == M_MEMORY)
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[DIT_NOMEM].TextCast, NULL, NULL, NULL, 1);
		break;

	default:				/*----------------- Name und Farbtiefe des Bildes einf�gen --------*/
		name = resource[CONV_PICNAME].TextCast;
		strncpy(name, picture_windows[active_pic].wtitle + 12, 24);
		name[23] = 0;
		itoa(smurf_picture[active_pic]->depth, str, 10);
		strcpy(resource[CONV_SRCDEPTH].TextCast, str);
		Window.redraw(&wind_s[WIND_TRANSFORM], NULL, CONV_OLDPIC, 0);
		break;
	}
}
