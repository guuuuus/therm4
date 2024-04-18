#include "ch32v20x.h"
#include "usb_serial.h"
#include "ssd1306_ch32v.h"
#include "ch32v_max6675.h"
#include "millis.h"
#define MEASURE_INCREMENT 260 // interval for measurement in ms, mx6675 need at least 220?

#define VKHEADER                                 \
    {                                            \
        0x02, 0x80, 0x00, 0x01, 0x02, 0x02, 0x02 \
    }
#define VKTAIL                       \
    {                                \
        0x00, 0x00, 0x00, 0x00, 0x03 \
    }
#define PADZERO                                        \
    {                                                  \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
    }

typedef struct __attribute__((__packed__)) voltkraftReply_t
{
    unsigned char header[7];

    unsigned char currentData[4][2];

    unsigned char rel[8];
    unsigned char min[8];
    unsigned char max[8];

    unsigned char ol;
    unsigned char tail[5];

} voltkraftReply_t;

void enable_max6675(unsigned char num)
{
    // write alll to high
    for (unsigned char i = 0; i < 4; i++)
        GPIO_WriteBit(GPIOA, GPIO_Pin_0 << i, ENABLE);

    if (num < 4)
        GPIO_WriteBit(GPIOA, GPIO_Pin_0 << num, DISABLE);
}

void oled_print_pretty_i(unsigned long i, unsigned char positions)
{
    unsigned long max = 1;
    for (unsigned char i = 0; i < positions; i++)
        max = max * 10;
    while (max > i)
    {
        OLED_print_s(" ");
        max /= 10;
    }
    OLED_print_u(i);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    unsigned short count = 0;
    // systick
    millis_init();

    // max6675 enable pins
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    for (unsigned char i = 0; i < 4; i++)
    {
        GPIO_InitTypeDef enablepins;
        enablepins.GPIO_Mode = GPIO_Mode_Out_PP;
        enablepins.GPIO_Pin = GPIO_Pin_0 << i;
        enablepins.GPIO_Speed = GPIO_Speed_50MHz; //?
        GPIO_Init(GPIOA, &enablepins);
        GPIO_WriteBit(GPIOA, enablepins.GPIO_Pin, DISABLE);
    }

    // begin all the stuff
    usbSerial_begin();
    OLED_begin(128, 64, 0x3c, 1);
    //
    signed short measurements[4];
    unsigned long lastmeasure = millis() - MEASURE_INCREMENT;
    max6675_begin();
    while (1)
    {
        // get measurements
        if (millis() - lastmeasure > MEASURE_INCREMENT)
        {
            lastmeasure = millis();
            for (unsigned char i = 0; i < 4; i++)
            {
                enable_max6675(i);
                measurements[i] = max6675_getraw();
                delay(5);
            }
            enable_max6675(5); // disable all max's
        }
        // print on display
        for (unsigned char i = 0; i < 4; i++)
        {
            // OLED_print_ib(measurements[i], 16);
            OLED_setCursor(1, i);
            OLED_print_s("T");
            OLED_print_i(i + 1);
            OLED_print_s(": ");
            if (measurements[i] > 0)
            {
                oled_print_pretty_i(measurements[i] / 4, 3);
                OLED_print_s(".");
                OLED_print_i((measurements[i] % 4) * 25);
                if ((measurements[i] % 4) == 0)
                {
                    OLED_print_i(0);
                }
                OLED_print_s("C");
            }
            else if (measurements[i] == -1)
                OLED_print_s("Probe NC");

            else
            {
                OLED_print_s(" Error ");
                OLED_print_i(measurements[i]);
            }
        }

        // print on usb serial

        if (usbSerial_available()) //
        {
            unsigned char voltkraft = 0;
            // OLED_print_s("r");
            while (usbSerial_available())
            {
                unsigned char volatile d = usbSerial_read();
                if ((d == 'A') || (d == 'a'))
                    voltkraft = 1;
                // _OLED_print_char(d);
            }

            if (voltkraft)
            {
                voltkraftReply_t vk = {VKHEADER, {{0, 0}, {0, 0}, {0, 0}, {0, 10}}, PADZERO, PADZERO, PADZERO, 0x00, VKTAIL};
                for (unsigned char i = 0; i < 4; i++)
                {
                    if (measurements[i] > 0)
                    {
                        vk.currentData[i][0] = (0x7f & ((measurements[i] * 10) / 4) >> 8); // mask first bit for sign, if it is higher than +-3000C ??
                        vk.currentData[i][1] = 0xff & ((measurements[i] * 10) / 4);
                    }
                    else
                        vk.ol |= (0x01 << i);
                }

                usbSerial_writeP((unsigned char *)&vk, sizeof(vk));
                usbSerial_flush();
            }
            else
            {
                for (unsigned char i = 0; i < 4; i++)
                {
                    usbSerial_print_s("T");
                    usbSerial_print_i(i + 1);
                    usbSerial_print_s(": ");
                    if (measurements[i] > 0)
                    {
                        usbSerial_print_i(measurements[i] / 4);
                        usbSerial_print_s(".");
                        usbSerial_print_i((measurements[i] % 4) * 25);
                    }
                    else if (measurements[i] == -1)
                        usbSerial_print_s("probe nc");
                    else
                    {
                        usbSerial_print_s("error: ");
                        usbSerial_print_i(measurements[i]);
                    }
                    usbSerial_print_s(",");
                }
                usbSerial_println_s(" ");
                usbSerial_flush();
            }
        }

        delay(10);
    }
}