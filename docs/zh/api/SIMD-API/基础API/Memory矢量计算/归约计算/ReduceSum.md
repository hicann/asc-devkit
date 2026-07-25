# ReduceSum

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

`ReduceSum`接口对所有输入数据进行求和，计算过程如下。

**图1** `ReduceSum`计算示意图<a id="fig1"></a>

![](../../../../figures/ReduceSum_basic_api.png "ReduceSum计算示意图")

## 函数原型

- tensor前n个数据计算：

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count)
    ```

- tensor高维切分计算：
    - `mask`逐bit模式：

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride)
        ```

    - `mask`连续模式：

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride)
        ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask模式和mask值。<br>&bull; true，表示在接口内部设置。<br>&nbsp; tensor前n个数据计算API内部使用了mask的[Counter模式](../SIMD计算说明/掩码.md#mask-mode)，一般情况下保持isSetMask默认值即可，表示在API内部根据开发者传入的count参数进行mask模式和mask值的设置。<br>&bull; false，表示在接口外部设置。<br>&nbsp; 开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。此时，接口入参中的count不生效，建议设置成1。<br>部分产品型号该参数不生效，详情请参考[约束说明](#约束说明)。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| src | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| sharedTmpBuffer | 输入 | 指令执行期间存储中间结果，用于内部计算所需操作空间，需特别注意空间大小。详情请参考[关键特性说明](#关键特性说明)。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN、VECCALC、VECOUT）。 |
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。最大处理的数据量不能超过UB大小限制。 |
| mask/mask[] | 输入 | `mask`用于控制每次迭代内参与计算的源操作数。详细设置参考[掩码概述](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 迭代次数。关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。**注：与高维切分中不同的是，`repeatTime`可以支持更大的取值范围，保证不超过int32_t的最大值即可。** |
| srcRepStride | 输入 | 源操作数相邻迭代间的地址步长，即源操作数每次迭代跳过的DataBlock数目。取值范围为[0, $2^{16}-1$]。 |

**注：以上高维切分相关参数`mask`，`repeatTime`，`srcRepStride`请参考[高维切分](../SIMD计算说明/高维切分.md)中的介绍。**

## 数据类型

支持的数据类型如下：

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT，支持half、float、int64_t、uint64_t。数据类型int64_t、uint64_t仅支持tensor前n个数据计算接口。
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

<!-- npu="950,910,310p,310b,x90,9030" id23 -->
- 针对以下型号，模板参数`isSetMask`参数不生效，保持默认值即可：
  <!-- npu="950" id24 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id24 -->
  <!-- npu="310b" id25 -->
  - Atlas 200I/500 A2 推理产品
  <!-- end id25 -->
  <!-- npu="310p" id26 -->
  - Atlas 推理系列产品AI Core
  <!-- end id26 -->
  <!-- npu="910" id27 -->
  - Atlas 训练系列产品
  <!-- end id27 -->
  <!-- npu="x90" id28 -->
  - Kirin X90
  <!-- end id28 -->
  <!-- npu="9030" id29 -->
  - Kirin 9030
  <!-- end id29 -->
<!-- end id23 -->

## 关键特性说明

**`ReduceSum`的累加方式分为两种：**

- 方式一：同一repeat内采用二叉树累加，不同repeat的结果按顺序累加。
- 方式二：同一repeat内采用二叉树累加，不同repeat的结果也按二叉树累加。
- 方式三：不同repeat间通过向量加法将数据合并为一个repeat，repeat内采用二叉树累加。

`ReduceSum`接口以二叉树累加的方式完成每个repeat内的求和，详情请参考[ReduceDataBlock关键特性说明](./ReduceDataBlock.md#关键特性说明)。

**对于上述两种累加方式，`sharedTmpBuffer`所需空间设置：**

- 方式一：传入任意大小的`sharedTmpBuffer`，`sharedTmpBuffer`不会被使用。
- 方式二：按照如下计算公式计算所需空间：

    ```cpp
    int typeSize = 2;                           // half类型为2Bytes，float类型为4Bytes
    int elementsPerBlock = 32 / typeSize;       // 1个datablock存放的元素个数
    int elementsPerRepeat = 256 / typeSize;     // 1次repeat可以处理的元素个数
    int firstMaxRepeat = repeatTime;            // 最后确定首次最大repeat值
    // 注：对于tensor高维切分计算接口，firstMaxRepeat就是repeatTime；对于tensor前n个数据计算接口，firstMaxRepeat为count/elementsPerRepeat，对于count小于elementsPerRepeat的场景，firstMaxRepeat为1
    int iter1OutputCount = firstMaxRepeat;                                              // 第一轮操作产生的元素个数
    int iter1AlignEnd = DivCeil(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // 第一轮产生的元素个数做向上取整
    int finalWorkLocalNeedSize = iter1AlignEnd; // 最终sharedTmpBuffer所需的elements空间大小就是第一轮操作产生元素做向上取整后的结果
    ```

**不同硬件形态对应的`ReduceSum`相加方式如下：**

<!-- npu="950" id30 -->
- Ascend 950PR/Ascend 950DT，按场景采用不同的累加方式：
  - int64_t、uint64_t采用方式三。
  - [Counter模式](../SIMD计算说明/掩码.md#mask-mode)且数据量在256~1024字节之间采用方式三；否则采用方式二。
  - [Normal模式](../SIMD计算说明/掩码.md#mask-mode)采用方式二。
<!-- end id30 -->
<!-- npu="A3" id31 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，tensor前n个数据计算接口采用方式一，tensor高维切分计算接口采用方式二。
<!-- end id31 -->
<!-- npu="910b" id32 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，tensor前n个数据计算接口采用方式一，tensor高维切分计算接口采用方式二。
<!-- end id32 -->
<!-- npu="310b" id33 -->
- Atlas 200I/500 A2 推理产品，采用方式二。
<!-- end id33 -->
<!-- npu="310p" id34 -->
- Atlas 推理系列产品AI Core，采用方式二。
<!-- end id34 -->
<!-- npu="910" id35 -->
- Atlas 训练系列产品，采用方式二。
<!-- end id35 -->
<!-- npu="x90" id36 -->
- Kirin X90，采用方式二。
<!-- end id36 -->
<!-- npu="9030" id37 -->
- Kirin 9030，采用方式二。
<!-- end id37 -->

## 调用示例

更多样例请参考[ReduceSum样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation)。

- tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，使用tensor高维切分计算接口，设定repeatTime为65，mask为全部元素参与计算
    int32_t mask = 128;
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8);
    ```

- tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，使用tensor高维切分计算接口，设定repeatTime为65，mask为全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8);
    ```

- tensor前n个数据计算样例：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，使用tensor前n个数据计算接口
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320);
    ```

- tensor高维切分计算接口示例：

    ```cpp
    #include "kernel_operator.h"
    int srcDataSize = 8320;
    int dstDataSize = 16;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask; // 这里是65

    // 初始化srcLocal 、dstLocal 、sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();
    // mask为128一次计算128个元素,65次repeat计算完8320个数
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride);
    // 释放Tensor
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    示例结果如下：

    > 输入数据src_gm：[1. 1. 1. ... 1. 1. 1.]
    >
    > 输出数据dst_gm：[8320.    0.    0.    0.    0.    0.    0.    0.    0.    0.   0.    0.    0.    0.    0.    0.]

- tensor前n个数据计算接口示例：

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // 初始化srcLocal 、dstLocal 、sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // level2接口计算前288个数，计算前288个数的和
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize);
    // 释放Tensor
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    示例结果如下：

    > 输入数据src_gm：[1. 1. 1. ... 1. 1. 1.]
    >
    > 输出数据dst_gm：[288.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.]
