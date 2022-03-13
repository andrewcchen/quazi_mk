#pragma once

#include <sys/printk.h>

// Redirect printf to Zephyr's printk
#define printf printk
#define print_set_sendchar(...)

#define print(s) printf(s)
#define println(s) printf(s "\r\n")
#define xprintf printf
#define uprint(s) printf(s)
#define uprintln(s) printf(s "\r\n")
#define uprintf printf
