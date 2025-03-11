file(GLOB_RECURSE fls *.c)

pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLFW REQUIRED glfw3)

add_library(engine SHARED ${fls})
target_include_directories(engine PRIVATE
	${GLEW_INCLUDE_DIRECTORIES}
	${GLFW_INCLUDE_DIRECTORIES}
)
target_link_libraries(engine PRIVATE
	${GLEW_STATIC_LIBRARIES}
	${GLFW_STATIC_LIBRARIES}
	math
	platform
)
