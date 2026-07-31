# L1 Buffer、L0A Buffer/L0B Buffer内存结构介绍<a name="ZH-CN_TOPIC_0000002538231122"></a>

为提升数据访问效率和吞吐量，Buffer采用了等容量的内存模块（Bank）结构设计。内存空间被划分为多个Bank，并以Bank为单位进一步分组形成多个Bank Group（BG）；每个Bank Group包含若干Bank，每个Bank都是一个可独立操作的存储单元子模块。

下面将介绍Cube侧相关内存（L1 Buffer、L0A Buffer、L0B Buffer）的详细结构。

## L1 Buffer内存结构<a name="zh-cn_topic_0000002545385271_section133445171205"></a>

L1 Buffer的总容量为512K字节，由16个Bank组成。每个Bank的容量为32K字节，由1024行组成，每行的宽度为32字节。**同一Bank最多同时允许一读或一写**。

这16个Bank被进一步组织为8个Bank Group。每个Bank Group包含2个Bank（Bank0和Bank1）。**同属于一个Bank Group的两个Bank支持一个读、另一个写，但不支持同时读或同时写**。

L1 Buffer的内存结构如[图1](#zh-cn_topic_0000002545385271_fig5383229131020)所示，地址编码格式如下：

```cpp
L1_ADDR[18:0] = {BANK[0:0], BANK_DEPTH[9:0], BG[2:0], BANK_WIDTH[4:0]}    // bit位顺序为从高位到低位，共19位
// BANK表示地址所在Bank的编号，占用1位bit位，取值范围为[0, 1]
// BANK_DEPTH表示地址在Bank中的行数，占用10位bit位，取值范围为[0, 1023]
// BG表示地址所在Bank Group的编号，占用3位bit位，取值范围为[0, 7]
// BANK_WIDTH表示地址在Bank一行上的偏移量，占用5位bit位，取值范围为[0, 31]
```

**图1** L1 Buffer内存结构图<a name="zh-cn_topic_0000002545385271_fig5383229131020"></a>  
![](../../../../figures/l1_memory_bank.png "L1内存结构图")

## L0A Buffer/L0B Buffer内存结构<a name="zh-cn_topic_0000002545385271_section6770135142516"></a>

L0A Buffer/L0B Buffer的总容量为64K字节，只包含1个Bank，有128行，每行的宽度为512字节。**不允许同时读写同一个地址，允许LoadData同时写入和Mmad同时读取同一Bank的不同地址。**

L0A Buffer/L0B Buffer的内存结构如[图2](#zh-cn_topic_0000002545385271_fig2046019020496)所示，L0A Buffer和L0B Buffer采用相同的地址编码格式：

```cpp
L0A_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }    // bit位顺序为从高位到低位，共16位
L0B_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }
// BANK_DEPTH表示地址在Bank中的行数，占用7位bit位，取值范围为[0, 127]
// BANK_WIDTH表示地址在Bank一行上的偏移量，占用9位bit位，取值范围为[0, 511]
```

**图2** L0A Buffer/L0B Buffer内存结构图<a name="zh-cn_topic_0000002545385271_fig2046019020496"></a>  
![](../../../../figures/l0al0b_memory_bank.png "L0A-B内存结构图")
