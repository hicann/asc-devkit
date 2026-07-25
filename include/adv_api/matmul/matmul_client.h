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
 * \file matmul_client.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "matmul_client.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_CLIENT_H__
#endif

#ifndef LIB_MATMUL_MATMUL_CLIENT_H
#define LIB_MATMUL_MATMUL_CLIENT_H

#include "kernel_basic_intf.h"
#include "../../../impl/basic_api/utils/kernel_utils_constants.h"
#include "../../../impl/adv_api/detail/kfc/kfc_register_obj.h"
#include "include/adv_api/matmul/constant_tiling.h"
#include "include/adv_api/matmul/tiling.h"
#include "../../../impl/adv_api/detail/matmul/policy/matmul_policy.h"
#include "../../../impl/adv_api/detail/matmul/utils/matmul_call_back.h"
#include "../../../impl/adv_api/detail/matmul/utils/matmul_module.h"
#include "../../../impl/adv_api/detail/matmul/utils/matmul_utils.h"
#if ASCENDC_CPU_DEBUG
#include "../../../impl/adv_api/detail/matmul/kfc/matmul_server_aux.h"
#endif

namespace AscendC {

constexpr int32_t VECTOR_QUANT_MODE = 2;
constexpr int32_t VECTOR_QUANT_MODE_L1 = 3;
constexpr int32_t NUM_EIGHT = 8;
constexpr uint16_t NUM_SIXTEEN = 16;
constexpr uint16_t NUM_THIRTYTWO = 32;
constexpr uint16_t NUM_FORTYEIGHT = 48;

#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
/**
 * @class MatmulClientBase
 *
 * @brief Base class for MatmulClient
 *
 * Service function of matrix multiplication on the AIV client side, acting as the unit for message sending.
 */
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
class MatmulClientBase {
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

public:
#if defined(USE_SSBUF)
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.Init(cubeTiling, tpipe);
            }
#endif
            return;
        }
        ASSERT(sizeof(KfcMsg) % CACHE_LINE_SIZE == 0);
        ASSERT(cubeTiling != nullptr && "tiling cannot be nullptr when init matmul client");
        ASSERT(sizeof(TCubeTiling) % sizeof(uint64_t) == 0);
        // copy tiling to the last mem in ssbuf
        MSG_POS TilingInfo* tilingSSbuf = reinterpret_cast<MSG_POS TilingInfo*>(GetTilingAddr(GetSubBlockIdxImpl()));
        while (tilingSSbuf->valid) {
        }
        tilingSSbuf->valid = 1;
        auto tempTilingSSbuf = reinterpret_cast<MSG_POS uint64_t*>(&(tilingSSbuf->tCubeTiling));
        auto tempTiling = reinterpret_cast<uint64_t*>(const_cast<TCubeTiling*>(cubeTiling));
        for (int i = 0; i < sizeof(TCubeTiling) / sizeof(uint64_t); ++i, ++tempTilingSSbuf, ++tempTiling) {
            *tempTilingSSbuf = *tempTiling;
        }
        this->cubeTiling.SetTiling(cubeTiling);
        PostMessage<KFC_Enum::MMFUN_INIT, false>();
        nIter_ = ConstCeil(this->cubeTiling.GetSingleCoreN(), this->cubeTiling.GetBaseN());
        mIter_ = ConstCeil(this->cubeTiling.GetSingleCoreM(), this->cubeTiling.GetBaseM());
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            uint32_t kIter = ConstCeil(this->cubeTiling.GetSingleCoreK(), this->cubeTiling.GetBaseK());
            mnIter_ = nIter_ * mIter_ * kIter;
        } else {
            mnIter_ = nIter_ * mIter_;
        }
        cacheWorkspaceAddr = nullptr;
        singleCoreM_ = this->cubeTiling.GetSingleCoreM();
        singleCoreN_ = this->cubeTiling.GetSingleCoreN();
        singleCoreK_ = this->cubeTiling.GetSingleCoreK();
    }
#else
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.Init(cubeTiling, tpipe);
            }
#endif
            return;
        }
        ASSERT(sizeof(KfcMsg) % CACHE_LINE_SIZE == 0);
        ASSERT(cubeTiling != nullptr && "cubeTiling cannot be nullptr when init matmul client");
        ASSERT(sizeof(TCubeTiling) % sizeof(uint64_t) == 0);
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            if (GetSubBlockIdxImpl() == 1) {
                return;
            }
        }
        constexpr uint32_t tCubeTilingSize = ConstCeil(sizeof(TCubeTiling), CACHE_LINE_SIZE) * CACHE_LINE_SIZE;
        int32_t ubAddr = -1;
        GM_ADDR tilingGM = client->AllocUB(tCubeTilingSize, ubAddr);
        auto tempTilingGM = reinterpret_cast<__gm__ uint32_t*>(tilingGM);
        auto tempTiling = reinterpret_cast<uint32_t*>(const_cast<TCubeTiling*>(cubeTiling));
        for (int i = 0; i < sizeof(TCubeTiling) / sizeof(uint32_t); ++i, ++tempTilingGM, ++tempTiling) {
            *tempTilingGM = *tempTiling;
        }
        this->cubeTiling.SetTiling(cubeTiling);
        GlobalTensor<int64_t> global;
        for (int i = 0; i < tCubeTilingSize; i += CACHE_LINE_SIZE) {
            Barrier();
            global.SetGlobalBuffer((__gm__ int64_t*)(tilingGM + i));
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(global);
        }
        Barrier();

        auto msg = client->AllocMessage();
        client->ubMsg->tilingInfo.tilingAddr = tilingGM;
        client->ubMsg->head = KfcMsgMakeFlag(KFC_Enum::MMFUN_INIT, this->instIdx);
        client->ubMsg->ubAddr = ubAddr;
        client->PostMessage<false>(msg); // Initialize the local client after the expected processing is complete.

        *((uint64_t*)&kfcMsg_) = 0;
        *((uint64_t*)&(kfcMsg_.body)) = 0;
        nIter_ = ConstCeil(this->cubeTiling.GetSingleCoreN(), this->cubeTiling.GetBaseN());
        mIter_ = ConstCeil(this->cubeTiling.GetSingleCoreM(), this->cubeTiling.GetBaseM());
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            uint32_t kIter = ConstCeil(this->cubeTiling.GetSingleCoreK(), this->cubeTiling.GetBaseK());
            mnIter_ = nIter_ * mIter_ * kIter;
        } else {
            mnIter_ = nIter_ * mIter_;
        }
        cacheWorkspaceAddr = nullptr;
    }
#endif
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe = nullptr)
    {
        TCubeTiling cubeTiling;
        CopyTiling<A_TYPE, B_TYPE, MM_CFG>(gmCubeTiling, cubeTiling);
        Init(&cubeTiling, tpipe);
    }
#endif
    template <class T>
    __aicore__ inline void SetWorkspace(GlobalTensor<T>& addr)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "SetWorkspace not support when enableMixDualMaster is enabled");
        ASSERT(addr.GetSize() > 0);
        SetWorkspace(addr.GetPhyAddr(), addr.GetSize() * sizeof(T));
    }
    template <class T>
    __aicore__ inline void SetWorkspace(__gm__ const T* addr, int size)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "SetWorkspace not support when enableMixDualMaster is enabled");
        ASSERT(addr != nullptr);
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreM == 0) {
            ASSERT(!this->cubeTiling.IsNull());
        }

        cacheWorkspaceAddr = reinterpret_cast<GM_ADDR>(const_cast<__gm__ T*>(addr));
        cOffset_ = 0;
    }

    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetOrgShape(orgM, orgN, orgK);
            }
#endif
            return;
        }
        SetOrgShape(orgM, orgN, orgK, orgK, orgN);
    }

    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetOrgShape(orgM, orgN, orgKa, orgKb, orgKc);
            }
#endif
            return;
        }
#if defined(USE_SSBUF)
        kfcMsg_.body.orgM = orgM;
        kfcMsg_.body.orgN = orgN;
        kfcMsg_.body.orgKa = orgKa;
        kfcMsg_.body.orgKb = orgKb;
        kfcMsg_.body.orgKc = orgKc;
        kfcMsg_.body.setOrgShape = 1;
#else
        kfcMsg_.orgShape.orgM = orgM;
        kfcMsg_.orgShape.orgN = orgN;
        kfcMsg_.orgShape.orgKa = orgKa;
        kfcMsg_.orgShape.orgKb = orgKb;
        kfcMsg_.orgShape.orgKc = orgKc;
        PostMessage<KFC_Enum::MMFUN_SET_ORG_SHAPE, false>();
#endif
    }

    __aicore__ inline void SetSingleShape(int singleM, int singleN, int singleK)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetSingleShape(singleM, singleN, singleK);
            }
#endif
            return;
        }
        SetTail(singleM, singleN, singleK);
    }

    __aicore__ inline void SetTail(int tailM = -1, int tailN = -1, int tailK = -1)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetTail(tailM, tailN, tailK);
            }
#endif
            return;
        }
        if (tailM != -1) {
            mIter_ = ConstCeil(tailM, cubeTiling.GetBaseM());
        }
        if (tailN != -1) {
            nIter_ = ConstCeil(tailN, cubeTiling.GetBaseN());
        }
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            uint32_t singleCoreK = tailK != -1 ? tailK : cubeTiling.GetSingleCoreK();
            uint32_t kIter = ConstCeil(singleCoreK, cubeTiling.GetBaseK());
            mnIter_ = nIter_ * mIter_ * kIter;
        } else {
            mnIter_ = nIter_ * mIter_;
        }
#if defined(USE_SSBUF)
        singleCoreM_ = tailM != -1 ? tailM : singleCoreM_;
        singleCoreN_ = tailN != -1 ? tailN : singleCoreN_;
        singleCoreK_ = tailK != -1 ? tailK : singleCoreK_;
#endif
        kfcMsg_.body.singleM = tailM;
        kfcMsg_.body.singleN = tailN;
        kfcMsg_.body.singleK = tailK;
        kfcMsg_.body.setTail = 1;
    }

    // transMode only support 0 or 1
    // 0: round mode is round to the nearest tie to even
    // 1: round mode is round to the nearest tie away from zero
    __aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetHF32(enableHF32, transMode);
            }
#endif
            return;
        }
        kfcMsg_.body.enHF32 = enableHF32;
        kfcMsg_.body.hf32TransMode = transMode;

        PostMessage<KFC_Enum::MMFUN_SET_HF32, false>();
    }

#if defined(USE_SSBUF)
    __aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorA localTensor not support when enableMixDualMaster is enabled");
        ASSERT(
            isTransposeA <= A_TYPE::isTrans &&
            "It is not allowed to do A transpose when matmul A transpose is not defined.");
        kfcMsg_.body.isTransA = static_cast<uint32_t>(isTransposeA);
        kfcMsg_.body.setTensorA = 1;
        kfcMsg_.body.isFirstIter = 1;
        if constexpr (A_TYPE::pos == TPosition::TSCM) {
            auto tmpAddr = GetTscmAddr(leftMatrix);
            auto intraId = (reinterpret_cast<TBufType*>(leftMatrix.GetBufferHandle()))->enQueEvtID;
            // 8 bit for intraID, 32 bit for addr
            kfcMsg_.body.aAddr = (((uint64_t)intraId) << VALID_ADDR_BITS_NUM) + tmpAddr;
            sizeAmatrix_ = leftMatrix.GetSize() * sizeof(SrcAT);
        } else {
            MSG_POS MsgMatmulL1Addr* matmulL1AddrMsg =
                (MSG_POS MsgMatmulL1Addr*)GetMatmulL1AddrMsg(GetSubBlockIdxImpl(), this->instIdx);
            while (!(matmulL1AddrMsg->valid)) {
            }
            uint64_t aL1Addr = matmulL1AddrMsg->l1aAddr;
            kfcMsg_.body.aAddr = aL1Addr;
            sizeAmatrix_ = leftMatrix.GetSize() * sizeof(SrcAT);
            aAddr_ = (uint64_t)leftMatrix.GetPhyAddr();
        }
    }
#else
    __aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
            ASSERT("SetTensorA localTensor not support when enableMixDualMaster is enabled");
#endif
            return;
        }
        ASSERT(
            isTransposeA <= A_TYPE::isTrans &&
            "It is not allowed to do A transpose when matmul A transpose is not defined.");
        kfcMsg_.body.isTransA = static_cast<uint32_t>(isTransposeA);
        kfcMsg_.body.setTensorA = 1;
        kfcMsg_.body.isFirstIter = 1;
        if constexpr (A_TYPE::pos == TPosition::TSCM) {
            kfcMsg_.body.aAddr = GetTscmAddr(leftMatrix);
            kfcMsg_.body.sizeAmatrix = leftMatrix.GetSize() * sizeof(SrcAT);
        } else {
            kfcMsg_.body.aAddr = GetGlobalAddr<SrcAT, true>(leftMatrix);
            kfcMsg_.body.sizeAmatrix = leftMatrix.GetSize() * sizeof(SrcAT);
        }
    }
#endif

#if defined(USE_SSBUF)
    __aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorB localTensor not support when enableMixDualMaster is enabled");
        ASSERT(
            isTransposeB <= B_TYPE::isTrans &&
            "It is not allowed to do B transpose when matmul B transpose is not defined.");
        kfcMsg_.body.isTransB = static_cast<uint32_t>(isTransposeB);
        kfcMsg_.body.setTensorB = 1;
        kfcMsg_.body.isFirstIter = 1;

        if constexpr (B_TYPE::pos == TPosition::TSCM) {
            auto tmpAddr = GetTscmAddr(rightMatrix);
            auto intraId = (reinterpret_cast<TBufType*>(rightMatrix.GetBufferHandle()))->enQueEvtID;
            kfcMsg_.body.bAddr = (((uint64_t)intraId) << VALID_ADDR_BITS_NUM) + tmpAddr;
            sizeBmatrix_ = rightMatrix.GetSize() * sizeof(SrcBT);
        } else {
            MSG_POS MsgMatmulL1Addr* matmulL1AddrMsg =
                (MSG_POS MsgMatmulL1Addr*)GetMatmulL1AddrMsg(GetSubBlockIdxImpl(), this->instIdx);
            while (!(matmulL1AddrMsg->valid)) {
            }
            uint64_t bL1Addr = matmulL1AddrMsg->l1bAddr;
            kfcMsg_.body.bAddr = bL1Addr;
            sizeBmatrix_ = rightMatrix.GetSize() * sizeof(SrcBT);
            bAddr_ = (uint64_t)rightMatrix.GetPhyAddr();
        }
    }
#else
    __aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
            ASSERT("SetTensorB localTensor not support when enableMixDualMaster is enabled");
#endif
            return;
        }
        ASSERT(
            isTransposeB <= B_TYPE::isTrans &&
            "It is not allowed to do B transpose when matmul B transpose is not defined.");
        kfcMsg_.body.isTransB = static_cast<uint32_t>(isTransposeB);
        kfcMsg_.body.setTensorB = 1;
        kfcMsg_.body.isFirstIter = 1;

        if constexpr (B_TYPE::pos == TPosition::TSCM) {
            kfcMsg_.body.bAddr = GetTscmAddr(rightMatrix);
            kfcMsg_.body.sizeBmatrix = rightMatrix.GetSize() * sizeof(SrcBT);
        } else {
            kfcMsg_.body.bAddr = GetGlobalAddr<SrcBT, true>(rightMatrix);
            kfcMsg_.body.sizeBmatrix = rightMatrix.GetSize() * sizeof(SrcBT);
        }
    }
#endif

#if defined(USE_SSBUF)
    __aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetBias localTensor not support when enableMixDualMaster is enabled");
        kfcMsg_.body.setTensorBias = 1;
        kfcMsg_.body.isFirstIter = 1;
        if constexpr (BIAS_TYPE::pos == TPosition::TSCM) {
            kfcMsg_.body.biasAddr = GetTscmAddr(inputBias);
            sizeBiasmatrix_ = inputBias.GetSize() * sizeof(BiasT);
        } else {
            MSG_POS MsgMatmulL1Addr* matmulL1AddrMsg =
                (MSG_POS MsgMatmulL1Addr*)GetMatmulL1AddrMsg(GetSubBlockIdxImpl(), this->instIdx);
            while (!(matmulL1AddrMsg->valid)) {
            }
            kfcMsg_.body.biasAddr = matmulL1AddrMsg->l1biasAddr;
            biasAddr_ = (uint64_t)inputBias.GetPhyAddr();
            sizeBiasmatrix_ = inputBias.GetSize() * sizeof(BiasT);
        }
    };
