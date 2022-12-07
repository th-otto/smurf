extern void *(*SMalloc)(long amount);
extern void (*SMfree)(void *ptr);

#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

#define MOTOROLA 1
#define INTEL 2


#define DEBUG	0
#define PRG		0
#define TIMER 0


void tiff24Planes_24Normal(GARGAMEL *smurf_struct, uint8_t *source1, uint8_t *dest, long Width, long Height,
	long DataCount, long *DataArray);
void tiff32CMYK_24Normal(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, long DataOffset);
void tiff4_to_stf(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height);
void tiff2_to_stf(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height);
void tiff24_colormap(uint16_t *source, uint8_t *dest, long colors, short format);
void tiff24_min_is_black(uint8_t *dest, short Depth);
void tiff24_min_is_white(uint8_t *dest, short Depth);
short tiff32773_depack(GARGAMEL *smurf_struct, uint8_t *buffer, uint8_t *ziel, long width, long height,
							short BitsPerPixel, long RowPerStrip, long *DataArray);
short tiffCCITT3_depack(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, short Depth);
short tiffLZW_depack(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, short Depth,
						  long DataCount, long *DataArray, long RowPerStrip, short Difference);
void invert_1Bit(uint8_t *data, long length);
