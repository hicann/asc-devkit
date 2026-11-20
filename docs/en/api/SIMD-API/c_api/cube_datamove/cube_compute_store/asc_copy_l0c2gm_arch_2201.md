---
okftype: reference.api
description: "Stores the quantized cube result from the L0C Buffer to Global Memory on Atlas A3 and Atlas A2 products, covering 9 associated quantization modes, with optional associated ReLU and F32 channel splitting, and enabling NZ2ND format conversion as needed."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_key_features/UnitFlag.md
    kind: constrained_by
    condition: "The unit_flag_mode value must coordinate with the unit_flag setting of MMAD instructions"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12fb_arch_2201.md
    kind: requires
    condition: "Loads the Quantization Parameter into the Fixpipe Buffer during associated Vector Quantization, only for Atlas A3 products and Atlas A2 products"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "Configure the quantization parameter start address index during vector quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_params.md
    kind: requires
    condition: "Configure the cube count and cube interval when NZ2ND is enabled"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "Set the quantization parameter during associated scalar quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space and alignment, generic address overlap constraint"
version: ">=9.1.0"
version_changes: []
---
# asc_copy_l0c2gm (NPU Architecture Version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:08:59.718Z pushedAt=2026-11-18T08:15:34.296Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
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

After cube computation is complete, quantization is performed on the result, and then the processed result is stored to GM. There are 9 quantization modes in total:
- NoQuant: Quantization is not enabled.
- F322F16: float is quantized to half. The quantization result does not support the INF_NAN mode.
- F322BF16: float is quantized to bfloat16_t. The quantization result does not support the INF_NAN mode.
- DEQF16: int32_t is quantized to half. The quantization result does not support the INF_NAN mode.
- VDEQF16: int32_t is quantized to half. The quantization result does not support the INF_NAN mode.
- QF322B8_PRE: float is quantized to uint8_t/int8_t. Scalar quantization.
- VQF322B8_PRE: float is quantized to uint8_t/int8_t. Vector quantization.
- REQ8: Quantizes int32_t to uint8_t/int8_t. Scalar quantization.
- VREQ8: Quantizes int32_t to uint8_t/int8_t. Vector quantization.

For mode selection, coefficient preparation, and restrictions of associated processing, see [Associated Quantization](../cube_store_key_features/accompanying_quantization.md), [Associated ReLU](../cube_store_key_features/accompanying_relu.md), and [Scenario Combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md). Whether features can be combined depends on the data types, modes, and constraints supported by this API.

Scalar quantization parameters are set through [asc_set_l0c_copy_prequant(uint64_t config)](../cube_store_aux_config/asc_set_l0c_copy_prequant.md). Vector quantization parameters are loaded through [asc_copy_l12fb (Atlas A3 products and Atlas A2 products)](../cube_compute_load/asc_copy_l12fb_arch_2201.md), and the start address index is configured through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md). The number of cubes and cube interval for NZ2ND are configured through [asc_set_l0c_copy_params](../cube_store_aux_config/asc_set_l0c_copy_params.md).

## Prototype

- Regular transfer

    ```cpp
    __aicore__ inline void asc_copy_l0c2gm(__gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| n_size | Input | Size of the source NZ cube in the N direction.<br> - When NZ2ND is disabled: if channel_split is enabled, n_size must be a multiple of 8, with a value range of [1, 4095]. If channel_split is disabled, n_size must be a multiple of 16, with a value range of [1, 4095].<br>- When NZ2ND is enabled: the value range of n_size is [1, 4095]. |
| m_size | Input | Size of the source NZ cube in the M direction.<br> - When NZ2ND is disabled: the value range is [1, 65535].<br> - When NZ2ND is enabled: the value range of m_size is [1, 8192]. |
| dst_stride_dst_d | Input | <br> - When NZ2ND is disabled: the start address offset between adjacent Z layouts in the destination NZ cube. The value must not be 0. Unit: 32 bytes.<br> - When NZ2ND is enabled: the number of elements in each row of the destination ND cube. The value must not be 0. Unit: element.|
| src_stride | Input | Start address offset between adjacent Z layouts in the source NZ cube. Value range: [0, 65535]. Unit: C0_Size (16*sizeof(T), where T is the data type of src). |
| unit_flag_mode | Input | Related to the unit_flag parameter. Values are as follows:<br>0: reserved value;<br>2: enables unit_flag. After the hardware finishes executing the instruction, the register is not set;<br>3: enables unit_flag. After the hardware finishes executing the instruction, unit_flag is disabled.<br>For how to use it with compute instructions, see [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md). |
| quant_pre |Input|Quantization parameter. For values, see [Function Description](asc_copy_l0c2gm_arch_2201.md#description).<br>For coefficient meanings and configuration steps, see [Associated Quantization](../cube_store_key_features/accompanying_quantization.md). |
| relu_pre | Input | Enables ReLU.<br>For modes and configuration APIs, see [Associated ReLU](../cube_store_key_features/accompanying_relu.md). |
| channel_split | Input | Whether to enable channel split. The default value is false, meaning the feature is disabled. Channel split can be enabled only when both src and dst are float, and channel_split and NZ2ND cannot be enabled at the same time.<br>For features and supported paths, see [F32 Channel Split](../cube_store_key_features/F32-Channel-Split.md). |
| nz2nd_en | Input |Enables the NZ2ND switch. false: disabled; true: enabled.<br>For layout and stride examples, see [NZ2ND](../cube_store_key_features/NZ2ND.md); when format conversion is disabled, see [NZ2NZ](../cube_store_key_features/NZ2NZ.md). |
## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- Quantization parameters cannot be inf/nan or denormalized numbers.
- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The quantization coefficients used in quantization and activation modes must not be INF/NAN or denormal numbers; otherwise, the quantization activation result will be incorrect.

## Examples

```cpp
// total_length indicates the total length of data involved in the transfer.
constexpr uint64_t total_length = 128;
// dst and src correspond to the output address of the destination operand and the input address of the source operand, respectively. dst is an externally input int32_t GM address.
__cc__ int32_t src[total_length];
// Pass the remaining input parameters with their default values.
uint16_t n_size = 16;
uint16_t m_size = 16;
uint32_t dst_stride_dst_d = 32;
uint16_t src_stride = 1;
uint8_t unit_flag_mode = 0;
uint64_t quant_pre = QuantMode_t::NoQuant;
uint8_t relu_pre = 0;
bool channel_split = false;
bool nz2nd_en = false;
// Function call.
asc_copy_l0c2gm(dst, src, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode, quant_pre, relu_pre, channel_split, nz2nd_en);
asc_sync();
```
