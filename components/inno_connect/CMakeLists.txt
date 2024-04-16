                    
 set(srcs "stdin_out.c"
         "addr_from_stdin.c"
         "inno_connect.c"
         "wifi_connect.c")

if(CONFIG_INNO_PROVIDE_WIFI_CONSOLE_CMD)
    list(APPEND srcs "console_cmd.c")
endif()

#if(CONFIG_EXAMPLE_CONNECT_ETHERNET)
#    list(APPEND srcs "eth_connect.c")
#endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_netif driver esp_wifi vfs)

if(CONFIG_INNO_PROVIDE_WIFI_CONSOLE_CMD)
    idf_component_optional_requires(PRIVATE console)
endif()

#if(CONFIG_EXAMPLE_CONNECT_ETHERNET)
#    idf_component_optional_requires(PRIVATE esp_eth)
#endif()
                    

