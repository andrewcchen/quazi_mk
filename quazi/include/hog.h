#pragma once

void quazi_hog_init(void);

uint8_t quazi_hog_keyboard_leds(void);

void quazi_hog_send_keyboard(uint8_t *report);

void quazi_hog_send_mouse(uint8_t *report);

void quazi_hog_send_system(uint16_t data);

void quazi_hog_send_consumer(uint16_t data);
