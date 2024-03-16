#ifndef _MC60_H_
#define _MC60_H_

#include "main.h"
#include <string.h>
#include "GNSS_data.h"
#include "stdio.h"

#define NUMBER_AFTER_DECIMAL_POINT 6

extern uint8_t rxbuffer[200];
extern uint8_t rxbufferServer_Open[300];
extern uint8_t rxbuffer_stt[1000];

extern char* rmc_data;
extern RMC rmc;

// Turn on MC60
void MC60_PowerOn();
// Turn off MC60
void MC60_PowerOff();
// Send AT Command
void MC60_ATCommand_Send(UART_HandleTypeDef *huart,const char * ATCOM);
// Public message to server
void MC60_Server_Pub(UART_HandleTypeDef * huart);
void MC60_Server_Message(UART_HandleTypeDef * huart, char* buffer);
void MC60_GSM_GNSS_Status(UART_HandleTypeDef * huartz);

#endif /* SRC_MC60_H_ */