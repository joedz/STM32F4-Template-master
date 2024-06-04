#ifndef __AW3644_H
#define __AW3644_H

#include "stm32f4xx.h"
#include "AW3644_Reg.h"

#ifdef __cplusplus
extern "C" {
#endif


void AW3644_Init(void);
	
void AW3644_Write(uint8_t reg, uint8_t data);
uint8_t AW3644_Read(uint8_t reg);

uint8_t AW3644_GetChipID(void);



#ifdef __cplusplus
}
#endif

#endif // __AW3644_H
