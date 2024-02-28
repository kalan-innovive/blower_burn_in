	message(STATUS "Building Burnin  UI\n")

SET(SOURCES 
	ui_main.c
	ui_boot_animate.c
	ui_boot_animate.h     
	ui_sr.c
	ui.c
	ui_events.c           
    ui_helpers.c
	ui_blower_burn_in.c 

add_library(ui ${SOURCES})
