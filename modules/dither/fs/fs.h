extern short (*set_16_pixels)(uint8_t *source, uint8_t *dest, short depth, long planelen, short howmany);
extern short (*seek_nearest_col)(long *par, short maxcol);
extern SERVICE_FUNCTIONS *service;
extern short not_in_nct;

short floyd_steinberg68000(SMURF_PIC *picture, DITHER_DATA *dither, uint8_t *output);
short floyd_steinberg68030(SMURF_PIC *picture, DITHER_DATA *dither, uint8_t *output);
