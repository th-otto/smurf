extern short PROCESSOR;
extern long filelen;

short decode_lzw_normal(uint8_t *buffer, uint8_t *ziel, short width, short height, uint8_t BitsPerPixel);
short decode_lzw_fast(uint8_t *buffer, uint8_t *ziel);
