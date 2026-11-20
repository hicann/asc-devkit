---
okftype: reference.api
description: "On the Ascend 950PR&950DT products, copies the L0C Buffer cube result into the L1 Buffer, using strongly-typed enums to combine real-time quantization, ReLU, Nz2ND and Nz2DN format conversion, and channel split. The destination data can be int4b_t, fp8_e4m3fn_t, or hifloat8_t."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [quantization, rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_key_features/UnitFlag.md
    kind: constrained_by
    condition: "unit_flag_mode must be used in conjunction with the UnitFlag setting of MMAD-type instructions"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md
    kind: requires
    condition: "Configure a non-zero source cube stride before Nz2DN conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "Set the quantization tensor start address during in-band tensor quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md
    kind: requires
    condition: "Configure activation parameters during in-band Leaky ReLU activation"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: requires
    condition: "Configure format conversion parameters before Nz2ND or Nz2DN format conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "Set quantization parameters during in-band scalar quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md
    kind: requires
    condition: "Configure activation parameters during in-band ReLU activation"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/F32-Channel-Split.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Function Prototype"
    scope_basis: resolved
    note: "Added the 3510 strongly-typed enum prototypes: unit_flag_mode, quant_pre_mode, and relu_pre_mode are changed to asc_unit_flag_mode, asc_quant_mode, and asc_relu_pre_mode, and the enable_nz2dn and enable_clip_relu_pre parameters are added, for a total of 14 overloads. The destination types are extended to int4b_t, fp8_e4m3fn_t, and hifloat8_t. The 9.1.0 integer-parameter prototypes (including 20 parameters such as unit_flag_ctl, quant_pre, quant_post, and eltwise_op) are marked as deprecated in the same version and moved to the deprecated interface documentation, but not removed."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "CANN_9_2_0.md defect fix: Fixed the issue where some 3510 implementations of asc_copy_l0c2l1 lacked parameters when calling the underlying CCE instruction. The behavior of the 3510 prototypes on this page is based on 9.2.0 and later."
---
# asc_copy_l0c2l1 (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:14:40.766Z pushedAt=2026-11-18T08:15:34.307Z nl=2 -->

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

Moves the cube computation result from the L0C Buffer to the L1 Buffer. During the move, real-time quantization, ReLU, Nz2ND format conversion, and channel split can be combined. For the specific supported dtype combinations and parameters, refer to the function prototype.

The following figure shows the valid combinations, intermediate data types, and data paths of real-time quantization, real-time ReLU, real-time format conversion, real-time channel split, and real-time channel merge. The supported scenario combinations are distinguished by the line colors. In the following figure, F32-\>F16 and F32-\>BF16 are Cast, and the rest are real-time scalar/tensor quantization modes.

**Figure 1** asc_copy_l0c2l1 real-time function combination

![](../../../../figures/C_API_L0C2L1_Function_Combination_950.png)

This interface supports combinations of multiple real-time capabilities. You must pre-configure the quantization parameters, activation parameters, channel parameters, and other registers through the companion interfaces, and then call this interface to complete the move.

- In the Nz2ND format conversion scenario, you must pre-configure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md), and use it together with the `enable_nz2nd` parameter of this interface;
- In the Nz2DN format conversion scenario, you must pre-configure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md) and [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and use them together with the `enable_nz2dn` parameter of this interface;
- In the real-time scalar quantization mode, you must set the real-time scalar quantization parameters through [asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md), and use them together with the `quant_pre_mode` parameter of this interface;
- In in-band tensor quantization mode, the start address of the tensor used for in-band tensor quantization must be set via [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md), where each element of the quantization tensor represents a quantization parameter, and it must be used together with the `quant_pre_mode` parameter of this API;
- In in-band activation mode, the ReLU/Leaky ReLU activation parameters must be pre-configured via [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) and [asc_set_l0c_copy_lrelu_alpha](../cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md), and they must be used together with the `enable_clip_relu_pre` and `relu_pre_mode` parameters of this API.

The enum values supported by the `quant_pre_mode` quantization mode parameter are as follows:

- NoQuant: quantization function disabled
- DEQF16: int32_t quantized to half, scalar quantization
- VDEQF16: int32_t quantized to half, tensor quantization
- REQ4: int32_t quantized to int4b_t, scalar quantization
- VREQ4: int32_t quantized to int4b_t, tensor quantization
- REQ8: int32_t quantized to int8_t/uint8_t, scalar quantization
- VREQ8: int32_t quantized to int8_t/uint8_t, tensor quantization
- QS322BF16_PRE: Quantize To bfloat16_t from int32_t, scalar quantization
- VQS322BF16_PRE: Quantize To bfloat16_t from int32_t, tensor quantization
- QF322F16_PRE: Quantize To half from float, scalar quantization
- VQF322F16_PRE: Quantize To half from float, tensor quantization
- QF322BF16_PRE: Quantize To bfloat16_t from float, scalar quantization
- VQF322BF16_PRE: Quantize To bfloat16_t from float, tensor quantization
- F322F16: cast to half from float, cast mode is CAST_RINT mode
- F322BF16: cast to bfloat16_t from float, cast mode is CAST_RINT mode
- QF322S4_PRE: Quantize To int4b_t from float, scalar quantization
- VQF322S4_PRE: Quantize To int4b_t from float, tensor quantization
- QF322B8_PRE: Quantize To int8_t/uint8_t from float, scalar quantization
- VQF322B8_PRE: Quantize To int8_t/uint8_t from float, tensor quantization
- QF322FP8_PRE: Quantize To fp8_e4m3fn_t from float, scalar quantization
- VQF322FP8_PRE: Quantize To fp8_e4m3fn_t from float, tensor quantization
- QF322HIF8_PRE: Quantize To hifloat8_t (Half to Away Round) from float, scalar quantization
- VQF322HIF8_PRE: Quantize To hifloat8_t (Half to Away Round) from float, tensor quantization
- QF322HIF8_PRE_HYBRID: Quantize To hifloat8_t (Hybrid Round) from float, scalar quantization
- VQF322HIF8_PRE_HYBRID: Quantize To hifloat8_t (Hybrid Round) from float, tensor quantization
- QF322F32_PRE: Quantize To float from float, scalar quantization, with precision reaching two parts per thousand but not two parts per ten thousand.
- VQF322F32_PRE: Quantize To float from float, tensor quantization, with precision reaching two parts per thousand but not two parts per ten thousand. Two parts per ten thousand means that the error between each actual data value and the true data value does not exceed one part per ten thousand, and the total number of data values whose error exceeds one part per ten thousand does not exceed one part per ten thousand of the total number of data values.

This interface is valid only when executed on the AIC.

For mode selection, coefficient preparation, and restrictions of in-band processing, see [real-time quantization](../cube_store_key_features/accompanying_quantization.md), [in-band ReLU](../cube_store_key_features/accompanying_relu.md), and [scenario combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md). Whether the features can be used in combination is subject to the data types, modes, and constraints supported by this interface.

## Prototype

```c
// Placeholder form
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       asc_unit_flag_mode unit_flag_mode,
                                       asc_quant_mode quant_pre_mode,
                                       asc_relu_pre_mode relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       bool enable_nz2dn,
                                       bool enable_clip_relu_pre)
```

### Data Types Supported by dtype

The following combinations of src dtype and dst dtype are supported:

- When `src_dtype` is `int32_t`, `dst_dtype` supports `int4b_t`, `int8_t`, `uint8_t`, `half`, `bfloat16_t`, and `int32_t`.
- When `src_dtype` is `float`, `dst_dtype` supports `int4b_t`, `int8_t`, `uint8_t`, `fp8_e4m3fn_t`, `hifloat8_t`, `half`, `bfloat16_t`, and `float`.

### Prototype Example

```c
// Example: copy of int8_t type
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst,
                                       __cc__ int32_t* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       asc_unit_flag_mode unit_flag_mode,
                                       asc_quant_mode quant_pre_mode,
                                       asc_relu_pre_mode relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       bool enable_nz2dn,
                                       bool enable_clip_relu_pre)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|:-----------------|:------|:-----------------------------------|
| dst | Output | Destination operand, stored in the L1 Buffer. The data format is Nz, ND, or DN, and the start address must be 32-byte aligned. |
| src | Input | Source operand, stored in the L0C Buffer. The data format is Nz, and the start address must be 64-byte aligned. |
| n_size | Input | Size of the source Nz cube in the N direction. Value range: [1, 4095].<br>For the Nz output scenario:<br>&nbsp;&nbsp;&bull; If `enable_channel_split` is set to `true` to enable the Channel Split function, `n_size` must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the Channel Split function is disabled, `n_size` must be a multiple of 16.<br>For the ND output scenario:<br>&nbsp;&nbsp;&bull; $n\_size\times\operatorname{sizeof}(dst\_dtype)$ Must be a multiple of 32; if `dst_dtype` is set to `int4b_t`, `n_size` must be a multiple of 64.<br> |
| m_size | Input | Size of the source Nz cube in the M direction.<br>&nbsp;&nbsp;&bull; When the Nz2ND function is disabled, value range: [1, 65535];<br>&nbsp;&nbsp;&bull; When the Nz2ND function is enabled, value range: [1, 8192].<br>&nbsp;&nbsp;&bull; For the DN output scenario: $m\_size\times\operatorname{sizeof}(dst\_dtype)$ Must be a multiple of 32; if `dst_dtype` is set to `int4b_t`, `m_size` must be a multiple of 64.<br>  |
| dst_stride | Input | &nbsp;&nbsp;&bull; If the Nz2ND function is disabled, dst_stride indicates the start address offset between adjacent Z layouts in the destination Nz cube. The value is non-zero, in elements.<br>&nbsp;&nbsp;&bull; If the Nz2ND/Nz2DN function is enabled, dst_stride indicates the number of elements in each row of the destination ND cube. The value is non-zero, in elements. The alignment constraints for different `dst_dtype` values are as follows: `int4b_t` output must be a multiple of 64, 8-bit output must be a multiple of 32, 16-bit output must be a multiple of 16, and 32-bit output must be a multiple of 8.<br> |
| src_stride | Input | Start address offset between adjacent Z layouts in the source Nz cube. Value range: [0, 65535], in units of 64 bytes, that is, $16\times\operatorname{sizeof}(src\_dtype)$. |
| unit_flag_mode | Input | unit_flag is a fine-grained parallel function of mmad-type instructions and cube copy-out instructions. When this function is enabled, the computation result is copied out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`: unit_flag is disabled;<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`: unit_flag is enabled, and the hardware does not reset the unit flag bit after executing the instruction;<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`: unitFlag is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When this function is enabled, the unit_flag values of the mmad-type instructions and cube copy-out instructions must be set to `asc_unit_flag_mode::ENABLE_KEEP` or `asc_unit_flag_mode::ENABLE_UPDATE`.<br>For how to coordinate with compute instructions, see [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md). |
| quant_pre_mode | Input | Quantization parameter in the preprocessing stage. For values, see [Description](#description).<br>For coefficient meanings and configuration steps, see [real-time quantization](../cube_store_key_features/accompanying_quantization.md). |
| relu_pre_mode | Input | ReLU mode control in the preprocessing stage. The values are as follows:<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::NONE`: ReLU is disabled;<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::NORMAL`: Normal ReLU is enabled;<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::SCALAR`: Scalar ReLU is enabled;<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::VECTOR`: Vector ReLU is enabled.<br>For modes and configuration interfaces, see [in-band ReLU](../cube_store_key_features/accompanying_relu.md). |
| enable_channel_split | Input | Enable flag for the channel split function. Channel Split can be enabled only when both src and dst are float, and Channel Split and Nz2ND cannot be enabled at the same time.<br>&nbsp;&nbsp;&bull; false: disabled;<br>&nbsp;&nbsp;&bull; true: enabled.<br>For features and supported paths, see [F32 Channel Split](../cube_store_key_features/F32-Channel-Split.md). |
| enable_nz2nd | Input | Enable flag for the Nz2ND function.<br>&nbsp;&nbsp;&bull; false: disabled;<br>&nbsp;&nbsp;&bull; true: enabled.<br>For layout and stride examples, see [NZ2ND](../cube_store_key_features/NZ2ND.md); when format conversion is disabled, see [NZ2NZ](../cube_store_key_features/NZ2NZ.md). |
| enable_nz2dn | Input | Enable flag for the Nz2DN function.<br>&nbsp;&nbsp;&bull; false: disabled;<br>&nbsp;&nbsp;&bull; true: enabled.<br>For layout and configuration methods, see [NZ2DN](../cube_store_key_features/NZ2DN.md). |
| enable_clip_relu_pre | Input | Enable flag for Clip ReLU. It must be used together with Normal ReLU, and the quantization function must be enabled. The values are as follows:<br>&nbsp;&nbsp;&bull; `false`: Clip ReLU is disabled;<br>&nbsp;&nbsp;&bull; `true`: Clip ReLU (scalar mode) is enabled. |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

### General Constraints

- This interface takes effect only on the AIC. Calls from non-AIC contexts return directly.
- The `dst` Start Address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, the copy will be abnormal.
- The `src` Start Address must be 64-byte aligned (L0C Buffer alignment requirement, when `src` is `int32_t` or `float`); otherwise, the copy will be abnormal.
- If this instruction has data dependencies with other instructions, insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L0C Buffer capacity limit: The total L0C Buffer capacity is 256 KB. The `src` offset plus the copy size must not exceed the boundary; otherwise, an exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The `dst` offset plus the copy size must not exceed the boundary; otherwise, an exception is triggered.

### In-band Conversion Constraints

- `n_size`, `m_size`, and `dst_stride` must satisfy alignment constraints determined by the dtype and function mode. For details, see the parameter description. Failure to satisfy the alignment constraints will cause the copy result to be unexpected.
- The src and dst dtype combination must match the `quant_pre_mode` quantization mode; otherwise, the copy result will be unexpected.
- `enable_channel_split` can be set to true only when the output dtype is `float` and the output is in Nz format.
- The quantization coefficients used in the quantization and activation modes must not be INF/NAN or denormalized numbers; otherwise, the quantization and activation results will be incorrect.
- When Nz2DN conversion is enabled, the source cube stride must be pre-configured via [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and the source cube stride must not be 0; otherwise, the copy will be abnormal.
- When Nz2DN conversion is enabled, the UnitFlag function can be enabled simultaneously only when the source cube stride configured via [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md) is 1.
- When `enable_clip_relu_pre` is set to Clip ReLU (scalar mode), it must be used together with `relu_pre_mode` and the quantization function.

## Examples

For the complete project sample, see [data_copy_l0c2l1 sample](../../../../../../../examples/02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2l1).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

In this example, `M=N=K=128` is fixed. The L0C Buffer result is generated through MMAD, converted to ND format by calling this API, and moved to the L1 Buffer. It is then transferred back to GM through UB and compared with the Golden result on the Host side.

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t M = 128, N = 128, K = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr int64_t READY_FLAG = 11;

__global__ __mix__(1, 2) void CopyL0c2l1Kernel(
    __gm__ int8_t* a, __gm__ int8_t* b, __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __cbuf__ int32_t output_l1[ELEMENTS];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];
    __ubuf__ int32_t* output_ub = reinterpret_cast<__ubuf__ int32_t*>(0);
    if ASC_IS_AIC {
        asc_set_gm2l1_nz_para(1, 1, M, 0);
        asc_copy_gm2l1_nd2nz(a_l1, a, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
        asc_set_gm2l1_nz_para(1, 1, K, 0);
        asc_copy_gm2l1_nd2nz(b_l1, b, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / 16, K / 32, M / 16, M / 16);
        asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / 16, K / 32, N / 16, N / 16);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c_copy_nz_para(1, 0, 0);
        asc_copy_l0c2l1(output_l1, c_l0, N, M, N, M, asc_unit_flag_mode::DISABLE,
            asc_quant_mode::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
        asc_sync_notify(PIPE_FIX, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_FIX, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(output_ub, output_l1, 0, 1, ELEMENTS * sizeof(int32_t) / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_block_arrive(PIPE_MTE1, READY_FLAG);
    }
    if ASC_IS_AIV {
        asc_sync_block_wait(PIPE_MTE3, READY_FLAG);
        if (asc_get_sub_block_id() == 0)
            asc_copy_ub2gm_align(output, output_ub, ELEMENTS * sizeof(int32_t));
    }
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t r = 0; r < M; ++r) for (uint32_t k = 0; k < K; ++k)
        a[r * K + k] = static_cast<int8_t>(static_cast<int32_t>((r + 2 * k) % 5) - 2);
    for (uint32_t c = 0; c < N; ++c) for (uint32_t k = 0; k < K; ++k)
        b[c * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * c + k) % 5) - 2);
    for (uint32_t r = 0; r < M; ++r) for (uint32_t c = 0; c < N; ++c)
        for (uint32_t k = 0; k < K; ++k) golden[r * N + c] += a[r * K + k] * b[c * K + k];
    aclInit(nullptr); aclrtSetDevice(0);
    int8_t *ad = nullptr, *bd = nullptr; int32_t* od = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&ad), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&bd), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&od), output.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(ad, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(bd, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    CopyL0c2l1Kernel<<<1, 0>>>(ad, bd, od);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), od,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = ret == ACL_SUCCESS && output == golden;
    std::printf("M=%u N=%u K=%u asc_copy_l0c2l1=%s\n", M, N, K, passed ? "PASS" : "FAIL");
    aclrtFree(ad); aclrtFree(bd); aclrtFree(od); aclrtResetDevice(0); aclFinalize();
    return passed ? 0 : 1;
}
```
