# copy (Global Memory to L1 Buffer Movement)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:40:07.695Z pushedAt=2026-10-27T13:03:35.323Z nl=2 -->

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

The header file path is `tensor_api/tensor.h`.

The Tensor API uses the `copy` API to uniformly perform movement across different paths. This API moves data from Global Memory to L1 Buffer. The `copy` API selects the specific movement implementation based on the storage location, data type, and layout of the source tensor and destination tensor. The number of movement blocks, movement length, source/destination-side strides, and format conversion information are derived from the Tensor layout, so users do not need to pass additional movement parameters in the `copy` call.

The API supports moving a complete tensor, and also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`.

This path is used for cube computation input move-in and supports the following scenarios:

- Contiguous data move-in: The data format remains unchanged after the source data is moved from Global Memory to L1 Buffer.
- High-dimensional sliced data move-in: When strides exist on the source/destination side, multiple movement segments are derived based on the layout.
- Inline format conversion move-in: Cube format conversion from ND/DN to NZ/ZN and the like is completed while moving data from Global Memory to L1 Buffer.
- Batch multi-cube move-in: When the source/destination layout has a Batch axis, a single `copy` completes the movement of multiple cubes, supporting `ND2ND`, `ND2NZ`, `DN2NZ`, `ND2ZN`, `DN2ZN`, `ScaleAND2ZZ`, `ScaleADN2ZZ`, `ZZ2ZZ`, `ScaleBND2NN`, `ScaleBDN2NN`, and `NN2NN` inline format conversion.
- Scale data move-in: Used to move ScaleA/ScaleB data from Global Memory to L1 Buffer in MX cube computation.
- Convolution feature map move-in: Supports contiguous or sliced move-in from `NC1HWC0` to `NC1HWC0`, as well as inline format conversion from `NHWC` and `NCHW` to `NC1HWC0`.

## Function Prototype

- Performs a Global Memory to L1 Buffer movement.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and destination tensor, and performs a Global Memory to L1 Buffer movement using the [default trait](../../algorithm.md#copy_traits).

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs a Global Memory to L1 Buffer movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and destination tensor, and performs a Global Memory to L1 Buffer movement using the default trait according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Constructs a movement atomic object using the default trait.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- Constructs a movement atomic object using the specified trait.

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation,
        const CopyTrait& trait)
    ```

The Global Memory to L1 Buffer path uses `copy_gm_to_l1{}` as the `operation` and `gm_to_l1_trait_default{}` as the default trait object.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type. For a Global Memory to L1 Buffer movement, it must be a Tensor API Tensor type with the storage location `location::l1`. |
| SrcTensor | Source tensor type. For a Global Memory to L1 Buffer movement, it must be a Tensor API Tensor type with the storage location `location::gm`. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |
| CopyOperation | Movement operation object type. For a Global Memory to L1 Buffer movement, use `copy_gm_to_l1`. |
| CopyTrait | Movement trait object type. For a Global Memory to L1 Buffer movement, use `gm_to_l1_trait_default` by default. |

**Table 2**  `copy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, with the storage location `location::l1`. |
| src | Input | Source tensor, with the storage location `location::gm`. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor; can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor; can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 3**  `make_copy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. For Global Memory to L1 Buffer movement, use `copy_gm_to_l1{}`. |
| trait | Input | Movement trait object. For Global Memory to L1 Buffer, the default is `gm_to_l1_trait_default{}`. |

### `copy_gm_to_l1` Description

`copy_gm_to_l1` identifies the movement path from Global Memory to L1 Buffer. Only its public members are listed:

