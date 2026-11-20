---
okftype: reference.api
description: "On the Ascend 950PR&950DT products, moves the L0C Buffer cube result out to Global Memory, combining real-time quantization, in-line activation, Nz2ND and Nz2DN format conversion, and channel splitting using strongly typed enums. The quantization, activation, and channel registers must be configured before the call."
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
    condition: "Set the quantization tensor start address for in-line tensor quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md
    kind: requires
    condition: "Configure the activation parameter for in-line Leaky ReLU activation"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: requires
    condition: "Configure the format conversion parameter before Nz2ND or Nz2DN format conversion"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "Set the quantization parameter for in-line scalar quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md
    kind: requires
    condition: "Configure the activation parameter for in-line ReLU activation"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/F32-Channel-Split.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Function Prototype"
    scope_basis: resolved
    note: "Added strongly-typed enum prototypes: l2_cache_mode, unit_flag_mode, quant_pre_mode, and relu_pre_mode are changed to asc_store_l2_cache_mode, asc_unit_flag_mode, asc_quant_mode, and asc_relu_pre_mode respectively, and the enable_nz2dn and enable_clip_relu_pre parameters are added, for a total of 14 overloads covering the int4b_t, fp8_e4m3fn_t, and hifloat8_t destination types; the integer-parameter prototypes of 9.1.0 (including 21 parameters such as l2_cache_ctl, clip_relu_pre, unit_flag_ctl, quant_post, and eltwise_op) are marked as deprecated in the same version and moved to the deprecated API documentation, but not removed."
---
# asc_copy_l0c2gm (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:10:09.876Z pushedAt=2026-11-18T08:15:34.303Z nl=2 -->

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

Moves cube computation results from the L0C Buffer to Global Memory. During the movement, it can simultaneously support combinations of capabilities such as real-time quantization, in-line activation, and in-line format conversion (Nz2ND/Nz2DN).

The following figure shows the valid combinations, intermediate data types, and data paths of real-time quantization, in-line ReLU, in-line format conversion, in-line channel splitting, and in-line channel merging. The connection line colors distinguish the supported scenario combinations. In the following figure, F32-\>F16 and F32-\>BF16 are Cast, and the rest are in-line scalar/tensor quantization modes.

**Figure 1** asc_copy_l0c2gm in-line function combinations

![](../../../../figures/C_API_L0C2GM_Function_Combination_950.png)

This interface supports combinations of multiple in-line capabilities. You need to pre-configure registers such as quantization parameters, activation parameters, and channel parameters through the supporting interfaces, and then call this interface to complete the data movement.

