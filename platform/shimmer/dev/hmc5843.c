#include "contiki.h"
#include "hmc5843.h"
#include "hwconf.h"

#define HMC5843_ID (0x1e)
HWCONF_PIN(GYRO_PWREN_N, 1, 1)

static uint8_t reg;

void hmc5843_enable(void)
{
    hmc5843_disable();
    GYRO_PWREN_N_CLEAR();
    udelay(5000);
}

void hmc5843_disable(void)
{
    GYRO_PWREN_N_SELECT_IO();
    GYRO_PWREN_N_MAKE_OUTPUT();
    GYRO_PWREN_N_SET();
    udelay(5000);
}

void hmc5843_prepare_read(void)
{
    reg = 0x03; // first data register
    uart0_i2c_write(HMC5843_ID, &reg, 1, NULL);
}

void hmc5843_read_data(uint8_t *buffer, uint8_t size, uart0_i2c_callback callback)
{
    uart0_i2c_read(HMC5843_ID, buffer, size, callback);
}
