# DataCopy（L1ToGM高维切分数据搬运）<a name="ZH-CN_TOPIC_0000002594407261"></a>

## 产品支持情况<a name="zh-cn_topic_0000002540558032_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000002540558032_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口主要实现将矩阵从L1 Buffer搬运至Global Memory，数据搬运时格式和内容保持不变。

高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。

## 函数原型<a name="zh-cn_topic_0000002540558032_section82039854412"></a>

接口同时支持非连续搬运和连续搬运：

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

对于连续搬运场景，推荐使用[DataCopy（L1ToGM连续数据搬运）](DataCopy（L1ToGM连续数据搬运）.md)。

## 参数说明<a name="zh-cn_topic_0000002540558032_section16128134420472"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002540558032_section4219135304818)。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为GlobalTensor，存储位置为Global Memory，地址需要1字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，存储位置为L1 Buffer，地址需要32字节对齐。 |
| repeatParams | 输入 | 搬运参数，类型为DataCopyParams。通过该参数可以配置搬运的数据块个数、长度、地址间隔等信息，同时支持非连续和连续搬运。<br>DataCopyParams参数说明请参考[表3](#zh-cn_topic_0000002540558032_table144203616291)。 |

**表3** DataCopyParams结构体参数定义<a id="zh-cn_topic_0000002540558032_table144203616291"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| blockCount | 搬运的数据块个数。uint16_t类型，取值范围为blockCount∈[0, 4095]，默认值为1。<br>**注：blockCount = 0表示不执行搬运，该接口将被视为NOP（空操作）**。 |
| blockLen | 搬运的每个数据块长度。uint16_t类型，取值范围为blockLen∈[0, 65535]，单位：DataBlock（32字节）。<br>**注：blockLen = 0表示不执行搬运，该接口将被视为NOP（空操作）**。 |
| srcGap | 源操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），uint16_t类型，取值范围为srcGap∈[0, 65535]，单位为DataBlock（32字节）。<br>&bull; blockCount = 1时，srcGap无意义，设置为0即可。 |
| dstGap | 目的操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），uint16_t类型，取值范围为dstGap∈[0, 65535]，单位为DataBlock（32字节）。<br>&bull; blockCount = 1时，dstGap无意义，设置为0即可。 |

下图呈现了DataCopyParams结构体参数的使用方法，样例中完成了2个连续传输数据块的搬运，每个数据块含有8个DataBlock，源操作数相邻数据块之间无间隔，目的操作数相邻数据块尾与头之间间隔1个DataBlock。

![](../../../../figures/repeat-times.png)

## 数据类型<a id="zh-cn_topic_0000002540558032_section4219135304818"></a>

源矩阵和目的矩阵支持的数据类型保持一致。

<!-- npu="A3" id10 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id10 -->

<!-- npu="910b" id11 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id11 -->

<!-- npu="x90" id12 -->
针对Kirin X90，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id12 -->

<!-- npu="9030" id13 -->
Kirin 9030，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id13 -->

## 返回值说明

无

## 约束说明<a name="zh-cn_topic_0000002540558032_section2045914466492"></a>

- 位于L1 Buffer的源地址必须32字节对齐，位于Global Memory的目的地址必须1字节对齐。
- 当DataCopyParams结构体参数blockCount、blockLen任意一个值为0时，该指令不会执行。
<!-- npu="A3,910b" id14 -->
- 针对如下产品型号：
    <!-- npu="A3" id15 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id16 -->
    在跨卡通信算子开发场景，DataCopy类接口支持跨卡数据搬运，仅支持HCCS物理链路，不支持其他通路；开发者开发过程中，需要关注涉及卡间通信的物理通路，可通过npu-smi info -t topo命令查询HCCS物理链路。
<!-- end id14 -->
- DataCopyParams结构体参数的值需在取值范围内：

    **表4** DataCopyParams结构体参数取值范围

    | 参数名称 | 取值范围 |
    | ---------- | ---------- |
    | blockCount | [1, 4095] |
    | blockLen | [1, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

## 关键特性说明<a name="zh-cn_topic_0000002540558032_section1084342081618"></a>

**连续搬运**

针对连续搬运场景，推荐使用[DataCopy（L1ToGM连续数据搬运）](DataCopy（L1ToGM连续数据搬运）.md#ZH-CN_TOPIC_0000002563687928)，以下示例仅展示高维切分数据搬运接口具有连续搬运能力。

以half数据类型为例，源操作数的shape为1 \* 128。

[图1](#zh-cn_topic_0000002540558032_fig187115784817)将源操作数中128个元素连续搬运至目的操作数，DataCopyParams结构体参数配置如下：

- blockCount = 1，搬运1个数据块，表示连续搬运。
- blockLen = 8，一个数据块长度为8个DataBlock。
- srcGap = 0，源操作数相邻数据块之间的间隔为0。
- dstGap = 0，目的操作数相邻数据块之间的间隔为0。

**图1** 连续搬运示意图<a id="zh-cn_topic_0000002540558032_fig187115784817"></a>  

![](../../../../figures/continuous_data_copy_diagram.png)

**非连续搬运**

以half数据类型为例，源操作数的shape为1 \* 112。

[图2](#zh-cn_topic_0000002540558032_fig123821924175312)将源操作数中非连续数据搬运至目的操作数，DataCopyParams结构体参数配置如下：

- blockCount= 2，搬运2个数据块。
- blockLen= 3，一个数据块的长度为3个DataBlock。
- srcGap= 1，源操作数相邻数据块之间的间隔为1。
- dstGap= 2，目的操作数相邻数据块之间的间隔为2。

**图2** 非连续搬运示意图<a id="zh-cn_topic_0000002540558032_fig123821924175312"></a>  

![](../../../../figures/discontinuous_data_copy_diagram.png)

## 调用示例<a name="zh-cn_topic_0000002540558032_section088124295117"></a>

以[图2](#zh-cn_topic_0000002540558032_fig123821924175312)所示场景为例，展示非连续搬运时的调用示例：

```cpp
constexpr uint32_t srcElemCount = 112;
constexpr uint32_t dstElemCount = 128;

// 源操作数：L1 Buffer，按“48个有效half + 16个跳过half + 48个有效half”排布。
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::A1, a1Addr, srcElemCount);

// 目的操作数：GM上预留128个half，两个有效数据块之间保留32个half间隔。
AscendC::GlobalTensor<half> dstGm;
dstGm.SetGlobalBuffer((__gm__ half *)dst, dstElemCount);

AscendC::DataCopyParams repeatParams;
// 搬运2个连续传输数据块。
repeatParams.blockCount = 2;
// 每个数据块长度为3个DataBlock。
repeatParams.blockLen = 3;
// 源端相邻数据块之间跳过1个DataBlock。
repeatParams.srcGap = 1;
// 目的端相邻数据块之间间隔2个DataBlock。
repeatParams.dstGap = 2;
AscendC::DataCopy(dstGm, srcLocal, repeatParams);
```
