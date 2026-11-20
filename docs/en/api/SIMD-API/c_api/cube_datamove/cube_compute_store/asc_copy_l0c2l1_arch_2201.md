---
okftype: reference.api
description: "On Atlas A3 and Atlas A2 products, quantize and activate the L0C Buffer cube result and then move it into the L1 Buffer, covering 9 associated quantization modes, with optional associated ReLU, F32 channel splitting, and NZ2ND format conversion, and providing a void pointer generic overload."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_key_features/UnitFlag.md
    kind: constrained_by
    condition: "The unit_flag_mode value must be coordinated with the unit_flag setting of MMAD instructions"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12fb_arch_2201.md
    kind: requires
    condition: "During associated vector quantization, move the quantization parameters into the Fixpipe Buffer, only for Atlas A3 products and Atlas A2 products"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "Configure the quantization parameter start address index during vector quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_params.md
    kind: requires
    condition: "Configure the number of cubes and cube interval when NZ2ND is enabled"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md
    kind: requires
    condition: "Set the quantization parameters during associated scalar quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space and alignment, general address overlap constraints"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Function Prototype"
    scope_basis: resolved
    note: "New feature in CANN_9_1_0.md: added asc_copy_l0c2l1_sync, and added two combinations: int16_t destination with int32_t source, and bfloat16_t destination with float source, as well as a generic overload with destination as __cbuf__ void* and source as __cc__ float* and __cc__ int32_t* respectively (the latter uses the int4b_t quantization path)."
---
# asc_copy_l0c2l1 (NPU Architecture Version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:13:24.530Z pushedAt=2026-11-18T08:15:34.300Z nl=2 -->

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

After cube computation is complete, the result is quantized, and then the processed result is moved to the L1 Buffer. There are 9 quantization modes in total:
- NoQuant: Quantization is not enabled.
- DEQF16: int32_t is quantized to half. The quantization result does not support the INF_NAN mode.
- F322BF16: float is quantized to bfloat16_t. The quantization result does not support the INF_NAN mode.
- F322F16: float is quantized to half. The quantization result does not support the INF_NAN mode.
- QF322B8_PRE: float is quantized to uint8_t/int8_t. Scalar quantization.
- REQ8: int32_t is quantized to uint8_t/int8_t. Scalar quantization.
- VDEQF16: int32_t is quantized to half. The quantization result does not support the INF_NAN mode.
- VQF322B8_PRE: Quantizes float into uint8_t/int8_t. Vector quantization.
- VREQ8: Quantizes int32_t into uint8_t/int8_t. Vector quantization.

In addition, this copy instruction also supports quantization of the int4b_t type. The related quantization types are divided into two kinds: quantizing int32_t into int4b_t, and quantizing float into int4b_t.

For the mode selection, coefficient preparation, and restrictions of associated processing, see [Associated Quantization](../cube_store_key_features/accompanying_quantization.md), [Associated ReLU](../cube_store_key_features/accompanying_relu.md), and [Scenario Combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md). Whether the features can be used in combination is subject to the data types, modes, and constraints supported by this interface.

Scalar quantization parameters are set through [asc_set_l0c_copy_prequant(uint64_t config)](../cube_store_aux_config/asc_set_l0c_copy_prequant.md). Vector quantization parameters are moved in through [asc_copy_l12fb (Atlas A3 products and Atlas A2 products)](../cube_compute_load/asc_copy_l12fb_arch_2201.md), and the start address index is configured through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md). The number of cubes and the cube interval of Nz2ND are configured through [asc_set_l0c_copy_params](../cube_store_aux_config/asc_set_l0c_copy_params.md).

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ half* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ void* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1(__cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|:---------------------|:------|:---------------------------------------------------------------------------------------------------------------------------------|
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand. |
| n_size | Input | Size of the source NZ cube in the N direction.<br/>&bull; When NZ2ND is disabled, value range: [1, 4095];<br/>&bull; When NZ2ND is enabled, value range: [1, 4095]. |
| m_size | Input | Size of the source NZ cube in the M direction.<br/>&bull; When NZ2ND is disabled, value range: [1, 65535];<br/>&bull; When NZ2ND is enabled, value range: [1, 8192]. |
| dst_stride | Input | &bull; When NZ2ND is disabled, the start address offset between adjacent Z layouts in the destination NZ cube. The value is not 0. Unit: 32 bytes.<br/>&bull; When NZ2ND is enabled, the number of elements in each row of the destination ND cube. The value is not 0. Unit: element. |
| src_stride | Input | Start address offset between adjacent Z layouts in the source NZ cube. Value range: [0, 65535]. Unit: C0_Size(16*sizeof(T), where T is the data type of src). |
| unit_flag_mode | Input | Related to the unit_flag parameter. Values are as follows:<br/>&bull; 0: reserved value;<br/>&bull; 2: enable unit_flag. After the hardware finishes executing the instruction, the register is not set;<br/>&bull; 3: enable unit_flag. After the hardware finishes executing the instruction, unit_flag is disabled.<br>For how to cooperate with compute instructions, see [UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md). |
| quant_pre | Input | Quantization parameter. For values, see [Function Description](asc_copy_l0c2l1_arch_2201.md#description).<br>For the coefficient meaning and configuration steps, see [Associated Quantization](../cube_store_key_features/accompanying_quantization.md). |
| relu_pre | Input | Enable relu.<br>For modes and configuration interfaces, see [Associated ReLU](../cube_store_key_features/accompanying_relu.md). |
| enable_channel_split | Input | Whether to enable the channel split function. The default value is false, which means the function is disabled. Channel split can be enabled only when both src and dst are float, and channel_split and NZ2ND cannot be enabled at the same time.<br>For features and supported paths, see [F32 Channel Split](../cube_store_key_features/F32-Channel-Split.md). |
| enable_nz2nd | Input | Enable the NZ2ND switch.<br/>&bull; false: disabled;<br/>&bull; true: enabled.<br>For the layout and stride examples, see [NZ2ND](../cube_store_key_features/NZ2ND.md); when format conversion is disabled, see [NZ2NZ](../cube_store_key_features/NZ2NZ.md). |
## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- Quantization parameters cannot be inf/nan or denormalized numbers.
- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If this instruction has data dependencies with other instructions, you need to insert a [sync instruction](../../sync/overview.md#synchronization-control-overview) to ensure that multiple instructions are serialized and prevent abnormal data.
- The quantization coefficients used in quantization and activation modes must not be INF/NAN or denormalized numbers; otherwise, the quantization activation result will be incorrect.

## Examples

```cpp
// dst and src correspond to the output address of the destination operand and the input address of the source operand, respectively.
constexpr uint64_t total_length = 256;
__cbuf__ int32_t dst[total_length];
__cc__ int32_t src[total_length];
// The remaining input parameters are passed with default values.
uint16_t n_size = 16;
uint16_t m_size = 16;
uint32_t dst_stride = 8;
uint16_t src_stride = 8;
uint8_t unit_flag_mode = 0;
uint64_t quant_pre = QuantMode_t::NoQuant;
uint8_t relu_pre = 0;
bool channel_split = false;
bool enable_nz2nd = false;
asc_copy_l0c2l1(dst, src, n_size, m_size, dst_stride, src_stride, unit_flag_mode, quant_pre, relu_pre, enable_channel_split, enable_nz2nd);
```
