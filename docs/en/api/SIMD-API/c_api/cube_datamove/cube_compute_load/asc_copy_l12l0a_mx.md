---
okftype: reference.api
description: "Copies the left cube quantization coefficients (fp8_e8m0_t) required for cube computation from the L1 Buffer to the L0A_MX Buffer in 32-byte fractals. It must be used together with the left cube 2D copy to ensure consistent address mapping."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [datablock_granularity, quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad_mx.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md
    kind: related
    condition: "Must be used together with the left cube data copy"
version: ">=9.0.0"
version_changes: []
---
# asc_copy_l12l0a_mx

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:45:08.716Z pushedAt=2026-11-18T08:15:34.272Z nl=2 -->

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

It is responsible for copying the quantization coefficients corresponding to the left cube required during MX cube computation. The data path is L1 Buffer -> L0A_MX Buffer. The left quantization coefficient cube is copied in units of 32-byte data fractals (with a fixed data type of `fp8_e8m0_t` and a fractal size of 16×2).

The size of the L0A_MX Buffer is 4KB, and its address mapping relationship with the L0A Buffer address is as follows:

$$
L0A\_MX\ Buffer\ Address = L0A\ Buffer\ Address / 16
$$

This interface is the quantization coefficient copy interface for MX cube computation and must be used together with the corresponding data copy interface [asc_copy_l12l0a](asc_copy_l12l0a_2d_arch_3510.md).

This interface is valid only when executed on the AIC.

For the data layout before copying, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md). For address alignment and copy granularity requirements, see [Cube Computation Input Copy Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this interface.

For the fractal layout of the Scale cube, see [Auxiliary Cube Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/aux_cube_fractal_format_details.md). After preparing the data cube and the corresponding Scale cube, use [asc_mmad_mx](../../cube_compute/mmad_compute/asc_mmad_mx.md) to complete the computation.

## Prototype

```cpp
__aicore__ inline void asc_copy_l12l0a_mx(uint64_t dst,
                                          __cbuf__ fp8_e8m0_t* src,
                                          uint16_t x_start_pos,
                                          uint16_t y_start_pos,
                                          uint8_t x_step,
                                          uint8_t y_step,
                                          uint16_t src_stride,
                                          uint16_t dst_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination operand, stored in the L0A_MX Buffer. The start address must be 32-byte aligned. Value range: $[0, 2^{64}-1]$. |
| src | Input | Source operand, stored in the L1 Buffer, with a fixed data type of `fp8_e8m0_t`. The start address must be 32-byte aligned. |
| x_start_pos | Input | Start fractal position of the source cube on the X axis, in units of 32 bytes, that is, 1 fractal. Value range: $[0, 2^{16}-1]$. |
| y_start_pos | Input | Start fractal position of the source cube on the Y axis, in units of 32 bytes. Value range: $[0, 2^{16}-1]$. |
| x_step | Input | Copy length of the source cube along the X axis, in units of 1 fractal, that is, 32 bytes. Value range: $[0, 255]$. |
| y_step | Input | Copy length of the source cube along the Y axis, in units of 32 bytes. Value range: $[0, 255]$. |
| src_stride | Input | Interval between the start addresses of adjacent fractals of the source cube along the X axis, in units of 32 bytes. Value range: $[0, 2^{16}-1]$. |
| dst_stride | Input | Interval between the start addresses of adjacent fractals of the destination cube along the X axis, in units of 32 bytes. Value range: $[0, 2^{16}-1]$. |

Assume that the shape of the left cube A is $(M, K)$, then the shape of the ScaleA cube is $(M, K/32)$. The data type of ScaleA is `fp8_e8m0_t`, and its fractal layout in the L0A_MX Buffer is shown in [Figure 1](#l12l0a_mx_layout).

**Figure 1**  Fractal layout of ScaleA in the L0A_MX Buffer<a id="l12l0a_mx_layout"></a>

![Fractal layout of ScaleA in the L0A_MX Buffer](../../../../figures/scalea_l0a_fractal.png "Fractal layout of ScaleA in the L0A_MX Buffer")

[Figure 2](#fig_l12l0a_mx_params) shows the configuration parameters during the process of copying ScaleA from the L1 Buffer to the L0A_MX Buffer. Each row is 32 bytes, corresponding to one fractal in [Figure 1](#l12l0a_mx_layout). `x_step` is the number of fractals in the M dimension, which is $x\_step=M/16=3$ in the example; `y_step` is the number of 32-byte data blocks in the K dimension, which is $y\_step=K/32/2=21$ in the example; `src_stride` and `dst_stride` indicate the interval between the start addresses of adjacent data blocks in the K dimension, in units of 32 bytes.

**Figure 2**  Configuration parameters for copying ScaleA from the L1 Buffer to the L0A_MX Buffer<a id="fig_l12l0a_mx_params"></a>

![Configuration parameters for copying ScaleA from the L1 Buffer to the L0A_MX Buffer](../../../../figures/LoadData_2D_MX_nd2nz.png "Configuration parameters for copying ScaleA from the L1 Buffer to the L0A_MX Buffer")

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- If this interface is not called by AIC, it returns directly.
- The `dst` start address must be aligned to 32 bytes; otherwise, an address alignment exception is triggered.
- The `src` start address must be aligned to 32 bytes (L1 Buffer alignment requirement); otherwise, an address alignment exception is triggered.
- `src` is located in the L1 Buffer, and `dst` points to the L0A_MX Buffer. The two belong to different physical storage units, so there is no scenario where the source operand and destination operand addresses overlap.
- If this instruction has data dependencies with other instructions, a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) must be inserted to ensure that multiple instructions are serialized and to prevent abnormal data.
- L0A_MX Buffer capacity limit: The total capacity of the L0A_MX Buffer is 4KB. The sum of the `dst` offset and the copy size must not exceed the boundary; otherwise, an address overflow exception is triggered.
- L1 Buffer capacity limit: The total capacity of the L1 Buffer is 512KB. The sum of the `src` offset and the size occupied by the source cube must not exceed the boundary; otherwise, an address overflow exception is triggered.

### Copy Constraints

- This API must be used together with the corresponding data copy API [asc_copy_l12l0a](asc_copy_l12l0a_2d_arch_3510.md), so that the quantization coefficients and the left cube data are written to the mutually mapped L0A_MX Buffer and L0A Buffer addresses.
- When `x_step` or `y_step` is set to 0, this API is treated as a NOP (no operation). Avoid passing 0 unintentionally; otherwise, no valid data is produced and instruction scheduling overhead is increased.
- The fractal of the quantization coefficient cube is fixed at $(16, 2)$, and the corresponding fractal of the L0A Buffer is $(16, 32)$, occupying $1/16$ of the L0A Buffer address space. The API uses a complete 32-byte fractal as the minimum copy granularity and does not support copying only part of the elements within a fractal. Invalid elements in the fractal must be filled before the call according to the subsequent cube computation requirements. `dst` and `dst_stride` must be set according to the address constraints; otherwise, a copy exception is triggered.

## Examples

For the complete project sample in the repository, see the [mmad_mx sample](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/mmad_mx).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
The NPU architecture corresponding to the Ascend 950PR&950DT products is `dav-3510`. The compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t SCALE_K = K / 32;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;
constexpr uint8_t FP8_ONE = 0x38;
constexpr uint8_t FP8_NEG_ONE = 0xB8;

__global__ __cube__ void AscCopyL12l0aMxKernel(__gm__ uint8_t* a, __gm__ uint8_t* scale_a,
    __gm__ uint8_t* b, __gm__ uint8_t* scale_b, __gm__ float* output)
{
    asc_init();
    __cbuf__ fp8_e4m3fn_t a_l1[M * K], b_l1[N * K];
    __cbuf__ fp8_e8m0_t scale_a_l1[M * SCALE_K], scale_b_l1[N * SCALE_K];
    __ca__ fp8_e4m3fn_t a_l0[M * K];
    __cb__ fp8_e4m3fn_t b_l0[N * K];
    __cc__ float c_l0[M * N];

    // Configure the Nz format of the 128 x 128 cube and its quantization coefficient cube.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(a), K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_a_l1),
        reinterpret_cast<__gm__ half*>(scale_a), SCALE_K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, SCALE_K / 2, M, 0,
        false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(b), K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_b_l1),
        reinterpret_cast<__gm__ half*>(scale_b), SCALE_K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, SCALE_K / 2, N, 0,
        false);
    // Wait until cube and scale transfers finish before PIPE_MTE1 reads L1 Buffer.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move A and ScaleA to their mapped L0A Buffer and L0A_MX Buffer addresses.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0a_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(a_l0)) / 16,
        scale_a_l1, 0, 0, M / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    // Move B and ScaleB to L0B Buffer and L0B_MX Buffer to provide the second MX operand.
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_copy_l12l0b_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(b_l0)) / 16,
        scale_b_l1, 0, 0, N / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Run the 128x128x128 MX cube multiplication after all L0A Buffer and L0B Buffer operands are ready.
    asc_mmad_mx(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in L0C to ND format and copy it back to GM.
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_FIX);
}

float DecodeScale(uint8_t value)
{
    return std::ldexp(1.0f, static_cast<int32_t>(value) - 127);
}

void PrintRow(const char* label, const std::vector<float>& data)
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
    // Generate deterministic FP8 inputs, nonuniform ScaleA, and the host-side golden result.
    std::vector<uint8_t> a(M * K), b(N * K);
    std::vector<uint8_t> scale_a(M * SCALE_K), scale_b(N * SCALE_K, 127);
    std::vector<float> output(M * N), golden(M * N);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = ((row + 2 * k) % 5 < 2) ? FP8_ONE : FP8_NEG_ONE;
        }
        for (uint32_t block = 0; block < SCALE_K; ++block) {
            scale_a[row * SCALE_K + block] = 126 + (row + block) % 3;
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = ((3 * col + k) % 7 < 3) ? FP8_ONE : FP8_NEG_ONE;
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                const float a_value = (a[row * K + k] == FP8_ONE ? 1.0f : -1.0f) *
                    DecodeScale(scale_a[row * SCALE_K + k / 32]);
                const float b_value = b[col * K + k] == FP8_ONE ? 1.0f : -1.0f;
                golden[row * N + col] += a_value * b_value;
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *a_device = nullptr, *b_device = nullptr;
    uint8_t *scale_a_device = nullptr, *scale_b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scale_a_device), scale_a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scale_b_device), scale_b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scale_a_device, scale_a.size(), scale_a.data(), scale_a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scale_b_device, scale_b.size(), scale_b.data(), scale_b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyL12l0aMxKernel<<<1, 0>>>(a_device, scale_a_device, b_device, scale_b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    bool passed = true;
    for (uint32_t i = 0; i < output.size(); ++i) {
        if (std::fabs(output[i] - golden[i]) > 1e-5f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_copy_l12l0a_mx result is correct."
                         : "[Failed] asc_copy_l12l0a_mx result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(scale_a_device);
    aclrtFree(scale_b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
