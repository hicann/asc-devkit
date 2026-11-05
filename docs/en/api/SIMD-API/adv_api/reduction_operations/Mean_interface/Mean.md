# Mean

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:12:14.315Z pushedAt=2026-10-29T02:15:03.069Z nl=2 -->

## Product Support

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
- Vector Core of Atlas inference products: not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/Mean_interface/Mean_res.md#id1 -->

## Description

Calculates the average of elements along the last axis.

If the input is a vector, the elements in the vector are summed and averaged. If the input is a matrix, the elements are averaged along the last dimension. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

As shown in the following figure, an averaging operation is performed on a two-dimensional matrix with shape \(4, 5\), and the output result is \[3, 8, 13, 18\].

![](../../../../figures/nz_reduce.png)

Before understanding the specific functions of the API, you need to understand some essential concepts: the number of rows of the data is called the **outer axis length (outter)**, the actual number of elements in each row is called the **actual number of elements on the inner axis (n)**, and the number of elements after the actual number of elements on the inner axis n is padded upward to 32-byte alignment is called the **number of elements on the inner axis after padding \(inner\)**. This API requires the inner axis length of the input to satisfy 32-byte alignment. Therefore, when the byte length occupied by n is not an integer multiple of 32 bytes, you need to pad it upward to an integer multiple of 32 bytes. In the following example, the element type is float, the actual number of elements n in each row is 5, and the occupied byte length is 20 bytes, which is not an integer multiple of 32 bytes. After padding upward, 32 bytes are obtained, corresponding to 8 elements. In the figure, padding represents the padding operation. The relationship between n and inner is as follows: **inner = \(n \*sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\)**.

![](../../../../figures/nz_reduce_47.png)

## Prototype

- Passes temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
    __aicore__ inline void Mean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const MeanParams& meanParams)
    ```

- Temporary space allocated by the API framework

    ```
    template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
    __aicore__ inline void Mean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const MeanParams& meanParams)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passed by you through the sharedTmpBuffer input parameter** and **allocated by the API framework**.

- Passed through the sharedTmpBuffer input parameter: this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
- The API framework allocates the temporary space: you do not need to allocate it, but you need to reserve the size of the temporary space.

When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor; when it is allocated by the API framework, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetMeanMaxMinTmpSize](GetMeanMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| accType | Data type that actually participates in the computation. When the precision of accType is higher than that of the input T, the input is converted to accType before computation, the computation is performed using accType, and the result is converted back to the original data type after computation. Setting accType to a higher precision can prevent data type overflow. When T is half, you can set accType to float to increase the precision of the half input to float for computation. A case where the precision of accType is lower than that of the input T is not supported. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| isBasicBlock | Reserved parameter, not supported yet. |
| reduceDim | Specifies the dimension along which the data is summed. This API is implemented along the last dimension and does not support the reduceDim parameter. Pass the default value -1. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The output value requires outter * sizeof(T) bytes of space for storage. You need to allocate the actual memory space for dstTensor based on this size and the alignment requirements of the framework. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand.<br><br>The shape of the input data is outter * inner. You need to allocate outter * inner * sizeof(T) bytes of space for it. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of Mean. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetMeanMaxMinTmpSize](GetMeanMaxMinTmpSize.md). |
| MeanParams | Input | Shape information of srcTensor. The type is MeanParams, and its definition is shown in the following code. The meanings of the parameters are as follows:<br>outter: Outer axis length of the input data.<br>inner: Number of elements on the inner axis of the input data after the actual number of elements is padded to 32 bytes. inner\*sizeof(T) must be an integer multiple of 32 bytes.<br>n: Actual number of elements on the inner axis of the input data.<br>Note: MeanParams.inner is the value obtained by aligning MeanParams.n upward to 32 bytes, that is, inner = (n * sizeof(T) + 32 - 1) / 32 * 32 / sizeof(T). Therefore, MeanParams.n must satisfy: 1 <= MeanParams.n <= MeanParams.inner. |

```
struct MeanParams {
    uint32_t outter = 1;
    uint32_t inner;
    uint32_t n;
};
```

## Return Value

None

## Constraints

- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- **Overlap between the source operand and destination operand addresses is not supported.**
- Overlap between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
- Currently, only ND-format input is supported. Other formats are not supported.
- For mean, the method used is to sum first and then divide. The underlying addition method used internally during summation is consistent with that used internally by [Sum](../Sum_interface/Sum.md), [ReduceSum](../../../basic_api/memory_vector_compute/reduction_compute/ReduceSum.md), and [ReduceRepeat](../../../basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md). It uses a binary tree method, adding pairwise. For details, see [Sum](../Sum_interface/Sum.md).

## Examples

```
#include "kernel_operator.h"
// Define the shape information of srcTensor. The input element type is half, and the data is a 2*3 two-dimensional array.
AscendC::MeanParams meanParams;
// m is 2, and outter equals the value of m.
meanParams.outter = outter;
// n is 3.
meanParams.n = n;
// inner = (n * sizeof(half) + 32 - 1)/32 * 32 / sizeof(half) = 16
meanParams.inner = inner;
// T is half, and accTypes is the actual computation type, which is half here.
// dstLocal is the output data, srcLocal is the input data, and tmplocalTensor is the temporary space passed by the user.
// AscendC::Mean<T, accType>(dstLocal, srcLocal,tmplocalTensor, meanParams)
// tmplocalTensor can also be omitted.
// Call AscendC::Mean<T, accType>(dstLocal, srcLocal, meanParams) as follows:
```

The result example is as follows:

```
Input data (srcLocal): [[1 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0],
                     [4 5 6 0 0 0 0 0 0 0 0 0 0 0 0 0]]
Output data (dstLocal): [2 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
```
