add_executable(Bob Main.cpp Main.hpp Grid.cpp Grid.hpp Gameplay.cpp Gameplay.hpp Gui.hpp Gui.cpp)
target_link_libraries(Bob ${SDL2_LIBRARY} ${SDL2GFX_LIBRARY} ${Boost_LIBRARIES})
