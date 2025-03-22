file(GLOB fls *.cpp)

add_library(game SHARED ${fls})
set_target_properties(game PROPERTIES PREFIX "")
