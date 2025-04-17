#ifndef NVS_REGISTER
#define NVS_REGISTER

#include "nvs_flash.h"
#include "nvs.h"

bool isDeviceRegistered();
void setDeviceRegistered(bool state);
void resetDeviceRegistered();

#endif // NVS_REGISTER