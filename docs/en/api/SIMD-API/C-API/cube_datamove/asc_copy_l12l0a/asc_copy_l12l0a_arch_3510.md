# asc_copy_l12l0a

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:22:36.282Z pushedAt=2026-08-29T07:48:42.733Z -->

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

Copies a 512B matrix stored in the **L1 Buffer** to the **L0A Buffer**. Supports 2D data copy, 2D transposed data copy, and 3D data copy.

## Prototype

- Regular 2D data copy

     ```cpp

    __aicore__ inline void asc_copy_l12l0a(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a(__ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)

    ```

- Synchronous regular 2D data copy

     ```cpp

    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)

    ```

- Transposed 2D data copy

     ```cpp

    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose(__ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)

    ```

- Synchronous transposed 2D data copy

     ```cpp

    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    __aicore__ inline void asc_copy_l12l0a_transpose_sync(__ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
    
    ```

- High-dimensional split 3D data copy

    ```cpp

    __aicore__ inline void asc_copy_l12l0a(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a(__ca__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)

    ```

- Synchronous high-dimensional split 3D data copy

```cpp

    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0a_sync(__ca__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)

```

## Parameters

Table 1 2D data copy
| Parameter              | Input/Output | Description |
|:-----------------| :--- |:-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst              | Output | Destination L0A Buffer address. |
| src              | Input | Source L1 Buffer address. |
| m_start_position | Input | Taking an M*K matrix as an example, the start position of the source matrix along the M-axis, in units of 16 elements. |
| k_start_position | Input | Taking an M*K matrix as an example, the start position of the source matrix along the K-axis, in units of 32B. |
| m_step           | Input | Taking an M*K matrix as an example, the move length of the source matrix along the M-axis, in units of 16 elements. Value range: [0, 255]. When performing a transposed move, the following additional constraints must also be met:<br>When the data bit width is 4, m_step must be a multiple of 4;<br>When the data bit width is 8, m_step must be a multiple of 2;<br>When the data bit width is 16, m_step must be a multiple of 1;<br>When the data bit width is 32, m_step has no additional constraint. |
| k_step           | Input | Taking an M*K matrix as an example, the move length of the source matrix along the K-axis, in units of 32B. Value range: [0, 255]. When performing a transposed move, the following additional constraints must also be met:<br>When the data bit width is 4, 8, or 16, k_step has no additional constraint;<br>When the data bit width is 32, k_step must be a multiple of 2. |
| src_stride       | Input | Taking an M*K matrix as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the source matrix, in units of 512B. |
| dst_stride       | Input | Taking an M*K matrix as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the destination matrix, in units of 512B. |

Table 2 3D data copy
| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination L0A Buffer address. |
| src | Input | Source L1 Buffer address. |
| k_extension | Input | Transfer length of this instruction in the width dimension of the destination operand.<br>If the rightmost fractal is not covered, it must be a multiple of 16 for the half type and a multiple of 32 for the int8_t/uint8_t type;<br>if the rightmost fractal is covered, there is no multiple requirement. Value range: [1, 65535]. |
| m_extension | Input | Transfer length of this instruction in the height dimension of the destination operand.<br>If the bottommost fractal is not covered, it must be a multiple of 16 for the half/int8_t/uint8_t type;<br>if the bottommost fractal is covered, there is no multiple requirement. Value range: [1, 65535]. |
| k_start_pt | Input | Starting point of this instruction in the width dimension of the destination operand. It must be a multiple of 16 for the half type and a multiple of 32 for the int8_t/uint8_t type. Value range: [0, 65535]. |
| m_start_pt | Input | Starting point of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for half/int8_t/uint8_t; if the bottommost fractal is covered, there is no multiple requirement. Value range: [0, 65535]. |
| stride_w | Input | Stride by which the convolution kernel slides in the width dimension of the source operand. Value range: [1, 63]. |
| stride_h | Input | Stride by which the convolution kernel slides in the height dimension of the source operand. Value range: [1, 63]. |
| filter_w | Input | Convolution kernel width. Value range: [1, 255]. |
| filter_h | Input | Convolution kernel height. Value range: [1, 255]. |
| dilation_filter_w | Input | Dilation factor of the convolution kernel width. Value range: [1, 255]. |
| dilation_filter_h | Input | Dilation factor of the convolution kernel height. Value range: [1, 255]. |
| filter_size_w | Input | Whether to increase the convolution kernel width by 256 elements on the basis of filter_w. true means increase; false means no increase. |
| filter_size_h | Input | Whether to increase the convolution kernel height by 256 elements on the basis of filter_h. true means increase; false means no increase. |
| transpose | Input | Whether to enable the transpose function to transpose the entire target matrix. It is valid only when the source operand is of the half type. true means enable; false means disable. |
| f_matrix_ctrl | Input | Indicates whether the asc_copy_l12l0a instruction obtains the FeatureMap attribute description from the left matrix or the right matrix. Currently only false is supported. |
| channel_size | Input | Number of channels of the source operand. Value range: [1, 63]. <br>For uint32_t/int32_t/float, channelSize can be 4, N * 8, or N * 8 + 4;<br>for half/bfloat16, channelSize can be 4, 8, N * 16, N * 16 + 4, or N * 16 + 8;<br>for int8_t/uint8_t, channelSize can be 4, 8, 16, 32 * N, N * 32 + 4, N * 32 + 8, or N * 32 + 16;<br>for int4b_t, ChannelSize can be 8, 16, 32, N * 64, N * 64 + 8, N * 64 + 16, or N * 64 + 32. N is a positive integer. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start address of dst must be 512-byte aligned, and the start address of src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## 3D Data Format Description

