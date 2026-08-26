/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m510/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_IMPL_H

#include "../../../include/basic_api/kernel_operator_mm_bitmode_intf.h"

namespace AscendC {
/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASSERT(false);
}
template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASSERT(false);
}
/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int16_t, float, int32_t,
            uint32_t, int4b_t>(),
        "LoadData 2dv2 only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, int16_t, float, int32_t, uint32_t, int4b_t on current device!");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_ca_s4(
                (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.GetMStartPosition(),
                loadDataParam.GetKStartPosition(), loadDataParam.GetMStep(), loadDataParam.GetKStep(),
                loadDataParam.GetSrcStride(), loadDataParam.GetDstStride(), 1);
        } else {
            load_cbuf_to_ca_s4(
                (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.GetMStartPosition(),
                loadDataParam.GetKStartPosition(), loadDataParam.GetMStep(), loadDataParam.GetKStep(),
                loadDataParam.GetSrcStride(), loadDataParam.GetDstStride(), 0);
        }
    } else {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_ca(
                dst, src, loadDataParam.GetMStartPosition(), loadDataParam.GetKStartPosition(),
                loadDataParam.GetMStep(), loadDataParam.GetKStep(), loadDataParam.GetSrcStride(),
                loadDataParam.GetDstStride(), 1);
        } else {
            load_cbuf_to_ca(
                dst, src, loadDataParam.GetMStartPosition(), loadDataParam.GetKStartPosition(),
                loadDataParam.GetMStep(), loadDataParam.GetKStep(), loadDataParam.GetSrcStride(),
                loadDataParam.GetDstStride(), 0);
        }
    }
#else
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_ca_s4(
                (__ca__ T*)dst, (__cbuf__ T*)src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), true);
        } else {
            load_cbuf_to_ca_s4(
                (__ca__ T*)dst, (__cbuf__ T*)src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), false);
        }
    } else {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_ca(dst, src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), true);
        } else {
            load_cbuf_to_ca(dst, src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), false);
        }
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    static_assert(
        SupportType<
            T, int4b_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int16_t, float,
            int32_t, uint32_t>(),
        "LoadData 2dv2 only support int4b_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, int16_t, float, int32_t, uint32_t on current device!");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.GetMStartPosition(),
                loadDataParam.GetKStartPosition(), loadDataParam.GetMStep(), loadDataParam.GetKStep(),
                loadDataParam.GetSrcStride(), loadDataParam.GetDstStride(), 1);
        } else {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.GetMStartPosition(),
                loadDataParam.GetKStartPosition(), loadDataParam.GetMStep(), loadDataParam.GetKStep(),
                loadDataParam.GetSrcStride(), loadDataParam.GetDstStride(), 0);
        }
    } else {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_cb(
                dst, src, loadDataParam.GetMStartPosition(), loadDataParam.GetKStartPosition(),
                loadDataParam.GetMStep(), loadDataParam.GetKStep(), loadDataParam.GetSrcStride(),
                loadDataParam.GetDstStride(), 1);
        } else {
            load_cbuf_to_cb(
                dst, src, loadDataParam.GetMStartPosition(), loadDataParam.GetKStartPosition(),
                loadDataParam.GetMStep(), loadDataParam.GetKStep(), loadDataParam.GetSrcStride(),
                loadDataParam.GetDstStride(), 0);
        }
    }
