# store

<!-- md-trans-meta sourceCommit=83bafdf59a5b8695f652c2e8bbc7b9f91b0d8584 translatedAt=2026-11-19T02:57:57.438Z pushedAt=2026-11-19T05:54:06.676Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_data_store.h"`, and the namespace is `asc::te::experimental`.

`store` is used to move the data in [reg_tensor](../reg_tensor/reg_tensor.md) out to a Unified Buffer (UB) tensor. The API supports the following two types of source registers:

- [Vector Data Register Move-out](#vector-data-register-move-out): Select normal move-out, first element move-out, interleaved move-out, or compressed move-out through `store_sideband_mode`.
- [mask register move-out](#mask-register-move-out): Use `reg_tensor<bool>` as the single-source operand to move out mask bits as-is through `direct`, or compress every other bit through `pack`. Both modes do not read `src.mask`.

The API supports the following two addressing modes:

- When `coord` is passed in, writing starts from the element corresponding to `tensor.engine() + tensor.layout()(coord)`.
- When `coord` is not passed in, writing starts from the start position of `tensor`.

The `intlv` mode uses the dual-source operand overload, while other modes use the single-source operand overload.

## Prototype

### Vector Data Register Move-out

<a id="store-single-source"></a>

- Single-source operand, with coordinates specified:

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::direct,
              typename Tensor,
              typename Coord,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor,
        const Coord& coord,
        const reg_tensor<RegDataType>& src)
    ```

- Single-source operand, moved out from the start position of the Tensor:

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::direct,
              typename Tensor,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor, const reg_tensor<RegDataType>& src)
    ```

- Dual-source operand, with coordinates specified:

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::intlv,
              typename Tensor,
              typename Coord,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor,
        const Coord& coord,
        const reg_tensor<RegDataType>& src0,
        const reg_tensor<RegDataType>& src1)
    ```

- Dual-source operands, moved out from the start position of the Tensor:

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::intlv,
              typename Tensor,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor,
        const reg_tensor<RegDataType>& src0,
        const reg_tensor<RegDataType>& src1)
    ```

### mask register move-out

