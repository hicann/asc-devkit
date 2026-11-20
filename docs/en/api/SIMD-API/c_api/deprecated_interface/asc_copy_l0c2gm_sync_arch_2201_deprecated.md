---
okftype: reference.api
description: "Deprecated interface page: On the 2201 architecture, asc_copy_l0c2gm_sync processes the result according to the quantization mode after matrix computation and synchronously copies it to GM, supporting NoQuant and 9 quantization modes including F322F16, F322BF16, DEQF16, REQ8, and VREQ8. It is recommended to use the non-synchronous asc_copy_l0c2gm plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [conversion, store]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Performs synchronous waiting in the alternative solution"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 2201 synchronous interface with ASC_DEPRECATED(9.2.0). The alternative path is the non-synchronous asc_copy_l0c2gm plus asc_sync; the interface has not been removed."
---
# asc_copy_l0c2gm_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:52:33.996Z pushedAt=2026-11-18T03:12:16.408Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l0c2gm_sync` interface is deprecated. Use the [corresponding asynchronous interface](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_2201.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

After matrix computation is complete, the result is quantized, and then the processed result is copied to GM. There are 9 quantization modes in total:
- NoQuant: Quantization is disabled.
- F322F16: Quantize float to half. The quantization result does not support the INF_NAN mode.
- F322BF16: Quantize float to bfloat16_t. The quantization result does not support the INF_NAN mode.
- DEQF16: Quantize int32_t to half. The quantization result does not support the INF_NAN mode.
- VDEQF16: Quantize int32_t to half. The quantization result does not support the INF_NAN mode.
- QF322B8_PRE: Quantize float to uint8_t/int8_t. Scalar quantization.
- VQF322B8_PRE: Quantize float to uint8_t/int8_t. Vector quantization.
- REQ8: Quantize int32_t to uint8_t/int8_t. Scalar quantization.
- VREQ8: Quantize int32_t to uint8_t/int8_t. Vector quantization.

## Prototype

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d, uint16_t src_stride, uint8_t unit_flag_mode,  uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| n_size | Input | Size of the source NZ matrix in the N direction.<br> - NZ2ND disabled: If the channel_split function is enabled, n_size must be a multiple of 8, with a value range of [1, 4095]. If the channel_split function is disabled, n_size must be a multiple of 16, with a value range of [1, 4095].<br>- NZ2ND enabled: The value range of n_size is [1, 4095]. |
| m_size | Input | Size of the source NZ matrix in the M direction.<br> - NZ2ND disabled: The value range is [1, 65535].<br> - NZ2ND enabled: The value range of m_size is [1, 8192]. |
| dst_stride_dst_d | Input | <br> - NZ2ND disabled: Offset between the start addresses of adjacent Z layouts in the destination NZ matrix. The value cannot be 0. Unit: element.<br> - NZ2ND/NZ2DN enabled: Number of elements in each row of the destination ND matrix. The value cannot be 0. Unit: element.|
| src_stride | Input | Offset between the start addresses of adjacent Z layouts in the source NZ matrix. Value range: [0, 65535]. Unit: C0_Size(16*sizeof(T), where T is the data type of src). |
| unit_flag_mode | Input | Related to the unit_flag parameter. Values are as follows:<br>0: Reserved value;<br>2: Enable unit_flag. After the hardware finishes executing the instruction, the register is not set;<br>3: Enable unit_flag. After the hardware finishes executing the instruction, unit_flag is disabled. |
| quant_pre |Input|Quantization parameter. For values, see [Function Description](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_2201.md#description).|
| relu_pre | Input | Enable relu. |
| channel_split | Input | Whether to enable the channel split function. The default value is false, which means the function is disabled. Channel split can be enabled only when both src and dst are float, and channel_split and NZ2ND cannot be enabled at the same time. |
| nz2nd_en | Input |Enable the nz2nd switch. false: Disabled; true: Enabled. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// total_length refers to the total length of the data to be copied.
constexpr uint64_t total_length = 128;
// dst and src correspond to the output address of the destination operand and the input address of the source operand respectively. dst is an externally input GM address of the int32_t type.
__cc__ int32_t src[total_length];
// The remaining input parameters are passed in with default values.
uint16_t n_size = 16;
uint16_t m_size = 16;
uint32_t dst_stride_dst_d = 0;
uint16_t src_stride = 8;
uint8_t unit_flag_mode = 0;
uint64_t quant_pre = QuantMode_t::NoQuant;
uint8_t relu_pre = 0;
bool channel_split = false;
bool nz2nd_en = false;
// Function call.
asc_copy_l0c2gm_sync(dst, src, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode, quant_pre, relu_pre, channel_split, nz2nd_en);
```
