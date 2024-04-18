#include "ssd1306_ch32v.h"

#define TIMOUT_WAITS 50000

unsigned char _y_size;
unsigned char _x_size;
unsigned char _invert;
unsigned char _char_x = fontlenght;
unsigned char _char_y = fontheigt;
unsigned char _addr_i2c;

unsigned char _pos_x = 0;
unsigned char _pos_y = 0;
unsigned char _border_x;
unsigned char _type;
unsigned char _fontsize;

unsigned char _comconfig = 0; /// 0-1-2or3

#if defined(CH32X035) || defined(SOFTI2C)
void _oled_delay()
{
    // unsigned long t = 1;
    // while (t)
    // {
    //     t--;
    __asm volatile("nop");
    // }
}
void _oled_SDA(unsigned bit)
{
    if (bit)
        // GPIO_SetBits(GPIOA, SDA_pin);
        SSDGPIOPORT->BSHR = SDA_pin;

    else
        // GPIO_ResetBits(GPIOA, SDA_pin);
        SSDGPIOPORT->BCR = SDA_pin;
}
void _oled_SCL(unsigned char bit)
{
    if (bit)
        SSDGPIOPORT->BSHR = SCL_pin;

    // GPIO_SetBits(GPIOA, SCL_pin);
    else
        // GPIO_ResetBits(GPIOA, SCL_pin);
        SSDGPIOPORT->BCR = SCL_pin;
}
void _oled_sclrun()
{
    // GPIO_ResetBits(GPIOA, SCL_pin);
    // _oled_delay();
    // GPIO_SetBits(GPIOA, SCL_pin);
    SSDGPIOPORT->BSHR = SCL_pin;

    _oled_delay();
    // GPIO_ResetBits(GPIOA, SCL_pin);
    SSDGPIOPORT->BCR = SCL_pin;
    _oled_delay();
}

void _oled_writebyte(unsigned char d)
{
    // static unsigned char
    for (unsigned char i = 0; i < 8; i++)
    {
        _oled_SDA(d & (0x80 >> i));
        // if (d & (0x80 >> i))
        //     // GPIO_SetBits(GPIOA, SDA_pin);
        //     GPIOA->BSHR = SDA_pin;
        // else
        //     // GPIO_ResetBits(GPIOA, SDA_pin);
        //     GPIOA->BCR = SDA_pin;

        _oled_sclrun();
    }
}

void _OLED_i2c_write(unsigned char value, unsigned char control_byte)
{
    _oled_SDA(0);
    _oled_delay();
    _oled_SCL(0);
    _oled_writebyte(_addr_i2c);
    _oled_sclrun();

    _oled_writebyte(control_byte);
    _oled_sclrun();

    _oled_writebyte(value);
    _oled_sclrun();

    _oled_delay();
    _oled_delay();

    _oled_SCL(1);
    _oled_delay();
    _oled_delay();
    _oled_SDA(1);
}
#else
void _OLED_i2c_write(unsigned char value, unsigned char control_byte)
{
    unsigned long timeout = 0;
    // unsigned short timeout = 0;
    // while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
    while ((I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) && (timeout < TIMOUT_WAITS))
        // ;
        timeout++;
    // I2C_GenerateSTART(ENABLE);
    I2C_GenerateSTART(I2C1, ENABLE);

    // while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout < TIMOUT_WAITS))
        // ;
        timeout++;

    I2C_Send7bitAddress(I2C1, _addr_i2c, I2C_Direction_Transmitter);

    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout < TIMOUT_WAITS))
        // ;
        timeout++;

    I2C_SendData(I2C1, control_byte);

    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) && (timeout < TIMOUT_WAITS))
        // ;
        timeout++;

    I2C_SendData(I2C1, value);
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout < TIMOUT_WAITS))
        // while ((!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout < 2000)))
        timeout++;
    // ;

    I2C_GenerateSTART(I2C1, DISABLE);

    I2C_GenerateSTOP(I2C1, ENABLE);
}
#endif

void OLED_begin(unsigned char xsize, unsigned char ysize, unsigned char addr, unsigned char comconf)
{

    _x_size = xsize;
    _y_size = ysize;
    _addr_i2c = addr;
    _comconfig = comconf;

    _addr_i2c = _addr_i2c << 1; // shift 1 bit
    _char_x = sizeof(font[1]) / _char_y;
    _border_x = (_x_size % _char_x) / 2;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_Port, ENABLE);

    GPIO_InitTypeDef gpioi2c;
#if defined(CH32X035) || defined(SOFTI2C)
    // gpioi2c.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioi2c.GPIO_Mode = GPIO_Mode_Out_PP;
#else
    gpioi2c.GPIO_Mode = GPIO_Mode_AF_OD;
    // gpioi2c.GPIO_Mode = GPIO_Mode_AF_PP;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

#endif

    gpioi2c.GPIO_Pin = SCL_pin;
    // gpioi2c.GPIO_Speed = GPIO_Speed_2MHz;
    gpioi2c.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SSDGPIOPORT, &gpioi2c);

    gpioi2c.GPIO_Pin = SDA_pin;

    GPIO_Init(SSDGPIOPORT, &gpioi2c);
