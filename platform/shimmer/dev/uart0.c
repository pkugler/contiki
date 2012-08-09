#include "contiki.h"
#include "isr_compat.h"
#include "dev/hwconf.h"
#include "dev/uart0.h"
#include "lib/ringbuf.h"

HWCONF_PIN(SIMO0, 3, 1)
HWCONF_PIN(SOMI0, 3, 2)
HWCONF_PIN(UCLK0, 3, 3)

enum Mode
{
  MODE_NONE, MODE_UART, MODE_SPI, MODE_I2C
};

static enum Mode mode = MODE_NONE;

static int (*uart0_input_handler)(unsigned char c);

static uart0_i2c_callback transfer_handler;
static volatile uint8_t rx_in_progress;
static volatile uint8_t transmitting;

#define TXBUFSIZE 128
static struct ringbuf txbuf;
static uint8_t txbuf_data[TXBUFSIZE];
static const uint8_t *txbuf_beg;
static uint8_t *rxbuf_beg;
static uint8_t txbuf_size;
static uint8_t txbuf_pos;

uint8_t
uart0_active(void)
{
  return ((~UTCTL0) & TXEPT) | rx_in_progress | transmitting;
}

void
uart0_set_input(int (*input)(unsigned char c))
{
  uart0_input_handler = input;
}

void
uart0_writeb(unsigned char c)
{
  /* Put the outgoing byte on the transmission buffer. If the buffer
   is full, we just keep on trying to put the byte into the buffer
   until it is possible to put it there. */
  while (ringbuf_put(&txbuf, c) == 0)
    ;

  /* If there is no transmission going, we need to start it by putting
   the first byte into the UART. */
  if (transmitting == 0) {
    transmitting = 1;
    TXBUF0 = ringbuf_get(&txbuf);
  }
}

void
uart0_start(uint8_t ubr0, uint8_t ubr1, uint8_t umctl)
{
  mode = MODE_UART;

  P3DIR &= ~0x20;
  P3DIR |= 0x10;
  P3SEL |= 0x30;

  UCTL0 = SWRST | CHAR;
  UTCTL0 = SSEL1;

  UBR00 = ubr0;
  UBR10 = ubr1;
  UMCTL0 = umctl;

  ME1 &= ~USPIE0;
  ME1 |= (UTXE0 | URXE0);

  UCTL0 &= ~SWRST;

  /* clear pending interrupts before enable */
  IFG1 &= ~URXIFG0;
  U0TCTL |= URXSE;

  rx_in_progress = 0;
  transmitting = 0;

  ringbuf_init(&txbuf, txbuf_data, sizeof(txbuf_data));
  IE1 |= URXIE0 | UTXIE0;
}

void
uart0_spi_start(void)
{
  mode = MODE_SPI;
  UCTL0 = SWRST;
  UCTL0 |= SYNC | CHAR | MM;

  //UxTCTL = STC;

  /* configure MOSI and clock as output and MISO as input */
  SOMI0_SELECT_PM();
  SOMI0_MAKE_INPUT();

  SIMO0_SELECT_PM();
  SIMO0_MAKE_OUTPUT();

  UCLK0_SELECT_PM();
  UCLK0_MAKE_OUTPUT();

  /* enable SPI */
  UCTL0 &= ~SWRST;
}

void
uart0_spi_stop(void)
{
  mode = MODE_NONE;
  UCTL0 = 0;
  UCTL0 = SWRST;

  /* set SPI pins to input mode to save power */
  SOMI0_SELECT_IO();
  SOMI0_MAKE_INPUT();

  SIMO0_SELECT_IO();
  SIMO0_MAKE_INPUT();

  UCLK0_SELECT_IO();
  UCLK0_MAKE_INPUT();
}

