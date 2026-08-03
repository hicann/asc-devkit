# DataCopy（GMToL1随路转换-ND2NZ搬运）<a id="ZH-CN_TOPIC_0000002569070913"></a>

## 产品支持情况<a id="zh-cn_topic_0000002566538879_section796754519912"></a>

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
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a id="zh-cn_topic_0000002566538879_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口主要实现将矩阵从Global Memory搬运至L1 Buffer（TPosition为A1/B1），并支持在数据搬运时进行ND到NZ格式的转换。

ND到NZ的格式转换等价于DN到ZN的格式转换，如[图1](#zh-cn_topic_0000002566538879_fig444462417355)。

**图1** ND2NZ与DN2ZN示意图<a id="zh-cn_topic_0000002566538879_fig444462417355"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_copy.png)

## 函数原型<a id="zh-cn_topic_0000002566538879_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
```

<!-- npu="950" id10 -->
特别针对Ascend 950PR/Ascend 950DT，函数原型请参考如下：

```cpp
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
```
<!-- end id10 -->

## 参数说明<a id="zh-cn_topic_0000002566538879_section16128134420472"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002566538879_section4219135304818)。 |
| enableSmallC0 | SmallC0模式开关：当dValue小于等于4的时候，C0_SIZE会补齐到4 * sizeof(T)字节。<br>默认不开启。不同的型号，enableSmallC0支持度不同，请参考[enableSmallC0参数支持度说明](#enablesmallc0参数支持度说明)。 |

### enableSmallC0参数支持度说明

<!-- npu="950" id11 -->
- 针对Ascend 950PR/Ascend 950DT，新增enableSmallC0参数，调用该接口必须配置参数enableSmallC0；
<!-- end id11 -->

- 其他型号不支持该参数。

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，存储位置为L1 Buffer（TPosition为A1/B1），目的地址需要32字节对齐。 |
| src | 输入 | 源操作数，类型为GlobalTensor，存储位置为Global Memory，源地址需要1字节对齐。 |
| intriParams | 输入 | ND2NZ搬运参数，类型为Nd2NzParams。<br>Nd2NzParams参数说明请参考[表4](#zh-cn_topic_0000002566538879_table144203616291)。 |

**表3** Nd2NzParams结构体参数定义<a id="zh-cn_topic_0000002566538879_table144203616291"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| ndNum | 源矩阵中ND矩阵的数量，取值范围：ndNum∈[0, 4095]。<br>**注：ndNum = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| nValue | 源矩阵中ND矩阵的行数，取值范围：nValue∈[0, 16384]。<br>**注：nValue = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| dValue | 源矩阵中ND矩阵的列数，取值范围：dValue∈[0, 65535]。当dValue * sizeof(T)不满足32字节对齐时，在目的矩阵中会补0对齐到32字节。<br>**注：dValue = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcNdMatrixStride | 源矩阵相邻ND矩阵起始地址间的偏移，取值范围：srcNdMatrixStride∈[0, 65535]，单位：元素个数。<br>&nbsp;&nbsp;&bull; ndNum = 1时，srcNdMatrixStride无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; ndNum ≠ 1时，当srcNdMatrixStride = 0时，表示重复搬出源矩阵的第一个ND矩阵。 |
| srcDValue | 源矩阵一行中包含的元素个数，取值范围：srcDValue∈[1, 65535]，单位：元素个数。 |
| dstNzC0Stride | ND转换为NZ格式后，目的NZ矩阵中相邻Z矩阵起始地址的偏移，取值范围：dstNzC0Stride∈[1, 16384]，单位：C0_SIZE（32字节）。 |
| dstNzNStride | 目的矩阵中，NZ矩阵中相邻行起始地址的偏移，取值范围：dstNzNStride∈[1, 16384]，单位：C0_SIZE（32字节）。 |
| dstNzMatrixStride | 目的矩阵中，相邻NZ矩阵起始地址的偏移，取值范围：dstNzMatrixStride∈[0, 16384]，单位：元素个数。<br>&nbsp;&nbsp;&bull; ndNum = 1时，dstNzMatrixStride无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; ndNum ≠ 1时，当dstNzMatrixStride = 0时，表示目的矩阵中搬入的每个NZ矩阵都会覆盖第一个NZ矩阵。 |

## 数据类型<a id="zh-cn_topic_0000002566538879_section4219135304818"></a>

源矩阵和目的矩阵支持的数据类型保持一致。

<!-- npu="950" id12 -->
针对Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、fp8_e8m0_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32。
<!-- end id12 -->

<!-- npu="A3" id13 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：b4（int4b_t）、int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、half、bfloat16_t、float。
<!-- end id13 -->

<!-- npu="910b" id14 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：b4（int4b_t）、int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、half、bfloat16_t、float。
<!-- end id14 -->

<!-- npu="310p" id15 -->
针对Atlas 推理系列产品AI Core，支持数据类型为：int16_t、uint16_t、int32_t、uint32_t、half、float。
<!-- end id15 -->

<!-- npu="x90" id16 -->
Kirin X90，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half。
<!-- end id16 -->

<!-- npu="9030" id17 -->
Kirin 9030，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half。
<!-- end id17 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002566538879_section2045914466492"></a>

- 当输入数据类型是b4时，ND2NZ过程中数据是基于b8类型进行处理，因此参数需要根据b8类型进行设置。
- 位于Global Memory的源地址必须1字节对齐，位于L1 Buffer的目的地址必须32字节对齐。
- 当Nd2NzParams结构体参数ndNum、nValue、dValue任意一个值为0时，该指令不会执行，该接口将被视为NOP（空操作）。
- 搬运至L1 Buffer的数据不能重叠，如果存在重叠写入，硬件不会产生任何警告或错误，并且不保证重叠数据的写入顺序。
- Nd2NzParams结构体参数的值需在取值范围内：

    **表4** Nd2NzParams结构体参数取值范围

    | 参数名称 | 取值范围 |
    | ---------- | ---------- |
    | ndNum | [0, 4095] |
    | nValue | [0, 16384] |
    | dValue | [0, 65535] |
    | srcNdMatrixStride | [0, 65535] |
    | srcDValue | [1, 65535] |
    | dstNzC0Stride | [1, 16384] |
    | dstNzNStride | [1, 16384] |
    | dstNzMatrixStride | [0, 16384] |

<!-- npu="310p" id18 -->
- 针对Atlas 推理系列产品AI Core，使用Global Memory -\> Local Memory通路的ND2NZ搬运接口时，需要预留8K的UB空间，作为接口的临时数据存放区。
<!-- end id18 -->

## 关键特性说明

### 连续搬运<a id="zh-cn_topic_0000002566538879_section7495192151713"></a>

以half数据类型为例，[图2](#zh-cn_topic_0000002566538879_fig7420114233419)为ND2NZ连续搬运示意图。Nd2NzParams结构体参数配置说明如下：

- ndNum = 1，表示搬运的ND矩阵的数量为1。
- nValue = 16，一个ND矩阵的行数。
- dValue = 23，一个ND矩阵的列数。当dValue * sizeof(T)不满足32字节对齐时，在目的矩阵中会补0对齐到32字节。
- srcNdMatrixStride = 0，只有一个ND矩阵，该值为0。
- srcDValue = 32，表示源矩阵一行所含元素个数，即32个half数据类型的元素。
- dstNzC0Stride = 16，NZ矩阵中相邻Z矩阵起始地址的偏移，偏移为16个C0_SIZE。
- dstNzNStride = 1，表示NZ矩阵中相邻行起始地址的偏移。
- dstNzMatrixStride = 0，只有一个NZ矩阵，该值为0。

**图2** ND2NZ转换示意图（连续搬运）<a id="zh-cn_topic_0000002566538879_fig7420114233419"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_continuous_copy.png)

### 非连续搬运<a id="zh-cn_topic_0000002566538879_section1721231581717"></a>

以half数据类型为例，[图3](#zh-cn_topic_0000002566538879_fig19978184883316)为ND2NZ非连续搬运示意图。Nd2NzParams结构体参数配置说明如下：

- ndNum = 2，表示搬运的ND矩阵的数量为2。
- nValue = 8，一个ND矩阵的行数。
- dValue = 23，一个ND矩阵的列数。当dValue * sizeof(T)不满足32字节对齐时，在目的矩阵中会补0对齐到32字节。
- srcNdMatrixStride = 384，表示相邻ND矩阵起始地址的偏移，每行32个元素，共12行，偏移为32 * 12 = 384个元素。
- srcDValue = 32，表示源矩阵一行所含元素个数，即32个half数据类型的元素。
- dstNzC0Stride = 37，NZ矩阵中相邻Z矩阵起始地址的偏移，偏移为37个C0_SIZE。
- dstNzNStride = 2，表示NZ矩阵中相邻行起始地址的偏移。
- dstNzMatrixStride = 320，相邻NZ矩阵起始地址的偏移，每行16个元素，共20行，偏移为16 * 20 = 320个元素。

**图3** ND2NZ转换示意图（非连续搬运）<a id="zh-cn_topic_0000002566538879_fig19978184883316"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_noncontinuous_copy.png)

### SmallC0模式

enableSmallC0开启模式下的ND2NZ转换示意图如下：

**图4** enableSmallC0开启模式下的ND2NZ转换示意图（half数据类型）<a name="fig07641913195410"></a>  

![](../../../../figures/enableSmallC0开启模式下的ND2NZ转换示意图-（half数据类型）.png "enableSmallC0开启模式下的ND2NZ转换示意图-（half数据类型）")

## 调用示例<a id="zh-cn_topic_0000002566538879_section088124295117"></a>

如下示例中：在A矩阵不转置，数据类型为half的场景下，使用DataCopy进行随路ND2NZ数据搬运。

搬运过程的数据排布变化示意图如下：

![](../../../../figures/datacopy_gm2l1_nd2nz_demo.png)

示例代码片段如下，仅展示样例中的部分代码，完整示例请参考：[DataCopy_GM2L1样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
// m=40,k=56,fractalShape[0] = 16,fractalShape[1] = 16,fractalSize = 16 * fractalShape[1]
AscendC::Nd2NzParams nd2nzA1Params;
// 传输ND矩阵的数目
nd2nzA1Params.ndNum = 1;
// ND矩阵的行数
nd2nzA1Params.nValue = m;
// ND矩阵的列数
nd2nzA1Params.dValue = k;
// 只传输了1个ND矩阵，该参数无效
nd2nzA1Params.srcNdMatrixStride = 0;
// 源矩阵一行中包含的元素个数
nd2nzA1Params.srcDValue = k;
// 以下这个参数取A矩阵在L1 Buffer上，高度方向的对齐后的长度
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]);
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = 0;
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```