Reuses the preceding [two single-source operand prototypes](#store-single-source), which respectively support specifying `coord` and move-out from the start position of the Tensor. Pass in `reg_tensor<bool>`, and `RegDataType` is deduced as `bool` from `src`. The dual-source operand prototype is not supported.

## Parameters

### Vector Data Register Move-out

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sideband_mode | Move-out mode, whose type is `store_sideband_mode`. The default value of the single-source operand overload is `store_sideband_mode::direct`; the default value of the dual-source operand overload is `store_sideband_mode::intlv`. For supported modes, refer to [Table 3](#table-3-store_sideband_mode-parameter-description). |
| Tensor | Destination Tensor type, deduced from the function argument. The Tensor must reside in UB. |
| Coord | Coordinate type of the destination Tensor, deduced from the function argument. |
| RegDataType | Source register element type, automatically deduced from function arguments. It is consistent with the element type of the destination tensor, but when the element type of the destination tensor is `int4b_t`, the element type of the source register is `int4x2_t`. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tensor | Output | Destination Tensor, which must reside in UB. |
| coord | Input | Coordinate of the store start element in the logical layout of `tensor`. |
| src | Input | Source register of the single-source operand overload. `direct`, `pack`, and `pack_quarter` use `src.mask` to filter valid elements, while `store_1st` ignores `src.mask`. |
| src0 | Input | First source register of the `intlv` mode. This mode ignores `src0.mask`. |
| src1 | Input | Second source register of the `intlv` mode. This mode ignores `src1.mask`. |

<a id="table-3-store_sideband_mode-parameter-description"></a>

**Table 3** Vector data register move-out mode

In the table, VL denotes the length of a vector data register, in bytes. Data of type `int4b_t` is stored in registers as `int4x2_t`. The `int4x2_t` data type packs two 4-bit integers into one 8-bit storage unit, and the `fp4x2_e2m1_t` and `fp4x2_e1m2_t` data types pack two independent 4-bit floating-point numbers into one 8-bit storage unit. Therefore, for these three types, the "element" described in the following table refers to the transfer unit corresponding to 8 bits, rather than a single 4-bit value.

| Value | Meaning | Store start address alignment constraint (Byte) |
| --- | --- | --- |
| store_sideband_mode::direct | Normal mode. Based on `src.mask`, the valid elements in the source register are written consecutively to the corresponding positions in UB. | 32 |
| store_sideband_mode::store_1st | Writes only the first element of `src` to UB, ignoring `src.mask`. | `sizeof(RegDataType)` |
| store_sideband_mode::intlv | Interleaved mode. The elements in `src0` and `src1` are written to UB alternately, producing 2\*VL data size, ignoring the `mask` of both source operands. Only the dual-source operand overload is supported. | 32 |
| store_sideband_mode::pack | 2x compression mode. Based on `src.mask`, valid elements are filtered, and the lower half of the bits of each valid element is written to the compressed position. | min(32, VL/2) |
| store_sideband_mode::pack_quarter | 4x compression mode. Based on `src.mask`, valid elements are filtered, and the lower 8 bits of each valid 32-bit element are written to the compressed position. | min(32, VL/4) |

### Mask Register Move-out

**Table 4** Template parameters

| Parameter | Description |
| --- | --- |
| sideband_mode | Move-out mode. The type is `store_sideband_mode`, and the default value is `store_sideband_mode::direct`. For supported modes, refer to [Table 6](#table-6-mask-register-move-out-mode). |
| Tensor | Destination tensor type, automatically deduced from function arguments. The tensor must be located in UB. For supported element types, refer to [Data Type](#mask-register-move-out-2). |
| Coord | Coordinate type of the destination tensor, automatically deduced from function arguments. |
| RegDataType | Source register element type, deduced as `bool` from the `reg_tensor<bool>` argument. |

**Table 5** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tensor | Output | UB tensor for storing mask bits. |
| coord | Input | Coordinate of the move-out start element in the logical layout of `tensor`. |
| src | Input | Type is `reg_tensor<bool>`. Only the mask bits in `src.reg` are moved out; `src.mask` is not read. |

<a id="table-6-mask-register-move-out-mode"></a>

**Table 6** Mask register move-out mode

Mask bits are continuously written to UB and are not converted to per-element Boolean values. When VL is 256 bytes, the mask register length is VL/8, that is, 32 bytes.

| Value | Meaning | UB write amount (Byte) | Move-out start address alignment constraint (Byte) |
| --- | --- | --- | --- |
| store_sideband_mode::direct | Writes all bits in `src.reg` as-is. | VL/8 (32) | 32 |
| store_sideband_mode::pack | Retains the bits at even indices (starting from 0) in `src.reg` and writes them sequentially. | VL/16 (16) | 16 |

## Data Type

### Vector Data Register Move-out

The supported destination tensor element types are as follows. Unlisted type or mode combinations cause a compile-time error. When the source register type is `reg_tensor<int4x2_t>`, the corresponding data type of the destination tensor is `int4b_t`.

| sideband_mode | Supported Data Types |
| --- | --- |
| direct, store_1st | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float` |
| intlv | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t` |
| pack | `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t` |
| pack_quarter | `int32_t`, `uint32_t`, `float` |

### Mask Register Move-out

The source register element type is `bool`, and the destination Tensor element type only supports `uint8_t`, `uint16_t`, and `uint32_t`. For supported modes, refer to [Table 6](#table-6-mask-register-move-out-mode).

## Return Value

Neither type of move-out returns a value.

## Constraints

- Only **Tensor** located in the UB is supported.
- The actual move-out start address must satisfy the alignment constraint in [Table 3](#table-3-store_sideband_mode-parameter-description) or [Table 6](#table-6-mask-register-move-out-mode).
- When `coord` is passed, the coordinate dimensionality must match the **Tensor** layout, and the coordinates must not be out of bounds.
- The single-source operand overload does not support `store_sideband_mode::intlv`; the dual-source operand overload supports only `store_sideband_mode::intlv`.
- When moving out a vector data register, the `direct`, `pack`, and `pack_quarter` modes require both `src.reg` and `src.mask` to be initialized; the `store_1st` and `intlv` modes require only the `reg` members involved in the move-out to be initialized.
- When a vector data register is moved out in `direct` mode, if the remaining space in the destination UB is less than one VL, the out-of-bounds positions must be disabled through `src.mask`, and at least one DataBlock (32B) of space must remain in the destination UB.
- When moving out a mask register, only `src.reg` needs to be initialized. The UB buffer of the destination Tensor must accommodate the complete write amount listed in [Table 6](#table-6-mask-register-move-out-mode), and the write range cannot be reduced through `src.mask`.
- Register data move-out should be performed in an `__simd_vf__` function or an `__simd_callee__` function.

## Example

### Normal Move-Out

The following example loads the source tensor into registers, binds a fully valid mask, and then writes to the destination tensor.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename T, typename SrcTensor, typename DstTensor>
__simd_vf__ inline void LoadAndStore(const SrcTensor& src, DstTensor& dst)
{
    auto coord = asc::te::make_coord(0);
    auto value = asc::te::experimental::load(src, coord);
    value.with_mask(asc::te::experimental::all_mask<T>());
    asc::te::experimental::store(dst, coord, value);
}
```

### Interleaved Store

```cpp
template <typename T, typename DstTensor>
__simd_vf__ inline void StoreInterleaved(
    DstTensor& dst,
    const asc::te::experimental::reg_tensor<T>& even,
    const asc::te::experimental::reg_tensor<T>& odd)
{
    asc::te::experimental::store<asc::te::experimental::store_sideband_mode::intlv>(
        dst, asc::te::make_coord(0), even, odd);
}
```

### Mask Register Move-out

The following example will move the same mask out to two UB tensors respectively. The element type of both tensors can be `uint8_t`, `uint16_t`, or `uint32_t`. `direct_dst` must be 32-byte aligned and contain at least 32 bytes, and `packed_dst` must be 16-byte aligned and contain at least 16 bytes.

```cpp
template <typename DirectTensor, typename PackedTensor>
__simd_vf__ inline void StoreMask(DirectTensor direct_dst, PackedTensor packed_dst)
{
    using namespace asc::te::experimental;
    auto mask = all_mask<uint16_t>();
    store(direct_dst, mask);
    store<store_sideband_mode::pack>(packed_dst, asc::te::make_coord(0), mask);
}
```
