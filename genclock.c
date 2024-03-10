#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_uart.h"
#include "stm32u5xx_hal_rtc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "genclock.h"
#define AES_KEY_SIZE 16
#define DATA_SIZE 256

RTC_HandleTypeDef hrtc;
UART_HandleTypeDef huart1;
HAL_StatusTypeDef status = HAL_ERROR;
HAL_StatusTypeDef statusTime1 = HAL_ERROR;
HAL_StatusTypeDef statusTime2 = HAL_ERROR;
uint8_t aes_key[AES_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};

uint8_t iv[AES_KEY_SIZE] = {0x00};
char password[256];
int count = 0;

void aes_encrypt(uint8_t *input, uint8_t *output, uint8_t *key, uint8_t *iv) {
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, input, DATA_SIZE);
    memcpy(output, input, DATA_SIZE);
}

void UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Inizializza l'oscillatore interno ad alta velocità (HSI)
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 4; // Modificare questi valori in base alle necessità specifiche del tuo hardware
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Seleziona l'orologio PLL come sorgente di clock di sistema e configura i divisori
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
    char *simboli = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    char caratteri_possibili[256]; // Buffer abbastanza grande per tutti i caratteri

    strcpy(caratteri_possibili, lettere_minuscole);
    strcat(caratteri_possibili, lettere_maiuscole);
    strcat(caratteri_possibili, numeri);
    strcat(caratteri_possibili, simboli);


    int lunghezza_set = strlen(caratteri_possibili);
    unsigned long seme;
    for (int i = 0; i < lunghezza; i++) {
        password[i] = caratteri_possibili[hash % lunghezza_set];
        if(hash != (hash / rand()%lunghezza_set)){
            hash /= rand()%lunghezza_set;
        }else{
            seme = genera_seme_pseudocasuale();
            hash = hash_seme(seme);
        }
    }
    password[lunghezza] = '\0';
}





void RTC_Init(void) {
    //RTC_HandleTypeDef hrtc;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    // Attiva gli orologi LSE come fonte di RTC
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // Nessun PLL utilizzato

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Seleziona LSE come fonte di orologio RTC
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Abilita l'accesso ai registri RTC
    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127; // Valore predivisore per ottimizzare la precisione
    hrtc.Init.SynchPrediv = 255; // Valore predivisore per ottimizzare la precisione
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

    // Questa funzione assume che hrtc sia già stato inizializzato e configurato
    extern RTC_HandleTypeDef hrtc; // Assicurati che hrtc sia accessibile

    HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // Necessario dopo la lettura del tempo per sbloccare i registri
}


void Error_Handler(void) {
    // Funzione di gestione degli errori
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    UART_Init();
    RTC_Init();
    count++;
    //uint8_t lunghezza_buf[2]; // Assumiamo che la lunghezza venga inviata come stringa ASCII di due cifre
    //char password[256]; // Massima lunghezza supportata per la sicurezza

    while (1) {
        // Ricevi la lunghezza della password dall'ESP8266
        //if(HAL_UART_Receive(&huart1, lunghezza_buf, 2, HAL_MAX_DELAY) == HAL_OK) {
            //lunghezza_buf[2] = '\0'; // Assicura che la stringa sia terminata correttamente
            //int lunghezza_pw = atoi((char*)lunghezza_buf);

            // Assicura che la lunghezza sia nel range valido
            //if(lunghezza_pw > 0 && lunghezza_pw < sizeof(password)) {
                unsigned long seme = genera_seme_pseudocasuale();
                count++;
                unsigned long hash = hash_seme(seme);
                //count++;
                genera_password(password, 120, hash);
                password;
                HAL_Delay(2000);
                //uint8_t encrypted_data[DATA_SIZE];
                //aes_encrypt((uint8_t*)password, encrypted_data, aes_key, iv);
                //HAL_UART_Transmit(&huart1, (uint8_t*)password, lunghezza_pw, HAL_MAX_DELAY);
            //}
        //}
    }
}
