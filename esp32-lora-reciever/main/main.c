#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_intr_alloc.h>
#include <esp_log.h>
#include <sx127x.h>
#include <inttypes.h>
#include <esp_utils.h>
#include <string.h>
#include <stdbool.h>
#include "include/apiRequests.h"
#include "include/wifi.h"
#include "include/nvsRegister.h"
#include "include/common.h"

sx127x device;

void lora_rx_callbacks(sx127x *device, uint8_t *data, uint16_t data_length) {
  const char EXPECTED_MSG[] = "Hello";
  const size_t EXPECTED_LEN = 5;

  // Convert to hex for debug
  char payload_hex[514];
  const char SYMBOLS[] = "0123456789ABCDEF";

  for (size_t i = 0; i < data_length; i++) {
      uint8_t cur = data[i];
      payload_hex[2 * i] = SYMBOLS[cur >> 4];
      payload_hex[2 * i + 1] = SYMBOLS[cur & 0x0F];
  }
  payload_hex[data_length * 2] = '\0';

  int16_t rssi;
  float snr;
  int32_t freq_error;
  ESP_ERROR_CHECK(sx127x_rx_get_packet_rssi(device, &rssi));
  ESP_ERROR_CHECK(sx127x_lora_rx_get_packet_snr(device, &snr));
  ESP_ERROR_CHECK(sx127x_rx_get_frequency_error(device, &freq_error));

  ESP_LOGI(TAG, "Received HEX: %s | Length: %d", payload_hex, data_length);
  ESP_LOGI(TAG, "Received ASCII: %.*s", data_length, data);
  ESP_LOGI(TAG, "rssi: %d snr: %.2f freq_error: %" PRId32, rssi, snr, freq_error);

  // Debug each byte
  for (int i = 0; i < data_length; i++) {
      ESP_LOGI(TAG, "Byte %d: 0x%02X (%c)", i, data[i], (data[i] >= 32 && data[i] <= 126) ? data[i] : '.');
  }

  // Check if message is exactly "Hello"
  if (data_length == EXPECTED_LEN && strncmp((const char *)data, EXPECTED_MSG, EXPECTED_LEN) == 0) {
      send_post_logDevice();
  }
}


void app_main() {
  ESP_LOGI(TAG, "starting up");

  esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

  wifi_init();

  vTaskDelay(pdMS_TO_TICKS(5000));

  //resetDeviceRegistered();

  if(!isDeviceRegistered()) {
    send_post_registerDevice();
    setDeviceRegistered(true);
  }

  sx127x_reset();

  spi_device_handle_t spi_device;
  sx127x_init_spi(&spi_device);

    uint8_t version = sx127x_read_new_register(spi_device, 0x42);
    ESP_LOGI(TAG, "SX1278 Version: 0x%02X", version);

  ESP_ERROR_CHECK(sx127x_create(spi_device, &device));
  ESP_ERROR_CHECK(sx127x_set_opmod(SX127x_MODE_SLEEP, SX127x_MODULATION_LORA, &device));
  ESP_ERROR_CHECK(sx127x_set_frequency(437200012, &device));
  ESP_ERROR_CHECK(sx127x_lora_reset_fifo(&device));
  ESP_ERROR_CHECK(sx127x_rx_set_lna_boost_hf(true, &device));
  ESP_ERROR_CHECK(sx127x_set_opmod(SX127x_MODE_STANDBY, SX127x_MODULATION_LORA, &device));
  ESP_ERROR_CHECK(sx127x_rx_set_lna_gain(SX127x_LNA_GAIN_G4, &device));
  ESP_ERROR_CHECK(sx127x_lora_set_bandwidth(SX127x_BW_125000, &device));
  ESP_ERROR_CHECK(sx127x_lora_set_implicit_header(NULL, &device));
  ESP_ERROR_CHECK(sx127x_lora_set_modem_config_2(SX127x_SF_9, &device));
  ESP_ERROR_CHECK(sx127x_lora_set_syncword(18, &device));
  ESP_ERROR_CHECK(sx127x_set_preamble_length(8, &device));
  sx127x_rx_set_callback(lora_rx_callbacks, &device);
  sx127x_lora_cad_set_callback(cad_callback, &device);

  ESP_ERROR_CHECK(setup_task(&device));

  gpio_install_isr_service(0);
  setup_gpio_interrupts((gpio_num_t)DIO0, &device, GPIO_INTR_POSEDGE);

  ESP_ERROR_CHECK(sx127x_set_opmod(SX127x_MODE_RX_CONT, SX127x_MODULATION_LORA, &device));
}