#include "contiki.h"
#include "gyro.h"

void gyro_enable(void)
{
    // enable gyro on extension board
    P1SEL &= ~0x10;
    P1DIR |= 0x10;
    P1OUT |= 0x10;

    P3SEL &= ~0x10;
    P3DIR |= 0x10;
    P3OUT &= ~0x10;

    P1SEL &= ~0x02;
    P1DIR |= 0x02;
    P1OUT |= 0x02;

    // wait for capacitors to discharge (6 seconds???)
    short i;
    for (i = 0; i < 6000; i++) {
        udelay(1000);
    }

    P1OUT &= ~0x02;
    P1OUT &= ~0x10;
}

void gyro_disable(void)
{
    P1SEL &= ~0x10;
    P1DIR |= 0x10;
    P1OUT |= 0x10;

    P3SEL &= ~0x10;
    P3DIR |= 0x10;
    P3OUT &= ~0x10;

    P1SEL &= ~0x02;
    P1DIR |= 0x02;
    P1OUT |= 0x02;
}
