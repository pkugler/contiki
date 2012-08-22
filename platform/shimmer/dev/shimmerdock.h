#ifndef SHIMMERDOCK_H
#define SHIMMERDOCK_H

#include "contiki.h"

typedef void (*shimmerdock_callback)(unsigned char connected);

void shimmerdock_set_callback(shimmerdock_callback);
void shimmerdock_isr();

#endif // SHIMMERDOCK_H
