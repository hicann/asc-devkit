---
okftype: reference.api
description: "Configures the stride, count, direction, and destination position parameters required for the right cube repeat mode in 3D img2col copy, for use by asc_copy_l12l0b when f_matrix_ctrl is true. It takes effect only on AIC."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md
    kind: configures
    condition: "This configuration is used only when the f_matrix_ctrl parameter of the 3D copy is true"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_l13d_repeat_direction.md
    kind: requires
    condition: "Used to select the repeat direction in the M or K direction"
version: ">=9.2.0"
version_changes: []
---
# asc_set_l13d_rpt_b

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:41:07.776Z pushedAt=2026-11-18T08:15:34.339Z nl=2 -->

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

This interface is used to configure the parameters required for moving the right cube in repeat mode under the 3D img2col copy mode. The configuration is used by the [asc_copy_l12l0b (3D cube copy)](../cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md) interface when the parameter `f_matrix_ctrl` is set to `true`.

This interface is a cube load related configuration interface and takes effect only on the AIC.

## Prototype

```cpp
__aicore__ inline void asc_set_l13d_rpt_b(uint16_t repeat_stride,
                                          uint8_t repeat_times,
                                          asc_l13d_repeat_direction repeat_direction,
                                          uint16_t dst_stride_k,
                                          uint16_t dst_start_pos_m)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| repeat_stride | Input | Distance between the start positions of two adjacent iterations. Value range: [0, 65535]. When `repeat_direction` is `asc_l13d_repeat_direction::M_DIRECTION`, the unit is 16 elements; when it is `asc_l13d_repeat_direction::K_DIRECTION`, the unit is `32 / sizeof(data_type)` elements, where `data_type` is the data type of the source operand. |
| repeat_times | Input | Number of iterations in the M or K direction. Value range: [0, 255]. `0` means no copy is performed, and the 3D copy interface is treated as a NOP (no operation). |
| repeat_direction | Input | An enumeration value of type [asc_l13d_repeat_direction](../../defs/enum/asc_l13d_repeat_direction.md), indicating the repeat direction.<br>&nbsp;&nbsp;&bull; `asc_l13d_repeat_direction::M_DIRECTION`: iterate along the height (M) direction;<br>&nbsp;&nbsp;&bull; `asc_l13d_repeat_direction::K_DIRECTION`: iterate along the width (K) direction. |
| dst_stride_k | Input | Stride of the destination cube in the K direction. Value range: [0, 65535]. unit: 512-byte fractals. |
| dst_start_pos_m | Input | Start position of the destination cube in the M direction. Value range: [0, 65535]. unit: 512-byte fractals. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This interface takes effect only on the AIC. Calls from non-AIC return directly.
- This interface must be called before the corresponding 3D copy interface. The configuration takes effect only when the parameter `f_matrix_ctrl` of the 3D copy interface is set to `true`.
- After being called, the configuration remains in effect until it is overwritten by calling this interface again. When different 3D copy tasks use different configurations, the configuration must be set again before the corresponding copy interface is executed.

<!-- npu="950" id8 -->

## Examples

The following example uses `asc_copy_l12l0b` to expand a 2×2, 16-channel all-zero Feature Map into a 4×4 cube with a ring of padding, and automatically transposes it into the L0B Buffer. The B-side padding value is set to 2, and `f_matrix_ctrl` is set to `true`. After performing MMAD with an all-ones L0A cube, each output accumulates the 12 padding positions in the 4×4 region, and the expected value is `12 × 2 = 24`.

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t H = 2;
constexpr uint32_t W = 2;
constexpr uint32_t C0 = 16;
constexpr uint32_t CIN = C0;
constexpr uint32_t COUT = C0;
constexpr uint32_t PAD = 1;
constexpr uint32_t HOUT = H + 2 * PAD;
constexpr uint32_t WOUT = W + 2 * PAD;
constexpr uint32_t M = HOUT * WOUT;
constexpr uint32_t M_ALIGN = 16;
constexpr uint32_t K = CIN;
constexpr uint32_t K_ALIGN = 16;
constexpr half PAD_VALUE = static_cast<half>(2);
constexpr uint32_t FMAP_ELEMENTS = H * W * CIN;
constexpr uint32_t WEIGHT_ELEMENTS = COUT * K;
constexpr uint32_t OUTPUT_ELEMENTS = COUT * M;

__global__ __cube__ void asc_set_l13d_rpt_b_kernel(
    __gm__ half* fmap, __gm__ half* weight, __gm__ half* output)
{
    asc_init();
    __cbuf__ half fmap_l1[FMAP_ELEMENTS];
    __cbuf__ half weight_l1[WEIGHT_ELEMENTS];
    __ca__ half weight_l0[COUT * K_ALIGN];
    __cb__ half fmap_l0[K_ALIGN * M_ALIGN];
    __cc__ float output_l0[COUT * M_ALIGN];

    asc_set_gm2l1_nz_para(1, 1, H * W, 0);
    asc_copy_gm2l1_nd2nz(fmap_l1, fmap, CIN * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, H * W, CIN, 0, false);
    asc_set_gm2l1_nz_para(1, 1, K_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(weight_l1, weight, K * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, COUT, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_copy_l12l0a(weight_l0, weight_l1, 0, 0, COUT / 16, K_ALIGN / 16, COUT / 16, COUT / 16);

    // When f_matrix_ctrl=true, the 3D copy reads the Feature Map descriptor and padding value from the B-side register group.
    asc_set_l13d_fmatrix_b(W, H, PAD, PAD, PAD, PAD);
    asc_set_l12l0b_3d_padding(PAD_VALUE);
    asc_set_l13d_rpt_b(0, 1, asc_l13d_repeat_direction::M_DIRECTION, M_ALIGN / 16, 0);
    // The destination address is the L0B Buffer; f_matrix_ctrl=true selects the B-side register group configured above.
    asc_copy_l12l0b(
        fmap_l0, fmap_l1, K_ALIGN, M, 0, 0, 1, 1, 1, 1, 1, 1, false, false, false, true, CIN);

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(output_l0, weight_l0, fmap_l0, COUT, K_ALIGN, M,
        asc_unit_flag_mode::DISABLE, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, output_l0, M, COUT, M, COUT,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, asc_quant_mode::F322F16,
        asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync();
}

} // namespace

int main()
{
    // The valid Feature Map is all zeros; the L0A cube is all ones, so that the MMAD result reflects only the padding values in L0B.
    std::vector<half> fmap(FMAP_ELEMENTS, static_cast<half>(0));
    std::vector<half> weight(WEIGHT_ELEMENTS, static_cast<half>(0));
    std::vector<half> output(OUTPUT_ELEMENTS);
    for (uint32_t i = 0; i < WEIGHT_ELEMENTS; ++i) weight[i] = static_cast<half>(1);

    half *fmap_device = nullptr, *weight_device = nullptr, *output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&fmap_device), fmap.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&weight_device), weight.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS) {
        return 1;
    }
    aclrtMemcpy(fmap_device, fmap.size() * sizeof(half), fmap.data(), fmap.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(weight_device, weight.size() * sizeof(half), weight.data(), weight.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_l13d_rpt_b_kernel<<<1, 0>>>(fmap_device, weight_device, output_device);
    const bool runtime_ok = aclrtSynchronizeDevice() == ACL_SUCCESS &&
        aclrtMemcpy(output.data(), output.size() * sizeof(half), output_device, output.size() * sizeof(half), ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    uint32_t match_count = 0;
    uint32_t unexpected_count = 0;
    constexpr float EXPECTED_OUTPUT = 12.0f * static_cast<float>(PAD_VALUE);
    for (uint32_t i = 0; i < OUTPUT_ELEMENTS; ++i) {
        const float value = static_cast<float>(output[i]);
        if (std::fabs(value - EXPECTED_OUTPUT) <= 1e-3f) ++match_count;
        else {
            ++unexpected_count;
            if (unexpected_count <= 12) std::cout << "UNEXPECTED index=" << i << " value=" << value << std::endl;
        }
    }
    const bool passed = runtime_ok && match_count == OUTPUT_ELEMENTS && unexpected_count == 0;
    std::cout << "RESULT status=" << (passed ? "PASS" : "FAIL") << " expected=" << EXPECTED_OUTPUT
              << " match_count=" << match_count << '/' << OUTPUT_ELEMENTS
              << " unexpected=" << unexpected_count << std::endl;
    aclrtFree(output_device);
    aclrtFree(weight_device);
    aclrtFree(fmap_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
