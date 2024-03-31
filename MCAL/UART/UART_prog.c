/*****************************************************************
 * FILE DESCRIPTION:
----------------------------------------------------------------
 *  file    : UART_prog.c
 *  details : Source file for UART Driver.
 *****************************************************************/

/******************************************
  INCLUDES
 *******************************************/

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "Mcu_HW.h"

#include "UART_priv.h"
#include "UART_types.h"
#include "UART_cfg.h"
#include "UART_int.h"

/******************************************
  Local Data
 *******************************************/
/*global variable used as a flag of interrupt source of transmission [either a string or a character]*/
u8 TRAN_INTERRUPT_SRC = IDLE ;

/******************************************
  Global Data
 *******************************************/
/*global pointer to set received data asynchronously*/
u8 * GlobalPtrToRecChar = NULL ;

/*global string to send string asynchronously */
char * GlobalString  = NULL;



/******************************************
  Local Functions  body
 *******************************************/
/*Global pointer to function for end of transmission ISR*/
void (*EndOfTransmission)(void) = NULL ;

/*Global pointer to function for end of receive ISR*/
void (*EndOfReceive)(void) = NULL ;

/******************************************
  Global Functions body
 *******************************************/
Error_State MUART_enInit(const UART_ConfigType * Copy_pxUart)
{
	//local variable for error checking
	Error_State Local_enErrorState = NO_ERROR;

	//local variable to assign value of UCSRC register*/
	//u8 Local_u8UCSRC_Value = 0;

	/*Switching over modes of USART
	switch(Copy_pxUart->Mode)
	{
	//in case of running as a transmitter
	case TRANSMITTER:
		SET_BIT(UCSRB,UCSRB_TXEN);
		break;
		//in case of running as a receiver
	case RECEIVER:
		SET_BIT(UCSRB,UCSRB_RXEN);
		break;
	case TRANS_AND_REC:
		SET_BIT(UCSRB,UCSRB_TXEN);
		SET_BIT(UCSRB,UCSRB_RXEN);
		break;
	default:
		Local_enErrorState = ERROR;
		break;
	}*/

	/*Switching over different options of exchanged data size*/
	/*switch(Copy_pxUart->CharSize)
	{
	case CHAR_SIZE_5:
		Local_u8UCSRC_Value = (1<<UCSRC_URSEL) | ((Copy_pxUart->Mode)<<UCSRC_UMSEL) | ((Copy_pxUart->Parity)<<UCSRC_UPM0) | ((Copy_pxUart->StopBits)<<UCSRC_USBS) | (0<<UCSRC_UCSZ1) | (0<<UCSRC_UCSZ0);
		UCSRC = Local_u8UCSRC_Value;
		CLR_BIT(UCSRB,UCSRB_UCSZ2);
		break;
	case CHAR_SIZE_6:
		Local_u8UCSRC_Value = (1<<UCSRC_URSEL) | ((Copy_pxUart->Mode)<<UCSRC_UMSEL) | ((Copy_pxUart->Parity)<<UCSRC_UPM0) | ((Copy_pxUart->StopBits)<<UCSRC_USBS) | (0<<UCSRC_UCSZ1) | (1<<UCSRC_UCSZ0);
		UCSRC = Local_u8UCSRC_Value;
		CLR_BIT(UCSRB,UCSRB_UCSZ2);
		break;
	case CHAR_SIZE_7:
		Local_u8UCSRC_Value = (1<<UCSRC_URSEL) | ((Copy_pxUart->Mode)<<UCSRC_UMSEL) | ((Copy_pxUart->Parity)<<UCSRC_UPM0) | ((Copy_pxUart->StopBits)<<UCSRC_USBS) | (1<<UCSRC_UCSZ1) | (0<<UCSRC_UCSZ0);
		UCSRC = Local_u8UCSRC_Value;
		CLR_BIT(UCSRB,UCSRB_UCSZ2);
		break;
	case CHAR_SIZE_8:
		Local_u8UCSRC_Value = (1<<UCSRC_URSEL) | ((Copy_pxUart->Mode)<<UCSRC_UMSEL) | ((Copy_pxUart->Parity)<<UCSRC_UPM0) | ((Copy_pxUart->StopBits)<<UCSRC_USBS) | (1<<UCSRC_UCSZ1) | (1<<UCSRC_UCSZ0);
		UCSRC = Local_u8UCSRC_Value;
		CLR_BIT(UCSRB,UCSRB_UCSZ2);
		break;
	case CHAR_SIZE_9:
		Local_u8UCSRC_Value = (1<<UCSRC_URSEL) | ((Copy_pxUart->Mode)<<UCSRC_UMSEL) | ((Copy_pxUart->Parity)<<UCSRC_UPM0) | ((Copy_pxUart->StopBits)<<UCSRC_USBS) | (1<<UCSRC_UCSZ1) | (1<<UCSRC_UCSZ0);
		SET_BIT(UCSRB,UCSRB_UCSZ2);
		break;
	default:
		Local_enErrorState = ERROR;
		break;
	}*/

	UCSRB |= 0x18;

	UCSRC |= 0x86;

	/* Baud Rate = 9600 */
	UBBRL = 51;

	return Local_enErrorState;
}

