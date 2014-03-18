# ---CMakeLists.txt---
# Project: PVController
# Author:  Travis Bueter

cmake_minimum_required (VERSION 2.6)

project (PVController)

add_executable (PVController PVC_Main.cpp) #PVC_Xbox.cpp

INCLUDE_DIRECTORIES (/usr/local/include/wiic #comment out line for xbox
	/usr/local/Trolltech/Qt-4.8.3/include
	/usr/include/json)

target_link_libraries (PVController -lwiicpp) #comment out line for xbox
target_link_libraries (PVController json)
target_link_libraries (PVController -lpthread)
