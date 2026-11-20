---
okftype: reference.api
description: "Transposes and copies 2D data from L1 Buffer to L0B Buffer on the AIC in units of 512-byte fractals, organized by fractal index, repeat count, source stride, and inter-fractal gap, covering b4 to b32 data types."
tag:
  - roles: [data_movement]
  - operations: [copy, transpose]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
version: ">=9.1.0"
version_changes: []
---
# asc_copy_l12l0b_trans (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:01:56.653Z pushedAt=2026-11-18T08:15:34.290Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Copies data from L1 Buffer to L0B Buffer. During the copy, a 2D fractal cube transpose is performed in units of 512-byte data fractals. This interface organizes the copy according to parameters such as fractal index, repeat count, source-side stride, destination-side gap, and fractal interval, and is applicable to scenarios where 2D-format data required for ordinary cube computation is copied between L1 Buffer and L0B Buffer with transposition.

The following examples explain the interface functions and key parameters: in the figures below, an N-shape or a Z-shape represents a fractal.

- For the uint8_t/int8_t data type, each iteration processes 32 × 32 × 1B data and can handle 2 fractals (a fractal is 512B). In each iteration, 2 consecutive 16 × 32 fractals in the source operand will be merged into 1 32 × 32 block cube, the cube transpose is performed based on blocks, and after transposition it is split into 2 16 × 32 fractals, which can have different layouts according to the destination operand division and fractal interval parameters.

As shown in the figure below:

- A total of 3072B of data is processed. Each iteration processes 32 × 32 × 1B data, so 3 iterations can complete the operation, repeat_time = 3;
- src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block cube and the next block cube in the source operand is 1 (unit: 32 × 32 × 1B). The unit here is actually the cube size after concatenation;
- dst_gap = 1, indicating that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B);
- dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512B).
![](../../../../figures/asc_copy_l12l0_trans_b8_example1.png)

As shown in the figure below:

- The explanation of repeat_time and src_stride is the same as in the example in the figure above.
- dst_gap = 0, indicating that between adjacent iterations, the end address of the first fractal of the previous iteration of the destination operand and the start address of the first fractal of the next iteration have no interval.
- dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).
![](../../../../figures/asc_copy_l12l0_trans_b8_example2.png)

- For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B data and can process 1 fractal (a fractal is 512B). In each iteration, one 16 × 16 fractal in the source operand will be transposed.
  - Because each iteration processes 16 × 16 × 2B data, 3 iterations are required to complete, so repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block cube and the next block cube of the source operand is 1 (unit: 16 × 16 × 2B);
  - dst_gap = 0, indicating that between adjacent iterations, the end address of the first fractal of the previous iteration of the destination operand and the start address of the first fractal of the next iteration have no interval;
  - In this scenario, because the fractal is the block cube, each iteration processes one fractal, and there is no interval between fractals within an iteration, so this parameter is invalid.
![](../../../../figures/asc_copy_l12l0_trans_b16_example.png)

- For the float/int32_t/uint32_t data type, each iteration processes 16 × 16 × 4B data and can process 2 fractals (a fractal is 512B). In each iteration, 2 consecutive 16 × 8 fractals of the source operand will be merged into 1 16 × 16 block cube, transposition is performed based on the block cube, and after transposition it is split into 2 16 × 8 fractals, which can have different arrangements according to parameters such as the destination operand fractal interval.
As shown in the figure below:
  - Because each iteration processes 16 × 16 × 4B data, 3 iterations can complete the operation, repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block cube and the next block cube of the source operand is 1 (unit: 16 × 16 × 4B). The unit here is actually the cube size after concatenation;
  - dst_gap = 1, indicating that between adjacent iterations, the interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand is 1 (unit: 512B);
  - dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).
![](../../../../figures/asc_copy_l12l0_trans_b32_example1.png)

As shown in the figure below:
- The explanation of repeat_time and src_stride is consistent with the example in the figure above.
- dst_gap = 0, indicating that between adjacent iterations, the destination operand's previous iteration first fractal's end address and the next iteration first fractal start address has no interval.
- dst_frac_gap = 2, indicating that within each iteration, the interval between the destination operand's previous fractal's end address and the next fractal start address is 2 (unit: 512B).
![](../../../../figures/asc_copy_l12l0_trans_b32_example2.png)

- For the int4b\_t data type, each iteration processes 64\*64\*0.5B data and can handle 4 fractals (a fractal is 512B). In each iteration, 4 consecutive 16\*64 fractals in the source operand will be merged into 1 64\*64 block cube, transposition is performed based on the block cube, and after transposition it is split into 4 16\*64 fractals, which can have different arrangements according to the destination operand fractal interval and other parameters.

    The int4b\_t data type requires two numbers to be combined into one int8\_t or uint8\_t number. The combination rules are as follows:

    ![](../../../../figures/int4b_example.png)

  As shown in the figure below:
  - A total of 6144B of data needs to be processed. Each iteration processes 64\*64\*0.5B data, and 3 iterations can complete the processing, so repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the source operand's previous block cube and the next block cube start address is 1 (unit: 64\*64\*0.5B). The unit here is actually the cube size after concatenation;
  - dst_gap = 1, indicating that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration of the destination operand to the start address of the first fractal of the next iteration is 1 (unit: 512B);
  - dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).

  ![](../../../../figures/asc_copy_l12l0_trans_b4_example1.png)

  As shown in the example below:

  - The explanation of repeat_time and src_stride is consistent with the example in the figure above.
  - dst_gap = 0, indicating that between adjacent iterations, the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand have no interval.
  - dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).

  ![](../../../../figures/asc_copy_l12l0_trans_b4_example2.png)

