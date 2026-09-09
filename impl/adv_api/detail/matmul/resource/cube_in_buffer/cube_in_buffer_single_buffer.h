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
 * \file cube_in_buffer_single_buffer.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/resource/cube_in_buffer/cube_in_buffer_single_buffer.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_SINGLE_BUFFER_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_SINGLE_BUFFER_H
#define IMPL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_SINGLE_BUFFER_H

#include "cube_in_buffer_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    CubeInBuffer is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CubeInBuffer is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, const auto& MM_CFG, PolicyType POLICY_TYPE>
class CubeInBuffer<
    IMPL, INPUT_TYPE, MM_CFG, POLICY_TYPE,
    enable_if_t<GetCubeInBufferType<INPUT_TYPE, MM_CFG>() == CubeInBufferType::SINGLE_BUFFER>> {
    using TransT = typename INPUT_TYPE::TRANS_T;

public:
    __aicore__ inline CubeInBuffer() {}
    __aicore__ inline ~CubeInBuffer() {}
    __aicore__ inline void Init(int32_t baseBlockSize, int32_t cacheNum)
    {
        (void)cacheNum;
        int32_t matrixByteSize = baseBlockSize * AscendC::GetBitSize<TransT>() / ONE_BYTE_BIT_SIZE;
        GetTPipePtr()->InitBuffer(qid_, SINGLE_QUE, matrixByteSize);
    }

    __aicore__ inline void Destroy()
    {
        if constexpr (INPUT_TYPE::layout == LayoutMode::NONE) {
            if (cacheProc_) {
                qid_.FreeTensor(cacheHead_);
                cacheProc_ = 0;
            }
        }
        qid_.FreeAllEvent();
    }

    __aicore__ inline LocalTensor<TransT> AllocTensor(int32_t bufferPos = -1)
    {
        cacheHead_ = qid_.template AllocTensor<TransT>();
        cacheProc_ = 1;
        return cacheHead_[0];
    }

    __aicore__ inline void FreeTensor(int32_t bufferPos = -1, const LocalTensor<TransT>& tensor = LocalTensor<TransT>{})
    {
        if (cacheProc_ > 0) {
            cacheProc_ = 0;
            qid_.FreeTensor(cacheHead_);
        }
    }

    __aicore__ inline void Reset()
    {
        if constexpr (INPUT_TYPE::layout == LayoutMode::NONE) {
            FreeTensor();
        }
    }

    __aicore__ inline bool Hit(int32_t iterIndex, int32_t bufferPos = -1) { return false; }

    __aicore__ inline LocalTensor<TransT> GetBuffer(int32_t iterIndex, int32_t bufferPos = -1)
    {
        return LocalTensor<TransT>{};
    }

    __aicore__ inline void EnQue(LocalTensor<TransT>& tensor) { qid_.EnQue(tensor); }

    __aicore__ inline void DeQue() { (void)qid_.DeQue(); }

    __aicore__ inline uint64_t GetBufferHeadAddr()
    {
// wait for GetTQueHeadAddr
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        return GetTQueHeadAddr(qid_);
#else
        return 0;
#endif
    }

private:
    typename CubeInQueType<INPUT_TYPE>::QUE qid_;
    LocalTensor<TransT> cacheHead_;
    int32_t cacheProc_{0};
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _CUBE_IN_BUFFER_SINGLE_BUFFER_H_

#if defined( \
    __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_SINGLE_BUFFER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_SINGLE_BUFFER_H__
#endif
