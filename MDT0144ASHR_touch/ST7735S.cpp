/****************************************************************
 *   File        : ST7735S.cpp
 *   Description : Initialization code for ST7735S controller IC
 *   Author      : I.Antonakis
 *   Date        : 24/03/2022
 *   Version     : 1.3
 *   Works with  : MDT0144ASHR-MULTI, MDT0144ASH-MULTI and
 *                 MCT018E12W128160PML
 *                 
 *   Uses the following external macros (defined in .h file): 
 *            offset_X       //start Column address
 *            end_X          //end Column address
 *            offset_Y       //start Page address
 *            end_Y          //end Page address
 *            rotate180      //display rotated with FFC on top
 *            SET_12bit      // 12-bit colour debth mode
 *            SET_16bit      // 16-bit colour debth mode
 *            SET_18bit      // 18-bit colour debth mode
 ****************************************************************/

#include "display.h"

void SET_COLUMN_ADDRESS(unsigned int start_addr, unsigned int end_addr)
{
   write_command(0x2A);         // COLUMN ADDRESS
   write_data(start_addr>>8);   // set START address [15:8]
   write_data(start_addr);      // set START address [7:0]
   write_data((end_addr)>>8);   // set END address [15:8]
   write_data(end_addr);        // set END address [7:0]
}
void SET_PAGE_ADDRESS(unsigned int start_addr, unsigned int end_addr)
{
   write_command(0x2B);         // PAGE ADDRESS
   write_data(start_addr>>8);   // set START address [15:8]
   write_data(start_addr);      // set START address [7:0]
   write_data((end_addr)>>8);   // set END address [15:8]
   write_data(end_addr);        // set END address [7:0]
}

void Init_ST7735S(void)
{
  //--------------------------------   Start ST7735S Frame Rate     --------------------------------//
  write_command(0xB1); //Frame Rate Control (In normal mode/ Full colors)
  write_data(0x02);
  write_data(0x35);    // Frame Rate = 80Hz
  write_data(0x36);

  write_command(0xB2); //Frame Rate Control (In Idle mode/ 8-colors)
  write_data(0x02);
  write_data(0x35);    // Frame Rate = 80Hz
  write_data(0x36);

  write_command(0xB3); //Frame Rate Control (In Partial mode/ full colors)
  write_data(0x02);
  write_data(0x35);    // Frame Rate = 80Hz
  write_data(0x36);
  write_data(0x02);
  write_data(0x35);
  write_data(0x36);
  //--------------------------------    End ST7735S Frame Rate      --------------------------------//
  
  write_command(0xB4); //Display Inversion Control
  write_data(0x03);    // normal mode = Dot Inversion, idle/partial = Column Inversion
  
  //--------------------------------  Start ST7735S Power Sequence  --------------------------------//
  write_command(0xC0); //Power Control 1
  write_data(0xA2);
  write_data(0x02);
  write_data(0x84);

  write_command(0xC1); //Power Control 2
  write_data(0xC5);

  write_command(0xC2); //Power Control 3
  write_data(0x0D);
  write_data(0x00);

  write_command(0xC3); //Power Control 4 (in Idle mode/ 8-colors)
  write_data(0x8D);
  write_data(0x2A);

  write_command(0xC4); //Power Control 5 (in Partial mode/ full-colors)
  write_data(0x8D);
  write_data(0xEE);
  //-------------------------------   End ST7735S Power Sequence   --------------------------------//
  
  write_command(0xC5); //VCOM
  write_data(0x03);
    
  //-------------------------------- Start ST7735S Gamma Sequence  --------------------------------//  
  write_command(0xE0);
  write_data(0x12);
  write_data(0x1C);
  write_data(0x10);
  write_data(0x18);
  write_data(0x33);
  write_data(0x2C);
  write_data(0x25);
  write_data(0x28);
  write_data(0x28);
  write_data(0x27);
  write_data(0x2F);
  write_data(0x3C);
  write_data(0x00);
  write_data(0x03);
  write_data(0x03);
  write_data(0x10);

  write_command(0xE1);
  write_data(0x12);
  write_data(0x1C);
  write_data(0x10);
  write_data(0x18);
  write_data(0x2D);
  write_data(0x28);
  write_data(0x23);
  write_data(0x28);
  write_data(0x28);
  write_data(0x26);
  write_data(0x2F);
  write_data(0x3B);
  write_data(0x00);
  write_data(0x03);
  write_data(0x03);
  write_data(0x10);
  //---------------------------------  End ST7735S Gamma Sequence  ---------------------------------//
  
  write_command(0xFC);
  write_data(0x8C);

  //-------------------------------------  Read Display Setup  -------------------------------------// 
  write_command(0x36); //Memory Data Access Control
  #ifdef rotate180
     write_data(0x40); //MY=0, MX=1, Top-Bottom, RGB mode (Works as Bottom-Top refresh, BGR mode)
  #else
     //write_data(0x90); //MX=0, MY=1, Top-Bottom refresh, RGB mode
     write_data(0xD8); //MY=1, MX=1, Bottom-Top, BGR mode (Appears as Top-Bottom refresh, RGB mode)
  #endif
  // Note: BITMAP images are displayed upside-down with reverse (BGR) colours.
  //       Use opposite settings for text/graphics (see SET_BITMAP and SET_GRAPHICS functions)
  
  write_command(0x3A);   //Interface Pixel Format
  #ifdef SET_12bit
     write_data(0x03);   //0x03 = 12-bit ppx  
  #elif SET_16bit
     write_data(0x05);   //0x05 = 16-bit ppx
  #elif SET_18bit
     write_data(0x06);   //0x06 = 18-bit per pixel
  #else //Default (undefined)
     write_data(0x06);   //18-bit per pixel
  #endif

  SET_COLUMN_ADDRESS(offset_X, end_X);
  SET_PAGE_ADDRESS(offset_Y, end_Y);

  ExitSleep();
  delay(25);
  write_command(0x2C);
}//end Init
//*********************************************************************************************

void EnterSleep(void)
{
    write_command(0x28);
    delay(20);
    write_command(0x10);
}

void ExitSleep(void)
{
    write_command(0x11);  // Exit Sleep
    delay(120);
    write_command(0x29);  // Display On
}

void SET_for_BITMAP(uint8_t X, uint8_t Y, uint8_t width, uint8_t height)
{
   write_command(0x36);                    //Change Memory Data Access 
   #ifdef rotate180
      write_data(0x90);                    //MX=0, MY=1, Bottom-Top refresh, RGB mode
      SET_COLUMN_ADDRESS(X, X+width-1);
      SET_PAGE_ADDRESS(Y+32, Y+height+31);
   #else
      write_data(0x40);                    //MY=0, MX=1, Top-Bottom refresh, RGB mode
      SET_COLUMN_ADDRESS(X, X+width-1);
      SET_PAGE_ADDRESS(Y, Y+height-1);
   #endif
}
void SET_for_GRAPHICS(void)
{
   write_command(0x36);                   //Change Memory Data Access 
   #ifdef rotate180
      write_data(0x08);                   //MY=0, MX=1, Top-Bottom, BGR mode (Appears as Bottom-Top refresh, RGB mode)
   #else
      write_data(0xD8);                   //MY=1, MX=1, Bottom-Top, BGR mode (Appears as Top-Bottom refresh, RGB mode)
   #endif
}
