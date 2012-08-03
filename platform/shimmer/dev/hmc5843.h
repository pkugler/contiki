#ifndef HMC5843_H
#define HMC5843_H

#include "dev/uart0.h"

void hmc5843_enable(void);
void hmc5843_disable(void);
void hmc5843_read_data(int16_t *data, uart0_i2c_callback callback);

#endif // HMC5843_H