void MUART_voidSendByteSync(u8 Copy_u8Data)
{

	/*assigning data to UDR to get transmitted*/
	UDR = Copy_u8Data;

	/*polling till UDR register is ready to receive new data*/
	while((GET_BIT(UCSRA , UCSRA_UDRE))== 0);

	/*polling till end of transmission*/
	while((GET_BIT(UCSRA , UCSRA_TXC)) == 0);

	/*clearing end of transmission flag*/
	CLR_BIT(UCSRA , UCSRA_TXC);
}

Error_State MUART_enSendByteAsync(u8 Copy_u8Data , void (*CopyPtrToFunc)(void))
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	/*setting flag of source of interrupt to be a single character*/
	TRAN_INTERRUPT_SRC = CHAR;

	/*polling till UDR register is ready to receive new data*/
	while((GET_BIT(UCSRA , UCSRA_UDRE))== 0);

	/*assigning data to UDR to get transmitted*/
	UDR = Copy_u8Data;

	/*enable end of transmission interrupt*/
	SET_BIT(UCSRB, UCSRB_TXCIE);

	/*checking for notification function*/
	if(CopyPtrToFunc != NULL)
	{
		EndOfTransmission = CopyPtrToFunc ;
	}
	else
	{
		Local_enErrorState = NULL_POINTER ;
	}
	return Local_enErrorState;
}

u8 MUART_u8ReceiveByteSync(void)
{
	/*polling till data is completely received*/
	while((GET_BIT(UCSRA , UCSRA_RXC))==0);

	/*clearing flag and getting received data*/
	return UDR;
}

Error_State MUART_enReceiveByteAsync(u8 * Copy_pu8ReceivedData , void (*CopyPtrToFunc)(void))
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	/*enable end of receive interrupt*/
	SET_BIT(UCSRB, UCSRB_RXCIE);

	/*Setting global pointer to access the same address*/
	GlobalPtrToRecChar = Copy_pu8ReceivedData;

	/*checking for notification function*/
	if(CopyPtrToFunc != NULL)
	{
		EndOfReceive = CopyPtrToFunc;
	}
	else
	{
		Local_enErrorState = NULL_POINTER;
	}

	return Local_enErrorState;
}

void MUART_voidSendStringSync(char_t * Copy_pcharString)
{
	/*local variable to loop over characters of the string*/
	u8 Local_u8Counter = 0 ;

	/*looping over string characters*/
	while(Copy_pcharString[Local_u8Counter] != '\0')
	{
		/*sending characters one by one*/
		MUART_voidSendByteSync(Copy_pcharString[Local_u8Counter]);

		Local_u8Counter++;
	}
}

Error_State MUART_enSendStringAsync(char_t * Copy_pcharString , void (*CopyPtrToFunc)(void))
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	/*setting flag of source of interrupt to be a string*/
	TRAN_INTERRUPT_SRC = STRING;

	/*set global pointer to access the same address*/
	GlobalString = Copy_pcharString;

	/*polling till UDR register is ready to receive new data*/
	while((GET_BIT(UCSRA , UCSRA_UDRE))== 0);

	if((Copy_pcharString != NULL) && (Copy_pcharString[0] != '\0'))
	{
		/*assigning data to UDR to get transmitted*/
		UDR = Copy_pcharString[0];
	}
	else
	{
		Local_enErrorState = ERROR;
	}

	/*enable end of transmission interrupt*/
	SET_BIT(UCSRB, UCSRB_TXCIE);

	/*checking for notification function*/
	if(CopyPtrToFunc != NULL)
	{
		EndOfTransmission = CopyPtrToFunc;
	}
	else
	{
		Local_enErrorState = NULL_POINTER;
	}

	return Local_enErrorState;

}

void MUART_voidReceiveBufferSync(char_t *Copy_pcharReceivedBuffer , u8 Copy_u8BufferSize)
{
	/*local variable to loop over buffer elements*/
	u8 Local_u8Counter = 0 ;

	/*looping over buffer elements*/
	for(Local_u8Counter=0; Local_u8Counter<Copy_u8BufferSize; Local_u8Counter++)
	{
		/*getting received character*/
		Copy_pcharReceivedBuffer[Local_u8Counter] = MUART_u8ReceiveByteSync();
	}
}


void MUART_voidSendArray(const char_t * Copy_pcharData , u8 Copy_u8Size)
{
	u8 Local_u8Counter;

	for(Local_u8Counter = 0; Local_u8Counter<Copy_u8Size;Local_u8Counter++)
	{
		MUART_voidSendByteSync(*Copy_pcharData);
		*Copy_pcharData++;
	}
}

/*ISR for USART end of transmission*/
void __vector_15 (void)
{
	/*in case end of transmission of a single character*/
	if(TRAN_INTERRUPT_SRC == CHAR)
	{
		/*calling notification function*/
		EndOfTransmission();
	}
	else if(TRAN_INTERRUPT_SRC == STRING)
	{
		u8 Local_u8Counter = 1;

		/*sending remaining characters*/
		while(GlobalString[Local_u8Counter] != '\0')
		{
			MUART_voidSendByteSync(GlobalString[Local_u8Counter]);
			Local_u8Counter++;
		}

		/*calling notification function*/
		EndOfTransmission();
	}
	else
	{
		/* Do nothing */
	}
}

/*ISR for USART end of receive*/
void __vector_13 (void)
{
	/*getting received character*/
	*GlobalPtrToRecChar = UDR;

	/*calling notification function*/
	EndOfReceive();
}
