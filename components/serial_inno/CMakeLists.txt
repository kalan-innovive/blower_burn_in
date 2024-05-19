idf_component_register(SRCS "serial_inno.c" "msg16.c" "rack_device.c" "serial_inno_api.c" 
                    INCLUDE_DIRS "include"
                    REQUIRES json esp_event             
                    PRIV_REQUIRES driver inno_register)

