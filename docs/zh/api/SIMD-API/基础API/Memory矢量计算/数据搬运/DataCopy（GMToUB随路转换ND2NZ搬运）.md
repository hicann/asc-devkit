# DataCopy（GMToUB随路转换ND2NZ搬运）<a name="ZH-CN_TOPIC_0000002349187356"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id16 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id16 -->
<!-- npu="A3" id17 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id17 -->
<!-- npu="910b" id18 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id18 -->
<!-- npu="310b" id19 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id19 -->
<!-- npu="310p" id20 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id21 -->
<!-- npu="910" id22 -->
- Atlas 训练系列产品：不支持
<!-- end id22 -->
<!-- npu="x90" id23 -->
- Kirin X90：不支持
<!-- end id23 -->
<!-- npu="9030" id24 -->
- Kirin 9030：不支持
<!-- end id24 -->

## 功能说明<a name="section12840195813362"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口为软仿接口，从易用性角度出发进行设计，支持在从Global Memory到Unified Buffer的数据搬运过程中进行ND到NZ格式的转换。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN

## 函数原型<a name="section1954364615315"></a>

- 不支持enableSmallC0模式

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id1 -->
- 支持enableSmallC0模式（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    // 该函数原型仅支持Ascend 950PR/Ascend 950DT
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id1 -->

## 参数说明<a name="section1251613311396"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#section4219135304818)。 |
| enableSmallC0 | SmallC0模式开关：当dValue小于等于4的时候，C0_SIZE会补齐到4 * sizeof(T)字节。<br>默认不开启。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数，类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)。 |
| src | 输入 | 源操作数，类型为[GlobalTensor](../../数据结构/GlobalTensor/GlobalTensor简介.md)。 |
| intriParams | 输入 | 搬运参数，类型为Nd2NzParam，具体参数请参考[表3](#table_nd2nz_3)。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3**  Nd2NzParams结构体参数定义<a name="table_nd2nz_3"></a>

| 参数名 | 描述 |
| :--- | :--- |
| ndNum | 传输ND矩阵的数目，取值范围：ndNum∈[0, 4095]。 |
| nValue | ND矩阵的行数，取值范围：nValue∈[0, 16384]。 |
| dValue | ND矩阵的列数，取值范围：dValue∈[0, 65535]。 |
| srcNdMatrixStride | 源操作数相邻ND矩阵起始地址间的偏移，取值范围：srcNdMatrixStride∈[0, 65535]，单位为元素。 |
| srcDValue | 源操作数同一ND矩阵的相邻行起始地址间的偏移，取值范围：srcDValue∈[1, 65535]，单位为元素。 |
| dstNzC0Stride | ND转换到NZ格式后，源操作数中的一行会转换为目的操作数的多行。dstNzC0Stride表示，目的NZ矩阵中，来自源操作数同一行的多行数据相邻行起始地址间的偏移，取值范围：dstNzC0Stride∈[1, 16384]，单位：C0_SIZE（32B）。 |
| dstNzNStride | 目的NZ矩阵中，Z型矩阵相邻行起始地址之间的偏移。取值范围：dstNzNStride∈[1, 16384]，单位：C0_SIZE（32B）。 |
| dstNzMatrixStride | 目的NZ矩阵中，相邻NZ矩阵起始地址间的偏移，取值范围：dstNzMatrixStride∈[1, 65535]，单位为元素。 |

ND2NZ转换示意图如下，样例中参数设置值和解释说明如下：

- ndNum = 2，表示传输ND矩阵的数目为2 \(ND矩阵1为A1\~A2 + B1\~B2，ND矩阵2为C1\~C2 + D1\~D2\)。
- nValue = 2，ND矩阵的行数，也就是矩阵的高度为2。
- dValue = 24，ND矩阵的列数，也就是矩阵的宽度为24个元素。当dValue不满足32字节对齐时，在目的操作数中不足的部分会被补齐为0，例如图示中A2所在DataBlock的空白部分会被补齐为0。
- srcNdMatrixStride = 144，表达相邻ND矩阵起始地址间的偏移，即为A1\~C1的距离，即为9个DataBlock，9 \* 16 = 144个元素。
- srcDValue = 48，表示一行的所含元素个数，即为A1到B1的距离，即为3个DataBlock，3 \* 16 = 48个元素。
- dstNzC0Stride = 11。ND转换到NZ格式后，源操作数中的一行会转换为目的操作数的多行，例如src中A1和A2为1行，dst中A1和A2被分为2行。多行数据起始地址之间的偏移就是A1和A2在dst中的偏移，偏移为11个DataBlock。
- dstNzNStride = 2，表示src中一个ND矩阵的第x行和第x+1行转换为NZ格式后在dst中的偏移，即A1和B1在dst之间的偏移为2个DataBlock。
- dstNzMatrixStride = 96，表达dst中第x个ND矩阵的起点和第x+1个ND矩阵的起点的偏移，即A1和C1之间的距离，即为6个DataBlock，6 \* 16 = 96个元素。

**图1**  ND2NZ转换示意图（half数据类型）<a name="fig128961542184620"></a>  
![](../../../../figures/nd2nz_conversion_half.png "ND2NZ转换示意图（half数据类型）")

enableSmallC0开启模式下的ND2NZ转换示意图如下：

**图2**  enableSmallC0开启模式下的ND2NZ转换示意图（half数据类型）<a name="fig07641913195410"></a>  
![](../../../../figures/enable_smallc0_nd2nz_conversion_half.png "enableSmallC0开启模式下的ND2NZ转换示意图-（half数据类型）")

## 数据类型<a name="section4219135304818"></a>

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32。
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id5 -->

## 返回值说明<a name="section446456163012"></a>

无

## 约束说明<a name="section1140315215118"></a>

- 该接口为软仿接口，从易用性角度出发进行设计。
- Nd2NzParams结构体参数的值需在取值范围内：

    **表4**  Nd2NzParams结构体参数取值范围

    | 参数名 | 取值范围 |
    | --- | --- |
    | ndNum | [0, 4095] |
    | nValue | [0, 16384] |
    | dValue | [0, 65535] |
    | srcNdMatrixStride | [0, 65535] |
    | srcDValue | [1, 65535] |
    | dstNzC0Stride | [1, 16384] |
    | dstNzNStride | [1, 16384] |
    | dstNzMatrixStride | [1, 65535] |

<!-- npu="310p" id6 -->
- 针对Atlas 推理系列产品AI Core，需要预留8KB的Unified Buffer空间，作为接口的临时数据存放区。
<!-- end id6 -->

## 调用示例<a name="section10309141400"></a>

intriParams参数解析请参考[图1](#fig128961542184620)。

```cpp
// dstLocal为half类型的Unified Buffer上的LocalTensor，srcGlobal为half类型的GlobalTensor。
AscendC::Nd2NzParams intriParams{1, 32, 32, 0, 32, 32, 1, 0};
// Global Memory -> Local Memory
AscendC::DataCopy(dstLocal, srcGlobal, intriParams);
```

结果示例：

```text
输入数据(srcGlobal): [1 2 3 ... 1024]
输出数据(dstLocal):[1 2 ... 15 16 33 34 ... 47 48 65 66 ... 79 80 97 98 ... 111 112 ... 1009 1010... 1023 1024]
```
