# Type Judgment Tools

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T04:29:52.747Z pushedAt=2026-10-27T13:03:35.497Z nl=2 -->

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

Type judgment tools are used to determine at compile time whether the input is a Layout, Tensor, Coord, Shape, or quantization parameter type supported by the Tensor API.

## Prototype

```cpp
template <typename Layout>
constexpr bool is_layout_v = is_layout<Std::remove_cvref_t<Layout>>::value;

template <typename Tensor>
constexpr bool is_attr_tensor_v = is_attr_tensor<Std::remove_cvref_t<Tensor>>::value;

template <typename Coord>
constexpr bool is_valid_coord_v;

template <typename CopyShape>
constexpr bool is_valid_shape_v;

template <typename Quant>
constexpr bool is_valid_quant_v;
```

## Tool Description

**Table 1** Tool function description

| Tool | Input | Result |
| :--- | :--- | :--- |
| `is_layout_v` | Type to be determined | Returns true when the input, after removing const, volatile, and reference, is of the `layout` type; otherwise, returns false. |
| `is_attr_tensor_v` | Type to be determined | Returns true when the input, after removing const, volatile, and reference, is of the `global_tensor` or `local_tensor` type; otherwise, returns false. |
| `is_valid_coord_v` | Type to be determined | Returns true when the input is a Tuple type or `zero_coord_type`; otherwise, returns false. |
| `is_valid_shape_v` | Type to be determined | Returns true when the input, after removing const, volatile, and reference, is a Tuple type; otherwise, returns false. |
| `is_valid_quant_v` | Type to be determined | Returns true when the input is a Tensor type or a scalar type of `uint64_t`; otherwise, returns false. |

## Return Value

`is_layout_v`, `is_attr_tensor_v`, `is_valid_coord_v`, `is_valid_shape_v`, and `is_valid_quant_v` return a compile-time Boolean value.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto tensor_layout = make_layout(make_shape(_8{}, _16{}), make_stride(_16{}, _1{}));
static_assert(is_layout_v<decltype(tensor_layout)>);

auto tensor = make_tensor(make_mem_ptr<location::gm, float>(0), tensor_layout);
static_assert(is_attr_tensor_v<decltype(tensor)>);

auto coord = make_coord(0, 0);
auto shape = make_shape(8, 16);
static_assert(is_valid_coord_v<decltype(coord)>);
static_assert(is_valid_shape_v<decltype(shape)>);
static_assert(is_valid_quant_v<uint64_t>);
```
