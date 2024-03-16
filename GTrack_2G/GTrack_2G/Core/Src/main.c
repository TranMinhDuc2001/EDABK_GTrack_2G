//* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "main.h"
#include "MC60.h"
#include "GNSS_data.h"
#include "BMA253.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
#define RX_BUFFER_SIZE 2048
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index  = 0;
volatile uint8_t rx_buffer_temp[RX_BUFFER_SIZE];
volatile uint8_t rx_index_temp = 0;
volatile uint8_t receive_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void UART_Rx_Process(char* response, const char* expectedResponse, const char* ATCommand);
void AGPS_On();
void UART_RxCallback_Process();
char* expectedResponse(const char* ATCommand);
void sendCommandToMC60(const char* ATCommand);
void HandlePublishError();
uint8_t VerifyResponseContent(char* response, const char* expectedResponse);
void MQTT_Process();


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint32_t start_time;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_3){
			HAL_ResumeTick();
			start_time = HAL_GetTick();
	}
}


void UART_RxCallback_Process(){
	if (rx_buffer[rx_index] == '\n') {
            if (rx_index > 0 && rx_buffer[rx_index - 1] == '\r') {
                rx_buffer[rx_index - 1] = '\n';
								rx_buffer[rx_index] = '\0';
            }	else {
                rx_buffer[rx_index] = '\0';
            }
						memcpy(rx_buffer_temp + rx_index_temp, rx_buffer, strlen((const char *)rx_buffer));
						rx_index_temp += strlen((const char *)rx_buffer);
						rx_index = 0;	// Nhan du lieu moi
            receive_flag = 1;
        } else {
            rx_index++;
						if (rx_index >= RX_BUFFER_SIZE - 1) {
						rx_index = 0; // Tránh tràn buffer
						}
        }
        HAL_UART_Receive_IT(&huart3, &rx_buffer[rx_index], 1); // San sang nhan ky tu tiep theo
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
        UART_RxCallback_Process();
  }
}
char* expectedResponse(const char* ATCommand){
		char* expected_response = (char*)malloc(20);
		if (strstr(ATCommand, "AT+CPIN?")){
				strcpy(expected_response, "+CPIN: READY");
		}
		else if (strstr(ATCommand, "AT+CREG?;+CGREG?")){
				strcpy(expected_response, "+CREG: 0,1\n\n+CGREG: 0,1");
		}
		else if (strstr(ATCommand, "AT+CREG?")){
				strcpy(expected_response, "+CREG: 0,2");
		}
		else if (strstr(ATCommand, "AT+QIFGCNT=2") || strstr(ATCommand, "AT+QICSGP=1") || strstr(ATCommand, "AT+QGNSSEPO=1") || strstr(ATCommand, "AT+QGNSSC=1")){
				strcpy(expected_response, "OK");
		}
		else if (strstr(ATCommand, "AT+QMTOPEN=")){
				strcpy(expected_response, "+QMTOPEN: 0,0");
		}
		else if (strstr(ATCommand, "AT+QMTCONN=")){
				strcpy(expected_response, "+QMTCONN: 0,0,0");
		}
		else if (strstr(ATCommand, "AT+QMTPUB=")){
				strcpy(expected_response, "+QMTPUB: 0,0,0");
		}
		else if (strstr(ATCommand, "AT+QMTDISC=0")){
				strcpy(expected_response, "+QMTDISC: 0,0");
		}
		else strcpy(expected_response, "OK");
		return expected_response;
}

void sendCommandToMC60(const char* ATCommand) {
		if(strstr(ATCommand, "AT+QMTPUB=")){
				MC60_Server_Pub(&huart3);
		}
		else {
    HAL_UART_Transmit(&huart3, (uint8_t*)ATCommand, strlen(ATCommand), HAL_MAX_DELAY); // G?i l?nh qua UART3
		}
		HAL_Delay(1000);
		char* expected_response = expectedResponse(ATCommand);
		UART_Rx_Process(rx_buffer_temp, expected_response, ATCommand);
		free(expected_response);
}

void HandlePublishError(){
		sendCommandToMC60("AT+QMTOPEN=0,\"io.adafruit.com\",1883\r\n");
		sendCommandToMC60("AT+QMTCONN=0,\"Duc\",\"ductran143\",\"aio_fXMj751Ph1Q36BzoARbP9e1xztJQ\"\r\n");
		//MC60_Server_Pub(&huart3);
}

