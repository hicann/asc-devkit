# ReduceMax

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:19:44.678Z pushedAt=2026-10-29T02:15:03.097Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceMax_interface/ReduceMax_res.md#id1 -->

## Description

Computes the maximum value of a multidimensional vector along a specified dimension.

The dimension on which the computation is performed (the Reduce axis) is defined as the R axis, and the other dimensions (the Normal axes) are defined as the A axes. As shown in the following figure, for a two-dimensional matrix with shape \(2, 3\), when the maximum value is computed along the first dimension, the output is \[4, 5, 6\]; when the maximum value is computed along the second dimension, the output is \[3, 6\]. The first dimension is the outer axis, and the second dimension is the inner axis.

**Figure 1**  Example of ReduceMax computation along the first dimension  
![](../../../../figures/reducemax_first_axis.png "Example of ReduceMax computation along the first dimension")

**Figure 2**  Example of ReduceMax computation along the last dimension  
![](../../../../figures/reducemax_last_axis.png "Example of ReduceMax computation along the last dimension")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework allocates temporary space

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be obtained in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter: this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call completes, so that the memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space: you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetReduceMaxMaxMinTmpSize](GetReduceMaxMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [supported data types](#li1946486111510). |
| pattern | Used to specify the ReduceMax computation axes, including the Reduce axis and the Normal axis. pattern is formed by a combination of the letters A and R, with the number of letters equal to the number of vector dimensions. The letter A indicates the Normal axis, and R indicates the Reduce axis. For example, AR indicates that ReduceMax computation is performed on a two-dimensional vector: the first dimension is the Normal axis and the second dimension is the Reduce axis, that is, the maximum value is computed along the second dimension.<br><br>pattern is a structure defined in the AscendC::Pattern::Reduce namespace, and you do not need to pay attention to its member variables.<br><br>pattern currently only supports the values AR and RA. |
| isReuseSource | Whether to allow modification of the source operand. The default value is false. If you allow the source operand to be overwritten, you can set this parameter to true to enable it, which saves some memory space.<br><br>When set to true, the src memory space is reused during the internal computation of this API, saving memory space; when set to false, the src memory space is not reused during the internal computation of this API.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Target operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of ReduceMax. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceMaxMaxMinTmpSize](GetReduceMaxMaxMinTmpSize.md). |
| srcShape | Input | Array of type uint32_t, representing the shape information of the source operand. The dimension of this shape must be consistent with the dimension of the template parameter pattern. Currently only two-dimensional shapes are supported. For example, if pattern is AR, the shape dimension can only be two-dimensional. |
| srcInnerPad | Input | Whether the innermost axis data to be actually computed is 32-byte aligned.<!-- npu="950" id7 --><br><br>For Ascend 950PR&950DT products, this parameter is reserved. The API internally calculates whether the innermost axis data is 32-byte aligned based on the srcShape and pattern parameters.<!-- end id7 --><!-- npu="A3" id8 --><br><br>For Atlas A3 products, currently only true is supported.<!-- end id8 --><!-- npu="910b" id9 --><br><br>For Atlas A2 products, currently only true is supported.<!-- end id9 --> |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   When srcInnerPad is True, the inner axis of the input vector must be an integer multiple of 32 bytes. When the byte length occupied by the inner axis length n of the original vector is not an integer multiple of 32, you need to pad it upward to an integer multiple of 32. After padding, the inner axis size is inner = \(n \* sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\).

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   Supported data types<a id="li1946486111510"></a>

    <!-- npu="950" id10 -->
    Ascend 950PR&950DT products support the following data types: int8\_t, uint8\_t, int16\_t, uint16\_t, half, bfloat16\_t, int32\_t, uint32\_t, float, int64\_t, and uint64\_t.
    <!-- end id10 -->
    <!-- npu="A3" id11 -->
    Atlas A3 products support the following data types: half and float.
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    Atlas A2 products support the following data types: half and float.
    <!-- end id12 -->

## Examples

-   Inner axis 32-byte aligned

    ```
    uint32_t shape[] = {2, 8};
    constexpr bool isReuse = true;
    // tmp is the passed temporary cache tensor, shape is the shape of the srcLocal input, and true indicates whether the address is 32-byte aligned.
    AscendC::ReduceMax<float, AscendC::Pattern::Reduce::AR, isReuse>(
        dstLocal, srcLocal, tmp, shape, true);
    ```

    The result example is as follows:

    ```
    The input and output data type is float.
    Input data (src):
    [[ 0.0 4.0 2.0 0.0 -1.0 2.0 -1.0 7.0],
     [ 0.0 1.0 -9.0 2.0 2.0 2.0 8.0 3.0]]
    Input pattern: AR
    Input shape: (2,8)
    Output data (dst): [7.0 8.0]
    ```

-   Inner axis not 32-byte aligned

    The data type is half, the input shape is \[2,5\], and the input pattern is AR.

    ```
    uint32_t shape[2] = {2, 5};
    ReduceMax<T, AscendC::Pattern::Reduce::AR, false>(dstLocal, srcLocal, tmp, shape, true);
    ```

    The result example is as follows:

    ```
    The data type of the original input and output is half
    Original input data (src):
    [[ 0.0 4.0 2.0 0.0 -1.0],
     [ 0.0 1.0 -9.0 2.0 2.0]]
    Input pattern: AR
    Original input shape: (2,5)
    Output data (dst): [4.0 2.0]
    ```
