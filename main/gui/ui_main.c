/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
//
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "bsp/esp-box.h"
#include "lvgl.h"
#include "lv_symbol_extra_def.h"
//#include "bsp_btn.h"
//#include "app_wifi.h"
//#include "app_rmaker.h"
#include "settings.h"
#include "ui_main.h"
#include "ui.h"
#include "ui_blower_burn_in.h"

//#include "ui_sr.h"
//#include "ui_mute.h"
//#include "ui_hint.h"
//#include "ui_player.h"
//#include "ui_device_ctrl.h"
//#include "ui_about_us.h"
//#include "ui_net_config.h"
#include "ui_boot_animate.h"

static const char *TAG = "ui_main";

static TaskHandle_t g_lvgl_task_handle;
static int g_item_index = 0;
//static lv_group_t *g_btn_op_group = NULL;
//static lv_indev_t *g_button_indev = NULL;
static button_style_t g_btn_styles;
static lv_obj_t *g_page_menu = NULL;

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t g_guisemaphore;
//static lv_obj_t *g_lab_wifi = NULL;
//static lv_obj_t *g_lab_cloud = NULL;
//static lv_obj_t *g_status_bar = NULL;
//
static void ui_main_menu(int32_t index_id);
////static void ui_led_set_visible(bool visible);
//
typedef enum _screen_id {
	MAIN_SCREEN = 0, APP_SCREEN, BLOWER_SCREEN, SETTING_SCREEN
} screen_id;
///**
// * Get active screen
// * @ret scr pointer to a screen
// */
////lv_obj_t* lv_disp_get_scr_index(screen_id scr_id) {
////	lv_disp_get_scr_act()
////	switch (scr_id) {
////	case:
////	return
////}
////
////lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
////}
//static int blower_list[4] = { 0, 0, 0, 0 };
//
//static check_blowers() {
//	blower_list[0] = 1;
//	blower_list[1] = 1;
//}
//
//static current_screen = MAIN_SCREEN;
//#define BLOWER_LEN 4
//
//
//

//}
//
//static update_app_screen() {
//	set_blower_values();
//}
//static void lvgl_gui_update(void *pvParam) {
//	(void) pvParam;
//	g_guisemaphore = xSemaphoreCreateMutex();
//
//	do {
//		/* Try to take the semaphore, call lvgl related function on success */
//		if (pdTRUE == xSemaphoreTake(g_guisemaphore, portMAX_DELAY)) {
//			switch (current_screen) {
//			case MAIN_SCREEN:
//				check_blowers();
//				break;
//			case APP_SCREEN:
//				update_app_screen();
//				break;
//			}
//			xSemaphoreGive(g_guisemaphore);
//		}
//		vTaskDelay(pdMS_TO_TICKS(10));
//
//	} while (true);
//
//	vTaskDelete(NULL);
//}

void ui_acquire(void) {
	bsp_display_lock(0);
//	TaskHandle_t task = xTaskGetCurrentTaskHandle();
//	if (g_lvgl_task_handle != task) {
//		xSemaphoreTake(g_guisemaphore, portMAX_DELAY);
//	}
}

void ui_release(void) {
	bsp_display_unlock();
//	TaskHandle_t task = xTaskGetCurrentTaskHandle();
//	if (g_lvgl_task_handle != task) {
//		xSemaphoreGive(g_guisemaphore);
//	}
}

