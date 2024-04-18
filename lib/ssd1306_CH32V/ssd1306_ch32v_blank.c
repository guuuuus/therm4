#include "ssd1306_ch32v.h"


void OLED_display()
{
    _OLED_i2c_write((Set_Display_ON_or_OFF_CMD | Display_ON), CMD);
}


void OLED_noDisplay()
{
    _OLED_i2c_write((Set_Display_ON_or_OFF_CMD | Display_OFF), CMD);
}