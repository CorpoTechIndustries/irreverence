file(GLOB fls *.c)

pkg_check_modules(CGLM REQUIRED cglm)
add_library(math STATIC ${fls})
target_link_libraries(math PRIVATE ${CGLM_LIBRARIES_STATIC})
target_include_directories(math PRIVATE ${CGLM_INCLUDE_DIRECTORIES})
