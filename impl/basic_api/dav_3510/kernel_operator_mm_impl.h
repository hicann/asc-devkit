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
    "impl/basic_api/dav_3510/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_struct_aipp.h"
#include "../../../include/basic_api/kernel_struct_mm.h"
#include "../../../include/basic_api/kernel_operator_mm_bitmode_intf.h"
#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"

namespace AscendC {
/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(
    __ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d from GM to L0A Buffer(A2) on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(
    __cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_2d from GM to L0B Buffer(B2) on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(
    __cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam, const uint8_t cacheMode = 0)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, float, int32_t, uint32_t on "
        "current device!");
    if ASCEND_IS_AIC {
        uint16_t mStartPosition = 0;
        uint16_t kStartPosition = loadDataParam.startIndex;
        uint8_t mStep = loadDataParam.srcStride;
        uint8_t kStep = loadDataParam.repeatTimes;
        int16_t srcStride = static_cast<int16_t>(loadDataParam.srcStride);
        uint16_t dstStride = loadDataParam.dstGap + 1;
        set_mte2_src_para(uint64_t(srcStride));
        load_gm_to_cbuf_2dv2(
            dst, src, mStartPosition, kStartPosition, dstStride, mStep, kStep, loadDataParam.sid, 0, cacheMode);
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, float, int32_t, uint32_t on "
        "current device!");
    if ASCEND_IS_AIC {
        uint16_t mStartPosition = 0;
        uint16_t kStartPosition = loadDataParam.startIndex;
        uint8_t kStep = loadDataParam.repeatTimes;
        int16_t srcStride = static_cast<int16_t>(loadDataParam.srcStride);
        uint16_t dstStride = loadDataParam.dstGap + 1;

        if (loadDataParam.ifTranspose) {
            load_cbuf_to_ca(dst, src, mStartPosition, kStartPosition, 1, kStep, srcStride, dstStride, 1);
        } else {
            load_cbuf_to_ca(dst, src, mStartPosition, kStartPosition, 1, kStep, srcStride, dstStride, 0);
        }
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, float, int32_t, uint32_t on "
        "current device!");
    if ASCEND_IS_AIC {
        uint16_t mStartPosition = 0;
        uint16_t kStartPosition = loadDataParam.startIndex;
        uint8_t kStep = loadDataParam.repeatTimes;
        int16_t srcStride = static_cast<int16_t>(loadDataParam.srcStride);
        uint16_t dstStride = loadDataParam.dstGap + 1;

        if (loadDataParam.ifTranspose) {
            for (uint8_t i = 0; i < kStep; i++) {
                const uint64_t srcFractalIndex =
                    static_cast<uint64_t>(kStartPosition) + static_cast<uint64_t>(i) * loadDataParam.srcStride;
                load_cbuf_to_cb(
                    dst + static_cast<uint64_t>(i) * VALUE_512 / sizeof(T),
                    src + srcFractalIndex * VALUE_512 / sizeof(T), mStartPosition, 0, 1, 1, srcStride, dstStride, 1);
            }
        } else {
            load_cbuf_to_cb(dst, src, mStartPosition, kStartPosition, 1, kStep, srcStride, dstStride, 0);
        }
    }
}
/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    static_assert(
        SupportType<
            T, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t,
            float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
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
#else
        if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat4x2_t>()) {
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
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    static_assert(
        SupportType<
            T, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t,
            float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
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
#else
        if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat4x2_t>()) {
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
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    static_assert(
        SupportType<
            T, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t,
            float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            if (loadDataParam.ifTranspose) {
                load_cbuf_to_ca_s4(
                    dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                    loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
            } else {
                load_cbuf_to_ca_s4(
                    dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                    loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
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
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    static_assert(
        SupportType<
            T, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t,
            float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            if (loadDataParam.ifTranspose) {
                load_cbuf_to_cb_s4(
                    dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                    loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
            } else {
                load_cbuf_to_cb_s4(
                    dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                    loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
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
    if ASCEND_IS_AIC {
        load_cbuf_to_cb_transpose(
            dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride,
            loadDataParam.dstGap, inc, loadDataParam.dstFracGap, loadDataParam.srcFracGap);
    }
}

template <typename T, typename U = T, typename V = fp8_e8m0_t>
__aicore__ inline void LoadData2DL12L0ACal(
    __ca__ U* dst, __cbuf__ T* src0, __cbuf__ V* src1, const LoadData2DParamsV2& loadDataParam,
    const LoadData2DMxParams& loadMxDataParams)
{
    static_assert(
        SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>() ||
            SupportType<Tuple<U, T>, Tuple<mx_fp8_e4m3_t, fp8_e4m3fn_t>, Tuple<mx_fp8_e5m2_t, fp8_e5m2_t>>() ||
            SupportType<Tuple<U, T>, Tuple<fp8_e4m3fn_t, fp8_e4m3fn_t>, Tuple<fp8_e5m2_t, fp8_e5m2_t>>(),
        "LoadData 2dv2 with scale matrix only support fp4/fp8 dtype on current device!");
    if ASCEND_IS_AIC {
        if constexpr (SupportType<U, mx_fp8_e4m3_t, mx_fp8_e5m2_t, fp8_e5m2_t, fp8_e4m3fn_t>()) {
            if (loadDataParam.ifTranspose) {
                load_cbuf_to_ca(
                    reinterpret_cast<__ca__ T*>(dst), (__cbuf__ T*)src0, loadDataParam.mStartPosition,
                    loadDataParam.kStartPosition, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride,
                    loadDataParam.dstStride, 1);
            } else {
                load_cbuf_to_ca(
                    reinterpret_cast<__ca__ T*>(dst), (__cbuf__ T*)src0, loadDataParam.mStartPosition,
                    loadDataParam.kStartPosition, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride,
                    loadDataParam.dstStride, 0);
            }
        } else {
            if (loadDataParam.ifTranspose) {
                load_cbuf_to_ca_s4(
                    (__ca__ T*)dst, (__cbuf__ T*)src0, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                    loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
            } else {
                load_cbuf_to_ca_s4(
                    (__ca__ T*)dst, (__cbuf__ T*)src0, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                    loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
            }
        }
#if ASCENDC_CPU_DEBUG
        uint64_t l0ABaseAddr = static_cast<uint64_t>(
            reinterpret_cast<uintptr_t>(ConstDefiner::Instance().GetHardwareBaseAddr(Hardware::L0A)));
        uint64_t bufferOffset = (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) - l0ABaseAddr) / 16;
        uint64_t mxDstAddr =
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0AMx)) + bufferOffset;
#else
        uint64_t mxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
#endif
        load_cbuf_to_ca_mx(
            mxDstAddr, static_cast<__cbuf__ void*>(src1), loadMxDataParams.xStartPosition,
            loadMxDataParams.yStartPosition, loadMxDataParams.xStep, loadMxDataParams.yStep, loadMxDataParams.srcStride,
            loadMxDataParams.dstStride);
    }
}

template <typename T, typename U = T, typename V = fp8_e8m0_t>
__aicore__ inline void LoadData2DL12L0BCal(
    __cb__ U* dst, __cbuf__ T* src0, __cbuf__ V* src1, const LoadData2DParamsV2& loadDataParam,
    const LoadData2DMxParams& loadMxDataParams)
{
    static_assert(
        SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>() ||
            SupportType<Tuple<U, T>, Tuple<mx_fp8_e4m3_t, fp8_e4m3fn_t>, Tuple<mx_fp8_e5m2_t, fp8_e5m2_t>>() ||
            SupportType<Tuple<U, T>, Tuple<fp8_e4m3fn_t, fp8_e4m3fn_t>, Tuple<fp8_e5m2_t, fp8_e5m2_t>>(),
        "LoadData 2dv2 with scale matrix only support fp4/fp8 dtype on current device!");
    if ASCEND_IS_AIC {
        if constexpr (SupportType<U, mx_fp8_e4m3_t, mx_fp8_e5m2_t, fp8_e5m2_t, fp8_e4m3fn_t>()) {
            if (loadDataParam.ifTranspose) {
                load_cbuf_to_cb(
                    reinterpret_cast<__cb__ T*>(dst), (__cbuf__ T*)(src0), loadDataParam.mStartPosition,
                    loadDataParam.kStartPosition, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride,
                    loadDataParam.dstStride, 1);
            } else {
                load_cbuf_to_cb(
                    reinterpret_cast<__cb__ T*>(dst), (__cbuf__ T*)(src0), loadDataParam.mStartPosition,
                    loadDataParam.kStartPosition, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride,
                    loadDataParam.dstStride, 0);
            }
        } else {
            if (loadDataParam.ifTranspose) {
                load_cbuf_to_cb_s4(
                    (__cb__ T*)dst, (__cbuf__ T*)src0, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                    loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
            } else {
                load_cbuf_to_cb_s4(
                    (__cb__ T*)dst, (__cbuf__ T*)src0, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                    loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
            }
        }

#if ASCENDC_CPU_DEBUG
        uint64_t l0BBaseAddr = static_cast<uint64_t>(
            reinterpret_cast<uintptr_t>(ConstDefiner::Instance().GetHardwareBaseAddr(Hardware::L0B)));
        uint64_t bufferOffset = (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) - l0BBaseAddr) / 16;
        uint64_t mxDstAddr =
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0BMx)) + bufferOffset;
#else
        uint64_t mxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
#endif
        load_cbuf_to_cb_mx(
            mxDstAddr, static_cast<__cbuf__ void*>(src1), loadMxDataParams.xStartPosition,
            loadMxDataParams.yStartPosition, loadMxDataParams.xStep, loadMxDataParams.yStep, loadMxDataParams.srcStride,
            loadMxDataParams.dstStride);
    }
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
            T, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t,
            float, int32_t, uint32_t>(),
        "LoadData 2dv2 only support fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        set_mte2_src_para(uint64_t(loadDataParam.srcStride));
        if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            load_gm_to_cbuf_2dv2_s4(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.dstStride,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0, cacheMode);
        } else {
            load_gm_to_cbuf_2dv2(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.dstStride,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0, cacheMode);
        }
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

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, const MmadParams& mmadParams)
{
    if ASCEND_IS_AIC {
        bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, mx_fp8_e4m3_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e4m3_t, mx_fp8_e5m2_t>, Tuple<float, mx_fp8_e5m2_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e5m2_t, mx_fp8_e5m2_t>>();
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx(
                c, (__ca__ AType*)a, (__cb__ BType*)b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag,
                mmadParams.disableGemv, mmadParams.cmatrixSource, cmatrixInitVal);
        } else {
            mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag, mmadParams.disableGemv,
                mmadParams.cmatrixSource, cmatrixInitVal);
        }
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    if ASCEND_IS_AIC {
        bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, mx_fp8_e4m3_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e4m3_t, mx_fp8_e5m2_t>, Tuple<float, mx_fp8_e5m2_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e5m2_t, mx_fp8_e5m2_t>>();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx(
                (__cc__ DstT*)c, (__ca__ AType*)a, (__cb__ BType*)b, bias, mmadParams.m, mmadParams.k, mmadParams.n,
                mmadParams.unitFlag, mmadParams.disableGemv, cmatrixSource, cmatrixInitVal);
        } else {
            mad(c, a, b, bias, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag, mmadParams.disableGemv,
                cmatrixSource, cmatrixInitVal);
        }
#else
        // Xd[31:0]: matrix c addr in L0C, Xd[63:32]: bias addr in bias table buffer;
        uint64_t xd = ((uint64_t)c) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx(
                (__cc__ DstT*)xd, (__ca__ AType*)a, (__cb__ BType*)b, mmadParams.m, mmadParams.k, mmadParams.n,
                mmadParams.unitFlag, mmadParams.disableGemv, cmatrixSource, cmatrixInitVal);
        } else {
            mad((__cc__ DstT*)xd, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag,
                mmadParams.disableGemv, cmatrixSource, cmatrixInitVal);
        }
#endif
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, const MmadBitModeParams& mmadParams)
{
    if ASCEND_IS_AIC {
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, mx_fp8_e4m3_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e4m3_t, mx_fp8_e5m2_t>, Tuple<float, mx_fp8_e5m2_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e5m2_t, mx_fp8_e5m2_t>>();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx(
                c, (__ca__ AType*)a, (__cb__ BType*)b, mmadParams.GetM(), mmadParams.GetK(), mmadParams.GetN(),
                mmadParams.GetUnitFlag(), mmadParams.GetDisableGemv(), mmadParams.GetCmatrixSource(),
                mmadParams.GetCmatrixInitVal());
        } else {
            mad(c, a, b, mmadParams.GetM(), mmadParams.GetK(), mmadParams.GetN(), mmadParams.GetUnitFlag(),
                mmadParams.GetDisableGemv(), mmadParams.GetCmatrixSource(), mmadParams.GetCmatrixInitVal());
        }
#else
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx(c, (__ca__ AType*)a, (__cb__ BType*)b, mmadParams.GetConfig0());
        } else {
            mad(c, a, b, mmadParams.GetConfig0());
        }
#endif
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, MmadBitModeParams& mmadParams)
{
    if ASCEND_IS_AIC {
        mmadParams.SetCmatrixInitVal(false);
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, mx_fp8_e4m3_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e4m3_t, mx_fp8_e5m2_t>, Tuple<float, mx_fp8_e5m2_t, mx_fp8_e4m3_t>,
            Tuple<float, mx_fp8_e5m2_t, mx_fp8_e5m2_t>>();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx(
                (__cc__ DstT*)c, (__ca__ AType*)a, (__cb__ BType*)b, bias, mmadParams.GetM(), mmadParams.GetK(),
                mmadParams.GetN(), mmadParams.GetUnitFlag(), mmadParams.GetDisableGemv(), mmadParams.GetCmatrixSource(),
                mmadParams.GetCmatrixInitVal());
        } else {
            mad(c, a, b, bias, mmadParams.GetM(), mmadParams.GetK(), mmadParams.GetN(), mmadParams.GetUnitFlag(),
                mmadParams.GetDisableGemv(), mmadParams.GetCmatrixSource(), mmadParams.GetCmatrixInitVal());
        }
#else
        // Xd[31:0]: matrix c addr in L0C, Xd[63:32]: bias addr in bias table buffer;
        uint64_t xd = ((uint64_t)c) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        if constexpr (isMx) {
            using AType = typename GetDstType<Src0T>::Type;
            using BType = typename GetDstType<Src1T>::Type;
            mad_mx((__cc__ DstT*)xd, (__ca__ AType*)a, (__cb__ BType*)b, mmadParams.GetConfig0());
        } else {
            mad((__cc__ DstT*)xd, a, b, mmadParams.GetConfig0());
        }
#endif
    }
}
#endif

/* **************************************************************************************************
 * MmadMx                                                                                           *
 * **************************************************************************************************/
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadMxCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, const MmadParams& mmadParams)
{
    if ASCEND_IS_AIC {
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, fp8_e4m3fn_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e4m3fn_t, fp8_e5m2_t>, Tuple<float, fp8_e5m2_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e5m2_t, fp8_e5m2_t>>();
        static_assert(isMx, "MmadMx not support data type on current device!");
        bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
        mad_mx(
            c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag, mmadParams.disableGemv,
            mmadParams.cmatrixSource, cmatrixInitVal);
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadMxCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    if ASCEND_IS_AIC {
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, fp8_e4m3fn_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e4m3fn_t, fp8_e5m2_t>, Tuple<float, fp8_e5m2_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e5m2_t, fp8_e5m2_t>>();
        static_assert(isMx, "MmadMx not support data type on current device!");
        bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        mad_mx(
            c, a, b, bias, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag, mmadParams.disableGemv,
            cmatrixSource, cmatrixInitVal);
#else
        // Xd[31:0]: matrix c addr in L0C, Xd[63:32]: bias addr in bias table buffer;
        uint64_t xd = ((uint64_t)c) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad_mx(
            (__cc__ DstT*)xd, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag,
            mmadParams.disableGemv, cmatrixSource, cmatrixInitVal);
#endif
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadMxCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, MmadBitModeParams& mmadParams)
{
    if ASCEND_IS_AIC {
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, fp8_e4m3fn_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e4m3fn_t, fp8_e5m2_t>, Tuple<float, fp8_e5m2_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e5m2_t, fp8_e5m2_t>>();
        static_assert(isMx, "MmadMx not support data type on current device!");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        mad_mx(
            c, a, b, mmadParams.GetM(), mmadParams.GetK(), mmadParams.GetN(), mmadParams.GetUnitFlag(),
            mmadParams.GetDisableGemv(), mmadParams.GetCmatrixSource(), mmadParams.GetCmatrixInitVal());
#else
        mad_mx(c, a, b, mmadParams.GetConfig0());
#endif
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadMxCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, MmadBitModeParams& mmadParams)
{
    if ASCEND_IS_AIC {
        constexpr bool isMx = SupportType<
            Tuple<DstT, Src0T, Src1T>, Tuple<float, fp4x2_e1m2_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e1m2_t, fp4x2_e2m1_t>, Tuple<float, fp4x2_e2m1_t, fp4x2_e1m2_t>,
            Tuple<float, fp4x2_e2m1_t, fp4x2_e2m1_t>, Tuple<float, fp8_e4m3fn_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e4m3fn_t, fp8_e5m2_t>, Tuple<float, fp8_e5m2_t, fp8_e4m3fn_t>,
            Tuple<float, fp8_e5m2_t, fp8_e5m2_t>>();
        static_assert(isMx, "MmadMx not support data type on current device!");
        mmadParams.SetCmatrixInitVal(false);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        mad_mx(
            c, a, b, bias, mmadParams.GetM(), mmadParams.GetK(), mmadParams.GetN(), mmadParams.GetUnitFlag(),
            mmadParams.GetDisableGemv(), mmadParams.GetCmatrixSource(), mmadParams.GetCmatrixInitVal());
#else
        uint64_t xd = ((uint64_t)c) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad_mx((__cc__ DstT*)xd, a, b, mmadParams.GetConfig0());
#endif
    }
}
#endif

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
    if ASCEND_IS_AIC {
        int64_t repeatBit = (static_cast<uint64_t>(initConstValueParams.blockNum) << 16) |
                            (static_cast<uint64_t>(initConstValueParams.dstGap) << 32) |
                            initConstValueParams.repeatTimes;
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
            ASCENDC_ASSERT(false, {
                KERNEL_LOG(KERNEL_ERROR, "InitConstValue doesn't support current data type on current device");
            });
        }
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
    if ASCEND_IS_AIC {
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
}

__aicore__ inline void Load3DSetFMatrixCal(uint64_t regFMatrix)
{
    if ASCEND_IS_AIC {
        set_fmatrix(regFMatrix);
    }
}

__aicore__ inline void Load3DSetFMatrixBCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    if ASCEND_IS_AIC {
        uint64_t regFMatrix = 0;
        regFMatrix |= static_cast<uint64_t>(l1W);

        constexpr uint32_t l1HShiftBit = 16;
        regFMatrix |= static_cast<uint64_t>(l1H) << l1HShiftBit;

        constexpr uint32_t padNumber = 4;
        constexpr uint32_t padListShiftBit = 8;
        constexpr uint32_t padListShiftBase = 32;
        for (uint32_t i = 0; i < padNumber; i++) {
            regFMatrix |= uint64_t(padList[i] & 0xFF) << (padListShiftBase + i * padListShiftBit);
        }
        set_fmatrix_b(regFMatrix);
    }
}

__aicore__ inline void Load3DSetFMatrixBCal(uint64_t regFMatrix)
{
    if ASCEND_IS_AIC {
        set_fmatrix_b(regFMatrix);
    }
}

template <typename T>
__aicore__ inline void Load3DSetPaddingCal(const T padValue)
{
    uint16_t paddingValue = 0;
    constexpr uint16_t padValueShiftBit = 8;

    if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        paddingValue = static_cast<uint16_t>(GetScalarBitcodeValue((T)padValue));
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        paddingValue = static_cast<uint32_t>(GetScalarBitcodeValue((T)padValue));
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

    Internal::g_rptConfig = rptConfig;
}

template <typename T>
__aicore__ inline void SetLoadDataRepeatStride(const LoadData3DParamsV2<T>& loadDataParams)
{
    uint16_t dstStride = loadDataParams.enTranspose ? CeilDivision(loadDataParams.kExtension, BLOCK_CUBE) :
                                                      CeilDivision(loadDataParams.mExtension, BLOCK_CUBE);
    uint64_t rptConfig = Internal::g_rptConfig;
    constexpr uint32_t dstStrideShiftBit = 32;
    rptConfig |= uint64_t(dstStride) << dstStrideShiftBit;
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
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, uint16_t, int16_t, float,
            int32_t, uint32_t>(),
        "LoadData 3dv2 only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, uint16_t, int16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        SetLoadDataRepeatStride(loadDataParams);
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, uint16_t, int16_t, float,
            int32_t, uint32_t>(),
        "LoadData 3dv2 only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, uint16_t, int16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        SetLoadDataRepeatStride(loadDataParams);
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    if ASCEND_IS_AIC {
        SetLoadDataRepeatStride(loadDataParams);
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    if ASCEND_IS_AIC {
        SetLoadDataRepeatStride(loadDataParams);
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
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
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, uint16_t, int16_t, float,
            int32_t, uint32_t>(),
        "LoadData 3dv2 only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, uint16_t, int16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BWithStrideCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, uint16_t, int16_t, float,
            int32_t, uint32_t>(),
        "LoadData 3dv2 only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, uint16_t, int16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

__aicore__ inline void LoadData3DV2L12L0AWithStrideCal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

__aicore__ inline void LoadData3DV2L12L0BWithStrideCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
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
    if ASCEND_IS_AIC {
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
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load3DBitModeParam& loadDataParams)
{
    if ASCEND_IS_AIC {
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
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadData 3dv2Pro only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.extConfig, loadDataParams.extConfig >> LOAD_M_EXTENSION,
            loadDataParams.extConfig >> LOAD_K_START_POSITION, loadDataParams.extConfig >> LOAD_M_START_POSITION,
            loadDataParams.filterConfig, loadDataParams.filterConfig >> LOAD_STRIDE_H,
            loadDataParams.filterConfig >> LOAD_FILTER_W, loadDataParams.filterConfig >> LOAD_FILTER_H,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.filterSizeW,
            loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
            loadDataParams.channelSize);
    }
}

template <>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_ca(
            (__ca__ half*)dst, (__cbuf__ half*)src, loadDataParams.extConfig,
            loadDataParams.extConfig >> LOAD_M_EXTENSION, loadDataParams.extConfig >> LOAD_K_START_POSITION,
            loadDataParams.extConfig >> LOAD_M_START_POSITION, loadDataParams.filterConfig,
            loadDataParams.filterConfig >> LOAD_STRIDE_H, loadDataParams.filterConfig >> LOAD_FILTER_W,
            loadDataParams.filterConfig >> LOAD_FILTER_H, loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.filterSizeW,
            loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
            loadDataParams.channelSize);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, float, int32_t, uint32_t>(),
        "LoadData 3dv2Pro only support uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, float, int32_t, uint32_t on current device!");
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.extConfig, loadDataParams.extConfig >> LOAD_M_EXTENSION,
            loadDataParams.extConfig >> LOAD_K_START_POSITION, loadDataParams.extConfig >> LOAD_M_START_POSITION,
            loadDataParams.filterConfig, loadDataParams.filterConfig >> LOAD_STRIDE_H,
            loadDataParams.filterConfig >> LOAD_FILTER_W, loadDataParams.filterConfig >> LOAD_FILTER_H,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.filterSizeW,
            loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
            loadDataParams.channelSize);
    }
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    if ASCEND_IS_AIC {
        img2colv2_cbuf_to_cb(
            (__cb__ half*)dst, (__cbuf__ half*)src, loadDataParams.extConfig,
            loadDataParams.extConfig >> LOAD_M_EXTENSION, loadDataParams.extConfig >> LOAD_K_START_POSITION,
            loadDataParams.extConfig >> LOAD_M_START_POSITION, loadDataParams.filterConfig,
            loadDataParams.filterConfig >> LOAD_STRIDE_H, loadDataParams.filterConfig >> LOAD_FILTER_W,
            loadDataParams.filterConfig >> LOAD_FILTER_H, loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.filterSizeW,
            loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
            loadDataParams.channelSize);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "LoadData3DV2L12UB is not supported 3DParamsV2Pro on current device"); });
}
/* **************************************************************************************************
 * LoadImageToLocal                                               *
 * ************************************************************************************************* */
