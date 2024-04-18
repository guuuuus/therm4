#include "ch32v_max6675.h"
#define SPI_TIMOUT_LOOPS 100000
// sets up the spi peri
void max6675_begin()
{
    GPIO_InitTypeDef gpiosck;
    GPIO_InitTypeDef gpiomiso;
    SPI_InitTypeDef spiinit;
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);

    spiinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spiinit.SPI_Mode = SPI_Mode_Master;
    spiinit.SPI_DataSize = SPI_DataSize_16b;
    // spiinit.SPI_DataSize = SPI_DataSize_8b;
    spiinit.SPI_CPOL = SPI_CPOL_Low;
    spiinit.SPI_CPHA = SPI_CPHA_1Edge;
    spiinit.SPI_NSS = SPI_NSS_Soft;
    if (clocks.SYSCLK_Frequency > 128000000)
        spiinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    else if (clocks.SYSCLK_Frequency > 64000000)
        spiinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    else
        spiinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // 3mhz
    spiinit.SPI_FirstBit = SPI_FirstBit_MSB;
    spiinit.SPI_CRCPolynomial = 7;

    gpiosck.GPIO_Pin = GPIOCLK;
    gpiomiso.GPIO_Pin = GPIOMISO;

    gpiosck.GPIO_Speed = GPIO_Speed_50MHz;
    gpiomiso.GPIO_Speed = GPIO_Speed_50MHz;

    gpiosck.GPIO_Mode = GPIO_Mode_AF_PP;
    gpiomiso.GPIO_Mode = GPIO_Mode_AF_OD;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_Init(SPIGPIOPORT, &gpiosck);
    GPIO_Init(SPIGPIOPORT, &gpiomiso);

    SPI_I2S_DeInit(SPI1);
    SPI_Init(SPI1, &spiinit);

    SPI_Cmd(SPI1, ENABLE);
}

// get the raw 1/4C data, -1 for readfault/ not connected thermocouple
signed short max6675_getraw()
{
    unsigned short u = 0x0000;
    signed short r = 0;
    // if previous end is complete
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
        ;

    SPI_I2S_SendData(SPI1, 0xffff); // write 1 short to trigger the clock for 16 cycles
    while ((!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)) || (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)))
        ;

    u = 0xffff & SPI1->DATAR;

    if (u & max6675_BIT_DUMMY)
        r = -2; // no device connected?

    else if (u & max6675_BIT_OPEN)
        r = -1; // no thermoal couple on device

    else
        r = (u >> 3) & 0x0fff;

    // r = u;
    return r;
}

signed short max6675_getCshort()
{
    signed short r = max6675_getraw();
    if (r >= 0)
        r = r >> 2;
    return r;
}

#ifdef USEFLOAT
float max6675_getcFloat()
{
    float r;
    signed short s = max6675_getraw();
    if (s >= 0)
        r = s / 4;
    else
        r = 0.0 / 0.0; // nan?
    return r;
}
#endif