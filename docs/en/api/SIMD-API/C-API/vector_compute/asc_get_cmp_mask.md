# asc_get_cmp_mask

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:08:20.258Z pushedAt=2026-09-08T03:45:05.490Z -->

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

This API is used to obtain the comparison result of the Compare operation.

## Prototype

```cpp
__aicore__ inline void asc_get_cmp_mask(__ubuf__ void* dst)
```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Address for storing the comparison result.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

It must be used together with the Compare operation.

## Example

```cpp
constexpr uint32_t total_length = 128;
__ubuf__ int8_t dst[total_length];
...     // Perform the Compare operation.
asc_get_cmp_mask(dst);
```