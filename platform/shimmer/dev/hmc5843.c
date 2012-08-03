#include "contiki.h"
#include "hmc5843.h"
#include "hwconf.h"

#define HMC5843_ID (0x1e)
HWCONF_PIN(GYRO_PWREN_N, 1, 1)
static uint8_t command[2];
static uint8_t buffer[7];
static int16_t *user_data;
static uart0_i2c_callback hmc5843_callback;

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

enum state
{
    STATE_IDLE,
    STATE_STARTING,
    STATE_PREPARING,
    STATE_READING
};

static enum state state = STATE_IDLE;

static void callback_function(uint8_t success);

static void hmc5843_set_mode(enum hmc5843_mode mode)
{
    state = STATE_STARTING;
    command[0] = HMC5843_MODE;
    command[1] = mode;
    uart0_i2c_write(HMC5843_ID, command, 2, callback_function);
}

static void hmc5843_set_register(enum hmc5843_register reg)
{
    state = STATE_PREPARING;
    command[0] = reg;
    uart0_i2c_write(HMC5843_ID, command, 1, callback_function);
}

void hmc5843_read_data(int16_t *data, uart0_i2c_callback callback)
{
    if (state == STATE_IDLE) {
        state = STATE_READING;
        hmc5843_callback = callback;
        user_data = data;
        uart0_i2c_read(HMC5843_ID, buffer, 7, callback_function);
    }
}

void hmc5843_enable(void)
{
    hmc5843_disable();
    GYRO_PWREN_N_CLEAR();
    udelay(5000);
    hmc5843_set_mode(HMC5843_MODE_SINGLE);
}

void hmc5843_disable(void)
{
    GYRO_PWREN_N_SELECT_IO();
    GYRO_PWREN_N_MAKE_OUTPUT();
    GYRO_PWREN_N_SET();
    udelay(5000);
}

static void callback_function(uint8_t success)
{
    if (success) {
        uint8_t i;

        switch (state) {
        case STATE_IDLE:
            break;

        case STATE_STARTING:
            state = STATE_IDLE;
            break;

        case STATE_PREPARING:
            state = STATE_IDLE;
            break;

        case STATE_READING:
            for (i = 0; i < 3; i++) {
                user_data[i] = (((int16_t) buffer[i * 2 + 0]) << 8) | ((int16_t) buffer[i * 2 + 1]);
            }

            if (hmc5843_callback) {
                hmc5843_callback(success);
            }

            hmc5843_set_mode(HMC5843_MODE_SINGLE);
        }
    }
}
