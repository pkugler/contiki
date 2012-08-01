#include "contiki.h"
#include "hmc5843.h"
#include "hwconf.h"

#define HMC5843_ID (0x1e)
HWCONF_PIN(GYRO_PWREN_N, 1, 1)
static uint8_t command[2];

enum hmc5843_register
{
    HMC5843_CONFIG_A    =  0,
    HMC5843_CONFIG_B    =  1,
    HMC5843_MODE        =  2,
    HMC5843_OUTPUT_X_H  =  3,
    HMC5843_OUTPUT_X_L  =  4,
    HMC5843_OUTPUT_Y_H  =  5,
    HMC5843_OUTPUT_Y_L  =  6,
    HMC5843_OUTPUT_Z_H  =  7,
    HMC5843_OUTPUT_Z_L  =  8,
    HMC5843_STATUS      =  9,
    HMC5843_ID_A        = 10,
    HMC5843_ID_B        = 11,
    HMC5843_ID_C        = 12
};

enum hmc5843_mode
{
    HMC5843_MODE_CONTINUOUS = 0x00,
    HMC5843_MODE_SINGLE     = 0x01,
    HMC5843_MODE_IDLE       = 0x02,
    HMC5843_MODE_SLEEP      = 0x03
};

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

void hmc5843_start_continuous(void)
{
    command[0] = HMC5843_MODE;
    command[1] = HMC5843_MODE_CONTINUOUS;
    uart0_i2c_write(HMC5843_ID, command, 2, NULL);
}

void hmc5843_prepare_read(void)
{
    command[0] = HMC5843_OUTPUT_X_H;
    uart0_i2c_write(HMC5843_ID, command, 1, NULL);
}

void hmc5843_read_data(uint8_t *buffer, uint8_t size, uart0_i2c_callback callback)
{
    uart0_i2c_read(HMC5843_ID, buffer, size, callback);
}
