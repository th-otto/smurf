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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "import.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "smurf.h"
#include "globdefs.h"
#include "vaproto.h"
#include "bindings.h"
#include "dit_mod.h"

#include "smurfobs.h"
#include "ext_obs.h"


#define UNLIMITED	0x7fffffffL

/* Klammert einen String mit ' ein und verdoppelt alle im String	*/
/* enthaltenen Quotemarks.											*/
char *quote_arg(char *s)
{
	char *t;
	char *ot;

	t = calloc(1, SM_PATH_MAX + 1);
	ot = t;

	/* eric's file -> 'eric''s file' */
	*t++ = '\'';						/* ganz vorne ein Quote */

	while (*s)
	{
		if (*s == '\'')
			*t++ = '\'';

		*t++ = *s++;
	}

	*t++ = '\'';						/* und ganz hinten ein Quote */
	*t = '\0';

	return ot;
}


/* Entfernt die Anfangs-, die End- und die doppelten Quotemarks (') */
/* aus Strings (Pfadnamen). 										*/
char *unquote_arg(char *s)
{
	char *t;
	char *os;

	os = s;

	/* 'eric''s file' -> eric's file */
	t = s;
	if (*s != '\'')
		return s;

	s++;								/* ganz vorne ein Quote weg */

	while (*s)
	{
		if (*s == '\'' && *(s + 1) == '\'')
			s++;

		*t++ = *s++;
	}

	*(t - 1) = '\0';					/* und ganz hinten ein Quote weg */

	return os;
}


/* Die Funktion arbeitet �hnlich der Bibliotheksfunktion strtok() */
/* Sie wird immer mit dem gleichen Zeiger aufgerufen und zur�ckgegeben */
/* wird ein hinter dem Parameter abgeschlossener Zeiger bzw. NULL wenn */
/* kein weiterer Parameter entdeckt werden konnte. */
/* Als Trennzeichen gelten immer Spaces */
char *strargvtok(char *s)
{
	static char *t = NULL;
	char *back;
	char in = 0;

	if (t == NULL)						/* Nur beim Ersten Aufruf der Fall */
		t = s;

	if (*t == '\0')						/* Wenn der String zu Ende tokenisiert wurde, */
	{									/* also nach dem letzten g�ltigen Aufruf. */
		t = NULL;						/* Und das, damit nachfolgende Strings auch noch */
		return NULL;					/* gemacht werden k�nnen. */
	}

	s = t;

/*	strcpy(mike, "'Parameter 2' 'Paramet'' 4' 'Paramet''5'"); */
	if (*s == '\'')						/* n�tig, da Parameter mit einem ' nicht gequotet werden */
	{
		in = 1;
		strcpy(s, s + 1);				/* alles vorziehen */
	}

	while (*s)
	{
		if (*s == ' ')
		{
			if (!in)
			{
				*s++ = '\0';
				break;
			}
		} else if (*s == '\'')
		{
			if (in)
			{
				strcpy(s, s + 1);		/* alles vorziehen */

				if (*s != '\'')
				{
					in = 0;
					if (*s)				/* nur ausf�hren, wenn nicht */
						*s++ = '\0';	/* sowieso schon am Ende */
					break;
				}
			}
		}

		s++;
	}

	back = t;							/* Eingangsstring wegsichern */
	t = s;								/* und neuen Anfang setzen */

	return back;
}


/* So stelle ich mir die Implementierung der Bibliotheksfunktion strtok() vor. */
/* Mit dem Unterschied, da� weitere Aufrufe nicht mit NULL in s aufgerufen */
/* werden mu�, sondern mystrtok() folgende Aufrufe selbst erkennt. */
/* Ein Problem ist allerdings, da� diese Erkennung nur funktioniert, wenn ein */
/* String zuende tokenisiert wurde. Ein vorheriger Abbruch bringt alles durcheinander. */
/* Deshalb gibt es die M�glichkeit des manuellen Inits mit NULL als erstem Parameter. */
char *mystrtok(char *s, char c)
{
	static char *t = NULL;
	char *back;

	if (s == NULL)						/* manueller Init */
	{
		t = NULL;
		return NULL;
	}

	if (t == NULL)						/* Nur am Stringanfang der Fall */
		t = s;

	if (*t == '\0')						/* Wenn der String zu Ende tokenisiert wurde, */
	{									/* also nach dem letzten g�ltigen Aufruf. */
		t = NULL;						/* Und das, damit nachfolgende Strings auch noch */
		return NULL;					/* gemacht werden k�nnen. */
	}

	s = t;

	while (*s)
	{
		if (*s == c)
		{
			*s++ = '\0';
			break;
		} else
			s++;
	}

	back = t;							/* Eingangsstring wegsichern */
	t = s;								/* und neuen Anfang setzen */

	return back;
}


