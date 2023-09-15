idf_component_register(SRCS "serial_inno.c" "serial_inno_test.c" "msg16.c" "rack_device.c"
                    INCLUDE_DIRS "include"
                    REQUIRES json esp_event             
                    PRIV_REQUIRES driver)

