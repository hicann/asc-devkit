# asc_copy_l0c2gm

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

矩阵计算完成后，对结果进行量化处理，之后将处理结果搬运到GM中。量化参数共有2个：quant_pre和quant_post，分别对应预处理和后处理阶段。

quant_pre可选量化模式，分别为：
- NoQuant：不开启量化功能。
- F322BF16：float量化成bfloat16_t。量化结果不支持INF_NAN模式。
- F322F16：float量化成half。量化结果支持INF_NAN模式。
- DEQF16：int32_t量化成half。量化结果不支持INF_NAN模式。
- VDEQF16：int32_t量化成half。量化结果不支持INF_NAN模式。
- QF322B8_PRE：float量化成uint8_t/int8_t。scalar量化。
- VQF322B8_PRE：float量化成uint8_t/int8_t。矢量量化。
- REQ8：int32_t量化成uint8_t/int8_t。scalar量化。
- VREQ8：int32_t量化成uint8_t/int8_t。矢量量化。
- QF322FP8_PRE：float量化成fp8_e4m3fn_t，scalar量化。
- VQF322FP8_PRE：float量化成fp8_e4m3fn_t，矢量量化。
- QF322HIF8_PRE：float量化成hifloat8_t（Half to Away Round），scalar量化。
- VQF322HIF8_PRE：float量化成hifloat8_t（Half to Away Round），矢量量化。
- QF322HIF8_PRE_HYBRID：float量化成hifloat8_t（Hybrid Round），scalar量化。
- VQF322HIF8_PRE_HYBRID：float量化成hifloat8_t（Hybrid Round），矢量量化。
- QS322BF16_PRE：int32_t量化成bfloat16_t，scalar量化。
- VQS322BF16_PRE：int32_t量化成bfloat16_t，矢量量化。
- QF322F16_PRE：float量化成half，scalar量化。
- VQF322F16_PRE：float量化成half，矢量量化。
- QF322BF16_PRE：float量化成bfloat16_t，scalar量化。
- VQF322BF16_PRE：float量化成bfloat16_t，矢量量化。
- QF322F32_PRE：float量化成float，scalar量化。该量化模式精度无法达到双万分之一，可以达到双千分之一。
- VQF322F32_PRE：float量化成float，矢量量化。该量化模式精度无法达到双万分之一，可以达到双千分之一。

quant_post可选量化模式分别为：
- NoConv：不开启量化功能。
- QS162B8_POST：int16_t量化成uint8_t/int8_t，scalar量化。
- VQS162B8_POST：int16_t量化成uint8_t/int8_t，矢量量化。
- QF162B8_POST：half量化成uint8_t/int8_t，scalar量化。
- VQF162B8_POST：half量化成uint8_t/int8_t，矢量量化。
- QS162S4_POST：int16_t量化成int4_t，scalar量化。
- VQS162S4_POST：int16_t量化成int4_t，矢量量化。
- QF162S4_POST：half量化成int4_t类型，scalar量化。
- VQF162S4_POST：half量化成int4_t类型，矢量量化。
- QS162S16_POST：int16_t量化成int16_t，scalar量化。
- VQS162S16_POST：int16_t量化成int16_t，矢量量化。
- QF162S16_POST：half量化成int16_t，scalar量化。
- VQF162S16_POST：half量化成int16_t，矢量量化。

## 函数原型

- 常规搬运

    ```cpp
    __aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ half *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ fp8_e4m3fn_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ fp8_e5m2_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ hifloat8_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int8_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ uint8_t *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ float *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ bfloat16_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ half *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ fp8_e4m3fn_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ fp8_e5m2_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ hifloat8_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int8_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ uint8_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ int32_t *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ void *dst_addr, __cc__ float *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm(__gm__ void *dst_addr, __cc__ int32_t *src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ bfloat16_t* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ half* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ fp8_e4m3fn_t* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ fp8_e5m2_t* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ hifloat8_t* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int8_t* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ uint8_t* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ float* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ half* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ fp8_e4m3fn_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ fp8_e5m2_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int8_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ uint8_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ int32_t* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ void* dst_addr, __cc__ float* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    __aicore__ inline void asc_copy_l0c2gm_sync(__gm__ void* dst_addr, __cc__ int32_t* src_addr,
                        uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl,
                        uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en,
                        bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
    ```

## 参数说明

**表1**  参数说明

