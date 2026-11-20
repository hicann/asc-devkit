---
okftype: reference.api
description: "Deprecated API page: the old parameter form of asc_copy_l0c2ub and its _sync overload under the 3510 architecture moves the matrix result from the L0C Buffer to UB, and can combine inline quantization, ReLU, channel splitting, dual-destination control, and Nz2ND or Nz2DN conversion. It is recommended to use the strongly-typed enum overload plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [conversion, store]
  - traits: [quantization, rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires switching to the new overload with strongly-typed enum parameters plus asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md
    kind: requires
    condition: "The Nz2DN scenario requires pre-configuration of the source matrix stride"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "Inline tensor quantization requires setting the quantization tensor start address first"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: requires
    condition: "Before enabling Nz2ND or Nz2DN, format conversion parameters must be configured first"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "Inline scalar quantization requires setting quantization parameters first"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When the destination address overlaps with other instructions, a synchronization instruction must be inserted for serialization"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 3510 sync form with ASC_DEPRECATED(9.2.0). The Release Notes 9.2.0 deprecation table requires switching to the overload with strongly-typed enum parameters plus asc_sync; the interface has not been removed."
---
# asc_copy_l0c2ub (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:59:59.287Z pushedAt=2026-11-18T03:12:16.420Z nl=2 -->

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

**The legacy parameter form of `asc_copy_l0c2ub` and the `asc_copy_l0c2ub_sync` API are deprecated. Use the [asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) API and the [asc_sync](../sync/intra_core_sync/asc_sync.md) API instead.**

The header file paths are: `"c_api/cube_datamove/cube_datamove.h"`, `"c_api/composite/cube_datamove_composite.h"`.

Moves the matrix computation result from the L0C Buffer to the Unified Buffer (UB). During the move, it can simultaneously support combinations of capabilities such as inline quantization, inline activation, and inline format conversion (Nz2ND/Nz2DN).

The following figure shows the valid combinations, intermediate data types, and data paths of inline quantization, inline ReLU, inline format conversion, inline channel splitting, and inline channel merging. In the following figure, F32-\>F16 and F32-\>BF16 are Cast, and the rest are inline scalar/tensor quantization modes.

**Figure 1** asc_copy_l0c2ub inline function combinations

![](../../../figures/C_API_L0C2UB_Function_Combination_950.png)

This API supports combinations of multiple inline capabilities. You need to pre-configure registers such as quantization parameters, activation parameters, and channel parameters through the supporting APIs, and then call this API to complete the move.

- In the Nz2ND format conversion scenario, you need to pre-configure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md), and use them together with the `enable_nz2nd` parameter of this API;
- In the Nz2DN format conversion scenario, you need to pre-configure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) and [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and use them together with the `enable_nz2dn` parameter of this API;
- In inline scalar quantization mode, you need to set the inline scalar quantization parameters through [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md), and use it together with the `quant_pre_mode` parameter of this interface;
- In inline tensor quantization mode, you need to set the start address of the tensor used for inline tensor quantization through [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md), where each element of the quantization tensor represents a quantization parameter, and use it together with the `quant_pre_mode` parameter of this interface;
- In inline activation mode, you need to pre-configure the ReLU/Leaky ReLU activation parameters through [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) and [asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md), and use them together with the `enable_clip_relu_pre` and `relu_pre_mode` parameters of this interface;

The enumeration values supported by the `quant_pre_mode` quantization mode parameter are as follows:

- NoQuant: quantization is not enabled.
- DEQF16: int32_t quantize to half, scalar quantization.
- VDEQF16: int32_t quantize to half, tensor quantization.
- REQ4: int32_t quantize to int4b_t, scalar quantization.
- VREQ4: int32_t quantize to int4b_t, tensor quantization.
- REQ8: int32_t quantize to int8_t/uint8_t, scalar quantization.
- VREQ8: Quantize int32_t to int8_t/uint8_t, tensor quantization.
- QS322BF16_PRE: Quantize int32_t to bfloat16_t, scalar quantization.
- VQS322BF16_PRE: Quantize int32_t to bfloat16_t, tensor quantization.
- QF322F16_PRE: Quantize float to half, scalar quantization.
- VQF322F16_PRE: Quantize float to half, tensor quantization.
- QF322BF16_PRE: Quantize float to bfloat16_t, scalar quantization.
- VQF322BF16_PRE: Quantize float to bfloat16_t, tensor quantization.
- F322F16: Cast float to half, cast mode is CAST_RINT mode.
- F322BF16: Cast float to bfloat16_t, cast mode is CAST_RINT mode.
- QF322S4_PRE: Quantize float to int4b_t, scalar quantization.
- VQF322S4_PRE: Quantize float to int4b_t, tensor quantization.
- QF322B8_PRE: Quantize float to int8_t/uint8_t, scalar quantization.
- VQF322B8_PRE: Quantize float to int8_t/uint8_t, tensor quantization.
- QF322FP8_PRE: Quantize float to fp8_e4m3fn_t, scalar quantization.
- VQF322FP8_PRE: Quantize float to fp8_e4m3fn_t, tensor quantization.
- QF322HIF8_PRE: Quantize float to hifloat8_t (Half to Away Round), scalar quantization.
- VQF322HIF8_PRE: Quantize float to hifloat8_t (Half to Away Round), tensor quantization.
- QF322HIF8_PRE_HYBRID: Quantize float to hifloat8_t (Hybrid Round), scalar quantization.
- VQF322HIF8_PRE_HYBRID: Quantize float to hifloat8_t (Hybrid Round), tensor quantization.
- QF322F32_PRE: Quantize float to float, scalar quantization, with precision reaching two thousandths but not two ten-thousandths.
- VQF322F32_PRE: Quantize float to float, tensor quantization, with precision up to two thousandths, but not up to two ten-thousandths.

This interface is valid only when executed on the AIC.

## Prototype

```cpp
// Placeholder form
__aicore__ inline void asc_copy_l0c2ub(__ubuf__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t dual_dst_ctrl,
                                       bool sub_blockid,
                                       uint8_t enable_clip_relu_pre,
                                       uint8_t unit_flag_mode,
                                       uint64_t quant_pre_mode,
                                       uint8_t relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       uint64_t quant_post,
                                       uint8_t relu_post,
                                       bool clip_relu_post,
                                       uint8_t eltwise_op,
                                       bool eltwise_antq_en,
                                       bool c0_pad_en,
                                       bool broadcast_en,
                                       bool enable_nz2dn)
__aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ <dst_dtype>* dst,
                                            __cc__ <src_dtype>* src,
                                            uint16_t n_size,
                                            uint16_t m_size,
                                            uint32_t dst_stride,
                                            uint16_t src_stride,
                                            uint8_t dual_dst_ctrl,
                                            bool sub_blockid,
                                            uint8_t enable_clip_relu_pre,
                                            uint8_t unit_flag_mode,
                                            uint64_t quant_pre_mode,
                                            uint8_t relu_pre_mode,
                                            bool enable_channel_split,
                                            bool enable_nz2nd,
                                            uint64_t quant_post,
                                            uint8_t relu_post,
                                            bool clip_relu_post,
                                            uint8_t eltwise_op,
                                            bool eltwise_antq_en,
                                            bool c0_pad_en,
                                            bool broadcast_en,
                                            bool enable_nz2dn)
```

### Data Types Supported by dtype

src dtype and dst dtype support the following combinations:

- When `src_dtype` is `int32_t`, `dst_dtype` supports `int4b_t`, `int8_t`, `uint8_t`, `half`, `bfloat16_t`, and `int32_t`.
- When `src_dtype` is `float`, `dst_dtype` supports `int4b_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, and `float`.

### Prototype Example

```cpp
// Example: Convert float data to half and then move it.
__aicore__ inline void asc_copy_l0c2ub(__ubuf__ half* dst,
                                       __cc__ float* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t dual_dst_ctrl,
                                       bool sub_blockid,
                                       uint8_t enable_clip_relu_pre,
                                       uint8_t unit_flag_mode,
                                       uint64_t quant_pre_mode,
                                       uint8_t relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       uint64_t quant_post,
                                       uint8_t relu_post,
                                       bool clip_relu_post,
                                       uint8_t eltwise_op,
                                       bool eltwise_antq_en,
                                       bool c0_pad_en,
                                       bool broadcast_en,
                                       bool enable_nz2dn)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination operand, stored in UB. The start address must be 32-byte aligned. |
| src | Input | Source operand, stored in the L0C Buffer. The start address must be 64-byte aligned. |
| n_size | Input | Size of the source Nz matrix in the N direction, with a value range of $[1, 2^{12}-1]$.<br>For the Nz output scenario:<br>&nbsp;&nbsp;&bull; If `enable_channel_split` is set to `true` to enable the Channel Split function, `n_size` must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the Channel Split function is not enabled, `n_size` must be a multiple of 16.<br>For the ND output scenario:<br>&nbsp;&nbsp;&bull; $n\_size\times\operatorname{sizeof}(dst\_dtype)$ Must be a multiple of 32; if `dst_dtype` is set to `int4b_t`, `n_size` must be a multiple of 64.<br> |
| m_size | Input | Size of the source Nz matrix in the M direction, with a value range of $[1, 2^{16}-1]$.<br>For the DN output scenario:<br>&nbsp;&nbsp;&bull; $m\_size\times\operatorname{sizeof}(dst\_dtype)$ Must be a multiple of 32; if `dst_dtype` is set to `int4b_t`, `m_size` must be a multiple of 64.<br> |
| dst_stride | Input | Destination matrix stride, with a value range of $[1, 2^{32}-1]$, and the corresponding byte length must be 32-byte aligned.<br>&nbsp;&nbsp;&bull; If the Nz2ND function is not enabled, `dst_stride` indicates the start address offset between adjacent Z layouts in the destination Nz matrix, in elements.<br>&nbsp;&nbsp;&bull; If the Nz2ND/Nz2DN function is enabled, `dst_stride` indicates the number of elements in each row of the destination ND/DN matrix, in elements. The alignment constraints for different `dst_dtype` values are as follows: `int4b_t` output must be a multiple of 64, 8-bit output must be a multiple of 32, 16-bit output must be a multiple of 16, and 32-bit output must be a multiple of 8.<br> |
| src_stride | Input | Start address offset between adjacent Z layouts in the source Nz matrix, in units of 64 bytes, that is, $16\times\operatorname{sizeof}(src\_dtype)$. The value range is $[0, 2^{16}-1]$. |
| dual_dst_ctrl | Input | Dual-destination mode control parameter. When there is one Cube Core and two Vector Cores in the same AI Core, if dual-destination mode control is enabled, the M×N matrix in the L0C Buffer is split into two halves and written simultaneously into the UBs of the two Vector Cores respectively, where the first half is written into SUB BLOCK0 and the second half into SUB BLOCK1. The parameter values are described as follows:<br>&nbsp;&nbsp;&bull; `0`: Single-destination mode, writes the entire matrix into the target UB configured by the `sub_blockid` parameter.<br>&nbsp;&nbsp;&bull; `1`: Dual-destination mode, splits the matrix along the M dimension into two matrices of shape $M/2\times N$, and writes them into two UBs respectively. M must be a multiple of 2.<br>&nbsp;&nbsp;&bull; `2`: Dual-destination mode, splits the matrix along the N dimension into two matrices of shape $M\times N/2$, and writes them into two UBs respectively. N must be a multiple of 32.<br>This parameter supports only the normal move mode (Nz2Nz) or the Nz2ND move scenario, and does not support other inline function scenarios. |
| sub_blockid | Input | Indicates the SUB BLOCK ID of the target UB when single-destination mode is enabled. The value is 0 or 1. A value of 0 writes into SUB BLOCK0, and a value of 1 writes into SUB BLOCK1. |
| enable_clip_relu_pre | Input | Whether to enable Clip ReLU. It must be used together with Normal ReLU, and the quantization function must be enabled. The values are as follows:<br>&nbsp;&nbsp;&bull; `0`: Do not enable Clip ReLU.<br>&nbsp;&nbsp;&bull; `1`: Enable Clip ReLU (scalar mode).<br> |
| unit_flag_mode | Input | UnitFlag is a fine-grained parallel function for MMAD-type instructions and matrix move-out instructions. When this function is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; `0`: Do not enable UnitFlag.<br>&nbsp;&nbsp;&bull; `2`: Enable UnitFlag. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; `3`: Enable UnitFlag. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this function is enabled, the UnitFlag values of the MMAD-type instructions and matrix move-out instructions must be set to 2 or 3. |
| quant_pre_mode | Input | Quantization mode in the pre-processing stage. For the values, see [Description](#description). |
| relu_pre_mode | Input | ReLU mode control in the pre-processing stage. The values are as follows:<br>&nbsp;&nbsp;&bull; `0`: Do not enable ReLU.<br>&nbsp;&nbsp;&bull; `1`: Enable Normal ReLU.<br>&nbsp;&nbsp;&bull; `2`: Enable Scalar ReLU.<br>&nbsp;&nbsp;&bull; `3`: Enable Vector ReLU.<br> |
| enable_channel_split | Input | Whether to enable the channel split function. It can be enabled only when both `src_dtype` and `dst_dtype` are `float` and the output is in Nz format.<br>&nbsp;&nbsp;&bull; `false`: Do not enable.<br>&nbsp;&nbsp;&bull; `true`: Enable.<br> |
| enable_nz2nd | Input | Nz2ND format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Disable Nz2ND conversion.<br>&nbsp;&nbsp;&bull; `true`: Enable Nz2ND conversion.<br> |
| quant_post | Input | Reserved parameter. It must be set to 0 currently. |
| relu_post | Input | Reserved parameter. It must be set to 0 currently. |
| clip_relu_post | Input | Reserved parameter. It must be set to `false` currently. |
| eltwise_op | Input | Reserved parameter. It must be set to 0 currently. |
| eltwise_antq_en | Input | Reserved parameter. It must be set to `false` currently. |
| c0_pad_en | Input | Reserved parameter. It must be set to `false` currently. |
| broadcast_en | Input | Reserved parameter. It must be set to `false` currently. |
| enable_nz2dn | Input | Nz2DN format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Disable Nz2DN conversion.<br>&nbsp;&nbsp;&bull; `true`: Enable Nz2DN conversion.<br> |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

### General Constraints

- If this API is not called by the AIC, it returns directly.
- The start address of `dst` must be 32-byte aligned; otherwise, an exception is triggered.
- The start address of `src` must be 64-byte aligned; otherwise, an exception is triggered.
- If the destination address of this instruction overlaps with that of other instructions, synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) must be inserted to serialize multiple instructions and prevent abnormal data.
- UB capacity limit: The total UB capacity is 256 KB. By default, 6 KB is reserved for the SIMD VF stack and 2 KB for Ascend C reserved space, leaving 248 KB available. When SIMD and SIMT are mixed, an additional 32 KB to 128 KB is allocated as Data Cache, further reducing the available capacity. The sum of the `dst` offset and the transfer size must not exceed the actual available capacity; otherwise, a write overflow exception is triggered.
- L0C Buffer capacity limit: The total L0C Buffer capacity is 256 KB. The sum of the `src` offset and the transfer size must not exceed the L0C Buffer capacity; otherwise, a read overflow exception is triggered.
- An Nz matrix uses 16×16 elements as one basic fractal. In boundary fractals, data beyond the range specified by `n_size` or `m_size` does not belong to the valid transfer result.

### Inline Conversion Constraints

- `n_size`, `m_size`, and `dst_stride` must satisfy the alignment constraints determined by the dtype and function mode. For details, see the parameter description. Failure to satisfy the alignment constraints will cause the copy result to be unexpected.
- The src and dst dtype combination must match the `quant_pre_mode` Quantization Mode; otherwise, the copy result will be unexpected.
- `dual_dst_ctrl` is supported only in the normal copy mode (Nz2Nz) or the Nz2ND copy scenario, and is not supported in other inline function scenarios.
- `enable_channel_split` can be set to true only when the output dtype is `float` and the output is in Nz format.
- The quantization coefficients used in the quantization and activation modes must not be INF/NaN or denormal numbers; otherwise, the quantization and activation result will be incorrect.
- When Nz2DN conversion is enabled, the source matrix stride must be pre-configured through [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and the source matrix stride must not be 0; otherwise, the copy will be abnormal.
- When Nz2DN conversion is enabled, the UnitFlag feature can be enabled simultaneously only when the source matrix stride configured through [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) is 1.
- When `enable_clip_relu_pre` is set to Clip ReLU (scalar mode), it must be used together with `relu_pre_mode` and the quantization feature.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
        asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
        asc_set_gm2l1_nz_para(1, 1, 128, 0);
        asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

        // Move matrices A and B from L1 Buffer to L0A Buffer and L0B Buffer.
        asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
        asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

        // Perform 128x128x128 matrix multiplication and write the result to L0C Buffer.
        asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

        // Convert the Nz result in L0C Buffer to continuous ND format and move it out to the AIV-side UB.
        asc_set_l0c_copy_nz_para(1, 0, ELEMENTS);
        asc_copy_l0c2ub(output_ub, c_l0, N, M, N, M, 0, false, 0, 0,
            static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
        asc_sync_block_arrive(PIPE_FIX, 0x8);
    }
    if ASC_IS_AIV {
        if (asc_get_sub_block_id() != 0) {
            return;
        }
        // Wait for the AI Core to complete the L0C-to-UB transfer, then have AIV0 move the result out to GM.
        asc_sync_block_wait(PIPE_MTE3, 0x8);
        asc_copy_ub2gm(output, output_ub, M, N * sizeof(int32_t),
            N * sizeof(int32_t), N * sizeof(int32_t));
        asc_sync_pipe(PIPE_MTE3);
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
