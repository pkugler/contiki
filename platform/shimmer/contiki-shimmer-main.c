#include "contiki.h"
#include "dev/adc.h"
#include "dev/bluetooth.h"
#include "dev/leds.h"
#include "dev/uart0.h"
#include "dev/uart1.h"
#include "dev/ds2411.h"
#include "dev/serial-line.h"
#include "dev/shimmerdock.h"
#include "dev/watchdog.h"
#include "isr_compat.h"

//SENSORS(NULL);

int putchar(int c)
{
    uart0_writeb(c);
    return c;
}

int dock_connected(void)
{
	if (P2IN & 0x08) {
		return 0;
	} else {
		return 1;
	}
}

ISR(PORT2, port2_interrupt)
{
  shimmerdock_isr();
  bluetooth_isr();

  // clear interrupts
  P2IFG = 0;
}

static void
msb_ports_init(void)
{
  P1SEL = 0x00; P1OUT = 0x00; P1DIR = 0x00;
  P2SEL = 0x00; P2OUT = 0x00; P2DIR = 0x00;
  P3SEL = 0x00; P3OUT = 0x00; P3DIR = 0x00;
  P4SEL = 0x00; P4OUT = 0x00; P4DIR = 0x00;
  P5SEL = 0x00; P5OUT = 0x00; P5DIR = 0x00;
  P6SEL = 0x00; P6OUT = 0x00; P6DIR = 0x00;
}

int
main(void)
{
  msp430_cpu_init();
  watchdog_stop();

  /* Platform-specific initialization. */
  msb_ports_init();
  adc_reset();

  clock_init();
  rtimer_init();

// use XT2 as main clock, set clock divder for SMCLK to 1
// MLCK:   8 MHz
// SMCLK:  8 MHz
// ACLK:  32.768 kHz
  BCSCTL1 = RSEL2 | RSEL1 | RSEL0;
  BCSCTL2 = SELM1 | SELS;// | DIVS0 | DIVS1;

//  sht11_init();
  leds_init();
  leds_on(LEDS_ALL);

  bluetooth_disable();

  // accelerometer g range select
  P4DIR |= 0x10;
  P4SEL &= ~0x10;
  P4OUT &= ~0x10;

  // accelerometer disable sleep
  P5DIR |= 0x01;
  P5SEL &= ~0x01;
  P5OUT |= 0x01;

  // accelerometer input pins
  P6DIR &= ~0x38;
  P6OUT &= ~0x38;
  P6SEL |= 0x38;

  ADC12CTL0 = 0;
  ADC12CTL0 = ADC12ON;// | SHT02;
  ADC12CTL1 = SHP | CONSEQ0;// ADC12SSEL0 | ADC12SSEL1;

  ADC12MCTL0 = INCH0 | INCH1;
  ADC12MCTL1 = INCH2 | EOS;
  ADC12MCTL2 = INCH2 | INCH0 | EOS;

  // start conversion
  ADC12CTL0 |= ENC | ADC12SC;

  process_init();

  /* System timers */
  process_start(&etimer_process, NULL);
  ctimer_init();

  leds_off(LEDS_ALL);

  ds2411_init();

  // University of California Berkeley's OUI
  ds2411_id[0] = 0x00;
  ds2411_id[1] = 0x12;
  ds2411_id[2] = 0x6d;

  // Following two octets must be 'LO' -- "local" in order to use UCB's OUI
  ds2411_id[3] = 'L';
  ds2411_id[4] = 'O';

  autostart_start(autostart_processes);

  /*
   * This is the scheduler loop.
   */
  ENERGEST_ON(ENERGEST_TYPE_CPU);

  while (1) {
    int r;
#if PROFILE_CONF_ON
    profile_episode_start();
#endif /* PROFILE_CONF_ON */
    do {
      /* Reset watchdog. */
      watchdog_periodic();
      r = process_run();
    } while(r > 0);

#if PROFILE_CONF_ON
    profile_episode_end();
#endif /* PROFILE_CONF_ON */

    /*
     * Idle processing.
     */
    int s = splhigh();		/* Disable interrupts. */
    if (process_nevents() != 0) {
      splx(s);			/* Re-enable interrupts. */
    } else {
      static unsigned long irq_energest = 0;
      /* Re-enable interrupts and go to sleep atomically. */
      ENERGEST_OFF(ENERGEST_TYPE_CPU);
      ENERGEST_ON(ENERGEST_TYPE_LPM);
     /*
      * We only want to measure the processing done in IRQs when we
      * are asleep, so we discard the processing time done when we
      * were awake.
      */
      energest_type_set(ENERGEST_TYPE_IRQ, irq_energest);
/*
      if (uart_edge) {
	_BIC_SR(LPM1_bits + GIE);
      } else {
	_BIS_SR(LPM1_bits + GIE);
      }
*/
      /*
       * We get the current processing time for interrupts that was
       * done during the LPM and store it for next time around. 
       */
      dint();
      irq_energest = energest_type_time(ENERGEST_TYPE_IRQ);
      eint();
      ENERGEST_OFF(ENERGEST_TYPE_LPM);
      ENERGEST_ON(ENERGEST_TYPE_CPU);
#if PROFILE_CONF_ON
      profile_clear_timestamps();
#endif /* PROFILE_CONF_ON */
    }
  }

  return 0;
}
