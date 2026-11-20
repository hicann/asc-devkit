---
okftype: reference.api
description: "Moves a 2D cube from L1 Buffer into L0B Buffer on the AIC in units of 512-byte fractals, supporting both non-transpose and transpose modes, covering b4 to b32 data types."
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
# asc_copy_l12l0b (NPU Architecture Version 3510, 2D Cube Move)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:52:56.213Z pushedAt=2026-11-18T08:15:34.279Z nl=2 -->

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

It is responsible for moving the 2D format data required for cube computation. The move granularity is a data fractal of 512 bytes, and it supports moving non-transpose and transpose cubes from L1 Buffer to L0B Buffer.

For different data types, the corresponding fractal cubes are as follows:

- For the b4 data type, each data fractal is a 64x16 cube in L0B Buffer.
- For the b8 data type, each data fractal is a 32x16 cube in L0B Buffer.
- For the b16 data type, each data fractal is a 16x16 cube.
- For the b32 data type, each data fractal is an 8x16 cube in L0B Buffer.

This interface is valid only when executed on AIC.

For the data layout before moving, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and move granularity requirements, see [Cube Computation Input Move Constraint](cube_compute_input_move_constraint.md), and use them in combination with the parameter and constraint descriptions of this interface.

### B Cube Input Layout and Interface Selection

The "non-transpose move" and "transpose move" of this interface describe how data is moved from L1 Buffer to L0B Buffer, and do not directly indicate whether the right cube in the cube multiplication expression is transposed.

Take the target computation `C = A × B` as an example, where the shape of A is `M×K` and the shape of B is `K×N`. When the input in GM uses the ND format and is converted to the Nz format in L1 Buffer through `asc_copy_gm2l1_nd2nz`, the B-side interface is selected as follows:

| Data Stored in GM | GM Input Shape | L1 to L0B Move Interface | Target Computation |
| --- | --- | --- | --- |
| B | `K×N` | `asc_copy_l12l0b_transpose` | `C = A × B` |
| Bᵀ, that is, the pre-transposed B | `N×K` | `asc_copy_l12l0b` | `C = A × B` |

L0B Buffer uses the Zn fractal layout. The interface should be selected based on the actual layout of the source cube, and the final multiplication direction cannot be determined solely by the `transpose` in the interface name. Transpose move must still satisfy the fractal, alignment, and space requirements in the parameter description and constraint description on this page.

## Prototype

### 2D Move Mode

