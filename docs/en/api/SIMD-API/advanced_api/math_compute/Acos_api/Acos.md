# Acos

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:22:34.132Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs the inverse cosine (arccosine) calculation element by element. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002183047492.png)

![](../../../../figures/zh-cn_formulaimage_0000002223323813.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Acos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Acos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Acos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Acos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetAcosMaxMinTmpSize](GetAcosMaxMinTmpSize.md).

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
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Acos, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetAcosMaxMinTmpSize](GetAcosMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the calculation. |

## Return Value

None

## Constraints

-   The input source data must be kept within the value range of \[-1, 1\]. If the input is out of range, the output result is invalid.
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the calculation result.
// srcLocal: Input tensor involved in the calculation.
// sharedTmpBuffer: Temporary buffer for storing intermediate variables during complex computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Acos(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The results are as follows:

```
// The first and last two positions are special values.
Input data (srcLocal): [0.000000 0.001000 0.002000 ...  0.252000 0.253000 -1.000000 1.000000]
Output data (dstLocal): [1.570796 1.569796 1.568796  ...  1.316050 1.315016 3.141593 0.000000]
```
