# unalign_load

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T02:54:46.670Z pushedAt=2026-11-19T05:54:06.672Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_data_load.h"`, and the namespace is `asc::te::experimental`.

`unalign_load` is used to move non-32-byte-aligned Unified Buffer (UB) data into [reg_tensor](../reg_tensor/reg_tensor.md). The object internally maintains a 32-byte non-aligned move-in register and provides the following two operations:

- `prepare`: Initializes the non-aligned move-in register based on the source Tensor start address. Let the source start byte address be `src_start_addr`; align it downward to a 32-byte boundary to obtain `aligned_src_start_addr`, and cache the data in `[aligned_src_start_addr, aligned_src_start_addr + 32)` into the non-aligned register. This operation does not generate a vector data register.
- `load`: Combined with the preceding cache, moves one VL (Vector Length, the byte length of a vector register) of data from the specified UB address, supporting moving from the Tensor start position or from specified coordinates.

## Prototype

```cpp
struct unalign_load {
    __simd_callee__ unalign_load() = default;

    template <typename Tensor>
    __simd_callee__ inline void prepare(const Tensor& src_base);

    template <typename Tensor>
    __simd_callee__ inline auto load(const Tensor& src_tile);

    template <typename Tensor, typename Coord>
    __simd_callee__ inline auto load(const Tensor& src_tile, const Coord& coord);
};
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tensor | Source Tensor type, automatically deduced from the function argument, which must reside in UB. |
| Coord | Source Tensor coordinate type, automatically deduced from the function argument, used only by the `load` overload that specifies coordinates. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src_base | Input | Source tensor used by `prepare`, whose start address must be the same as the actual move-in address of the first `load` in this continuous move-in sequence. |
| src_tile | Input | Source tensor used by `load`. When `coord` is not passed, data is moved in from `src_tile.engine().begin().get()`. |
| coord | Input | Coordinates of the move-in start element in the logical layout of `src_tile`. The actual move-in address is `(src_tile.data() + src_tile.layout()(coord)).get()`. |

## Data Type

| Source Tensor element type | Returned register type |
| --- | --- |
| `int4b_t` | `reg_tensor<int4x2_t>`, which stores 4-bit data in packed form. |
| `fp4x2_e2m1_t`, `fp4x2_e1m2_t` | `reg_tensor` of the corresponding element type. |
| `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t` | `reg_tensor` of the corresponding element type. |
| `int16_t`, `uint16_t`, `half`, `bfloat16_t` | `reg_tensor` of the corresponding element type. |
| `int32_t`, `uint32_t`, `float` | `reg_tensor` of the corresponding element type. |
| `int64_t` | `reg_tensor<int64_t>` |

## Return Value

- `prepare`: None.
- `load`: Returns a `reg_tensor` containing one VL of data, initializing only its `reg` member and not its `mask` member. Before subsequent computations that depend on the mask or [store](../reg_data_store/store.md), a valid mask must be set through methods such as `with_mask`.

## Constraints

- The start address does not need to be 32-byte aligned, but it must satisfy the alignment requirement of the element type.
- The same continuous move-in sequence must reuse the same `unalign_load` object and use the same element type. Call `prepare` once before the sequence starts, and the start address of its source Tensor must be the same as the actual move-in address of the first `load`; each subsequent move-in address increments by one VL. When the addresses are not continuous, `prepare` must be called again based on the new start address.
- When `coord` is passed, the coordinate dimensionality must match the Tensor layout, and the coordinates must not be out of bounds.
- `load` always moves in one VL of data, does not truncate based on the Tensor shape, and does not support shortening the actual read range through a mask. When the tail is less than one VL, valid UB space must still be reserved for the complete read and the prefetch cache access; subsequent computation can use a mask to mask out invalid elements.
- Both the 32-byte cache access range aligned to a lower address by `prepare` and the actual read range of `load` must be within the valid UB address space. Access safety cannot be determined solely based on the logical Tensor size.

## Example

### Move in from the Start Position of the Tensor

The following example assumes that `src` and `dst` are located in the UB, the start addresses are aligned by `uint8_t`, the source side has reserved legal space for a complete VL read and prefetch cache, and the destination side has at least one VL of writable space.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/vector_ops.h"

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void CopyOneVector(SrcTensor src, DstTensor dst)
{
    asc::te::experimental::unalign_load loader;
    asc::te::experimental::unalign_store storer;
    loader.prepare(src);
    auto value = loader.load(src);
    constexpr uint32_t count = asc::te::get_vf_len() / sizeof(uint8_t);
    storer.store(dst, value, count);
    storer.flush(dst, static_cast<int32_t>(count));
}
```

### Move in at a Specified Coordinate

`prepare` has no coordinate parameter, so first copy the Tensor view and set the engine start address of the copy to the address corresponding to this coordinate. This operation does not modify the original Tensor, nor does it copy UB data. The source Tensor element type is `uint8_t`, and the actual UB space must be sufficient to cover the complete VL read starting from `offset`.

```cpp
template <typename SrcTensor>
__simd_vf__ inline void LoadAtCoordinate(SrcTensor src, uint32_t offset)
{
    auto coord = asc::te::make_coord(offset);
    auto src_tile = src;
    src_tile.engine().begin() = src.data() + src.layout()(coord);
    asc::te::experimental::unalign_load loader;
    loader.prepare(src_tile);
    auto value = loader.load(src, coord);
    (void)value;
}
```
