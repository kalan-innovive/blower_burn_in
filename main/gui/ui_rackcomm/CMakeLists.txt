message(STATUS "Building Burnin  UI\n")

SET(SOURCES components/ui_comp_blowerbipanel.c
    components/ui_comp.c
    screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_Font_Awesome_Solid.c)

add_library(ui ${SOURCES})