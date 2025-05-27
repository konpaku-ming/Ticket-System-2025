# Konpaku_ming's Ticket_System-2025

这里是youming的大作业Ticket_System，作业要求见[管理系统文档](https://github.com/konpaku-ming/Ticket-System-2025/blob/main/management_system.md)
希望阅读该文档能帮助你理解本仓库代码<br>

## 仓库结构

#### include/

- BPlusTree.h
- account.h
- accountDatabase.h
- date.h
- exceptions.h
- map.h
- order.h
- priority_queue.h
- ticket.h
- time.h
- tokenscanner.h
- tools.h
- train.h
- trainDatabase.h
- utility.h
- vector.h

#### src/

- BPlusTree.cpp
- account.cpp
- date.cpp
- main.cpp
- order.cpp
- time.cpp
- tokenscanner.cpp
- train.cpp

**CMakeLists.txt**
**README.md**
**management_system.md**

## 文件概述

utility.h, exceptions.h, vector.h, priority_queue.h, map.h实现了与STL接近的vector, priority_queue, map的功能

BPlusTree.h与BPlusTree.cpp实现了一个外存中的B+树，允许将一个string类型的key和一个int类型的value作为键值对储存，并支持查找和删除功能

date.h, date.cpp, time.h, time.cpp实现了日期、时刻类，支持日期和时刻与整型的简单加减，实现了日期、时刻类的输出流运算符重载

account.h, account.cpp实现了用户类，accountDatabase实现了一个用户管理类，支持添加用户、登录、修改用户信息等操作

train.h, train.cpp实现了火车类和余票信息类，order.h, order.cpp实现了订单类与候补订单类，ticket类实现了直通票类和转乘票类

trainDatabase实现了一个火车票管理类，支持添加火车、发布火车、查询车票、买票退票等功能

tools.h中有一些小工具，如字符串转整数、字符串分割等

tokenscanner.h和tokenscanner.cpp实现了一个可以将命令划分并存储各参数的类，便于指令与accountDatabase和trainDatabase的交流

## 数据库设计

##### 核心理念：

用MemoryRiver储存信息，将信息在文件中的索引与某个key绑定存入B+树。查询时通过B+树先查到索引，再从文件索引位置读出信息。

##### accountDatabase:

`//key为username，value为索引的B+树`
`BPT account_bpt_{"account_bpt"}; `

`//存放用户信息，每个Account在文件中的位置与其username作为键值对插入到BPT中`
`MemoryRiver<Account, 0> account_file_; `

##### trainDatabase:

`//key为trainID，value为用户索引的B+树`
`BPT train_bpt_{"train_bpt_"}; `

`//存放火车信息，每个Train在文件中的位置与其trainID作为键值对插入到BPT中`
`MemoryRiver<Train, 0> train_file_; `

`//key为station，value为列车索引的B+树`
`BPT station_bpt_{"station_bpt_"}; //记录经过station的车的索引`

`//Train类里面有一个ticket_idx是其余票在文件中的位置 `
`MemoryRiver<Ticket, 0> ticket_file_; //存放余票信息`

`//key为username，value为订单索引的B+树`
`BPT order_bpt_{"order_bpt_"};`

`//存放订单信息，每个Order在文件中的位置与其username作为键值对插入到BPT中`
`MemoryRiver<Order, 0> order_file_; `

`//key为trainID+发售天数，value为候补订单索引的B+树`
`BPT pending_bpt_{"pending_bpt_"};`

`//存放候补订单信息，每个PendingOrder在文件中的位置与其购买的车次+天数作为键值对插入到BPT中`
`MemoryRiver<PendingOrder, 0> pending_file_; `


