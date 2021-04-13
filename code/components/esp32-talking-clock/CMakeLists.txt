#idf_component_register(SRCS "smbus.c" INCLUDE_DIRS include)

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "talking_clock.c")
set(COMPONENT_PRIV_REQUIRES "esp32-sdcard-mp3")

register_component()
