/*
 * FINGERPRINT_prog.c
 *
 *  Created on: Mar 4, 2023
 *      Author: Mahmoud_Ali
 */
#include <util/delay.h>
#include "STD_TYPES.h"

#include "UART_int.h"
#include "CLCD_int.h"

#include "FINGERPRINT_priv.h"
#include "FINGERPRINT_types.h"
#include "FINGERPRINT_int.h"

extern const UART_ConfigType UART_xConfig;

char_t HFINGERORINT_charSendCommand2FP(u8 Copy_u8Order)
{
	u8 Local_u8Successed = again;
	u8 Local_u8ID;
	u16 Local_u16ArrSize;
	while(Local_u8Successed == again)
	{
		HCLCD_voidClearScreen();

		switch(Copy_u8Order)
		{
		case FB_connect:
			HCLCD_voidSendString("Search 4 Module");
			break;
		case F_find:
			HCLCD_voidSendString("Please UR Finger");
			break;
		case F_im1:
			HCLCD_voidSendString("Processing...");
			break;
		case F_im2:
			HCLCD_voidSendString("Processing...");
			break;
		case F_cretModl:
			HCLCD_voidSendString("Creating Model");
			break;
		case FP_empty:
			HCLCD_voidSendString("IDs Erasing");
			break;
		case F_search:
			HCLCD_voidSendString("Searching...");
			break;
		case F_store:
			HCLCD_voidSendString("Storing Model");
			break;
		case F_delete:
			HCLCD_voidSendString("Delete Finger");
			break;
		}

		//First three Packages (Common between any command)
		MUART_voidSendArray(&Header,2);
		MUART_voidSendArray(&Address,4);
		MUART_voidSendArray(&Command,2);

		switch(Copy_u8Order)
		{
		case FB_connect:
			Local_u16ArrSize = (sizeof(PassVfy)) / sizeof((PassVfy)[0]);
			MUART_voidSendArray(&PassVfy, Local_u16ArrSize);
			break;
		case F_find:
			Local_u16ArrSize = (sizeof(f_detect)) / sizeof((f_detect)[0]);
			MUART_voidSendArray(&f_detect, Local_u16ArrSize);
			break;
		case F_im1:
			Local_u16ArrSize = (sizeof(f_imz2ch1)) / sizeof((f_imz2ch1)[0]);
			MUART_voidSendArray(&f_imz2ch1,Local_u16ArrSize);
			break;
		case F_im2:
			Local_u16ArrSize = (sizeof(f_imz2ch2)) / sizeof((f_imz2ch2)[0]);
			MUART_voidSendArray(&f_imz2ch2,Local_u16ArrSize);
			break;
		case F_cretModl:
			Local_u16ArrSize = (sizeof(f_createModel)) / sizeof((f_createModel)[0]);
			MUART_voidSendArray(&f_createModel,Local_u16ArrSize);
			break;
		case FP_empty:
			Local_u16ArrSize = (sizeof(fp_empty)) / sizeof((fp_empty)[0]);
			MUART_voidSendArray(&fp_empty,Local_u16ArrSize);
			break;
		case F_search:
			Local_u16ArrSize = (sizeof(f_search)) / sizeof((f_search)[0]);
			MUART_voidSendArray(&f_search,Local_u16ArrSize);
			break;
		case F_store:
			Local_u8ID = HFINGERPRINT_voidGetID();
			f_storeModel[4] = Local_u8ID;
			f_storeModel[6] = (0xE + Local_u8ID);
			Local_u16ArrSize = (sizeof(f_storeModel)) / sizeof((f_storeModel)[0]);
			MUART_voidSendArray(&Header ,2);
			MUART_voidSendArray(&Address,4);
			MUART_voidSendArray(&Command,2);
			MUART_voidSendArray(&f_storeModel,Local_u16ArrSize);
			break;
		case F_delete:
			Local_u8ID = HFINGERPRINT_voidGetID();
			f_delete[3] = Local_u8ID;
			f_delete[7] = (0x15 + Local_u8ID);
			Local_u16ArrSize = (sizeof(f_delete)) / sizeof((f_delete)[0]);
			MUART_voidSendArray(&Header ,2);
			MUART_voidSendArray(&Address,4);
			MUART_voidSendArray(&Command,2);
			MUART_voidSendArray(&f_delete,Local_u16ArrSize);
			break;
		}
		_delay_ms(1000);

		if(cont>1)
		{
			if(rcvData[6] == 0x07 && (rcvData[8] == 0x03 || rcvData[8] == 0x07))
			{
				HCLCD_voidGotoXY(1, 0);

				if(rcvData[9] == 0)
				{
					Local_u8Successed = success;
				}
				else
				{
					Local_u8Successed = fail;
					if(rcvData[9]==0x01)
					{
						HCLCD_voidSendString("Rcv packet error");
					}
					else if(rcvData[9]==0x04)
					{
						HCLCD_voidSendString("fail. Try Again");
					}
				}
			}
		}
	}
}

