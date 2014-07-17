
find_package (Portaudio REQUIRED)

include_directories (beatdetektor/cpp ${PORTAUDIO_INCLUDE_DIRS})

add_definitions (${PORTAUDIO_DEFINITIONS})

add_executable (yarvis-server beatdetektor/cpp/beatdetektor.cpp server.cpp)
target_link_libraries (yarvis-server ${PORTAUDIO_LIBRARIES})
