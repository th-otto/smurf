/* ------------------------------------------------------------------------- */
/* ----- Selectric Structure ----------------------------------------------- */
/* ------------------------------------------------------------------------- */

typedef struct
{
	unsigned long	id;				/* Selectric ID (`SLCT') */
	unsigned short	version;		/* Version (BCD-Format) */
	struct
	{
		unsigned           : 7;		/* reserviert */
		unsigned todaytime : 1;		/* aktuelle Dateien mit Uhrzeit */
		unsigned pthsav    : 1;		/* TOS-Pfade sichern */
		unsigned stdest    : 1;		/* Im Zielpfad bleiben */
		unsigned           : 1;		/* reserviert */
		unsigned numsrt    : 1;		/* numerisches Sortieren */
		unsigned lower     : 1;		/* Kleinbuchstaben benutzen */
		unsigned dclick    : 1;		/* Ordner mit Doppelklick */
		unsigned hidden    : 1;		/* versteckte Dateien */
		unsigned onoff     : 1;		/* Selectric¿ AN/AUS */
	} config;
	short	sort;					/* Sortiermodus (neg. = rev.) */
	short	num_ext;				/* Anzahl Extensions */
	char	*(*ext)[];				/* Standard-Extensions */
	short	num_paths;				/* Anzahl Pfade */
	char	*(*paths)[];			/* Standard-Pfade */
	short	comm;					/* communication word */
	short	in_count;				/* input counter */
	void	*in_ptr;				/* input pointer */
	short	out_count;				/* output counter */
	void	*out_ptr;				/* output pointer */
	short	cdecl (*get_first)(DTA *dta, short attrib);
	short	cdecl (*get_next)(DTA *dta);
	short	cdecl (*release_dir)(void);
} SLCT_STR;

/* ------------------------------------------------------------------------- */
