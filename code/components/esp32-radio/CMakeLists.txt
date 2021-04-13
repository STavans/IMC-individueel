set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "radioController.c")
set(COMPONENT_PRIV_REQUIRES "esp32-smbus" "esp32-i2c-lcd1602" "esp32-radio" "nvs_flash" "audio_pipeline" "audio_stream" "esp_peripherals")
register_component()
