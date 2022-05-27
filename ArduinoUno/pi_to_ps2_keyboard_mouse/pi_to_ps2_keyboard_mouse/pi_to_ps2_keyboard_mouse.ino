/*
 * This file includes code from the USB4VC project
 * which has been modifed to work on an Arduino instead
 * of the STM microcontroller the protocol board uses
 */

#include <ps2dev.h>
#include "ps2mouse.h"

#define PS2_MOUSE_CLOCK_PIN 2
#define PS2_MOUSE_DATA_PIN  3

#define PS2_KEYBOARD_CLOCK_PIN 4
#define PS2_KEYBOARD_DATA_PIN  5

#define SPI_MOSI_MAGIC 0xde
#define SPI_MOSI_MSG_TYPE_KEYBOARD_EVENT 8
#define SPI_MOSI_MSG_TYPE_MOUSE_EVENT 9
#define SPI_MOSI_MSG_TYPE_GAMEPAD_EVENT_RAW 10
#define SPI_MOSI_MSG_TYPE_GAMEPAD_EVENT_MAPPED 11

#define LINUX_KEYCODE_TO_PS2_SCANCODE_SINGLE_SIZE 89
#define LINUX_KEYCODE_TO_PS2_SCANCODE_SPECIAL_SIZE 32

#define CODE_UNUSED 0xff
#define PS2KB_WRITE_DEFAULT_TIMEOUT_MS 20
#define MOUSE_AVG 20

typedef struct
{
  uint8_t button_1;
  uint8_t button_2;
  uint8_t button_3;
  uint8_t button_4;
  uint8_t button_lt;
  uint8_t button_rt;
  uint8_t button_lt2;
  uint8_t button_rt2;
  uint8_t axis_x;
  uint8_t axis_y;
  uint8_t axis_rx;
  uint8_t axis_ry;
} gamepad_event;

const uint8_t linux_keycode_to_ps2_scancode_lookup_single_byte_codeset2[LINUX_KEYCODE_TO_PS2_SCANCODE_SINGLE_SIZE] =
{
  CODE_UNUSED, // KEY_RESERVED       0
  0x76, // KEY_ESC            1
  0x16, // KEY_1          2
  0x1E, // KEY_2          3
  0x26, // KEY_3          4
  0x25, // KEY_4          5
  0x2E, // KEY_5          6
  0x36, // KEY_6          7
  0x3D, // KEY_7          8
  0x3E, // KEY_8          9
  0x46, // KEY_9          10
  0x45, // KEY_0          11
  0x4E, // KEY_MINUS      12
  0x55, // KEY_EQUAL      13
  0x66, // KEY_BACKSPACE      14
  0x0D, // KEY_TAB            15
  0x15, // KEY_Q          16
  0x1D, // KEY_W          17
  0x24, // KEY_E          18
  0x2D, // KEY_R          19
  0x2C, // KEY_T          20
  0x35, // KEY_Y          21
  0x3C, // KEY_U          22
  0x43, // KEY_I          23
  0x44, // KEY_O          24
  0x4D, // KEY_P          25
  0x54, // KEY_LEFTBRACE      26
  0x5B, // KEY_RIGHTBRACE     27
  0x5A, // KEY_ENTER      28
  0x14, // KEY_LEFTCTRL       29
  0x1C, // KEY_A          30
  0x1B, // KEY_S          31
  0x23, // KEY_D          32
  0x2B, // KEY_F          33
  0x34, // KEY_G          34
  0x33, // KEY_H          35
  0x3B, // KEY_J          36
  0x42, // KEY_K          37
  0x4B, // KEY_L          38
  0x4C, // KEY_SEMICOLON      39
  0x52, // KEY_APOSTROPHE     40
  0x0E, // KEY_GRAVE      41
  0x12, // KEY_LEFTSHIFT      42
  0x5D, // KEY_BACKSLASH      43
  0x1A, // KEY_Z          44
  0x22, // KEY_X          45
  0x21, // KEY_C          46
  0x2A, // KEY_V          47
  0x32, // KEY_B          48
  0x31, // KEY_N          49
  0x3A, // KEY_M          50
  0x41, // KEY_COMMA      51
  0x49, // KEY_DOT            52
  0x4A, // KEY_SLASH      53
  0x59, // KEY_RIGHTSHIFT     54
  0x7C, // KEY_KPASTERISK     55
  0x11, // KEY_LEFTALT        56
  0x29, // KEY_SPACE      57
  0x58, // KEY_CAPSLOCK       58
  0x05, // KEY_F1         59
  0x06, // KEY_F2         60
  0x04, // KEY_F3         61
  0x0C, // KEY_F4         62
  0x03, // KEY_F5         63
  0x0B, // KEY_F6         64
  0x83, // KEY_F7         65
  0x0A, // KEY_F8         66
  0x01, // KEY_F9         67
  0x09, // KEY_F10            68
  0x77, // KEY_NUMLOCK        69
  0x7E, // KEY_SCROLLLOCK     70
  0x6C, // KEY_KP7            71
  0x75, // KEY_KP8            72
  0x7D, // KEY_KP9            73
  0x7B, // KEY_KPMINUS        74
  0x6B, // KEY_KP4            75
  0x73, // KEY_KP5            76
  0x74, // KEY_KP6            77
  0x79, // KEY_KPPLUS     78
  0x69, // KEY_KP1            79
  0x72, // KEY_KP2            80
  0x7A, // KEY_KP3            81
  0x70, // KEY_KP0            82
  0x71, // KEY_KPDOT      83
  CODE_UNUSED, // KEY_UNUSED     84
  CODE_UNUSED, // KEY_ZENKAKUHANKAKU 85
  0x61, // KEY_102ND      86
  0x78, // KEY_F11            87
  0x07, // KEY_F12            88
};

