---
okftype: reference.api
description: "Copies half, bfloat16_t, int32_t, or float bias data from the L1 Buffer to the BiasTable Buffer. half and bfloat16_t support in-line conversion to float. Two copy modes are provided: High-Dimensional Tiling and contiguous copy by size."
tag:
  - roles: [data_movement]
  - operations: [conversion, copy]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: related
    condition: "Bias is read by Mmad when used as the initial value of the C matrix"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_copy_l12bt__u64_PL1f16_u32"
    scope_basis: resolved
    note: "The Contiguous Data Copy Mode overload (uint64_t dst, __cbuf__ <dtype>* src, uint32_t size) was added with the 9.2.0 header file unification and parameterization refactoring; 9.1.0 only has the seven-parameter High-Dimensional Tiling prototype."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_copy_l12bt__u64_PL1bf16_u32"
    scope_basis: resolved
    note: "The contiguous data copy mode overload (uint64_t dst, __cbuf__ <dtype>* src, uint32_t size) was added with the 9.2.0 header file unification and parameterization refactoring; 9.1.0 only has the seven-parameter high-dimensional tiling prototype."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_copy_l12bt__u64_PL1i32_u32"
    scope_basis: resolved
    note: "The contiguous data copy mode overload (uint64_t dst, __cbuf__ <dtype>* src, uint32_t size) was added with the 9.2.0 header file unification and parameterization refactoring; 9.1.0 only has the seven-parameter high-dimensional tiling prototype."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_copy_l12bt__u64_PL1f32_u32"
    scope_basis: resolved
    note: "The contiguous data copy mode overload (uint64_t dst, __cbuf__ <dtype>* src, uint32_t size) was added with the 9.2.0 header file unification and parameterization refactoring; 9.1.0 only has the seven-parameter high-dimensional tiling prototype."
---
# asc_copy_l12bt (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:38:31.553Z pushedAt=2026-11-18T08:15:34.264Z nl=2 -->

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

Copies the bias data in the L1 Buffer to the BiasTable Buffer for the cube unit (Mmad) to read as bias input. It supports copying source operands of the `half`, `bfloat16_t`, `int32_t`, and `float` types. During the copy, `half` and `bfloat16_t` undergo in-line format conversion before being written to the BiasTable Buffer: `bfloat16_t` is automatically converted to `float`; `half` is converted to `float` when conversion is enabled, that is, when `conv_control` is set to 1, and when conversion is not enabled, that is, when `conv_control` is set to 0, each `half` element is only expanded to 32 bits (the upper 16 bits are placeholder data).

This API provides two functional modes with completely different parameter lists. Users select the corresponding overload as needed:

- **High-Dimensional Tiling Copy**: Implements contiguous and non-contiguous copying by configuring copy parameters such as the number of data blocks, the length of a single data block, and the source/destination stride.
- **Contiguous Data Copy**: Specifies only `size` and copies as a single contiguous data block, with both the source and destination strides being 0.

This API is valid only when executed on the AIC.

For the format of the Bias cube, see [Auxiliary Cube Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/aux_cube_fractal_format_details.md); in [asc_mmad](../../cube_compute/mmad_compute/asc_mmad.md), select Bias as the initial value of the C cube according to the overload for the corresponding architecture.

## Prototype

### High-Dimensional Tiling Copy Mode

```c
// Placeholder form.
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint16_t conv_control,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t source_gap,
                                      uint16_t dst_gap)
```

#### Data Types Supported by dtype

`dtype` can be: `half`, `bfloat16_t`, `int32_t`, `float`.

#### Prototype Example

```c
// Example: high-dimensional tiling copy of the bfloat16_t type.
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ bfloat16_t* src,
                                      uint16_t conv_control,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t source_gap,
                                      uint16_t dst_gap)
```

### Contiguous Data Copy Mode

```c
// Placeholder form
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint32_t size)
```

#### Data Types Supported by dtype

The value of `dtype` is one of the following: `half`, `bfloat16_t`, `int32_t`, `float`.

