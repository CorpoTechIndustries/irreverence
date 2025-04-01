pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(ASSIMP REQUIRED assimp)
pkg_check_modules(AL REQUIRED openal)

file(GLOB engine-files *.c)

file(GLOB audio-files audio/*.c)
file(GLOB render-files renderer/*.c)

add_library(engine SHARED
	${engine-files}
	${audio-files}
	${render-files}
	${IMGUI_BACKEND_DIRECTORIES}/imgui_impl_glfw.cpp
	${IMGUI_BACKEND_DIRECTORIES}/imgui_impl_opengl3.cpp
)
target_include_directories(engine PRIVATE
	${GLEW_INCLUDE_DIRECTORIES}
	${GLFW_INCLUDE_DIRECTORIES}
	${ASSIMP_INCLUDE_DIRECTORIES}
	${AL_INCLUDE_DIRECTORIES}
	${JOLTC_INCLUDE_DIRECTORIES}
	${STB_INCLUDE_DIRECTORIES}
	${IMGUI_INCLUDE_DIRECTORIES}
)
target_link_libraries(engine PRIVATE
	${GLEW_STATIC_LIBRARIES}
	${GLFW_STATIC_LIBRARIES}
	${ASSIMP_STATIC_LIBRARIES}
	${AL_STATIC_LIBRARIES}
	joltc
	cimgui
	math
	platform
	util
)

target_compile_definitions(engine PRIVATE -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)
