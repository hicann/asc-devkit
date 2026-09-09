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
 * \file matmul_chip_cap.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/feature_trait/matmul_chip_cap.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_FEATURE_TRAIT_MATMUL_CHIP_CAP_H__
#endif

#ifndef IMPL_MATMUL_FEATURE_TRAIT_MATMUL_CHIP_CAP_H
#define IMPL_MATMUL_FEATURE_TRAIT_MATMUL_CHIP_CAP_H

namespace AscendC {
namespace Impl {
namespace Detail {

enum class FixpipeParamsType : int8_t { V220, V300, V310, V311, V510, NONE };

class MatmulChipCap {
public:
    struct Feature {
        bool supportUnitFlag;
        bool ifNeedUB;        // if some func needs vec, such as nd2nz
        bool ifSupportUBToL1; // if cube support ub to l1
        bool supportMNL0DB;
        FixpipeParamsType fixpipeParamsType;
        bool ifSupportLoad3dV2;
        bool ifSupportLoad2dTranspose;
        bool ifSupportLoad2dV2;
        bool ifSupportCmatrixInitVal;
        bool ifSupportFmatrixB;
        bool ifSupportUserDefine;
        bool ifSupportUBToL1Singleshape; // if support ub to l1 fullload, from vec
        bool ifMmadInstrSupportAntiQuant;
        bool ifSupportL0CToUB;
        bool ifSupportTrianMatmul;
        bool ifSupportDisableGemvMode;
    };

    __aicore__ constexpr static const Feature& GetFeatures() { return features[GetChipType()]; }

private:
    enum {
        CHIP_TYPE_100,
        CHIP_TYPE_200,
        CHIP_TYPE_220,
        CHIP_TYPE_300,
        CHIP_TYPE_310,
        CHIP_TYPE_510R2,
        CHIP_TYPE_L300,
        CHIP_TYPE_L311,
    };

    __aicore__ inline constexpr static uint8_t GetChipType()
    {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
        return CHIP_TYPE_100;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
        return CHIP_TYPE_200;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
        return CHIP_TYPE_220;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
        return CHIP_TYPE_300;
#elif __NPU_ARCH__ == 3003
        return CHIP_TYPE_L300;
#elif __NPU_ARCH__ == 3113
        return CHIP_TYPE_L311;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        return CHIP_TYPE_310;
#elif __NPU_ARCH__ == 5102
        return CHIP_TYPE_510R2;
#else
        static_assert("Find invalid chip type!");
        return CHIP_TYPE_100;
#endif
    }

private:
    constexpr static Feature features[] = {
        /*supportUnitFlag, ifNeedUB, ifSupportUBToL1, supportMNL0DB, fixpipeParamsType,
        ifSupportLoad3dV2, ifSupportLoad2dTranspose, ifSupportLoad2dV2,
        ifOnlyUseIsBiasForMmad, ifSupportFmatrixB, ifSupportUserDefine, ifSupportUBToL1Singleshape,
        ifMmadInstrSupportAntiQuant, ifSupportL0CToUB, ifSupportTrianMatmul, ifSupportDisableGemvMode*/
        /*100*/ {
            false, true, true, false, FixpipeParamsType::NONE, false, false, false, true, false, false, false, false,
            false, false, false},
        /*200*/
        {false, true, true, false, FixpipeParamsType::NONE, true, false, false, false, false, false, false, false,
         false, false, false},
        /*220*/
        {true, false, false, true, FixpipeParamsType::V220, true, true, false, false, true, true, false, false, false,
         true, false},
        /*300*/
        {true, false, true, false, FixpipeParamsType::V220, true, true, false, false, true, false, false, false, false,
         false, false},
        /*310*/
        {true, false, false, true, FixpipeParamsType::V310, true, false, true, false, true, true, true, false, true,
         true, true},
        /*510R2*/
        {true, false, true, false, FixpipeParamsType::V510, true, true, true, false, true, false, false, true, true,
         false, true},
        /*L300*/
        {true, false, false, true, FixpipeParamsType::V300, true, true, false, false, true, true, false, false, false,
         true, false},
        /*L311*/
        {true, false, true, false, FixpipeParamsType::V311, true, true, true, false, true, false, false, false, false,
         false, false}};
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _MATMUL_CHIP_CAP_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_FEATURE_TRAIT_MATMUL_CHIP_CAP_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_FEATURE_TRAIT_MATMUL_CHIP_CAP_H__
#endif
