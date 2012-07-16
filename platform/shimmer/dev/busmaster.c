#include "bluetooth.h"
#include "busmaster.h"

void uart0_disable_all(void)
{
//    sdc_disable();
}

void uart1_disable_all(void)
{
    bluetooth_disable();
}
