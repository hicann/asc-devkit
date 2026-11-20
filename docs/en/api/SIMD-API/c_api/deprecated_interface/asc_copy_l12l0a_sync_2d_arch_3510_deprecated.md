---
okftype: reference.api
description: "Deprecated interface page: On the 3510 architecture, asc_copy_l12l0a_sync and asc_copy_l12l0a_transpose_sync copy a 2D matrix from L1 Buffer into L0A Buffer in units of 512-byte fractals, supporting transpose and non-transpose as well as b4, b8, b16, and b32. It is recommended to use the non-synchronous interface together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [load, transpose]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous 2D interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, a synchronization instruction must be inserted for serialization"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, insert synchronization instructions to serialize them"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks the 3510 2D forms of asc_copy_l12l0a_sync and asc_copy_l12l0a_transpose_sync with ASC_DEPRECATED(9.2.0). The alternative path is the non-synchronous 2D interface plus asc_sync; the interface has not been removed."
---
# asc_copy_l12l0a_sync (NPU Architecture Version 3510, 2D Matrix Copy, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:11:06.819Z pushedAt=2026-11-18T03:12:16.427Z nl=2 -->

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

**The `asc_copy_l12l0a_sync` and `asc_copy_l12l0a_transpose_sync` interfaces are deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

It is responsible for copying 2D-format data required for matrix computation, with a copy granularity of a 512-byte data fractal, and supports copying both non-transposed and transposed matrices from L1 Buffer to L0A Buffer.

For different data types, the corresponding fractal matrices are as follows:

- For the b4 data type, each data fractal is a 16×64 matrix in L0A Buffer.
- For the b8 data type, each data fractal is a 16×32 matrix in L0A Buffer.
- For the b16 data type, each data fractal is a 16×16 matrix.
- For the b32 data type, each data fractal is a 16×8 matrix in L0A Buffer.

This interface is valid only when executed on the AIC.

## Prototype

### 2D Copy Mode

```cpp
__aicore__ inline void asc_copy_l12l0a_sync(__ca__ <dtype>* dst,
                                            __cbuf__ <dtype>* src,
                                            uint16_t m_start_position,
                                            uint16_t k_start_position,
                                            uint8_t m_step,
                                            uint8_t k_step,
                                            int16_t src_stride,
                                            uint16_t dst_stride)
```

#### Data Types Supported by dtype

The value of `dtype` can be: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### 2D Transpose Copy Mode

```cpp
__aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ <dtype>* dst,
                                                      __cbuf__ <dtype>* src,
                                                      uint16_t m_start_position,
                                                      uint16_t k_start_position,
                                                      uint8_t m_step,
                                                      uint8_t k_step,
                                                      int16_t src_stride,
                                                      uint16_t dst_stride)
```

#### Data Types Supported by dtype

`dtype` values: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination matrix, stored in the L0A Buffer. The start address must be 512-byte aligned. |
| src | Input | Source matrix, stored in the L1 Buffer. The start address must be 32-byte aligned. |
| m_start_position | Input | Start fractal position in the source matrix row direction, in units of 16 elements. Value range: $[0, 2^{16}-1]$. |
| k_start_position | Input | Start fractal position in the source matrix column direction, in units of 32 bytes. Value range: $[0, 2^{16}-1]$. |
| m_step | Input | Fractal step in the source matrix row direction, that is, the number of fractal rows to copy, in units of 16 elements. Value range: $[0, 255]$.<br>In the transpose scenario (calling `asc_copy_l12l0a_transpose`), in addition to the value range, m_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, m_step must be a multiple of 4;<br>&nbsp;&nbsp;&bull;When the data type is b8, m_step must be a multiple of 2;<br>&nbsp;&nbsp;&bull;When the data type is b16, m_step must be a multiple of 1;<br>&nbsp;&nbsp;&bull;When the data type is b32, m_step has no additional constraint.<br>**Note: m_step=0 means no copy is performed, and the interface is treated as a NOP (no-op).** |
| k_step | Input | Fractal step in the source matrix column direction, that is, the number of fractal columns to copy, in units of 32 bytes. Value range: $[0, 255]$.<br>In the transpose scenario (calling `asc_copy_l12l0a_transpose`), in addition to the value range, k_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, b8, or b16, k_step has no additional constraint;<br>&nbsp;&nbsp;&bull;When the data type is b32, k_step must be a multiple of 2.<br>**Note: k_step=0 means no copy is performed, and the interface is treated as a NOP (no-op).** |
| src_stride | Input | Interval between the start address of the previous fractal and the start address of the next fractal in the source matrix column direction, in units of 512 bytes. Value range: $[-2^{15}, 2^{15}-1]$. |
| dst_stride | Input | Interval between the start address of the previous fractal and the start address of the next fractal in the destination matrix column direction, in units of 512 bytes. Value range: $[1, 2^{16}-1]$. |

