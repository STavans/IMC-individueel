set(COMPONENT_SRCS "goertzel_filter.c" "goertzel.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_PRIV_REQUIRES "esp32-smbus" "nvs_flash" "audio_pipeline" "audio_stream" "esp_peripherals" "esp32-lcdmenu" "esp32-i2c-lcd1602")

register_component()