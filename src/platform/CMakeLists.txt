
if (WIN32)

elseif (UNIX)
	file(GLOB fls linux/*.c)
	add_library(platform STATIC ${fls})
endif()
