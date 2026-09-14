# asc_get_l0c2gm_unitflag

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:31:52.227Z -->

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

When in-path quantization is performed during data transfer, call this API to obtain the **unit_flag** setting.

## Prototype

```cpp
__aicore__ inline uint64_t asc_get_l0c2gm_unitflag()
```

## Parameters

None

## Return Value

**unit_flag** setting. **unit_flag** enables fine-grained parallelism between matrix computation instructions and matrix transfer instructions. When this feature is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. This feature does not apply to scenarios where the L0C Buffer is used for accumulation.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
uint64_t unitflag_value = asc_get_l0c2gm_unitflag();
```
