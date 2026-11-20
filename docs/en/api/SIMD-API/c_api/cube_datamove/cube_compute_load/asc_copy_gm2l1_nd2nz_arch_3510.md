---
okftype: reference.api
description: "Moves ND data from Global Memory into the L1 Buffer on the AIC and converts it to Nz fractal layout. Supports 12 data types, L2 Cache policy enums, and SmallC0 mode. When d_value is not 32-byte aligned, the destination is automatically zero-padded."
tag:
  - roles: [data_movement]
  - operations: [copy, layout_transform]
  - traits: [aligned_transfer, datablock_granularity, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md
    kind: requires
    condition: "Before calling, the Nz destination cube stride and the ND cube transfer count must be configured."
version: ">=9.2.0"
version_changes: []
---
# asc_copy_gm2l1_nd2nz (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:33:38.429Z pushedAt=2026-11-18T08:15:34.259Z nl=2 -->

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

Transfers data from Global Memory to the L1 Buffer, and during the transfer converts the source ND-format data arranged contiguously along the N dimension into the Nz fractal layout on the L1 Buffer side, so that it meets the requirement of subsequent cube computation for fractal column continuity. This interface is also applicable to conversion scenarios such as converting the convolution format NHWC (ND) to the NC1HWC0 format.

Before performing an ND2Nz format transfer, you must first configure the stride of the destination Nz cube and the ND cube transfer count for the ND2Nz transfer through the [asc_set_gm2l1_nz_para](../cube_load_aux_config/asc_set_gm2l1_nz_para.md) interface.

This interface is valid only when executed on the AIC.

As shown in [Figure 1](#nd2nz_equivalent_conversion), the ND-to-Nz format conversion is also equivalent to the DN-to-Zn format conversion. If you need to perform a DN-to-Zn format conversion, you can use this instruction.

**Figure 1** ND2Nz and DN2Zn diagram<a id="nd2nz_equivalent_conversion"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_copy.png)

For the data layout before transfer, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and transfer granularity requirements, see [Cube Computation Input Transfer Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this interface.

## Prototype

```cpp
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint64_t src_d_value,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint16_t n_value,
                                            uint32_t d_value,
                                            uint64_t src_nd_matrix_stride,
                                            bool enable_small_c0)
```

### Data Types Supported by dtype

The value range of `dtype` is: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Prototype Example

```cpp
// Example: transfer of half type from ND to Nz format.
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ half* dst,
                                            __gm__ half* src,
                                            uint64_t src_d_value,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint16_t n_value,
                                            uint32_t d_value,
                                            uint64_t src_nd_matrix_stride,
                                            bool enable_small_c0)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination operand, stored in L1 Buffer. The start address must be 32-byte aligned. |
| src | Input | Source operand, stored in GM. The start address must be 1-byte aligned. |
| src_d_value | Input | Offset between the start addresses of adjacent rows in the source cube, in bytes. Value range: [0, $2^{40}-1$]. |
| l2_cache_mode | Input | Uses the [asc_load_l2_cache_mode](../../defs/enum/asc_load_l2_cache_mode.md) enumeration value to configure the management policy of the input GM data in L2 Cache. Only `asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`, `asc_load_l2_cache_mode::NORMAL_LAST_VICTIM`, `asc_load_l2_cache_mode::NORMAL_PERSISTENT`, and `asc_load_l2_cache_mode::NOTALLOC_KEEP` are supported. |
| n_value | Input | Number of rows of the ND cube in the source cube, in elements. Value range: [1, 65535]. |
| d_value | Input | Number of columns of the ND cube in the source cube, in elements. Value range: [1, $2^{21}-1$]. During the transfer, when $d\_value \times \mathrm{sizeof}(\mathrm{dtype})$ does not satisfy 32-byte alignment, the hardware automatically pads zeros to 32-byte alignment in the destination cube. |
| src_nd_matrix_stride | Input | Offset between the start addresses of adjacent ND cubes in the source cube, in bytes. Value range: [0, $2^{40}-1$].<br>&nbsp;&nbsp;&bull; When the number of ND cubes in the source cube is 1, this parameter is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When the number of ND cubes in the source cube is greater than 1 and `src_nd_matrix_stride = 0`, it indicates that the first ND cube of the source cube is transferred repeatedly. |
| enable_small_c0 | Input | SmallC0 mode switch. It can be enabled only when $d\_value \le 4$; otherwise, the transfer is abnormal.<br>&nbsp;&nbsp;&bull; `false`: SmallC0 mode is not enabled, and the transfer follows the C0 (32-byte) layout of the standard Nz format.<br>&nbsp;&nbsp;&bull; `true`: SmallC0 mode is enabled. During the transfer, when the amount of transferred data does not satisfy $4 \times \mathrm{sizeof}(\mathrm{dtype})$-byte alignment, the hardware automatically pads zeros to $4 \times \mathrm{sizeof}(\mathrm{dtype})$-byte alignment in the destination cube. |

The ND-to-Nz format conversion is shown in [Figure 2](#nd2nz_conversion).

**Figure 2** ND2Nz conversion diagram<a id="nd2nz_conversion"></a>

![Data layout after ND2Nz format conversion](../../../../figures/asc_copy_gm2l1_nd2nz.png)

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- If this API is not called by the AIC, it returns directly.
- The `dst` start address must be aligned to 32 bytes (L1 Buffer alignment requirement); otherwise, the transfer will be abnormal.
- The `src` start address must be aligned to 1 byte (GM alignment requirement); otherwise, the transfer will be abnormal.
- If this instruction has data dependencies with other instructions, you need to insert [synchronization instructions](../../sync/overview.md#synchronization-control-overview) to ensure that multiple instructions are serialized and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the `dst` offset and the transfer size must not exceed the boundary; otherwise, an exception is triggered.

### ND2Nz Transfer Constraints

- Before calling this instruction, call [asc_set_gm2l1_nz_para](../cube_load_aux_config/asc_set_gm2l1_nz_para.md) to configure the destination Nz cube stride and the ND cube transfer count for ND2Nz transfer.
- The value range of `src_d_value` is [0, $2^{40}-1$]. Values exceeding the value range are clipped, causing the transfer result to be inconsistent with expectations.
- The value range of `n_value` is [1, 65535]. Values exceeding the value range are clipped, causing the transfer result to be inconsistent with expectations.
- The value range of `d_value` is [1, $2^{21}-1$]. Values exceeding the value range are clipped, causing the transfer result to be inconsistent with expectations.
- The value range of `src_nd_matrix_stride` is [0, $2^{40}-1$]. Values exceeding the value range are clipped, causing the transfer result to be inconsistent with expectations.
- When `enable_small_c0` is set to `true`, $d\_value \le 4$; otherwise, the transfer is abnormal.

## Examples

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

__global__ __cube__ void AscCopyGm2l1Nd2nzKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];

    // Transfer two 128 x 128 ND cubes from GM to L1 and convert them to Nz format.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K * sizeof(int8_t), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K * sizeof(int8_t), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move the Nz cubes into L0A and L0B respectively for cube multiplication.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K,
        M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K,
        N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Compute the 128 x 128 cube multiplication and move the result from L0C back to GM.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

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
    // Construct the input and the CPU-side reference result.
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

    // Allocate Device memory, launch the kernel, and verify the result.
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
    AscCopyGm2l1Nd2nzKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Input A row 0", a);
    PrintRow("Input B row 0", b);
    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_gm2l1_nd2nz preserved the MMAD operands/result."
                         : "[Failed] asc_copy_gm2l1_nd2nz result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
