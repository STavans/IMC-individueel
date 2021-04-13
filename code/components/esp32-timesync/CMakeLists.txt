#idf_component_register(SRCS "sntp_sync.c" "smbus.c" INCLUDE_DIRS include)

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCDIRS .)
register_component()
