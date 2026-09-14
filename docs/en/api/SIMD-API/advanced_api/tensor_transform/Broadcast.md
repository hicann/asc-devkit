# Broadcast

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T15:51:24.618Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

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

Taking the float type, ND format, broadcasting from \[m, 1\] to \[m, k\] as an example, the internal algorithm block diagram of the Broadcast high-order API is shown as follows.

**Figure 1**  Broadcast algorithm block diagram  
![](../../../figures/Broadcast_algorithm_block_diagram.png "Broadcast algorithm block diagram")

The computation process consists of the following steps, all performed on the vector:

1.  brcb step: Broadcast each element into a datablock.
2.  Copy step: Copy each datablock into multiple datablocks. In the k-aligned scenario, this is the result y.
3.  For the k-non-aligned scenario, use GatherMask to extract \[m, k\] elements, where k' indicates the size of k aligned upward to 32B.

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim], const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim], const uint32_t srcShape[dim])
    ```

-   Supports dynamic shape.

    Only supported on Ascend 950PR/Ascend 950DT.

    ```
    template<class T, int constRank=-1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr, bool constSrcInnerPad = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t* dstShape, const uint32_t* srcShape, BroadcastTiling* tiling)
    ```

This API requires additional temporary space to store intermediate variables during computation. Temporary space can be provided in two ways: **passed by you through the sharedTmpBuffer input parameter, or allocated by the API framework**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.
-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to apply for space for the tensor; when the API framework applies for temporary space, you need to reserve it. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetBroadCastMaxMinTmpSize](GetBroadCastMaxMinTmpSize.md) to obtain the size of the space that needs to be reserved.

In addition, a kernel-side API for computing Tiling is provided. It computes Tiling for the Broadcast implementation and obtains the Tiling result. The template parameters of this API have the same functions as those of the Broadcast API that supports dynamic shape. For descriptions of the other parameters, see [Table 5](#table5458981523).

-   **Kernel-side Tiling computation API**

    Only supported on Ascend 950PR/Ascend 950DT.

    ```
    template<class T, int constRank=-1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr>
    __aicore__ inline void GetBroadcastTilingInfo(uint32_t rank, const uint32_t* dstShape, const uint32_t* srcShape, bool srcInnerPad, BroadcastTiling& tiling)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [Supported Data Types](#li12616155731718). |
| dim | Dimension of the input/output tensor. Currently only 1D and 2D are supported. |
| axis | Dimension to be broadcast. Currently only 0 and 1 are supported. A value of 0 indicates broadcasting the first dimension, and a value of 1 indicates broadcasting the second dimension. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  Template parameter description of the API that supports dynamic shape

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Currently supports int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |
| constRank | Number of dimensions of the input/output tensor.<br>The default value -1 indicates a dynamic shape scenario, in which the value is calculated based on the rank parameter in the GetBroadcastTilingInfo API.<br>When constRank is greater than 0, it must be the same as the rank parameter in the GetBroadcastTilingInfo API. The currently supported range of rank is [1, 9]. |
| constDstShape | Shape of the output tensor. A uint32_t array.<br>If the value of any dimension in this array is 0, it indicates that this dimension is a dynamic scenario, and the actual value of this dimension is determined by the value of the corresponding dimension of the dstShape parameter.<br>If the value of any dimension in this array is greater than 0, it indicates that this dimension is a static scenario, and the value of this dimension is the same as the value of the corresponding dimension in the dstShape parameter.<br><br>This parameter is preset. Pass the default value nullptr. |
| constSrcShape | Shape of the input tensor. A uint32_t array.<br>If the value of any dimension in this array is 0, it indicates that this dimension is a dynamic scenario, and the actual shape is determined by the srcShape parameter.<br>If the value of any dimension in this array is greater than 0, it indicates that this dimension is a static scenario, and the value of this dimension is the same as the value of the corresponding dimension in the srcShape parameter.<br><br>This parameter is preset. Pass the default value nullptr. |
| constSrcInnerPad | Indicates whether the last dimension srcShape[rank-1] of the input is 32B-aligned, where rank is the number of dimensions of the input/output tensor.<br><br>This parameter is preset. Pass the default value false. |

**Table 3**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The data type of the source operand must be consistent with that of the destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| dstShape | Input | Shape of the output tensor: an array of the uint32_t type, with a length of 1 or 2. The number of dimensions of the input/output shapes must be consistent. |
| srcShape | Input | Shape of the input tensor: an array of the uint32_t type, with a length of 1 or 2. The number of dimensions of the input/output shapes must be consistent. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Broadcast, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetBroadCastMaxMinTmpSize](GetBroadCastMaxMinTmpSize.md). |

