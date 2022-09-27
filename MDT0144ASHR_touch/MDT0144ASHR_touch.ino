/**************************************************************************************
 *                                 MDT0144ASHR-MULTI
 *
 * Size/Type:       1.44"  TFT
 * Resolution:      128x128
 * Description:     TFT connected to a uC32 via an MCIB-12 and MPBV6 adaptor boards 
 *                  using 8-bit 8080 Parallel interface. In addition this display has 
 *                  a resistive touch panel (RTP), which the demo utilizes.
 *                  
 * IC:              ST7735S
 * MCU:             uC32
 * Accessories:     MCIB-12, MPBV6
 * Company:         Midas Displays
 * Author(s):       I. Antonakis
 * Date:            28/03/2022
 * Release:         v 1.8
 *
 *************************************************************************************/

#include "display.h"

unsigned char sdcard;
char  file_names[50][13];
int num_of_files; 
File imageFile, graphicFile;
unsigned long count;

int get_file_list(File dir);
void display_image(int nFile);
void display_graphic(char* FileDir, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void gallery(unsigned int time_ms);
void squares(unsigned int time_cs);
void draw(uint8_t penSize, uint32_t penColour, unsigned long timer);
void font_page(unsigned int timeout);
void demo_page(unsigned int timeout);
/**************************************************************************************
 *                                  SET-UP FUNCTION
 *************************************************************************************/
void setup() 
{
   Serial.begin(9600);
   Serial.println("Initializing DEMO");
   
   //Set port directions.  
   pinMode(DC, OUTPUT);   
   pinMode(RESET, OUTPUT);  
   //pinMode(IM0, OUTPUT);
   pinMode(IM2, OUTPUT);
   pinMode(SPI4W, OUTPUT);

   //Select interface      ->   Parallel 8-bit 
   //digitalWrite(IM0, LOW);   // 0
   digitalWrite(IM2, HIGH);  // 1
   digitalWrite(SPI4W, LOW); // 0
   
   //Toggle reset and enable display.
   digitalWrite(RESET,HIGH);
   delay(1);
   digitalWrite(RESET,LOW);
   delay(1);
   digitalWrite(RESET,HIGH);
   delay(120);
   
   //Initialization routines
   setup_PMP_for_display();
   Init_ST7735S(); 
   
   SET_for_GRAPHICS();
   fill_screen(MAGENTA);

   Serial.print("Checking... ");
   // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
   // Note that even if it's not used as the CS pin, the hardware SS pin 
   // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
   // or the SD library functions will not work. 
   //pinMode(10, OUTPUT);
   
   if (!SD.begin(SD_detect)) 
   {
      sdcard=0;
      Serial.println("NO SD card!");
   }
   else
   {
      sdcard=1;
      Serial.println("SD card detected.");
      //Get list of image files and sort them by name
      imageFile = SD.open("/");
      num_of_files = get_file_list(imageFile);  
      imageFile.close();
   }  
   delay(500);
   /*
   Serial.println("Flash test"); //Flash colours to indicate the display is working
   fill_screen(RED);
   delay(500);
   fill_screen(GREEN);
   delay(500);
   fill_screen(BLUE);
   delay(500);
   fill_screen(BLACK);
   delay(500);
   //*/
   
} // END of setup()

/**************************************************************************************
 *                                     MAIN LOOP
 *************************************************************************************/
void loop() 
{
   count = 0;
   //-------- Menu Button --------
   display_graphic("/menu/start.bmp",0,0,128,128);
   //delay(1000);

   while(count<1000)
   {
      if(touch_detect())
      {
         touch_mode();
         if(Xd>15 && Xd<110 && Yd>45 && Yd<83)
         {
            display_graphic("/menu/touched.bmp",16,44,96,40);
            while(touch_detect()); //wait while touched
            display_graphic("/menu/start.bmp",0,0,128,128);
            delay(200);
            break;
         }
      }
      delay(10);
      count++;
   }

   //-------- Demo Page --------
   demo_page(8000);

   //-------- Drawing Canvas --------
   draw(4, RED, 1000);
   
   //-------- Page with fonts --------
   font_page(8000);

   //-------- Images from SD card --------
   if(sdcard) gallery(4000);
   //squares(600);

      
   //rainbow_screen_v2(3,100);
   
} // End of loop()

/**************************************************************************************
 *                                   MAIN ROUTINES
 *************************************************************************************/
void gallery(unsigned int time_ms)
{
   int nfile;
   unsigned int timeout; 
   for(nfile=0; nfile<num_of_files; nfile++)
   {
      display_image(nfile);
      //delay(time_ms);
      
      timeout = time_ms/10;
      while(timeout--) { //exit when time is out or touch detected
         if(touch_detect()) {  
            while(touch_detect()); 
            return;
         }
      } //end timout
   } //end for loop
}
void squares(unsigned int time_cs)
{
   unsigned int t, r, g, b;
   long temp;
   fill_screen(RED);
   for(t=0;t<time_cs;t++)      //draw random filled boxes
   { 
      r=rand() % 256;
      g=rand() % 256;
      b=rand() % 256;
      temp=(r<<16)+(g<<8)+b;
      draw_square((rand()%100)+1,(rand()%disp_width),(rand()%disp_height)-3,temp);
      delay(10);  //ms
   }
}
/**************************************************************************************
 *                                 SD CARD  FUNCTIONS
 **************************************************************************************
 * The following functions read and display BMP images from an SD card. They are based
 * on the Arduino SD library (SD.h) and can work on a variety of displays. 
 **************************************************************************************/
//The strcmp() function compares two strings and returns 0 if both strings are identical
int cmpfunc( const void *a, const void *b)
{
   char const *aa = (char const *)a;
   char const *bb = (char const *)b;
   return strcmp(aa, bb);
}
//Gets a list of bmp files from selected directory and sorts them alpha-numerically
int get_file_list(File dir)
{
   int number=0;

   while (true) 
   {
      File entry =  dir.openNextFile();
      if (! entry) 
         break;
      strcpy(file_names[number],entry.name());
      entry.close();
      number++;
   }
   qsort(&file_names,number,sizeof(file_names[0]),cmpfunc);
   return(number);
}
void display_image(int nFile)
{
   unsigned int x, y;
   imageFile = SD.open(file_names[nFile]);              //opens SD card
   if((imageFile.read()=='B')&&(imageFile.read()=='M')) //checks if file is a BMP image
   { 
      SET_for_BITMAP(0,0, disp_width, disp_height);     //Set BMP orientation and memory addressing
      
      imageFile.seek(0x36);           //Goto start of image data (0x36 for 24bit BMP)
      write_command(0x2C);            //write to memory
      for(y=0;y<disp_height;++y)
      {
         for(x=0;x<(disp_width*3);++x)
         {
            write_data(imageFile.read()); //write image to display 
         }

         switch(disp_width%4)         //strip out packing bytes from end of bitmap line
         {
         case 3:  
            imageFile.read();  
         case 2:  
            imageFile.read();
         case 1:  
            imageFile.read();
         case 0:  
            break;
         default: 
            break;
         }
      }
   }
   imageFile.close();
}
void display_graphic(char* FileDir, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
   unsigned int i, j;
   graphicFile = SD.open(FileDir);              //opens SD card
   
   SET_for_BITMAP(x, y, width, height);     //Set BMP orientation and memory addressing
   
   graphicFile.seek(0x36);               //Goto start of image data (0x36 for 24bit BMP)
   write_command(0x2C);
   for(i=0;i<height;++i)
   {
      for(j=0;j<(width*3);++j)
      {
         write_data(graphicFile.read()); //write image to display 
      }

      switch(width%4)
      {
      case 3:  
         graphicFile.read();  
      case 2:  
         graphicFile.read();
      case 1:  
         graphicFile.read();
      case 0:  
         break;
      default: 
         break;
      }
   }
   graphicFile.close();
}

// Draw ----------------------------------------------------------------------------

void draw(uint8_t penSize, uint32_t penColour, unsigned long timer)
{
   fill_screen(BLACK);                           //canvas
   display_String(36,10,"Draw",3,WHITE,BLACK);
   while(timer)
   {
      if(touch_detect())
      {
         touch_mode();
         if((Xd+penSize)>=disp_width) Xd = disp_width-penSize-1;  //correct boundary errors
         if((Yd+penSize)>=disp_height) Yd = disp_height-penSize-1;
         draw_square(penSize, Xd, Yd, penColour);
         //exit feature
         if(Xd>36 && Xd<92 && Yd>10 && Yd<36) 
         {
            display_String(36,10,"Exit",3,MAGENTA,CYAN);
            while(touch_detect());
            return;
         }
      }
      delay(10);
      timer--;
   }
}

void font_page(unsigned int timeout)
{
   unsigned char smallDelay = 100;    //delay between text in 10ms steps
   timeout = (timeout/10)+smallDelay; //delay at the end (1ms to 10ms steps)
   SET_for_GRAPHICS();
   fill_screen(GREEN);
   // display_String(36,0,"DEMO",3,BLACK,GREEN);
   display_Char(92,18,'A',1,BLACK,WHITE);
   display_Char(99,10,'A',2,BLACK,WHITE);
   display_Char(110,2,'A',3,BLACK,WHITE);
   while(smallDelay--)  {
      if(touch_detect()) {      //delay 1s or exit on touch 
         while(touch_detect()); //wait while touched
         return;
      }
   }  smallDelay = 100;    //reset small delay
   display_String(0,20,"Font 1:",1,BLACK,GREEN);
   display_String(1,30,"abcdefghijklmnopqr",1,BLUE,BLACK); //7x10 18 chars
   while(smallDelay--)  {
      if(touch_detect()) {  
         while(touch_detect()); 
         return;
      }
   }  smallDelay = 100; 
   display_String(0,40,"Font 2:",2,BLACK,GREEN);
   display_String(3,58,"STUVWXYZ_01",2,RED,CYAN); //11x18 - 11 chars
   while(smallDelay--)  {
      if(touch_detect()) {  
         while(touch_detect()); 
         return;
      }
   }  smallDelay = 100; 
   display_String(0,75,"Font 3:",3,BLACK,GREEN);
   display_String(0,102,"23456789",3,YELLOW,BLACK); //16x26 - 8 chars

   while(timeout--) { //exit when time is out or touch detected
      if(touch_detect()) {  
         while(touch_detect()); 
         break;
      }
   } //end timout
}

void demo_page(unsigned int timeout)
{
   unsigned int delay1=0, delay2=0, delay3;
   int P=100,T=25;
   char pressure[5];
   char temp[3];
   
   timeout = (timeout/10); 
   
   SET_for_GRAPHICS();
   fill_screen(SILVER);
   //draw_rect(128, 26, 0, 0, GOLD);
   display_String(20,0," DEMO ",3,BLACK,GOLD);
   display_String(5,40,"Temp.C:",2,BLACK,SILVER); display_String(90,40,"25",2,BLACK,SILVER);
   
   display_String(5,75,"Pressure:",1,BLACK,SILVER); display_String(70,75,"100 Pa",1,BLACK,SILVER);
   delay(100);
   //draw_rect(200, 100, 10, 110, ORANGE);

   while(timeout--) {           //exit when time runs out or touch detected
      delay1++;
      delay2++;
      if(touch_detect()) {      //takes ~ 10ms
         while(touch_detect()); 
         break;
      }
      display_String(70,75,itoa(P++,pressure,10),1,BLACK,SILVER);
      if(delay2==20) {
         display_String(90,40,itoa(T++,temp,10),2,BLACK,SILVER);
         delay2=0;
      }
      if(delay1==100)     // ~ 1 second
         display_String(0,100,"WARNING!",3,RED,BLACK);
      if(delay1==200) {   // ~ 2 seconds
         draw_rect(128, 26, 0, 100, SILVER);
         delay1=0;
      }
   } //end timout
}