```cpp
__aicore__ inline void asc_copy_l12l0b(__cb__ <dtype>* dst,
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

### Prototype Example

```cpp
// Example: 2D move of the half type.
__aicore__ inline void asc_copy_l12l0b(__cb__ half* dst,
                                       __cbuf__ half* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

### 2D Transpose Move Mode

```cpp
__aicore__ inline void asc_copy_l12l0b_transpose(__cb__ <dtype>* dst,
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

### Prototype Example

```cpp
// Example: 2D move of the half type.
__aicore__ inline void asc_copy_l12l0b_transpose(__cb__ half* dst,
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
| --- | --- | --- |
| dst | Output | Destination cube, stored in L0B Buffer. The start address must be aligned to 512 bytes. |
| src | Input | Source cube, stored in L1 Buffer. The start address must be aligned to 32 bytes. |
| m_start_position | Input | Start fractal position of the source cube in the row direction. Unit is 16 elements, corresponding to the fixed width of the fractal row direction. Value range: [0, $2^{16}-1$]. |
| k_start_position | Input | Start fractal position of the source cube in the column direction. Unit is 32 bytes, corresponding to the basic addressing granularity of the fractal column direction. Value range: [0, $2^{16}-1$]. |
| m_step | Input | Number of fractal rows moved in the source cube row direction. Unit is 16 elements, corresponding to the fixed width of the fractal row direction. Value range: [0, 255].<br>In the transpose scenario (when calling `asc_copy_l12l0b_transpose`), in addition to satisfying the value range, m_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, m_step must be a multiple of 4;<br>&nbsp;&nbsp;&bull;When the data type is b8, m_step must be a multiple of 2;<br>&nbsp;&nbsp;&bull;When the data type is b16, m_step must be a multiple of 1;<br>&nbsp;&nbsp;&bull;When the data type is b32, m_step has no additional constraint.<br>**Note: m_step=0 means no move is performed, and the interface is treated as a NOP (no-op).** |
| k_step | Input | Number of fractal columns moved in the source cube column direction. Unit is 32 bytes, corresponding to the basic addressing granularity of the fractal column direction. Value range: [0, 255].<br>In the transpose scenario (when calling `asc_copy_l12l0b_transpose`), in addition to satisfying the value range, k_step must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, b8, or b16, k_step has no additional constraint;<br>&nbsp;&nbsp;&bull;When the data type is b32, k_step must be a multiple of 2.<br>**Note: k_step=0 means no move is performed, and the interface is treated as a NOP (no-op).** |
| src_stride | Input | Interval between the start addresses of adjacent fractals in the source cube column direction. Unit is 512 bytes, that is, the size of one data fractal. Value range: [$-2^{15}$, $2^{15}-1$]. |
| dst_stride | Input | Interval between the start addresses of adjacent fractals in the destination cube column direction. Unit is 512 bytes, that is, the size of one data fractal. Value range: [1, $2^{16}-1$]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- This interface returns directly when called by a non-AIC.
- src is located in the L1 Buffer, and dst is located in the L0B Buffer. They reside in different physical storage units, so there is no address overlap.
- If this instruction has data dependencies with other instructions, you need to insert a [sync instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L0B Buffer capacity limit: The total L0B Buffer capacity is 64KB. The sum of the dst offset and the move size must not exceed the boundary; otherwise, a write overflow exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512KB. The sum of the src offset and the size occupied by the source cube must not exceed the boundary; otherwise, an exception is triggered.

### 2D Fractal Move Constraints

- When m_step or k_step is 0, no move is performed, and this interface is treated as a NOP (No-op). Avoid passing 0 unintentionally; otherwise, no valid data is produced and instruction scheduling overhead is increased.
- The minimum move granularity is one 512-byte data fractal. When the cube edge is smaller than one fractal, the source cube must be padded to a complete fractal; the move includes the invalid data in the padded region.
- For the cube transpose scenario, different data types must satisfy different m_step and k_step constraints: for the b4 data type, m_step must be a multiple of 4; for the b8 data type, m_step must be a multiple of 2; for the b16 data type, m_step must be a multiple of 1; for the b32 data type, k_step must be a multiple of 2.
- For the cube transpose scenario, because of the m_step and k_step constraints, ensure that sufficient space is reserved in the L1 Buffer to avoid read out-of-bounds, which would cause undefined behavior during the move;
- For the b8 transpose scenario move, that is, calling the `asc_copy_l12l0b_transpose` interface, assume the C cube size is (M, N). If $N > 32$ and $N \bmod 32 \le 16$:
  - In this scenario, if the `asc_copy_l12l0b_transpose` move interface is called only once, because the minimum move unit in the b8 transpose scenario is two fractals, one invalid fractal is moved in the N direction. During subsequent Mmad computation, this fractal must participate in the computation; otherwise, the computation result is incorrect. Finally, when moving the cube out, only the valid region is moved out. In this calling mode, the computed data size and the moved-out data size are inconsistent, so the UnitFlag feature cannot be enabled when moving the cube out.
  - If the `asc_copy_l12l0b_transpose` move interface is called multiple times via a for loop, set the `dst_stride` parameter appropriately to skip the invalid fractal in the N direction that is read extra during transpose when writing to the L0B Buffer. Then the Mmad computation does not need this fractal to participate in the computation, and the computed data size and the cube moved-out data size remain consistent. In this scenario, the UnitFlag feature can be enabled when moving the cube out. However, note that using a for loop for multiple moves can easily cause the MTE1 instruction queue to become full, and in extreme pipelining scenarios it may cause pipeline stalls. Choose reasonably whether to use a for loop for multiple moves.

## Examples

The following example demonstrates the non-transpose move of `asc_copy_l12l0b`. The input cube A has shape `M×K`, the input cube B has shape `N×K`, the actual computation is `C = A × Bᵀ`, and the output shape is `M×N`. The Host-side Golden is computed using the same formula.

In this example, B refers to the input cube stored in the code variable `b`; it is equivalent to the pre-transposed input in the interface selection table described earlier.

For a complete project covering transposed and non-transposed inputs of cubes A/B and the int8_t/half/float data types, see the [load_data_l12l0_950 sample](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/load_data_l12l0_950/README.md).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The sample occupies 32KB L1 Buffer, 16KB L0A Buffer, 16KB L0B Buffer, and 64KB L0C Buffer on a single core. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

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

    // Move the A cube of shape M×K and the B cube of shape N×K from GM into L1 Buffer, and convert them to Nz format.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move the A cube and B cube from L1 Buffer into L0A Buffer and L0B Buffer respectively. Use asc_copy_l12l0b non-transpose move on the B side.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Perform the cube multiplication C = A × Bᵀ, where M=N=K=128.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in L0C Buffer to ND format and move it out to GM.
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
    // Construct the input cubes, and compute the Golden result on the Host side according to C = A × Bᵀ.
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

    // Allocate Device memory, execute the kernel, and retrieve the result.
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
