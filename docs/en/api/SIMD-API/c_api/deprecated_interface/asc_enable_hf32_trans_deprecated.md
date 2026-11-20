---
okftype: reference.api
description: "Deprecated HF32 rounding mode configuration interface. Selects the tie-breaking rule of round to nearest even or round away from zero based on mode. HF32 mode must be enabled first and this interface must be called before matrix multiply-accumulate. The replacement is asc_set_hf32_round_mode."
tag:
  - roles: [configuration]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: requires
    condition: "Must be called before the matrix multiply-accumulate instruction is executed"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_enable_hf32.md
    kind: requires
    condition: "The rounding mode takes effect only while HF32 is enabled. HF32 mode must be enabled first"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md
    kind: replaced_by
    condition: "Use a strongly typed enum to configure the HF32 rounding mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked ASC_DEPRECATED(9.2.0, 2027/09/07, asc_set_hf32_round_mode); the CANN_9_2_0 Release Notes deprecation table states that enums should be used to configure the HF32 rounding mode; the interface is retained and not removed."
---
# asc_enable_hf32_trans (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:53:25.841Z pushedAt=2026-11-18T03:12:16.473Z nl=2 -->

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

**The `asc_enable_hf32_trans` interface is deprecated. Use the [asc_set_hf32_round_mode](../cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md) interface instead.**

The header file path is `"c_api/cube_compute/cube_compute.h"`.

This interface is used to set the HF32 mode rounding mode. Before using this interface, you need to enable HF32 mode by using [asc_enable_hf32](../cube_compute/mmad_compute_aux_config/asc_enable_hf32.md).

## Prototype

```c
__aicore__ inline void asc_enable_hf32_trans(uint32_t mode)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|:-------|:----------|:------|
| mode | Input | HF32 rounding mode control parameter, of the uint32_t type. The following two values are supported:<br>&nbsp;&nbsp;&bull; 0: FP32 is rounded to HF32 by rounding to the nearest value, with ties rounded to even.<br>&nbsp;&nbsp;&bull; 1: FP32 is rounded to HF32 by rounding to the nearest value, with ties rounded away from zero. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This interface must be called before the matrix multiply-accumulate instruction ([asc_mmad](../cube_compute/mmad_compute/asc_mmad.md)) is executed, to ensure that the mode configuration takes effect during the matrix multiply-accumulate computation.
- The rounding mode configured by this interface takes effect only while HF32 mode is enabled. You must enable HF32 mode before calling this interface; otherwise, the rounding mode configuration has no actual effect.
- After the rounding mode is configured, it remains in effect. If HF32 mode is disabled and then enabled again, the previous rounding mode configuration is still used. To switch the rounding mode, call this interface again.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The following sample code is supported only on the Ascend 950PR&950DT products.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16, ELEMENTS = DIM * DIM;

__global__ __cube__ void asc_enable_hf32_trans_kernel(
    __gm__ float* a, __gm__ float* b, __gm__ float* output, uint32_t nearest_away)
{
    asc_init();
    __cbuf__ float a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ float a_l0[ELEMENTS];
    __cb__ float b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
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
    asc_enable_hf32_trans(nearest_away ? 1 : 0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
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
    constexpr float tie = 1.0f + 0.5f / 1024.0f;
    std::vector<float> a(ELEMENTS), b(ELEMENTS), nearest_away(ELEMENTS), nearest_even(ELEMENTS);
    std::vector<float> away_golden(ELEMENTS), even_golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = tie;
        b[i * DIM + i] = 1.0f;
        away_golden[i * DIM + i] = 1.0f + 1.0f / 1024.0f;
        even_golden[i * DIM + i] = 1.0f;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *a_device = nullptr, *b_device = nullptr, *away_device = nullptr, *even_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&away_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&even_device), ELEMENTS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, ELEMENTS * sizeof(float), a.data(), ELEMENTS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, ELEMENTS * sizeof(float), b.data(), ELEMENTS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_enable_hf32_trans_kernel<<<1, 0>>>(a_device, b_device, away_device, 1);
    aclrtSynchronizeDevice();
    asc_enable_hf32_trans_kernel<<<1, 0>>>(a_device, b_device, even_device, 0);
    aclrtSynchronizeDevice();
    aclrtMemcpy(nearest_away.data(), ELEMENTS * sizeof(float), away_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(nearest_even.data(), ELEMENTS * sizeof(float), even_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input tie row 0", a);
    print_row("NEAREST_AWAY row 0", nearest_away);
    print_row("NEAREST_EVEN row 0", nearest_even);
    const bool passed = nearest_away == away_golden && nearest_even == even_golden;
    std::cout << (passed ? "[Success] asc_enable_hf32_trans selects tie rounding."
                         : "[Failed] asc_enable_hf32_trans result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(away_device); aclrtFree(even_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
