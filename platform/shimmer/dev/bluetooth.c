#include "bluetooth.h"
#include "hwconf.h"
#include "isr_compat.h"
#include "dev/uart1.h"
#include "lib/ringbuf.h"

HWCONF_PIN(BT_RTS, 1, 6)
HWCONF_IRQ(BT_RTS, 1, 6)
HWCONF_PIN(BT_CTS, 1, 7)
HWCONF_PIN(BT_PIO, 2, 6)
HWCONF_IRQ(BT_PIO, 2, 6)
HWCONF_PIN(SW_BT_PWR_N, 4, 6)
HWCONF_PIN(BT_RESET, 5, 5)

static void (*connect)(unsigned char connected);

#define TXBUFSIZE 128

static struct ringbuf txbuf;
static uint8_t txbuf_data[TXBUFSIZE];
static volatile uint8_t transmitting;
static volatile uint8_t buffer_full;

static void process_connect_change()
{
  if (BT_PIO_READ()) {
    BT_PIO_IRQ_EDGE_SELECTD();
    if (connect) {
      connect(1);
    }
  } else {
    BT_PIO_IRQ_EDGE_SELECTU();
    if (connect) {
      connect(0);
    }
  }
}

void bluetooth_enable(void (*connect_handler)(unsigned char connected))
{
  // power up bluetooth module
  SW_BT_PWR_N_SELECT_IO();
  SW_BT_PWR_N_MAKE_OUTPUT();
  SW_BT_PWR_N_CLEAR();

  // disable bluetooth reset
  BT_RESET_SELECT_IO();
  BT_RESET_MAKE_OUTPUT();
  BT_RESET_SET();

  // wait for reset to complete
  int16_t i;
  for (i = 0; i < 400; i++) {
    udelay(5000);
  }

  // RTS is input
  BT_RTS_SELECT_IO();
  BT_RTS_MAKE_INPUT();
  if (BT_RTS_READ()) {
    BT_RTS_IRQ_EDGE_SELECTD();
  } else {
    BT_RTS_IRQ_EDGE_SELECTU();
  }
  BT_RTS_ENABLE_IRQ();

  // CTS is output
  BT_CTS_SELECT_IO();
  BT_CTS_MAKE_OUTPUT();
  BT_CTS_SET();

  // enable connect interrupt
  connect = connect_handler;
  BT_PIO_SELECT_IO();
  BT_PIO_MAKE_INPUT();
  process_connect_change();
  BT_PIO_ENABLE_IRQ();

  // rise cts to wake up device
  BT_CTS_CLEAR();
  BT_CTS_SET();

  // waking up from sleep mode can take up to 5ms
  udelay(5000);
}

void bluetooth_disable(void)
{
  // disable connect interrupt
  connect = NULL;
  BT_PIO_DISABLE_IRQ();

  // enable bluetooth reset
  BT_RESET_SELECT_IO();
  BT_RESET_MAKE_OUTPUT();
  BT_RESET_CLEAR();

  // power down bluetooth module
  SW_BT_PWR_N_SELECT_IO();
  SW_BT_PWR_N_MAKE_OUTPUT();
  SW_BT_PWR_N_SET();
}

void bluetooth_enable_communication(int (*data_handler)(unsigned char c))
{
  // 115200 bit/s at 8MHz clock
  uart1_init(0x45);
  uart1_set_input(data_handler);

  ringbuf_init(&txbuf, txbuf_data, sizeof(txbuf_data));
  IE2 |= UTXIE1;

  BT_CTS_CLEAR();
}

void bluetooth_disable_communication(void)
{
  BT_CTS_SET();
}

void bluetooth_writeb(unsigned char b)
{
  while (ringbuf_put(&txbuf, b) == 0);

  if (transmitting == 0 && !BT_RTS_READ()) {
    transmitting = 1;
    TXBUF1 = ringbuf_get(&txbuf);
  }
}

/**
 * Test if bluetooth transmission is completed
 */
uint8_t
bluetooth_active()
{
  return (((~ UTCTL1) & TXEPT) | (ringbuf_elements(&txbuf) > 0)) ;
}

/**
 * Tests IRQ state and calls configured connect_handler
 *
 * @param connected     Connection state of the device
 */
void
bluetooth_isr(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if (BT_PIO_CHECK_IRQ()) {
    process_connect_change();
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/**
 * Restart transmission if RTS goes low
 */
void
bluetooth_rts_isr(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if (BT_RTS_CHECK_IRQ()) {
    if (BT_RTS_READ()) {
      buffer_full = 1;
      BT_RTS_IRQ_EDGE_SELECTD();
    } else {
      buffer_full = 0;
      if (transmitting == 0 && ringbuf_elements(&txbuf) > 0) {
        transmitting = 1;
        TXBUF1 = ringbuf_get(&txbuf);
      }
      BT_RTS_IRQ_EDGE_SELECTU();
    }
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/**
 * Send next byte or disable transmission if RTS is high
 */
ISR(UART1TX, uart1_tx_interrupt)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if(ringbuf_elements(&txbuf) == 0) {
    transmitting = 0;
  } else if (!buffer_full) {
    TXBUF1 = ringbuf_get(&txbuf);
  } else {
    transmitting = 0;
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/* vim: set et ts=2 sw=2: */
