/****************************************************************
 *   File        : IO.cpp
 *   Description : Interface communication for MCU with displays.
 *   Author      : I.Antonakis
 *   Date        : 25/03/2022
 *   Version     : 1.8
 *   Works with  : uC32, UNO32
 ****************************************************************/
#include "io.h"

//Set up 8-bit bus 8080 Parallel interface
void setup_PMP_for_display(void)
{
   /*
   Note: This function utilizes the internal Parallel Master Port (PMP)
   found on most PIC32 microcontrollers. It can also be used on a uC32.
   PMDIN is the parallel port and CS1 (35) is the chip select line.
   http://ww1.microchip.com/downloads/en/DeviceDoc/60001128H.pdf
   */
   IEC1CLR = 0x0004;   // Disable PMP interrupt
   PMCON = 0x0000;     // Stop PMP module and clear control register
   PMCONSET = 0x0380;  // Enable Ports and set CS1, CS2, WR and RD as ACTIVE LOW
   PMMODE = 0x0204;    // Configure the mode2 8bit mode wait states for data strobe 
   PMAEN = 0xc000;     // Chip Select lines ENABLED
   PMADDRCLR = 0x8000; // De-activate CS2
   PMADDRSET = 0x4000; // Activate CS1
   PMCONSET = 0x8000;  // Enable PMP module
}
/************************************************************************************************/

void write_command(unsigned char cmd)
{
  digitalWrite(DC,LOW);   
  PMDIN=cmd;
}
void write_data(unsigned char dat)
{
  digitalWrite(DC,HIGH);
  PMDIN=dat;
}
/****************************************************************
 * Function : write_16bitRGB                                    * 
 * Summary  : Converts standard Hex RGB Colour Code to 16-bit   *
 *            RGB 5-6-5 format and writes to display memory.    *  
 * Interface: 8-bit Parallel                                    *
 * Input    : unsigned int hexRGB24 (24-bit RGB data)           *
 * Output   : None                                              *
 ****************************************************************/
void write_16bitRGB(uint32_t hexRGB24)
{
   unsigned char R, G, B;
   R = hexRGB24>>16;
   G = hexRGB24>>8;
   B = hexRGB24;

   digitalWrite(DC,HIGH);
   PMDIN = (R&0xF8)+(G>>5);       // 5-bit R + 3-bit G
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   PMDIN = ((G<<3)&0xE0)+(B>>3);  // 3-bit G + 5-bit B
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   __asm__("nop\n\t");
}
/****************************************************************
 * Function : write_24bitRGB                                    * 
 * Summary  : Writes 24-bit/Pixel (RGB 8-8-8) via 8-bit bus.    *
 *            Also works for 18-bit/Pixel (RGB 6-6-6) displays  *
 * Interface: 8-bit Parallel                                    *
 * Input    : unsigned int hexRGB24 (24-bit RGB data)           *
 * Output   : None                                              *
 ****************************************************************/
void write_24bitRGB(uint32_t hexRGB24)
{
   digitalWrite(DC,HIGH);
   PMDIN = hexRGB24>>16;   //upper 8 bits (RED)
   __asm__("nop\n\t");     //delay 1 cycle
   __asm__("nop\n\t");     //delay 1 cycle
   __asm__("nop\n\t");     //delay 1 cycle
   PMDIN = hexRGB24>>8;    //middle 8 bits (GREEN)
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   PMDIN = hexRGB24;       //lower 8 bits (BLUE)
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   __asm__("nop\n\t");
}
void write_3bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
   digitalWrite(DC,HIGH);
   PMDIN = byte1;
   __asm__("nop\n\t");     //delay 1 cycle
   __asm__("nop\n\t");     //delay 1 cycle
   __asm__("nop\n\t");     //delay 1 cycle
   PMDIN = byte2;
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   PMDIN = byte3;
   __asm__("nop\n\t");
   __asm__("nop\n\t");
   __asm__("nop\n\t");
}
void write_array(uint8_t *buff, uint32_t buff_size)
{
   uint32_t count = 0;
   while (count < buff_size)    //count through each byte
   {
      write_data(buff[count]);  //send data to display
      count++;                  //and increment count
   }
}
