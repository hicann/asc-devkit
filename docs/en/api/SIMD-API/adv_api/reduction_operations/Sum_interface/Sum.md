# Sum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:31:12.622Z pushedAt=2026-10-29T02:15:03.147Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/Sum_interface/Sum_res.md#id1 -->

## Description

Obtains the sum of elements in the last dimension.

If the input is a vector, the elements in the vector are added together; if the input is a matrix, the elements in each row are summed along the last dimension. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

As shown in the following figure, an operation is performed on a two-dimensional matrix with a shape of \(2, 3\), and the output result is \[6, 15\].

![](../../../../figures/reduce_sum_adv_api.png)

To compute the process above, some essential concepts are introduced: the number of rows is called the **outer axis length (outter)**, the actual number of elements in each row is called the **actual number of elements on the inner axis (n)**, and the number of elements obtained after rounding up the byte length required to store n elements to a multiple of 32 and converting it is called the **number of elements on the inner axis after padding (inner)**. This API requires the inner axis length of the input to be a multiple of 32 bytes. Therefore, when the byte length occupied by n is not a multiple of 32, you need to round it up to a multiple of 32. For example, in the following sample, the element type is half, the actual number of elements n in each row is 3, and the occupied byte length is 6 bytes, which is not a multiple of 32 bytes. After rounding up, 32 bytes are obtained, which is converted to 16 elements. Therefore, outter = 2, n = 3, and inner = 16. In the figure, padding represents the padding operation. The relationship between n and inner is as follows: **inner = \(n \*sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\)**.

![](../../../../figures/sum.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void Sum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SumParams& sumParams)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void Sum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const SumParams& sumParams)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetSumMaxMinTmpSize](GetSumMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| reduceDim | Specifies the dimension along which the data is summed. This API implements summation along the last dimension and does not support the reduceDim parameter. Pass the default value -1. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| isBasicBlock | Reserved parameter, not supported yet. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The output value requires a space of outter * sizeof(T) for storage. You need to allocate the actual memory space for dstTensor based on this size and the alignment requirements of the framework.<br>**Note**: Follow the framework's memory allocation requirements (the allocated memory size must satisfy 32-byte alignment). That is, when outter * sizeof(T) is not 32-byte aligned, it must be rounded up to 32-byte alignment. The extra memory space allocated for alignment is not filled with values and contains random values. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Sum. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSumMaxMinTmpSize](GetSumMaxMinTmpSize.md). |
| sumParams | Input | Shape information of srcTensor. The type is SumParams, whose definition is shown in the following code. The meanings of its parameters are as follows:<br>outter: outer axis length of the input data.<br>inner: number of elements after the actual number of elements on the inner axis of the input data is padded to 32 bytes. inner\*sizeof(T) must be an integer multiple of 32 bytes.<br>n: actual number of elements on the inner axis of the input data.<br>Note: sumParams.inner is the value of sumParams.n rounded up to 32-byte alignment, inner = (n *sizeof(T) + 32 - 1) / 32 * 32 / sizeof(T). Therefore, the size of sumParams.n must satisfy: 1 <= sumParams.n <= sumParams.inner. |

```
struct SumParams {
    uint32_t outter = 1;
    uint32_t inner;
    uint32_t n;
};
```

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   Currently, only ND-format input is supported. Other formats are not supported.
-   For one-dimensional input, set outter to 1. For two-dimensional input, fill in outter and n based on the actual situation, and calculate inner using the formula above. Otherwise, the function will be incorrect.
-   srcTensor must be able to accommodate the space occupied by the inner-axis-aligned data, and dstTensor must be able to accommodate the space occupied by the data after aligning outter results.
-   For Sum, the underlying addition method it uses internally is the same as that used internally by [ReduceSum](../../../basic_api/memory_vector_compute/reduction_compute/ReduceSum.md) and [ReduceRepeat](../../../basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md), which adopts a binary tree approach and adds elements pairwise:

    Assume that the source operand consists of 128 half-type data elements \[data0,data1,data2...data127\], which can be computed in one repeat. The computation process is as follows.

    1.  data0 and data1 are added to obtain data00, data2 and data3 are added to obtain data01, ..., data124 and data125 are added to obtain data62, and data126 and data127 are added to obtain data63;
    2.  data00 and data01 are added to obtain data000, data02 and data03 are added to obtain data001, ..., data62 and data63 are added to obtain data031;
    3.  By analogy, the destination operand is obtained as one half-type data element \[data\].

## Examples

```
// Define the shape information of srcTensor. The input element type is half, and the data is a 2D array of size 2*3.
AscendC::SumParams params;
// m is 2, and outter equals the value of m.
params.outter = outter;
// n is 3.
params.n = n;
// inner = (n * sizeof(half) + 32 - 1)/32 * 32 / sizeof(half) = 16
params.inner = inner;
// Value used for padding.
T scalar(0);
// yLocal is padded with 0. out_inner is the value obtained by rounding 2 up to 16, which is 16. For the computation method, see inner.
AscendC::Duplicate<T>(yLocal, scalar, out_inner);
// Perform the operation on a 2D matrix with shape (2, 3). The output is [6, 15].
// The 0 in xLocal and yLocal is the padding value used during computation.
AscendC::Sum(yLocal, xLocal, sharedTmpBuffer, params);
```

The result is as follows.

```
Input data srcLocal: [[1 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0],
                     [4 5 6 0 0 0 0 0 0 0 0 0 0 0 0 0]]
Output data dstLocal: [6 15 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
```