#else
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_cb_s4(
                (__cb__ T*)dst, (__cbuf__ T*)src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), true);
        } else {
            load_cbuf_to_cb_s4(
                (__cb__ T*)dst, (__cbuf__ T*)src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), false);
        }
    } else {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_cb(dst, src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), true);
        } else {
            load_cbuf_to_cb(dst, src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), false);
        }
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int16_t, float, int32_t,
            uint32_t, int4b_t>(),
        "LoadData 2dv2 only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, int16_t, float, int32_t, uint32_t, int4b_t on current device!");
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_ca_s4(
                (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_ca_s4(
                (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    } else {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_ca(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_ca(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    static_assert(
        SupportType<
            T, int4b_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int16_t, float,
            int32_t, uint32_t>(),
        "LoadData 2dv2 only support int4b_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, int16_t, float, int32_t, uint32_t on current device!");
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    } else {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_cb(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void LoadData2DGM2L1Cal(
    __cbuf__ T* dst, __gm__ U* src, const LoadData2DParamsV2& loadDataParam, const Nd2NzParamsV2& nd2nzParams)
{
    static_assert(
        ((IsSameType<T, int8_t>::value && IsSameType<U, int4b_t>::value) ||
         (IsSameType<T, int4b_t>::value && IsSameType<U, int2b_t>::value) ||
         (IsSameType<T, int8_t>::value && IsSameType<U, int2b_t>::value) ||
         (IsSameType<T, int4b_t>::value && IsSameType<U, uint1b_t>::value)),
        "LoadData dst type or src type is not supported on current device!");
    set_mte2_qtable0(nd2nzParams.lookupTable0);
    set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    uint8_t decompressMode = 0;
    if constexpr (IsSameType<T, int4b_t>::value && IsSameType<U, int2b_t>::value) {
        decompressMode = 0b0111;
    } else if constexpr (IsSameType<T, int8_t>::value && IsSameType<U, int2b_t>::value) {
        decompressMode = 0b0001;
    } else if constexpr (IsSameType<T, int4b_t>::value && IsSameType<U, uint1b_t>::value) {
        decompressMode = 0b0110;
    } else if constexpr (IsSameType<T, int8_t>::value && IsSameType<U, int4b_t>::value) {
        set_mte2_qtable1(nd2nzParams.lookupTable1);
        decompressMode = 0b0011;
    }
    load_gm_to_cbuf_2dv2(
        (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
        loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, 0, decompressMode, 0);
}

/* **************************************************************************************************
 * LoadDataWithTranspose                                        *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadDataWithTranspose only support uint8_t, int8_t, half, bfloat16_t, float, int32_t, uint32_t \
         on current device!");
    uint32_t unitMultiples = 1;
    if (IsSameType<T, int8_t>::value || IsSameType<T, uint8_t>::value || IsSameType<T, int32_t>::value ||
        IsSameType<T, uint32_t>::value || IsSameType<T, float>::value) {
        unitMultiples = 2;
    }
    // Value multiplication due to unit mismatch
    LoadData2dTransposeParamsV2 loadDataParamsV2(
        loadDataParam.startIndex * unitMultiples, loadDataParam.repeatTimes, loadDataParam.srcStride * unitMultiples,
        loadDataParam.dstGap, loadDataParam.dstFracGap, 0, loadDataParam.addrMode);
    LoadData2DL12L0BTransposeCal(dst, src, loadDataParamsV2);
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadDataWithTranspose only support uint8_t, int8_t, half, bfloat16_t, float, int32_t, uint32_t \
         on current device!");
    load_cbuf_to_cb_transpose(
        dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.dstGap,
        inc, loadDataParam.dstFracGap, loadDataParam.srcFracGap);
}

template <typename T, typename U = T, typename V = fp8_e8m0_t>
__aicore__ inline void LoadData2DL12L0ACal(
    __ca__ U* dst, __cbuf__ T* src0, __cbuf__ V* src1, const LoadData2DParamsV2& loadDataParam,
    const LoadData2DMxParams& loadMxDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "current device don't support LoadData using LoadData2DMxParams"); });
}

template <typename T, typename U = T, typename V = fp8_e8m0_t>
__aicore__ inline void LoadData2DL12L0BCal(
    __cb__ U* dst, __cbuf__ T* src0, __cbuf__ V* src1, const LoadData2DParamsV2& loadDataParam,
    const LoadData2DMxParams& loadMxDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "current device don't support LoadData using LoadData2DMxParams"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(
    __ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d_v2 from GM to L0A Buffer(A2) on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(
    __cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d_v2 from GM to L0B Buffer(B2) on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(
    __cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const uint8_t cacheMode = 0)
{
    static_assert(
        SupportType<
            T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half,
            bfloat16_t, int16_t, float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support int4b_t fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, int16_t, float, int32_t, uint32_t on current device!");
    set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        load_gm_to_cbuf_2dv2_s4(
            (__cbuf__ void*)dst, (__gm__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0, cacheMode);
    } else if constexpr (SupportBytes<T, 2>()) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0, cacheMode);
    } else {
        load_gm_to_cbuf_2dv2(
            dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.dstStride,
            loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0, cacheMode);
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(
            KERNEL_ERROR,
            "LoadDataWithTranspose from L1 Buffer(A1) to L0A Buffer(A2) is not supported on current device");
    });
}

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, const MmadParams& mmadParams)
{
    bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
    // f162s32,f16s8, e4m3e4m3, e4m3s8, e4m3s4 algorithm will need configure fixval, others do not
    if constexpr (SupportType<Tuple<Src0T, Src1T, DstT>, Tuple<half, half, int32_t>>()) {
        // f162s32
        mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.fixShiftVal, mmadParams.unitFlag,
            mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
    } else if constexpr (SupportType<Tuple<Src0T, Src1T, DstT>, Tuple<half, int8_t, int32_t>>()) {
        // f16s8
        mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.fixShiftVal, mmadParams.unitFlag,
            mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
    } else if constexpr (SupportType<Tuple<Src0T, Src1T, DstT>, Tuple<int8_t, int4b_t, int32_t>>()) {
        // s8s4
        constexpr uint8_t fixShiftVal = 0;
        mad_s8s4(
            c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, fixShiftVal, mmadParams.unitFlag, mmadParams.disableGemv,
            mmadParams.cmatrixSource, cmatrixInitVal);
    } else if constexpr ((IsSameType<Src0T, int4b_t>::value) && (IsSameType<Src1T, int4b_t>::value)) {
        mad_s4(
            c, (__ca__ void*)a, (__cb__ void*)b, mmadParams.m, mmadParams.k, mmadParams.n, 0, mmadParams.unitFlag,
            mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
    } else {
        // currently support: s16s8, s8, e4m3e4m3, s16s16, e4m3s8
        constexpr uint8_t fixShiftVal = 0;
        mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, fixShiftVal, mmadParams.unitFlag, mmadParams.disableGemv,
            mmadParams.cmatrixSource, cmatrixInitVal);
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
    // Xd[31:0]: matrix c addr in L0C, Xd[63:32]: bias addr in bias table buffer;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint64_t xd = (uint64_t)c;
#else
    uint64_t xd = ((uint64_t)c) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
#endif
    // f162s32,f16s8, e4m3e4m3, e4m3s8, e4m3s4 algorithm will need configure fixval, others do not
    if constexpr (SupportType<Tuple<Src0T, Src1T, DstT>, Tuple<half, half, int32_t>>()) {
        // f162s32
        mad((__cc__ DstT*)xd, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.fixShiftVal,
            mmadParams.unitFlag, mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
    } else if constexpr (SupportType<Tuple<Src0T, Src1T, DstT>, Tuple<half, int8_t, int32_t>>()) {
        // f16s8
        mad((__cc__ DstT*)xd, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.fixShiftVal,
            mmadParams.unitFlag, mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
    } else if constexpr (SupportType<Tuple<Src0T, Src1T, DstT>, Tuple<int8_t, int4b_t, int32_t>>()) {
        // s8s4
        constexpr uint8_t fixShiftVal = 0;
        mad_s8s4(
            (__cc__ DstT*)xd, a, b, mmadParams.m, mmadParams.k, mmadParams.n, fixShiftVal, mmadParams.unitFlag,
            mmadParams.disableGemv, cmatrixSource, cmatrixInitVal);
    } else if constexpr ((IsSameType<Src0T, int4b_t>::value) && (IsSameType<Src1T, int4b_t>::value)) {
        mad_s4(
            c, (__ca__ void*)a, (__cb__ void*)b, mmadParams.m, mmadParams.k, mmadParams.n, 0, mmadParams.unitFlag,
            mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
    } else {
        // currently support: s16s8, s8, e4m3e4m3, s16s16, e4m3s8
        constexpr uint8_t fixShiftVal = 0;
        mad((__cc__ DstT*)xd, a, b, mmadParams.m, mmadParams.k, mmadParams.n, fixShiftVal, mmadParams.unitFlag,
            mmadParams.disableGemv, cmatrixSource, cmatrixInitVal);
    }
}

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    broadcast_ub_to_cc(dstLocal, srcLocal, blockCount, blockLen, srcGap, dstGap);
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    int64_t repeatBit = ((uint64_t)initConstValueParams.blockNum << 16) |
                        ((uint64_t)initConstValueParams.dstGap << 32) | initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, bfloat16_t>::value) {
        create_cbuf_matrix_bf16(dst, repeatBit, initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, uint32_t>::value || IsSameType<T, half>::value) {
        create_cbuf_matrix(dst, repeatBit, static_cast<T>(initConstValueParams.initValue));
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_cbuf_matrix(dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else if constexpr (IsSameType<T, float>::value || IsSameType<T, int32_t>::value) {
        create_cbuf_matrix(
            dst, repeatBit, static_cast<uint32_t>(GetScalarBitcodeValue(initConstValueParams.initValue)));
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "InitConstValue doesn't support current data type on current device"); });
    }
}

/* **************************************************************************************************
 * InitL0ANzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0ANzMatrixCal(__ca__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "InitConstValue in L0A Buffer(A2) is not supported on current device"); });
}

/* **************************************************************************************************
 * InitL0BNzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0BNzMatrixCal(__cb__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "InitConstValue in L0B Buffer(B2) is not supported on current device"); });
}

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV1L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v1 from l1 to l0a"); });
}

template <typename T>
__aicore__ inline void LoadData3DV1L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v1 from l1 to l0b"); });
}

template <typename T>
__aicore__ inline void LoadData3DV1L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v1 from l1 to ubuf"); });
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * ************************************************************************************************* */
__aicore__ inline void Load3DSetFMatrixCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    uint64_t regFMatrix = 0;
    regFMatrix |= uint64_t(l1W & 0xFFFF);

    constexpr uint32_t l1HShiftBit = 16;
    regFMatrix |= uint64_t(l1H & 0xFFFF) << l1HShiftBit;

    constexpr uint32_t padNumber = 4;
    constexpr uint32_t padListShiftBit = 8;
    constexpr uint32_t padListShiftBase = 32;
    for (uint32_t i = 0; i < padNumber; i++) {
        regFMatrix |= uint64_t(padList[i] & 0xFF) << (padListShiftBase + i * padListShiftBit);
    }
    set_fmatrix(regFMatrix);
}

__aicore__ inline void Load3DSetFMatrixCal(uint64_t regFMatrix) { set_fmatrix(regFMatrix); }

__aicore__ inline void Load3DSetFMatrixBCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    uint64_t regFMatrix = 0;
    regFMatrix |= (uint64_t)l1W;

    constexpr uint32_t l1HShiftBit = 16;
    regFMatrix |= (uint64_t)l1H << l1HShiftBit;

    constexpr uint32_t padNumber = 4;
    constexpr uint32_t padListShiftBit = 8;
    constexpr uint32_t padListShiftBase = 32;
    for (uint32_t i = 0; i < padNumber; i++) {
        regFMatrix |= uint64_t(padList[i] & 0xFF) << (padListShiftBase + i * padListShiftBit);
    }
    set_fmatrix_b(regFMatrix);
}

__aicore__ inline void Load3DSetFMatrixBCal(uint64_t regFMatrix) { set_fmatrix_b(regFMatrix); }

template <typename T>
__aicore__ inline void Load3DSetPaddingCal(const T padValue)
{
    uint16_t paddingValue = 0;
    constexpr uint16_t padValueShiftBit = 8;

    if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        paddingValue = (uint16_t)GetScalarBitcodeValue((T)padValue);
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        paddingValue = (uint32_t)GetScalarBitcodeValue((T)padValue);
    } else {
        // type = b8, PADDING[15:8] and PADDING[7:0] should be the same, while PADDING[31:16] is neglected.
        uint16_t u16Value = static_cast<uint16_t>(padValue) & 0xFFu;
        paddingValue = (u16Value << padValueShiftBit) | u16Value;
    }
    set_padding(paddingValue);
}

__aicore__ inline void SetLoadDataRepeatWithStrideCal(const LoadDataRepeatParamWithStride repeatParams)
{
    uint64_t rptConfig = 0;
    constexpr uint32_t repeatTimeShiftBit = 16;
    rptConfig |= uint64_t(repeatParams.repeatStride);
    rptConfig |= uint64_t(repeatParams.repeatTime) << repeatTimeShiftBit;
    constexpr uint32_t repeatModeShiftBit = 24;
    rptConfig |= uint64_t(repeatParams.repeatMode) << repeatModeShiftBit;

    constexpr uint32_t dstStrideShiftBit = 32;
    rptConfig |= uint64_t(repeatParams.dstStride) << dstStrideShiftBit;
    set_l3d_rpt(rptConfig);
}

__aicore__ inline void SetLoadDataRepeatCal(const LoadDataRepeatParam repeatParams)
{
    uint64_t rptConfig = 0;
    constexpr uint32_t repeatTimeShiftBit = 16;
    rptConfig |= uint64_t(repeatParams.repeatStride);
    rptConfig |= uint64_t(repeatParams.repeatTime) << repeatTimeShiftBit;
    constexpr uint32_t repeatModeShiftBit = 24;
    rptConfig |= uint64_t(repeatParams.repeatMode) << repeatModeShiftBit;

    constexpr uint32_t dstStrideShiftBit = 32;
    rptConfig |= uint64_t(repeatParams.dstStride) << dstStrideShiftBit;
    set_l3d_rpt(rptConfig);
}

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataBoundaryCal(uint32_t boundaryValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetLoadDataBoundary is not supported on current device!"); });
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12UB is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0AWithStrideCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BWithStrideCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

__aicore__ inline void LoadData3DV2L12L0AWithStrideCal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

__aicore__ inline void LoadData3DV2L12L0BWithStrideCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBWithStrideCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12UB is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(__ca__ T* dst, __cbuf__ T* src, const Load3DBitModeParam& loadDataParams)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.GetKExtension(), loadDataParams.GetMExtension(), loadDataParams.GetKStartPt(),
        loadDataParams.GetMStartPt(), loadDataParams.GetStrideW(), loadDataParams.GetStrideH(),
        loadDataParams.GetFilterW(), loadDataParams.GetFilterH(), loadDataParams.GetDilationFilterW(),
        loadDataParams.GetDilationFilterH(), loadDataParams.GetFilterSizeW(), loadDataParams.GetFilterSizeH(),
        loadDataParams.GetTranspose(), loadDataParams.GetFMatrixCtrl(), loadDataParams.GetChannelSize());
#else
    if constexpr (std::is_same<T, bfloat16_t>::value) {
        img2colv2_cbuf_to_ca(
            (__ca__ half*)dst, (__cbuf__ half*)src, loadDataParams.GetConfig0(), loadDataParams.GetConfig1());
    } else {
        img2colv2_cbuf_to_ca(dst, src, loadDataParams.GetConfig0(), loadDataParams.GetConfig1());
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load3DBitModeParam& loadDataParams)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.GetKExtension(), loadDataParams.GetMExtension(), loadDataParams.GetKStartPt(),
        loadDataParams.GetMStartPt(), loadDataParams.GetStrideW(), loadDataParams.GetStrideH(),
        loadDataParams.GetFilterW(), loadDataParams.GetFilterH(), loadDataParams.GetDilationFilterW(),
        loadDataParams.GetDilationFilterH(), loadDataParams.GetFilterSizeW(), loadDataParams.GetFilterSizeH(),
        loadDataParams.GetTranspose(), loadDataParams.GetFMatrixCtrl(), loadDataParams.GetChannelSize());
#else
    if constexpr (std::is_same<T, bfloat16_t>::value) {
        img2colv2_cbuf_to_cb(
            (__cb__ half*)dst, (__cbuf__ half*)src, loadDataParams.GetConfig0(), loadDataParams.GetConfig1());
    } else {
        img2colv2_cbuf_to_cb(dst, src, loadDataParams.GetConfig0(), loadDataParams.GetConfig1());
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(__ubuf__ T* dst, __cbuf__ T* src, const Load3DBitModeParam& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12UB is not supported on current device"); });
}
/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0A is not supported 3DParamsV2Pro on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0B is not supported 3DParamsV2Pro on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12UB is not supported 3DParamsV2Pro on current device"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
