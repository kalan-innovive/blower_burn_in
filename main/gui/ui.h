// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: InnoRack

#ifndef _INNORACK_UI_H
#define _INNORACK_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

extern const char *notify_burnin_on;
extern const char *notify_burnin_off;
extern const char *notify_burnin_ready;

extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_Header_Main_Panel;
void ui_event_Header_Descriptors_Label(lv_event_t *e);
extern lv_obj_t *ui_Header_Descriptors_Label;
extern lv_obj_t *ui_ESPNameLabel;
extern lv_obj_t *ui_ESPadressLabel;
void ui_event_StartButton(lv_event_t *e);
extern lv_obj_t *ui_StartButton;
extern lv_obj_t *ui_Screen1_Label3;
extern lv_obj_t *ui_Screen2;
extern lv_obj_t *ui_Header_Program_Panel;
void ui_event_Screen2_Button2(lv_event_t *e);
extern lv_obj_t *ui_Screen2_Button2;
extern lv_obj_t *ui_Back;
void ui_event_Timer_Label(lv_event_t *e);
extern lv_obj_t *ui_Timer_Label;
extern lv_obj_t *ui_Screen2_Body_Panel;
extern lv_obj_t *ui_BloweSAPanel;
extern lv_obj_t *ui_BlowerLabelSA;
extern lv_obj_t *ui_ChipIDLabelSA;
extern lv_obj_t *ui_OffsetLabelSA;
extern lv_obj_t *ui_RangeValLabelSA;
extern lv_obj_t *ui_RangeSA;
extern lv_obj_t *ui_ChipIDValLabelSA;
extern lv_obj_t *ui_OffsetValLabelSA;
extern lv_obj_t *ui_PassedLabelSA;
extern lv_obj_t *ui_BlowerEAPanel;
extern lv_obj_t *ui_BlowerLabel1;
extern lv_obj_t *ui_ChipIDLabel1;
extern lv_obj_t *ui_OffsetLabel1;
extern lv_obj_t *ui_RangeValLabel1;
extern lv_obj_t *ui_Range1;
extern lv_obj_t *ui_ChipIDValLabel1;
extern lv_obj_t *ui_OffsetValLabel1;
extern lv_obj_t *ui_PassedLabel1;
extern lv_obj_t *ui_BlowerSAPanel2;
extern lv_obj_t *ui_BlowerLabel2;
extern lv_obj_t *ui_ChipIDLabel2;
extern lv_obj_t *ui_OffsetLabel2;
extern lv_obj_t *ui_RangeValLabel2;
extern lv_obj_t *ui_Range2;
extern lv_obj_t *ui_ChipIDValLabel2;
extern lv_obj_t *ui_OffsetValLabel2;
extern lv_obj_t *ui_PassedLabel2;
void ui_event_BlowerSAPanel(lv_event_t *e);
void ui_event_BlowerSBPanel(lv_event_t *e);
void ui_event_BlowerEAPanel(lv_event_t *e);
void ui_event_BlowerEBPanel(lv_event_t *e);

extern lv_obj_t *ui_BlowerSAPanel;
extern lv_obj_t *ui_BlowerLabel3;
extern lv_obj_t *ui_ChipIDLabel3;
extern lv_obj_t *ui_OffsetLabel3;
extern lv_obj_t *ui_RangeValLabel3;
extern lv_obj_t *ui_BlowerVASLabel;
extern lv_obj_t *ui_BlowerQCLabel;

extern lv_obj_t *ui_Range3;
extern lv_obj_t *ui_ChipIDValLabel3;
extern lv_obj_t *ui_OffsetValLabel3;
extern lv_obj_t *ui_PassedLabel3;
extern lv_obj_t *ui_BlowerSBPanel;
extern lv_obj_t *ui_BlowerLabel4;
extern lv_obj_t *ui_ChipIDLabel4;
extern lv_obj_t *ui_OffsetLabel4;
extern lv_obj_t *ui_RangeValLabel4;
extern lv_obj_t *ui_Range4;
extern lv_obj_t *ui_ChipIDValLabel4;
extern lv_obj_t *ui_OffsetValLabel4;
extern lv_obj_t *ui_PassedLabel4;
extern lv_obj_t *ui_Screen4;
extern lv_obj_t *ui_Header_Panel_Blower_Information;
void ui_event_Screen4_Back_Button(lv_event_t *e);
extern lv_obj_t *ui_Screen4_Back_Button;
extern lv_obj_t *ui_Back4_Label;
extern lv_obj_t *ui_BlowerHeaderLabel;

