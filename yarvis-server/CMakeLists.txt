
find_package (Portaudio REQUIRED)
find_package (OpenGL REQUIRED)
find_package (GLFW REQUIRED)
find_package (SDL REQUIRED)
find_package (SDL_net REQUIRED)

include_directories (beatdetektor/cpp kiss_fft130 kiss_fft130/tools ${PORTAUDIO_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${SDL_INCLUDE_DIR} ${SDL_NET_INCLUDE_DIRS})

add_definitions (${PORTAUDIO_DEFINITIONS})

# BeatDetektor sources
list (APPEND YARVIS_SERVER_SOURCES "beatdetektor/cpp/BeatDetektor.cpp")

# KISS FFT sources
list (APPEND YARVIS_SERVER_SOURCES "kiss_fft130/kiss_fft.c" "kiss_fft130/tools/kiss_fftr.c")

# Server sources
list (APPEND YARVIS_SERVER_SOURCES "main.cpp" "server.cpp" "debug_window.cpp" "audio_analyzer.cpp")

add_executable (yarvis-server ${YARVIS_SERVER_SOURCES})
target_link_libraries (yarvis-server ${PORTAUDIO_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL_LIBRARY} ${SDL_NET_LIBRARIES})
