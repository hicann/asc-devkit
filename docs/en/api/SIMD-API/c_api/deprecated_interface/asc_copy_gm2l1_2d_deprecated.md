---
okftype: reference.api
description: "Deprecated 2D matrix copy legacy overload that copies data from GM to L1 Buffer in 512-byte fractal units by row/column start positions and strides, containing two integer parameters decomp_mode and l2_cache_ctl; the replacement uses the l2_cache_mode enum overload."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Synchronization scenario replaced by asc_copy_gm2l1_sync"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The old 2D overloads using uint8_t decomp_mode and uint8_t l2_cache_ctl, and the corresponding _sync overloads, are deprecated. Use asc_copy_gm2l1_2d_arch_3510 with asc_load_l2_cache_mode l2_cache_mode instead; all 30 declarations in include/c_api/cube_datamove/cube_datamove.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_copy_gm2l1 (NPU architecture version 3510, 2D matrix copy mode, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:27:00.057Z pushedAt=2026-11-18T03:12:16.380Z nl=2 -->

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

**The legacy regular overloads of `asc_copy_gm2l1` that use `uint8_t decomp_mode` and `uint8_t l2_cache_ctl`, and the corresponding `asc_copy_gm2l1_sync` interface, are deprecated. Use the [asc_copy_gm2l1 (NPU architecture version 3510, 2D matrix copy mode)](../cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md) interface with the `asc_load_l2_cache_mode l2_cache_mode` parameter instead.**

The header file paths are: `"c_api/cube_datamove/cube_datamove.h"`, `"c_api/composite/cube_datamove_composite.h"`.

Copies data from Global Memory to the L1 Buffer. Using 512-byte fractals as the unit, it extracts multiple fractal matrices from GM according to the start positions and strides in the row and column directions and copies them to the L1 Buffer. Fractal format conversion is not supported during the copy. This interface is used to load 2D-format data required for matrix computation.

This interface is valid only when executed on the AIC.

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
                                      uint8_t decomp_mode,
                                      uint8_t l2_cache_ctl)
__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ <dtype>* dst,
                                           __gm__ <dtype>* src,
                                           uint32_t m_start_position,
                                           uint32_t k_start_position,
                                           uint16_t dst_stride,
                                           uint16_t m_step,
                                           uint16_t k_step,
                                           uint8_t decomp_mode,
                                           uint8_t l2_cache_ctl)
```

### Data Types Supported by dtype

The values of `dtype` are: `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `void`.

### Prototype Example