| 参数名              | 输入/输出 | 描述                                                                                                                               |
|:-----------------|:------|:---------------------------------------------------------------------------------------------------------------------------------|
| dst_addr              | 输出    | 目的操作数（矢量）的起始地址。                                                                                                                  |
| src_addr              | 输入    | 源操作数（矢量）的起始地址。                                                                                                                   |
| n_size           | 输入    | 源NZ矩阵在N方向上的大小。<br/>&bull;不开启NZ2ND功能，取值范围：[1, 4095]；<br/>&bull;开启NZ2ND功能，取值范围：[1, 4095]。                                        |
| m_size           | 输入    | 源NZ矩阵在M方向上的大小。<br/>&bull;不开启NZ2ND功能，取值范围：[1, 65535]；<br/>&bull;开启NZ2ND功能，取值范围：[1, 8192]。                                       |
| loop_dst_stride       | 输入    | <br> - 不开启NZ2ND功能,目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位：element。<br> - 开启NZ2ND/NZ2DN功能,目的ND矩阵每一行中的元素个数，取值不为0 ，单位：element。  |
| loop_src_stride       | 输入    | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围：[0, 65535]，单位：C0_Size(16*sizeof(T), T为src_addr的数据类型)。 |
| l2_cache_ctl | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表2](#table2)。 |
| clip_relu_pre    | 输入    | 预处理阶段开启clip_relu，需搭配normal relu（归一化的relu函数）一起使用且需要开启量化功能。                                                                        |
| unit_flag_ctl   | 输入    | 与unit_flag参数相关，取值如下：<br/>&bull;0保留值；<br/>&bull;2 开启unit_flag，硬件执行完指令之后，不会设置寄存器；<br/>&bull;3 开启unit_flag，硬件执行完指令后，会将unit_flag关闭。 |
| quant_pre        | 输入    | 预处理阶段量化参数。取值见[功能说明](#功能说明)。                                                                        |
| relu_pre         | 输入    | 预处理阶段开启relu。                                                                                                                     |
| split_en    | 输入    | 是否开启通道拆分的功能，默认false，不开启该功能。仅在src_addr和dst_addr都为float时才能开启通道拆分，且不能同时开启split_en和NZ2ND功能。                                               |
| nz2nd_en         | 输入    | 开启NZ2ND开关。<br/>&bull;false：不开启；<br/>&bull;true：开启。                                                                               |
| quant_post       | 输入    | 后处理阶段量化参数。取值见[功能说明](#功能说明)。                                                                        |
| relu_post        | 输入    | 后处理阶段开启relu。                                                                                                                     |
| clip_relu_post   | 输入    | 后处理阶段开启clip_relu，需搭配normal relu一起使用，且需要开启量化功能。                                                                                   |
| eltwise_op       | 输入    | 定义数据从l0c搬运至gm时的目的操作数地址和通道步长。                                                                                                     |
| eltwise_antq_en | 输入    | 按位开启元素的反量化操作。                                                                                                                    |
| c0_pad_en        | 输入    | 开启为C0配置填充位，C0是通道循环的目标步长。                                                                                                     |
| broadcast_en        | 输入    | 是否开启广播能力。<br/>&bull;false：不开启；<br/>&bull;true：开启，在数据搬运时沿M轴方向进行数据广播。                                                                                                         |
| nz2dn_en        | 输入    |  开启NZ2DN开关。<br/>&bull;false：不开启；<br/>&bull;true：开启。                                                                                       |

**表2**  l2_cache_ctl取值说明 <a id="table2"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0    | NORMAL模式 | 启用L2 Cache，采用write back策略写入L2 Cache，并且将分配的Cache Line标记为高替换优先级。|
| 1    | LAST模式 | &bull; 启用L2 Cache，采用write back策略写入L2 Cache，并且将分配的Cache Line标记为低替换优先级。<br>&bull; **LAST模式功能，暂不支持。**|
| 2    | PERSISTENT模式 | &bull; 启用L2 Cache，采用write back策略写入L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GlobalTensor的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。<br>&bull; **目前该驻留模式功能尚在开发中，暂不支持，计划于Ascend 950PR/Ascend 950DT上提供支持。**|
| 4    | DISABLE模式 | 不启用L2 Cache。如果写入地址在L2 Cache中已经被分配了Cache Line，则将本次写入的数据覆盖Cache Line原有数据后将Cache Line中最新数据写回到GM，并且将该Cache Line标记为invalid。如果写入地址在L2 Cache中没有被分配Cache Line，则直接写回到GM。|

## 返回值说明

无

## 流水类型

PIPE_FIX

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../通用说明和约束.md#存储单元说明)。

## 调用示例

```cpp
// dst是外部输入的float类型的GM地址。
__cc__ float src[256];
uint16_t n_size = 1;
uint16_t m_size = 1;
uint32_t loop_dst_stride = 0;
uint16_t loop_src_stride = 0;
uint8_t l2_cache_ctl = 0;
uint8_t clip_relu_pre = 0;
uint8_t unit_flag_ctl = 0;
uint64_t quant_pre = QuantMode_t::QF322F32_PRE;
uint8_t relu_pre = 0;
bool split_en = true;
bool nz2nd_en = false;
uint64_t quant_post = QuantMode_post::NoConv;
uint8_t relu_post = 0;
bool clip_relu_post = false;
uint8_t eltwise_op = 0;
bool eltwise_antq_en = true;
bool c0_pad_en = true;
bool broadcast_en = false;
bool nz2dn_en = false;
asc_copy_l0c2gm(dst, src, n_size, m_size, loop_dst_stride, loop_src_stride, l2_cache_ctl, clip_relu_pre,
    unit_flag_ctl, quant_pre, relu_pre, split_en, nz2nd_en, quant_post, relu_post, clip_relu_post,
    eltwise_op, eltwise_antq_en, c0_pad_en, broadcast_en, nz2dn_en);
```
