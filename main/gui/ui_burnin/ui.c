// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: InnoRack
#include "stddef.h"
#include "stdbool.h"
#include "ui.h"
#include "ui_helpers.h"
#include "esp_log.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t *ui_Screen1;
lv_obj_t *ui_Header_Main_Panel;
void ui_event_Header_Descriptors_Label(lv_event_t *e);
lv_obj_t *ui_Header_Descriptors_Label;
lv_obj_t *ui_ESPNameLabel;
lv_obj_t *ui_ESPadressLabel;
void ui_event_StartButton(lv_event_t *e);
lv_obj_t *ui_StartButton;
lv_obj_t *ui_Screen1_Label3;
lv_obj_t *ui_Screen2;
lv_obj_t *ui_Header_Program_Panel;
void ui_event_Screen2_Button2(lv_event_t *e);
lv_obj_t *ui_Screen2_Button2;
lv_obj_t *ui_Back;
void ui_event_Timer_Label(lv_event_t *e);
lv_obj_t *ui_Timer_Label;
lv_obj_t *ui_Screen2_Body_Panel;
lv_obj_t *ui_BloweSAPanel;
lv_obj_t *ui_BlowerLabelSA;
lv_obj_t *ui_ChipIDLabelSA;
lv_obj_t *ui_OffsetLabelSA;
lv_obj_t *ui_RangeValLabelSA;
lv_obj_t *ui_RangeSA;
lv_obj_t *ui_ChipIDValLabelSA;
lv_obj_t *ui_OffsetValLabelSA;
lv_obj_t *ui_PassedLabelSA;
lv_obj_t *ui_BlowerEAPanel;
lv_obj_t *ui_BlowerLabel1;
lv_obj_t *ui_ChipIDLabel1;
lv_obj_t *ui_OffsetLabel1;
lv_obj_t *ui_RangeValLabel1;
lv_obj_t *ui_Range1;
lv_obj_t *ui_ChipIDValLabel1;
lv_obj_t *ui_OffsetValLabel1;
lv_obj_t *ui_PassedLabel1;
lv_obj_t *ui_BlowerSAPanel2;
lv_obj_t *ui_BlowerLabel2;
lv_obj_t *ui_ChipIDLabel2;
lv_obj_t *ui_OffsetLabel2;
lv_obj_t *ui_RangeValLabel2;
lv_obj_t *ui_Range2;
lv_obj_t *ui_ChipIDValLabel2;
lv_obj_t *ui_OffsetValLabel2;
lv_obj_t *ui_PassedLabel2;
void ui_event_BlowerSAPanel(lv_event_t *e);
void ui_event_BlowerSBPanel(lv_event_t *e);
void ui_event_BlowerEAPanel(lv_event_t *e);
void ui_event_BlowerEBPanel(lv_event_t *e);

lv_obj_t *ui_BlowerSAPanel;
lv_obj_t *ui_BlowerLabel3;
lv_obj_t *ui_ChipIDLabel3;
lv_obj_t *ui_OffsetLabel3;
lv_obj_t *ui_RangeValLabel3;
lv_obj_t *ui_Range3;
lv_obj_t *ui_ChipIDValLabel3;
lv_obj_t *ui_OffsetValLabel3;
lv_obj_t *ui_PassedLabel3;
lv_obj_t *ui_BlowerSBPanel;
lv_obj_t *ui_BlowerLabel4;
lv_obj_t *ui_ChipIDLabel4;
lv_obj_t *ui_OffsetLabel4;
lv_obj_t *ui_RangeValLabel4;
lv_obj_t *ui_Range4;
lv_obj_t *ui_ChipIDValLabel4;
lv_obj_t *ui_OffsetValLabel4;
lv_obj_t *ui_PassedLabel4;
lv_obj_t *ui_Screen4;
// Added for detail screen
lv_obj_t *ui_DetailPanel;
lv_obj_t *ui_VasValue;
lv_obj_t *ui_QC_Value;
lv_obj_t *ui_ErrorLable;
//lv_obj_t *ui____initial_actions0;

lv_obj_t *ui_Header_Panel_Blower_Information;
void ui_event_Screen4_Back_Button(lv_event_t *e);
lv_obj_t *ui_Screen4_Back_Button;
lv_obj_t *ui_Back4_Label;
lv_obj_t *ui_BlowerHeaderLabel;
lv_obj_t *ui_Blower_Val_Chart;
lv_obj_t *ui_Screen0;
lv_obj_t *ui_Header_Panel_Settings;
void ui_event_Screen2_Button3(lv_event_t *e);
lv_obj_t *ui_Screen2_Button3;
lv_obj_t *ui_Back2;
void ui_event_Screen0_Header_Label(lv_event_t *e);
lv_obj_t *ui_Screen0_Header_Label;
lv_obj_t *ui_Screen0_Body_Panel;
void ui_event_SettingsPanel(lv_event_t *e);
lv_obj_t *ui_SettingsPanel;
lv_obj_t *ui_SettingNamePanel;
lv_obj_t *ui_EspLabel;
void ui_event_ESPIDTextArea(lv_event_t *e);
lv_obj_t *ui_ESPIDTextArea;
lv_obj_t *ui_SettingwifiPanel;
lv_obj_t *ui_WifiPanelLabel;
void ui_event_WifiTextArea(lv_event_t *e);
lv_obj_t *ui_WifiTextArea;
lv_obj_t *ui_SettingPassWordPanel;
lv_obj_t *ui_PasswordPanelLabel;
void ui_event_PasswordTextArea(lv_event_t *e);
lv_obj_t *ui_PasswordTextArea;
lv_obj_t *ui_SettingProgramPanel;
lv_obj_t *ui_ProgramLabel;
void ui_event_Program_Dropdown(lv_event_t *e);
lv_obj_t *ui_Program_Dropdown;
void ui_event_Settings_Keyboard(lv_event_t *e);
lv_obj_t *ui_Settings_Keyboard;
t_gui_timer gui_timer;
lv_timer_t *ui_timer;

lv_obj_t *ui_NotaficationPanel;
lv_obj_t *ui_NotificationLabel;
void ui_event_OKButton(lv_event_t *e);
lv_obj_t *ui_OKButton;
lv_obj_t *ui_OKLabel;
//lv_obj_t *ui____initial_actions0;

// Notification Timerv_obj_t *ui_TimerNotificationPanel;
lv_obj_t *ui_TimerNotificationPanel;
void ui_event_TimerButton(lv_event_t *e);
lv_obj_t *ui_TimerButton;
lv_obj_t *ui_TimerNotificationLabel;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=1
#error "LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

void ui_event_TimerButton(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_CLICKED) {
		lv_obj_add_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_HIDDEN);

	}
}

void ui_event_OKButton(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_CLICKED) {
		lv_obj_add_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);
	}
}

