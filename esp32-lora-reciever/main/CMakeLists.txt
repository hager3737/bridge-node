idf_component_register(SRCS   "main.c"
                              "../components/sx127x/examples/common/esp_utils/esp_utils.c"
                              "src/apiRequests.c"
                              "src/wifi.c"
                              "src/nvsRegister.c"
                              "src/common.c"
                       INCLUDE_DIRS 
                                   "."
                                   "include"
                       PRIV_REQUIRES 
                                   "sx127x" 
                                   "nvs_flash"
                                   "esp_wifi"
                                   "esp_http_client"
                       PRIV_INCLUDE_DIRS "../components/sx127x/examples/common/esp_utils")
