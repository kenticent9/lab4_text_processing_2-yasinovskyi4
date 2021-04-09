cmake_minimum_required(VERSION 3.17)
project(process_text)

set(CMAKE_CXX_STANDARD 14)

find_package( ICU REQUIRED COMPONENTS uc i18n )
# Повний список ICU components = data, i18n, io, le, lx,
#                                       test, tu and uc.

set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
find_package( Boost REQUIRED COMPONENTS
        filesystem locale system
        )

find_package(LibArchive REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} ${LibArchive_INCLUDE_DIRS} )

add_executable(process_text src/main.cpp src/conf_reading.cpp src/conf_reading.h src/ctr_writing.cpp src/ctr_writing.h src/LibArchiveArchive.cpp src/LibArchiveArchive.h src/time_measurement.h)
target_link_libraries(process_text ${Boost_LIBRARIES} ${ICU_LIBRARIES}
        ${LibArchive_LIBRARIES})