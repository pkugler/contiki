#include "bluetooth.h"
#include "busmaster.h"
#include "dev/uart1.h"

static void (*connect)(unsigned char connected);

void bluetooth_enable(void (*connect_handler)(unsigned char connected), int (*data_handler)(unsigned char c))
{
    // first disable all devices on the bus including SPI
    uart1_disable_all();

    uart1_init(0x45);
    uart1_set_input(data_handler);

    // power up bluetooth module
    P4DIR |= 0x40;
    P4SEL &= ~0x40;
    P4OUT &= ~0x40;

    // disable bluetooth reset
    P5DIR |= 0x20;
    P5SEL &= ~0x20;
    P5OUT |= 0x20;

    // wait for reset to complete
    int16_t i;
    for (i = 0; i < 400; i++) {
      udelay(5000);
    }

    P1DIR |= 0x80;
    P1DIR &= ~0x40;
    P1SEL &= ~0xC0;

    // enable connect interrupt
    connect = connect_handler;
    P2DIR &= ~0x40;
    P2SEL &= ~0x40;
    if (P2IN & 0x40) {
        P2IES |= 0x40;
    } else {
        P2IES &= ~0x40;
    }
    P2IE |= 0x40;

    // rise cts to wake up device
    P1OUT &= ~0x80;
    P1OUT |= 0x80;

    // waking up from sleep mode can take up to 5ms
    udelay(5000);

    // tell bluetooth module that msp is ready
    P1OUT &= ~0x80;
}

void bluetooth_disable(void)
{
    // disable connect interrupt
    connect = NULL;
    P2IE &= ~0x40;

    // enable bluetooth reset
    P5DIR &= ~0x20;
    P5SEL &= ~0x20;
    P5OUT &= ~0x20;

    // power down bluetooth module
    P4DIR |= 0x40;
    P4SEL &= ~0x40;
    P4OUT |= 0x40;
}

void bluetooth_set_connected(int connected)
{
    if (connect) {
        connect(connected);
    }
}
