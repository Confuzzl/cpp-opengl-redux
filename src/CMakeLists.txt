target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	"main.cpp"
	"util.cpp"
	"gl_util.cpp"
	"callback.cpp"
	"color.cpp"
	"runtime_array.cpp"
)

add_subdirectory("input")
add_subdirectory("geom")
add_subdirectory("gl")
add_subdirectory("app")