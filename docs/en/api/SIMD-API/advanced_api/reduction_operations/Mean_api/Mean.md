# Mean

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T12:25:42.034Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the average of the elements along the last axis.

If the input is a vector, the elements in the vector are summed and averaged. If the input is a matrix, the elements are averaged along the last dimension. **This API supports input of at most two dimensions and does not support input of higher dimensions.**

As shown in the following figure, an averaging operation is performed on a two-dimensional matrix with shape \(4, 5\), and the output result is \[3, 8, 13, 18\].

![](../../../../figures/nz-reduce.png)

Before understanding the specific functions of the API, you need to understand some essential concepts: the number of rows of the data is called the **outer axis length (outter)**, the actual number of elements in each row is called the **actual number of elements in the inner axis (n)**, and the number of elements after the actual number of inner-axis elements n is aligned upward to 32 bytes is called the **number of inner-axis elements after padding \(inner\)**. This API requires the inner-axis length of the input to satisfy 32-byte alignment. Therefore, when the byte length occupied by n is not a multiple of 32 bytes, you need to pad it upward to a multiple of 32 bytes. In the following example, the element type is float, the actual number of elements n in each row is 5, and the occupied byte length is 20 bytes, which is not a multiple of 32 bytes. After padding upward, 32 bytes are obtained, corresponding to 8 elements. The padding in the figure represents the padding operation. The relationship between n and inner is as follows: **inner = \(n \*sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\)**.

![](../../../../figures/nz-reduce-47.png)

## Prototype

- Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
    __aicore__ inline void Mean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const MeanParams& meanParams)
    ```

- The API framework allocates temporary space.

    ```
    template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
    __aicore__ inline void Mean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const MeanParams& meanParams)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

- Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
- The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetMeanMaxMinTmpSize](GetMeanMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| accType | Data type actually used in computation. When the precision of **accType** is higher than that of the input **T**, the input is converted to **accType** before computation, the computation is performed using **accType**, and the result is converted back to the original data type after computation. Setting **accType** to a higher precision can prevent data type overflow. When **T** is half, you can set **accType** to float, which means the input of type half is converted to a higher-precision float for computation. The case where the precision of **accType** is lower than that of the input **T** is not supported. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| isBasicBlock | Reserved parameter, not supported yet. |
| reduceDim | Specifies the dimension along which the data is summed. This API is implemented along the last dimension and does not support the **reduceDim** parameter. Pass the default value **-1**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The output value requires a space of outter * sizeof(T) for storage. You need to allocate the actual memory space for **dstTensor** based on this size and the alignment requirements of the framework. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand.<br><br>The shape of the input data is outter * inner. You need to allocate a space of outter * inner * sizeof(T) for it. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Mean, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetMeanMaxMinTmpSize](GetMeanMaxMinTmpSize.md). |
| MeanParams | Input | Shape information of **srcTensor**. The type is **MeanParams**, which is defined in the following code. The meanings of the parameters are as follows:<br>outter: Outer axis length of the input data.<br>inner: Number of elements after the actual number of inner-axis elements of the input data is aligned to 32 bytes. inner\*sizeof(T) must be a multiple of 32 bytes.<br>n: Actual number of elements in the inner axis of the input data.<br>Note that **MeanParams.inner** is the value of **MeanParams.n** aligned upward to 32 bytes, that is, inner = (n * sizeof(T) + 32 - 1) / 32 * 32 / sizeof(T). Therefore, the size of **MeanParams.n** must satisfy: 1 <= MeanParams.n <= MeanParams.inner. |

```
struct MeanParams{
    uint32_t outter = 1; 
    uint32_t inner; 
    uint32_t n; 
};
```

## Return Value

None

## Constraints

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- **Address overlap between the source operand and the destination operand is not supported.**
- Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
- Currently, only ND-format input is supported, and other formats are not supported.
- For mean, the approach is to first perform summation and then division. The underlying addition method used internally during summation is consistent with that of [Sum](../Sum_api/Sum.md), [ReduceSum](../../../basic_api/memory_vector_compute/reduction_compute/ReduceSum.md), and [ReduceRepeat](../../../basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md), which uses a binary tree approach with pairwise addition. For details, see [Sum](../Sum_api/Sum.md).

## Examples

```
#include "kernel_operator.h"
// Define the shape information of srcTensor. The input element type is half, and the data is a two-dimensional array of size 2*3.
AscendC::MeanParams meanParams;
// m is 2, and outter equals the value of m.
meanParams.outter = outter;
// n is 3.
meanParams.n = n;
// inner = (n * sizeof(half) + 32 - 1)/32 * 32 / sizeof(half) = 16
meanParams.inner = inner;
// T is half, and accTypes is the actual computation type, which is half here.
// dstLocal is the output data, srcLocal is the input data, and tmplocalTensor is the temporary buffer passed in by the user.
AscendC::Mean<T, accType>(dstLocal, srcLocal, tmplocalTensor, meanParams)
// tmplocalTensor can also be omitted. Call it as follows.
// AscendC::Mean<T, accType>(dstLocal, srcLocal, meanParams);
```

The results are as follows:

```
Input data (srcLocal): [[1 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0],
                     [4 5 6 0 0 0 0 0 0 0 0 0 0 0 0 0]]
Output data (dstLocal): [2 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
```
