# load_broadcast

<!-- md-trans-meta sourceCommit=83bafdf59a5b8695f652c2e8bbc7b9f91b0d8584 translatedAt=2026-11-19T02:53:26.701Z pushedAt=2026-11-19T05:54:06.669Z nl=2 -->

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

`load_broadcast` moves data from a Unified Buffer (UB) Tensor and broadcasts it to a [reg_tensor](../reg_tensor/reg_tensor.md) in the manner specified by `broadcast_mode`.

When `coord` is passed in, move-in starts from the element corresponding to `tensor.engine() + tensor.layout()(coord)`; when `coord` is not passed in, move-in starts from the beginning of `tensor`.

## Prototype

- Specify the coordinate:

    ```cpp
    template <broadcast_mode brc_mode = broadcast_mode::elem,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline auto load_broadcast(
        const Tensor& tensor, const Coord& coord)
    ```

- Move in from the start position of the Tensor:

    ```cpp
    template <broadcast_mode brc_mode = broadcast_mode::elem,
              typename Tensor>
    __simd_callee__ inline auto load_broadcast(const Tensor& tensor)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| brc_mode | Broadcast mode, of type `broadcast_mode`, with a default value of `broadcast_mode::elem`. For details about the values, see [Table 3](#table3-broadcast_mode-parameter-description). |
| Tensor | Source Tensor type, automatically deduced from the function argument. The Tensor must be located in the UB. |
| Coord | Coordinate type of the source Tensor, automatically deduced from the function argument. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tensor | Input | Source Tensor, which must be located in UB. |
| coord | Input | Coordinates of the move-in start element in the logical layout of `tensor`. |

<a id="table3-broadcast_mode-parameter-description"></a>

**Table 3** `broadcast_mode` Parameter Description

In the table, VL indicates the length of one vector data register, in bytes. Data of the `int4b_t` type is stored in registers in the `int4x2_t` form. The `int4x2_t` data type packs two 4-bit integers into one 8-bit storage unit, and the `fp4x2_e2m1_t` and `fp4x2_e1m2_t` data types pack two independent 4-bit floating-point numbers into one 8-bit storage unit. Therefore, for these three types, the "element" described in the following table refers to the transfer unit corresponding to one 8-bit value, rather than a single 4-bit value.

| Value | Meaning | Move-in Start Address Alignment Constraint (Byte) |
| --- | --- | --- |
| broadcast_mode::elem | Reads one element from UB and broadcasts it to all element positions of the destination register. | Size of the element |
| broadcast_mode::datablock | Reads one DataBlock (32B) from UB and repeatedly broadcasts it to the entire destination register. | 32 |
| broadcast_mode::elem2datablock | Reads VL/32B elements in sequence and broadcasts each element to one DataBlock of the destination register. | VL/16 for 16-bit types; VL/8 for 32-bit types |

## Data Type

| brc_mode | Supported Data Types |
| --- | --- |
| elem, datablock | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float` |
| elem2datablock | `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float` |

## Return Value

The API returns a `reg_tensor` corresponding to the source Tensor element type by value. When the source Tensor element type is `int4b_t`, it returns `reg_tensor<int4x2_t>`. Its `reg` member stores the broadcast result, and its `mask` member is uninitialized.

## Constraints

- Only **Tensor** located in the UB is supported.
- The move-in start address must satisfy the alignment constraints in [Table 3](#table3-broadcast_mode-parameter-description).
- When `coord` is passed, the coordinate dimensionality must match the **Tensor** layout, and the coordinates must not be out of bounds.
- Before using the broadcast result for register calculation that reads `mask`, or for the `direct`, `pack`, and `pack_quarter` modes of [store](../reg_data_store/store.md), you must call `with_mask` to bind a valid mask. The `store_1st` and `intlv` modes do not read the `mask` of the source register.
- Register data move-in must be performed in a `__simd_vf__` function or a `__simd_callee__` function.

## Example

The following example reads a `float` element from UB and broadcasts it to the entire register.

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"

template <typename SrcTensor>
__simd_vf__ inline void LoadScalarBroadcast(const SrcTensor& src)
{
    auto value = asc::te::experimental::load_broadcast<asc::te::experimental::broadcast_mode::elem>(
        src, asc::te::make_coord(0));
    value.with_mask(asc::te::experimental::all_mask<float>());
    // Use value to complete subsequent register computation.
}
```
