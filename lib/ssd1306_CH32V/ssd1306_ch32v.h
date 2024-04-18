/*
guus okt 2023
*/

#ifndef ssd1306_ch32v_h
#define ssd1306_ch32v_h

#if defined(CH32V00X)
#include <ch32v00x.h>
#include <ch32v00x_gpio.h>
#include <ch32v00x_rcc.h>
#include <ch32v00x_i2c.h>
#define SSDGPIOPORT GPIOC
#define RCC_APB2Periph_Port RCC_APB2Periph_GPIOC
#define SCL_pin GPIO_Pin_1
#define SDA_pin GPIO_Pin_2
#endif

#if defined(CH32X035)
#include <ch32x035.h>
#include <ch32x035_gpio.h>
#include <ch32x035_rcc.h>
#include <ch32x035_i2c.h>
#define SSDGPIOPORT GPIOA
#define RCC_APB2Periph_Port RCC_APB2Periph_GPIOA
#define SCL_pin GPIO_Pin_10
#define SDA_pin GPIO_Pin_11


#endif

#if defined(CH32V10X)
#include <ch32v10x.h>
#include <ch32v10x_gpio.h>
#include <ch32v10x_rcc.h>
#include <ch32v10x_i2c.h>
#endif

#if defined(CH32V20X)
// #define SOFTI2C

#include <ch32v20x.h>
#include <ch32v20x_gpio.h>
#include <ch32v20x_rcc.h>
#include <ch32v20x_i2c.h>

#endif

#if defined(CH32V30X)
#include <ch32v30x.h>
#include <ch32v30x_gpio.h>
#include <ch32v30x_rcc.h>
#include <ch32v30x_i2c.h>
#endif

#if defined CH32V30X || CH32V20X || CH32V30X
#define SSDGPIOPORT GPIOB
#define RCC_APB2Periph_Port RCC_APB2Periph_GPIOB
#define SCL_pin GPIO_Pin_6
#define SDA_pin GPIO_Pin_7

#endif

#ifndef font
// #include "font_std.h"
// #include "font_edit.h"
// #include "font_5x7.h"
// #include "font_digital.h"
// #include "../../fonts/font_uc_ls.h"
// #include "font_uc_ls.h"
// #include "font_double.h"
#include "font_double_uc.h"
#endif


// ssd1306 i2c comands
#define DAT 0x60
#define CMD 0x00
#define Set_Lower_Column_Start_Address_CMD 0x00
#define Set_Higher_Column_Start_Address_CMD 0x10
#define Set_Memory_Addressing_Mode_CMD 0x20
#define Set_Column_Address_CMD 0x21
#define Set_Page_Address_CMD 0x22
#define Set_Display_Start_Line_CMD 0x40
#define Set_Contrast_Control_CMD 0x81
#define Set_Charge_Pump_CMD 0x8D
#define Set_Segment_Remap_CMD 0xA0
#define Set_Entire_Display_ON_CMD 0xA4         // sh1106 0xAF
#define Set_Entire_Display_OFF_CMD 0xA3        // sh1106 0xAE
#define Set_Normal_or_Inverse_Display_CMD 0xA6 // 0xa6 == normal/0xa7 inverse
#define Set_Multiplex_Ratio_CMD 0xA8
#define Set_Display_ON_or_OFF_CMD 0xAE
#define Set_Page_Start_Address_CMD 0xB0
#define Set_COM_Output_Scan_Direction_CMD 0xC0
#define Set_Display_Offset_CMD 0xD3
#define Set_Display_Clock_CMD 0xD5
#define Set_Pre_charge_Period_CMD 0xD9
#define Set_Common_HW_Config_CMD 0xDA
#define Set_VCOMH_Level_CMD 0xDB
#define Set_NOP_CMD 0xE3

// ssd1306 modes
#define Horizontal_Addressing_Mode 0x00
#define Vertical_Addressing_Mode 0x01
#define Page_Addressing_Mode 0x02
#define Disable_Charge_Pump 0x00
#define Enable_Charge_Pump 0x14
#define Column_Address_127_Mapped_to_SEG0 0x00
#define Column_Address_0_Mapped_to_SEG127 0x01
#define Normal_Display 0x00
#define Entire_Display_ON 0x01
#define Non_Inverted_Display 0x00
#define Inverted_Display 0x01
#define Display_OFF 0x00
#define Display_ON 0x01
#define Scan_from_COM0_to_63 0x00
#define Scan_from_COM63_to_0 0x08

extern unsigned char _char_y;

extern unsigned char _pos_x;
extern unsigned char _pos_y;

// void _OLED_i2c_write(unsigned char value, unsigned char control_byte);
// void _OLED_goto(unsigned char col, unsigned char page);
// void _OLED_print_char(unsigned char ch);

void OLED_begin(unsigned char xzise, unsigned char yzise, unsigned char addr, unsigned char comconf);

// void OLED_begin_charsize(unsigned char f);

void OLED_clear(void);
void OLED_setCursor(unsigned char c, unsigned char r);

// void OLED_command(unsigned char arg);

void OLED_write(unsigned char val);

void OLED_print_i(signed long value);
void OLED_print_ib(signed long value, unsigned char base);
void OLED_print_u(unsigned long value);
void OLED_print_ub(unsigned long value, unsigned char base);
void OLED_print_s(char *p);

// void OLED_invert(void);
void OLED_invert(unsigned char inv);
void OLED_setContrast(unsigned char c);
void OLED_display(void);
void OLED_noDisplay(void);
void OLED_rotate(unsigned char rot);

void OLED_display_arr(const unsigned char *p, unsigned char x, unsigned char y);

void _OLED_i2c_write(unsigned char value, unsigned char control_byte);

#define OLED_home() OLED_setCursor(0, 0)

#endif
