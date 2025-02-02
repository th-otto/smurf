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

/*---------------------------------------------------------	*/
/* -Save & Load Configuration 								*/
/* -Set Environment Config									*/
/*	f�r SMURF Bildkonverter, 								*/
/*	Copyright Therapy Seriouz Software						*/
/*---------------------------------------------------------	*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "import.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurf.h"
#include "globdefs.h"
#include "smurfine.h"

#include "smurfobs.h"
#include "ext_obs.h"
#include "debug.h"

#define PLOADINFO	0x500c

#define CNFVERSION  0x04

struct ploadinfo
{
	short fnamelen;
	char *cmdlin;
	char *fname;
};

#define	DEBUGGER	1

static void check_and_create(char *chpath);
static unsigned short InqMagX(void);
static void inquire_clipboard(void);


short load_config(void)
{
	char path[SM_PATH_MAX];
	char *buf;
	char help[SM_PATH_MAX];

	DEBUG_MSG(("Load smurf.cnf...\n"));

	strcpy(path, Sys_info.home_path);
	strncat(path, "\\smurf.cnf", 11);

	buf = fload(path, 0);

	if (buf != NULL)
	{
		if (*(uint16_t *) buf != CNFVERSION)
		{
			f_alert("Falsche Version der Konfigurationsdatei gefunden! Aus Sicherheitsgr�nden wird diese nicht eingeladen.",
				 NULL, NULL, NULL, 1); /* FIXME: translate */
			SMfree(buf);
			return -1;
		}

		if (*(uint32_t *) (buf + 2 + sizeof(SYSTEM_INFO) + sizeof(DISPLAY_MODES)) != 0x434e4621L)	/* 'CNF!' */
		{
			f_alert("Keine Smurf-Konfigurationsdatei oder Datei defekt! Datei wird nicht geladen.", NULL, NULL, NULL, 1);	/* FIXME: translate */
			SMfree(buf);
			DEBUG_MSG(("Load smurf.cnf...Ende -1, 1\n"));
			return -1;
		}

		/* Zeiger retten, da er �berschrieben wird */
		strcpy(help, Sys_info.home_path);

		memcpy(&Sys_info, buf + 2, sizeof(SYSTEM_INFO));
		memcpy(&Display_Opt, buf + 2 + sizeof(SYSTEM_INFO), sizeof(DISPLAY_MODES));
		strcpy(Sys_info.standard_path, stpath);	/* �berschriebenen String wieder restaurieren */

		/* Zeiger restaurieren */
		strcpy(Sys_info.home_path, help);

		SMfree(buf);

		DEBUG_MSG(("Load smurf.cnf...Ende\n"));

		return 0;
	}

	SMfree(buf);
	DEBUG_MSG(("Load smurf.cnf...Ende -1, 2\n"));
	return -1;
}


void save_config(void)
{
	char path[128];
	int filehandle;
	int16_t ver;
	int32_t fcback, len;

	DEBUG_MSG(("Save smurf.cnf...\n"));

	Dialog.busy.reset(128, "speichere SMURF.CNF");

	graf_mouse(BUSYBEE, NULL);

	strcpy(path, Sys_info.home_path);
	strncat(path, "\\smurf.cnf", 11);

	if ((fcback = Fcreate(path, 0)) >= 0)
	{
		filehandle = (int) fcback;
		ver = CNFVERSION;
		if (Fwrite(filehandle, 2, &ver) != 2)
			Dialog.winAlert.openAlert("Fehler beim Schreiben der Konfigurationsdatei! M�glicherweise ist das Laufwerk voll.", NULL, NULL, NULL, 1); /* FUXME: translate */
		len = sizeof(SYSTEM_INFO);
		if (Fwrite(filehandle, len, &Sys_info) != len)
			Dialog.winAlert.openAlert("Fehler beim Schreiben der Konfigurationsdatei! M�glicherweise ist das Laufwerk voll.", NULL, NULL, NULL, 1); /* FUXME: translate */
		len = sizeof(DISPLAY_MODES);
		if (Fwrite(filehandle, len, &Display_Opt) != len)
			Dialog.winAlert.openAlert("Fehler beim Schreiben der Konfigurationsdatei! M�glicherweise ist das Laufwerk voll.", NULL, NULL, NULL, 1); /* FUXME: translate */
		Fwrite(filehandle, 4, "CNF!");
		Fclose(filehandle);
	} else
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[CNF_OPENERR].TextCast, NULL, NULL, NULL, 1);
		Dialog.busy.reset(128, "Fehler");
	}

	graf_mouse(ARROW, NULL);

	Dialog.busy.ok();
}


