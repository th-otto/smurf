extern short *lacetab;

uint8_t *encode_lzw_17bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height, uint8_t BitsPerPixel);
uint8_t *encode_lzw_8bit(uint8_t *buffer, uint8_t *ziel, unsigned short width, unsigned short height);

