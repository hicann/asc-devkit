# Sum

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T13:21:33.950Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the sum of elements along the last dimension.

If the input is a vector, the elements in the vector are added together. If the input is a matrix, the elements in each row are summed along the last dimension. **This API supports at most two-dimensional input data and does not support higher-dimensional input.**

As shown in the following figure, the operation is performed on a two-dimensional matrix with the shape \(2, 3\), and the output result is \[6, 15\].

![](../../../../figures/reduce_sum_adv_api.png)

To compute the process above, some essential concepts are introduced: the number of rows is called the **outer axis length (outter)**, the actual number of elements in each row is called the **actual number of elements on the inner axis (n)**, and the number of elements obtained by padding the byte length required to store n elements to a multiple of 32 and then converting it is called the **number of elements on the inner axis after padding \(inner\)**. This API requires the inner axis length of the input to be a multiple of 32 bytes. Therefore, when the byte length occupied by n is not a multiple of 32, you need to round it up to a multiple of 32. For example, in the following sample, the element type is half, the actual number of elements n in each row is 3, and the occupied byte length is 6 bytes, which is not a multiple of 32 bytes. After padding, 32 bytes are obtained, which is converted to 16 elements. Therefore, outter = 2, n = 3, and inner = 16. The padding in the figure represents the padding operation. The relationship between n and inner is as follows: **inner = \(n \*sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\)**.

![](../../../../figures/sum.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void Sum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SumParams& sumParams)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void Sum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const SumParams& sumParams)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetSumMaxMinTmpSize](GetSumMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| reduceDim | Specifies the dimension along which the data is summed. This API is implemented along the last dimension and does not support the **reduceDim** parameter. Pass the default value -1. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| isBasicBlock | Reserved parameter, which is not supported yet. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The output value requires outter * sizeof(T) space for storage. You need to allocate the actual memory space for **dstTensor** according to this size and the alignment requirements of the framework.<br>**Note**: Follow the memory allocation requirements of the framework (the allocated memory size must be 32-byte aligned). That is, when outter * sizeof(T) is not 32-byte aligned, it needs to be rounded up to 32-byte alignment. The extra memory space allocated for alignment is not filled with values, and contains some random values. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Sum, provided by you.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetSumMaxMinTmpSize](GetSumMaxMinTmpSize.md). |
| sumParams | Input | Shape information of **srcTensor**. The type is SumParams, and its definition is shown in the following code, where the parameters mean:<br>outter: Outer axis length of the input data.<br>inner: Number of elements after the actual number of elements on the inner axis of the input data is padded to 32-byte alignment, inner\*sizeof(T) must be a multiple of 32 bytes.<br>n: Actual number of elements on the inner axis of the input data.<br>Note: **sumParams.inner** is the value obtained by rounding sumParams.n up to 32-byte alignment, inner = (n *sizeof(T) + 32 - 1) / 32 * 32 / sizeof(T). Therefore, the size of sumParams.n must satisfy: 1 <= sumParams.n <= sumParams.inner. |

```
struct SumParams{
    uint32_t outter = 1; 
    uint32_t inner; 
    uint32_t n; 
};
```

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   Currently, only ND-format input is supported, and other formats are not supported.
-   For one-dimensional input, set **outter** to 1; for two-dimensional input, fill in **outter** and **n** according to the actual situation, and calculate **inner** using the formula above; otherwise, the function will not work correctly.
-   **srcTensor** must be able to accommodate the space occupied by the data after inner-axis alignment, and **dstTensor** must be able to accommodate the space occupied by the data after **outter** results are aligned.
-   For **Sum**, the underlying addition method used internally is the same as that used internally by [ReduceSum](../../../basic_api/memory_vector_compute/reduction_compute/ReduceSum.md) and [ReduceRepeat](../../../basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md), which uses a binary tree approach to add elements pairwise:

    Assume the source operand consists of 128 **half** data elements \[data0,data1,data2...data127\], which can be computed in one repeat. The computation process is as follows.

    1.  **data0** and **data1** are added to obtain **data00**, **data2** and **data3** are added to obtain **data01**... **data124** and **data125** are added to obtain **data62**, and **data126** and **data127** are added to obtain **data63**;
    2.  **data00** and **data01** are added to obtain **data000**, **data02** and **data03** are added to obtain **data001**... **data62** and **data63** are added to obtain **data031**;
    3.  By analogy, the destination operand is obtained as one **half** data element \[data\].

## Examples

```
// Define the shape information of srcTensor, where the input element type is half and the data is a 2*3 two-dimensional array.
AscendC::SumParams params;
// m is 2, and outter equals the value of m.
params.outter = outter;
// n is 3.
params.n = n;
// inner = (n * sizeof(half) + 32 - 1)/32 * 32 / sizeof(half) = 16
params.inner = inner;
// Value used for padding.
T scalar(0);
// yLocal is padded with 0. out_inner is 2 rounded up to a multiple of 16, which is 16. For the calculation method, see inner.
AscendC::Duplicate<T>(yLocal, scalar, out_inner);
// Perform the operation on a two-dimensional matrix with shape (2, 3). The output result is [6, 15].
// The 0s in xLocal and yLocal are the values used for padding during computation.
AscendC::Sum(yLocal, xLocal, sharedTmpBuffer, params);
```

The results are as follows.

```
Input data srcLocal: [[1 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0],
                     [4 5 6 0 0 0 0 0 0 0 0 0 0 0 0 0]]
Output data dstLocal: [6 15 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
```
