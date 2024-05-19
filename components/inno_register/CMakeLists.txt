
set(HEADER_FILES
        include/register.h
        include/dev_types.h
        include/reg_types.h
        include/msg16_types.h
)
#Creates an env var for building SOURCE_FILES
set(SOURCE_FILES
        register.c
        dev_types.c
        msg16_types.c
        burnin_types.c
)
if (${GTEST})
	project(inno_register)
	
	message("Running GTEST: inno_register")
	
	add_library(inno_register_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
#
#    if (${GTEST})
#        project(testInnoRegister)
#
#        #Adds source files from tes_lib to be built from the SOURCE_FILES, will be statically linked
#        add_executable(inno_register ${SOURCE_FILES} ${HEADER_FILES})
#    else ()
#        #        set(COMPONENT_SRCS "${SOURCE_FILES}")
#        #        set(COMPONENT_ADD_INCLUDEDIRS ".")
#        #        register_component()
#    endif ()
else ()
	idf_component_register(SRCS ${SOURCE_FILES}
		INCLUDE_DIRS "include"
#		REQUIRES json esp_event
#		PRIV_REQUIRES serial_inno 
	)
endif ()