Taking the non-transpose scenario as an example, the parameter values and explanations are shown in Figure 1:

- $m\_start\_position=2$, indicating that the start position in the source matrix row direction is the 32nd element, that is, $2 \times 16=32$ elements.
- $k\_start\_position=2$, indicating that the start offset in the source matrix column direction is $2 \times 32=64$ bytes.
- $m\_step=2$, indicating that the copy length in the source matrix row direction is $2 \times 16=32$ elements.
- $k\_step=3$, indicating that the copy length in the source matrix column direction is $3 \times 32=96$ bytes.
- $src\_stride=5$, indicating that the interval between the start addresses of adjacent fractals in the source matrix column direction is $5 \times 512=2560$ bytes.
- $dst\_stride=3$, indicating that the interval between the start addresses of adjacent fractals in the destination matrix column direction is $3 \times 512=1536$ bytes.

**Figure 1**  l12l0a_2d copy parameter example (using the half data type, non-transpose)

![l12l0a_2d copy parameter example](../../../figures/loaddata2dparamsv2_half_no_transpose.png)

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This API returns directly when it is not called by the AIC.
- The dst start address must be 512-byte aligned (L0A Buffer alignment requirement); otherwise, a copy exception occurs.
- The src start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, a copy exception occurs.
- src is located in the L1 Buffer, and dst is located in the L0A Buffer. They belong to different physical storage units, so there is no scenario where the source operand and destination operand addresses overlap.
- If this instruction overlaps with other instructions in destination address, insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to serialize multiple instructions and prevent abnormal data.
- L0A Buffer capacity limit: The total L0A Buffer capacity is 64 KB. The sum of the dst offset and the copy size must not exceed the boundary; otherwise, an exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the src offset and the size occupied by the source matrix must not exceed the boundary; otherwise, an exception is triggered.

### 2D Fractal Copy Constraints

- When m_step or k_step is 0, no copy is performed and this API is treated as a NOP (no-op). Avoid passing 0 unintentionally, otherwise no valid data is produced and instruction scheduling overhead is increased.
- The API uses a complete 512-byte fractal as the minimum copy granularity and does not support copying only part of the elements within a fractal. Invalid elements in a fractal must be filled before the call according to the subsequent matrix computation requirements.
- For the matrix transpose scenario, different data types require different m_step and k_step constraints: for the b4 data type, m_step must be a multiple of 4; for the b8 data type, m_step must be a multiple of 2; for the b16 data type, m_step must be a multiple of 1; for the b32 data type, k_step must be a multiple of 2.
- For the matrix transpose scenario, due to the m_step and k_step constraints, ensure that sufficient space is reserved in the L1 Buffer to avoid read out-of-bounds, which would cause undefined behavior during the copy;
- For the b8 transpose scenario copy, that is, calling the `asc_copy_l12l0a_transpose` API, assuming the C matrix size is (M, N), if $M > 32$ and $M \bmod 32 \le 16$:
  - In this scenario, if the `asc_copy_l12l0a_transpose` copy API is called only once, because the minimum copy unit in the b8 transpose scenario is two fractals, one invalid fractal is copied extra in the M direction. During subsequent Mmad computation, this fractal must participate in the computation, otherwise the computation result will be incorrect. Finally, when the matrix is copied out, only the valid region is copied out. In this calling mode, the computed data size and the copied-out data size are inconsistent, so the UnitFlag feature cannot be enabled when the matrix is copied out.
  - If the `asc_copy_l12l0a_transpose` copy API is called multiple times in a for loop, set the `dst_stride` parameter properly so that when writing to the L0A Buffer, the invalid fractal in the M direction that is read extra during transpose is skipped. Then the Mmad computation does not need this fractal to participate, and the computed data size and the matrix copied-out data size remain consistent. In this scenario, the UnitFlag feature can be enabled when the matrix is copied out. However, note that using a for loop for multiple copies can easily fill up the MTE1 instruction queue, and in extreme pipelining scenarios it may cause pipeline blocking. Therefore, choose reasonably whether to use a for loop for multiple copies.

