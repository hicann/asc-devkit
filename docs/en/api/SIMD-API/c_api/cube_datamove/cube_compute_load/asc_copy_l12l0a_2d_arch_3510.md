---
okftype: reference.api
description: "Loads a 2D cube from the L1 Buffer into the L0A Buffer on the AIC in units of 512-byte fractals, supporting both non-transpose and transpose modes, covering multiple data types from b4 to b32."
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
# asc_copy_l12l0a (NPU Architecture Version 3510, 2D Cube Copy)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:42:26.374Z pushedAt=2026-11-18T08:15:34.270Z nl=2 -->

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

It is responsible for copying the 2D format data required for cube computation. The copy granularity is a 512-byte data fractal, and it supports both non-transposed and transposed cube copy from L1 Buffer to L0A Buffer.

For different data types, the corresponding fractal cubes are as follows:

- For the b4 data type, each data fractal is a 16×64 cube in L0A Buffer.
- For the b8 data type, each data fractal is a 16×32 cube in L0A Buffer.
- For the b16 data type, each data fractal is a 16×16 cube.
- For the b32 data type, each data fractal is a 16×8 cube in L0A Buffer.

This interface is valid only when executed on the AIC.

For the data layout before loading, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and copy granularity requirements, see [Cube Computation Input Copy Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this interface.

### A Cube Input Layout and Interface Selection

The "non-transpose copy" and "transpose copy" of this interface describe the data copy method from L1 Buffer to L0A Buffer, and do not directly indicate whether the left cube in the cube multiplication expression is transposed.

Take the target computation `C = A × B` as an example, where the shape of A is `M×K` and the shape of B is `K×N`. When the input in GM uses the ND format and is converted to the Nz format in L1 Buffer through `asc_copy_gm2l1_nd2nz`, the A-side interface selection is as follows:

| Data Stored in GM | GM Input Shape | L1 to L0A Copy Interface | Target Computation |
| --- | --- | --- | --- |
| A | `M×K` | `asc_copy_l12l0a` | `C = A × B` |
| Aᵀ, that is, pre-transposed A | `K×M` | `asc_copy_l12l0a_transpose` | `C = A × B` |

L0A Buffer uses the Nz fractal layout. The interface should be selected based on the actual layout of the source cube, and the final multiplication direction cannot be determined solely by the `transpose` in the interface name. The B side also needs to select the corresponding L1 Buffer to L0B Buffer copy interface based on the actual input layout. Transpose copy must still meet the fractal, alignment, and space requirements in the parameter description and constraint description on this page.

## Prototype

### 2D Copy Mode

```cpp
__aicore__ inline void asc_copy_l12l0a(__ca__ <dtype>* dst,
                                       __cbuf__ <dtype>* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

#### Data Types Supported by dtype

The value of `dtype` is: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Prototype Example

```cpp
// Example: 2D copy of the half type.
__aicore__ inline void asc_copy_l12l0a(__ca__ half* dst,
                                       __cbuf__ half* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

### 2D Transpose Copy Mode

```cpp
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ <dtype>* dst,
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

### Prototype Example

```cpp
// Example: 2D copy of the half type.
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ half* dst,
                                                 __cbuf__ half* src,
                                                 uint16_t m_start_position,
                                                 uint16_t k_start_position,
                                                 uint8_t m_step,
                                                 uint8_t k_step,
                                                 int16_t src_stride,
                                                 uint16_t dst_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination cube, stored in the L0A Buffer. The start address must follow 512-byte alignment. |
| src | Input | Source cube, stored in the L1 Buffer. The start address must follow 32-byte alignment. |
| m_start_position | Input | Start fractal position of the source cube in the row direction, in units of 16 elements. Value range: $[0, 2^{16}-1]$. |
| k_start_position | Input | Start fractal position of the source cube in the column direction, in units of 32 bytes. Value range: $[0, 2^{16}-1]$. |
| m_step | Input | Fractal step of the source cube in the row direction, that is, the number of fractal rows to copy, in units of 16 elements. Value range: $[0, 255]$.<br>In the transpose scenario (when calling `asc_copy_l12l0a_transpose`), in addition to the value range, m_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, m_step must be a multiple of 4;<br>&nbsp;&nbsp;&bull;When the data type is b8, m_step must be a multiple of 2;<br>&nbsp;&nbsp;&bull;When the data type is b16, m_step must be a multiple of 1;<br>&nbsp;&nbsp;&bull;When the data type is b32, m_step has no additional constraint.<br>**Note: m_step=0 means no copy is performed, and the interface is treated as a NOP (no operation).** |
| k_step | Input | Fractal step of the source cube in the column direction, that is, the number of fractal columns to copy, in units of 32 bytes. Value range: $[0, 255]$.<br>In the transpose scenario (when calling `asc_copy_l12l0a_transpose`), in addition to the value range, k_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, b8, or b16, k_step has no additional constraint;<br>&nbsp;&nbsp;&bull;When the data type is b32, k_step must be a multiple of 2.<br>**Note: k_step=0 means no copy is performed, and the interface is treated as a NOP (no operation).** |
| src_stride | Input | Interval between the start address of the previous fractal and the start address of the next fractal in the source cube column direction, in units of 512 bytes. Value range: $[-2^{15}, 2^{15}-1]$. |
| dst_stride | Input | Interval between the start address of the previous fractal and the start address of the next fractal in the destination cube column direction, in units of 512 bytes. Value range: $[1, 2^{16}-1]$. |

Taking the non-transpose scenario as an example, the parameter settings and explanations are shown in Figure 1:

- $m\_start\_position=2$, indicating that the start position of the source cube in the row direction is the 32nd element, that is, $2 \times 16=32$ elements.
- $k\_start\_position=2$, indicating that the start offset of the source cube in the column direction is $2 \times 32=64$ bytes.
- $m\_step=2$, indicating that the copy length of the source cube in the row direction is $2 \times 16=32$ elements.
- $k\_step=3$, indicating that the copy length of the source cube in the column direction is $3 \times 32=96$ bytes.
- $src\_stride=5$, indicating that the interval between the start addresses of adjacent fractals in the source cube column direction is $5 \times 512=2560$ bytes.
- $dst\_stride=3$, indicating that the interval between the start addresses of adjacent fractals in the destination cube column direction is $3 \times 512=1536$ bytes.

**Figure 1**  l12l0a_2d copy parameter example (with half data type, non-transpose as an example)

![l12l0a_2d copy parameter example](../../../../figures/loaddata2dparamsv2_half_no_transpose.png)

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This interface returns directly when not called by the AIC.
- The dst start address must follow 512-byte alignment (L0A Buffer alignment requirement); otherwise, it will cause a copy exception.
- The src start address must follow 32-byte alignment (L1 Buffer alignment requirement); otherwise, it will cause a copy exception.
- src is located in the L1 Buffer, and dst is located in the L0A Buffer. The two belong to different physical storage units, so there is no scenario where the source operand and destination operand addresses overlap.
- If this instruction has a data dependency with other instructions, a [sync instruction](../../sync/overview.md#synchronization-control-overview) must be inserted to serialize multiple instructions and prevent abnormal data.
- L0A Buffer capacity limit: The total L0A Buffer capacity is 64 KB. The sum of the dst offset and the copy size must not exceed the boundary; otherwise, an exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the src offset and the size occupied by the source cube must not exceed the boundary; otherwise, an exception is triggered.

### 2D Fractal Copy Constraints

- When m_step or k_step is 0, no copy is performed and this interface is treated as a NOP (No Operation). Avoid passing 0 unintentionally; otherwise, no valid data is produced and instruction scheduling overhead increases.
- The interface uses a complete 512-byte fractal as the minimum copy granularity and does not support copying only part of the elements within a fractal. Invalid elements in the fractal must be filled before the call according to the subsequent cube computation requirements.
- For the cube transpose scenario, different data types must satisfy different m_step and k_step constraints: for the b4 data type, m_step must be a multiple of 4; for the b8 data type, m_step must be a multiple of 2; for the b16 data type, m_step must be a multiple of 1; for the b32 data type, k_step must be a multiple of 2.
- For the cube transpose scenario, because of the m_step and k_step constraints, ensure that sufficient space is reserved in the L1 Buffer to avoid read out-of-bounds, which would cause undefined behavior during the copy;
- For the b8 transpose scenario copy, that is, calling the `asc_copy_l12l0a_transpose` interface, assume the C cube size is (M, N). If $M > 32$ and $M \bmod 32 \le 16$:
  - In this scenario, if the `asc_copy_l12l0a_transpose` copy interface is called only once, because the minimum copy unit in the b8 transpose scenario is two fractals, one invalid fractal is copied in the M direction. During subsequent Mmad computation, this fractal must participate in the computation; otherwise, the computation result is incorrect. Finally, when the cube is copied out, only the valid region is copied out. In this calling mode, the computed data size and the copied-out data size are inconsistent, so the UnitFlag feature cannot be enabled when the cube is copied out.
  - If the `asc_copy_l12l0a_transpose` copy interface is called multiple times via a for loop, set the `dst_stride` parameter properly so that when writing to the L0A Buffer, the invalid fractal in the M direction that is read extra during transpose is skipped. Then the Mmad computation does not need this fractal to participate, and the computed data size and the cube copied-out data size remain consistent. In this scenario, the UnitFlag feature can be enabled when the cube is copied out. However, note that using a for loop for multiple copies can easily fill the MTE1 instruction queue, and in extreme pipelining scenarios it may cause pipeline stalls. Reasonably decide whether to use a for loop for multiple copies.

## Key Feature Description

### Non-transpose Scenario Load and Copy Start Position Calculation

Use m_step and k_step to copy data blocks in multiple directions, taking the Nz fractal of $[M, K]$ as an example:

- m_start_position is 2, indicating that the M-direction copy start position is at a position $16 \times 2=32$ elements away from the source operand start position along the M axis.
- k_start_position is 2, indicating that the K-direction copy start position is at a position $32 \times 2=64$ bytes away from the source operand start position along the K axis.
- src_stride is 5, indicating that in the source operand, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction is 5.
- dst_stride is 3, indicating that in the destination operand, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction is 3.

The start address calculation formula is:

$$
startAddr = srcAddr + (k_start_position \times \lvert src_stride \rvert + m_start_position) \times 512B
$$

Therefore, the final copy start position is the thirteenth fractal cube (the source operand start address is the first fractal cube).

**Figure 2** Copy start position diagram

![Copy Start Position Diagram](../../../../figures/load2dv2_start_addr_demo.png "Copy Start Position Diagram")

### Transpose Scenario Load

For cube transpose operations on b4/b8/b16/b32, the following constraints on m_step and k_step must be satisfied:

- For the b4 data type, m_step must be a multiple of 4.
- For the b8 data type, m_step must be a multiple of 2.
- For the b16 data type, m_step must be a multiple of 1.
- For the b32 data type, k_step must be a multiple of 2.

The transpose layouts of each data type are shown in Figure 3 to Figure 6.

**Figure 3** b4 data type transpose diagram

![b4 data type transpose diagram](../../../../figures/load2dv2_l12l0_trans_b4.png "b4 data type transpose diagram")

**Figure 4** b8 data type transpose diagram

![b8 data type transpose diagram](../../../../figures/load2dv2_l12l0_trans_b8.png "b8 data type transpose diagram")

**Figure 5** b16 data type transpose diagram

![b16 data type transpose diagram](../../../../figures/load2dv2_l12l0_trans_b16.png "b16 data type transpose diagram")

**Figure 6** b32 data type transpose diagram

![b32 data type transpose diagram](../../../../figures/load2dv2_l12l0_trans_b32.png "b32 data type transpose diagram")

## Examples

The following example demonstrates the non-transpose copy of `asc_copy_l12l0a`. The input cube A has a shape of `M×K`, and the input cube B has a shape of `N×K`. The A/B sides use `asc_copy_l12l0a` and `asc_copy_l12l0b` respectively for non-transpose copy. The actual computation is `C = A × Bᵀ`, and the output shape is `M×N`. The Host-side Golden is computed using the same formula.

In this example, B refers to the input cube stored in the code variable `b`; the non-transpose copy on the A side does not mean that the entire example computes `C = A × B`.

For a complete project covering transpose and non-transpose inputs of A/B cubes and int8_t/half/float data types, see [load_data_l12l0_950 sample](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/load_data_l12l0_950/README.md).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

    // Copy cube A of shape M×K and cube B of shape N×K from GM into the L1 Buffer, and convert them to Nz format.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    // Wait until both GM-to-L1 transfers finish before PIPE_MTE1 reads L1 Buffer.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Load cube A and cube B from L1 Buffer into L0A Buffer and L0B Buffer respectively in non-transpose mode.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // After waiting for PIPE_MTE1 to complete the copy, execute the cube multiplication C = A × Bᵀ, where M=N=K=128.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in L0C to ND format and copy it back to GM.
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
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
    // Construct deterministic input cubes and compute the Golden result on the Host side according to C = A × Bᵀ.
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
