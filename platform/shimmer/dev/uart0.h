/*
 * The MSP UART0 peripheral supports UART, SPI and I2C modes.
 * Before using any of these modes, the corresponding <mode>_start()
 * function has to be called. Before switching to another mode
 * the <mode>_stop() function of the currently running mode should
 * be called to ensure, that unused pins are reset to input mode,
 * reducing power consumption.
 */

#ifndef UART0_H
#define UART0_H

/**
 * I2C transfer callback function
 * @param success       Indicates if the transfer completed successfully.
 *                      Failures may be caused by an invalid slave address
 *                      or a defective slave device.
 */
typedef void (*uart0_i2c_callback)(uint8_t success);

void uart0_start(uint8_t ubr0, uint8_t ubr1, uint8_t umctl);
void uart0_stop(void);

void uart0_spi_start(void);
void uart0_spi_stop(void);

void uart0_i2c_start(void);
void uart0_i2c_stop(void);

void uart0_i2c_write(uint8_t address, const void *data, uint8_t size,
    uart0_i2c_callback callback);
void uart0_i2c_read(uint8_t address, void *data, uint8_t size,
    uart0_i2c_callback callback);

void
uart0_set_input(int
(*input)(unsigned char c));

void
uart0_writeb(unsigned char c);

uint8_t
uart0_active(void);

#endif // UART0_H
