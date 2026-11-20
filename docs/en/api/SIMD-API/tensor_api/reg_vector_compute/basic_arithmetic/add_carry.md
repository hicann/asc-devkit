# add_carry

<!-- md-trans-meta sourceCommit=7e60dec365f6254fedd6bbb74d7032e62730ccec translatedAt=2026-10-27T03:18:47.705Z pushedAt=2026-10-27T13:03:35.367Z nl=2 -->

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

Header file path: `"tensor_api/experimental/arch/vector/basic_arithmetic.h"`.

This API performs element-wise addition on the input elements according to **src0.mask**, and returns the low-order result and the carry. The two overloads are without an input carry and with an input carry, respectively. The calculation formulas are as follows:

$$
\{second_i, first_i\} = \{0, src0_i\} + \{0, src1_i\}
$$

$$
\{second_i, first_i\} = \{0, src0_i\} + \{0, src1_i\} + carry\_src_i
$$

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& carry_src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Source operand, whose type is `reg_tensor<T>`. `src0.reg` stores the vector data, and `src0.mask` controls whether each element participates in the computation. When the bit corresponding to an element in `src0.mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |
| src1 | Input | Source operand, of type reg_tensor&lt;T&gt;. src1.mask does not participate in this computation. |
| carry_src | Input | Carry input, whose type is `reg_tensor<bool>`. When the corresponding element is 1, 1 is added. It is used only by the second overload. `carry_src.mask` does not participate in this computation. |

## Return Value

The return type is `reg_pair<T, bool>`: `first` stores the low-order 32 bits of the addition result, and `second` stores the carry. The `mask` of both members is set to `src0.mask`.

## Data Type

**Table 3** Data type combinations

| src0 | src1 | carry_src | first | second |
| --- | --- | --- | --- | --- |
| int32_t | int32_t | bool | int32_t | bool |
| uint32_t | uint32_t | bool | uint32_t | bool |

## Constraints

- src0.mask must be set in advance through the `with_mask` API. If it is not set, the content of mask is undefined, which may cause incorrect element positions to participate in the computation.
- **carry_src.reg** must be initialized.

## Example

### Without Input Carry

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void add_carry_vf(
    const InputTensor input0, const InputTensor input1, OutputTensor output, uint16_t repeat_times,
    uint32_t total, uint32_t one_repeat_size)
{
    using data_type = typename InputTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src0_reg = asc::te::experimental::load(input0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(input1, coord);
        auto result = asc::te::experimental::add_carry(src0_reg, src1_reg);
        asc::te::experimental::store(output, coord, result.first);
    }
}
```

### With Input Carry

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void add_carry_with_carry_vf(
    const InputTensor input0, const InputTensor input1, OutputTensor output, uint16_t repeat_times,
    uint32_t total, uint32_t one_repeat_size)
{
    using data_type = typename InputTensor::data_type;
    uint32_t remain = total;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        auto src0_reg = asc::te::experimental::load(input0, coord).with_mask(mask);
        auto src1_reg = asc::te::experimental::load(input1, coord);
        auto carry_src = asc::te::experimental::none_mask<data_type>();
        auto result = asc::te::experimental::add_carry(src0_reg, src1_reg, carry_src);
        asc::te::experimental::store(output, coord, result.first);
    }
}
```
