# Frac

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:03:14.381Z pushedAt=2026-10-29T02:15:02.745Z nl=2 -->

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Frac_interface/Frac_res.md#id1 -->

## Description

Computes the fractional part of each element. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218334537.png)

For example:

Frac\(-258.41888\) = -0.41888428

Frac\(5592.625\) = 0.625

## Prototype

-   The API framework allocates temporary space.
    -   All elements of the source operand tensor participate in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

-   Temporary space is passed through the sharedTmpBuffer input parameter.
    -   All elements of the source operand tensor participate in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

    -   All or part of the source operand participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be provided in two ways: **passed through the sharedTmpBuffer input parameter** by you, or **allocated by the API framework**.

-   The API framework allocates the temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

-   Pass the temporary space through the sharedTmpBuffer input parameter and use this tensor as the temporary space for processing. The API framework no longer allocates it. In this way, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization.

When the API framework allocates the temporary space, you need to reserve the temporary space. When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the **GetFracMaxMinTmpSize** API provided in [GetFracMaxMinTmpSize](GetFracMaxMinTmpSize.md) to obtain the upper and lower limits of the space size to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Frac. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetFracMaxMinTmpSize](GetFracMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

<!-- npu="310p" id8 -->
-   For the Atlas inference products AI Core, the input data is limited to the \[-2147483647.0, 2147483647.0\] range.
<!-- end id8 -->
-   **Overlapping between the source operand address and the destination operand address is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete call example, see [Frac Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/frac).

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
AscendC::Frac<srcType, false>(dstLocal, srcLocal);
```

The result is as follows:

```
Input data (srcLocal): [9.33 -8.07 -6.38 8.45 5.83 6.46 4.18 1.93]
Output data (dstLocal): [0.33 -0.07 -0.38 0.45 0.83 0.46 0.18 0.93]
```
