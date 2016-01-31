add_executable(Bob Bob.cpp Gameplay.cpp Gui.cpp Events.cpp Wrapper.cpp Bots.hpp Bots.cpp)
target_link_libraries(Bob ${SDL2_LIB} ${SDL2_GFX_LIB} ${SDL2_TTF_LIB} ${Boost_LIBRARIES})