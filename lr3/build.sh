#!/bin/bash

rm CMakeCache.txt packer psnr_tool Makefile unpacker cmake_install.cmake 
rm -rf CMakeFiles                                                        
cmake CMakeLists.txt
make