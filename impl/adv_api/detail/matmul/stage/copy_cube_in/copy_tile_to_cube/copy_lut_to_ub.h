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
 * \file copy_lut_to_ub.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/copy_lut_to_ub.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_LUT_TO_UB_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_LUT_TO_UB_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_LUT_TO_UB_H

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto& MM_CFG, typename = void>
class QtableProcessor {
public:
    __aicore__ inline QtableProcessor() = default;
    __aicore__ inline ~QtableProcessor() = default;

    /**
     * @description: Init UB buffer
     * @return: void
     */
    __aicore__ inline void Init(uint32_t qtableNum) {}

    /**
     * @description: Set qtable tensor params
     * @param: qtable tensor params
     * @return: void
     */
    __aicore__ inline void SetLookupTable(const GlobalTensor<uint64_t>& qtableTensor) {}

    /**
     * @description: Get qtable buf
     * @param: index: the index of qtable buf
     * @return: qtable
     */
    __aicore__ inline uint64_t GetQtable(uint16_t index) const { return 0; }
};

template <typename IMPL, const auto& MM_CFG>
class QtableProcessor<IMPL, MM_CFG, AscendC::enable_if_t<IsDecompMode<MM_CFG>(), void>> {
public:
    __aicore__ inline QtableProcessor() = default;
    __aicore__ inline ~QtableProcessor() = default;

    /**
     * @description: Init UB buffer
     * @return: void
     */
    __aicore__ inline void Init(uint32_t qtableNum)
    {
        byteSize_ = qtableNum * sizeof(uint64_t);
        GetTPipePtr()->InitBuffer(qidQtable_, CeilAlign(byteSize_, ONE_BLK_SIZE));
    }
    /**
     * @description: Set qtable tensor params
     * @param: qtable tensor params
     * @return: void
     */
    __aicore__ inline void SetLookupTable(const GlobalTensor<uint64_t>& qtableTensor) {}

    /**
     * @description: Get qtable buf
     * @param: index: the index of qtable buf
     * @return: qtable
     */
    __aicore__ inline uint64_t GetQtable(uint16_t index) const
    {
        return qtableTensor_.GetValue(static_cast<uint32_t>(index));
    }

private:
    TBuf<TPosition::VECIN> qidQtable_;
    LocalTensor<uint64_t> qtableTensor_;
    uint32_t byteSize_ = 0;
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined( \
    __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_LUT_TO_UB_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_LUT_TO_UB_H__
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_LUT_TO_UB_H
