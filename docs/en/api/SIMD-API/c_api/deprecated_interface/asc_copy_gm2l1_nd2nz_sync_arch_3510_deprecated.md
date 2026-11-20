---
okftype: reference.api
description: "Deprecated API page: asc_copy_gm2l1_nd2nz_sync on the 3510 architecture synchronously completes the ND-to-Nz copy from GM to L1 Buffer and implicitly waits. Parameters include row offset, L2 policy, N/D sizes, and the SmallC0 switch. It is officially recommended to use the non-synchronous overload with the L2 policy parameter plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy, layout_transform]
  - traits: [aligned_transfer, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md
    kind: requires
    condition: "Before calling this API, configure the destination Nz matrix stride and the number of ND matrices for the ND2Nz copy"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, a synchronization instruction must be inserted to serialize them"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, a synchronization instruction must be inserted to serialize them"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 3510 synchronous overload with ASC_DEPRECATED(9.2.0). The Release Notes 9.2.0 deprecation table requires switching to the overload with asc_load_l2_cache_mode plus asc_sync; the API has not been removed."
---
# asc_copy_gm2l1_nd2nz_sync (NPU architecture version 3510, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:38:32.099Z pushedAt=2026-11-18T03:12:16.395Z nl=2 -->

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

**The `asc_copy_gm2l1_nd2nz_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) and the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Copies data from Global Memory to the L1 Buffer, and during the copy converts the source ND-format data, which is arranged contiguously along the N dimension, into the Nz fractal layout on the L1 Buffer side, so that it meets the requirement of fractal column continuity for subsequent matrix computation. This API is also applicable to conversion scenarios such as converting the convolution format NHWC (ND) to the NC1HWC0 format.

Before performing an ND2Nz format copy, configure the stride of the destination Nz matrix and the number of ND matrices to copy for the ND2Nz copy through the [asc_set_gm2l1_nz_para](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md) API.

This API takes effect only when executed on the AIC.

As shown in [Figure 1](#nd2nz_equivalent_conversion), the ND-to-Nz format conversion is also equivalent to the DN-to-Zn format conversion. If you need to perform a DN-to-Zn format conversion, you can use this instruction.

**Figure 1** ND2Nz and DN2Zn diagram<a id="nd2nz_equivalent_conversion"></a>

![](../../../figures/datacopy_gm2l1_nd2nz_copy.png)

## Prototype

```cpp
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ <dtype>* dst,
                                                 __gm__ <dtype>* src,
                                                 uint64_t loop1_src_stride,
                                                 uint8_t l2_cache_ctl,
                                                 uint16_t n_value,
                                                 uint32_t d_value,
                                                 uint64_t loop4_src_stride,
                                                 bool enable_small_c0)
```

### Data Types Supported by dtype

The values of `dtype` are: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Prototype Example

```cpp
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ <dtype>* dst,
                                                 __gm__ <dtype>* src,
                                                 uint64_t loop1_src_stride,
                                                 uint8_t l2_cache_ctl,
                                                 uint16_t n_value,
                                                 uint32_t d_value,
                                                 uint64_t loop4_src_stride,
                                                 bool smallc0_en)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination operand, stored in L1 Buffer. The start address must be 32-byte aligned. |
