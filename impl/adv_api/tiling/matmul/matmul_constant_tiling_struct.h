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
 * \file matmul_constant_tiling_struct.h
 * \brief
 */
#ifndef IMPL_MATMUL_TILING_MATMUL_CONSTANT_TILING_STRUCT_H
#define IMPL_MATMUL_TILING_MATMUL_CONSTANT_TILING_STRUCT_H

#include "../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../include/adv_api/matmul/tiling.h"
#include "../../detail/matmul/utils/matmul_utils.h"

namespace AscendC {
template <const auto& TILING>
struct MatmulTiling {
    using TILING_TYPE = typename std::remove_cv<typename std::remove_reference<decltype(TILING)>::type>::type;
    __aicore__ inline void SetTiling(const TCubeTiling* __restrict tiling) { tiling_ = tiling; }
    __aicore__ inline const TCubeTiling* __restrict GetTiling() { return tiling_; }
    __aicore__ inline bool IsNull() const { return (tiling_ == nullptr); }
    __aicore__ inline int32_t GetUsedCoreNum() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.usedCoreNum != -1) ? TILING.usedCoreNum : tiling_->usedCoreNum;
        } else {
            return tiling_->usedCoreNum;
        }
    }
    __aicore__ inline int32_t GetM() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.M != -1) ? TILING.M : tiling_->M;
        } else {
            return tiling_->M;
        }
    }
    __aicore__ inline int32_t GetN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.N != -1) ? TILING.N : tiling_->N;
        } else {
            return tiling_->N;
        }
    }
    __aicore__ inline int32_t GetKa() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.Ka != -1) ? TILING.Ka : tiling_->Ka;
        } else {
            return tiling_->Ka;
        }
    }
    __aicore__ inline int32_t GetKb() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.Kb != -1) ? TILING.Kb : tiling_->Kb;
        } else {
            return tiling_->Kb;
        }
    }
    __aicore__ inline int32_t GetSingleCoreM() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.singleCoreM != -1) ? TILING.singleCoreM : tiling_->singleCoreM;
        } else {
            return tiling_->singleCoreM;
        }
    }
    __aicore__ inline int32_t GetSingleCoreN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.singleCoreN != -1) ? TILING.singleCoreN : tiling_->singleCoreN;
        } else {
            return tiling_->singleCoreN;
        }
    }
    __aicore__ inline int32_t GetSingleCoreK() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.singleCoreK != -1) ? TILING.singleCoreK : tiling_->singleCoreK;
        } else {
            return tiling_->singleCoreK;
        }
    }
    __aicore__ inline int32_t GetBaseM() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.baseM != -1) ? TILING.baseM : tiling_->baseM;
        } else {
            return tiling_->baseM;
        }
    }
    __aicore__ inline int32_t GetBaseN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.baseN != -1) ? TILING.baseN : tiling_->baseN;
        } else {
            return tiling_->baseN;
        }
    }
    __aicore__ inline int32_t GetBaseK() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.baseK != -1) ? TILING.baseK : tiling_->baseK;
        } else {
            return tiling_->baseK;
        }
    }
    __aicore__ inline int32_t GetDepthA1() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.depthA1 != -1) ? TILING.depthA1 : tiling_->depthA1;
        } else {
            return tiling_->depthA1;
        }
    }
    __aicore__ inline int32_t GetDepthB1() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.depthB1 != -1) ? TILING.depthB1 : tiling_->depthB1;
        } else {
            return tiling_->depthB1;
        }
    }
    __aicore__ inline int32_t GetStepM() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.stepM != -1) ? TILING.stepM : tiling_->stepM;
        } else {
            return tiling_->stepM;
        }
    }
    __aicore__ inline int32_t GetStepN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.stepN != -1) ? TILING.stepN : tiling_->stepN;
        } else {
            return tiling_->stepN;
        }
    }
    __aicore__ inline int32_t IsBias() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.isBias != -1) ? TILING.isBias : tiling_->isBias;
        } else {
            return tiling_->isBias;
        }
    }
    __aicore__ inline int32_t GetTransLength() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.transLength != -1) ? TILING.transLength : tiling_->transLength;
        } else {
            return tiling_->transLength;
        }
    }
    __aicore__ inline int32_t GetIterateOrder() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.iterateOrder != -1) ? TILING.iterateOrder : tiling_->iterateOrder;
        } else {
            return tiling_->iterateOrder;
        }
    }
    __aicore__ inline int32_t GetShareMode() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.shareMode != -1) ? TILING.shareMode : tiling_->shareMode;
        } else {
            return tiling_->shareMode;
        }
    }
    __aicore__ inline int32_t GetShareL1Size() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.shareL1Size != -1) ? TILING.shareL1Size : tiling_->shareL1Size;
        } else {
            return tiling_->shareL1Size;
        }
    }
    __aicore__ inline int32_t GetShareL0CSize() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.shareL0CSize != -1) ? TILING.shareL0CSize : tiling_->shareL0CSize;
        } else {
            return tiling_->shareL0CSize;
        }
    }
    __aicore__ inline int32_t GetShareUbSize() const
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
        return 0;
