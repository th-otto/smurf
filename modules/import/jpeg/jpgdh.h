/*	JPEGD header file
	(C) 1992-93 Brainstorm & Atari Corporation.
	to be included after vdi.h
	Last modification on 8-apr-1993.

	Modified for ARAnyM by Patrice Mandin
*/

#ifndef JPGDH_H
#define JPGDH_H

#if defined(__atarist__) || defined(__TOS__)
#define VOID_PTR	void *
#define UCHAR_PTR	unsigned char *
#define FUNC_PTR(x,y)	short (*x)(y)
#define _LONG		long
#define _ULONG		unsigned long
#else
/* otherwise in emulator */
#define	VOID_PTR	uint32
#define	UCHAR_PTR	uint32
#define FUNC_PTR(x,y)	uint32 x
#define _LONG		int32
#define _ULONG		uint32
#endif

typedef struct _JPGD_STRUCT JPGD_STRUCT;
typedef JPGD_STRUCT	*JPGD_PTR;

struct _JPGD_STRUCT {
	/*  0 */ VOID_PTR	InPointer;							/* JPEG Image Pointer */
	/*  4 */ VOID_PTR	OutPointer;							/* Output Buffer/Filename Pointer (see OutFlag) */
	/*  8 */ _LONG	InSize;									/* JPEG Image Size (Bytes) */
	/* 12 */ _LONG	OutSize;								/* Output Image Size (Bytes) */
	/* 16 */ short	InComponents;							/* JPEG Image Components Number (1->4) */
	/* 18 */ short	OutComponents;							/* Output Components Number (1->4) */
	/* 20 */ short	OutPixelSize;							/* Output Pixel Size (1->4) */
	/* 22 */ short	OutFlag;								/* 0 (RAM Output) / -1 (Disk Output) */
	/* 24 */ short	XLoopCounter;							/* Number of MCUs per Row */
	/* 26 */ short	YLoopCounter;							/* Number of MCUs per Column */
	/* 28 */ FUNC_PTR(Create, JPGD_STRUCT *);				/* Pointer to User Routine / NULL */
	/* 32 */ FUNC_PTR(Write, JPGD_STRUCT *);					/* Pointer to User Routine / NULL */
	/* 36 */ FUNC_PTR(Close, JPGD_STRUCT *);					/* Pointer to User Routine / NULL */
	/* 40 */ FUNC_PTR(SigTerm, JPGD_STRUCT *);				/* Pointer to User Routine / NULL */
	/* 44 */ UCHAR_PTR	Comp1GammaPtr;						/* Component 1 Gamma Table / NULL */
	/* 48 */ UCHAR_PTR	Comp2GammaPtr;						/* Component 2 Gamma Table / NULL */
	/* 52 */ UCHAR_PTR	Comp3GammaPtr;						/* Component 3 Gamma Table / NULL */
	/* 56 */ UCHAR_PTR	Comp4GammaPtr;						/* Component 4 Gamma Table / NULL */
	/* 60 */ FUNC_PTR(UserRoutine, JPGD_STRUCT *);			/* Pointer to User Routine (Called during Decompression) / NULL */
	/* 64 */ VOID_PTR	OutTmpPointer;						/* Current OutPointer / Temporary Disk Buffer Pointer (see OutFlag) */
	/* 68 */ short	MCUsCounter;							/* Number of MCUs not Decoded */
	/* 70 */ short	OutTmpHeight;							/* Number of Lines in OutTmpPointer */
	/* 72 */ _LONG	User[2];								/* Free, Available for User */
	/* 80 */ short	OutHandle;								/* 0 / Output File Handle (see OutFlag) */

	/* Output image MFDB */
	/* 82 */ VOID_PTR	MFDBAddress;
	/* 86 */ short	MFDBPixelWidth;
	/* 88 */ short	MFDBPixelHeight;
	/* 90 */ short	MFDBWordSize;
	/* 92 */ short	MFDBFormatFlag;
	/* 94 */ short	MFDBBitPlanes;
	/* 96 */ short	MFDBReserved1;
	/* 98 */ short	MFDBReserved2;
	/* 100 */ short	MFDBReserved3;

	/* Official structure stop here, what follows is decoder-dependant */

	/* 102 */ _ULONG handle;	/* ARAnyM image handle */
	/* 106 */ 
};

#define	JPGD_MAGIC	0x5F4A5044L /* '_JPD' */
#define	JPGD_VERSION	1

