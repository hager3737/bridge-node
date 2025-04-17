#ifndef API_REQUESTS_H
#define API_REQUESTS_H

#include "esp_http_client.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "../config.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "common.h"

char* getMacAddress();
void send_post_registerDevice();
void send_post_logDevice();

#endif // API_REQUESTS_H