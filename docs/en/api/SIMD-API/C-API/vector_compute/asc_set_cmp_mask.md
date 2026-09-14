# asc_set_cmp_mask

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:14:28.855Z pushedAt=2026-09-08T03:45:05.578Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Sets the Mask used for selection in the [asc_select](asc_select.md) operation.

## Prototype

```cpp
__aicore__ inline void asc_set_cmp_mask(__ubuf__ void* sel_mask)
```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|sel_mask|Input|Start address of the Mask used for selection.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

Must be used together with the [asc_select](asc_select.md) operation.

## Example

```cpp
constexpr uint32_t total_length = 256;
constexpr uint32_t sel_length = 8;
__ubuf__ uint8_t sel_mask[sel_length];
__ubuf__ float dst[total_length];
__ubuf__ float src0[total_length];
__ubuf__ float src1[total_length];
asc_set_cmp_mask(sel_mask);
asc_sync_pipe(PIPE_V);
asc_select(dst, src0, src1, total_length);
```
