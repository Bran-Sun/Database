cmake_minimum_required(VERSION 3.6)
project(Database)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -mtune=native -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -mtune=native -fomit-frame-pointer")

set(SOURCE_FILES src/main.cpp src/bufmanager/BufPageManager.h src/bufmanager/FindReplace.h src/fileio/FileManager.h src/fileio/FileTable.h src/utils/compare.h src/utils/MyBitMap.h src/utils/MyHashMap.h src/utils/MyLinkList.h src/utils/pagedef.h src/recordManager/PageHeaderFile.h src/recordManager/RID.cpp src/recordManager/RID.h
        src/recordManager/RM_FileHandle.h src/recordManager/RM_FileHandle.cpp src/recordManager/RM_Manager.h src/recordManager/RM_Manager.cpp src/recordManager/RM_Record.h src/recordManager/RM_Record.cpp src/recordManager/RM_Iterator.h src/recordManager/RM_Iterator.cpp src/ixmanager/IX_Manager.h src/ixmanager/IX_Manager.cpp
        src/ixmanager/IX_IndexHandle.cpp src/ixmanager/IX_IndexHandle.h src/ixmanager/IX_HeaderPage.h src/ixmanager/BpNode.h src/ixmanager/BpNode.cpp src/utils/utils.cpp src/ixmanager/IX_IndexScan.h src/ixmanager/IX_IndexScan.cpp src/smmanager/SM_Manager.cpp src/smmanager/SM_Manager.h src/smmanager/Printer.h src/smmanager/Printer.cpp
        src/smmanager/parser.h src/smmanager/DatabaseHandle.h src/smmanager/DatabaseHandle.cpp src/smmanager/TableHandle.cpp src/smmanager/TableHandle.h)


add_executable(Database ${SOURCE_FILES})