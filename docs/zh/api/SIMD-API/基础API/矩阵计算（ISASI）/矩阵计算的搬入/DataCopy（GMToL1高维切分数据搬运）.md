# DataCopy（GMToL1高维切分数据搬运）<a id="ZH-CN_TOPIC_0000002538231128"></a>

## 产品支持情况<a id="zh-cn_topic_0000002566658905_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a id="zh-cn_topic_0000002566658905_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口主要实现将矩阵从Global Memory搬运至L1 Buffer（TPosition为A1/B1），数据搬运时格式和内容保持不变。

高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。

## 函数原型<a id="zh-cn_topic_0000002566658905_section82039854412"></a>

接口同时支持非连续搬运和连续搬运：

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
```

注：对于连续搬运场景，推荐使用[DataCopy（GMToL1连续数据搬运）](DataCopy（GMToL1连续数据搬运）.md)。

## 参数说明<a id="zh-cn_topic_0000002566658905_section16128134420472"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002566658905_section4219135304818)。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，存储位置为L1 Buffer（TPosition为A1/B1），目的地址需要32字节对齐。 |
| src | 输入 | 源操作数，类型为GlobalTensor，存储位置为Global Memory，源地址需要1字节对齐。 |
| repeatParams | 输入 | 搬运参数，类型为DataCopyParams。通过该参数可以配置搬运的数据块个数、长度、地址间隔等信息，同时支持非连续和连续搬运。<br>DataCopyParams参数说明请参考[表3](#zh-cn_topic_0000002566658905_table144203616291)。 |

**表3** DataCopyParams结构体参数定义<a id="zh-cn_topic_0000002566658905_table144203616291"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| blockCount | 搬运的数据块个数。uint16_t类型，取值范围：blockCount∈[0, 4095]，默认值为1。<br>**注：blockCount = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| blockLen | 搬运的每个数据块长度。uint16_t类型，取值范围：blockLen∈[0, 65535]，单位：DataBlock（32字节）。<br>**注：blockLen = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcGap | 源操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），uint16_t类型，取值范围：srcGap∈[0, 65535]，单位：DataBlock（32字节）。<br>&nbsp;&nbsp;&bull; blockCount = 1时，srcGap无意义，设置为0即可。 |
| dstGap | 目的操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），uint16_t类型，取值范围：dstGap∈[0, 65535]，单位：DataBlock（32字节）。<br>&nbsp;&nbsp;&bull; blockCount = 1时，dstGap无意义，设置为0即可。 |

## 数据类型<a id="zh-cn_topic_0000002566658905_section4219135304818"></a>

源矩阵和目的矩阵支持的数据类型保持一致。

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，支持数据类型为：b8、b16、b32、b64。
<!-- end id10 -->

<!-- npu="A3" id11 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：b8、b16、b32、b64。
<!-- end id11 -->

<!-- npu="910b" id12 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：b8、b16、b32、b64。
<!-- end id12 -->

<!-- npu="310p" id13 -->
针对Atlas 推理系列产品AI Core，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id13 -->

<!-- npu="910" id14 -->
针对Atlas 训练系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id14 -->

<!-- npu="x90" id15 -->
针对Kirin X90，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id15 -->

<!-- npu="9030" id16 -->
针对Kirin 9030，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id16 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002566658905_section2045914466492"></a>

- 位于Global Memory的源地址必须1字节对齐，位于L1 Buffer的目的地址必须32字节对齐。
- 当DataCopyParams结构体参数blockCount、blockLen任意一个值为0时，表示指令不会执行，该接口将被视为NOP（空操作）。
- DataCopyParams结构体参数的值需在取值范围内：

    **表4** DataCopyParams结构体参数取值范围

    | 参数名称 | 取值范围 |
    | ---------- | ---------- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

- 如果需要执行多个DataCopy指令，且DataCopy的目的地址存在重叠，需要通过调用[PipeBarrier\(ISASI\)](../../同步控制/核内同步/PipeBarrier(ISASI).md)来插入同步指令，保证多个DataCopy指令的串行化，防止出现异常数据。如下图左侧示意图，执行两个DataCopy指令，搬运的目的GM地址存在重叠，两条搬运指令之间需要通过调用PipeBarrier<PIPE\_MTE3\>\(\)添加MTE3搬出流水的同步；如下图右侧示意图所示，搬运的目的地址Unified Buffer存在重叠，两条搬运指令之间需要调用PipeBarrier<PIPE\_MTE2\>\(\)添加MTE2搬入流水的同步。

    ![](../../../../figures/绘图4.png)

<!-- npu="A3,910b" id17 -->
- 针对如下产品型号：

    <!-- npu="A3" id18 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品；
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品；
    <!-- end id19 -->

    在跨卡通信算子开发场景，DataCopy类接口支持跨卡数据搬运，仅支持HCCS物理链路，不支持其他通路；开发者开发过程中，需要关注涉及卡间通信的物理通路，可通过npu-smi info -t topo命令查询HCCS物理链路。
<!-- end id17 -->

## 关键特性说明

### 连续搬运<a id="zh-cn_topic_0000002566658905_section3150944151518"></a>

针对连续搬运场景，推荐使用[DataCopy（GMToL1连续数据搬运）](DataCopy（GMToL1连续数据搬运）.md)，以下示例仅展示高维切分数据搬运接口具有连续搬运能力。

以half数据类型为例，源操作数的shape为1 * 128。

[图1](#zh-cn_topic_0000002566658905_fig769164701717)将源操作数中128个元素连续搬运至目的操作数，DataCopyParams结构体参数配置如下：

- blockCount = 1，搬运1个数据块，表示连续搬运。
- blockLen = 8，一个数据块长度为8个DataBlock。
- srcGap = 0，源操作数相邻数据块之间的间隔为0。
- dstGap = 0，目的操作数相邻数据块之间的间隔为0。

**图1** 连续搬运示意图<a id="zh-cn_topic_0000002566658905_fig769164701717"></a>

![](../../../../figures/continuous_data_copy_diagram.png)

### 非连续搬运<a id="zh-cn_topic_0000002566658905_section1084342081618"></a>

以half数据类型为例，源操作数的shape为1 * 112。

[图2](#zh-cn_topic_0000002566658905_fig12371910189)将源操作数中非连续数据搬运至目的操作数，DataCopyParams结构体参数配置如下：

- blockCount= 2，搬运2个数据块。
- blockLen= 3，一个数据块的长度为3个DataBlock。
- srcGap= 1，源操作数相邻数据块之间的间隔为1。
- dstGap= 2，目的操作数相邻数据块之间的间隔为2。

**图2** 非连续搬运示意图<a id="zh-cn_topic_0000002566658905_fig12371910189"></a>

![](../../../../figures/discontinuous_data_copy_diagram.png)

## 调用示例<a id="zh-cn_topic_0000002566658905_section088124295117"></a>

连续搬运以[图1 连续搬运示意图](#zh-cn_topic_0000002566658905_fig769164701717)所示场景为例：

```cpp
constexpr uint32_t copyCount = 128;
// 源操作数：GM上连续存放128个half。
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, copyCount);
// 目的操作数：L1 Buffer。
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, copyCount);

