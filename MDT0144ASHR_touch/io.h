/****************************************************************
 *                    Midas Displays Library                    * 
 *     Designed for the MCIB-12 board for prototyping demos.    *
 ****************************************************************/

#ifndef IO_H
#define IO_H

#include <Arduino.h>
//#include <plib.h>
#include <MCIB12.h>
#include <SD.h>

//             MCIB-12 Pins  Description
#define CS         M_09       // CS
#define RD         M_10       // E/RD
#define WR         M_11       // R/W (8080 8-bit)
#define DC         M_12       // DC
#define D0         M_13       // D0
#define D1         M_14       // D1
#define D2         M_15       // D2
#define D3         M_16       // D3
#define D4         M_17       // D4
#define D5         M_18       // D5
#define D6         M_19       // D6
#define D7         M_20       // D7
#define RESET      M_22       // RESET

//             Interface select pins
//#define IM0                 connected to GND
#define IM2        M_21
#define SPI4W      M_25

#define SD_detect  M_24       // SS10
#define SCLK       M_32       // SCLK  (SD card reader)
#define MOSI       M_34       // MOSI  (SD card reader)
#define MISO       M_36       // MISO  (SD card reader)
#define sSD        M_39       // SSD  (or CS for SD card reader)

#define YU         M_41       // A1  Note: Ensure analogue pins are enabled
#define XL         M_42       // A7        via the JP6 and JP8 jumper links!
#define YD         M_43       // A2
#define XR         M_44       // A8

void setup_PMP_for_display(void);
void write_command(unsigned char cmd);
void write_data(unsigned char dat);
void write_16bitRGB(uint32_t hexRGB24);
void write_24bitRGB(uint32_t hexRGB24);
void write_3bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3);
void write_array(unsigned char *buff, unsigned long buff_size);

#endif
