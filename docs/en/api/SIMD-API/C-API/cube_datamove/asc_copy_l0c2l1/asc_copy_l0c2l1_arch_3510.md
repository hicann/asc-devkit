# asc_copy_l0c2l1

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:24:21.859Z pushedAt=2026-08-29T09:30:27.786Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

After matrix computation is complete, the result is quantized and then moved to the L1 Buffer. There are two quantization parameters: **quant_pre** and **quant_post**, corresponding to the pre-processing and post-processing stages, respectively.

The optional quantization modes of **quant_pre** are as follows:
- **NoQuant**: Quantization is disabled.
- **F322BF16**: Quantizes float to bfloat16_t. The quantization result does not support the INF_NAN mode.
- **F322F16**: Quantizes float to half. The quantization result supports the INF_NAN mode.
- **DEQF16**: Quantizes int32_t to half. The quantization result does not support the INF_NAN mode.
- **VDEQF16**: Quantizes int32_t to half. The quantization result does not support the INF_NAN mode.
- **QF322B8_PRE**: Quantizes float to uint8_t/int8_t. Scalar quantization.
- VQF322B8_PRE: Quantizes float to uint8_t/int8_t. Vector quantization.
- REQ8: Quantizes int32_t to uint8_t/int8_t. Scalar quantization.
- VREQ8: Quantizes int32_t to uint8_t/int8_t. Vector quantization.
- QF322FP8_PRE: Quantizes float to fp8_e4m3fn_t. Scalar quantization.
- VQF322FP8_PRE: Quantizes float to fp8_e4m3fn_t. Vector quantization.
- QF322HIF8_PRE: Quantizes float to hifloat8_t (Half to Away Round). Scalar quantization.
- VQF322HIF8_PRE: Quantizes float to hifloat8_t (Half to Away Round). Vector quantization.
- QF322HIF8_PRE_HYBRID: Quantizes float to hifloat8_t (Hybrid Round). Scalar quantization.
- VQF322HIF8_PRE_HYBRID: Quantizes float to hifloat8_t (Hybrid Round). Vector quantization.
- QS322BF16_PRE: Quantizes int32_t to bfloat16_t. Scalar quantization.
- VQS322BF16_PRE: Quantizes int32_t to bfloat16_t. Vector quantization.
- QF322F16_PRE: Quantizes float to half. Scalar quantization.
- VQF322F16_PRE: Quantizes float to half. Vector quantization.
- QF322BF16_PRE: Quantizes float to bfloat16_t. Scalar quantization.
- VQF322BF16_PRE: Quantizes float to bfloat16_t. Vector quantization.
- QF322F32_PRE: Quantizes float to float using scalar quantization. This mode cannot ensure that fewer than 1/10,000 of the values have an error greater than 1/10,000, but can ensure that fewer than 1/1,000 have an error greater than 1/1,000.
- VQF322F32_PRE: Quantizes float to float using Vector quantization. This mode cannot ensure that fewer than 1/10,000 of the values have an error greater than 1/10,000, but can ensure that fewer than 1/1,000 have an error greater than 1/1,000.

The optional quantization modes of quant_post are as follows:
- NoConv: Quantization is disabled.
- QS162B8_POST: Quantizes int16_t to uint8_t/int8_t. Scalar quantization.
- VQS162B8_POST: Quantizes int16_t to uint8_t/int8_t. Vector quantization.
- QF162B8_POST: Quantizes half to uint8_t/int8_t. Scalar quantization.
- VQF162B8_POST: Quantizes half to uint8_t/int8_t. Vector quantization.
- QS162S4_POST: Quantizes int16_t to int4_t. Scalar quantization.
- VQS162S4_POST: Quantizes int16_t to int4_t. Vector quantization.
- QF162S4_POST: Quantizes half to int4_t. Scalar quantization.
- VQF162S4_POST: Quantizes half to int4_t. Vector quantization.
- QS162S16_POST: Quantizes int16_t to int16_t. Scalar quantization.
- VQS162S16_POST: Quantizes int16_t to int16_t. Vector quantization.
- QF162S16_POST: Quantizes half to int16_t. Scalar quantization.
- **VQF162S16_POST**: Quantizes half to int16_t. Vector quantization.

## Prototype

```cpp
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_en, bool c0_pad_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ void *dst, __cc__ float *src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ void *dst, __cc__ int32_t *src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
```

## Parameters

