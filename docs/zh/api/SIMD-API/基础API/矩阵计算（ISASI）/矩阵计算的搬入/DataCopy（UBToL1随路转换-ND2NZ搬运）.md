# DataCopy（UBToL1随路转换-ND2NZ搬运）<a id="ZH-CN_TOPIC_0000002349187356"></a>

## 产品支持情况<a id="section1550532418810"></a>

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

## 功能说明<a id="section12840195813362"></a>

> [!NOTE]说明
> 本接口为软件仿真实现，是在Matmul高阶API的基础上，利用Matmul高阶API中的workspace GM空间作为数据中转空间，数据先搬入GM，再搬入L1 Buffer。因此，在使用本接口时，需要先使用REGISTER_MATMUL注册高阶API。

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

支持在数据搬运时进行ND到NZ格式的转换。数据从Unified Buffer（UB，TPosition为VECIN/VECCALC/VECOUT）搬运至L1 Buffer，搬运过程中完成ND->NZ格式转换。

## 函数原型<a id="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
```

## 参数说明<a id="section1251613311396"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#section4219135304818)。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，存储位置为L1 Buffer（TSCM）。 |
| src | 输入 | 源操作数，类型为LocalTensor，存储位置为Unified Buffer（TPosition为VECIN/VECCALC/VECOUT）。 |
| intriParams | 输入 | 搬运参数，类型为[Nd2NzParams](#table844881954715)。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3** Nd2NzParams结构体参数定义<a id="table844881954715"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| ndNum | 传输ND矩阵的数目，取值范围：ndNum∈[0, 4095]。 |
| nValue | ND矩阵的行数，取值范围：nValue∈[0, 16384]。 |
| dValue | ND矩阵的列数，取值范围：dValue∈[0, 65535]。 |
| srcNdMatrixStride | 源操作数相邻ND矩阵起始地址间的偏移，取值范围：srcNdMatrixStride∈[0, 65535]，单位为元素。 |
| srcDValue | 源操作数同一ND矩阵的相邻行起始地址间的偏移，取值范围：srcDValue∈[1, 65535]，单位为元素。 |
| dstNzC0Stride | ND转换到NZ格式后，源操作数中的一行会转换为目的操作数的多行。dstNzC0Stride表示，目的NZ矩阵中，来自源操作数同一行的多行数据相邻行起始地址间的偏移，取值范围：dstNzC0Stride∈[1, 16384]，单位：C0_SIZE（32字节）。 |
| dstNzNStride | 目的NZ矩阵中，Z型矩阵相邻行起始地址之间的偏移。取值范围：dstNzNStride∈[1, 16384]，单位：C0_SIZE（32字节）。 |
| dstNzMatrixStride | 目的NZ矩阵中，相邻NZ矩阵起始地址间的偏移，取值范围：dstNzMatrixStride∈[0, 16384]，单位为元素。 |

ND2NZ转换示意图如下，样例中参数设置值和解释说明如下：

- ndNum = 2，表示传输ND矩阵的数目为2（ND矩阵1为A1~A2 + B1~B2，ND矩阵2为C1~C2 + D1~D2）。
- nValue = 2，ND矩阵的行数，也就是矩阵的高度为2。
- dValue = 24，ND矩阵的列数，也就是矩阵的宽度为24个元素。当dValue不满足32字节对齐时，在目的操作数中不足的部分会被补齐为0，例如图示中A2所在DataBlock的空白部分会被补齐为0。
- srcNdMatrixStride = 144，表示相邻ND矩阵起始地址间的偏移，即为A1~C1的距离，即为9个DataBlock，9 * 16 = 144个元素。
- srcDValue = 48，表示一行的所含元素个数，即为A1到B1的距离，即为3个DataBlock，3 * 16 = 48个元素。
- dstNzC0Stride = 11。ND转换到NZ格式后，源操作数中的一行会转换为目的操作数的多行，例如src中A1和A2为1行，dst中A1和A2被分为2行。多行数据起始地址之间的偏移就是A1和A2在dst中的偏移，偏移为11个DataBlock。
- dstNzNStride = 2，表示src中一个ND矩阵的第x行和第x+1行转换为NZ格式后在dst中的偏移，即A1和B1在dst之间的偏移为2个DataBlock。
- dstNzMatrixStride = 96，表示dst中第x个ND矩阵的起点和第x+1个ND矩阵的起点的偏移，即A1和C1之间的距离，即为6个DataBlock，6 * 16 = 96个元素。

**图1** ND2NZ转换示意图（half数据类型）<a id="fig128961542184620"></a>

![](../../../../figures/ND2NZ转换示意图（half数据类型）.png)

## 数据类型<a id="section4219135304818"></a>

源矩阵和目的矩阵支持的数据类型保持一致。

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，支持数据类型为：bool、int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、fp8_e8m0_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32。
<!-- end id10 -->

<!-- npu="A3" id11 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、half、bfloat16_t、float。
<!-- end id11 -->

<!-- npu="910b" id12 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、half、bfloat16_t、float。
<!-- end id12 -->

## 返回值说明

无

## 约束说明

- 本接口通过软件仿真实现，首先会进行数据分形的转换，然后再通过搬运指令进行数据的搬运。
- 本接口内部会申请一块临时的UB空间用于数据分形的转换，开发者无需申请，但是需要预留临时空间的大小，空间大小计算公式如下（单位为字节）：

$$((dValue \times sizeof(T) / 32 - 1) \times dstNzC0Stride + (nValue - 1) \times dstNzNStride + 1) \times 32$$

- 本接口仅支持配置ndNum为1。
- 位于Unified Buffer的源地址必须32字节对齐，位于L1 Buffer的目的地址必须32字节对齐。
- Nd2NzParams结构体参数ndNum、nValue、dValue设置不能为0。
- 搬运至L1 Buffer的数据不能重叠，如果存在重叠写入，硬件不会产生任何警告或错误，并且不保证重叠数据的写入顺序。

<!-- npu="950" id13 -->
- 针对Ascend 950PR/Ascend 950DT，支持ub->L1 Buffer的1:2硬通道的方式；同时提供1:1的兼容模式，解决A2/A3的代码迁移兼容问题，这种模式通过Global Memory进行中转，效率稍低。
<!-- end id13 -->

- Nd2NzParams结构体参数的值需在取值范围内，参数取值范围如[表4](#table_ub_nd2nz_range)所示。

**表 4**  Nd2NzParams结构体参数取值范围<a id="table_ub_nd2nz_range"></a>

|参数名称|取值范围|
|----------|----------|
|ndNum|[1]|
|nValue|[1, 16384]|
|dValue|[1, 65535]|
|srcNdMatrixStride|[0, 65535]|
|srcDValue|[1, 65535]|
|dstNzC0Stride|[1, 16384]|
|dstNzNStride|[1, 16384]|
|dstNzMatrixStride|[0, 16384]|

## 调用示例<a id="section10309141400"></a>

示例场景如[图1](#fig128961542184620)所示，示例代码片段如下：

```cpp
AscendC::Nd2NzParams nd2nzParams;
// 传输2个ND矩阵。
nd2nzParams.ndNum = 2;
// 每个ND矩阵高度为2行。
nd2nzParams.nValue = 2;
// 每行宽度为24个half；不足32B对齐的部分在目的端补0。
nd2nzParams.dValue = 24;
// 源端相邻ND矩阵起始地址间隔为9个DataBlock，即9 * 16 = 144个half。
nd2nzParams.srcNdMatrixStride = 144;
// 源端同一ND矩阵相邻行起始地址间隔为3个DataBlock，即3 * 16 = 48个half。
nd2nzParams.srcDValue = 48;
// 目的NZ中同一源行拆出的相邻C0块间隔为11个DataBlock。
nd2nzParams.dstNzC0Stride = 11;
// 目的NZ中相邻源行转换后的起始地址间隔为2个DataBlock。
nd2nzParams.dstNzNStride = 2;
// 目的端相邻NZ矩阵起始地址间隔为6个DataBlock，即6 * 16 = 96个half。
nd2nzParams.dstNzMatrixStride = 96;

AscendC::DataCopy(dstLocal, srcLocal, nd2nzParams);
```
