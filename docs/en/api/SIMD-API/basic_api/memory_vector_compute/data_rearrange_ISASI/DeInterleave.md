# DeInterleave<a name="ZH-CN_TOPIC_0000002131264492"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:02:46.923Z pushedAt=2026-10-28T03:14:39.522Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_rearrange_ISASI/DeInterleave_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_duplicate_intf.h"`.

Given source operands src0 and src1, deinterleave the elements in src0 and src1 and store them into destination operands dst0 and dst1. The deinterleaving arrangement is shown in the following figure, where each square represents an element.

![](../../../../figures/deinterleave_memory.png)

## Prototype<a name="section620mcpsimp"></a>

-   Two inputs

    ```
    template <typename T>
    __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t count)
    ```

-   One input

    ```
    template <typename T>
    __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const int32_t srcCount)
    ```

## Parameters<a name="section176711403104"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst0/dst1 | Output | Destination operand. Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. The start address of LocalTensor must be 32-byte aligned. |
| src/src0/src1 | Input | Source operand. Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. The start address of LocalTensor must be 32-byte aligned. The data type of the source operand must be the same as that of the destination operand. |
| count | Input | Number of input/output data elements. The length of dst0/dst1/src0/src1 is count. count must be an even number. |
| srcCount | Input | Number of input data elements. The size of each output is half of the input. srcCount must be an even number. |

## Data Types

Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. The data type of the source operand must be the same as that of the destination operand.

## Return Value<a name="section14483414194"></a>

None

## Constraints<a name="section633mcpsimp"></a>

Overlapping addresses between the source operand and the destination operand are not supported. 

## Examples<a name="section176061616102911"></a>

-   Two inputs

    ```
    AscendC::DeInterleave(dst0Local, dst1Local, src0Local, src1Local, 512);
    ```

    The result is as follows:

    ```
    Input data src0Local: [1 2 3 ... 512]
    Input data src1Local: [513 514 515 ... 1024]
    Output data dst0Local: [1 3 5 ... 1023]
    Output data dst1Local: [2 4 6 ... 1024]
    ```

-   One input

    ```
    AscendC::DeInterleave(dst0Local, dst1Local, srcLocal, 512);
    ```

    The result is as follows:

    ```
    Input data srcLocal: [1 2 3 ... 512]
    Output data dst0Local: [1 3 5 ... 511]
    Output data dst1Local: [2 4 6 ... 512]
    ```
