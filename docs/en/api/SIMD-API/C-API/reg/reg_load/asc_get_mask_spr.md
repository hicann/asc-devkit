# asc_get_mask_spr

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:40:14.492Z pushedAt=2026-09-03T12:34:34.543Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

Reads the mask value from the special registers **SPR**{**MASK1**, **MASK0**} and returns the corresponding mask data according to the data type format. Both **MASK0** and **MASK1** are 64-bit registers.

- For the b16 type, the complete 128-bit {**MASK1**, **MASK0**} data is read, and each bit is duplicated into 2 bits and returned.

- For the b32 type, the 64-bit **MASK0** data is read, and each bit is duplicated into 4 bits and returned.

## Prototype

```cpp
__simd_callee__ inline vector_bool asc_get_mask_spr_b16()
__simd_callee__ inline vector_bool asc_get_mask_spr_b32()
```

## Parameters

None

## Return Value

Returns the mask value read from the special registers.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
vector_bool mask_b16 = asc_get_mask_spr_b16();
vector_bool mask_b32 = asc_get_mask_spr_b32();
```