#else
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.shareUbSize != -1) ? TILING.shareUbSize : tiling_->shareUbSize;
        } else {
            return tiling_->shareUbSize;
        }
#endif
    }
    __aicore__ inline int32_t GetStepKa() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.stepKa != -1) ? TILING.stepKa : tiling_->stepKa;
        } else {
            return tiling_->stepKa;
        }
    }
    __aicore__ inline int32_t GetStepKb() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.stepKb != -1) ? TILING.stepKb : tiling_->stepKb;
        } else {
            return tiling_->stepKb;
        }
    }
    __aicore__ inline int32_t GetDepthAL1CacheUB() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.depthAL1CacheUB != -1) ? TILING.depthAL1CacheUB : tiling_->depthAL1CacheUB;
        } else {
            return tiling_->depthAL1CacheUB;
        }
    }
    __aicore__ inline int32_t GetDepthBL1CacheUB() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.depthBL1CacheUB != -1) ? TILING.depthBL1CacheUB : tiling_->depthBL1CacheUB;
        } else {
            return tiling_->depthBL1CacheUB;
        }
    }
    __aicore__ inline int32_t GetDbL0A() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.dbL0A != -1) ? TILING.dbL0A : tiling_->dbL0A;
        } else {
            return tiling_->dbL0A;
        }
    }
    __aicore__ inline const int32_t GetDbL0B() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.dbL0B != -1) ? TILING.dbL0B : tiling_->dbL0B;
        } else {
            return tiling_->dbL0B;
        }
    }
    __aicore__ inline const int32_t GetDbL0C() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.dbL0C != -1) ? TILING.dbL0C : tiling_->dbL0C;
        } else {
            return tiling_->dbL0C;
        }
    }
    __aicore__ inline const int32_t GetALayoutInfoB() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.ALayoutInfoB != -1) ? TILING.ALayoutInfoB : tiling_->ALayoutInfoB;
        } else {
            return tiling_->ALayoutInfoB;
        }
    }
    __aicore__ inline const int32_t GetALayoutInfoS() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.ALayoutInfoS != -1) ? TILING.ALayoutInfoS : tiling_->ALayoutInfoS;
        } else {
            return tiling_->ALayoutInfoS;
        }
    }
    __aicore__ inline const int32_t GetALayoutInfoN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.ALayoutInfoN != -1) ? TILING.ALayoutInfoN : tiling_->ALayoutInfoN;
        } else {
            return tiling_->ALayoutInfoN;
        }
    }
    __aicore__ inline const int32_t GetALayoutInfoG() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.ALayoutInfoG != -1) ? TILING.ALayoutInfoG : tiling_->ALayoutInfoG;
        } else {
            return tiling_->ALayoutInfoG;
        }
    }
    __aicore__ inline const int32_t GetALayoutInfoD() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.ALayoutInfoD != -1) ? TILING.ALayoutInfoD : tiling_->ALayoutInfoD;
        } else {
            return tiling_->ALayoutInfoD;
        }
    }
    __aicore__ inline int32_t GetBLayoutInfoB() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.BLayoutInfoB != -1) ? TILING.BLayoutInfoB : tiling_->BLayoutInfoB;
        } else {
            return tiling_->BLayoutInfoB;
        }
    }
    __aicore__ inline int32_t GetBLayoutInfoS() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.BLayoutInfoS != -1) ? TILING.BLayoutInfoS : tiling_->BLayoutInfoS;
        } else {
            return tiling_->BLayoutInfoS;
        }
    }
    __aicore__ inline int32_t GetBLayoutInfoN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.BLayoutInfoN != -1) ? TILING.BLayoutInfoN : tiling_->BLayoutInfoN;
        } else {
            return tiling_->BLayoutInfoN;
        }
    }
    __aicore__ inline int32_t GetBLayoutInfoG() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.BLayoutInfoG != -1) ? TILING.BLayoutInfoG : tiling_->BLayoutInfoG;
        } else {
            return tiling_->BLayoutInfoG;
        }
    }
    __aicore__ inline int32_t GetBLayoutInfoD() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.BLayoutInfoD != -1) ? TILING.BLayoutInfoD : tiling_->BLayoutInfoD;
        } else {
            return tiling_->BLayoutInfoD;
        }
    }
    __aicore__ inline int32_t GetCLayoutInfoB() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.CLayoutInfoB != -1) ? TILING.CLayoutInfoB : tiling_->CLayoutInfoB;
        } else {
            return tiling_->CLayoutInfoB;
        }
    }
    __aicore__ inline int32_t GetCLayoutInfoS1() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.CLayoutInfoS1 != -1) ? TILING.CLayoutInfoS1 : tiling_->CLayoutInfoS1;
        } else {
            return tiling_->CLayoutInfoS1;
        }
    }
    __aicore__ inline int32_t GetCLayoutInfoN() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.CLayoutInfoN != -1) ? TILING.CLayoutInfoN : tiling_->CLayoutInfoN;
        } else {
            return tiling_->CLayoutInfoN;
        }
    }
    __aicore__ inline int32_t GetCLayoutInfoG() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.CLayoutInfoG != -1) ? TILING.CLayoutInfoG : tiling_->CLayoutInfoG;
        } else {
            return tiling_->CLayoutInfoG;
        }
    }
    __aicore__ inline int32_t GetCLayoutInfoS2() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.CLayoutInfoS2 != -1) ? TILING.CLayoutInfoS2 : tiling_->CLayoutInfoS2;
        } else {
            return tiling_->CLayoutInfoS2;
        }
    }
    __aicore__ inline int32_t GetBatchNum() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.BatchNum != -1) ? TILING.BatchNum : tiling_->BatchNum;
        } else {
            return tiling_->BatchNum;
        }
    }

    __aicore__ inline uint8_t GetScaleFactorKa() const
    {
        constexpr static uint64_t scaleFactorKaMask = 0x0000007f;
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.mxTypePara != -1) ? (TILING.mxTypePara & scaleFactorKaMask) :
                                               tiling_->mxTypePara & scaleFactorKaMask;
        } else {
            return tiling_->mxTypePara & scaleFactorKaMask;
        }
    }
    __aicore__ inline uint8_t GetScaleFactorKb() const
    {
        constexpr static uint64_t scaleFactorKbMask = 0x00007f00;
        constexpr static uint32_t scaleFactorKbOffset = 8;
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.mxTypePara != -1) ? ((TILING.mxTypePara & scaleFactorKbMask) >> scaleFactorKbOffset) :
                                               ((tiling_->mxTypePara & scaleFactorKbMask) >> scaleFactorKbOffset);
        } else {
            return (tiling_->mxTypePara & scaleFactorKbMask) >> scaleFactorKbOffset;
        }
    }
    __aicore__ inline uint8_t GetScaleFactorM() const
    {
        if constexpr (ToMatmulConfig(TILING).doMTE2Preload > 0) {
            return 1;
        }
        constexpr static uint64_t scaleFactorMMask = 0x007f0000;
        constexpr static uint32_t scaleFactorMOffset = 16;
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.mxTypePara != -1) ? ((TILING.mxTypePara & scaleFactorMMask) >> scaleFactorMOffset) :
                                               ((tiling_->mxTypePara & scaleFactorMMask) >> scaleFactorMOffset);
        } else {
            return (tiling_->mxTypePara & scaleFactorMMask) >> scaleFactorMOffset;
        }
    }
    __aicore__ inline uint8_t GetScaleFactorN() const
    {
        if constexpr (ToMatmulConfig(TILING).doMTE2Preload > 0) {
            return 1;
        }
        constexpr static uint64_t scaleFactorNMask = 0x7f000000;
        constexpr static uint32_t scaleFactorNOffset = 24;
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return (TILING.mxTypePara != -1) ? ((TILING.mxTypePara & scaleFactorNMask) >> scaleFactorNOffset) :
                                               ((tiling_->mxTypePara & scaleFactorNMask) >> scaleFactorNOffset);
        } else {
            return (tiling_->mxTypePara & scaleFactorNMask) >> scaleFactorNOffset;
        }
    }

    __aicore__ inline int32_t GetMxTypePara() const
    {
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
            return TILING.mxTypePara;
        } else {
            return tiling_->mxTypePara;
        }
    }

private:
    const TCubeTiling* __restrict tiling_;
};
} // namespace AscendC
#endif // _MATMUL_CONSTANT_TILING_STRUCT_H_