#else
    __aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
            ASSERT("SetBias localTensor not support when enableMixDualMaster is enabled");
#endif
            return;
        }
        kfcMsg_.body.setTensorBias = 1;
        if constexpr (BIAS_TYPE::pos == TPosition::TSCM) {
            kfcMsg_.body.biasAddr = GetTscmAddr(inputBias);
        } else {
            kfcMsg_.body.biasAddr = GetGlobalAddr<BiasT, true>(inputBias);
        }
    };
#endif

    __aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetTensorA(gm, isTransposeA);
            }
#endif
            return;
        }
#if defined(USE_SSBUF)
        static_assert(
            (GetPhyType(A_TYPE::pos) == Hardware::GM),
            "SetTensorA GlobalTensor not support when A_TYPE position is not GM");
#endif
        ASSERT(
            isTransposeA <= A_TYPE::isTrans &&
            "It is not allowed to do A transpose when matmul A transpose is not defined.");
        kfcMsg_.body.isTransA = static_cast<uint32_t>(isTransposeA);
        kfcMsg_.body.setTensorA = 1;
        kfcMsg_.body.isFirstIter = 1;
#if defined(USE_SSBUF)
        kfcMsg_.body.aAddr = reinterpret_cast<uint64_t>(gm.address_);
        sizeAmatrix_ = gm.GetSize() * sizeof(SrcAT);
#else
        kfcMsg_.body.aAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.sizeAmatrix = gm.GetSize() * sizeof(SrcAT);
#endif
    }

    __aicore__ inline void SetTensorB(const GlobalTensor<SrcBT>& gm, bool isTransposeB = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetTensorB(gm, isTransposeB);
            }
#endif
            return;
        }
#if defined(USE_SSBUF)
        static_assert(
            (GetPhyType(B_TYPE::pos) == Hardware::GM),
            "SetTensorB GlobalTensor not support when B_TYPE position is not GM");
#endif
        ASSERT(
            isTransposeB <= B_TYPE::isTrans &&
            "It is not allowed to do B transpose when matmul B transpose is not defined.");
        kfcMsg_.body.isTransB = static_cast<uint32_t>(isTransposeB);
        kfcMsg_.body.setTensorB = 1;
        kfcMsg_.body.isFirstIter = 1;
#if defined(USE_SSBUF)
        kfcMsg_.body.bAddr = reinterpret_cast<uint64_t>(gm.address_);
        sizeBmatrix_ = gm.GetSize() * sizeof(SrcBT);
#else
        kfcMsg_.body.bAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.sizeBmatrix = gm.GetSize() * sizeof(SrcBT);
#endif
    }

#if defined(USE_SSBUF)
    template <class T>
    __aicore__ inline void SetSelfDefineData(T dataPtr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetSelfDefineData(dataPtr);
            }
#endif
            return;
        }
        ASSERT(sizeof(T) % 4 == 0);
        uint32_t* ptr = reinterpret_cast<uint32_t*>(&dataPtr);
        if constexpr (sizeof(T) == 4) {
            kfcMsg_.userCustomData = *ptr;
            kfcMsg_.body.userInfoType = 1;
        } else if constexpr (sizeof(T) == 8) {
            kfcMsg_.userCustomData = (*ptr);
            kfcMsg_.body.userCustomData = *(ptr + 1);
            kfcMsg_.body.userInfoType = 1;
        } else {
            // send msg
            uint32_t* ptrMsg = reinterpret_cast<uint32_t*>(&(kfcMsg_.body));
            for (int i = 0; i < sizeof(T) / sizeof(uint32_t); i++) {
                *(ptrMsg + i) = *(ptr + i);
            }
            PostMessage<KFC_Enum::MMFUN_SET_USER_DEF_INFO, false>();
        }
    }
#else
    __aicore__ inline void SetSelfDefineData(const uint64_t dataPtr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetSelfDefineData(dataPtr);
            }
#endif
            return;
        }
        kfcMsg_.body.dataPtr = dataPtr;
    }
#endif

    __aicore__ inline void SetUserDefInfo(const uint64_t tilingPtr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetUserDefInfo(tilingPtr);
            }
#endif
            return;
        }
        kfcMsg_.userDefInfo.tilingPtr = tilingPtr;
#if defined(USE_SSBUF)
        kfcMsg_.userCustomData = 1;
#endif
        PostMessage<KFC_Enum::MMFUN_SET_USER_DEF_INFO, false>();
    }

    __aicore__ inline void SetSparseIndex(const GlobalTensor<uint8_t>& indexGlobal)
    {
        ASSERT("SetSparseIndex is not supported in matmul client.");
        return;
    }

    __aicore__ inline void SetQuantScalar(const uint64_t quantScalar)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetQuantScalar(quantScalar);
            }
#endif
            return;
        }
        kfcMsg_.body.setQuant = 1;
        kfcMsg_.body.quantMode = 1;
        kfcMsg_.body.quantScalar = quantScalar;
    }

    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetQuantVector(quantTensor);
            }
#endif
            return;
        }
        kfcMsg_.body.setQuant = 1;
        kfcMsg_.body.quantMode = VECTOR_QUANT_MODE;
        kfcMsg_.body.quantAddr = reinterpret_cast<uint64_t>(quantTensor.GetPhyAddr());
        kfcMsg_.body.quantSize = quantTensor.GetSize() * sizeof(uint64_t);
    }

    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetQuantVector localTensor is not supported when enableMixDualMaster is enabled.");
        kfcMsg_.body.setQuant = 1;
        kfcMsg_.body.quantMode = VECTOR_QUANT_MODE_L1;
        kfcMsg_.body.quantAddr = GetTscmAddr(quantTensor);
        kfcMsg_.body.quantSize = quantTensor.GetSize() * sizeof(uint64_t);
    }

    __aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetBias(biasGlobal);
            }
#endif
            return;
        }
        kfcMsg_.body.biasAddr = reinterpret_cast<uint64_t>(biasGlobal.GetPhyAddr());
        kfcMsg_.body.setTensorBias = 1;
    }

    __aicore__ inline void SetTensorA(SrcAT aScalar)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetTensorA(aScalar);
            }
#endif
            return;
        }
        auto temp1 = (uint8_t*)&(aScalar);
        auto temp2 = reinterpret_cast<uint8_t*>(&(kfcMsg_.body.aAddr));

        for (int i = 0; i < sizeof(SrcAT); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        kfcMsg_.body.setTensorA = 1;
    }

    __aicore__ inline void SetTensorB(SrcBT bScalar)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.SetTensorB(bScalar);
            }
#endif
            return;
        }
        auto temp1 = (uint8_t*)&(bScalar);
        auto temp2 = reinterpret_cast<uint8_t*>(&(kfcMsg_.body.aAddr));

        for (int i = 0; i < sizeof(SrcBT); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        kfcMsg_.body.setTensorB = 1;
    }

    __aicore__ inline void DisableBias()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.DisableBias();
            }
#endif
            return;
        }
        kfcMsg_.body.setTensorBias = 0;
    }

    __aicore__ inline void ClearBias()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.ClearBias();
            }
#endif
            return;
        }
        DisableBias();
    }

    __aicore__ inline void End()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            return;
        }
        if (isSyncGetC) {
            PostMessage<KFC_Enum::MMFUN_END, false>();
        }
    }

    template <bool sync = true, typename T>
    __aicore__ inline bool Iterate(bool enPartialSum, const LocalTensor<T>& localCmatrix)
    {
        return false;
    }

    template <bool sync = true>
    __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster, "Iterate not support when enableMixDualMaster is enabled.");
#if !defined(USE_SSBUF)
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            ASSERT(false && "Iterate not support when sameab is enabled");
            return false;
        }
#endif
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        if (unlikely(kfcMsg_.body.isFirstIter)) {
            cntIter_ = 0;
            cOffset_ = 0;
            curProcess = 0;
        } else {
            if (++cntIter_ >= mnIter_) {
                TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
                return false;
            }
            if constexpr (!sync) {
                TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
                return true;
            }
        }

        if constexpr (!sync) { // Asynchronous mode. Only UB.
#if defined(USE_WORKSPACE)
            ASSERT(cacheWorkspaceAddr != 0); // The cache address must be configured in asynchronous mode.
            ASSERT(PhyPosIsUB(C_TYPE::pos)); // Asynchronous mode. Only UB.
#endif
        }

        isSyncGetC = sync;

        // Synchronous mode. no cache for the first time
        kfcMsg_.body.enPartialSum = enPartialSum;
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(cacheWorkspaceAddr);
#if defined(USE_SSBUF)
        kfcMsg_.body.hasSetWorkspace = (cacheWorkspaceAddr != 0);
        PrepareABFromGM();
        const bool isTransA = kfcMsg_.body.isTransA; // kfcMsg body will be reset after postMessage
        const bool isTransB = kfcMsg_.body.isTransB;
        const bool isTransScaleA = kfcMsg_.body.quantMode & 0b01;
        const bool isTransScaleB = (kfcMsg_.body.quantMode >> 1) & 0b01;
        const bool isBias = kfcMsg_.body.setTensorBias;
#endif
        PostMessage<KFC_Enum::MMFUN_ITERATE, false>();
        SyncCubeWithVec<A_TYPE::ibShare, B_TYPE::ibShare>();
#if defined(USE_SSBUF)
        // wait and copy data from UB->L1
        PrepareABFromUb(isTransA, isTransB, isBias, isTransScaleA, isTransScaleB);
#endif
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
        return true;
    }

    // Only support the mode that the IterateAll is asynchronous and GM output is continuous.
    // In discontinuous scenarios, the system stops responding.
    __aicore__ inline void WaitIterateAll()
    {
        ASSERT(!isSyncGetC); // Must be asynchronous mode
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                return;
            }
#endif
            WaitEvent(this->instIdx);
            return;
        }
#if defined(USE_SSBUF)
        if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
        } else {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
        }
#else
        auto intraId = this->devEvtID;
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            if (GetSubBlockIdxImpl() == 1) {
                intraId = this->devEvtID - 1;
            }
        }
        WaitEvent(intraId);
#endif
    }

    // Only support the mode that the IterateAll is asynchronous and GM output is continuous.
    // In discontinuous scenarios, the system stops responding.
    __aicore__ inline void WaitIterateBatch()
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "WaitIterateBatch not support when enableMixDualMaster is enabled");
        ASSERT(!isSyncGetC); // Must be asynchronous mode
#if defined(USE_SSBUF)
        CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
#else
        WaitEvent(this->devEvtID);
#endif
    }

#if defined(USE_SSBUF)
    template <bool sync = true>
    __aicore__ inline void IterateAll(
        const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false, bool waitIterateAll = false,
        bool fakeMsg = false)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), {
            KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput.");
        });
        static_assert(
            !(ToMatmulConfig(MM_CFG).enableMixDualMaster && !(A_TYPE::ibShare && B_TYPE::ibShare)),
            "IBShare in A/BTYPE should be true when enableMixDualMaster is enabled.");
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                cubeObj.cubeObj[0].mul.IterateAll(gm, enAtomic, enSequentialWrite, waitIterateAll, fakeMsg);
                if (sync || waitIterateAll) {
                    CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                        cubeObj.cubeObj[0].instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), 0U));
                    if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                        CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                            cubeObj.cubeObj[0].instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP),
                            1U)); // 1 means sub_block 1
                    }
                }
                cubeObj.cubeObj[0].mul.End();
                return;
            }
#endif
            PrepareABFromGM();
            if constexpr (sync) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
            }
            return;
        }

        if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) {
            cntIter_ = 0; // input from ub only copy once
        }
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        kfcMsg_.body.iterateFakeMsg = fakeMsg;
        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.waitIterateAll = waitIterateAll;

        PrepareABFromGM();
        const bool isTransA = kfcMsg_.body.isTransA; // kfcMsg body will be reset after postMsg
        const bool isTransB = kfcMsg_.body.isTransB;
        const bool isTransScaleA = kfcMsg_.body.quantMode & 0b01;
        const bool isTransScaleB = (kfcMsg_.body.quantMode >> 1) & 0b01;
        const bool isBias = kfcMsg_.body.setTensorBias;
        PostMessage<KFC_Enum::MMFUN_ITERATE_ALL, sync>();
        PrepareABFromUb(isTransA, isTransB, isBias, isTransScaleA, isTransScaleB);
        if constexpr (sync) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }

    template <bool sync = true>
    __aicore__ inline void IterateAll(
        const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0, bool enSequentialWrite = false,
        bool waitIterateAll = false)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), {
            KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput.");
        });
        static_assert(
            !(ToMatmulConfig(MM_CFG).enableMixDualMaster && !(A_TYPE::ibShare && B_TYPE::ibShare)),
            "IBShare in A/BTYPE should be true when enableMixDualMaster is enabled.");
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            IterateAllCPU(ubCmatrix, enAtomic, enSequentialWrite, waitIterateAll);
            PrepareABFromGM();
            CrossCoreSetFlag<INTRA_MODE, PIPE_V>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + this->instIdx);
            if constexpr (sync) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
            }
            return;
        }
        ASSERT(enAtomic == 0);
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        if (ubCmatrix.GetPosition() == static_cast<int32_t>(TPosition::TSCM)) {
            kfcMsg_.body.cAddr = GetTscmAddr(ubCmatrix);
            kfcMsg_.body.cIsTscm = 1;
        } else {
            kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(ubCmatrix.GetPhyAddr());
        }
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.waitIterateAll = waitIterateAll;
        if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) {
            cntIter_ = 0;
        }
        ASSERT(kfcMsg_.body.enSequentialWrite == 0);
        PrepareABFromGM();
        const bool isTransA = kfcMsg_.body.isTransA;
        const bool isTransB = kfcMsg_.body.isTransB;
        const bool isTransScaleA = kfcMsg_.body.quantMode & 0b01;
        const bool isTransScaleB = (kfcMsg_.body.quantMode >> 1) & 0b01;
        const bool isBias = kfcMsg_.body.setTensorBias;
        CrossCoreSetFlag<INTRA_MODE, PIPE_V>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + this->instIdx);
        PostMessage<KFC_Enum::MMFUN_ITERATE_ALL, sync>();
        PrepareABFromUb(isTransA, isTransB, isBias, isTransScaleA, isTransScaleB);
        if constexpr (sync) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }
#else
    template <bool sync = true>
    __aicore__ inline void IterateAll(
        const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false, bool waitIterateAll = false,
        bool fakeMsg = false)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), {
            KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput.");
        });
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            constexpr uint16_t eventID = 9U;
#if ASCENDC_CPU_DEBUG
            if ASCEND_IS_AIC {
                WaitEvent(eventID);
                cubeObj.cubeObj[0].mul.IterateAll(gm, enAtomic, enSequentialWrite, waitIterateAll, fakeMsg);
                if (sync || waitIterateAll) {
                    NotifyEvent<PIPE_FIX>(cubeObj.cubeObj[0].instID);
                }
                cubeObj.cubeObj[0].mul.End();
                return;
            }
#endif
            NotifyEvent<PIPE_MTE3>(eventID);
            if constexpr (sync) {
                WaitEvent(this->instIdx);
            }
            return;
        }
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        kfcMsg_.body.iterateFakeMsg = fakeMsg;
        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.waitIterateAll = waitIterateAll;
        PostMessage<KFC_Enum::MMFUN_ITERATE_ALL, sync>();
        SyncCubeWithVec<A_TYPE::ibShare, B_TYPE::ibShare>();
        if constexpr (sync) {
            auto intraId = this->devEvtID;
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                if (GetSubBlockIdxImpl() == 1) {
                    intraId = this->devEvtID - 1;
                }
            }
            WaitEvent(intraId);
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }

    template <bool sync = true>
    __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), {
            KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput.");
        });
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
            ASSERT("IterateAll localTensor not support when enableMixDualMaster is enabled");
