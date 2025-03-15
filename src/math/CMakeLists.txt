file(GLOB fls *.c)

add_library(math STATIC ${fls})
target_link_libraries(math PRIVATE cglm)
target_include_directories(math PRIVATE ${CGLM_INCLUDE_DIRECTORIES})
