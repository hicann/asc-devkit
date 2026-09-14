# asc_copy_l0c2ub

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:40:59.714Z pushedAt=2026-08-27T12:10:43.862Z -->

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

Header file path: `"c_api/cube_datamove/cube_datamove.h"`.

After matrix computation is complete, the result is quantized and then moved to the Unified Buffer. There are two quantization parameters: **quant_pre** and **quant_post**, corresponding to the pre-processing and post-processing stages, respectively.

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

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ bfloat16_t *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ half *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ fp8_e4m3fn_t *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ fp8_e5m2_t *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ hifloat8_t *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ int8_t *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ uint8_t *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ float *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ bfloat16_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ half *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ fp8_e4m3fn_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ fp8_e5m2_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ hifloat8_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ int8_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ uint8_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ int32_t *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ void *dst_addr, __cc__ float *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub(__ubuf__ void *dst_addr, __cc__ int32_t *src_addr, uint16_t n_size, uint16_t m_size,
                                uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                                uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                                uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                                bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ bfloat16_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ half* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ fp8_e4m3fn_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ fp8_e5m2_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ hifloat8_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ int8_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ uint8_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ float* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ half* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ fp8_e4m3fn_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ fp8_e5m2_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ int8_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ uint8_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ int32_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ void* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2ub_sync(__ubuf__ void* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
                            uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
                            uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
                            uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en,
                            bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    ```

## Parameters

| Parameter | Input/Output | Description |
|:-----------------|:------|:---------------------------------------------------------------------------------------------------------------------------------|
| dst_addr | Output | Start location of the destination operand (vector). |
| src_addr | Input | Start location of the source operand (vector). |
| n_size | Input | Size of the source NZ matrix in the N direction.<br/>&bull;When the NZ2ND function is disabled, value range: [1, 4095].<br/>&bull;When the NZ2ND function is enabled, value range: [1, 4095]. |
| m_size | Input | Size of the source NZ matrix in the M direction.<br/>&bull;When the NZ2ND function is disabled, value range: [1, 65535].<br/>&bull;When the NZ2ND function is enabled, value range: [1, 8192]. |
| loop_dst_stride | Input | <br> - When the NZ2ND function is disabled, the start address offset between adjacent Z layouts in the destination NZ matrix. The value must not be 0. Unit: element.<br> - When the NZ2ND/NZ2DN function is enabled, the number of elements in each row of the destination ND matrix. The value must not be 0. Unit: element. |
| loop_src_stride | Input | Start address offset between adjacent Z layouts in the source NZ matrix. Value range: [0, 65535]. Unit: C0_Size (16*sizeof(T), where T is the data type of src_addr). |
| dual_dst_ctl | Input | Control parameter for dual destinations. |
| sub_blockid | Input | Sub-block ID. |
| clip_relu_pre | Input | Enables **clip_relu** in the pre-processing stage. It must be used together with normal relu (the normalized relu function) and requires the quantization function to be enabled. |
| unit_flag_ctl | Input | Related to the **unit_flag** parameter. Values are as follows:<br/>&bull;0: Reserved value.<br/>&bull;2: Enables **unit_flag**. After the hardware finishes executing the instruction, the register is not set.<br/>&bull;3: Enables **unit_flag**. After the hardware finishes executing the instruction, **unit_flag** is disabled. |
| quant_pre | Input | Quantization parameter in the pre-processing stage. For values, see [Description](#description). |
| relu_pre | Input | Enables **relu** in the pre-processing stage. |
| split_en | Input | Whether to enable the channel splitting function. The default value is **false**, which disables this function. Channel splitting can be enabled only when both **src_addr** and **dst_addr** are of the float type, and **split_en** and the NZ2ND function cannot be enabled at the same time. |
| nz2nd_en | Input | Enables the NZ2ND switch.<br/>&bull;**false**: Disabled.<br/>&bull;**true**: Enabled. |
| quant_post | Input | Quantization parameter in the post-processing stage. For values, see [Description](#description). |
| relu_post | Input | Enables **relu** in the post-processing stage. |
| clip_relu_post | Input | Enables **clip_relu** in the post-processing stage. It must be used together with normal relu and requires the quantization function to be enabled. |
| eltwise_op | Input | Defines the destination operand address and channel stride when data is moved from L0C to UB. |
| eltwise_antq_en | Input | Enables element-wise dequantization bit by bit. |
| c0_pad_en | Input | Enables configuring padding bits for C0, where C0 is the target stride of the channel loop. |
| broadcast_en | Input | Whether to enable the broadcast capability.<br/>&bull;**false**: Disabled.<br/>&bull;**true**: Enabled. Data is broadcast along the M-axis direction during data movement. |
| nz2dn_en | Input | Enables the NZ2DN switch.<br/>&bull;**false**: Disabled.<br/>&bull;**true**: Enabled. |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- The start address of **src_addr** must be aligned to the number of bytes occupied by the corresponding data type.
- The start address of **dst_addr** must be 32-byte aligned.

## Example

```cpp
__ubuf__ bfloat16_t dst[256];
__cc__ float src[256];
uint16_t n_size = 1;
uint16_t m_size = 1;
uint32_t loop_dst_stride = 0;
uint16_t loop_src_stride = 0;
uint8_t dual_dst_ctl = 5;
bool sub_blockid = true;
uint8_t clip_relu_pre = 0;
uint8_t unit_flag_ctl = 0;
uint64_t quant_pre = QuantMode_t::F322BF16;
uint8_t relu_pre = 0;
bool split_en = false;
bool nz2nd_en = true;
uint64_t quant_post = QuantMode_post::NoConv;
uint8_t relu_post = 0;
bool clip_relu_post = false;
uint8_t eltwise_op = 0;
bool eltwise_antq_en = true;
bool c0_pad_en = true;
bool broadcast_en = false;
bool nz2dn_en = false;
asc_copy_l0c2ub(dst, src, n_size, m_size, loop_dst_stride, loop_src_stride, dual_dst_ctl, sub_blockid, clip_relu_pre,
        unit_flag_ctl, quant_pre, relu_pre, split_en, nz2nd_en, quant_post, relu_post, clip_relu_post,
        eltwise_op, eltwise_antq_en, c0_pad_en, broadcast_en, nz2dn_en);
```
