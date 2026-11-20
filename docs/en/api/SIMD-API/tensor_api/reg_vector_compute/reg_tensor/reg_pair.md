# reg_pair

<!-- md-trans-meta sourceCommit=63c0b0eaa8a2923ff3c351fb2a0592ad269f94d6 translatedAt=2026-11-19T02:55:56.744Z pushedAt=2026-11-19T05:54:06.674Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_tensor.h"`, and the namespace is `asc::te::experimental`.

Stores two `reg_tensor` objects whose element types can be the same or different, and is used for dual-result APIs such as `deinterleave`. The element type of `first` is specified by `DataType`, and the element type of `second` is specified by `CarryType`. `CarryType` defaults to the same type as `DataType`.

## Prototype

```cpp
template <typename DataType, typename CarryType = DataType>
struct reg_pair {
    reg_tensor<DataType> first;
    reg_tensor<CarryType> second;
};
```

## Parameters

| Parameter | Description |
| --- | --- |
| `DataType` | Element type of the first register tensor. |
| `CarryType` | Element type of the second register tensor, defaulting to `DataType`. |

## Member Description

| Member | Description |
| --- | --- |
| `first` | The first register tensor, of type `reg_tensor<DataType>`. |
| `second` | The second register tensor, of type `reg_tensor<CarryType>`. |

## Example

Receives the two register tensors returned by the `deinterleave` API:

```cpp
auto src0 = asc::te::experimental::fill(1.0f);
auto src1 = asc::te::experimental::fill(2.0f);
auto result = asc::te::experimental::deinterleave(src0, src1);
asc::te::experimental::reg_tensor<float> first = result.first;
asc::te::experimental::reg_tensor<float> second = result.second;
```

Receives the register tensors of two element types returned by the [add_carry](../basic_arithmetic/add_carry.md) API. `first` stores the lower 32 bits of the `uint32_t` addition result, and `second` stores the `bool` carry.

```cpp
auto src0 = asc::te::experimental::fill(uint32_t{0xffffffff});
auto src1 = asc::te::experimental::fill(uint32_t{1});
asc::te::experimental::reg_pair<uint32_t, bool> result =
    asc::te::experimental::add_carry(src0, src1);
asc::te::experimental::reg_tensor<uint32_t> sum = result.first;
asc::te::experimental::reg_tensor<bool> carry = result.second;
```
