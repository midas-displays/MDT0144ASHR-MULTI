#include "display.h"


/**************************************************************************************
 *                                  DISPLAY ROUTINES
 **************************************************************************************/
//Fill screen with any colour using standard hex colour codes, e.g. 0x00FF00 (green)
void fill_screen(unsigned int colour)
{
   unsigned char x,y;
   
   SET_for_GRAPHICS();                   //set orientation
   SET_COLUMN_ADDRESS(offset_X, end_X);  //set memory addresses
   SET_PAGE_ADDRESS(offset_Y, end_Y);
   
   write_command(0x2C);                  //write to memory 
   for(y=0;y<disp_height;y++)
   {
      for(x=0;x<disp_width;x++)
      { 
         write_24bitRGB(colour);
      }
   }     
}

//writes an single character ch at x,y using font indicated by size and of selected colour on selected bgcolour
void display_Char(unsigned int x, unsigned int y, char ch, unsigned short size,  unsigned int colour, unsigned int bgcolour)
{
   unsigned int i=0, c=0;
   unsigned short ycount=0, xcount=0, w=0, h=0;
   if (size==1) {w=7,h=10;}                        //size 1 is 7x10
   if (size==2) {w=11,h=18;}                       //size 2 is 11x18
   if (size==3) {w=16,h=26;}                       //size 3 is 16x26
   SET_for_GRAPHICS();                             //set correnct orientation (see display.h)
   SET_COLUMN_ADDRESS(x+offset_X, offset_X+x+w-1); //set adress window to enclose the whole character
   SET_PAGE_ADDRESS(y+offset_Y, offset_Y+y+h-1);
   i=(ch-32)*h;                                    //skip first 32 ASCII characters
   write_command(0x2C);                            //write to memory
   for (ycount=0;ycount<h;ycount++)                //y count starts at top row
   {
      if (size==1) {c=Font7x10[i+ycount];}         //which depends on font size
      if (size==2) {c=Font11x18[i+ycount];}        //counting through each row
      if (size==3) {c=Font16x26[i+ycount];}        //of the selected font
      
      for (xcount=0; xcount<w; xcount++)           //x count steps through each pixel
      {  
         if (c & 0x8000) {                         //of the font row
            write_24bitRGB(colour);
         }
         else {
            write_24bitRGB(bgcolour);
         }
         c=c<<1;                                      //shift to next bit in row.
      }
   }
}

//writes a text string, starting at x,y of selected size, forgound and background colour
void display_String(unsigned int x, unsigned int y, const char *str, unsigned short size, unsigned int colour, unsigned int bgcolour)
{
   unsigned short w=0,h=0;                     //determine character step size
   if (size==1) {w=7,h=10;}                    //for various fonts
   if (size==2) {w=11,h=18;}
   if (size==3) {w=16,h=26;}
   while (*str) {
      if (x + w > disp_width) {
         x = 0;
         y += h;
         if (y + h > disp_height) break;
         if (*str == ' ')
         {   // skip spaces in the beginning of the new line
             str++;
             continue;
         }
      }
      display_Char(x, y, *str, size, colour, bgcolour);
      x += w;
      str++;
   }
}
/*************************************************************************************
 * The following function displays RGB colours on the screen sequentially with respect
 * to HUE, just like the rainbow spectrum. Adjust the speed of the sequence by giving
 * values to the 'steps' and 'time' variables.
 * Version 2 (v2) will now dispay text on the screen with the colour values.
 *************************************************************************************/

// TO DO
// Convert int to string and display RGB values to display.

void rainbow_screen_v2(int steps, int time_ms)
{
   unsigned char R,G,B;
   unsigned int paint = 0;
   
   // HUE = 0 deg.
   R=255; G=0; B=0;
   for(G=0; G<255; G+=steps)
   {
      paint = (R<<16)+(G<<8)+B;
      fill_screen(paint);
      //display_String(10,80,"Rainbow",2,BLACK,paint);
      delay(time_ms);
   }
   // HUE = 60 deg.
   R=255; G=255; B=0;
   for(R=255; R>0; R-=steps)
   {
      paint = (R<<16)+(G<<8)+B;
      fill_screen(paint);
      //display_String(10,89,"Rainbow",2,BLACK,paint);
      delay(time_ms);
   }
   // HUE = 120 deg.
   R=0; G=255; B=0;
   for(B=0; B<255; B+=steps)
   {
      paint = (R<<16)+(G<<8)+B;
      fill_screen(paint);
      //display_String(10,80,"Rainbow",2,BLACK,paint);
      delay(time_ms);
   }
   // HUE = 180 deg.
   R=0; G=255; B=255;
   for(G=255; G>0; G-=steps)
   {
      fill_screen((R<<16)+(G<<8)+B);
      //Serial.println(colours,HEX);
      delay(time_ms);
   }
   // HUE = 240 deg.
   R=0; G=0; B=255;
   for(R=0; R<255; R+=steps)
   {
      fill_screen((R<<16)+(G<<8)+B);
      //Serial.println(colours,HEX);
      delay(time_ms);
   }
   // HUE = 300 deg.
   R=255; G=0; B=255;
   for(B=255; B>0; B-=steps)
   {
      fill_screen((R<<16)+(G<<8)+B);
      //Serial.println(colours,HEX);
      delay(time_ms);
   }
}

