 set(srcs "wifi_cmd.c"
         "console_cmd.c"
         "serial_inno_cmd.c"
         "sensor_cmd.c"
         "system_cmd.c"
         "nvs_cmd.c"
         )
         
         
idf_component_register(
			SRCS "${srcs}"
			INCLUDE_DIRS "include"
			REQUIRES  console spi_flash driver nvs_flash pressure_sensor json serial_inno
			PRIV_REQUIRES esp_netif esp_wifi vfs inno_connect fatfs 
			)

