---
okftype: reference.api
description: "Deprecated API page: Under the 3510 architecture, asc_copy_l12l0b_mx_sync moves the MX right quantization coefficient in the L1 Buffer to the L0B_MX Buffer in units of 32-byte fractals, with the fractal fixed at 2x16. It must be used together with asc_copy_l12l0b. It is recommended to use the non-synchronous interface plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [datablock_granularity, quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md
    kind: requires
    condition: "Must be used together with the right matrix data movement interface so that the quantization coefficient and data are written to mutually mapped addresses"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous interface and explicitly call asc_sync instead"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "Insert a synchronization instruction for serialization when the destination address overlaps with other instructions"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked with ASC_DEPRECATED(9.2.0) before the asc_copy_l12l0b_mx_sync declaration. The replacement path is the non-synchronous asc_copy_l12l0b_mx plus asc_sync; the interface is not removed."
---
# asc_copy_l12l0b_mx_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:19:00.452Z pushedAt=2026-11-18T03:12:16.434Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12l0b_mx_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

It is responsible for moving the quantization coefficients corresponding to the right matrix required during MX matrix computation. The data path is L1 Buffer -> L0B_MX Buffer. The right quantization coefficient matrix is moved in units of 32-byte data fractals (with a fixed data type of `fp8_e8m0_t` and a fractal size of 2x16).

The size of the L0B_MX Buffer is 4KB, and its address mapping relationship with the L0B Buffer is as follows:

$$
L0B\_MX\ Buffer\ Address = L0B\ Buffer\ Address / 16
$$

This interface is the quantization coefficient movement interface for MX matrix computation and must be used together with the corresponding data movement interface [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md).

This interface is valid only when executed on the AIC.

## Prototype

