# copy (Movement from Unified Buffer to Global Memory)

<!-- md-trans-meta sourceCommit=c9b7d4a1649837fcf9936b094fdf2aa5b580d86a translatedAt=2026-10-27T04:18:58.102Z pushedAt=2026-10-27T13:03:35.479Z nl=2 -->

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

This API copies data from the Unified Buffer (UB) to Global Memory. It automatically selects the `copy_ub_to_gm` path based on the storage locations of the source and destination tensors, and derives the number of blocks to move, single block length, source stride and destination stride based on the source/destination Layout. It supports movement with a single block length that is not 32-byte aligned.

ND2ND, DN2DN, NZ2NZ, and ZN2ZN copies are currently supported. No data type conversion is performed during the movement, and the data format and valid content remain unchanged. The API supports moving an entire tensor and moving a local region specified by coordinates and shape. The API uses the [L2 Cache Hint](../../tensor/tensor.md#set_l2_cache_hint) set on the destination tensor in Global Memory to determine the L2 cache mode for this movement.

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

The UB-to-Global Memory path uses `copy_ub_to_gm{}` as the movement operation object, with the default trait `ub_to_gm_trait_default{}`.

## Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which can be constructed via `make_copy(copy_ub_to_gm{})` or `make_copy(copy_ub_to_gm{}, ub_to_gm_trait_default{})`. |
| dst | Output | Destination tensor, whose storage location must be `location::gm`. The L2 cache mode for this movement is obtained from the L2 Cache Hint of the destination tensor. For the setting method, see [L2 Cache Mode](#l2-cache-mode). |
| src | Input | Source tensor, whose storage location must be `location::ub`. |
| dst_coord | Input | Start coordinate of the destination region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| src_coord | Input | Start coordinate of the source region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| copy_shape | Input | Logical Shape of the movement region. The API derives the start address and movement parameters based on this Shape and the source/destination Layout. |

### Movement Parameter Derivation

`copy` automatically derives `blockCount`, `blockLen`, the source stride and destination stride based on the Layout of the source and destination Tensors. In the Tensor API, the single block length and stride are both in bytes, and the stride represents the gap between the start addresses of adjacent data blocks.

For whole-tensor movement, the derivation rules for each Layout mode are as follows:

| Layout Mode | Number of blocks to move | Single block length | Source/destination stride |
| :--- | :--- | :--- | :--- |
| ND | Total number of rows of the source Layout | The smaller of the total column bytes of the source and destination Layouts | Row stride of the corresponding Tensor × element bytes |
| DN | Total number of columns of the source Layout | The smaller of the total row bytes of the source and destination Layouts | Column stride of the corresponding Tensor × element bytes |
| NZ | Outer column count of the fractal format | Total number of rows × inner column count × element bytes | Outer column stride of the corresponding Tensor × element bytes |
| ZN | Outer row count of the fractal format | Total column count × inner row count × element bytes | Outer row stride of the corresponding Tensor × element bytes |

## Data Type

The data types of the source tensor and the destination tensor must be the same.

| Source/Destination Layout | Supported Data Types |
| :--- | :--- |
| ND, DN, NZ, ZN | `int4b_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t` |

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The source Tensor must be located in UB, and the destination Tensor must be located in Global Memory.
- The source UB address must meet the 32-byte alignment requirement, and the destination Global Memory address supports 1-byte alignment.
- The source and destination Layout Patterns must be the same. ND2ND, DN2DN, NZ2NZ, and ZN2ZN are currently supported.
- The source and target data types must be the same, and the capacity of the destination Tensor or destination region must not be smaller than the movement region.
- The number of blocks to move, single block length, and stride derived from the Layout must satisfy the field range and alignment requirements of the underlying `asc_copy_ub2gm_align` instruction. The valid number of blocks to move ranges from [1, 4095], the single block length ranges from [1, 2097151] bytes, and the single block length must be an integer multiple of the element bytes.
- The valid source stride must be equal to the single block length or a positive integer multiple of 32 bytes. The former indicates that the data blocks in the UB are compactly arranged, while the latter indicates that each data block is arranged on a 32-byte boundary.
- Unlike the `DataCopyExtParams` API on some products, the Tensor API requires that the source/destination Layout and the regional movement Shape be valid, and cannot rely on a block count or single block length of 0 to represent a no-op.
- For the b4 type, the block length and stride are adjusted at the b8 movement granularity.

## Key Features

### Unaligned Copy-Out

The UB source address is 32-byte aligned, while the Global Memory destination allows 1-byte alignment. When the single block length is an integer multiple of 32 bytes, all data within the block is written to Global Memory. When the single block length is not an integer multiple of 32 bytes, the underlying instruction processes the UB-side tail at a 32-byte granularity, but only the valid bytes specified by the single block length are written to Global Memory; the padding data in the tail is not written to the destination Tensor.

Take an ND Tensor with element type `half` and movement shape `(2, 23)` as an example. The valid single block length is `23 × sizeof(half) = 46` bytes:

| UB Source Tensor Row Stride | GM Destination Tensor Row Stride | Derivation Result | Copy Method |
| :--- | :--- | :--- | :--- |
| 32 elements (64 bytes) | 23 elements (46 bytes) | 2 blocks, single block length of 46 bytes, source stride of 64 bytes, destination stride of 46 bytes | On the UB side, each block is arranged on a 32-byte boundary, and the trailing 18 bytes are not written to GM; on the GM side, valid data is arranged contiguously. |
| 23 elements (46 bytes) | 23 elements (46 bytes) | 2 blocks, single block length of 46 bytes, both the source stride and destination stride are 46 bytes | Valid data blocks in both UB and GM are compactly arranged. |

When the destination stride is greater than the single block length, a gap of `destination stride - single block length` bytes is reserved between adjacent valid data blocks in Global Memory, and this gap is not written.

### L2 Cache Mode

`copy` obtains the L2 Cache Hint from the destination tensor in Global Memory as the L2 cache mode for this movement, without requiring additional parameters to be passed to `copy`.

Before calling `copy`, you can set the L2 Cache Hint through the [set_l2_cache_hint()](../../tensor/tensor.md#set_l2_cache_hint) API of the destination tensor, and obtain the current configuration through the [get_cache_mode()](../../tensor/tensor.md#get_cache_mode) API. If not explicitly set, the default value is `asc::te::cache_mode::normal`.

```cpp
dst.set_l2_cache_hint(asc::te::cache_mode::normal);
auto cache_mode = dst.get_cache_mode();
asc::te::copy(dst, src);
```

## Example

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t M = 8;
constexpr uint32_t N = 64;

__aicore__ inline void copy_ub_to_gm_example(__gm__ half* gm_addr)
{
    __ubuf__ half ub_buf[M * N];

    auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn,
        asc::te::layout_trait_default<half>>(M, N);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(ub_buf), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::gm>(gm_addr), layout);

    asc::te::copy(dst, src);
}
```

The ND Layout in this example contains 8 rows, each with 64 `half` elements. Therefore, the API derives 8 blocks, a single block length of 128 bytes, and both the source stride and destination stride are 128 bytes. The single block length satisfies the 32-byte alignment requirement, and the data blocks in both UB and GM are arranged contiguously.

The following example explicitly constructs a movement atomic object and copies a local region whose single block length is not 32-byte aligned:

```cpp
auto atom = asc::te::make_copy(asc::te::copy_ub_to_gm{}, asc::te::ub_to_gm_trait_default{});
asc::te::copy(atom, dst, src, asc::te::make_coord(1, 0), asc::te::make_coord(2, 0),
    asc::te::make_shape(2, 23));
```

This region contains 2 rows, each moving 23 `half` elements. Therefore, the API derives 2 blocks and a single block length of 46 bytes. The row stride of both the source and destination Tensors is 128 bytes, so both the source stride and destination stride are 128 bytes. Only 46 bytes of valid data per block are written to GM, and the padding data at the tail of the UB side is not written; an 82-byte gap is reserved between adjacent valid data blocks in GM.
