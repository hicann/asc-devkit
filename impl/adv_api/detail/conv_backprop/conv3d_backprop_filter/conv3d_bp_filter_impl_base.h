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
 * \file conv3d_bp_filter_impl_base.h
 * \brief
 */

#ifndef CONV3D_BP_FILTER_IMPL_BASE_H
#define CONV3D_BP_FILTER_IMPL_BASE_H

#include "../../../../../include/adv_api/conv_backprop/common/conv3d_bp_config_base.h"
#include "conv3d_bp_filter_func.h"
#include "../common/conv3d_bp_util.h"
#include "../../../../basic_api/kernel_utils.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"

namespace ConvBackpropApi {
template <typename Intf, class Config_>
struct ConvBpFilterImpl {
public:
    using Config = Config_;

public:
    __aicore__ inline ConvBpFilterImpl() {}

    DECLARE_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, Init, Intf);
    DECLARE_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, SetInput, Intf);
    DECLARE_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, SetGradOutput, Intf);
    DECLARE_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, SetSingleShape, Intf);
    DECLARE_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, SetStartPosition, Intf);
    DECLARE_SYNC_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, Iterate, Intf);
    DECLARE_SYNC_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, IterateAll, Intf);
    DECLARE_SYNC_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, GetTensorC, Intf);
    DECLARE_IMPL(ConvBackpropFilterFunc, Config_, ConvBackpropFilterFunc, End, Intf);
    struct ContextData : public Config::ContextData {
        __aicore__ inline ContextData(){};
        DEFINE_STUCT_FIELD(AscendC::TPipe, pipe_);
        DEFINE_STUCT_FIELD(const TConv3DBpFilterTiling* __restrict, tiling_);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, l0cPing_, AscendC::TPosition::CO1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, l0cPong_, AscendC::TPosition::CO1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, a1Ping_, AscendC::TPosition::A1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, a1Pong_, AscendC::TPosition::A1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, b1Ping_, AscendC::TPosition::B1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, b1Pong_, AscendC::TPosition::B1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TBuf, l0aBuf_, AscendC::TPosition::A2);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TBuf, l0bBuf_, AscendC::TPosition::B2);
        DEFINE_STUCT_FIELD(uint64_t, curNL0Idx_);
        DEFINE_STUCT_FIELD(uint64_t, curNL1Idx_);
        DEFINE_STUCT_FIELD(uint64_t, stepKaRound);
        DEFINE_STUCT_FIELD(uint64_t, stepKbRound);
        DEFINE_STUCT_FIELD(uint64_t, hwO_);
        DEFINE_STUCT_FIELD(uint64_t, hwI_);
        DEFINE_STUCT_FIELD(uint64_t, mIter_);
        DEFINE_STUCT_FIELD(uint64_t, nIter_);
        DEFINE_STUCT_FIELD(uint64_t, kIter_);
        DEFINE_STUCT_FIELD(uint64_t, singleShapeHo_);
        DEFINE_STUCT_FIELD(uint64_t, singleShapeCin_);
        DEFINE_STUCT_FIELD(uint64_t, singleShapeCout_);
        DEFINE_STUCT_FIELD(uint64_t, dstL12L0aOffset_);
        DEFINE_STUCT_FIELD(uint64_t, srcL12L0aOffset_);
        DEFINE_STUCT_FIELD(uint64_t, srcL0aOffset_);
        DEFINE_STUCT_FIELD(uint64_t, srcL0bOffset_);
        DEFINE_STUCT_FIELD(uint64_t, dstL0cOffset_);
        DEFINE_STUCT_FIELD(int64_t, strideKernelDilationH);
        DEFINE_STUCT_FIELD(AscendC::MmadParams, mmad_);

        DEFINE_STUCT_FIELD(uint32_t, tailM_);
        DEFINE_STUCT_FIELD(uint32_t, tailN_);
        DEFINE_STUCT_FIELD(uint32_t, tailK_);
        DEFINE_STUCT_FIELD(uint32_t, curStepM_);
        DEFINE_STUCT_FIELD(uint32_t, curStepN_);
        DEFINE_STUCT_FIELD(uint32_t, curML0Idx_);
        DEFINE_STUCT_FIELD(uint32_t, curML1Idx_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseM_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseN_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseK_);
        DEFINE_STUCT_FIELD(uint32_t, baseMK_);
        DEFINE_STUCT_FIELD(uint32_t, baseKN_);
        DEFINE_STUCT_FIELD(uint32_t, baseMN_);
        DEFINE_STUCT_FIELD(uint32_t, kal1_);
        DEFINE_STUCT_FIELD(uint32_t, kbl1_);
        DEFINE_STUCT_FIELD(uint32_t, mal1_);
        DEFINE_STUCT_FIELD(uint32_t, nbl1_);
        DEFINE_STUCT_FIELD(uint32_t, hwK_);
        DEFINE_STUCT_FIELD(uint32_t, hoStartIdx_);
        DEFINE_STUCT_FIELD(int32_t, hiStartIdx_);
        DEFINE_STUCT_FIELD(uint32_t, bL1HiCopyLenPing);
        DEFINE_STUCT_FIELD(uint32_t, bL1HiCopyLenPong);
        DEFINE_STUCT_FIELD(uint32_t, bL1PadUpPing);
        DEFINE_STUCT_FIELD(uint32_t, bL1PadUpPong);
        DEFINE_STUCT_FIELD(uint32_t, curLoadKal1_);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
        DEFINE_STUCT_FIELD(AscendC::LoadData2DParamsV2, load2dv2_);
#endif
        using LoadData3DParamsV2SrcT = AscendC::LoadData3DParamsV2<typename Intf::SrcT>;
        DEFINE_STUCT_FIELD(LoadData3DParamsV2SrcT, load3dA_);
        DEFINE_STUCT_FIELD(LoadData3DParamsV2SrcT, load3dB_);
        DEFINE_STUCT_FIELD(uint8_t, l0aPingPongFlag_);
        DEFINE_STUCT_FIELD(uint8_t, l0bPingPongFlag_);
        DEFINE_STUCT_FIELD(uint8_t, l0cPingPongFlag_);
        DEFINE_STUCT_FIELD(uint8_t, useL0PingPong_);
        DEFINE_STUCT_FIELD(uint8_t, isFirstIter_);
        using LocalTensor = AscendC::LocalTensor<typename Intf::SrcT>;
        DEFINE_STUCT_FIELD(LocalTensor, cacheA1BufPing_);
        DEFINE_STUCT_FIELD(LocalTensor, cacheA1BufPong_);
        DEFINE_STUCT_FIELD(LocalTensor, cacheB1BufPing_);
        DEFINE_STUCT_FIELD(LocalTensor, cacheB1BufPong_);
        using GlobalTensor = AscendC::GlobalTensor<typename Intf::SrcT>;
        DEFINE_STUCT_FIELD(GlobalTensor, outBackPropGlobal_);
        DEFINE_STUCT_FIELD(GlobalTensor, fmapGlobal_);
    };
};

} // namespace ConvBackpropApi

#endif // CONV3D_BP_FILTER_IMPL_BASE_H
