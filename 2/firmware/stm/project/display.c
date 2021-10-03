#include "display.h"
#include "measure.h"
#include "string.h"


//*******************************************************************
static u8g_t u8g;
extern ct ct1,ct2;
extern vt vtac;
extern p p1,p2;

void display_init(void)
	{
		u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, u8g_com_hw_i2c_fn);
		}
void draw(int words)
{
  u8g_SetFont(&u8g, u8g_font_profont10);
  
  //u8g_DrawBox(&u8g, 3,41,20,10);// (x,y,width,height)
  //u8g_DrawFrame(&u8g, 25,41,20,10);// (x,y,width,height)
  
  //u8g_DrawCircle(&u8g, 10,58,5,U8G_DRAW_ALL);// (x,y,radius)
  //u8g_DrawDisc(&u8g, 30,58,5,U8G_DRAW_ALL);
  //u8g_DrawDisk(&u8g, x, y, rad, [options])
  
  //u8g_DrawEllipse(&u8g,55,46,8,4,U8G_DRAW_ALL);
  //u8g_DrawFilledEllipse(&u8g,75,46,8,4,U8G_DRAW_ALL);
 
  //u8g_DrawLine(&u8g,84,1,127,16);//( x,y - x,y)
  //u8g_DrawVLine(&u8g,88,38,28);// (x,y,length)
  //u8g_DrawHLine(&u8g,55,55,28);//(x,y,Length)

  //Print a variable: (no float support)
  //a=0;
  //xsprintf(array, "a=%03d",a); 
  //u8g_DrawStr(&u8g, 12, 56, array);

 // u8g_DrawRFrame(&u8g, 2,6,126,43,8);// (x,y,h,l,radius)

  if (words==1 || words==3){
    u8g_DrawStr180(&u8g, 100, 24, "SSD1306 OLED");
  }

  if (words==2 || words==3){
    u8g_DrawStr180(&u8g, 100, 42, "STM32F103");
   }
  
}

void display(void)
	{
		u8g_FirstPage(&u8g);
    do{
      //draw(2);
			  u8g_SetFont(&u8g, u8g_font_profont10);
			
			//print UAC_0.1V
			char array[20];
			  sprintf(array, "UAC_V = %3.1f",(float) vtac.Uavr); 
				u8g_DrawStr180(&u8g, 127, 56, array);
			//print I1_mA
				sprintf(array, "I1_A = %1.3f",(float) (ct1.Iavr/1000)); 
				u8g_DrawStr180(&u8g, 127, 42, array);
				//print I2_mA
				sprintf(array, "I2_A = %1.3f",(float) (ct2.Iavr/1000)); 
				u8g_DrawStr180(&u8g, 127, 28, array);
				//print P1_mW
				sprintf(array, "P1_W = %2.2f",(float) (p1.Pavr/1000)); 
				u8g_DrawStr180(&u8g, 127, 14, array);
				//print P2_mW
				sprintf(array, "P2_W = %2.2f",(float) (p2.Pavr/1000)); 
				u8g_DrawStr180(&u8g, 127, 0, array);
				//print f_Hz
				sprintf(array, "f_Hz = %2.2f",(float) vtac.favr); 
				u8g_DrawStr180(&u8g, 53, 28, array);
				//print PF1
				sprintf(array, "PF1 = %2.2f",(float) p1.PFavr); 
				u8g_DrawStr180(&u8g, 53, 14, array);
				//print PF2
				sprintf(array, "PF2 = %2.2f",(float) p2.PFavr); 
				u8g_DrawStr180(&u8g, 53, 0, array);
			
			} while (u8g_NextPage(&u8g));
	}