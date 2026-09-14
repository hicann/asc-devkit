# Lgamma

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T16:07:17.023Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the absolute value of the gamma function of x element-wise and then computes its natural logarithm. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218495125.png)

![](../../../../figures/zh-cn_formulaimage_0000002188278974.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Lgamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Lgamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [6.1.55 Lgamma Tiling](GetLgammaMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is **false**. This parameter takes effect only when the input data type is float.<br><br>**true**: You allow the source operand to be overwritten. You can set this parameter to **true** to enable it. After it is enabled, the memory space of **srcTensor** is reused during the internal computation of this API, saving some memory space.<br><br>**false**: The memory space of **srcTensor** is not reused during the internal computation of this API.<br><br>For an example of using **isReuseSource**, see [More Samples](../more_samples_83.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Lgamma, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetLgammaMaxMinTmpSize](GetLgammaMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor involved in the computation.
// sharedTmpBuffer: Temporary buffer for storing intermediate variables during complex computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Lgamma(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (srcLocal): [0.000000  0.010000  0.020000  0.030000  0.040000  ...]
Output data (dstLocal): [inf       4.599480  3.900805  3.489971  3.197078  ...]
```
