# deinterleave

<!-- md-trans-meta sourceCommit=83bafdf59a5b8695f652c2e8bbc7b9f91b0d8584 translatedAt=2026-11-19T02:47:43.530Z pushedAt=2026-11-19T05:54:06.660Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/data_reorder.h"`.

Given the source operand registers src0 and src1, deinterleave the elements in src0 and src1 and store them into the destination operands dst0 and dst1. The deinterleaving arrangement is shown in the following figure, where each cell represents an element:

![DeInterleave diagram](../../../../figures/reg_regdeinterleave.png)

## Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_pair<T> deinterleave(reg_tensor<T> src0, reg_tensor<T> src1);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operands and source operands. For supported data types, see [data type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0<br>src1 | Input | Source operands.<br>Type: [reg_tensor]. |

## Data Type

Supported data types: int8_t, uint8_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, hifloat8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

## Return Value

Returns `reg_pair<T>`, consisting of two `reg_tensor<T>` that store the two deinterleaved result values respectively. The `mask` of the return value is a full mask related to data type T.

## Constraints

- The data types of src0 and src1 must be consistent.
- src0 and src1 can be the same reg_tensor.

## Example

```cpp
template <typename Src0TensorType, typename Src1TensorType, typename Dst0TensorType, typename Dst1TensorType>
__simd_vf__ inline void data_reorder_vf(
    Src0TensorType src0_tensor, Src1TensorType src1_tensor, Dst0TensorType dst0_tensor, Dst1TensorType dst1_tensor,
    uint32_t repeat_times, uint32_t one_repeat_size)
{
    for (uint16_t i = 0; i < repeat_times; ++i) {
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src0_reg = asc::te::experimental::load(src0_tensor, coord);
        auto src1_reg = asc::te::experimental::load(src1_tensor, coord);
        auto result = asc::te::experimental::deinterleave(src0_reg, src1_reg);
        asc::te::experimental::store(dst0_tensor, coord, result.first);
        asc::te::experimental::store(dst1_tensor, coord, result.second);
    }
}
```