template <uint32_t size = sizeof(uint8_t)>
struct ImageOutputDataType {
    using T = uint8_t;
};

template <>
struct ImageOutputDataType<sizeof(uint16_t)> {
    using T = uint16_t;
};

template <typename T>
__aicore__ inline void LoadImageToLocalCalLoop0(
    __gm__ T* dst, __gm__ uint8_t* src0, __gm__ uint8_t* src1, uint32_t& dstSize, AippInputFormat format,
    AippParams<T> config, const LoadImageToLocalParams& loadDataParams)
{
    uint16_t horizSize = loadDataParams.horizSize;
    uint16_t vertSize = loadDataParams.vertSize;
    uint16_t horizStartPos = loadDataParams.horizStartPos;
    uint16_t vertStartPos = loadDataParams.vertStartPos;
    uint16_t srcHorizSize = loadDataParams.srcHorizSize;
    uint8_t topPadSize = loadDataParams.topPadSize;
    uint8_t botPadSize = loadDataParams.botPadSize;
    uint16_t leftPadSize = loadDataParams.leftPadSize;
    uint16_t rightPadSize = loadDataParams.rightPadSize;

    // channel padding configs
    uint32_t cPaddingMode = config.cPaddingParams.cPaddingMode;
    T cPaddingValue = config.cPaddingParams.cPaddingValue;
    // single line copy configs
    bool isSingleLineCopy = config.singleLineParams.isSingleLineCopy;
    // swap channel configs
    bool isSwapRB = config.swapParams.isSwapRB;
    bool isSwapUV = config.swapParams.isSwapUV;
    bool isSwapAX = config.swapParams.isSwapAX;
    // color space conversion configs
    bool isEnableCsc = config.cscParams.isEnableCsc;
    int16_t cscMatrixR0C0 = config.cscParams.cscMatrixR0C0;
    int16_t cscMatrixR0C1 = config.cscParams.cscMatrixR0C1;
    int16_t cscMatrixR0C2 = config.cscParams.cscMatrixR0C2;
    int16_t cscMatrixR1C0 = config.cscParams.cscMatrixR1C0;
    int16_t cscMatrixR1C1 = config.cscParams.cscMatrixR1C1;
    int16_t cscMatrixR1C2 = config.cscParams.cscMatrixR1C2;
    int16_t cscMatrixR2C0 = config.cscParams.cscMatrixR2C0;
    int16_t cscMatrixR2C1 = config.cscParams.cscMatrixR2C1;
    int16_t cscMatrixR2C2 = config.cscParams.cscMatrixR2C2;
    uint8_t cscBiasOut0 = config.cscParams.cscBiasOut0;
    uint8_t cscBiasOut1 = config.cscParams.cscBiasOut1;
    uint8_t cscBiasOut2 = config.cscParams.cscBiasOut2;
    uint8_t cscBiasIn0 = config.cscParams.cscBiasIn0;
    uint8_t cscBiasIn1 = config.cscParams.cscBiasIn1;
    uint8_t cscBiasIn2 = config.cscParams.cscBiasIn2;
    // datatype conversion configs
    uint8_t dtcMeanCh0 = config.dtcParams.dtcMeanCh0;
    uint8_t dtcMeanCh1 = config.dtcParams.dtcMeanCh1;
    uint8_t dtcMeanCh2 = config.dtcParams.dtcMeanCh2;
    half dtcMinCh0 = config.dtcParams.dtcMinCh0;
    half dtcMinCh1 = config.dtcParams.dtcMinCh1;
    half dtcMinCh2 = config.dtcParams.dtcMinCh2;
    half dtcVarCh0 = config.dtcParams.dtcVarCh0;
    half dtcVarCh1 = config.dtcParams.dtcVarCh1;
    half dtcVarCh2 = config.dtcParams.dtcVarCh2;
    // data padding mode
    uint32_t paddingMode = config.paddingParams.paddingMode;

    int16_t cscMatrix[AIPP_OUTPUT_CHANNEL_NUM][AIPP_OUTPUT_CHANNEL_NUM] = {
        {cscMatrixR0C0, cscMatrixR0C1, cscMatrixR0C2},
        {cscMatrixR1C0, cscMatrixR1C1, cscMatrixR1C2},
        {cscMatrixR2C0, cscMatrixR2C1, cscMatrixR2C2}};
    uint8_t cscBiasOut[AIPP_OUTPUT_CHANNEL_NUM] = {cscBiasOut0, cscBiasOut1, cscBiasOut2};
    uint8_t dtcMean[AIPP_OUTPUT_CHANNEL_NUM] = {dtcMeanCh0, dtcMeanCh1, dtcMeanCh2};
    half dtcMin[AIPP_OUTPUT_CHANNEL_NUM] = {dtcMinCh0, dtcMinCh1, dtcMinCh2};
    half dtcVar[AIPP_OUTPUT_CHANNEL_NUM] = {dtcVarCh0, dtcVarCh1, dtcVarCh2};

    if (isSingleLineCopy) { // single line copy mode
        vertSize = 1;
    }
    if (paddingMode == 0) {
        topPadSize = 0;
        botPadSize = 0;
    }
    uint32_t cpaddingNum = AIPP_OUTPUT_PADDING_MODE0_CHANNEL_NUM;
    uint32_t channelNum = AIPP_OUTPUT_CHANNEL_NUM;
    if (cPaddingMode == 0) {
        cpaddingNum = GetDataBlockSizeInBytes() / sizeof(T);
    }
    if (!isEnableCsc && format == AippInputFormat::YUV400_U8) {
        channelNum = AIPP_YUV400_OUTPUT_CHANNEL_NUM;
    }
    uint32_t src0ChannelNum = channelNum;
    if (format == AippInputFormat::YUV420SP_U8 || format == AippInputFormat::YUV400_U8) {
        src0ChannelNum = AIPP_YUV420_SRC0_CHANNEL_NUM;
    } else if (format == AippInputFormat::XRGB8888_U8) {
        src0ChannelNum = AIPP_XRGB8888_OUTPUT_CHANNEL_NUM;
    }
    uint32_t height = vertSize + topPadSize + botPadSize;
    uint32_t width = horizSize + leftPadSize + rightPadSize;
    dstSize = height * width * cpaddingNum;
    for (uint32_t n = 0; n < dstSize; n += 1) {
        uint32_t i = n / (width * cpaddingNum);
        uint32_t j = n % (width * cpaddingNum) / cpaddingNum;
        uint32_t k = n % cpaddingNum;
        if (k >= channelNum) { // channel padding
            dst[n] = cPaddingValue;
        } else {
            if (i >= topPadSize && i < topPadSize + vertSize && j >= leftPadSize && j < horizSize + leftPadSize) {
                uint32_t indexI = 0;
                uint32_t indexJ = 0;
                uint8_t dstVal = 0;
                if (isEnableCsc) { // color space conversion
                    if (format == AippInputFormat::RGB888_U8 || format == AippInputFormat::XRGB8888_U8) {
                        indexI = vertStartPos + i - topPadSize;
                        indexJ = horizStartPos + j - leftPadSize;
                        uint32_t src0Index = indexI * srcHorizSize * src0ChannelNum + indexJ * src0ChannelNum;
                        uint8_t channelRVal;
                        uint8_t channelGVal;
                        uint8_t channelBVal;
                        if (format == AippInputFormat::XRGB8888_U8 && isSwapAX && isSwapRB) {
                            channelRVal = src0[src0Index + 3];
                            channelGVal = src0[src0Index + 2];
                            channelBVal = src0[src0Index + 1];
                        } else if (format == AippInputFormat::XRGB8888_U8 && isSwapAX) {
                            channelRVal = src0[src0Index + 1];
                            channelGVal = src0[src0Index + 2];
                            channelBVal = src0[src0Index + 3];
                        } else if (isSwapRB) {
                            channelRVal = src0[src0Index + 2];
                            channelGVal = src0[src0Index + 1];
                            channelBVal = src0[src0Index];
                        } else {
                            channelRVal = src0[src0Index];
                            channelGVal = src0[src0Index + 1];
                            channelBVal = src0[src0Index + 2];
                        }
                        dstVal = (channelRVal * cscMatrix[k][0] + channelGVal * cscMatrix[k][1] +
                                  channelBVal * cscMatrix[k][2] + 128) /
                                     256 +
                                 cscBiasOut[k];
                    } else if (format == AippInputFormat::YUV420SP_U8 || format == AippInputFormat::YUV400_U8) {
                        indexI = vertStartPos + i - topPadSize;
                        indexJ = horizStartPos + j - leftPadSize;
                        uint32_t src0Index = indexI * srcHorizSize * src0ChannelNum + indexJ * src0ChannelNum;
                        uint8_t channelYVal = src0[src0Index];
                        uint8_t channelUVal = 0;
                        uint8_t channelVVal = 0;
                        if (format == AippInputFormat::YUV420SP_U8) {
                            indexI = vertStartPos / 2 + (i - topPadSize) / 2;
                            indexJ = horizStartPos + (j - leftPadSize) / 2 * 2;
                            uint32_t src1Index = indexI * srcHorizSize + indexJ;
                            if (isSwapUV) {
                                channelUVal = src1[src1Index + 1];
                                channelVVal = src1[src1Index];
                            } else {
                                channelUVal = src1[src1Index];
                                channelVVal = src1[src1Index + 1];
                            }
                        }
                        dstVal = ((channelYVal - cscBiasIn0) * cscMatrix[k][0] +
                                  (channelUVal - cscBiasIn1) * cscMatrix[k][1] +
                                  (channelVVal - cscBiasIn2) * cscMatrix[k][2] + 128) /
                                 256;
                    }
                } else {
                    // swap channel
                    if (isSwapAX && isSwapRB && format == AippInputFormat::XRGB8888_U8) { // XGRB -> BGRX
                        k = channelNum - k;
                    } else if (isSwapUV && format == AippInputFormat::YUV420SP_U8) { // YUV -> YVU
                        k = k > 0 ? channelNum - k : k;
                    } else if (isSwapAX && format == AippInputFormat::XRGB8888_U8) { // XRGB -> RGBX
                        k = k + 1;
                    } else if (
                        isSwapRB && format == AippInputFormat::RGB888_U8 ||
                        (isSwapRB && format == AippInputFormat::XRGB8888_U8)) { // RGB(X) -> BGR(X)
                        k = channelNum - 1 - k;
                    }
                    if (format == AippInputFormat::YUV420SP_U8 && k != 0) {
                        indexI = vertStartPos / 2 + (i - topPadSize) / 2;
                        indexJ = horizStartPos + (j - leftPadSize) / 2 * 2;
                        uint32_t src1Index = indexI * srcHorizSize + indexJ + k - 1;
                        dstVal = src1[src1Index];
                    } else {
                        indexI = vertStartPos + i - topPadSize;
                        indexJ = horizStartPos + j - leftPadSize;
                        uint32_t src0Index = indexI * srcHorizSize * src0ChannelNum + indexJ * src0ChannelNum + k;
                        dstVal = src0[src0Index];
                    }
                }
                if constexpr (IsSameType<T, int8_t>::value) { // datatype conversion
                    dst[n] = static_cast<T>(dstVal - dtcMean[k]);
                } else if constexpr (IsSameType<T, half>::value) {
                    dst[n] = (static_cast<T>(dstVal - dtcMean[k]) - dtcMin[k]) * dtcVar[k];
                } else if constexpr (IsSameType<T, uint8_t>::value) {
                    dst[n] = dstVal;
                }
            }
        }
    }
}

