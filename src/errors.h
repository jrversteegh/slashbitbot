#ifndef SLASHBITBOT_ERRORS_H__
#define SLASHBITBOT_ERRORS_H__

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/reboot.h>

#include "display.h"

// We don't have user input so OK with non literal format strings
#pragma GCC diagnostic ignored "-Wformat-security"


template <typename... Args>
inline void error(int category, char const* const message, Args... args) {
  printk(message, args...);
  show_sad(2, category);
  k_sleep(K_SECONDS(2));
  sys_reboot(SYS_REBOOT_COLD);
}

#endif
