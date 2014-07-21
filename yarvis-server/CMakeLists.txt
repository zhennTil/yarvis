
find_package (Portaudio REQUIRED)

include_directories (beatdetektor/cpp kiss_fft130 kiss_fft130/tools ${PORTAUDIO_INCLUDE_DIRS})

add_definitions (${PORTAUDIO_DEFINITIONS})

# BeatDetektor sources
list (APPEND YARVIS_SERVER_SOURCES "beatdetektor/cpp/BeatDetektor.cpp")

# KISS FFT sources
list (APPEND YARVIS_SERVER_SOURCES "kiss_fft130/kiss_fft.c" "kiss_fft130/tools/kiss_fftr.c")

# Server sources
list (APPEND YARVIS_SERVER_SOURCES "server.cpp")

add_executable (yarvis-server ${YARVIS_SERVER_SOURCES})
target_link_libraries (yarvis-server ${PORTAUDIO_LIBRARIES})