| src | Input | Source operand, stored in GM. The start address must be 1-byte aligned. |
| loop1_src_stride | Input | Offset between the start addresses of adjacent rows in the source matrix, in bytes. Value range: [0, $2^{40}-1$]. |
| l2_cache_ctl | Input | Configures the management policy of the input GM data in L2 Cache. The value must be one of the defined policies (0/1/2/4). Other values trigger an L2 Cache policy exception. See [Table 2](#l2_cache_ctl_values) for value descriptions. |
| n_value | Input | Number of rows of the ND matrix in the source matrix, in elements. Value range: [1, 65535]. |
| d_value | Input | Number of columns of the ND matrix in the source matrix, in elements. Value range: [1, $2^{21}-1$]. During the copy, when $d\_value \times \mathrm{sizeof}(\mathrm{dtype})$ does not satisfy 32-byte alignment, the hardware automatically pads zeros in the destination matrix to 32-byte alignment. |
| loop4_src_stride | Input | Offset between the start addresses of adjacent ND matrices in the source matrix, in bytes. Value range: [0, $2^{40}-1$].<br>&nbsp;&nbsp;&bull; When the number of ND matrices in the source matrix is 1, this parameter is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When the number of ND matrices in the source matrix is greater than 1, `loop4_src_stride = 0` indicates that the first ND matrix of the source matrix is copied repeatedly. |
| smallc0_en | Input | SmallC0 mode switch. It can be enabled only when $d\_value \le 4$; otherwise, the copy is abnormal.<br>&nbsp;&nbsp;&bull; `false`: SmallC0 mode is disabled, and the data is copied in the standard Nz format C0 (32-byte) layout.<br>&nbsp;&nbsp;&bull; `true`: SmallC0 mode is enabled. When the amount of data copied does not satisfy $4 \times \mathrm{sizeof}(\mathrm{dtype})$-byte alignment, the hardware automatically pads zeros in the destination matrix to $4 \times \mathrm{sizeof}(\mathrm{dtype})$-byte alignment. |

**Table 2** `l2_cache_ctl` value description <a id="l2_cache_ctl_values"></a>

| Value | Mode | Meaning |
|------|------|------|
| 0 | Normal mode | Enables L2 Cache and marks the allocated Cache Lines as high replacement priority. |
| 1 | Last mode | &bull; Enables L2 Cache and marks the allocated Cache Lines as low replacement priority.<br>&bull; **Last mode is not supported yet.**|
| 2 | Persistent mode | &bull; Enables L2 Cache. Data already stored in L2 Cache may be replaced. To ensure that data of specific GM always remains in L2 Cache, use the persistent mode.<br>&bull; Cache Lines marked as persistent mode can only be replaced by other Cache Lines also marked as persistent mode.<br>&bull; **Persistent mode is not supported yet.**|
| 4 | Disable mode | Does not enable L2 Cache. Data is read directly from GM each time, and the state of existing Cache Lines remains unchanged. |

The ND-to-Nz format conversion is shown in [Figure 2](#nd2nz_conversion).

**Figure 2** ND2Nz conversion diagram<a id="nd2nz_conversion"></a>

![ND2Nz data layout after format conversion](../../../figures/asc_copy_gm2l1_nd2nz.png)

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- If this API is not called by the AIC, it returns directly.
- The start address of `dst` must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, the copy will be abnormal.
- The start address of `src` must be 1-byte aligned (GM alignment requirement); otherwise, the copy will be abnormal.
- If the destination address of this instruction overlaps with that of other instructions, insert synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the `dst` offset and the copy size must not exceed the boundary; otherwise, an exception is triggered.

### ND2Nz Copy Constraints

- Before calling this instruction, call [asc_set_gm2l1_nz_para](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md) to configure the destination Nz matrix stride and the number of ND matrices for the ND2Nz copy.
- The value range of `loop1_src_stride` is [0, $2^{40}-1$]. A value outside the value range will be clipped, which may cause the copy result to be incorrect.
- The value range of `n_value` is [1, 65535]. A value outside the value range will be clipped, which may cause the copy result to be incorrect.
- The value range of `d_value` is [1, $2^{21}-1$]. A value outside the value range will be clipped, which may cause the copy result to be incorrect.
- The value range of `loop4_src_stride` is [0, $2^{40}-1$]. A value outside the value range will be clipped, which may cause the copy result to be incorrect.
- When `smallc0_en` is set to `true`, $d\_value \le 4$; otherwise, the copy will be abnormal.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

    // Copy two 128 x 128 ND matrices from GM to L1 and convert them to Nz format.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz_sync(a_l1, a, K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz_sync(b_l1, b, K, 0, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Copy the Nz matrices into L0A and L0B respectively for matrix multiplication.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K,
        M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K,
        N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Compute the 128 x 128 matrix multiplication and copy the result from L0C back to GM.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

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
