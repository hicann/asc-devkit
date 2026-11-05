# ReduceProd

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:24:56.475Z pushedAt=2026-10-29T02:15:03.122Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceProd_interface/ReduceProd_res.md#id1 -->

## Description

Computes the product of a multidimensional vector along a specified dimension.

Define the dimension on which the computation is performed (the Reduce axis) as the R axis, and the non-specified dimension (the Normal axis) as the A axis. As shown in the following figure, for a two-dimensional matrix with shape \(2, 3\), when the product is computed along the first dimension, the output is \[4, 10, 18\]; when the product is computed along the second dimension, the output is \[6, 120\].

**Figure 1**  Example of ReduceProd computation along the first dimension  
![](../../../../figures/reduceprod_first_axis.png "Example of ReduceProd computation along the first dimension")

**Figure 2**  Example of ReduceProd computation along the last dimension  
![](../../../../figures/reduceprod_last_axis.png "Example of ReduceProd computation along the last dimension")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceProd(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework allocates temporary space.

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceProd(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passing through the sharedTmpBuffer input parameter** and **applying through the API framework**.

-   When temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes, so that memory is not repeatedly allocated and released. This provides higher flexibility and higher memory utilization.
-   When the API framework allocates temporary space, you do not need to allocate it, but you must reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetReduceProdMaxMinTmpSize](GetReduceProdMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data type is float. |
| pattern | Used to specify the ReduceProd computation axes, including the Reduce axis and the Normal axis. pattern is formed by a combination of the letters A and R, with the number of letters equal to the number of vector dimensions. The letter A indicates the Normal axis, and R indicates the Reduce axis. For example, AR indicates that ReduceProd computation is performed on a two-dimensional vector: the first dimension is the Normal axis and the second dimension is the Reduce axis, that is, the product is computed along the second dimension.<br><br>pattern is a structure defined in the AscendC::Pattern::Reduce namespace, and you do not need to pay attention to its member variables.<br><br>pattern currently only supports the values AR and RA. You need to explicitly specify pattern as AscendC::Pattern::Reduce::AR or AscendC::Pattern::Reduce::RA. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is false. If you allow the source operand to be overwritten, you can set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the src memory space is reused during internal computation of this API, saving memory space; if set to false, the src memory space is not reused during internal computation of this API.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Target operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of ReduceProd. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceProdMaxMinTmpSize](GetReduceProdMaxMinTmpSize.md). |
| srcShape | Input | Array of type uint32_t, representing the shape information of the source operand. The dimension of this shape must be consistent with that of the template parameter pattern. Currently, only two-dimensional shapes are supported. For example, if pattern is AR, the shape dimension can only be two-dimensional. |
| srcInnerPad | Input | Indicates whether the data of the innermost axis that actually needs to be computed is 32-byte aligned. Currently, only true is supported. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The internal algorithm does not handle data overflow during the multiplication-accumulation computation. In overflow scenarios, the API precision is not guaranteed.
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.

## Examples

```
// dstLocal: output result
// srcLocal: input data
// tmp: temporary space for storing intermediate results
// shape: shape information of the input data, currently only two dimensions are supported
uint32_t shape[] = {2, 8};
// isReuse: whether the source operand srcLocal is allowed to be modified. true: allowed, false: not allowed.
constexpr bool isReuse = true;
// Compute the product along the last axis, allowing modification of srcLocal.
AscendC::ReduceProd<float, AscendC::Pattern::Reduce::AR, isReuse>(dstLocal, srcLocal, tmp, shape, true);
```

The result example is as follows:

```
The data type of the input and output is float.
Input data (src):
[[ 1, 2, 3, 4, 5, 6, 7, 8],
 [ 1, 2, 0.5, 4, 0.5, 0.5, 5, 6]]
Input pattern: AR
Input shape: (2,8)

Output data (dst): [40320,30]
```
