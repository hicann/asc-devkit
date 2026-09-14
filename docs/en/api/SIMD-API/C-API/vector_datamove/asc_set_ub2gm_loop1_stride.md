# asc_set_ub2gm_loop1_stride

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:56:51.030Z pushedAt=2026-09-08T13:17:42.538Z -->

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

When moving data from the Unified Buffer (UB) to Global Memory (GM), call this API to set the interval between data blocks of the source operand in adjacent iterations of the inner loop, as well as the interval between data blocks of the destination operand in adjacent iterations.

The following figure uses the source operand transfer scenario as an example.

![Source operand transfer scenario example](../figures/source_operand_transfer_scenario_example.png)

## Prototype

```cpp
__aicore__ inline void asc_set_ub2gm_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| loop1_src_stride | Input | Sets the interval between data blocks of the source operand in adjacent iterations of the inner loop, in bytes. The value range is [0, 2^21), and loop1_src_stride must be 32B-aligned. |
| loop1_dst_stride | Input | Sets the interval between data blocks of the destination operand in adjacent iterations of the inner loop, in bytes. The value range is [0, 2^40). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

After each setting of loop-related parameters, a register reset is required (set the loop count to 1); otherwise, the next data transfer will be affected.

## Example

```cpp
uint32_t loop1_size = 2;
uint32_t loop2_size = 2;
uint64_t loop1_src_stride = 96;
uint64_t loop1_dst_stride = 128;
uint64_t loop2_src_stride = 192;
uint64_t loop2_dst_stride = 288;
asc_set_ub2gm_loop_size(loop1_size, loop2_size);
asc_set_ub2gm_loop1_stride(loop1_src_stride, loop1_dst_stride);
asc_set_ub2gm_loop2_stride(loop2_src_stride, loop2_dst_stride);
asc_copy_ub2gm_align(dst, src, 2, 48 * sizeof(int8_t), 0, 48, 48);
asc_set_ub2gm_loop_size(1, 1);
```
