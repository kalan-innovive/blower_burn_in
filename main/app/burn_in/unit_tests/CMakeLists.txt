# CMake file for main/app/burn_in unit_tests

# Set Test component files
set(TEST_FILES
        valve_setup_task_test.cpp
        MockSerialInno.cpp
        MockSerialInno.h
        offset_data_test.cpp
        #        ../../components/inno_register/dev_types.c
        #        serial_inno.h
)


# Add the test executable
add_executable(BurnInUnitTests ${TEST_FILES})


target_link_libraries(BurnInUnitTests
        idf_moc_lib
        inno_register_lib
        BurninLib
        gtest
        gmock
        gtest_main
)

add_test(NAME BurnInUnitTests COMMAND BurnInUnitTests)

