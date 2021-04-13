set(COMPONENT_SRCS "animations.c" "exclamation.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_PRIV_REQUIRES "esp32-smbus" "esp32-lcdmenu" "esp32-i2c-lcd1602")

register_component()