**Table 4**  Parameter description of the dynamic shape API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The data type of the source operand must be consistent with that of the destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| dstShape | Input | Shape of the output tensor: an array of the uint32_t type, with a length value range of [1, 9]. The number of dimensions of the input/output shapes must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied. |
| srcShape | Input | Shape of the input tensor: an array of the uint32_t type, with a length value range of [1, 9]. The number of dimensions of the input/output shapes must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied.<br><br>When the value of srcShape[i] is 1 and dstShape[i] is not equal to srcShape[i], it indicates that axis i is a broadcast axis. |
| tiling | Input | Tiling information required by the Broadcast API. The type is BroadcastTiling*, obtained by calling the tiling computation API GetBroadcastTilingInfo on the kernel side. |

**Table 5**  Parameter description of the kernel-side tiling computation API

<a name="table5458981523"></a>
| Parameter | Input/Output | Description |
| --- | --- | --- |
| rank | Input | Number of dimensions of the input/output tensor. The currently supported value range is [1, 9]. |
| dstShape | Input | Shape of the output tensor: an array of the uint32_t type, with a length value range of [1, 9]. The number of dimensions of the input/output shapes must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied. |
| srcShape | Input | Shape of the input tensor: an array of the uint32_t type, with a length value range of [1, 9]. The number of dimensions of the input/output shapes must be consistent, and the condition dstShape[i] >= srcShape[i] must be satisfied.<br><br>When the value of srcShape[i] is 1 and dstShape[i] is not equal to srcShape[i], it indicates that axis i is a broadcast axis. |
| srcInnerPad | Input | Indicates whether the last dimension srcShape[rank-1] of the input is 32B-aligned.<br><br>Currently only supports the value false. |
| tiling | Output | Tiling information returned by the computation. The type is BroadcastTiling&. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Currently, only ND-format input is supported, and other formats are not supported.
-   dim currently only supports 1 or 2, and axis currently only supports 0 or 1.
-   For the AI Core of Atlas inference products, when dim=2 and axis=1, srcShape\[0\] must be 32B-aligned. That is, when the input/output tensor has 2 dimensions and the broadcast dimension is 1, the data of dimension 0 of the input tensor must be a multiple of 32 bytes.
-   When dim=2 and axis=0, srcShape\[1\] must be 32B-aligned.
-   For Ascend 950PR/Ascend 950DT, the number of dimensions supported by the input/output tensor, that is, the supported value range of rank, is \[1, 9\].
-   Supported data types<a id="li12616155731718"></a>

    For Ascend 950PR/Ascend 950DT, the supported data types are the data types corresponding to b8, b16, b32, and b64 bit widths. For details about the specific data types, see [data types matching different bit widths](../../basic_api/data_structures/built_in_data_type.md#section16395539499).

    For Atlas A3 training products/Atlas A3 inference products, the supported data types are int8\_t, uint8\_t, half, and float.

    For Atlas A2 training products/Atlas A2 inference products, the supported data types are int8\_t, uint8\_t, half, and float.

    For the AI Core of Atlas inference products, the supported data types are int8\_t, uint8\_t, half, and float.

## Examples

```
// dstLocal: Output tensor.
// srcLocal: Input tensor.
const uint32_t srcShape[2] = {1, 16}; // Source data shape.
const uint32_t dstShape[2] = {16, 16};// broadcast data shape.
AscendC::Broadcast<float, 2, 1>(dstLocal, srcLocal, dstShape, srcShape); // The broadcast data type is float, the source data is 2-dimensional, and the first dimension is broadcast.
```

The results are as follows:

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