#if !defined(CH32X035) && !defined(SOFTI2C)

    I2C_InitTypeDef i2cinit;
    // i2cinit.I2C_ClockSpeed = 800000; // doesnt; seem to run in 1mbit, but runs at 800kbit? twice as fast as 400kbit
    i2cinit.I2C_ClockSpeed = 400000;
    i2cinit.I2C_Mode = I2C_Mode_I2C;
    i2cinit.I2C_DutyCycle = I2C_DutyCycle_2;
    i2cinit.I2C_OwnAddress1 = 0x01;
    i2cinit.I2C_Ack = I2C_Ack_Enable;
    // i2cinit.I2C_Ack = I2C_Ack_Disable;
    i2cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_DeInit(I2C1);
    I2C_Init(I2C1, &i2cinit);
    I2C_Cmd(I2C1, ENABLE);
    // setup gpio port
    // GPIO_Init(SCL_pin, GPIO_MODE_OUT_OD_HIZ_FAST);
    // GPIO_Init(SDA_pin, GPIO_MODE_OUT_OD_HIZ_FAST);

    // // setup i2c
    // I2C_DeInit();

    // I2C_Init(
    //     400000, // I2C_SPEED, max
    //     // 100000,		// I2C_SPEED, standard
    //     0x01,            // OwnAddress, doesn't matter
    //     I2C_DUTYCYCLE_2, // 0x00
    //     // I2C_ACK_NONE,
    //     I2C_ACK_CURR, // 0x01
    //     // I2C_ACK_NEXT,     // 0x01
    //     I2C_ADDMODE_7BIT, // 0x00
    //     F_CPU / 1000000u  // InputClockFrequencyMhz
    // );

    // disable all i2c the interrupts
    // I2C_ITConfig(I2C_IT_ERR, DISABLE);
    // I2C_ITConfig(I2C_IT_EVT, DISABLE);
    // I2C_ITConfig(I2C_IT_BUF, DISABLE);
    // I2C_Cmd(ENABLE);

    // block delay till i2c is up and running
    for (long i = 0; i < 30000; i++)
    {
        ;
        __asm volatile("nop");
    }
#endif

    // initialize the display, see datasheet block diagram
    _OLED_i2c_write((Set_Display_ON_or_OFF_CMD | Display_OFF), CMD);
    _OLED_i2c_write(Set_Multiplex_Ratio_CMD, CMD);
    _OLED_i2c_write(_y_size - 1, CMD);
    // _OLED_i2c_write(0x20-1, CMD);

    _OLED_i2c_write(Set_Display_Offset_CMD, CMD);
    _OLED_i2c_write(0x00, CMD);
    _OLED_i2c_write(Set_Display_Start_Line_CMD, CMD);
    _OLED_i2c_write(0x00, CMD);

    _OLED_i2c_write((Set_Segment_Remap_CMD | Column_Address_0_Mapped_to_SEG127), CMD);
    _OLED_i2c_write((Set_COM_Output_Scan_Direction_CMD | Scan_from_COM63_to_0), CMD);
    _OLED_i2c_write(Set_Common_HW_Config_CMD, CMD);
    // _OLED_i2c_write(0x00, CMD);

    _OLED_i2c_write(((_comconfig << 4) | 0x02), CMD);
    // was in : https://embedded-lab.com/blog/stm8-microcontrollers-final-chapters/3/
    //  _OLED_i2c_write(0x12, CMD);

    // start with contr at full
    _OLED_i2c_write(Set_Contrast_Control_CMD, CMD);
    _OLED_i2c_write(0xFF, CMD);

    _OLED_i2c_write(Set_Entire_Display_ON_CMD, CMD);
    _OLED_i2c_write(Set_Normal_or_Inverse_Display_CMD, CMD);
    // display clock doesn't seem to change anything
    _OLED_i2c_write(Set_Display_Clock_CMD, CMD);
    _OLED_i2c_write(0x80, CMD);
    _OLED_i2c_write(Set_Pre_charge_Period_CMD, CMD);
    _OLED_i2c_write(0x25, CMD);
    _OLED_i2c_write(Set_VCOMH_Level_CMD, CMD);
    _OLED_i2c_write(0x20, CMD);
    _OLED_i2c_write(Set_Page_Address_CMD, CMD);
    _OLED_i2c_write(0x00, CMD);
    if (_y_size == 32)
    {
        _OLED_i2c_write(0x03, CMD);
    }
    else
    {
        _OLED_i2c_write(0x07, CMD);
    }
    _OLED_i2c_write(Set_Page_Start_Address_CMD, CMD);
    _OLED_i2c_write(Set_Higher_Column_Start_Address_CMD, CMD);
    _OLED_i2c_write(Set_Lower_Column_Start_Address_CMD, CMD);
    _OLED_i2c_write(Set_Memory_Addressing_Mode_CMD, CMD);
    // _OLED_i2c_write(Horizontal_Addressing_Mode, CMD);
    _OLED_i2c_write(Page_Addressing_Mode, CMD);

    _OLED_i2c_write(Set_Charge_Pump_CMD, CMD);
    _OLED_i2c_write(Enable_Charge_Pump, CMD);
    _OLED_i2c_write((Set_Display_ON_or_OFF_CMD | Display_ON), CMD);

    OLED_clear();

    OLED_setCursor(0, 0);

    _fontsize = sizeof(font) / sizeof(font[0]);
}

