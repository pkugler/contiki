#include "shimmerdock.h"
#include "hwconf.h"

HWCONF_PIN(DOCK_N, 2, 3)
HWCONF_IRQ(DOCK_N, 2, 3)

static shimmerdock_callback callback;

static void
invoke_callback()
{
  if (DOCK_N_READ()) {
    DOCK_N_IRQ_EDGE_SELECTD();
    if (callback) {
      callback(0);
    }
  } else {
    DOCK_N_IRQ_EDGE_SELECTU();
    if (callback) {
      callback(1);
    }
  }
}

/**
 * Sets a callback function to detect dock connections
 *
 * The callback function will be called whenever the node is inserted into
 * or removed from a dock.
 * @param       cb      Callback function
 */
void
shimmerdock_set_callback(shimmerdock_callback cb)
{
  DOCK_N_DISABLE_IRQ();
  callback = cb;
  DOCK_N_SELECT_IO();
  DOCK_N_MAKE_INPUT();
  invoke_callback();
  DOCK_N_ENABLE_IRQ();
}

/**
 * Tests IRQ state and calls configured connect_handler
 *
 * @param connected     Connection state of the device
 */
void
shimmerdock_isr()
{
  if (DOCK_N_CHECK_IRQ()) {
    invoke_callback();
  }
}
