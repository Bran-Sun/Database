# Database
关系数据库模型



## 运行方式

根目录下

```
mkdir build && cd build
cmake ..
make
./Database
```



## 记录管理模块

参照了stanford cs346课程的内容，依次实现了RM_Manager, RM_FileHandle, RM_Iterator, RM_Record等类，基本实现了往页式文件系统中写记录，读取记录，修改记录等功能。

* RM_Manger负责调用fileio和bufmanager中的接口，创建文件，控制读写。
* RM_FileHandle负责管理一个文件中的读写记录等操作。
* RM_Record负责记录每条记录在文件中的位置和内容。
* RM_Iterator负责从文件中读取符合条件的记录。
* 每个文件的第一页结构如PageHeaderFile中所示。

## 索引管理模块

同样依照CS346课程的内容，实现了IX_Manager, IX_indexHandle, BpNode等类。通过B+树来管理索引和RID。
目前索引的插入、删除已经完成，但索引的删除还只是懒惰删除(删除时没有考虑再平衡)。

* IX_Manager 负责打开，关闭，创建，删除索引文件。
* IX_IndexManager 负责管理B+树，并向其中插入索引，删除索引。
* IX_Iterator 负责读取相关内容
* 每个索引文件的第一页如IX_HeaderPage所示

**注：因为大量使用new，会导致内存碎片化，需要进行优化**

已经进行了内存上的优化，使用了stl标准库来管理内存，防止内存的碎片化，但是降低了一点速度。

## 系统管理模块

根据本课程的要求，定义了一下几个类，来实现系统管理的内容

* SM_Manager负责打开数据库，创建数据库等数据库层面上的操作。
* DatabaseHandle负责创建Table等数据表层面上的操作。
* TableHandle负责创建索引等表内部的操作，之后也会在其中实现查询等操作。

**注：为每个database创建一个独立的文件夹和管理文件(以dbf为后缀)。可以在一个database内对多张表进行操作。**

## 查询解析模块

能够实现基本的创建、删除数据库和数据表的命令，以及插入、修改、删除记录的命令。

* SystemManager模块为最外层的模块，Action对象直接作用在这个模块上。
* 目前支持NULL等值的赋值，以及索引中出现重复值的情况。

**注：目前的select，insert，update等模块都是循环遍历来查找，效率还比较低，而且不支持多表查询，所以还需要对此进行优化**

## 命令解析模块

根据助教发的parser.txt，手写了lexer和parser，能够解析所有要求的命令。

* Lexer.h和Lexer.cpp 负责进行词法分析。
* Parser.h和Parser.cpp 负责进行语法分析，同时生成不同的Action对象。使用的是LL(1)文法。

注：
1. 添加了词法分析中的FLOAT等缺少的内容
2. 将原本的语法修改成LL(1)文法进行实现。为了简便，在部分产生式的实现过程中用到了LL(2)文法。
3. 生成的Action对象都是Aciton类派生出的不同子类，能够对数据库系统进行不同的操作。