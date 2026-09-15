# MrgSort

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T17:12:47.076Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Merges up to four sorted queues into a single queue, with the result sorted by the score field in descending order. The arrangement mode is as follows:

Ascend 950PR/Ascend 950DT: Arrangement mode 1 is used.

Atlas A3 training products/Atlas A3 inference products: Arrangement mode 1 is used.

Atlas A2 training products/Atlas A2 inference products: Arrangement mode 1 is used.

Atlas inference products AI Core: Arrangement mode 2 is used.

-   Arrangement mode 1:

    The data processed by MrgSort is generally the data processed by Sort, that is, the output of the Sort API. The queue structure is as follows:

    -   The data type is float, and each structure occupies 8 bytes.

        ![](../../../figures/zh-cn_image_0000002043076745.png)

    -   The data type is half, and each structure also occupies 8 bytes, with 2 bytes reserved in the middle.

        ![](../../../figures/zh-cn_image_0000002006877740.png)

-   Arrangement mode 2: Region Proposal arrangement

    Both the input and output data are Region Proposal. For details, see arrangement mode 2 in [Sort](Sort.md).

## Prototype

```
template <typename T, bool isExhaustedSuspension = false>
__aicore__ inline void MrgSort(const LocalTensor<T> &dst, const MrgSortSrcList<T> &sortList, const uint16_t elementCountList[4], uint32_t sortedNum[4], uint16_t validBit, const int32_t repeatTime)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isExhaustedSuspension | Whether to stop merging after a queue is exhausted (that is, all its elements have been sorted into the destination operand). The type is bool, and the values are as follows:<br>false: Merging stops only after all queues are exhausted.<br>true: Merging stops after a queue is exhausted.<br><br>The default value is false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, which stores the sorted data.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sortList | Input | Source operand, which supports 2 to 4 queues, each of which is already sorted. The type is the MrgSortSrcList structure. For details, see the following table. The queues to be merged are passed in MrgSortSrcList, which is defined as shown in the following code. |
| elementCountList | Input | Lengths of the four source queues (arrangement mode 1: number of 8-byte structures; arrangement mode 2: number of 16*sizeof(T)-byte structures). The type is an array of uint16_t with a length of 4. Theoretically, each element ranges from [0, 4095], but it cannot exceed the UB storage space. |
| sortedNum | Output | In exhaustion mode (that is, when isExhaustedSuspension is true), the number of elements already sorted in each queue when merging stops. |
| validBit | Input | Number of valid queues. The values are as follows:<br>0b11: The first two queues are valid.<br>0b111: The first three queues are valid.<br>0b1111: All four queues are valid. |
| repeatTime | Input | Number of iterations. In each iteration, the source operand and the destination operand skip the total length of the four queues. Value range: repeatTime∈[1,255].<br>The repeatTime parameter takes effect only when all of the following four conditions are met:<br>srcLocal contains four queues and validBit=15.<br>The four source queues have the same length.<br>The four source queues are stored contiguously.<br>isExhaustedSuspension is false. |

```
template <typename T>
struct MrgSortSrcList {
    LocalTensor<T> src1;
    LocalTensor<T> src2;
    LocalTensor<T> src3; // When the number of queues to be merged is less than 3, it can be an empty tensor.
    LocalTensor<T> src4; // When the number of queues to be merged is less than 4, it can be an empty tensor.
};
```

**Table 3**  MrgSortSrcList parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| src1 | Input | Source operand, the first sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with the destination operand. Supported data types: half and float. |
| src2 | Input | Source operand, the second sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with the destination operand. Supported data types: half and float. |
| src3 | Input | Source operand, the third sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with the destination operand. Supported data types: half and float. |
| src4 | Input | Source operand, the fourth sorted queue.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type is consistent with the destination operand. Supported data types: half and float. |

## Return Value

None

## Constraints

-   When score\[i\] and score\[j\] are the same, if i\>j, score\[j\] is selected first and placed before score[i], that is, the order of indices is consistent with the input order.
-   Data within each iteration is sorted, while data across different iterations is not sorted.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

-   Processes 128 half-type data elements.

    This example applies to:

    Ascend 950PR/Ascend 950DT

    Atlas A2 training products/Atlas A2 inference products

    Atlas A3 training products/Atlas A3 inference products

    ```
    uint32_t elementCount = 128; // Number of elements.
    uint32_t calcBufferSize = elementCount * 8; // Each element occupies 8 bytes.
    uint32_t tmpBufferSize = elementCount * 8;
    uint32_t sortedLocalSize = elementCount * 4;
    uint32_t sortRepeatTimes = elementCount / 32;
    uint32_t extractRepeatTimes = elementCount / 32;
    uint32_t sortTmpLocalSize = elementCount * 4;

    uint32_t singleMergeTmpElementCount = elementCount / 4;
    uint32_t baseOffset = AscendC::GetSortOffset<half>(singleMergeTmpElementCount);
    AscendC::MrgSortSrcList sortList = AscendC::MrgSortSrcList(sortedLocal[0], sortedLocal[baseOffset], sortedLocal[2 * baseOffset], sortedLocal[3 * baseOffset]); // sortList: list of sorted queues to be merged.
    uint16_t singleDataSize = elementCount / 4; // Queue length.
    const uint16_t elementCountList[4] = {singleDataSize, singleDataSize, singleDataSize, singleDataSize}; // Length of the four queues.
    uint32_t sortedNum[4];
    // Merge the four queues in sortList.
    AscendC::MrgSort<half, false>(sortTmpLocal, sortList, elementCountList, sortedNum, 0b1111, 1);
    ```

    ```
    Example result
    Input data (srcValueGm): 128 half-type data elements
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

    This example applies to:

    Atlas inference products AI Core

    ```
    uint32_t elementCount = 64; // Number of elements.
    // Number of elements in a single queue.
    uint32_t singleMergeTmpElementCount = elementCount / 4;
    uint32_t baseOffset = AscendC::GetSortOffset<half>(singleMergeTmpElementCount);
    AscendC::MrgSortSrcList sortList = AscendC::MrgSortSrcList(sortedLocal[0], sortedLocal[baseOffset], sortedLocal[2 * baseOffset], sortedLocal[3 * baseOffset]); // sortList: List of sorted queues to be merged.
    uint16_t singleDataSize = elementCount / 4; // Queue length.
    const uint16_t elementCountList[4] = {singleDataSize, singleDataSize, singleDataSize, singleDataSize}; // Lengths of the four queues.
    uint32_t sortedNum[4];
    // Merge the 4 queues in sortList.
    AscendC::MrgSort<half, false>(sortTmpLocal, sortList, elementCountList, sortedNum, 0b1111, 1);
    ```

    ```
    Example result
    Input data (srcValueGm): 64 half-type data elements.
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
