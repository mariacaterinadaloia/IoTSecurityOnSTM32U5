/*
 * main.h
 *
 *  Created on: Jan 14, 2024
 *      Author: colg
 */

#ifndef INC_GENCLOCK_H_
#define INC_GENCLOCK_H_

#include "stm32u5xx_hal.h"

//Function prototype
void SystemClock_Config(void);
void RTC_Init(void);
void Error_Handler(void);
unsigned long genera_seme_pseudocasuale(void);
unsigned long hash_seme(unsigned long seme);
void genera_password(char* password, int lunghezza, unsigned long hash);
void aes_encrypt(uint8_t *input, uint8_t *output, uint8_t *key, uint8_t *iv);
void SystemClock_Config(void);
void MX_USART2_UART_Init(void);
void MX_LPUART1_UART_Init(void);
void MX_GPIO_Init(void);
void Error_Handler(void);


#endif /* INC_GENCLOCK_H_ */
