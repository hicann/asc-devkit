# ReduceMin

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

`ReduceMin`接口用于从所有输入数据中找出最小值和最小值索引。

`ReduceMin`计算过程如下图所示：首先，在每个repeat迭代中计算得到最小值和repeat内部索引，这些中间结果暂存于`sharedTmpBuffer`工作区中；然后，在中间结果的基础上继续按repeat迭代得到最终的最小值和最小值索引。需要注意的是，每次repeat迭代获取的最小值索引是repeat内部索引，返回最终结果时，需要根据迭代位置和repeat内部索引推导全量数据的最小值索引。

**图1**  `ReduceMin`计算示意图<a id="fig1"></a>

![ReduceMin计算示意图](../../../../figures/ReduceMin_scale.png "ReduceMin计算示意图")

## 函数原型

- tensor前n个数据计算：

    ```cpp
    template <typename T>
    __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count, bool calIndex = 0)
    ```

- tensor高维切分计算：
    - `mask`逐bit模式：

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
        ```

    - `mask`连续模式：

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
        ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| src | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| sharedTmpBuffer | 输入 | 指令执行期间存储中间结果，用于内部计算所需操作空间，需特别注意空间大小。详情请参考[关键特性说明](#关键特性说明)。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。最大处理的数据量不能超过UB大小限制。  |
| mask/mask[] | 输入 | `mask`用于控制每次迭代内参与计算的源操作数。详细设置参考[掩码概述](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 迭代次数。关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。**注：与高维切分中不同的是，`repeatTime`可以支持更大的取值范围，保证不超过int32_t的最大值即可。** |
| srcRepStride | 输入 | 源操作数相邻迭代间的地址步长，即源操作数每次迭代跳过的DataBlock数目。取值范围为[0, $2^{16}-1$]。 |
| calIndex | 输入 | 指定是否获取最小值的索引，bool类型，默认值为`false`，取值：<br>&bull; `true`：同时获取最小值和最小值索引。<br>&bull; `false`：不获取索引，只获取最小值。 |

**注：以上高维切分相关参数`mask`，`repeatTime`，`srcRepStride`请参考[高维切分](../SIMD计算说明/高维切分.md)中的介绍。**

## 数据类型

支持的数据类型如下：

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT，支持int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t。数据类型int64_t、uint64_t仅支持tensor前n个数据计算接口。
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

## 返回值说明

无

## 约束说明

- 源操作数及`sharedTmpBuffer`的地址对齐约束请参考[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)，起始地址需要32字节对齐；目的操作数的起始地址对齐约束请参考[ReduceRepeat-表3](./ReduceRepeat.md#tab3)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#通用地址重叠约束)。
- 需要使用`sharedTmpBuffer`的情况下，支持`dst`与`sharedTmpBuffer`地址重叠（通常情况下`dst`比`sharedTmpBuffer`所需的空间要小），此时`sharedTmpBuffer`必须满足所需空间要求，详情请参考[关键特性说明](#关键特性说明)。

<!-- npu="950,A3,910b" id18 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id19 -->
  - 针对如下型号，当参数count或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
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

- `srcRepStride`取值范围为[0, $2^{16}-1$]，需要结合UB的实际大小避免出现越界。
- 如果存在多个最小值，该指令会将最小索引写入目的操作数。
- `dst`结果存储顺序为最小值，最小值索引，若不需要索引，只会存储最小值。
- 索引按操作数的数据类型存储，读取索引需要将类型转换到整型。请参考[ReduceRepeat关键特性说明](./ReduceRepeat.md#关键特性说明)。
- 当输入类型是`half`的时候，只支持获取最大不超过65535（`uint16_t`能表示的最大值）的索引值。
<!-- npu="950" id23 -->
- 对于Ascend 950PR/Ascend 950DT，因接口内部算法实现不同，无需使用`sharedTmpBuffer`，可以直接传入`src`或者任意大小的`sharedTmpBuffer`。
<!-- end id23 -->

<!-- npu="A3,910b,910,310p,310b,x90,9030" id24 -->
- 针对如下型号，需要使用`sharedTmpBuffer`。
  <!-- npu="A3" id25 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id25 -->
  <!-- npu="910b" id26 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id26 -->
  <!-- npu="310b" id27 -->
  - Atlas 200I/500 A2 推理产品
  <!-- end id27 -->
  <!-- npu="310p" id28 -->
  - Atlas 推理系列产品AI Core
  <!-- end id28 -->
  <!-- npu="910" id29 -->
  - Atlas 训练系列产品
  <!-- end id29 -->
  <!-- npu="x90" id30 -->
  - Kirin X90
  <!-- end id30 -->
  <!-- npu="9030" id31 -->
  - Kirin 9030
  <!-- end id31 -->
<!-- end id24 -->

## 关键特性说明

- **索引值需要强制类型转换**，详情请参考[`ReduceRepeat`关键特性说明](./ReduceRepeat.md#关键特性说明)。

- **`sharedTmpBuffer`最小所需空间设置**：与`ReduceMax`类似，详情请参考[ReduceMax关键特性说明](./ReduceMax.md#关键特性说明)。

## 调用示例

更多样例请参考[ReduceMin样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation)。

- tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor高维切分计算接口，设定repeatTime为65，mask为全部元素参与计算
    int32_t mask = 128;
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor高维切分计算接口，设定repeatTime为65,mask为全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- tensor前n个数据计算样例：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor前n个数据计算接口
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320, true);
    ```

- <a name="li0437172884019"></a>tensor高维切分计算接口完整调用示例：

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 512;
    int dstDataSize = 512;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask;

    // 初始化srcLocal 、dstLocal 、sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // mask为128一次计算128个元素,4次repeat计算完512个数,calIndex为true，获取最小值的索引
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride, true);
    // 释放Tensor
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    示例结果如下：

    > 输入数据src_gm：

    ```plain
    [0.769    0.8584   0.1082   0.2715   0.1759   0.7646   0.6406   0.2944   0.4255   0.927    0.8022   0.04507  0.9688   0.919    0.3008   0.7144   0.3206   0.6753   0.8276
     ...
     0.304   ]
    ```

    > 输出数据dst_gm：[0.01034,  2.104e-05]
    > 2.104e-05需要使用reinterpret_cast方法转换得到索引值353

- tensor前n个数据计算接口完整调用示例：

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // 初始化srcLocal 、dstLocal 、sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // level2接口计算前288个数，calIndex为true，获取最小值的索引
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize, true);
    // 释放Tensor
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    示例结果如下：

    > 输入数据src_gm：

    ```plain
    [0.556    0.5225   0.3623   0.214    0.556    0.0643   0.769    0.594    0.261    0.3652   0.911    0.924    0.386    0.3696   0.2296   0.5957   0.1709   0.79     0.8516
     ...
     0.2242  ]
    ```

    > 输出数据dst_gm：[0.00393,  4.3e-06]
    > 4.3e-06需要使用reinterpret_cast方法转换得到索引值72