//static void ui_status_bar_set_visible(bool visible) {
//	if (visible) {
//// update all state
//		ui_main_status_bar_set_wifi(app_wifi_is_connected());
//		ui_main_status_bar_set_cloud(app_rmaker_is_connected());
//		lv_obj_clear_flag(g_status_bar, LV_OBJ_FLAG_HIDDEN);
//	} else {
//		lv_obj_add_flag(g_status_bar, LV_OBJ_FLAG_HIDDEN);
//	}
//}
//
//lv_obj_t* ui_main_get_status_bar(void) {
//	return g_status_bar;
//}
//
//void ui_main_status_bar_set_wifi(bool is_connected) {
//	if (g_lab_wifi) {
//		lv_label_set_text_static(g_lab_wifi,
//				is_connected ? LV_SYMBOL_WIFI : LV_SYMBOL_EXTRA_WIFI_OFF);
//	}
//}
//
//void ui_main_status_bar_set_cloud(bool is_connected) {
//	if (g_lab_cloud) {
//		lv_label_set_text_static(g_lab_cloud,
//				is_connected ? LV_SYMBOL_EXTRA_CLOUD_CHECK : "  ");
//	}
//}
//
//static void hint_end_cb(void) {
//	ESP_LOGI(TAG, "hint end");
//	ui_main_menu(g_item_index);
//}
//
//static void player_end_cb(void) {
//	ESP_LOGI(TAG, "player end");
//	ui_main_menu(g_item_index);
//}
//
//static void dev_ctrl_end_cb(void) {
//	ESP_LOGI(TAG, "dev_ctrl end");
//	ui_main_menu(g_item_index);
//}
//
//static void about_us_end_cb(void) {
//	ESP_LOGI(TAG, "about_us end");
//	ui_main_menu(g_item_index);
//}
//
//static void net_end_cb(void) {
//	ESP_LOGI(TAG, "net end");
//	ui_main_menu(g_item_index);
//}
//
//static void ui_help(void) {
//	ui_hint_start(hint_end_cb);
//}
//
//typedef struct {
//	char *name;
//	void *img_src;
//} item_desc_t;
//
//typedef struct {
//	char *name;
//	void *img_src;
//} blower_desc_t;
//
//static item_desc_t item[] = { { .name = "Device Control", .img_src =
//		(void*) &icon_dev_ctrl }, { .name = "Network", .img_src =
//		(void*) &icon_network }, { .name = "Media Player", .img_src =
//		(void*) &icon_media_player }, { .name = "Help", .img_src =
//		(void*) &icon_help }, { .name = "About Us", .img_src =
//		(void*) &icon_about_us }, };
//
//static lv_obj_t *g_img_item = NULL;
//static lv_obj_t *g_lab_item = NULL;
//static lv_obj_t *g_led_item[5];
//static size_t g_item_size = sizeof(item) / sizeof(item[0]);
//
//static void menu_prev_cb(lv_event_t *e) {
//	lv_led_off(g_led_item[g_item_index]);
//	if (0 == g_item_index) {
//		g_item_index = g_item_size;
//	}
//	g_item_index--;
//	lv_led_on(g_led_item[g_item_index]);
//	lv_img_set_src(g_img_item, item[g_item_index].img_src);
//	lv_label_set_text_static(g_lab_item, item[g_item_index].name);
//}
//
//static void menu_next_cb(lv_event_t *e) {
//	lv_led_off(g_led_item[g_item_index]);
//	g_item_index++;
//	if (g_item_index >= g_item_size) {
//		g_item_index = 0;
//	}
//	lv_led_on(g_led_item[g_item_index]);
//	lv_img_set_src(g_img_item, item[g_item_index].img_src);
//	lv_label_set_text_static(g_lab_item, item[g_item_index].name);
//}
//
//static void menu_enter_cb(lv_event_t *e) {
//	ESP_LOGI(TAG, "menu item index=%d", g_item_index);
//	lv_obj_t *obj = lv_event_get_user_data(e);
//	if (ui_get_btn_op_group()) {
//		lv_group_remove_all_objs(ui_get_btn_op_group());
//	}
//	ui_btn_rm_all_cb();
//	ui_led_set_visible(false);
//	lv_obj_del(obj);
//
//	switch (g_item_index) {
//	case 0:
//		ui_status_bar_set_visible(true);
//		ui_device_ctrl_start(dev_ctrl_end_cb);
//		break;
//	case 1:
//		ui_status_bar_set_visible(true);
//		ui_net_config_start(net_end_cb);
//		break;
//	case 2:
//		ui_status_bar_set_visible(true);
//		ui_media_player(player_end_cb);
//		break;
//	case 3:
//		ui_status_bar_set_visible(false);
//		ui_help();
//		break;
//	case 4:
//		ui_status_bar_set_visible(true);
//		ui_about_us_start(about_us_end_cb);
//		break;
//	default:
//		break;
//	}
//}