```cpp
struct copy_gm_to_l1 {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and movement parameters, and dispatches the movement from Global Memory to L1 Buffer.

### gm_to_l1_trait Description

`gm_to_l1_trait` represents the static configuration of this path. It currently has no configurable members and is defined as follows:

```cpp
struct gm_to_l1_trait {};
```

`gm_to_l1_trait_default` provides the default Trait type and default value for this path:

```cpp
struct gm_to_l1_trait_default {
    using trait_type = gm_to_l1_trait;
    static constexpr const trait_type value = {};
};
```


## Data Type

The data types of the source tensor and the destination tensor must be consistent.

| Source Operand Data Format | Destination Operand Data Format | Source Operand/Destination Operand Data Type |
| -- | -- | -- |
| ND | ND | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ND | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| ND | ZN | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | NZ | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NZ | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ZN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ScaleAND | ZZ | fp8_e8m0_t |
| ScaleADN | ZZ | fp8_e8m0_t |
| ZZ | ZZ | fp8_e8m0_t |
| ScaleBND | NN | fp8_e8m0_t |
| ScaleBDN | NN | fp8_e8m0_t |
| NN | NN | fp8_e8m0_t |
| NC1HWC0 | NC1HWC0 | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NHWC | NC1HWC0 | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NCHW | NC1HWC0 | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |

## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The source address in Global Memory must be 1B-aligned, and the destination address in L1 Buffer must be 32B-aligned.
- In the contiguous movement scenario, the number of bytes to move must be 32B-aligned. If it is not aligned, the movement amount is rounded down to 32B alignment.
- In the high-dimensional split movement scenario, the movement length, source stride, and destination stride are all aligned to 32B.
- When the input data is of the b4 type, it is processed at the b8 type movement granularity, and the layout derivation parameters must satisfy the corresponding granularity constraints.
- Batch multi-cube move-in supports only the following format conversions: `ND2ND`, `ND2NZ`, `DN2NZ`, `ND2ZN`, `DN2ZN`, `ScaleAND2ZZ`, `ScaleADN2ZZ`, `ZZ2ZZ`, `ScaleBND2NN`, `ScaleBDN2NN`, and `NN2NN`. The Batch count is limited by the field range of the movement instruction and must not exceed 4095.
- The convolution feature map move-in from `NHWC` or `NCHW` to `NC1HWC0` currently supports only N = 1. The H, W, and C of the source tensor and destination tensor must be consistent, and `C = C1 * C0` must be satisfied.
- When moving in from `NC1HWC0` to `NC1HWC0`, the N, C1, H, W, and C0 of the source tensor and the destination tensor must be consistent.

## Key Features

### Inline Format Conversion Move-in

Inline format conversion move-in means that cube format conversion is completed synchronously when data is moved from Global Memory into L1 Buffer. The API automatically selects the conversion direction based on the source layout and destination layout, and supports the following format conversions: `ND2ND`, `ND2NZ`, `ND2ZN`, `DN2NZ`, `DN2ZN`, `NZ2NZ`, `ZN2ZN`. The ND-to-NZ format conversion is equivalent to the DN-to-ZN format conversion, and the DN-to-NZ format conversion is equivalent to the ND-to-ZN format conversion.

#### ND2NZ Contiguous Movement

ND2NZ contiguous movement means that the source cube is contiguously laid out in the ND format and the destination cube is contiguously laid out in the NZ format. In this scenario, the single-segment movement parameters are automatically derived from the layout, the ND-to-NZ format conversion is completed during the movement process, and the destination side is padded with zeros according to 32B alignment.

#### ND2NZ Non-Contiguous Movement

ND2NZ non-contiguous movement indicates that high-dimensional slicing information such as stride and slicing dimensions exists on the source side or the destination side. This scenario falls under high-dimensional sliced data move-in. `copy` automatically derives multi-segment movement parameters based on the layout, completes the ND-to-NZ format conversion during the movement process, and pads zeros on the destination side according to 32B alignment.

#### DN2NZ Contiguous Movement

DN2NZ contiguous movement means that the source cube is laid out contiguously in DN format and the destination cube is laid out contiguously in NZ format. In this scenario, layout automatically derives single-segment movement parameters, the layout conversion from DN to NZ is completed during the movement process, and the destination side is zero-padded to 32B alignment.

#### DN2NZ Non-Contiguous Movement

DN2NZ non-contiguous movement indicates that high-dimensional slicing information such as stride and slicing dimension exists on the source side or the destination side. This scenario falls under high-dimensional sliced data move-in. `copy` automatically derives multi-segment movement parameters based on the layout, completes the DN-to-NZ format conversion during the movement process, and the destination side is padded with zeros according to 32B alignment.

### Scale Data Move-in

Scale data move-in is used to move scale-related data from Global Memory to L1 Buffer during cube computation. The scale related to the cube A supports format conversion from `ScaleAND` to `ZZ`, from `ScaleADN` to `ZZ`, and from `ZZ` to `ZZ`. The scale related to the cube B supports format conversion from `ScaleBND` to `NN`, from `ScaleBDN` to `NN`, and from `NN` to `NN`.

### Convolution Feature Map Move-in

Convolution feature map move-in supports the following layout combinations:

- When both the source tensor and the destination tensor are `NC1HWC0`, the data format remains unchanged after move-in. After using `slice` to extract partial data along the W axis from the source tensor, `copy` performs non-contiguous movement according to the sliced layout.
- When the source tensor is `NHWC` and the destination tensor is `NC1HWC0`, the C axis is split into C1 and C0 during the move-in process.
- When the source tensor is `NCHW` and the destination tensor is `NC1HWC0`, both the channel dimension layout conversion and the C axis split are completed during the move-in process.

### Batch Multi-Cube Move-in

Batch multi-cube move-in means that a single `copy` completes the movement of multiple cubes. When the layouts of the source tensor and the destination tensor carry a Batch axis, a single `copy` instruction completes the movement of multiple cubes. The API is invoked in exactly the same way as for single-cube movement, with no additional Batch-related parameters required.

A layout with a Batch axis adds one Batch dimension outside the original cube layout. For the scenario where Batch memory is contiguous, the layout can be constructed through `make_frame_layout`; for the scenario where Batch memory is non-contiguous, the layout can be constructed through `make_layout` by explicitly specifying shape and stride.

For example, in the scenario where Batch memory is contiguous, the layout is constructed through `make_frame_layout<LayoutPattern>(b, m, k)`, where `b` is the number of batches, and `m` and `k` are the rows and columns of a single cube. The layout semantics are as follows:

- The shape is `(b, (m, k))`.
- The stride is `(m * k, (k, 1))`, meaning that the cubes are laid out contiguously in memory at intervals of `m * k`, and a single cube is laid out contiguously in memory in ND format.

In the scenario where Batch memory is non-contiguous, the layout can be constructed through `make_layout(shape, stride)`, where `shape` describes the number of batches and the shape of a single cube, and `stride` describes the address intervals of the Batch axis and the axes within the cube. Taking an ND cube laid out contiguously across batches in the K direction as an example, the layout semantics are as follows:

- The shape is `(b, (m, k))`.
- The stride is `(k, (b * k, 1))`, meaning that the cubes are non-contiguous in memory, and memory is laid out contiguously across batches in the K direction.

Copy behavior:

- Source (Global Memory) side: The source address spacing between Batch cubes is determined by the Batch axis stride of the layout. With the Batch axis stride parameter, Batch data supports both contiguous and non-contiguous layouts between batches.
- Destination (L1 Buffer) side: The Batch cubes are stored contiguously in the L1 Buffer, and the address spacing between batches equals the occupied space of a single cube after alignment.

## Example

The following example moves an ND cube from Global Memory into the L1 Buffer and converts it to the NZ format.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t m = 32;
constexpr uint32_t k = 64;

__aicore__ inline void copy_gm_to_l1_example(__gm__ half* gm_addr)
{
    __cbuf__ half l1_buf[m * k];

    // Source tensor: an ND-format cube in Global Memory; destination tensor: an NZ-format cube in L1 Buffer.
    auto gm_a = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(m, k));
    auto l1_a = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nz_layout_ptn, half>(m, k));

    // Construct the GM-to-L1 movement atomic object. The movement operation automatically completes the inline ND-to-NZ format conversion based on the source/destination layout.
    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_a, gm_a);
}
```