void ui_event_Header_Descriptors_Label(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_screen_change(ui_Screen0, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);

	}
}
void ui_event_StartButton(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		start_pressed();
		burn_in_test_start(ui_timer);
		_ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_OVER_LEFT, 500, 50);
	}
}
void ui_event_Screen2_Button2(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		burn_in_cancel(ui_timer);
		_ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);

	}
}
void ui_event_Screen2_Button3(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		ESP_LOGW("SCR2_BTN3", "Event_Pressed");

		_ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);

	}
}
void ui_event_Timer_Label(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
//		burn_in_cancel(ui_timer);
	}
	if (event_code == LV_EVENT_CANCEL) {
		_ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
	}
}
void ui_event_BlowerSAPanel(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 30);
		set_sa_pressed(e);
	}
}
void ui_event_BlowerSBPanel(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 30);
		set_sb_pressed(e);
	}
}
void ui_event_BlowerEAPanel(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 30);
		set_ea_pressed(e);
	}
}
void ui_event_BlowerEBPanel(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 30);
		set_eb_pressed(e);
	}
}
void ui_event_Screen4_Back_Button(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
		stop_data_requests(e);
	}
}
//void ui_event_Screen4_Load(lv_event_t *e) {
//	lv_event_code_t event_code = lv_event_get_code(e);
//	lv_obj_t *target = lv_event_get_target(e);
//	if (event_code == LV_EVENT_PRESSED) {
//		lv_chart_cb(e);
//	}
//}
void ui_event_Screen0_Header_Label(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_READY) {
		update_time_counter(e);
	}
}
void ui_event_SettingsPanel(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_flag_modify(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN,
		_UI_MODIFY_FLAG_ADD);
		_ui_state_modify(ui_Screen1, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);
	}
}
void ui_event_ESPIDTextArea(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_flag_modify(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN,
		_UI_MODIFY_FLAG_REMOVE);
	}
}
void ui_event_WifiTextArea(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_flag_modify(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN,
		_UI_MODIFY_FLAG_REMOVE);
	}
}
void ui_event_PasswordTextArea(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_PRESSED) {
		_ui_flag_modify(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN,
		_UI_MODIFY_FLAG_REMOVE);
	}
}
void ui_event_Program_Dropdown(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_FOCUSED) {
		_ui_flag_modify(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN,
		_UI_MODIFY_FLAG_ADD);
	}
}
void ui_event_Settings_Keyboard(lv_event_t *e) {
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t *target = lv_event_get_target(e);
	if (event_code == LV_EVENT_READY) {
		_ui_flag_modify(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN,
		_UI_MODIFY_FLAG_ADD);
	}
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void) {
	ui_Screen1 = lv_obj_create(NULL);
	lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0x393838),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Header_Main_Panel = lv_obj_create(ui_Screen1);
	lv_obj_set_width(ui_Header_Main_Panel, lv_pct(100));
	lv_obj_set_height(ui_Header_Main_Panel, lv_pct(25));
	lv_obj_set_align(ui_Header_Main_Panel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_Header_Main_Panel, LV_OBJ_FLAG_SCROLLABLE);   /// Flags
	lv_obj_set_style_bg_color(ui_Header_Main_Panel, lv_color_hex(0x2095F6),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_Header_Main_Panel, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui_Header_Main_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Header_Main_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Header_Main_Panel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Header_Main_Panel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Header_Descriptors_Label = lv_obj_create(ui_Header_Main_Panel);
	lv_obj_set_width(ui_Header_Descriptors_Label, lv_pct(70));
	lv_obj_set_height(ui_Header_Descriptors_Label, lv_pct(100));
	lv_obj_set_align(ui_Header_Descriptors_Label, LV_ALIGN_CENTER);
	lv_obj_clear_flag(ui_Header_Descriptors_Label, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_pad_left(ui_Header_Descriptors_Label, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Header_Descriptors_Label, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Header_Descriptors_Label, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Header_Descriptors_Label, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_ESPNameLabel = lv_label_create(ui_Header_Descriptors_Label);
	lv_obj_set_width(ui_ESPNameLabel, lv_pct(45));
	lv_obj_set_height(ui_ESPNameLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_ESPNameLabel, LV_ALIGN_LEFT_MID);
	lv_label_set_text(ui_ESPNameLabel, "Esp244");
	lv_obj_set_style_text_align(ui_ESPNameLabel, LV_TEXT_ALIGN_CENTER,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_ESPadressLabel = lv_label_create(ui_Header_Descriptors_Label);
	lv_obj_set_width(ui_ESPadressLabel, LV_SIZE_CONTENT);   /// 60
	lv_obj_set_height(ui_ESPadressLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_ESPadressLabel, LV_ALIGN_RIGHT_MID);
	lv_label_set_long_mode(ui_ESPadressLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(ui_ESPadressLabel, "10.10.4.95");

	ui_StartButton = lv_btn_create(ui_Screen1);
	lv_obj_set_width(ui_StartButton, 100);
	lv_obj_set_height(ui_StartButton, 100);
	lv_obj_set_x(ui_StartButton, 0);
	lv_obj_set_y(ui_StartButton, 15);
	lv_obj_set_align(ui_StartButton, LV_ALIGN_CENTER);
	lv_obj_add_flag(ui_StartButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
	lv_obj_clear_flag(ui_StartButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_radius(ui_StartButton, 100,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_StartButton, lv_color_hex(0x1603EA),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_StartButton, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui_StartButton, lv_color_hex(0xD7D7D7),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_main_stop(ui_StartButton, 150,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_stop(ui_StartButton, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui_StartButton, LV_GRAD_DIR_NONE,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui_StartButton, lv_color_hex(0xFCFCFC),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_StartButton, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_StartButton, 2,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui_StartButton, LV_BORDER_SIDE_FULL,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Screen1_Label3 = lv_label_create(ui_Screen1);
	lv_obj_set_width(ui_Screen1_Label3, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Screen1_Label3, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Screen1_Label3, 0);
	lv_obj_set_y(ui_Screen1_Label3, 12);
	lv_obj_set_align(ui_Screen1_Label3, LV_ALIGN_CENTER);
	lv_label_set_text(ui_Screen1_Label3, "Start");
	lv_obj_set_style_text_color(ui_Screen1_Label3, lv_color_hex(0xFAFAFB),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Screen1_Label3, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_add_event_cb(ui_Header_Descriptors_Label,
			ui_event_Header_Descriptors_Label, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_StartButton, ui_event_StartButton, LV_EVENT_ALL,
	NULL);

}

void ui_Screen2_screen_init(void) {
	ui_Screen2 = lv_obj_create(NULL);
	lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	/* Create Header /*/
	ui_Header_Program_Panel = lv_obj_create(ui_Screen2);
	lv_obj_set_width(ui_Header_Program_Panel, lv_pct(99));
	lv_obj_set_height(ui_Header_Program_Panel, lv_pct(10));
	lv_obj_set_align(ui_Header_Program_Panel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_Header_Program_Panel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_border_color(ui_Header_Program_Panel,
			lv_color_hex(0x3D3D3D), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_Header_Program_Panel, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_Header_Program_Panel, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui_Header_Program_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Header_Program_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Header_Program_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Header_Program_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Screen2_Button2 = lv_btn_create(ui_Header_Program_Panel);
	lv_obj_set_width(ui_Screen2_Button2, lv_pct(25));
	lv_obj_set_height(ui_Screen2_Button2, lv_pct(110));
	lv_obj_set_align(ui_Screen2_Button2, LV_ALIGN_LEFT_MID);
	lv_obj_add_flag(ui_Screen2_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);  /// Flags
	lv_obj_clear_flag(ui_Screen2_Button2, LV_OBJ_FLAG_SCROLLABLE);     /// Flags
	lv_obj_set_style_text_color(ui_Screen2_Button2, lv_color_hex(0xCCC2C2),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Screen2_Button2, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_Screen2_Button2, LV_TEXT_ALIGN_LEFT,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_decor(ui_Screen2_Button2, LV_TEXT_DECOR_UNDERLINE,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Back = lv_label_create(ui_Header_Program_Panel);
	lv_obj_set_width(ui_Back, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Back, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Back, 6);
	lv_obj_set_y(ui_Back, 0);
	lv_obj_set_align(ui_Back, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_Back, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_Back, "Cancel");
	lv_obj_set_style_text_color(ui_Back, lv_color_hex(0xF4F4F4),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Back, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Timer_Label = lv_label_create(ui_Header_Program_Panel);
	lv_obj_set_width(ui_Timer_Label, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Timer_Label, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_Timer_Label, LV_ALIGN_CENTER);
	lv_label_set_text(ui_Timer_Label, "10:00");

	/*Create Main Body*/

	ui_Screen2_Body_Panel = lv_obj_create(ui_Screen2);
	lv_obj_set_width(ui_Screen2_Body_Panel, lv_pct(100));
	lv_obj_set_height(ui_Screen2_Body_Panel, lv_pct(90));
	lv_obj_set_align(ui_Screen2_Body_Panel, LV_ALIGN_BOTTOM_MID);
	lv_obj_clear_flag(ui_Screen2_Body_Panel, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
	lv_obj_set_style_outline_color(ui_Screen2_Body_Panel,
			lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_outline_opa(ui_Screen2_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui_Screen2_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Screen2_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Screen2_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Screen2_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Supply A panel*/

//	ui_BloweSAPanel = lv_obj_create(ui_Screen2_Body_Panel);
//	lv_obj_set_width(ui_BloweSAPanel, lv_pct(48));
//	lv_obj_set_height(ui_BloweSAPanel, lv_pct(49));
//	lv_obj_clear_flag(ui_BloweSAPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
//	lv_obj_set_style_pad_left(ui_BloweSAPanel, 5,
//			LV_PART_MAIN | LV_STATE_DEFAULT);
//	lv_obj_set_style_pad_right(ui_BloweSAPanel, 5,
//			LV_PART_MAIN | LV_STATE_DEFAULT);
//	lv_obj_set_style_pad_top(ui_BloweSAPanel, 0,
//			LV_PART_MAIN | LV_STATE_DEFAULT);
//	lv_obj_set_style_pad_bottom(ui_BloweSAPanel, 0,
//			LV_PART_MAIN | LV_STATE_DEFAULT);
//
	/* Supply A panel*/
	ui_BlowerSAPanel = lv_obj_create(ui_Screen2_Body_Panel);
	lv_obj_set_width(ui_BlowerSAPanel, lv_pct(49));
	lv_obj_set_height(ui_BlowerSAPanel, lv_pct(49));
	lv_obj_clear_flag(ui_BlowerSAPanel,
			LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_pad_left(ui_BlowerSAPanel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_BlowerSAPanel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_BlowerSAPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_BlowerSAPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_BlowerLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_BlowerLabel3, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_BlowerLabel3, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_BlowerLabel3, LV_ALIGN_TOP_MID);
	lv_label_set_text(ui_BlowerLabel3, "Supply A");
	lv_obj_set_style_text_font(ui_BlowerLabel3, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_ChipIDLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_ChipIDLabel3, lv_pct(40));
	lv_obj_set_height(ui_ChipIDLabel3, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDLabel3, lv_pct(0));
	lv_obj_set_y(ui_ChipIDLabel3, lv_pct(20));
	lv_label_set_long_mode(ui_ChipIDLabel3, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_ChipIDLabel3, "ChipID:");
	lv_obj_set_style_text_font(ui_ChipIDLabel3, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_OffsetLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_OffsetLabel3, lv_pct(45));
	lv_obj_set_height(ui_OffsetLabel3, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_OffsetLabel3, lv_pct(0));
	lv_obj_set_y(ui_OffsetLabel3, lv_pct(40));
	lv_label_set_text(ui_OffsetLabel3, "Offset:   ");
	lv_obj_set_style_text_font(ui_OffsetLabel3, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_RangeValLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_RangeValLabel3, lv_pct(45));
	lv_obj_set_height(ui_RangeValLabel3, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_RangeValLabel3, lv_pct(0));
	lv_obj_set_y(ui_RangeValLabel3, lv_pct(60));
	lv_obj_set_align(ui_RangeValLabel3, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_RangeValLabel3, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_RangeValLabel3, "SAr");
	//TODO::Change back to 16
	lv_obj_set_style_text_font(ui_RangeValLabel3, &lv_font_montserrat_16,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Range3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_Range3, lv_pct(50));
	lv_obj_set_height(ui_Range3, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Range3, 0);
	lv_obj_set_y(ui_Range3, lv_pct(60));
	lv_label_set_text(ui_Range3, "Range:");
	lv_label_set_recolor(ui_Range3, "true");
	//TODO::Change back to 16
	lv_obj_set_style_text_font(ui_Range3, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_ChipIDValLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_ChipIDValLabel3, lv_pct(60));
	lv_obj_set_height(ui_ChipIDValLabel3, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDValLabel3, lv_pct(0));
	lv_obj_set_y(ui_ChipIDValLabel3, lv_pct(20));
	lv_obj_set_align(ui_ChipIDValLabel3, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_ChipIDValLabel3, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_ChipIDValLabel3, "SAid");
	lv_obj_set_style_text_font(ui_ChipIDValLabel3, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_OffsetValLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_OffsetValLabel3, lv_pct(45));
	lv_obj_set_height(ui_OffsetValLabel3, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_OffsetValLabel3, lv_pct(0));
	lv_obj_set_y(ui_OffsetValLabel3, lv_pct(40));
	lv_obj_set_align(ui_OffsetValLabel3, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_OffsetValLabel3, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_OffsetValLabel3, "SAof");
	lv_obj_set_style_text_font(ui_OffsetValLabel3, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_PassedLabel3 = lv_label_create(ui_BlowerSAPanel);
	lv_obj_set_width(ui_PassedLabel3, lv_pct(90));
	lv_obj_set_height(ui_PassedLabel3, lv_pct(20));
	lv_obj_set_x(ui_PassedLabel3, 0);
	lv_obj_set_y(ui_PassedLabel3, lv_pct(-2));
	lv_obj_set_align(ui_PassedLabel3, LV_ALIGN_BOTTOM_MID);
	lv_label_set_long_mode(ui_PassedLabel3, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(ui_PassedLabel3, "Running");
	lv_obj_set_style_text_color(ui_PassedLabel3, lv_color_hex(0x00DB24),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_PassedLabel3, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_PassedLabel3, LV_TEXT_ALIGN_CENTER,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui_PassedLabel3, 4,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_PassedLabel3, lv_color_hex(0x00E946),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_PassedLabel3, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_PassedLabel3, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Supply B panel*/
	ui_BlowerSBPanel = lv_obj_create(ui_Screen2_Body_Panel);
	lv_obj_set_width(ui_BlowerSBPanel, lv_pct(48));
	lv_obj_set_height(ui_BlowerSBPanel, lv_pct(49));
	lv_obj_set_align(ui_BlowerSBPanel, LV_ALIGN_BOTTOM_LEFT);
	lv_obj_clear_flag(ui_BlowerSBPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_pad_left(ui_BlowerSBPanel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_BlowerSBPanel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_BlowerSBPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_BlowerSBPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_BlowerLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_BlowerLabel4, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_BlowerLabel4, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_BlowerLabel4, LV_ALIGN_TOP_MID);
	lv_label_set_text(ui_BlowerLabel4, "Supply B");

	ui_ChipIDLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_ChipIDLabel4, lv_pct(45));
	lv_obj_set_height(ui_ChipIDLabel4, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDLabel4, lv_pct(0));
	lv_obj_set_y(ui_ChipIDLabel4, lv_pct(20));
	lv_label_set_text(ui_ChipIDLabel4, "ChipID:");

	ui_OffsetLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_OffsetLabel4, lv_pct(45));
	lv_obj_set_height(ui_OffsetLabel4, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_OffsetLabel4, lv_pct(0));
	lv_obj_set_y(ui_OffsetLabel4, lv_pct(40));
	lv_label_set_text(ui_OffsetLabel4, "Offset:   ");

	ui_RangeValLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_RangeValLabel4, lv_pct(45));
	lv_obj_set_height(ui_RangeValLabel4, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_RangeValLabel4, lv_pct(0));
	lv_obj_set_y(ui_RangeValLabel4, lv_pct(60));
	lv_obj_set_align(ui_RangeValLabel4, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_RangeValLabel4, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_RangeValLabel4, "SBr");

	ui_Range4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_Range4, lv_pct(50));
	lv_obj_set_height(ui_Range4, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Range4, 0);
	lv_obj_set_y(ui_Range4, lv_pct(60));
	lv_label_set_text(ui_Range4, "Range:");
	lv_label_set_recolor(ui_Range4, "true");

	ui_ChipIDValLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_ChipIDValLabel4, lv_pct(60));
	lv_obj_set_height(ui_ChipIDValLabel4, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDValLabel4, lv_pct(0));
	lv_obj_set_y(ui_ChipIDValLabel4, lv_pct(20));
	lv_obj_set_align(ui_ChipIDValLabel4, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_ChipIDValLabel4, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_ChipIDValLabel4, "SBid");

	ui_OffsetValLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_OffsetValLabel4, lv_pct(45));
	lv_obj_set_height(ui_OffsetValLabel4, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_OffsetValLabel4, lv_pct(0));
	lv_obj_set_y(ui_OffsetValLabel4, lv_pct(40));
	lv_obj_set_align(ui_OffsetValLabel4, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_OffsetValLabel4, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_OffsetValLabel4, "SBof");
	//TODO::Change back to 16
	lv_obj_set_style_text_font(ui_OffsetValLabel4, &lv_font_montserrat_14,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_PassedLabel4 = lv_label_create(ui_BlowerSBPanel);
	lv_obj_set_width(ui_PassedLabel4, lv_pct(90));
	lv_obj_set_height(ui_PassedLabel4, lv_pct(20));
	lv_obj_set_x(ui_PassedLabel4, 0);
	lv_obj_set_y(ui_PassedLabel4, lv_pct(-2));
	lv_obj_set_align(ui_PassedLabel4, LV_ALIGN_BOTTOM_MID);
	lv_label_set_long_mode(ui_PassedLabel4, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(ui_PassedLabel4, "SB_status");
	// Box around label
	lv_obj_set_style_text_color(ui_PassedLabel4, lv_color_hex(0x00DB24),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_PassedLabel4, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_PassedLabel4, LV_TEXT_ALIGN_CENTER,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui_PassedLabel4, 4,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_PassedLabel4, lv_color_hex(0x00E946),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_PassedLabel3, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_PassedLabel4, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Exhaust A panel*/
	ui_BlowerEAPanel = lv_obj_create(ui_Screen2_Body_Panel);
	lv_obj_set_width(ui_BlowerEAPanel, lv_pct(48));
	lv_obj_set_height(ui_BlowerEAPanel, lv_pct(49));
	lv_obj_set_align(ui_BlowerEAPanel, LV_ALIGN_TOP_RIGHT);
	lv_obj_clear_flag(ui_BlowerEAPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_pad_left(ui_BlowerEAPanel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_BlowerEAPanel, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_BlowerEAPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_BlowerEAPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_BlowerLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_BlowerLabel1, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_BlowerLabel1, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_BlowerLabel1, LV_ALIGN_TOP_MID);
	lv_label_set_text(ui_BlowerLabel1, "Exhaust A");

	ui_ChipIDLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_ChipIDLabel1, lv_pct(45));
	lv_obj_set_height(ui_ChipIDLabel1, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDLabel1, lv_pct(0));
	lv_obj_set_y(ui_ChipIDLabel1, lv_pct(20));
	lv_label_set_text(ui_ChipIDLabel1, "ChipID:");

	ui_OffsetLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_OffsetLabel1, lv_pct(45));
	lv_obj_set_height(ui_OffsetLabel1, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_OffsetLabel1, lv_pct(0));
	lv_obj_set_y(ui_OffsetLabel1, lv_pct(40));
	lv_label_set_text(ui_OffsetLabel1, "Offset:   ");

	ui_RangeValLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_RangeValLabel1, lv_pct(45));
	lv_obj_set_height(ui_RangeValLabel1, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_RangeValLabel1, lv_pct(0));
	lv_obj_set_y(ui_RangeValLabel1, lv_pct(60));
	lv_obj_set_align(ui_RangeValLabel1, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_RangeValLabel1, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_RangeValLabel1, "EAr");

	ui_Range1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_Range1, lv_pct(50));
	lv_obj_set_height(ui_Range1, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Range1, 0);
	lv_obj_set_y(ui_Range1, lv_pct(60));
	lv_label_set_text(ui_Range1, "Range:");
	lv_label_set_recolor(ui_Range1, "true");

	ui_ChipIDValLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_ChipIDValLabel1, lv_pct(60));
	lv_obj_set_height(ui_ChipIDValLabel1, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDValLabel1, lv_pct(0));
	lv_obj_set_y(ui_ChipIDValLabel1, lv_pct(20));
	lv_obj_set_align(ui_ChipIDValLabel1, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_ChipIDValLabel1, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_ChipIDValLabel1, "EAid");

	ui_OffsetValLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_OffsetValLabel1, lv_pct(45));
	lv_obj_set_height(ui_OffsetValLabel1, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_OffsetValLabel1, lv_pct(0));
	lv_obj_set_y(ui_OffsetValLabel1, lv_pct(40));
	lv_obj_set_align(ui_OffsetValLabel1, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_OffsetValLabel1, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_OffsetValLabel1, "EAof");

	ui_PassedLabel1 = lv_label_create(ui_BlowerEAPanel);
	lv_obj_set_width(ui_PassedLabel1, lv_pct(90));
	lv_obj_set_height(ui_PassedLabel1, lv_pct(20));
	lv_obj_set_x(ui_PassedLabel1, 0);
	lv_obj_set_y(ui_PassedLabel1, lv_pct(-2));
	lv_obj_set_align(ui_PassedLabel1, LV_ALIGN_BOTTOM_MID);
	lv_label_set_long_mode(ui_PassedLabel1, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(ui_PassedLabel1, "Starting");
	lv_obj_set_style_text_color(ui_PassedLabel1, lv_color_hex(0x00DB24),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_PassedLabel1, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_PassedLabel1, LV_TEXT_ALIGN_CENTER,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui_PassedLabel1, 4,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_PassedLabel1, lv_color_hex(0x00E946),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_PassedLabel1, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_PassedLabel1, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Exhaust B panel*/
	ui_BlowerSAPanel2 = lv_obj_create(ui_Screen2_Body_Panel);
	lv_obj_set_width(ui_BlowerSAPanel2, lv_pct(48));
	lv_obj_set_height(ui_BlowerSAPanel2, lv_pct(49));
	lv_obj_set_align(ui_BlowerSAPanel2, LV_ALIGN_BOTTOM_RIGHT);
	lv_obj_clear_flag(ui_BlowerSAPanel2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_pad_left(ui_BlowerSAPanel2, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_BlowerSAPanel2, 5,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_BlowerSAPanel2, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_BlowerSAPanel2, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_BlowerLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_BlowerLabel2, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_BlowerLabel2, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_BlowerLabel2, LV_ALIGN_TOP_MID);
	lv_label_set_text(ui_BlowerLabel2, "Exhaust B");

	ui_ChipIDLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_ChipIDLabel2, lv_pct(45));
	lv_obj_set_height(ui_ChipIDLabel2, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDLabel2, lv_pct(0));
	lv_obj_set_y(ui_ChipIDLabel2, lv_pct(20));
	lv_label_set_text(ui_ChipIDLabel2, "ChipID:");

	ui_OffsetLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_OffsetLabel2, lv_pct(45));
	lv_obj_set_height(ui_OffsetLabel2, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_OffsetLabel2, lv_pct(0));
	lv_obj_set_y(ui_OffsetLabel2, lv_pct(40));
	lv_label_set_text(ui_OffsetLabel2, "Offset:   ");

	ui_RangeValLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_RangeValLabel2, lv_pct(45));
	lv_obj_set_height(ui_RangeValLabel2, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_RangeValLabel2, lv_pct(0));
	lv_obj_set_y(ui_RangeValLabel2, lv_pct(60));
	lv_obj_set_align(ui_RangeValLabel2, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_RangeValLabel2, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_RangeValLabel2, "EBr");

	ui_Range2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_Range2, lv_pct(50));
	lv_obj_set_height(ui_Range2, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Range2, 0);
	lv_obj_set_y(ui_Range2, lv_pct(60));
	lv_label_set_text(ui_Range2, "Range:");
	lv_label_set_recolor(ui_Range2, "true");

	ui_ChipIDValLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_ChipIDValLabel2, lv_pct(60));
	lv_obj_set_height(ui_ChipIDValLabel2, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_ChipIDValLabel2, lv_pct(0));
	lv_obj_set_y(ui_ChipIDValLabel2, lv_pct(20));
	lv_obj_set_align(ui_ChipIDValLabel2, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_ChipIDValLabel2, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_ChipIDValLabel2, "EBid");

	ui_OffsetValLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_OffsetValLabel2, lv_pct(45));
	lv_obj_set_height(ui_OffsetValLabel2, LV_SIZE_CONTENT);    /// 60
	lv_obj_set_x(ui_OffsetValLabel2, lv_pct(0));
	lv_obj_set_y(ui_OffsetValLabel2, lv_pct(40));
	lv_obj_set_align(ui_OffsetValLabel2, LV_ALIGN_TOP_RIGHT);
	lv_label_set_long_mode(ui_OffsetValLabel2, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_OffsetValLabel2, "EBo");

	ui_PassedLabel2 = lv_label_create(ui_BlowerSAPanel2);
	lv_obj_set_width(ui_PassedLabel2, lv_pct(90));
	lv_obj_set_height(ui_PassedLabel2, lv_pct(20));
	lv_obj_set_x(ui_PassedLabel2, 0);
	lv_obj_set_y(ui_PassedLabel2, lv_pct(-2));
	lv_obj_set_align(ui_PassedLabel2, LV_ALIGN_BOTTOM_MID);
	lv_label_set_long_mode(ui_PassedLabel2, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(ui_PassedLabel2, "EBs");
	lv_obj_set_style_text_color(ui_PassedLabel2, lv_color_hex(0x00DB24),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_PassedLabel2, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_PassedLabel2, LV_TEXT_ALIGN_CENTER,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui_PassedLabel2, 4,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_PassedLabel2, lv_color_hex(0x00E946),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_PassedLabel2, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_PassedLabel2, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Notification pop up box*/

	ui_NotaficationPanel = lv_obj_create(ui_Screen2);
	lv_obj_set_width(ui_NotaficationPanel, 284);
	lv_obj_set_height(ui_NotaficationPanel, 160);
	lv_obj_set_x(ui_NotaficationPanel, 1);
	lv_obj_set_y(ui_NotaficationPanel, 20);
	lv_obj_set_align(ui_NotaficationPanel, LV_ALIGN_CENTER);
	lv_obj_add_flag(ui_NotaficationPanel, LV_OBJ_FLAG_HIDDEN);   /// Flags
	lv_obj_clear_flag(ui_NotaficationPanel, LV_OBJ_FLAG_SCROLLABLE);   /// Flags
	lv_obj_set_style_radius(ui_NotaficationPanel, 30,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_NotaficationPanel, lv_color_hex(0xFFFFFF),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_NotaficationPanel, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui_NotaficationPanel, lv_color_hex(0x000000),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_NotaficationPanel, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_NotaficationPanel, 3,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_NotificationLabel = lv_label_create(ui_NotaficationPanel);
	lv_obj_set_width(ui_NotificationLabel, lv_pct(100));
	lv_obj_set_height(ui_NotificationLabel, lv_pct(60));
	lv_obj_set_align(ui_NotificationLabel, LV_ALIGN_TOP_MID);
	lv_obj_set_style_text_align(ui_NotificationLabel, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_recolor(ui_NotificationLabel, true); /*Enable re-coloring by commands in the text*/

	lv_label_set_text(ui_NotificationLabel, notify_burnin_ready);
	lv_obj_set_style_text_font(ui_NotificationLabel, &lv_font_montserrat_16,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_OKButton = lv_btn_create(ui_NotaficationPanel);
	lv_obj_set_width(ui_OKButton, 150);
	lv_obj_set_height(ui_OKButton, 40);
	lv_obj_set_x(ui_OKButton, 0);
	lv_obj_set_y(ui_OKButton, 4);
	lv_obj_set_align(ui_OKButton, LV_ALIGN_BOTTOM_MID);
	lv_obj_add_flag(ui_OKButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);   /// Flags
	lv_obj_clear_flag(ui_OKButton, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
	lv_obj_set_style_bg_color(ui_OKButton, lv_color_hex(0xFFFFFF),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_OKButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui_OKButton, lv_color_hex(0x000000),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_OKButton, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_OKButton, 2,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui_OKButton, lv_color_hex(0x808080),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_OKButton, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui_OKButton, &lv_font_montserrat_16,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_OKLabel = lv_label_create(ui_OKButton);
	lv_obj_set_width(ui_OKLabel, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height(ui_OKLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_x(ui_OKLabel, 0);
	lv_obj_set_y(ui_OKLabel, -8);
	lv_obj_set_align(ui_OKLabel, LV_ALIGN_CENTER);
	lv_label_set_text(ui_OKLabel, "OK");
	lv_obj_set_style_text_font(ui_OKLabel, &lv_font_montserrat_16,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	// Setup timer Notification window

	ui_TimerNotificationPanel = lv_obj_create(ui_Screen2);
	lv_obj_set_width(ui_TimerNotificationPanel, 155);
	lv_obj_set_height(ui_TimerNotificationPanel, 73);
	lv_obj_set_x(ui_TimerNotificationPanel, -3);
	lv_obj_set_y(ui_TimerNotificationPanel, 0);
	lv_obj_set_align(ui_TimerNotificationPanel, LV_ALIGN_CENTER);
	lv_obj_add_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_FLOATING);  /// Flags
	lv_obj_clear_flag(ui_TimerNotificationPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_border_color(ui_TimerNotificationPanel,
			lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_TimerNotificationPanel, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_TimerNotificationPanel, 4,
			LV_PART_MAIN | LV_STATE_DEFAULT);
//	lv_obj_set_style_text_font(ui_TimerNotificationPanel, &lv_font_montserrat_44, LV_PART_MAIN| LV_STATE_DEFAULT);

	lv_obj_set_style_border_color(ui_TimerNotificationPanel,
			lv_color_hex(0x000000), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_TimerNotificationPanel, 200,
			LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_TimerNotificationPanel, 8,
			LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

	ui_TimerButton = lv_btn_create(ui_TimerNotificationPanel);
	lv_obj_set_width(ui_TimerButton, 231);
	lv_obj_set_height(ui_TimerButton, 119);
	lv_obj_set_align(ui_TimerButton, LV_ALIGN_CENTER);
	lv_obj_add_flag(ui_TimerButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);   /// Flags
	lv_obj_clear_flag(ui_TimerButton, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
	lv_obj_set_style_bg_color(ui_TimerButton, lv_color_hex(0xFFFFFF),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_TimerButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_TimerNotificationLabel = lv_label_create(ui_TimerNotificationPanel);
	lv_obj_set_width(ui_TimerNotificationLabel, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height(ui_TimerNotificationLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_align(ui_TimerNotificationLabel, LV_ALIGN_CENTER);
	lv_label_set_text(ui_TimerNotificationLabel, "1:05 ");
	lv_obj_set_style_text_font(ui_TimerNotificationLabel,
			&lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_add_event_cb(ui_OKButton, ui_event_OKButton, LV_EVENT_ALL, NULL);

	lv_obj_add_event_cb(ui_Screen2_Button2, ui_event_Screen2_Button2,
			LV_EVENT_ALL, NULL);

	lv_obj_add_event_cb(ui_TimerButton, ui_event_TimerButton, LV_EVENT_ALL,
	NULL);

	/* Set up timer*/

	gui_timer.lab_time = ui_Timer_Label;
	gui_timer.time = 0;
	//TODO: fix timer start stop
//	ESP_LOGI("Create Timer", "time:%d pointer")
	ui_timer = lv_timer_create(clock_run_cb, 1000, &gui_timer);
	lv_timer_set_repeat_count(ui_timer, -1);

	/* Set up callback events*/

//	lv_obj_add_event_cb(ui_Back, ui_event_Cancel_Label, LV_EVENT_ALL,
//			ui_timer);
	lv_obj_add_event_cb(ui_StartButton, ui_event_Timer_Label, LV_EVENT_ALL,
			ui_timer);

	lv_obj_add_event_cb(ui_BlowerSAPanel, ui_event_BlowerSAPanel, LV_EVENT_ALL,
	NULL);
	lv_obj_add_event_cb(ui_BlowerSBPanel, ui_event_BlowerSBPanel, LV_EVENT_ALL,
	NULL);
	lv_obj_add_event_cb(ui_BlowerEAPanel, ui_event_BlowerEAPanel, LV_EVENT_ALL,
	NULL);
	lv_obj_add_event_cb(ui_BlowerSAPanel2, ui_event_BlowerEBPanel, LV_EVENT_ALL,
	NULL);

}
void ui_Screen4_screen_init(void) {
	ui_Screen4 = lv_obj_create(NULL);
	lv_obj_clear_flag(ui_Screen4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

	ui_Header_Panel_Blower_Information = lv_obj_create(ui_Screen4);
	lv_obj_set_width(ui_Header_Panel_Blower_Information, lv_pct(99));
	lv_obj_set_height(ui_Header_Panel_Blower_Information, lv_pct(10));
	lv_obj_set_align(ui_Header_Panel_Blower_Information, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_Header_Panel_Blower_Information,
			LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_border_color(ui_Header_Panel_Blower_Information,
			lv_color_hex(0x3D3D3D), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_Header_Panel_Blower_Information, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_Header_Panel_Blower_Information, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui_Header_Panel_Blower_Information, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Header_Panel_Blower_Information, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Header_Panel_Blower_Information, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Header_Panel_Blower_Information, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Screen4_Back_Button = lv_btn_create(ui_Header_Panel_Blower_Information);
	lv_obj_set_width(ui_Screen4_Back_Button, lv_pct(25));
	lv_obj_set_height(ui_Screen4_Back_Button, lv_pct(110));
	lv_obj_set_align(ui_Screen4_Back_Button, LV_ALIGN_LEFT_MID);
	lv_obj_add_flag(ui_Screen4_Back_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
	lv_obj_clear_flag(ui_Screen4_Back_Button, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_text_color(ui_Screen4_Back_Button, lv_color_hex(0xCCC2C2),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Screen4_Back_Button, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_Screen4_Back_Button, LV_TEXT_ALIGN_LEFT,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_decor(ui_Screen4_Back_Button, LV_TEXT_DECOR_UNDERLINE,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Back4_Label = lv_label_create(ui_Header_Panel_Blower_Information);
	lv_obj_set_width(ui_Back4_Label, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Back4_Label, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Back4_Label, 6);
	lv_obj_set_y(ui_Back4_Label, 0);
	lv_obj_set_align(ui_Back4_Label, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_Back4_Label, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_Back4_Label, "Back");
	lv_obj_set_style_text_color(ui_Back4_Label, lv_color_hex(0xF4F4F4),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Back4_Label, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_BlowerHeaderLabel = lv_label_create(ui_Header_Panel_Blower_Information);
	lv_obj_set_width(ui_BlowerHeaderLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_BlowerHeaderLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_BlowerHeaderLabel, LV_ALIGN_CENTER);
	lv_label_set_text(ui_BlowerHeaderLabel, "Blower Dev");

	// Detail panel

	ui_DetailPanel = lv_obj_create(ui_Screen4);
	lv_obj_set_width(ui_DetailPanel, lv_pct(95));
	lv_obj_set_height(ui_DetailPanel, lv_pct(27));
	lv_obj_set_x(ui_DetailPanel, 0);
	lv_obj_set_y(ui_DetailPanel, lv_pct(12));
	lv_obj_set_align(ui_DetailPanel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_DetailPanel, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
	lv_obj_set_style_bg_color(ui_DetailPanel, lv_color_hex(0xFFFFFF),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_DetailPanel, 50,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_VasValue = lv_label_create(ui_DetailPanel);
	lv_obj_set_width(ui_VasValue, LV_SIZE_CONTENT);  /// 50
	lv_obj_set_height(ui_VasValue, LV_SIZE_CONTENT);   /// 1
	lv_label_set_text(ui_VasValue, "VAS Value: -56\n");

	ui_QC_Value = lv_label_create(ui_DetailPanel);
	lv_obj_set_width(ui_QC_Value, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height(ui_QC_Value, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_align(ui_QC_Value, LV_ALIGN_BOTTOM_LEFT);
	lv_label_set_text(ui_QC_Value, "QC Value:  -55");

	ui_ErrorLable = lv_label_create(ui_DetailPanel);
	lv_obj_add_flag(ui_ErrorLable, LV_OBJ_FLAG_HIDDEN);

	lv_obj_set_width(ui_ErrorLable, lv_pct(50));
	lv_obj_set_height(ui_ErrorLable, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_x(ui_ErrorLable, 0);
	lv_obj_set_y(ui_ErrorLable, -10);
	lv_obj_set_align(ui_ErrorLable, LV_ALIGN_TOP_RIGHT);
	lv_label_set_text(ui_ErrorLable,
			"Error: \nOver Range Limit; \nOver TEB Limit");
	lv_obj_set_style_text_color(ui_ErrorLable, lv_color_hex(0xFF0000),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_ErrorLable, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui_ErrorLable, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_ErrorLable, lv_color_hex(0xEBE7E7),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_ErrorLable, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui_ErrorLable, lv_color_hex(0x5E5E5E),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_ErrorLable, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_ErrorLable, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui_ErrorLable, LV_BORDER_SIDE_FULL,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	/////Chart setup////

	ui_Blower_Val_Chart = lv_chart_create(ui_Screen4);
	lv_obj_set_width(ui_Blower_Val_Chart, 240);
	lv_obj_set_height(ui_Blower_Val_Chart, 140);
	lv_obj_set_x(ui_Blower_Val_Chart, 5);
	lv_obj_set_y(ui_Blower_Val_Chart, -10);
	lv_obj_set_align(ui_Blower_Val_Chart, LV_ALIGN_BOTTOM_MID);

	lv_obj_add_event_cb(ui_Screen4_Back_Button, ui_event_Screen4_Back_Button,
			LV_EVENT_ALL, NULL);
//	lv_obj_add_event_cb(ui_Screen4, ui_event_Screen4_Load, LV_EVENT_ALL,
//			ui_Blower_Val_Chart);

}
void ui_Screen0_screen_init(void) {
	ui_Screen0 = lv_obj_create(NULL);
	lv_obj_clear_flag(ui_Screen0, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

	ui_Header_Panel_Settings = lv_obj_create(ui_Screen0);
	lv_obj_set_width(ui_Header_Panel_Settings, lv_pct(99));
	lv_obj_set_height(ui_Header_Panel_Settings, lv_pct(10));
	lv_obj_set_align(ui_Header_Panel_Settings, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_Header_Panel_Settings, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_border_color(ui_Header_Panel_Settings,
			lv_color_hex(0x3D3D3D), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_Header_Panel_Settings, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_Header_Panel_Settings, 1,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui_Header_Panel_Settings, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Header_Panel_Settings, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Header_Panel_Settings, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Header_Panel_Settings, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Screen2_Button3 = lv_btn_create(ui_Header_Panel_Settings);
	lv_obj_set_width(ui_Screen2_Button3, lv_pct(25));
	lv_obj_set_height(ui_Screen2_Button3, lv_pct(110));
	lv_obj_set_align(ui_Screen2_Button3, LV_ALIGN_LEFT_MID);
	lv_obj_add_flag(ui_Screen2_Button3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);  /// Flags
	lv_obj_clear_flag(ui_Screen2_Button3, LV_OBJ_FLAG_SCROLLABLE);     /// Flags
	lv_obj_set_style_text_color(ui_Screen2_Button3, lv_color_hex(0xCCC2C2),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Screen2_Button3, 255,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui_Screen2_Button3, LV_TEXT_ALIGN_LEFT,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_decor(ui_Screen2_Button3, LV_TEXT_DECOR_UNDERLINE,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Back2 = lv_label_create(ui_Header_Panel_Settings);
	lv_obj_set_width(ui_Back2, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Back2, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Back2, 6);
	lv_obj_set_y(ui_Back2, 0);
	lv_obj_set_align(ui_Back2, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_Back2, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_Back2, "Back");
	lv_obj_set_style_text_color(ui_Back2, lv_color_hex(0xF4F4F4),
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Back2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Screen0_Header_Label = lv_label_create(ui_Header_Panel_Settings);
	lv_obj_set_width(ui_Screen0_Header_Label, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Screen0_Header_Label, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_Screen0_Header_Label, LV_ALIGN_CENTER);
	lv_label_set_text(ui_Screen0_Header_Label, "Settings");

	ui_Screen0_Body_Panel = lv_obj_create(ui_Screen0);
	lv_obj_set_width(ui_Screen0_Body_Panel, lv_pct(100));
	lv_obj_set_height(ui_Screen0_Body_Panel, lv_pct(90));
	lv_obj_set_align(ui_Screen0_Body_Panel, LV_ALIGN_BOTTOM_MID);
	lv_obj_add_state(ui_Screen0_Body_Panel, LV_STATE_FOCUSED);       /// States
	lv_obj_set_style_pad_left(ui_Screen0_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Screen0_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Screen0_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Screen0_Body_Panel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_SettingsPanel = lv_obj_create(ui_Screen0_Body_Panel);
	lv_obj_set_width(ui_SettingsPanel, lv_pct(100));
	lv_obj_set_height(ui_SettingsPanel, lv_pct(184));
	lv_obj_set_align(ui_SettingsPanel, LV_ALIGN_TOP_MID);
	lv_obj_add_flag(ui_SettingsPanel, LV_OBJ_FLAG_SCROLL_ONE);     /// Flags
	lv_obj_set_scroll_dir(ui_SettingsPanel, LV_DIR_VER);
	lv_obj_set_style_pad_left(ui_SettingsPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_SettingsPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_SettingsPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_SettingsPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_SettingNamePanel = lv_obj_create(ui_SettingsPanel);
	lv_obj_set_width(ui_SettingNamePanel, lv_pct(100));
	lv_obj_set_height(ui_SettingNamePanel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_SettingNamePanel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_SettingNamePanel, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
	lv_obj_set_style_pad_left(ui_SettingNamePanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_SettingNamePanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_SettingNamePanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_SettingNamePanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_EspLabel = lv_label_create(ui_SettingNamePanel);
	lv_obj_set_width(ui_EspLabel, lv_pct(30));
	lv_obj_set_height(ui_EspLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_y(ui_EspLabel, 0);
	lv_obj_set_x(ui_EspLabel, lv_pct(5));
	lv_obj_set_align(ui_EspLabel, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_EspLabel, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_EspLabel, "ESP ID:");

	ui_ESPIDTextArea = lv_textarea_create(ui_SettingNamePanel);
	lv_obj_set_width(ui_ESPIDTextArea, lv_pct(60));
	lv_obj_set_height(ui_ESPIDTextArea, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_ESPIDTextArea, LV_ALIGN_RIGHT_MID);
	lv_textarea_set_max_length(ui_ESPIDTextArea, 15);
	lv_textarea_set_text(ui_ESPIDTextArea, "esp");
	lv_textarea_set_placeholder_text(ui_ESPIDTextArea, "esp");
	lv_textarea_set_one_line(ui_ESPIDTextArea, true);

	ui_SettingwifiPanel = lv_obj_create(ui_SettingsPanel);
	lv_obj_set_width(ui_SettingwifiPanel, lv_pct(100));
	lv_obj_set_height(ui_SettingwifiPanel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_SettingwifiPanel, 0);
	lv_obj_set_y(ui_SettingwifiPanel, lv_pct(10));
	lv_obj_set_align(ui_SettingwifiPanel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_SettingwifiPanel, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
	lv_obj_set_style_pad_left(ui_SettingwifiPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_SettingwifiPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_SettingwifiPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_SettingwifiPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_WifiPanelLabel = lv_label_create(ui_SettingwifiPanel);
	lv_obj_set_width(ui_WifiPanelLabel, lv_pct(30));
	lv_obj_set_height(ui_WifiPanelLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_y(ui_WifiPanelLabel, 0);
	lv_obj_set_x(ui_WifiPanelLabel, lv_pct(5));
	lv_obj_set_align(ui_WifiPanelLabel, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_WifiPanelLabel, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_WifiPanelLabel, "SSID:");

	ui_WifiTextArea = lv_textarea_create(ui_SettingwifiPanel);
	lv_obj_set_width(ui_WifiTextArea, lv_pct(60));
	lv_obj_set_height(ui_WifiTextArea, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_WifiTextArea, LV_ALIGN_RIGHT_MID);
	lv_textarea_set_max_length(ui_WifiTextArea, 15);
	lv_textarea_set_text(ui_WifiTextArea, "innoENG");
	lv_textarea_set_placeholder_text(ui_WifiTextArea, "esp");
	lv_textarea_set_one_line(ui_WifiTextArea, true);

	ui_SettingPassWordPanel = lv_obj_create(ui_SettingsPanel);
	lv_obj_set_width(ui_SettingPassWordPanel, lv_pct(100));
	lv_obj_set_height(ui_SettingPassWordPanel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_SettingPassWordPanel, 0);
	lv_obj_set_y(ui_SettingPassWordPanel, lv_pct(20));
	lv_obj_set_align(ui_SettingPassWordPanel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_SettingPassWordPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_pad_left(ui_SettingPassWordPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_SettingPassWordPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_SettingPassWordPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_SettingPassWordPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_PasswordPanelLabel = lv_label_create(ui_SettingPassWordPanel);
	lv_obj_set_width(ui_PasswordPanelLabel, lv_pct(30));
	lv_obj_set_height(ui_PasswordPanelLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_y(ui_PasswordPanelLabel, 0);
	lv_obj_set_x(ui_PasswordPanelLabel, lv_pct(5));
	lv_obj_set_align(ui_PasswordPanelLabel, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_PasswordPanelLabel, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_PasswordPanelLabel, "Password:");

	ui_PasswordTextArea = lv_textarea_create(ui_SettingPassWordPanel);
	lv_obj_set_width(ui_PasswordTextArea, lv_pct(60));
	lv_obj_set_height(ui_PasswordTextArea, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_PasswordTextArea, LV_ALIGN_RIGHT_MID);
	lv_textarea_set_max_length(ui_PasswordTextArea, 15);
	lv_textarea_set_text(ui_PasswordTextArea, "Inn0ENG$#");
	lv_textarea_set_placeholder_text(ui_PasswordTextArea, "esp");
	lv_textarea_set_one_line(ui_PasswordTextArea, true);

	ui_SettingProgramPanel = lv_obj_create(ui_SettingsPanel);
	lv_obj_set_width(ui_SettingProgramPanel, lv_pct(100));
	lv_obj_set_height(ui_SettingProgramPanel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_SettingProgramPanel, 0);
	lv_obj_set_y(ui_SettingProgramPanel, lv_pct(30));
	lv_obj_set_align(ui_SettingProgramPanel, LV_ALIGN_TOP_MID);
	lv_obj_clear_flag(ui_SettingProgramPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_pad_left(ui_SettingProgramPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_SettingProgramPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_SettingProgramPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_SettingProgramPanel, 0,
			LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_ProgramLabel = lv_label_create(ui_SettingProgramPanel);
	lv_obj_set_width(ui_ProgramLabel, lv_pct(30));
	lv_obj_set_height(ui_ProgramLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_y(ui_ProgramLabel, 0);
	lv_obj_set_x(ui_ProgramLabel, lv_pct(5));
	lv_obj_set_align(ui_ProgramLabel, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(ui_ProgramLabel, LV_LABEL_LONG_CLIP);
	lv_label_set_text(ui_ProgramLabel, "Program:");

	ui_Program_Dropdown = lv_dropdown_create(ui_SettingProgramPanel);
	lv_dropdown_set_options(ui_Program_Dropdown, "Blower BurnIn\nLogging");
	lv_obj_set_width(ui_Program_Dropdown, lv_pct(65));
	lv_obj_set_height(ui_Program_Dropdown, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_Program_Dropdown, LV_ALIGN_RIGHT_MID);
	lv_obj_add_flag(ui_Program_Dropdown, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags

	ui_Settings_Keyboard = lv_keyboard_create(ui_Screen0_Body_Panel);
	lv_keyboard_set_mode(ui_Settings_Keyboard, LV_KEYBOARD_MODE_NUMBER);
	lv_obj_set_width(ui_Settings_Keyboard, 279);
	lv_obj_set_height(ui_Settings_Keyboard, 118);
	lv_obj_set_align(ui_Settings_Keyboard, LV_ALIGN_BOTTOM_LEFT);
	lv_obj_add_flag(ui_Settings_Keyboard, LV_OBJ_FLAG_HIDDEN);     /// Flags
	lv_obj_clear_flag(ui_Settings_Keyboard, LV_OBJ_FLAG_PRESS_LOCK);   /// Flags
	lv_obj_set_scrollbar_mode(ui_Settings_Keyboard, LV_SCROLLBAR_MODE_OFF);

	lv_obj_add_event_cb(ui_Screen2_Button3, ui_event_Screen2_Button3,
			LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_Screen0_Header_Label, ui_event_Screen0_Header_Label,
			LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_ESPIDTextArea, ui_event_ESPIDTextArea, LV_EVENT_ALL,
	NULL);
	lv_obj_add_event_cb(ui_WifiTextArea, ui_event_WifiTextArea, LV_EVENT_ALL,
	NULL);
	lv_obj_add_event_cb(ui_PasswordTextArea, ui_event_PasswordTextArea,
			LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_Program_Dropdown, ui_event_Program_Dropdown,
			LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_SettingsPanel, ui_event_SettingsPanel, LV_EVENT_ALL,
	NULL);
	lv_keyboard_set_textarea(ui_Settings_Keyboard, ui_ESPIDTextArea);
	lv_obj_add_event_cb(ui_Settings_Keyboard, ui_event_Settings_Keyboard,
			LV_EVENT_ALL, NULL);

}

void ui_init(void) {
	lv_disp_t *dispp = lv_disp_get_default();
	lv_theme_t *theme = lv_theme_default_init(dispp,
			lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
			false, LV_FONT_DEFAULT);
	lv_disp_set_theme(dispp, theme);
	ui_Screen1_screen_init();
	ui_Screen2_screen_init();
	ui_Screen4_screen_init();
	ui_Screen0_screen_init();
	lv_disp_load_scr(ui_Screen1);

}