#endif
            return;
        }
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        ASSERT(sync == true);
        ASSERT(enAtomic == 0);
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        ASSERT((PhyPosIsL1(C_TYPE::pos)) && "IterateAll LocalTensor only supports L1 Buffer(A1/B1)");
        ASSERT(
            !(A_TYPE::ibShare && B_TYPE::ibShare) && "IterateAll LocalTensor not support when sameab"
                                                     " is enabled");
        if (ubCmatrix.GetPosition() == static_cast<int32_t>(TPosition::TSCM)) {
            kfcMsg_.body.cAddr = GetTscmAddr(ubCmatrix);
            kfcMsg_.body.cIsTscm = 1;
        } else {
            kfcMsg_.body.cAddr = GetGlobalAddr<typename C_TYPE::T, false>(ubCmatrix);
        }
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.sync = sync;
        ASSERT(kfcMsg_.body.enSequentialWrite == 0);
        GM_ADDR gmDataAddr = reinterpret_cast<GM_ADDR>(kfcMsg_.body.cAddr);
        PostMessage<KFC_Enum::MMFUN_ITERATE_ALL, sync>();

        if constexpr (sync) {
            WaitEvent(this->devEvtID);
            CopyToUB(ubCmatrix, gmDataAddr, ubCmatrix.GetSize());
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }
#endif

    template <bool sync = true, bool waitIterateBatch = false>
    __aicore__ inline void IterateBatch(
        const GlobalTensor<DstT>& gm, uint32_t batchA, uint32_t batchB, bool enSequentialWrite,
        const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0,
        const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateBatch not support when enableMixDualMaster is enabled");
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        ASSERT(
            !(A_TYPE::ibShare && B_TYPE::ibShare) && "IterateBatch not support when sameab"
                                                     " is enabled");
        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.batchA = batchA;
        kfcMsg_.body.batchB = batchB;
        kfcMsg_.body.matrixStrideA = matrixStrideA;
        kfcMsg_.body.matrixStrideB = matrixStrideB;
        kfcMsg_.body.matrixStrideC = matrixStrideC;
        kfcMsg_.body.waitIterateBatch = waitIterateBatch;
        kfcMsg_.body.enPartialSum = enPartialSum;
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.setBatch = 1;

#if defined(USE_SSBUF)
        PrepareABFromGM();
#endif
        PostMessage<KFC_Enum::MMFUN_ITERATE_BATCH_ALL, sync>();

        if constexpr (sync) {
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
#else
            WaitEvent(this->devEvtID);
#endif
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }

    template <bool sync = true>
    __aicore__ inline void IterateBatch(
        const LocalTensor<DstT>& ubCmatrix, uint32_t batchA, uint32_t batchB, bool enSequentialWrite,
        const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0,
        const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateBatch not support when enableMixDualMaster is enabled");
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        ASSERT(sync == true);
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        ASSERT(!(A_TYPE::ibShare && B_TYPE::ibShare) && "IterateBatch not support when sameab is enabled");
        if (ubCmatrix.GetPosition() == static_cast<int32_t>(TPosition::TSCM)) {
            kfcMsg_.body.cAddr = GetTscmAddr(ubCmatrix);
            kfcMsg_.body.cIsTscm = 1;
        } else {
#if defined(USE_SSBUF)
            kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(ubCmatrix.GetPhyAddr());
#else
            kfcMsg_.body.cAddr = GetGlobalAddr<typename C_TYPE::T, false>(ubCmatrix);
#endif
        }
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.batchA = batchA;
        kfcMsg_.body.batchB = batchB;
        kfcMsg_.body.matrixStrideA = matrixStrideA;
        kfcMsg_.body.matrixStrideB = matrixStrideB;
        kfcMsg_.body.matrixStrideC = matrixStrideC;
        kfcMsg_.body.enPartialSum = enPartialSum;
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.setBatch = 1;
        GM_ADDR gmDataAddr = reinterpret_cast<GM_ADDR>(kfcMsg_.body.cAddr);
#if defined(USE_SSBUF)
        PrepareABFromGM();
        CrossCoreSetFlag<INTRA_MODE, PIPE_V>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + this->instIdx);
#endif
        PostMessage<KFC_Enum::MMFUN_ITERATE_BATCH_ALL, sync>();

        if constexpr (sync) {
#if defined(USE_SSBUF)
            if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
            }
#else
            WaitEvent(this->devEvtID);
            CopyToUB(ubCmatrix, gmDataAddr, ubCmatrix.GetSize());
#endif
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }

    template <bool sync = true, bool waitIterateBatch = false>
    __aicore__ inline void IterateNBatch(
        const uint32_t batchLoop, uint32_t batchA, uint32_t batchB, bool enSequentialWrite,
        const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0,
        const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "IterateNBatch not support when enableMixDualMaster is enabled.");
        static_assert(
            A_TYPE::layout != LayoutMode::NONE && B_TYPE::layout != LayoutMode::NONE &&
                A_TYPE::layout != LayoutMode::NORMAL && B_TYPE::layout != LayoutMode::NORMAL &&
                C_TYPE::layout != LayoutMode::NORMAL,
            "BMM does not support the layout being NONE or NORMAL");
        if constexpr (!ToMatmulConfig(MM_CFG).isNBatch) {
            return;
        }
        TRACE_START(TraceId::KFC_CLIENT_POST_MSG);
        cntIter_ = 0;
        cOffset_ = 0;
        nbatchIter_ = 0;
        curProcess = 0;
        ASSERT(kfcMsg_.body.isFirstIter == 1);
        ASSERT(cacheWorkspaceAddr);
        ASSERT(!(A_TYPE::ibShare && B_TYPE::ibShare) && "IterateNBatch not support when sameab is enabled");
        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(cacheWorkspaceAddr);
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.sync = sync;
        kfcMsg_.body.batchLoop = batchLoop;
        kfcMsg_.body.batchA = batchA;
        kfcMsg_.body.batchB = batchB;
        kfcMsg_.body.matrixStrideA = matrixStrideA;
        kfcMsg_.body.matrixStrideB = matrixStrideB;
        kfcMsg_.body.matrixStrideC = matrixStrideC;
        kfcMsg_.body.enPartialSum = enPartialSum;
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.setBatch = 1;
        kfcMsg_.body.waitIterateBatch = waitIterateBatch;
#if defined(USE_SSBUF)
        PrepareABFromGM();
#endif
        PostMessage<KFC_Enum::MMFUN_ITERATE_N_BATCH_ALL, sync>();
        if constexpr (sync) {
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
#else
            WaitEvent(this->devEvtID);
#endif
        }
        isSyncGetC = sync;
        TRACE_STOP(TraceId::KFC_CLIENT_POST_MSG);
    }

#if defined(USE_SSBUF)
    template <bool sync = true>
    __aicore__ inline void GetTensorC(
        const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster, "GetTensorC not support when enableMixDualMaster is enabled.");
        static_assert(
            ToMatmulConfig(MM_CFG).enableGetTensorC, "GetTensorC is not support when enableGetTensorC is disabled");
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        if (!isSyncGetC) { // Asynchronous
            // If the buffer is not configured, the output is stored in the L0C buffer.
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                if (GetSubBlockIdxImpl() == 1) {
                    if constexpr (!IsBasic(ToMatmulConfig(MM_CFG))) {
                        CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
                    }
                    return;
                }
            }
            auto msg = client->AllocMessage();
            msg->body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
            msg->body.enAtomic = (uint8_t)(enAtomic);
            msg->body.enSequentialWrite = enSequentialWrite;
            msg->head = KfcMsgMakeFlag(KFC_Enum::MMFUN_GET_TENSOR_C, this->instIdx);
            client->PostMessage<false>(msg);
            if constexpr (!(IsBasic(ToMatmulConfig(MM_CFG)) && (A_TYPE::ibShare && B_TYPE::ibShare))) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
            }
            TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
            return;
        }

        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.sync = sync;

        PostMessage<KFC_Enum::MMFUN_GET_TENSOR_C, sync>();
        if constexpr (sync) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
        }
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
    }

    template <bool sync = true, bool doPad = false>
    __aicore__ inline void GetTensorC(
        const LocalTensor<DstT>& c, uint8_t enAtomic = 0, bool enSequentialWrite = false, uint32_t height = 0,
        uint32_t width = 0, uint32_t srcGap = 0, uint32_t dstGap = 0)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster, "GetTensorC not support when enableMixDualMaster is enabled.");
        static_assert(
            ToMatmulConfig(MM_CFG).enableGetTensorC, "GetTensorC is not support when enableGetTensorC is disabled");
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_UB);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        uint64_t singleSize;
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreMN != 0) {
            singleSize = ToMatmulConfig(MM_CFG).singleCoreMN;
        } else {
            singleSize = cubeTiling.GetSingleCoreM() * cubeTiling.GetSingleCoreN();
        }
        // Asynchronous
        if (!isSyncGetC) {
            ASSERT(enAtomic == 0);
            // check if setworkspace enabled and less than total ub size to decide copy gm to ub or not
            if (cacheWorkspaceAddr == 0) {
                GetTensorCWithoutGm(c, enAtomic, enSequentialWrite);
                return;
            }
            // If buffer is configured, block MTE2 to ensure that the output can be transported.
            if (curProcess < INC_PROCESS_CHECK) {
                ++curProcess;
                CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
            }

            uint32_t baseSize;
            if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
                baseSize = ToMatmulConfig(MM_CFG).baseMN * sizeof(typename C_TYPE::T);
            } else {
                baseSize = cubeTiling.GetBaseM() * cubeTiling.GetBaseN() * sizeof(typename C_TYPE::T);
            }
            if constexpr (doPad) {
                CopyToUBPad(c, cacheWorkspaceAddr + cOffset_, height, width, srcGap, dstGap);
            } else {
                CopyToUB(c, cacheWorkspaceAddr + cOffset_, c.GetSize());
            }
            cOffset_ += baseSize;
            TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_UB);
            return;
        }

        // Must be the same as Iterate.
        ASSERT(sync == true);
        ASSERT(enAtomic == 0);
        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(c.GetPhyAddr());
        kfcMsg_.body.sync = 1;
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        CrossCoreSetFlag<INTRA_MODE, PIPE_V>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + this->instIdx);
        PostMessage<KFC_Enum::MMFUN_GET_TENSOR_C, true>();
        CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_UB);
        return;
    }
#else
    // Synchronous interface. The user sends the GM address, which contains 64 bits.
    template <bool sync = true>
    __aicore__ inline void GetTensorC(
        const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetTensorC not support when enableMixDualMaster is enabled");
        static_assert(
            ToMatmulConfig(MM_CFG).enableGetTensorC, "GetTensorC is not support when enableGetTensorC is disabled");
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            ASSERT(false && "GetTensorC not support when sameab is enabled");
            return;
        }
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        ASSERT(isSyncGetC); // The mode must be synchronous.

        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.sync = sync;

        PostMessage<KFC_Enum::MMFUN_GET_TENSOR_C, sync>();

        if constexpr (sync) {
            WaitEvent(this->devEvtID);
        }
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
    }

    // Synchronous interface
    template <bool sync = true, bool doPad = false>
    __aicore__ inline void GetTensorC(
        const LocalTensor<DstT>& c, uint8_t enAtomic = 0, bool enSequentialWrite = false, uint32_t height = 0,
        uint32_t width = 0, uint32_t srcGap = 0, uint32_t dstGap = 0)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetTensorC not support when enableMixDualMaster is enabled");
        static_assert(
            ToMatmulConfig(MM_CFG).enableGetTensorC, "GetTensorC is not support when enableGetTensorC is disabled");
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_UB);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        if (!isSyncGetC) { // Asynchronous
            ASSERT(cacheWorkspaceAddr);
            ASSERT(enAtomic == 0);

            if (curProcess < INC_PROCESS_CHECK) {
                ++curProcess;
                WaitEvent(this->devEvtID);
            }

            uint32_t size;
            if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
                size = ToMatmulConfig(MM_CFG).baseMN * sizeof(typename C_TYPE::T);
            } else {
                size = cubeTiling.GetBaseM() * cubeTiling.GetBaseN() * sizeof(typename C_TYPE::T);
            }
            if constexpr (doPad) {
                CopyToUBPad(c, cacheWorkspaceAddr + cOffset_, height, width, srcGap, dstGap);
            } else {
                CopyToUB(c, cacheWorkspaceAddr + cOffset_, c.GetSize());
            }
            cOffset_ += size;
            TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_UB);
            return;
        }

        ASSERT(sync == true); // must be the same as Iterate.
        ASSERT(enAtomic == 0);
        kfcMsg_.body.cAddr = GetGlobalAddr<typename C_TYPE::T, false>(c);
        kfcMsg_.body.sync = 1;
        kfcMsg_.body.enAtomic = (uint8_t)(enAtomic);
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;

        GM_ADDR gmDataAddr = reinterpret_cast<GM_ADDR>(kfcMsg_.body.cAddr);
        PostMessage<KFC_Enum::MMFUN_GET_TENSOR_C, true>();

        WaitEvent(this->devEvtID);

        if constexpr (PhyPosIsUB(C_TYPE::pos)) {
            if constexpr (doPad) {
                CopyToUBPad(c, (__gm__ DstT*)gmDataAddr, height, width);
            } else {
                CopyToUB(c, (__gm__ DstT*)gmDataAddr, c.GetSize());
            }
        }
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_UB);
        return;
    }
#endif

    template <bool sync = true>
    __aicore__ inline void GetTensorC(
        const GlobalTensor<DstT>& gm, const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0,
        bool enSequentialWrite = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster, "GetTensorC not support when enableMixDualMaster is enabled.");
        static_assert(
            ToMatmulConfig(MM_CFG).enableGetTensorC, "GetTensorC is not support when enableGetTensorC is disabled");
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        ASSERT(isSyncGetC); // must synchronization mode

        kfcMsg_.body.cAddr = reinterpret_cast<uint64_t>(gm.GetPhyAddr());
        kfcMsg_.body.enAtomic = (uint8_t)enAtomic;
        kfcMsg_.body.enSequentialWrite = enSequentialWrite;
        kfcMsg_.body.sync = sync;

        PostMessage<KFC_Enum::MMFUN_GET_TENSOR_C, sync>();

        if constexpr (sync) {
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
#else
            WaitEvent(this->devEvtID);
#endif
        }

        CopyToUB(co2Local, gm.GetPhyAddr(), co2Local.GetSize());
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
    }

    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetTensorC(uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster, "GetTensorC not support when enableMixDualMaster is enabled.");
        static_assert(
            ToMatmulConfig(MM_CFG).enableGetTensorC, "GetTensorC is not support when enableGetTensorC is disabled");
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        ASSERT(!isSyncGetC); // Asynchronous only
        ASSERT(cacheWorkspaceAddr);
        if (curProcess < INC_PROCESS_CHECK) {
            ++curProcess;
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
#else
            auto intraId = this->devEvtID;
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                if (GetSubBlockIdxImpl() == 1) {
                    intraId = this->devEvtID - 1;
                }
            }
            WaitEvent(intraId);