#### Prototype Example

```c
// Example: contiguous data copy of the float type.
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ float* src,
                                      uint32_t size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, stored in the BiasTable Buffer. The destination address must be 64-byte aligned. |
| src | Input | Source operand, stored in the L1 Buffer. The source address must be 32-byte aligned. |
| conv_control | Input | When the source operand is of the `half` type, specifies whether to enable in-place format conversion.<br>&nbsp;&nbsp;&bull; `0`: Conversion is disabled. Each `half` element is expanded to 32 bits (the upper 16 bits are placeholder data) and then written to BT.<br>&nbsp;&nbsp;&bull; `1`: Conversion is enabled. Each `half` element is converted to `float` and then written to BT. This can be set only when dtype is `half`; for other dtypes, it must be set to 0. |
| n_burst | Input | Number of contiguous data blocks to be copied. Value range [1, 4095]. |
| len_burst | Input | Length of each contiguous data block, in 32-byte units. Value range [1, 65535]. When dtype is `float` or `int32_t`, it must be 2-aligned (that is, the value is an even number); otherwise, triggers an exception. |
| source_gap | Input | Gap between two adjacent copies on the source side (L1 Buffer), in 32-byte units, from the end position of the previous copy to the start position of the next copy. Value range [0, 65535]. It is meaningless when `n_burst=1`; set it to 0. |
| dst_gap | Input | Gap between two adjacent copies on the destination side (BiasTable Buffer), in 32-byte units, from the end position of the previous copy to the start position of the next copy. Value range [0, 65535]. It is meaningless when `n_burst=1`; set it to 0. When dtype is `float` or `int32_t`, it must be 2-aligned (that is, the value is an even number); otherwise, triggers an exception. |
| size | Input | Total size of the data to be copied, in bytes.<br>**Note: size must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This API is not an AIC call and returns directly.
- The dst start address must be 64-byte aligned (BiasTable Buffer alignment requirement), and the src start address must be 32-byte aligned (L1 Buffer alignment requirement). Otherwise, an exception is triggered.
- The BiasTable Buffer capacity limit is 4KB. The sum of the dst offset and the copy size must not exceed the boundary. Otherwise, a write overflow exception is triggered.
- The L1 Buffer capacity limit is 512KB. The sum of the src offset and the copy size must not exceed the boundary. Otherwise, a read exception is triggered.
- If this instruction has data dependencies with other instructions, you need to insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to ensure that multiple instructions are serialized and prevent abnormal data.
- When n_burst is 0, no copy is performed, and this API is treated as a NOP (no operation).
- When dtype is `float` or `int32_t`, both len_burst and dst_gap must be 2-aligned (that is, the value is an even number), and size/32 must be an even number. Otherwise, a copy exception is triggered.
- When conv_control is enabled (non-zero), dtype must be `half`. Otherwise, triggers a copy exception. For values other than 0/1, only the lowest bit takes effect (for example, 2 is equivalent to 0, meaning conversion is disabled; 3 is equivalent to 1, meaning conversion is enabled).

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
constexpr uint32_t BYTES = N * sizeof(half);

__global__ __cube__ void CopyL12btKernel(__gm__ half* input)
{
    asc_init();
    __cbuf__ half l1[N];
    asc_copy_gm2l1(l1, input, 1, BYTES / 32, asc_channel_pad_mode::NONE, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12bt(0, l1, BYTES);
    asc_sync();
}

int main()
{
    aclInit(nullptr); aclrtSetDevice(0);
    half* input = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    CopyL12btKernel<<<1, 0>>>(input);
    const aclError ret = aclrtSynchronizeDevice();
    std::printf("M=%u N=%u K=%u asc_copy_l12bt=%s\n", M, N, K,
        ret == ACL_SUCCESS ? "PASS" : "FAIL");
    aclrtFree(input); aclrtResetDevice(0); aclFinalize();
    return ret == ACL_SUCCESS ? 0 : 1;
}
```
