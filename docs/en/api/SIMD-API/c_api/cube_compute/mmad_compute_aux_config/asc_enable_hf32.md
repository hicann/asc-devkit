---
okftype: reference.api
description: "Enables HF32 mode for Mmad computation. FP32 data in the L0A Buffer and L0B Buffer is rounded to HF32 before participating in cube multiply-accumulate, improving FP32 computation performance while results are still output as FP32. It must be called before asc_mmad, and the rounding mode is configured by asc_set_hf32_round_mode."
tag:
  - roles: [configuration]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: constrained_by
    condition: "must be called before the cube multiply-accumulate instruction executes"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_disable_hf32.md
    kind: related
    condition: "to disable HF32 mode, call the counterpart interface"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md
    kind: related
    condition: "must be called after this interface to configure the rounding mode"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_key_features/HF32.md
    kind: related
    condition: "Description of HF32 format, rounding mode, and precision impact"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "9.0.0 is declared and implemented only in the 2201 branch; 9.1.0 adds a declaration and implementation with the same signature in the __NPU_ARCH__==3510 branch of include and in impl/c_api/instr_impl/npu_arch_3510/cube_compute_impl.h. Ascend 950PR&950DT (dav-3510) is available since 9.1.0, with unchanged interface semantics and signature."
---
# asc_enable_hf32

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:10:56.248Z pushedAt=2026-11-18T08:15:34.220Z nl=2 -->

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

This interface is used to enable HF32 mode for Mmad computation. After this mode is enabled, the performance of Mmad computation on FP32 data is improved, but a certain degree of precision loss is introduced.

The impact of enabling HF32 on the theoretical computation performance of Mmad is shown in the following table:

<!-- npu="A3,910b" id8 -->
**Table 1** Impact of HF32 on the theoretical computation performance of Mmad ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123815311"></a>

| Interface | Left cube A | Right cube B | $cube_m$ | $cube_n$ | $cube_k$ | $k_0$ |
| --- | --- | --- | --- | --- | --- | --- |
| asc_mmad (HF32 disabled) | float | float | 16 | 16 | 4 | 8 |
| asc_mmad (HF32 enabled) | float | float | 16 | 16 | 8 | 8 |

<!-- end id8 -->

<!-- npu="950" id9 -->
**Table 2** Impact of HF32 on the theoretical computation performance of Mmad ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123815915"></a>

| Interface | Left cube A | Right cube B | $cube_m$ | $cube_n$ | $cube_k$ | $k_0$ |
| --- | --- | --- | --- | --- | --- | --- |
| asc_mmad (HF32 disabled) | float | float | 16 | 16 | 1 | 8 |
| asc_mmad (HF32 enabled) | float | float | 16 | 16 | 8 | 8 |

<!-- end id9 -->

The performance calculation formula is as follows:

$$
\begin{gathered}
{ceil_m} = \left\lceil \frac{m}{16} \right\rceil \times 16 \\[12pt]
{ceil_n} = \left\lceil \frac{n}{16} \right\rceil \times 16 \\[12pt]
{ceil_k} = \left\lceil \frac{k}{k_0} \right\rceil \times k_0 \\[16pt]
\text{cube utilization} =
\frac{ (m \times n \times k) / ({cube_m} \times {cube_n} \times {cube_k}) }
{ \Delta t + ({ceil_m} \times {ceil_n} \times {ceil_k}) / ({cube_m} \times {cube_n} \times {cube_k}) }
\end{gathered}
$$

Key variables and constants:

- $m, n, k$: actual computation sizes of the mmad input parameters.
- $ceil_m, ceil_n, ceil_k$: values of $m, n, k$ after upward alignment to the fractal size.
- $cube_m, cube_n, cube_k$: actual hardware parallelism (unit: elements/cycle).
- $k_0$: minimum fractal size in the K direction on the L0 Buffer.
- $\Delta t$: number of head overhead cycles.

After HF32 mode is enabled, the FP32 data in the L0A Buffer/L0B Buffer is rounded to the HF32 format before participating in the Mmad computation. The rounding mode is configured through the [asc_set_hf32_round_mode](asc_set_hf32_round_mode.md) interface. Intermediate computation uses the HF32 format, and the final operation result is still output in the FP32 format to ensure compatibility with subsequent processing.

The accuracy comparison between the FP32 and HF32 formats is shown in the following figure:

<!-- npu="A3,910b" id10 -->
**Figure 1** Schematic diagram of the precision of the FP32 and HF32 formats ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh_cn_topic_hf32_figure1"></a>

