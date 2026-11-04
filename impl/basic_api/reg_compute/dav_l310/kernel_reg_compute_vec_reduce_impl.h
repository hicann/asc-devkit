/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_reg_compute_vec_reduce_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_vec_reduce_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_reduce_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_REDUCE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_REDUCE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_REDUCE_IMPL_H

#include "kernel_reg_compute_common_impl.h"
namespace AscendC {
namespace Reg {
template <
    typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename DstRegT,
    typename SrcRegT>
__simd_callee__ inline void ReduceSumImpl(DstRegT& dstReg, SrcRegT srcReg, MaskReg mask)
{
    using ActualDstRegT = typename DstRegT::ActualT;
    using ActualSrcRegT = typename SrcRegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualDstRegT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualSrcRegT>, "U type is not correct!");
    static_assert(
        (SupportType<
            Tuple<ActualDstRegT, ActualSrcRegT>, Tuple<int32_t, int16_t>, Tuple<int32_t, int32_t>,
            Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint16_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint32_t>,
            Tuple<half, half>, Tuple<float, float>>()),
        "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcadd(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void ReduceMaxImpl(RegT& dstReg, RegT srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, int8_t, uint8_t, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcmax(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void ReduceMinImpl(RegT& dstReg, RegT srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, int8_t, uint8_t, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcmin(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void ReduceSumWithDataBlockImpl(RegT& dstReg, RegT srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert((SupportType<ActualT, float, half>()), "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgadd(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void ReduceMaxWithDataBlockImpl(RegT& dstReg, RegT srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert((SupportType<ActualT, float, half>()), "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgmax(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void ReduceMinWithDataBlockImpl(RegT& dstReg, RegT srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert((SupportType<ActualT, float, half>()), "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgmin(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void PairReduceSumImpl(RegT& dstReg, RegT srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert((SupportType<ActualT, float, half>()), "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcpadd(dstReg, srcReg, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_REDUCE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_REDUCE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_REDUCE_IMPL__
#endif
