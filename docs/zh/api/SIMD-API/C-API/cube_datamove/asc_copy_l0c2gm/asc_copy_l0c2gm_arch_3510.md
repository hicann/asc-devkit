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

该接口用于将矩阵计算的结果从L0C Buffer搬运至Global Memory（GM）中，并且在搬运过程中支持与其它接口组合使用，配置多种随路能力。

下图展示了随路量化、随路ReLU、随路格式转换、随路通道拆分以及随路通道合并的有效组合、中间数据类型和数据路径。下图中的F32-\>F16与F32-\>BF16为Cast，其余为随路scalar/tensor量化模式。

**图1** asc_copy_l0c2gm随路功能组合

![](../../../../figures/C_API_L0C2GM_Function_Combination_950.png)

quant_pre_mode量化模式参数支持的枚举值如下：

- NoQuant：不开启量化功能
- DEQF16：int32_t量化成half， scalar量化
- VDEQF16：int32_t量化成half，tensor量化
- REQ4：int32_t量化成int4b_t，scalar量化
- VREQ4：int32_t量化成int4b_t，tensor量化
- REQ8：int32_t量化成int8_t/uint8_t，scalar量化
- VREQ8：int32_t量化成int8_t/uint8_t，tensor量化
- QS322BF16_PRE：int32_t量化成bfloat16_t，scalar量化
- VQS322BF16_PRE：int32_t量化成bfloat16_t，tensor量化
- QF322F16_PRE：float量化成half，scalar量化
- VQF322F16_PRE：float量化成half，tensor量化
- QF322BF16_PRE：float量化成bfloat16_t，scalar量化
- VQF322BF16_PRE：float量化成bfloat16_t，tensor量化
- F322F16：float cast成half，cast mode为CAST_RINT模式
- F322BF16：float cast成bfloat16_t，cast mode为CAST_RINT模式
- QF322S4_PRE：float量化成int4b_t，scalar量化
- VQF322S4_PRE：float量化成int4b_t，tensor量化
- QF322B8_PRE：float量化成int8_t/uint8_t，scalar量化
- VQF322B8_PRE：float量化成int8_t/uint8_t，tensor量化
- QF322FP8_PRE：float量化成fp8_e4m3fn_t，scalar量化
- VQF322FP8_PRE：float量化成fp8_e4m3fn_t，tensor量化
- QF322HIF8_PRE：float量化成hifloat8_t(Half to Away Round)，scalar量化
- VQF322HIF8_PRE：float量化成hifloat8_t(Half to Away Round)，tensor量化
- QF322HIF8_PRE_HYBRID：float量化成hifloat8_t(Hybrid Round)，scalar量化
- VQF322HIF8_PRE_HYBRID：float量化成hifloat8_t(Hybrid Round)，tensor量化
- QF322F32_PRE：float量化成float，scalar量化，精度可以达到双千分之一，无法达到双万分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。
- VQF322F32_PRE：float量化成float，tensor量化，精度可以达到双千分之一，无法达到双万分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。

## 函数原型

