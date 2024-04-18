#include "ssd1306_ch32v.h"

void OLED_rotate(unsigned char rot)
{
    // this->_rotation = rot;
    if (rot)
    {
        _OLED_i2c_write((Set_Segment_Remap_CMD | Column_Address_127_Mapped_to_SEG0), CMD);
        // delay(100);
        _OLED_i2c_write((Set_COM_Output_Scan_Direction_CMD | Scan_from_COM0_to_63), CMD);
    }
    else
    {
        _OLED_i2c_write((Set_Segment_Remap_CMD | Column_Address_0_Mapped_to_SEG127), CMD);
        // delay(100);
        _OLED_i2c_write((Set_COM_Output_Scan_Direction_CMD | Scan_from_COM63_to_0), CMD);
    }
}