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
 * \param success       Indicates if the transfer completed successfully.
 *                      Failures may be caused by an invalid slave address
 *                      or a defective slave device.
 */
typedef void (*uart0_i2c_callback)(uint8_t success);

/**
 * Start UART0 in UART mode
 * \param ubr0  Value for baudrate register 0
 * \param ubr1  Value for baudrate register 1
 * \param umctl Value for modulation register
 */
void
uart0_start(uint8_t ubr0, uint8_t ubr1, uint8_t umctl);

/**
 * Stop UART0 UART mode
 *
 * Deinitializes UART0 and reset
 */
/**
 * Start UART0 in SPI mode
 */
void
uart0_spi_start(void);

/**
 * Stop UART0 SPI mode
 *
 * Deinitializes UART0 and resets SPI pins.
 * \note Only SPI pins will be reset, not UART RX/TX!
 */
void
uart0_spi_stop(void);

/**
 * Start UART0 in I2C mode
 */
void
uart0_i2c_start(void);

/**
 * Stop UART0 I2C mode
 *
 * Deinitializes UART0 and resets I2C pins.
 * \note Only I2C pins will be reset, not UART RX/TX or SIMO!
 */
void
uart0_i2c_stop(void);

/**
 * Send data to a specific slave device
 *
 * \param address       Slave address
 * \param data          Pointer to data buffer
 * \param size          Size of data buffer in bytes
 * \param callback      Callback function, will be called when transfer is
 *                      completed, either successfully or with an error
 */
void
uart0_i2c_write(uint8_t address, const void *data, uint8_t size,
    uart0_i2c_callback callback);

/**
 * Receive data from specific slave device
 *
 * \param address       Slave address
 * \param data          Pointer to data buffer
 * \param size          Size of data buffer in bytes
 * \param callback      Callback function, will be called when transfer is
 *                      completed, either successfully or with an error
 */
void
uart0_i2c_read(uint8_t address, void *data, uint8_t size,
    uart0_i2c_callback callback);

void
uart0_set_input(int
(*input)(unsigned char c));

void
uart0_writeb(unsigned char c);

uint8_t
uart0_active(void);

#endif // UART0_H
