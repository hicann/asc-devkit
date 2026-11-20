---
okftype: reference.api
description: "On the Ascend 950PR&950DT products, moves the L0C Buffer cube result into the Unified Buffer, supports in-path quantization, activation, and Nz2ND/Nz2DN format conversion, and can use dual-target mode to split along the M or N dimension and write simultaneously to the UB of two Vector Cores."
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
    condition: "Set the quantization tensor start address for in-path tensor quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md
    kind: requires
    condition: "Configure activation parameters for in-path Leaky ReLU activation"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: requires
    condition: "Configure format conversion parameters before Nz2ND or Nz2DN format conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "Set quantization parameters for in-path scalar quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md
    kind: requires
    condition: "Configure activation parameters for in-path ReLU activation"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/F32-Channel-Split.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/L0C_to_UB_dual_target_mode.md
    kind: constrained_by
    condition: "Determines the splitting direction and constraints when the dual_dst_ctrl dual-target mode is enabled"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:sub_blockid"
    scope_basis: resolved
    note: "sub_blockid changed from bool to int8_t: the 9.1.0 prototype uses bool true or false to select the destination Vector, and from 9.2.0 onward uses 0 or 1 to write to SUB BLOCK0 and SUB BLOCK1 respectively; this change only involves the Ascend 950PR&950DT products, that is, the 3510 prototype."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Function Prototype"
    scope_basis: resolved
    note: "Added strongly-typed enum prototypes: dual_dst_ctrl is replaced with asc_dual_dst_mode; unit_flag_mode, quant_pre_mode, and relu_pre_mode are replaced with asc_unit_flag_mode, asc_quant_mode, and asc_relu_pre_mode; and the enable_nz2dn and enable_clip_relu_pre parameters are newly added. The integer parameter prototypes from 9.1.0 (including 21 parameters such as dual_dst_ctl, unit_flag_ctl, quant_pre, quant_post, and eltwise_op) are marked as deprecated in the same version and migrated to the deprecated API documentation, but not removed."
---
# asc_copy_l0c2ub

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:19:01.249Z pushedAt=2026-11-18T08:15:34.313Z nl=2 -->

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

Moves the cube computation result from the L0C Buffer to the Unified Buffer (UB). During the move, it can simultaneously support combinations of capabilities such as in-path quantization, in-path activation, and in-path format conversion (Nz2ND/Nz2DN).

The following figure shows the valid combinations, intermediate data types, and data paths of in-path quantization, in-path ReLU, in-path format conversion, in-path channel splitting, and in-path channel merging. The connection line colors distinguish the supported scenario combinations. In the following figure, F32-\>F16 and F32-\>BF16 are Cast, and the rest are in-path scalar/tensor quantization modes.

**Figure 1** asc_copy_l0c2ub in-path function combination

![](../../../../figures/C_API_L0C2UB_Function_Combination_950.png)

This interface supports combinations of multiple in-path capabilities. You need to preconfigure registers such as quantization parameters, activation parameters, and channel parameters through the supporting interfaces, and then call this interface to complete the move.