The input feature map and filter are required to be in the NC1HWC0 format, where C0 is the lowest dimension and is fixed at 16 (32 for the u8/s8 type), and C1 = C/C0.

To simplify the scenario, the following assumes that the input feature map has 4 channels, that is, Ci = 4. The shape of the input feature maps in A1 is (Hi, Wi, Ci), and after load3dv1 processing, the data shape in A2 is (Wo\*Ho, Hk\*Wk\*Ci), where Wo and Ho are the output shape after convolution, and Hk and Wk are the shape of the filter.

Intuitively, the img2col process is the one in which the filter sweeps over the feature map and expands the corresponding feature map data into each row of the output data. The filter first slides Wo steps in the W direction, then moves one step in the H direction and repeats the above process, finally outputting Wo * Ho rows of data. In the following figure, the red and yellow data represent the first row and the second row, respectively. The numbers indicate the association among the original input data, the filter, and the output data. It can be seen that load3dv1 first moves the 4 numbers corresponding to 00 in the Ci dimension of the input data, and then moves the 4 numbers corresponding to 01. Finally, the size of this row is Hk\*Wk\*Ci, that is, 3\*3\*4 = 36 numbers.

- The corresponding feature map format is shown in the following figure:

![ ](../../figures/load3d_01.png)

- The corresponding filter format is shown in the following figure:

![ ](../../figures/load3d_02.png)

Here, n is the number of filters. It can be seen that the dimension layout is (Hk, Wk, Ci, n). However, note that the format in the following figure also needs to be converted according to the format of the B matrix in Mmad.

In actual operation, due to limitations in storage space or computing capability, the entire convolution computation is usually divided into blocks, and only a small block of data is moved and computed at a time.

![ ](../../figures/load3d_03.png)

For the A2 feature map, there are two schemes: horizontal blocking and vertical blocking, corresponding to **repeatMode** values 0 and 1, respectively.

Note: The fractal matrix size in the following figures is 4x4, but the actual size should be 16x16 (16x32 for the u8/s8 types).

When **repeatMode** = 0, each repeat changes the position of the data point read in the filter window, and then jumps to the next C0 position.

![ ](../../figures/load3d_04.png)

When **repeatMode** = 1, the position of the data read in the filter window remains unchanged, and each repeat advances by C0 elements in the feature map.

![ ](../../figures/load3d_05.png)

## Example

```cpp
__ca__ bfloat16_t dst[256];
__cbuf__ bfloat16_t src[256];
uint16_t m_start_position = 8;
uint16_t k_start_position = 2;
uint8_t m_step = 4;
uint8_t k_step = 4;
int16_t src_stride = 1;
uint16_t dst_stride = 1;
asc_copy_l12l0a(dst, src, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);
```
