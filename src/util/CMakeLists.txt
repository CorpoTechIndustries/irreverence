file(GLOB fls *.c)

add_library(util STATIC ${fls})
target_link_libraries(util PRIVATE
	platform
)