const uint8_t linux_keycode_to_ps2_scancode_lookup_special_codeset2[LINUX_KEYCODE_TO_PS2_SCANCODE_SPECIAL_SIZE] =
{
  0x5A, // KPENTER    96
  0x14, // KEY_RIGHTCTRL    97
  0x4A, // KEY_KPSLASH    98
  CODE_UNUSED, // 99
  0x11, // KEY_RIGHTALT   100
  CODE_UNUSED, // 101
  0x6C, // KEY_HOME   102
  0x75, // KEY_UP     103
  0x7D, // KEY_PAGEUP   104
  0x6B, // KEY_LEFT   105
  0x74, // KEY_RIGHT    106
  0x69, // KEY_END      107
  0x72, // KEY_DOWN   108
  0x7A, // KEY_PAGEDOWN   109
  0x70, // KEY_INSERT   110
  0x71, // KEY_DELETE   111
  CODE_UNUSED, // KEY_MACRO   112
  CODE_UNUSED, // KEY_MUTE    113
  CODE_UNUSED, // KEY_VOLUMEDOWN    114
  CODE_UNUSED, // KEY_VOLUMEUP    115
  CODE_UNUSED, // KEY_POWER   116
  CODE_UNUSED, // KEY_KPEQUAL   117
  CODE_UNUSED, // KEY_KPPLUSMINUS   118
  CODE_UNUSED, // KEY_PAUSE   119
  CODE_UNUSED, // KEY_SCALE   120
  CODE_UNUSED, // KEY_KPCOMMA   121
  CODE_UNUSED, // KEY_HANGEUL   122
  CODE_UNUSED, // KEY_HANJA   123
  CODE_UNUSED, // KEY_YEN     124
  0x1F, // KEY_LEFTMETA   125
  0x27, // KEY_RIGHTMETA    126
  0x2F, // KEY_COMPOSE    127
};


static uint8_t pkt[32];

static mouse_event latest_mouse_event;
static gamepad_event latest_gamepad_event;
static uint8_t enabled = 1;

static uint32_t last_mouse = 0;
static uint8_t cur_buttons[3];
static uint8_t last_buttons[3];
static uint8_t mouse_cnt = 0;
static volatile int mouse_dx = 0;
static volatile int mouse_dy = 0;

static uint8_t ps2mouse_host_cmd = 0, ps2mouse_bus_status = 0;
static ps2_outgoing_buf my_ps2_outbuf;

static uint32_t last_kb_status_check;
static uint8_t kbd_ms = 0;


PS2dev keyboard(PS2_KEYBOARD_CLOCK_PIN, PS2_KEYBOARD_DATA_PIN);

