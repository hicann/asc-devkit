# ReduceMean

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:20:47.544Z pushedAt=2026-10-29T02:15:03.100Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceMean_interface/ReduceMean_res.md#id1 -->

## Description

Computes the mean of a multi-dimensional vector along a specified dimension.

The dimension along which the computation is performed (the Reduce axis) is defined as the R axis, and the non-specified dimensions (the Normal axes) are defined as the A axes. As shown in the following figure, for a two-dimensional matrix with a shape of \(2, 3\), computing the mean along the first dimension yields \[2.5, 3.5, 4.5\]; computing the mean along the second dimension yields \[2, 5\].

**Figure 1**  Example of ReduceMean computation along the first dimension
![](../../../../figures/reducemean_first_axis.png "Example of ReduceMean computation along the first dimension")

**Figure 2**  Example of ReduceMean computation along the last dimension
![](../../../../figures/reducemean_last_axis_example.png "Example of ReduceMean computation along the last dimension")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceMean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework allocates temporary space

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceMean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter. This tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetReduceMeanMaxMinTmpSize](GetReduceMeanMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data type is float. |
| pattern | Used to specify the ReduceMean computation axes, including the Reduce axis and the Normal axis. pattern is formed by a combination of the letters A and R, the number of which is the same as the number of vector dimensions. The letter A indicates the Normal axis, and R indicates the Reduce axis. For example, AR indicates that ReduceMean computation is performed on a two-dimensional vector: the first dimension is the Normal axis, and the second dimension is the Reduce axis, that is, the average is computed over the data in the second dimension.<br><br>pattern is a structure defined in the AscendC::Pattern::Reduce namespace, and you do not need to pay attention to its member variables.<br><br>pattern currently supports only the values AR and RA. You need to explicitly specify pattern as AscendC::Pattern::Reduce::AR or AscendC::Pattern::Reduce::RA. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is false. If you allow the source operand to be overwritten, you can set this parameter to true to enable it, which saves some memory space.<br><br>If it is set to true, the src memory space is reused during the internal computation of this API, saving memory space. If it is set to false, the src memory space is not reused during the internal computation of this API.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Target operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of ReduceMean, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceMeanMaxMinTmpSize](GetReduceMeanMaxMinTmpSize.md). |
| srcShape | Input | Array of type uint32_t, indicating the shape information of the source operand. The dimension of this shape must be consistent with the dimension of the template parameter pattern, and currently supports only two-dimensional shapes. For example, if pattern is AR, the shape dimension can only be two-dimensional. |
| srcInnerPad | Input | Indicates whether the innermost axis data to be actually computed is 32-byte aligned. Currently supports only true. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   The internal algorithm does not handle data overflow during accumulation computation, and API precision is not guaranteed in overflow scenarios.

## Examples

```
uint32_t shape[] = {2, 8};
constexpr bool isReuse = true;
// tmp is the size of the passed temporary space, shape is the shape input by srcLocal, and true indicates whether the address is 32-byte aligned.
AscendC::ReduceMean<T, AscendC::Pattern::Reduce::AR, isReuse>(
    dstLocal, srcLocal, tmp, shape, true);
```

The result is as follows:

```
The input and output data type is float.
Input data (src):
[[ 0.0 4.0 2.0 0.0 -1.0 2.0 -1.0 7.0],
 [ 0.0 1.0 -9.0 2.0 2.0 2.0 8.0 3.0]]
Input pattern: AR
Input shape: (2,8)
Output data (dst): [1.625 1.125]
```
