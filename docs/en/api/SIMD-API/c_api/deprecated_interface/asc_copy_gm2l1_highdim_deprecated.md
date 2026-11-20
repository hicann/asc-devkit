---
okftype: reference.api
description: "Deprecated high-dimensional split movement legacy overload. Moves GM data to the L1 Buffer by data block count and length, and inserts or removes 32-byte granularity padding through the uint8 pad_func_mode. The replacement uses the asc_channel_pad_mode enum overload."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [high_dimensional, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_padding.md
    kind: constrained_by
    condition: "When pad_func_mode is fill mode, the padding value must be configured first"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_channel_pad_mode.md
    kind: related
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The old high-dimensional split overload using uint8_t pad_func_mode and the corresponding _sync overload are deprecated. Use asc_copy_gm2l1_highdim_split_arch_3510 with asc_channel_pad_mode pad_mode instead. Both declarations, include/c_api/cube_datamove/cube_datamove.h (asc_copy_gm2l1) and include/c_api/composite/cube_datamove_composite.h (asc_copy_gm2l1_sync), carry ASC_DEPRECATED(9.2.0)."
---
# asc_copy_gm2l1 (NPU architecture version 3510, high-dimensional split data movement, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:33:25.069Z pushedAt=2026-11-18T03:12:16.389Z nl=2 -->

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

**The old regular overload of `asc_copy_gm2l1` using `uint8_t pad_func_mode` and its `asc_copy_gm2l1_sync` interface are deprecated. Use the [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) interface with the `asc_channel_pad_mode pad_mode` parameter instead.**

The header file paths are: `"c_api/cube_datamove/cube_datamove.h"`, `"c_api/composite/cube_datamove_composite.h"`.

Moves data from Global Memory to the L1 Buffer, keeping the format and content unchanged during data movement. High-dimensional split means that non-contiguous movement can be achieved by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset. In addition, this interface supports inserting or removing padding for 32-byte granularity data in channel padding mode during the movement. The padding value needs to be set through [asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md).

This interface is valid only when executed on the AIC.

## Prototype

### High-Dimensional Split Data Movement

```c
__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst,
                                      uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride,
                                      uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst,
                                           uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride,
                                           uint32_t dst_stride)
```

## Parameters

### High-Dimensional Split Data Movement

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand, stored in the L1 Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, stored in GM. The start address must be 1-byte aligned. |
| n_burst | Input | Number of data blocks to move. Value range: [1, $2^{17}-1$]. |
| len_burst | Input | Length of each data block to move, in 32 bytes. Value range: [1, $2^{17}-1$]. |
| pad_func_mode | Input | Fill or removal data mode during the movement. Value range: [0, 8]. Fill mode means inserting padding values into the destination data. The padding values must be set through [asc_set_gm2l1_padding](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_padding.md). Removal mode means removing part of the data from each 32 bytes being moved.<br>&nbsp;&nbsp;&bull; `0`: No padding;<br>&nbsp;&nbsp;&bull; `1`: For every 1 byte of data, insert 31 padding values, where the bit width of each inserted padding is b8;<br>&nbsp;&nbsp;&bull; `2`: For every 2 bytes of data, insert 15 padding values, where the bit width of each inserted padding is b16;<br>&nbsp;&nbsp;&bull; `3`: For every 4 bytes of data, insert 14 padding values, where the bit width of each inserted padding is b16;<br>&nbsp;&nbsp;&bull; `4`: For every 8 bytes of data, insert 12 padding values, where the bit width of each inserted padding is b16; <br>&nbsp;&nbsp;&bull; `5`: For every 16 bytes of data, insert 8 padding values, where the bit width of each inserted padding is b16;<br>&nbsp;&nbsp;&bull; `6`: For every 32 bytes of data, remove the 28 most significant bits of data and keep only the 4 least significant bytes;<br>&nbsp;&nbsp;&bull; `7`: For every 32 bytes of data, remove the 24 most significant bits of data and keep only the 8 least significant bytes;<br>&nbsp;&nbsp;&bull; `8`: For every 32 bytes of data, remove the 16 most significant bits of data and keep only the 16 least significant bytes. |
| src_stride | Input | Interval between adjacent data blocks of the source operand (that is, the difference between the start address of the previous data block and the start address of the next data block). Value range: [0, $2^{36}-1$], in 32 bytes.<br>&nbsp;&nbsp;&bull; When n_burst = 1, `src_stride` is meaningless and can be set to 0. |
| dst_stride | Input | Interval between adjacent data blocks of the destination operand (that is, the difference between the start address of the previous data block and the start address of the next data block). Value range: [0, $2^{17}-1$], in 32 bytes.<br>&nbsp;&nbsp;&bull; When n_burst = 1, `dst_stride` is meaningless and can be set to 0. |

- For the scenario where `pad_func_mode` is set to fill data mode:
  - In fill mode, the length of each data block to move, `len_burst`, must be set to `1`. In each fill mode, the length of each data block after filling is 32 bytes.
  - In fill mode, moving one data block reads only 1/2/4/8/16 bytes of data from the source operand GM, corresponding to the 5 fill modes in sequence.
  - Each data block movement reads continuously from the source operand GM, so `src_stride` can be set to `0`.
- For the scenario where `pad_func_mode` is set to removal data mode:
  - For one data block movement in the source operand, the data read is `len_burst * 32` bytes, and the actual data written to the destination operand is `len_burst * 4/8/16` bytes, corresponding to the 3 removal modes in sequence.
  - Each data block movement writes continuously to the destination operand L1 Buffer, so `dst_stride` can be set to `0`.

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- This API takes effect only on the AIC. Calls from non-AIC contexts return directly.
- The `dst` start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise it will cause a movement exception.
- The `src` start address must be 1-byte aligned (Global Memory alignment requirement); otherwise it will cause a movement exception.
- If this instruction overlaps with other instructions in destination addresses, insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512KB. The sum of the dst offset and the movement size must not exceed the boundary; otherwise an exception is triggered.

### High-Dimensional Split Data Movement Constraints

- The value range of `n_burst` is [1, $2^{17}-1$]. Values outside this range are clipped, causing the movement result to be inconsistent with expectations.
- The value range of `len_burst` is [1, $2^{17}-1$]. Values outside this range are clipped, causing the movement result to be inconsistent with expectations.
- The value range of `src_stride` is [0, $2^{36}-1$]. Values outside this range are clipped, causing the movement result to be inconsistent with expectations.
- The value range of `dst_stride` is [0, $2^{17}-1$]. Values outside this range are clipped, causing the movement result to be inconsistent with expectations.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

In the sample, the shapes of matrices A, B, and C are all [128,128], and the shape of the Bias matrix is [1,128]. Bias is moved from GM to the L1 Buffer in two 256-byte blocks through the high-dimensional split movement interface, then moved to the BiasTable Buffer by calling [asc_copy_l12bt](../cube_datamove/cube_compute_load/asc_copy_l12bt_arch_3510.md), and finally MMAD completes the `A × B + Bias` computation. In the sample, a single core occupies 64.5KB L1 Buffer, 32KB L0A Buffer, 32KB L0B Buffer, 64KB L0C Buffer, and 512 bytes BiasTable Buffer. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

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
constexpr uint32_t A_ELEMENTS = M * K;
constexpr uint32_t B_ELEMENTS = N * K;
constexpr uint32_t C_ELEMENTS = M * N;
constexpr uint32_t BIAS_ELEMENTS = N;
constexpr uint32_t CUBE = 16;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00;
constexpr uint32_t BIAS_BLOCK_BYTES = 256;
constexpr uint32_t BIAS_BLOCKS = 2;
constexpr uint32_t BIAS_BLOCK_LEN = BIAS_BLOCK_BYTES / 32;

__global__ __cube__ void AscCopyGm2l1HighdimBiasKernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ float* bias, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[A_ELEMENTS], b_l1[B_ELEMENTS];
    __cbuf__ float bias_l1[BIAS_ELEMENTS];
    __ca__ half a_l0[A_ELEMENTS];
    __cb__ half b_l0[B_ELEMENTS];
    __cc__ float c_l0[C_ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), K * sizeof(half), 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), K * sizeof(half), 0, N, K, 0, false);

    // Split the 1 x 128 data of Bias into two 256-byte blocks and move them from GM to the L1 Buffer. Alternatively, move a 512-byte block in one continuous operation.
    asc_copy_gm2l1(reinterpret_cast<__cbuf__ void*>(bias_l1), reinterpret_cast<__gm__ void*>(bias), BIAS_BLOCKS,
        BIAS_BLOCK_LEN, 0, BIAS_BLOCK_LEN, BIAS_BLOCK_LEN);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE, K / CUBE, M / CUBE, M / CUBE);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE, K / CUBE, N / CUBE, N / CUBE);
    asc_copy_l12bt(0, bias_l1, BIAS_ELEMENTS * sizeof(float));
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Read Bias from the BiasTable Buffer to initialize the C matrix, then perform the A x B + Bias computation.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, false, true, false);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync();
}

