#ifndef ESP32_LORAWAN_DEVICE_ID
#define ESP32_LORAWAN_DEVICE_ID

#include <stdint.h>
#include <stdlib.h>

void lorawanConvertDeviceEUIStr(const char *devEUI, uint8_t *output);
void lorawanConvertAppKeyStr(const char *appKey, uint8_t *output);

#endif