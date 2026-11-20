# copy (Movement from Global Memory to Unified Buffer)

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:05:10.257Z pushedAt=2026-11-19T05:54:06.681Z nl=2 -->

> [!NOTE]Description
> This is an experimental API and may be adjusted or improved in later versions. Developers should pay attention to updates in later versions when using it.

## Supported Products

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/algorithm/copy.h`, and the namespace is `asc::te`.

The Tensor API uses the `copy` API to uniformly perform movement over different paths. This API copies data from Global Memory to the Unified Buffer (UB). Based on the storage locations of the source tensor and destination tensor, the API automatically selects the `copy_gm_to_ub` path, and derives the number of movement blocks, single-block length, source stride, and destination stride according to the layout, implementing contiguous or non-contiguous high-dimensional sliced movement.

Currently, ND2ND, DN2DN, NZ2NZ, and ZN2ZN copies are supported. The source and destination layout patterns must be the same. No format conversion is performed during the movement, and the data format and content remain unchanged.

In addition to whole-tensor movement, the API also supports specifying a local region through the destination coordinates, source coordinates, and movement shape. When left and right padding needs to be added on the destination side, it can be configured through `gm_to_ub_params` and bound to the movement atomic object using `copy_atom::with`.

## Prototype

- Automatically derive the path based on the storage locations of the source and destination tensors.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Use the movement atomic object to perform movement.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- Copy a specified region.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src,
        const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Construct the movement atomic object.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)

    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(
        const CopyOperation& operation, const CopyTrait& trait)
    ```

The Global Memory-to-UB path uses `copy_gm_to_ub{}` as the movement operation object, with the default trait being `gm_to_ub_trait_default{}`.

## Parameters

**Table 1** `copy` API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which can be constructed through `make_copy(copy_gm_to_ub{})` or `make_copy(copy_gm_to_ub{}, gm_to_ub_trait_default{})`. |
| dst | Output | Destination tensor, whose storage location must be `location::ub`. |
| src | Input | Source tensor, whose storage location must be `location::gm`. |
| dst_coord | Input | Start coordinate of the destination region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| src_coord | Input | Start coordinate of the source region. The structure of the explicit coordinate must be consistent with `copy_shape`, and `zero_coord` can also be used. |
| copy_shape | Input | Logical shape of the movement region. The API derives the address and movement parameters based on this shape and the source/destination layout. |

### Parameter Derivation

`copy` automatically derives the underlying multi-block movement parameters based on the source and destination Tensor layouts. In the following table, the single-block length and stride are all in bytes; the source and destination strides respectively represent the gap between the start addresses of adjacent data blocks.

| Layout Mode | Block Count Derivation | Single-Block Length Derivation | Source/Destination Stride Derivation |
| :--- | :--- | :--- | :--- |
| ND | Total number of rows in the Layout | Total number of columns in the Layout × element bytes | Row stride of the corresponding Tensor × element bytes |
| DN | Total number of columns in the Layout | Total number of rows in the Layout × element bytes | Column stride of the corresponding Tensor × element bytes |
| NZ | Outer column count of the fractal format | Total number of rows × inner column count × element bytes | Outer column stride of the corresponding Tensor × element bytes |
| ZN | Outer row count of the fractal format | Total column count × inner row count × element bytes | Outer row stride of the corresponding Tensor × element bytes |

- When the source and destination strides are both equal to the single-block length, the data blocks are arranged contiguously in the corresponding storage space.
- When the source or destination stride is greater than the single-block length, the API performs a non-contiguous movement; the gap is described by the Layout stride of the corresponding Tensor and does not need to be configured separately.

### gm_to_ub_params Description

```cpp
struct gm_to_ub_params {
    uint8_t left_padding_count = 0;
    uint8_t right_padding_count = 0;
    bool enable_constant_pad = true;
};
```

| Member | Default Value | Description |
| :--- | :---: | :--- |
| left_padding_count | `0` | Number of padding elements on the left side of each movement block. |
| right_padding_count | `0` | Number of padding elements on the right side of each movement block. |
| enable_constant_pad | `true` | Controls the padding method in non-32-byte-aligned scenarios. Takes effect only when both `left_padding_count` and `right_padding_count` are `0`: when set to `true`, constant padding is performed using the value set by [set_copy_pad_val](../../data_move/set_copy_pad_val.md); when set to `false`, dummy data is automatically padded by hardware, with the padding value being the first element of each transfer block. When any padding count is not `0`, this parameter does not take effect, and both left and right padding use the value set by `set_copy_pad_val`. |

Bind the runtime movement parameters in the following way:

```cpp
asc::te::gm_to_ub_params params{left_padding, right_padding, true};
auto atom = asc::te::make_copy(asc::te::copy_gm_to_ub{}).with(params);
asc::te::copy(atom, dst, src);
```

