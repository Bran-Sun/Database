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

