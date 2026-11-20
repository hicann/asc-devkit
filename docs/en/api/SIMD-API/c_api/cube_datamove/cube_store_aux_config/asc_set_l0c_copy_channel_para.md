---
okftype: reference.api
description: "When moving data out of the L0C Buffer and performing Nz2DN format conversion, configure the address offset of adjacent rows in the source Nz cube for use by the L0C move-out interface with Nz2DN enabled. This takes effect only on AIC."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "This configuration is used only when enable_nz2dn of the move-out interface is true"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: configures
    condition: "This configuration is used only when enable_nz2dn of the move-out interface is true"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: configures
    condition: "This configuration is used only when enable_nz2dn of the move-out interface is true"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: related
    condition: "The number of cubes and the cube interval are configured by this API"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2DN.md
    kind: related
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "Renamed from asc_set_l0c2gm_channel_para (provided in 9.1.0, with the parameter uint64_t config) to asc_set_l0c_copy_channel_para, with the parameter changed to passing uint16_t src_nz_fractal_stride item by item; in the Release Notes CANN_9_2_0 rename table, it is recorded as \"function name changed; the uint64_t config parameter changed to item-by-item passing\", and the old name is retained as a deprecated_interface page."
---
# asc_set_l0c_copy_channel_para

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:48:15.732Z pushedAt=2026-11-18T08:15:34.350Z nl=2 -->

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

This API is used to configure the address offset of adjacent rows in the source Nz cube when the L0C Buffer is moved out and Nz2DN format conversion is performed. After the configuration is written, it is used by the subsequent [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md), or [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md) APIs that enable the Nz2DN function. When the Nz2DN function is not enabled, this configuration does not take effect.

This API is a cube move-out related configuration API and takes effect only on the AIC.

For the correspondence between `src_nz_fractal_stride` and the source cube layout, see [NZ2DN](../cube_store_key_features/NZ2DN.md). The number of cubes and the cube interval are configured by [asc_set_l0c_copy_nz_para](asc_set_l0c_copy_nz_para.md).

## Prototype

```c
__aicore__ inline void asc_set_l0c_copy_channel_para(uint16_t src_nz_fractal_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| src_nz_fractal_stride | Input | The address offset of adjacent rows in the source cube Nz fractal for Nz2DN format conversion configuration. The value range is [1, 65535], in units of 64 bytes (`16 × sizeof(dtype)`, where `dtype` is the data type on the L0C Buffer, and `int32_t` and `float` are supported). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- If this API is not called on the AIC, it returns directly.
- This API must be called before the corresponding L0C Buffer move-out API is executed, and the move-out API must enable the Nz2DN function, that is, the `enable_nz2dn` parameter of the move-out API must be set to `true`.
- When the Nz2DN function is enabled, `src_nz_fractal_stride` cannot be 0.
- When the Nz2DN function is enabled, if `src_nz_fractal_stride` is not equal to 1, UnitFlag cannot be enabled at the same time.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The following call example code is supported only by the Ascend 950PR&950DT products.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 16;
constexpr uint32_t K = 16;
constexpr uint32_t N = 32;
constexpr uint32_t A_ELEMENTS = M * K;
constexpr uint32_t B_ELEMENTS = K * N;
constexpr uint32_t C_ELEMENTS = M * N;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00;

__global__ __cube__ void asc_set_l0c_copy_channel_para_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[A_ELEMENTS];
    __cbuf__ half b_l1[B_ELEMENTS];
    __ca__ half a_l0[A_ELEMENTS];
    __cb__ half b_l0[B_ELEMENTS];
    __cc__ float c_l0[C_ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), K * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), N * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, K, N, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 2, 1, 2);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_channel_para(1);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, M, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, false, true, false);
    asc_sync();
}

void print_row(const char* label, const std::vector<float>& data, uint32_t offset)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[offset + i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(A_ELEMENTS, 0);
    std::vector<uint16_t> b(B_ELEMENTS, 0);
    std::vector<float> input_b(B_ELEMENTS, 0.0f);
    std::vector<float> output(C_ELEMENTS, 0.0f);
    std::vector<float> golden(C_ELEMENTS, 0.0f);
    for (uint32_t i = 0; i < M; ++i) {
        a[i * K + i] = HALF_ONE;
        b[i * N + i] = HALF_ONE;
        b[i * N + 16 + i] = HALF_NEG_ONE;
        input_b[i * N + i] = 1.0f;
        input_b[i * N + 16 + i] = -1.0f;
        golden[i * M + i] = 1.0f;
        golden[(16 + i) * M + i] = -1.0f;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_l0c_copy_channel_para_kernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input B row 0", input_b, 0);
    print_row("DN output row 0", output, 0);
    print_row("DN output row 16", output, 16 * M);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c_copy_channel_para passed."
                         : "[Failed] asc_set_l0c_copy_channel_para failed.") << std::endl;
    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
