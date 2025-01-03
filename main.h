/* 
 * File:   main.h
 * Author: magdu
 *
 * Created on 14 November, 2024, 10:15 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include "car_blackbox_def.h"
#include <string.h>
#include "External_EEPROM.h"
#include "timers.h"
#include "uart.h"


#define DASH_BOARD                0x01
#define LOGIN                     0x02
#define MAIN_MENU                 0x03
#define VIEW_LOG_SCREEN           0x04

#define RESET_PASSWORD            0x11

#define RESET_NOTHING             0x12

#define RETURN_BACK               0x33

#define LOGIN_SUCCESS             0x44

#define RESET_MENU                0x55
 
#define VIEW_LOG                  0x66

#define CLEAR_LOG                 0x77

#define DOWNLOAD_LOG              0x88

#define SET_TIME                  0x99

#define CHANGE_PASS               0x10

#define MENU_SCREEN_IDLE          0x13

#define LONG_PRESS_UP             0x14

#define LONG_PRESS_DOWN           0x15

#define LONG_PRESS_UP_TIME        0x17

#define LONG_PRESS_DOWN_TIME      0x18

#define LONG_PRESS_UP_CLEAR       0x19

#define LONG_PRESS_DOWN_CLEAR     0x20

#define LONG_PRESS_UP_DOWNLOAD    0x21

#define LONG_PRESS_DOWN_DOWNLOAD  0x22

#define LONG_PRESS_UP_PASS        0x21

#define LONG_PRESS_DOWN_PASS      0x22

#define NO_ACTION                 0x16

#define ENTER_PASS                0x23

#define NOTHING                   0x24


#endif	/* MAIN_H */

