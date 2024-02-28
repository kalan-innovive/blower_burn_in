# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

set(src_files "spi_mod.c" "helpers.c" "ms45sensor.c" "pressureuartdriver.c" "main.c")
set(priv_inc "json" "driver" "freertos")
set(requir "driver")



idf_component_register(
    SRCS 			${src_files}# list the source files of this component
    INCLUDE_DIRS 	"include" 	# optional, add here public include directories
    REQUIRES		${require}			# optional, list the public requirements (component names)
    PRIV_REQUIRES   ${priv_inc}	# optional, list the private requirements
    )

#idf_component_register(SRCS "spi_mod.c" "helpers.c" "ms45sensor.c" "pressureuartdriver.c"
#                    INCLUDE_DIRS "include"
#                    REQUIRES console cmd_wifi cmd_nvs cmd_system sensor_cmd
#                    PRIV_REQUIRES fatfs)