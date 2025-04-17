#include "../include/apiRequests.h"

// Function to get the esp32 MAC address
char* getMacAddress() {
    static char mac[18];  
    uint8_t macAddr[6];
    
    esp_read_mac(macAddr, ESP_MAC_WIFI_STA);  
    
    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
             macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    
    return mac;  
}

// Function to send POST registerDevice request
void send_post_registerDevice() {
    esp_http_client_config_t config = {
        .url = WEB_SERVER_REGISTERDEVICE_URL,
        .method = HTTP_METHOD_POST
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // JSON payload
    char post_data[150];
    snprintf(post_data, sizeof(post_data), 
             "{\"deviceId\":\"%s\", \"name\":\"%s\", \"type\":\"%s\", \"location\":\"%s\"}", 
             getMacAddress(), "Motion light", "Motion sensor", "Front door");

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        char response[200]; // Buffer for response
        int content_length = esp_http_client_read(client, response, sizeof(response) - 1);
        if (content_length >= 0) {
            response[content_length] = '\0'; // Null-terminate
            ESP_LOGI(TAG, "HTTP POST Status = %d, Response = %s", status_code, response);
        } else {
            ESP_LOGI(TAG, "HTTP POST Status = %d, No Response", status_code);
        }
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}


// Function to send POST logDevice request
void send_post_logDevice() {
    esp_http_client_config_t config = {
        .url = WEB_SERVER_LOGDEVICE_URL,
        .method = HTTP_METHOD_POST
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // JSON payload
    char post_data[100];
    snprintf(post_data, sizeof(post_data), 
             "{\"event\":\"%s\", \"value\":\"%s\", \"deviceId\":\"%s\"}", 
             "Motion detected", "Lights turned on", getMacAddress());

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    ESP_LOGI("POST_DATA", "Sending: %s", post_data);


    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        char response[200]; // Buffer for response
        int content_length = esp_http_client_read(client, response, sizeof(response) - 1);
        
        if (content_length >= 0) {
            response[content_length] = '\0'; // Null-terminate
            ESP_LOGI(TAG, "HTTP POST Status = %d, Response = %s", status_code, response);
        } else {
            ESP_LOGI(TAG, "HTTP POST Status = %d, No Response", status_code);
        }
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}