| Parameter          | Input/Output | Description                                                                                                                               |
|:-----------------|:------|:---------------------------------------------------------------------------------------------------------------------------------|
| dst              | Output    | Start address of the destination operand (vector).                                                                                                                  |
| src              | Input    | Start address of the source operand (vector).                                                                                                                   |
| n_size           | Input    | Size of the source NZ matrix in the N direction.<br/>&bull; When the NZ2ND function is not enabled, value range: [1, 4095];<br/>&bull; When the NZ2ND function is enabled, value range: [1, 4095].                                        |
| m_size           | Input    | Size of the source NZ matrix in the M direction.<br/>&bull; When the NZ2ND function is not enabled, value range: [1, 65535];<br/>&bull; When the NZ2ND function is enabled, value range: [1, 8192].                                       |
| dst_stride       | Input    | &bull; When the NZ2ND function is not enabled, the start address offset between adjacent Z layouts in the destination NZ matrix. The value is not 0. Unit: element.<br>&bull; When the NZ2ND/NZ2DN function is enabled, the number of elements in each row of the destination ND matrix. The value is not 0. Unit: element.  |
| src_stride       | Input    | Start address offset between adjacent Z layouts in the source NZ matrix. Value range: [0, 65535]. Unit: C0_Size (16*sizeof(T), where T is the data type of src).      |
| l2_cache_ctl | Input | Configures the data management policy in the L2 Cache. The values are described as follows:  <br>&bull; 0: DISABLE mode, suitable for data that is accessed only once. <br>&bull; 1: NORMAL mode, suitable for data whose reuse pattern is unknown or not extreme. <br>&bull; 2: LAST mode, suitable for data that is accessed frequently and repeatedly. <br>&bull; 4: PERSISTENT mode, suitable for data that needs to reside in the cache for a long time. |
| clip_relu_pre    | Input    | Enables clip_relu in the pre-processing stage. It must be used together with normal relu (the normalized relu function) and requires the quantization function to be enabled.                                                                        |
| unit_flag_mode   | Input    | Related to the unit_flag parameter. The values are as follows:<br/>&bull; 0: Retain the value;<br/>&bull; 2: Enable unit_flag. After the hardware finishes executing the instruction, the register is not set;<br/>&bull; 3: Enable unit_flag. After the hardware finishes executing the instruction, unit_flag is disabled. |
| quant_pre        | Input    | Quantization parameter in the pre-processing stage. For the values, see [Description](./asc_copy_l0c2l1_arch_3510.md#description).                                                                        |
| relu_pre         | Input    | Enables relu in the pre-processing stage.                                                                                                                     |
| channel_split    | Input    | Whether to enable the channel split function. The default value is false, which means the function is not enabled. Channel split can be enabled only when both src and dst are float, and channel_split and NZ2ND cannot be enabled at the same time.                                               |
| nz2nd_en         | Input    | Enables the NZ2ND switch.<br/>&bull;false: Not enabled;<br/>&bull;true: Enabled.                                                                               |
| quant_post       | Input    | Quantization parameter in the post-processing stage. For the values, see [Description](./asc_copy_l0c2l1_arch_3510.md#description).                                                                        |
| relu_post        | Input    | Enables relu in the post-processing stage.                                                                                                                     |
| clip_relu_post   | Input    | Enables clip_relu in the post-processing stage. It must be used together with normal relu and requires the quantization function to be enabled.                                                                                   |
| eltwise_op       | Input    | Defines the destination operand address and channel stride when data is moved from l0c to l1.                                                                                                     |
| eltwise_antq_en | Input    | Enables the dequantization operation of elements bit by bit.                                                                                                                    |
| c0_pad_en        | Input    | Enables configuring padding bits for C0, where C0 is the target stride count of the channel loop.                                                                                                        |
| broadcast_en        | Input    | Whether to enable the broadcast capability.<br/>&bull; false: Not enabled;<br/>&bull; true: Enabled. Data is broadcast along the M-axis direction during data movement.                                                                                                         |
| nz2dn_en        | Input    |  Enables the NZ2DN switch.<br/>&bull; false: Not enabled;<br/>&bull; true: Enabled.                                                                                       |

For details about the vector data register, see [reg Data Definition](../../reg/reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.

## Example

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
uint8_t l2_cache_ctl = 0;
uint8_t clip_relu_pre = 0;
uint8_t unit_flag_mode = 0;
uint64_t quant_pre = QuantMode_t::NoQuant;
uint8_t relu_pre = 0;
bool channel_split = false;
bool nz2nd_en = false;
uint64_t quant_post = QuantMode_post::NoConv;
uint8_t relu_post = 0;
bool clip_relu_post = false;
uint8_t eltwise_op = 0;
uint8_t eltwise_antq_en = 0;
bool c0_pad_en = false;
bool broadcast_en = false;
bool nz2dn_en = false;
asc_copy_l0c2l1(dst, src, n_size, m_size, dst_stride, src_stride, l2_cache_ctl, clip_relu_pre, unit_flag_mode, quant_pre, relu_pre, channel_split, nz2nd_en, quant_post, relu_post, clip_relu_post, eltwise_op, eltwise_antq_en, c0_pad_en, broadcast_en, nz2dn_en);
```