AscendC::DataCopyParams repeatParams;
repeatParams.blockCount = 1; // 搬运1个数据块，表示连续搬运。
repeatParams.blockLen = 8;   // 每个数据块长度为8个DataBlock，即256B，等于128个half。
repeatParams.srcGap = 0;     // blockCount = 1时，源端间隔无意义，设置为0。
repeatParams.dstGap = 0;     // blockCount = 1时，目的端间隔无意义，设置为0。

AscendC::DataCopy(dstLocal, srcGm, repeatParams);
```

非连续搬运以[图2 非连续搬运示意图](#zh-cn_topic_0000002566658905_fig12371910189)所示场景为例：

```cpp
constexpr uint32_t srcCount = 112;
constexpr uint32_t dstCount = 128;
// 源操作数：GM上有112个half，按“48个有效元素+16个跳过元素+48个有效元素”排布。
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, srcCount);

// 目的操作数：L1 Buffer，两个数据块之间预留32个half的间隔。
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, dstCount);

AscendC::DataCopyParams repeatParams;
repeatParams.blockCount = 2; // 搬运2个数据块。
repeatParams.blockLen = 3;   // 每个数据块长度为3个DataBlock，即96B，等于48个half。
repeatParams.srcGap = 1;     // 源端两个数据块之间跳过1个DataBlock，即16个half。
repeatParams.dstGap = 2;     // 目的端两个数据块之间间隔2个DataBlock，即32个half。

AscendC::DataCopy(dstLocal, srcGm, repeatParams);
```