## Key Feature Description

### Non-transpose Scenario Copy-in and Copy Start Position Calculation

Use m_step and k_step to copy data blocks in multiple directions, taking the Nz fractal of $[M, K]$ as an example:

- m_start_position is 2, indicating that the copy start position in the M direction is at a position $16 \times 2=32$ elements away from the start position of the source operand along the M axis.
- k_start_position is 2, indicating that the copy start position in the K direction is at a position $32 \times 2=64$ bytes away from the start position of the source operand along the K axis.
- src_stride is 5, indicating that in the source operand, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction is 5.
- dst_stride is 3, indicating that in the destination operand, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction is 3.

The start address calculation formula is:

$$
startAddr = srcAddr + (k_start_position \times \lvert src_stride \rvert + m_start_position) \times 512B
$$

Therefore, the final copy start position is the thirteenth fractal matrix (the first fractal matrix starts from the start address of the source operand).

**Figure 2** Copy start position diagram

![Copy Start Position Diagram](../../../figures/load2dv2_start_addr_demo.png "Copy Start Position Diagram")

### Transpose Scenario Copy-In

For matrix transpose operations on b4/b8/b16/b32, the following constraints on m_step and k_step must be satisfied:

- For the b4 data type, m_step must be a multiple of 4.
- For the b8 data type, m_step must be a multiple of 2.
- For the b16 data type, m_step must be a multiple of 1.
- For the b32 data type, k_step must be a multiple of 2.

For the transpose layout of each data type, see Figure 3 to Figure 6.

**Figure 3** b4 Data Type Transpose Diagram

![b4 Data Type Transpose Diagram](../../../figures/load2dv2_l12l0_trans_b4.png "b4 Data Type Transpose Diagram")

**Figure 4** b8 Data Type Transpose Diagram

![b8 data type transpose diagram](../../../figures/load2dv2_l12l0_trans_b8.png "b8 data type transpose diagram")

**Figure 5** b16 data type transpose diagram

![b16 data type transpose diagram](../../../figures/load2dv2_l12l0_trans_b16.png "b16 data type transpose diagram")

**Figure 6** b32 data type transpose diagram

![b32 data type transpose diagram](../../../figures/load2dv2_l12l0_trans_b32.png "b32 data type transpose diagram")

## Examples

For the complete project sample in the repository, see [mmad sample](../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/mmad).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

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

__global__ __cube__ void AscCopyL12l0aKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];

    // Configure the Nz format and copy the 128 x 128 matrix from GM to the L1 Buffer.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
    // Wait until both GM-to-L1 transfers finish before PIPE_MTE1 reads L1 Buffer.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move all A and B fractals to L0A Buffer and L0B Buffer for the 128x128x128 matrix multiplication.
    asc_copy_l12l0a_sync(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Multiply A and B after PIPE_MTE1 has produced both operands.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in L0C to ND format and copy it back to GM.
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
    // Generate deterministic dense inputs and the corresponding host-side golden result.
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
    AscCopyL12l0aKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_l12l0a result is correct."
                         : "[Failed] asc_copy_l12l0a result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
