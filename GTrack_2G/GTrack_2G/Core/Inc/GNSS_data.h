#ifndef _GNSS_DATA_H_
#define _GNSS_DATA_H_
#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define TIME_LENGTH 7
#define LAT_LENGTH 10
#define LONG_LENGTH 11
#define SPEED_LENGTH 5
#define COURSE_LENGTH 7
#define DATE_LENGTH 7

typedef struct {
    char Time[TIME_LENGTH];
    char Lat[LAT_LENGTH];
    char Long[LONG_LENGTH];
    char Speed[SPEED_LENGTH];
    char Course[COURSE_LENGTH];
    char Date[DATE_LENGTH];
} RMC;

char* get_sentence_by_header(char* GNSS_data, const char* header);

void send_sentence_over_UART(char* GNSS_data, const char* header, UART_HandleTypeDef *huart);
// Convert string data to float data
float stringToFloat(char *str);
// Convert float data to string data
void floatToString(float value, char *str, int afterDecimalPoint);
// Convert NMEA sentence Lat to Decimal Degrees
float decodeLat(char* lat_data_raw);
// Convert NMEA sentence Long to Decimal Degrees
float decodeLong(char* long_data_raw);
// Decodes RMC data
void decodeRMC(char* RMCbuffer, RMC* rmc);
#endif /* SRC_GNSS_DATA_H_ */
