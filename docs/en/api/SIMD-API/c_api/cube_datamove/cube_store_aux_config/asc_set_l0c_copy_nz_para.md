---
okftype: reference.api
description: "When moving out of the L0C Buffer with the accompanying Nz-to-ND or Nz-to-DN format conversion enabled, configure the number of cubes, the offset between adjacent source Nz cubes, and the offset between adjacent destination ND or DN cubes. Supported only on Ascend 950PR&950DT."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "This interface must be called first when enable_nz2nd or enable_nz2dn of the move-out interface is true"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: configures
    condition: "This interface must be called first when enable_nz2nd or enable_nz2dn of the move-out interface is true"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: configures
    condition: "This interface must be called first when enable_nz2nd or enable_nz2dn of the move-out interface is true"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md
    kind: related
    condition: "For the Nz2DN scenario, this interface must also be used to set the source Nz fractal row interval"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2DN.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "Changed from asc_set_l0c2gm_nz2nd (provided in 9.0.0, with parameters assembled into config) to asc_set_l0c_copy_nz_para(matrix_num, src_nz_matrix_stride, dst_matrix_stride). The number, types, and units of parameters are adjusted accordingly (the source stride unit is 64 bytes, and the destination stride unit is elements). The old name is retained as a deprecated_interface page."
---
# asc_set_l0c_copy_nz_para

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:49:55.634Z pushedAt=2026-11-18T08:15:34.353Z nl=2 -->

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

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

This interface is used to configure the parameters required for format conversion when the L0C Buffer move-out interfaces [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md), and [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md) enable the in-band Nz-to-ND/DN format conversion feature.

This interface is a cube move-out related configuration interface and takes effect only on the AIC.

For diagrams of the cube count and the source and destination cube strides, see [NZ2ND](../cube_store_key_features/NZ2ND.md) and [NZ2DN](../cube_store_key_features/NZ2DN.md). For the Nz2DN scenario, you also need to use [asc_set_l0c_copy_channel_para](asc_set_l0c_copy_channel_para.md) to set the source Nz fractal row stride.

## Prototype

```c
__aicore__ inline void asc_set_l0c_copy_nz_para(uint16_t matrix_num,
                                                uint16_t src_nz_matrix_stride,
                                                uint32_t dst_matrix_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| matrix_num | Input | Number of ND/DN cubes. The value range is [1, 65535]. |
| src_nz_matrix_stride | Input | Offset between adjacent source Nz cubes (the head-to-head stride between adjacent Nz cubes). When matrix_num is configured as 1, src_nz_matrix_stride does not take effect and can be configured as 0. The value range is [0, 65535], in units of 64 bytes (`16 × sizeof(dtype)`, where `dtype` is the data type on the L0C Buffer, supporting `int32_t` and `float`). |
| dst_matrix_stride | Input | Offset between adjacent destination ND/DN cubes (the head-to-head stride between adjacent ND/DN cubes). When matrix_num is configured as 1, dst_matrix_stride does not take effect and can be configured as 0. The value range is [0, 2^32-1], in units of elements. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- If this interface is not called by the AIC, it returns directly.
- Before calling the L0C Buffer move-out interfaces [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md), and [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md) and when in-band Nz-to-ND/DN format conversion is required, that is, when either `enable_nz2nd` or `enable_nz2dn` in the move-out interface is set to `true`, you must call this interface first to configure the format conversion parameters.

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
constexpr uint32_t DIM = 16, ELEMENTS = DIM * DIM;
constexpr uint16_t HALF_ONE = 0x3c00;

__global__ __cube__ void asc_set_l0c_copy_nz_para_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), DIM * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), DIM * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync();
}

void print_row(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<float> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = HALF_ONE;
        b[i * DIM + i] = HALF_ONE;
        golden[i * DIM + i] = 1.0f;
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
    asc_set_l0c_copy_nz_para_kernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("ND output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c_copy_nz_para produced row-major ND output."
                         : "[Failed] asc_set_l0c_copy_nz_para result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
