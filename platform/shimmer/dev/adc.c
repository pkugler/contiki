#include "adc.h"
#include "contiki.h"

static unsigned char num_ports;

void adc_reset(void)
{
    num_ports = 0;

    // disable ADC
    ADC12CTL0 = 0;
}

void adc_add(unsigned char pin)
{
    switch (num_ports) {
    case 0:
        // enable ADC when first pin is added
        ADC12CTL0 = ADC12ON | REF2_5V | MSC | SHT0_0 | SHT1_0;
        ADC12CTL1 = SHP | CONSEQ0 | ADC12SSEL0 | ADC12SSEL1 | ADC12DIV_0 ;
        ADC12MCTL0 = (pin & 0x07) | EOS;
        break;

    case 1:
        ADC12MCTL0 &= ~EOS;
        ADC12MCTL1 = (pin & 0x07) | EOS;
        break;

    case 2:
        ADC12MCTL1 &= ~EOS;
        ADC12MCTL2 = (pin & 0x07) | EOS;
        break;

    case 3:
        ADC12MCTL2 &= ~EOS;
        ADC12MCTL3 = (pin & 0x07) | EOS;
        break;

    case 4:
        ADC12MCTL3 &= ~EOS;
        ADC12MCTL4 = (pin & 0x07) | EOS;
        break;

    case 5:
        ADC12MCTL4 &= ~EOS;
        ADC12MCTL5 = (pin & 0x07) | EOS;
        break;

    case 6:
        ADC12MCTL5 &= ~EOS;
        ADC12MCTL6 = (pin & 0x07) | EOS;
        break;

    case 7:
        ADC12MCTL6 &= ~EOS;
        ADC12MCTL7 = (pin & 0x07) | EOS;
        break;

    default:
        // no more ADC channels available
        return;
    }

    num_ports++;
}

void adc_start(void)
{
    ADC12CTL0 |= ENC;
    ADC12CTL0 |= ADC12SC;
}

unsigned short adc_get(unsigned char channel)
{
    switch (channel) {
    case 0:
        return ADC12MEM0;

    case 1:
        return ADC12MEM1;

    case 2:
        return ADC12MEM2;

    case 3:
        return ADC12MEM3;

    case 4:
        return ADC12MEM4;

    case 5:
        return ADC12MEM5;

    case 6:
        return ADC12MEM6;

    case 7:
        return ADC12MEM7;
    }

    return 0;
}

unsigned char adc_num(void)
{
    return num_ports;
}
