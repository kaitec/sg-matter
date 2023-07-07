set(PRIV_REQUIRES_LIST driver esp_matter esp_matter_console)

idf_component_register(SRCS "enocean.c" "motor.c" "flash.c" "main.c" "matter.cpp" "hardware.c"
                        #SRC_DIRS           "."
                        PRIV_INCLUDE_DIRS  "."
                        PRIV_REQUIRES      ${PRIV_REQUIRES_LIST})
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 14)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")