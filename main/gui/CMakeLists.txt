if(APP_BLOWERBURNIN)
	message(STATUS "Including blower burn in UI\n")
	add_subdirectory("ui_burnin")
else()
	message(STATUS "Including rack comm  UI\n")
	add_subdirectory("ui_raccomm")
endif()
