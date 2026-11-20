---
okftype: reference.api
description: "On the Ascend 950PR and 950DT products, copies multiple fractal cubes from Global Memory to the L1 Buffer in units of 512-byte fractals, split by row/column start positions and strides. Supports 2D cube copy mode without fractal format conversion during copy. Valid only when executed by AIC."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md
    kind: requires
    condition: "Source data must be arranged in fractal format before copy-in"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
    condition: "Address alignment and copy granularity requirements"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: requires
    condition: "l2_cache_mode parameter value and semantics"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "insert a synchronization instruction when there is a data dependency with other instructions"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "overload:asc_copy_gm2l1__PL1bf16_PGbf16_u32_u32_u16_u16_u16_u8_u8"
    scope_basis: resolved
    note: "The legacy-parameter 2D overload (uint8_t decomp_mode, uint8_t l2_cache_ctl) and its corresponding _sync form have been marked deprecated since 9.2.0 (header ASC_DEPRECATED(9.2.0)), but the implementation is retained and still callable; the replacement is the strongly-typed overload on this page combined with asc_sync."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: fallback
    note: "The CANN 9.2.0 Release Notes record a fix for the issue where some 3510 implementations of asc_copy_gm2l1 lacked a parameter when calling the underlying CCE instruction; the original text only says "some 3510 implementations" without specifying the exact overload, so it is temporarily recorded at the page level pending further refinement."
---
# asc_copy_gm2l1 (NPU architecture version 3510, 2D cube copy mode)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:26:08.339Z pushedAt=2026-11-18T08:15:34.246Z nl=2 -->

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
<!-- npu="910" id4 -->
- Atlas training products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id7 -->

## Description

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Copies data from Global Memory to the L1 Buffer. Using a 512-byte fractal as the unit, it slices and extracts multiple fractal cubes from GM according to the start position and stride in the row and column directions, and copies them to the L1 Buffer. Fractal format conversion is not supported during the copy. This interface is used to load 2D format data required for cube computation.

This interface is valid only when executed on the AIC.

For the data layout before copying, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md). For address alignment and copy granularity requirements, see [Cube Computation Input Copy Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this interface.

## Prototype

```cpp
// Placeholder form
__aicore__ inline void asc_copy_gm2l1(__cbuf__ <dtype>* dst,
                                      __gm__ <dtype>* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      asc_load_l2_cache_mode l2_cache_mode)
```

### Data Types Supported by dtype

The values of `dtype` are: `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `void`.

### Prototype Example

```cpp
// Example: 2D cube copy of the bfloat16_t type.
__aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t* dst,
                                      __gm__ bfloat16_t* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      asc_load_l2_cache_mode l2_cache_mode)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand, stored in the L1 Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, stored in GM. The source address must be 1-byte aligned. |
| m_start_position | Input | Start position of the first fractal in the source cube in the row direction, in units of 16 elements. Value range: $[0, 2^{32}-1]$. |
| k_start_position | Input | Start position of the first fractal in the source cube in the column direction, in units of 32 bytes. Value range: $[0, 2^{32}-1]$. |
| dst_stride | Input | Interval between the start address of the previous fractal and the start address of the next fractal in the column direction of the destination cube, in units of 512 bytes. Value range: [0, 4095]. |
| m_step | Input | Fractal stride in the row direction of the source cube, that is, the number of fractal rows to copy, in units of 16 elements. Value range: [1, 4095]. |
| k_step | Input | Fractal stride in the column direction of the source cube, that is, the number of fractal columns to copy, in units of 32 bytes. Value range: [1, 4095]. |
| l2_cache_mode | Input | Uses the [asc_load_l2_cache_mode](../../defs/enum/asc_load_l2_cache_mode.md) enumeration value to configure the data management policy in the L2 Cache. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- If this API is not called by AIC, it returns directly.
- The dst start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, it will cause an abnormal copy.
- The src start address must be 1-byte aligned (Global Memory alignment requirement); otherwise, it will cause an abnormal copy.
- If this instruction has data dependencies with other instructions, you need to insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to ensure that multiple instructions are serialized and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the dst offset and the copy size must not exceed the boundary; otherwise, an exception is triggered.

### 2D Cube Copy Mode Constraints

- When m_step or k_step is 0, no copy is performed, and this API is treated as a NOP (no operation).
- The API uses a complete 512-byte fractal as the minimum copy granularity. Copying only part of the elements within a fractal is not supported, and the padding function is not supported.
- The value range of dst_stride is [0, 4095]. An out-of-range value will be clipped, causing the copy result to be unexpected.
- The value range of m_step is [1, 4095]. An out-of-range value will be clipped, causing the copy result to be unexpected.
- The value range of k_step is [1, 4095]. An out-of-range value will be clipped, causing the copy result to be unexpected.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

