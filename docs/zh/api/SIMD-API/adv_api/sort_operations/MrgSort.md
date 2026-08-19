# MrgSort

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/MrgSort_res.md#id1 -->

## 功能说明

将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序，排布方式如下：

<!-- npu="950" id7 -->
Ascend 950PR/Ascend 950DT采用方式一。
<!-- end id7 -->

<!-- npu="A3" id8 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品采用方式一。
<!-- end id8 -->

<!-- npu="910b" id9 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品采用方式一。
<!-- end id9 -->

<!-- npu="310p" id10 -->
Atlas 推理系列产品AI Core采用方式二。
<!-- end id10 -->

-   排布方式一：

    MrgSort处理的数据一般是经过Sort处理后的数据，也就是Sort接口的输出，队列的结构如下所示：

    -   数据类型为float，每个结构占据8Bytes。

        ![](../../../figures/zh-cn_image_0000002043076745.png)

    -   数据类型为half，每个结构也占据8Bytes，中间有2Bytes保留。

        ![](../../../figures/zh-cn_image_0000002006877740.png)

-   排布方式二：Region Proposal排布

    输入输出数据均为Region Proposal，具体请参见[Sort](Sort.md)中的排布方式二。

## 函数原型

```
template <typename T, bool isExhaustedSuspension = false>
__aicore__ inline void MrgSort(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList, const uint16_t elementCountList[4], uint32_t sortedNum[4], uint16_t validBit, const int32_t repeatTime)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isExhaustedSuspension | 某条队列耗尽（即该队列已经全部排序到目的操作数）后，是否需要停止合并。类型为bool，参数取值如下：<br>false：直到所有队列耗尽完才停止合并。<br>true：某条队列耗尽后，停止合并。<br><br>默认值为false。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，存储经过排序后的数据。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sortList | 输入 | 源操作数，支持2-4个队列，并且每个队列都已经排好序，类型为MrgSortSrcList结构体，具体请参考下表。MrgSortSrcList中传入要合并的队列，定义如下方代码所示。 |
| elementCountList | 输入 | 四个源队列的长度（排序方式一：8Bytes结构的数目，排序方式二：16*sizeof(T)Bytes结构的数目），类型为长度为4的uint16_t数据类型的数组，理论上每个元素取值范围[0, 4095]，但不能超出UB的存储空间。 |
| sortedNum | 输出 | 耗尽模式下（即isExhaustedSuspension为true时），停止合并时每个队列已排序的元素个数。 |
| validBit | 输入 | 有效队列个数，取值如下：<br>0b11：前两条队列有效<br>0b111：前三条队列有效<br>0b1111：四条队列全部有效 |
| repeatTime | 输入 | 迭代次数，每一次源操作数和目的操作数跳过四个队列总长度。取值范围：repeatTime∈[1,255]。<br>repeatTime参数生效是有条件的，需要同时满足以下四个条件：<br>srcLocal包含四条队列并且validBit=15。<br>四个源队列的长度一致。<br>四个源队列连续存储。<br>isExhaustedSuspension为false。 |

```
template <typename T>
struct MrgSortSrcList {
    LocalTensor<T> src1;
    LocalTensor<T> src2;
    LocalTensor<T> src3; // 当要合并的队列个数小于3，可以为空tensor
    LocalTensor<T> src4; // 当要合并的队列个数小于4，可以为空tensor
};
```

**表3**  MrgSortSrcList参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| src1 | 输入 | 源操作数，第一个已经排好序的队列。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>数据类型与目的操作数保持一致。支持的数据类型为：half、float。 |
| src2 | 输入 | 源操作数，第二个已经排好序的队列。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>数据类型与目的操作数保持一致。支持的数据类型为：half、float。 |
| src3 | 输入 | 源操作数，第三个已经排好序的队列。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>数据类型与目的操作数保持一致。支持的数据类型为：half、float。 |
| src4 | 输入 | 源操作数，第四个已经排好序的队列。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>数据类型与目的操作数保持一致。支持的数据类型为：half、float。 |

## 返回值说明

无

## 约束说明

-   当存在score\[i\]与score\[j\]相同时，如果i\>j，则score\[j\]将首先被选出来，排在前面，即index的顺序与输入顺序一致。
-   每次迭代内的数据会进行排序，不同迭代间的数据不会进行排序。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

-   处理128个half类型数据。

    该样例适用于：

    <!-- npu="950" id11 -->
    Ascend 950PR/Ascend 950DT
    <!-- end id11 -->

    <!-- npu="910b" id12 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id12 -->

    <!-- npu="A3" id13 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id13 -->

    ```
    uint32_t elementCount = 128;                // 元素个数
    uint32_t calcBufferSize = elementCount * 8; // 每个元素占据8字节
    uint32_t tmpBufferSize = elementCount * 8;
    uint32_t sortedLocalSize = elementCount * 4;
    uint32_t sortRepeatTimes = elementCount / 32;
    uint32_t extractRepeatTimes = elementCount / 32;
    uint32_t sortTmpLocalSize = elementCount * 4;

    uint32_t singleMergeTmpElementCount = elementCount / 4;
    uint32_t baseOffset = AscendC::GetSortOffset<half>(singleMergeTmpElementCount);
    // sortList：待合并的有序队列列表
    AscendC::MrgSortSrcList sortList = AscendC::MrgSortSrcList(
        sortedLocal[0], sortedLocal[baseOffset], sortedLocal[2 * baseOffset], sortedLocal[3 * baseOffset]);
    uint16_t singleDataSize = elementCount / 4; // 队列长度
    const uint16_t elementCountList[4] = {singleDataSize, singleDataSize, singleDataSize, singleDataSize}; // 4个队列的长度
    uint32_t sortedNum[4];
    // 合并sortList中的4条队列
    AscendC::MrgSort<half, false>(sortTmpLocal, sortList, elementCountList, sortedNum, 0b1111, 1);
    ```

    ```
    示例结果
    输入数据(srcValueGm): 128个half类型数据
    [31 30 29 ... 2 1 0
     63 62 61 ... 34 33 32
     95 94 93 ... 66 65 64
     127 126 125 ... 98 97 96]
    输入数据(srcIndexGm):
    [31 30 29 ... 2 1 0
     63 62 61 ... 34 33 32
     95 94 93 ... 66 65 64
     127 126 125 ... 98 97 96]
    输出数据(dstValueGm):
    [127 126 125 ... 2 1 0]
    输出数据(dstIndexGm):
    [127 126 125 ... 2 1 0]
    ```

-   处理64个half类型数据。

    该样例适用于：

    <!-- npu="310p" id14 -->
    Atlas 推理系列产品AI Core
    <!-- end id14 -->

    ```
    uint32_t elementCount = 64; // 元素个数
    // 单条队列元素个数
    uint32_t singleMergeTmpElementCount = elementCount / 4;
    uint32_t baseOffset = AscendC::GetSortOffset<half>(singleMergeTmpElementCount);
    // sortList：待合并的有序队列列表
    AscendC::MrgSortSrcList sortList = AscendC::MrgSortSrcList(
        sortedLocal[0], sortedLocal[baseOffset], sortedLocal[2 * baseOffset], sortedLocal[3 * baseOffset]);
    uint16_t singleDataSize = elementCount / 4; // 队列长度
    const uint16_t elementCountList[4] = {singleDataSize, singleDataSize, singleDataSize, singleDataSize}; // 4个队列的长度
    uint32_t sortedNum[4];
    // 合并sortList中的4条队列
    AscendC::MrgSort<half, false>(sortTmpLocal, sortList, elementCountList, sortedNum, 0b1111, 1);
    ```

    ```
    示例结果
    输入数据(srcValueGm): 64个half类型数据
    [15 14 13 ... 2 1 0
     31 30 29 ... 18 17 16
     47 46 45 ... 34 33 32
     63 62 61 ... 50 49 48]
    输入数据(srcIndexGm):
    [15 14 13 ... 2 1 0
     31 30 29 ... 18 17 16
     47 46 45 ... 34 33 32
     63 62 61 ... 50 49 48]
    输出数据(dstValueGm):
    [63 62 61 ... 2 1 0]
    输出数据(dstIndexGm):
    [63 62 61 ... 2 1 0]
    ```
