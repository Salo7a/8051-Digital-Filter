#include <REGX51.H>
#include <stdio.h>
sfr LCD_Port=0x90;				 				/* P1 port as data port */
sbit rs=P1^0;        					 				/* Register select pin */
sbit rw=P1^1;        									/* Read/Write pin */
sbit en=P1^2;        									/* Enable pin */
int ADCValue = 0;
sbit ale=P3^3;  
sbit oe=P3^0; 
sbit sc=P3^2; 
sbit eoc=P3^1; 
sbit clk=P3^7;  
#define input_port P0  //ADC
#define output_port P2
char Val[5];

void timer0() interrupt 1  // Function to generate clock of frequency 500KHZ using Timer 0 interrupt.
{
clk=~clk;
}

void delay(unsigned int count)    		
{
    int i,j;
     for(i=0;i<count;i++)
     for(j=0;j<112;j++);
}
void read_adc()
{
   ADCValue=0;
   ale=1;
   sc=1;
   delay(1);
   ale=0;
   sc=0;
   while(eoc==1);
   while(eoc==0);
   oe=1;
   ADCValue=input_port;
   delay(1);
}

void LCD_Command (char cmnd)   /* LCD16x2 command funtion */
{
     LCD_Port =(LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
     rs=0;	 													  /* command reg. */
     rw=0;														  /* Write operation */
     en=1; 
		delay(1);
			en=0;
		delay(2);
	
	  LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
		 en=1; 									/* enable pulse */
		delay(1);
		en=0;
		delay(5);
}

void LCD_Char (char char_data)							/* LCD data write function */
{
    LCD_Port =(LCD_Port & 0x0F) | (char_data & 0xF0); /* sending upper nibble */    
    rs=1;									/*Data reg.*/
    rw=0;									/*Write operation*/
		en=1;   				
		delay(1);
		en=0;
		delay(2);

	  LCD_Port = (LCD_Port & 0x0F) | (char_data << 4);  /* sending lower nibble */
		en=1; 									/* enable pulse */
		delay(1);
		en=0;
		delay(5);

}

void LCD_String (char *str)							/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)							/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);							/* Call LCD data write */
	}
}

void LCD_String_xy (char row, char pos, char *str)	/* Send string to LCD function */
{
	if (row == 0)
	LCD_Command((pos & 0x0F)|0x80);					/* Command of first row and required position<16 */
	else if (row == 1)
	LCD_Command((pos & 0x0F)|0xC0);					/* Command of first row and required position<16 */
	LCD_String(str);								/* Call LCD string function */
}

void LCD_Init (void)					/* LCD Initialize function */
{
	delay(20);					/* LCD Power ON Initialization time >15ms */
	
	LCD_Command (0x02);				/* 4bit mode */
	LCD_Command (0x28);				/* Initialization of 16X2 LCD in 4bit mode */
	LCD_Command (0x0C);				/* Display ON Cursor OFF */
	LCD_Command (0x06);				/* Auto Increment cursor */
	LCD_Command (0x01);				/* clear display */
	LCD_Command (0x80);				/* cursor at home position */
}
void LCD_Clear(void)
{
	LCD_Command (0x01);	
	LCD_Command (0x80);
}

void main()
{

	LCD_Init();							/* initialization of LCD*/
	eoc=1;
	ale=0;
	oe=0;
	sc=0;
	TMOD=0x02;
	TH0=0xFD;
	LCD_String("ADC Connecting");
	delay(500);
	LCD_Clear();
	LCD_String("ADC Value");
	IE=0x82;
	TR0=1;
	while(1)
	{
		LCD_String_xy(1,1,"   ");
		read_adc();
		LCD_Command(0xc0);					/*go to 2nd line*/
		output_port = ADCValue;
		sprintf(Val, "%d", ADCValue);
		LCD_String_xy(1,1,Val);			/*write string on 2nd line*/
		ADCValue=0;
		delay(200);
	}
	
}
 