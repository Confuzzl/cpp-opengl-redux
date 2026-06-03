# THIS IS USING GLAD 2.0 GENERATOR WITH GL CORE 4.6

project(glad)

add_library(glad
	"include/glad/gl.h"
	"include/KHR/khrplatform.h"	
	"src/gl.c"
)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/include)