#endif
        }
        uint32_t size;
        GlobalTensor<DstT> global;
        if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
            size = ToMatmulConfig(MM_CFG).baseMN * sizeof(typename C_TYPE::T);
            global.SetGlobalBuffer(
                reinterpret_cast<__gm__ DstT*>(cacheWorkspaceAddr + cOffset_), ToMatmulConfig(MM_CFG).baseMN);
        } else {
            size = cubeTiling.GetBaseM() * cubeTiling.GetBaseN() * sizeof(typename C_TYPE::T);
            global.SetGlobalBuffer(
                reinterpret_cast<__gm__ DstT*>(cacheWorkspaceAddr + cOffset_),
                cubeTiling.GetBaseM() * cubeTiling.GetBaseN());
        }
        cOffset_ += size;
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
        return global;
    }

    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetBatchTensorC(
        uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetBatchTensorC not support when enableMixDualMaster is enabled");
        GlobalTensor<DstT> global;
        if constexpr (!ToMatmulConfig(MM_CFG).isNBatch) {
            return global;
        }
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        ASSERT(!isSyncGetC); // only support async
        ASSERT(cacheWorkspaceAddr);
        if (curProcess < INC_PROCESS_CHECK) {
            ++curProcess;
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId);
#else
            WaitEvent(this->devEvtID);
#endif
        }

        uint32_t batch = batchA > batchB ? batchA : batchB;
        global.SetGlobalBuffer(
            reinterpret_cast<__gm__ DstT*>(cacheWorkspaceAddr + cOffset_),
            batch * cubeTiling.GetSingleCoreM() * cubeTiling.GetSingleCoreN());
        uint32_t size = batch * cubeTiling.GetSingleCoreM() * cubeTiling.GetSingleCoreN() * sizeof(typename C_TYPE::T);
        cOffset_ += size;
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
        return global;
    }

    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetBatchC(uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster && "GetBatchC not support when enableMixDualMaster is enabled");
        return GetBatchTensorC(batchA, batchB, enSequentialWrite);
    }

    // coordinated use with IterateNBatch, get single IterateBatch outcome
    template <bool sync = true>
    __aicore__ inline void GetBatchTensorC(
        const LocalTensor<DstT>& c, uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetBatchTensorC not support when enableMixDualMaster is enabled");
        if constexpr (!ToMatmulConfig(MM_CFG).isNBatch) {
            return;
        }
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        ASSERT(cacheWorkspaceAddr);
        ASSERT(!isSyncGetC); // only support async

        if (curProcess < INC_PROCESS_CHECK) {
            ++curProcess;
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(waitFixpId); // GM to UB
#else
            WaitEvent(this->devEvtID);
#endif
        }
        // 计算batchC
        uint32_t batchC = GetBatchCNum(
            batchA, batchB, cubeTiling.GetALayoutInfoG(), cubeTiling.GetBLayoutInfoG(), cubeTiling.GetCLayoutInfoG());
        cOffset_ = CalcNBatchoffset<C_TYPE>(
            batchC, nbatchIter_, cubeTiling.GetCLayoutInfoN(), cubeTiling.GetCLayoutInfoG(),
            cubeTiling.GetSingleCoreN(), cubeTiling.GetCLayoutInfoS1());
        CopyToUBBatch(c, cacheWorkspaceAddr + cOffset_, batchC);
        nbatchIter_ += 1;

        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
    }

    template <bool sync = true>
    __aicore__ inline void GetBatchC(
        const LocalTensor<DstT>& c, uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster && "GetBatchC not support when enableMixDualMaster is enabled");
        GetBatchTensorC(c, batchA, batchB, enSequentialWrite);
    }

    __aicore__ inline void AsyncGetTensorC(const LocalTensor<DstT>& c)
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "AsyncGetTensorC not support when enableMixDualMaster is enabled");
        TRACE_START(TraceId::KFC_CLIENT_REV_MSG_GM);
        ASSERT(kfcMsg_.body.isFirstIter == 0);
        ASSERT(cacheWorkspaceAddr);
        ASSERT(!isSyncGetC);

        if (curProcess < INC_PROCESS_CHECK) {
            ++curProcess;
#if defined(USE_SSBUF)
            CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
#else
            WaitEvent(this->devEvtID);
#endif
        }

        uint32_t size = cubeTiling.GetBaseM() * cubeTiling.GetBaseN() * sizeof(typename C_TYPE::T);
        CopyToUB<DstT, uint8_t, false>(c, cacheWorkspaceAddr + cOffset_, c.GetSize());
        cOffset_ += size;
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_GM);
        return;
    }

    __aicore__ inline void WaitGetTensorC()
    {
        ASSERT(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "WaitGetTensorC not support when enableMixDualMaster is enabled");
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        SetFlag<HardEvent::MTE2_V>(eventID);
        WaitFlag<HardEvent::MTE2_V>(eventID);
    }

    template <bool isTurnOnDebug = true>
    __aicore__ inline MatrixOffset GetOffsetC()
    {
        if constexpr (isTurnOnDebug) {
            ASCENDC_REPORT_NOT_SUPPORT(false, "GetOffsetC");
        }
    }

    __aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer)
    {
#if defined(USE_SSBUF)
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetLocalWorkspace not support when enableMixDualMaster is enabled.");
        localWorkspace_ = tmpBuffer;
#if ASCENDC_CPU_DEBUG
        int32_t minUbNd2NzTmpSize = 0;
        int32_t scaleK = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * 2;
        c0Size_ = AscendCUtils::GetC0Count(sizeof(fp8_e8m0_t));
        if constexpr (PhyMxScalePosIsUB<A_TYPE>()) {
            if constexpr (A_TYPE::scaleFormat == CubeFormat::ND) {
                if constexpr (A_TYPE::isScaleTrans) {
                    minUbNd2NzTmpSize += CeilAlign(singleCoreM_, c0Size_) * scaleK;
                } else {
                    minUbNd2NzTmpSize += CeilAlign(scaleK, c0Size_) * singleCoreM_;
                }
            }
        }
        if constexpr (PhyMxScalePosIsUB<B_TYPE>()) {
            if constexpr (B_TYPE::scaleFormat == CubeFormat::ND) {
                if constexpr (B_TYPE::isScaleTrans) {
                    minUbNd2NzTmpSize += singleCoreN_ * CeilAlign(scaleK, c0Size_);
                } else {
                    minUbNd2NzTmpSize += scaleK * CeilAlign(singleCoreN_, c0Size_);
                }
            }
        }
        ASCENDC_ASSERT((localWorkspace_.GetSize() >= minUbNd2NzTmpSize), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "For mxMatmul ub position input in ND format,"
                " scaleA/scaleB requires at latest %d Byte of temporary space.",
                minUbNd2NzTmpSize);
        });
#endif
#endif
    }

#if defined(USE_SSBUF)
    using ScaleT = fp8_e8m0_t;

    __aicore__ inline void SetTensorScaleA(const GlobalTensor<ScaleT>& gm, bool isTransposeScaleA = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleA not support when enableMixDualMaster is enabled.");
        ASSERT(
            isTransposeScaleA <= A_TYPE::isScaleTrans &&
            "It is not allowed to do scaleA transpose when matmul scaleA transpose is not defined.");
        kfcMsg_.body.quantMode = (kfcMsg_.body.quantMode & 0b10) | (static_cast<uint32_t>(isTransposeScaleA) & 0b01);
        kfcMsg_.body.quantAddr = reinterpret_cast<uint64_t>(gm.address_);
    }

    __aicore__ inline void SetTensorScaleA(const LocalTensor<ScaleT>& leftMatrix, bool isTransposeScaleA = false)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleA not support when enableMixDualMaster is enabled.");
        ASSERT(
            isTransposeScaleA <= A_TYPE::isScaleTrans &&
            "It is not allowed to do scaleA transpose when matmul scaleA transpose is not defined.");
        kfcMsg_.body.quantMode = (kfcMsg_.body.quantMode & 0b10) | (static_cast<uint32_t>(isTransposeScaleA) & 0b01);
        if constexpr (PhyMxScalePosIsL1<A_TYPE>()) {
            kfcMsg_.body.quantAddr = GetTscmAddr(leftMatrix);
            sizeScaleAmatrix_ = leftMatrix.GetSize() * sizeof(ScaleT);
            auto intraId = (reinterpret_cast<TBufType*>(leftMatrix.GetBufferHandle()))->enQueEvtID;
            // 8 bit for intraID, 32 bit for addr
            kfcMsg_.body.quantAddr = (((uint64_t)intraId) << VALID_ADDR_BITS_NUM) + kfcMsg_.body.quantAddr;
        } else {
            MSG_POS MsgMatmulL1Addr* matmulL1AddrMsg =
                (MSG_POS MsgMatmulL1Addr*)GetMatmulL1AddrMsg(GetSubBlockIdxImpl(), this->instIdx);
            while (!(matmulL1AddrMsg->valid)) {
            }
            kfcMsg_.body.quantAddr = matmulL1AddrMsg->l1aScaleAddr;
            sizeScaleAmatrix_ = leftMatrix.GetSize() * sizeof(ScaleT);
            aScaleAddr_ = (uint64_t)leftMatrix.GetPhyAddr();
        }
    }

    __aicore__ inline void SetTensorScaleB(const GlobalTensor<ScaleT>& gm, bool isTransposeScaleB = true)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleB not support when enableMixDualMaster is enabled.");
        ASSERT(
            isTransposeScaleB <= B_TYPE::isScaleTrans &&
            "It is not allowed to do scaleB transpose when matmul scaleB transpose is not defined.");
        kfcMsg_.body.quantMode =
            (kfcMsg_.body.quantMode & 0b01) | ((static_cast<uint32_t>(isTransposeScaleB) & 0b01) << 1);
        kfcMsg_.body.quantScalar = reinterpret_cast<uint64_t>(gm.address_);
    }

    __aicore__ inline void SetTensorScaleB(const LocalTensor<ScaleT>& rightMatrix, bool isTransposeScaleB = true)
    {
        static_assert(
            !ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleB not support when enableMixDualMaster is enabled.");
        ASSERT(
            isTransposeScaleB <= B_TYPE::isScaleTrans &&
            "It is not allowed to do scaleB transpose when matmul scaleB transpose is not defined.");
        kfcMsg_.body.quantMode =
            (kfcMsg_.body.quantMode & 0b01) | ((static_cast<uint32_t>(isTransposeScaleB) & 0b01) << 1);
        if constexpr (PhyMxScalePosIsL1<B_TYPE>()) {
            kfcMsg_.body.quantScalar = GetTscmAddr(rightMatrix);
            sizeScaleBmatrix_ = rightMatrix.GetSize() * sizeof(ScaleT);
            auto intraId = (reinterpret_cast<TBufType*>(rightMatrix.GetBufferHandle()))->enQueEvtID;
            // 8 bit for intraID, 32 bit for addr
            kfcMsg_.body.quantScalar = (((uint64_t)intraId) << VALID_ADDR_BITS_NUM) + kfcMsg_.body.quantScalar;
        } else {
            MSG_POS MsgMatmulL1Addr* matmulL1AddrMsg =
                (MSG_POS MsgMatmulL1Addr*)GetMatmulL1AddrMsg(GetSubBlockIdxImpl(), this->instIdx);
            while (!(matmulL1AddrMsg->valid)) {
            }
            kfcMsg_.body.quantScalar = matmulL1AddrMsg->l1bScaleAddr;
            sizeScaleBmatrix_ = rightMatrix.GetSize() * sizeof(ScaleT);
            bScaleAddr_ = (uint64_t)rightMatrix.GetPhyAddr();
        }
    }
#endif

#if ASCENDC_CPU_DEBUG
public:
    // this is useless code just for cpu debug
    typename MatmulInstAux<IsSharedObj(MM_CFG), A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::MATMUL
        cubeObj;
#endif

private:
    GM_ADDR cacheWorkspaceAddr;
    // Multiple instances with only one message queue maintained.
    // Use shared memory to get the queue.
    KfcCommClient* client;
    TPipe* tpipe;
    MatmulTiling<MM_CFG> cubeTiling;
    KfcMsg kfcMsg_;

    bool isSyncGetC;
    uint16_t devEvtID;
    uint16_t instIdx;
    uint16_t curProcess;

    uint32_t mIter_;
    uint32_t nIter_;
    uint32_t cntIter_;
    uint32_t mnIter_;
    uint64_t cOffset_;
    uint32_t nbatchIter_;

#if defined(USE_SSBUF)
    uint32_t sizeAmatrix_;
    uint32_t sizeBmatrix_;
    LocalTensor<uint8_t> localWorkspace_ = LocalTensor<uint8_t>();
    uint64_t aAddr_;
    uint64_t bAddr_;
    uint64_t biasAddr_;
    uint8_t waitFixpId;
    int32_t singleCoreM_;
    int32_t singleCoreN_;
    int32_t singleCoreK_;
    int32_t c0Size_;
    uint32_t sizeScaleAmatrix_;
    uint32_t sizeScaleBmatrix_;
    uint64_t aScaleAddr_;
    uint64_t bScaleAddr_;
    uint32_t sizeBiasmatrix_;
    uint32_t scaleTmpBufSize_ = 0;
#endif
    template <class T, class U>
    friend __aicore__ inline void InitKfcClient(
        T& cubeObj, U* tiling, TPipe* tpipe, KfcCommClient* client, int instIdx, GM_ADDR workspace);
    template <class... Args>
    friend struct AscendC::GetCubeObjConfig;
    constexpr static bool enableMixDualMaster = ToMatmulConfig(MM_CFG).enableMixDualMaster;
    constexpr static bool enableABShare = A_TYPE::ibShare && B_TYPE::ibShare;

private:
    template <class T>
    __aicore__ inline void InitStatic(T* tiling)
    {
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
        if constexpr (IsSameTypeV<T, const __gm__ TCubeTiling>) {
            TCubeTiling cubeTiling;
            CopyTiling<A_TYPE, B_TYPE, MM_CFG>(tiling, cubeTiling);
            this->cubeTiling.SetTiling(&cubeTiling);
        } else {
#endif
            this->cubeTiling.SetTiling((TCubeTiling*)tiling);
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
        }
#endif
        if (ToMatmulConfig(MM_CFG).singleCoreM == 0 && this->cubeTiling.IsNull()) {
            return;
        }
        ASSERT(sizeof(KfcMsg) % CACHE_LINE_SIZE == 0);

        *((uint64_t*)&kfcMsg_) = 0;
        *((uint64_t*)&(kfcMsg_.body)) = 0;
        nIter_ = ConstCeil(cubeTiling.GetSingleCoreN(), cubeTiling.GetBaseN());
        mIter_ = ConstCeil(cubeTiling.GetSingleCoreM(), cubeTiling.GetBaseM());
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            uint32_t kIter = ConstCeil(cubeTiling.GetSingleCoreK(), cubeTiling.GetBaseK());
            mnIter_ = nIter_ * mIter_ * kIter;
        } else {
            mnIter_ = nIter_ * mIter_;
        }
        cacheWorkspaceAddr = nullptr;
#if defined(USE_SSBUF)
        singleCoreM_ = this->cubeTiling.GetSingleCoreM();
        singleCoreN_ = this->cubeTiling.GetSingleCoreN();
        singleCoreK_ = this->cubeTiling.GetSingleCoreK();
#endif
    }

#if defined(USE_WORKSPACE)
    template <class T>
    __aicore__ inline uint64_t CopyGlobalAddr(GM_ADDR& gmDataAddr, const LocalTensor<T>& data)
    {
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);

        struct DataCopyParams param;
        param.blockLen = data.GetSize() / AscendCUtils::GetC0Count(sizeof(T));
        GlobalTensor<T> globalTensor;
        globalTensor.SetGlobalBuffer((__gm__ T*)gmDataAddr);
        DataCopy(globalTensor, data, param);

        return reinterpret_cast<uint64_t>(gmDataAddr);
    }

    template <class T, bool isCopy>
    __aicore__ inline uint64_t GetGlobalAddr(const LocalTensor<T>& data)
    {
        uint64_t size = Ceil(data.GetSize() * sizeof(T), ONE_BLK_SIZE) * ONE_BLK_SIZE;
        if constexpr (IsSameType<T, int4b_t>::value) {
            size /= INT4_TWO;
        }
        auto gmDataAddr = client->AllocUB(size, kfcMsg_.ubAddr);

        if constexpr (isCopy) {
            return CopyGlobalAddr(gmDataAddr, data);
        }
        return reinterpret_cast<uint64_t>(gmDataAddr);
    }
#endif

    template <class T>
    __aicore__ inline uint64_t GetTscmAddr(const LocalTensor<T>& data)
    {
#if ASCENDC_CPU_DEBUG
        ASSERT(GetTPipePtr() != nullptr && "tpipe cannot be nullptr when matmul client post msg");
        return GetAbsAddr<T>(GetTPipePtr(), data);
#else
        return (uint64_t)data.GetPhyAddr();
#endif
    }

#if defined(USE_SSBUF)
    template <KFC_Enum funID, bool isAck>
    __aicore__ inline void PostMessage()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            *((uint32_t*)&kfcMsg_.body) = 0;
            return;
        }
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            if (GetSubBlockIdxImpl() == 1) {
                client->PostSameABFakeMsg<isAck>(funID, this->instIdx);
                *((uint32_t*)&kfcMsg_.body) = 0;
                return;
            }
        }
        auto msg = client->AllocMessage();
        ASSERT(msg != nullptr && "msg cannot be nullptr when matmul client post msg");
        auto msgDst = reinterpret_cast<__ssbuf__ uint64_t*>(&(msg->body));
        auto msgSrc = reinterpret_cast<uint64_t*>(&(kfcMsg_.body));
        if constexpr (
            ToMatmulConfig(MM_CFG).enableQuantVector ||
            (ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_BATCH) != 0 ||
            (ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_N_BATCH) != 0 ||
            HasScalePosition<A_TYPE>::value || HasScalePosition<B_TYPE>::value) {
            SendSSbufData<15>(msgSrc, msgDst);
        } else if constexpr (ToMatmulConfig(MM_CFG).enableSetBias) {
            SendSSbufData<9>(msgSrc, msgDst);
        } else if constexpr (ToMatmulConfig(MM_CFG).enableSetTail) {
            SendSSbufData<8>(msgSrc, msgDst);
        } else if constexpr (ToMatmulConfig(MM_CFG).enableSetOrgShape) {
            SendSSbufData<7>(msgSrc, msgDst);
        } else {
            SendSSbufData<4>(msgSrc, msgDst);
        }
        if constexpr (ToMatmulConfig(MM_CFG).enableSetDefineData) {
            msg->userCustomData = kfcMsg_.userCustomData;
        }
        msg->head = KfcMsgMakeFlag(funID, this->instIdx, A_TYPE::ibShare && B_TYPE::ibShare);
        client->PostMessage<isAck>(msg);
        // clear flag
        *((uint32_t*)&kfcMsg_.body) = 0;
    }
