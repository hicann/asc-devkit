---
okftype: reference.api
description: "Deprecated Reg bitwise OR floating-point overload. Under the control of mask, it performs bitwise OR on two vector data registers and writes the result to the destination register. It supports half and float, takes effect only on AIV, and its replacement is the integer overload of the same bit width."
tag:
  - roles: [compute]
  - operations: [bitwise_or]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_logic/asc_or.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The floating-point type overloads (half, float) are deprecated. Use the integer overload of the same bit width instead. The two declarations near include/c_api/reg_compute/compute/reg_logic.h:1589,1602 both carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table row 128 has the same source."
---
# asc_or (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:32:06.538Z pushedAt=2026-11-18T03:12:16.632Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

**The floating-point overloads of this API are deprecated. Use the [asc_or](../reg_compute/reg_logic/asc_or.md) integer overload of the same bit width based on the data type instead.**

Performs a bitwise OR (|) operation on source operands `src0` and `src1` according to `mask`, and writes the result to destination operand `dst`.

The calculation formula is as follows:

$$
dst_i = src0_i \,|\, src1_i
$$

This API takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline void asc_or(vector_<dtype>& dst,
                                   vector_<dtype> src0,
                                   vector_<dtype> src1,
                                   vector_bool mask)
```

### Data Types Supported by dtype

`dtype` can be `half` or `float`.

### Prototype Example

```c
__simd_callee__ inline void asc_or(vector_half& dst,
                                   vector_half src0,
                                   vector_half src1,
                                   vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :--- |
| dst | Output | Destination operand (vector data register). |
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register). When the corresponding bit is 1, the element participates in the computation; when it is 0, the element does not participate, and the elements that do not participate are set to 0 in the output. |

## Return Value

None

## Constraints

- When this API is called on a non-AIV, it returns directly.
- `mask` must be assigned a value in advance through the [mask setting API](../defs/type/data_type_definition.md#mask-register) before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect positions of valid elements.
- The number of elements participating in the computation is VL divided by `sizeof(dtype)`.
- When the `mask` bit is 0, the corresponding bit of `dst` is written as 0.

## Examples

The following example uses the deprecated overload of the `half` type, and is provided only to illustrate compatible invocation. For new code, migrate to the integer overload of equal bit width as described in the function description.

```cpp
vector_half dst;
vector_half src0;
vector_half src1;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_or(dst, src0, src1, mask);
```
