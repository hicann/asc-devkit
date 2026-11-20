# iota

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:55:47.671Z pushedAt=2026-11-19T05:54:06.674Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_index_compute.h"`, and the namespace is `asc::te::experimental`.

Generates an increasing or decreasing sequence with a step of 1, using the scalar `start` as the base value, and stores it in the returned [reg_tensor](../reg_tensor/reg_tensor.md). Let `VL_T` be the number of result elements that a vector data register can hold. The sequence is as follows:

| order | Result sequence |
| --- | --- |
| iota_order::increase | `{start, start + 1, ..., start + VL_T - 1}` |
| iota_order::decrease | `{start + VL_T - 1, start + VL_T - 2, ..., start}` |

The decreasing mode reverses the entire sequence of the increasing mode, rather than subtracting 1 from `start` successively. For example, when `VL_T` of int16_t is 128 and `start` is 10, the increasing sequence is `{10, 11, ..., 137}`, and the decreasing sequence is `{137, 136, ..., 10}`.

This API takes effect only on the AIV. The increasing mode calls `asc_arange`, and the decreasing mode calls `asc_arange_descend`. For the underlying semantics, see [asc_arange](../../../c_api/reg_compute/reg_index/asc_arange.md).

## Prototype

```cpp
template <iota_order order = iota_order::increase, typename T>
__simd_callee__ inline reg_tensor<Std::remove_cvref_t<T>> iota(const T& start);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| order | Sequence direction. `iota_order::increase` indicates increasing, and `iota_order::decrease` indicates decreasing. The default value is `iota_order::increase`. |
| T | Type of the scalar start, which can be deduced from the function argument. After removing const, volatile, and reference qualifiers, it must be a type supported in [Data Type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| start | Input | Scalar base value for generating the sequence. The first element of the increasing mode and the last element of the decreasing mode are start, whose value must be within the representable range of the corresponding data type. |

## Data Type

After removing const, volatile, and reference qualifiers, T supports: int8_t, int16_t, half, int32_t, and float.

## Return Value

Returns `reg_tensor<Std::remove_cvref_t<T>>`, where `reg` saves the generated sequence and `mask` is `all_mask<Std::remove_cvref_t<T>>().reg`, that is, the full valid mask set according to the result element type.

## Constraints

- `order` supports only `iota_order::increase` and `iota_order::decrease`.
- The length of the generated sequence is determined by the result element type and the register bit width, not by the value of `start`.
- When an integer result exceeds the representable range of the corresponding data type, it wraps around without raising an exception.
- The returned object automatically carries a full valid mask. When only part of the sequence is needed, rebind the valid mask through [with_mask](../reg_tensor/reg_tensor.md#with_mask) before subsequent computation or write-back.

## Example

The following code snippet is called in VF. It generates int32_t increment and decrement sequences with 10 as the base value, and both results automatically carry the full valid mask of the int32_t type.

```cpp
__simd_callee__ inline void iota_example(
    asc::te::experimental::reg_tensor<int32_t>& ascending,
    asc::te::experimental::reg_tensor<int32_t>& descending)
{
    const int32_t start = 10;
    ascending = asc::te::experimental::iota(start);
    descending = asc::te::experimental::iota<asc::te::experimental::iota_order::decrease>(start);
}
```
