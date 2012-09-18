#include "sdcard.h"
#include "uart0.h"
#include "hwconf.h"

HWCONF_PIN(DOCK_N, 2, 3)
HWCONF_PIN(SW_SD_PWR_N, 4, 5)
HWCONF_PIN(SD_CS_N, 3, 0)

#define SPI_TX_DONE     do {} while ((U0TCTL & TXEPT) == 0)
#define CS_LOW()        SD_CS_N_CLEAR()
#define CS_HIGH()       do { SPI_TX_DONE; SD_CS_N_SET(); } while (0)

typedef uint8_t mmcerror_t;
#define MMC_SUCCESS           0x00
#define MMC_BLOCK_SET_ERROR   0x01
#define MMC_RESPONSE_ERROR    0x02
#define MMC_DATA_TOKEN_ERROR  0x03
#define MMC_INIT_ERROR        0x04
#define MMC_CRC_ERROR         0x10
#define MMC_WRITE_ERROR       0x11
#define MMC_OTHER_ERROR       0x12
#define MMC_TIMEOUT_ERROR     0xFF

#define MMC_GO_IDLE_STATE          0x40     //CMD0
#define MMC_SEND_OP_COND           0x41     //CMD1
#define MMC_READ_CSD               0x49     //CMD9
#define MMC_SEND_CID               0x4a     //CMD10
#define MMC_STOP_TRANSMISSION      0x4c     //CMD12
#define MMC_SEND_STATUS            0x4d     //CMD13
#define MMC_SET_BLOCKLEN           0x50     //CMD16 Set block length for next read/write
#define MMC_READ_SINGLE_BLOCK      0x51     //CMD17 Read block from memory
#define MMC_READ_MULTIPLE_BLOCK    0x52     //CMD18
#define MMC_CMD_WRITEBLOCK         0x54     //CMD20 Write block to memory
#define MMC_WRITE_BLOCK            0x58     //CMD24
#define MMC_WRITE_MULTIPLE_BLOCK   0x59     //CMD25
#define MMC_WRITE_CSD              0x5b     //CMD27 PROGRAM_CSD
#define MMC_SET_WRITE_PROT         0x5c     //CMD28
#define MMC_CLR_WRITE_PROT         0x5d     //CMD29
#define MMC_SEND_WRITE_PROT        0x5e     //CMD30
#define MMC_TAG_SECTOR_START       0x60     //CMD32
#define MMC_TAG_SECTOR_END         0x61     //CMD33
#define MMC_UNTAG_SECTOR           0x62     //CMD34
#define MMC_TAG_EREASE_GROUP_START 0x63     //CMD35
#define MMC_TAG_EREASE_GROUP_END   0x64     //CMD36
#define MMC_UNTAG_EREASE_GROUP     0x65     //CMD37
#define MMC_EREASE                 0x66     //CMD38
#define MMC_READ_OCR               0x67     //CMD39
#define MMC_CRC_ON_OFF             0x68     //CMD40

static int
data_handler(unsigned char c)
{
  return 0;
}

void
sdcard_start(void)
{
  uart0_start(0x45, 0x00, 0xAA);
  uart0_set_input(data_handler);
}

void
sdcard_stop(void)
{
  uart0_stop();
}

void
sdcard_restart(void)
{
  SD_CS_N_SELECT_IO();
  SD_CS_N_MAKE_OUTPUT();

  register uint8_t i;

  // wait until the tx buf is clear before killing the card
  CS_HIGH();

  // this connects the path from mcu to card
  DOCK_N_MAKE_OUTPUT();
  DOCK_N_SET();

  SW_SD_PWR_N_SET();
  SD_CS_N_CLEAR();

  /*
   * here we have to clear all input pins to the card, as
   * the card in spi mode will leech power from any pin
   */
  uart0_spi_stop();

  for(i = 0; i < 10; i++)
    udelay(6000);

  SD_CS_N_SET();
  SW_SD_PWR_N_CLEAR();

  // undo the override above
  //DOCK_N_MAKE_INPUT();
}

void
sdcard_send_command(const uint8_t cmd, uint32_t data, const uint8_t crc)
{
  uint8_t frame[6];
  register int8_t i;

  frame[0] = cmd | 0x40;
  for(i = 3; i >= 0; i--) {
    frame[4 - i] = (uint8_t)(data >> 8 * i);
  }

  frame[5] = crc;
  for(i = 0; i < 6; i++) {
    uart0_spi_write_byte(frame[i]);
  }
}

static enum SDCardStatus
sdcard_set_idle(void)
{
  CS_LOW();

  // put card in SPI mode
  sdcard_send_command(MMC_GO_IDLE_STATE, 0, 0x95);

  unsigned char response = uart0_spi_write_byte(0x00);
  if (response != 0x01) {
    return SDCARD_ERROR;
  }

//  do{
//    CS_HIGH();
//    spiSendByte(0xff);
//    CS_LOW();
//    sendCmd(MMC_SEND_OP_COND, 0x00, 0xff);
//  }while((response = getResponse()) == 0x01);

  CS_HIGH();

  return SDCARD_SUCCESS;
//  spiSendByte(0xff);

//  return MMC_SUCCESS;
}

enum SDCardStatus
sdcard_initialize(void)
{
  register uint8_t i;


  CS_HIGH();

  for (i = 0; i < 10; i++) {
    uart0_spi_write_byte(0xff);
  }

  enum SDCardStatus status;

  status = sdcard_set_idle();

  // here's where we set the clock speed up to smclk / 2 (512k)

  //call Usart.setUbr(0x0002);
  //call Usart.setUmctl(0x00);

  return status;
}
