# Compile-time Integer Aliases

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T04:25:40.054Z pushedAt=2026-10-27T13:03:35.492Z nl=2 -->

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

Tensor API provides a set of type aliases for [Std::Int](../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md), used to express compile-time integers in Layout, Shape, Stride, and Coord.

## Prototype

```cpp
using _0 = Std::Int<0>;
using _1 = Std::Int<1>;
using _2 = Std::Int<2>;
using _3 = Std::Int<3>;
using _4 = Std::Int<4>;
using _5 = Std::Int<5>;
using _6 = Std::Int<6>;
using _7 = Std::Int<7>;
using _8 = Std::Int<8>;
using _9 = Std::Int<9>;
using _10 = Std::Int<10>;
using _16 = Std::Int<16>;
using _24 = Std::Int<24>;
using _32 = Std::Int<32>;
using _64 = Std::Int<64>;
using _128 = Std::Int<128>;
using _256 = Std::Int<256>;
using _512 = Std::Int<512>;
using _1024 = Std::Int<1024>;
using _2048 = Std::Int<2048>;
using _4096 = Std::Int<4096>;
```

## Types

**Table 1**  Types

| Type | Description |
| :--- | :--- |
| The `_N` types listed in the prototype | Type aliases of `Std::Int` for the specified integer values. The available types and their corresponding integer values are subject to the list in [prototype definition](#prototype). |

Compile-time integer aliases are data types. When using them, construct an object in the form of `_N{}`.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Construct the shape and stride using compile-time integers.
auto tensor_layout = make_layout(make_shape(_8{}, _16{}), make_stride(_16{}, _1{}));
```