/* Die Funktion sucht von vorne her nach dem ersten Vorkommen des */
/* �bergebenen Zeichen und gibt die Position oder -1 bei nicht gefunden zur�ck. */
int strsrchl(char *s, char c)
{
	int i;

	i = 0;
	while (*s != c && *s != '\0')
	{
		s++;
		i++;
	}

	if (*s == '\0')
		return -1;
	else
		return i;
}



/* Die Funktion sucht von hinten her nach dem ersten Vorkommen des �bergebenen */
/* Zeichens und gibt die Position oder -1 bei nicht gefunden zur�ck. */
int strsrchr(char *s, char c)
{
	int i;

	i = (int) strlen(s) - 1;			/* Index auf letztes Zeichen setzen */
	s += i;
	while (i >= 0 && *s != c)			/* i vor s pr�fen, da *s m�glicherweise ung�ltig! */
	{
		s--;
		i--;
	}

	return i;
}


/* Sucht das Environment nach Tempdirs ab und f�llt es in tmpdir. */
/* Falls keines gefunden wurde, wird -1 zur�ckgegeben. */
void get_tmp_dir(char *tmpdir)
{
	char *result;

	*tmpdir = '\0';
	if ((result = getenv("TMP")) != NULL)
		strcpy(tmpdir, result);
	if ((result = getenv("TEMP")) != NULL)
		strcpy(tmpdir, result);
	if ((result = getenv("TMPDIR")) != NULL)
		strcpy(tmpdir, result);

	if (*tmpdir == '\0')
		strcpy(tmpdir, Sys_info.standard_path);
}


/* Funktion um Cookie auf Anwesenheit zu testen */
/* Christian Eyrich irgendwann im 20. Jahrhundert */
/* Umgestellt um ohne Supervisormodus auszukommen am 22.4.99 */
/* Erweitert um Ssystem(GETCOOKIE) am 2.5.99 */
BOOLEAN get_cookie(unsigned long cookie, unsigned long *value)
{
	unsigned long *cookiejar;


	/* ansonsten value auf 0L */
	*value = 0;

	/* Zeiger auf Cookie Jar holen */
	cookiejar = (unsigned long *) Setexc(0x5a0 / 4, (void (*)()) -1);
	/* Cookie Jar installiert? */
	if (cookiejar)
	{
		/* Keksdose nach cookie durchsuchen */
		while (*cookiejar)
		{
			if (*cookiejar++ == cookie)
			{
				/* wenn cookie gefunden wurde, value auf Cookiewert setzen, */
				*value = *cookiejar;
				return TRUE;
			}
			cookiejar++;
		}
	}
	return FALSE;
}


/* --- Funktion zur Reservierung von Speicher + Kontrolle ------- */
/* --- f�r Mario ;-) 	*/
void *SMalloc(long amount)
{
	char *buffer;

	if ((buffer = (char *) Malloc(amount)) == 0)
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MALLOC_ERR].TextCast, NULL, NULL, NULL, 1);
	else
		memset(buffer, 0x0, amount);

	return buffer;
}


/* --- Funktion zum Freigeben von Speicher + Kontrolle ------- */
int SMfree(void *ptr)
{
	if (ptr == NULL)
		return -1;

	if (Mfree(ptr) != 0)
	{
		Dialog.winAlert.openAlert(Dialog.winAlert.alerts[MFREE_ERR].TextCast, NULL, NULL, NULL, 1);
		return -1;
	}

	return 0;
}

void exported_SMfree(void *ptr)
{
	SMfree(ptr);
}

