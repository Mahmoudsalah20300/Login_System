/*
 * main.c
 *
 *  Created on: Feb 19, 2023
 *      Author: Mahmoud_Ali
 */
#include <util/delay.h>

#include "STD_TYPES.h"
#include "Led_int.h"
#include "PB_int.h"
#include "Dio_int.h"
#include "Port_int.h"
#include "CLCD_int.h"
#include "KPD_int.h"
#include "UART_int.h"
#include "EXTI_int.h"
#include "GIE_int.h"
#include "TIMER1_int.h"
#include "ULTRASONIC_int.h"




#define WrongPass	1
#define RightPass	0
#define MaxTryNum	3

u8 ScanPassword (void);
u8 PasswordState = WrongPass;


extern const Port_Configtype Port_ConfigArr[PORT_PINS_CONFIG_ARR_SIZE];
void main(void)
{

	u16 Local_u16Distance=0;
	u8 Local_u8Flag;
	u8 Local_u8NumberOfTries= 0;
	u8 Local_u8KeyPressed = NO_PRESSED_KEY_STATE;

	MPort_voidInit(Port_ConfigArr);
	HCLCD_voidInit();
	HUltrasonic_voidinit();

	HCLCD_voidClearScreen();

	while(1)
	{
		HULTRASONIC_voidTrigger();
		Local_u16Distance = MTIMER1_u16ICUTimeOn(TIMER1_8_PRESCALER);

		if(Local_u16Distance <= 15)
		{
			MDIO_voidSetPinValue(ULTRASONIC_TRIGGER_PIN, DIO_LOW);

			HCLCD_voidSendString("Hello");
			HCLCD_voidGotoXY(1, 0);
			_delay_ms(1500);
			HCLCD_voidSendString("Enter Password:");

			_delay_us(10);
			HCLCD_voidClearScreen();
			Local_u8Flag = ScanPassword();

			switch(Local_u8Flag)
			{
			case WrongPass:
				while(Local_u8Flag == WrongPass)
				{
					Local_u8NumberOfTries++;
					HCLCD_voidClearScreen();
					HCLCD_voidSendString("Wrong Password");
					HCLCD_voidGotoXY(1, 1);
					HCLCD_voidSendString("Please Try Again");
					_delay_ms(100);

					Local_u8Flag = ScanPassword();

					if(Local_u8NumberOfTries>=MaxTryNum)
					{
						HCLCD_voidClearScreen();
						HCLCD_voidSendString("Wait 2 minutes");
						_delay_ms(2000);
						Local_u8NumberOfTries = 0;
					}

					if(Local_u8NumberOfTries<= MaxTryNum && Local_u8Flag == RightPass)
					{
						HCLCD_voidClearScreen();
						HCLCD_voidSendString("Correct Password");
						HCLCD_voidGotoXY(1, 1);
						HCLCD_voidSendString("Welcome Mohamed");
						_delay_ms(100);
						break;
					}
					break;
			case RightPass:
				HCLCD_voidClearScreen();
				HCLCD_voidSendString("Correct Password");
				HCLCD_voidGotoXY(1, 1);
				HCLCD_voidSendString("Welcome Mohamed");
				_delay_ms(100);
				break;
				}

				HCLCD_voidClearScreen();
				HCLCD_voidSendString("1- LED ON");
				HCLCD_voidGotoXY(1, 0);
				HCLCD_voidSendString("2- LED OFF");
				_delay_ms(100);
			}
			while(1)
			{
				do
				{
					Local_u8KeyPressed = HKPD_u8GetPressedKey();
				}while(Local_u8KeyPressed == NO_PRESSED_KEY_STATE);
				_delay_ms(200);

				if(Local_u8KeyPressed == '1')
				{
					HCLCD_voidClearScreen();
					HCLCD_voidSendString("LED ON");
					HLED_voidTurnOn(LED_BLUE);
				}
				else if(Local_u8KeyPressed == '2')
				{
					HCLCD_voidClearScreen();
					HCLCD_voidSendString("LED OFF");
					HLED_voidTurnOff(LED_BLUE);
				}

			}
		}
	}

}



u8 ScanPassword (void)
{
	u8 Local_u8Flag =0;
	u8 Local_u8Counter;

	char_t Local_charPassword[4] = {'1' , '2' , '3' , '4'};
	char_t Local_charPassLogin[4];

	HCLCD_voidClearScreen();
	for(Local_u8Counter=0; Local_u8Counter<4;Local_u8Counter++)
	{
		do
		{
			Local_charPassLogin[Local_u8Counter] = HKPD_u8GetPressedKey();
		}while(Local_charPassLogin[Local_u8Counter] == NO_PRESSED_KEY_STATE);


		HCLCD_voidSendData(Local_charPassLogin[Local_u8Counter]);

		if(Local_charPassLogin[Local_u8Counter] != Local_charPassword[Local_u8Counter])
		{
			Local_u8Flag=1;
		}
	}

	return Local_u8Flag;
}

