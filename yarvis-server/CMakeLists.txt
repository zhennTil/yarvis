
find_package (Portaudio REQUIRED)
find_package (OpenGL REQUIRED)
find_package (GLFW REQUIRED)
find_package (SDL REQUIRED)
find_package (SDL_net REQUIRED)

include_directories (${YARVIS_LIB_DIR}/beatdetektor/cpp ${YARVIS_LIB_DIR}/kiss_fft130 ${YARVIS_LIB_DIR}/kiss_fft130/tools ${PORTAUDIO_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${SDL_INCLUDE_DIR} ${SDL_NET_INCLUDE_DIRS})

add_definitions (${PORTAUDIO_DEFINITIONS})


# BeatDetektor sources
list (APPEND YARVIS_SERVER_SOURCES "${YARVIS_LIB_DIR}/beatdetektor/cpp/BeatDetektor.cpp")

# KISS FFT sources
list (APPEND YARVIS_SERVER_SOURCES "${YARVIS_LIB_DIR}/kiss_fft130/kiss_fft.c" "${YARVIS_LIB_DIR}/kiss_fft130/tools/kiss_fftr.c")

# Server sources
file (GLOB_RECURSE YARVIS_SERVER_SOURCES_GLOB "*.cpp")
list (APPEND YARVIS_SERVER_SOURCES "${YARVIS_SERVER_SOURCES_GLOB}")

add_executable (yarvis-server ${YARVIS_SERVER_SOURCES})
target_link_libraries (yarvis-server ${PORTAUDIO_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL_LIBRARY} ${SDL_NET_LIBRARIES})
