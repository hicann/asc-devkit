---
okftype: reference.api
description: "Sets the rounding mode for FP32-to-HF32 conversion in HF32 mode, selecting between NEAREST_AWAY and NEAREST_EVEN via the asc_hf32_round_mode enum; must be called after enabling HF32 mode and before asc_mmad, and defaults to NEAREST_EVEN when not configured."
tag:
  - roles: [configuration]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: constrained_by
    condition: "Must be called before the cube multiply-accumulate instruction executes"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_enable_hf32.md
    kind: requires
    condition: "HF32 mode must be enabled first; otherwise the configuration has no actual effect"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_key_features/HF32.md
    kind: related
    condition: "Description of the HF32 format, rounding mode, and precision impact"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_hf32_round_mode.md
    kind: requires
    condition: "Input parameter type enumeration, with values NEAREST_AWAY and NEAREST_EVEN"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "Replaces asc_enable_hf32_trans(uint32_t mode), which was deprecated in 9.2.0: it writes to the same HF32_TRANS_MODE_BIT (bit 47 of the control register), but the input parameter changes from uint32_t 0/1 to the asc_hf32_round_mode enumeration (NEAREST_AWAY, NEAREST_EVEN; the old NEAREST_ZERO is retained as an alias of NEAREST_AWAY); the basis is a comparison of the two implementations, ASC_DEPRECATED(9.2.0, ..., asc_set_hf32_round_mode), and the unified enumeration description in CANN_9_2_0.md."
---
# asc_set_hf32_round_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:14:31.096Z pushedAt=2026-11-18T08:15:34.224Z nl=2 -->

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

The header file path is: `"c_api/cube_compute/cube_compute.h"`.

This API is used to set the rounding mode for HF32 mode. Before using this API, you need to call [asc_enable_hf32](asc_enable_hf32.md) to enable HF32 mode.

For details about the HF32 format, rounding modes, and their impact on computation precision, see [HF32 Feature Description](../mmad_compute_key_features/HF32.md).

## Prototype

```c
__aicore__ inline void asc_set_hf32_round_mode(asc_hf32_round_mode hf32_round_mode)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|:-------|:----------|:------|
| hf32_round_mode | Input | HF32 rounding mode control input parameter, of the [asc_hf32_round_mode](../../defs/enum/asc_hf32_round_mode.md) enumeration type. It supports the following two enumeration values:<br>&nbsp;&nbsp;&bull; NEAREST_AWAY: FP32 is rounded to HF32 by rounding to the nearest value, with ties away from zero.<br>&nbsp;&nbsp;&bull; NEAREST_EVEN: FP32 is rounded to HF32 by rounding to the nearest value, with ties to even. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- After HF32 mode is enabled, if this API is not called, the rounding mode represented by `NEAREST_EVEN` is used by default.
- This API must be called before the cube multiply-accumulate instruction ([asc_mmad](../mmad_compute/asc_mmad.md)) is executed, to ensure that the mode configuration takes effect during the cube multiply-accumulate computation.
- The rounding mode configured by this API takes effect only while HF32 mode is enabled. If HF32 mode is not enabled, the rounding mode configuration has no actual effect.
- The rounding mode configuration remains in effect after being configured. If HF32 mode is disabled and then enabled again, the last rounding mode configuration is still used. To switch the rounding mode, call this API again.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

__global__ __cube__ void asc_set_hf32_round_mode_kernel(
    __gm__ float* a, __gm__ float* b, __gm__ float* output, asc_hf32_round_mode round_mode)
{
    asc_init();
    __cbuf__ float a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ float a_l0[ELEMENTS];
    __cb__ float b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, DIM * sizeof(float), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, DIM * sizeof(float), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 2, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 2, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_enable_hf32();
    asc_set_hf32_round_mode(round_mode);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, asc_quant_mode::NoQuant,
        asc_relu_pre_mode::NONE, false, true, false, false);
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
    asc_set_hf32_round_mode_kernel<<<1, 0>>>(
        a_device, b_device, away_device, asc_hf32_round_mode::NEAREST_AWAY);
    aclrtSynchronizeDevice();
    asc_set_hf32_round_mode_kernel<<<1, 0>>>(
        a_device, b_device, even_device, asc_hf32_round_mode::NEAREST_EVEN);
    aclrtSynchronizeDevice();
    aclrtMemcpy(nearest_away.data(), ELEMENTS * sizeof(float), away_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(nearest_even.data(), ELEMENTS * sizeof(float), even_device, ELEMENTS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input tie row 0", a);
    print_row("NEAREST_AWAY row 0", nearest_away);
    print_row("NEAREST_EVEN row 0", nearest_even);
    const bool passed = nearest_away == away_golden && nearest_even == even_golden;
    std::cout << (passed ? "[Success] asc_set_hf32_round_mode selects tie rounding."
                         : "[Failed] asc_set_hf32_round_mode result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(away_device); aclrtFree(even_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