#else
    template <KFC_Enum funID, bool isAck>
    __aicore__ inline void PostMessage()
    {
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            ASSERT(DoMatmulNorm(MM_CFG) && "MM_CFG should use norm config when sameab is enabled");
            if (GetSubBlockIdxImpl() == 1) {     // Do not send v1's message to cube
                *((uint32_t*)&kfcMsg_.body) = 0; // Clear all flag bits.
                kfcMsg_.ubAddr = -1;
                return;
            }
        }
        kfcMsg_.head = KfcMsgMakeFlag(funID, this->instIdx);

        auto msg = client->AllocMessage();
        ASSERT(msg != nullptr && "msg cannot be nullptr when matmul client post msg");

        auto tmp1 = reinterpret_cast<__ubuf__ uint64_t*>(client->ubMsg);
        auto tmp2 = reinterpret_cast<uint64_t*>(&kfcMsg_);
        for (int i = 0; i < sizeof(kfcMsg_) / sizeof(uint64_t); i++, tmp1++, tmp2++) {
            *tmp1 = *tmp2;
        }

        client->PostMessage<isAck>(msg);

        // clear flag
        *((uint32_t*)&kfcMsg_.body) = 0; // Clear all flag bits.
        kfcMsg_.ubAddr = -1;
    }
#endif

#if defined(USE_SSBUF)
    template <bool sync = true>
    __aicore__ inline void IterateAllCPU(
        const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0, bool enSequentialWrite = false,
        bool waitIterateAll = false)
    {
#if ASCENDC_CPU_DEBUG
        if ASCEND_IS_AIC {
            if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                    cubeObj.cubeObj[0].instID, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP), 0));
                CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(
                    cubeObj.cubeObj[0].instID + static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + INTRA_NUM);
            }
            cubeObj.cubeObj[0].mul.IterateAll(ubCmatrix, enAtomic);
            if (sync || waitIterateAll) {
                CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                    cubeObj.cubeObj[0].instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), 0U));
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                    CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                        cubeObj.cubeObj[0].instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP),
                        1U)); // 1 means sub_block 1
                }
            }
            cubeObj.cubeObj[0].mul.End();
            return;
        }
#endif
    }

    __aicore__ inline void GetTensorCWithoutGm(
        const LocalTensor<DstT>& c, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        // If the cache is not configured, the output is stored in the L0C buffer.
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            if (GetSubBlockIdxImpl() == 1) {
                CrossCoreSetFlag<INTRA_MODE, PIPE_V>(
                    static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + this->instIdx);
                CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
                return;
            }
        }
        CrossCoreSetFlag<INTRA_MODE, PIPE_V>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + this->instIdx);
        if constexpr (!ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            auto msg = client->AllocMessage();
            msg->body.cAddr = reinterpret_cast<uint64_t>(c.GetPhyAddr());
            msg->body.enAtomic = (uint8_t)(enAtomic);
            msg->body.enSequentialWrite = enSequentialWrite;
            msg->head = KfcMsgMakeFlag(KFC_Enum::MMFUN_GET_TENSOR_C, this->instIdx);
            client->PostMessage<false>(msg);
        }
        CrossCoreWaitFlag<INTRA_MODE, PIPE_V>(waitFixpId);
        TRACE_STOP(TraceId::KFC_CLIENT_REV_MSG_UB);
        return;
    }

    __aicore__ inline void PrepareABFromGM()
    {
        // there is hidden logic in c220, so only when use ssbuf need to check if gm is ready
        if constexpr (
            (GetPhyType(A_TYPE::pos) == Hardware::GM && GetPhyType(A_TYPE::srcPos) == Hardware::UB) ||
            (GetPhyType(B_TYPE::pos) == Hardware::GM && GetPhyType(B_TYPE::srcPos) == Hardware::UB) ||
            (GetPhyType(BIAS_TYPE::pos) == Hardware::GM && GetPhyType(BIAS_TYPE::srcPos) == Hardware::UB)) {
            // Op sometimes execute ub->gm in MTE3 before using iterate, can find matched wait flag in matmul server
            CrossCoreSetFlag<INTRA_MODE, PIPE_MTE3>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM));
        }
    }

    __aicore__ inline void PrepareABFromUb(
        bool isTransA, bool isTransB, bool isBias, bool isTransScaleA, bool isTransScaleB)
    {
        constexpr bool isAnyInputFromUb = GetPhyType(A_TYPE::pos) == Hardware::UB ||
                                          GetPhyType(B_TYPE::pos) == Hardware::UB || PhyMxScalePosIsUB<A_TYPE>() ||
                                          PhyMxScalePosIsUB<B_TYPE>() || GetPhyType(BIAS_TYPE::pos) == Hardware::UB;
        // Ensure that the condition judgment matches the function WaitAB on the mmserver.
        if constexpr (
            ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) && isAnyInputFromUb) {
            // iterate mode only copy ub to l1 once
            if (cntIter_ != 0) {
                return;
            }
        }

        if constexpr (isAnyInputFromUb) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE3>(static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::UB_L1_L1_L0AB));
        }

        if constexpr (GetPhyType(A_TYPE::pos) == Hardware::UB) {
            if constexpr (
                ToMatmulConfig(MM_CFG).singleCoreM != 0 && ToMatmulConfig(MM_CFG).singleCoreN != 0 &&
                ToMatmulConfig(MM_CFG).singleCoreK != 0 && IsStaticPaddingEnable(MM_CFG)) {
                CopyUbAToL1StaticTiling(isTransA);
            } else {
                CopyUbAToL1(isTransA);
            }
        }
        if constexpr (PhyMxScalePosIsUB<A_TYPE>()) {
            CopyScaleUbAToL1(isTransScaleA);
        }
        if constexpr (GetPhyType(B_TYPE::pos) == Hardware::UB) {
            CopyUbBToL1(isTransB);
        }
        if constexpr (PhyMxScalePosIsUB<B_TYPE>()) {
            CopyScaleUbBToL1(isTransScaleB);
        }
        if constexpr (GetPhyType(BIAS_TYPE::pos) == Hardware::UB && ToMatmulConfig(MM_CFG).enableSetBias) {
            if (isBias) {
                CopyUbBiasToL1();
            }
        }
        if constexpr (isAnyInputFromUb) {
            CrossCoreSetFlag<INTRA_MODE, PIPE_MTE3>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::L1_L0AB_UB_L1));
        }
    }

    template <class T>
    __aicore__ inline LocalTensor<T> GetVecTensor(uint64_t addr, const uint64_t size)
    {
        LocalTensor<T> cLocal;
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::VECCALC);
        tbufOutTmp.bufferAddr = addr;
#if ASCENDC_CPU_DEBUG
        if (IsTypeOneOfV<T, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
            tbufOutTmp.dataLen = size / AscendC::Impl::FP4_TWO;
        } else {
            tbufOutTmp.dataLen = size * sizeof(T);
        }
        tbufOutTmp.absAddr = reinterpret_cast<uint8_t*>(addr);