- In the Nz2ND format conversion scenario, you need to preconfigure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md), and use them together with the `enable_nz2nd` parameter of this interface;
- In the Nz2DN format conversion scenario, you need to preconfigure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md) and [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and use them together with the `enable_nz2dn` parameter of this interface;
- In the in-path scalar quantization mode, you need to set the in-path scalar quantization parameters through [asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md), and use them together with the `quant_pre_mode` parameter of this interface;
- In in-path tensor quantization mode, use [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md) to set the start address of the tensor used for in-path tensor quantization, where each element of the quantization tensor represents a quantization parameter, and it must be used together with the `quant_pre_mode` parameter of this API;
- In in-path activation mode, use [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) and [asc_set_l0c_copy_lrelu_alpha](../cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) to preconfigure the ReLU/Leaky ReLU activation parameters, and they must be used together with the `enable_clip_relu_pre` and `relu_pre_mode` parameters of this API;

The enumeration values supported by the `quant_pre_mode` quantization mode parameter are as follows:

- NoQuant: Quantization is not enabled.
- DEQF16: Quantize int32_t to half, scalar quantization.
- VDEQF16: Quantize int32_t to half, tensor quantization.
- REQ4: Quantize int32_t to int4b_t, scalar quantization.
- VREQ4: Quantize int32_t to int4b_t, tensor quantization.
- REQ8: Quantize int32_t to int8_t/uint8_t, scalar quantization.
- VREQ8: Quantize int32_t to int8_t/uint8_t, tensor quantization.
- QS322BF16_PRE: Quantize int32_t to bfloat16_t, scalar quantization.
- VQS322BF16_PRE: Quantize int32_t to bfloat16_t, tensor quantization.
- QF322F16_PRE: Quantize float to half, scalar quantization.
- VQF322F16_PRE: Quantize float to half, tensor quantization.
- QF322BF16_PRE: Quantize float to bfloat16_t, scalar quantization.
- VQF322BF16_PRE: Quantize float to bfloat16_t, tensor quantization.
- F322F16: Cast float to half, cast mode is CAST_RINT.
- F322BF16: Cast float to bfloat16_t, cast mode is CAST_RINT.
- QF322S4_PRE: Quantize float to int4b_t, scalar quantization.
- VQF322S4_PRE: Quantize float to int4b_t, tensor quantization.
- QF322B8_PRE: Quantize To int8_t/uint8_t, scalar quantization.
- VQF322B8_PRE: Quantize To int8_t/uint8_t, tensor quantization.
- QF322FP8_PRE: Quantize To fp8_e4m3fn_t, scalar quantization.
- VQF322FP8_PRE: Quantize To fp8_e4m3fn_t, tensor quantization.
- QF322HIF8_PRE: Quantize To hifloat8_t (Half to Away Round), scalar quantization.
- VQF322HIF8_PRE: Quantize To hifloat8_t (Half to Away Round), tensor quantization.
- QF322HIF8_PRE_HYBRID: Quantize To hifloat8_t (Hybrid Round), scalar quantization.
- VQF322HIF8_PRE_HYBRID: Quantize To hifloat8_t (Hybrid Round), tensor quantization.
- QF322F32_PRE: Quantize To float, scalar quantization. The precision can reach two thousandths but cannot reach two ten-thousandths.
- VQF322F32_PRE: Quantize to float, tensor quantization. The precision can reach two thousandths but cannot reach two ten-thousandths.

This interface is valid only when executed on the AIC.

For mode selection, coefficient preparation, and restrictions of in-path processing, see [In-Path Quantization](../cube_store_key_features/accompanying_quantization.md), [In-Path ReLU](../cube_store_key_features/accompanying_relu.md), and [Scenario Combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md). Whether features can be combined depends on the data types, modes, and constraints supported by this interface.

## Prototype

```cpp
// Placeholder form
__aicore__ inline void asc_copy_l0c2ub(__ubuf__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       int8_t sub_blockid,
                                       asc_dual_dst_mode dual_dst_ctrl,
                                       asc_unit_flag_mode unit_flag_mode,
                                       asc_quant_mode quant_pre_mode,
                                       asc_relu_pre_mode relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       bool enable_nz2dn,
                                       bool enable_clip_relu_pre)
```

### Data Types Supported by dtype

src dtype and dst dtype support the following combinations:

- When `src_dtype` is `int32_t`, `dst_dtype` supports `int4b_t`, `int8_t`, `uint8_t`, `half`, `bfloat16_t`, and `int32_t`.
- When `src_dtype` is `float`, `dst_dtype` supports `int4b_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, and `float`.

### Prototype Example

```cpp
// Example: Convert float data to half and then copy it.
__aicore__ inline void asc_copy_l0c2ub(__ubuf__ half* dst,
                                       __cc__ float* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       int8_t sub_blockid,
                                       asc_dual_dst_mode dual_dst_ctrl,
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

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination operand, stored in UB. The start address must be 32-byte aligned. |
| src | Input | Source operand, stored in L0C Buffer. The start address must be 64-byte aligned. |
| n_size | Input | Size of the source Nz cube in the N direction, with a value range of $[1, 2^{12}-1]$.<br>For the Nz output scenario:<br>&nbsp;&nbsp;&bull; If `enable_channel_split` is set to `true` to enable the Channel Split feature, `n_size` must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the Channel Split feature is not enabled, `n_size` must be a multiple of 16.<br>For the ND output scenario:<br>&nbsp;&nbsp;&bull; $n\_size\times\operatorname{sizeof}(dst\_dtype)$ must be a multiple of 32; if `dst_dtype` is set to `int4b_t`, `n_size` must be a multiple of 64.<br> |
| m_size | Input | Size of the source Nz cube in the M direction, with a value range of $[1, 2^{16}-1]$.<br>For the DN output scenario:<br>&nbsp;&nbsp;&bull; $m\_size\times\operatorname{sizeof}(dst\_dtype)$ must be a multiple of 32; if `dst_dtype` is set to `int4b_t`, `m_size` must be a multiple of 64.<br> |
| dst_stride | Input | Destination cube stride, with a value range of $[1, 2^{32}-1]$. The corresponding byte length must be 32-byte aligned.<br>&nbsp;&nbsp;&bull; If the Nz2ND feature is not enabled, `dst_stride` indicates the start address offset between adjacent Z layouts in the destination Nz cube, in elements.<br>&nbsp;&nbsp;&bull; If the Nz2ND/Nz2DN feature is enabled, `dst_stride` indicates the number of elements in each row of the destination ND/DN cube, in elements. The alignment constraints for different `dst_dtype` values are as follows: `int4b_t` output must be a multiple of 64, 8-bit output must be a multiple of 32, 16-bit output must be a multiple of 16, and 32-bit output must be a multiple of 8.<br> |
| src_stride | Input | Start address offset between adjacent Z layouts in the source Nz cube, in units of 64 bytes, that is, $16\times\operatorname{sizeof}(src\_dtype)$. The value range is $[0, 2^{16}-1]$. |
| sub_blockid | Input | Indicates the SUB BLOCK ID of the target UB when single-target mode is enabled. The value is 0 or 1. A value of 0 writes to SUB BLOCK0, and a value of 1 writes to SUB BLOCK1. |
| dual_dst_ctrl | Input | Dual-target mode control parameter. When there is one Cube Core and two Vector Cores in the same AI Core, if dual-target mode control is enabled, the M×N cube in L0C Buffer is split into two halves and written simultaneously to the UB of each of the two Vector Cores, with the first half written to SUB BLOCK0 and the second half written to SUB BLOCK1. The parameter values are described as follows:<br>&nbsp;&nbsp;&bull; `asc_dual_dst_mode::DUAL_DST_DISABLE`: Single-target mode, writes the entire cube to the target UB configured by the `sub_blockid` parameter.<br>&nbsp;&nbsp;&bull; `asc_dual_dst_mode::DUAL_DST_SPLIT_M`: Dual-target mode, splits the cube into two cubes of shape $M/2\times N$ along the M dimension and writes them to the two UBs respectively. M must be a multiple of 2.<br>&nbsp;&nbsp;&bull; `asc_dual_dst_mode::DUAL_DST_SPLIT_N`: Dual-target mode, splits the cube into two cubes of shape $M\times N/2$ along the N dimension and writes them to the two UBs respectively. N must be a multiple of 32.<br>This parameter supports only the normal copy mode (Nz2Nz) or the Nz2ND copy scenario, and does not support other in-path feature scenarios.<br>For the split direction and constraints, see [L0C Buffer to UB Dual-Target Mode](../cube_store_key_features/L0C_to_UB_dual_target_mode.md). |
| unit_flag_mode | Input | UnitFlag is a fine-grained parallel feature for MMAD-type instructions and cube copy-out instructions. When this feature is enabled, the hardware copies out the computation result each time it finishes computing one fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`: UnitFlag is not enabled.<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`: UnitFlag is enabled. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`: UnitFlag is enabled. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this feature is enabled, the UnitFlag values of MMAD-type instructions and cube copy-out instructions must be set to `asc_unit_flag_mode::ENABLE_KEEP` or `asc_unit_flag_mode::ENABLE_UPDATE`.<br>For how to coordinate with compute instructions, see [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md). |
| quant_pre_mode | Input | Quantization mode in the preprocessing stage. For the values, see [Description](#description).<br>For the meaning of coefficients and configuration steps, see [In-Path Quantization](../cube_store_key_features/accompanying_quantization.md). |
| relu_pre_mode | Input | ReLU mode control in the preprocessing stage. The values are as follows:<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::NONE`: ReLU is not enabled.<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::NORMAL`: Normal ReLU is enabled.<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::SCALAR`: Scalar ReLU is enabled.<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::VECTOR`: Vector ReLU is enabled.<br><br>For modes and configuration interfaces, see [In-Path ReLU](../cube_store_key_features/accompanying_relu.md). |
| enable_channel_split | Input | Whether to enable the channel split feature. It can be enabled only when both `src_dtype` and `dst_dtype` are `float` and the output is in Nz format.<br>&nbsp;&nbsp;&bull; `false`: Not enabled.<br>&nbsp;&nbsp;&bull; `true`: Enabled.<br><br>For the feature and supported paths, see [F32 Channel Split](../cube_store_key_features/F32-Channel-Split.md). |
| enable_nz2nd | Input | Nz2ND format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Nz2ND conversion is disabled.<br>&nbsp;&nbsp;&bull; `true`: Nz2ND conversion is enabled.<br><br>For the layout and stride examples, see [NZ2ND](../cube_store_key_features/NZ2ND.md); when format conversion is disabled, see [NZ2NZ](../cube_store_key_features/NZ2NZ.md). |
| enable_nz2dn | Input | Nz2DN format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Nz2DN conversion is disabled.<br>&nbsp;&nbsp;&bull; `true`: Nz2DN conversion is enabled.<br><br>For the layout and configuration method, see [NZ2DN](../cube_store_key_features/NZ2DN.md). |
| enable_clip_relu_pre | Input | Whether to enable Clip ReLU. It must be used together with Normal ReLU and requires the quantization feature to be enabled. The values are as follows:<br>&nbsp;&nbsp;&bull; `false`: Clip ReLU is not enabled.<br>&nbsp;&nbsp;&bull; `true`: Clip ReLU is enabled (scalar mode).<br> |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

### General Constraints

- If this API is not called by AIC, it returns directly.
- This API applies only to Mix operator scenarios, and supports only the configuration where the ratio of Cube cores to Vector cores is 1:2.
- The `dst` start address must be aligned to 32 bytes; otherwise, an exception is triggered.
- The `src` start address must be aligned to 64 bytes; otherwise, an exception is triggered.
- If this instruction has data dependencies with other instructions, insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- UB capacity limit: The total UB capacity is 256KB. By default, 6KB is reserved for the SIMD VF stack and 2KB for Ascend C reserved space, leaving 248KB available. When SIMD and SIMT are mixed, an additional 32KB to 128KB is allocated as Data Cache, further reducing the available capacity. The sum of the dst offset and the copy size must not exceed the actual available capacity; otherwise, a write overflow exception is triggered.
- L0C Buffer capacity limit: The total L0C Buffer capacity is 256KB. The sum of the src offset and the copy size must not exceed the L0C Buffer capacity; otherwise, a read overflow exception is triggered.
- An Nz cube uses 16×16 elements as one basic fractal. In boundary fractals, data beyond the range specified by `n_size` or `m_size` does not belong to the valid copy result.

### In-Path Conversion Constraints

- `n_size`, `m_size`, and `dst_stride` must be determined according to the dtype and function mode for alignment constraints. For details, see the parameter description. Failure to meet the alignment constraints will cause the copy result to be inconsistent with expectations.
- The src and dst dtype combination must match the `quant_pre_mode` quantization mode; otherwise, the copy result will be inconsistent with expectations.
- `dual_dst_ctrl` is supported only in the normal copy mode (Nz2Nz) or the Nz2ND copy scenario, and is not supported in other in-path function scenarios.
- `enable_channel_split` can be set to true only when the output dtype is `float` and the output is in Nz format.
- The quantization coefficients used in the quantization and activation modes must not be INF/NAN or denormalized numbers; otherwise, the quantization and activation results will be incorrect.
- When Nz2DN conversion is enabled, the source cube stride must be preconfigured through [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and the source cube stride must not be 0; otherwise, the copy will be abnormal.
- When Nz2DN conversion is enabled, the UnitFlag function can be enabled simultaneously only when the source cube stride configured through [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md) is 1.
- When `enable_clip_relu_pre` is set to Clip ReLU (scalar mode), it must be used together with `relu_pre_mode` and the quantization function.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

This example occupies 32KB L1 Buffer, 16KB L0A Buffer, 16KB L0B Buffer, 64KB L0C Buffer, and 64KB UB, and uses `EVENT_ID0` and the synchronization resource number `0x8` to complete pipeline and inter-core synchronization.

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;

__global__ __mix__(1, 2) void AscCopyL0c2ubKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];
    __ubuf__ int32_t* output_ub = reinterpret_cast<__ubuf__ int32_t*>(0);

    if ASC_IS_AIC {
        // Convert the two 128x128 ND inputs to the Nz layout required by MMAD.
        asc_set_gm2l1_nz_para(1, 1, 128, 0);
        asc_copy_gm2l1_nd2nz(a_l1, a, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
        asc_set_gm2l1_nz_para(1, 1, 128, 0);
        asc_copy_gm2l1_nd2nz(b_l1, b, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

        // Move cubes A and B from L1 Buffer to L0A Buffer and L0B Buffer.
        asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
        asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

        // Perform 128x128x128 cube multiplication and write the result to L0C Buffer.
        asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

        // Convert the Nz result in L0C Buffer to continuous ND format and move it out to the AIV-side UB.
        asc_set_l0c_copy_nz_para(1, 0, ELEMENTS);
        asc_copy_l0c2ub(output_ub, c_l0, N, M, N, M, 0, asc_dual_dst_mode::DUAL_DST_DISABLE,
            asc_unit_flag_mode::DISABLE, asc_quant_mode::NoQuant, asc_relu_pre_mode::NONE,
            false, true, false, false);
        asc_sync_block_arrive(PIPE_FIX, 0x8);
    }
    if ASC_IS_AIV {
        if (asc_get_sub_block_id() != 0) {
            return;
        }
        // Wait for AI Core to complete the L0C-to-UB transfer, then AIV0 moves the result out to GM.
        asc_sync_block_wait(PIPE_MTE3, 0x8);
        asc_copy_ub2gm(output, output_ub, M, N * sizeof(int32_t),
            N * sizeof(int32_t), N * sizeof(int32_t));
        asc_sync();
    }
}

template <typename T>
void PrintRow(const char* label, const std::vector<T>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << +data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(ELEMENTS), golden(ELEMENTS);
    // Construct the asymmetric input and compute the Golden result on the Host side.
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = static_cast<int8_t>(static_cast<int32_t>((row + 2 * k) % 5) - 2);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * col + k) % 5) - 2);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                golden[row * N + col] += a[row * K + k] * b[col * K + k];
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    int8_t *a_device = nullptr, *b_device = nullptr;
    int32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyL0c2ubKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_l0c2ub result is correct."
                         : "[Failed] asc_copy_l0c2ub result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