void HFINGERPRINT_voidInit(void)
{
	MUART_enInit(&UART_xConfig);
}

void HFINGERPRINT_voidUARTTransmit(u8 Copy_u8Data)
{
	MUART_voidSendByteSync(Copy_u8Data);
}

u8 HFRINGERPRINT_u8UARTRecieve(void)
{
	u8 Local_u8Finger;

	Local_u8Finger = MUART_u8ReceiveByteSync();

	return Local_u8Finger;
}

void HFINGERPRINT_voidTransmitHeader(u16 Copy_u16lenght)
{
	/* Header */
	HFINGERPRINT_voidUARTTransmit(239);
	HFINGERPRINT_voidUARTTransmit(1);

	/* Address */
	HFINGERPRINT_voidUARTTransmit(0xFF);
	HFINGERPRINT_voidUARTTransmit(0xFF);
	HFINGERPRINT_voidUARTTransmit(0xFF);
	HFINGERPRINT_voidUARTTransmit(0xFF);

	/* Package Identifier (PID) */
	HFINGERPRINT_voidUARTTransmit(1);

	/* Package Length */
	HFINGERPRINT_voidUARTTransmit(Copy_u16lenght & 0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16lenght & 0x00FF);
}

u16 HFINGERPRINT_u16Receive(void)
{
	u16 Local_u16Length;
	u16 Local_u16Result;

	/* Header */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	/* Module Address */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	/* PID */
	HFRINGERPRINT_u8UARTRecieve();

	/* Package Length */
	Local_u16Length = ((HFRINGERPRINT_u8UARTRecieve()<<8) | (HFRINGERPRINT_u8UARTRecieve()));
	Local_u16Length -=2;

	Local_u16Result = HFRINGERPRINT_u8UARTRecieve();
	if(Local_u16Length == 0x0002)
	{
		Local_u16Result = (HFRINGERPRINT_u8UARTRecieve()<<8);
	}
	else if(Local_u16Length == 0x0003)
	{
		if(Local_u16Result==0x00)
		{
			Local_u16Result = ((HFRINGERPRINT_u8UARTRecieve()<<8) | (HFRINGERPRINT_u8UARTRecieve()));
		}
	}

	/* CheckSum */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	return Local_u16Result;
}

u16 HFINGERPRINT_u16SearchReceive(void)
{
	u8 Local_u8Temp;
	u8 Local_u8Low;
	u8 Loca_u8High;
	u16 Local_u16Reult = 0;

	/* Header */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	/* Module Address */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	/* PID */
	HFRINGERPRINT_u8UARTRecieve();

	/* Package Length */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	Local_u8Temp = HFRINGERPRINT_u8UARTRecieve();

	/* Page id */
	if(Local_u8Temp == 0x01)
	{
		Local_u8Low = HFRINGERPRINT_u8UARTRecieve();

		Loca_u8High = HFRINGERPRINT_u8UARTRecieve();

		Local_u16Reult = ((Local_u8Low<<8)+Loca_u8High);
	}
	else if(Local_u8Temp == 0x09)
	{
		Local_u16Reult =0;
		HFRINGERPRINT_u8UARTRecieve();
		HFRINGERPRINT_u8UARTRecieve();
	}
	else
	{
		Local_u16Reult = HFRINGERPRINT_u8UARTRecieve();
		Local_u16Reult = (HFRINGERPRINT_u8UARTRecieve()<<8);
	}

	/* Score */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	/* CheckSum */
	HFRINGERPRINT_u8UARTRecieve();
	HFRINGERPRINT_u8UARTRecieve();

	return Local_u16Reult;
}

char_t * HFINGERPRINT_pcharGetResponseString(u8 Copy_u8Code)
{
	switch(Copy_u8Code)
	{
	case 0x00: return "complete";
	case 0x01: return "error receiving data";
	case 0x02: return "no finger";
	case 0x03: return "fail to enroll";
	case 0x06: return "disorderly fingerprint image";
	case 0x07: return "small fingerprint image";
	case 0x08: return "finger doesn’t match";
	case 0x09: return "fail to find the matching";
	case 0x0A: return "fail to combine";
	case 0x0B: return "invalid PageID";
	case 0x0C: return "invalid template";
	case 0x0D: return "error uploading template";
	case 0x0E: return "can’t data packages.";
	case 0x0F: return "error uploading image";
	case 0x10: return "fail to delete template";
	case 0x11: return "fail to clear library";
	case 0x13: return "wrong password!";
	case 0x15: return "invalid primary image";
	case 0x18: return "error writing flash";
	case 0x19: return "undefined error";
	case 0x1A: return "invalid register";
	case 0x1B: return "incorrect configuration";
	case 0x1C: return "wrong notepad page";
	case 0x1D: return "failed communication port";
	}
	return "Unknown error";
}

