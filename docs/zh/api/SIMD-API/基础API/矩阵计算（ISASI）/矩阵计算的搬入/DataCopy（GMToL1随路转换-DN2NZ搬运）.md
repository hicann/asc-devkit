# DataCopy（GMToL1随路转换-DN2NZ搬运）<a id="ZH-CN_TOPIC_0000002563847574"></a>

## 产品支持情况<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->

## 功能说明<a id="section474617392321"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

随路格式转换数据搬运，适用于在搬运时进行DN到NZ格式的转换。数据从Global Memory搬运至L1 Buffer（TPosition为A1/B1），搬运过程中完成DN->NZ格式转换。

DN到NZ的格式转换等价于ND到ZN的格式转换，如下图所示：

**图1** DN到NZ格式转换示意图

![](../../../../figures/gm2l1_dn2nz_concept.png "DN到NZ格式转换示意图")

## 函数原型<a id="section1954364615315"></a>

```cpp
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
```

## 参数说明<a id="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 源操作数或者目的操作数的数据类型。 |
| enableSmallC0 | SmallC0模式开关：当dValue小于等于4的时候，C0_SIZE会补齐到4 * sizeof(T)字节，默认不开启。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，存储位置为L1 Buffer（TPosition为A1/B1）。 |
| src | 输入 | 源操作数，类型为GlobalTensor。<br>[SetL2CacheHint](../../数据结构/GlobalTensor/SetL2CacheHint.md)支持的L2 Cache控制模式如下：CacheMode::CACHE_MODE_DISABLE、CacheMode::CACHE_MODE_NORMAL。 |
| intriParams | 输入 | 搬运参数，Dn2NzParams类型，具体参数说明请参考[表3](#table9182515919)。 |

**表3** Dn2NzParams结构体参数定义<a id="table9182515919"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| dnNum | 传输DN矩阵的数目，取值范围：dnNum∈[0, 4095]。<br>**注：dnNum=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| nValue | DN矩阵的列数，取值范围：nValue∈[0, 16384]。<br>**注：nValue=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| dValue | DN矩阵的行数，取值范围：dValue∈[0, 2^21-1]。<br>**注：dValue=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcDnMatrixStride | 源操作数相邻DN矩阵起始地址间的偏移，取值范围：srcDnMatrixStride∈[0, 2^40-1]，单位为元素。<br>&nbsp;&nbsp;&bull; dnNum = 1时，srcDnMatrixStride无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; dnNum ≠ 1时，当srcDnMatrixStride = 0时，表示源矩阵中搬入的每个DN矩阵都从第一个DN矩阵读取。 |
| srcDValue | 源操作数同一DN矩阵的相邻行起始地址间的偏移，取值范围：srcDValue∈[1, 2^40-1]，单位为元素。 |
| dstNzC0Stride | DN转换到NZ格式后，源操作数中的一列会转换为目的操作数的多行。dstNzC0Stride表示，目的NZ矩阵中，来自源操作数同一列的多行数据相邻行起始地址间的偏移，取值范围：dstNzC0Stride∈[1, 16384]，单位：C0_SIZE（32字节）。 |
| dstNzNStride | 目的NZ矩阵中，Z型矩阵相邻行起始地址之间的偏移。取值范围：dstNzNStride∈[1, 16384]，单位：C0_SIZE（32字节）。 |
| dstNzMatrixStride | 目的NZ矩阵中，相邻NZ矩阵起始地址的偏移，取值范围：dstNzMatrixStride∈[0, 16384]，单位为元素个数。<br>&nbsp;&nbsp;&bull; dnNum = 1时，dstNzMatrixStride无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; dnNum ≠ 1时，当dstNzMatrixStride = 0时，表示目的矩阵中搬入的每个NZ矩阵都会覆盖第一个NZ矩阵。 |

**连续搬运**

以half数据类型为例，下图为DN2NZ连续搬运示意图。Dn2NzParams结构体参数配置说明如下：

- dnNum = 1，表示搬运的DN矩阵的数量为1。
- nValue = 16，一个DN矩阵的列数。
- dValue = 23，一个DN矩阵的行数。当dValue \* sizeof\(T\)不满足32字节对齐时，在目的矩阵中会补0对齐到32字节。
- srcDnMatrixStride = 0，只有一个DN矩阵，该值为0。
- srcDValue = 16，表示源矩阵一行的所含元素个数，即16个half数据类型的元素。
- dstNzC0Stride = 16，NZ矩阵中相邻Z矩阵起始地址的偏移，偏移为16个C0\_SIZE，也就是16 \* 32字节。
- dstNzNStride = 1，表示NZ矩阵中相邻行起始地址的偏移。
- dstNzMatrixStride = 0，输入只有一个DN矩阵，该值为0。

**图2** DN2NZ转换示意图（连续搬运）

![](../../../../figures/gm2l1_dn2nz_continuous.png "DN2NZ转换示意图（连续搬运）")

**非连续搬运**

以half数据类型为例，下图为DN2NZ非连续搬运示意图。Dn2NzParams结构体参数配置说明如下：

- dnNum = 2，表示搬运的DN矩阵的数量为2。
- nValue = 8，一个DN矩阵的列数。
- dValue = 23，一个DN矩阵的行数。当dValue \* sizeof\(T\)不满足32字节对齐时，在目的矩阵中会补0对齐到32字节。
- srcDnMatrixStride = 400，表示相邻DN矩阵起始地址的偏移，每行16个元素，共25行，偏移为16 \* 25= 400个元素。
- srcDValue = 16，表示源矩阵一行的所含元素个数，即16个half数据类型的元素。
- dstNzC0Stride = 37，NZ矩阵中相邻Z矩阵起始地址的偏移，偏移为37个C0\_SIZE。
- dstNzNStride = 2，表示NZ矩阵中相邻行起始地址的偏移。
- dstNzMatrixStride = 320，相邻NZ矩阵起始地址的偏移，每行16个元素，共20行，偏移为16 \* 20 = 320个元素。

**图3** DN2NZ转换示意图（非连续搬运）

![](../../../../figures/gm2l1_dn2nz.png "DN2NZ转换示意图（非连续搬运）")

**SmallC0模式**

当dValue小于等于4的时候，可以使能SmallC0模式，C0\_SIZE会对齐到4 \* sizeof\(T\)字节，当dValue \* sizeof\(T\)不满足4 \* sizeof\(T\)对齐时，在目的矩阵中会补0对齐到4 \* sizeof\(T\)。

若整个目的矩阵没有C0\_SIZE（32字节）对齐，则目的矩阵中尾部补0直到32字节对齐。在该模式下，dstNzNStride参数和dstNzC0Stride参数，无需配置。

**图4** DN2NZ转换示意图（SmallC0模式）

![](../../../../figures/gm2l1_dn2nz_smallc0.png "DN2NZ转换示意图（SmallC0模式）")

## 数据类型<a id="section4219135304818"></a>

支持的数据类型为：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、int16_t、uint16_t、int32_t、uint32_t、half、bfloat16_t、float。

## 返回值说明

无

## 约束说明

- 当输入数据类型是b4时，DN2NZ过程中数据是基于b8类型进行处理，因此参数需要根据b8类型进行设置，nValue必须是2的倍数。
- 位于Global Memory的源地址必须1字节对齐，位于L1 Buffer的目的地址必须32字节对齐。
- 当Dn2NzParams结构体参数dnNum、nValue、dValue任意一个值为0时，该指令不会执行，该接口将被视为NOP（空操作）。
- 搬运至L1 Buffer的数据不能重叠，如果存在重叠写入，硬件不会产生任何警告或错误，并且不保证重叠数据的写入顺序。
- Dn2NzParams结构体参数的值需在取值范围内，参数取值范围如[表4](#table_dn2nz_range)所示。

**表 4**  Dn2NzParams结构体参数取值范围<a id="table_dn2nz_range"></a>

|参数名称|取值范围|
|----------|----------|
|dnNum|[0, 4095]|
|nValue|[0, 16384]|
|dValue|[0, 2^21-1]|
|srcDnMatrixStride|[0, 2^40-1]|
|srcDValue|[1, 2^40-1]|
|dstNzC0Stride|[1, 16384]|
|dstNzNStride|[1, 16384]|
|dstNzMatrixStride|[0, 16384]|

## 调用示例<a id="section122101199486"></a>

如下示例中：在A矩阵转置，数据类型为half的场景下，使用DataCopy进行随路DN2NZ数据搬运。

搬运过程的数据排布变化示意图如下：

![](../../../../figures/datacopy_gm2l1_dn2nz_demo.png)

示例代码片段如下，完整示例请参考[data_copy_gm2l1样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1)中场景3。

```cpp
// m=40,k=56,fractalShape[0] = 16,fractalShape[1] = 16,fractalSize = 16 * fractalShape[1] 
AscendC::Dn2NzParams dn2nzA1Params; 
// 传输DN矩阵的数目 
dn2nzA1Params.dnNum = 1; 
// DN矩阵的列数 
dn2nzA1Params.nValue = 40; 
// DN矩阵的行数 
dn2nzA1Params.dValue = 56; 
// 只传输了1个DN矩阵，该参数无效 
dn2nzA1Params.srcDnMatrixStride = 0; 
// 源操作数中同一DN矩阵行起始地址间的偏移 
dn2nzA1Params.srcDValue = 40; 
dn2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]); 
dn2nzA1Params.dstNzNStride = 1; 
dn2nzA1Params.dstNzMatrixStride = 0; 
AscendC::DataCopy(a1Local, aGM, dn2nzA1Params);

```
