#pragma once

#include <sys/printk.h>

// Redirect printf to Zephyr's printk
#define printf printk
#define print_set_sendchar(...)
