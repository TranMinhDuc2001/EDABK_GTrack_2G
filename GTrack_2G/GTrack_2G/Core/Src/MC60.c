#include "MC60.h"
#include "BMA253.h"

uint8_t	rxbuffer[200];
uint8_t rxbuffer_NMEA[700];
uint8_t rxbuffer_stt[1000];

uint8_t rxbuffer_temporary[100];
char* rmc_data;
uint8_t ctrlZ = 26;

RMC rmc;

// Turn on MC60
void MC60_PowerOn(){
	HAL_GPIO_WritePin(P_GSM_EN_GPIO_Port, P_GSM_EN_Pin, 0);
	HAL_Delay(100);
	HAL_GPIO_WritePin(P_MC60_PWRKEY_GPIO_Port, P_MC60_PWRKEY_Pin, 1);
	HAL_Delay(2000);
}
void MC60_PowerOff(){
	HAL_GPIO_WritePin(P_MC60_PWRKEY_GPIO_Port, P_MC60_PWRKEY_Pin, 1);
	HAL_Delay(900);
	HAL_GPIO_WritePin(P_MC60_PWRKEY_GPIO_Port, P_MC60_PWRKEY_Pin, 0);

	// Wait for the module to log out of the network
	HAL_Delay(12000);

	HAL_GPIO_WritePin(P_GSM_EN_GPIO_Port, P_GSM_EN_Pin, 1);
}

// Send AT Command
void MC60_ATCommand_Send(UART_HandleTypeDef *huart, const char * ATCOM){
		HAL_UART_Transmit(huart, (uint8_t *)ATCOM, strlen(ATCOM), HAL_MAX_DELAY);
}

char* lat_long_data_format(RMC* rmc_latlong) {
    uint8_t size = strlen("lat:") + strlen(rmc_latlong->Lat) + strlen(",long:") + strlen(rmc_latlong->Long) + strlen("{}");
    char* result = (char*)malloc(size);
    if (result == NULL) {
        return NULL;
    }

//		char lat_data_string[strlen(rmc_latlong->Lat)];
		char* lat_data_string = (char*)malloc(strlen(rmc_latlong->Lat));
		if (strcmp(rmc_latlong->Lat, "00.000000") == 0) {
				strcpy(lat_data_string, rmc_latlong->Lat);
		} else {
		float lat_data = decodeLat(rmc_latlong->Lat);
		floatToString(lat_data, lat_data_string, NUMBER_AFTER_DECIMAL_POINT);
		}

//		char long_data_string[strlen(rmc_latlong->Long)];
		char* long_data_string = (char*)malloc(strlen(rmc_latlong->Long));
		if (strcmp(rmc_latlong->Long, "000.000000") == 0) {
				strcpy(long_data_string, rmc_latlong->Long);
		} else {
		float long_data = decodeLong(rmc_latlong->Long);
		floatToString(long_data, long_data_string, NUMBER_AFTER_DECIMAL_POINT);
		}

    //strcpy(result, "{lat:");
    strcpy(result, lat_data_string);
    strcat(result, ",");
    strcat(result, long_data_string);
		 strcat(result, ",");

//		snprintf(result, size, "{lat:%s,long:%s}", rmc_latlong->Lat, rmc_latlong->Long);
		free(lat_data_string);
		free(long_data_string);
    return result;
}

char* time_data_format(RMC* rmc_time){
	//uint8_t size = strlen("time:") + strlen(rmc_time->Time) + strlen("{}\n");
	uint8_t size = strlen(rmc_time->Time);
	char* result = (char*)malloc(size);
  if (result == NULL) {
			free(result);
			return NULL;
  }
	strcpy(result, rmc_time->Time);
	strcat(result, ",");

	return result;
}
char* date_data_format(RMC* rmc_date){
	uint8_t size = strlen(rmc_date->Date) + strlen(",");
	char* result = (char*)malloc(size);
	if(result == NULL){
		free(result);
		return NULL;
	}
	strcpy(result,rmc_date->Date);
	strcat(result,",");
	
	return result;
}


	

void MC60_Server_Pub(UART_HandleTypeDef * huart){
	sendCommandToMC60("AT+QGNSSRD=\"NMEA/RMC\"\r\n");


	if (rmc_data != NULL) {
			strcpy(rxbuffer_temporary,rmc_data);
			decodeRMC(rxbuffer_temporary, &rmc);
			free(rmc_data);
			char* date = date_data_format(&rmc);
			char* time = time_data_format(&rmc);
			char* LatLong = lat_long_data_format(&rmc);

		
			char *result = (char *)malloc(strlen(date) + strlen(time) + strlen(LatLong)  + 1);
		
			strcpy(result,date);
			strcat(result,time);
			strcat(result,LatLong);
		

			sendCommandToMC60("AT+CREG?;+CGREG?\r\n");

			if(strstr(LatLong,"00.000000,000.000000,") == NULL){
						MC60_ATCommand_Send(huart, "AT+QMTPUB=0,0,0,0,\"ductran143/feeds/device1\"\r\n");
						HAL_Delay(4000);
						HAL_GPIO_TogglePin(CTRL_LED_G_GPIO_Port,CTRL_LED_G_Pin);
						MC60_Server_Message(huart,result);
			}
			
				if (result != NULL){
						HAL_UART_Transmit(huart, (uint8_t*)result, strlen(result), HAL_MAX_DELAY);
						free(result);
						free(date);
						free(time);
						free(LatLong);
				}
	}
}

void MC60_Server_Message(UART_HandleTypeDef * huart, char* buffer){
	HAL_UART_Transmit(huart, (uint8_t*)buffer , strlen(buffer), HAL_MAX_DELAY);
	HAL_Delay(200);
	HAL_UART_Transmit_IT(huart, &ctrlZ, 2);

}

void MC60_GSM_GNSS_Status(UART_HandleTypeDef * huartz){
	HAL_UART_Transmit_IT(huartz,(uint8_t *)rxbuffer_stt,sizeof(rxbuffer_stt));
}
