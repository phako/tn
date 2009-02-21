include(FindPkgConfig)
add_definitions(-Wall -Werror)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libswscale)
set(foo ${FFMPEG_libavformat_INCLUDEDIR}/libavformat)
MESSAGE(STATUS ${foo})
find_path(AVFORMAT_H avformat.h ${foo})
if (AVFORMAT_H)
    ADD_DEFINITIONS(-DAVCODEC_NEW_INCLUDE)
endif (AVFORMAT_H)

# check for libjpeg
find_package(JPEG)
if (JPEG_FOUND)
    ADD_DEFINITIONS(-DHAVE_JPEG)
endif (JPEG_FOUND)

pkg_check_modules(CAIRO cairo cairo-png)
add_executable(tn tn.c video.c histogram.c image.c util.c)
if (CAIRO_FOUND)
    ADD_DEFINITIONS(-DHAVE_CAIRO)
endif (CAIRO_FOUND)
target_link_libraries(tn 
    ${FFMPEG_LIBRARIES} 
    ${JPEG_LIBRARIES}
    ${CAIRO_LIBRARIES})
include_directories(${FFMPEG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS})
