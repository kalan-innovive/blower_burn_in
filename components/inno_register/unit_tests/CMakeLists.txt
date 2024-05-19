# This assumes Google Test is available at a certain path or as a submodule
#add_subdirectory(../../../libs/googletest)
#
## Include the Google Test headers
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
include_directories(../include)
set(SOURCE_FILES
        ../dev_types.c
        ../msg16_types.c
        ../register.c
        # Add other source files of your component here
)

set(TEST_FILES
        msg16_types_test.cpp
        register_test.cpp
        dev_types_test.cpp
)

# Add the test executable
add_library(inno_register_lib STATIC ${SOURCE_FILES})
#add_executable(inno_register_tests ${TEST_FILES})
## Link the test executable with GoogleTest and the main component library
#target_link_libraries(inno_register_tests gtest gtest_main inno_register)