- In the Nz2ND format conversion scenario, you need to pre-configure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md), and use it together with the `enable_nz2nd` parameter of this interface.
- In the Nz2DN format conversion scenario, you need to pre-configure the format conversion parameters through [asc_set_l0c_copy_nz_para](../cube_store_aux_config/asc_set_l0c_copy_nz_para.md) and [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and use them together with the `enable_nz2dn` parameter of this interface.
- In the in-line scalar quantization mode, you need to set the in-line scalar quantization parameters through [asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md), and use them together with the `quant_pre_mode` parameter of this interface.
- In in-line tensor quantization mode, use [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md) to set the start address of the tensor used for in-line tensor quantization, where each element of the quantization tensor represents a quantization parameter, and it must be used together with the `quant_pre_mode` parameter of this API;
- In in-line activation mode, use [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) and [asc_set_l0c_copy_lrelu_alpha](../cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) to pre-configure the ReLU/Leaky ReLU activation parameters, and they must be used together with the `enable_clip_relu_pre` and `relu_pre_mode` parameters of this API;

The enumeration values supported by the `quant_pre_mode` quantization mode parameter are as follows:

- NoQuant: quantization is disabled.
- DEQF16: int32_t is quantized to half, scalar quantization.
- VDEQF16: int32_t is quantized to half, tensor quantization.
- REQ4: int32_t is quantized to int4b_t, scalar quantization.
- VREQ4: int32_t is quantized to int4b_t, tensor quantization.
- REQ8: int32_t is quantized to int8_t/uint8_t, scalar quantization.
- VREQ8: int32_t is quantized to int8_t/uint8_t, tensor quantization.
- QS322BF16_PRE: quantizes int32_t to bfloat16_t, scalar quantization.
- VQS322BF16_PRE: quantizes int32_t to bfloat16_t, tensor quantization.
- QF322F16_PRE: quantizes float to half, scalar quantization.
- VQF322F16_PRE: quantizes float to half, tensor quantization.
- QF322BF16_PRE: quantizes float to bfloat16_t, scalar quantization.
- VQF322BF16_PRE: quantizes float to bfloat16_t, tensor quantization.
- F322F16: casts float to half, with the cast mode set to CAST_RINT.
- F322BF16: casts float to bfloat16_t, with the cast mode set to CAST_RINT.
- QF322S4_PRE: quantizes float to int4b_t, scalar quantization.
- VQF322S4_PRE: quantizes float to int4b_t, tensor quantization.
- QF322B8_PRE: quantizes float to int8_t/uint8_t, scalar quantization.
- VQF322B8_PRE: quantizes float to int8_t/uint8_t, tensor quantization.
- QF322FP8_PRE: quantizes float to fp8_e4m3fn_t, scalar quantization.
- VQF322FP8_PRE: quantizes float to fp8_e4m3fn_t, tensor quantization.
- QF322HIF8_PRE: quantizes float to hifloat8_t (Half to Away Round), scalar quantization.
- VQF322HIF8_PRE: quantizes float to hifloat8_t (Half to Away Round), tensor quantization.
- QF322HIF8_PRE_HYBRID: quantizes float to hifloat8_t (Hybrid Round), scalar quantization.
- VQF322HIF8_PRE_HYBRID: quantizes float to hifloat8_t (Hybrid Round), tensor quantization.
- QF322F32_PRE: quantizes float to float, scalar quantization, with precision up to two thousandths but not two ten-thousandths.
- VQF322F32_PRE: quantizes float to float, tensor quantization, with precision up to two thousandths but not two ten-thousandths.

This interface is valid only when executed on the AIC.

For mode selection, coefficient preparation, and limitations of in-line processing, see [real-time quantization](../cube_store_key_features/accompanying_quantization.md), [in-line ReLU](../cube_store_key_features/accompanying_relu.md), and [scenario combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md). Whether the features can be used in combination is subject to the data types, modes, and constraints supported by this interface.

## Prototype

```c
__aicore__ inline void asc_copy_l0c2gm(__gm__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       asc_store_l2_cache_mode l2_cache_mode,
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

```c
// Example: Convert float data to bfloat16_t and then move it.
__aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t* dst,
                                       __cc__ float* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       asc_store_l2_cache_mode l2_cache_mode,
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
|---|---|---|
| dst | Output | Destination operand, stored in GM. The start address must be 1-byte aligned. |
| src | Input | Source operand, stored in L0C Buffer. The start address must be 64-byte aligned. |
| n_size | Input | Size of the source Nz cube in the N direction. Value range: $[1, 2^{12}-1]$.<br>For the Nz output scenario:<br>&nbsp;&nbsp;&bull; If `enable_channel_split` is set to `true` to enable the Channel Split feature, `n_size` must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the Channel Split feature is not enabled, `n_size` must be a multiple of 16.<br>For the ND output scenario:<br>&nbsp;&nbsp;&bull; If `dst_dtype` is set to `int4b_t`, `n_size` must be a multiple of 2.<br> |
| m_size | Input | Size of the source Nz cube in the M direction. Value range: $[1, 2^{16}-1]$.<br>For the DN output scenario:<br>&nbsp;&nbsp;&bull; If `dst_dtype` is set to `int4b_t`, `m_size` must be a multiple of 2.<br> |
| dst_stride | Input | Destination cube stride. Value range: $[1, 2^{32}-1]$.<br>&nbsp;&nbsp;&bull; If the Nz2ND feature is not enabled, `dst_stride` indicates the start address offset between adjacent Z layouts in the destination Nz cube, in elements.<br>&nbsp;&nbsp;&bull; If the Nz2ND/Nz2DN feature is enabled, `dst_stride` indicates the number of elements in each row of the destination ND/DN cube, in elements.<br>For the output scenario where `dst_dtype` is set to `int4b_t`, `dst_stride` must be a multiple of 2. |
| src_stride | Input | Start address offset between adjacent Z layouts in the source Nz cube, in units of 64 bytes, that is $16\times\operatorname{sizeof}(T)$, where $T$ is the data type of `src`. Value range: $[0, 2^{16}-1]$. |
| l2_cache_mode | Input | Configures the management policy for output GM data in L2 Cache. Only `asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM`, `asc_store_l2_cache_mode::NORMAL_LAST_VICTIM`, `asc_store_l2_cache_mode::NORMAL_PERSISTENT`, and `asc_store_l2_cache_mode::NOTALLOC_CLEAN` are supported. |
| unit_flag_mode | Input | UnitFlag is a fine-grained parallel feature for MMAD-type instructions and cube move-out instructions. When this feature is enabled, the hardware moves out the computation result each time a fractal is computed. Value description:<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`: UnitFlag is not enabled.<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`: UnitFlag is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`: UnitFlag is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When this feature is enabled, the UnitFlag values of MMAD-type instructions and cube move-out instructions must be set to `asc_unit_flag_mode::ENABLE_KEEP` or `asc_unit_flag_mode::ENABLE_UPDATE`.<br>For how to coordinate with compute instructions, see [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md). |
| quant_pre_mode | Input | Quantization mode in the preprocessing stage. For values, see [Description](#description).<br>For coefficient meanings and configuration steps, see [real-time quantization](../cube_store_key_features/accompanying_quantization.md). |
| relu_pre_mode | Input | ReLU mode control in the preprocessing stage. Values are as follows:<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::NONE`: ReLU is not enabled.<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::NORMAL`: Normal ReLU is enabled.<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::SCALAR`: Scalar ReLU is enabled.<br>&nbsp;&nbsp;&bull; `asc_relu_pre_mode::VECTOR`: Vector ReLU is enabled.<br><br>For modes and configuration interfaces, see [in-line ReLU](../cube_store_key_features/accompanying_relu.md). |
| enable_channel_split | Input | Whether to enable the channel split feature. It can be enabled only when both `src_dtype` and `dst_dtype` are `float` and the output is in Nz format.<br>&nbsp;&nbsp;&bull; `false`: Not enabled.<br>&nbsp;&nbsp;&bull; `true`: Enabled.<br><br>For features and supported paths, see [F32 Channel Split](../cube_store_key_features/F32-Channel-Split.md). |
| enable_nz2nd | Input | Nz2ND format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Nz2ND conversion is disabled.<br>&nbsp;&nbsp;&bull; `true`: Nz2ND conversion is enabled.<br><br>For layout and stride examples, see [NZ2ND](../cube_store_key_features/NZ2ND.md); when format conversion is disabled, see [NZ2NZ](../cube_store_key_features/NZ2NZ.md). |
| enable_nz2dn | Input | Nz2DN format conversion enable.<br>&nbsp;&nbsp;&bull; `false`: Nz2DN conversion is disabled.<br>&nbsp;&nbsp;&bull; `true`: Nz2DN conversion is enabled.<br><br>For layout and configuration methods, see [NZ2DN](../cube_store_key_features/NZ2DN.md). |
| enable_clip_relu_pre | Input | Whether to enable Clip ReLU. It must be used together with Normal ReLU, and the quantization feature must be enabled. Values are as follows:<br>&nbsp;&nbsp;&bull; `false`: Clip ReLU is not enabled.<br>&nbsp;&nbsp;&bull; `true`: Clip ReLU is enabled (scalar mode).<br> |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

### General Constraints

- If this interface is not called by AIC, it returns directly.
- The `src` start address must be 64-byte aligned; otherwise, a transfer exception occurs.
- The `dst` start address must be 1-byte aligned; otherwise, a transfer exception occurs.
- If this instruction has data dependencies with other instructions, you need to insert [synchronization instructions](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L0C Buffer capacity limit: The total L0C Buffer capacity is 256KB. The sum of the src offset and the transfer size must not exceed the L0C Buffer capacity; otherwise, a source address out-of-bounds exception is triggered.
- An Nz cube uses 16×16 elements as one basic fractal. In boundary fractals, data beyond the range specified by `n_size` or `m_size` does not belong to the valid transfer result.

### In-line Conversion Constraints

- `n_size`, `m_size`, and `dst_stride` must follow the alignment constraints determined by the dtype and function mode. For details, see the parameter description. Failure to meet the alignment constraints will cause the transfer result to be unexpected.
- The src and dst dtype combination must match the `quant_pre_mode` quantization mode; otherwise, the transfer result will be unexpected.
- `enable_channel_split` can be set to true only when the output dtype is `float` and the output is in Nz format.
- The quantization coefficients used in quantization and activation modes must not be INF/NAN or denormalized numbers; otherwise, the quantization and activation results will be incorrect.
- When Nz2DN conversion is enabled, you must pre-configure the source cube stride through [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md), and the source cube stride must not be 0; otherwise, a transfer exception will occur.
- When Nz2DN conversion is enabled, the UnitFlag function can be enabled simultaneously only when the source cube stride configured through [asc_set_l0c_copy_channel_para](../cube_store_aux_config/asc_set_l0c_copy_channel_para.md) is 1.
- When `enable_clip_relu_pre` is set to Clip ReLU (scalar mode), it must be used together with `relu_pre_mode` and the quantization function.

## Examples

For the complete project sample, see [data_copy_l0c2gm sample](../../../../../../../examples/02_simd_c_api/03_c_api/00_data_movement/data_copy_l0c2gm).

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

This example occupies 32KB L1 Buffer, 16KB L0A Buffer, 16KB L0B Buffer, and 64KB L0C Buffer, and uses `EVENT_ID0` for pipeline synchronization.

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
    asc_copy_gm2l1_nd2nz(a_l1, a, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move cubes A and B from L1 Buffer into L0A Buffer and L0B Buffer.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Perform 128x128x128 cube multiplication and write the result to L0C Buffer.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // Convert the Nz result in L0C Buffer to contiguous ND format and move it out to GM.
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, asc_quant_mode::NoQuant, asc_relu_pre_mode::NONE,
        false, true, false, false);
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
    // Construct asymmetric inputs and compute the golden result on the host side.
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
