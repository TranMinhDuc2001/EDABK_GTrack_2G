#include "GNSS_data.h"

char* get_sentence_by_header(char* GNSS_data, const char* header) {
    char* token;
    char temp_data[strlen(GNSS_data) + 1];
    strcpy(temp_data, GNSS_data);

    // split string into lines
    token = strtok(temp_data, " \r\n");
    while (token != NULL) {
        if (strstr(token, header) != NULL) {
            char* temp = (char*)malloc((strlen(token) + 1) * sizeof(char)); // dynamic allocation for temp
            strcpy(temp, token);
            return temp;
        }
        token = strtok(NULL, " \r\n");
    }

    return NULL;  // return NULL if the line with the corresponding header is not found
}


void send_sentence_over_UART(char* GNSS_data, const char* header, UART_HandleTypeDef *huart) {
    char* sentence = get_sentence_by_header(GNSS_data, header);
    if (sentence != NULL) {
        HAL_UART_Transmit(huart, (uint8_t*)sentence, strlen(sentence), HAL_MAX_DELAY);
        free(sentence);
    }
}


void moveToNextComma(char* buffer, int* index) {
    while (buffer[(*index)] != ',' && buffer[(*index)] != '\0') {
        (*index)++;
    }
    if (buffer[(*index)] != '\0') (*index)++;
}

// Convert string data to float data
float stringToFloat(char *str) {
    float result = 0.0;
    float fraction = 1.0;
    uint8_t isNegative = 0;
    uint8_t inFraction = 0;

    // X? lý s? âm
    if (*str == '-') {
        isNegative = 1;
        ++str;
    }

    // Chuy?n d?i chu?i thành s?
    while (*str != '\0') {
        if (*str == '.') {
            inFraction = 1;
            ++str;
            continue;
        }

        int digit = *str - '0';
        if (digit >= 0 && digit <= 9) {
            if (inFraction) {
                fraction /= 10.0;
                result += digit * fraction;
            } else {
                result = result * 10.0 + (double)digit;
            }
        } else {
            // Ký t? không h?p l?: d?ng vi?c chuy?n d?i
            break;
        }
        ++str;
    }

    if (isNegative) {
        result = -result;
    }

    return result;
}

// Convert int data to string data
void intToStr(int num, char* str) {
    char temp[20];
    int i = 0;
    int isNegative = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (isNegative) {
        str[0] = '-';
        memcpy(str + 1, temp, i);
        // Reverse the string
        for (int j = 0; j < i / 2; j++) {
            char t = str[j + 1];
            str[j + 1] = str[i - j];
            str[i - j] = t;
        }
        str[i + 1] = '\0';
    } else {
        memcpy(str, temp, i);
        // Reverse the string
        for (int j = 0; j < i / 2; j++) {
            char t = str[j];
            str[j] = str[i - j - 1];
            str[i - j - 1] = t;
        }
        str[i] = '\0';
    }
}

// Convert float data to string data
void floatToString(float value, char *str, int afterDecimalPoint) {
    int intPart = (int)value;
    intToStr(intPart, str);
    float fractionPart = value - (float)intPart;

    while (*str != '\0') {
        str++;
    }

    if (afterDecimalPoint > 0) {
        *str++ = '.';

        while (afterDecimalPoint-- > 0) {
            fractionPart *= 10;
            int digit = (int)fractionPart;
            *str++ = '0' + digit;
            fractionPart -= digit;
        }

        *str = '\0';
    }
}

// Convert NMEA sentence Lat to Decimal Degrees
// 1234.5678 -> 12 + 34.5678 / 60;
float decodeLat(char* lat_data_raw){
	float result = 0.0;
	float lat_data = stringToFloat(lat_data_raw);
	uint16_t deg = (uint16_t)((float)lat_data / 100); // degrees
	float sec = (float)((lat_data) - deg * 100); // seconds
	result = deg + sec / 60;
	return result;
}

// Convert NMEA sentence Long to Decimal Degrees
float decodeLong(char* long_data_raw){
	float result = 0.0;
	float long_data = stringToFloat(long_data_raw);
	uint16_t deg = (uint16_t)((float)long_data / 100); // degrees
	float sec = (float)((long_data) - deg * 100); // seconds
	result = deg + sec / 60;
	return result;
}

// Decode RMC data
void decodeRMC(char* RMCbuffer, RMC* rmc) {
    int buffer_index = 0;

    // Get Time
    moveToNextComma(RMCbuffer, &buffer_index);
    memset(rmc->Time, '\0', TIME_LENGTH);
    for (int j = 0; j < TIME_LENGTH - 1; j++) {
        rmc->Time[j] = RMCbuffer[buffer_index++];
    }

    // Get Latitude
    moveToNextComma(RMCbuffer, &buffer_index);
    moveToNextComma(RMCbuffer, &buffer_index);
    memset(rmc->Lat, '\0', LAT_LENGTH);
    if (RMCbuffer[buffer_index] != ',') {
        for (int j = 0; j < LAT_LENGTH - 1 && RMCbuffer[buffer_index] != ','; j++) {
            rmc->Lat[j] = RMCbuffer[buffer_index++];
        }
    } else {
        strcpy(rmc->Lat, "00.000000");
    }

    // Get Longitude
    moveToNextComma(RMCbuffer, &buffer_index);
    moveToNextComma(RMCbuffer, &buffer_index);
    memset(rmc->Long, '\0', LONG_LENGTH);
    if (RMCbuffer[buffer_index] != ',') {
        for(int j = 0; j < LONG_LENGTH - 1 && RMCbuffer[buffer_index] != ','; j ++){
            rmc->Long[j] = RMCbuffer[buffer_index++];
        }
    } else {
        strcpy(rmc->Long, "000.000000");
    }

    // Get Speed
    moveToNextComma(RMCbuffer, &buffer_index);
    moveToNextComma(RMCbuffer, &buffer_index);
    memset(rmc->Speed, '\0', SPEED_LENGTH);
    for(int j = 0; j < SPEED_LENGTH - 1; j ++){
            if (RMCbuffer[buffer_index] != ',')
        rmc->Speed[j] = RMCbuffer[buffer_index++];
    }

    // Get Course
    moveToNextComma(RMCbuffer, &buffer_index);
    memset(rmc->Course, '\0', COURSE_LENGTH);
    for(int j = 0; j < COURSE_LENGTH - 1; j ++){
            if (RMCbuffer[buffer_index] != ',')
        rmc->Course[j] = RMCbuffer[buffer_index++];
    }

    // Get Date
    moveToNextComma(RMCbuffer, &buffer_index);
    memset(rmc->Date, '\0', DATE_LENGTH);
    for (int j = 0; j < DATE_LENGTH - 1; j++) {
        rmc->Date[j] = RMCbuffer[buffer_index++];
    }
}