![Schematic diagram of the precision of the FP32 and HF32 formats (NPU architecture version 2201)](../../../../figures/mmad_hf32.png "Schematic diagram of the precision of the FP32 and HF32 formats (NPU architecture version 2201)")
<!-- end id10 -->

<!-- npu="950" id11 -->
**Figure 2** FP32 and HF32 format precision schematic diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh_cn_topic_hf32_figure2"></a>

![FP32 and HF32 format precision schematic diagram (NPU architecture version 3510)](../../../../figures/mmad_hf32_950.png "FP32 and HF32 format precision schematic diagram (NPU architecture version 3510)")
<!-- end id11 -->

For the HF32 format, rounding mode, and their impact on computation precision, see [HF32 Feature Description](../mmad_compute_key_features/HF32.md).

## Prototype

```c
__aicore__ inline void asc_enable_hf32()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This interface must be called before the cube multiply-accumulate instruction ([asc_mmad](../mmad_compute/asc_mmad.md)) is executed, to ensure that the mode configuration takes effect during the cube multiply-accumulate computation.
- After HF32 mode is enabled, it remains effective and is not automatically disabled. Subsequent cube multiply-accumulate instructions will continue to use the current mode unless explicitly reconfigured. To disable HF32 mode, call the [asc_disable_hf32](asc_disable_hf32.md) interface again.
- After HF32 mode is enabled, the rounding mode for converting FP32 to HF32 format is configured by the companion [asc_set_hf32_round_mode](asc_set_hf32_round_mode.md) interface. This companion interface must be called after this interface to configure the rounding mode; otherwise, the rounding mode will continue to use the previous configuration or the default value.
- HF32 mode takes effect only for cube multiply-accumulate operations where the input cube data type is `float`.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id12 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id12 -->

The following call example code is supported only by the Ascend 950PR&950DT products.

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16;
constexpr uint32_t ELEMENTS = DIM * DIM;

__global__ __cube__ void asc_enable_hf32_kernel(
    __gm__ float* a, __gm__ float* b, __gm__ float* fp32_output, __gm__ float* hf32_output)
{
    asc_init();
    __cbuf__ float a_l1[ELEMENTS];
    __cbuf__ float b_l1[ELEMENTS];
    __ca__ float a_l0[ELEMENTS];
    __cb__ float b_l0[ELEMENTS];
    __cc__ float fp32_l0[ELEMENTS];
    __cc__ float hf32_l0[ELEMENTS];

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
    asc_disable_hf32();
    asc_mmad(fp32_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_enable_hf32();
    asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_EVEN);
    asc_mmad(hf32_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(fp32_output, fp32_l0, DIM, DIM, DIM, DIM,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, asc_quant_mode::NoQuant,
        asc_relu_pre_mode::NONE, false, true, false, false);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(hf32_output, hf32_l0, DIM, DIM, DIM, DIM,
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
    std::vector<float> a(ELEMENTS, 0.0f), b(ELEMENTS, 0.0f);
    std::vector<float> fp32(ELEMENTS, 0.0f), hf32(ELEMENTS, 0.0f), golden(ELEMENTS, 0.0f);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = 1.0003f;
        b[i * DIM + i] = 1.0f;
        golden[i * DIM + i] = 1.0003f;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *a_device = nullptr, *b_device = nullptr, *fp32_device = nullptr, *hf32_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&fp32_device), fp32.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&hf32_device), hf32.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(float), a.data(), a.size() * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(float), b.data(), b.size() * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_enable_hf32_kernel<<<1, 0>>>(a_device, b_device, fp32_device, hf32_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(fp32.data(), fp32.size() * sizeof(float), fp32_device, fp32.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(hf32.data(), hf32.size() * sizeof(float), hf32_device, hf32.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_row("Input A row 0", a);
    print_row("FP32 output row 0", fp32);
    print_row("HF32 output row 0", hf32);
    print_row("FP32 golden row 0", golden);
    bool passed = true;
    for (uint32_t i = 0; i < ELEMENTS; ++i) passed &= std::fabs(fp32[i] - golden[i]) < 1e-6f;
    passed &= std::fabs(hf32[0] - fp32[0]) > 1e-5f;
    std::cout << (passed ? "[Success] asc_enable_hf32 shows HF32 rounding."
                         : "[Failed] asc_enable_hf32 result mismatch.") << std::endl;
    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(fp32_device);
    aclrtFree(hf32_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
