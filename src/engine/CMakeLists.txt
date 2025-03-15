file(GLOB_RECURSE fls *.c)

pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(ASSIMP REQUIRED assimp)
pkg_check_modules(AL REQUIRED openal)

add_library(engine SHARED ${fls})
target_include_directories(engine PRIVATE
	${GLEW_INCLUDE_DIRECTORIES}
	${GLFW_INCLUDE_DIRECTORIES}
	${ASSIMP_INCLUDE_DIRECTORIES}
	${AL_INCLUDE_DIRECTORIES}
	${JOLTC_INCLUDE_DIRECTORIES}
	${STB_INCLUDE_DIRECTORIES}
)
target_link_libraries(engine PRIVATE
	${GLEW_STATIC_LIBRARIES}
	${GLFW_STATIC_LIBRARIES}
	${ASSIMP_STATIC_LIBRARIES}
	${AL_STATIC_LIBRARIES}
	joltc
	math
	platform
	util
)