/*-------------------------- System analysieren ----------------------*/
void f_analyze_system(void)
{
	char gi = 0;
	WORD out1, out2, out3, out4, ok;
	unsigned long value;
	unsigned long dummy;

	Sys_info.OS = 0;

	/*-------------- OS feststellen -----------------*/
	if (get_cookie(0x4D616758L, &dummy) || get_cookie(0x4D674D63L, &dummy) || get_cookie(0x4D675043L, &dummy)
		|| get_cookie(0x4D674D78L, &dummy))
		Sys_info.OS |= MATSCHIG;

	if (get_cookie(0x4D675043L, &dummy))	/* 'MgPC' MagiC PC nochmal extra */
		Sys_info.OS |= MAG_PC;

	if (get_cookie(0x4D694E54L, &dummy))	/* 'MiNT' */
		Sys_info.OS |= MINT;

	if (get_cookie(0x6E414553L, &dummy))	/* 'nAES' */
		Sys_info.OS |= NAES;

	if (get_cookie(0x4d544f53L, &dummy))	/* 'MTOS' */
		Sys_info.OS |= MTOS;

	if (get_cookie(0x57494E58L, &dummy))	/* 'WINX' */
		Sys_info.OS |= WINX;

	Sys_info.OSFeatures = 0;

	/* GETINFO */
	if (_AESversion >= 0x400 || appl_find("?AGI") >= 0)
	{
		Sys_info.OSFeatures |= GETINFO;
		gi = 1;
	}

	/* CICONBLK */
	if (gi)
	{
		ok = appl_getinfo(2, &out1, &out2, &out3, &out4);
		if (ok && out3 == 1)
			Sys_info.OSFeatures |= COLICONS;
	}

	if (!gi || !ok)
	{
		if (Sys_info.AES_version >= 0x340)
			Sys_info.OSFeatures |= COLICONS;
	}

	/* WINDOW - BEVENT */
	if (gi)
	{
		ok = appl_getinfo(11, &out1, &out2, &out3, &out4);
		if (ok && out1 & 32)
			Sys_info.OSFeatures |= BEVT;
	}

	if (!gi || !ok)
	{
		if (Sys_info.AES_version >= 0x331)
			Sys_info.OSFeatures |= BEVT;
	}

	/* MENU_POPUP */
	if (gi)
	{
		ok = appl_getinfo(9, &out1, &out2, &out3, &out4);
		if (ok && out2 == 1)
			Sys_info.OSFeatures |= MPOPUP;
	}

	if (!gi || !ok)
	{
		if (Sys_info.AES_version >= 0x331 && !(Sys_info.OS & MATSCHIG))
			Sys_info.OSFeatures |= MPOPUP;
	}

	/* 3D-AES */
	if (gi)
	{
		ok = appl_getinfo(13, &out1, &out2, &out3, &out4);
		if (ok && out1 == 1)
			Sys_info.OSFeatures |= AES3D;
	}

	if (!gi || !ok)
	{
		if (Sys_info.AES_version >= 0x331 && !(Sys_info.OS & MATSCHIG))
			Sys_info.OSFeatures |= AES3D;
	}

	/* PEXEC(10x) */
	if ((Sys_info.OS & MINT) || (InqMagX() >= 0x0500 && _AESnumapps != 1))
		Sys_info.OSFeatures |= PEXEC10x;

	if (gi)
	{
		ok = appl_getinfo(13, &out1, &out2, &out3, &out4);
		if (ok && out2 == 1)
		{
			Sys_info.OSFeatures |= OSYSVAR;
			objc_sysvar(0, 5, 0, 0, &out1, &out2);	/* AES-Hintergrund */
			Sys_info.AES_bgcolor = out1;
		}
	}

	if (!gi || !ok)
		Sys_info.AES_bgcolor = G_LWHITE;

	if (Sys_info.bitplanes == 1 || !(Sys_info.OSFeatures & AES3D))
		Sys_info.AES_bgcolor = G_WHITE;


	/* Clipboardpfad ermitteln und ggf. setzen */
	inquire_clipboard();


	/* CPU-Typ ermitteln */
	get_cookie(0x5F435055L, &value);	/* '_CPU' */
	if (value == 0 || value == 10)
		global_services.CPU_type = MC68000;
	else if (value == 20)
		global_services.CPU_type = MC68020;
	else if (value == 30)
		global_services.CPU_type = MC68030;
	else if (value == 40)
		global_services.CPU_type = MC68040;
	else if (value == 60)
		global_services.CPU_type = MC68060;

	get_cookie(0x5F465055L, &value);	/* '_FPU' */
	if ((value >> 16) != 0)
		global_services.CPU_type |= FPU;

	if (get_cookie(0x5f43465fL, &value))
		global_services.CPU_type |= MCOLDFIRE | FPU;

	DEBUG_MSG(("--> Systeminfos\n"));
	DEBUG_MSG(("    OS         : %#x\n", Sys_info.OS));
	DEBUG_MSG(("    Features   : %#x\n", Sys_info.OSFeatures));
	DEBUG_MSG(("    AES_bgcolor: %#x\n", Sys_info.AES_bgcolor));
	DEBUG_MSG(("    CPU        : %#x\n", global_services.CPU_type));
}


