file(GLOB fls *.cpp)

add_library(client SHARED ${fls})
set_target_properties(client PROPERTIES PREFIX "")
