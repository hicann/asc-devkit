# ReduceMax

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

`ReduceMax`接口用于从所有输入数据中找出最大值和最大值索引。

`ReduceMax`计算过程如下图所示：首先，在每个repeat迭代中计算得到最大值和repeat内部索引，这些中间结果暂存于`sharedTmpBuffer`工作区中；然后，在中间结果的基础上继续按repeat迭代得到最终的最大值和最大值索引。需要注意的是，每次repeat迭代获取的最大值索引是repeat内部索引，返回最终结果时，需要根据迭代位置和repeat内部索引推导全量数据的最大值索引。

**图1**  `ReduceMax`计算示意图<a id="fig1"></a>

![ReduceMax计算示意图](../../../../figures/ReduceMax_scale.png "ReduceMax计算示意图")

## 函数原型

- tensor前n个数据计算：

    ```cpp
    template <typename T>
    __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count, bool calIndex = 0)
    ```

- tensor高维切分计算：
    - `mask`逐bit模式：

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
        ```

    - `mask`连续模式：

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
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
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。最大处理的数据量不能超过UB大小限制。 |
| mask/mask[] | 输入 | `mask`用于控制每次迭代内参与计算的源操作数。详细设置参考[掩码概述](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 迭代次数。关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。**注：与高维切分中不同的是，`repeatTime`可以支持更大的取值范围，保证不超过int32_t的最大值即可。** |
| srcRepStride | 输入 | 源操作数相邻迭代间的地址步长，即源操作数每次迭代跳过的DataBlock数目。取值范围为[0, $2^{16}-1$]。 |
| calIndex | 输入 | 指定是否获取最大值的索引，bool类型，默认值为`false`，取值：<br>&bull; `true`：同时获取最大值和最大值索引。<br>&bull; `false`：不获取索引，只获取最大值。 |

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
- 如果存在多个最大值，该指令会将最小索引写入目的操作数。
- `dst`结果存储顺序为最大值，最大值索引，若不需要索引，只会存储最大值。
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

- **索引值需要强制类型转换**，详情请参考[ReduceRepeat关键特性说明](./ReduceRepeat.md#关键特性说明)。

- **`sharedTmpBuffer`所需空间设置：**

    `sharedTmpBuffer`空间需要开发者申请并传入，根据是否需要获取索引，`sharedTmpBuffer`空间计算方式不同：需要返回索引的情况下，需要把每轮计算所需的空间进行累加，同时每轮计算的空间都要考虑UB空间32字节对齐的要求；无需返回索引的情况下，只需要提供第一轮计算所需的空间并满足32字节对齐要求即可，后续的轮次可以直接使用这块空间，此时不需要推导索引的过程，所以之前轮次的中间数据可以直接覆盖。计算所需空间的算法如下：

    - 无需返回最大值索引：

        ```cpp
        int firstMaxRepeat = repeatTime; // 对于tensor高维切分计算接口，firstMaxRepeat就是repeatTime；对于tensor前n个数据计算接口，firstMaxRepeat为count/elementsPerRepeat
        int iter1OutputCount = firstMaxRepeat * 2; // 第一轮操作产生的元素个数，无论开发者是否需要返回索引，底层指令都会返回索引，所以这里要为索引预留空间，产生的元素个数为repeat次数*2
        int iter1AlignEnd = DivCeil(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // 第一轮产生的元素个数按照datablock(32字节)向上对齐
        int finalWorkLocalNeedSize = iter1AlignEnd; // 第一轮计算完成后，后续可能还需要多轮迭代，但是可以复用同一块空间，所以第一轮计算所需的空间就是最终sharedTmpBuffer所需的空间大小
        ```

    - 需要返回最大值索引：

        ```cpp
        int firstMaxRepeat = repeatTime; 
        // 对于tensor高维切分计算接口，firstMaxRepeat就是repeatTime；对于tensor前n个数据计算接口，firstMaxRepeat为count/elementsPerRepeat
        int iter1OutputCount = firstMaxRepeat * 2;                                            // 第一轮操作产生的元素个数
        int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // 第二轮操作起始位置偏移，即第一轮产生的元素个数按照datablock(32字节)向上对齐的结果
        // 第一轮计算完成后，后续可能还需要多轮迭代，此时不可以复用同一块空间，因为第一轮的中间结果索引还需要再进行使用，所以需要继续准备后续轮次的空间
        int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat) * 2;              // 第二轮操作产生的元素个数
        int iter2AlignEnd = RoundUp(iter2OutputCount, elementsPerBlock) * elementsPerBlock;   // 第二轮产生的元素个数按照datablock(32字节)向上对齐的结果
        int finalWorkLocalNeedSize = iter2AlignStart + iter2AlignEnd;                         // 第二轮结束即可获取最大值和索引时，最终sharedTmpBuffer所需的空间大小
        if (iter2OutputCount > 2) {                                                           // 第二轮操作产生的元素个数大于2时，需要继续进行第三轮操作
            int iter3AlignStart = iter2AlignEnd;                                              // 第三轮操作相对第二轮输出空间的起始位置偏移
            int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat) * 2;          // 第三轮操作产生的元素个数
            int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock; // 第三轮产生的元素个数按照datablock(32字节)向上对齐的结果
            finalWorkLocalNeedSize = iter2AlignStart + iter3AlignStart + iter3AlignEnd;       // 最终sharedTmpBuffer所需的空间大小
        }
        ```

    以上计算出来的最终的空间大小单位是元素个数，若转成Bytes数表示为`finalWorkLocalNeedSize * typeSize`(Bytes)。

    > [!NOTE]说明
    > 开发者为了节省地址空间，可以选择`sharedTmpBuffer`空间复用源操作数的空间。此时因为`sharedTmpBuffer`需要的最小空间一定小于源操作数的空间，所以无需关注和计算最小空间。

## 调用示例<a name="section107745237168"></a>

详细示例请参考[ReduceMax样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation)。

- tensor高维切分计算样例-`mask`连续模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor高维切分计算接口，设定repeatTime为65，mask为全部元素参与计算
    int32_t mask = 128;
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- tensor高维切分计算样例-`mask`逐bit模式：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor高维切分计算接口，设定repeatTime为65,mask为全部元素参与计算
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- tensor前n个数据计算样例：

    ```cpp
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,并且连续排布，需要索引值，使用tensor前n个数据计算接口
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320, true);
    ```

- `sharedTmpBuffer`空间计算示例：

    ```cpp
    // ReduceMax接口sharedTmpBuffer计算示例一：
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为8320,使用tensor高维切分计算接口, repeatTime为65, mask为128,需要索引值
    // tensor高维切分计算接口调用示例为：
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 65, 8, true);
    // 此时sharedTmpBuffer所需的最小空间计算过程为：
    int RoundUp(int a, int b)
    {
        return (a + b - 1) / b;
    }
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 130;                                          // 第一轮操作产生的元素个数
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 144; // 对第一轮操作输出个数向上取整
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 4;                // 第二轮操作产生的元素个数
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // 对第二轮操作输出个数向上取整
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // 第三轮操作产生的元素个数
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;  // 第三轮产生的元素个数做向上取整
    // 最终sharedTmpBuffer所需的最小空间为iter2AlignStart + iter3AlignStart + iter3AlignEnd = 144 + 16 + 16 = 176 ,也就是352Bytes
    // ReduceMax接口sharedTmpBuffer计算示例二：
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为32640,使用tensor高维切分计算接口,repeatTime为255, mask为128,需要索引值
    // tensor高维切分计算接口调用示例为：
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 255, 8, true);
    // 此时sharedTmpBuffer所需的最小空间计算过程为：
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 510;                                          // 第一轮操作产生的元素个数
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 512; // 对第一轮操作输出个数向上取整
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 8;                // 第二轮操作产生的元素个数
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // 对第二轮操作输出个数向上取整
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // 第三轮操作产生的元素个数
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;  // 第三轮产生的元素个数做向上取整
    // 需要的空间为iter2AlignStart + iter3AlignStart + iter3AlignEnd = 512 + 16 + 16 = 544 ,也就是1088Bytes
    // ReduceMax接口sharedTmpBuffer计算示例三：
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量为65408,使用tensor前n个数据计算接口,count=65408,需要索引值
    // tensor前n个数据计算接口调用示例为：
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 65408, true);
    // 此时sharedTmpBuffer所需的最小空间计算过程为：
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = count / elementsPerRepeat = 511;
    int iter1OutputCount = firstMaxRepeat * 2 = 1022;                                          // 第一轮操作产生的元素个数
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 1024; // 对iter1OutputCount输出个数向上取整
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 16;                // 第二轮操作产生的元素个数
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;   // 对iter2OutputCount输出个数向上取整
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                 // 第三轮操作产生的元素个数
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;   // 第三轮产生的元素个数做向上取整
    // 需要的空间为iter2AlignStart + iter3AlignStart + iter3AlignEnd = 1024 + 16 + 16 = 1056,也就是2112Bytes
    // ReduceMax接口sharedTmpBuffer计算示例四：
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型，srcLocal的计算数据量为512，使用tensor高维切分计算接口,repeatTime为4, mask为128,需要索引值
    // tensor高维切分计算接口调用示例为：
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 4, 8, true);
    // 此时sharedTmpBuffer所需的最小空间计算过程为：
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 8;                                           // 第一轮操作产生的元素个数
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 16; // 对iter1OutputCount输出个数向上取整
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 2;               // 第二轮操作产生的元素个数
    // 本用例中，由于第二轮操作产生的元素个数为2，即第二轮结束就可以拿到最大值和其索引值，因此需要的空间为iter2AlignStart + RoundUp(iter2OutputCount, elementsPerBlock) * elementsPerBlock = 16 + 16 = 32，也就是64Bytes
    // ReduceMax接口sharedTmpBuffer计算示例五：
    // dstLocal,srcLocal和sharedTmpBuffer均为half类型,srcLocal的计算数据量count为65408,使用tensor前n个数据计算接口,count=65408，不需要索引值
    // tensor前n个数据计算接口调用示例为：
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 65408, false);
    // 此时sharedTmpBuffer所需的最小空间计算过程为：
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = count / elementsPerRepeat = 511;
    int iter1OutputCount = firstMaxRepeat * 2 = 1022;                                          // 第一轮操作产生的元素个数
    int iter1AlignEnd = RoundUp(iter1OutputCount, elementsPerBlock) * elementsPerBlock = 1024; // 第一轮产生的元素个数做向上取整
    // 由于calIndex为false,因此最终sharedTmpBuffer所需的最小空间大小就是对第一轮产生元素做向上取整后的结果，此处就是1024，也就是2048Bytes
    // ReduceMax接口sharedTmpBuffer计算示例六：
    // dstLocal,srcLocal和sharedTmpBuffer均为float类型,srcLocal的计算数据量为8320,使用tensor高维切分计算接口, repeatTime为130, mask为64,需要索引值
    // tensor高维切分计算接口调用示例为：
    AscendC::ReduceMax<float>(dstLocal, srcLocal, sharedTmpBuffer, 64, 130, 8, true);
    // 此时sharedTmpBuffer所需的最小空间计算过程为：
    int typeSize = 4;
    int elementsPerBlock = 32 / typeSize = 8;
    int elementsPerRepeat = 256 / typeSize = 64;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 260;                                          // 第一轮操作产生的元素个数
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 264; // 对第一轮操作输出个数向上取整
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 10;               // 第二轮操作产生的元素个数
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // 对第二轮操作输出个数向上取整
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // 第三轮操作产生的元素个数
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 8;   // 第三轮产生的元素个数做向上取整
    // 最终sharedTmpBuffer所需的最小空间就是iter2AlignStart + iter3AlignStart + iter3AlignEnd = 264 + 16 + 8 = 288,也就是1152Bytes
    ```

- <a name="li7962161711114"></a>tensor高维切分计算接口完整示例：

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 512;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask;

    // 初始化srcLocal 、dstLocal 、sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();
    // mask为128一次计算128个元素,4次repeat计算完512个数,calIndex为true，获取最大值的索引
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride, true);
    // 释放Tensor
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    示例结果如下：

    > 输入数据src_gm：

    ```plain
    [0.4795   0.951    0.866    0.008545 0.8037   0.551    0.754    0.73     0.6035   0.251    0.4841   0.05914  0.9414   0.379    0.664    0.6914   0.9307   0.3853   0.4048
     ...
     0.4106   0.604   ]
    ```

    > 输出数据dst_gm：[0.9985,  6.8e-06]
    > 6.8e-06使用reinterpret_cast方法转换后为索引值114

- tensor前n个数据计算接口完整调用示例：

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // 初始化srcLocal 、dstLocal 、sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // level2接口计算前288个数，calIndex为true，获取最大值的索引
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize, true);
    // 释放Tensor
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    示例结果如下：

    > 输入数据src_gm：

    ```plain
    [0.4778   0.5903   0.2433   0.698    0.1943   0.407    0.891    0.1766   0.5977   0.9473   0.6523   0.10913  0.0143   0.86     0.2366   0.625    0.3696   0.708    0.946
     ...
     0.262   ]
    ```

    > 输出数据dst_gm：[0.999,  1.38e-05]
    > 1.38e-05使用reinterpret_cast方法转换后为索引值232