uint8_t VerifyResponseContent(char* response, const char* expectedResponse){
		if (strstr(response,expectedResponse) != NULL) return 1;
		else return 0;
}
int count = 0;
void UART_Rx_Process(char* response, const char* expectedResponse, const char* ATCommand) {


    if (receive_flag) {
        HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen((const char *)response), HAL_MAX_DELAY);
				if(strstr(ATCommand, "AT+QGNSSRD")){
						rmc_data = get_sentence_by_header(response,"GNRMC");
				}
				uint8_t isTrue = VerifyResponseContent(response, expectedResponse);
        memset(response, '\0', RX_BUFFER_SIZE); // Lam sach buffer sau khi dã x? lý
        receive_flag = 0; // Reset flag
				rx_index_temp = 0;
				if (!isTrue) {
						if (strstr(ATCommand,"AT+QMTPUB=")){
								HandlePublishError();
								isTrue = 0;
						} 
						else if(strstr(ATCommand,"AT+CREG?;+CGREG?")){
									//sendCommandToMC60("AT+QICSGP=1,\"m-wap\",\"mms\",\"mms\"\r\n");
									//isTrue = 0;
									count++;
									if(count == 3) {
										count = 0;
										NVIC_SystemReset();
									}
						}
							
						else {
								sendCommandToMC60(ATCommand);
								isTrue = 0;
						}
				}
    }

}
// MQTT process
uint8_t tmp = 1;
void MQTT_Process(){
		if(tmp){
			//sendCommandToMC60("AT+QMTDISC=0\r\n");
			sendCommandToMC60("AT+QMTOPEN=0,\"io.adafruit.com\",1883\r\n");
			sendCommandToMC60("AT+QMTCONN=0,\"Duc\",\"ductran143\",\"aio_yvzd63qSCyn36SvE88pLXkBVWt4c\"\r\n");
			tmp = 0;
		}	
	
		MC60_Server_Pub(&huart3);
		//MC60_GSM_GNSS_Status(&huart1);


}
// Activate AGPS
void AGPS_On(){
		sendCommandToMC60("AT+QIFGCNT=2\r\n");
		sendCommandToMC60("AT+QICSGP=1,\"m-wap\",\"mms\",\"mms\"\r\n");
		HAL_Delay(5000);
		sendCommandToMC60("AT+CREG?;+CGREG?\r\n");
		sendCommandToMC60("AT+QGNSSTS?\r\n");
		sendCommandToMC60("AT+QGREFLOC=21.04196,105.786865\r\n");
		sendCommandToMC60("AT+QGNSSEPO=1\r\n");
		HAL_Delay(20000);
		sendCommandToMC60("AT+QGNSSC=1\r\n");
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
}
/* USER CODE END 0 */

void Init_System(){
	
	HAL_UART_Receive_IT(&huart3, rx_buffer, 1); // Start receiving data from MC60
	
	MC60_PowerOn();
	AGPS_On();
	configureNoMotionInterrupt(&hi2c1);
	
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_GPIO_WritePin(CTRL_LED_G_GPIO_Port,CTRL_LED_G_Pin,1);
	HAL_GPIO_WritePin(CTRL_LED_R_GPIO_Port,CTRL_LED_R_Pin,1);
	Init_System();

  /* USER CODE END 2 */
	int temp = 0;
	while (1)
  {
    /* USER CODE END WHILE */
			//sendCommandToMC60("AT+CBC\r\n");
			//HAL_GPIO_TogglePin(CTRL_LED_G_GPIO_Port,CTRL_LED_G_Pin);
			//HAL_Delay(200);

			uint32_t current_time = HAL_GetTick();
			if(current_time - start_time > 900000){
				HAL_UART_Transmit(&huart1,(uint8_t*)("isStop\n"), strlen("isStop\n"),HAL_MAX_DELAY);
				HAL_Delay(200);
				sendCommandToMC60("AT+QMTDISC=0\r\n");
				tmp = 1;
				HAL_SuspendTick();
				HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
			}
			else{
				HAL_UART_Transmit(&huart1,(uint8_t*)("isRunning\n"),strlen("isRunning\n"), HAL_MAX_DELAY);
				HAL_Delay(1000);
				MQTT_Process();
			}
		}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, P_GSM_EN_Pin|P_GNSS_EN_Pin|P_MC60_PWRKEY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CTRL_LED_G_Pin|CTRL_LED_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : P_GSM_EN_Pin P_GNSS_EN_Pin P_MC60_PWRKEY_Pin */
  GPIO_InitStruct.Pin = P_GSM_EN_Pin|P_GNSS_EN_Pin|P_MC60_PWRKEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : P_MC60_VDD_EXT_Pin */
  GPIO_InitStruct.Pin = P_MC60_VDD_EXT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(P_MC60_VDD_EXT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CTRL_LED_G_Pin CTRL_LED_R_Pin */
  GPIO_InitStruct.Pin = CTRL_LED_G_Pin|CTRL_LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
