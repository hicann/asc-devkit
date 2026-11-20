---
okftype: reference.api
description: "Configures the scale factor required for the accompanying Scalar activation during L0C Buffer eviction. The value uses a high 19-bit format with 1 sign bit, 8 exponent bits, and 10 mantissa bits, and must be used in conjunction with the activation mode of the eviction interface."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "must be used in conjunction with the relu_pre_mode of the eviction interface"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: configures
    condition: "must be used in conjunction with the relu_pre_mode of the eviction interface"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: configures
    condition: "must be used in conjunction with the relu_pre_mode of the eviction interface"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_relu.md
    kind: related
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "Renamed from asc_set_l0c2gm_lrelu_alpha (9.1.0 provided both half& and float& parameter forms) to asc_set_l0c_copy_lrelu_alpha; the new interface retains only the float scalar parameter form. In the Release Notes CANN_9_2_0 rename table, it is recorded as \"interface name change; the new interface supports only the float parameter and no longer provides the half parameter form\", and the old name is retained as a deprecated_interface page."
---
# asc_set_l0c_copy_lrelu_alpha

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:49:45.833Z pushedAt=2026-11-18T08:15:34.352Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

This interface is used to configure the scale factor required for activation computation when performing accompanying Scalar activation during L0C Buffer eviction. After configuration, it is used by the subsequently called [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md), or [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md) interfaces, and must be used in conjunction with the `relu_pre_mode` of the eviction interface.

This interface is a cube eviction-related configuration interface and takes effect only on the AIC.

For activation modes and supporting configurations, see [Accompanying ReLU](../cube_store_key_features/accompanying_relu.md); when used together with quantization, see [Accompanying Quantization and ReLU Scenario Combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md).

## Prototype

```cpp
__aicore__ inline void asc_set_l0c_copy_lrelu_alpha(float scalar_relu_pre_alpha);
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| scalar_relu_pre_alpha | Input | Scalar activation scale factor. Only the upper 19 bits are valid, in the (1, 8, 10) floating-point format, that is, 1-bit sign, 8-bit exponent, and 10-bit mantissa. The value cannot be inf or nan. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This interface takes effect only on the AIC. Calling it on the AIV returns directly.
- This interface must be called before the corresponding L0C Buffer eviction interface is executed, and the corresponding Scalar activation mode must be configured in the eviction interface.
- `scalar_relu_pre_alpha` cannot be inf, nan, or a denormalized number. If the value is 0, both the exponent bits and the mantissa bits must be 0.
- After the call, the configuration remains in effect until this interface is called again to overwrite it. When different eviction tasks use different coefficients, the configuration must be performed again before the corresponding eviction interface is executed.

<!-- npu="950" id8 -->

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

The following call example code is supported only by the Ascend 950PR&950DT products.

```cpp
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16, ELEMENTS = DIM * DIM;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00, HALF_NEG_HALF = 0xb800;

__global__ __cube__ void asc_set_l0c_copy_lrelu_alpha_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ uint16_t* output)
{
    asc_init();
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), DIM * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), DIM * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_lrelu_alpha(0.5f);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(reinterpret_cast<__gm__ half*>(output), c_l0, DIM, DIM, DIM, DIM,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, asc_quant_mode::F322F16,
        asc_relu_pre_mode::SCALAR, false, true, false, false);
    asc_sync();
}

void print_row(const char* label, const std::vector<uint16_t>& data)
{
    std::cout << label << ':' << std::hex;
    for (uint32_t i = 0; i < 8; ++i) std::cout << " 0x" << data[i];
    std::cout << std::dec << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<uint16_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = HALF_ONE;
        b[i * DIM + i] = HALF_NEG_ONE;
        golden[i * DIM + i] = HALF_NEG_HALF;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_l0c_copy_lrelu_alpha_kernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint16_t), output_device, output.size() * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input B row 0 (FP16 bits)", b);
    print_row("Leaky ReLU output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c_copy_lrelu_alpha applied alpha 0.5 to -1.0."
                         : "[Failed] asc_set_l0c_copy_lrelu_alpha result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
