# Frac

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:11:01.508Z pushedAt=2026-11-18T08:54:29.436Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
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

![](../../../../figures/formulaimage_0000002218334537.png)

For example:

Frac\(-258.41888\) = -0.41888428

Frac\(5592.625\) = 0.625

## Prototype

-   The API framework allocates temporary space.
    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Frac(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.

When the API framework allocates the temporary space, you need to reserve the temporary space. When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the **GetFracMaxMinTmpSize** API provided in [GetFracMaxMinTmpSize](GetFracMaxMinTmpSize.md) to obtain the upper and lower limits of the space size to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Frac. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetFracMaxMinTmpSize](GetFracMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

<!-- npu="310p" id8 -->
-   For the Atlas inference products AI Core, the input data is limited to the range of \[-2147483647.0, 2147483647.0\].
<!-- end id8 -->
-   **Overlapping between the source operand and destination operand addresses is not supported.**
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
