cmake_minimum_required(VERSION 3.5)

project(offline_speech_analysis)
   
include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-fdiagnostics-color -ftree-vectorize")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_support)

set(DEPS_PREFIX ${PROJECT_BINARY_DIR}/deps)
list(APPEND CMAKE_PREFIX_PATH ${DEPS_PREFIX})

ExternalProject_Add(eigen3
    PREFIX ${PROJECT_BINARY_DIR}/eigen3
    URL https://gitlab.com/libeigen/eigen/-/archive/7252163335f56f23fcc7381c1efdea47161005fa/eigen-7252163335f56f23fcc7381c1efdea47161005fa.tar.bz2
    URL_HASH SHA256=bbb89ecda2d27f854cea0bc6da7a472ccee12424ad14da91cd367e6f46e8070c
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEPS_PREFIX} -DBUILD_TESTING=OFF
)

set(EIGEN_INCLUDE_DIR ${DEPS_PREFIX}/include/eigen3)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(PortAudio REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS AVFORMAT AVUTIL AVCODEC SWRESAMPLE)
find_package(FFTW REQUIRED COMPONENTS DOUBLE_LIB)
find_package(BLAS REQUIRED)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

set(SOURCES
    resources.qrc
    src/main/main.cc
    src/main/track.h
    src/main/medianfilter.h
    src/gui/gui.h
    src/gui/winmain.cc
    src/gui/style.h
    src/gui/style.cc
    src/gui/mainwindow.h
    src/gui/mainwindow.cc
    src/gui/trackview.h
    src/gui/trackview.cc
    src/plot/graph.h
    src/plot/graph.cc
    src/plot/series.h
    src/plot/series.cc
    src/plot/axis.h
    src/plot/axis.cc
    src/plot/transform.h
    src/plot/transform_data.h
    src/plot/transform_axis.h
    src/plot/transform.cc
    src/audio/audio.h
    src/audio/decode_file.c
    src/karma/karma.h
    src/karma/ar.cc
    src/karma/cepstrum.cc
    src/karma/state.cc
    src/karma/track.cc
    src/karma/model.cc
    src/karma/jacobian.cc
    src/formant/formant.h
    src/formant/track.cc
    src/formant/lpc.h
    src/formant/roots.h
    src/formant/bairstow.h
    src/formant/bairstow.cc
    src/formant/cauchy.h
    src/formant/resample.h
    src/formant/viterbi.h
    src/formant/viterbi.cc
    src/reaper/reaper.h
    src/reaper/compute.h
    src/reaper/compute.cc
    src/reaper/core/file_resource.h
    src/reaper/core/file_resource.cc
    src/reaper/core/float_matrix-inl.h
    src/reaper/core/float_matrix.h
    src/reaper/core/float_matrix.cc
    src/reaper/core/track.h
    src/reaper/core/track.cc
    src/reaper/epoch_tracker/epoch_tracker.h
    src/reaper/epoch_tracker/epoch_tracker.cc 
    src/reaper/epoch_tracker/fd_filter.h
    src/reaper/epoch_tracker/fd_filter.cc
    src/reaper/epoch_tracker/fft.h
    src/reaper/epoch_tracker/fft.cc
    src/reaper/epoch_tracker/lpc_analyzer.h
    src/reaper/epoch_tracker/lpc_analyzer.cc
)

add_executable(offline_analysis WIN32 ${SOURCES})
add_dependencies(offline_analysis eigen3)

include_directories(
    src/
    ${EIGEN_INCLUDE_DIR}
    ${PORTAUDIO_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${FFTW_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

target_link_libraries(offline_analysis PUBLIC
    ${PORTAUDIO_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${SWRESAMPLE_LIBRARIES}
    ${FFTW_LIBRARIES}
    Qt5::Widgets
    ${BLAS_LIBRARIES}
)

if (UNIX AND NOT APPLE)
    target_link_libraries(offline_analysis PUBLIC pthread m dl)
endif()

if (WIN32)
add_custom_command (TARGET offline_analysis POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:offline_analysis>)
endif()

