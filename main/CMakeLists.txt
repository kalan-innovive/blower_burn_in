# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

#file(GLOB_RECURSE LV_DEMOS_SOURCES ./*.c)
#message(STATUS "\n\n")
#message(STATUS "LV_DEMOS_SOURCES:   ${LV_DEMOS_SOURCES}")

if(APP_BLOWERBURNIN)
	message(STATUS "Including blower burn in Source\n")
	SET(SOURCES 
	app/app_burnin_test.c
	app/burn_in.c
	app/apprackcomm.c
	app/burn_in.h
	app/offset_data.c
	app/app_rack.c
	
)
set(include_dirs  "." "gui/ui_burnin" "app")
else()
	file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "gui/ui_rackcomm/*.c")
	message(STATUS "Including rack comm  Source\n")
	SET(src "app/apprackcomm.c" "${SRC_UI}" "main.c" "settings.c")  
	set(include_dirs  "." "gui/ui_rackcomm" "app")
endif()


set(APP_BLOWERBURNIN off)
set(APP_RACKCOMM on)

#add_subdirectory(gui)
#set(include_dirs  "." "gui" "app")
#set(src_dirs  "." "app")

#set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components)
#	SRC_DIRS		${src_dirs}

idf_component_register(
	SRCS  ${src}
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