template <typename T>
__aicore__ inline void LoadImageToLocalCalLoop1(
    __gm__ T* dst, uint32_t dstSize, AippInputFormat format, AippParams<T> config,
    const LoadImageToLocalParams& loadDataParams)
{
    uint16_t horizSize = loadDataParams.horizSize;
    uint16_t vertSize = loadDataParams.vertSize;
    uint16_t horizStartPos = loadDataParams.horizStartPos;
    uint16_t vertStartPos = loadDataParams.vertStartPos;
    uint16_t srcHorizSize = loadDataParams.srcHorizSize;
    uint8_t topPadSize = loadDataParams.topPadSize;
    uint8_t botPadSize = loadDataParams.botPadSize;
    uint16_t leftPadSize = loadDataParams.leftPadSize;
    uint16_t rightPadSize = loadDataParams.rightPadSize;

    // data padding configs
    uint32_t paddingMode = config.paddingParams.paddingMode;
    T paddingValueCh0 = config.paddingParams.paddingValueCh0;
    T paddingValueCh1 = config.paddingParams.paddingValueCh1;
    T paddingValueCh2 = config.paddingParams.paddingValueCh2;
    T paddingValueCh3 = config.paddingParams.paddingValueCh3;
    T paddingValue[4] = {paddingValueCh0, paddingValueCh1, paddingValueCh2, paddingValueCh3};
    // single line copy configs
    bool isSingleLineCopy = config.singleLineParams.isSingleLineCopy;
    // channel padding mode
    uint32_t cPaddingMode = config.cPaddingParams.cPaddingMode;
    // color space conversion enabled or not
    bool isEnableCsc = config.cscParams.isEnableCsc;

    if (isSingleLineCopy) { // single line copy mode
        vertSize = 1;
    }
    if (paddingMode == 0) {
        topPadSize = 0;
        botPadSize = 0;
    }
    uint32_t cpaddingNum = AIPP_OUTPUT_PADDING_MODE0_CHANNEL_NUM;
    uint32_t channelNum = AIPP_OUTPUT_CHANNEL_NUM;
    if (cPaddingMode == 0) {
        cpaddingNum = GetDataBlockSizeInBytes() / sizeof(T);
    }
    if (!isEnableCsc && format == AippInputFormat::YUV400_U8) {
        channelNum = AIPP_YUV400_OUTPUT_CHANNEL_NUM;
    }
    uint32_t height = vertSize + topPadSize + botPadSize;
    uint32_t width = horizSize + leftPadSize + rightPadSize;
    uint32_t paddingChannelNum = paddingMode == 0 ? 4 : channelNum;
    for (uint32_t n = 0; n < dstSize; n += 1) {
        uint32_t i = n / (width * cpaddingNum);
        uint32_t j = n % (width * cpaddingNum) / cpaddingNum;
        uint32_t k = n % cpaddingNum;
        // data padding
        if ((k < paddingChannelNum) &&
            (i < topPadSize || i >= topPadSize + vertSize || j < leftPadSize || j >= horizSize + leftPadSize)) {
            uint32_t indexI = 0;
            uint32_t indexJ = 0;
            if (paddingMode == 0) {
                dst[n] = paddingValue[k];
            } else if (paddingMode == 1) {
                if (i < topPadSize) {
                    indexI = topPadSize;
                } else if (i >= topPadSize && i < topPadSize + vertSize) {
                    indexI = i;
                } else {
                    indexI = topPadSize + vertSize - 1;
                }
                if (j < leftPadSize) {
                    indexJ = leftPadSize;
                } else if (j >= leftPadSize && j < horizSize + leftPadSize) {
                    indexJ = j;
                } else {
                    indexJ = leftPadSize + horizSize - 1;
                }
                uint32_t dstIndex = indexI * width * cpaddingNum + indexJ * cpaddingNum + k;
                dst[n] = dst[dstIndex];
            } else if (paddingMode == 2) {
                if (i < topPadSize) {
                    indexI = topPadSize + i;
                } else if (i >= topPadSize && i < topPadSize + vertSize) {
                    indexI = i;
                } else {
                    indexI = i - botPadSize;
                }
                if (j < leftPadSize) {
                    indexJ = leftPadSize + j;
                } else if (j >= leftPadSize && j < horizSize + leftPadSize) {
                    indexJ = j;
                } else {
                    indexJ = j - rightPadSize;
                }
                uint32_t dstIndex = indexI * width * cpaddingNum + indexJ * cpaddingNum + k;
                dst[n] = dst[dstIndex];
            } else {
                if (i < topPadSize) {
                    indexI = topPadSize * 2 - i - 1;
                } else if (i >= topPadSize && i < topPadSize + vertSize) {
                    indexI = i;
                } else {
                    indexI = i - (i - topPadSize - vertSize) * 2 - 1;
                }
                if (j < leftPadSize) {
                    indexJ = leftPadSize * 2 - j - 1;
                } else if (j >= leftPadSize && j < horizSize + leftPadSize) {
                    indexJ = j;
                } else {
                    indexJ = j - (j - leftPadSize - horizSize) * 2 - 1;
                }
                uint32_t dstIndex = indexI * width * cpaddingNum + indexJ * cpaddingNum + k;
                dst[n] = dst[dstIndex];
            }
        }
    }
}