This interface is valid only when executed on the AIC.

For the data layout before copying, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and copy granularity requirements, see [Cube Computation Input Copy Constraints](cube_compute_input_move_constraint.md), and use them in conjunction with the parameter and constraint descriptions of this interface.

## Prototype

```c
// Placeholder form.
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ <dtype>* dst,
                                             __cbuf__ <dtype>* src,
                                             uint16_t index_id,
                                             uint8_t repeat,
                                             uint16_t src_stride,
                                             uint16_t dst_gap,
                                             uint16_t dst_frac_gap,
                                             uint16_t src_frac_gap)
```

### Data Types Supported by dtype

`dtype` values: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Prototype Example

```c
// Example: transpose copy of the half type.
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ half* dst,
                                             __cbuf__ half* src,
                                             uint16_t index_id,
                                             uint8_t repeat,
                                             uint16_t src_stride,
                                             uint16_t dst_gap,
                                             uint16_t dst_frac_gap,
                                             uint16_t src_frac_gap)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand, stored in the L0B Buffer. The destination address must be 512-byte aligned. |
| src | Input | Source operand, stored in the L1 Buffer. The source address must be 32-byte aligned. |
| index_id | Input | Source-side start fractal index, that is, the sequence number of the start fractal cube in the source cube (counted from 0). Value range: [0, $2^{16}-1$]. |
| repeat | Input | Number of iterative copies. Value range: [1, 255].<br> For the `b4` data type, each iteration processes 4 fractals, and each fractal is 16\*64\*0.5B data;<br> For the `b8` data type, each iteration processes 2 fractals, and each fractal is 16\*32\*1B data;<br>For the `b16` data type, each iteration processes 1 fractal, and each fractal is 16\*16\*2B data; for the `b32` data type, each iteration processes 4 fractals, and each fractal is 16\*8\*4B data. |
| src_stride | Input | Between adjacent repeats, the interval between the start address of the previous fractal and the next fractal of the source operand, in units of 512 bytes. Value range: [0, $2^{16}-1$]. |
| dst_gap | Input | Between adjacent repeats, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand, in units of 512 bytes. The effective stride of the instruction is the passed value plus 1 (that is, passing 0 represents a stride of 1 fractal). Value range: [0, $2^{16}-1$]. |
| dst_frac_gap | Input | Within the same repeat, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand, in units of 512 bytes. The effective stride of the instruction is the passed value plus 1, and this parameter takes effect only for b4/b8/b32. Value range: [0, $2^{16}-1$]. |
| src_frac_gap | Input | Within the same repeat, the interval between the end address of the previous fractal and the start address of the next fractal of the source operand, in units of 512 bytes. The effective stride of the instruction is the passed value plus 1, and this parameter takes effect only for b4/b8/b32. Value range: [0, $2^{16}-1$]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- This interface returns directly when called by a non-AIC.
- The src start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, an address alignment exception is triggered.
- The dst start address must be 512-byte aligned (L0B Buffer alignment requirement); otherwise, an address alignment exception is triggered.
- If this instruction has a data dependency with other instructions, insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512KB. The sum of the src offset and the copy size must not exceed the boundary; otherwise, an address overflow exception is triggered.
- L0B Buffer capacity limit: The total L0B Buffer capacity is 64KB. The sum of the dst offset and the copy size must not exceed the boundary; otherwise, an address overflow exception is triggered.
- When `repeat` is 0, no copy is performed, and this interface is treated as a NOP (no operation).
- The user must ensure that the destination operand's fractals do not overlap after transposition; otherwise, the copy result will not meet expectations.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

constexpr uint32_t M = 128, N = 128, K = 128;
constexpr uint32_t BYTES = K * N * sizeof(half);

__global__ __cube__ void CopyL12l0bTransKernel(__gm__ half* input)
{
    asc_init();
    __cbuf__ half l1[K * N];
    __cb__ half l0b[K * N];
    asc_copy_gm2l1(l1, input, 1, BYTES / 32, asc_channel_pad_mode::NONE, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0b_trans(l0b, l1, static_cast<uint16_t>(0), static_cast<uint8_t>(N / 16),
        static_cast<uint16_t>(K / 16), static_cast<uint16_t>(0), static_cast<uint16_t>(0),
        static_cast<uint16_t>(0));
    asc_sync_pipe(PIPE_MTE1);
}

int main()
{
    aclInit(nullptr); aclrtSetDevice(0);
    half* input = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    CopyL12l0bTransKernel<<<1, 0>>>(input);
    const aclError ret = aclrtSynchronizeDevice();
    std::printf("M=%u N=%u K=%u asc_copy_l12l0b_trans=%s\n", M, N, K,
        ret == ACL_SUCCESS ? "PASS" : "FAIL");
    aclrtFree(input); aclrtResetDevice(0); aclFinalize();
    return ret == ACL_SUCCESS ? 0 : 1;
}
```
