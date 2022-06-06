#include "lorawan_id_utils.h"
#include "utils.h"

#include <string.h>
#include <Arduino.h>

static void convertHexStringToBytes(const char *input, uint8_t *output, size_t size)
{
    char temp[3] = {0};

    size_t index = 0;
    for (size_t i = 0; i < size; i += 2)
    {
        temp[0] = input[i];
        temp[1] = input[i + 1];
        temp[2] = 0;
        output[index++] = strtol((const char *)temp, NULL, 16);
    }
}

void lorawanConvertDeviceEUIStr(const char *devEUI, uint8_t *output)
{
    uint8_t temp[9] = {0};
    temp[8] = '\0';
    convertHexStringToBytes(devEUI, temp, strlen(devEUI));
    for (size_t i = 0; i < 8; i++)
    {
        output[(8 - i) - 1] = temp[i];
    }
}

void lorawanConvertAppKeyStr(const char *appKey, uint8_t *output)
{
    char cleaned[33] = {0};
    cleaned[32] = '\0';
    size_t cleanedIdx = 0;
    for (size_t i = 0; i < strlen(appKey); i++)
    {
        char current = appKey[i];
        if (current != ' ')
        {
            cleaned[cleanedIdx++] = appKey[i];
        }
    }
    convertHexStringToBytes(cleaned, output, 32);
}
