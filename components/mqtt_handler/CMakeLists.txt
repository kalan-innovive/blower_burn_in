# (Not part of the boilerplate)
# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
#set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

idf_component_register(SRCS "mqtt_handler.c" "test_mqtt_handler.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mqtt esp_wifi esp_event nvs_flash app_event_handler unity
                    )

