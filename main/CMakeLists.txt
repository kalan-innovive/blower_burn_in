# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

#file(GLOB_RECURSE LV_DEMOS_SOURCES ./*.c)

set(include_dirs  "." "gui" "app")
set(src_dirs  "." "gui" "app" "gui/image")

set(EXTRA_COMPONENT_DIRS

    $ENV{IDF_PATH}/examples/common_components
    )

idf_component_register(
	SRC_DIRS		${src_dirs}
    INCLUDE_DIRS    ${include_dirs} # optional, add here public include directories

    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            app_event_handler esp-box 	# optional, list the public requirements (component names)
    PRIV_REQUIRES       bsp es7210 nvs_flash# optional, list the private requirements
)
#
#file(GLOB_RECURSE LV_DEMOS_SOURCES ./*.c)
#
#idf_component_register(
#    SRC_DIRS
#        "."
#        "app"
#        "gui"
##        "gui/font"
##        "gui/image"
##        "rmaker"
#
#    INCLUDE_DIRS
#        "."
#        "gui"
#        "app"
##        "rmaker")
#)
#
#
#target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
#
#target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-D RMAKER_DEMO_PROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")
#
#set_source_files_properties(
#    ${LV_DEMOS_SOURCES}
#    PROPERTIES COMPILE_OPTIONS
#    -DLV_LVGL_H_INCLUDE_SIMPLE)

#spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
