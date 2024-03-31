/*
 * FINGERPRINT_int.h
 *
 *  Created on: Mar 4, 2023
 *      Author: Mahmoud_Ali
 */

#ifndef FINGERPRINT_INT_H_
#define FINGERPRINT_INT_H_

char_t HFINGERORINT_charSendCommand2FP(u8 Copy_u8Order);

void HFINGERPRINT_voidInit(void);

void HFINGERPRINT_voidUARTTransmit(u8 Copy_u8Data);

u8 HFRINGERPRINT_u8UARTRecieve(void);

void HFINGERPRINT_voidTransmitHeader(u16 Copy_u16lenght);

u16 HFINGERPRINT_u16Receive(void);

u16 HFINGERPRINT_u16SearchReceive(void);

char_t * HFINGERPRINT_pcharGetResponseString(u8 Copy_u8Code);

u16 HFINGERPRINT_u16Read(void);

u16 HFINGERPRINT_u16UpImg(void);

u16 HFINGERPRINT_u16UpChar(u16 Copy_u16BufferId);

u16 HFINGERPRINT_u16GenerateCharFile(u16 Copy_u16BufferId);

u16 HFINGERPRINT_u16GenerateTemplate(void);

u16 HFINGERPRINT_u16GetStorageLocation(void);

u16 HFINGERPRINT_u16Store(u16 Copy_u16Location);

u16 HFINGERPRINT_u16Delete(u16 Copy_u16Location);

u16 HFINGERPRINT_u16Search(void);

#endif /* FINGERPRINT_INT_H_ */
