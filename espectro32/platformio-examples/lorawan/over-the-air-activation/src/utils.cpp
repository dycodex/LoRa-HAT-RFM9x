#include "utils.h"
#include <Arduino.h>

void printHexArray(uint8_t *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        Serial.printf("%02x", data[i]);
        Serial.print(" ");
    }

    Serial.println();
}