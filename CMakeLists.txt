cmake_minimum_required(VERSION 3.6)
project(Database)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -mtune=native -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -mtune=native -fomit-frame-pointer")

set(SOURCE_FILES src/bufmanager/BufPageManager.h src/bufmanager/FindReplace.h src/fileio/FileManager.h src/fileio/FileTable.h src/utils/compare.h src/utils/MyBitMap.h src/utils/MyHashMap.h src/utils/MyLinkList.h src/utils/pagedef.h src/recordManager/PageHeaderFile.h src/recordManager/RID.cpp src/recordManager/RID.h
        src/recordManager/RM_FileHandle.h src/recordManager/RM_FileHandle.cpp src/recordManager/RM_Manager.h src/recordManager/RM_Manager.cpp src/recordManager/RM_Record.h src/recordManager/RM_Record.cpp src/recordManager/RM_Iterator.h src/recordManager/RM_Iterator.cpp src/ixmanager/IX_Manager.h src/ixmanager/IX_Manager.cpp
        src/ixmanager/IX_IndexHandle.cpp src/ixmanager/IX_IndexHandle.h src/ixmanager/IX_HeaderPage.h src/ixmanager/BpNode.h src/ixmanager/BpNode.cpp src/utils/utils.cpp src/ixmanager/IX_IndexScan.h src/ixmanager/IX_IndexScan.cpp src/smmanager/SM_Manager.cpp src/smmanager/SM_Manager.h src/smmanager/Printer.h src/smmanager/Printer.cpp
        src/smmanager/DataInfo.h src/smmanager/DatabaseHandle.h src/smmanager/DatabaseHandle.cpp src/smmanager/TableHandle.cpp src/smmanager/TableHandle.h src/qlmanager/SystemManager.cpp src/qlmanager/SystemManager.h)
set(PARSE_FILES src/parser/Lexer.h src/parser/Lexer.cpp src/parser/Parser.h src/parser/Parser.cpp src/Action/Action.h
        src/Action/Action.h src/Action/ShowAction.h src/smmanager/DataInfo.h)
add_executable(Database ${SOURCE_FILES}  src/main.cpp)

add_executable(Parser src/parser/Lexer.h src/parser/Lexer.cpp src/parser/Parser.h src/parser/Parser.cpp src/Action/Action.h
        src/Action/Action.h src/Action/ShowAction.h src/Action/CreateAction.h src/Action/DropAction.h src/smmanager/DataInfo.h
        src/Action/UseAction.h src/Action/TableAction.h src/parser/main.cpp)
