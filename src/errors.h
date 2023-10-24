#ifndef SLASHBITBOT_ERRORS_H__
#define SLASHBITBOT_ERRORS_H__

#include <zephyr/sys/printk.h>

#include "display.h"


template <typename... Args>
inline void error(char const* message, int cat, Args... args) {
  printk(message, args...);
  show_sad(2, cat);
  k_sleep(K_SECONDS(2));
  sys_reboot(SYS_REBOOT_COLD);
}

#endif
