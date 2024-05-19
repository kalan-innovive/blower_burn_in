# Cmake file for main/app/burn_in Components

#Cmake for unit_test
if (${GTEST})
    message("Running GTEST CMake: main/app/burn_in")

    #add header directories
    set(SOURCE_FILES
            burnin_valve.c
            valve_setup_task.c
            offset_data.c
            valve_ach_task.c
            valve_position_task.c
    )

    # Add the library
    add_library(BurninLib STATIC ${SOURCE_FILES})

    # Add the linked libraries
    target_link_libraries(BurninLib PUBLIC
            idf_moc_lib
            inno_register_lib

    )

    #add header directories
    target_include_directories(BurninLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests)

    # Add all subdirectories for tests
    add_subdirectory(unit_tests)
else ()

    # Add the header file to the HEADER_FILES list
    list(APPEND HEADER_FILES
            unit_tests/serial_inno.h
    )
    message("Header files: ${HEADER_FILES}")
    list(APPEND SOURCE_FILES
            unit_tests/serial_inno.c
    )
else()
	
endif ()

