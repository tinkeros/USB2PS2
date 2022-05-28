/*
   This file includes code from the USB4VC project
   which has been modifed to work on an Arduino instead
   of the STM microcontroller the protocol board uses
*/

#ifndef __PS2MOUSE_H
#define __PS2MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Arduino.h>

#define PS2_BUS_IDLE 3
#define PS2_BUS_INHIBIT 2
#define PS2_BUS_REQ_TO_SEND 1
#define PS2_BUS_UNKNOWN 0
#define PS2_OUT_BUF_MAXSIZE 16

typedef struct
{
  uint8_t size;
  uint8_t data[PS2_OUT_BUF_MAXSIZE];
} ps2_outgoing_buf;


typedef struct
{
  int16_t movement_x;
  int16_t movement_y;
  int8_t scroll_vertical;
  int8_t scroll_horizontal;
  uint8_t button_left;
  uint8_t button_middle;
  uint8_t button_right;
  uint8_t button_side;
  uint8_t button_extra;
} mouse_event;

void ps2mouse_enable_reporting(void);
void ps2mouse_init(int clk_port, uint16_t clk_pin, int data_port, uint16_t data_pin);
uint8_t ps2mouse_get_bus_status(void);
uint8_t ps2mouse_read(uint8_t* result, uint8_t timeout_ms);
uint8_t ps2mouse_write(uint8_t data, uint8_t timeout_ms);
void ps2mouse_host_req_reply(uint8_t cmd, mouse_event* mevent);
void ps2mouse_release_lines(void);
uint8_t ps2mouse_get_outgoing_data(mouse_event* this_event, ps2_outgoing_buf* pbuf);
uint8_t ps2mouse_send_update(ps2_outgoing_buf* pbuf);
uint8_t ps2mouse_write_nowait(uint8_t data);
void ps2mouse_reset(void);
#ifdef __cplusplus
}
#endif

#endif
