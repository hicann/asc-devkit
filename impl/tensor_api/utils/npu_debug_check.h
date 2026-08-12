/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning                                                                                                               \
    "impl/tensor_api/utils/npu_debug_check.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define TENSOR_API_DEBUG_CHECK_OWNS_INTERNAL_HEADER_ACCESS
#endif

/*!
 * \file npu_debug_check.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_NPU_DEBUG_CHECK_H
#define IMPL_TENSOR_API_UTILS_NPU_DEBUG_CHECK_H

#include "impl/tensor_api/utils/npu_debug_report.h"

#ifdef ASCENDC_DEBUG
namespace asc {
namespace te {

template <typename LayoutType, typename CoordType>
__aicore__ inline constexpr void debug_check_coord(const tensor_api_assert_context& context, const LayoutType& layout,
                                                   const CoordType& coord, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(context, (is_coord_in_shape(coord, layout.shape())),
                               TENSOR_API_REPORT_INTERNAL(report_coord_error, layout, coord, api_name));
}

template <typename ShapeType, typename CoordType>
__aicore__ inline constexpr void debug_check_coord_shape(const tensor_api_assert_context& context,
                                                         const ShapeType& shape, const CoordType& coord,
                                                         __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(context, (is_coord_in_shape(coord, shape)),
                               TENSOR_API_REPORT_INTERNAL(report_coord_shape_error, shape, coord, api_name));
}

#define TENSOR_API_DETAIL_CHECK_LAYOUT_STRIDE(ctx, value, name, api)                                                   \
    TENSOR_API_DEBUG_ASSERT_AT(                                                                                        \
        ctx, (is_stride_valid((value).stride())),                                                                      \
        TENSOR_API_REPORT_INTERNAL(report_tensor_layout_error<tensor_layout_error_kind::stride>, (value), name, api))

template <typename LayoutType>
__aicore__ inline constexpr void debug_check_layout(const tensor_api_assert_context& context, const LayoutType& layout,
                                                    __gm__ const char* tensor_name, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(context, (is_shape_valid(layout.shape())),
                               TENSOR_API_REPORT_INTERNAL(report_tensor_layout_error, layout, tensor_name, api_name));
    TENSOR_API_DETAIL_CHECK_LAYOUT_STRIDE(context, layout, tensor_name, api_name);
}

template <typename LayoutType>
__aicore__ inline constexpr void debug_check_make_tensor_shape(const tensor_api_assert_context& context,
                                                               const LayoutType& layout)
{
    TENSOR_API_DEBUG_ASSERT_AT(context, (is_shape_valid(layout.shape())),
                               TENSOR_API_REPORT_INTERNAL(report_tensor_layout_error, layout, "result", "make_tensor"));
    TENSOR_API_DETAIL_CHECK_LAYOUT_STRIDE(context, layout, "result", "make_tensor");
}

template <typename ShapeType, typename StrideType>
__aicore__ inline constexpr void debug_check_make_tensor_shape(const tensor_api_assert_context& context,
                                                               const ShapeType& shape, const StrideType& stride)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (is_shape_valid(shape)),
        TENSOR_API_REPORT_INTERNAL(report_tensor_layout_error, make_layout(shape, stride), "result", "make_tensor"));
    TENSOR_API_DETAIL_CHECK_LAYOUT_STRIDE(context, make_layout(shape, stride), "result", "make_tensor");
}

template <typename SrcTensorType, typename DstTensorType>
__aicore__ inline constexpr void debug_check_copy_size(const tensor_api_assert_context& context,
                                                       const SrcTensorType& src, const DstTensorType& dst,
                                                       __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(context, (dst.size() >= src.size()),
                               TENSOR_API_REPORT_INTERNAL(report_copy_size_error, src, dst, api_name));
}

template <typename LayoutType, typename CoordType, typename InfoType>
__aicore__ inline constexpr void debug_check_slice_args(const tensor_api_assert_context& context,
                                                        const LayoutType& layout, const CoordType& coord,
                                                        const InfoType& info)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (is_shape_valid(layout.shape())),
        TENSOR_API_REPORT_INTERNAL(report_slice_error<slice_error_kind::source_shape>, layout, coord, info));
    TENSOR_API_DETAIL_CHECK_LAYOUT_STRIDE(context, layout, "source", "slice");
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (is_slice_shape_valid(info)),
        TENSOR_API_REPORT_INTERNAL(report_slice_error<slice_error_kind::slice_shape>, layout, coord, info));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (is_coord_in_shape(coord, layout.shape())),
        TENSOR_API_REPORT_INTERNAL(report_slice_error<slice_error_kind::coord>, layout, coord, info));
}

#undef TENSOR_API_DETAIL_CHECK_LAYOUT_STRIDE

template <typename ParamsType>
__aicore__ inline constexpr void debug_check_mmad_params(const tensor_api_assert_context& context,
                                                         const ParamsType& params, bool disable_gemv,
                                                         __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (params.m <= DEBUG_MMAD_DIM_MAX),
        TENSOR_API_LOG_INTERNAL("Failed to check m in %s, m must be in range [%u, %u]; current value is %u.", api_name,
                                static_cast<unsigned int>(DEBUG_MMAD_DIM_MIN),
                                static_cast<unsigned int>(DEBUG_MMAD_DIM_MAX), static_cast<unsigned int>(params.m)));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (params.n <= DEBUG_MMAD_DIM_MAX),
        TENSOR_API_LOG_INTERNAL("Failed to check n in %s, n must be in range [%u, %u]; current value is %u.", api_name,
                                static_cast<unsigned int>(DEBUG_MMAD_DIM_MIN),
                                static_cast<unsigned int>(DEBUG_MMAD_DIM_MAX), static_cast<unsigned int>(params.n)));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (params.k <= DEBUG_MMAD_DIM_MAX),
        TENSOR_API_LOG_INTERNAL("Failed to check k in %s, k must be in range [%u, %u]; current value is %u.", api_name,
                                static_cast<unsigned int>(DEBUG_MMAD_DIM_MIN),
                                static_cast<unsigned int>(DEBUG_MMAD_DIM_MAX), static_cast<unsigned int>(params.k)));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (disable_gemv || params.m == DEBUG_GEMV_M_SIZE),
        TENSOR_API_LOG_INTERNAL("Failed to check m in %s, m must be %u when GEMV is enabled; current value is %u.",
                                api_name, static_cast<unsigned int>(DEBUG_GEMV_M_SIZE),
                                static_cast<unsigned int>(params.m)));
}

template <typename ValueType>
__aicore__ inline constexpr void debug_check_unit_flag(const tensor_api_assert_context& context,
                                                       const ValueType& unit_flag, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (unit_flag == DEBUG_UNIT_FLAG_DEFAULT || unit_flag == DEBUG_UNIT_FLAG_KEEP
         || unit_flag == DEBUG_UNIT_FLAG_RESET),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check unitFlag in %s, unitFlag must be %u, %u, or %u; current value is %u.", api_name,
            static_cast<unsigned int>(DEBUG_UNIT_FLAG_DEFAULT), static_cast<unsigned int>(DEBUG_UNIT_FLAG_KEEP),
            static_cast<unsigned int>(DEBUG_UNIT_FLAG_RESET), static_cast<unsigned int>(unit_flag)));
}

template <typename CountType>
__aicore__ inline constexpr void debug_check_block_count(const tensor_api_assert_context& context,
                                                         const CountType& block_count, __gm__ const char* value_name,
                                                         __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (block_count >= DEBUG_BLOCK_COUNT_MIN && block_count <= DEBUG_BLOCK_COUNT_MAX),
        TENSOR_API_LOG_INTERNAL("Failed to check %s in %s, %s must be in range [%u, %u]; current value is %lld.",
                                value_name, api_name, value_name, static_cast<unsigned int>(DEBUG_BLOCK_COUNT_MIN),
                                static_cast<unsigned int>(DEBUG_BLOCK_COUNT_MAX), static_cast<long long>(block_count)));
}

template <typename LengthType, typename MaxLengthType>
__aicore__ inline constexpr void debug_check_block_len(const tensor_api_assert_context& context,
                                                       const LengthType& block_len, const MaxLengthType& max_block_len,
                                                       __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (block_len >= DEBUG_BLOCK_LEN_MIN && block_len <= max_block_len),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check blockLen in %s, blockLen must be in range [%u, %llu]; current value is %llu.", api_name,
            static_cast<unsigned int>(DEBUG_BLOCK_LEN_MIN), static_cast<unsigned long long>(max_block_len),
            static_cast<unsigned long long>(block_len)));
}

template <typename ValueType, typename MaxValueType>
__aicore__ inline constexpr void debug_check_block_limit(const tensor_api_assert_context& context,
                                                         const ValueType& value, const MaxValueType& max_value,
                                                         __gm__ const char* value_name, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (value <= max_value),
        TENSOR_API_LOG_INTERNAL("Failed to check %s in %s, %s must be in range [%u, %llu]; current value is %llu.",
                                value_name, api_name, value_name, static_cast<unsigned int>(DEBUG_COPY_NOP_VALUE),
                                static_cast<unsigned long long>(max_value), static_cast<unsigned long long>(value)));
}

template <typename StrideType, typename LengthType, typename CountType>
__aicore__ inline constexpr void debug_check_gm2ub_stride(const tensor_api_assert_context& context,
                                                          const StrideType& dst_stride, const LengthType& block_len,
                                                          const CountType& block_count, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (block_count == DEBUG_COPY_NOP_VALUE || block_len == DEBUG_COPY_NOP_VALUE
         || (block_count == DEBUG_SINGLE_BLOCK_COUNT && dst_stride == DEBUG_COPY_NOP_VALUE)
         || dst_stride == static_cast<StrideType>(block_len)
         || (dst_stride > DEBUG_COPY_NOP_VALUE && dst_stride % DEBUG_DATA_BLOCK_BYTES == 0)),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check dstStride in %s, for an active copy dstStride must equal blockLen, be a positive "
            "multiple of %u bytes, or be %u when blockCount is %u; current dstStride is %lld, blockLen is %llu, "
            "blockCount is %llu.",
            api_name, static_cast<unsigned int>(DEBUG_DATA_BLOCK_BYTES),
            static_cast<unsigned int>(DEBUG_COPY_NOP_VALUE), static_cast<unsigned int>(DEBUG_SINGLE_BLOCK_COUNT),
            static_cast<long long>(dst_stride), static_cast<unsigned long long>(block_len),
            static_cast<unsigned long long>(block_count)));
}

template <typename ElementType, typename PaddingType, typename StrideType, typename LengthType, typename CountType>
__aicore__ inline constexpr void
debug_check_gm2ub_padding(const tensor_api_assert_context& context, const PaddingType& left_padding_count,
                          const PaddingType& right_padding_count, const StrideType& dst_stride,
                          const LengthType& block_len, const CountType& block_count, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (static_cast<unsigned long long>(left_padding_count) * sizeof(ElementType) <= DEBUG_GM2UB_PADDING_BYTES_MAX),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check leftPaddingCount in %s, left padding must be at most %u bytes; current "
            "leftPaddingCount is %u, dispatchElementBytes is %u.",
            api_name, static_cast<unsigned int>(DEBUG_GM2UB_PADDING_BYTES_MAX),
            static_cast<unsigned int>(left_padding_count), static_cast<unsigned int>(sizeof(ElementType))));
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (static_cast<unsigned long long>(right_padding_count) * sizeof(ElementType) <= DEBUG_GM2UB_PADDING_BYTES_MAX),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check rightPaddingCount in %s, right padding must be at most %u bytes; current "
            "rightPaddingCount is %u, dispatchElementBytes is %u.",
            api_name, static_cast<unsigned int>(DEBUG_GM2UB_PADDING_BYTES_MAX),
            static_cast<unsigned int>(right_padding_count), static_cast<unsigned int>(sizeof(ElementType))));
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (block_count == DEBUG_COPY_NOP_VALUE || block_len == DEBUG_COPY_NOP_VALUE
         || dst_stride != static_cast<StrideType>(block_len)
         || (left_padding_count == DEBUG_COPY_NOP_VALUE && right_padding_count == DEBUG_COPY_NOP_VALUE)),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check padding in %s, leftPaddingCount and rightPaddingCount must both be %u in active Compact "
            "mode; current leftPaddingCount is %u, rightPaddingCount is %u, dstStride is %lld, blockLen is %llu, "
            "blockCount is %llu.",
            api_name, static_cast<unsigned int>(DEBUG_COPY_NOP_VALUE), static_cast<unsigned int>(left_padding_count),
            static_cast<unsigned int>(right_padding_count), static_cast<long long>(dst_stride),
            static_cast<unsigned long long>(block_len), static_cast<unsigned long long>(block_count)));
}

template <typename StrideType, typename LengthType, typename CountType>
__aicore__ inline constexpr void debug_check_ub2gm_stride(const tensor_api_assert_context& context,
                                                          const StrideType& src_stride, const LengthType& block_len,
                                                          const CountType& block_count, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (block_count == DEBUG_COPY_NOP_VALUE || block_len == DEBUG_COPY_NOP_VALUE
         || (src_stride > DEBUG_COPY_NOP_VALUE
             && (src_stride == static_cast<StrideType>(block_len) || src_stride % DEBUG_DATA_BLOCK_BYTES == 0))),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check srcStride in %s, for an active copy srcStride must equal blockLen or be a positive "
            "multiple of %u bytes; current srcStride is %lld, blockLen is %llu, blockCount is %llu.",
            api_name, static_cast<unsigned int>(DEBUG_DATA_BLOCK_BYTES), static_cast<long long>(src_stride),
            static_cast<unsigned long long>(block_len), static_cast<unsigned long long>(block_count)));
}

template <typename BatchType>
__aicore__ inline constexpr void debug_check_batch_count(const tensor_api_assert_context& context,
                                                         const BatchType& batch, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (batch <= DEBUG_BATCH_COUNT_MAX),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check src batch shape size in %s, src batch shape size must be at most %u; current value is "
            "%lld.",
            api_name, static_cast<unsigned int>(DEBUG_BATCH_COUNT_MAX), static_cast<long long>(batch)));
}

template <typename SrcBatchType, typename DstBatchType>
__aicore__ inline constexpr void debug_check_batch_match(const tensor_api_assert_context& context,
                                                         const SrcBatchType& src_batch, const DstBatchType& dst_batch,
                                                         __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (src_batch == dst_batch),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check src and dst batch shape sizes in %s, src batch shape size must equal dst batch shape "
            "size; current srcBatchShape is %lld, dstBatchShape is %lld.",
            api_name, static_cast<long long>(src_batch), static_cast<long long>(dst_batch)));
}

template <typename SrcColumnBytesType, typename DstColumnBytesType>
__aicore__ inline constexpr void
debug_check_l12bt_column_bytes(const tensor_api_assert_context& context, const SrcColumnBytesType& src_column_bytes,
                               const DstColumnBytesType& dst_column_bytes, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (src_column_bytes % DEBUG_DATA_BLOCK_BYTES == 0 && dst_column_bytes % DEBUG_DATA_BLOCK_BYTES == 0),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check src and dst column byte sizes in %s, src and dst column byte sizes must be %u-byte "
            "aligned; current srcColumnBytes is %llu, dstColumnBytes is %llu.",
            api_name, static_cast<unsigned int>(DEBUG_DATA_BLOCK_BYTES),
            static_cast<unsigned long long>(src_column_bytes), static_cast<unsigned long long>(dst_column_bytes)));
}

template <typename SrcStrideType, typename DstStrideType>
__aicore__ inline constexpr void
debug_check_fixpipe_stride(const tensor_api_assert_context& context, const SrcStrideType& src_stride,
                           const DstStrideType& dst_stride, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (dst_stride > DEBUG_COPY_NOP_VALUE),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check dstStride in %s, dstStride must be greater than %u; current value is %u.", api_name,
            static_cast<unsigned int>(DEBUG_COPY_NOP_VALUE), static_cast<unsigned int>(dst_stride)));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (src_stride <= DEBUG_FIXPIPE_SRC_STRIDE_MAX),
        TENSOR_API_LOG_INTERNAL("Failed to check srcStride in %s, srcStride must be at most %u; current value is %u.",
                                api_name, static_cast<unsigned int>(DEBUG_FIXPIPE_SRC_STRIDE_MAX),
                                static_cast<unsigned int>(src_stride)));
}

template <typename SizeType>
__aicore__ inline constexpr void debug_check_fixpipe_m(const tensor_api_assert_context& context, const SizeType& m_size,
                                                       bool nz2nd_en, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (m_size >= DEBUG_FIXPIPE_M_MIN
         && ((!nz2nd_en && m_size <= DEBUG_FIXPIPE_M_MAX) || (nz2nd_en && m_size <= DEBUG_FIXPIPE_M_ND_MAX))),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check mSize in %s, mSize must be in range [%u, %u] when the dst format is ND and [%u, %u] "
            "otherwise; current mSize is %u, nz2ndEn is %u.",
            api_name, static_cast<unsigned int>(DEBUG_FIXPIPE_M_MIN), static_cast<unsigned int>(DEBUG_FIXPIPE_M_ND_MAX),
            static_cast<unsigned int>(DEBUG_FIXPIPE_M_MIN), static_cast<unsigned int>(DEBUG_FIXPIPE_M_MAX),
            static_cast<unsigned int>(m_size), static_cast<unsigned int>(nz2nd_en)));
}

template <typename SizeType>
__aicore__ inline constexpr void debug_check_fixpipe_n(const tensor_api_assert_context& context, const SizeType& n_size,
                                                       bool enable_channel_split, bool nz2nd_en, bool nz2dn_en,
                                                       __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (nz2nd_en || nz2dn_en
         || (enable_channel_split ? n_size % DEBUG_FIXPIPE_SPLIT_N_MULTIPLE == 0 :
                                    n_size % DEBUG_FIXPIPE_N_MULTIPLE == 0)),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check nSize in %s NZ instruction, nSize must be divisible by %u when enableChannelSplit is "
            "enabled or by %u otherwise; current nSize is %u, enableChannelSplit is %u, nz2ndEn is %u, nz2dnEn is "
            "%u.",
            api_name, static_cast<unsigned int>(DEBUG_FIXPIPE_SPLIT_N_MULTIPLE),
            static_cast<unsigned int>(DEBUG_FIXPIPE_N_MULTIPLE), static_cast<unsigned int>(n_size),
            static_cast<unsigned int>(enable_channel_split), static_cast<unsigned int>(nz2nd_en),
            static_cast<unsigned int>(nz2dn_en)));
}

template <typename CountType, typename LengthType>
__aicore__ inline constexpr void debug_check_copy_blocks(const tensor_api_assert_context& context,
                                                         const CountType& block_count, const LengthType& block_len,
                                                         __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (block_count > DEBUG_COPY_NOP_VALUE),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check blockCount in %s, blockCount must be greater than %u; current value is %u.", api_name,
            static_cast<unsigned int>(DEBUG_COPY_NOP_VALUE), static_cast<unsigned int>(block_count)));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (block_len > DEBUG_COPY_NOP_VALUE),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check blockLen in %s, blockLen must be greater than %u; current value is %u.", api_name,
            static_cast<unsigned int>(DEBUG_COPY_NOP_VALUE), static_cast<unsigned int>(block_len)));
}

template <typename DataSizeType, typename StepType>
__aicore__ inline constexpr void debug_check_l0_transpose(const tensor_api_assert_context& context, bool transpose,
                                                          const DataSizeType& data_size, const StepType& k_step,
                                                          __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context,
        (!transpose || data_size != DEBUG_L0_TRANSPOSE_DATA_BYTES || k_step % DEBUG_L0_TRANSPOSE_K_STEP_MULTIPLE == 0),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check kStep in %s transpose instruction, kStep must be divisible by %u for %u-byte data; "
            "current value is %u.",
            api_name, static_cast<unsigned int>(DEBUG_L0_TRANSPOSE_K_STEP_MULTIPLE),
            static_cast<unsigned int>(DEBUG_L0_TRANSPOSE_DATA_BYTES), static_cast<unsigned int>(k_step)));
}

template <typename StepType>
__aicore__ inline constexpr void debug_check_l0_step(const tensor_api_assert_context& context, const StepType& step,
                                                     __gm__ const char* step_name, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (step <= DEBUG_L0_STEP_MAX),
        TENSOR_API_LOG_INTERNAL("Failed to check %s in %s, %s must be at most %u; current value is %lld.", step_name,
                                api_name, step_name, static_cast<unsigned int>(DEBUG_L0_STEP_MAX),
                                static_cast<long long>(step)));
}

template <typename StepType>
__aicore__ inline constexpr void debug_check_l0_m_step(const tensor_api_assert_context& context, const StepType& m_step,
                                                       bool is_b4, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (m_step % (is_b4 ? DEBUG_L0_B4_M_STEP_MULTIPLE : DEBUG_L0_B8_M_STEP_MULTIPLE) == 0),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check mStep in %s, mStep must be divisible by %u for b4 or by %u for b8; current value is "
            "%lld.",
            api_name, static_cast<unsigned int>(DEBUG_L0_B4_M_STEP_MULTIPLE),
            static_cast<unsigned int>(DEBUG_L0_B8_M_STEP_MULTIPLE), static_cast<long long>(m_step)));
}

template <typename SrcStrideType, typename SrcCapacityType, typename DstStrideType, typename DstCapacityType>
__aicore__ inline constexpr void
debug_check_l0_batch_stride(const tensor_api_assert_context& context, const SrcStrideType& src_stride,
                            const SrcCapacityType& src_capacity, const DstStrideType& dst_stride,
                            const DstCapacityType& dst_capacity, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (src_stride == src_capacity && dst_stride == dst_capacity),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check src and dst batch strides in %s, each batch stride must equal its matrix capacity; "
            "current srcBatchStride is %lld, srcMatrixCapacity is %lld, dstBatchStride is %lld, dstMatrixCapacity is "
            "%lld.",
            api_name, static_cast<long long>(src_stride), static_cast<long long>(src_capacity),
            static_cast<long long>(dst_stride), static_cast<long long>(dst_capacity)));
}

template <typename ElementType, typename ParamsType>
__aicore__ inline constexpr void debug_check_img2col_coord(const tensor_api_assert_context& context,
                                                           const ParamsType& params, __gm__ const char* api_name)
{
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (params.m_start_pt <= DEBUG_IMG2COL_M_START_MAX),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check img2col start coordinate in %s, mStartPt=%u, kStartPt=%u, dataBits=%u; mStartPt must be "
            "in range [0, %u].",
            api_name, static_cast<unsigned int>(params.m_start_pt), static_cast<unsigned int>(params.k_start_pt),
            static_cast<unsigned int>(sizeof(ElementType) * DEBUG_IMG2COL_DATA_BITS_PER_BYTE),
            static_cast<unsigned int>(DEBUG_IMG2COL_M_START_MAX)));
    TENSOR_API_DEBUG_ASSERT_AT(
        context, (params.k_start_pt % DEBUG_IMG2COL_K_START_MULTIPLE<ElementType> == DEBUG_IMG2COL_ALIGNED_REMAINDER),
        TENSOR_API_LOG_INTERNAL(
            "Failed to check img2col start coordinate in %s, mStartPt=%u, kStartPt=%u, dataBits=%u; kStartPt must be "
            "a multiple of %u for b%u data.",
            api_name, static_cast<unsigned int>(params.m_start_pt), static_cast<unsigned int>(params.k_start_pt),
            static_cast<unsigned int>(sizeof(ElementType) * DEBUG_IMG2COL_DATA_BITS_PER_BYTE),
            static_cast<unsigned int>(DEBUG_IMG2COL_K_START_MULTIPLE<ElementType>),
            static_cast<unsigned int>(sizeof(ElementType) * DEBUG_IMG2COL_DATA_BITS_PER_BYTE)));
}

} // namespace te
} // namespace asc
#endif // ASCENDC_DEBUG

#endif // IMPL_TENSOR_API_UTILS_NPU_DEBUG_CHECK_H

#if defined(TENSOR_API_DEBUG_CHECK_OWNS_INTERNAL_HEADER_ACCESS)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef TENSOR_API_DEBUG_CHECK_OWNS_INTERNAL_HEADER_ACCESS
#endif
