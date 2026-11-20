---
okftype: reference.api
description: "Deprecated Reg bitwise AND floating-point overload. Performs bitwise AND on two vector data registers under mask control and writes the result to the destination register. Supports fp8_e4m3fn_t, fp8_e5m2_t, half, bfloat16_t, and float. Takes effect only on AIV."
tag:
  - roles: [compute]
  - operations: [bitwise_and]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_logic/asc_and.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The floating-point overload is deprecated; use the integer overload of the same bit width instead. In include/c_api/reg_compute/compute/reg_logic.h, all five declarations of fp8_e4m3fn_t, fp8_e5m2_t, half, bfloat16_t, and float carry ASC_DEPRECATED(9.2.0), same source as row 128 of the CANN_9_2_0 deprecated table."
---
# asc_and (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:14:55.494Z pushedAt=2026-11-18T03:12:16.361Z nl=2 -->

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

**The floating-point type overloads of this interface are deprecated. Use the [asc_and](../reg_compute/reg_logic/asc_and.md) integer overload of the same bit width according to the data type instead.**

Performs a bitwise AND (&) operation on the source operands `src0` and `src1` according to `mask`, and writes the result to the destination operand `dst`.

The calculation formula is as follows:

$$
dst_i = src0_i \& src1_i
$$

This interface takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline void asc_and(vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)
```

### Data Types Supported by dtype

The value of `dtype` is one of the following: `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `float`.

### Prototype Example

```c
__simd_callee__ inline void asc_and(vector_half& dst,
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

- When this interface is called on a non-AIV, it returns directly.
- `mask` must be assigned in advance through the [mask setting interface](../defs/type/data_type_definition.md#mask-register) before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect positions of valid elements.
- The number of elements participating in the computation is VL divided by `sizeof(dtype)`.
- When the `mask` bit is 0, the corresponding bit of `dst` is written as 0.

## Examples

The following example uses the deprecated overload of the `half` type, which is only for illustrating compatible invocation. For new code, migrate to the integer overload of the same bit width as described in the function description.

```cpp
vector_half dst;
vector_half src0;
vector_half src1;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_and(dst, src0, src1, mask);
```
