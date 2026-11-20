# copy (Movement from Unified Buffer to Unified Buffer)

<!-- md-trans-meta sourceCommit=c9b7d4a1649837fcf9936b094fdf2aa5b580d86a translatedAt=2026-10-27T04:16:02.888Z pushedAt=2026-10-27T13:03:35.477Z nl=2 -->

> [!NOTE]Note
> This is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/algorithm/copy.h`, and the namespace is `asc::te`.

This API is used to perform high-dimensional splitting movement within the Unified Buffer (UB). Based on the storage locations of the source and destination tensors, the API automatically selects the `copy_ub_to_ub` path, and derives the number of data blocks, the single block length, and the source/destination inter-block intervals according to the Layout. It supports both contiguous and non-contiguous copies.

ND2ND, DN2DN, NZ2NZ, and ZN2ZN copies are supported. The source and destination Layout patterns must be identical. No format conversion or data type conversion is performed during the movement, and the data format and content remain unchanged. This API is suitable for adjusting the data storage location in the UB and for moving local regions between tensors with strides.

## Function Prototype

```cpp
template <typename DstTensor, typename SrcTensor>
__aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)

template <typename Atom, typename DstTensor, typename SrcTensor>
__aicore__ inline void copy(const copy_atom<Atom>& atom,
    const DstTensor& dst, const SrcTensor& src)

template <typename DstTensor, typename SrcTensor, typename DstCoord,
    typename SrcCoord, typename CopyShape>
__aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src,
    const DstCoord& dst_coord, const SrcCoord& src_coord,
    const CopyShape& copy_shape)

template <typename CopyOperation>
__aicore__ inline constexpr auto make_copy(const CopyOperation& operation)

template <typename CopyOperation, typename CopyTrait>
__aicore__ inline constexpr auto make_copy(
    const CopyOperation& operation, const CopyTrait& trait)
