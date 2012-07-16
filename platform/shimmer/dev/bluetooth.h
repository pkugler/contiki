#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "contiki.h"

void bluetooth_enable(int (*handler)(unsigned char c));
void bluetooth_disable(void);

#endif // BLUETOOTH_H
