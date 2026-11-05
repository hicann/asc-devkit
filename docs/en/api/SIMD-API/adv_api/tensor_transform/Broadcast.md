# Broadcast

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:09:58.555Z pushedAt=2026-10-27T11:54:36.420Z nl=2 -->

## Applicable Products

<!-- npu="950" id5 -->
- Ascend 950PR&950DT products: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/tensor_transform/Broadcast_res.md#id1 -->

## Description

Broadcasts the input according to the output shape.

For example, if the shape of A is \(2,1\) and the target broadcast shape is \(2,16\), the original single column is expanded into 16 identical columns.

```
Input data:
[[ 1]
 [ 2]]
Output data:
[[ 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1]
 [ 2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2]]
```

## Implementation Principle

Taking the float type, ND format, \[m, 1\] broadcasting to \[m, k\] as an example, the internal algorithm diagram of the Broadcast high-order API is described as shown in the following figure.

**Figure 1**  Broadcast algorithm diagram  
![](../../../figures/broadcast_algorithm_diagram.png "Broadcast algorithm diagram")

The computation process is divided into the following steps, all performed on Vector:

1.  brcb step: Broadcast each element into a datablock.
2.  Copy step: Copy each datablock into multiple datablocks. In the k-aligned scenario, this is the result y.
3.  For the k non-aligned scenario, use GatherMask to extract \[m, k\] elements, where k' indicates the size of k aligned upward to 32B.

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim], const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim], const uint32_t srcShape[dim])
    ```

-   Support dynamic shape.

    <!-- npu="950" id11 -->
    Supported only on Ascend 950PR&950DT products.
    <!-- end id11 -->

    ```
    template<class T, int constRank=-1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr, bool constSrcInnerPad = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t* dstShape, const uint32_t* srcShape, BroadcastTiling* tiling)
    ```

This API requires additional temporary space to store intermediate variables during computation. Temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **applying through the API framework**.

-   When passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, developers can manage the sharedTmpBuffer memory space by themselves and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, providing higher flexibility and higher memory utilization.
-   When the API framework applies for the temporary space, developers do not need to apply for it, but they need to reserve the size of the temporary space.

When passed through sharedTmpBuffer, developers need to apply for space for the tensor; when the API framework applies for it, developers need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetBroadCastMaxMinTmpSize](GetBroadCastMaxMinTmpSize.md).

In addition, an API is provided for computing Tiling on the kernel function (Kernel) side, which computes Tiling for the Broadcast implementation and obtains the Tiling result. The template parameter functions of this API are the same as those of the Broadcast API that supports dynamic shape. For descriptions of the remaining parameters, see [Table 5](#table5458981523).

-   **Kernel-side Tiling computation API**

    <!-- npu="950" id12 -->
    Supported only on Ascend 950PR&950DT products.
    <!-- end id12 -->

    ```
    template<class T, int constRank=-1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr>
    __aicore__ inline void GetBroadcastTilingInfo(uint32_t rank, const uint32_t* dstShape, const uint32_t* srcShape, bool srcInnerPad, BroadcastTiling& tiling)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Function |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [supported data types](#li12616155731718). |
| dim | Dimension of the input/output tensor. Currently only 1D and 2D are supported. |
| axis | Dimension to be broadcast. Currently only 0 and 1 are supported. A value of 0 indicates broadcasting the first dimension, and a value of 1 indicates broadcasting the second dimension. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved; pass the default value false. |

**Table 2**  Template parameter description of the API that supports dynamic shape

| Parameter | Function |
| --- | --- |
| T | Data type of the operand. Currently supports int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |
| constRank | Number of dimensions of the input/output tensor.<br>The default value -1 indicates a dynamic shape scenario, in which the value is calculated internally based on the **rank** parameter in the **GetBroadcastTilingInfo** API.<br>When **constRank** is greater than 0, it must be consistent with the value of the **rank** parameter in the **GetBroadcastTilingInfo** API. The currently supported range of **rank** is [1, 9]. |
| constDstShape | Shape of the output tensor. An array of the uint32_t type.<br>If the value of any dimension in this array is 0, it indicates that this dimension is a dynamic scenario, and the actual value of this dimension is determined by the value of the corresponding dimension in the **dstShape** parameter.<br>If the value of any dimension in this array is greater than 0, it indicates that this dimension is a static scenario, and the value of this dimension is the same as the value of the corresponding dimension in the **dstShape** parameter.<br><br>This parameter is reserved. Pass the default value nullptr. |
| constSrcShape | Shape of the input tensor. An array of the uint32_t type.<br>If the value of any dimension in this array is 0, it indicates that this dimension is a dynamic scenario, and the actual shape is determined by the **srcShape** parameter.<br>If the value of any dimension in this array is greater than 0, it indicates that this dimension is a static scenario, and the value is the same as the value of the corresponding dimension in the **srcShape** parameter.<br><br>This parameter is reserved. Pass the default value nullptr. |
| constSrcInnerPad | Indicates whether the last dimension srcShape[rank-1] of the input is 32B aligned, where **rank** is the number of dimensions of the input/output tensor.<br><br>This parameter is reserved. Pass the default value false. |

**Table 3**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Target operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The data type of the source operand must be consistent with that of the target operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| dstShape | Input | Shape of the output tensor: an array of the uint32_t type with a length of 1 or 2. The number of dimensions of the input/output shape must be consistent. |
| srcShape | Input | Shape of the input tensor: an array of the uint32_t type with a length of 1 or 2. The number of dimensions of the input/output shape must be consistent. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Broadcast. It is provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [GetBroadCastMaxMinTmpSize](GetBroadCastMaxMinTmpSize.md). |

**Table 4**  Parameter description of the API that supports dynamic shape

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Target operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The data type of the source operand must be consistent with that of the target operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| dstShape | Input | Shape of the output tensor: an array of the uint32_t type with a length value in the range [1, 9]. The number of dimensions of the input/output shape must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied. |
| srcShape | Input | Shape of the input tensor: an array of the uint32_t type with a length value in the range [1, 9]. The number of dimensions of the input/output shape must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied.<br><br>When the value of srcShape[i] is 1 and dstShape[i] is not equal to srcShape[i], it indicates that the i axis is a broadcast axis. |
| tiling | Input | Tiling information required by the Broadcast API. The type is BroadcastTiling*, obtained by calling the tiling computation API **GetBroadcastTilingInfo** on the kernel function (Kernel) side. |

**Table 5**  Parameter description of the kernel-side tiling computation API

<a name="table5458981523"></a>

| Parameter | Input/Output | Function |
| --- | --- | --- |
| rank | Input | Number of dimensions of the input/output tensor. The currently supported value is [1, 9]. |
| dstShape | Input | Shape of the output tensor: an array of the uint32_t type with a length value in the range [1, 9]. The number of dimensions of the input/output shape must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied. |
| srcShape | Input | Shape of the input tensor: an array of the uint32_t type with a length value in the range [1, 9]. The number of dimensions of the input/output shape must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied.<br><br>When the value of srcShape[i] is 1 and dstShape[i] is not equal to srcShape[i], it indicates that the i axis is a broadcast axis. |
| srcInnerPad | Input | Indicates whether the last dimension srcShape[rank-1] of the input is 32B aligned.<br><br>Currently only supports the value false. |
| tiling | Output | Tiling information returned by the computation. The type is BroadcastTiling&. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the target operand is not supported.**
-   Currently, only ND-format input is supported; other formats are not supported.
-   **dim** currently supports only 1 or 2, and **axis** currently supports only 0 or 1.
<!-- npu="310p" id13 -->
-   For the Atlas inference products AI Core, when **dim**=2 and **axis**=1, srcShape\[0\] must be 32B aligned. That is, when the input/output Tensor has 2 dimensions and the broadcast dimension is 1, the data of dimension 0 of the input Tensor must be a multiple of 32 bytes.
<!-- end id13 -->
-   When **dim**=2 and **axis**=0, srcShape\[1\] must be 32B aligned.
<!-- npu="950" id14 -->
-   For Ascend 950PR&950DT products, the number of dimensions supported by the input/output tensor, that is, the supported value range of rank, is \[1, 9\].
<!-- end id14 -->
-   Supported data types<a id="li12616155731718"></a>

    <!-- npu="950" id15 -->
    For Ascend 950PR&950DT products, the supported data types are the data types corresponding to b8, b16, b32, and b64 bit widths. For specific data types, see [Data Types Corresponding to Different Bit Widths](../../basic_api/data_structures/builtin_data_types.md#section16395539499).
    <!-- end id15 -->
    <!-- npu="A3" id16 -->
    For Atlas A3 products, the supported data types are int8\_t, uint8\_t, half, and float.
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    For Atlas A2 products, the supported data types are int8\_t, uint8\_t, half, and float.
    <!-- end id17 -->
    <!-- npu="310p" id18 -->
    For the Atlas inference products AI Core, the supported data types are int8\_t, uint8\_t, half, and float.
    <!-- end id18 -->

## Examples

```
// dstLocal: output Tensor
// srcLocal: input Tensor
const uint32_t srcShape[2] = {16, 1};  // source data shape
const uint32_t dstShape[2] = {16, 16}; // broadcast data shape
AscendC::Broadcast<float, 2, 1>(
    dstLocal, srcLocal, dstShape, srcShape); // broadcast data type is float, source data is 2-dimensional, broadcast the 1st dimension
```

The result example is as follows:

```
Input data (srcLocal):
[[ 1]
 [ 2]
 [ 3]
 [ 4]
 [ 5]
 [ 6]
 [ 7]
 [ 8]
 [ 9]
 [10]
 [11]
 [12]
 [13]
 [14]
 [15]
 [16]]
dim: 2
axis: 1
Output data (dstLocal):
[[ 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1]
 [ 2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2]
 [ 3  3  3  3  3  3  3  3  3  3  3  3  3  3  3  3]
 [ 4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4]
 [ 5  5  5  5  5  5  5  5  5  5  5  5  5  5  5  5]
 [ 6  6  6  6  6  6  6  6  6  6  6  6  6  6  6  6]
 [ 7  7  7  7  7  7  7  7  7  7  7  7  7  7  7  7]
 [ 8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8]
 [ 9  9  9  9  9  9  9  9  9  9  9  9  9  9  9  9]
 [10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10]
 [11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11]
 [12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12]
 [13 13 13 13 13 13 13 13 13 13 13 13 13 13 13 13]
 [14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14]
 [15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15]
 [16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]]
```
