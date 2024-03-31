/*
 * OS_int.h
 *
 *  Created on: Mar 5, 2023
 *      Author: m_hsa
 */

#ifndef OOS_INT_H_
#define OS_INT_H_

void OS_voidStart(void);

void OS_voidCreateTask(u8 copy_u8Priority , u16 copy_u16Periodicity , void (*copy_pvTaskFunc)(void));
#endif /* OS_INT_H_ */
