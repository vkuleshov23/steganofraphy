cmake_minimum_required(VERSION 3.18)
project(lr LANGUAGES CXX)

set(LR_NUMBER 3)

include_directories(${CMAKE_SOURCE_DIR}/libs/ ${CMAKE_SOURCE_DIR})

list(APPEND source 
    src/Settings.cpp
    src/BMPContainer.cpp
    libs/EasyBmp/EasyBMP.cpp
    src/Message.cpp
    src/main.cpp)

if(${LR_NUMBER} EQUAL 3)
    list(INSERT source 3 src/BMPContainerExtended.cpp)
endif()

add_executable(packer ${source})
add_executable(unpacker ${source})

target_compile_definitions(packer PRIVATE IS_PACKER PROGNAME="packer")
target_compile_definitions(unpacker PRIVATE PROGNAME="unpacker")

target_compile_definitions(packer PRIVATE ARGC=3 LABNUM=${LR_NUMBER})
if(${LR_NUMBER} EQUAL 2)
    target_compile_definitions(unpacker PRIVATE ARGC=2 LABNUM=${LR_NUMBER})
else(${LR_NUMBER} EQUAL 3)
    target_compile_definitions(unpacker PRIVATE ARGC=3 LABNUM=${LR_NUMBER})
endif()


add_executable(psnr_tool src/PsnrTool.cpp libs/EasyBmp/EasyBMP.cpp)
