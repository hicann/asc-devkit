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
    "impl/basic_api/dav_5161/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
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
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData A1 To A2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const Load2DBitModeParam& loadDataParam)
{
    static_assert(
        SupportType<
            T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half,
            bfloat16_t>(),
        "LoadData 2dv2 only support int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, uint8_t, int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, \
         half, bfloat16_t, on current device!");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if (loadDataParam.GetIfTranspose()) {
        load_cbuf_to_cb(
            dst, src, loadDataParam.GetMStartPosition(), loadDataParam.GetKStartPosition(), loadDataParam.GetMStep(),
            loadDataParam.GetKStep(), loadDataParam.GetSrcStride(), loadDataParam.GetDstStride(), 1);
    } else {
        load_cbuf_to_cb(
            dst, src, loadDataParam.GetMStartPosition(), loadDataParam.GetKStartPosition(), loadDataParam.GetMStep(),
            loadDataParam.GetKStep(), loadDataParam.GetSrcStride(), loadDataParam.GetDstStride(), 0);
    }
#else
    if constexpr (SupportType<T, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat4x2_t>()) {
        if (loadDataParam.GetIfTranspose()) {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), true);
        } else {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.GetConfig0(), loadDataParam.GetConfig1(), false);
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
#endif

/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData A1 To A2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParams)
{
    static_assert(
        SupportType<T, int4b_t, uint8_t, uint16_t, int8_t, int16_t, hifloat8_t, half, bfloat16_t>(),
        "LoadData 2dv2 only support int4b_t, uint8_t, uint16_t, int8_t, int16_t, hifloat8_t, half, bfloat16_t on "
        "current device!");

    if constexpr (IsSameType<T, int4b_t>::value) {
        load_cbuf_to_cb_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride, loadDataParams.dstStride, false);
    } else if (loadDataParams.ifTranspose) {
        load_cbuf_to_cb(
            (__cb__ T*)dst, (__cbuf__ T*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride, loadDataParams.dstStride, true);
    } else {
        load_cbuf_to_cb(
            (__cb__ T*)dst, (__cbuf__ T*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride, loadDataParams.dstStride, false);
    }
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData GM To A2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParams)
{
    if (loadDataParams.kStep != 1) {
        set_mte2_src_para(uint64_t(loadDataParams.srcStride));
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ half*)dst, (__gm__ half*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ float*)dst, (__gm__ float*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ half*)dst, (__gm__ half*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ float*)dst, (__gm__ float*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParams)
{
    if (loadDataParams.kStep != 1) {
        set_mte2_src_para(uint64_t(loadDataParams.srcStride));
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ float*)dst, (__gm__ float*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ float*)dst, (__gm__ float*)src, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
            loadDataParams.dstStride, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.sid, 0, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif
}

/* **************************************************************************************************
 * LoadDataWithTranspose                                        *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData A1 To A2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData A1 To A2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParams)
{
    // LoadData2dTransposeParams do not match hardware capabilities, not currently supported, will be added later
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData B1 To B2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData B1 To B2 is not supported!"); });
}

/* **************************************************************************************************
 * LoadDataWithMx                                        *
 * ************************************************************************************************* */

template <typename T, typename U = T, typename V = fp8_e8m0_t>
__aicore__ inline void LoadData2DL12L0ACal(
    __ca__ U* dst, __cbuf__ T* src0, __cbuf__ V* src1, const LoadData2DParamsV2& loadDataParams,
    const LoadData2DMxParams& loadMxDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData A1 To A2 is not supported!"); });
}

template <typename T, typename U = T, typename V = fp8_e8m0_t>
__aicore__ inline void LoadData2DL12L0BCal(
    __cb__ U* dst, __cbuf__ T* src0, __cbuf__ V* src1, const LoadData2DParamsV2& loadDataParams,
    const LoadData2DMxParams& loadMxDataParams)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current version is not supported!"); });
#else
    static_assert(
        SupportType<T, fp8_e4m3fn_t, int4b_t>() ||
            SupportType<Tuple<U, T>, Tuple<fp8_e4m3fn_t, fp8_e4m3fn_t>, Tuple<int4b_t, int4b_t> >(),
        "LoadData 2dv2 with scale matrix only support fp8_e4m3fn_t/int4b_t dtype on current device!");

    if constexpr (SupportType<U, fp8_e4m3fn_t>()) {
        if (loadDataParams.ifTranspose) {
            load_cbuf_to_cb(
                reinterpret_cast<__cb__ U*>(dst), (__cbuf__ T*)(src0), loadDataParams.mStartPosition,
                loadDataParams.kStartPosition, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride,
                loadDataParams.dstStride, true);
        } else {
            load_cbuf_to_cb(
                reinterpret_cast<__cb__ U*>(dst), (__cbuf__ T*)(src0), loadDataParams.mStartPosition,
                loadDataParams.kStartPosition, loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride,
                loadDataParams.dstStride, false);
        }
    } else {
        if (loadDataParams.ifTranspose) {
            load_cbuf_to_cb_s4(
                (__cb__ U*)dst, (__cbuf__ T*)src0, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
                loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride, loadDataParams.dstStride, true);
        } else {
            load_cbuf_to_cb_s4(
                (__cb__ U*)dst, (__cbuf__ T*)src0, loadDataParams.mStartPosition, loadDataParams.kStartPosition,
                loadDataParams.mStep, loadDataParams.kStep, loadDataParams.srcStride, loadDataParams.dstStride, false);
        }
    }

    uint64_t mxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 8;
    load_cbuf_to_cb_mx(
        mxDstAddr, static_cast<__cbuf__ void*>(src1), loadMxDataParams.xStartPosition, loadMxDataParams.yStartPosition,
        loadMxDataParams.xStep, loadMxDataParams.yStep, loadMxDataParams.srcStride, loadMxDataParams.dstStride);
#endif
}

/* **************************************************************************************************
 * LoadDataWithDecomp                                             *
 * ************************************************************************************************* */
__aicore__ inline void EnableS4ToS8Cal() { bisheng::cce::enable_s4_expand_s8_datacopy(); }

__aicore__ inline void EnableS3ToS8Cal() { bisheng::cce::enable_s3_expand_s8_datacopy(); }

template <typename T>
__aicore__ inline void LoadData2DGM2L0BDecompCal(
    __cb__ T* dst, __gm__ T* src, const LoadData2DDecompParamsV2& loadDataDecompParams)
{
    if (loadDataDecompParams.kStep != 1) {
        set_mte2_src_para(uint64_t(loadDataDecompParams.srcStride));
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ int8_t*)dst, (__gm__ int8_t*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ half*)dst, (__gm__ half*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ float*)dst, (__gm__ float*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ int8_t*)dst, (__gm__ int8_t*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ half*)dst, (__gm__ half*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ float*)dst, (__gm__ float*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1DecompCal(
    __cbuf__ T* dst, __gm__ T* src, const LoadData2DDecompParamsV2& loadDataDecompParams)
{
    if (loadDataDecompParams.kStep != 1) {
        set_mte2_src_para(uint64_t(loadDataDecompParams.srcStride));
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ float*)dst, (__gm__ float*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ float*)dst, (__gm__ float*)src, loadDataDecompParams.mStartPosition,
            loadDataDecompParams.kStartPosition, loadDataDecompParams.dstStride, loadDataDecompParams.mStep,
            loadDataDecompParams.kStep, loadDataDecompParams.sid, loadDataDecompParams.decomp, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
#endif
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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

__aicore__ inline void Load3DSetFMatrixCal(uint64_t regFMatrix)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

__aicore__ inline void Load3DSetFMatrixBCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

__aicore__ inline void Load3DSetFMatrixBCal(uint64_t regFMatrix)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 from l1 to ubuf"); });
}

/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 3D is not supported!"); });
}

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported broadcast from ub to l0c!"); });
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    if (static_cast<uint64_t>(initConstValueParams.repeatTimes) > 32767UL) {
        ASCENDC_ASSERT(
            (false), { KERNEL_LOG(KERNEL_ERROR, "InitConstValue repeatTimes out of range, max is 32767!"); });
    }
#endif

    if constexpr (2 == sizeof(T)) {
        T tmpValue = initConstValueParams.initValue;
        half initValue = *(half*)(&tmpValue);
        set_l0_set_value_h(initValue);
        int64_t config = 0;
        config |= (uint64_t)initConstValueParams.repeatTimes;
        config |= (uint64_t)initConstValueParams.blockNum << 16;
        config |= (uint64_t)initConstValueParams.dstGap << 32;
        set_l1_2d((__cbuf__ half*)dst, config);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported the data type!"); });
    }
}

/* **************************************************************************************************
 * InitL0ANzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0ANzMatrixCal(__ca__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "InitConstValue unsupported the position A2!"); });
}

/* **************************************************************************************************
 * InitL0BNzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0BNzMatrixCal(__cb__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "InitConstValue unsupported the position B2!"); });
}

/* **************************************************************************************************
 * SetLoadDataRepeat                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataRepeatCal(const LoadDataRepeatParam& repeatParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported SetLoadDataRepeat!"); });
}

__aicore__ inline void SetLoadDataRepeatBCal(const LoadDataRepeatParam& repeatParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported Right SetLoadDataRepeat"); });
}

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataBoundaryCal(uint32_t boundaryValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported SetLoadDataBoundary!"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
