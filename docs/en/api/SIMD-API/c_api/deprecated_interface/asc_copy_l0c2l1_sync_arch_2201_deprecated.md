---
okftype: reference.api
description: "Deprecated interface page: Under the 2201 architecture, asc_copy_l0c2l1_sync processes the result according to the quantization mode after matrix computation and synchronously moves it to the L1 Buffer. It supports 9 quantization modes and int4b_t quantization combinations. It is recommended to use the asynchronous asc_copy_l0c2l1 together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [conversion, store]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding asynchronous interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "The alternative solution handles synchronous waiting"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 2201 synchronous interface with ASC_DEPRECATED(9.2.0). The alternative path is the non-synchronous asc_copy_l0c2l1 plus asc_sync. The interface has not been removed."
---
# asc_copy_l0c2l1_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:55:34.715Z pushedAt=2026-11-18T03:12:16.413Z nl=2 -->

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

**The `asc_copy_l0c2l1_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_2201.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

After matrix computation is complete, the result is quantized, and then the processed result is moved to the L1 Buffer. There are 9 quantization modes in total:
- NoQuant: Quantization is disabled.
- DEQF16: Quantize int32_t to half. The quantization result does not support the INF_NAN mode.
- F322BF16: Quantize float to bfloat16_t. The quantization result does not support the INF_NAN mode.
- F322F16: Quantize float to half. The quantization result does not support the INF_NAN mode.
- QF322B8_PRE: Quantize float to uint8_t/int8_t. Scalar quantization.
- REQ8: Quantize int32_t to uint8_t/int8_t. Scalar quantization.
- VDEQF16: Quantize int32_t to half. The quantization result does not support the INF_NAN mode.
- VQF322B8_PRE: Quantize float to uint8_t/int8_t. Vector quantization.
- VREQ8: Quantize int32_t to uint8_t/int8_t. Vector quantization.

In addition, this transfer instruction also supports quantization of the int4b_t type. The related quantization types are divided into two types: quantize int32_t to int4b_t; quantize float to int4b_t.

## Prototype

- Synchronous transfer

    ```cpp
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ half* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ void* dst, __cc__ float* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    __aicore__ inline void asc_copy_l0c2l1_sync(__cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size,uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
    ```

## Parameters

**Table 1** Parameter description

| Parameter        | Input/Output | Description                                                                                                                                 |
|:---------------------|:------|:---------------------------------------------------------------------------------------------------------------------------------|
| dst                  | Output    | Start address of the destination operand (vector).                                                                                                                  |
| src                  | Input    | Start address of the source operand.                                                                                                                       |
| n_size               | Input    | Size of the source NZ matrix in the N direction.<br/>&bull; When the NZ2ND function is disabled, value range: [1, 4095];<br/>&bull; When the NZ2ND function is enabled, value range: [1, 4095].                                        |
| m_size               | Input    | Size of the source NZ matrix in the M direction.<br/>&bull; When the NZ2ND function is disabled, value range: [1, 65535];<br/>&bull; When the NZ2ND function is enabled, value range: [1, 8192].                                       |
| dst_stride           | Input    | &bull; When the NZ2ND function is disabled, the start address offset of adjacent Z layouts in the destination NZ matrix. The value is not 0. Unit: element.<br/>&bull; When the NZ2ND/NZ2DN function is enabled, the number of elements in each row of the destination ND matrix. The value is not 0. Unit: element.   |
| src_stride           | Input    | Start address offset of adjacent Z layouts in the source NZ matrix. Value range: [0, 65535]. Unit: C0_Size (16*sizeof(T), where T is the data type of src).  |
| unit_flag_mode       | Input    | Related to the unit_flag parameter. The values are as follows:<br/>&bull; 0: reserved value;<br/>&bull; 2: enable unit_flag. After the hardware finishes executing the instruction, the register is not set;<br/>&bull; 3: enable unit_flag. After the hardware finishes executing the instruction, unit_flag is disabled. |
| quant_pre            | Input    | Quantization parameter. For the values, see [Function Description](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_2201.md#description).                                                                             |
| relu_pre             | Input    | Enable relu.                                                                                                                          |
| enable_channel_split | Input    | Whether to enable the channel split function. The default value is false, which means the function is disabled. Channel split can be enabled only when both src and dst are float, and channel_split and NZ2ND cannot be enabled at the same time.                                               |
| enable_nz2nd         | Input    | Enable the NZ2ND switch.<br/>&bull; false: disabled;<br/>&bull; true: enabled.                                                                               |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If multiple asc_copy_l0c2l1 instructions need to be executed and the destination addresses of the asc_copy_l0c2l1 instructions overlap, a synchronization instruction must be inserted to ensure serialization of multiple asc_copy_l0c2l1 instructions and prevent abnormal data.

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
asc_copy_l0c2l1_sync(dst, src, n_size, m_size, dst_stride, src_stride, unit_flag_mode, quant_pre, relu_pre, enable_channel_split, enable_nz2nd);
```
