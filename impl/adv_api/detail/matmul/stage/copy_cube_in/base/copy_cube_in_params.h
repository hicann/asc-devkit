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
 * \file copy_cube_in_params.h
 * \brief copy cube in variable manager module
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_params.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_H

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE, typename = void>
class CopyCubeInParams {
public:
    __aicore__ inline int32_t GetDepthL1CacheUB() { return 0; }

    __aicore__ constexpr int32_t GetCopyHeight(int32_t i) { return 0; }

    template <bool IS_TRANS = false>
    __aicore__ constexpr int32_t GetCopyWidth(int32_t i, int32_t baseWidth)
    {
        return 0;
    }

    __aicore__ constexpr bool IsBufferPosEnd(int32_t i) { return false; }

    __aicore__ constexpr bool IsBufferPosEnd() { return false; }

    __aicore__ constexpr bool IsBufferKPosEnd(int32_t i) { return false; }

    __aicore__ constexpr bool IsBufferKPosEnd() { return false; }

    __aicore__ constexpr bool IsL1KFullLoad() const { return false; }

    __aicore__ inline bool IsTranspose() { return false; }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetStepCol() const
    {
        return 0;
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetStepRow() const
    {
        return 0;
    }

    __aicore__ inline int32_t GetBufferPos() { return 0; }

    __aicore__ inline int32_t GetBufferSize() { return 0; }

    __aicore__ inline int GetDepth() const { return 0; }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline int32_t GetOrgHeight()
    {
        return 0;
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline int32_t GetOrgWidth()
    {
        return 0;
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
    __aicore__ inline int32_t GetSingleHeight() const
    {
        return 0;
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBaseHeight() const
    {
        return 0;
    }

    template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
    __aicore__ inline int32_t GetSingleWidth() const
    {
        return 0;
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBaseWidth() const
    {
        return 0;
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetTotalRow()
    {
        return 0;
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetTotalCol()
    {
        return 0;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline bool IsKRowDirec() const
    {
        return false;
    }
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC

#include "copy_cube_in_params_base.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "copy_cube_in_params_mx.h"
#endif

#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_H__
#endif