#endif
        cLocal.SetAddr(tbufOutTmp);
        return cLocal;
    }

    template <class T>
    __aicore__ inline void NDPadZeros(
        LocalTensor<T>& dst, const int32_t height, const int32_t width, const int32_t gCol)
    {
        int32_t calcWidth = Ceil(width, c0Size_);
        if (gCol % BLOCK_CUBE) {
            int tail = width % c0Size_;
            // tail pad zero
            if (tail) {
                auto offset = width / c0Size_ * c0Size_;
                uint64_t mask[2];
                uint16_t mask_tail = ~((1 << tail) - 1);
                uint64_t masktail = mask_tail;
                mask[0] =
                    masktail + (masktail << NUM_SIXTEEN) + (masktail << NUM_THIRTYTWO) + (masktail << NUM_FORTYEIGHT);
                mask[1] = mask[0];
                int stride = calcWidth * (c0Size_ * sizeof(T) / DEFAULT_C0_SIZE);
                int32_t totalRep = Ceil(height, NUM_EIGHT);
                if (masktail != 0) {
                    // duplicate framework not support fp8/hif8, SE suggested pad by int8 0.
                    if constexpr (
                        IsSameType<T, hifloat8_t>::value || IsSameType<T, fp8_e5m2_t>::value ||
                        IsSameType<T, fp8_e4m3fn_t>::value) {
                        LocalTensor<int8_t> tmp = dst.template ReinterpretCast<int8_t>();
                        Duplicate(tmp[offset], (int8_t)0, mask, totalRep, stride, NUM_EIGHT * stride);
                    } else {
                        Duplicate(dst[offset], (T)0, mask, totalRep, stride, NUM_EIGHT * stride);
                    }
                }
            }
        }
        // If the value of high is not an integer multiple of 16, add 0.
        int tailHigh = height % BLOCK_CUBE;
        if (tailHigh) {
            auto dstOffset = height * calcWidth * BLOCK_CUBE;
            if constexpr (
                IsSameType<T, hifloat8_t>::value || IsSameType<T, fp8_e5m2_t>::value ||
                IsSameType<T, fp8_e4m3fn_t>::value) {
                // duplicate framework not support fp8/hif8, SE suggested pad by int8 0.
                LocalTensor<int8_t> tmp = dst.template ReinterpretCast<int8_t>();
                Duplicate(tmp[dstOffset], (int8_t)0, (BLOCK_CUBE - tailHigh) * calcWidth * BLOCK_CUBE);
            } else {
                Duplicate(dst[dstOffset], (T)0, (BLOCK_CUBE - tailHigh) * calcWidth * BLOCK_CUBE);
            }
        }
    }

    template <class T>
    __aicore__ inline void NDTrans2NZ(
        LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t calcHigh, const int32_t calcWidth)
    {
        // Use Muls, convert to NZ format
        struct UnaryRepeatParams intriParams;
        uint64_t mask[2] = {static_cast<uint64_t>(-1), static_cast<uint64_t>(-1)};
        intriParams.dstBlkStride = (BLOCK_CUBE * sizeof(T) / DEFAULT_C0_SIZE);
        intriParams.srcBlkStride = calcWidth * (BLOCK_CUBE * sizeof(T) / DEFAULT_C0_SIZE);
        intriParams.dstRepStride = intriParams.dstBlkStride * DEFAULT_BLK_NUM;
        intriParams.srcRepStride = intriParams.srcBlkStride * DEFAULT_BLK_NUM;
        int dstOffset = 0;
        int srcOffset = 0;
        // ensure rep stride be less than 256
        constexpr int maxSrcBlkStride = 32;
        constexpr int TWO = 2;
        if (intriParams.srcBlkStride >= maxSrcBlkStride) {
            intriParams.dstBlkStride = 1;
            intriParams.srcBlkStride = 1;
            mask[0] = (1 << BLOCK_CUBE) - 1;
            mask[1] = 0;
            for (int i = 0; i < calcWidth; i++) {
                for (int j = 0; j < calcHigh * BLOCK_CUBE; ++j) {
                    dstOffset = i * calcHigh * CUBE_MAX_SIZE + j * BLOCK_CUBE;
                    srcOffset = j * calcWidth * BLOCK_CUBE + i * BLOCK_CUBE;
                    Muls(dst[dstOffset], src[srcOffset], (T)1, mask, 1, intriParams);
                    if constexpr (sizeof(T) == sizeof(float)) {
                        Muls(dst[dstOffset + c0Size_], src[srcOffset + c0Size_], (T)1, mask, 1, intriParams);
                    }
                }
            }
        } else {
            for (int i = 0; i < calcWidth; i++) {
                dstOffset = i * calcHigh * CUBE_MAX_SIZE;
                srcOffset = i * BLOCK_CUBE;
                Muls(dst[dstOffset], src[srcOffset], (T)1, mask, TWO * calcHigh, intriParams);
                if constexpr (sizeof(T) == sizeof(float)) {
                    Muls(dst[dstOffset + c0Size_], src[srcOffset + c0Size_], (T)1, mask, TWO * calcHigh, intriParams);
                }
            }
        }
    }

    template <class T>
    __aicore__ inline void CopyNDBlock(
        const LocalTensor<T>& transTensor, const LocalTensor<T>& src, const int64_t srcOffset, const int32_t height,
        const int32_t width)
    {
        int srcStride = 0;
        int blockLen = Ceil(width, c0Size_) * c0Size_ * sizeof(T) / DEFAULT_C0_SIZE;
        uint16_t dstStride = 0;
        DataCopy(
            transTensor, src[srcOffset],
            {static_cast<uint16_t>(height), static_cast<uint16_t>(blockLen), static_cast<uint16_t>(srcStride),
             dstStride});
        auto enQueEvtID = GetTPipePtr()->FetchEventID(HardEvent::MTE2_V);
        SetFlag<HardEvent::MTE2_V>((event_t)enQueEvtID);
        WaitFlag<HardEvent::MTE2_V>((event_t)enQueEvtID);
    }

    template <class T, InputTypeTag TAG, typename INPUT_TYPE>
    __aicore__ inline void CopyUB2L1ND2NZ(
        LocalTensor<T>& dst, LocalTensor<T>& src, const int32_t row, const int32_t col, const int32_t gCol,
        bool isTrans, const int32_t dstHeight)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enVecND2NZ) {
            LocalTensor<T> srcTensor = localWorkspace_[0].template ReinterpretCast<T>();
            srcTensor.SetSize(cubeTiling.GetTransLength());
            CopyNDBlock(srcTensor, src, 0, row, col);
            LocalTensor<T> tmpBuffer = localWorkspace_[cubeTiling.GetTransLength()].template ReinterpretCast<T>();
            int64_t size = Ceil(row, BLOCK_CUBE) * BLOCK_CUBE * Ceil(col, c0Size_) * c0Size_;
            tmpBuffer.SetSize(cubeTiling.GetTransLength());
            CopyUBND2UBNZ(tmpBuffer, srcTensor, row, col, gCol);
            DataCopy(dst, tmpBuffer, size);
        } else {
            CopyND2NZOnTheFly<T, TAG, INPUT_TYPE>(dst, src, row, col, gCol, isTrans, dstHeight);
        }
    }

    // borrowed from the same func in data_copy_wrapper_using_ub_nd.h, with only small adjustments.
    template <class T, InputTypeTag TAG, typename INPUT_TYPE>
    __aicore__ inline void CopyND2NZOnTheFly(
        const LocalTensor<T>& dst, LocalTensor<T>& src, const int32_t height, const int32_t width, const int32_t gCol,
        bool isTrans, const int32_t dstHeight)
    {
        int tail = width % c0Size_;
        int calcWidthExr = Ceil(width, c0Size_);
        int calcHeightExr = Ceil(height, BLOCK_CUBE);

        if constexpr (!HasScalePosition<INPUT_TYPE>::value) {
            // set2d, pad tail zero
            if (height % BLOCK_CUBE != 0) {
                int64_t repeat = calcWidthExr * calcHeightExr;
                if constexpr (IsTypeOneOfV<T, int8_t, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t>) {
                    LocalTensor<int16_t> tmp = dst.template ReinterpretCast<int16_t>();
                    InitConstValueParams<int16_t> initConstValueParams;
                    initConstValueParams.repeatTimes = (uint16_t)repeat;
                    initConstValueParams.initValue = 0;
                    InitConstValue(tmp, initConstValueParams);
                } else {
                    InitConstValueParams<T> initConstValueParams;
                    initConstValueParams.repeatTimes = (uint16_t)repeat;
                    initConstValueParams.initValue = 0;
                    InitConstValue(dst, initConstValueParams);
                }

                event_t eventIDMte2ToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
                SetFlag<HardEvent::MTE2_MTE3>(eventIDMte2ToMte3);
                WaitFlag<HardEvent::MTE2_MTE3>(eventIDMte2ToMte3);
            }
        }
        // gCol unaligned, can not use dma copy repeat stride
        if (tail != 0) {
            if constexpr (!PhyMxScalePosIsUB<INPUT_TYPE>()) {
                CopyND2NZOnTheFlyWithTail<T, TAG>(dst, src, height, width, gCol, isTrans);
            }
        } else {
            CopyND2NZOnTheFlyWithoutTail<T, INPUT_TYPE>(dst, src, height, width, gCol, dstHeight);
        }
    }

    template <class T, InputTypeTag TAG>
    __aicore__ inline void CopyND2NZOnTheFlyWithTail(
        const LocalTensor<T>& dst, LocalTensor<T>& src, const int32_t height, const int32_t width, const int32_t gCol,
        bool isTrans)
    {
        int calcWidth = width / c0Size_; // cube block numbers that do not need to be pad zero
        int tail = width % c0Size_;
        int dstOffset = 0;
        int srcOffset = 0;
        int calcWidthExr = Ceil(width, c0Size_);
        int calcHeightExr = Ceil(height, BLOCK_CUBE);
        // tail elements that need to be pad zero
        int blockLen = calcWidthExr * (c0Size_ * sizeof(T) / DEFAULT_C0_SIZE);
        DataCopyEnhancedParams enhancedParams;
        enhancedParams.blockMode = BlockMode::BLOCK_MODE_VECTOR;

        // ub->l1
        int srcGap = gCol * sizeof(T) / ONE_BLK_SIZE - 1;
        int srcColOffset = Ceil(gCol, c0Size_) * c0Size_;
        if (gCol % c0Size_ || srcGap >= UINT16_MAX) {
            // each block len is only 32B
            for (int i = 0; i < calcWidth; i++) {
                for (int j = 0; j < height; j++) {
                    DataCopy(
                        dst[dstOffset + i * calcHeightExr * BLOCK_CUBE * c0Size_ + j * c0Size_],
                        src[srcOffset + j * srcColOffset + i * c0Size_], {1, 1, 0, 0}, enhancedParams);
                }
            }
        } else {
            // data copy stride is aligned
            for (int i = 0; i < calcWidth; i++) {
                DataCopy(
                    dst[dstOffset], src[srcOffset],
                    {static_cast<uint16_t>(height), 1, static_cast<uint16_t>(srcGap), 0}, enhancedParams);
                dstOffset += calcHeightExr * BLOCK_CUBE * c0Size_;
                srcOffset += c0Size_;
            }
        }

        // tail ub->ub pad zero, and then ub->l1
        int size = 0;
        if constexpr (TAG == InputTypeTag::A) {
            size = (isTrans ? singleCoreK_ * NUM_THIRTYTWO : singleCoreM_ * NUM_THIRTYTWO) / sizeof(T);
        } else {
            size = (isTrans ? singleCoreN_ * NUM_THIRTYTWO : singleCoreK_ * NUM_THIRTYTWO) / sizeof(T);
        }

        LocalTensor<T> trans = localWorkspace_.template ReinterpretCast<T>();
        trans.SetSize(size);

        TailPadZero(trans, src, height, width, gCol);

        // ub->l1
        int heightAlignBlock = Ceil(height, BLOCK_CUBE);
        int tailDstOffset = heightAlignBlock * BLOCK_CUBE * c0Size_ * calcWidth;
        DataCopy(dst[tailDstOffset], trans, {static_cast<uint16_t>(height), 1, 0, 0}, enhancedParams);
    }

    template <class T>
    __aicore__ inline void TailPadZero(
        const LocalTensor<T>& trans, const LocalTensor<T>& src, const int32_t height, const int32_t width,
        const int32_t gCol)
    {
        int calcWidth = width / c0Size_; // cube block numbers that do not need to be pad zero
        int tail = width % c0Size_;
        int tailSrcoffset = calcWidth * c0Size_;
        int srcColOffset = Ceil(gCol, c0Size_) * c0Size_;
        // ub->ub
        if constexpr (IsSupportB8<T>()) {
            // duplicate api not support fp8/hif8, SE suggested pad by int8 0.
            LocalTensor<int8_t> tmpTrans = trans.template ReinterpretCast<int8_t>();
            Duplicate(tmpTrans, static_cast<int8_t>(0), height * c0Size_);
            // adds api not support fp8/hif8, trans to int8.
            LocalTensor<int8_t> tmpSrc = src.template ReinterpretCast<int8_t>();
            for (int i = 0; i < height; i++) {
                Adds(tmpTrans[i * c0Size_], tmpSrc[tailSrcoffset], static_cast<int8_t>(0), tail);
                tailSrcoffset += srcColOffset;
            }
        } else {
            Duplicate(trans, static_cast<T>(0), height * c0Size_);
            for (int i = 0; i < height; i++) {
                Adds(trans[i * c0Size_], src[tailSrcoffset], static_cast<T>(0), tail);
                tailSrcoffset += srcColOffset;
            }
        }

        event_t eventIDVToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventIDVToMte3);
        WaitFlag<HardEvent::V_MTE3>(eventIDVToMte3);
    }

    template <class T, typename INPUT_TYPE>
    __aicore__ inline void CopyND2NZOnTheFlyWithoutTail(
        const LocalTensor<T>& dst, LocalTensor<T>& src, const int32_t height, const int32_t width, const int32_t gCol,
        const int32_t dstHeight)
    {
        int calcWidth = width / c0Size_; // cube block numbers that do not need to be pad zero
        int dstOffset = 0;
        int srcOffset = 0;
        int calcHeightExr = Ceil(height, BLOCK_CUBE);
        DataCopyEnhancedParams enhancedParams;
        enhancedParams.blockMode = BlockMode::BLOCK_MODE_VECTOR;

        int srcGap = gCol * sizeof(T) / ONE_BLK_SIZE - 1;
        if (gCol % c0Size_ || srcGap >= UINT16_MAX) {
            int oriSrcOffset = srcOffset;
            int oriDstOffset = dstOffset;
            // each block len is only 32B
            for (int i = 0; i < calcWidth; i++) {
                for (int j = 0; j < height; j++) {
                    DataCopy(dst[dstOffset], src[srcOffset], {1, 1, 0, 0}, enhancedParams);
                    dstOffset += c0Size_;
                    srcOffset += gCol;
                }
                srcOffset = oriSrcOffset + (i + 1) * c0Size_;
                dstOffset = oriDstOffset + (i + 1) * calcHeightExr * BLOCK_CUBE * c0Size_;
            }
        } else {
            // data copy stride is aligned
            int32_t repDstOffset;
            if constexpr (HasScalePosition<INPUT_TYPE>::value) {
                repDstOffset = dstHeight * c0Size_;
            } else {
                repDstOffset = calcHeightExr * BLOCK_CUBE * c0Size_;
            }
            constexpr int32_t maxRepTimes = 4095;
            int32_t mainHeight = height > maxRepTimes ? maxRepTimes : height;
            int32_t tailHeight = height % maxRepTimes;
            for (int i = 0; i < calcWidth; i++) {
                DataCopy(
                    dst[dstOffset], src[srcOffset],
                    {static_cast<uint16_t>(mainHeight), 1, static_cast<uint16_t>(srcGap), 0}, enhancedParams);
                dstOffset += repDstOffset;
                srcOffset += c0Size_;
            }
            if (tailHeight > 0 && height > maxRepTimes) {
                dstOffset = maxRepTimes * c0Size_;
                srcOffset = maxRepTimes * width;
                for (int i = 0; i < calcWidth; i++) {
                    DataCopy(
                        dst[dstOffset], src[srcOffset],
                        {static_cast<uint16_t>(tailHeight), 1, static_cast<uint16_t>(srcGap), 0}, enhancedParams);
                    dstOffset += repDstOffset;
                    srcOffset += c0Size_;
                }
            }
        }
    }

    template <class T>
    __aicore__ inline void CopyUBND2UBNZ(
        LocalTensor<T>& dst, LocalTensor<T>& src, const int32_t row, const int32_t col, const int32_t gCol)
    {
        int32_t calcHigh = Ceil(row, BLOCK_CUBE);
        int32_t calcWidth = Ceil(col, c0Size_);
        NDPadZeros(src, row, col, gCol);
        NDTrans2NZ(dst, src, calcHigh, calcWidth);
        event_t eventIdVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventIdVToMTE3);
        WaitFlag<HardEvent::V_MTE3>(eventIdVToMTE3);
    }

    template <class T>
    __aicore__ inline void CopyUB2L1NZ2NZ(
        const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t row, const int32_t col)
    {
        if constexpr (HasScalePosition<A_TYPE>::value || HasScalePosition<B_TYPE>::value) {
            if (IsTypeOneOfV<T, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
                DataCopy(dst, src, row * col / INT4_TWO);
            } else {
                DataCopy(dst, src, row * col);
            }
        } else {
            int32_t rowAlign = Ceil(row, BLOCK_CUBE) * BLOCK_CUBE;
            int32_t colAlign = Ceil(col, c0Size_) * c0Size_;
            DataCopy(dst, src, rowAlign * colAlign);
        }
    }

    __aicore__ inline void CopyUbAToL1ForND(LocalTensor<SrcAT>& leftMatrix, LocalTensor<SrcAT>& srcTensor, bool isTrans)
    {
        constexpr uint8_t multiOfB8b4 = 2;
        if (isTrans) {
            int32_t dstHeight;
            if constexpr (HasScalePosition<A_TYPE>::value) {
                if constexpr (IsSupportMxFp8<SrcAT>()) {
                    dstHeight = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
                    CopyUB2L1ND2NZ<SrcAT, InputTypeTag::A, A_TYPE>(
                        leftMatrix, srcTensor, singleCoreK_, singleCoreM_, singleCoreM_, isTrans, dstHeight);
                } else if constexpr (IsSupportMxFp4<SrcAT>()) {
                    dstHeight = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
                    auto leftMatrixFp8 = leftMatrix.template ReinterpretCast<fp8_e4m3fn_t>();
                    auto srcFp8 = srcTensor.template ReinterpretCast<fp8_e4m3fn_t>();
                    c0Size_ = AscendC::Impl::B8_C0SIZE;
                    CopyUB2L1ND2NZ<fp8_e4m3fn_t, InputTypeTag::A, A_TYPE>(
                        leftMatrixFp8, srcFp8, singleCoreK_, singleCoreM_ / multiOfB8b4, singleCoreM_ / multiOfB8b4,
                        isTrans, dstHeight);
                }
            } else {
                dstHeight = singleCoreK_;
                CopyUB2L1ND2NZ<SrcAT, InputTypeTag::A, A_TYPE>(
                    leftMatrix, srcTensor, singleCoreK_, singleCoreM_, singleCoreM_, isTrans, dstHeight);
            }
        } else {
            if constexpr (IsSupportMxFp4<SrcAT>()) {
                auto leftMatrixFp8 = leftMatrix.template ReinterpretCast<fp8_e4m3fn_t>();
                auto srcFp8 = srcTensor.template ReinterpretCast<fp8_e4m3fn_t>();
                c0Size_ = AscendC::Impl::B8_C0SIZE;
                CopyUB2L1ND2NZ<fp8_e4m3fn_t, InputTypeTag::A, A_TYPE>(
                    leftMatrixFp8, srcFp8, singleCoreM_, singleCoreK_ / multiOfB8b4, singleCoreK_ / multiOfB8b4,
                    isTrans, Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE);
            } else {
                CopyUB2L1ND2NZ<SrcAT, InputTypeTag::A, A_TYPE>(
                    leftMatrix, srcTensor, singleCoreM_, singleCoreK_, singleCoreK_, isTrans,
                    Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE);
            }
        }
    }

    __aicore__ inline int32_t ComputeAL1Size()
    {
        int32_t orgHeightAlign;
        int32_t orgWidthAlign;
        if constexpr (IsSupportMxFp4<SrcAT>()) {
            c0Size_ = AscendC::Impl::B4_C0SIZE;
        }
        if constexpr (A_TYPE::isTrans) {
            orgHeightAlign = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
            orgWidthAlign = Ceil(singleCoreM_, c0Size_) * c0Size_;
        } else {
            orgHeightAlign = Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE;
            orgWidthAlign = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
        }
        if constexpr (IsSupportMxFp4<SrcAT>()) {
            return orgHeightAlign * orgWidthAlign / AscendC::Impl::FP4_TWO;
        } else {
            return orgHeightAlign * orgWidthAlign * sizeof(SrcAT);
        }
    }

    template <class T>
    __aicore__ inline void UbASizeCheck()
    {
#if ASCENDC_CPU_DEBUG
        int32_t innerDim = CeilAlign(singleCoreK_, c0Size_);
        int32_t outDim = singleCoreM_;
        int32_t srcUbSize = sizeAmatrix_;
        if constexpr (A_TYPE::isTrans) {
            innerDim = CeilAlign(singleCoreM_, c0Size_);
            outDim = singleCoreK_;
        }
        int32_t minAUbSize = outDim * innerDim;
        if constexpr (IsTypeOneOfV<T, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
            srcUbSize = srcUbSize / AscendC::Impl::FP4_TWO;
            minAUbSize = minAUbSize / AscendC::Impl::FP4_TWO;
        }
        ASCENDC_ASSERT((srcUbSize >= minAUbSize), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "The width of matrix A on UB should be 32Byte aligned, and The expected size is height * "
                "CeilAlign(width, 32B) = %d Byte; currently, it is %d Byte.",
                minAUbSize, srcUbSize);
        });
#endif
    }

    template <class T>
    __aicore__ inline void UbBSizeCheck()
    {
#if ASCENDC_CPU_DEBUG
        int32_t innerDim = CeilAlign(singleCoreN_, c0Size_);
        int32_t outDim = singleCoreK_;
        int32_t srcUbSize = sizeBmatrix_;
        if constexpr (B_TYPE::isTrans) {
            innerDim = CeilAlign(singleCoreK_, c0Size_);
            outDim = singleCoreN_;
        }
        int32_t minBUbSize = outDim * innerDim;
        if constexpr (IsTypeOneOfV<T, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
            srcUbSize = srcUbSize / AscendC::Impl::FP4_TWO;
            minBUbSize = minBUbSize / AscendC::Impl::FP4_TWO;
        }
        ASCENDC_ASSERT((srcUbSize >= minBUbSize), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "The width of matrix B on UB should be 32Byte aligned, and The expected size is height * "
                "CeilAlign(width, 32B) = %d Byte; currently, it is %d Byte.",
                minBUbSize, srcUbSize);
        });
#endif
    }

    __aicore__ inline void CopyUbAToL1(bool isTrans)
    {
        c0Size_ = AscendCUtils::GetC0Count(sizeof(SrcAT));
        auto bitSize = AscendC::GetBitSize<SrcAT>();
        int32_t orgHeightAlign = (IsNeedC0Align<SrcAT>() && A_TYPE::isTrans) ?
                                     Ceil(singleCoreM_, c0Size_) * c0Size_ :
                                     Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE;
        int32_t orgWidthAlign = (IsTypeOneOfV<SrcAT, float> || IsNeedC0Align<SrcAT>()) ?
                                    Ceil(singleCoreK_, c0Size_) * c0Size_ :
                                    Ceil(singleCoreK_, BLOCK_CUBE) * BLOCK_CUBE;
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::A1);
        if constexpr (HasScalePosition<A_TYPE>::value && A_TYPE::format == CubeFormat::ND) {
            UbASizeCheck<SrcAT>();
            tbufOutTmp.dataLen = ComputeAL1Size();
        } else {
            tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign * bitSize;
        }

        tbufOutTmp.bufferAddr = kfcMsg_.body.aAddr;
#if ASCENDC_CPU_DEBUG
        tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::A1)) + kfcMsg_.body.aAddr;
#endif

        LocalTensor<SrcAT> leftMatrix;
        leftMatrix.SetAddr(tbufOutTmp);
        LocalTensor<SrcAT> srcTensor = GetVecTensor<SrcAT>(aAddr_, sizeAmatrix_ / sizeof(SrcAT));
        if constexpr (A_TYPE::format == CubeFormat::ND) {
            CopyUbAToL1ForND(leftMatrix, srcTensor, isTrans);
        } else if constexpr (A_TYPE::format == CubeFormat::NZ) {
            if (isTrans) {
                CopyUB2L1NZ2NZ(leftMatrix, srcTensor, singleCoreK_, singleCoreM_);
            } else {
                CopyUB2L1NZ2NZ(leftMatrix, srcTensor, singleCoreM_, singleCoreK_);
            }
        } else if constexpr (A_TYPE::format == CubeFormat::VECTOR) {
            if (isTrans) {
                ASCENDC_ASSERT((!isTrans), { KERNEL_LOG(KERNEL_ERROR, "A vector does not support transpose."); });
                return;
            }
            DataCopy(leftMatrix, srcTensor[0], {1, static_cast<uint16_t>(Ceil(singleCoreK_, c0Size_)), 0, 0});
        }
    }

    __aicore__ inline void CopyUbAToL1StaticTiling(bool isTrans)
    {
        c0Size_ = AscendCUtils::GetC0Count(sizeof(SrcAT));
        auto bitSize = AscendC::GetBitSize<SrcAT>();
        int32_t orgHeightAlign = (IsNeedC0Align<SrcAT>() && A_TYPE::isTrans) ?
                                     Ceil(ToMatmulConfig(MM_CFG).singleCoreM, c0Size_) * c0Size_ :
                                     Ceil(ToMatmulConfig(MM_CFG).singleCoreM, BLOCK_CUBE) * BLOCK_CUBE;
        int32_t orgWidthAlign = (IsTypeOneOfV<SrcAT, float> || IsNeedC0Align<SrcAT>()) ?
                                    Ceil(ToMatmulConfig(MM_CFG).singleCoreK, c0Size_) * c0Size_ :
                                    Ceil(ToMatmulConfig(MM_CFG).singleCoreK, BLOCK_CUBE) * BLOCK_CUBE;
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::A1);
        tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign * bitSize;
        tbufOutTmp.bufferAddr = kfcMsg_.body.aAddr;
