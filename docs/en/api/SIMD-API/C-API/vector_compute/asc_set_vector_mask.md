# asc_set_vector_mask

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:20:38.364Z pushedAt=2026-09-08T03:45:05.583Z -->

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

Some vector computation APIs require the mask to be set in advance to indicate which data participates in the computation. This API is used to set the mask. Before using it, call [asc_set_mask_count](asc_set_mask_count.md) and [asc_set_mask_norm](asc_set_mask_norm.md) to set the mask mode. The meaning of the mask in different modes is as follows:
- Normal mode is used for high-dimensional split computation APIs. In this mode, the **Mask** parameter controls the elements that participate in the computation within a single iteration. It controls which elements participate in the computation on a bit-by-bit basis: a bit value of 1 indicates participation in the computation, and 0 indicates non-participation. It is divided into **mask1** (high-order mask) and **mask0** (low-order mask). The value range of the parameter is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, mask0, mask1 ∈ [0, 2^64-1], and they cannot both be 0. When the operand is 32-bit, mask1 is 0 and mask0 ∈ (0, 2^64-1]. When the operand is 64-bit, mask1 is 0 and mask0 ∈ (0, 2^32-1].
- Counter mode is used for the first-n data computation APIs. In this mode, the **Mask** parameter controls the number of elements that participate in the computation during the entire computation process. The first-n data computation APIs already implement the conversion between Normal mode and Counter mode, so users do not need to configure it themselves.

## Prototype

```cpp
__aicore__ inline void asc_set_vector_mask(uint64_t mask1, uint64_t mask0)
```

## Parameters

|Parameter Name|Input/Output|Description|
| ------------ | ------------ | ------------ |
|mask1|Input|Normal mode: high-order mask value.<br>Counter mode: Set to 0; this input parameter does not take effect.|
|mask0|Input|Normal mode: low-order mask value.<br>Counter mode: number of elements that participate in the computation during the entire vector computation process.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

You must first call [asc_set_mask_count](asc_set_mask_count.md) or [asc_set_mask_norm](asc_set_mask_norm.md) to set the mask mode.

## Example

See [asc_set_mask_count](asc_set_mask_count.md#example) or [asc_set_mask_norm](asc_set_mask_norm.md#example).