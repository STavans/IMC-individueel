set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "lcd-menu-elaboration.c" "lcd-menu.c")
set(COMPONENT_PRIV_REQUIRES "esp32-smbus" "esp32-i2c-lcd1602" "esp32-radio" "esp32-sdcard-mp3" "esp32-talking-clock" "goertzel" "esp32-animations")
register_component()
