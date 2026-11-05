# MrgSort

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:26:01.608Z pushedAt=2026-10-27T11:51:48.253Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/MrgSort_res.md#id1 -->

## Description

Merges up to four already-sorted queues into a single queue, with the result sorted by the score field in descending order. The arrangement modes are as follows:

<!-- npu="950" id7 -->
Ascend 950PR&950DT products adopt arrangement mode 1.
<!-- end id7 -->

<!-- npu="A3" id8 -->
Atlas A3 products adopt arrangement mode 1.
<!-- end id8 -->

<!-- npu="910b" id9 -->
Atlas A2 products adopt arrangement mode 1.
<!-- end id9 -->

<!-- npu="310p" id10 -->
The Atlas inference products AI Core adopts arrangement mode 2.
<!-- end id10 -->

-   Arrangement mode 1:

    The data processed by **MrgSort** is generally the data processed by **Sort**, that is, the output of the **Sort** API. The queue structure is as follows:

    -   The data type is float, and each structure occupies 8 bytes.

        ![](../../../figures/zh-cn_image_0000002043076745.png)

    -   The data type is half, and each structure also occupies 8 bytes, with 2 bytes reserved in the middle.

        ![](../../../figures/zh-cn_image_0000002006877740.png)

-   Arrangement mode 2: Region Proposal arrangement

    Both the input and output data are Region Proposal. For details, see arrangement mode 2 in [Sort](Sort.md).

## Prototype

