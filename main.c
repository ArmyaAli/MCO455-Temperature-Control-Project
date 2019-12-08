/*
	Title:		Final Project - Temperature Control Project
	Author:		Manalo_Kenneth 100284181, Umar_Ali 148218183   MCO455 Lab Section: E
	Date:		11/22/2019
	Description: 	Using seven segment displays and pushbuttons, this program manipulates 
					setpoint and temperature values, outputting it to an LCD display and 
					PC Screen as well as increasing and decreasing the values using pushbuttons. 
					On the seven segment display shows a default value of 20 degrees celsius for
					the setpoint and temperature. A connection between TP10 and TP8 is made to
					read the temperature values and is used to also determine when temperature is
					rising and falling indicating a heater/fan/cooler condition that is outputted
					to the PC Screen and LCD display. 
*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include <E:/library_de1.h>

unsigned char setPt;
unsigned char Temp;
unsigned int count;
unsigned static const  char digits[] = {0x02, 0x9e, 0x24, 0x0c, 
										0x98, 0x48, 0x40, 0x1e, 
										0x00, 0x18};
unsigned char threshold;

void LCD_MENU(void);
void startup(void);
void updateLCDScreenLine2(void);
void updateHexDisplayIncreaseSetpoint(void);
void updateHexDisplayDecreaseSetpoint(void);
void printPCScreen(void);
void updateSetPt(void);
void updateTempRead(void);
void updateAllIncrease(void);
void updateAllDecrease(void);
void printOFF(void);

void main(void) 
{
	setPt = 0x14;
		Temp = 0x14;
		threshold = 0x14;
		SCI1BDL = 0X82;
		SCI1C2 = 0X0C;
		SOPT_COPE = 0;
		
		devices_init();

		LCD_MENU();
		scr_clear();
		printPCScreen();
		startup();
		
		for(;;)
		{
			if(Temp != setPt && SW_KEY2 || SW_KEY3)
			{
				analog_write_int(1, setPt << 4);
				threshold = (analog_read_int(1)) >> 4;
				
				if(Temp < threshold)
				{
					LEDRL = 0x01;
					LEDG = 0x80;
					lcd_setcursor(3, 0);
					lcd_print("ON   ON    OFF");
					Temp++;
				}
				else if(Temp > threshold)
				{
					LEDG = 0xA0;
					lcd_setcursor(3, 0);
					lcd_print("OFF  ON    ON  ");
					Temp--;
				}
				
				lcd_setcursor(1, 13);
				lcd_write(Temp);
				updateTempRead();
				
				HEX1 = digits[hex2dec(Temp) >> 4];
				HEX0 = digits[hex2dec(Temp) & 0x0F];
				updateLCDScreenLine2();
				
				for(count = 0; count < 6; count++)
				{
					delay_milli(100);
				}
			}
			if(!SW_KEY2)
			{
				updateAllIncrease();
			}
			if(!SW_KEY3)
			{
				updateAllDecrease();
			}
			if(!SW_KEY1) //Advanced Section
			{
				while(SCI1S1 & 0x80)
					SCI1D = Temp;
					
				if(SCI1S1 & 0x20)
				{
					unsigned char b;
					b = SCI1D;
				}
			}
			else
			{
				if(Temp == setPt)
				{
					printOFF();
					LEDG = 0x00;
					LEDRL = 0x00;
				}
			}
			delay_milli(100);
	}
}

void startup(void)
{
	HEX3 = digits[2];
	HEX2 = digits[0];
	HEX1 = digits[2];
	HEX0 = digits[0];
}

void updateHexDisplayIncreaseSetpoint(void)

{	
	if(hex2dec(setPt) < 0x35)
	{
		setPt++;
		HEX3 = digits[hex2dec(setPt) >> 4];
		HEX2 = digits[hex2dec(setPt) & 0x0F];
		
		delay_milli(200);
	}
	
}

void updateHexDisplayDecreaseSetpoint(void)
{
	if(hex2dec(setPt) > 0x10)
	{
		setPt--;
		HEX3 = digits[hex2dec(setPt) >> 4];
		HEX2 = digits[hex2dec(setPt) & 0x0F];
		
		delay_milli(200);
	}
}

void updateLCDScreenLine2(void)
{
	lcd_setcursor(1, 5);
	lcd_write(hex2dec(setPt));
	
	lcd_setcursor(1, 13);
	lcd_write(hex2dec(Temp));
}

void LCD_MENU(void)
{
	LCD_ROWS = 4;
	LCD_COLS = 16;
	
	lcd_setcursor(0, 0); // line 1
	lcd_print("TEMP Ctrl System");
	
	lcd_setcursor(1, 0); // line 2
	lcd_print("StPt 20  Tmp 20");
	
	lcd_setcursor(1, 7); // line 2
	lcd_writech(0xDF);
	
	lcd_setcursor(1, 15); // line 2
	lcd_writech(0xDF);
	
	lcd_setcursor(2, 0);
	lcd_print("HTR  FAN   Cool"); // line 3
	
	lcd_setcursor(3, 0);
	lcd_print("OFF  OFF   OFF");                            //
}

void printPCScreen(void)                                    //function used to print to PC Screen
{
	scr_setcursor(1,19);
	scr_print("Temperature Control System Monitor Program");//prints Temperature Control System Monitor Program on the PC screen
	
	scr_setcursor(3,34);
	scr_print("Local System");                              //prints Local System on the PC Screen
	
	scr_setcursor(5,19);
	scr_print("SETPOINT TEMPERATURE");                      //prints SETPOINT TEMPERATURE on the PC Screen
	
	scr_setcursor(6,26);
	scr_print("20");                                        //prints 20 on the PC screen
	scr_print("DEG ");                                      //prints DEG on the PC screen
	scr_print("C");                                         //prints C on the PC screen
	
	scr_setcursor(5,43);
	scr_print("TEMPERATURE READING");                       //prints TEMPERATURE READING on the PC screen
	
	scr_setcursor(6,53);
	scr_print("20");                                        //prints 20 on the PC screen
	scr_print("DEG ");                                      //prints DEG on the PC screen
	scr_print("C");                                         //prints C on the PC screen
	
	scr_setcursor(9,19);    
	scr_print("HEATING SYSTEM");                            //prints HEATING SYSTEM on the PC screen
	
	scr_setcursor(9,39);
	scr_print("FAN");                                       //prints FAN on the PC screen
	
	scr_setcursor(9,47);
	scr_print("COOLING SYSTEM");                            //prints COOLING SYSTEM on the PC screen
	
	scr_setcursor(16,33);
	scr_print("REMOTE SYSTEM");                             //prints REMOTE SYSTEM on the PC screen
	
	scr_setcursor(18,27); 
	scr_print("SETPOINT");                                  //prints SETPOINT on the PC screen
	
	scr_setcursor(18,42); 
	scr_print("TEMPERATURE");                               //prints TEMPERATURE on the PC screen
}

void updateSetPt(void)
{
	scr_setcursor(6,26);
	scr_write(hex2dec(setPt));
	scr_print("DEG ");                                      //prints DEG on the PC Screen
	scr_print("C");                                         //prints C on the PC Screen
}

void updateTempRead(void)		
{
	scr_setcursor(6, 53);
	scr_write(hex2dec(Temp));
	scr_print("DEG ");                                     //prints DEG on the PC Screen
	scr_print("C");                                        //prints C on the PC Screen
}

void updateAllIncrease(void)
{
	lcd_setcursor(3, 0);
	lcd_print("ON   ON    OFF");
	
	scr_setcursor(10,25);
	scr_print("ON ");                                      //prints ON on the PC Screen
				
	scr_setcursor(10,39);
	scr_print("ON ");                                      //prints ON on the PC Screen
				
	scr_setcursor(10,53);
	scr_print("OFF");                                      //prints OFF on the PC Screen
	
	do
	{
		updateHexDisplayIncreaseSetpoint();
		updateLCDScreenLine2();
		updateSetPt();
	}while(!SW_KEY2);
}

void updateAllDecrease(void) 	
{
	scr_setcursor(10,25);
	scr_print("OFF");
							
	scr_setcursor(10,39);
	scr_print("ON ");
							
	scr_setcursor(10,53);
	scr_print("ON ");
	
	lcd_setcursor(3, 0);
	lcd_print("OFF  ON    ON  ");
	
	do{
	updateHexDisplayDecreaseSetpoint();
	updateLCDScreenLine2();
	updateSetPt();
	}while(!SW_KEY3);
}

void printOFF(void)                                        //prints "OFF" to the LCD and PC Screen
{
	lcd_setcursor(3, 0);
	lcd_print("OFF  OFF   OFF ");
				
	scr_setcursor(10,25);
	scr_print("OFF");
							
	scr_setcursor(10,39);
	scr_print("OFF");
							
	scr_setcursor(10,53);
	scr_print("OFF");
}










