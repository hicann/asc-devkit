# DataCopyPad（UBToGM非对齐数据搬运）<a name="ZH-CN_TOPIC_0000001894460401"></a>

## 产品支持情况<a name="section1550532418810"></a>

### 不支持数据搬运模式mode的原型

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
- Atlas 200I/500 A2 推理产品：支持
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

### 支持数据搬运模式mode的原型

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas 训练系列产品：不支持
<!-- end id14 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口提供将数据从Unified Buffer非对齐搬运至Global Memory的功能。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Global Memory
    - VECIN -> GM
    - VECOUT -> GM

- 如[图1](#fig_datacopypad3)所示，对于32字节对齐搬运场景，从Unified Buffer读取的所有数据都会搬运至Global Memory。
- 如[图2](#fig_datacopypad4)所示，对于非32字节对齐的场景，在读取Unified Buffer数据时会填入dummy假数据，对齐到32B，搬入Global Memory时会将dummy空数据丢弃，从而实现Unified Buffer到Global Memory的非对齐搬运。

## 函数原型<a name="section620mcpsimp"></a>

- 不支持配置设置数据搬运模式mode

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- npu="950" id15 -->

- 支持配置设置数据搬运模式mode（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    // 该函数原型仅支持Ascend 950PR/Ascend 950DT
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- end id15 -->

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 操作数的数据类型。各产品支持的数据类型请参考[数据类型](#section4219135304818)。 |
| mode | 配置数据搬运模式。PaddingMode类型，定义如下：<br><pre>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // 默认模式，与原有数据搬运格式保持一致，每次数据搬运都会补齐至32字节对齐<br>    Compact,     // 紧凑模式，允许单次搬运不对齐，统一在整块数据末尾补齐至32字节对齐<br>};</pre> |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数，类型为[GlobalTensor](../../数据结构/GlobalTensor/GlobalTensor简介.md)。<br>起始地址无地址对齐约束。 |
| src | 输入 | 源操作数，类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)。<br>起始地址需要保证32字节对齐。 |
| dataCopyParams | 输入 | 搬运参数，DataCopyExtParams类型，具体参数说明请参考[表3](#table_ub2gm_pad_3)。 |

下文表格中列出的结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。

**表3**  DataCopyExtParams结构体参数定义<a name="table_ub2gm_pad_3"></a>

| 参数名 | 描述 |
| :--- | :--- |
| blockCount | 指定该指令包含的连续传输数据块个数，数据类型为uint16_t，取值范围：blockCount∈[0, 4095]。 |
| blockLen | 指定该指令每个连续传输数据块长度，**该指令支持非对齐搬运**，**每个连续传输数据块长度单位为字节**。数据类型为uint32_t，取值范围：blockLen∈[0, 2097151]**，blockLen必须是sizeof(T)的整数倍，需要注意不要超过UB空间大小**。 |
| srcStride | 源操作数，相邻连续数据块的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值）。<br>源操作数的逻辑位置为VECIN/VECOUT，单位为dataBlock（32字节）。<br>数据类型为uint32_t，取值范围为[0, 2^32-1]。不同产品中srcStride的数据类型和支持的取值范围可能不同，详细请参考[约束说明](#section633mcpsimp)。 |
| dstStride | 目的操作数，相邻连续数据块间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值）。<br>目的操作数的逻辑位置为GM，单位为字节。<br>数据类型为uint32_t，取值范围为[0, 2^32-1]。不同产品中dstStride的数据类型和支持的取值范围可能不同，详细请参考[约束说明](#section633mcpsimp)。 |
| rsv | 保留字段。 |

下面通过两个场景介绍Unified Buffer到Global Memory的非对齐搬运，分别对应32字节对齐和非32字节对齐：

- 32字节对齐场景<a name="32字节对齐场景"></a>

    如[图1](#fig_datacopypad3)所示，blockLen为64，每个连续传输数据块包含64字节；srcStride为1，源操作数的逻辑位置为VECIN/VECOUT，srcStride的单位为dataBlock（32字节），即源操作数相邻数据块之间间隔1个dataBlock；dstStride为1，目的操作数的逻辑位置为GM，dstStride的单位为字节，即目的操作数相邻数据块之间间隔1字节。

    对于32字节对齐搬运场景，从Unified Buffer读取的所有数据都会搬运至Global Memory。

    **图1**  blockLen为32字节对齐时Unified Buffer到Global Memory的非对齐搬运示意图<a name="fig_datacopypad3"></a>  
    ![](../../../../figures/datacopypad3.png)

- 非32字节对齐场景<a name="非32字节对齐场景"></a>

    如[图2](#fig_datacopypad4)所示，blockLen为47，每个连续传输数据块包含47字节，不满足32字节对齐；srcStride为1，表示源操作数相邻数据块之间间隔1个dataBlock；dstStride为1，表示目的操作数相邻数据块之间间隔1字节。

    对于非32字节对齐的场景，由于Unified Buffer要求32字节对齐，框架在搬出时会自动补充17字节的假数据来保证对齐，搬到Global Memory时再自动将填充的假数据丢弃掉，从而实现Unified Buffer到Global Memory的非对齐搬运。

    **图2**  blockLen不满足32字节对齐时Unified Buffer到Global Memory的非对齐搬运示意图<a name="fig_datacopypad4"></a>  
    ![](../../../../figures/datacopypad4.png)

## 数据类型<a name="section4219135304818"></a>

<!-- npu="950" id16 -->

- Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。

<!-- end id16 -->

<!-- npu="A3" id17 -->

- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id17 -->

<!-- npu="910b" id18 -->

- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id18 -->

<!-- npu="310b" id19 -->

- Atlas 200I/500 A2 推理产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

<!-- end id19 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 位于Unified Buffer的源地址必须32字节对齐，位于Global Memory的目的地址必须1字节对齐。
- DataCopyExtParams结构体参数的值需在取值范围内：

    **表4**  DataCopyExtParams结构体参数取值范围

    | 参数名 | 取值范围 |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 2097151] |
    | srcStride | [0, 2^32 - 1] |
    | dstStride | [0, 2^32 - 1] |

    <!-- npu="950" id20 -->

    > [!NOTE]说明
    > 特别地，针对Ascend 950PR/Ascend 950DT，srcStride和dstStride的数据类型和取值范围如下：
    > - srcStride：数据类型为int64_t，取值范围为[0, 65535]。
    > - dstStride：数据类型为int64_t，取值范围为[0, 2^40-1]。

    <!-- end id20 -->

<!-- npu="A3,910b,950" id24 -->
- 当DataCopyExtParams结构体参数blockCount、blockLen任意一个值为0时，该接口将被视为NOP（空操作）。该说明针对如下型号生效：
  <!-- npu="A3" id21 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id21 -->
  <!-- npu="910b" id22 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id22 -->
  <!-- npu="950" id23 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id23 -->
<!-- end id24 -->

## 调用示例<a name="section177231425115410"></a>

```cpp
AscendC::DataCopyExtParams copyParams{1, dstTotalLength * sizeof(T), 0, 0, 0};
AscendC::DataCopyPad(dstGlobal, srcLocal, copyParams);
```

完整样例请参考[DataCopyPad样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm)。
