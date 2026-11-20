---
okftype: reference.api
description: "Deprecated matrix multiply-accumulate mode configuration API, used to disable HF32 mode so that float data in L0A and L0B is not rounded before participating in Mmad computation. The replacement is asc_disable_hf32."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: constrained_by
    condition: "Must be called before asc_mmad is executed to take effect"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_disable_hf32.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_enable_hf32.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "New 3510 (Ascend 950PR&950DT) implementation: history_probe shows that asc_set_fp32_mode in npu_arch_3510/cube_compute_impl.h has appeared since 9.1.0, and the CANN_9_1_0 new features list includes this API; in 9.0.0 it was available only on 2201 (Atlas A2/A3)."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The API is deprecated; use the semantically clear asc_disable_hf32 instead. The declaration starting at include/c_api/cube_compute/cube_compute.h:1135 carries ASC_DEPRECATED(9.2.0, asc_disable_hf32), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_set_fp32_mode (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:47:47.521Z pushedAt=2026-11-18T03:12:16.658Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

**The `asc_set_fp32_mode` interface is deprecated. Use [asc_disable_hf32](../cube_compute/mmad_compute_aux_config/asc_disable_hf32.md) instead.**

The header file path is `"c_api/cube_compute/cube_compute.h"`.

This interface is used to disable the HF32 mode for Mmad computation. Its effect is opposite to that of [asc_enable_hf32](../cube_compute/mmad_compute_aux_config/asc_enable_hf32.md), and the two interfaces do not take effect at the same time. After the HF32 mode is disabled, the `float` data in the L0A Buffer and L0B Buffer is not rounded before participating in Mmad computation.

## Prototype

```c
__aicore__ inline void asc_set_fp32_mode()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This interface must be called before the matrix multiply-accumulate instruction ([asc_mmad](../cube_compute/mmad_compute/asc_mmad.md)) is executed, to ensure that the mode configuration takes effect during the matrix multiply-accumulate computation.
- Once enabled, the FP32 mode remains in effect and is not automatically disabled. Subsequent matrix multiply-accumulate instructions will keep using the current mode unless explicitly reconfigured. To enable the HF32 mode, call the [asc_enable_hf32](../cube_compute/mmad_compute_aux_config/asc_enable_hf32.md) interface again.

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
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16, ELEMENTS = DIM * DIM;

__global__ __cube__ void asc_set_fp32_mode_kernel(
    __gm__ float* a, __gm__ float* b, __gm__ float* hf32_output, __gm__ float* fp32_output)
{
    asc_init();
    __cbuf__ float a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ float a_l0[ELEMENTS];
    __cb__ float b_l0[ELEMENTS];
    __cc__ float hf32_l0[ELEMENTS], fp32_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, DIM * sizeof(float), 0, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, DIM * sizeof(float), 0, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 2, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 2, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_enable_hf32();
    asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_EVEN);
    asc_mmad(hf32_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_set_fp32_mode();
    asc_mmad(fp32_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(hf32_output, hf32_l0, DIM, DIM, DIM, DIM, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(fp32_output, fp32_l0, DIM, DIM, DIM, DIM, 0, 0, 0,
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
    std::vector<float> a(ELEMENTS), b(ELEMENTS), hf32(ELEMENTS), fp32(ELEMENTS), golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = golden[i * DIM + i] = 1.0003f;
        b[i * DIM + i] = 1.0f;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *a_device = nullptr, *b_device = nullptr, *hf32_device = nullptr, *fp32_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&hf32_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&fp32_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, ELEMENTS * sizeof(float), a.data(), ELEMENTS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, ELEMENTS * sizeof(float), b.data(), ELEMENTS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_fp32_mode_kernel<<<1, 0>>>(a_device, b_device, hf32_device, fp32_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(hf32.data(), ELEMENTS * sizeof(float), hf32_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(fp32.data(), ELEMENTS * sizeof(float), fp32_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input A row 0", a);
    print_row("HF32 output row 0", hf32);
    print_row("Restored FP32 output row 0", fp32);
    print_row("FP32 golden row 0", golden);
    bool passed = fp32 == golden && std::fabs(hf32[0] - fp32[0]) > 1e-5f;
    std::cout << (passed ? "[Success] asc_set_fp32_mode restores full-precision MMAD."
                         : "[Failed] asc_set_fp32_mode result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(hf32_device); aclrtFree(fp32_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
