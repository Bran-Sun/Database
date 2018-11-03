cmake_minimum_required(VERSION 3.6)
project(Database)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/bufmanager/BufPageManager.h src/bufmanager/FindReplace.h src/fileio/FileManager.h src/fileio/FileTable.h src/utils/compare.h src/utils/MyBitMap.h src/utils/MyHashMap.h src/utils/MyLinkList.h src/utils/pagedef.h)
add_executable(Database ${SOURCE_FILES})