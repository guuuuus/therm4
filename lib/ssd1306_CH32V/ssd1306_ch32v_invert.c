#include "ssd1306_ch32v.h"

void OLED_invert(unsigned char inv)
{
    _OLED_i2c_write((Set_Normal_or_Inverse_Display_CMD | inv), CMD);
    // _invert = !_invert;
}