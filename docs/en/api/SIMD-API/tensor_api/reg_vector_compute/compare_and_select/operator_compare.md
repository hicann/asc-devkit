# operator_compare

<!-- md-trans-meta sourceCommit=11d0c21f2d5b775da836aa7915c2f0fd3c24dafd translatedAt=2026-11-19T02:45:41.405Z pushedAt=2026-11-19T05:54:06.656Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/compare_and_select.h"`.

The comparison operators perform element-wise comparison on the source operands. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. Three operand combinations are supported: reg_tensor with reg_tensor, reg_tensor with scalar, and scalar with reg_tensor. The computation rules are as follows:

| Operator | Computation Rule |
| --- | --- |
| `operator==` | `dst_i = (src0_i == src1_i)` |
| `operator!=` | `dst_i = (src0_i != src1_i)` |
| `operator<` | `dst_i = (src0_i < src1_i)` |
| `operator<=` | `dst_i = (src0_i <= src1_i)` |
| `operator>` | `dst_i = (src0_i > src1_i)` |
| `operator>=` | `dst_i = (src0_i >= src1_i)` |

## Prototype

<a id="operator-eq"></a>

### operator==

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-ne"></a>

### operator!=

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-lt"></a>

### operator<

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-le"></a>

### operator<=

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-gt"></a>

### operator>

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(
    const T& src0, const reg_tensor<T>& src1)
```

<a id="operator-ge"></a>

### operator>=

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(
    const reg_tensor<T>& src0, const T& src1)

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(
    const T& src0, const reg_tensor<T>& src1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Source operand, of type `reg_tensor<T>` or scalar T. |
| src1 | Input | Source operand, of type `reg_tensor<T>` or scalar T. |

## Data Type

The data types of the two source operands must be the same. The supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.

## Return Value

The return type is `reg_tensor<bool>`. The reg of the return value stores the element-wise comparison result: a value of 1 at the corresponding position indicates that the comparison result holds, and a value of 0 indicates that it does not hold. The mask of the return value is not set, and its content is indeterminate. Do not use it directly.

## Constraints

- The mask of the `reg_tensor` operand used to control the computation must be set in advance through the `with_mask` API: when `src0` is a `reg_tensor`, use `src0.mask`; when `src0` is a scalar and `src1` is a `reg_tensor`, use `src1.mask`. If the mask is not set, its content is undefined, which causes incorrect element positions in the computation. A scalar operand does not require a mask.
- This API must be called within a Vector Function (a function marked with `__simd_vf__`).
- When a bit of the input mask is 0, the corresponding bit of the computation result is written as 0.
- In floating-point comparison, +0.0 and -0.0 are considered equal.
- For floating-point comparisons, when the input contains nan, the corresponding bits of the result of `operator==`, `operator<`, `operator<=`, `operator>`, and `operator>=` are written as 0, and the corresponding bits of the result of `operator!=` are written as 1.

## Example

```cpp
#include "tensor_api/experimental/vector_ops.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void compare_example(InputTensor input, OutputTensor output)
{
    using asc::te::experimental::operator>=;
    auto src = asc::te::experimental::load(input, asc::te::make_coord(0))
                   .with_mask(asc::te::experimental::all_mask<float>());
    auto condition = src >= 0.0f;
    auto zero = asc::te::experimental::fill(0.0f, asc::te::experimental::all_mask<float>());
    auto dst = asc::te::experimental::select(condition, src, zero);
    asc::te::experimental::store(output, asc::te::make_coord(0), dst);
}
```