In the sample, the shapes of cubes A, B, and C are all [128,128], and the data format is Nz. A and B are stored in GM in Nz format and are copied to the L1 Buffer by cyclically calling the 2D cube copy overload of this API for 8 fractal columns, copying one fractal column at a time. Then A is copied to the L0A Buffer via [asc_copy_l12l0a](asc_copy_l12l0a_2d_arch_3510.md), and B is copied to the L0B Buffer via [asc_copy_l12l0b_transpose](asc_copy_l12l0b_2d_arch_3510.md). [asc_mmad](../../cube_compute/mmad_compute/asc_mmad.md) is called to complete the computation. When C is copied out from the L0C Buffer to GM, Nz2ND and Nz2DN are disabled to keep the Nz format. In the sample, a single core occupies 64KB of L1 Buffer, 32KB of L0A Buffer, 32KB of L0B Buffer, and 64KB of L0C Buffer. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr uint32_t CUBE = 16;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00;
constexpr uint16_t M_FRACTALS = M / CUBE;
constexpr uint16_t K_FRACTALS = K / CUBE;
constexpr uint16_t N_FRACTALS = N / CUBE;

__global__ __cube__ void AscCopyGm2l1TwoDimNzKernel(
    __gm__ half* a, __gm__ half* b, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[M * K], b_l1[K * N];
    __ca__ half a_l0[M * K];
    __cb__ half b_l0[K * N];
    __cc__ float c_l0[ELEMENTS];

    // A and B are already arranged in Nz format in GM, and no format conversion is performed during the 2D copy.
    for (uint16_t k_block = 0; k_block < K_FRACTALS; ++k_block) {
        const uint32_t offset = k_block * M * CUBE;
        asc_copy_gm2l1(a_l1 + offset, a + offset, 0, 0, M_FRACTALS, M_FRACTALS, 1,
            asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    }
    for (uint16_t n_block = 0; n_block < N_FRACTALS; ++n_block) {
        const uint32_t offset = n_block * K * CUBE;
        asc_copy_gm2l1(b_l1 + offset, b + offset, 0, 0, K_FRACTALS, K_FRACTALS, 1,
            asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    }
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // A is converted from Nz to the Zz arrangement required by the L0A Buffer, and B is converted from Nz to the Zn arrangement required by the L0B Buffer.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M_FRACTALS, K_FRACTALS, M_FRACTALS, M_FRACTALS);
    asc_copy_l12l0b_transpose(
        b_l0, b_l1, 0, 0, K_FRACTALS, N_FRACTALS, K_FRACTALS, N_FRACTALS);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Disable Nz2ND and Nz2DN to keep the Nz layout in the L0C Buffer and copy it directly to GM.
    asc_copy_l0c2gm(output, c_l0, N, M, M * CUBE, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, false, false, false);
    asc_sync();
}

uint16_t ToHalfBits(int32_t value)
{
    return value > 0 ? HALF_ONE : (value < 0 ? HALF_NEG_ONE : 0);
}

size_t NzIndex(uint32_t row, uint32_t col, uint32_t rows)
{
    return static_cast<size_t>(col / CUBE) * rows * CUBE + row * CUBE + col % CUBE;
}

void PrintNzBlock(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<float> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
            a[NzIndex(row, k, M)] = ToHalfBits(value);
        }
    }
    for (uint32_t k = 0; k < K; ++k) {
        for (uint32_t col = 0; col < N; ++col) {
            const int32_t value = static_cast<int32_t>((2 * k + col) % 3) - 1;
            b[NzIndex(k, col, K)] = ToHalfBits(value);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            int32_t sum = 0;
            for (uint32_t k = 0; k < K; ++k) {
                const int32_t a_value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
                const int32_t b_value = static_cast<int32_t>((2 * k + col) % 3) - 1;
                sum += a_value * b_value;
            }
            golden[NzIndex(row, col, M)] = static_cast<float>(sum);
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    half *a_device = nullptr, *b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    AscCopyGm2l1TwoDimNzKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device,
        output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintNzBlock("Output Nz first block", output);
    PrintNzBlock("Golden Nz first block", golden);
    bool passed = true;
    for (size_t i = 0; i < output.size(); ++i) {
        if (output[i] != golden[i]) {
            std::cerr << "Mismatch at Nz offset " << i << ": output=" << output[i]
                      << ", golden=" << golden[i] << std::endl;
            passed = false;
            break;
        }
    }

    aclrtFree(output_device);
    aclrtFree(b_device);
    aclrtFree(a_device);
    aclrtResetDevice(0);
    aclFinalize();
    if (!passed) {
        return 1;
    }
    std::cout << "[Success] asc_copy_gm2l1 2D Nz cube multiplication passed." << std::endl;
    return 0;
}
```
