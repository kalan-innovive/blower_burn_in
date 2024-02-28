                    
 set(srcs "stdin_out.c"
         "addr_from_stdin.c"
         "inno_connect.c"
         "wifi_connect.c")



idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_netif driver esp_wifi vfs)


                    

