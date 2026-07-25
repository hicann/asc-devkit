# ReduceRepeat

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

`ReduceRepeat`接口用于对每个repeat内所有数据进行归约操作，根据模板参数reduceType进行求和/求最大值/求最小值操作，结果按顺序写入目标地址。

在求最大值或最小值时可以返回对应的索引，返回的索引值为每个repeat内部索引。支持[高维切分](../SIMD计算说明/高维切分.md)计算。

**图1**  ReduceRepeat计算示意图

![](../../../../figures/ReduceRepeat.png "ReduceRepeat计算示意图")

## 函数原型

- `mask`逐bit模式：

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceRepeat(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
    ```

- `mask`连续模式：

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceRepeat(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| reduceType | 归约操作类型。<br>类型为`ReduceType`枚举类型，支持：<br>&bull; `ReduceType::SUM`：求和；<br>&bull; `ReduceType::MAX`：求最大值；<br>&bull; `ReduceType::MIN`：求最小值。 |
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
| dstRepStride | 输入 | 目的操作数相邻迭代间的地址步长，以一个repeatTime归约后的长度为单位。取值范围为[0, $2^{16}-1$]。<br>**注意：dstRepStride的单位受操作数数据类型、`reduceType`和`order`参数的影响**，参考[表3](#tab3)：<br>&bull;返回索引和最值时，单位为dst数据类型所占字节长度的两倍；<br>&bull;仅返回最值时，单位为dst数据类型所占字节长度；<br>&bull;仅返回索引时，单位为uint32_t类型所占字节长度。<!-- npu="910" id46 --><br>**注意：Atlas 训练系列产品不支持配置0。**<!-- end id46 --> |
| srcBlkStride | 输入 | 单次迭代内DataBlock的地址步长，单位为32字节。取值范围为[0, $2^{16}-1$]。 |
| srcRepStride | 输入 | 源操作数相邻迭代间的地址步长，即源操作数每次迭代跳过的DataBlock数目。取值范围为[0, $2^{16}-1$]。 |
| order | 输入 | 在归约操作类型为MAX或MIN时，指定dst中最大值value和索引值index的相对位置以及返回结果行为，参数类型为`ReduceOrder`枚举类型，默认值为`ORDER_VALUE_INDEX`。<br>`ReduceOrder`取值如下：<br>&bull; `ORDER_VALUE_INDEX`：表示value位于低半部，返回结果存储顺序为[value, index]。<br>&bull; `ORDER_INDEX_VALUE`：表示index位于低半部，返回结果存储顺序为[index, value]。<br>&bull; `ORDER_ONLY_VALUE`：表示只返回最值，返回结果存储顺序为[value]。<br>&bull; `ORDER_ONLY_INDEX`：表示只返回最值索引，返回结果存储顺序为[index]。<br>**注：归约操作类型为SUM时，该参数不生效。** |

**注：以上高维切分相关参数`mask`，`repeatTime`，`dstRepStride`，`srcBlkStride`，`srcRepStride`请参考[高维切分](../SIMD计算说明/高维切分.md)中的介绍。**

## 数据类型

**支持的数据类型如下：**

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT，支持int16_t、uint16_t、half、int32_t、uint32_t、float。
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

<!-- npu="950" id18 -->
**针对Ascend 950PR/Ascend 950DT：**

- 在`reduceType`取`MAX`或`MIN`时目的操作数与源操作数的数据类型需要保持一致。
- 在`reduceType`取`SUM`时，支持如下数据类型组合：

    | 目的操作数数据类型T |源操作数数据类型U |
    | --- | --- |
    | `int32_t` | `int16_t`，`int32_t` | 
    | `uint32_t` | `uint16_t`，`uint32_t` |
    | `half` | `half` |
    | `float` | `float` |
<!-- end id18 -->

<!-- npu="A3,910b,910,310p,310b,x90,9030" id19 -->
**针对如下型号，目的操作数与源操作数的数据类型需要保持一致。**

<!-- npu="A3" id20 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id20 -->
<!-- npu="910b" id21 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id21 -->
<!-- npu="310b" id22 -->
- Atlas 200I/500 A2 推理产品
<!-- end id22 -->
<!-- npu="310p" id23 -->
- Atlas 推理系列产品AI Core
<!-- end id23 -->
<!-- npu="910" id24 -->
- Atlas 训练系列产品
<!-- end id24 -->
<!-- npu="x90" id25 -->
- Kirin X90
<!-- end id25 -->
<!-- npu="9030" id26 -->
- Kirin 9030
<!-- end id26 -->
<!-- end id19 -->

## 返回值说明

无

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#通用地址重叠约束)。
- 源操作数的地址对齐约束请参考[通用地址对齐约束](../../../通用说明和约束.md#通用地址对齐约束)，起始地址需要32字节对齐；目的操作数的起始地址对齐约束受操作数数据类型、`reduceType`和`order`参数的影响，请参考[表3](#tab3)。

    **表3**  目的操作数地址对齐约束及dstRepStride单位

    <a id="tab3"></a>

    | 归约操作类型 | 数据类型 | 目的操作数地址对齐约束 | dstRepStride单位 |
    | --- | --- | --- | --- |
    | `SUM` | `half` | 2字节对齐 | 2字节 |
    | `SUM` | `float` | 4字节对齐 | 4字节 |

    | 归约操作类型 | 数据类型 | `order` | 目的操作数地址对齐约束 | dstRepStride单位 |
    | --- | --- | --- | --- | --- |
    | MAX/MIN | `half` | `ORDER_VALUE_INDEX` | 4字节对齐 | 4字节 |
    | MAX/MIN | `half` | `ORDER_INDEX_VALUE` | 4字节对齐 | 4字节 |
    | MAX/MIN | `half` | `ORDER_ONLY_VALUE` | 2字节对齐 | 2字节 |
    | MAX/MIN | `half` | `ORDER_ONLY_INDEX` | 4字节对齐 | 4字节 |
    | MAX/MIN | `float` | `ORDER_VALUE_INDEX` | 8字节对齐 | 8字节 |
    | MAX/MIN | `float` | `ORDER_INDEX_VALUE` | 8字节对齐 | 8字节 |
    | MAX/MIN | `float` | `ORDER_ONLY_VALUE` | 4字节对齐 | 4字节 |
    | MAX/MIN | `float` | `ORDER_ONLY_INDEX` | 4字节对齐 | 4字节 |

- `dstRepStride`、`srcBlkStride`、`srcRepStride`取值范围为[0, $2^{16}-1$]，需要结合UB的实际大小避免出现越界。

<!-- npu="950,A3,910b" id27 -->
- 当参数mask或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id28 -->
  - 针对如下型号，当参数mask或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id29 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id30 -->
  <!-- end id28 -->

  <!-- npu="950" id31 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数mask或repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
  <!-- end id31 -->
<!-- end id27 -->

<!-- npu="950" id32 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - tensor高维切分计算占用8KB Unified Buffer。
  - tensor前n个数据连续计算不涉及8KB Unified Buffer的占用。
<!-- end id32 -->
- 归约操作类型取MAX或MIN时的约束：
  - 如果存在多个最值，该指令会将最小索引写入目的操作数。
  - 索引按操作数的数据类型存储，读取索引需要将类型转换到整型，请参考[关键特性说明](#关键特性说明)。
  - 不同芯片支持的`ReduceOrder`如下：
    <!-- npu="950" id33 -->
    - Ascend 950PR/Ascend 950DT，支持`ORDER_VALUE_INDEX`、`ORDER_INDEX_VALUE`、`ORDER_ONLY_VALUE`、`ORDER_ONLY_INDEX`。
    <!-- end id33 -->
    <!-- npu="A3" id34 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持`ORDER_VALUE_INDEX`、`ORDER_INDEX_VALUE`、`ORDER_ONLY_VALUE`、`ORDER_ONLY_INDEX`。
    <!-- end id34 -->
    <!-- npu="910b" id35 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持`ORDER_VALUE_INDEX`、`ORDER_INDEX_VALUE`、`ORDER_ONLY_VALUE`、`ORDER_ONLY_INDEX`。
    <!-- end id35 -->
    <!-- npu="310b" id36 -->
    - Atlas 200I/500 A2 推理产品，支持`ORDER_VALUE_INDEX`、`ORDER_ONLY_VALUE`。
    <!-- end id36 -->
    <!-- npu="310p" id37 -->
    - Atlas 推理系列产品AI Core，支持`ORDER_VALUE_INDEX`、`ORDER_INDEX_VALUE`。
    <!-- end id37 -->
    <!-- npu="910" id38 -->
    - Atlas 训练系列产品，支持`ORDER_VALUE_INDEX`。
    <!-- end id38 -->
    <!-- npu="x90" id39 -->
    - Kirin X90，支持`ORDER_VALUE_INDEX`、`ORDER_INDEX_VALUE`。
    <!-- end id39 -->
    <!-- npu="9030" id40 -->
    - Kirin 9030，支持`ORDER_VALUE_INDEX`、`ORDER_INDEX_VALUE`。
    <!-- end id40 -->

## 关键特性说明

- **索引值需要强制类型转换**：

    dst的最值索引按照最值的数据类型存储，比如dst为half类型时，索引按照half类型存储，因此读取索引需要使用`reinterpret_cast`方法转换到整数类型。若最值数据类型是half，需要使用`reinterpret_cast<uint16_t*>`；若最值数据类型是float，需要使用`reinterpret_cast<uint32_t*>`。

    例如，输入数据是half类型，计算结果为[0.9985,  6.8e-06]，6.8e-06需要使用`reinterpret_cast<uint16_t*>`方法转换得到索引值114。转换示例如下：

    ```cpp
    half maxIndex = dst.GetValue(1);
    uint16_t realIndex = *reinterpret_cast<uint16_t*>(&maxIndex);
    ```

    <!-- npu="950,A3,910b" id41 -->

    特别地
    <!-- npu="950" id42 -->
    - 针对Ascend 950PR/Ascend 950DT，`ORDER_ONLY_INDEX`（仅返回最值索引）情况下，当操作数数据类型为`int16_t`，`uint16_t`，`half`时，读取index都需要使用`reinterpret_cast<uint32_t*>`。
    <!-- end id42 -->

    <!-- npu="A3,910b" id43 -->
    - 针对如下型号，`ORDER_ONLY_INDEX`（仅返回最值索引）情况下，读取索引值index时都需要使用`reinterpret_cast<uint32_t*>`。
      <!-- npu="A3" id44 -->
      - Atlas A3 训练系列产品/Atlas A3 推理系列产品
      <!-- end id44 -->
      <!-- npu="910b" id45 -->
      - Atlas A2 训练系列产品/Atlas A2 推理系列产品
      <!-- end id45 -->
    <!-- end id43 -->

    <!-- end id41 -->

- 归约操作类型取`SUM`时接口以二叉树累加的方式完成每个repeat内的求和，详情请参考[ReduceDataBlock关键特性说明](./ReduceDataBlock.md#关键特性说明)。

## 调用示例

更多样例可参考[ReduceRepeat样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_repeat)。

- `ReduceRepeat<ReduceType::MAX>` tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // dstLocal,srcLocal均为half类型，srcLocal的计算数据量为512，连续排布，计算结果也需要连续排布，使用tensor高维切分计算接口，设定mask为最多的128个全部元素参与计算
    // 根据以上信息，推断出repeatTime为4，dstRepStride为1，srcBlkStride为1，srcRepStride为8
    // 若求最大值及索引，并且需要存储顺序为[value, index]的结果，可以使用默认order，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8);

    // 若求最大值及索引，并且需要存储顺序为[index, value]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);

    // 若只求最大值，并且需要存储[value]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);

    // 若只求索引，并且需要存储[index]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

- `ReduceRepeat<AscendC::ReduceType::MAX>` tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal均为half类型，srcLocal的计算数据量为512，连续排布，计算结果也需要连续排布，使用tensor高维切分计算接口，设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };

    // 根据以上信息，推断出repeatTime为4，dstRepStride为1，srcBlkStride为1，srcRepStride为8

    // 若求最大值及索引，并且需要存储顺序为[value, index]的结果，使用默认order，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);

    // 若求最大值及索引，并且需要存储顺序为[index, value]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);

    // 若只求最大值，并且需要存储[value]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);

    // 若只求索引，并且需要存储[index]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

    示例结果如下：

    > 输入数据src_gm：

    ```plain
    [1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
     1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
     1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
     1   1   1   1   11   1   1   1   1   1   1   1   1   1   1   1
     ...
     2   2   2   2   2   2   2   2   2   2   2   2   2   2   2   2
     2   2   2   2   12   2   2   2   2   2   2   2   2   2   2   2
     ...
     3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3
     3   3   3   13   3   3   3   3   3   3   3   3   3   3   3   3
     3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3
     3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3]
    ```

    > 输出数据dst_gm：
    > - 若`ReduceOrder`类型为`ORDER_VALUE_INDEX`或默认：[11 3.09944e-06 12 5.96046e-06 ... 13 1.13249e-06]
    > - 若`ReduceOrder`类型为`ORDER_INDEX_VALUE`：[3.09944e-06 11 5.96046e-06 12 ... 1.13249e-06 13]
    > - 若`ReduceOrder`类型为`ORDER_ONLY_VALUE`：[11 12 ... 13 0 0 0 ...]
    > - 若`ReduceOrder`类型为`ORDER_ONLY_INDEX`：[3.09944e-06 0 5.96046e-06 0 ... 1.13249e-06 0]
    >
    > 其中，index的值为int数值的二进制在half中的表达，以上述结果为例：
    > - 前128个数中，11的位置在对应的repeat中为52，十六进制为0x0034，对应half值为3.09944e-06。
    > - 第二个128个数中，12的位置在对应的repeat中为100，十六进制为0x0064，对应half值为5.96046e-06。
    > - 最后128个数中，13的位置在对应的repeat中为19，十六进制为0x0013，对应half值为1.13249e-06。

- `ReduceRepeat<AscendC::ReduceType::MIN>` tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal均为half类型，srcLocal的计算数据量为512，连续排布，计算结果也需要连续排布，使用tensor高维切分计算接口，设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    // 根据以上信息，推断出repeatTime为4，dstRepStride为1，srcBlkStride为1，srcRepStride为8
    // 若求最小值及索引，并且需要存储顺序为[value, index]的结果，使用默认order，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);
    
    // 若求最小值及索引，并且需要存储顺序为[index, value]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);
    
    // 若只求最小值，并且需要存储[value]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);
    
    // 若只求索引，并且需要存储[index]的结果，接口示例为：
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

    示例结果如下：

    > 输入数据src_gm：

    ```plain
    [10  10  10  10  10  10  10  10  10  10  10  10  10  10  10  10
     10  10  10  10  10  10  10  10  10  10  10  10  10  10  10  10
     10  10  10  10  10  10  10  10  10  10  10  10  10  10  10  10
     10  10  10  10  1   10  10  10  10  10  10  10  10  10  10  10
     ...
     20  20  20  20  20  20  20  20  20  20  20  20  20  20  20  20
     20  20  20  20  2   20  20  20  20  20  20  20  20  20  20  20
     ...
     30  30  30  30  30  30  30  30  30  30  30  30  30  30  30  30
     30  30  30  3   30  30  30  30  30  30  30  30  30  30  30  30
     30  30  30  30  30  30  30  30  30  30  30  30  30  30  30  30
     30  30  30  30  30  30  30  30  30  30  30  30  30  30  30  30]
    ```

    > 输出数据dst_gm：
    > - 若`ReduceOrder`类型为`ORDER_VALUE_INDEX`或默认：[1 3.09944e-06 2 5.96046e-06 ... 3 1.13249e-06]
    > - 若`ReduceOrder`类型为`ORDER_INDEX_VALUE`：[3.09944e-06 1 5.96046e-06 2 ... 1.13249e-06 3]
    > - 若`ReduceOrder`类型为`ORDER_ONLY_VALUE`：[1 2 ... 3 0 0 0 ...]
    > - 若`ReduceOrder`类型为`ORDER_ONLY_INDEX`：[3.09944e-06 0 5.96046e-06 0 ... 1.13249e-06 0]
    >
    > 其中，index的值为int数值的二进制在half中的表达，以上述结果为例：
    > - 前128个数中，1的位置在对应的repeat中为52，十六进制为0x0034，对应half值为3.09944e-06。
    > - 第二个128个数中，2的位置在对应的repeat中为100，十六进制为0x0064，对应half值为5.96046e-06。
    > - 最后128个数中，3的位置在对应的repeat中为19，十六进制为0x0013，对应half值为1.13249e-06。

- `ReduceRepeat<ReduceType::SUM>` tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal均为half类型，srcLocal的计算数据量为512，连续排布，计算结果也需要连续排布，使用tensor高维切分计算接口，设定mask为最多的128个全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    // 根据以上信息，推断出repeatTime为4，dstRepStride为1，srcBlkStride为1，srcRepStride为8
    AscendC::ReduceRepeat<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);
    ```