```
template <typename T, bool isExhaustedSuspension = false>
__aicore__ inline void MrgSort(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList, const uint16_t elementCountList[4], uint32_t sortedNum[4], uint16_t validBit, const int32_t repeatTime)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isExhaustedSuspension | Whether to stop merging after a queue is exhausted (that is, all elements of the queue have been sorted into the destination operand). The type is bool. The value options are as follows:<br>false: Merging stops only after all queues are exhausted.<br>true: Merging stops after a queue is exhausted.<br><br>The default value is false. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, which stores the sorted data.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sortList | Input | Source operand, which supports 2 to 4 queues, each of which has been sorted. The type is the MrgSortSrcList structure. For details, see the following table. The queues to be merged are passed in MrgSortSrcList, which is defined as shown in the following code. |
| elementCountList | Input | Lengths of the four source queues (arrangement mode 1: number of 8-byte structures; arrangement mode 2: number of 16*sizeof(T)-byte structures). The type is a uint16_t array of length 4. Theoretically, each element ranges from [0, 4095], but cannot exceed the UB storage space. |
| sortedNum | Output | In exhaustion mode (that is, when isExhaustedSuspension is true), the number of elements already sorted in each queue when merging stops. |
| validBit | Input | Number of valid queues. The value options are as follows:<br>0b11: The first two queues are valid.<br>0b111: The first three queues are valid.<br>0b1111: All four queues are valid. |
| repeatTime | Input | Number of iterations. In each iteration, the source operand and destination operand skip the total length of the four queues. Value range: repeatTime∈[1,255].<br>The repeatTime parameter takes effect only when all the following four conditions are met:<br>srcLocal contains four queues and validBit=15.<br>The four source queues have the same length.<br>The four source queues are stored contiguously.<br>isExhaustedSuspension is false. |

```
template <typename T>
struct MrgSortSrcList {
    LocalTensor<T> src1;
    LocalTensor<T> src2;
    LocalTensor<T> src3; // When the number of queues to be merged is less than 3, it can be an empty tensor.
    LocalTensor<T> src4; // When the number of queues to be merged is less than 4, it can be an empty tensor.
};
```

**Table 3**  MrgSortSrcList parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src1 | Input | Source operand, the first sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with that of the destination operand. Supported data types: half and float. |
| src2 | Input | Source operand, the second sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with that of the destination operand. Supported data types: half and float. |
| src3 | Input | Source operand, the third sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with that of the destination operand. Supported data types: half and float. |
| src4 | Input | Source operand, the fourth sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with that of the destination operand. Supported data types: half and float. |

## Return Value

None

## Constraints

-   When score\[i\] is the same as score\[j\], if i\>j, then score\[j\] is selected first and placed in front, that is, the order of index is consistent with the input order.
-   Data within each iteration is sorted, while data across different iterations is not sorted.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples<a name="section642mcpsimp"></a>

-   Process 128 half-type data elements.

    This sample applies to:

    <!-- npu="950" id11 -->
    Ascend 950PR&950DT products
    <!-- end id11 -->

    <!-- npu="910b" id12 -->
    Atlas A2 products
    <!-- end id12 -->

    <!-- npu="A3" id13 -->
    Atlas A3 products
    <!-- end id13 -->

    ```
    uint32_t elementCount = 128;                // Number of elements.
    uint32_t calcBufferSize = elementCount * 8; // Each element occupies 8 bytes.
    uint32_t tmpBufferSize = elementCount * 8;
    uint32_t sortedLocalSize = elementCount * 4;
    uint32_t sortRepeatTimes = elementCount / 32;
    uint32_t extractRepeatTimes = elementCount / 32;
    uint32_t sortTmpLocalSize = elementCount * 4;

    uint32_t singleMergeTmpElementCount = elementCount / 4;
    uint32_t baseOffset = AscendC::GetSortOffset<half>(singleMergeTmpElementCount);
    // sortList: List of sorted queues to be merged.
    AscendC::MrgSortSrcList sortList = AscendC::MrgSortSrcList(
        sortedLocal[0], sortedLocal[baseOffset], sortedLocal[2 * baseOffset], sortedLocal[3 * baseOffset]);
    uint16_t singleDataSize = elementCount / 4; // Queue length.
    const uint16_t elementCountList[4] = {singleDataSize, singleDataSize, singleDataSize, singleDataSize}; // Length of the four queues.
    uint32_t sortedNum[4];
    // Merge the four queues in sortList.
    AscendC::MrgSort<half, false>(sortTmpLocal, sortList, elementCountList, sortedNum, 0b1111, 1);
    ```

    ```
    Example result
    Input data (srcValueGm): 128 half-type data elements.
    [31 30 29 ... 2 1 0
     63 62 61 ... 34 33 32
     95 94 93 ... 66 65 64
     127 126 125 ... 98 97 96]
    Input data (srcIndexGm):
    [31 30 29 ... 2 1 0
     63 62 61 ... 34 33 32
     95 94 93 ... 66 65 64
     127 126 125 ... 98 97 96]
    Output data (dstValueGm):
    [127 126 125 ... 2 1 0]
    Output data (dstIndexGm):
    [127 126 125 ... 2 1 0]
    ```

-   Processes 64 half-type data elements.

    This sample applies to:

    <!-- npu="310p" id14 -->
    Atlas inference products AI Core
    <!-- end id14 -->

    ```
    uint32_t elementCount = 64; // Number of elements.
    // Number of elements in a single queue.
    uint32_t singleMergeTmpElementCount = elementCount / 4;
    uint32_t baseOffset = AscendC::GetSortOffset<half>(singleMergeTmpElementCount);
    // sortList: List of sorted queues to be merged.
    AscendC::MrgSortSrcList sortList = AscendC::MrgSortSrcList(
        sortedLocal[0], sortedLocal[baseOffset], sortedLocal[2 * baseOffset], sortedLocal[3 * baseOffset]);
    uint16_t singleDataSize = elementCount / 4; // Queue length.
    const uint16_t elementCountList[4] = {singleDataSize, singleDataSize, singleDataSize, singleDataSize}; // Lengths of the four queues.
    uint32_t sortedNum[4];
    // Merge the four queues in sortList.
    AscendC::MrgSort<half, false>(sortTmpLocal, sortList, elementCountList, sortedNum, 0b1111, 1);
    ```

    ```
    Example result
    Input data (srcValueGm): 64 half-type data elements
    [15 14 13 ... 2 1 0
     31 30 29 ... 18 17 16
     47 46 45 ... 34 33 32
     63 62 61 ... 50 49 48]
    Input data (srcIndexGm):
    [15 14 13 ... 2 1 0
     31 30 29 ... 18 17 16
     47 46 45 ... 34 33 32
     63 62 61 ... 50 49 48]
    Output data (dstValueGm):
    [63 62 61 ... 2 1 0]
    Output data (dstIndexGm):
    [63 62 61 ... 2 1 0]
    ```
