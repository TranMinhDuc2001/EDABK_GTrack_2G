/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void sendCommandToMC60(const char* ATCommand);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STM32_LPUART3_RX_Pin GPIO_PIN_10
#define STM32_LPUART3_RX_GPIO_Port GPIOB
#define STM32_LPUART3_RXB11_Pin GPIO_PIN_11
#define STM32_LPUART3_RXB11_GPIO_Port GPIOB
#define P_GSM_EN_Pin GPIO_PIN_12
#define P_GSM_EN_GPIO_Port GPIOB
#define P_GNSS_EN_Pin GPIO_PIN_13
#define P_GNSS_EN_GPIO_Port GPIOB
#define P_MC60_PWRKEY_Pin GPIO_PIN_14
#define P_MC60_PWRKEY_GPIO_Port GPIOB
#define P_MC60_VDD_EXT_Pin GPIO_PIN_15
#define P_MC60_VDD_EXT_GPIO_Port GPIOB
#define STM32_UART1_TX_Pin GPIO_PIN_9
#define STM32_UART1_TX_GPIO_Port GPIOA
#define STM32_UART1_RX_Pin GPIO_PIN_10
#define STM32_UART1_RX_GPIO_Port GPIOA
#define CTRL_LED_G_Pin GPIO_PIN_11
#define CTRL_LED_G_GPIO_Port GPIOA
#define CTRL_LED_R_Pin GPIO_PIN_12
#define CTRL_LED_R_GPIO_Port GPIOA
#define STM32_I2C1_SCL_Pin GPIO_PIN_6
#define STM32_I2C1_SCL_GPIO_Port GPIOB
#define STM32_I2C1_SDA_Pin GPIO_PIN_7
#define STM32_I2C1_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