#undef NOERROR
enum _JPGD_ENUM {
	NOERROR=0,			/* File correctly uncompressed */
	UNKNOWNFORMAT,		/* File is not JFIF (Error) */
	INVALIDMARKER,		/* Reserved CCITT Marker Found (Error) */
	SOF1MARKER,			/* Mode not handled by the decoder (Error) */
	SOF2MARKER,			/* Mode not handled by the decoder (Error) */
	SOF3MARKER,			/* Mode not handled by the decoder (Error) */
	SOF5MARKER,			/* Mode not handled by the decoder (Error) */
	SOF6MARKER,			/* Mode not handled by the decoder (Error) */
	SOF7MARKER,			/* Mode not handled by the decoder (Error) */
	SOF9MARKER,			/* Mode not handled by the decoder (Error) */
	SOF10MARKER,		/* Mode not handled by the decoder (Error) */
	SOF11MARKER,		/* Mode not handled by the decoder (Error) */
	SOF13MARKER,		/* Mode not handled by the decoder (Error) */
	SOF14MARKER,		/* Mode not handled by the decoder (Error) */
	SOF15MARKER,		/* Mode not handled by the decoder (Error) */
	RSTmMARKER,			/* Unexpected RSTm found (Error) */
	BADDHTMARKER,		/* Buggy DHT Marker (Error) */
	DACMARKER,			/* Marker not handled by the decoder (Error) */
	BADDQTMARKER,		/* Buggy DQT Marker (Error) */
	BADDNLMARKER,		/* Invalid/Unexpected DNL Marker (Error) */
	BADDRIMARKER,		/* Invalid DRI Header Size (Error) */
	DHPMARKER,			/* Marker not handled by the decoder (Error) */
	EXPMARKER,			/* Marker not handled by the decoder (Error) */
	BADSUBSAMPLING,		/* Invalid components subsampling (Error) */
	NOTENOUGHMEMORY,	/* Not enough memory... (Error) */
	DECODERBUSY,		/* Decoder is busy (Error) */
	DSPBUSY,			/* Can't lock the DSP (Error) */
	BADSOFnMARKER,		/* Buggy SOFn marker (Error) */
	BADSOSMARKER,		/* Buggy SOS marker (Error) */
	HUFFMANERROR,		/* Buggy Huffman Stream (Error) */
	BADPIXELFORMAT,		/* Invalid Output Pixel Format (Error) */
	DISKFULL,			/* Hard/Floppy Disk Full (Error) */
	MISSINGMARKER,		/* Marker expected but not found (Error) */
	IMAGETRUNCATED,		/* More bytes Needed (Error) */
	EXTRABYTES,			/* Dummy Bytes after EOI Marker (Warning) */
	USERABORT,			/* User Routine signaled 'Abort' */
	DSPMEMORYERROR,		/* Not Enough DSP RAM (Error) */
	NORSTmMARKER,		/* RSTm Marker expected but not found */
	BADRSTmMARKER,		/* Invalid RSTm Marker Number */
	DRIVERCLOSED,		/* Driver is Already Closed. */
	ENDOFIMAGE			/* Stop Decoding (Internal Message, Should Never Appear) */
};
typedef	long	JPGD_ENUM;

typedef struct {
	long		JPGDVersion;
	JPGD_ENUM	(*JPGDOpenDriver)(JPGD_STRUCT *);
	JPGD_ENUM	(*JPGDCloseDriver)(JPGD_STRUCT *);
	long		(*JPGDGetStructSize)(void);
	JPGD_ENUM	(*JPGDGetImageInfo)(JPGD_STRUCT *);
	JPGD_ENUM	(*JPGDGetImageSize)(JPGD_STRUCT *);
	JPGD_ENUM	(*JPGDDecodeImage)(JPGD_STRUCT *);
} JPGDDRV_STRUCT;
typedef JPGDDRV_STRUCT	*JPGDDRV_PTR;

#define	JPGDOpenDriver(x,y)		(x->JPGDOpenDriver)(y)
#define	JPGDCloseDriver(x,y)	(x->JPGDCloseDriver)(y)
#define	JPGDGetStructSize(x)	(x->JPGDGetStructSize)()
#define	JPGDGetImageInfo(x,y)	(x->JPGDGetImageInfo)(y)
#define	JPGDGetImageSize(x,y)	(x->JPGDGetImageSize)(y)
#define	JPGDDecodeImage(x,y)	(x->JPGDDecodeImage)(y)

#endif /* JPGDH_H */