// void _OLED_i2c_write(unsigned char value, unsigned char control_byte)
// {
//     unsigned short timeout = 0;
//     while ((I2C_GetFlagStatus(I2C_FLAG_BUSBUSY) && (timeout < 400)))
//         timeout++;
//     I2C->CR2 = 0;

//     I2C_GenerateSTART(ENABLE);
//     I2C->CR2 = 0;

//     while ((!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) && (timeout < 800))
//         timeout++;
//     I2C->CR2 = 0;

//     I2C_Send7bitAddress(_addr_i2c, I2C_DIRECTION_TX);
//     I2C->CR2 = 0;
//     (void)I2C->SR3; // read SR3 to clear ADDR event bit

//     while ((!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (timeout < 1200)))
//         timeout++;

//     I2C_SendData(control_byte);

//     while ((!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)) && (timeout < 1600))
//         timeout++;
//     I2C->CR2 = 0;

//     I2C_SendData(value);
//     while ((!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout < 2000)))
//         timeout++;

//     I2C->CR2 = 0;
//     I2C_GenerateSTART(DISABLE);

//     I2C_GenerateSTOP(ENABLE);
//     if (timeout > 1000)
//         I2C_ClearFlag(0xffff);

//     I2C->CR2 = 0;
// }

// a col here is just a pixel, not as a char width....
void _OLED_goto(unsigned char col, unsigned char page)
{
    _OLED_i2c_write((Set_Page_Start_Address_CMD + page), CMD);
    _OLED_i2c_write(((col & 0x0F) | Set_Lower_Column_Start_Address_CMD), CMD);
    _OLED_i2c_write((((col & 0xF0) >> 0x04) | Set_Higher_Column_Start_Address_CMD), CMD);
}

void OLED_setCursor(unsigned char c, unsigned char r)
{
    _pos_x = _border_x + (c * (_char_x + 1)); //+1 for space between chars
    _pos_y = r * _char_y;
    _OLED_goto(_pos_x, _pos_y);
}

void _OLED_print_char(unsigned char chr)
{
    chr -= 0x20;
    if (chr > _fontsize)
        chr -= 0x20;
#if fontheight < 2
    // in "horizontal ddr mode" we don;t need to keep track of the pos in pixels
    for (unsigned char i = 0; i < _char_x; i++)
    {
        _OLED_i2c_write(font[chr][i], DAT);
        // _pos_x++;
    }
    // write empty col of 1 pixel:
    _OLED_i2c_write(0x00, DAT);
    // _pos_x++;
#endif
#if fontheigt == 2
    _OLED_goto(_pos_x, _pos_y); // go to next col, << col

    // now we need to keep track of col and pages...
    for (unsigned char b = 0; b < _char_x; b++)
    {
        _OLED_i2c_write(font[chr][b], DAT);
        // _pos_x++;
    }
    _OLED_goto(_pos_x, _pos_y + 1); // go to next col, << col
    for (unsigned char b = 0; b < _char_x; b++)
    {
        _OLED_i2c_write(font[chr][_char_x + b], DAT);
        // _pos_x++;
    }
    _pos_x += _char_x;
    _OLED_goto(_pos_x, _pos_y); // go to next col, << col
#endif
}

void OLED_write(unsigned char chr)
{
    chr -= 0x20; // first 32 control chars
    while (chr > (short)sizeof(font) / sizeof(font[0]))
    {
        chr -= 0x20; // if no lowercase, map lowercase to uppercase
    }
    _OLED_print_char(chr);
}

void OLED_print_s(char *ch)
{
    do
    {
        _OLED_print_char(*ch++);
    } while ((*ch >= 0x20) && (*ch <= 0x7F) && (*ch != '\n'));
}

void OLED_print_ub(unsigned long value, unsigned char base)
{
    char ch[14];
    unsigned char pos = sizeof(ch) - 1;
    ch[pos] = '\n';
    if (base < 1)
        base = 10;
    do
    {
        unsigned char num = value % base;
        pos--;
        if (num < 10)
            ch[pos] = num | 0x30;
        else
            ch[pos] = num + 0x37;
        value /= base;

    } while (value);
    OLED_print_s(&ch[pos]);
}

void OLED_print_ib(signed long value, unsigned char base)
{

    if (value < 0)
    {
        value = -value;
        OLED_print_s("-");
    }
    OLED_print_ub(value, base);
}

void OLED_print_i(signed long value)
{
    OLED_print_ib(value, 10);
}

void OLED_print_u(unsigned long value)
{
    OLED_print_ub(value, 10);
}

void OLED_clear()
{
    // just write zeros to entire display
    for (unsigned char i = 0; i < _y_size / 8; i++)
    {
        _OLED_goto(0, i);
        for (unsigned char j = 0; j < 128; j++)
        {
            _OLED_i2c_write(0x00, DAT);
        }
    }
}
