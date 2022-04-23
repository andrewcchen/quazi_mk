#pragma once

enum hid_report_ids {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE,
    REPORT_ID_SYSTEM,
    REPORT_ID_CONSUMER,
    REPORT_ID_NKRO,
    REPORT_ID_JOYSTICK
};

const static uint8_t hid_report_descriptor[] = {
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x06,        // Usage (Keyboard)
0xA1, 0x01,        // Collection (Application)
0x85, REPORT_ID_KEYBOARD, // Report ID (1)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0xE0,        //   Usage Minimum (0xE0)
0x29, 0xE7,        //   Usage Maximum (0xE7)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x01,        //   Logical Maximum (1)
0x95, 0x08,        //   Report Count (8)
0x75, 0x01,        //   Report Size (1)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x01,        //   Report Count (1)
0x75, 0x08,        //   Report Size (8)
0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0x00,        //   Usage Minimum (0x00)
0x29, 0xFF,        //   Usage Maximum (0xFF)
0x15, 0x00,        //   Logical Minimum (0)
0x26, 0xFF, 0x00,  //   Logical Maximum (255)
0x95, 0x06,        //   Report Count (6)
0x75, 0x08,        //   Report Size (8)
0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x08,        //   Usage Page (LEDs)
0x19, 0x01,        //   Usage Minimum (Num Lock)
0x29, 0x05,        //   Usage Maximum (Kana)
0x95, 0x05,        //   Report Count (5)
0x75, 0x01,        //   Report Size (1)
0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x95, 0x01,        //   Report Count (1)
0x75, 0x03,        //   Report Size (3)
0x91, 0x03,        //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              // End Collection

#ifdef EXTRAKEY_ENABLE
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x80,        // Usage (Sys Control)
0xA1, 0x01,        // Collection (Application)
0x85, REPORT_ID_SYSTEM, // Report ID (3)
0x19, 0x01,        //   Usage Minimum (Pointer)
0x2A, 0xB7, 0x00,  //   Usage Maximum (Sys Display LCD Autoscale)
0x15, 0x01,        //   Logical Minimum (1)
0x26, 0xB7, 0x00,  //   Logical Maximum (183)
0x95, 0x01,        //   Report Count (1)
0x75, 0x10,        //   Report Size (16)
0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection

0x05, 0x0C,        // Usage Page (Consumer)
0x09, 0x01,        // Usage (Consumer Control)
0xA1, 0x01,        // Collection (Application)
0x85, REPORT_ID_CONSUMER, // Report ID (4)
0x19, 0x01,        //   Usage Minimum (Consumer Control)
0x2A, 0xA0, 0x02,  //   Usage Maximum (0x02A0)
0x15, 0x01,        //   Logical Minimum (1)
0x26, 0xA0, 0x02,  //   Logical Maximum (672)
0x95, 0x01,        //   Report Count (1)
0x75, 0x10,        //   Report Size (16)
0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection
#endif
};
