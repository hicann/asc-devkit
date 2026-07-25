# ReducePairElem

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

## 功能说明

头文件路径为：`"basic_api/kernel_operator_vec_reduce_intf.h"`。

`ReducePairElem`接口根据模板参数`reduceType`，对相邻两个（奇偶）元素进行归约操作，结果按顺序写入目标地址。**当前仅支持求和操作。**

例如，[a1, a2, a3, a4, a5, a6, ...]的求和结果为[a1+a2, a3+a4, a5+a6, ...]。计算过程如下图所示，灰色部分代表无效元素。

**图1**  `ReducePairElem<ReduceType::SUM>`示意图

![ReducePairElem<ReduceType::SUM>示意图](../../../../figures/ReducePairElem.png "ReducePairElem<ReduceType::SUM>示意图")

## 函数原型

- `mask`逐bit模式：

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReducePairElem(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

- `mask`连续模式：

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReducePairElem(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| reduceType | 归约操作类型。<br>类型为`ReduceType`枚举类型，支持：<br>&bull; `ReduceType::SUM`：求和。 |
| T | 目的操作数数据类型。 |
| U | 源操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| src | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| mask[]/mask | 输入 | `mask`用于控制每次迭代内参与计算的源操作数。详细设置参考[掩码概述](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 迭代次数。取值范围为[0, 255]。 |
| dstRepStride | 输入 | 目的操作数相邻迭代间的地址步长，以一个repeatTime归约后的长度为单位，即128字节。取值范围为[0, $2^{16}-1$]。<!-- npu="910" id28 --><br>**注意，Atlas 训练系列产品不支持此参数值配为0。**<!-- end id28 --> |
| srcBlkStride | 输入 | 单次迭代内DataBlock的地址步长，单位为32字节。取值范围为[0, $2^{16}-1$]。 |
| srcRepStride | 输入 | 源操作数相邻迭代间的地址步长，即源操作数每次迭代跳过的DataBlock数目。取值范围为[0, $2^{16}-1$]。 |

**注：以上高维切分相关参数`mask`，`repeatTime`，`dstRepStride`，`srcBlkStride`，`srcRepStride`请参考[高维切分](../SIMD计算说明/高维切分.md)中的介绍。**

## 数据类型

支持的数据类型如下：

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT，支持half、float。
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持half、float。
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持half、float。
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 推理产品，支持half、float。
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core，支持half、float。
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas 训练系列产品，支持half。
<!-- end id13 -->

目的操作数与源操作数的数据类型需要保持一致。

## 返回值说明

无

## 约束说明

- 操作数的起始地址对齐约束请参考[通用地址对齐约束](../../../通用说明和约束.md#通用地址对齐约束)，需要32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#通用地址重叠约束)。
- `dstRepStride`、`srcBlkStride`、`srcRepStride`取值范围为[0, $2^{16}-1$]，需要结合UB的实际大小避免出现越界。

<!-- npu="950,A3,910b" id14 -->
- 当参数mask或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id15 -->
  - 针对如下型号，当参数mask或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id16 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id17 -->
  <!-- end id15 -->

  <!-- npu="950" id18 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数mask或repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
  <!-- end id18 -->
<!-- end id14 -->

<!-- npu="950" id19 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - tensor高维切分计算占用8KB Unified Buffer。
  - tensor前n个数据连续计算不涉及8KB Unified Buffer的占用。
<!-- end id19 -->

<!-- npu="950,A3,910b,310p,310b" id20 -->
- 当一对相邻元素的掩码均为0时，该接口的行为如下：
  <!-- npu="A3,910b,310p" id21 -->
  - 针对如下型号，对应的目的元素将跳过写入，保持不变。
    <!-- npu="A3" id22 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id23 -->
    <!-- npu="310p" id24 -->
    - Atlas 推理系列产品AI Core
    <!-- end id24 -->
  <!-- end id21 -->

  <!-- npu="950,310b" id25 -->
  - 针对如下型号，对应的目的操作数中的值会置为0。
    <!-- npu="950" id26 -->
    - Ascend 950PR/Ascend 950DT
    <!-- end id26 -->
    <!-- npu="310b" id27 -->
    - Atlas 200I/500 A2 推理产品
    <!-- end id27 -->
  <!-- end id25 -->
<!-- end id20 -->

## 调用示例

本样例中只展示`Compute`流程中的部分代码。更多样例可参考[ReducePairElem样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_pair_elem)。

- `ReducePairElem` tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // 设定mask为最多的128个全部元素参与计算
    int32_t mask = 256/sizeof(half);
    // 每个repeat128个元素，一共128个元素。
    int repeat = 1;
    // dstLocal: 目的操作数tensor
    // srcLocal: 源操作数tensor
    // srcBlkStride = 1，在一个repeat中，block间没有空隙。
    // dstRepStride = 1，srcRepStride = 8，repeat间没有空隙。
    AscendC::ReducePairElem<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

- `ReducePairElem` tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // 设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 每个repeat128个元素，一共128个元素。
    int repeat = 1;
    // dstLocal: 目的操作数tensor
    // srcLocal: 源操作数tensor
    // srcBlkStride = 1，在一个repeat中，block间没有空隙。
    // dstRepStride = 1，srcRepStride = 8，repeat间没有空隙。
    AscendC::ReducePairElem<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

- 示例结果

    > 输入数据src_gm：

    ```plain
    [1, 1, 1, -1, 2, 2, -1, 2,
     3, 3, 3, -1, 4, 4, -2, 4,
     ....
    ]
    ```

    > 输出数据dst_gm：
    
    ```plain
    [2, 0, 4, 1, 6, 2, 8, 2,
    ....
    ]
    ```