static __gm__ uint8_t tmp[36 * 256 * 1024] = {0};

template <typename T>
__aicore__ inline void LoadImageToLocalCal(__cbuf__ T* dst, const LoadImageToLocalParams& loadDataParams)
{
    if ASCEND_IS_AIC {
        static_assert(
            SupportType<T, uint8_t, int8_t, half>(),
            "LoadImageToLocal only supports uint8_t, int8_t, half on current device!");

        auto src0 = (__gm__ uint8_t*)(Internal::g_aippSrc0 & 0xffffffffffff);
        auto src1 = (__gm__ uint8_t*)(Internal::g_aippSrc1 & 0xffffffffffff);
        AippInputFormat format = static_cast<AippInputFormat>((Internal::g_aippArgs >> AIPP_OFFSET_FORMAT) & 0x1f);
        // data padding configs
        uint32_t paddingMode = (Internal::g_aippArgs >> AIPP_OFFSET_PADDING_MODE) & 0x3;
        using ImageType = typename ImageOutputDataType<sizeof(T)>::T;
        ImageType tVal = Internal::g_aippPaddingVal & 0xffff;
        T paddingValueCh0 = (T&)tVal;
        tVal = (Internal::g_aippPaddingVal >> AIPP_OFFSET_CH1) & 0xffff;
        T paddingValueCh1 = (T&)tVal;
        tVal = (Internal::g_aippPaddingVal >> AIPP_OFFSET_CH2) & 0xffff;
        T paddingValueCh2 = (T&)tVal;
        tVal = (Internal::g_aippPaddingVal >> AIPP_OFFSET_CH3) & 0xffff;
        T paddingValueCh3 = (T&)tVal;
        AippPaddingParams<T> paddingParams = {
            paddingMode, paddingValueCh0, paddingValueCh1, paddingValueCh2, paddingValueCh3};

        // channel padding configs
        uint32_t cPaddingMode = (Internal::g_aippArgs >> AIPP_OFFSET_CPADDING_MODE) & 0x1;
        tVal = Internal::g_aippArgs & 0xffff;
        T cPaddingValue = (T&)tVal;
        AippChannelPaddingParams<T> cPaddingParams = {cPaddingMode, cPaddingValue};

        // single line copy configs
        bool isSingleLineCopy = (Internal::g_aippArgs >> AIPP_OFFSET_SINGLE_LINE) & 0x1;
        AippSingleLineParams singleLineParams = {isSingleLineCopy};

        // swap channel configs
        bool isSwapRB = (Internal::g_aippArgs >> AIPP_OFFSET_SWAP_RB) & 0x1;
        bool isSwapUV = (Internal::g_aippArgs >> AIPP_OFFSET_SWAP_UV) & 0x1;
        bool isSwapAX = (Internal::g_aippArgs >> AIPP_OFFSET_SWAP_AX) & 0x1;
        AippSwapParams swapParams = {isSwapRB, isSwapUV, isSwapAX};

        // color space conversion configs
        bool isEnableCsc = (Internal::g_aippSrc1 >> AIPP_OFFSET_CSC_ENABLE) & 0x1;
        int16_t cscMatrixR0C0 = static_cast<int16_t>((Internal::g_aippCscRc0 & 0xffff));
        int16_t cscMatrixR0C1 = static_cast<int16_t>((Internal::g_aippCscRc0 >> AIPP_OFFSET_CH1) & 0xffff);
        int16_t cscMatrixR0C2 = static_cast<int16_t>((Internal::g_aippCscRc0 >> AIPP_OFFSET_CH2) & 0xffff);
        int16_t cscMatrixR1C0 = static_cast<int16_t>((Internal::g_aippCscRc0 >> AIPP_OFFSET_CH3) & 0xffff);
        int16_t cscMatrixR1C1 = static_cast<int16_t>((Internal::g_aippCscRc1 & 0xffff));
        int16_t cscMatrixR1C2 = static_cast<int16_t>((Internal::g_aippCscRc1 >> AIPP_OFFSET_CH1) & 0xffff);
        int16_t cscMatrixR2C0 = static_cast<int16_t>((Internal::g_aippCscRc1 >> AIPP_OFFSET_CH2) & 0xffff);
        int16_t cscMatrixR2C1 = static_cast<int16_t>((Internal::g_aippCscRc1 >> AIPP_OFFSET_CH3) & 0xffff);
        int16_t cscMatrixR2C2 = static_cast<int16_t>((Internal::g_aippCscBias & 0xffff));
        uint8_t cscBiasOut0 = (Internal::g_aippCscBias >> AIPP_OFFSET_CSC_OUT_CH0) & 0xff;
        uint8_t cscBiasOut1 = (Internal::g_aippCscBias >> AIPP_OFFSET_CSC_OUT_CH1) & 0xff;
        uint8_t cscBiasOut2 = (Internal::g_aippCscBias >> AIPP_OFFSET_CSC_OUT_CH2) & 0xff;
        uint8_t cscBiasIn0 = (Internal::g_aippCscBias >> AIPP_OFFSET_CSC_IN_CH0) & 0xff;
        uint8_t cscBiasIn1 = (Internal::g_aippCscBias >> AIPP_OFFSET_CSC_IN_CH1) & 0xff;
        uint8_t cscBiasIn2 = (Internal::g_aippCscBias >> AIPP_OFFSET_CSC_IN_CH2) & 0xff;
        AippColorSpaceConvParams cscParams = {isEnableCsc,   cscMatrixR0C0, cscMatrixR0C1, cscMatrixR0C2,
                                              cscMatrixR1C0, cscMatrixR1C1, cscMatrixR1C2, cscMatrixR2C0,
                                              cscMatrixR2C1, cscMatrixR2C2, cscBiasIn0,    cscBiasIn1,
                                              cscBiasIn2,    cscBiasOut0,   cscBiasOut1,   cscBiasOut2};

        // datatype conversion configs
        uint8_t dtcMeanCh0 = Internal::g_aippDtcMean & 0xff;
        uint8_t dtcMeanCh1 = (Internal::g_aippDtcMean >> AIPP_OFFSET_CH1) & 0xff;
        uint8_t dtcMeanCh2 = (Internal::g_aippDtcMean >> AIPP_OFFSET_CH2) & 0xff;
        uint16_t u16Val = Internal::g_aippDtcMin & 0xffff;
        half dtcMinCh0 = (half&)u16Val;
        u16Val = (Internal::g_aippDtcMin >> AIPP_OFFSET_CH1) & 0xffff;
        half dtcMinCh1 = (half&)u16Val;
        u16Val = (Internal::g_aippDtcMin >> AIPP_OFFSET_CH2) & 0xffff;
        half dtcMinCh2 = (half&)u16Val;
        u16Val = Internal::g_aippDtcVar & 0xffff;
        half dtcVarCh0 = (half&)u16Val;
        u16Val = (Internal::g_aippDtcVar >> AIPP_OFFSET_CH1) & 0xffff;
        half dtcVarCh1 = (half&)u16Val;
        u16Val = (Internal::g_aippDtcVar >> AIPP_OFFSET_CH2) & 0xffff;
        half dtcVarCh2 = (half&)u16Val;
        AippDataTypeConvParams dtcParams = {dtcMeanCh0, dtcMeanCh1, dtcMeanCh2, dtcMinCh0, dtcMinCh1,
                                            dtcMinCh2,  dtcVarCh0,  dtcVarCh1,  dtcVarCh2, 0};

        AippParams<T> config = {paddingParams, swapParams, singleLineParams, dtcParams, cPaddingParams, cscParams};

        uint32_t dstSize;

        auto actual_tmp = reinterpret_cast<__gm__ PrimT<T>*>(tmp + GetBlockIdx() * 256 * 1024);

        LoadImageToLocalCalLoop0<T>(actual_tmp, src0, src1, dstSize, format, config, loadDataParams);

        LoadImageToLocalCalLoop1<T>(actual_tmp, dstSize, format, config, loadDataParams);

        dcci(
            reinterpret_cast<__gm__ uint64_t*>(actual_tmp), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

        struct DataCopyParams repeatParams;
        repeatParams.blockLen = dstSize / AscendCUtils::GetC0Count(sizeof(PrimT<T>));
        DataCopyGM2L1Impl(dst, actual_tmp, repeatParams);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