static unsigned short InqMagX(void)
{
	MAGX_COOKIE *cv;

	if (get_cookie(0x4D616758L, (unsigned long *) &cv) != 0)	/* 'MagX' */
		if (cv->aesvars)
			return cv->aesvars->version;
		else
			return FALSE;
	else
		return FALSE;
}



/* inquire_clipboard --------------------------------------------------------
	Clipboardpfad ermitteln und ggf. setzen. Es wird ggfs. ein Clipboardordner
	C:\\CLIPBRD erzeugt.
	------------------------------------------------------------------------*/
static void inquire_clipboard(void)
{
	char *env_path;
	char chpath[SM_PATH_MAX];
	WORD back;

	Sys_info.scrp_path = calloc(1, SM_PATH_MAX);

	back = scrp_read(Sys_info.scrp_path);
#if 0
	printf("back: %d\n", back);
	printf("scrp_path: %s\n", Sys_info.scrp_path);
#endif

	if (back != 0 && *Sys_info.scrp_path != '\0')
	{
#if 0
		printf("scrp_read hat geklappt\n");
#endif

		check_and_create(chpath);
	} else
	{
#if 0
		printf("scrp_read hat nicht geklappt\n");
#endif

		if ((env_path = getenv("CLIPBRD")) != NULL)
			strcpy(Sys_info.scrp_path, env_path);
		else if ((env_path = getenv("SCRAPDIR")) != NULL)
			strcpy(Sys_info.scrp_path, env_path);
#if 0
		printf("getenv()\n");

		if (env_path != NULL)
			printf("env_path: %s\n", env_path);
		else
			printf("env_path: NULL\n");
#endif
		if (env_path == NULL)
			strcpy(Sys_info.scrp_path, "c:\\clipbrd");

		check_and_create(chpath);
	}

	if (Sys_info.scrp_path != NULL)
	{
		if (Sys_info.scrp_path[strlen(Sys_info.scrp_path) - 1] != '\\')
			strcat(Sys_info.scrp_path, "\\");

		/*
		 * Clipboardexporter testen 
		 */
		strcpy(chpath, Sys_info.standard_path);
		strcat(chpath, "\\modules\\export\\clipbrd.sxm");
		if (Fattrib(chpath, 0, 0) < 0)
		{
			free(Sys_info.scrp_path);
			Sys_info.scrp_path = NULL;
		}
	}
#if 0
	if (Sys_info.scrp_path != NULL)
		printf("scrp_path: %s\n", Sys_info.scrp_path);
	else
		printf("scrp_path: NULL\n");

	(void)Cnecin();
#endif
}


/* pr�ft ob der ermittelte Clipboardpfad tats�chlich vorhanden ist */
/* und versucht einen anzulegen wenn er nicht vorhanden ist */
static void check_and_create(char *chpath)
{
#if 0
	if (Sys_info.scrp_path != NULL)
		printf("scrp_path: %s\n", Sys_info.scrp_path);
	else
		printf("scrp_path: NULL\n");

	(void)Cnecin();
#endif

	if (Sys_info.scrp_path[strlen(Sys_info.scrp_path) - 1] == '\\')
		Sys_info.scrp_path[strlen(Sys_info.scrp_path) - 1] = '\0';

	strcpy(chpath, Sys_info.scrp_path);

	if (Fattrib(chpath, 0, 0) < 0)		/* Pfad nicht gefunden */
	{
		if (Dcreate(Sys_info.scrp_path) < 0)
		{
			free(Sys_info.scrp_path);
			Sys_info.scrp_path = NULL;
			return;
		} else
		{
			strcat(Sys_info.scrp_path, "\\");
			scrp_write(Sys_info.scrp_path);
		}
	}
}


