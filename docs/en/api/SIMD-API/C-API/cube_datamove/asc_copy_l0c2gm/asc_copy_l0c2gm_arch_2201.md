# asc_copy_l0c2gm

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:12:48.945Z pushedAt=2026-08-29T07:20:57.161Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

After matrix computation is complete, the result is quantized and then copied to GM. There are nine quantization modes in total:
- **NoQuant**: Quantization is disabled.
- **F322F16**: Quantizes float to half. The quantization result does not support the INF_NAN mode.
- **F322BF16**: Quantizes float to bfloat16_t. The quantization result does not support the INF_NAN mode.
- **DEQF16**: Quantizes int32_t to half. The quantization result does not support the INF_NAN mode.
- **VDEQF16**: Quantizes int32_t to half. The quantization result does not support the INF_NAN mode.
- **QF322B8_PRE**: Quantizes float to uint8_t/int8_t. Scalar quantization.
- VQF322B8_PRE: Quantizes float to uint8_t/int8_t. Vector quantization.
- REQ8: Quantizes int32_t to uint8_t/int8_t. Scalar quantization.
- VREQ8: Quantizes int32_t to uint8_t/int8_t. Vector quantization.

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

- Synchronous transfer

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

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| n_size | Input | Size of the source NZ matrix in the N direction.<br> - When the NZ2ND function is disabled: If the channel_split function is enabled, n_size must be a multiple of 8, with a value range of [1, 4095]. If the channel_split function is disabled, n_size must be a multiple of 16, with a value range of [1, 4095].<br>- When the NZ2ND function is enabled: The value range of n_size is [1, 4095]. |
| m_size | Input | Size of the source NZ matrix in the M direction.<br> - When the NZ2ND function is disabled: The value range is [1, 65535].<br> - When the NZ2ND function is enabled: The value range of m_size is [1, 8192]. |
| dst_stride_dst_d | Input | <br> - When the NZ2ND function is disabled, the start address offset between adjacent Z layouts in the destination NZ matrix. The value cannot be 0. Unit: element.<br> - When the NZ2ND/NZ2DN function is enabled, the number of elements in each row of the destination ND matrix. The value cannot be 0. Unit: element.|
| src_stride | Input | Start address offset between adjacent Z layouts in the source NZ matrix. Value range: [0, 65535]. Unit: C0_Size (16*sizeof(T), where T is the data type of src). |
| unit_flag_mode | Input | Related to the unit_flag parameter. The values are as follows:<br>0: Reserved value.<br>2: Enables unit_flag. After the hardware finishes executing the instruction, the register is not set.<br>3: Enables unit_flag. After the hardware finishes executing the instruction, unit_flag is disabled. |
| quant_pre |Input|Quantization parameter. For the values, see [Description](./asc_copy_l0c2gm_arch_2201.md#description).|
| relu_pre | Input | Enables relu. |
| channel_split | Input | Whether to enable channel splitting. The default value is **false**, which disables this function. Channel splitting can be enabled only when both src and dst are float, and channel_split and NZ2ND cannot be enabled at the same time. |
| nz2nd_en | Input |Enables the nz2nd switch. **false**: Disabled; **true**: Enabled. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
// total_length indicates the total length of data to be copied.
constexpr uint64_t total_length = 128;
// dst and src correspond to the output address of the destination operand and the input address of the source operand, respectively. dst is an externally input int32_t GM address.
__cc__ int32_t src[total_length];
// The remaining input parameters are passed with default values.
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
