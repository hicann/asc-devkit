# copy (Movement from Unified Buffer to L1 Buffer)

<!-- md-trans-meta sourceCommit=1c7a19aa96d0fd826ce85834da0ca73ec1f92cf6 translatedAt=2026-10-27T04:15:33.659Z pushedAt=2026-10-27T13:03:35.474Z nl=2 -->

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

This API is used to move data from the Unified Buffer (UB) to the L1 Buffer. It automatically selects the `copy_ub_to_l1` path based on the storage locations of the source and destination tensors, and derives the number of data blocks, single block length, and source/destination block gaps based on the Layout. It supports both contiguous and non-contiguous high-dimensional sliced movement.

Currently, ND2ND, DN2DN, NZ2NZ, and ZN2ZN copies are supported. The source and destination Layout Patterns must be identical. No format conversion or data type conversion is performed during the movement, and the data format and content remain unchanged. This path can be used to transfer data generated or preprocessed by vector computation to the L1 Buffer for subsequent cube computation or other on-chip flows.

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

The UB-to-L1 Buffer path uses `copy_ub_to_l1{}` as the movement operation object, with the default trait `ub_to_l1_trait_default{}`.

## Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which can be constructed via `make_copy(copy_ub_to_l1{})` or `make_copy(copy_ub_to_l1{}, ub_to_l1_trait_default{})`. |
| dst | Output | Destination tensor, whose storage location must be `location::l1`. |
| src | Input | Source tensor, whose storage location must be `location::ub`. |
| dst_coord | Input | Start coordinate of the destination region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| src_coord | Input | Start coordinate of the source region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| copy_shape | Input | Logical Shape of the movement region. The API derives the start address and movement parameters based on this Shape and the source/destination Layout. |

### Movement Parameter Derivation

`copy` automatically derives these parameters based on the Layout of the source and destination Tensors. The single block length and the source/destination block gaps are all measured in DataBlocks, where one DataBlock is 32 bytes.

For whole-Tensor copies, the derivation rules for each Layout pattern are as follows. In the table, "number of elements per DataBlock" equals `32 / sizeof(T)`; for the b4 type, it is calculated according to the corresponding 32-byte movement granularity.

| Layout Pattern | Number of Data Blocks | Single block length | Source/Destination Block Gap |
| :--- | :--- | :--- | :--- |
| ND | Total number of rows of the source Layout | Total number of columns divided by the number of elements per DataBlock, rounded up to the nearest multiple of 1 | Row stride of the corresponding Tensor minus the total number of columns, then divided by the number of elements per DataBlock and rounded up to the nearest multiple of 1 |
| DN | Total number of columns of the source Layout | Total number of rows divided by the number of elements per DataBlock, rounded up to the nearest multiple of 1 | Column stride of the corresponding Tensor minus the total number of rows, then divided by the number of elements per DataBlock and rounded up to the nearest multiple of 1 |
| NZ | Outer column count of the fractal format | Total number of rows in the Layout | Outer column stride of the corresponding Tensor divided by the number of elements per DataBlock, then subtract the single block length |
| ZN | Outer row count of the fractal format | Total number of columns in the Layout | Outer row stride of the corresponding Tensor divided by the number of elements per DataBlock, then subtract the single block length |

The derivation results correspond directly to `DataCopyParams`: the number of data blocks corresponds to `blockCount`, the single block length corresponds to `blockLen`, and the source/destination block gaps correspond to `srcGap` and `dstGap`, respectively. There is no need to construct or pass `DataCopyParams` when calling `copy`.

When using the region-movement overload, the API resolves the start address and the movement region based on `src_coord`, `dst_coord`, and `copy_shape`. For the ND and DN patterns, the parameters are derived from the number of rows and columns of the region; for the NZ and ZN patterns, the number of data blocks is also derived in combination with the intra-fractal axes and the Batch dimension.

## Data Type

The data types of the source tensor and the destination tensor must be the same.