- 常规搬运

    ```cpp
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm(
        __gm__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    __aicore__ inline void asc_copy_l0c2gm_sync(
        __gm__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
        uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
        uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
        bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool enable_nz2dn)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|:-----------------|:------|:-----------------------------------|
| dst | 输出 | 目的操作数的起始地址。数据格式为NZ、ND或DN格式。ND格式的地址需要满足1字节对齐，NZ和DN格式的地址需要满足32字节对齐。 |
| src | 输入 | 源操作数的起始地址。数据格式为NZ格式，地址需要满足64字节对齐。 |
| n_size | 输入 | 源NZ矩阵在N方向上的大小，取值范围：[1, 4095]。 |
| m_size | 输入 | 源NZ矩阵在M方向上的大小。<br>&nbsp;&nbsp;&bull; 不开启NZ2ND功能，取值范围：[1, 65535]；<br>&nbsp;&nbsp;&bull; 开启NZ2ND功能，取值范围：[1, 8192]。 |
| dst_stride | 输入 | &nbsp;&nbsp;&bull; 若不开启NZ2ND功能，dst_stride表示目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位：element。<br>&nbsp;&nbsp;&bull; 若开启NZ2ND/NZ2DN功能，dst_stride表示目的ND矩阵每一行中的元素个数，取值不为0，单位：element。 |
| src_stride | 输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围：[0, 65535]，单位：C0_Size(16*sizeof(T)，T为src的数据类型)。 |
| l2_cache_mode | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表2](#table2)。 |
| enable_clip_relu_pre | 输入 | 是否开启Clip ReLU，需搭配Normal ReLU一起使用，且需要开启量化功能，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启Clip ReLU；<br>&nbsp;&nbsp;&bull; 1：开启Clip ReLU（scalar 模式）。 |
| unit_flag_mode | 输入 | unit_flag是mmad类指令和矩阵搬出类指令细粒度的并行功能，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0：不开启unit_flag；<br>&nbsp;&nbsp;&bull; 2：开启unit_flag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将mmad类指令和矩阵搬出类指令的unit_flag值设置为2或3。 |
| quant_pre_mode | 输入 | 预处理阶段量化参数。取值见[功能说明](#功能说明)。 |
| relu_pre_mode | 输入 | 预处理阶段ReLU模式控制，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启ReLU；<br>&nbsp;&nbsp;&bull; 1：开启Normal ReLU；<br>&nbsp;&nbsp;&bull; 2：开启Scalar ReLU；<br>&nbsp;&nbsp;&bull; 3：开启Vector ReLU。 |
| enable_channel_split | 输入 | 是否开启通道拆分的功能。仅在src和dst都为float时才能开启Channel Split，且不能同时开启Channel Split和NZ2ND功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |
| enable_nz2nd | 输入 | 是否开启NZ2ND功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |
| quant_post | 输入 | 无效参数，用户无需关注，传入0即可。 |
| relu_post | 输入 | 无效参数，用户无需关注，传入0即可。 |
| clip_relu_post | 输入 | 无效参数，用户无需关注，传入false即可。 |
| eltwise_op | 输入 | 无效参数，用户无需关注，传入0即可。 |
| eltwise_antq_en | 输入 | 无效参数，用户无需关注，传入false即可。 |
| c0_pad_en | 输入 | 无效参数，用户无需关注，传入false即可。 |
| broadcast_en | 输入 | 无效参数，用户无需关注，传入false即可。 |
| enable_nz2dn | 输入 | 是否开启NZ2DN功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |

**表2** l2_cache_mode取值说明 <a id="table2"></a>

| 取值 | 模式 | 含义 |
|:------|:------|:------|
| 0 | NORMAL模式 | 启用L2 Cache，采用write back策略写入L2 Cache，并且将分配的Cache Line标记为高替换优先级。|
| 1 | LAST模式 | &bull; 启用L2 Cache，采用write back策略写入L2 Cache，并且将分配的Cache Line标记为低替换优先级。<br>&bull; LAST模式功能，暂不支持。 |
| 2 | PERSISTENT模式 | &bull; 启用L2 Cache，采用write back策略写入L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GlobalTensor的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。<br>&bull; PERSISTENT模式功能，暂不支持。 |
| 4 | DISABLE模式 | 不启用L2 Cache。如果写入地址在L2 Cache中已经被分配了Cache Line，则将本次写入的数据覆盖Cache Line原有数据后将Cache Line中最新数据写回到GM，并且将该Cache Line标记为invalid。如果写入地址在L2 Cache中没有被分配Cache Line，则直接写回到GM。|

## 返回值说明

无

## 流水类型

PIPE_FIX

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../通用说明和约束.md#存储单元说明)。

## 调用示例

```cpp
// dst是外部输入的float类型的GM地址。
constexpr uint64_t total_length = 128 * 64;
__cc__ float src[total_length];
uint16_t n_size = 64;
uint16_t m_size = 128;
uint32_t dst_stride = 2048;
uint16_t src_stride = 128;
uint8_t l2_cache_mode = 0;
uint8_t enable_clip_relu_pre = 0;
uint8_t unit_flag_mode = 0;
uint64_t quant_pre_mode = QuantMode_t::NoQuant;
uint8_t relu_pre_mode = 0;
bool enable_channel_split = false;
bool enable_nz2nd = false;
bool enable_nz2dn = false;
uint64_t quant_post = 0;  // 无效参数
uint8_t relu_post = 0;  // 无效参数
bool clip_relu_post = false;  // 无效参数
uint8_t eltwise_op = 0;  // 无效参数
bool eltwise_antq_en = false;  // 无效参数
bool c0_pad_en = false;  // 无效参数
bool broadcast_en = false;  // 无效参数
asc_copy_l0c2gm(dst, src, n_size, m_size, dst_stride, src_stride, l2_cache_mode, enable_clip_relu_pre,
    unit_flag_mode, quant_pre_mode, relu_pre_mode, enable_channel_split, enable_nz2nd, quant_post,
    relu_post, clip_relu_post, eltwise_op, eltwise_antq_en, c0_pad_en, broadcast_en, enable_nz2dn);
```