When `enable_constant_pad` is set to `true`, [set_copy_pad_val](../../data_move/set_copy_pad_val.md) must be called before transfer to set the padding value. For example, the following code sets the padding value in non-32-byte-aligned scenarios to `0`:

```cpp
#include "tensor_api/tensor.h"

asc::te::set_copy_pad_val<half>(static_cast<half>(0));
asc::te::gm_to_ub_params params{0, 0, true};
auto atom = asc::te::make_copy(asc::te::copy_gm_to_ub{}).with(params);
asc::te::copy(atom, dst, src);
```

The value set by `set_copy_pad_val` continues to take effect on subsequent transfers until this API is called again to overwrite the configuration. Therefore, if `enable_constant_pad` is `true` but no padding value is set, the API uses the current value already in the padding register, whose initial value should not be relied upon. When different transfer tasks require different padding values, the value should be explicitly set before each transfer.

If you do not need to specify a constant fill value, set `enable_constant_pad` to `false`:

```cpp
asc::te::gm_to_ub_params params{0, 0, false};
auto atom = asc::te::make_copy(asc::te::copy_gm_to_ub{}).with(params);
asc::te::copy(atom, dst, src);
```

At this time, the padding region generated by non-32-byte alignment is automatically padded by hardware with dummy data, and the value of the dummy data is the value of the first element of the data block. The specific padding range depends on the transfer mode. For details, see [Copy Mode and Padding](#copy-mode-and-padding). If `left_padding_count` or `right_padding_count` is not `0`, `enable_constant_pad` will not affect the padding value of the left and right padding, and `set_copy_pad_val` still needs to be called before transfer for configuration.

## Data Type

The data types of the source tensor and the destination tensor must be the same.

| Source/Destination Layout | Supported Data Types |
| :--- | :--- |
| ND, DN, NZ, ZN | `int4b_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t` |

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The source tensor must reside in Global Memory, and the destination tensor must reside in UB.
- The source address supports 1-byte alignment, and the destination UB address must meet the 32-byte alignment requirement.
- The source and destination layout patterns must be the same. ND2ND, DN2DN, NZ2NZ, and ZN2ZN are supported, while on-the-fly format conversion is not supported.
- The capacity of the destination tensor or destination region must be no less than the space required by the data to be copied and the padding.
- The number of blocks, block length, and stride derived from the layout must satisfy the field range and alignment requirements of the underlying `asc_copy_gm2ub_align` instruction. The number of valid movement blocks ranges from [1, 4095], and the single-block length ranges from [1, 2097151] bytes. Unlike the `DataCopyParams` API on some products, the Tensor API requires that the source and destination layouts and the regional movement shape be valid, and it cannot rely on a block count or single-block length of 0 to represent a no-op.
- For multiple valid data blocks, the destination stride must be equal to the single-block length or be a positive integer multiple of 32 bytes. Both the source stride and the destination stride represent the gap between the start addresses of adjacent data blocks, rather than the inter-block gap in `DataCopyParams`.
- The left and right single-side Padding must not exceed 32 bytes. For the 8-byte element type, the Padding count is distributed at a 4-byte granularity.
- When the destination stride is equal to the single-block movement length, that is, in compact movement mode, both the left and right Padding must be 0.
- For the b4 type, the block length and stride are adjusted at the b8 movement granularity.

## Key Features

### Layout Automatic Derivation

The ND path generates movement blocks along the row direction, and the DN path generates movement blocks along the column direction. The NZ and ZN paths derive the number of blocks, block length, and stride based on the inner and outer axes of the fractal layout. The corresponding strides of the source and destination layouts determine whether the data blocks are arranged contiguously or with gaps. Users only need to ensure that the layouts of the source and destination tensors match correctly.

### Contiguous and Non-Contiguous Movement

When the number of blocks is 1, the source and destination strides do not affect the movement result. When the number of blocks is greater than 1, if both the source and destination strides are equal to the single-block length, adjacent data blocks are contiguous end to end, which is a contiguous movement; if either stride is greater than the single-block length, there is a gap between data blocks in the corresponding storage space, which is a non-contiguous movement.

Take an ND Tensor with element type `half` and movement shape `(2, 32)` as an example. The single-block length is `32 × sizeof(half) = 64` bytes.

| Scenario | Source Tensor Row Stride | Destination Tensor Row Stride | Derivation Result |
| :--- | :--- | :--- | :--- |
| Contiguous Movement | 32 elements | 32 elements | The number of blocks is 2, the single-block length is 64 bytes, and both the source and destination strides are 64 bytes. |
| Non-contiguous movement | 48 elements | 64 elements | The number of blocks is 2, the single-block length is 64 bytes, the source stride is 96 bytes, and the destination stride is 128 bytes. Between each pair of data blocks, the source data skips 32 bytes, and the destination data retains a 64-byte gap. |

During the movement, in addition to writing valid data, `copy` also writes the left and right padding and the padding data required for 32-byte alignment according to the movement mode. Among the gaps generated by the destination tensor stride, only the parts not occupied by padding and alignment padding are not overwritten. The destination tensor's Layout stride and the actually allocated UB space should accommodate the corresponding padding regions.

### Copy Mode and Padding

If the length of the copied data is not 32-byte aligned, the underlying movement instruction performs 32-byte alignment padding.

For non-32-byte-aligned scenarios, the following two padding methods are supported. They are configured through `gm_to_ub_params` and bound to the movement atomic object using `copy_atom::with`:

- Manual padding: Call `set_copy_pad_val` before transfer to set the padding value, and set `enable_constant_pad` to `true`.
- Automatic padding: Set `enable_constant_pad` to `false`, and the hardware automatically fills dummy data, whose value is the value of the first element of the data block.

The movement modes are described as follows:

- Compact mode: The destination layout is compactly arranged, meaning adjacent data blocks are placed end to end with no inter-block gaps introduced by stride, and both `left_padding_count` and `right_padding_count` must be `0`. Multiple data blocks are compactly arranged in the UB and treated as a whole for 32-byte alignment padding. If the total length of all data blocks is not 32-byte aligned, padding is applied only at the end of the whole using the manual or automatic padding method described above to reach 32-byte alignment.
- Normal mode: The destination layout is not compactly arranged, the stride is greater than the single-block length, inter-block gaps are reserved between adjacent data blocks, data is not compactly arranged in the UB, and the address interval between the start addresses of adjacent destination data blocks is a positive integer multiple of 32 bytes. Intervals can be reserved between data blocks without requiring compact arrangement.
    - When both `left_padding_count` and `right_padding_count` are `0`, each data block is padded separately to 32-byte alignment using the manual or automatic padding method described above.
    - When `left_padding_count` or `right_padding_count` is not `0`, `enable_constant_pad` does not take effect, and `set_copy_pad_val` must be called before transfer to configure the padding value. If the total byte count of the single-block length and the left and right padding is not 32-byte aligned, the hardware continues padding dummy data on the right side to 32-byte alignment, and the value of the dummy data is the value configured by `set_copy_pad_val`.

For example, if the element type is `half`, the source tensor has an ND fractal Layout and a Shape of `(3, 24)`, and the destination tensor has an ND fractal Layout and a Shape of `(3, 24)`, then a Stride of `(24, 1)` indicates a compact permutation, which corresponds to the Compact mode, while a Stride of `(32, 1)` also indicates a compact permutation but corresponds to the Normal mode. These two modes correspond to the two cases in the following table.

| Copy mode | Destination Tensor Row Stride | Derived Destination Stride | Starting Offset of the Data Block in UB | 32-Byte Alignment Padding Method |
| :--- | :--- | :--- | :--- | :--- |
| Compact | 24 elements | 48 bytes | 0, 48, and 96 bytes | Pads 16 bytes only at the end of the 144 bytes of valid data, requiring at least 160 bytes reserved in UB. |
| Normal | 32 elements | 64 bytes | 0, 64, and 128 bytes | Pads 16 bytes for each data block separately, requiring at least 192 bytes reserved in UB. |

The row strides above are all in elements, while the derived destination strides and the offsets in the table are in bytes. The UB space actually allocated for the destination tensor must also cover the padded range.

### Partial Regional Movement

Region reload resolves the start address based on `dst_coord`, `src_coord`, and `copy_shape`. This method does not require constructing a Tensor slice first, making it suitable for copying a rectangular partial region from a large Tensor.

## Example

The following example copies ND data from Global Memory to the UB:

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t m = 8;
constexpr uint32_t n = 64;

__aicore__ inline void copy_gm_to_ub_example(__gm__ half* gm_addr)
{
    __ubuf__ half ub_buf[m * n];

    auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn,
        asc::te::layout_trait_default<half>>(m, n);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::gm>(gm_addr), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(ub_buf), layout);

    asc::te::copy(dst, src);
}
```

The ND layout in this example contains 8 rows, each with 64 `half` elements. Therefore, the API derives 8 blocks and a single-block length of 128 bytes, with both the source stride and destination stride being 128 bytes. The data blocks are contiguous end to end, which is a contiguous movement.

The following example copies a partial region and explicitly constructs a movement atomic object:

```cpp
auto atom = asc::te::make_copy(asc::te::copy_gm_to_ub{}, asc::te::gm_to_ub_trait_default{});
asc::te::copy(atom, dst, src, asc::te::make_coord(1, 0), asc::te::make_coord(2, 0), asc::te::make_shape(2, 32));
```

This region contains 2 rows, with 32 `half` elements copied per row. Therefore, the API derives 2 blocks and a single-block length of 64 bytes. The row stride of both the source and destination Tensors is 128 bytes, so the source stride and destination stride are both 128 bytes, leaving a 64-byte gap between adjacent data blocks.
