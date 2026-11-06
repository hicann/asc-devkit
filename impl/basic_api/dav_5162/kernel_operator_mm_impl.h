/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5162/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_IMPL_H

#include "../../../include/basic_api/kernel_operator_mm_bitmode_intf.h"
#include "../../../include/basic_api/kernel_struct_macro.h"
#include "kernel_utils.h"
#include "kernel_operator_mm_bitmode_intf.h"
#include "kernel_operator_sys_var_intf.h"
#include "kernel_struct_aipp.h"
#include "kernel_struct_waipp.h"

namespace AscendC {
/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d from L1 to L0A Buffer(A2) on current device"); });
}
template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d from L1 to L0B Buffer(B2) on current device"); });
}
/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d from L1 to L0A Buffer(A2) on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    static_assert(
        SupportType<
            T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t, half,
            bfloat16_t, uint16_t, int16_t>(),
        "LoadData 2dv2 L1 to L0B Buffer(B2) only support int4b_t, "
        "fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t, half, "
        "bfloat16_t, uint16_t, int16_t on current device!");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
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
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
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
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d_v2 from L1 to L0A Buffer(A2) on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    static_assert(
        SupportType<
            T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t, half,
            bfloat16_t, uint16_t, int16_t>(),
        "LoadData 2dv2 only support  int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, fp8_e5m2_t, fp8_e4m3fn_t, "
        "hifloat8_t, half, bfloat16_t, uint16_t, int16_t on current device!");
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
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
    __cbuf__ T* dst, __gm__ U* src, const LoadData2DParamsV2& loadDataParam, const Nd2NzParamsV2& nd2nzParams,
    const uint8_t sid = 12)
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
        loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, sid, decompressMode, 0);
}

template <typename T, typename U = T>
__aicore__ inline void LoadData2DL12L0ACal(
    __ca__ U* dst, __cbuf__ T* src0, __cbuf__ fp8_e8m0_t* src1, const LoadData2DParamsV2& loadDataParam,
    const LoadData2DMxParams& loadMxDataParams)
{
#if ASCENDC_CPU_DEBUG
    uint64_t l0ABaseAddr =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().GetHardwareBaseAddr(Hardware::L0A)));
    uint64_t bufferOffset = (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) - l0ABaseAddr) / 8;
    uint64_t mxDstAddr =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0AMx)) + bufferOffset;
#else
    uint64_t mxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 8;
#endif
    load_cbuf_to_ca_mx(
        mxDstAddr, static_cast<__cbuf__ void*>(src1), loadMxDataParams.xStartPosition, loadMxDataParams.yStartPosition,
        loadMxDataParams.xStep, loadMxDataParams.yStep, loadMxDataParams.srcStride, loadMxDataParams.dstStride);
}

template <typename T, typename U = T>
__aicore__ inline void LoadData2DL12L0BCal(
    __cb__ U* dst, __cbuf__ T* src0, __cbuf__ fp8_e8m0_t* src1, const LoadData2DParamsV2& loadDataParam,
    const LoadData2DMxParams& loadMxDataParams)
{
    LoadData2DL12L0BCal(dst, src0, loadDataParam);
#if ASCENDC_CPU_DEBUG
    uint64_t l0BBaseAddr =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().GetHardwareBaseAddr(Hardware::L0B)));
    uint64_t bufferOffset = (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) - l0BBaseAddr) / 8;
    uint64_t mxDstAddr =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0BMx)) + bufferOffset;
#else
    uint64_t mxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 8;
