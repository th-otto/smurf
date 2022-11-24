#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>

#define _(x) x

typedef struct _rs_header {
/* 0x00 */	uint16_t  rsh_vrsn;			/* version number (should be 1) */
/* 0x02 */	uint16_t  rsh_object;		/* offset to first object */
/* 0x04 */	uint16_t  rsh_tedinfo;		/* offset to TEDINFO structures */
/* 0x06 */	uint16_t  rsh_iconblk;		/* offset to ICONBLK structures */
/* 0x08 */	uint16_t  rsh_bitblk;		/* offset to BITBLK structures */
/* 0x0a */	uint16_t  rsh_frstr;		/* offset to free string (alert box texts) */
/* 0x0c */	uint16_t  rsh_string;		/* offset to string pool */
/* 0x0e */	uint16_t  rsh_imdata;		/* offset to image data */
/* 0x10 */	uint16_t  rsh_frimg;		/* offset to free images */
/* 0x12 */	uint16_t  rsh_trindex;		/* offset to tree addresses */
/* 0x14 */	uint16_t  rsh_nobs;			/* number of objects */
/* 0x16 */	uint16_t  rsh_ntree;		/* number of trees */
/* 0x18 */	uint16_t  rsh_nted;			/* number of TEDINFOs */
/* 0x1a */	uint16_t  rsh_nib;			/* number of ICONBLKs */
/* 0x1c */	uint16_t  rsh_nbb;			/* number of BITBLKs */
/* 0x1e */	uint16_t  rsh_nstring;		/* number of free strings */
/* 0x20 */	uint16_t  rsh_nimages;		/* number of free images */
/* 0x22 */	uint16_t  rsh_rssize;		/* total resource size */
} RS_HEADER;
#define SIZEOF_RS_HEADER ((size_t)(0x24))

#define SIZEOF_GEMDOS_HEADER 28

#define RSC_SIZEOF_LONG     4
#define RSC_SIZEOF_PTR      4
#define RSC_SIZEOF_TEDINFO  28
#define RSC_SIZEOF_ICONBLK  34
#define RSC_SIZEOF_BITBLK   14
#define RSC_SIZEOF_CICON    22
#define RSC_SIZEOF_CICONBLK 38
#define RSC_SIZEOF_USERBLK   8
#define RSC_SIZEOF_OBJECT   24

#define CICON_STR_SIZE      12

static char rscname[1024];

#define	 TRUE	1
#define	 FALSE  0


/* graphic types of obs */
#define G_BOX           20
#define G_TEXT          21
#define G_BOXTEXT       22
#define G_IMAGE         23
#define G_USERDEF       24
#define G_IBOX          25
#define G_BUTTON        26
#define G_BOXCHAR       27
#define G_STRING        28
#define G_FTEXT         29
#define G_FBOXTEXT      30
#define G_ICON          31
#define G_TITLE         32
#define G_CICON         33              /* AES >= 3.3 */
#define G_SWBUTTON      34              /* MAG!X */
#define G_DTMFDB		34				/* ViewMax: for internal AES use only: desktop image */
#define G_POPUP         35              /* MAG!X */
#define G_RESVD1        36              /* MagiC 3.1 */
#define G_WINTITLE		36				/* MagiC internal window title */
#define G_EDIT			37				/* MagiC extended edit object */
#define G_SHORTCUT		38				/* MagiC 6 menu entry with shortcut */
#define G_SLIST			39				/* XaAES scrolling list */
#define G_EXTBOX		40				/* XaAES */
#define G_OBLINK		41				/* XaAES */


#define OF_LASTOB          0x0020


static uint16_t getbeshort(const unsigned char *ptr)
{
	return ((ptr[0] << 8) & 0xff00) | (ptr[1] & 0x00ff);
}


static uint32_t getbelong(const unsigned char *ptr)
{
	return ((uint32_t)getbeshort(ptr) << 16) | ((uint32_t)getbeshort(ptr + 2));
}


static void putbeshort(unsigned char *ptr, uint16_t val)
{
	ptr[0] = (val >> 8) & 0xff;
	ptr[1] = val & 0xff;
}


static void putbelong(unsigned char *ptr, uint32_t val)
{
	putbeshort(ptr, val >> 16);
	putbeshort(ptr + 2, val);
}


/*** ---------------------------------------------------------------------- ***/

