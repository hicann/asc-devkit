# Digamma

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T14:34:28.040Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the logarithmic derivative of the gamma function of x element-wise. The calculation formula is as follows, where ![](../../../../figures/zh-cn_formulaimage_0000001945339324.png) is the gamma function.

![](../../../../figures/zh-cn_formulaimage_0000002182895154.png)

![](../../../../figures/zh-cn_formulaimage_0000002218495557.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Digamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Digamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to allocate space for the tensor; when the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetDigammaMaxMinTmpSize](GetDigammaMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

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
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Digamma, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetDigammaMaxMinTmpSize](GetDigammaMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete calling example, see [More Samples](../more_samples_83.md).

```
// dstLocal: Output tensor.
// srcLocal: Input tensor.
// sharedTmpBuffer: temporary space.
// The input tensor length is 1024, the operator input data type is float, and the actual number of computations is 1024.
AscendC::Digamma<float, false>(dstLocal, srcLocal, sharedTmpBuffer, 1024);
```

The results are as follows:

```
Input data (srcLocal): [5.3675685 0.26528683 -2.872628 ... 2.9387941 9.001339]
Output data (dstLocal): [1.5843406 -3.978809 -6.2081366 ... 0.8983184 2.1407988]
```
