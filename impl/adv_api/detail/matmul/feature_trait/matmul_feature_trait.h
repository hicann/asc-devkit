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
 * \file matmul_feature_trait.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/feature_trait/matmul_feature_trait.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_FEATURE_TRAIT_MATMUL_FEATURE_TRAIT_H__
#endif

#ifndef IMPL_MATMUL_FEATURE_TRAIT_MATMUL_FEATURE_TRAIT_H
#define IMPL_MATMUL_FEATURE_TRAIT_MATMUL_FEATURE_TRAIT_H

#include "../utils/matmul_config_utils.h"
#include "../utils/matmul_type_def.h"
#include "matmul_chip_cap.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <const auto& MM_CFG = CFG_NORM>
class MatmulFeatureTrait {
public:
    __aicore__ inline constexpr static bool IsUnitFlagEnabled()
    {
        return EnUnitFlag(MM_CFG) && MatmulChipCap::GetFeatures().supportUnitFlag;
    }

    __aicore__ inline constexpr static bool IsNeedUB() { return MatmulChipCap::GetFeatures().ifNeedUB; }

    __aicore__ inline constexpr static bool IsSupportUBToL1() { return MatmulChipCap::GetFeatures().ifSupportUBToL1; }

    __aicore__ inline constexpr static bool IsSupportMNL0DB()
    {
        // only V220 and MDL and NORM scene support L0DB
        return MatmulChipCap::GetFeatures().supportMNL0DB &&
               ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT &&
               (DoMatmulMDL(MM_CFG) || DoMatmulNorm(MM_CFG));
    }
    __aicore__ inline constexpr static FixpipeParamsType GetFixpipeParamsType()
    {
        return MatmulChipCap::GetFeatures().fixpipeParamsType;
    }

    __aicore__ inline constexpr static bool IsSupportLoad3dV2()
    {
        return MatmulChipCap::GetFeatures().ifSupportLoad3dV2;
    }

    __aicore__ inline constexpr static bool IsSupportLoad2dTranspose()
    {
        return MatmulChipCap::GetFeatures().ifSupportLoad2dTranspose;
    }

    __aicore__ inline constexpr static bool IsSupportLoad2dV2()
    {
        return MatmulChipCap::GetFeatures().ifSupportLoad2dV2;
    }

    __aicore__ inline constexpr static bool IsSupportCmatrixInitVal()
    {
        return MatmulChipCap::GetFeatures().ifSupportCmatrixInitVal;
    }

    __aicore__ inline constexpr static bool IsSupportFmatrixB()
    {
        return MatmulChipCap::GetFeatures().ifSupportFmatrixB;
    }

    __aicore__ inline constexpr static bool IsSupportUserDefine()
    {
        return MatmulChipCap::GetFeatures().ifSupportUserDefine;
    }

    __aicore__ inline constexpr static bool IsSupportUBToL1Singleshape()
    {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) && defined(USE_WORKSPACE)
        return false;
#else
        return MatmulChipCap::GetFeatures().ifSupportUBToL1Singleshape;
#endif
    }

    __aicore__ inline constexpr static bool IsMmadInstrSupportAntiQuant()
    {
        return MatmulChipCap::GetFeatures().ifMmadInstrSupportAntiQuant;
    }

    __aicore__ inline constexpr static bool IsSupportL0CToUB()
    {
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) && defined(USE_WORKSPACE)
        return false;
#else
        return MatmulChipCap::GetFeatures().ifSupportL0CToUB;
#endif
    }

    __aicore__ inline constexpr static bool IsSupportTrianMatmul()
    {
        return MatmulChipCap::GetFeatures().ifSupportTrianMatmul;
    }

    __aicore__ inline constexpr static bool IsSupportDisableGemvMode()
    {
        return MatmulChipCap::GetFeatures().ifSupportDisableGemvMode;
    }
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _MATMUL_FEATURE_TRAIT_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_FEATURE_TRAIT_MATMUL_FEATURE_TRAIT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_FEATURE_TRAIT_MATMUL_FEATURE_TRAIT_H__
#endif
