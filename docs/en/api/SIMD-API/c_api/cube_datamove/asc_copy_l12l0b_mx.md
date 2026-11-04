# asc_copy_l12l0b_mx

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-22T13:08:19.936Z pushedAt=2026-10-28T12:45:45.443Z nl=2 -->

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
- Atlas inference product AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference product Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Copies the quantization coefficient corresponding to the right cube required during MX cube computation. The data path is L1 Buffer -> L0B_MX Buffer. The right quantization coefficient cube is copied in units of 32-byte data fractals (with a fixed data type of `fp8_e8m0_t` and a fractal size of 2×16).

The L0B_MX Buffer is 4KB in size, and its address mapping relationship with the L0B Buffer address is as follows:

$$
L0B\_MX\ Buffer\ Address = L0B\ Buffer\ Address / 16
$$

This API copies the quantization coefficient for MX cube computation and must be used together with the corresponding data copy API [asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md).

This API takes effect only when executed on the AIC.

## Prototype

```cpp
__aicore__ inline void asc_copy_l12l0b_mx(uint64_t dst,
                                          __cbuf__ fp8_e8m0_t* src,
                                          uint16_t x_start_pos,
                                          uint16_t y_start_pos,
                                          uint8_t x_step,
                                          uint8_t y_step,
                                          uint16_t src_stride,
                                          uint16_t dst_stride)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand, stored in the L0B_MX Buffer. The start address must be 32-byte aligned. |
| src | Input | Source operand, stored in the L1 Buffer. The start address must be 32-byte aligned. |
| x_start_pos | Input | Start fractal position of the source cube on the X axis. The unit is 32 bytes, that is, one 2×16 fractal. |
| y_start_pos | Input | Start fractal position of the source cube on the Y axis. The unit is 32 bytes, corresponding to the basic addressing granularity of the fractal. |
| x_step | Input | Copy length of the source cube along the X axis. The unit is 32 bytes, that is, one 2×16 fractal. Value range: [1, 255]. |
| y_step | Input | Copy length of the source cube along the Y axis. The unit is 32 bytes, corresponding to the basic addressing granularity of the fractal. Value range: [1, 255]. |
| src_stride | Input | Interval between the start addresses of adjacent fractals of the source cube along the X axis. The unit is 32 bytes, that is, one 2×16 fractal. |
| dst_stride | Input | Interval between the start addresses of adjacent fractals of the destination cube along the X axis. The unit is 32 bytes, that is, one 2×16 fractal. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This API returns directly when it is not called by the AIC.
- `src` is located in the L1 Buffer, and dst is located in the L0B_MX Buffer. They are located in different physical storage units, so there is no address overlap.
- If the destination address of this instruction overlaps with that of other instructions, insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to serialize the instructions and prevent abnormal data.
- L0B_MX Buffer capacity limit: The total L0B_MX Buffer capacity is 4 KB. The sum of the `dst` offset and the copy size must not exceed the boundary; otherwise, an address overflow exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the `src` offset and the size occupied by the source cube must not exceed the boundary; otherwise, an address overflow exception is triggered.

### Transfer Constraints

- This API must be used together with the corresponding data transfer API [asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) so that the quantization coefficient and the right cube data are written to the mutually mapped L0B_MX Buffer and L0B Buffer addresses.
- When `x_step` or `y_step` is set to 0, this API is treated as a NOP (no operation). 0 is not within the valid value range and should not be set to 0 in a valid task, to avoid generating invalid instructions and affecting performance.
- The minimum transfer granularity of the quantization coefficient cube is one 2×16 fractal. When the cube edge is smaller than one fractal, the source cube must be padded to a complete fractal.
- The fractal of the quantization coefficient cube is fixed at 2×16, and the corresponding fractal of the L0B Buffer is 32×16, occupying 1/16 of the L0B Buffer address. dst and dst_stride must be set according to the address mapping relationship; otherwise trigger a transfer exception.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command. The `--npu-arch` parameter must be set to the NPU architecture corresponding to the actual product model. For the mapping between products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
The NPU architecture corresponding to the Ascend 950PR&950DT products is `dav-3510`. The compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The sample occupies 33KB of L1 Buffer, 16KB of L0A Buffer, 16KB of L0B Buffer, and 64KB of L0C Buffer on a single core, and occupies 512 bytes of L0A_MX Buffer and L0B_MX Buffer respectively. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, from MTE1 to M, and from M to FIX.

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

    // Move the cube and quantization coefficient from GM to the L1 Buffer, and convert them to the corresponding Nz format.
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
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move the left and right cubes and their quantization coefficients to the L0A, L0A_MX, L0B, and L0B_MX Buffers respectively.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0a_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(a_l0)) / 16,
        scale_a_l1, 0, 0, M / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_copy_l12l0b_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(b_l0)) / 16,
        scale_b_l1, 0, 0, N / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Perform cube multiplication with M=N=K=128 using the MX quantization coefficient.
    asc_mmad_mx(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in the L0C Buffer to ND format and move it out to GM.
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
    // Construct FP8 inputs and quantization coefficients, and compute the golden result on the host side.
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

    // Allocate device memory, execute the kernel, and retrieve the result.
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
