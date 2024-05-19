# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

#file(GLOB_RECURSE LV_DEMOS_SOURCES ./*.c)

set(include_dirs  "." "gui" "app" "app/burn_in/include")
set(src_dirs  "." "gui" "app" "gui/image" "app/burn_in")

#set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components)

idf_component_register(
	SRC_DIRS		${src_dirs}
    INCLUDE_DIRS    ${include_dirs} 												# optional, add here public include directories

#    PRIV_INCLUDE_DIRS   															# optional, add here private include directories
#    REQUIRES            app_event_handler esp-box serial_inno lvgl mqtt_handler inno_connect	# optional, list the public requirements (component names)
#    PRIV_REQUIRES       bsp es7210 nvs_flash 										# optional, list the private requirements
)


target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wno-deprecated-declarations")


set_source_files_properties(
    ${LV_DEMOS_SOURCES}
    PROPERTIES COMPILE_OPTIONS
    -DLV_LVGL_H_INCLUDE_SIMPLE)
