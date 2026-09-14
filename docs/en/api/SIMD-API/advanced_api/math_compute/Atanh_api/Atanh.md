# Atanh

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:55:15.688Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise inverse hyperbolic cosine computation, with the computation formula as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182892020.png)

![](../../../../figures/zh-cn_formulaimage_0000001765074025.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to allocate space for the tensor; when the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetAtanhMaxMinTmpSize](GetAtanhMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Atanh, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetAtanhMaxMinTmpSize](GetAtanhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The input source data must be kept within the value range of \(-0.99, -0.001\), or \(0.001, 0.99\). If the input is out of range, the output result is invalid.

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete calling sample, see the [operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/acosh).

```
// dstLocal: Tensor that stores the Atanh computation result.
// srcLocal: Tensor that stores the Atanh computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Atanh computation.
// The input data type of the operator is half, and the number of elements involved in the computation is 512.
AscendC::Atanh(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (srcLocal): [0.000000 0.010000 0.020000 ...  0.990000 1.000000 1.010000 ...]
Output data (dstLocal): [0.000000 0.010000 0.020003 ...  2.646650 inf -nan ...]
```
