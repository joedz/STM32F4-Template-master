#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // __USART_H
