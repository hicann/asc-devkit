---
okftype: reference.api
description: "Disables the HiF8 mode of Mmad computation. When both the left cube A and the right cube B are fp8_e4m3fn_t, the L0A Buffer and L0B Buffer data are no longer converted to hifloat8_t, but instead directly participate in cube multiplication as FP8; must be called before asc_mmad, and is supported only on 950PR/950DT."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: constrained_by
    condition: "Must be called before the cube multiply-accumulate instruction is executed"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad_mx.md
    kind: constrained_by
    condition: "Must be called before the cube multiply-accumulate instruction is executed"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_enable_hif8.md
    kind: related
    condition: "Has the opposite effect; to re-enable the HiF8 mode, call the counterpart interface"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "The parameterless interface asc_disable_hif8 with clear semantics replaces asc_enable_fp8, which was deprecated in 9.2.0: both implementations clear HIF8_MODE_BIT (bit 45 of the control register), and both signatures are parameterless with no return value; the basis is the impl comparison and ASC_DEPRECATED(9.2.0, ..., asc_disable_hif8)."
---
# asc_disable_hif8

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:10:28.570Z pushedAt=2026-11-18T08:15:34.219Z nl=2 -->

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

The header file path is: `"c_api/cube_compute/cube_compute.h"`.

This interface is used to disable HiF8 mode for Mmad computation. Its effect is opposite to that of [asc_enable_hif8](asc_enable_hif8.md), and the two interfaces do not take effect simultaneously. After HiF8 mode is disabled, when both the left cube A and the right cube B of a cube multiply-accumulate instruction use `fp8_e4m3fn_t` as the input data type, the data in the L0A Buffer and L0B Buffer is not converted to `hifloat8_t` before participating in the cube multiplication operation; instead, it directly participates in the computation as the `fp8_e4m3fn_t` type.

This interface is a cube computation related configuration interface and takes effect only on the AIC.

## Prototype

```c
__aicore__ inline void asc_disable_hif8()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This interface must be called before the cube multiply-accumulate instruction ([asc_mmad](../mmad_compute/asc_mmad.md), [asc_mmad_mx](../mmad_compute/asc_mmad_mx.md)) is executed, to ensure that the mode configuration takes effect before the cube multiply-accumulate result is generated.
- After HiF8 mode is disabled, it remains in effect. Subsequent cube multiply-accumulate instructions will continue to use the current mode unless it is explicitly reconfigured. To enable HiF8 mode, call the [asc_enable_hif8](asc_enable_hif8.md) interface again.
- This interface takes effect only when the cube multiply-accumulate input data type is `fp8_e4m3fn_t`×`fp8_e4m3fn_t`. For other FP8 data type combinations (`fp8_e4m3fn_t`×`fp8_e5m2_t`, `fp8_e5m2_t`×`fp8_e4m3fn_t`, `fp8_e5m2_t`×`fp8_e5m2_t`), HiF8 mode selection is not supported, and calling this interface has no actual effect.

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
constexpr uint32_t K = 32;
constexpr uint32_t N = 16;
constexpr uint32_t A_ELEMENTS = M * K;
constexpr uint32_t B_ELEMENTS = N * K;
constexpr uint32_t C_ELEMENTS = M * N;
constexpr uint8_t FP8_E4M3_18 = 0x59;
constexpr float FP8_RESULT = 324.0f;
constexpr float HIF8_RESULT = 25.0f / 65536.0f;

__global__ __cube__ void asc_disable_hif8_kernel(
    __gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ float* hif8_output, __gm__ float* fp8_output)
{
    asc_init();
    __cbuf__ fp8_e4m3fn_t a_l1[A_ELEMENTS], b_l1[B_ELEMENTS];
    __ca__ fp8_e4m3fn_t a_l0[A_ELEMENTS];
    __cb__ fp8_e4m3fn_t b_l0[B_ELEMENTS];
    __cc__ float hif8_l0[C_ELEMENTS], fp8_l0[C_ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(a), K,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(b), K,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_enable_hif8();
    asc_mmad(hif8_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_disable_hif8();
    asc_mmad(fp8_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(hif8_output, hif8_l0, N, M, N, M,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, asc_quant_mode::NoQuant,
        asc_relu_pre_mode::NONE, false, true, false, false);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(fp8_output, fp8_l0, N, M, N, M,
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
    std::vector<uint8_t> a(A_ELEMENTS), b(B_ELEMENTS);
    std::vector<float> hif8(C_ELEMENTS), fp8(C_ELEMENTS), hif8_golden(C_ELEMENTS), fp8_golden(C_ELEMENTS);
    for (uint32_t i = 0; i < M; ++i) {
        a[i * K + i] = FP8_E4M3_18;
        b[i * K + i] = FP8_E4M3_18;
        hif8_golden[i * N + i] = HIF8_RESULT;
        fp8_golden[i * N + i] = FP8_RESULT;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *a_device = nullptr, *b_device = nullptr;
    float *hif8_device = nullptr, *fp8_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&hif8_device), hif8.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&fp8_device), fp8.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_disable_hif8_kernel<<<1, 0>>>(a_device, b_device, hif8_device, fp8_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(hif8.data(), hif8.size() * sizeof(float), hif8_device, hif8.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(fp8.data(), fp8.size() * sizeof(float), fp8_device, fp8.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("HiF8 output row 0", hif8);
    print_row("Restored FP8 output row 0", fp8);
    print_row("HiF8 golden row 0", hif8_golden);
    print_row("FP8 golden row 0", fp8_golden);
    const bool passed = hif8 == hif8_golden && fp8 == fp8_golden;
    std::cout << (passed ? "[Success] asc_disable_hif8 restores FP8 MMAD."
                         : "[Failed] asc_disable_hif8 result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(hif8_device); aclrtFree(fp8_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