/*-----------------------------------------------------------------	*/
/* convert_units													*/
/*	Konvertiert Zoll, Millimeter, DTP-Punkt und Pixel unterein- und */
/*	durcheinander in allen Kombinationen mit freier DPI-Zahl.		*/
/*	R�ckgabewert ist ein Konvertierungsfaktor (Multiplikator) f�r	*/
/*	den alten Wert im 64Bit-Flie�kommaformat.						*/
/*-----------------------------------------------------------------	*/
float convert_units(short oldunit, short newunit, float dpi)
{
	float conv_factor = 0;

	if (newunit == UNIT_PIXELS)
	{
		switch (oldunit)
		{
		case UNIT_MM:
			conv_factor = dpi / 25.4;
			break;
		case UNIT_INCH:
			conv_factor = dpi;
			break;
		case UNIT_POINTS:
			conv_factor = dpi / 72.0;
			break;
		}
	} else if (newunit == UNIT_MM)
	{
		switch (oldunit)
		{
		case UNIT_PIXELS:
			conv_factor = 25.4 / dpi;
			break;
		case UNIT_INCH:
			conv_factor = 25.4;
			break;
		case UNIT_POINTS:
			conv_factor = 25.4 / 72.0;
			break;
		}
	} else if (newunit == UNIT_INCH)
	{
		switch (oldunit)
		{
		case UNIT_PIXELS:
			conv_factor = 1.0 / dpi;
			break;
		case UNIT_MM:
			conv_factor = 1.0 / 25.4;
			break;
		case UNIT_POINTS:
			conv_factor = 1.0 / 72.0;
			break;
		}
	} else if (newunit == UNIT_POINTS)
	{
		switch (oldunit)
		{
		case UNIT_PIXELS:
			conv_factor = 72.0 / dpi;
			break;
		case UNIT_MM:
			conv_factor = 72.0 / 25.4;
			break;
		case UNIT_INCH:
			conv_factor = 72.0;
			break;
		}
	}

	return conv_factor;
}


char *load_palfile(char *path, WORD *red, WORD *green, WORD *blue, WORD max_cols)
{
	WORD fsback;
	WORD *palbuf;
	WORD *palcpy;
	WORD max_count;
	WORD t;
	static char pal_loadpath[SM_PATH_MAX];

	strcpy(pal_loadpath, path);
	fsback = f_fsbox(pal_loadpath, "Palette laden", 0); /* FIXME: translate */

	if (fsback != FALSE)
	{
		palbuf = (WORD *) fload(pal_loadpath, 0);
		max_count = (WORD) (f_len / 6);
		if (max_count > max_cols)
		{
			SMfree(palbuf);
			Dialog.winAlert.openAlert(Dialog.winAlert.alerts[PAL_DEPTHERR].TextCast, NULL, NULL, NULL, 1);
			return NULL;
		} else
		{
			palcpy = palbuf;
			/* Mit der ersten Farbe im File die Palette ausnullen */
			for (t = 0; t < SM_PALETTE_MAX; t++)
			{
				red[t] = (WORD) (255L * (long) palcpy[0] / 1000L);
				green[t] = (WORD) (255L * (long) palcpy[1] / 1000L);
				blue[t] = (WORD) (255L * (long) palcpy[2] / 1000L);
			}

			/* und �bertragen */
			for (t = 0; t < max_count; t++)
			{
				red[t] = (WORD) (255L * (long) palcpy[t * 3] / 1000L);
				green[t] = (WORD) (255L * (long) palcpy[t * 3 + 1] / 1000L);
				blue[t] = (WORD) (255L * (long) palcpy[t * 3 + 2] / 1000L);
			}
		}

		SMfree(palbuf);
		return pal_loadpath;
	}

	return NULL;
}


/* get_maxnamelen ------------------------------------------------
	Gibt die maximale L�nge eines Dateinamen im Pfad path zur�ck.
	-------------------------------------------------------------*/
long get_maxnamelen(const char *path)
{
	long Name_Max;

	/* maximale Filenamenl�nge im Pfad erfragen */
	if ((Name_Max = Dpathconf(path, 3)) >= 0)	/* Funktionsergebnis g�ltig */
	{
		if (Name_Max == UNLIMITED)		/* L�nge unbegrenzt */
			Name_Max = NAME_MAX;
	} else
	{
		Name_Max = 12;					/* Funktion nicht vorhanden */
	}

	return Name_Max;
}


