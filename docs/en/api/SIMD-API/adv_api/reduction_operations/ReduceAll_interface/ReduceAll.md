# ReduceAll

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:14:31.345Z pushedAt=2026-10-29T02:15:03.075Z nl=2 -->

## Product Support

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
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
- Atlas inference products AI Core: not supported
- Vector Core of Atlas inference products: not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceAll_interface/ReduceAll_res.md#id1 -->

## Description

Performs a logical AND reduction on a multi-dimensional vector along a specified dimension.

The dimension specified for computation (Reduce axis) is defined as the R axis, and the non-specified dimension (Normal axis) is defined as the A axis. As shown in the following figure, for a two-dimensional matrix with shape \(2, 3\), when the logical AND is specified along the first dimension, the output result is \[1, 0, 0\]; when the logical AND is specified along the second dimension, the output result is \[0, 0\].

**Figure 1**  Example of ReduceAll computation along the first dimension  
![](../../../../figures/reduceall_first_axis.png "Example of ReduceAll computation along the first dimension")

**Figure 2**  Example of ReduceAll computation along the last dimension  
![](../../../../figures/reduceall_last_axis.png "Example of ReduceAll computation along the last dimension")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceAll(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework allocates temporary space

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceAll(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **applying through the API framework**.

-   When temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates space. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   When the API framework allocates temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetReduceAllMaxMinTmpSize](GetReduceAllMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are: uint8_t, float. |
| pattern | Specifies the computation axis of ReduceAll, including the Reduce axis and the Normal axis. pattern is formed by combining the letters A and R, the number of which equals the number of vector dimensions. The letter A indicates the Normal axis, and R indicates the Reduce axis. For example, AR indicates that ReduceAll computation is performed on a two-dimensional vector: the first dimension is the Normal axis and the second dimension is the Reduce axis, that is, logical AND is computed on the second-dimension data.<br><br>pattern is a structure defined in the AscendC::Pattern::Reduce namespace, and you do not need to pay attention to its member variables.<br><br>pattern currently only supports the values AR and RA. You need to explicitly specify pattern as AscendC::Pattern::Reduce::AR or AscendC::Pattern::Reduce::RA. |
| isReuseSource | Whether to allow modification of the source operand. The default value is false. If you allow the source operand to be overwritten, you can set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the memory space of src is reused during internal computation of this API, saving memory space. If set to false, the memory space of src is not reused during internal computation of this API.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of ReduceAll. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceAllMaxMinTmpSize](GetReduceAllMaxMinTmpSize.md). |
| srcShape | Input | Array of the uint32_t type, indicating the shape information of the source operand. The dimension of this shape must be consistent with the dimension of the template parameter pattern. Currently, only two-dimensional shapes are supported. For example, if pattern is AR, the shape dimension can only be two-dimensional. |
| srcInnerPad | Input | Indicates whether the data of the innermost axis that actually needs to be computed is 32-byte aligned. Currently, only true is supported. |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The input data of the source operand srcTensor can only be 0 or 1.

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the addresses of the source operand and destination operand is not supported.

## Examples

```
// dstLocal: output result
// srcLocal: input data
// tmp: temporary space for storing intermediate results
// shape: shape information of the input data. Currently only 2 dimensions are supported.
uint32_t shape[] = {2, 8};
// isReuse: Whether the source operand srcLocal can be modified. true: allowed; false: not allowed.
constexpr bool isReuse = true;
// Perform logical AND along the last axis.
AscendC::ReduceAll<float, AscendC::Pattern::Reduce::AR, isReuse>(dstLocal, srcLocal, tmp, shape, true);
```

The result is as follows:

```
The data type of the input and output is float.
Input data (src):
[[ 0.0 1.0 1.0 0.0 1.0 0.0 1.0 1.0],
 [ 0.0 1.0 0.0 1.0 0.0 1.0 1.0 0.0]]
Input pattern: AR
Input shape: (2,8)
Output data (dst): [0.0 0.0]
```
