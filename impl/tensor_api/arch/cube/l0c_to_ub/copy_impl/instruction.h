/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {

class CopyMatrixCcToUbInstr {
public:
    template <QuantMode_t quantPre, typename T, typename U, typename... Params>
    __aicore__ inline static void DataCopy(const T& dst, const U& src, const Params&... params)
    {
        CopyMatrixCcToUb<quantPre>(dst.Data().Get(), src.Data().Get(), params...);
    }

private:
    template <QuantMode_t quantPre, typename T, typename U>
    __aicore__ inline static void CopyMatrixCcToUb(
        __ubuf__ T* dst, __cc__ U* src, uint32_t nSize, uint32_t mSize, uint32_t srcStride, uint32_t dstStride,
        uint8_t dualDstCtl, bool reluEn, uint8_t unitFlag, bool subBlockId, bool isChannelSplit, bool nz2ndEn,
        bool nz2dnEn)
    {
        if ASCEND_IS_AIV {
            return;
        }

        asc_copy_l0c2ub(
            dst, src, static_cast<uint16_t>(nSize), static_cast<uint16_t>(mSize), dstStride,
            static_cast<uint16_t>(srcStride), dualDstCtl, subBlockId, 0, unitFlag, static_cast<uint64_t>(quantPre),
            static_cast<uint8_t>(reluEn), isChannelSplit, nz2ndEn, static_cast<uint64_t>(QuantMode_post::NoConv), 0,
            false, 0, false, false, false, nz2dnEn);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
