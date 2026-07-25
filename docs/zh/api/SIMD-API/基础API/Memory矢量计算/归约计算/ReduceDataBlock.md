# ReduceDataBlock

## 产品支持情况

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

## 功能说明

头文件路径为：`"basic_api/kernel_operator_vec_reduce_intf.h"`。

如图1所示，`ReduceDataBlock`接口对输入数据以DataBlock为单位进行归约操作，根据模板参数`reduceType`，对每个DataBlock内的数据求和/求最大值/求最小值。

`ReduceDataBlock`对输入数据进行多次迭代计算，每个迭代会取出8个DataBlock（每个DataBlock数据块内部地址连续，大小为32字节）进行计算，输出结果会连续写入目的地址。

**图1**  `ReduceDataBlock`归约示意图

![ReduceDataBlock归约示意图](../../../../figures/ReduceDataBlock.png "ReduceDataBlock归约示意图")

## 函数原型

- `mask`逐bit模式：

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceDataBlock(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

- `mask`连续模式：

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceDataBlock(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| ------ | ---- |
| reduceType | 归约操作类型。<br>类型为`ReduceType`枚举类型，支持：<br>&bull; `ReduceType::SUM`：求和；<br>&bull; `ReduceType::MAX`：求最大值；<br>&bull; `ReduceType::MIN`：求最小值。 |
| T | 目的操作数数据类型。 |
| U | 源操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| -------- | -------- | ---- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| src | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| mask[]/mask | 输入 | `mask`用于控制每次迭代内参与计算的源操作数。详细设置参考[掩码概述](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 迭代次数。取值范围为[0, 255]。 |
| dstRepStride | 输入 | 目的操作数相邻迭代间的地址步长。<br>以一个`repeatTime`归约后的长度为单位，在`half`数据类型时，单位为16字节；在`float`数据类型时，单位为32字节。取值范围为[0, $2^{16}-1$]。<!-- npu="910" id29 --><br>**注意：Atlas 训练系列产品不支持配置0。**<!-- end id29 --> |
| srcBlkStride | 输入 | 源操作数单次迭代内DataBlock间的地址步长，单位为32字节。取值范围为[0, $2^{16}-1$]。 |
| srcRepStride | 输入 | 源操作数相邻迭代间的地址步长，即源操作数每次迭代跳过的DataBlock数目。取值范围为[0, $2^{16}-1$]。 |

**注：以上高维切分相关参数`mask`，`repeatTime`，`dstRepStride`，`srcBlkStride`，`srcRepStride`请参考[高维切分](../SIMD计算说明/高维切分.md)中的介绍。**

## 数据类型

支持的数据类型如下：

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT，支持half、float。
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持half、float。
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持half、float。
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品，支持half、float。
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品AI Core，支持half、float。
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas 训练系列产品，支持half。
<!-- end id15 -->
<!-- npu="x90" id16 -->
- Kirin X90，支持half、float。
<!-- end id16 -->
<!-- npu="9030" id17 -->
- Kirin 9030，支持half、float。
<!-- end id17 -->

目的操作数与源操作数的数据类型需要保持一致。

## 返回值说明

无

## 约束说明

- 源操作数的对齐约束请参见[通用地址对齐约束](../../../通用说明和约束.md#通用地址对齐约束)，起始地址需要32字节对齐；目的操作数起始地址的对齐约束与操作数数据类型有关，在`half`数据类型时需要保证16字节对齐，在`float`数据类型时需要保证32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#通用地址重叠约束)。
- `dstRepStride`、`srcBlkStride`、`srcRepStride`取值范围为[0, $2^{16}-1$]，需要结合UB的实际大小避免出现越界。

<!-- npu="950,A3,910b" id18 -->
- 当参数mask或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id19 -->
  - 针对如下型号，当参数mask或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id20 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id21 -->
  <!-- end id19 -->

  <!-- npu="950" id22 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数count或repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
  <!-- end id22 -->
<!-- end id18 -->

<!-- npu="950" id23 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - tensor高维切分计算占用8KB Unified Buffer。
  - tensor前n个数据连续计算不涉及8KB Unified Buffer的占用。
<!-- end id23 -->

<!-- npu="A3,910b" id24 -->
- 针对如下型号，若配置`mask[]/mask`参数后，存在某个DataBlock里的任何一个元素都不参与计算，不会有值写入对应目的操作数，保留目的操作数原有数据。
  <!-- npu="A3" id25 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id25 -->
  <!-- npu="910b" id26 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id26 -->
<!-- end id24 -->

<!-- npu="310b" id27 -->
- 针对Atlas 200I/500 A2 推理产品，若配置`mask[]/mask`参数后，存在某个DataBlock里的任何一个元素都不参与计算，则会以默认值填充对应目的操作数，默认值与`reduceType`和数据类型有关：
  - `reduceType == ReduceType::SUM`时填充0；
  - `reduceType == ReduceType::MAX`，操作数数据类型为`float`时填充-inf，操作数数据类型为`half`时填充-65504；
  - `reduceType == ReduceType::MIN`，操作数数据类型为`float`时填充inf，操作数数据类型为`half`时填充65504。
  - 比如`float`场景下，当`reduceType == ReduceType::MAX`，`mask=32`，即只计算前4个DataBlock，则后四个DataBlock内的最大值会返回-inf。
<!-- end id27 -->

<!-- npu="950" id28 -->
- 针对Ascend 950PR/Ascend 950DT，若配置`mask[]/mask`参数后，存在某个DataBlock里的任何一个元素都不参与计算，则会以默认值填充对应目的操作数，默认值与`reduceType`和数据类型有关：
  - `reduceType == ReduceType::SUM`时填充0；
  - `reduceType == ReduceType::MAX`时填充-inf；
  - `reduceType == ReduceType::MIN`时填充inf。
  - 比如`float`场景下，当`reduceType == ReduceType::MAX`，`mask=32`，即只计算前4个DataBlock，则后四个DataBlock内的最大值会返回-inf。
<!-- end id28 -->

- `float`数据类型只支持寄存器非饱和模式，`half`数据类型默认是寄存器非饱和模式。寄存器的非饱和/饱和模式具体配置方式参考[SetCtrlSpr(ISASI).md](../../特殊寄存器访问/SetCtrlSpr(ISASI).md)。
    - 下图说明`reduceType`取`ReduceType::SUM`时，在饱和模式下`half`数据类型的计算过程。源操作数为$[60000,60000,-30000,100]$，首先$60000+60000$溢出，结果为$65504$，然后计算$-30000+100=-29900$，最后计算$65504-29900=35604$。

        **图2**  `ReduceDataBlock<ReduceType::SUM>`饱和模式数值溢出情况处理说明

        ![ReduceDataBlock<ReduceType::SUM>饱和模式数值溢出情况处理说明](../../../../figures/vcgadd_overflow.png "ReduceDataBlock<ReduceType::SUM>饱和模式数值溢出情况处理说明")

## 关键特性说明

接口以二叉树的形式计算每个DataBlock内的结果。

以`half`类型的数据求和为例，在每个DataBlock内有16个数，通过二叉树的方式，两两相加，计算过程如下图所示：

1. data1和data2相加得到data01，data3和data4相加得到data02，...，data13和data14相加得到data07，data15和data16相加得到data08；
2. data01和data02相加得到data001，data03和data04相加得到data002，...，data07和data08相加得到data004；
3. 以此类推，得到目的操作数为1个`half`类型的数据sum。

**图3**  `ReduceDataBlock<ReduceType::SUM>`求和示意图

![ReduceDataBlock<ReduceType::SUM>求和示意图](../../../../figures/vcgadd_binary_add.png "ReduceDataBlock<ReduceType::SUM>求和示意图")

## 调用示例

本样例中只展示`Compute`流程中的部分代码。更多样例可参考[ReduceDataBlock系列归约指令样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_data_block)。

- `ReduceDataBlock<ReduceType::MAX>`-tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // 设定mask为最多的128个全部元素参与计算
    int32_t mask = 256/sizeof(half);
    // 每个repeat128个元素，一共128个元素。
    int repeat = 1;
    // dstLocal: 目的操作数tensor
    // srcLocal: 源操作数tensor
    // srcBlkStride = 1，在一个repeat中，block间没有空隙。
    // dstRepStride = 1，srcRepStride = 8，repeat间没有空隙。
    AscendC::ReduceDataBlock<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    结果示例如下：
    
    > 输入数据src_gm：

    ```plain
    [1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 2, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 3, 1, 1,
     ...
     1, 1, 1, 4, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
    ]
    ```

    > 输出数据dst_gm：[2, 3, ..., 4]

- `ReduceDataBlock<ReduceType::MAX>`-tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // 设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 每个repeat128个元素，一共128个元素。
    int repeat = 1;
    // dstLocal: 目的操作数tensor
    // srcLocal: 源操作数tensor
    // srcBlkStride = 1，在一个repeat中，block间没有空隙。
    // dstRepStride = 1，srcRepStride = 8，repeat间没有空隙。
    AscendC::ReduceDataBlock<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    结果示例如下：

    > 输入数据src_gm：

    ```plain
    [1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 2, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 3, 1, 1,
     ...
     1, 1, 1, 4, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
    ]
    ```

    > 输出数据dst_gm：[2, 3, ..., 4]

- `ReduceDataBlock<ReduceType::MIN>`-tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // 设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 每个repeat128个元素，一共128个元素。
    int repeat = 1;
    // dstLocal: 目的操作数tensor
    // srcLocal: 源操作数tensor
    // srcBlkStride = 1，在一个repeat中，block间没有空隙。
    // dstRepStride = 1，srcRepStride = 8，repeat间没有空隙。
    AscendC::ReduceDataBlock<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    结果示例如下：

    > 输入数据src_gm：

    ```plain
    [10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 2, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, -3, 10, 10,
     ...
     10, 10, 10, 10, 10, 10, 10, 10,
     10, 4, 10, 10, 10, 10, 10, 10,
    ]
    ```

    > 输出数据dst_gm：[2, -3, ..., 4]

- `ReduceDataBlock<ReduceType::SUM>`-tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // 设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 每个repeat128个元素，一共128个元素。
    int repeat = 1;
    // dstLocal: 目的操作数tensor
    // srcLocal: 源操作数tensor
    // srcBlkStride = 1，在一个repeat中，block间没有空隙。
    // dstRepStride = 1，srcRepStride = 8，repeat间没有空隙。
    ReduceDataBlock<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    结果示例如下：

    > 输入数据src_gm：

    ```plain
    [1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
     2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2,
     ...
     3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3]
    ```

    > 输出数据dst_gm：[16, 32, ..., 48]