The following example moves b ND cubes from Global Memory into the L1 Buffer in one operation and converts them to the NZ format. Both the source tensor and the destination tensor use layouts with a Batch axis, and the API is called in the same way as for single-cube movement.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t b = 4;
constexpr uint32_t m = 32;
constexpr uint32_t k = 64;

__aicore__ inline void copy_batch_gm_to_l1(__gm__ half* gm_addr)
{
    __cbuf__ half l1_buf[b * m * k];

    // Construct the source/destination layout with a Batch axis (b cubes). make_frame_layout adds a Batch dimension outside the original layout.
    auto gm_a = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(b, m, k));
    auto l1_a = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nz_layout_ptn, half>(b, m, k));

    // The call is the same as for a single cube. A single movement instruction completes the ND-to-NZ movement of b cubes.
    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_a, gm_a);
}
```

The following example moves an NHWC convolution feature map from Global Memory into the L1 Buffer and converts it to the NC1HWC0 format. For NCHW input, simply replace the source layout with `make_frame_layout<nchw_layout_ptn>(n, c, h, w)`.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t n = 1;
constexpr uint32_t h = 4;
constexpr uint32_t w = 4;
constexpr uint32_t c0 = 16;
constexpr uint32_t c = 32;
constexpr uint32_t c1 = c / c0;

__aicore__ inline void copy_conv_input_gm_to_l1(__gm__ half* gm_addr)
{
    __cbuf__ half l1_buf[n * c * h * w];

    auto gm_feature = make_tensor(
        make_mem_ptr(gm_addr), make_frame_layout<nhwc_layout_ptn>(n, h, w, c));
    auto l1_feature = make_tensor(
        make_mem_ptr(l1_buf), make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0));

    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_feature, gm_feature);
}
```