| Source/Destination Layout | Supported Data Types |
| :--- | :--- |
| ND, DN, NZ, ZN | `int4b_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t` |

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The source Tensor must reside in the UB, and the destination Tensor must reside in the L1 Buffer.
- Both the source UB address and the destination L1 Buffer address must meet the 32-byte alignment requirement.
- The source and destination layout patterns must be the same. ND2ND, DN2DN, NZ2NZ, and ZN2ZN are supported, while on-the-fly format conversion is not supported.
- The source and target data types must be the same, and the capacity of the destination Tensor or destination region must not be smaller than the movement region.
- The movement length and the source/destination gaps are measured in units of 32-byte DataBlocks. When the size of an ND or DN region is not divisible by the number of elements in a DataBlock, the API rounds up to the nearest multiple of the DataBlock granularity, and the caller must ensure that the source and destination buffers have sufficient space accordingly.
- The parameters derived from the Layout must satisfy the field ranges of the underlying `asc_copy_ub2l1` instruction. The number of data blocks for a valid movement ranges from [1, 4095], the single block length ranges from [1, 65535] DataBlocks, and both the source block gap and the destination block gap range from [0, 65535] DataBlocks.
- The Tensor API requires that the source and destination Layouts and the regional movement Shape be valid; it cannot rely on a number of data blocks or single block length of 0 to represent a no-op.
- When multiple `copy` operations are executed consecutively and their destination L1 Buffer regions overlap, MTE2 pipeline synchronization must be inserted between the two copies, for example by calling `PipeBarrier<PIPE_MTE2>()`, to prevent the later movement from overwriting data that the earlier movement has not yet completed.

## Key Features

### DataBlock-Granularity Movement

The ND path copies by row, and the DN path copies by column. The API calculates the number of elements that a 32-byte DataBlock can hold based on the data type, and rounds up the logical movement length to the number of DataBlocks. The NZ and ZN paths derive the DataBlock parameters directly from the intra-fractal axes.

For example, one DataBlock of the `half` type contains 16 elements. When moving a row of 20 `half` elements, the single block length is rounded up to 2 DataBlocks, and the underlying layer actually processes the space of 32 `half` elements. Therefore, the source and destination buffers and Layout strides must cover the rounded-up range.

### Contiguous and Non-Contiguous Movement

When both the source and destination block gaps are 0, adjacent data blocks are contiguous, which is a contiguous movement. When either block gap is greater than 0, it is a non-contiguous movement; the API copies only the address range covered by the data blocks, without reading the source gap or writing to the destination gap. When the number of data blocks is 1, the block gap does not affect the movement result.

Taking the `half` data in the official `DataCopy` example as an example, each data block contains 8 DataBlocks, that is, 256 bytes:

| Number of Data Blocks | Single block length | Source Block Gap | Destination Block Gap | Copy Effect |
| :--- | :--- | :--- | :--- | :--- |
| 2 | 8 DataBlocks | 0 | 1 DataBlock | The two data blocks in the UB are read contiguously; when written to the L1 Buffer, a 32-byte gap is reserved between the two data blocks. |

### Partial Regional Movement

Region reload supports specifying different start coordinates in the source and destination tensors. The API generates multi-block copies based on `copy_shape` and Layout Stride, which can be used to write a partial rectangular region in the UB to a specified position in the L1 Buffer.

## Example

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t M = 8;
constexpr uint32_t N = 64;

__aicore__ inline void copy_ub_to_l1_example()
{
    __ubuf__ half ub_buf[M * N];
    __cbuf__ half l1_buf[M * N];

    auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn,
        asc::te::layout_trait_default<half>>(M, N);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(ub_buf), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::l1>(l1_buf), layout);

    asc::te::copy(dst, src);
}
```

The ND Layout in this example contains 8 rows, each with 64 `half` elements. One DataBlock contains 16 `half` elements, so the API derives `blockCount = 8`, `blockLen = 4`, `srcGap = 0`, and `dstGap = 0`, which is a contiguous movement.

The following example explicitly constructs a movement atomic object and copies a partial region:

```cpp
auto atom = asc::te::make_copy(asc::te::copy_ub_to_l1{}, asc::te::ub_to_l1_trait_default{});
asc::te::copy(atom, dst, src, asc::te::make_coord(1, 0), asc::te::make_coord(2, 0),
    asc::te::make_shape(2, 32));
```

This region contains 2 rows, with 32 `half` elements copied per row, and the single block length is 2 DataBlocks. The row stride of both the source and destination tensors is 64 elements, and 32 elements, that is, 2 DataBlocks, are reserved after the valid region of each row. Therefore, the API derives `blockCount = 2`, `blockLen = 2`, `srcGap = 2`, and `dstGap = 2`.
