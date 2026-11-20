---
okftype: reference.api
description: "Copies a cube from L1 Buffer to L0C Buffer as the initial value for cube multiply-add accumulation, supporting bfloat16_t, half, float, int32_t, uint32_t, and type combinations from float to half or bfloat16_t."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: related
    condition: "L0C data as the c_matrix_source/c_matrix_init_val accumulation initial value"
version: ">=9.1.0"
version_changes: []
---
# asc_copy_l12l0c

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:05:47.717Z pushedAt=2026-11-18T08:15:34.292Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Moves a cube from L1 Buffer to L0C Buffer. The data types supported by this move are as follows:
- bfloat16_t->bfloat16_t
- half->half
- float->half
- float->bfloat16_t
- float->float
- int32_t->int32_t
- uint32_t->uint32_t

For the formats of the source and destination cubes, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); when the data in L0C participates in cube multiply-add as the accumulation initial value, see the `c_matrix_source` and `c_matrix_init_val` parameters of [asc_mmad](../../cube_compute/mmad_compute/asc_mmad.md).

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l12l0c(__cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                              |
| :--- |:------|:----------------------------------------------------------------|
| dst | Output    | Start address of the destination operand.|
| src | Input    | Start address of the source operand.|
| n_burst | Input    | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input    | Length of each consecutive data block to be copied, in units of 32 bytes. Value range: [1, 65535]. |
| src_gap | Input    | Gap between adjacent iterations, from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the source operand. |
| dst_gap | Input    | Gap between adjacent iterations, from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If this instruction has data dependency with other instructions, insert [Synchronization Instructions](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.

## Examples

```cpp
// The number of consecutive data blocks to be copied is 1.
constexpr uint16_t n_burst = 1;
// The length of each consecutive data block to be copied is 32 bytes.
constexpr uint16_t len_burst = 1;
// Between adjacent iterations, the gap between the start addresses of the previous and next fractals in the source operand is 64 bytes.
constexpr uint16_t src_gap = 1;
// Between adjacent iterations, the gap between the start addresses of the previous and next fractals in the destination operand is 32 bytes.
constexpr uint16_t dst_gap = 0;
__cbuf__ half src[256];
__cc__ half dst[256];
asc_copy_l12l0c(dst, src, n_burst, len_burst, src_gap, dst_gap);
```
