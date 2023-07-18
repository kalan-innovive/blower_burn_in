// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: InnoRack

#include "ui.h"
#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "ui_blower_burn_in.h"
#define UI_BURN 0
#if UI_BURN
#define BURN_IN_TIME 10 *60
#define BURN_IN_COOLDOWN_TIME 30 *60
#else
#define BURN_IN_TIME 10 *1
#define BURN_IN_COOLDOWN_TIME  1*20
#endif
const char *tag = "UI_EVENT";
static const char *tag_timer = "UI_Timer";
//static const char *tag_det = "UI_Detail";
const char *notify_burnin_on =
		"#0000ff Calibration Cycle Complete#\n Press \"OK\"\n Record values then\nTurn Power Supply Off";
const char *notify_burnin_off = "Off Cycle Complete\n Turn Power Supply On";
const char *notify_burnin_ready =
		"#0000ff Burn In Ready#\n Turn Power Supply On";

void clock_run_cb(lv_timer_t *timer) {
	t_gui_timer *gt = timer->user_data;
	lv_obj_t *lab_time = gt->lab_time;
	gt->time--;
	ESP_LOGD(tag_timer, "%s, Timer: %d\n", __FUNCTION__, gt->time);
	if (gt->time <= 0) {
		lv_timer_pause(timer);

		//
		esp_err_t ret = ui_timer_finished();
		// only pause timer after successfully changing the state
		if (ret == ESP_OK) {
			lv_timer_pause(timer);
			ESP_LOGI(tag_timer, "Pausing Timer: %d\n", gt->time);
			lv_obj_clear_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);

			lv_obj_add_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

		} else {
			// Could not update the state let loop go around again
			ESP_LOGW(tag_timer, "Pausing Timer: %d\n", gt->time);
		}
		gt->time = 0;
	}
	int time = gt->time;
	int tm_time = time / 60;
	int ts_time = time % 60;
	lv_label_set_text_fmt(lab_time, "%02d:%02d", tm_time, ts_time);

	lv_label_set_text_fmt(ui_TimerNotificationLabel, "%02d:%02d", tm_time,
			ts_time);
}

void burn_in_test_start(lv_timer_t *timer) {
	ESP_LOGI(tag, "Start Pressed");
//	update_test_state(RUNNING_BURNIN_TEST);
	// Check if the rack is initialised
	burn_in_testing_state_t state = get_burn_in_state();

	if (state == RUNNING_BURNIN_TEST) {
//		update_test_state(RUNNING_BURNIN_TEST);
		update_timer_counter(timer, BURN_IN_TIME);
		lv_obj_add_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(ui_NotificationLabel, notify_burnin_on);
		lv_obj_clear_flag(ui_OKButton, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

	} else {
		update_timer_counter(timer, 0);
		lv_obj_clear_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(ui_NotificationLabel, notify_burnin_ready);

		lv_obj_add_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

		ESP_LOGW(tag, "Start pressed timer not reset current state: %d\n",
				state);

	}

}

void start_pressed(void) {
	update_test_state(STARTING_BURNIN_TEST);
	lv_obj_add_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void burn_in_cooldown_start(lv_timer_t *timer) {
//	t_gui_timer *gt = timer->user_data;
	ESP_LOGI(tag, "Cool Down Start Timer Event");
	update_timer_counter(timer, BURN_IN_COOLDOWN_TIME);
	lv_obj_add_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(ui_NotificationLabel, notify_burnin_off);
	lv_obj_add_flag(ui_OKButton, LV_OBJ_FLAG_HIDDEN);

	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void burn_in_cancel(lv_timer_t *timer) {
	t_gui_timer *gt = timer->user_data;
	gt->time = 0;
	lv_timer_pause(timer);
	ESP_LOGI(tag, "Cancel Event");
	lv_obj_clear_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(ui_NotificationLabel, notify_burnin_ready);

	lv_obj_add_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

	update_test_state(CANCEL_BURNIN_TEST);

}

void update_timer_counter(lv_timer_t *timer, int t) {
	ESP_LOGI(tag, "update the timer %d\n", (int ) t);
	t_gui_timer *gt = timer->user_data;
	gt->time = t;
	lv_timer_resume(timer);
	lv_obj_add_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);

	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void set_sa_pressed(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
	if (update_detail_values(SA_INDEX)) {
//		lv_chart_refresh(chart); /*Required after direct set*/
	}

	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void set_sb_pressed(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
	update_detail_values(SB_INDEX);
//	lv_chart_refresh(chart); /*Required after direct set*/

	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void set_ea_pressed(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
	update_detail_values(EA_INDEX);
//	lv_chart_refresh(chart); /*Required after direct set*/

	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void set_eb_pressed(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
	update_detail_values(EB_INDEX);
//	lv_chart_refresh(chart); /*Required after direct set*/

	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

}

void stop_data_requests(lv_event_t *e) {
// Your code here
}

void update_time_counter(lv_event_t *e) {

//	timer = lv_timer_create(timer_count_cb, 1000,  &burn_in_time);
}

//#if LV_USE_CHART && LV_BUILD_EXAMPLES

void lv_chart_cb(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
//
//	lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
//
//	/*Add three data series*/
//	lv_chart_series_t *ser1 = lv_chart_add_series(chart,
//			lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
//	lv_chart_series_t *ser2 = lv_chart_add_series(chart,
//			lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);
//
//	/*Set the next points on 'ser1'*/
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 10);
//	lv_chart_set_next_value(chart, ser1, 30);
//	lv_chart_set_next_value(chart, ser1, 70);
//	lv_chart_set_next_value(chart, ser1, 90);
//
//	/*Directly set points on 'ser2'*/
//	ser2->y_points[0] = 90;
//	ser2->y_points[1] = 70;
//	ser2->y_points[2] = 65;
//	ser2->y_points[3] = 65;
//	ser2->y_points[4] = 65;
//	ser2->y_points[5] = 65;
//	ser2->y_points[6] = 65;
//	ser2->y_points[7] = 65;
//	ser2->y_points[8] = 65;
//	ser2->y_points[9] = 65;

	lv_chart_refresh(chart); /*Required after direct set*/
}

//#endif

