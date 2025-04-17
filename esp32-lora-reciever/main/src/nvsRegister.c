#include <stdbool.h>
#include "../include/nvsRegister.h"

bool isDeviceRegistered() {
    nvs_handle_t my_handle;
    esp_err_t err;
    bool deviceRegistered = false;  // Default value

    // Open NVS storage
    err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err == ESP_OK) {
        // Read the deviceRegistered value
        size_t required_size = sizeof(deviceRegistered);
        err = nvs_get_blob(my_handle, "deviceRegistered", &deviceRegistered, &required_size);
        nvs_close(my_handle);
    }

    return deviceRegistered;  // Return the stored value
}

void setDeviceRegistered(bool state) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        // Write the new deviceRegistered value
        err = nvs_set_blob(my_handle, "deviceRegistered", &state, sizeof(state));
        if (err == ESP_OK) {
            nvs_commit(my_handle);
        }
        nvs_close(my_handle);
    }
}

void resetDeviceRegistered() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        bool deviceRegistered = false;  // Reset to false
        err = nvs_set_blob(my_handle, "deviceRegistered", &deviceRegistered, sizeof(deviceRegistered));
        if (err == ESP_OK) {
            nvs_commit(my_handle);  // Commit changes
        }
        nvs_close(my_handle);
    }
}