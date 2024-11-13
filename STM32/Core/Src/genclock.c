#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_uart.h"
#include "stm32u5xx_hal_rtc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "genclock.h"
#define AES_KEY_SIZE 16
#define DATA_SIZE 16
#define DATA_SIZE_R 256

RTC_HandleTypeDef hrtc;
UART_HandleTypeDef huart2;
UART_HandleTypeDef hlpuart1;
HAL_StatusTypeDef status = HAL_ERROR;
HAL_StatusTypeDef statusTime1 = HAL_ERROR;
HAL_StatusTypeDef statusTime2 = HAL_ERROR;
HAL_StatusTypeDef statusUart = HAL_ERROR;
HAL_StatusTypeDef statusLPUart = HAL_ERROR;
uint8_t receivedData[DATA_SIZE_R];
uint16_t bytesReceived = 0;
uint8_t aes_key[AES_KEY_SIZE] = {0x4E, 0x5A, 0x1B, 0xE2, 0x78, 0xDE, 0x08, 0x9A, 0x89, 0x0D, 0x6E, 0x9C, 0x05, 0x85, 0x99, 0x90};

uint8_t iv[AES_KEY_SIZE] = {0x8C, 0x0A, 0x15, 0x8E, 0xD8, 0x36, 0xC8, 0x24, 0xB6, 0x06, 0x7A, 0x95, 0x4F, 0xF3, 0x06, 0x4F};
char password[DATA_SIZE];
int count = 0;
int r;
int check = 5;
uint8_t encrypted_data[DATA_SIZE];
uint8_t rxBuffer[1];

void aes_encrypt(uint8_t *input, uint8_t *output, uint8_t *key, uint8_t *iv) {
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, input, DATA_SIZE);
    memcpy(output, input, DATA_SIZE);
}

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

void MX_LPUART1_UART_Init(void)
{

  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
    count++;
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
    count++;
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
    count++;
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
    count++;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *hlpuart1) {
    if (hlpuart1->Instance == LPUART1) {
        receivedData[bytesReceived++] = rxBuffer[0];


        if (rxBuffer[0] == '\n' || bytesReceived >= DATA_SIZE_R) {
            bytesReceived = 0;
        }
        HAL_UART_Receive_IT(hlpuart1, rxBuffer, 1);
    }
}


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; 
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

unsigned long genera_seme_pseudocasuale(void) {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    
    statusTime1 = HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    statusTime2 = HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // Necessario per sbloccare i registri

    unsigned long seme = ((unsigned long)sDate.Year << 16) | (sDate.Month << 12) | (sDate.Date << 8) | (sTime.Hours << 4) | sTime.Minutes;
    return seme;
}

unsigned long hash_seme(unsigned long seme) {
    seme = (seme ^ 0xDEADBEEF) * 0x12345677;
    return seme;
}

void genera_password(char* password, int lunghezza, unsigned long hash) {
    char *lettere_minuscole = "abcdefghijklmnopqrstuvwxyz";
    char *lettere_maiuscole = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *numeri = "0123456789";
    char *simboli = "!@$%^&*()_-=[]{}|:,.<>";
    char caratteri_possibili[256];
    r = 2;
    int i = 0;

    strcpy(caratteri_possibili, lettere_minuscole);
    strcat(caratteri_possibili, lettere_maiuscole);
    strcat(caratteri_possibili, numeri);
    strcat(caratteri_possibili, simboli);


    int lunghezza_set = strlen(caratteri_possibili);
    unsigned long seme;
    for (i = 0; i < lunghezza; i++) {
        password[i] = caratteri_possibili[hash % lunghezza_set];
        r = rand() % (65 + 1 - 2) + 2;
        if(hash != (hash / r%lunghezza_set)){
            hash /= r%lunghezza_set;
        }else{
            seme = genera_seme_pseudocasuale();
            hash = hash_seme(seme);
        }
    }
    password[i+1] = '\0';
}


void RTC_Init(void) {
    //RTC_HandleTypeDef hrtc;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};


    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // Nessun PLL utilizzato

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }


    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }


    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    status = HAL_RTC_Init(&hrtc);
    /*
    if (HAL_RTC_Init(&hrtc) == HAL_ERROR) {
    	count++;
        Error_Handler();
    }else {
    	count++;
    }
    count++;
    */
}
void RTC_ReadTime(RTC_TimeTypeDef *sTime) {
    RTC_DateTypeDef sDate;

    extern RTC_HandleTypeDef hrtc;

    HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

}


void Error_Handler(void) {
	count++;
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    RTC_Init();
    HAL_Delay(1);
    unsigned long previous = 0;
    while (1) {
                unsigned long seme = genera_seme_pseudocasuale();
                unsigned long hash = hash_seme(seme);
                if(previous != seme){
                	genera_password(password, 15, hash);
                	//uint8_t encrypted_data[DATA_SIZE];
                	aes_encrypt((uint8_t*)password, encrypted_data, aes_key, iv);
                	statusLPUart = HAL_UART_Transmit(&huart2,(uint8_t*) encrypted_data, sizeof(encrypted_data), HAL_MAX_DELAY);
                	previous = seme;
                	//HAL_UART_RxCpltCallback(&hlpuart1);
                	HAL_Delay(5000);
                }
        }
}
