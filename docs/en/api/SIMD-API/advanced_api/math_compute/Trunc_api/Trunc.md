# Trunc

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:47:28.563Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs floating-point truncation element-wise, that is, rounds toward zero. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888744.png)

For example:

Trunc\(3.9\) = 3

Trunc\(-3.9\) = -3

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Trunc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Trunc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Trunc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Trunc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. Temporary space can be obtained in two ways: **applied for by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

-   When the API framework applies for temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

-   When temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this way, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, providing higher flexibility and higher memory utilization.

When the API framework applies for the space, you need to reserve temporary space; when the space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space to be reserved by calling the **GetTruncMaxMinTmpSize** API provided in [GetTruncMaxMinTmpSize](GetTruncMaxMinTmpSize.md).

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Stores intermediate variables during complex internal computation of Trunc. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetTruncMaxMinTmpSize](GetTruncMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For the AI Core of Atlas inference products, the input data is limited to the range of \[-2147483647.0, 2147483647.0\].
-   Address overlap between the source operand and the destination operand is supported.
-   Address overlap between **sharedTmpBuffer** and the source operand or the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For a complete calling sample, see the [operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/ceil).

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
AscendC::Trunc<srcType, false>(dstLocal, srcLocal);
```

The result is as follows:

```
Input data (srcLocal): [9.33 -8.07 -6.38 8.45 5.83 6.46 4.18 1.93]
Output data (dstLocal): [9.   -8.   -6.   8.   5.   6.   4.   1.  ]
```
