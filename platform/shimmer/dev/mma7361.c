#include "hwconf.h"
#include "mma7361.h"

HWCONF_PIN(ACCEL_SEL0, 4, 4)
HWCONF_PIN(ACCEL_SLEEP_N, 5, 0)
HWCONF_PIN(ADC_ACCELZ, 6, 3)
HWCONF_PIN(ADC_ACCELY, 6, 4)
HWCONF_PIN(ADC_ACCELX, 6, 5)

void mma7361_init(void)
{
  // set to 1.5G
  ACCEL_SEL0_SELECT_IO();
  ACCEL_SEL0_MAKE_OUTPUT();
  ACCEL_SEL0_CLEAR();

  // enable sleep
  ACCEL_SLEEP_N_SELECT_IO();
  ACCEL_SLEEP_N_MAKE_OUTPUT();
  ACCEL_SLEEP_N_CLEAR();

  // input pins
  ADC_ACCELX_SELECT_PM();
  ADC_ACCELX_MAKE_INPUT();
  ADC_ACCELX_CLEAR();

  ADC_ACCELY_SELECT_PM();
  ADC_ACCELY_MAKE_INPUT();
  ADC_ACCELY_CLEAR();

  ADC_ACCELZ_SELECT_PM();
  ADC_ACCELZ_MAKE_INPUT();
  ADC_ACCELZ_CLEAR();
}

void mma7361_enable(void)
{
  ACCEL_SLEEP_N_SET();
}

void mma7361_disable(void)
{
  ACCEL_SLEEP_N_CLEAR();
}

void mma7361_set_range(enum mma7361_range range)
{
  switch (range) {
    case MMA7361_RANGE_1_5G:
      ACCEL_SEL0_CLEAR();
      break;

    case MMA7361_RANGE_6_0G:
      ACCEL_SEL0_SET();
      break;
  }
}

/* vim: set et ts=2 sw=2: */
