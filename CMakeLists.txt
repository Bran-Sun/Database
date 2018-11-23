cmake_minimum_required(VERSION 3.6)
project(Database)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/bufmanager/BufPageManager.h src/bufmanager/FindReplace.h src/fileio/FileManager.h src/fileio/FileTable.h src/utils/compare.h src/utils/MyBitMap.h src/utils/MyHashMap.h src/utils/MyLinkList.h src/utils/pagedef.h src/recordManager/PageHeaderFile.h src/recordManager/RID.cpp src/recordManager/RID.h
        src/recordManager/RM_FileHandle.h src/recordManager/RM_FileHandle.cpp src/recordManager/RM_Manager.h src/recordManager/RM_Manager.cpp src/recordManager/RM_Record.h src/recordManager/RM_Record.cpp src/recordManager/RM_Iterator.h src/recordManager/RM_Iterator.cpp src/ixmanager/IX_Manager.h src/ixmanager/IX_Manager.cpp
        src/ixmanager/IX_IndexHandle.cpp src/ixmanager/IX_IndexHandle.h src/ixmanager/IX_HeaderPage.h src/ixmanager/BpNode.h src/ixmanager/BpNode.cpp)
add_executable(Database ${SOURCE_FILES})