void
uart0_i2c_start(void)
{
  mode = MODE_I2C;
  UCTL0 = SWRST;
  UCTL0 |= SYNC | I2C;
  UCTL0 &= ~I2CEN;

  P3SEL |= 0x0A;
  P3DIR &= ~0x0A;

  I2CTCTL |= I2CSSEL1 | I2CRM; // select clock source
  I2CPSC = 3;
  I2CSCLH = 3;
  I2CSCLL = 3;
  I2COA = 0;
  UCTL0 |= I2CEN;
}

void
uart0_i2c_stop(void)
{
  mode = MODE_NONE;
  UCTL0 = 0;
  UCTL0 = SWRST;
  P3SEL &= ~0x0A;
  P3DIR &= ~0x0A;
}

void
uart0_i2c_write(uint8_t address, const void *data, uint8_t size,
    uart0_i2c_callback callback)
{
  txbuf_beg = data;
  txbuf_size = size;
  txbuf_pos = 0;
  transfer_handler = callback;

  UCTL0 |= MST; // set master
  I2CTCTL |= I2CTRX; // transmit mode
  I2CSA = address;
  I2CIE |= TXRDYIE; // tx ready irq
  I2CIE |= ARDYIE; // access ready irq
  I2CIE |= NACKIE; // no ack irq
  I2CTCTL |= I2CSTT;
}

void
uart0_i2c_read(uint8_t address, void *data, uint8_t size,
    uart0_i2c_callback callback)
{
  rxbuf_beg = data;
  txbuf_size = size;
  txbuf_pos = 0;
  transfer_handler = callback;

  UCTL0 |= MST; // set master
  I2CTCTL &= ~I2CTRX; // receive mode
  I2CSA = address;
  I2CIE |= RXRDYIE; // rx ready irq
  I2CIE |= ARDYIE; // access ready irq
  I2CIE |= NACKIE; // no ack irq
  I2CTCTL |= I2CSTT;
}

static void
end_transmit(uint8_t success)
{
  I2CIE = 0;
  if (transfer_handler) {
    transfer_handler(success);
  }
}

ISR(UART0RX, uart0_rx_interrupt)
{
  uint8_t c;
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if (!(URXIFG0 & IFG1)) {
    /* Edge detect if IFG not set? */
    U0TCTL &= ~URXSE; /* Clear the URXS signal */
    U0TCTL |= URXSE; /* Re-enable URXS - needed here?*/
    rx_in_progress = 1;
    LPM4_EXIT;
  } else {
    rx_in_progress = 0;
    /* Check status register for receive errors. */
    if (URCTL0 & RXERR) {
      c = RXBUF0; /* Clear error flags by forcing a dummy read. */
    } else {
      c = RXBUF0;
      if (uart0_input_handler != NULL) {
        if (uart0_input_handler(c)) {
          LPM4_EXIT;
        }
      }
    }
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

ISR(UART0TX, uart0_tx_interrupt)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  switch (mode) {
  case MODE_NONE:
    break;

  case MODE_UART:
    if (ringbuf_elements(&txbuf) == 0) {
      transmitting = 0;
    } else {
      TXBUF0 = ringbuf_get(&txbuf);
    }
    break;

  case MODE_SPI:
    break;

  case MODE_I2C:
    switch (I2CIV) {
    case I2CIV_NACK:
      end_transmit(0);
      break;

    case I2CIV_ARDY:
      end_transmit(1);
      break;

    case I2CIV_TXRDY:
      if (txbuf_pos + 1 == txbuf_size) {
        I2CTCTL |= I2CSTP;
      } else if (txbuf_pos == txbuf_size) {
        end_transmit(1);
        break;
      }

      I2CDRB = txbuf_beg[txbuf_pos++];
      break;

    case I2CIV_RXRDY:
      rxbuf_beg[txbuf_pos++] = I2CDRB;
      if (txbuf_pos == txbuf_size) {
        I2CTCTL |= I2CSTP;
        end_transmit(1);
      }
      break;
    }
    break;
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
