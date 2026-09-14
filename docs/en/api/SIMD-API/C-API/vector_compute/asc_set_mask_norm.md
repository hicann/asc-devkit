# asc_set_mask_norm

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:18:18.350Z pushedAt=2026-09-08T03:45:05.581Z -->

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

Sets the mask mode to Normal mode, which is the system default mode. In this mode, the **Mask** parameter controls the elements that participate in computation within a single iteration during high-dimensional split computation.

## Prototype

```cpp
__aicore__ inline void asc_set_mask_norm()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

This API must be used together with [asc_set_vector_mask](asc_set_vector_mask.md).

## Example

```cpp
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // Set all elements to participate in computation within each iteration.

// Perform high-dimensional split computation.
constexpr uint32_t total_length = 128; //total_length refers to the total length of the data participating in computation.
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];

uint8_t repeat = 1; // One repeat can process 128 half-type data elements.
uint8_t dst_block_stride = 1;
uint8_t src0_block_stride = 1;
uint8_t src1_block_stride = 1;
uint8_t dst_repeat_stride = 8;
uint8_t src0_repeat_stride = 8;
uint8_t src1_repeat_stride = 8;
asc_add(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride,
    dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