```cpp

__aicore__ inline void asc_copy_l12l0b_mx_sync(uint64_t dst,
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

| Parameter | Input/Output | meaning |
| --- | --- | --- |
| dst | Output | Destination operand, stored in the L0B_MX Buffer. The start address must be 32-byte aligned. |
| src | Input | Source operand, stored in the L1 Buffer. The start address must be 32-byte aligned. |
| x_start_pos | Input | Start fractal position of the source matrix on the X axis. unit: 32 bytes, that is, one 2x16 fractal. |
| y_start_pos | Input | Start fractal position of the source matrix on the Y axis. unit: 32 bytes, corresponding to the basic addressing granularity of the fractal. |
| x_step | Input | Movement length of the source matrix along the X axis. unit: 32 bytes, that is, one 2x16 fractal. value range: [1, 255]. |
| y_step | Input | Movement length of the source matrix along the Y axis. unit: 32 bytes, corresponding to the basic addressing granularity of the fractal. value range: [1, 255]. |
| src_stride | Input | Interval between the start addresses of adjacent fractals of the source matrix along the X axis. unit: 32 bytes, that is, one 2x16 fractal. |
| dst_stride | Input | Interval between the start addresses of adjacent fractals of the destination matrix along the X axis. unit: 32 bytes, that is, one 2x16 fractal. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This interface returns directly when it is not an AIC call.
- `src` is located in L1 Buffer, and dst is located in L0B_MX Buffer. They are in different physical storage units, so there is no address overlap.
- If this instruction has destination address overlap with other instructions, you need to insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to ensure that multiple instructions are serialized and prevent abnormal data.
- L0B_MX Buffer capacity limit: The total capacity of L0B_MX Buffer is 4KB. The sum of the `dst` offset and the transfer size must not exceed the boundary, otherwise an address overflow exception is triggered.
- L1 Buffer capacity limit: The total capacity of L1 Buffer is 512KB. The sum of the `src` offset and the size occupied by the source matrix must not exceed the boundary, otherwise an address overflow exception is triggered.

### Copy Constraints

- This API must be used together with the corresponding data copy API [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) so that the quantization coefficients and the right matrix data are written to the mutually mapped L0B_MX Buffer and L0B Buffer addresses.
- When `x_step` or `y_step` is set to 0, this API is treated as a NOP (no operation). 0 is not within the valid value range and should not be set to 0 in valid tasks, to avoid generating invalid instructions and affecting performance.
- The minimum copy granularity of the quantization coefficient matrix is one 2×16 fractal. When the matrix edge is less than one fractal, the source matrix must be padded to a complete fractal.
- The fractal of the quantization coefficient matrix is fixed at 2×16, and the corresponding fractal of the L0B Buffer is 32×16, occupying 1/16 of the L0B Buffer address. dst and dst_stride must be set according to the address mapping relationship, otherwise a copy exception is triggered.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
The NPU architecture corresponding to the Ascend 950PR&950DT products is `dav-3510`. The compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The sample occupies 33KB L1 Buffer, 16KB L0A Buffer, 16KB L0B Buffer, and 64KB L0C Buffer on a single core, and occupies 512 bytes of L0A_MX Buffer and L0B_MX Buffer respectively. The sample sequentially reuses `EVENT_ID0` to complete the synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

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

__global__ __cube__ void AscCopyL12l0bMxKernel(__gm__ uint8_t* a, __gm__ uint8_t* scale_a,
    __gm__ uint8_t* b, __gm__ uint8_t* scale_b, __gm__ float* output)
{
    asc_init();
    __cbuf__ fp8_e4m3fn_t a_l1[M * K], b_l1[N * K];
    __cbuf__ fp8_e8m0_t scale_a_l1[M * SCALE_K], scale_b_l1[N * SCALE_K];
    __ca__ fp8_e4m3fn_t a_l0[M * K];
    __cb__ fp8_e4m3fn_t b_l0[N * K];
    __cc__ float c_l0[M * N];

    // Copy the matrix and quantization coefficients from GM to the L1 Buffer, and convert them to the corresponding Nz format.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(a), K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_a_l1),
        reinterpret_cast<__gm__ half*>(scale_a), SCALE_K, 0, SCALE_K / 2, M, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(b), K, 0, N, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_b_l1),
        reinterpret_cast<__gm__ half*>(scale_b), SCALE_K, 0, SCALE_K / 2, N, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Copy the left and right matrices and their quantization coefficients to the L0A, L0A_MX, L0B, and L0B_MX Buffers respectively.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0a_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(a_l0)) / 16,
        scale_a_l1, 0, 0, M / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_copy_l12l0b_mx_sync(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(b_l0)) / 16,
        scale_b_l1, 0, 0, N / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Use the MX quantization coefficient to perform matrix multiplication with M=N=K=128.
    asc_mmad_mx(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in the L0C Buffer to ND format and move it out to GM.
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
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
    // Construct the FP8 input and quantization coefficient, and compute the Golden result on the Host side.
    std::vector<uint8_t> a(M * K), b(N * K);
    std::vector<uint8_t> scale_a(M * SCALE_K, 127), scale_b(N * SCALE_K);
    std::vector<float> output(M * N), golden(M * N);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = ((row + 2 * k) % 5 < 2) ? FP8_ONE : FP8_NEG_ONE;
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = ((3 * col + k) % 7 < 3) ? FP8_ONE : FP8_NEG_ONE;
        }
        for (uint32_t block = 0; block < SCALE_K; ++block) {
            scale_b[col * SCALE_K + block] = 126 + (col + block) % 3;
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                const float a_value = a[row * K + k] == FP8_ONE ? 1.0f : -1.0f;
                const float b_value = (b[col * K + k] == FP8_ONE ? 1.0f : -1.0f) *
                    DecodeScale(scale_b[col * SCALE_K + k / 32]);
                golden[row * N + col] += a_value * b_value;
            }
        }
    }

    // Allocate Device memory, execute the kernel function, and retrieve the result.
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
    AscCopyL12l0bMxKernel<<<1, 0>>>(a_device, scale_a_device, b_device, scale_b_device, output_device);
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
    std::cout << (passed ? "[Success] asc_copy_l12l0b_mx result is correct."
                         : "[Failed] asc_copy_l12l0b_mx result mismatch.") << std::endl;

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
