#include "ssd1306_ch32v.h"

void OLED_setContrast(unsigned char c)
{
    _OLED_i2c_write(Set_Contrast_Control_CMD, CMD);
    _OLED_i2c_write(c, CMD);
}