#if 0
static void get_rsc_header(const unsigned char *ptr, RS_HEADER *hdr)
{
	hdr->rsh_vrsn = getbeshort(ptr + 0);
	hdr->rsh_object = getbeshort(ptr + 2);
	hdr->rsh_tedinfo = getbeshort(ptr + 4);
	hdr->rsh_iconblk = getbeshort(ptr + 6);
	hdr->rsh_bitblk = getbeshort(ptr + 8);
	hdr->rsh_frstr = getbeshort(ptr + 10);
	hdr->rsh_string = getbeshort(ptr + 12);
	hdr->rsh_imdata = getbeshort(ptr + 14);
	hdr->rsh_frimg = getbeshort(ptr + 16);
	hdr->rsh_trindex = getbeshort(ptr + 18);
	hdr->rsh_nobs = getbeshort(ptr + 20);
	hdr->rsh_ntree = getbeshort(ptr + 22);
	hdr->rsh_nted = getbeshort(ptr + 24);
	hdr->rsh_nib = getbeshort(ptr + 26);
	hdr->rsh_nbb = getbeshort(ptr + 28);
	hdr->rsh_nstring = getbeshort(ptr + 30);
	hdr->rsh_nimages = getbeshort(ptr + 32);
	hdr->rsh_rssize = getbeshort(ptr + 34);
}
#endif

/*** ---------------------------------------------------------------------- ***/

static void put_rsc_header(unsigned char *ptr, RS_HEADER *hdr)
{
	putbeshort(ptr + 0, hdr->rsh_vrsn);
	putbeshort(ptr + 2, hdr->rsh_object);
	putbeshort(ptr + 4, hdr->rsh_tedinfo);
	putbeshort(ptr + 6, hdr->rsh_iconblk);
	putbeshort(ptr + 8, hdr->rsh_bitblk);
	putbeshort(ptr + 10, hdr->rsh_frstr);
	putbeshort(ptr + 12, hdr->rsh_string);
	putbeshort(ptr + 14, hdr->rsh_imdata);
	putbeshort(ptr + 16, hdr->rsh_frimg);
	putbeshort(ptr + 18, hdr->rsh_trindex);
	putbeshort(ptr + 20, hdr->rsh_nobs);
	putbeshort(ptr + 22, hdr->rsh_ntree);
	putbeshort(ptr + 24, hdr->rsh_nted);
	putbeshort(ptr + 26, hdr->rsh_nib);
	putbeshort(ptr + 28, hdr->rsh_nbb);
	putbeshort(ptr + 30, hdr->rsh_nstring);
	putbeshort(ptr + 32, hdr->rsh_nimages);
	putbeshort(ptr + 34, hdr->rsh_rssize);
}

/*** ---------------------------------------------------------------------- ***/

static void print_header(const RS_HEADER *header)
{
	printf("rsh_vrsn:     $%04x\n", header->rsh_vrsn);
	printf("rsh_object:   $%04x\n", header->rsh_object);
	printf("rsh_tedinfo:  $%04x\n", header->rsh_tedinfo);
	printf("rsh_iconblk:  $%04x\n", header->rsh_iconblk);
	printf("rsh_bitblk:   $%04x\n", header->rsh_bitblk);
	printf("rsh_frstr:    $%04x\n", header->rsh_frstr);
	printf("rsh_string:   $%04x\n", header->rsh_string);
	printf("rsh_imdata:   $%04x\n", header->rsh_imdata);
	printf("rsh_frimg:    $%04x\n", header->rsh_frimg);
	printf("rsh_trindex:  $%04x\n", header->rsh_trindex);
	printf("rsh_nobs:     $%04x\n", header->rsh_nobs);
	printf("rsh_ntree:    $%04x\n", header->rsh_ntree);
	printf("rsh_nted:     $%04x\n", header->rsh_nted);
	printf("rsh_nib:      $%04x\n", header->rsh_nib);
	printf("rsh_nbb:      $%04x\n", header->rsh_nbb);
	printf("rsh_nstring:  $%04x\n", header->rsh_nstring);
	printf("rsh_nimages:  $%04x\n", header->rsh_nimages);
	printf("rsh_rssize:   $%04x\n", header->rsh_rssize);
}

/*** ---------------------------------------------------------------------- ***/

