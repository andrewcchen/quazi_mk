#pragma once

extern __attribute__((format(printf, 1, 2))) void quazi_printf(const char *fmt, ...);

#define print(s) quazi_printf(s)
#define println(s) quazi_printf(s "\r\n")
#define xprintf quazi_printf
#define uprint(s) quazi_printf(s)
#define uprintln(s) quazi_printf(s "\r\n")
#define uprintf quazi_printf