uint8_t ps2kb_press_key_scancode_2(uint8_t linux_keycode, uint8_t linux_keyvalue)
{
  // linux_keyvalue: release 0 press 1 autorepeat 2
  uint8_t lookup_result;

  if (linux_keycode < LINUX_KEYCODE_TO_PS2_SCANCODE_SINGLE_SIZE)
  {
    lookup_result = linux_keycode_to_ps2_scancode_lookup_single_byte_codeset2[linux_keycode];
    if (lookup_result == CODE_UNUSED)
      return 2;
    if (linux_keyvalue)
    {
      keyboard.write(lookup_result);
    }
    else
    {
      keyboard.write(0xf0);
      keyboard.write(lookup_result);
    }
    return 0;
  }

  if (linux_keycode >= 96 && linux_keycode <= 127)
  {
    lookup_result = linux_keycode_to_ps2_scancode_lookup_special_codeset2[linux_keycode - 96];
    if (lookup_result == CODE_UNUSED)
      return 2;
    if (linux_keyvalue)
    {
      keyboard.write(0xe0);
      keyboard.write(lookup_result);
    }
    else
    {
      keyboard.write(0xe0);
      keyboard.write(0xf0);
      keyboard.write(lookup_result);
    }
    return 0;
  }
  return 0;
}

static void process_keyboard_pkt(uint8_t *kb_pkt)
{
  ps2kb_press_key_scancode_2(kb_pkt[4], kb_pkt[6]);
}

static int16_t byte_to_int16_t(uint8_t lsb, uint8_t msb)
{
  return (int16_t)((msb << 8) | lsb);
}

static void ps2mouse_update(void)
{
  if (!latest_mouse_event.button_extra) {
    // nothing to send now
    return;
  }

  ps2mouse_bus_status = ps2mouse_get_bus_status();
  if (ps2mouse_bus_status == PS2_BUS_INHIBIT)
  {
    ps2mouse_release_lines();
    return;
  }
  else if (ps2mouse_bus_status == PS2_BUS_REQ_TO_SEND)
  {
    ps2mouse_read(&ps2mouse_host_cmd, 10);
    ps2mouse_host_req_reply(ps2mouse_host_cmd, &latest_mouse_event);
    return;
  }

  mouse_event* this_mouse_event = &latest_mouse_event;

  if (ps2mouse_get_outgoing_data(this_mouse_event, &my_ps2_outbuf))
  {
    // if return value is not 0, no need to send out packets
    return;
  }

  // only pop the item if sending is complete
  if (ps2mouse_send_update(&my_ps2_outbuf) == 0)
  {
    // send is ok
    latest_mouse_event.button_extra = 0;
  }
}

static void process_mouse_pkt(uint8_t *backup_spi1_recv_buf)
{
  int res = 0;

  // Modified USB4VC packet pruned to 16 bytes
  latest_mouse_event.movement_x = byte_to_int16_t(backup_spi1_recv_buf[4], backup_spi1_recv_buf[5]);
  latest_mouse_event.movement_y = -1 * byte_to_int16_t(backup_spi1_recv_buf[6], backup_spi1_recv_buf[7]);
  latest_mouse_event.scroll_vertical = 0;
  latest_mouse_event.scroll_horizontal = 0;
  latest_mouse_event.button_left = backup_spi1_recv_buf[13];
  latest_mouse_event.button_right = backup_spi1_recv_buf[14];
  latest_mouse_event.button_middle = backup_spi1_recv_buf[15];

  latest_mouse_event.button_side = 0;
  latest_mouse_event.button_extra = 1;  // using this as a "this event needs to be sent flag"


  if (0x55 == backup_spi1_recv_buf[3])
  {
    ps2mouse_update();
  }
  else {
    latest_mouse_event.button_extra = 0;
  }
}

