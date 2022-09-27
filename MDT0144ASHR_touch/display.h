#include "io.h"
#include "Fonts.h"

//Uncomment to rotate display by 180 degrees
//#define rotate180

#define  disp_width   128
#define  disp_height  128

#ifdef rotate180
  #define  offset_X     0  //The ST7735S has a 132x162 RAM size
  #define  offset_Y     0  //For smaller displays you may need these offsets
#else 
  #define  offset_X     0
  #define  offset_Y     32 // offset_Y = RAM_Y - displayY (e.g. 160-128=32)
#endif
#define  end_X       (disp_width + offset_X -1)
#define  end_Y       (disp_height + offset_Y -1)

//Global display touch coordinates
  int Xd, Yd;

//-------- Colours --------
#define BLACK    0x000000
#define WHITE    0xffffff
#define RED      0xff0000
#define GREEN    0x00ff00
#define BLUE     0x0000ff
#define YELLOW   0xffff00
#define MAGENTA  0xff00ff
#define CYAN     0x00ffff

#define BRONZE   0xcd7f32
#define SILVER   0xc0c0c0
#define GOLD     0xffd700

#define ORANGE   0xffa500
//-------------------------

void SET_COLUMN_ADDRESS(unsigned int start_addr, unsigned int end_addr);
void SET_PAGE_ADDRESS(unsigned int start_addr, unsigned int end_addr);
void Init_ST7735S(void);
void EnterSleep(void);
void ExitSleep(void);
void SET_for_BITMAP(uint8_t X, uint8_t Y, uint8_t width, uint8_t height);
void SET_for_GRAPHICS(void);


void fill_screen(unsigned int colour);
void rainbow_screen_v2(int steps, int time_ms);

unsigned char draw_square(int ssize, int pos_x, int pos_y, long colour);
void draw_rect(int width, int height, int pos_x, int pos_y, long colour);
void display_Char(unsigned int x, unsigned int y, char ch, unsigned short size,  unsigned int colour, unsigned int bgcolour);
void display_String(unsigned int x, unsigned int y, const char *str, unsigned short size, unsigned int colour, unsigned int bgcolour);

void touch_mode();
uint8_t touch_detect();
uint8_t read_touch(uint8_t Xd, uint8_t Yd);