extern lv_obj_t *ui_Blower_Val_Chart;
// Added for detail screen

extern lv_obj_t *ui_DetailPanel;
extern lv_obj_t *ui_VasValue;
extern lv_obj_t *ui_QC_Value;
extern lv_obj_t *ui_ErrorLable;
//extern lv_obj_t *ui____initial_actions0;

extern lv_obj_t *ui_Screen0;
extern lv_obj_t *ui_Header_Panel_Settings;
void ui_event_Screen2_Button3(lv_event_t *e);
extern lv_obj_t *ui_Screen2_Button3;
extern lv_obj_t *ui_Back2;
void ui_event_Screen0_Header_Label(lv_event_t *e);
extern lv_obj_t *ui_Screen0_Header_Label;
extern lv_obj_t *ui_Screen0_Body_Panel;
void ui_event_SettingsPanel(lv_event_t *e);
extern lv_obj_t *ui_SettingsPanel;
extern lv_obj_t *ui_SettingNamePanel;
extern lv_obj_t *ui_EspLabel;
void ui_event_ESPIDTextArea(lv_event_t *e);
extern lv_obj_t *ui_ESPIDTextArea;
extern lv_obj_t *ui_SettingwifiPanel;
extern lv_obj_t *ui_WifiPanelLabel;
void ui_event_WifiTextArea(lv_event_t *e);
extern lv_obj_t *ui_WifiTextArea;
extern lv_obj_t *ui_SettingPassWordPanel;
extern lv_obj_t *ui_PasswordPanelLabel;
void ui_event_PasswordTextArea(lv_event_t *e);
extern lv_obj_t *ui_PasswordTextArea;
extern lv_obj_t *ui_SettingProgramPanel;
extern lv_obj_t *ui_ProgramLabel;
void ui_event_Program_Dropdown(lv_event_t *e);
extern lv_obj_t *ui_Program_Dropdown;
void ui_event_Settings_Keyboard(lv_event_t *e);
extern lv_obj_t *ui_Settings_Keyboard;
void lv_chart_cb(lv_event_t *e);
void update_timer_counter(lv_timer_t *timer, int t);
void set_sa_pressed(lv_event_t *e);
void set_sb_pressed(lv_event_t *e);
void set_ea_pressed(lv_event_t *e);
void set_eb_pressed(lv_event_t *e);

// Setter functions for name identification
void set_ui_esp_name(const char *esp_name);
void set_ui_ip(const char *ip_str);

void stop_data_requests(lv_event_t *e);
void update_time_counter(lv_event_t *e);
void clock_run_cb(lv_timer_t *timer);
void burn_in_test_start(lv_timer_t *timer);
void burn_in_cooldown_start(lv_timer_t *timer);
void burn_in_valve_finished(lv_timer_t *timer);
void burn_in_cancel(lv_timer_t *timer);
void start_pressed(void);

// Notification panel
extern lv_obj_t *ui_NotaficationPanel;
extern lv_obj_t *ui_NotificationLabel;
void ui_event_OKButton(lv_event_t *e);
extern lv_obj_t *ui_OKButton;
extern lv_obj_t *ui_OKLabel;
//extern lv_obj_t *ui____initial_actions0;

extern lv_obj_t *ui_TimerNotificationPanel;
void ui_event_TimerButton(lv_event_t *e);
extern lv_obj_t *ui_TimerButton;
extern lv_obj_t *ui_TimerNotificationLabel;

typedef struct _t_gui_timer {
	int time;
	lv_obj_t *lab_time;
} t_gui_timer;
extern t_gui_timer gui_timer;

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
