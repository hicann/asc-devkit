---
okftype: reference.api
description: "Deprecated API page: Under the 3510 architecture, asc_set_l0c2gm_nz2nd configures the parameters required for the follow-up Nz-to-ND conversion during L0C copy-out using the number of ND matrices and the source/destination ND strides. It must be called before the copy-out API and takes effect only on AIC. It is recommended to use asc_set_l0c_copy_nz_para instead."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "This API must be called to configure before enabling follow-up Nz-to-ND conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: configures
    condition: "This API must be called to configure before enabling follow-up Nz-to-ND conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: configures
    condition: "This API must be called to configure before enabling follow-up Nz-to-ND conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires switching to the strongly typed overload that passes parameters individually"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 deprecation table lists asc_set_l0c2gm_nz2nd as deprecated, replaced by asc_set_l0c_copy_nz_para; the interface remains available."
---
# asc_set_l0c2gm_nz2nd (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:51:55.291Z pushedAt=2026-11-18T03:12:16.663Z nl=2 -->

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

**The `asc_set_l0c2gm_nz2nd` API is deprecated. Use the [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) API instead.**

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

This API is used to configure the parameters required for format conversion when the L0C Buffer move-out APIs [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md), and [asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) enable the follow-up Nz-to-ND format conversion feature.

This API is a matrix move-out related configuration API and takes effect only on the AIC.

## Prototype

```c
__aicore__ inline void asc_set_l0c2gm_nz2nd(uint64_t nd_num,
                                            uint64_t src_nd_stride,
                                            uint64_t dst_nd_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| nd_num | Input | Number of ND matrices. Value range: [1, 65535]. |
| src_nd_stride | Input | Offset between adjacent source Nz matrices (the interval between the heads of adjacent Nz matrices). When nd_num is configured as 1, src_nd_stride does not take effect and can be configured as 0. Value range: [0, 65535], in units of 64 bytes (`16 × sizeof(dtype)`, where `dtype` is the data type on the L0C Buffer, supporting `int32_t` and `float`). |
| dst_nd_stride | Input | Offset between adjacent destination ND matrices (the interval between the heads of adjacent ND matrices). When nd_num is configured as 1, dst_nd_stride does not take effect and can be configured as 0. Value range: [0, 2^32-1], in units of elements. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API takes effect only on the AIC. Calling it on the AIV returns directly.
- Before calling the L0C Buffer move-out APIs [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md), and [asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) and performing the follow-up Nz-to-ND format conversion, you must first call this API to configure the format conversion parameters.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

__global__ __cube__ void asc_set_l0c2gm_nz2nd_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), DIM * sizeof(half), 0, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), DIM * sizeof(half), 0, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c2gm_nz2nd(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
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
    asc_set_l0c2gm_nz2nd_kernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("ND output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c2gm_nz2nd produced row-major ND output."
                         : "[Failed] asc_set_l0c2gm_nz2nd result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