static void write_file(FILE *fp, const unsigned char *address, long size)
{
	long written;
	
	written = fwrite(address, 1, (size_t)size, fp);
	if (written != size)
	{
		fprintf(stderr, _("error writing %s: %s\n"), rscname, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/*** ---------------------------------------------------------------------- ***/

int main(int argc, char **argv)
{
	FILE *fp;
	const char *filename;
	long file_size;
	unsigned char *buffer;
	unsigned char *address;
	const unsigned char *end;
	RS_HEADER gemhdr;
	int found;
	uint32_t file_offset;
	long text_size;
	long data_size;
	char *rscname_end;
	uint32_t ciconsize;
	uint32_t num_ciconblks;
	
	if (argc != 2)
	{
		fprintf(stderr, _("usage: findrsc <filename>\n"));
		return EXIT_FAILURE;
	}
	filename = argv[1];

	{
		strcpy(rscname, filename);
		rscname_end = strrchr(rscname, '.');
		if (rscname_end == NULL)
			rscname_end = rscname + strlen(rscname);
		if (strcasecmp(rscname_end, ".rsc") == 0)
		{
			fprintf(stderr, _("%s: invalid filename\n"), filename);
			return EXIT_FAILURE;
		}
		strcpy(rscname_end, ".rsc");
	}

	fp = fopen(filename, "rb");	/* open source file */
	if (fp == NULL)
	{
		fprintf(stderr, _("%s not found\n"), filename);
		return EXIT_FAILURE;
	}
	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	buffer = (unsigned char *)malloc(file_size);
	if (!buffer)
	{
		fprintf(stderr, "%s", strerror(errno));
		return EXIT_FAILURE;
	}
	fread(buffer, 1, file_size, fp);
	fclose(fp);

	file_offset = 0;

	if (file_size > (RSC_SIZEOF_OBJECT + 512) && getbeshort(buffer) == 0x64)
		file_offset += 512;

	if (file_size < (file_offset + SIZEOF_GEMDOS_HEADER + RSC_SIZEOF_OBJECT))
	{
		fprintf(stderr, _("%s: file too short\n"), filename);
		return EXIT_FAILURE;
	}

	if (getbeshort(buffer + file_offset) != 0x601a)
	{
		fprintf(stderr, _("%s: unknown magic $%04x\n"), filename, getbeshort(buffer + file_offset));
		return EXIT_FAILURE;
	}
	text_size = getbelong(buffer + file_offset + 2);
	data_size = getbelong(buffer + file_offset + 6);
	file_offset += SIZEOF_GEMDOS_HEADER;

	address = buffer + file_offset + text_size;
	end = address + data_size - RSC_SIZEOF_OBJECT;
	
	found = 0;
	for (; address < end; address += 2)
	{
		int32_t index;
		
		/*
		 * todo: check also that there is a relocation entry for this address
		 */
		index = getbelong(address);
		if (index >= text_size &&
			index <= text_size + data_size - RSC_SIZEOF_OBJECT)
		{
			unsigned char *obj = buffer + index + file_offset;

			if (getbeshort(obj + 0) == 0xffff &&
				getbeshort(obj + 2) == 1 &&
				getbeshort(obj + 6) == G_BOX)
			{
				uint32_t offset;
				uint32_t string_space;
				uint32_t imdata_size;
				unsigned char *rscbuf;
				uint32_t ntree;
				int ok = TRUE;
				uint32_t rsh_frstr = 0;
				uint32_t rsh_frimg = 0;
				uint32_t rsh_tedinfo = 0;
				uint32_t rsh_bitblk = 0;
				const char *str;

				gemhdr.rsh_vrsn = 1;
				gemhdr.rsh_nobs = 0;
				gemhdr.rsh_ntree = 0;
				gemhdr.rsh_nted = 0;
				gemhdr.rsh_nib = 0;
				gemhdr.rsh_nbb = 0;
				gemhdr.rsh_nstring = 0;
				gemhdr.rsh_nimages = 0;
				string_space = 0;
				imdata_size = 0;
				ciconsize = 0;
				num_ciconblks = 0;
				
				while (address + gemhdr.rsh_ntree * RSC_SIZEOF_PTR < end &&
					index >= text_size &&
					index + RSC_SIZEOF_OBJECT <= text_size + data_size &&
					gemhdr.rsh_ntree < 1)
				{
					++gemhdr.rsh_ntree;
					index = getbelong(address + gemhdr.rsh_ntree * RSC_SIZEOF_PTR);
				}
				ntree = gemhdr.rsh_ntree;
				
				/*
				 * pass1: count objects & calculate sizes
				 */
				while (ok && ntree && obj < end)
				{
					index = getbelong(obj + 12);
					switch (getbeshort(obj + 6) & 0xff)
					{
					case G_STRING:
					case G_TITLE:
					case G_BUTTON:
					case G_SHORTCUT:
						if (index <= 0 || index + file_offset >= file_size)
							ok = FALSE;
						else
							string_space += strlen((char*)buffer + index + file_offset) + 1;
						break;

					case G_BOX:
					case G_IBOX:
					case G_BOXCHAR:
					case G_EXTBOX:
						break;

					case G_TEXT:
					case G_BOXTEXT:
					case G_FTEXT:
					case G_FBOXTEXT:
						if (index <= 0 || index + file_offset >= file_size)
						{
							ok = FALSE;
						} else
						{
							unsigned char *ted = buffer + index + file_offset;
							string_space += getbeshort(ted + 24); /* te_txtlen */
							string_space += getbeshort(ted + 26); /* te_tmplen */
							string_space += strlen((char*)buffer + getbelong(ted + 8) + file_offset) + 1; /* te_pvalid */
							if (rsh_tedinfo == 0)
								rsh_tedinfo = index;
						}
						gemhdr.rsh_nted++;
						break;

					case G_IMAGE:
						if (index <= 0 || index + file_offset >= file_size)
						{
							ok = FALSE;
						} else
						{
							unsigned char *bit;
							uint16_t wb;
							uint32_t hl;
							uint32_t datasize;
						
							bit = buffer + index + file_offset;
							wb = getbeshort(bit + 4);
							hl = getbeshort(bit + 6);
							datasize = ((wb + 1) & ~1) * hl;
							imdata_size += datasize;
							if (rsh_bitblk == 0)
								rsh_bitblk = index;
						}
						gemhdr.rsh_nbb++;
						break;
					
					case G_ICON:
						if (index <= 0 || index + file_offset >= file_size)
						{
							ok = FALSE;
						} else
						{
							unsigned char *icon;
							uint16_t wb;
							uint32_t hl;
							uint32_t masksize;

							icon = buffer + index + file_offset;
							wb = getbeshort(icon + 22);
							hl = getbeshort(icon + 24);
							masksize = ((wb + 15) >> 4) * 2 * hl;
							imdata_size += masksize * 2;
							string_space += strlen((char*)buffer + getbelong(icon + 8) + file_offset) + 1; /* ib_ptext */
						}
						gemhdr.rsh_nib++;
						break;
					
					case G_CICON:
						if (index <= 0 || index + file_offset >= file_size)
						{
							ok = FALSE;
						} else
						{
							unsigned char *cicon;
							uint16_t wb;
							uint32_t hl;
							uint32_t masksize;
							uint32_t data;
							uint32_t mask;
							uint32_t ptext;
							uint32_t next_res;
							uint16_t planes;
							size_t len;
							
							gemhdr.rsh_vrsn |= 0x04;
							cicon = buffer + index + file_offset;
							wb = getbeshort(cicon + 22);
							hl = getbeshort(cicon + 24);
							data = getbelong(cicon + 0);
							mask = getbelong(cicon + 4);
							ptext = getbelong(cicon + 8);
							next_res = getbelong(cicon + 34);
							str = (char*)buffer + ptext + file_offset;
							len = strlen(str) + 1;
							if (len > CICON_STR_SIZE)
								string_space += len; /* ib_ptext */
							masksize = ((wb + 15) >> 4) * 2 * hl;
							ciconsize += masksize * 2 + RSC_SIZEOF_CICONBLK + CICON_STR_SIZE;
							while (next_res != 0)
							{
								cicon = buffer + next_res + file_offset;
								planes = getbeshort(cicon + 0);
								data = getbelong(cicon + 2);
								if (data)
									ciconsize += masksize * planes;
								mask = getbelong(cicon + 6);
								if (mask)
									ciconsize += masksize;
								data = getbelong(cicon + 10);
								if (data)
									ciconsize += masksize * planes;
								mask = getbelong(cicon + 14);
								if (mask)
									ciconsize += masksize;
								next_res = getbelong(cicon + 18);
								ciconsize += RSC_SIZEOF_CICON;
							}
							num_ciconblks++;
						}
						break;
					
					default:
						fprintf(stderr, "%s: unsupported object type %d\n", filename, getbeshort(obj + 6) & 0xff);
						ok = FALSE;
						break;
					}
					if (getbeshort(obj + 8) & OF_LASTOB)
						ntree--;
					
					gemhdr.rsh_nobs++;
					obj += RSC_SIZEOF_OBJECT;
				}
				
				/*
				 * pass2: try to locate free string array.
				 * it should be either just before the bitblk array,
				 * or just after the tedinfo array.
				 */
				if (ok && rsh_tedinfo)
				{
					uint32_t froffset;
					
					rsh_frstr = rsh_tedinfo + gemhdr.rsh_nted * RSC_SIZEOF_TEDINFO;
					froffset = rsh_frstr;
					while (froffset < text_size + data_size)
					{
						index = getbelong(buffer + file_offset + froffset);
						if (index < text_size || index >= text_size + data_size)
							break;
						gemhdr.rsh_nstring++;
						froffset += RSC_SIZEOF_PTR;
					}
					if (gemhdr.rsh_nstring == 0)
					{
						if (rsh_bitblk)
							rsh_frstr = rsh_bitblk - RSC_SIZEOF_PTR;
						else
							rsh_frstr = rsh_tedinfo - RSC_SIZEOF_PTR;
						froffset = rsh_frstr;
						while (froffset >= text_size)
						{
							index = getbelong(buffer + file_offset + froffset);
							if (index < text_size || index >= text_size + data_size)
								break;
							printf("string %d: %08x\n", gemhdr.rsh_nstring, index);
							gemhdr.rsh_nstring++;
							rsh_frstr = froffset;
							froffset -= RSC_SIZEOF_PTR;
						}
					}
					if (gemhdr.rsh_nstring == 0)
					{
						rsh_frstr = 0;
					} else
					{
						for (ntree = 0; ntree < gemhdr.rsh_nstring; ntree++)
						{
							index = getbelong(buffer + file_offset + rsh_frstr + ntree * RSC_SIZEOF_PTR);
							string_space += strlen((char*)buffer + index + file_offset) + 1;
						}
					}
				}

				/*
				 * pass3: try to locate free bitblk array.
				 * it should be just before the tedinfo array.
				 */
				if (ok && rsh_tedinfo)
				{
					uint32_t froffset;

					rsh_frimg = rsh_tedinfo - RSC_SIZEOF_PTR;
					froffset = rsh_frimg;
					while (froffset >= text_size)
					{
						index = getbelong(buffer + file_offset + froffset);
						if (index < text_size || index >= text_size + data_size)
							break;
						printf("image %d: %08x\n", gemhdr.rsh_nimages, index);
						gemhdr.rsh_nimages++;
						rsh_frimg = froffset;
						froffset -= RSC_SIZEOF_PTR;
					}
					if (gemhdr.rsh_nimages == 0)
					{
						rsh_frimg = 0;
					} else
					{
						for (ntree = 0; ntree < gemhdr.rsh_nimages; ntree++)
						{
							unsigned char *bit;
							uint16_t wb;
							uint32_t hl;
							uint32_t datasize;

							index = getbelong(buffer + file_offset + rsh_frimg + ntree * RSC_SIZEOF_PTR);
							bit = buffer + index + file_offset;
							wb = getbeshort(bit + 4);
							hl = getbeshort(bit + 6);
							datasize = ((wb + 1) & ~1) * hl;
							imdata_size += datasize;
						}
					}
				}

				printf(_("trindex at $%08lx; %u trees, %u objects, %u strings, %u images\n"), (unsigned long)(address - buffer - file_offset), gemhdr.rsh_ntree, gemhdr.rsh_nobs, gemhdr.rsh_nstring, gemhdr.rsh_nimages);

				/*
				 * pass4: collect data, and write resource
				 */
				if (ok)
				{
					size_t len;
					uint32_t cicon;
					uint16_t num_ext = 3;
					
					found++;
					
					if (string_space & 1)
						string_space++;
					
					gemhdr.rsh_nbb += gemhdr.rsh_nimages;

					offset = SIZEOF_RS_HEADER;
					gemhdr.rsh_string = offset;
					offset += string_space;
					gemhdr.rsh_imdata = offset;
					offset += imdata_size;
					gemhdr.rsh_frstr = offset;
					offset += gemhdr.rsh_nstring * RSC_SIZEOF_PTR;
					gemhdr.rsh_bitblk = offset;
					offset += gemhdr.rsh_nbb * RSC_SIZEOF_BITBLK;
					gemhdr.rsh_frimg = offset;
					offset += gemhdr.rsh_nimages * RSC_SIZEOF_PTR;
					gemhdr.rsh_tedinfo = offset;
					offset += gemhdr.rsh_nted * RSC_SIZEOF_TEDINFO;
					gemhdr.rsh_iconblk = offset;
					offset += gemhdr.rsh_nib * RSC_SIZEOF_ICONBLK;
					gemhdr.rsh_object = offset;
					offset += gemhdr.rsh_nobs * RSC_SIZEOF_OBJECT;
					gemhdr.rsh_trindex = offset;
					offset += gemhdr.rsh_ntree * RSC_SIZEOF_PTR;
					gemhdr.rsh_rssize = offset;
					
					printf(_("found resource at $%08lx, size $%04x\n"), (unsigned long)(address - buffer - file_offset), gemhdr.rsh_rssize);
		
					print_header(&gemhdr);

					cicon = 0;
					if (num_ciconblks != 0)
					{
						ciconsize += (num_ciconblks + 1) * RSC_SIZEOF_LONG + num_ext * RSC_SIZEOF_LONG;
						cicon = gemhdr.rsh_rssize + num_ext * RSC_SIZEOF_LONG + (num_ciconblks + 1) * RSC_SIZEOF_LONG;
						num_ciconblks = 0;
					}
					file_size = gemhdr.rsh_rssize + ciconsize;
					rscbuf = (unsigned char *)malloc(file_size);
					if (!rscbuf)
					{
						fprintf(stderr, "%s\n", strerror(errno));
						return EXIT_FAILURE;
					}
					memset(rscbuf, 0, file_size);
					
#if 1
					sprintf(rscname_end, "_%d.rsc", found);
#endif
					
					fp = fopen(rscname, "wb");
					if (fp == NULL)
					{
						fprintf(stderr, _("can't create %s: %s\n"), rscname, strerror(errno));
						exit(EXIT_FAILURE);
					}
					
					put_rsc_header(rscbuf, &gemhdr);
		
					for (ntree = 0; ntree < gemhdr.rsh_ntree; ntree++)
					{
						index = getbelong(address + ntree * RSC_SIZEOF_PTR);
						index -= getbelong(address);
						putbelong(rscbuf + gemhdr.rsh_trindex + ntree * RSC_SIZEOF_PTR, gemhdr.rsh_object + index);
					}
					
					if (rsh_bitblk == 0 && gemhdr.rsh_nimages > 0)
					{
						rsh_bitblk = getbelong(buffer + file_offset + rsh_frimg);
					}
					
					index = getbelong(address);
					obj = buffer + index + file_offset;
					for (ntree = 0; ntree < gemhdr.rsh_nobs; ntree++)
					{
						memcpy(rscbuf + gemhdr.rsh_object, obj, RSC_SIZEOF_OBJECT);
						
						index = getbelong(obj + 12);
						switch (getbeshort(obj + 6) & 0xff)
						{
						case G_STRING:
						case G_TITLE:
						case G_BUTTON:
						case G_SHORTCUT:
							str = (char*)buffer + index + file_offset;
							len = strlen(str) + 1;
							memcpy(rscbuf + gemhdr.rsh_string, str, len);
							putbelong(rscbuf + gemhdr.rsh_object + 12, gemhdr.rsh_string);
							gemhdr.rsh_string += len;
							break;
	
						case G_BOX:
						case G_IBOX:
						case G_BOXCHAR:
						case G_EXTBOX:
							break;

						case G_TEXT:
						case G_BOXTEXT:
						case G_FTEXT:
						case G_FBOXTEXT:
							{
								unsigned char *ted = buffer + index + file_offset;

								memcpy(rscbuf + gemhdr.rsh_tedinfo, ted, RSC_SIZEOF_TEDINFO);
								putbelong(rscbuf + gemhdr.rsh_object + 12, gemhdr.rsh_tedinfo);

								str = (char*)buffer + getbelong(ted + 0) + file_offset;
								len = getbeshort(ted + 24); /* te_txtlen */
								memcpy(rscbuf + gemhdr.rsh_string, str, len);
								putbelong(rscbuf + gemhdr.rsh_tedinfo + 0, gemhdr.rsh_string);
								gemhdr.rsh_string += len;

								str = (char*)buffer + getbelong(ted + 4) + file_offset;
								len = getbeshort(ted + 26); /* te_tmplen */
								memcpy(rscbuf + gemhdr.rsh_string, str, len);
								putbelong(rscbuf + gemhdr.rsh_tedinfo + 4, gemhdr.rsh_string);
								gemhdr.rsh_string += len;

								str = (char*)buffer + getbelong(ted + 8) + file_offset;
								len = strlen(str) + 1;
								memcpy(rscbuf + gemhdr.rsh_string, str, len);
								putbelong(rscbuf + gemhdr.rsh_tedinfo + 8, gemhdr.rsh_string);
								gemhdr.rsh_string += len;
								
								gemhdr.rsh_tedinfo += RSC_SIZEOF_TEDINFO;
							}
							break;
	
						case G_IMAGE:
							{
								unsigned char *bit = buffer + index + file_offset;
								uint16_t wb = getbeshort(bit + 4);
								uint32_t hl = getbeshort(bit + 6);
								uint32_t datasize = ((wb + 1) & ~1) * hl;

								memcpy(rscbuf + gemhdr.rsh_bitblk, bit, RSC_SIZEOF_BITBLK);
								putbelong(rscbuf + gemhdr.rsh_object + 12, gemhdr.rsh_bitblk);

								str = (char*)buffer + getbelong(bit + 0) + file_offset;
								memcpy(rscbuf + gemhdr.rsh_imdata, str, datasize);
								putbelong(rscbuf + gemhdr.rsh_bitblk, gemhdr.rsh_imdata);
								gemhdr.rsh_imdata += datasize;
								
								gemhdr.rsh_bitblk += RSC_SIZEOF_BITBLK;
							}
							break;
						
						case G_ICON:
							{
								unsigned char *icon = buffer + index + file_offset;
								uint16_t wb = getbeshort(icon + 22);
								uint32_t hl = getbeshort(icon + 24);
								uint32_t masksize = ((wb + 15) >> 4) * 2 * hl;

								memcpy(rscbuf + gemhdr.rsh_iconblk, icon, RSC_SIZEOF_ICONBLK);
								putbelong(rscbuf + gemhdr.rsh_object + 12, gemhdr.rsh_iconblk);

								str = (char*)buffer + getbelong(icon + 8) + file_offset;
								len = strlen(str) + 1;
								memcpy(rscbuf + gemhdr.rsh_string, str, len);
								putbelong(rscbuf + gemhdr.rsh_iconblk + 8, gemhdr.rsh_string);
								gemhdr.rsh_string += len;
								
								str = (char*)buffer + getbelong(icon + 0) + file_offset;
								memcpy(rscbuf + gemhdr.rsh_imdata, str, masksize);
								putbelong(rscbuf + gemhdr.rsh_iconblk, gemhdr.rsh_imdata);
								gemhdr.rsh_imdata += masksize;
								
								str = (char*)buffer + getbelong(icon + 4) + file_offset;
								memcpy(rscbuf + gemhdr.rsh_imdata, str, masksize);
								putbelong(rscbuf + gemhdr.rsh_iconblk + 4, gemhdr.rsh_imdata);
								gemhdr.rsh_imdata += masksize;
								
								gemhdr.rsh_iconblk += RSC_SIZEOF_ICONBLK;
							}
							break;
						
						case G_CICON:
							{
								unsigned char *icon = buffer + index + file_offset;
								uint32_t dest;
								uint16_t wb = getbeshort(icon + 22);
								uint32_t hl = getbeshort(icon + 24);
								uint32_t masksize = ((wb + 15) >> 4) * 2 * hl;
								uint32_t next_res;
								uint16_t planes;
								uint32_t data, mask, ptext;
								uint32_t num_cicons;
								uint32_t ciconblk;
								
								dest = cicon;
								ciconblk = cicon;
								memcpy(rscbuf + dest, icon, RSC_SIZEOF_CICONBLK);
								putbelong(rscbuf + gemhdr.rsh_object + 12, num_ciconblks);
								dest += RSC_SIZEOF_CICONBLK;
								
								str = (char*)buffer + getbelong(icon + 0) + file_offset;
								memcpy(rscbuf + dest, str, masksize);
								putbelong(rscbuf + cicon, dest);
								dest += masksize;
								
								str = (char*)buffer + getbelong(icon + 4) + file_offset;
								memcpy(rscbuf + dest, str, masksize);
								putbelong(rscbuf + cicon + 4, dest);
								dest += masksize;
								
								ptext = getbelong(icon + 8);
								str = (char*)buffer + ptext + file_offset;
								len = strlen(str) + 1;
								if (len > CICON_STR_SIZE)
								{
									memcpy(rscbuf + gemhdr.rsh_string, str, len);
									putbelong(rscbuf + cicon + 8, gemhdr.rsh_string);
									gemhdr.rsh_string += len;
									fprintf(stderr, "warning: icon text too long\n");
									len = CICON_STR_SIZE;
								} else
								{
									putbelong(rscbuf + cicon + 8, dest);
								}
								memcpy(rscbuf + dest, str, len);
								dest += CICON_STR_SIZE;

								next_res = getbelong(icon + 34);
								if (next_res != 0)
									putbelong(rscbuf + ciconblk + 34, dest);
								else
									putbelong(rscbuf + ciconblk + 34, 0);
								num_cicons = 0;
								while (next_res != 0)
								{
									cicon = dest;
									dest += RSC_SIZEOF_CICON;
									icon = buffer + next_res + file_offset;
									planes = getbeshort(icon + 0);
									putbeshort(rscbuf + cicon + 0, planes);
									data = getbelong(icon + 2);
									if (data)
									{
										memcpy(rscbuf + dest, buffer + data + file_offset, masksize * planes);
										putbelong(rscbuf + cicon + 2, dest);
										dest += masksize * planes;
									} else
									{
										putbelong(rscbuf + cicon + 2, 0);
									}
									mask = getbelong(icon + 6);
									if (mask)
									{
										memcpy(rscbuf + dest, buffer + mask + file_offset, masksize);
										putbelong(rscbuf + cicon + 6, dest);
										dest += masksize;
									} else
									{
										putbelong(rscbuf + cicon + 6, 0);
									}
									data = getbelong(icon + 10);
									if (data)
									{
										memcpy(rscbuf + dest, buffer + data + file_offset, masksize * planes);
										putbelong(rscbuf + cicon + 10, dest);
										dest += masksize * planes;
									} else
									{
										putbelong(rscbuf + cicon + 10, 0);
									}
									mask = getbelong(icon + 14);
									if (mask)
									{
										memcpy(rscbuf + dest, buffer + mask + file_offset, masksize);
										putbelong(rscbuf + cicon + 14, dest);
										dest += masksize;
									} else
									{
										putbelong(rscbuf + cicon + 14, 0);
									}
									next_res = getbelong(icon + 18);
									if (next_res != 0)
										putbelong(rscbuf + cicon + 18, dest);
									else
										putbelong(rscbuf + cicon + 18, 0);
									num_cicons++;
								}
								num_ciconblks++;
								cicon = dest;
								/*
								 * in a file, ciconblk.mainlist must be the number of
								 * CICON structures
								 */
								putbelong(rscbuf + ciconblk + 34, num_cicons);
							}
							break;
						}
						
						gemhdr.rsh_object += RSC_SIZEOF_OBJECT;
						obj += RSC_SIZEOF_OBJECT;
					}
					
					for (ntree = 0; ntree < gemhdr.rsh_nimages; ntree++)
					{
						unsigned char *bit;
						uint16_t wb;
						uint32_t hl;
						uint32_t datasize;
						
						index = getbelong(buffer + file_offset + rsh_frimg + ntree * RSC_SIZEOF_PTR);
						putbelong(rscbuf + gemhdr.rsh_frimg + ntree * RSC_SIZEOF_PTR, gemhdr.rsh_bitblk);
						bit = buffer + index + file_offset;
						memcpy(rscbuf + gemhdr.rsh_bitblk, bit, RSC_SIZEOF_BITBLK);
						
						wb = getbeshort(bit + 4);
						hl = getbeshort(bit + 6);
						datasize = ((wb + 1) & ~1) * hl;
						index = getbelong(bit);
						memcpy(rscbuf + gemhdr.rsh_imdata, buffer + file_offset + index, datasize);
						putbelong(rscbuf + gemhdr.rsh_bitblk, gemhdr.rsh_imdata);
						
						gemhdr.rsh_imdata += datasize;
						gemhdr.rsh_bitblk += RSC_SIZEOF_BITBLK;
					}
					
					for (ntree = 0; ntree < gemhdr.rsh_nstring; ntree++)
					{
						index = getbelong(buffer + file_offset + rsh_frstr + ntree * RSC_SIZEOF_PTR);
						str = (char*)buffer + index + file_offset;
						len = strlen(str) + 1;
						memcpy(rscbuf + gemhdr.rsh_string, str, len);
						putbelong(rscbuf + gemhdr.rsh_frstr + ntree * RSC_SIZEOF_PTR, gemhdr.rsh_string);
						gemhdr.rsh_string += len;
					}

					if (gemhdr.rsh_string & 1)
						rscbuf[gemhdr.rsh_string] = '\0';
					
					if (num_ciconblks != 0)
					{
						unsigned char *list;
						uint32_t i;
						
						list = rscbuf + gemhdr.rsh_rssize;
						putbelong(list + 0 * RSC_SIZEOF_LONG, file_size);
						putbelong(list + 1 * RSC_SIZEOF_LONG, gemhdr.rsh_rssize + num_ext * RSC_SIZEOF_LONG);
						putbelong(list + 2 * RSC_SIZEOF_LONG, 0);
						list += num_ext * RSC_SIZEOF_LONG;
						for (i = 0; i < num_ciconblks; i++)
							putbelong(list + i * RSC_SIZEOF_LONG, 0);
						putbelong(list + num_ciconblks * RSC_SIZEOF_LONG, -1);
					}
					
					if (cicon != file_size)
					{
						fprintf(stderr, "%08x != %08lx\n", cicon, file_size);
						assert(cicon == file_size);
					}
					write_file(fp, rscbuf, file_size);
					fclose(fp);
					free(rscbuf);
				}
			}
		}
	}
	
	if (!found)
	{
		fprintf(stderr, _("%s: no resource found\n"), filename);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
