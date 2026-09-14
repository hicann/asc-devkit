# asc_set_gm2l1_loop1_stride

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:54:36.925Z -->

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

When transferring data from Global Memory (GM) to L1 Buffer, call this API to set the stride between adjacent iteration data blocks in the inner loop.

The following figure uses the source operand transfer scenario as an example.

![Source operand transfer scenario example](../figures/source_operand_transfer_scenario_example.png)

## Prototype

```cpp
__aicore__ inline void asc_set_gm2l1_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| loop1_src_stride     | Input     | Stride between the data blocks of the source operand in adjacent iterations of the inner loop, in bytes, with a value range of [0, 2^40].|
| loop1_dst_stride     | Input     | Stride between the data blocks of the destination operand in adjacent iterations of the inner loop, in bytes, with a value range of [0, 2^21], and must be 32B-aligned.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
asc_set_gm2l1_loop_size(2, 2);
asc_set_gm2l1_loop1_stride(96, 128);
asc_set_gm2l1_loop2_stride(192, 288);
constexpr uint32_t n_burst = 2;
constexpr uint32_t len_burst = 2;
constexpr uint8_t left_padding_count = 0;
constexpr uint8_t right_padding_count = 0;
constexpr bool data_select_bit = false;
constexpr uint8_t l2_cache_ctl = 0;
constexpr uint64_t burst_src_stride = 0;
constexpr uint32_t burst_dst_stride = 0;
__gm__ half src[256];
__cbuf__ half dst[256];
asc_copy_gm2l1_align(dst, src, n_burst, len_burst,  left_padding_count, right_padding_count, data_select_bit, l2_cache_ctl, burst_src_stride, burst_dst_stride);
asc_set_gm2l1_loop_size(1, 1);
```
