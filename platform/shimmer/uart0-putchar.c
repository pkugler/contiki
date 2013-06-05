#include "dev/uart0.h"

int putchar(int c)
{
  uart0_writeb(c);
  return c;
}

/* vim: set et ts=2 sw=2: */