#endif
    load_cbuf_to_cb_mx(
        mxDstAddr, static_cast<__cbuf__ void*>(src1), loadMxDataParams.xStartPosition, loadMxDataParams.yStartPosition,
        loadMxDataParams.xStep, loadMxDataParams.yStep, loadMxDataParams.srcStride, loadMxDataParams.dstStride);
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(
    __ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d_v2 from gm to L0A Buffer(A2) on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(
    __cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const uint8_t cacheMode = 0,
    const uint8_t sid = 5)
{
    set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        load_gm_to_cb_2dv2_s4(
            (__cb__ void*)dst, (__gm__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, sid, 0, cacheMode);
    } else {
        load_gm_to_cb_2dv2(
            dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.dstStride,
            loadDataParam.mStep, loadDataParam.kStep, sid, 0, cacheMode);
    }
}

template <typename T, typename U>
__aicore__ inline void LoadData2DGM2L0BCal(
    __cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const Nd2NzParamsV2& nd2nzParams,
    const uint8_t sid = 5)
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
    load_gm_to_cb_2dv2(
        reinterpret_cast<__cbuf__ int8_t*>(dst), reinterpret_cast<__gm__ int8_t*>(src), loadDataParam.mStartPosition,
        loadDataParam.kStartPosition, loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, sid,
        decompressMode, 0);
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(
    __cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam, const uint8_t cacheMode = 0,
    const uint8_t sid = 12)
{
    static_assert(
        SupportType<
            T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half,
            bfloat16_t, uint16_t, int16_t, float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support int4b_t fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, "
        "fp8_e4m3fn_t, "
        "half, bfloat16_t, uint16_t, int16_t, float, int32_t, uint32_t on current device!");
    set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        load_gm_to_cbuf_2dv2_s4(
            (__cbuf__ void*)dst, (__gm__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, sid, 0, cacheMode);
    } else {
        load_gm_to_cbuf_2dv2(
            dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.dstStride,
            loadDataParam.mStep, loadDataParam.kStep, sid, 0, cacheMode);
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L1Cal(__cbuf__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    static_assert(
        SupportType<
            T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t, half,
            bfloat16_t, uint16_t, int16_t>(),
        "LoadData 2dv2 only support  int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, fp8_e5m2_t, fp8_e4m3fn_t, "
        "hifloat8_t, half, bfloat16_t, uint16_t, int16_t on current device!");
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cbuf_s4(
                (__cbuf__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_cbuf_s4(
                (__cbuf__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    } else {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cbuf(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_cbuf(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    }
}

/* **************************************************************************************************
 * LoadDataWithTranspose                                        *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "LoadDataWithTranspose from L1 to L0A Buffer(A2) is not supported on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "LoadDataWithTranspose from L1 to L0B Buffer(B2) is not supported on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "LoadDataWithTranspose from L1 to L0A Buffer(A2) is not supported on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "LoadDataWithTranspose from L1 to L0B Buffer(B2) is not supported on current device");
    });
}

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, const MmadParams& mmadParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Mmad is not supported on current device"); });
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Mmad is not supported on current device"); });
}

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "BroadCastVecToMM is not supported on current device"); });
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    if constexpr (sizeof(T) == 2) {
        T tmpValue = initConstValueParams.initValue;
        half initValue = (half&)tmpValue;
        set_l0_set_value_h(initValue);
        int64_t config = 0;
        config |= (uint64_t)initConstValueParams.repeatTimes;
        config |= (uint64_t)initConstValueParams.blockNum << 16;
        config |= (uint64_t)initConstValueParams.dstGap << 32;
        set_l1_2d((__cbuf__ half*)dst, config);
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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Load3DSetFMatrix is not supported on current device"); });
}

__aicore__ inline void Load3DSetFMatrixCal(uint64_t regFMatrix)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Load3DSetFMatrix is not supported on current device"); });
}

__aicore__ inline void Load3DSetFMatrixBCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Load3DSetFMatrixB is not supported on current device"); });
}

__aicore__ inline void Load3DSetFMatrixBCal(uint64_t regFMatrix)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Load3DSetFMatrixB is not supported on current device"); });
}

template <typename T>
__aicore__ inline void Load3DSetPaddingCal(const T padValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Load3DSetPadding is not supported on current device"); });
}

__aicore__ inline void SetLoadDataRepeatWithStrideCal(const LoadDataRepeatParamWithStride repeatParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "SetLoadDataRepeatWithStride is not supported on current device"); });
}

__aicore__ inline void SetLoadDataRepeatCal(const LoadDataRepeatParam repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetLoadDataRepeat is not supported on current device"); });
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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0A is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0B is not supported on current device"); });
}

__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0A is not supported on current device"); });
}

__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0B is not supported on current device"); });
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
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0AWithStride is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BWithStrideCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0BWithStride is not supported on current device"); });
}

__aicore__ inline void LoadData3DV2L12L0AWithStrideCal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0AWithStride is not supported on current device"); });
}

__aicore__ inline void LoadData3DV2L12L0BWithStrideCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0BWithStride is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBWithStrideCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12UBWithStride is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(__ca__ T* dst, __cbuf__ T* src, const Load3DBitModeParam& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0A is not supported on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load3DBitModeParam& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12L0B is not supported on current device"); });
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

/* **************************************************************************************************
 * Aipp/Waipp                                                                                        *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadImageToLocalCal(__cbuf__ T* dst, const LoadImageToLocalParams& params, const uint8_t sid = 6)
{
    static_assert(
        (SupportType<T, uint8_t, int8_t, int16_t>()),
        "LoadImageToLocal only support uint8_t/int8_t/int16_t on current device");
    load_image_to_cbuf(
        dst, static_cast<uint16_t>(params.horizSize - 1), static_cast<uint16_t>(params.vertSize - 1),
        params.horizStartPos, params.vertStartPos, static_cast<uint16_t>(params.srcHorizSize - 1), params.topPadSize,
        params.botPadSize, params.leftPadSize, params.rightPadSize, sid);
}

template <typename T, typename U>
__aicore__ inline void StoreImageToGlobalCal(
    __gm__ T* dst, __cbuf__ U* src, const StoreImageToGlobalParams& params, const uint8_t sid = 3)
{
    static_assert(
        (SupportType<T, uint8_t, int16_t>()), "StoreImageToGlobal only support uint8_t/int16_t on current device");
    store_l1_to_out_image(
        (__gm__ void*)dst, src, static_cast<uint16_t>(params.dstImageWidth - 1),
        static_cast<uint8_t>(params.outputDataType), static_cast<uint16_t>(params.dstImageHeight - 1),
        static_cast<uint8_t>(params.channelTransformMode), params.dstImageStride, sid, params.dtcParams.roundingMode,
        params.dtcParams.dtcOffset, params.dtcParams.dtcGain, params.srcImageStride, params.cOffset);
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
