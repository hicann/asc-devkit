# ReduceAny

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:15:42.566Z pushedAt=2026-10-29T02:15:03.084Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceAny_interface/ReduceAny_res.md#id1 -->

## Description

Performs a logical OR reduction on a multidimensional vector along a specified dimension.

Define the dimension on which the computation is performed (Reduce axis) as the R axis, and the other dimensions (Normal axis) as the A axis. As shown in the following figure, for a two-dimensional matrix with shape \(2, 3\), when the logical OR is performed along the first dimension, the output is \[1, 0, 1\]; when the logical OR is performed along the second dimension, the output is \[1, 1\].

**Figure 1**  Example of ReduceAny computation along the first dimension  
![](../../../../figures/reduceany_first_axis.png "Example of ReduceAny computation along the first dimension")

**Figure 2**  Example of ReduceAny computation along the last dimension  
![](../../../../figures/reduceany_last_axis.png "Example of ReduceAny computation along the last dimension")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceAny(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework allocates temporary space

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceAny(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in either of the following two ways: **passing it through the sharedTmpBuffer input parameter** or **having the API framework allocate it**.

-   Pass temporary space through the sharedTmpBuffer input parameter. In this case, the tensor is used as temporary space for processing, and the API framework no longer allocates it. This way, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   The API framework allocates temporary space. In this case, you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetReduceAnyMaxMinTmpSize](GetReduceAnyMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: uint8_t and float. |
| pattern | Specifies the ReduceAny computation axes, including the Reduce axis and the Normal axis. pattern is formed by a combination of the letters A and R, with the number of letters equal to the number of vector dimensions. The letter A indicates the Normal axis, and the letter R indicates the Reduce axis. For example, AR indicates that ReduceAny computation is performed on a two-dimensional vector: the first dimension is the Normal axis and the second dimension is the Reduce axis, that is, logical OR is computed on the data of the second dimension.<br><br>pattern is a structure defined in the AscendC::Pattern::Reduce namespace, and you do not need to pay attention to its member variables.<br><br>pattern currently supports only the values AR and RA. You need to explicitly specify pattern as AscendC::Pattern::Reduce::AR or AscendC::Pattern::Reduce::RA. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is false. If you allow the source operand to be overwritten, set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the internal computation of this API reuses the memory space of src, saving memory space. If set to false, the internal computation of this API does not reuse the memory space of src.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of ReduceAny. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceAnyMaxMinTmpSize](GetReduceAnyMaxMinTmpSize.md). |
| srcShape | Input | Array of the uint32_t type, indicating the shape information of the source operand. The dimension of this shape must be the same as that of the template parameter pattern. Currently, only two-dimensional shapes are supported. For example, if pattern is AR, the shape dimension can only be two-dimensional. |
| srcInnerPad | Input | Whether the data of the innermost axis to be actually computed is 32-byte aligned. Currently, only true is supported. |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The input data of the source operand srcTensor can only be 0 or 1.

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.

## Examples

```
uint32_t shape[] = {2, 8};
constexpr bool isReuse = true;
AscendC::ReduceAny<float, AscendC::Pattern::Reduce::AR, isReuse>(
    dstLocal, srcLocal, tmp, shape,
    true); // tmp is the size of the temporary space passed in, shape is the shape of the srcLocal input, and true indicates whether the address is 32B aligned.
```

The result example is as follows:

```
The data type of the input and output is float
Input data (src):
[[ 0.0 1.0 1.0 0.0 1.0 0.0 1.0 1.0],
 [ 0.0 1.0 0.0 1.0 0.0 1.0 1.0 0.0]]
Input pattern: AR
Input shape: (2,8)
Output data (dst): [1.0 1.0]
```
