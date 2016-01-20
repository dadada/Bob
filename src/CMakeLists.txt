
add_executable(Bob Main.cpp Main.hpp Grid.cpp Grid.hpp Gameplay.cpp Gameplay.hpp Gui.hpp Gui.cpp)
target_link_libraries(Bob ${SDL2_LIB} ${SDL2_GFX_LIB} ${SDL2_TTF_LIB} ${Boost_LIBRARIES})
