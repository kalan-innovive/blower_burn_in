idf_component_register(SRCS "app_event_handler.c" "server_eh.c" "garbage_collector.c" "server_helper.c"
                    INCLUDE_DIRS "include"
                    REQUIRES json esp_event
                    PRIV_REQUIRES serial_inno 
                    )

