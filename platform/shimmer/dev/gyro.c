#include "contiki.h"
#include "hwconf.h"
#include "gyro.h"

HWCONF_PIN(GYRO_PWREN_N, 1, 1)
HWCONF_PIN(SEL_A0, 1, 4)
HWCONF_PIN(GYRO_ZERO, 3, 4)

/**
 * Enable voltage regulator for gyroscope
 */
void gyro_enable(void)
{
    gyro_disable();

    // wait for capacitors to discharge (6 seconds???)
    short i;
    for (i = 0; i < 6000; i++) {
        udelay(1000);
    }

    GYRO_PWREN_N_CLEAR();
    SEL_A0_CLEAR();
}

/**
 * Disable voltage regulator for gyroscope
 */
void gyro_disable(void)
{
    SEL_A0_SELECT_IO();
    SEL_A0_MAKE_OUTPUT();
    SEL_A0_SET();

    GYRO_ZERO_SELECT_IO();
    GYRO_ZERO_MAKE_OUTPUT();
    GYRO_ZERO_CLEAR();

    GYRO_PWREN_N_SELECT_IO();
    GYRO_PWREN_N_MAKE_OUTPUT();
    GYRO_PWREN_N_SET();
}
