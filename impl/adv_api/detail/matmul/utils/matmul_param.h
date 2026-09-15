/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file matmul_param.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/utils/matmul_param.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_PARAM_H__
#endif

#ifndef IMPL_MATMUL_UTILS_MATMUL_PARAM_H
#define IMPL_MATMUL_UTILS_MATMUL_PARAM_H

#include "../../../../basic_api/kernel_macros.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/matmul/tiling.h"
#include "../dfx/dfx_proxy.h"
#include "../resource/cube_in_buffer/global_cache.h"
#include "../../../tiling/matmul/matmul_constant_tiling_impl.h"
#include "matmul_type_def.h"
#include "matmul_utils.h"
#include "matmul_config_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/* **************************************************************************************************
 * MatmulParamsBase                                             *
 * ************************************************************************************************* */
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsBase {
    __aicore__ inline MatmulParamsBase(){};
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, typename = void>
struct MatmulParamsNorm : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsNorm(){};
    using SrcT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    TQue<TPosition::C1, QUEUE_DEPTH> qidBias_;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
    TQue<TPosition::A2, QUEUE_DEPTH> qidA2_;
    TQue<TPosition::B2, QUEUE_DEPTH> qidB2_;
#endif

    LocalTensor<BiasT> cacheHeadBias_; // Allocate and release using qidBias_

    SrcT aScalar_;
    SrcBT bScalar_;

    LocalTensor<TensorTrait<SrcT>> leftMatrix_;
    LocalTensor<TensorTrait<SrcBT>> rightMatrix_;
    LocalTensor<TensorTrait<BiasT>> inputBias_;

    __gm__ SrcT* aGlobal_;
    __gm__ SrcBT* bGlobal_;
    __gm__ BiasT* biasGlobal_;

    TPipe* tpipe_;

    // baseUseX_ is the same as baseX in most cases, while it will be smaller than baseX when dealing with tail cases
    // measured in element
    int baseUseM_;
    int baseUseK_;
    int baseUseN_;
    // measured in cube block
    int blockUseM_;
    int blockUseK_;
    int blockUseN_;

    // int32_t cacheProcA_, cacheProcB_;
    bool isFirstIter_;
    // whether enable bias, default value is false
    bool enableBias_;

    int tailM_, tailK_, tailN_;
    // current c matrix coordinate
    int curM_, curN_;
    // current c matrix step size, there could be tail steps
    int curStepM_, curStepN_;
    // current c matrix step block coordinate
    int stepMIdx_, stepNIdx_;

    bool enHF32Mode_;
    int32_t hf32TransMode_;

    int baseMN_;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsNorm<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG,
    enable_if_t<isNormEnableScheduler<A_TYPE, MM_CFG> || IsBasicBlockEnable<MM_CFG> || IsIntrablock<MM_CFG>>>
    : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsNorm(){};
    using SrcT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

    TPipe* tpipe_;

    int32_t hf32TransMode_;
    int baseMN_;

    bool enHF32Mode_;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsMDL : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsMDL(){};
    using SrcT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

    TPipe* tpipe_;

    bool enHF32Mode_;
    int32_t hf32TransMode_;

    int baseMN_;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsMDLSparse : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsMDLSparse(){};
    using SrcT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

    TPipe* tpipe_;

    bool enHF32Mode_;
    int32_t hf32TransMode_;

    int baseMN_;
};
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsMxNorm : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsMxNorm(){};
    using SrcBT = typename B_TYPE::T;
    using SrcT = typename A_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

    TPipe* tpipe_;

    bool enHF32Mode_;
    int32_t hf32TransMode_;

    int baseMN_;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsMxMDL : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsMxMDL(){};
    using SrcT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    using DstT = typename C_TYPE::T;

    TPipe* tpipe_;

    bool enHF32Mode_;
    int32_t hf32TransMode_;

    int baseMN_;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsBasicBlock : public MatmulParamsNorm<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    __aicore__ inline MatmulParamsBasicBlock(){};
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParamsIBShareNorm : public MatmulParamsBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> {
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    __aicore__ inline MatmulParamsIBShareNorm(){};
    using SrcT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
    TQue<TPosition::A2, QUEUE_DEPTH> qidA2_;
    TQue<TPosition::B2, QUEUE_DEPTH> qidB2_;
#endif

    TPipe* tpipe_;

    bool enHF32Mode_;
    int32_t hf32TransMode_;

    int baseMN_;
};

/* **************************************************************************************************
 * MatmulParams                                             *
 * ************************************************************************************************* */
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, MatmulMode MM_VER,
    class ENABLE_SPARSE = void>
struct MatmulParams {
    __aicore__ inline MatmulParams(){};
};

// CFG_NORM
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(CFG_NORM),
    enable_if_t<!HasScalePosition<A_TYPE>::value && !HasScalePosition<B_TYPE>::value>> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsNorm<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

// CFG_MDL
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(CFG_MDL),
    enable_if_t<!HasSparseIndex<B_TYPE>() && (!HasScalePosition<A_TYPE>::value && !HasScalePosition<B_TYPE>::value)>> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsMDL<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

// CFG_MDL_SPARSE
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(CFG_MDL),
    enable_if_t<
        HasSparseIndex<B_TYPE>() && DoMatmulMDL(MM_CFG) &&
        (!HasScalePosition<A_TYPE>::value && !HasScalePosition<B_TYPE>::value)>> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsMDLSparse<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

// MX_CFG_NORM
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(CFG_NORM),
    enable_if_t<DoMatmulNorm(MM_CFG) && HasScalePosition<A_TYPE>::value && HasScalePosition<B_TYPE>::value>> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsMxNorm<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

// MX_CFG_MDL
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(CFG_MDL),
    enable_if_t<HasScalePosition<A_TYPE>::value && HasScalePosition<B_TYPE>::value>> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsMxMDL<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

// MM_CFG_BB
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(MM_CFG_BB)> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsBasicBlock<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

// CFG_IBSHARE_NORM
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulParams<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, GetMatmulMode(CFG_IBSHARE_NORM)> {
    __aicore__ inline MatmulParams(){};
    using PARAMS = MatmulParamsIBShareNorm<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _MATMUL_PARAM_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_PARAM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_PARAM_H__
#endif
