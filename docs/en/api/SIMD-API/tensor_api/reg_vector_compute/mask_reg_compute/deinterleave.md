# deinterleave (Mask Deinterleaving)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T03:52:30.071Z pushedAt=2026-10-27T13:03:35.434Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/mask_reg_compute.h"`, and the namespace is `asc::te::experimental`.

Deinterleaves the reg of two source `reg_tensor<bool>` registers by the element width corresponding to `DataType`. The even-index groups in `src0` and `src1` are written sequentially to `first.reg` of the return value, and the odd-index groups are written sequentially to `second.reg`.

For the b8, b16, and b32 data types, the API deinterleaves in groups of 1 bit, 2 bits, and 4 bits, respectively. `src0` produces the first half of the return result, and `src1` produces the second half.

This API processes only `reg_tensor<bool>` registers. For the data register deinterleaving API, see [deinterleave (Data Register Deinterleaving)](../data_reorder/deinterleave.md).

## Function Prototype

```cpp
template <typename DataType>
__simd_callee__ inline reg_pair<bool> deinterleave(
    reg_tensor<bool> src0, reg_tensor<bool> src1)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `DataType` | Input | Template parameter that determines the element grouping width of the Mask. It supports the b8, b16, and b32 data types. |
| `src0` | Input | First source Mask register, of type `reg_tensor<bool>`. Its `reg` member must be initialized. Its even-index groups are written to the first half of `first.reg`, and its odd-index groups are written to the first half of `second.reg`. |
| `src1` | Input | Second source Mask register, of type `reg_tensor<bool>`. Its `reg` member must be initialized. Its even-index groups are written to the second half of `first.reg`, and its odd-index groups are written to the second half of `second.reg`. |

## Return Value

Returns `reg_pair<bool>`:

- `first.reg` stores the even-index groups after deinterleaving.
- `second.reg` stores the odd-index groups after deinterleaving.

The returned object guarantees only that `first.reg` and `second.reg` are valid; it does not initialize the `mask` member of either.

## Constraints

- `src0` and `src1` can be the same `reg_tensor<bool>` object.
- This API must be called within a Vector Function (a function marked with `__simd_vf__`).
- The maximum number of Mask registers is 8. Register data exceeding this limit is written to the reserved 8K Unified Buffer (UB) memory, which may cause performance degradation.

## Example

```cpp
auto mask0 = asc::te::experimental::make_mask<
    asc::te::experimental::mask_pattern::every3, float>();
auto mask1 = asc::te::experimental::make_mask<
    asc::te::experimental::mask_pattern::every4, float>();

auto restored = asc::te::experimental::deinterleave<float>(
    mask0, mask1);
asc::te::experimental::reg_tensor<bool> restoredMask0 = restored.first;
asc::te::experimental::reg_tensor<bool> restoredMask1 = restored.second;
```
