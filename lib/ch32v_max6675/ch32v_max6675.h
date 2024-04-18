/*
guus 2024
very simple spi driver for max6675 thermocouple meter.
used for ch32v203, might work on other ch32v mcus
Regarding the cs line (pin7) of the max6675, you are 
responsible for setting this before getting a temp value */

#ifndef ch32v_max6675_h
#define ch32v_max6675_h

#include <ch32v20x.h>
#include <ch32v20x_gpio.h>
#include <ch32v20x_rcc.h>
#include <ch32v20x_spi.h>

#define SPIGPIOPORT GPIOA
// #define RCC_APB2Periph_GPIOPORT RCC_APB2Periph_GPIOA
#define GPIOCLK GPIO_Pin_5
#define GPIOMISO GPIO_Pin_7

#define max6675_BIT_DUMMY 0x8000
#define max6675_BIT_OPEN 0x0004
#define max6675_BIT_ID 0x0002
#define max6675_BIT_STATE 0x0001

// sets up the spi peri
void max6675_begin();
// get the raw 1/4C data, -1 for readfault/ not connected thermocouple
signed short max6675_getraw();
// get the data in integer, roudned to 1
signed short max6675_getCshort();
#ifdef USEFLOAT
// get the data in float
float max6675_getcFloat();
#endif
#endif