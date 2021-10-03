#include "stm32f10x_i2c.h"
#include "i2c.h"
#include "u8g_arm.h"

//static u8g_t u8g;



void display_init(void);
void draw(int words);
void display(void);