u16 HFINGERPRINT_u16Read(void)
{
	u16 Local_u16Code = 2;

	while(Local_u16Code != 0x0000)
	{
		HFINGERPRINT_voidTransmitHeader(0x0003);

		HFINGERPRINT_voidUARTTransmit(0x01);

		/* Sum */
		HFINGERPRINT_voidUARTTransmit(0x00);
		HFINGERPRINT_voidUARTTransmit(0x05);

		Local_u16Code = HFINGERPRINT_u16Receive();

		_delay_ms(500);
	}

	return Local_u16Code;
}

u16 HFINGERPRINT_u16UpImg(void)
{
	HFINGERPRINT_voidTransmitHeader(0x0003);

	HFINGERPRINT_voidUARTTransmit(0x0A);

	HFINGERPRINT_voidUARTTransmit(0x00);
	HFINGERPRINT_voidUARTTransmit(0x0E);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16UpChar(u16 Copy_u16BufferId)
{
	HFINGERPRINT_voidTransmitHeader(0x0004);

	HFINGERPRINT_voidUARTTransmit(0x08);

	HFINGERPRINT_voidUARTTransmit(Copy_u16BufferId);

	Copy_u16BufferId += 7;
	HFINGERPRINT_voidUARTTransmit(Copy_u16BufferId&0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16BufferId&0x00FF);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16GenerateCharFile(u16 Copy_u16BufferId)
{
	HFINGERPRINT_voidTransmitHeader(0x0004);

	HFINGERPRINT_voidUARTTransmit(0x02);

	HFINGERPRINT_voidUARTTransmit(Copy_u16BufferId);

	Copy_u16BufferId += 7;
	HFINGERPRINT_voidUARTTransmit(Copy_u16BufferId&0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16BufferId&0x00FF);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16GenerateTemplate(void)
{
	HFINGERPRINT_voidTransmitHeader(0x0003);

	HFINGERPRINT_voidUARTTransmit(0x05);

	HFINGERPRINT_voidUARTTransmit(0x00);
	HFINGERPRINT_voidUARTTransmit(0x09);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16GetStorageLocation(void)
{
	HFINGERPRINT_voidTransmitHeader(0x0003);

	HFINGERPRINT_voidUARTTransmit(0x1D);

	HFINGERPRINT_voidUARTTransmit(0x00);
	HFINGERPRINT_voidUARTTransmit(0x21);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16Store(u16 Copy_u16Location)
{
	HFINGERPRINT_voidTransmitHeader(0x0006);

	HFINGERPRINT_voidUARTTransmit(0x06);

	HFINGERPRINT_voidUARTTransmit(0x01);

	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0x00FF);

	Copy_u16Location += 14;

	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0x00FF);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16Delete(u16 Copy_u16Location)
{
	HFINGERPRINT_voidTransmitHeader(0x0007);

	HFINGERPRINT_voidUARTTransmit(0x0C);

	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0x00FF);

	HFINGERPRINT_voidUARTTransmit(0x00);
	HFINGERPRINT_voidUARTTransmit(0x01);

	Copy_u16Location += 21;
	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0xFF00);
	HFINGERPRINT_voidUARTTransmit(Copy_u16Location&0x00FF);

	return HFINGERPRINT_u16Receive();
}

u16 HFINGERPRINT_u16Search(void)
{
	HFINGERPRINT_voidTransmitHeader(0x0008);

	HFINGERPRINT_voidUARTTransmit(0x04);

	HFINGERPRINT_voidUARTTransmit(0x01);

	// Start Location
	HFINGERPRINT_voidUARTTransmit(0x00);
	HFINGERPRINT_voidUARTTransmit(0x01);
	// Length
	HFINGERPRINT_voidUARTTransmit(0x00);
	HFINGERPRINT_voidUARTTransmit(0xFF);

	// Sum
	HFINGERPRINT_voidUARTTransmit(0x01);
	HFINGERPRINT_voidUARTTransmit(0x0E);

	return HFINGERPRINT_u16SearchReceive();
}
