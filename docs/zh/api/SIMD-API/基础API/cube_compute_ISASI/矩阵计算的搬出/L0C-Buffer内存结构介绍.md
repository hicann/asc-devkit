# L0C Buffer内存结构介绍<a id="ZH-CN_TOPIC_0000002538071268"></a>

为提升数据访问效率和吞吐量，L0C Buffer采用了等容量的内存模块（Bank）结构设计。内存空间被划分为多个Bank，并以Bank为单位进一步分组形成多个Bank Group（BG）；每个Bank Group包含若干Bank，每个Bank都是一个可独立操作的存储单元子模块。

<!-- npu="A3,910b" id2 -->
以如下产品型号为例：
<!-- npu="A3" id3 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id3 -->
<!-- npu="910b" id4 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id4 -->
L0C Buffer的总容量为128K字节，由16个Bank组成。每个Bank的容量为8K字节，由128行组成，每行的宽度为64字节。这16个Bank被进一步组织为16个Bank Group。每个Bank Group包含1个Bank。

L0C Buffer的内存结构如[图1](#zh-cn_topic_0000002555148561_fig1715315610160)所示，L0C Buffer的地址编码格式及其bit位对应的含义说明如下：

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // bit位顺序为从高位到低位，共17位
// BANK_DEPTH表示地址在Bank中的行数，占用7位bit位，取值范围为[0, 127]
// BANK表示地址所在Bank的编号，占用4位bit位，取值范围为[0, 15]
// BANK_WIDTH表示地址在Bank一行上的偏移量，占用6位bit位，取值范围为[0, 63]
```

**图1** L0C Buffer内存结构图（[NPU架构版本2201](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh-cn_topic_0000002555148561_fig1715315610160"></a>  

![](../../../../figures/L0C_Memory_Structure.png)
<!-- end id2 -->

<!-- npu="950" id1 -->
以Ascend 950PR/Ascend 950DT为例，L0C Buffer总容量为256K字节，由16个Bank组成。每个Bank的容量为16K字节，由256行组成，每行的宽度为64字节。这16个Bank被进一步组织为16个Bank Group。每个 Bank Group包含1个Bank。

L0C Buffer的内存结构如[图2](#fig179810301543)所示，L0C Buffer的地址编码格式及其bit位对应的含义说明如下：

```text
L0C_ADDR[17:0] = { BANK_DEPTH[7:0], BANK[3:0], BANK_WIDTH[5:0] }    // bit位顺序为从高位到低位，共18位
// BANK_DEPTH表示地址在Bank中的行数，占用8位bit位，取值范围为[0, 255]
// BANK表示地址所在Bank的编号，占用4位bit位，取值范围为[0, 15]
// BANK_WIDTH表示地址在Bank一行上的偏移量，占用6位bit位，取值范围为[0, 63]
```

**图2** L0C Buffer内存结构图（[NPU架构版本3510](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="fig179810301543"></a>  

![](../../../../figures/L0C_Memory_Structure_950.png)
<!-- end id1 -->
