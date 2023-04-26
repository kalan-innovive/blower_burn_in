// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: InnoRack

#include "ui.h"
#include <stdio.h>
#include "esp_log.h"
#include "ui_blower_burn_in.h"

#define BURN_IN_TIME 10 *60

void clock_run_cb(lv_timer_t *timer) {
	t_gui_timer *gt = timer->user_data;
	lv_obj_t *lab_time = gt->lab_time;
	int time = gt->time--;
	ESP_LOGI("clock CB", "Timer: %d\n", time);
	if (time == 0) {
		lv_timer_pause(timer);
	}
	int tm_time = time / 60;
	int ts_time = time % 60;
	lv_label_set_text_fmt(lab_time, "%02d:%02d", tm_time, ts_time);
}

void update_timer_counter(lv_event_t *e) {
//	ESP_LOGI("LabelCB", "Event Called: %d\n", (int ) e->code);
	lv_timer_t *timer = (lv_timer_t*) e->user_data;
	lv_timer_set_repeat_count(timer, -1);
	t_gui_timer *gt = timer->user_data;
	lv_obj_t *ui_Timer_Label = gt->lab_time;
	lv_label_set_text_static(ui_Timer_Label, "23:00");
//	lv_timer_pause(timer);
	gt->time = BURN_IN_TIME;
//	lv_timer_reset(timer);

}

void set_sa_pressed(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
	if (update_detail_values(SA_INDEX)) {
		lv_chart_refresh(chart); /*Required after direct set*/
	}

}


void set_sb_pressed(lv_event_t *e) {
	lv_obj_t *chart = e->user_data;
	update_detail_values(SB_INDEX);
	lv_chart_refresh(chart); /*Required after direct set*/

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