#if ASCENDC_CPU_DEBUG
        tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::A1));
#endif

        LocalTensor<SrcAT> leftMatrix;
        leftMatrix.SetAddr(tbufOutTmp);
        LocalTensor<SrcAT> srcTensor = GetVecTensor<SrcAT>(aAddr_, sizeAmatrix_ / bitSize);
        if constexpr (A_TYPE::format == CubeFormat::ND) {
            if (isTrans) {
                CopyUB2L1ND2NZ<SrcAT, InputTypeTag::A, A_TYPE>(
                    leftMatrix, srcTensor, ToMatmulConfig(MM_CFG).singleCoreK, ToMatmulConfig(MM_CFG).singleCoreM,
                    singleCoreM_, isTrans);
            } else {
                CopyUB2L1ND2NZ<SrcAT, InputTypeTag::A, A_TYPE>(
                    leftMatrix, srcTensor, ToMatmulConfig(MM_CFG).singleCoreM, ToMatmulConfig(MM_CFG).singleCoreK,
                    singleCoreK_, isTrans);
            }
        } else if constexpr (A_TYPE::format == CubeFormat::NZ) {
            if (isTrans) {
                CopyUB2L1NZ2NZ(
                    leftMatrix, srcTensor, ToMatmulConfig(MM_CFG).singleCoreK, ToMatmulConfig(MM_CFG).singleCoreM);
            } else {
                CopyUB2L1NZ2NZ(
                    leftMatrix, srcTensor, ToMatmulConfig(MM_CFG).singleCoreM, ToMatmulConfig(MM_CFG).singleCoreK);
            }
        } else if constexpr (A_TYPE::format == CubeFormat::VECTOR) {
            if (isTrans) {
                ASCENDC_ASSERT((!isTrans), { KERNEL_LOG(KERNEL_ERROR, "A vector does not support transpose."); });
                return;
            }
            DataCopy(leftMatrix, srcTensor[0], {1, static_cast<uint16_t>(Ceil(singleCoreK_, c0Size_)), 0, 0});
        }
    }

    __aicore__ inline void CopyUbBToL1ForND(
        LocalTensor<SrcBT>& rightMatrix, LocalTensor<SrcBT>& srcTensor, bool isTrans)
    {
        constexpr uint8_t multiOfB8b4 = 2;
        if (isTrans) {
            if constexpr (IsSupportMxFp4<SrcBT>()) {
                auto rightMatrixFp8 = rightMatrix.template ReinterpretCast<fp8_e4m3fn_t>();
                auto srcFp8 = srcTensor.template ReinterpretCast<fp8_e4m3fn_t>();
                c0Size_ = AscendC::Impl::B8_C0SIZE;
                CopyUB2L1ND2NZ<fp8_e4m3fn_t, InputTypeTag::B, B_TYPE>(
                    rightMatrixFp8, srcFp8, singleCoreN_, singleCoreK_ / multiOfB8b4, singleCoreK_ / multiOfB8b4,
                    isTrans, Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE);
            } else {
                CopyUB2L1ND2NZ<SrcBT, InputTypeTag::B, B_TYPE>(
                    rightMatrix, srcTensor, singleCoreN_, singleCoreK_, singleCoreK_, isTrans,
                    Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE);
            }
        } else {
            int32_t dstHeight;
            if constexpr (HasScalePosition<B_TYPE>::value) {
                if constexpr (IsSupportMxFp8<SrcBT>()) {
                    dstHeight = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
                    CopyUB2L1ND2NZ<SrcBT, InputTypeTag::B, B_TYPE>(
                        rightMatrix, srcTensor, singleCoreK_, singleCoreN_, singleCoreN_, isTrans, dstHeight);
                } else if constexpr (IsSupportMxFp4<SrcBT>()) {
                    dstHeight = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
                    auto rightMatrixFp8 = rightMatrix.template ReinterpretCast<fp8_e4m3fn_t>();
                    auto srcFp8 = srcTensor.template ReinterpretCast<fp8_e4m3fn_t>();
                    c0Size_ = AscendC::Impl::B8_C0SIZE;
                    CopyUB2L1ND2NZ<fp8_e4m3fn_t, InputTypeTag::B, B_TYPE>(
                        rightMatrixFp8, srcFp8, singleCoreK_, singleCoreN_ / multiOfB8b4, singleCoreN_ / multiOfB8b4,
                        isTrans, dstHeight);
                }
            } else {
                dstHeight = singleCoreK_;
                CopyUB2L1ND2NZ<SrcBT, InputTypeTag::B, B_TYPE>(
                    rightMatrix, srcTensor, singleCoreK_, singleCoreN_, singleCoreN_, isTrans, dstHeight);
            }
        }
    }

    __aicore__ inline void CopyUbBToL1(bool isTrans)
    {
        c0Size_ = AscendCUtils::GetC0Count(sizeof(SrcBT));
        auto bitSize = AscendC::GetBitSize<SrcBT>();
        int32_t orgHeightAlign = (IsNeedC0Align<SrcBT>()) ? Ceil(singleCoreK_, c0Size_) * c0Size_ :
                                                            Ceil(singleCoreK_, BLOCK_CUBE) * BLOCK_CUBE;
        int32_t orgWidthAlign = (IsSameTypeV<SrcBT, float> || (IsNeedC0Align<SrcBT>() && !B_TYPE::isTrans)) ?
                                    Ceil(singleCoreN_, c0Size_) * c0Size_ :
                                    Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE;
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::B1);
        if constexpr (HasScalePosition<B_TYPE>::value && B_TYPE::format == CubeFormat::ND) {
            UbBSizeCheck<SrcBT>();
            if constexpr (IsSupportMxFp4<SrcBT>()) {
                c0Size_ = AscendC::Impl::B4_C0SIZE;
            }
            if constexpr (B_TYPE::isTrans) {
                orgHeightAlign = Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE;
                orgWidthAlign = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
            } else {
                orgHeightAlign = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * AscendC::Impl::MX_BASEK_FACTOR;
                orgWidthAlign = Ceil(singleCoreN_, c0Size_) * c0Size_;
            }
            if constexpr (IsSupportMxFp4<SrcBT>()) {
                tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign / AscendC::Impl::FP4_TWO;
            } else {
                tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign * sizeof(SrcBT);
            }
        } else {
            tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign * bitSize;
        }

        tbufOutTmp.bufferAddr = kfcMsg_.body.bAddr;
#if ASCENDC_CPU_DEBUG
        tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::B1)) + kfcMsg_.body.bAddr;
#endif
        LocalTensor<SrcBT> rightMatrix;
        rightMatrix.SetAddr(tbufOutTmp);
        LocalTensor<SrcBT> srcTensor = GetVecTensor<SrcBT>(bAddr_, sizeBmatrix_ / sizeof(SrcBT));
        if constexpr (B_TYPE::format == CubeFormat::ND) {
            CopyUbBToL1ForND(rightMatrix, srcTensor, isTrans);
        } else if constexpr (B_TYPE::format == CubeFormat::NZ) {
            if (isTrans) {
                CopyUB2L1NZ2NZ(rightMatrix, srcTensor, singleCoreN_, singleCoreK_);
            } else {
                CopyUB2L1NZ2NZ(rightMatrix, srcTensor, singleCoreK_, singleCoreN_);
            }
        }
    }

    __aicore__ inline void CopyUbBiasToL1()
    {
        c0Size_ = AscendCUtils::GetC0Count(sizeof(BiasT));
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::B1);
        tbufOutTmp.dataLen = Ceil(singleCoreM_ * sizeof(BiasT), ONE_BLK_SIZE);
        tbufOutTmp.bufferAddr = kfcMsg_.body.biasAddr;
#if ASCENDC_CPU_DEBUG
        tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::B1)) + kfcMsg_.body.biasAddr;
#endif

        LocalTensor<BiasT> biasMatrix;
        biasMatrix.SetAddr(tbufOutTmp);
        LocalTensor<BiasT> bias = GetVecTensor<BiasT>(biasAddr_, sizeBiasmatrix_ / sizeof(BiasT));
        DataCopy(biasMatrix, bias, {1, static_cast<uint16_t>(Ceil(singleCoreN_, c0Size_)), 0, 0});
    }

    __aicore__ inline void TransDataForScale(
        const LocalTensor<uint16_t>& dstU16, const LocalTensor<uint16_t>& srcU16, const uint32_t h, const uint32_t w)
    {
        constexpr int32_t dftTransData5hdLen = 16;
        constexpr int32_t h0 = 16;
        constexpr int32_t w0 = 16;
        int32_t h1 = Ceil(h, h0);
        int32_t curH = h;
        TransDataTo5HDParams transDataParams;
        transDataParams.dstHighHalf = false;
        transDataParams.srcHighHalf = false;
        transDataParams.repeatTimes = Ceil(w, w0);
        if (transDataParams.repeatTimes == 1) {
            transDataParams.srcRepStride = 0;
            transDataParams.dstRepStride = 0;
        } else {
            transDataParams.dstRepStride = h0;
            transDataParams.srcRepStride = 1;
        }
        uint64_t dstLocalList[dftTransData5hdLen];
        uint64_t srcLocalList[dftTransData5hdLen];
        uint64_t srcAddr = (uint64_t)srcU16.GetPhyAddr();
        uint64_t dstAddr = (uint64_t)dstU16.GetPhyAddr();
        for (int j = 0; j < h1; j++) {
            for (int i = 0; i < dftTransData5hdLen; i++) {
                dstLocalList[i] = (uint64_t)(dstAddr + (j * h0 * w + w0 * i) * sizeof(uint16_t));
                if (i < curH) {
                    srcLocalList[i] = (uint64_t)(srcAddr + (j * h0 * w + w * i) * sizeof(uint16_t));
                } else {
                    srcLocalList[i] = srcAddr;
                }
            }
            TransDataTo5HD<uint16_t>(dstLocalList, srcLocalList, transDataParams);
            curH -= h0;
        }
    }

    __aicore__ inline void ND2ScaleZZ(
        const LocalTensor<uint8_t>& l1Matrix, const LocalTensor<uint8_t>& src, uint32_t height, uint32_t width,
        uint32_t scaleK, uint32_t offset)
    {
        auto dst = localWorkspace_[offset].template ReinterpretCast<uint8_t>();
        LocalTensor<uint16_t> dstTmpBuff = dst.template ReinterpretCast<uint16_t>();
        LocalTensor<uint16_t> srcTmpBuff = src.template ReinterpretCast<uint16_t>();
        constexpr uint32_t factor = 2;
        uint32_t b16W = width / factor;
        TransDataForScale(dstTmpBuff, srcTmpBuff, height, b16W);
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        LocalTensor<uint8_t> dstU8 = dstTmpBuff.template ReinterpretCast<uint8_t>();
        uint16_t actualW = scaleK / factor;
        uint16_t blockCount = Ceil(height, BLOCK_CUBE);
        uint16_t blockLen = actualW;
        uint16_t srcGap = b16W - actualW;
        uint16_t dstGap = 0;
        DataCopyParams dataCopyParams = {blockCount, blockLen, srcGap, dstGap};
        DataCopy(l1Matrix, dstU8, dataCopyParams);
    }

    __aicore__ inline void CopyUbNZ2NZForScale(
        const LocalTensor<uint8_t>& l1Matrix, const LocalTensor<uint8_t>& src, uint32_t height, uint32_t width,
        uint32_t offset)
    {
        uint32_t dstSize =
            Ceil(height, BLOCK_CUBE) * BLOCK_CUBE * Ceil(width, BLOCK_CUBE) * BLOCK_CUBE * sizeof(uint8_t);
        auto dst = localWorkspace_[offset].template ReinterpretCast<uint8_t>();
        dst.SetSize(dstSize);
        constexpr int32_t factor = 2;
        uint32_t b16H = height / factor;
        uint32_t srcOffset = 0;
        uint32_t dstOffset = 0;
        constexpr uint32_t ONE_BLK_ELEMS = 16;
        LocalTensor<uint16_t> dstTmpBuff = dst.template ReinterpretCast<uint16_t>();
        LocalTensor<uint16_t> srcTmpBuff = src.template ReinterpretCast<uint16_t>();
        uint16_t repeat = b16H;
        uint16_t blkLen = 1;
        uint16_t srcGap = (width / ONE_BLK_ELEMS) - 1;
        uint16_t dstGap = 0;
        uint16_t wAlignRep = width / ONE_BLK_ELEMS;
        DataCopyParams copyParams{repeat, blkLen, srcGap, dstGap};
        for (int32_t i = 0; i < wAlignRep; i++) {
            srcOffset = i * ONE_BLK_ELEMS;
            dstOffset = i * ONE_BLK_ELEMS * b16H;
            DataCopy(dstTmpBuff[dstOffset], srcTmpBuff[srcOffset], copyParams);
        }
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        LocalTensor<uint8_t> dstU8 = dstTmpBuff.template ReinterpretCast<uint8_t>();
        CopyUB2L1NZ2NZ(l1Matrix, dstU8, height, width);
    }

    __aicore__ inline void ScaleAUbSizeCheck(const LocalTensor<uint8_t>& srcTensor, int32_t scaleK)
    {
#if ASCENDC_CPU_DEBUG
        int32_t innerDim = CeilAlign(scaleK, c0Size_);
        int32_t outDim = singleCoreM_;
        if constexpr (A_TYPE::isScaleTrans) {
            innerDim = CeilAlign(singleCoreM_, c0Size_);
            outDim = scaleK;
        }
        int32_t minScaleAUbSize = outDim * innerDim;
        ASCENDC_ASSERT((srcTensor.GetSize() >= minScaleAUbSize), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "The width of matrix scaleA on UB should be 32Byte aligned, and The expected size is height * "
                "CeilAlign(width, 32B) = %d Byte; currently, it is %d Byte.",
                minScaleAUbSize, srcTensor.GetSize());
        });
#endif
    }

    __aicore__ inline void ScaleBUbSizeCheck(const LocalTensor<uint8_t>& srcTensor, int32_t scaleK)
    {
#if ASCENDC_CPU_DEBUG
        int32_t innerDim = CeilAlign(singleCoreN_, c0Size_);
        int32_t outDim = scaleK;
        if constexpr (B_TYPE::isScaleTrans) {
            innerDim = CeilAlign(scaleK, c0Size_);
            outDim = singleCoreN_;
        }
        int32_t minScaleBUbSize = outDim * innerDim;
        ASCENDC_ASSERT((srcTensor.GetSize() >= minScaleBUbSize), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "The width of matrix scaleB on UB should be 32Byte aligned, and The expected size is height * "
                "CeilAlign(width, 32B) = %d Byte; currently, it is %d Byte.",
                minScaleBUbSize, srcTensor.GetSize());
        });
#endif
    }

    __aicore__ inline void CopyScaleUbAToL1(bool isTrans)
    {
        int32_t scaleK = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * 2;
        c0Size_ = AscendCUtils::GetC0Count(sizeof(ScaleT));
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::A1);
        if constexpr (A_TYPE::scaleFormat == CubeFormat::ND) {
            int32_t orgHeightAlign;
            int32_t orgWidthAlign;
            if (isTrans) {
                orgHeightAlign = scaleK;
                orgWidthAlign = Ceil(singleCoreM_, c0Size_) * c0Size_;
            } else {
                orgHeightAlign = Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE;
                orgWidthAlign = Ceil(scaleK, c0Size_) * c0Size_;
            }
            tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign * sizeof(ScaleT);
        } else {
            tbufOutTmp.dataLen = Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE * Ceil(singleCoreK_, BLOCK_CUBE) *
                                 BLOCK_CUBE * sizeof(ScaleT);
        }

        tbufOutTmp.bufferAddr = kfcMsg_.body.quantAddr;
