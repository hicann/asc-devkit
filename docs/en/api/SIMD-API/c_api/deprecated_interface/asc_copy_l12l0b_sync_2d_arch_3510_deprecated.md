---
okftype: reference.api
description: "Deprecated API page: On the 3510 architecture, asc_copy_l12l0b_sync and asc_copy_l12l0b_transpose_sync copy a 2D matrix from L1 Buffer to L0B Buffer in units of 512-byte fractals, supporting transpose and non-transpose as well as b4, b8, b16, and b32. It is recommended to use the asynchronous interface plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [load, transpose]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding asynchronous 2D interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, a synchronization instruction must be inserted to serialize them"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, insert synchronization instructions to serialize them"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked with ASC_DEPRECATED(9.2.0) before the asc_copy_l12l0b_sync declaration. The replacement path is the non-synchronous 2D interface plus asc_sync; the interface has not been removed."
---
# asc_copy_l12l0b_sync (NPU Architecture Version 3510, 2D Matrix Copy, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:22:13.104Z pushedAt=2026-11-18T03:12:16.435Z nl=2 -->

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

**The `asc_copy_l12l0b_sync` and `asc_copy_l12l0b_transpose_sync` interfaces are deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

It is responsible for copying the 2D format data required for matrix computation. The copy granularity is a data fractal of 512 bytes, and it supports copying both non-transposed and transposed matrices from the L1 Buffer to the L0B Buffer.

For different data types, the corresponding fractal matrices are as follows:

- For the b4 data type, each data fractal is a 64x16 matrix in the L0B Buffer.
- For the b8 data type, each data fractal is a 32x16 matrix in the L0B Buffer.
- For the b16 data type, each data fractal is a 16x16 matrix.
- For the b32 data type, each data fractal is an 8x16 matrix in the L0B Buffer.

This interface is valid only when executed on the AIC.

## Prototype

### 2D Copy Mode

```cpp
__aicore__ inline void asc_copy_l12l0b_sync(__cb__ <dtype>* dst,
                                            __cbuf__ <dtype>* src,
                                            uint16_t m_start_position,
                                            uint16_t k_start_position,
                                            uint8_t m_step,
                                            uint8_t k_step,
                                            int16_t src_stride,
                                            uint16_t dst_stride)
```

#### Data Types Supported by dtype

`dtype` values are: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### 2D Transpose Copy Mode

```cpp
__aicore__ inline void asc_copy_l12l0b_transpose_sync(__cb__ <dtype>* dst,
                                                      __cbuf__ <dtype>* src,
                                                      uint16_t m_start_position,
                                                      uint16_t k_start_position,
                                                      uint8_t m_step,
                                                      uint8_t k_step,
                                                      int16_t src_stride,
                                                      uint16_t dst_stride)
```

#### Data Types Supported by dtype

`dtype` values are: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination matrix, stored in the L0B Buffer. The start address must be aligned to 512 bytes. |
| src | Input | Source matrix, stored in the L1 Buffer. The start address must be aligned to 32 bytes. |
| m_start_position | Input | Start fractal position of the source matrix in the row direction. Unit: 16 elements, corresponding to the fixed width of the fractal row direction. Value Range: [0, $2^{16}-1$]. |
| k_start_position | Input | Start fractal position of the source matrix in the column direction. Unit: 32 bytes, corresponding to the basic addressing granularity of the fractal column direction. Value Range: [0, $2^{16}-1$]. |
| m_step | Input | Number of fractal rows copied in the source matrix row direction. Unit: 16 elements, corresponding to the fixed width of the fractal row direction. Value Range: [0, 255].<br>In the transpose scenario (calling `asc_copy_l12l0b_transpose`), in addition to satisfying the value range, m_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, m_step must be a multiple of 4;<br>&nbsp;&nbsp;&bull;When the data type is b8, m_step must be a multiple of 2;<br>&nbsp;&nbsp;&bull;When the data type is b16, m_step must be a multiple of 1;<br>&nbsp;&nbsp;&bull;When the data type is b32, m_step has no additional constraint.<br>**Note: m_step=0 means no copy is performed, and the interface is treated as a NOP (no-op).** |
| k_step | Input | Number of fractal columns copied in the source matrix column direction. Unit: 32 bytes, corresponding to the basic addressing granularity of the fractal column direction. Value Range: [0, 255].<br>In the transpose scenario (calling `asc_copy_l12l0b_transpose`), in addition to satisfying the value range, k_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, b8, or b16, k_step has no additional constraint;<br>&nbsp;&nbsp;&bull;When the data type is b32, k_step must be a multiple of 2.<br>**Note: k_step=0 means no copy is performed, and the interface is treated as a NOP (no-op).** |
| src_stride | Input | Interval between the start addresses of adjacent fractals in the source matrix column direction. Unit: 512 bytes, that is, the size of one data fractal. Value Range: [$-2^{15}$, $2^{15}-1$]. |
| dst_stride | Input | Interval between the start addresses of adjacent fractals in the destination matrix column direction. Unit: 512 bytes, that is, the size of one data fractal. Value Range: [1, $2^{16}-1$]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This interface returns directly when not called by AIC.
- src is located in L1 Buffer, and dst is located in L0B Buffer. They reside in different physical storage units, so there is no address overlap.
- If this instruction overlaps with other instructions in destination address, insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to serialize multiple instructions and prevent abnormal data.
- L0B Buffer capacity limit: The total L0B Buffer capacity is 64KB. The sum of the dst offset and the copy size must not exceed the boundary; otherwise, a write overflow exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512KB. The sum of the src offset and the size occupied by the source matrix must not exceed the boundary; otherwise, an exception is triggered.