//static void clock_run_cb(lv_timer_t *timer) {
//	lv_obj_t *lab_time = (lv_obj_t*) timer->user_data;
//	time_t now;
//	struct tm timeinfo;
//	time(&now);
//	localtime_r(&now, &timeinfo);
//	lv_label_set_text_fmt(lab_time, "%02u:%02u", timeinfo.tm_hour,
//			timeinfo.tm_min);
//}
//
//esp_err_t ui_blower_app_start(void) {
////	const board_res_desc_t *brd = bsp_board_get_description();
//	BaseType_t ret_val = xTaskCreatePinnedToCore(lvgl_task, "lvgl_Task",
//			6 * 1024, NULL, configMAX_PRIORITIES - 3, &g_lvgl_task_handle, 0);
//	ESP_ERROR_CHECK((pdPASS == ret_val) ? ESP_OK : ESP_FAIL);
//
//	ui_acquire();
//	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(237, 238, 239),
//			LV_STATE_DEFAULT);
//	ui_button_style_init();
//
//	lv_indev_t *indev = lv_indev_get_next(NULL);
//
//	if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
//		ESP_LOGI(TAG, "Input device type is keypad");
//		g_btn_op_group = lv_group_create();
//		lv_indev_set_group(indev, g_btn_op_group);
//	} else if (lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
//		ESP_LOGI(TAG, "Input device type have button");
//		g_button_indev = indev;
//	}
//
//// Create status bar
//	g_status_bar = lv_obj_create(lv_scr_act());
//	lv_obj_set_size(g_status_bar,
//			lv_obj_get_width(lv_obj_get_parent(g_status_bar)), 36);
//	lv_obj_clear_flag(g_status_bar, LV_OBJ_FLAG_SCROLLABLE);
//	lv_obj_set_style_radius(g_status_bar, 0, LV_STATE_DEFAULT);
//	lv_obj_set_style_bg_color(g_status_bar,
//			lv_obj_get_style_bg_color(lv_scr_act(), LV_STATE_DEFAULT),
//			LV_PART_MAIN);
//	lv_obj_set_style_border_width(g_status_bar, 0, LV_PART_MAIN);
//	lv_obj_set_style_shadow_width(g_status_bar, 0, LV_PART_MAIN);
//	lv_obj_align(g_status_bar, LV_ALIGN_TOP_MID, 0, 0);
//
//	lv_obj_t *lab_time = lv_label_create(g_status_bar);
//	lv_label_set_text_static(lab_time, "23:59");
//	lv_obj_align(lab_time, LV_ALIGN_LEFT_MID, 0, 0);
//	lv_timer_t *timer = lv_timer_create(clock_run_cb, 1000, (void*) lab_time);
//	clock_run_cb(timer);
//
//	g_lab_wifi = lv_label_create(g_status_bar);
//	lv_obj_align_to(g_lab_wifi, lab_time, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
//
//	g_lab_cloud = lv_label_create(g_status_bar);
//	lv_obj_set_style_text_font(g_lab_cloud, &font_icon_16, LV_PART_MAIN);
//	lv_obj_align_to(g_lab_cloud, g_lab_wifi, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
//
//	ui_status_bar_set_visible(0);
//
//	/* For speech animation */
//	ui_sr_anim_init();
//
//	boot_animate_start(ui_after_boot);
//	if (GPIO_NUM_NC != brd->GPIO_MUTE_NUM) {
//		ui_mute_init();
//	}
//	ui_release();
//	return ESP_OK;
//}
//
//void ui_btn_rm_all_cb(void) {
//	const board_res_desc_t *brd = bsp_board_get_description();
//	for (size_t i = 0; i < brd->BUTTON_TAB_LEN; i++) {
//		if (BOARD_BTN_ID_NEXT == brd->BUTTON_TAB[i].id) {
//			bsp_btn_rm_all_callback(BOARD_BTN_ID_NEXT);
//		} else if (BOARD_BTN_ID_PREV == brd->BUTTON_TAB[i].id) {
//			bsp_btn_rm_all_callback(BOARD_BTN_ID_PREV);
//		} else if (BOARD_BTN_ID_ENTER == brd->BUTTON_TAB[i].id) {
//			bsp_btn_rm_all_callback(BOARD_BTN_ID_ENTER);
//		}
//	}
//}

static void ui_after_boot(void)
{
    sys_param_t *param = settings_get_parameter();
    // Fill in the parametesrs in the header
    // - IP address of the event handler
    // - Node name
    // Set the wifi symbol if connected
//	lv_disp_load_scr(ui_Screen1);

}

esp_err_t ui_main_start(void)
{
    ui_acquire();


    lv_indev_t *indev = lv_indev_get_next(NULL);

//    if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
//        ESP_LOGI(TAG, "Input device type is keypad");
//        g_btn_op_group = lv_group_create();
//        lv_indev_set_group(indev, g_btn_op_group);
//    } else if (lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
//        ESP_LOGI(TAG, "Input device type have button");
//    } else if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
//        ESP_LOGI(TAG, "Input device type have pointer");
//    }
    ui_init();

//    boot_animate_start(ui_after_boot);
//    ui_after_boot();

    ui_release();
    init_test_vals();

    return ESP_OK;
}