static void process_gamepad_pkt(uint8_t *backup_spi1_recv_buf)
{
  // Modified USB4VC packet pruned to 16 bytes
  latest_gamepad_event.button_1 = backup_spi1_recv_buf[4];
  latest_gamepad_event.button_2 = backup_spi1_recv_buf[5];
  latest_gamepad_event.button_3 = backup_spi1_recv_buf[6];
  latest_gamepad_event.button_4 = backup_spi1_recv_buf[7];
  latest_gamepad_event.button_lt = 0;
  latest_gamepad_event.button_rt = 0;
  latest_gamepad_event.button_lt2 = 0;
  latest_gamepad_event.button_rt2 = 0;
  latest_gamepad_event.axis_x = backup_spi1_recv_buf[8];
  latest_gamepad_event.axis_y = backup_spi1_recv_buf[9];
  latest_gamepad_event.axis_rx = backup_spi1_recv_buf[10];
  latest_gamepad_event.axis_ry = backup_spi1_recv_buf[11];
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  keyboard.keyboard_init();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(1000000);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  delay(200);

  cur_buttons[0] = 0;
  cur_buttons[1] = 0;
  cur_buttons[2] = 0;
  last_buttons[0] = 0;
  last_buttons[1] = 0;
  last_buttons[2] = 0;
  mouse_dx = 0;
  mouse_dy = 0;

  latest_mouse_event.button_extra = 0;

  ps2mouse_init(PS2_MOUSE_CLOCK_PIN, PS2_MOUSE_CLOCK_PIN, PS2_MOUSE_DATA_PIN, PS2_MOUSE_DATA_PIN);
  ps2mouse_enable_reporting();

  last_kb_status_check = millis();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  uint8_t inByte, inByte2, leds;
  int i, res;

  if (millis() - last_kb_status_check > 5)
  {
    if (keyboard.keyboard_handle(&leds)) {
      //Serial.print('LEDS');
      //Serial.print(leds, HEX);
      digitalWrite(LED_BUILTIN, leds);
    }
    last_kb_status_check = millis();
  }


  ps2mouse_update();

}

void serialEvent()
{
  int i, tmp;
  uint8_t inByte, inByte2, inByte3;

  inByte = Serial.read();
  while (Serial.available() == 0) {
    ;
  }
  inByte2 = Serial.read();
  while (Serial.available() == 0) {
    ;
  }
  inByte3 = Serial.read();

  if ((SPI_MOSI_MAGIC != inByte) || ( 0 != inByte2 ) || (inByte3 > SPI_MOSI_MSG_TYPE_GAMEPAD_EVENT_MAPPED)) {

    while (Serial.available() > 0) {
      inByte = inByte2;
      inByte2 = inByte3;
      inByte3 = Serial.read();
      if ((SPI_MOSI_MAGIC == inByte) && ( 0 == inByte2 ) && (inByte3 <= SPI_MOSI_MSG_TYPE_GAMEPAD_EVENT_MAPPED)) {
        goto pkt_start;
      }
    }
  }
  else {
pkt_start:
    pkt[0] = inByte;
    pkt[1] = inByte2;
    pkt[2] = inByte3;
    i = 3;
    // Got what seems to be a valid packet, read more bytes and validate
    while (i < 16)
    {
      while (!Serial.available()) {
        delay(1);
      }
      tmp = Serial.read();
      if (tmp >= 0) {
        pkt[i++] = tmp;
      }
      else {
        // Bail out if we don't receive more bytes in a timely manner
        break;
      }
    }
    if ((i != 16) || (pkt[8] != 0xff) || (pkt[9] != 0xff)) {
      // Additional validation, if we get here things are not
      // looking right so we flush the serial port and then
      // wait for the next good packet
      while (Serial.available() > 0) {
        inByte = Serial.read();
      }
    }
    else {
      // It appears we got a "good" packet, first
      // try to clear out left over mouse events first (if any)
      ps2mouse_update();
      // Handle packet
      if (SPI_MOSI_MSG_TYPE_KEYBOARD_EVENT > pkt[2])
      {
        // We got an unknown event that isn't implemented by this sketch
      }
      else if (SPI_MOSI_MSG_TYPE_KEYBOARD_EVENT == pkt[2])
      {
        process_keyboard_pkt(pkt);
      }
      else if (SPI_MOSI_MSG_TYPE_MOUSE_EVENT == pkt[2])
      {
        process_mouse_pkt(pkt);
      }
      else if (SPI_MOSI_MSG_TYPE_GAMEPAD_EVENT_MAPPED == pkt[2])
      {
        process_gamepad_pkt(pkt);
      }
      else {
        // We got an unknown event that isn't implemented by this sketch
      }
    }
  }
}