### 2D Fractal Copy Constraints

- When m_step or k_step is 0, no copy is performed and this API is treated as a NOP (no-op). Avoid passing 0 unintentionally; otherwise, no valid data is produced and instruction scheduling overhead increases.
- The minimum copy granularity is one 512-byte data fractal. When the matrix edge is smaller than one fractal, the source matrix must be padded to a complete fractal; the copy includes the invalid data in the padded region.
- For the matrix transpose scenario, different data types require different m_step and k_step constraints: for the b4 data type, m_step must be a multiple of 4; for the b8 data type, m_step must be a multiple of 2; for the b16 data type, m_step must be a multiple of 1; for the b32 data type, k_step must be a multiple of 2.
- For the matrix transpose scenario, because of the m_step and k_step constraints, ensure that sufficient space is reserved in the L1 Buffer to avoid read out-of-bounds, which would cause undefined behavior during the copy;
- For the b8 transpose scenario copy, that is, calling the `asc_copy_l12l0b_transpose` API, assuming the C matrix size is (M, N), if $N > 32$ and $N \bmod 32 \le 16$:
  - In this scenario, if the `asc_copy_l12l0b_transpose` copy API is called only once, because the minimum copy unit in the b8 transpose scenario is two fractals, one invalid fractal is copied in the N direction. During subsequent Mmad computation, this fractal must participate in the computation; otherwise, the computation result is incorrect. Finally, when the matrix is copied out, only the valid region is copied out. In this calling mode, the computed data size and the copied-out data size are inconsistent, so the UnitFlag feature cannot be enabled when the matrix is copied out.
  - If the `asc_copy_l12l0b_transpose` copy API is called multiple times in a for loop, set the `dst_stride` parameter appropriately so that when writing to the L0B Buffer, the invalid fractal in the N direction that is read extra during transpose is skipped. Then the Mmad computation does not need this fractal to participate, and the computed data size and the matrix copied-out data size remain consistent. In this scenario, the UnitFlag feature can be enabled when the matrix is copied out. However, note that using a for loop for multiple copies can easily fill the MTE1 instruction queue, and in extreme pipeline scenarios it may cause pipeline blocking. Therefore, choose carefully whether to use a for loop for multiple copies.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The sample occupies 32KB of L1 Buffer, 16KB of L0A Buffer, 16KB of L0B Buffer, and 64KB of L0C Buffer on a single core. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, from MTE1 to M, and from M to FIX.

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
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;

__global__ __cube__ void AscCopyL12l0bKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];

    // Copy matrices A and B from GM into the L1 Buffer and convert them to Nz format.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Copy matrices A and B from the L1 Buffer into the L0A Buffer and L0B Buffer respectively.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b_sync(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Perform matrix multiplication with M=N=K=128.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in the L0C Buffer to ND format and copy it out to GM.
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
}

template <typename T>
void PrintRow(const char* label, const std::vector<T>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << +data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    // Construct the input matrices and compute the golden result on the host side.
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = static_cast<int8_t>(static_cast<int32_t>((row + 2 * k) % 5) - 2);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * col + k) % 5) - 2);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                golden[row * N + col] += a[row * K + k] * b[col * K + k];
            }
        }
    }

    // Allocate device memory, execute the kernel, and retrieve the result.
    aclInit(nullptr);
    aclrtSetDevice(0);
    int8_t *a_device = nullptr, *b_device = nullptr;
    int32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyL12l0bKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_l12l0b result is correct."
                         : "[Failed] asc_copy_l12l0b result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
