# DataCopyPad（UBToL1非对齐数据搬运）

## 产品支持情况<a name="section1550532418810"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

> [!NOTE]说明
> 本接口为软件仿真实现，是在Matmul高阶API的基础上，利用Matmul高阶API中的workspace GM空间作为数据中转空间，数据先搬入GM，再搬入L1 Buffer。因此，在使用本接口时，需要先使用REGISTER_MATMUL注册高阶API。

该接口提供从Unified Buffer到L1 Buffer的数据非对齐搬运功能。

## 函数原型<a name="section620mcpsimp"></a>

- 通路：Local Memory-\>Local Memory，实际搬运过程是UB-\>GM-\>L1 Buffer（TSCM）。

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const Nd2NzParams& nd2nzParams)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

<a name="table4835205712588"></a>

| 参数名 | 描述 |
| --- | --- |
| T | 操作数以及paddingValue（待填充数据值）的数据类型。 |

**表2** 接口参数说明

<a name="zh-cn_topic_0235751031_table33761356"></a>

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>LocalTensor的起始地址需要保证32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>LocalTensor的起始地址需要保证32字节对齐。 |
| dataCopyParams | 输入 | 搬运参数。<br>&nbsp;&nbsp;&bull; DataCopyExtParams类型，具体参数说明请参考[表4](#table10572141063919)。 |
| nd2nzParams | 输入 | 从UB->L1 Buffer（VECIN/VECOUT->TSCM）进行数据搬运时，可以进行ND到NZ的数据格式转换。nd2nzParams是用于控制数据格式转换的参数，Nd2NzParams类型，具体参数请参考[DataCopy（UBToL1随路转换-ND2NZ搬运）](DataCopy（UBToL1随路转换-ND2NZ搬运）.md)中的介绍。<br>**注意：本接口Nd2NzParams的ndNum仅支持设置为1**。 |

**表3** DataCopyExtParams结构体参数定义

<a name="table10572141063919"></a>

| 参数名称 | 含义 |
| --- | --- |
| blockCount | 指定该指令包含的连续传输数据块个数，数据类型为uint16_t，取值范围：blockCount∈[1, 4095]。 |
| blockLen | 指定该指令每个连续传输数据块长度，**该指令支持非对齐搬运**，**每个连续传输数据块长度单位为字节**。数据类型为uint32_t，取值范围：blockLen∈[1, 2097151]**，blockLen必须是sizeof(T)的整数倍，需要注意UB不要超限**。 |
| srcStride | 源操作数，相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔）。单位为dataBlock(32字节)。<br>数据类型为uint32_t，srcStride不要超出该数据类型的取值范围。 |
| dstStride | 目的操作数，相邻连续数据块间的间隔（前面一个数据块的尾与后面数据块的头的间隔）。单位为dataBlock(32字节)。数据类型为uint32_t，dstStride不要超出该数据类型的取值范围。 |
| rsv | 保留字段。 |

下面给出如下场景的配置示例：

- **UB->L1 Buffer（VECIN/VECOUT->TSCM）**

    > [!CAUTION]注意
    > 内部实现涉及AIC和AIV之间的通信，实际搬运路径为UB（VECIN/VECOUT）-\>GM-\>L1 Buffer（TSCM），**发送通信消息会有开销，性能会受到影响**。

    如[图1 UB->L1 Buffer（VECIN/VECOUT->TSCM）搬运示意图](#fig9329040132719)所示，展示了从UB搬运到GM，再搬运到L1 Buffer的过程：示例中数据类型为half，单个datablock（32字节）含有16个half元素，源操作数中的A1\~A6、B1\~B6、C1\~C6为需要进行搬运的数据。

    - 从UB-\>GM的搬运，数据存储格式没有发生转变，依然是ND。
        - **blockCount**为需要搬运的连续传输数据块个数，设置为3；
        - **blockLen**为一个连续传输数据块的大小（单位为字节），设置为6 \* 32 = 192；
        - **srcStride**为源操作数相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔），源操作数物理位置为UB，其单位为datablock，两个连续传输数据块（A1\~A6、B1\~B6）中间相隔1个A7，因此srcStride设置为1；
        - **dstStride**为目的操作数，相邻连续数据块间的间隔（前面一个数据块的尾与后面数据块的头的间隔），目的操作数逻辑位置为GM，其单位为字节，两个连续传输数据块（A1\~A6、B1\~B6）中间相隔2个空白的datablock，因此dstStride设置为64字节。

    - 从GM-\>L1 Buffer的搬运，数据存储格式由ND转换为NZ。
        - **ndNum**固定为1，即A1\~A6、B1\~B6、C1\~C6视作一整个ndMatrix；
        - **nValue**为ndMatrix的行数，即为3行；
        - **dValue**为ndMatrix中一行包含的元素个数，即为6 \* 16 = 96个元素；
        - **srcNdMatrixStride**为相邻ndMatrix之间的距离，因为仅涉及1个ndMatrix，所以可填为0；
        - **srcDValue**表明ndMatrix的第x行和第x+1行所相隔的元素个数，如A1\~B1的距离，即为8个datablock，8 \* 16 = 128个元素；
        - **dstNzC0Stride**为src同一行的相邻datablock在NZ矩阵中相隔datablock数，如A1\~A2的距离，即为7个datablock （A1 + 空白 + B1 + 空白 + C1 + 空白 \* 2）；
        - **dstNzNStride**为src中ndMatrix的相邻行在NZ矩阵中相隔多少个datablock，如A1\~B1的距离，即为2个datablock（A1 + 空白）；
        - **dstNzMatrixStride**为相邻NZ矩阵之间的元素个数，因为仅涉及1个NZ矩阵，所以可以填为0。

    **图1** UB->L1 Buffer（VECIN/VECOUT->TSCM）搬运示意图<a name="fig9329040132719"></a>  

    ![](../../../../figures/VECIN-VECOUT--TSCM搬运示意图.png "VECIN-VECOUT--TSCM搬运示意图")

## 数据类型

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。
<!-- end id8 -->

<!-- npu="A3" id9 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id9 -->

<!-- npu="910b" id10 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id10 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

<!-- npu="950" id11 -->
- 特别地，针对Ascend 950PR/Ascend 950DT，设置srcStride和dstStride参数时，数据类型为int64_t，取值范围为[0, 65535]。
<!-- end id11 -->

## 调用示例<a name="section177231425115410"></a>

以下示例展示[图1](#fig9329040132719)所示的场景：数据类型为half，单个DataBlock（32字节）含16个half元素，将UB中的ND格式数据经GM中转后，以NZ格式搬入L1 Buffer。

```cpp
constexpr uint32_t dataBlockElemCount = 16;
constexpr uint32_t srcElemCount = (3 * 7) * dataBlockElemCount;
constexpr uint32_t dstElemCount = (7 * 6) * dataBlockElemCount;
constexpr uint32_t tscmAddr = 0;

// 源操作数：UB。
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::VECIN, ubAddr, srcElemCount);

// 目的操作数：L1 Buffer。
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::TSCM, tscmAddr, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// UB（VECIN）->GM阶段搬运3个连续传输数据块。
copyParams.blockCount = 3;
// 每个连续传输数据块包含6个DataBlock。
copyParams.blockLen = 6 * 32;
// 源操作数在UB，单位为DataBlock；相邻行之间跳过1个DataBlock。
copyParams.srcStride = 1;
// 中转目的端为GM，单位为字节；相邻行之间保留2个空白DataBlock，即64B。
copyParams.dstStride = 64;
// 保留字段，固定填0。
copyParams.rsv = 0;

AscendC::Nd2NzParams nd2nzParams;
// 本接口约束ndNum仅支持设置为1。
nd2nzParams.ndNum = 1;
// ND矩阵高度为3行。
nd2nzParams.nValue = 3;
// 每行宽度为6个DataBlock。
nd2nzParams.dValue = 6 * dataBlockElemCount;
// 仅1个ND矩阵，不涉及相邻ND矩阵间隔。
nd2nzParams.srcNdMatrixStride = 0;
// GM中相邻行起始地址间隔为8个DataBlock。
nd2nzParams.srcDValue = 8 * dataBlockElemCount;
// 目的NZ中同一源行的相邻DataBlock间隔为7个DataBlock。
nd2nzParams.dstNzC0Stride = 7;
// 目的NZ中相邻源行转换后的起始地址间隔为2个DataBlock。
nd2nzParams.dstNzNStride = 2;
// 仅1个NZ矩阵。
nd2nzParams.dstNzMatrixStride = 0;

AscendC::DataCopyPad(dstLocal, srcLocal, copyParams, nd2nzParams);
```