#if ASCENDC_CPU_DEBUG
        tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::A1)) + tbufOutTmp.bufferAddr;
#endif
        LocalTensor<ScaleT> leftMatrix;
        leftMatrix.SetAddr(tbufOutTmp);
        LocalTensor<ScaleT> srcTensor = GetVecTensor<ScaleT>(aScaleAddr_, sizeScaleAmatrix_ / sizeof(ScaleT));
        if constexpr (PhyMxScalePosIsUB<A_TYPE>()) {
            if constexpr (A_TYPE::scaleFormat == CubeFormat::NZ) {
                if (isTrans) {
                    CopyUB2L1NZ2NZ(leftMatrix, srcTensor, singleCoreK_ / NUM_THIRTYTWO, singleCoreM_);
                } else {
                    CopyUB2L1NZ2NZ(leftMatrix, srcTensor, singleCoreM_, singleCoreK_ / NUM_THIRTYTWO);
                }
            } else if constexpr (A_TYPE::scaleFormat == CubeFormat::VECTOR) {
                DataCopy(
                    leftMatrix, srcTensor,
                    {1, static_cast<uint16_t>(Ceil(singleCoreK_ / NUM_THIRTYTWO, c0Size_)), 0, 0});
            } else if constexpr (A_TYPE::scaleFormat == CubeFormat::ND) {
                if constexpr (SupportType<ScaleT, fp8_e8m0_t>()) {
                    ScaleAUbSizeCheck(reinterpret_cast<LocalTensor<uint8_t>&>(srcTensor), scaleK);
                } else {
                    ScaleAUbSizeCheck(srcTensor, scaleK);
                }
                if (isTrans) {
                    if constexpr (SupportType<ScaleT, fp8_e8m0_t>()) {
                        CopyUbNZ2NZForScale(
                            reinterpret_cast<LocalTensor<uint8_t>&>(leftMatrix),
                            reinterpret_cast<LocalTensor<uint8_t>&>(srcTensor), scaleK,
                            Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE, scaleTmpBufSize_);
                    } else {
                        CopyUbNZ2NZForScale(
                            leftMatrix, srcTensor, scaleK, Ceil(singleCoreM_, BLOCK_CUBE) * BLOCK_CUBE,
                            scaleTmpBufSize_);
                    }
                } else {
                    if constexpr (SupportType<ScaleT, fp8_e8m0_t>()) {
                        ND2ScaleZZ(
                            reinterpret_cast<LocalTensor<uint8_t>&>(leftMatrix),
                            reinterpret_cast<LocalTensor<uint8_t>&>(srcTensor), singleCoreM_,
                            Ceil(scaleK, c0Size_) * c0Size_, scaleK, scaleTmpBufSize_);
                    } else {
                        ND2ScaleZZ(
                            leftMatrix, srcTensor, singleCoreM_, Ceil(scaleK, c0Size_) * c0Size_, scaleK,
                            scaleTmpBufSize_);
                    }
                }
                scaleTmpBufSize_ += isTrans ? (Ceil(singleCoreM_, c0Size_) * c0Size_ * scaleK) :
                                              (Ceil(scaleK, c0Size_) * c0Size_ * singleCoreM_);
            }
        }
    }

    __aicore__ inline void CopyScaleUbBToL1(bool isBTrans)
    {
        int32_t scaleK = Ceil(singleCoreK_, AscendC::Impl::MX_BASEK_FACTOR) * 2;
        c0Size_ = AscendCUtils::GetC0Count(sizeof(ScaleT));
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(TPosition::B1);
        if constexpr (B_TYPE::scaleFormat == CubeFormat::ND) {
            int32_t orgHeightAlign;
            int32_t orgWidthAlign;
            if (isBTrans) {
                orgHeightAlign = Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE;
                orgWidthAlign = Ceil(scaleK, c0Size_) * c0Size_;
            } else {
                orgHeightAlign = Ceil(scaleK, BLOCK_CUBE) * BLOCK_CUBE;
                orgWidthAlign = Ceil(singleCoreN_, c0Size_) * c0Size_;
            }
            tbufOutTmp.dataLen = orgHeightAlign * orgWidthAlign * sizeof(ScaleT);
        } else {
            tbufOutTmp.dataLen = Ceil(singleCoreK_ / NUM_THIRTYTWO, BLOCK_CUBE) * BLOCK_CUBE *
                                 Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE * sizeof(ScaleT);
        }
        tbufOutTmp.bufferAddr = kfcMsg_.body.quantScalar;
#if ASCENDC_CPU_DEBUG
        tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::B1)) + tbufOutTmp.bufferAddr;
#endif
        LocalTensor<ScaleT> rightMatrix;
        rightMatrix.SetAddr(tbufOutTmp);
        LocalTensor<ScaleT> srcTensor = GetVecTensor<ScaleT>(bScaleAddr_, sizeScaleBmatrix_ / sizeof(ScaleT));
        if constexpr (PhyMxScalePosIsUB<B_TYPE>()) {
            if constexpr (B_TYPE::scaleFormat == CubeFormat::NZ) {
                if (isBTrans) {
                    CopyUB2L1NZ2NZ(rightMatrix, srcTensor, singleCoreN_, singleCoreK_ / NUM_THIRTYTWO);
                } else {
                    CopyUB2L1NZ2NZ(rightMatrix, srcTensor, singleCoreK_ / NUM_THIRTYTWO, singleCoreN_);
                }
            } else if constexpr (B_TYPE::scaleFormat == CubeFormat::ND) {
                if constexpr (SupportType<ScaleT, fp8_e8m0_t>()) {
                    ScaleBUbSizeCheck(reinterpret_cast<LocalTensor<uint8_t>&>(srcTensor), scaleK);
                } else {
                    ScaleBUbSizeCheck(srcTensor, scaleK);
                }
                if (isBTrans) {
                    if constexpr (SupportType<ScaleT, fp8_e8m0_t>()) {
                        ND2ScaleZZ(
                            reinterpret_cast<LocalTensor<uint8_t>&>(rightMatrix),
                            reinterpret_cast<LocalTensor<uint8_t>&>(srcTensor), singleCoreN_,
                            Ceil(scaleK, c0Size_) * c0Size_, scaleK, scaleTmpBufSize_);
                    } else {
                        ND2ScaleZZ(
                            rightMatrix, srcTensor, singleCoreN_, Ceil(scaleK, c0Size_) * c0Size_, scaleK,
                            scaleTmpBufSize_);
                    }
                } else {
                    if constexpr (SupportType<ScaleT, fp8_e8m0_t>()) {
                        CopyUbNZ2NZForScale(
                            reinterpret_cast<LocalTensor<uint8_t>&>(rightMatrix),
                            reinterpret_cast<LocalTensor<uint8_t>&>(srcTensor), scaleK,
                            Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE, scaleTmpBufSize_);
                    } else {
                        CopyUbNZ2NZForScale(
                            rightMatrix, srcTensor, scaleK, Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE,
                            scaleTmpBufSize_);
                    }
                }
                scaleTmpBufSize_ +=
                    isBTrans ? (Ceil(singleCoreN_, BLOCK_CUBE) * BLOCK_CUBE * Ceil(scaleK, c0Size_) * c0Size_) :
                               (scaleK * Ceil(singleCoreN_, c0Size_) * c0Size_);
            }
        }
    }
#endif

    // height width in unit of element
    template <class T, class U, bool sync = true>
    __aicore__ inline void CopyToUBPad(
        const LocalTensor<T>& data, const __gm__ U* addr, uint32_t height = 0, uint32_t width = 0, uint32_t srcGap = 0,
        uint32_t dstGap = 0)
    {
        ASSERT(
            C_TYPE::format == CubeFormat::ND_ALIGN &&
            "Only support padding in ND_ALIGN mode, please check template param of GetTensorC.");

        DataCopyParams copyParams{
            static_cast<uint16_t>(height), static_cast<uint16_t>(width * sizeof(T)), static_cast<uint16_t>(srcGap),
            static_cast<uint16_t>(dstGap)};
        DataCopyPadParams padParams{
            true, 0,
            static_cast<uint8_t>(
                ConstCeil(width, AscendCUtils::GetC0Count(sizeof(T))) * AscendCUtils::GetC0Count(sizeof(T)) - width),
            0};
        GlobalTensor<T> globalTensor;
        globalTensor.SetGlobalBuffer((__gm__ T*)addr);
        DataCopyPad(data, globalTensor, copyParams, padParams);

        if constexpr (sync) {
            event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
            SetFlag<HardEvent::MTE2_V>(eventID);
            WaitFlag<HardEvent::MTE2_V>(eventID);
        }
    }

    template <class T, class U, bool sync = true>
    __aicore__ inline void CopyToUBBatch(const LocalTensor<T>& data, const __gm__ U* addr, uint32_t batchC)
    {
        GlobalTensor<T> globalTensor;
        globalTensor.SetGlobalBuffer((__gm__ T*)addr);
        if constexpr (C_TYPE::format == CubeFormat::ND_ALIGN) {
            CopyToUBBatchNDAlign(data, globalTensor, batchC);
        } else if constexpr (
            C_TYPE::layout == LayoutMode::BNGS1S2 || C_TYPE::layout == LayoutMode::NORMAL ||
            C_TYPE::format == CubeFormat::NZ) {
            uint32_t size = batchC * cubeTiling.GetSingleCoreM() * cubeTiling.GetSingleCoreN();
            DataCopy(data, globalTensor, size);
        } else if constexpr (C_TYPE::format == CubeFormat::ND) { // BSNGD SBNGD
            struct DataCopyParams repeatParams;
            repeatParams.blockCount = cubeTiling.GetSingleCoreM();
            repeatParams.blockLen = batchC * cubeTiling.GetSingleCoreN() * sizeof(T) / ONE_BLK_SIZE;
            if (C_TYPE::layout == LayoutMode::BSNGD) {
                repeatParams.srcGap = (cubeTiling.GetCLayoutInfoN() * cubeTiling.GetCLayoutInfoG() - batchC) *
                                      cubeTiling.GetSingleCoreN() * sizeof(T) / ONE_BLK_SIZE;
            } else if (C_TYPE::layout == LayoutMode::SBNGD) {
                repeatParams.srcGap =
                    (cubeTiling.GetCLayoutInfoB() * cubeTiling.GetCLayoutInfoN() * cubeTiling.GetCLayoutInfoG() -
                     batchC) *
                    cubeTiling.GetSingleCoreN() * sizeof(T) / ONE_BLK_SIZE;
            }
            repeatParams.dstGap = repeatParams.srcGap;
            DataCopy(data, globalTensor, repeatParams);
        }

        if constexpr (sync) {
            event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
            SetFlag<HardEvent::MTE2_V>(eventID);
            WaitFlag<HardEvent::MTE2_V>(eventID);
        }
    }

    template <class T>
    __aicore__ inline void CopyToUBBatchNDAlign(
        const LocalTensor<T>& data, const GlobalTensor<T> globalTensor, uint32_t batchC)
    {
        int32_t alignedSingleCoreN = ConstCeil(cubeTiling.GetSingleCoreN(), AscendCUtils::GetC0Count(sizeof(T))) *
                                     AscendCUtils::GetC0Count(sizeof(T));
        int32_t offset;
        if constexpr (C_TYPE::layout == LayoutMode::BNGS1S2 || C_TYPE::layout == LayoutMode::NORMAL) {
            offset = cubeTiling.GetSingleCoreM() * alignedSingleCoreN;
        } else if constexpr (C_TYPE::layout == LayoutMode::BSNGD || C_TYPE::layout == LayoutMode::SBNGD) {
            offset = alignedSingleCoreN;
        }
        int32_t padSize = alignedSingleCoreN - cubeTiling.GetSingleCoreN();
        for (int32_t idx = 0; idx < batchC; ++idx) {
            DataCopyParams copyParams;
            copyParams.blockCount = cubeTiling.GetSingleCoreM();
            copyParams.blockLen = cubeTiling.GetSingleCoreN() * sizeof(T);
            if (C_TYPE::layout == LayoutMode::BSNGD) {
                copyParams.srcStride =
                    ((cubeTiling.GetCLayoutInfoN() * cubeTiling.GetCLayoutInfoG() - 1) * alignedSingleCoreN + padSize) *
                    sizeof(T);
                copyParams.dstStride = (cubeTiling.GetCLayoutInfoN() * cubeTiling.GetCLayoutInfoG() - 1) *
                                       alignedSingleCoreN * sizeof(T) / ONE_BLK_SIZE; // 对齐逻辑
            } else if (C_TYPE::layout == LayoutMode::SBNGD) {
                copyParams.srcStride =
                    ((cubeTiling.GetCLayoutInfoB() * cubeTiling.GetCLayoutInfoN() * cubeTiling.GetCLayoutInfoG() - 1) *
                         alignedSingleCoreN +
                     padSize) *
                    sizeof(T);
                copyParams.dstStride =
                    (cubeTiling.GetCLayoutInfoB() * cubeTiling.GetCLayoutInfoN() * cubeTiling.GetCLayoutInfoG() - 1) *
                    alignedSingleCoreN * sizeof(T) / ONE_BLK_SIZE; // 对齐逻辑
            }

            DataCopyPadParams padParams{true, 0, static_cast<uint8_t>(padSize), 0};
            DataCopyPad(data[idx * offset], globalTensor[idx * offset], copyParams, padParams);
        }
    }

    template <class T, class U, bool sync = true>
    __aicore__ inline void CopyToUB(const LocalTensor<T>& data, const __gm__ U* addr, uint32_t size)
    {
        struct DataCopyParams repeatParams;
        repeatParams.blockLen = size / AscendCUtils::GetC0Count(sizeof(T));
        GlobalTensor<T> globalTensor;
        globalTensor.SetGlobalBuffer((__gm__ T*)addr);
        if constexpr (C_TYPE::format == CubeFormat::ND_ALIGN) {
            int32_t batchNum = 1;
            int32_t offset = 0;
            if constexpr (C_TYPE::layout != LayoutMode::NONE) {
                int32_t alignedSingleCoreN =
                    ConstCeil(cubeTiling.GetSingleCoreN(), AscendCUtils::GetC0Count(sizeof(T))) *
                    AscendCUtils::GetC0Count(sizeof(T));
                offset = cubeTiling.GetSingleCoreM() * alignedSingleCoreN;
                batchNum = size / offset;
            }
            for (int32_t idx = 0; idx < batchNum; ++idx) {
                DataCopyParams copyParams{
                    static_cast<uint16_t>(cubeTiling.GetSingleCoreM()),
                    static_cast<uint16_t>(cubeTiling.GetSingleCoreN() * sizeof(T)), 0, 0};
                DataCopyPadParams padParams{
                    true, 0,
                    static_cast<uint8_t>(
                        ConstCeil(cubeTiling.GetSingleCoreN(), AscendCUtils::GetC0Count(sizeof(T))) *
                            AscendCUtils::GetC0Count(sizeof(T)) -
                        cubeTiling.GetSingleCoreN()),
                    0};
                DataCopyPad(data[idx * offset], globalTensor[idx * offset], copyParams, padParams);
            }
        } else {
            DataCopy(data, globalTensor, repeatParams);
        }

        if constexpr (sync) {
            event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
            SetFlag<HardEvent::MTE2_V>(eventID);
            WaitFlag<HardEvent::MTE2_V>(eventID);
        }
    }

    template <class T>
    __aicore__ inline uint64_t GetGMAddrAndCopyUB(const __gm__ T* gmDataAddr, const LocalTensor<T>& data)
    {
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);

        struct DataCopyParams param;
        param.blockLen = data.GetSize() / AscendCUtils::GetC0Count(sizeof(T));
        GlobalTensor<T> globalTensor;
        globalTensor.SetGlobalBuffer((__gm__ T*)gmDataAddr);
        DataCopy(globalTensor, data, param);

        return reinterpret_cast<uint64_t>(gmDataAddr);
    }
};

// Match Policy with CallBack parameter
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
class MatmulClient : public MatmulClientBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
public:
    __aicore__ inline MatmulClient() {}
};
#else
// Kirin MatmulClient
#include "../../../impl/adv_api/detail/matmul/kfc/matmul_client_impl_aicore.h"
#endif
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_CLIENT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_CLIENT_H__
#endif
