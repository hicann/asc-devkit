# ReduceSum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:27:58.959Z pushedAt=2026-10-29T02:15:03.144Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceSum_interface/ReduceSum_res.md#id1 -->

## Description

Accumulates data of a multidimensional vector along a specified dimension.

The dimension on which the computation is specified (the Reduce axis) is defined as the R axis, and the non-specified dimension (the Normal axis) is defined as the A axis. As shown in the following figure, for a two-dimensional matrix with the shape \(2, 3\), when the accumulation is specified on the first dimension, the output is \[5, 7, 9\]; when the accumulation is specified on the second dimension, the output is \[6, 15\].

**Figure 1**  Example of ReduceSum computation along the first dimension  
![](../../../../figures/reducesum_first_axis.png "Example of ReduceSum computation along the first dimension")

**Figure 2**  Example of ReduceSum computation along the last dimension  
![](../../../../figures/reducesum_last_axis.png "Example of ReduceSum computation along the last dimension")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceSum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework allocates temporary space

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceSum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in either of the following two ways: **passing through the sharedTmpBuffer input parameter** or **allocated by the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter: this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   The API framework allocates temporary space: you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetReduceSumMaxMinTmpSize](GetReduceSumMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. For the data types supported by different models, see [supported data types](#li778844571310). |
| pattern | Specifies the computation axes of ReduceSum, including the Reduce axis and the Normal axis. pattern is formed by combining the letters A and R, with the number of letters equal to the number of vector dimensions. The letter A indicates the Normal axis, and R indicates the Reduce axis. For example, AR indicates ReduceSum computation on a two-dimensional vector: the first dimension is the Normal axis and the second dimension is the Reduce axis, that is, data accumulation is computed along the second dimension.<br><br>pattern is a structure defined in the AscendC::Pattern::Reduce namespace, and you do not need to pay attention to its member variables.<br><br>pattern currently only supports the values AR and RA. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is false. If you allow the source operand to be overwritten, you can set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the src memory space is reused during the internal computation of this API, saving memory space. If set to false, the src memory space is not reused during the internal computation of this API.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Target operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of ReduceSum. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceSumMaxMinTmpSize](GetReduceSumMaxMinTmpSize.md). |
| srcShape | Input | Array of the uint32_t type, representing the shape information of the source operand. The dimension of this shape must be consistent with that of the template parameter pattern. Currently only two-dimensional shapes are supported. For example, if pattern is AR, the shape dimension can only be two-dimensional. |
| srcInnerPad | Input | Whether the innermost axis data to be actually computed is 32-byte aligned. <!-- npu="950" id7 --><br><br>For Ascend 950PR&950DT products, this parameter is reserved. The API internally calculates whether the innermost axis data is 32-byte aligned based on the srcShape and pattern parameters. <!-- end id7 --><!-- npu="A3" id8 --><br><br>For Atlas A3 products, currently only true is supported. <!-- end id8 --><!-- npu="910b" id9 --><br><br>For Atlas A2 products, currently only true is supported. <!-- end id9 --> |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source or destination operand addresses is not supported.
-   The internal algorithm does not handle data overflow during accumulation. In overflow scenarios, the API precision is not guaranteed.
-   Supported data types<a id="li778844571310"></a>

    <!-- npu="950" id10 -->
    For Ascend 950PR&950DT products, the supported data types are: int32\_t, uint32\_t, float, int64\_t, and uint64\_t.
    <!-- end id10 -->
    <!-- npu="A3" id11 -->
    For Atlas A3 products, the supported data type is: float.
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    For Atlas A2 products, the supported data type is: float.
    <!-- end id12 -->

## Examples

```
uint32_t shape[] = {2, 8};
constexpr bool isReuse = true;
// tmp is the size of the temporary space passed in, shape is the shape of the srcLocal input, and true indicates whether the address is 32B-aligned.
AscendC::ReduceSum<float, AscendC::Pattern::Reduce::AR, isReuse>(
    dstLocal, srcLocal, tmp, shape, true);
```

The result is as follows:

```
The data type of the input and output is float.
Input data (src):
[[ 0.0 4.0 2.0 0.0 -1.0 2.0 -1.0 7.0],
 [ 0.0 1.0 -9.0 2.0 2.0 2.0 8.0 3.0]]
Input pattern: AR
Input shape: (2,8)
Output data (dst): [13.0 9.0]
```
