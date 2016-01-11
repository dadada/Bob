add_executable(Bob Main.cpp)
add_library(GameMap GameMap.cpp)
target_link_libraries(Bob GameMap ${SDL2_LIBRARY} ${SDL2GFX_LIBRARY} ${Boost_LIBRARIES})
