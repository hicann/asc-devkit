---
okftype: reference.api
description: "Deprecated API page: the old-parameter-form asc_copy_l0c2gm and _sync overloads on the 3510 architecture move the matrix result out of the L0C Buffer to GM, and can combine in-band quantization, ReLU, and Nz2ND or Nz2DN format conversion. The corresponding parameter registers must be configured first. It is recommended to use the strongly-typed enum overload plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [conversion, store]
  - traits: [aligned_transfer, quantization, rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using the new overload with strongly-typed enum parameters plus asc_sync instead"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md
    kind: requires
    condition: "The Nz2DN scenario requires the source matrix stride to be preconfigured"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "In-band tensor quantization requires the quantization tensor start address to be configured first"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md
    kind: requires
    condition: "In-band Leaky ReLU requires the activation parameter to be configured first"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: requires
    condition: "Nz2ND or Nz2DN must be configured with the format conversion parameter before being enabled"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "In-band scalar quantization requires the quantization parameter to be configured first"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md
    kind: requires
    condition: "In-band ReLU requires the activation parameter to be configured first"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists the old-parameter overload asc_copy_l0c2gm and the corresponding _sync overload as deprecated. The replacement path is the strongly-typed enum parameter overload plus asc_sync; the include still retains the prototype, which is deprecation rather than removal."
---
# asc_copy_l0c2gm (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:52:42.570Z pushedAt=2026-11-18T03:12:16.411Z nl=2 -->

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

**The legacy parameter form of `asc_copy_l0c2gm` and the `asc_copy_l0c2gm_sync` interface are deprecated. Use the [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) interface and the [asc_sync](../sync/intra_core_sync/asc_sync.md) interface instead.**

The header file paths are: `"c_api/cube_datamove/cube_datamove.h"` and `"c_api/composite/cube_datamove_composite.h"`.

Moves the matrix computation result from the L0C Buffer to Global Memory. During the movement, it can simultaneously support combinations of capabilities such as in-band quantization, in-band activation, and in-band format conversion (Nz2ND/Nz2DN).

The following figure shows the valid combinations, intermediate data types, and data paths of in-band quantization, in-band ReLU, in-band format conversion, in-band channel split, and in-band channel merge. In the figure, F32-\>F16 and F32-\>BF16 are Cast operations, and the rest are in-band scalar/tensor quantization modes.

**Figure 1** asc_copy_l0c2gm in-band function combination

![](../../../figures/C_API_L0C2GM_Function_Combination_950.png)

This interface supports combinations of multiple in-band capabilities. You must preconfigure registers such as quantization parameters, activation parameters, and channel parameters through the companion interfaces, and then call this interface to complete the movement.

- In the Nz2ND format conversion scenario, format conversion parameters must be preconfigured through [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md), and this must be used together with the `enable_nz2nd` parameter of this interface;
- In the Nz2DN format conversion scenario, format conversion parameters must be preconfigured through [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) and [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and this must be used together with the `enable_nz2dn` parameter of this interface;
- In in-band scalar quantization mode, the in-band scalar quantization parameters must be set through [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md), and this must be used together with the `quant_pre_mode` parameter of this API;
- In in-band tensor quantization mode, the start address of the tensor used for in-band tensor quantization must be set through [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md), where each element of the quantization tensor represents a quantization parameter, and this must be used together with the `quant_pre_mode` parameter of this API;
- In in-band activation mode, the ReLU/Leaky ReLU activation parameters must be preconfigured through [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) and [asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md), and these must be used together with the `enable_clip_relu_pre` and `relu_pre_mode` parameters of this API;

The enum values supported by the `quant_pre_mode` quantization mode parameter are as follows:

- NoQuant: quantization is not enabled.
- DEQF16: int32_t quantize to half, scalar quantization.
- VDEQF16: int32_t quantize to half, tensor quantization.
- REQ4: int32_t quantize to int4b_t, scalar quantization.
- VREQ4: int32_t quantize to int4b_t, tensor quantization.
- REQ8: int32_t quantize to int8_t/uint8_t, scalar quantization.
- VREQ8: quantize int32_t to int8_t/uint8_t, tensor quantization.
- QS322BF16_PRE: quantize int32_t to bfloat16_t, scalar quantization.
- VQS322BF16_PRE: quantize int32_t to bfloat16_t, tensor quantization.
- QF322F16_PRE: quantize float to half, scalar quantization.
- VQF322F16_PRE: quantize float to half, tensor quantization.
- QF322BF16_PRE: quantize float to bfloat16_t, scalar quantization.
- VQF322BF16_PRE: quantize float to bfloat16_t, tensor quantization.
- F322F16: cast float to half, cast mode is CAST_RINT mode.
- F322BF16: cast float to bfloat16_t, cast mode is CAST_RINT mode.
- QF322S4_PRE: quantize float to int4b_t, scalar quantization.
- VQF322S4_PRE: quantize float to int4b_t, tensor quantization.
- QF322B8_PRE: quantize float to int8_t/uint8_t, scalar quantization.
- VQF322B8_PRE: quantize float to int8_t/uint8_t, tensor quantization.
- QF322FP8_PRE: quantize float to fp8_e4m3fn_t, scalar quantization.
- VQF322FP8_PRE: quantize float to fp8_e4m3fn_t, tensor quantization.
- QF322HIF8_PRE: quantize float to hifloat8_t (Half to Away Round), scalar quantization.
- VQF322HIF8_PRE: quantize float to hifloat8_t (Half to Away Round), tensor quantization.
- QF322HIF8_PRE_HYBRID: quantize float to hifloat8_t (Hybrid Round), scalar quantization.
- VQF322HIF8_PRE_HYBRID: quantize float to hifloat8_t (Hybrid Round), tensor quantization.
- QF322F32_PRE: quantize float to float, scalar quantization, precision can reach two per thousand but cannot reach two per ten thousand.
- VQF322F32_PRE: quantize float to float, tensor quantization, with precision up to two per thousand but not two per ten thousand.

This interface is valid only when executed on the AIC.

## Prototype

```cpp
__aicore__ inline void asc_copy_l0c2gm(__gm__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t l2_cache_mode,
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
__aicore__ inline void asc_copy_l0c2gm_sync(__gm__ <dst_dtype>* dst,
                                            __cc__ <src_dtype>* src,
                                            uint16_t n_size,
                                            uint16_t m_size,
                                            uint32_t dst_stride,
                                            uint16_t src_stride,
                                            uint8_t l2_cache_mode,
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
// Example: Convert float data to bfloat16_t and then move it.
__aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t* dst,
                                       __cc__ float* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       uint8_t l2_cache_mode,
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
| dst | Output | Destination operand, stored in GM. The start address must be 1-byte aligned. |
| src | Input | Source operand, stored in the L0C Buffer. The start address must be 64-byte aligned. |
| n_size | Input | Size of the source Nz matrix in the N direction, with a value range of $[1, 2^{12}-1]$.<br>For Nz output scenarios:<br>&nbsp;&nbsp;&bull; If `enable_channel_split` is set to `true` to enable the Channel Split feature, `n_size` must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the Channel Split feature is not enabled, `n_size` must be a multiple of 16.<br>For ND output scenarios:<br>&nbsp;&nbsp;&bull; If `dst_dtype` is set to `int4b_t`, `n_size` must be a multiple of 2.<br> |
| m_size | Input | Size of the source Nz matrix in the M direction, with a value range of $[1, 2^{16}-1]$.<br>For DN output scenarios:<br>&nbsp;&nbsp;&bull; If `dst_dtype` is set to `int4b_t`, `m_size` must be a multiple of 2.<br> |
| dst_stride | Input | Destination matrix stride, with a value range of $[1, 2^{32}-1]$.<br>&nbsp;&nbsp;&bull; If the Nz2ND feature is not enabled, `dst_stride` indicates the start address offset between adjacent Z layouts in the destination Nz matrix, in elements.<br>&nbsp;&nbsp;&bull; If the Nz2ND/Nz2DN feature is enabled, `dst_stride` indicates the number of elements in each row of the destination ND/DN matrix, in elements.<br>For output scenarios where `dst_dtype` is set to `int4b_t`, `dst_stride` must be a multiple of 2. |
| src_stride | Input | Start address offset between adjacent Z layouts in the source Nz matrix, in units of 64 bytes, that is $16\times\operatorname{sizeof}(T)$, where $T$ is the data type of `src`. The value range is $[0, 2^{16}-1]$. |
| l2_cache_mode | Input | Configures the management policy for output GM data in the L2 Cache. The value must be a defined policy, that is 0, 1, 2, or 4; other values trigger an L2 Cache policy exception. See [Table 2](#l2_cache_mode_values) for value descriptions. |
| enable_clip_relu_pre | Input | Whether to enable Clip ReLU. It must be used together with Normal ReLU and requires quantization to be enabled. Values:<br>&nbsp;&nbsp;&bull; `0`: Disable Clip ReLU.<br>&nbsp;&nbsp;&bull; `1`: Enable Clip ReLU (scalar mode).<br> |
| unit_flag_mode | Input | UnitFlag is a fine-grained parallel feature for MMAD-type instructions and matrix move-out instructions. When enabled, the hardware moves out the computation result after each fractal is computed. Value descriptions:<br>&nbsp;&nbsp;&bull; `0`: Disable UnitFlag.<br>&nbsp;&nbsp;&bull; `2`: Enable UnitFlag. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; `3`: Enable UnitFlag. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When enabling this feature, set the UnitFlag value of both MMAD-type instructions and matrix move-out instructions to 2 or 3. |
| quant_pre_mode | Input | Quantization mode in the preprocessing stage. See [Description](#description) for values. |
| relu_pre_mode | Input | ReLU mode control in the preprocessing stage. Values:<br>&nbsp;&nbsp;&bull; `0`: Disable ReLU.<br>&nbsp;&nbsp;&bull; `1`: Enable Normal ReLU.<br>&nbsp;&nbsp;&bull; `2`: Enable Scalar ReLU.<br>&nbsp;&nbsp;&bull; `3`: Enable Vector ReLU.<br> |
| enable_channel_split | Input | Whether to enable the channel split feature. It can be enabled only when both `src_dtype` and `dst_dtype` are `float` and the output is in Nz format.<br>&nbsp;&nbsp;&bull; `false`: Disable.<br>&nbsp;&nbsp;&bull; `true`: Enable.<br> |
| enable_nz2nd | Input | Nz2ND format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Disable Nz2ND conversion.<br>&nbsp;&nbsp;&bull; `true`: Enable Nz2ND conversion.<br> |
| quant_post | Input | Reserved parameter. It must be set to 0 currently. |
| relu_post | Input | Reserved parameter. It must be set to 0 currently. |
| clip_relu_post | Input | Reserved parameter. It must be set to `false` currently. |
| eltwise_op | Input | Reserved parameter. It must be set to 0 currently. |
| eltwise_antq_en | Input | Reserved parameter. It must be set to `false` currently. |
| c0_pad_en | Input | Reserved parameter. It must be set to `false` currently. |
| broadcast_en | Input | Reserved parameter. It must be set to `false` currently. |
| enable_nz2dn | Input | Nz2DN format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Disable Nz2DN conversion.<br>&nbsp;&nbsp;&bull; `true`: Enable Nz2DN conversion.<br> |

**Table 2** l2_cache_mode value description <a id="l2_cache_mode_values"></a>

| Value | Mode | Meaning |
|------|------|------|
| 0 | `NORMAL mode` | Enable L2 Cache and mark the allocated Cache Lines as high replacement priority. |
| 1 | `LAST mode` | &bull; Enable L2 Cache and mark the allocated Cache Lines as low replacement priority.<br>&bull; **LAST mode: the feature is not supported yet.** |
| 2 | `PERSISTENT mode` | &bull; Enable L2 Cache. Data already stored in the L2 Cache may be replaced. To ensure that data of a specific GM always remains in the L2 Cache, use the persistent mode.<br>&bull; Note that Cache Lines marked as persistent mode can only be replaced by other Cache Lines also marked as persistent mode.<br>&bull; **PERSISTENT mode: the feature is not supported yet.** |
| 4 | `DISABLE mode` | Disable L2 Cache. Read directly from GM each time and keep the state of existing Cache Lines unchanged. |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

### General Constraints

- If this API is not called by the AIC, it returns directly.
- The `src` start address must be 64-byte aligned; otherwise, a migration exception occurs.
- The `dst` start address must be 1-byte aligned; otherwise, a migration exception occurs.
- If the destination address of this instruction overlaps with that of other instructions, a synchronization instruction must be inserted. For details, see [In-Core Synchronization](../sync/overview.md#synchronization-control-overview).
- L0C Buffer capacity limit: The total L0C Buffer capacity is 256KB. The sum of the `src` offset and the movement size must not exceed the L0C Buffer capacity; otherwise, a source address out-of-bounds exception is triggered.
- An Nz matrix uses 16×16 elements as one basic fractal. In boundary fractals, data beyond the range specified by `n_size` or `m_size` does not belong to the valid movement result.

### Inline Conversion Constraints

- `n_size`, `m_size`, and `dst_stride` must determine the alignment constraints based on the dtype and function mode. For details, see the parameter description. Failure to meet the alignment constraints will cause the movement result to be unexpected.
- The src and dst dtype combination must match the `quant_pre_mode` quantization mode; otherwise, the movement result will be unexpected.
- `enable_channel_split` can be set to true only when the output dtype is `float` and the output is in Nz format.
- The quantization coefficients used in the quantization and activation modes must not be INF/NaN or denormalized numbers; otherwise, the quantization activation result will be incorrect.
- When Nz2DN conversion is enabled, the source matrix stride must be preconfigured through [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and the source matrix stride must not be 0; otherwise, a migration exception will occur.
- When Nz2DN conversion is enabled, the UnitFlag function can be enabled simultaneously only when the source matrix stride configured through [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) is 1.
- When `enable_clip_relu_pre` is set to Clip ReLU (scalar mode), it must be used together with `relu_pre_mode` and the quantization function.

## Examples

For the complete project sample, see [data_copy_l0c2gm sample](../../../../../../examples/02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2gm).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

This example occupies 32KB L1 Buffer, 16KB L0A Buffer, 16KB L0B Buffer, and 64KB L0C Buffer, and uses `EVENT_ID0` to complete pipeline synchronization.

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

__global__ __cube__ void AscCopyL0c2gmKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];

    // Convert the two 128x128 ND inputs to the Nz layout required by MMAD.
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move matrices A and B from L1 Buffer into L0A Buffer and L0B Buffer.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Perform 128x128x128 matrix multiplication and write the result into L0C Buffer.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in L0C Buffer to continuous ND format and move it out to GM.
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
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
    // Construct asymmetric inputs and compute the Golden result on the Host side.
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
    AscCopyL0c2gmKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_l0c2gm result is correct."
                         : "[Failed] asc_copy_l0c2gm result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
