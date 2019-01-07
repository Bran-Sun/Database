//
// Created by 孙桢波 on 2018/11/1.
//

/*
 * testfilesystem.cpp
 *
 *  Created on: 2015年10月6日
 *      Author: lql
 *          QQ: 896849432
 * 各位同学十分抱歉，之前给的样例程序不支持同时打开多个文件
 * 是因为初始化没有做，现在的程序加上了初始化（main函数中的第一行）
 * 已经可以支持多个文件的维护
 *
 * 但是还是建议大家只维护一个文件，因为首先没有必要，我们可以把数据库中
 * 的索引和数据都放在同一个文件中，当我们开启一个数据库时，就关掉上一个
 * 其次，多个文件就要对应多个fileID，在BufPageManager中是利用一个hash函数
 * 将(fileID,pageID)映射为一个整数，但由于我设计的hash函数过于简单，就是fileID和
 * pageID的和，所以不同文件的页很有可能映射为同一个数，增加了hash的碰撞率，影响效率
 *
 * 还有非常重要的一点，BufType b = bpm->allocPage(...)
 * 在利用上述allocPage函数或者getPage函数获得指向申请缓存的指针后，
 * 不要自行进行类似的delete[] b操作，内存的申请和释放都在BufPageManager中做好
 * 如果自行进行类似free(b)或者delete[] b的操作，可能会导致严重错误
 */
#include "recordmanager/RM_Manager.h"
#include "ixmanager/IX_IndexHandle.h"
#include "ixmanager/IX_Manager.h"
#include "smmanager/SM_Manager.h"
#include "smmanager/DatabaseHandle.h"
#include <iostream>

using namespace std;

unsigned char MyBitMap::h[61]; //defination

int main() {
    MyBitMap::initConst();   //新加的初始化
    std::shared_ptr<FileManager> fm = std::make_shared<FileManager>(FileManager());
    std::shared_ptr<BufPageManager> bpm = std::make_shared<BufPageManager>(BufPageManager(fm));
    
    fm->createFile("testfile.txt"); //新建文件
    fm->createFile("testfile2.txt");
    int fileID, f2;
    fm->openFile("testfile.txt", fileID); //打开文件，fileID是返回的文件id
    fm->openFile("testfile2.txt", f2);
    
    printf("fid: %d\n", fileID);
    printf("fid2: %d\n", f2);
    
    /*
    for (int pageID = 0; pageID < 1000; ++ pageID) {
        int index;
        //为pageID获取一个缓存页
        BufType b = bpm->allocPage(fileID, pageID, index, false);
        //注意，在allocPage或者getPage后，千万不要进行delete[] b这样的操作
        //内存的分配和管理都在BufPageManager中做好，不需要关心，如果自行释放会导致问题
        b[0] = pageID; //对缓存页进行写操作
        b[1] = fileID;
        bpm->markDirty(index); //标记脏页
        //在重新调用allocPage获取另一个页的数据时并没有将原先b指向的内存释放掉
        //因为内存管理都在BufPageManager中做好了
        b = bpm->allocPage(f2, pageID, index, false);
        b[0] = pageID;
        b[1] = f2;
        bpm->markDirty(index);
    }
    for (int pageID = 0; pageID < 1000; ++ pageID) {
        int index;
        //为pageID获取一个缓存页
        BufType b = bpm->getPage(fileID, pageID, index);
        //注意，在allocPage或者getPage后，千万不要进行delete[] b这样的操作
        //内存的分配和管理都在BufPageManager中做好，不需要关心，如果自行释放会导致问题
        cout << b[0] << ":" << b[1] << endl; 		//读取缓存页中第一个整数
        bpm->access(index); //标记访问
        b = bpm->getPage(f2, pageID, index);
        cout << b[0] << ":" << b[1] << endl;
        bpm->access(index);
    }
    bpm->close();
    //程序结束前可以调用BufPageManager的某个函数将缓存中的内容写回
    //具体的函数大家可以看看ppt或者程序的注释
    */
    
    /*
    clock_t start, end;
    start = clock();
    
    std::shared_ptr<RM_Manager> rm_manager = std::make_shared<RM_Manager>(RM_Manager(fm, bpm));
    std::string tableName = "Student";
    
    std::vector<AttrInfo> infoIn;
    infoIn.emplace_back("name", 50, STRING);
    infoIn.emplace_back("age", 4, INT);
    
    rm_manager->createFile(tableName, infoIn);
    RM_FileHandle handle;
    rm_manager->openFile(tableName, handle);
    
    for (int k = 0; k < 1000000; k++)
    {
        std::vector<char> arr;
        for ( int i = 0; i < 58; i++ )
            arr.push_back('a');
        char *data = arr.data();
        RID rid(0, 0);
        handle.insertRecord(data, rid);
    }
    
    rm_manager->closeFile(handle);
    */
    
    /*
    clock_t start, end;
    start = clock();
    
    std::shared_ptr<IX_Manager> manager = std::make_shared<IX_Manager>(IX_Manager(fm, bpm));
    manager->createIndex("Student", 0, INT, 4);
    
    IX_IndexHandle handle;
    manager->openIndex("Student", 0, handle);
    
    int number = 100000;
    int begin = 1;
    int *array = new int[number];
    for (int i = 0; i < number; i++)
        array[i] = i + begin;
    for (int i = 0; i < number; i++)
        handle.insertEntry((void*)(array + i), RID(i + begin, i + begin));
    
//    int *p = new int;
//    *p = 60;
//    handle.deleteEntry((void*)p, RID(60, 60));
    manager->closeIndex(handle);
    
    
    clock_t start, end;
    start = clock();
    
    SM_Manager manager;
    DatabaseHandle handle;
    std::string dbName = "School";
    manager.openDb(dbName, handle);
    
    std::string tableName = "Student";
    std::string indexName = "name";
    
    std::vector<AttrInfo> infoIn;
    infoIn.emplace_back("name", 50, STRING);
    infoIn.emplace_back("age", 4, INT);
    
    handle.createIndex(tableName, indexName);
    
    manager.closeDb(handle);
    
    end = clock();
    cout<<"Run time: "<<(double)(end - start) / CLOCKS_PER_SEC<<"S"<<endl;*/
    return 0;
}