uint16_t ToHalfBits(int32_t value)
{
    return value > 0 ? HALF_ONE : (value < 0 ? HALF_NEG_ONE : 0);
}

void PrintRow(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(A_ELEMENTS), b(B_ELEMENTS);
    std::vector<float> bias(BIAS_ELEMENTS), output(C_ELEMENTS), golden(C_ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
            a[row * K + k] = ToHalfBits(value);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((2 * k + col) % 3) - 1;
            b[col * K + k] = ToHalfBits(value);
        }
        bias[col] = static_cast<float>(static_cast<int32_t>(col % 5) - 2);
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            int32_t sum = 0;
            for (uint32_t k = 0; k < K; ++k) {
                const int32_t a_value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
                const int32_t b_value = static_cast<int32_t>((2 * k + col) % 3) - 1;
                sum += a_value * b_value;
            }
            golden[row * N + col] = static_cast<float>(sum) + bias[col];
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    float *bias_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&bias_device), bias.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(bias_device, bias.size() * sizeof(float), bias.data(), bias.size() * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyGm2l1HighdimBiasKernel<<<1, 0>>>(a_device, b_device, bias_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Bias", bias);
    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_gm2l1 high-dimensional bias copy passed."
                         : "[Failed] asc_copy_gm2l1 high-dimensional bias copy mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(bias_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