/* gibt im Fehlerfall 0 zur�ck um returnwertkompatibel */
/* zu shel_read() zu sein (s.o.) */
static short getploadinfo(char *cmdlin, char *fname)
{
	int handle;
	long ret;
	struct ploadinfo pl;

	if ((handle = (int) Fopen("u:\\proc\\x.-1", FO_READ)) >= 0)
	{
		pl.fnamelen = SM_PATH_MAX + 1;
		pl.cmdlin = cmdlin;
		pl.fname = fname;

		ret = Fcntl(handle, (long) &pl, PLOADINFO);

		Fclose(handle);

		if (ret == 0)
			return 1;
	}
	return 0;
}


/* Holt sich das Smurflaufwerk */
/* noch einzubauen ist der Test mit Fcntl(... PLOADINFO) */
/* auf die eigene Proze�datei */
void GetSMPath(void)
{
	char *sh_rptail;
	char *sh_rpcmd;
	char *path;
	short drivenum;
	short back;

	stpath = calloc(1, SM_PATH_MAX + 1);
	sh_rpcmd = calloc(1, SM_PATH_MAX + 1);
	sh_rptail = calloc(1, SM_PATH_MAX + 1);

	if ((back = getploadinfo(sh_rptail, sh_rpcmd)) == 0)
		back = shel_read(sh_rpcmd, sh_rptail);
#if 0
	printf("back: %d\n", back);
	printf("sh_rpcmd: %s\n", sh_rpcmd);
	printf("sh_rptail: %s\n", sh_rptail);
#endif
	if (back != 0 && sh_rpcmd[0] != 0 && sh_rpcmd[1] == ':')	/* hat das geklappt ... */
	{									/* Pfad enthalten */
		if ((back = strsrchr(sh_rpcmd, '\\')) != -1)
		{
			sh_rpcmd[back] = '\0';
			strcpy(stpath, sh_rpcmd);
		}
	} else								/* oder nicht (z.B. TOS 1.0) */
	{
		memset(stpath, 0, SM_PATH_MAX + 1);
		if ((drivenum = Dgetdrv()) <= 25)	/* drivenum hier: A=0, B=1 ... */
			stpath[0] = drivenum + 'A';	/* alte Laufwerke A - Z, Laufwerksbuchstabe ermitteln */
		else
			stpath[0] = drivenum + '0';	/* neue Laufwerke 1 - 6, Laufwerksbuchstabe ermitteln */

		drivenum++;						/* von A=0 auf A=1 */

		stpath[1] = ':';

		Dgetpath(sh_rpcmd, drivenum);	/* drivenum hier: A=1, B=2 ... */

		if (sh_rpcmd[0] != '\\')
			stpath[2] = '\\';

		strcat(stpath, sh_rpcmd);
	}

	/* ***************Nur f�r den Debugger: ******************* */
#if DEBUGGER
	if (stricmp(stpath, "C:\\PC") == 0)
		strcpy(stpath, "H:\\smurf\\dist");
#endif

#if 0
	printf("stpath: %s\n", stpath);
#endif

	strcpy(Sys_info.standard_path, stpath);

#if 0
	printf("Sys_info.standard_path: %s\n", Sys_info.standard_path);
#endif

	/*
	 *  Pfad des $HOME-Verzeichnis auslesen
	 */
	path = getenv("HOME");

	if (path == NULL)
		strcpy(Sys_info.home_path, Sys_info.standard_path);
	else
	{
		strcpy(Sys_info.home_path, path);

		if (Sys_info.home_path[strlen(Sys_info.home_path) - 1] == '\\')
			Sys_info.home_path[strlen(Sys_info.home_path) - 1] = '\0';

		/* mit einem Trick testen, ob der Pfad vorhanden ist ist */
		/* Ergebnis < 0 wenn die Datei noch nicht vorhanden ist */
		strcpy(sh_rptail, Sys_info.home_path);
		if (Fattrib(sh_rptail, 0, 0) >= 0)	/* Pfad gefunden */
		{
			strcat(sh_rptail, "\\defaults");
			if (Fattrib(sh_rptail, 0, 0) >= 0)	/* Pfad gefunden */
				strcat(Sys_info.home_path, "\\defaults");
		} else
			strcpy(Sys_info.home_path, Sys_info.standard_path);
	}

	free(sh_rpcmd);
	free(sh_rptail);
}
