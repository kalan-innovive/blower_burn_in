// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: RackCommReader

#include "../ui.h"

void ui_Screen1_screen_init(void)
{
ui_Screen1 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

lv_obj_set_style_bg_img_recolor(ui_Screen1, lv_color_hex(0x0F0FB2), LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
lv_obj_set_style_bg_img_recolor_opa(ui_Screen1, 255, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);

ui_BlowerChipIDPanel = lv_obj_create(ui_Screen1);
lv_obj_set_width( ui_BlowerChipIDPanel, lv_pct(100));
lv_obj_set_height( ui_BlowerChipIDPanel, lv_pct(100));
lv_obj_set_align( ui_BlowerChipIDPanel, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_BlowerChipIDPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_BlowerChipIDPanel, lv_color_hex(0x0E029B), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_BlowerChipIDPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_BlowerChipIDPanel, lv_color_hex(0x170875), LV_PART_MAIN | LV_STATE_DEFAULT );

ui_Container1 = lv_obj_create(ui_BlowerChipIDPanel);
lv_obj_remove_style_all(ui_Container1);
lv_obj_set_width( ui_Container1, 159);
lv_obj_set_height( ui_Container1, 70);
lv_obj_set_x( ui_Container1, -63 );
lv_obj_set_y( ui_Container1, 41 );
lv_obj_set_align( ui_Container1, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_Container1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Container1, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Container1, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Container1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_Container1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_Container1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_Container1, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_side(ui_Container1, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_Container1, lv_color_hex(0x0C0C23), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_Container1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_Container1, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_Container1, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_Container1, -4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_Container1, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_Container1, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(ui_Container1, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_Container1, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_Container1, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_Container1, 10, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_chipidLabel = lv_label_create(ui_Container1);
lv_obj_set_width( ui_chipidLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_chipidLabel, LV_SIZE_CONTENT);   /// 75
lv_obj_set_align( ui_chipidLabel, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_chipidLabel,"00000000");

ui_Label1 = lv_label_create(ui_Container1);
lv_obj_set_width( ui_Label1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label1, LV_SIZE_CONTENT);   /// 1
lv_label_set_text(ui_Label1,"ChipID:");

ui_Label4 = lv_label_create(ui_Container1);
lv_obj_set_width( ui_Label4, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label4, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Label4, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_Label4,"UUID: ");

ui_uuidLabel = lv_label_create(ui_Container1);
lv_obj_set_width( ui_uuidLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_uuidLabel, LV_SIZE_CONTENT);   /// 75
lv_obj_set_align( ui_uuidLabel, LV_ALIGN_RIGHT_MID );
lv_label_set_text(ui_uuidLabel,"00000000");

ui_Label6 = lv_label_create(ui_Container1);
lv_obj_set_width( ui_Label6, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label6, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Label6, LV_ALIGN_BOTTOM_LEFT );
lv_label_set_text(ui_Label6,"Offset: ");

ui_offsetLabel = lv_label_create(ui_Container1);
lv_obj_set_width( ui_offsetLabel, LV_SIZE_CONTENT);  /// 75
lv_obj_set_height( ui_offsetLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_offsetLabel, LV_ALIGN_BOTTOM_RIGHT );
lv_label_set_text(ui_offsetLabel,"0");

ui_Container2 = lv_obj_create(ui_BlowerChipIDPanel);
lv_obj_remove_style_all(ui_Container2);
lv_obj_set_width( ui_Container2, 277);
lv_obj_set_height( ui_Container2, 53);
lv_obj_set_x( ui_Container2, -5 );
lv_obj_set_y( ui_Container2, -45 );
lv_obj_set_align( ui_Container2, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_Container2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Container2, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Container2, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Container2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_Container2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_Container2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_Container2, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_side(ui_Container2, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_Container2, lv_color_hex(0x0C0C23), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_Container2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_Container2, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_Container2, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_Container2, -4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_Container2, 10, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label3 = lv_label_create(ui_Container2);
lv_obj_set_width( ui_Label3, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label3, LV_SIZE_CONTENT);   /// 1
lv_label_set_text(ui_Label3,"Device: ");

ui_deviceLabel = lv_label_create(ui_Container2);
lv_obj_set_width( ui_deviceLabel, lv_pct(75));
lv_obj_set_height( ui_deviceLabel, LV_SIZE_CONTENT);   /// 100
lv_obj_set_align( ui_deviceLabel, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_deviceLabel,"00000000");

ui_Container3 = lv_obj_create(ui_BlowerChipIDPanel);
lv_obj_remove_style_all(ui_Container3);
lv_obj_set_height( ui_Container3, 25);
lv_obj_set_width( ui_Container3, lv_pct(110));
lv_obj_set_x( ui_Container3, 0 );
lv_obj_set_y( ui_Container3, -10 );
lv_obj_set_align( ui_Container3, LV_ALIGN_TOP_MID );
lv_obj_clear_flag( ui_Container3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Container3, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Container3, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Container3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_Container3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_Container3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_Container3, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_side(ui_Container3, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_Container3, lv_color_hex(0x0C0C23), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_Container3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_Container3, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_Container3, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_Container3, -4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_Container3, 10, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Header = lv_label_create(ui_Container3);
lv_obj_set_width( ui_Header, lv_pct(50));
lv_obj_set_height( ui_Header, LV_SIZE_CONTENT);   /// 50
lv_obj_set_align( ui_Header, LV_ALIGN_CENTER );
lv_label_set_text(ui_Header,"RackComm");

}