```

The UB-to-UB path uses `copy_ub_to_ub{}` as the movement operation object, with the default trait `ub_to_ub_trait_default{}`.

## Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which can be constructed via `make_copy(copy_ub_to_ub{})` or `make_copy(copy_ub_to_ub{}, ub_to_ub_trait_default{})`. |
| dst | Output | Destination tensor, whose storage location must be `location::ub`. |
| src | Input | Source tensor, whose storage location must be `location::ub`. |
| dst_coord | Input | Start coordinate of the destination region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| src_coord | Input | Start coordinate of the source region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| copy_shape | Input | Logical Shape of the movement region. The API derives the start address and movement parameters based on this Shape and the source/destination Layout. |

### Parameter Derivation

`copy` automatically derives the movement parameters required for [asc_copy_ub2ub high-dimensional splitting movement](../../../c_api/vector_datamove/asc_copy_ub2ub.md#parameters) based on the source and destination tensor layouts, so you do not need to pass them manually. For whole-tensor movement, the derivation rules for each layout mode are as follows.

| Layout Mode | Number of Data Blocks | Single Block Length (Bytes) | Source/Destination Inter-Block Interval (Bytes) |
| :--- | :--- | :--- | :--- |
| ND | Total number of rows in the Layout | Total number of columns in the Layout × element bytes | Row Stride of the corresponding Tensor × element bytes - single block length |
| DN | Total number of columns in the Layout | Total number of rows in the Layout × element bytes | Column stride of the corresponding tensor × element bytes - single block length |
| NZ | Outer column count of the fractal format | Total number of rows × inner column count × element bytes | Outer column stride of the corresponding tensor × element bytes - single block length |
| ZN | Outer row count of the fractal format | Total column count × inner row count × element bytes | Outer row stride of the corresponding tensor × element bytes - single block length |

The correspondence between the derived results and the `asc_copy_ub2ub` high-dimensional splitting movement parameters is as follows:

| Derivation Result | Corresponding Parameter | Conversion Relationship |
| :--- | :--- | :--- |
| Number of Data Blocks | `burst_count` | No conversion required. |
| Single Block Length (Bytes) | `burst_len` | Divide the single block length by 32. The unit is DataBlock (32 bytes). |
| Source Inter-Block Interval (Bytes) | `src_gap` | Divide the source inter-block interval by 32. The unit is DataBlock (32 bytes). |
| Destination Inter-Block Interval (Bytes) | `dst_gap` | Divide the destination inter-block interval by 32. The unit is DataBlock (32 bytes). |

When the regional movement overload is used, the API resolves the start address and movement region based on `src_coord`, `dst_coord`, and `copy_shape`, and derives the number of data blocks and single block length based on the region shape; the inter-block interval is still determined by the source and destination layout.

## Data Type

The data types of the source tensor and the destination tensor must be the same.

| Source/Destination Layout | Supported Data Types |
| :--- | :--- |
| ND, DN, NZ, ZN | `int4b_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t` |

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- Both the source Tensor and the destination Tensor must reside in the UB, and both the source and destination addresses must meet the 32-byte alignment requirement.
- The source and destination Layout Patterns must be identical. ND2ND, DN2DN, NZ2NZ, and ZN2ZN are supported.
- The source and target data types must be the same, and the capacity of the destination Tensor or destination region must not be smaller than the movement region.
- The single-block length and the source and destination inter-block intervals must be integer multiples of 32 bytes, and the Layout derived result must satisfy this alignment requirement.
- The number of data blocks derived from the Layout ranges from [1, 4095]; the single block length ranges from [1, 65535] DataBlocks. Both the source and destination inter-block intervals are represented by 16-bit fields, ranging from [0, 65535] DataBlocks.
- Unlike the `DataCopyParams` API on some products, the Tensor API requires that both the number of data blocks and the single block length derived from a valid Layout be non-zero.
- For the b4 type, the block length and inter-block interval are adjusted according to the b8 movement granularity.

## Key Features

### Whole Tensor and Partial Regional Movement

`copy(dst, src)` generates movement parameters based on the complete data range described by the source Layout. Region overloading copies only the region specified by `copy_shape`, and uses `src_coord` and `dst_coord` to calculate the start addresses respectively.

### Contiguous and Non-Contiguous Movement

When both the source and destination inter-block intervals are 0, adjacent data blocks are connected end to end, which is a contiguous movement. When the source or destination inter-block interval is greater than 0, it is a non-contiguous movement; the API copies only the region covered by the data blocks, without reading the source interval or writing to the destination interval. When the number of data blocks is 1, the inter-block interval does not affect the movement result.

Taking an ND Tensor with element type `int8_t` and movement shape `(2, 32)` as an example, the single block length is 32 bytes, that is, 1 DataBlock:

| Scenario | Source Tensor Row Stride | Destination Tensor Row Stride | Derivation Result |
| :--- | :--- | :--- | :--- |
| Contiguous Movement | 32 elements | 32 elements | `blockCount = 2`, `blockLen = 1`, `srcGap = 0`, `dstGap = 0` |
| Non-contiguous movement | 64 elements | 96 elements | `blockCount = 2`, `blockLen = 1`, `srcGap = 1`, `dstGap = 2` |

The above parameters are used only to illustrate the Layout derivation result. There is no need to construct or pass `DataCopyParams` when calling `copy`.

### ZN2ZN Fractal Copy

The ZN Layout splits a logical two-dimensional Shape into `((C0, ceil_div(M, C0)), (16, ceil_div(N, 16)))`, where `C0` is determined by the element type. For a ZN2ZN whole-tensor movement, data blocks are split along the outer-row direction:

- `blockCount` is the number of outer rows, `ceil_div(M, C0)`.
- `blockLen` is the total logical column count multiplied by the inner-row count and the element byte count, that is, `N × C0 × sizeof(T)` bytes.
- `srcGap` and `dstGap` are the outer-row stride of the corresponding Tensor minus `blockLen`, converted to the number of 32-byte DataBlocks.

Both the source Tensor and the destination Tensor must use the ZN Layout. The API copies only between identical fractal formats and does not change the fractal layout.

## Example

The following example performs movement within the UB: the source and destination tensors are located in the same UB address space, and the movement parameters (blockCount, blockLen, srcGap, dstGap) are automatically derived from the shape and stride of the layout, requiring no explicit configuration. The complete tensor movement form is as follows:

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t M = 8;
constexpr uint32_t N = 64;

__aicore__ inline void copy_ub_to_ub_example()
{
    __ubuf__ int8_t src_buf[M * N];
    __ubuf__ int8_t dst_buf[M * N];

    auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn,
        asc::te::layout_trait_default<int8_t>>(M, N);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src_buf), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst_buf), layout);

    asc::te::copy(dst, src);
}
```

The ND Layout in this example contains 8 rows, each with 64 `int8_t` elements. Therefore, the API derives `blockCount = 8`, `blockLen = 2`, `srcGap = 0`, and `dstGap = 0`.

An example of a partial-regional movement is as follows:

```cpp
asc::te::copy(dst, src, asc::te::make_coord(0, 0), asc::te::make_coord(2, 0),
    asc::te::make_shape(4, 32));
```

This region contains 4 rows, and 32 `int8_t` elements are copied per row. The row stride of both the source and destination Tensors is 64 bytes. Therefore, the API derives `blockCount = 4`, `blockLen = 1`, `srcGap = 1`, and `dstGap = 1`.

An example of a ZN2ZN fractal movement is as follows:

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t ZN_M = 32;
constexpr uint32_t ZN_N = 32;

__aicore__ inline void copy_ub_to_ub_zn_example()
{
    __ubuf__ half src_buf[ZN_M * ZN_N];
    __ubuf__ half dst_buf[ZN_M * ZN_N];

    auto layout = asc::te::make_frame_layout<asc::te::zn_layout_ptn,
        asc::te::layout_trait_default<half>>(ZN_M, ZN_N);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src_buf), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst_buf), layout);

    asc::te::copy(dst, src);
}
```

For `half`, `C0` is 16. The ZN Layout in this example is `((16, 2), (16, 2))`, and the API generates two data blocks along the outer row direction. Each data block contains 32 columns and 16 inner row elements, totaling 1024 bytes, that is, 32 DataBlocks. Both the source and destination Layouts are compactly arranged, so `srcGap = 0` and `dstGap = 0`.