/**************************************************************************************
 *                                  PATTERNS
 **************************************************************************************/

//Draws a box from botton left pos_x=0 pos_y=0 size in pixels and colour in 24bit mode.
unsigned char draw_square(int ssize, int pos_x, int pos_y, long colour) 
{
   int x,y,temp1,temp2;
     
   if(pos_x>=disp_width || pos_y>=disp_height)
      return(1);            //abort - outside bounds.
   
   temp1=pos_y;
   SET_COLUMN_ADDRESS(pos_x+offset_X, end_X);
   SET_PAGE_ADDRESS((pos_y+offset_Y), end_Y);

   write_command(0x2C);             //Write to memory
   digitalWrite(DC,HIGH);

   for(y=0;y<ssize;y++)
   {
      for(x=0;x<ssize;x++)
      { 
         write_24bitRGB(colour);
      }  
      temp1++;
      if(temp1+((pos_x+ssize)/disp_width)>=disp_height)
         return(0);
      
      SET_COLUMN_ADDRESS(pos_x, end_X);
      SET_PAGE_ADDRESS((temp1+offset_Y), end_Y);

      write_command(0x2C);              //Write to memory
      digitalWrite(DC,HIGH);
   }//end for loop
   return(0);
}

void draw_rect(int width, int height, int pos_x, int pos_y, long colour) 
{
   unsigned int x,y;
     
   if(pos_x>=disp_width || pos_y>=disp_height)
      return;            //abort - outside bounds.
   
   SET_COLUMN_ADDRESS(pos_x+offset_X, pos_x+offset_X+width-1);
   SET_PAGE_ADDRESS(pos_y+offset_Y, pos_y+offset_Y+height-1);
   
   write_command(0x2C);             //Write to memory
   for(y=0;y<height;y++)
   {
      for(x=0;x<(width*3);x++)
      { 
         write_24bitRGB(colour);
      }
   }
}

/**************************************************************************************
 *                                  RTP FUNCTIONS
 **************************************************************************************/
void touch_mode()
{
   int Vx, Vx0, Vx1, Vx2, Vx3;
   int Vy, Vy0, Vy1, Vy2, Vy3; 
  
  // Read Y coordinate  
  pinMode(XL,OUTPUT);
  pinMode(XR,OUTPUT);
  pinMode(YU,INPUT);   
  pinMode(YD,INPUT);
  digitalWrite(XL,HIGH);
  digitalWrite(XR,LOW);
  delay(10); //give enough time to settle ports

  Vy0 = analogRead(YU);
  Vy1 = analogRead(YD);
  delay(1);
  Vy2 = analogRead(YU);
  Vy3 = analogRead(YD);

  Vy = (Vy0+Vy1+Vy2+Vy3)/4;
  if (Vy<260) Vy=260; //min
  if (Vy>650) Vy=650; //max
  Yd = (Vy-260)/3.05;
  #ifdef rotate180
     Yd = disp_height - Yd;
  #endif

//  DEBUGGING
//  Serial.print("Y="); Serial.print(Vy); Serial.print(" YU="); Serial.print(Vy0); Serial.print(" YD="); Serial.print(Vy3);
//  Serial.print(" | Yd = "); Serial.print(Yd);
//  Serial.print("\n");    

  // Read X coordinate
  pinMode(XL,INPUT);  
  pinMode(XR,INPUT);
  pinMode(YU,OUTPUT);  digitalWrite(YU,HIGH);
   pinMode(YD,OUTPUT);  digitalWrite(YD,LOW);
  delay(10); //give enough time to settle ports

  Vx0 = analogRead(XR);
  Vx1 = analogRead(XL);
  delay(1);
  Vx2 = analogRead(XR);
  Vx3 = analogRead(XL);

  Vx = (Vx0+Vx1+Vx2+Vx3)/4;
  if (Vx<260) Vx=260; //min
  if (Vx>690) Vx=690; //max
  Xd = (Vx-260)/3.36;
  #ifdef rotate180
     Xd = disp_width - Xd;
  #endif

//  Serial.print("X="); Serial.print(Vx); Serial.print(" XL="); Serial.print(Vx1); Serial.print(" XR="); Serial.print(Vx2);
//  Serial.print(" | Xd = "); Serial.print(Xd);
//  Serial.println("\n--------"); 

}

uint8_t touch_detect()
{
   int vXL, vXR;
    
   pinMode(XL,INPUT);  
   pinMode(XR,INPUT);
   pinMode(YU,OUTPUT);  digitalWrite(YU,HIGH);
   pinMode(YD,OUTPUT);  digitalWrite(YD,LOW);
   delay(10);
   
   vXL = analogRead(XL); //Read X
   vXR = analogRead(XR);

   if(vXL>10 || vXR>10) //When no touch, XL and XR should read 0.
      return 1;
   else
      return 0;
}