/* Konvertiert ein int im BCD-Format in einen String */
/* inklusive Unterdr�ckung einer 0 im ersten Zeichen und */
/* Punkt an der richtigen Stelle */
void BCD2string(char *string, uint16_t bcd)
{
	if ((bcd >> 12) != 0)
		*string++ = '0' + (bcd >> 12);
	*string++ = '0' + ((bcd & 0x0f00) >> 8);
	*string++ = '.';
	*string++ = '0' + ((bcd & 0x00f0) >> 4);
	*string++ = '0' + (bcd & 0x000f);
	*string++ = '\0';
}


/* string right pointer break - Gegenst�ck zum string pointer break */
/* der Standardlib, das String 1 von rechts und nicht von links */
/* her nach dem Auftreten eines Zeichens aus String 2 absucht */
char *strrpbrk(char *s1beg, char *s1, char *s2)
{
	char *os2 = s2;

	do
	{
		s2 = os2;
		while (*s2 && *s1 != *s2)
			s2++;

		if (*s2)
			break;
	} while (s1-- != s1beg);

	if (*s1 && *s1 == *s2)
		return s1;
	else
		return NULL;
}


/* make_singular_display -----------------------------------------------
	rettet die DISPLAY_OPT nach *old und schreibt Dither und Pal als Modi
	in alle Variablen von Display_Opt, damit alle Farbtiefen nur mit diesen
	Modi gedithert werden (z.B. f�r Preview).
	--------------------------------------------------------------------*/
void make_singular_display(DISPLAY_MODES *old, short Dither, short Pal)
{
	memcpy(old, &Display_Opt, sizeof(DISPLAY_MODES));
	Display_Opt.dither_24 = Dither;
	Display_Opt.dither_8 = Dither;
	Display_Opt.dither_4 = Dither;
	Display_Opt.syspal_24 = Pal;
	Display_Opt.syspal_8 = Pal;
	Display_Opt.syspal_4 = Pal;
}


/* restore_display -------------------------------------------------------
	Kopiert die gerettete DISPLAY_MODES *old nach Display_Opt. Zum Restoren von
	make_singular_display.
	----------------------------------------------------------------------*/
void restore_display(DISPLAY_MODES *old)
{
	memcpy(&Display_Opt, old, sizeof(DISPLAY_MODES));
}


/* K�rzt f�r beliebige Strings zu lange Filenamen durch */
/* entfernen von Text in der Mitte und ersetzen durch "...". */
/* Aus "Dies ist ein zu langer Filename.img" w�rde bei K�rzung */
/* auf 27 Zeichen "Dies ist ein...Filename.img" */
char *shorten_name(char *string, short newlen)
{
	static char temp[SM_PATH_MAX + 1];

	/* nichts tun wenn String sowieso passend */
	if ((short)strlen(string) <= newlen)
		return string;

	memset(temp, 0, sizeof(temp));
	strncpy(temp, string, newlen / 2 - 1);	/* auf die H�lfte und eines weniger */
	strcat(temp, "...");				/* L�ckenf�ller rein */
	strcat(temp, string + strlen(string) - (newlen - newlen / 2 - 3));	/* und bis newlen L�nge mit Originalstring auff�llen */

	return temp;
}


/* Lesen des aktuellen Pfad. (drive = 0 f�r Defaultdrive) */

BOOLEAN get_path(char *path, char drive)
{
	short ret;
	short drive_nr;

	if (drive == 0)
	{
		drive = 'A' + Dgetdrv();		/* Aktuelles Laufwerk */
		if (drive > 'Z')
			drive = drive - 'Z' + '0';
	} else
	{
		drive = toupper(drive);
	}
	if (drive >= '1' && drive <= '6')	/* Laufwerk nach Z mit Big-DOS oder MetaDOS > 2.60 */
		drive_nr = drive - '1' + 26;
	else
		drive_nr = drive - 'A';
	path[0] = drive;
	path[1] = ':';
	ret = Dgetpath(path + 2, drive_nr + 1);
	strcat(path, "\\");

#if 0
	if (fs_case_sens(path) == NO_CASE)
		str_toupper(path);
#endif

	return ret == 0;
}


/* Setzt den aktuellen Pfad. */

BOOLEAN set_path(char *path)
{
	short drive;
	short ret;

	if (path[0] == '\0')
		return FALSE;

	path[0] = toupper(path[0]);
	if (path[0] >= '1' && path[0] <= '6')
		drive = path[0] - '1' + 26;
	else
		drive = path[0] - 'A';
	Dsetdrv(drive);
	ret = Dsetpath(path + 2);
	return ret == 0;
}
