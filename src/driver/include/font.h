#ifndef __FONT_CONFIG_H_
#define __FONT_CONFIG_H_

typedef struct {
        unsigned char Size;
        unsigned char Heigh;
        unsigned char Width;
        unsigned char *Table;
} Font_TypeDef;

extern const Font_TypeDef ASCII_Font[4];

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const unsigned char asc2_3216[95][128];

#endif
