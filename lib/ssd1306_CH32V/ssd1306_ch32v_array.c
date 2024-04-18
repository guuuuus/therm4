#include "ssd1306_ch32v.h"

void OLED_display_arr(const unsigned char *p, unsigned char x, unsigned char y)
{   
    unsigned char tposx = _pos_x;
    unsigned char tposy = _pos_y;
    for (unsigned char i = 0; i < y; i++)
    {
        _OLED_goto(tposx, i+tposy);

        for (unsigned char j = 0; j < x; j++)
        {
            _OLED_i2c_write(p[(i * x) + j], DAT);
        }
    }
}