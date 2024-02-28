#include <nvs_cmd.h>
#include <stdio.h>
#include "console_cmd.h"
//
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
//#include "inno_connect.h"
//#include "example_common_private.h"
//#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "sensor_cmd.h"
#include "nvs_cmd.h"
#include "wifi_cmd.h"
#include "serial_inno_cmd.h"
#include "system_cmd.h"
#include "serial_inno_cmd.h"



#define PROMPT_STR CONFIG_IDF_TARGET
#define CMD_LINE_MAX_LENGTH 1024
static const char *TAG = "inno_console";


esp_err_t setup_console_cmds(console_setup_config_t *cons_conf)
{
	esp_err_t ret = ESP_OK;
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = CMD_LINE_MAX_LENGTH;

#if CONFIG_CONSOLE_STORE_HISTORY
	    initialize_filesystem();

    repl_config.history_save_path = HISTORY_PATH;
    ESP_LOGI(TAG, "Command history enabled");
#else
    ESP_LOGI(TAG, "Command history disabled");
#endif

    /* Register commands */
    esp_console_register_help_command();
    if (cons_conf->using_sensor)
    	register_sensor();
    if (cons_conf->using_system)
    	register_system();
    if (cons_conf->using_wifi)
    	register_wifi();
    if (cons_conf->using_nvs)
    	register_nvs();
    if (cons_conf->using_serialinno)
    	register_serialinno();


#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ret |= esp_console_new_repl_uart(&hw_config, &repl_config, &repl);

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ret |= esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl);

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ret |= (esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

#else
#error Unsupported console type
#endif

    ret |= (esp_console_start_repl(repl));
    return ret;
}