```cpp
// Example: 2D matrix copy of the bfloat16_t type.
__aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t* dst,
                                      __gm__ bfloat16_t* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      uint8_t decomp_mode,
                                      uint8_t l2_cache_ctl)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand, stored in the L1 Buffer. The destination address requires 32-byte alignment. |
| src | Input | Source operand, stored in GM. The source address requires 1-byte alignment. |
| m_start_position | Input | Start position of the first fractal in the source matrix in the row direction, in units of 16 elements. Value range: $[0, 2^{32}-1]$. |
| k_start_position | Input | Start position of the first fractal in the source matrix in the column direction, in units of 32 bytes. Value range: $[0, 2^{32}-1]$. |
| dst_stride | Input | Interval between the start address of the previous fractal and the start address of the next fractal in the column direction of the destination matrix, in units of 512 bytes. Value range: [0, 4095]. |
| m_step | Input | Fractal stride in the row direction of the source matrix, that is, the number of fractal rows to copy, in units of 16 elements. Value range: [1, 4095]. |
| k_step | Input | Fractal stride in the column direction of the source matrix, that is, the number of fractal columns to copy, in units of 32 bytes. Value range: [1, 4095]. |
| decomp_mode | Input | Reserved parameter. It must be set to 0 currently. |
| l2_cache_ctl | Input | Configures the management policy of the data in the L2 Cache. For the value description, see [Table 2](#table2). |

**Table 2** l2_cache_ctl value description <a id="table2"></a>

| Value | Mode | Meaning |
|------|------|------|
| 0 | Normal mode | Enables L2 Cache and marks the allocated Cache Lines as high replacement priority. |
| 1 | Last mode | &bull; Enables L2 Cache and marks the allocated Cache Lines as low replacement priority.<br>&bull; **Last mode is not supported.**|
| 2 | Persistent mode | &bull; Enables L2 Cache. Data already stored in L2 Cache may be replaced. To ensure that data of a specific GM always remains in L2 Cache, use the resident mode.<br>&bull; Cache Lines marked as resident mode can only be replaced by other Cache Lines also marked as resident mode.<br>&bull; **Persistent mode is not supported.**|
| 4 | Disable mode | Does not enable L2 Cache. Each time, data is read directly from GM, and the state of existing Cache Lines remains unchanged. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- This interface returns directly when called by a non-AIC.
- The dst start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, copy exceptions occur.
- The src start address must be 1-byte aligned (Global Memory alignment requirement); otherwise, copy exceptions occur.
- If this instruction has destination address overlap with other instructions, insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512KB. The sum of the dst offset and the copy size must not exceed the boundary; otherwise, an exception is triggered.

### 2D Matrix Copy Mode Constraints

- When m_step or k_step is 0, no copy is performed, and this interface is treated as a NOP (no operation).
- The interface uses a complete 512-byte fractal as the minimum copy granularity. It does not support copying only part of the elements within a fractal, and it does not support padding.
- The value range of dst_stride is [0, 4095]. Out-of-range values are clipped, causing the copy result to be unexpected.
- The value range of m_step is [1, 4095]. Out-of-range values are clipped, causing the copy result to be unexpected.
- The value range of k_step is [1, 4095]. Out-of-range values are clipped, causing the copy result to be unexpected.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

In the sample, the shapes of matrices A, B, and C are all [128,128], and the data format is Nz. A and B are stored in GM in Nz format and are copied to the L1 Buffer by repeatedly calling the 2D matrix copy overload of this interface in units of 8 fractal columns, copying one fractal column at a time. Then A is copied to the L0A Buffer via [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md), B is copied to the L0B Buffer via [asc_copy_l12l0b_transpose](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md), and [asc_mmad](../cube_compute/mmad_compute/asc_mmad.md) is called to complete the computation. When C is copied out from the L0C Buffer to GM, Nz2ND and Nz2DN are disabled to keep the Nz format. In the sample, a single core occupies 64KB of L1 Buffer, 32KB of L0A Buffer, 32KB of L0B Buffer, and 64KB of L0C Buffer. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

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
        asc_copy_gm2l1(a_l1 + offset, a + offset, 0, 0, M_FRACTALS, M_FRACTALS, 1, 0, 0);
    }
    for (uint16_t n_block = 0; n_block < N_FRACTALS; ++n_block) {
        const uint32_t offset = n_block * K * CUBE;
        asc_copy_gm2l1(b_l1 + offset, b + offset, 0, 0, K_FRACTALS, K_FRACTALS, 1, 0, 0);
    }
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // A is converted from Nz to the Zz arrangement required by the L0A Buffer, and B is converted from Nz to the Zn arrangement required by the L0B Buffer.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M_FRACTALS, K_FRACTALS, M_FRACTALS, M_FRACTALS);
    asc_copy_l12l0b_transpose(
        b_l0, b_l1, 0, 0, K_FRACTALS, N_FRACTALS, K_FRACTALS, N_FRACTALS);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Disable Nz2ND and Nz2DN to keep the Nz layout in the L0C Buffer and copy it directly to GM.
    asc_copy_l0c2gm(output, c_l0, N, M, M * CUBE, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, false,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
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
    std::cout << "[Success] asc_copy_gm2l1 2D Nz matrix multiplication passed." << std::endl;
    return 0;
}
```
