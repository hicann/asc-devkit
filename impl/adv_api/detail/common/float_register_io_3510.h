/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_ADV_API_DETAIL_COMMON_FLOAT_REGISTER_IO_3510_H
#define IMPL_ADV_API_DETAIL_COMMON_FLOAT_REGISTER_IO_3510_H

#include "../../../../include/basic_api/kernel_basic_intf.h"

namespace AscendC::FloatRegisterIO3510 {
template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + offset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, mask);
    } else {
        Reg::LoadAlign(dstReg, src + offset);
    }
}

template <typename T>
__simd_callee__ inline void SaveDataWithT(
    __ubuf__ T* dst, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> regT;
        Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(regT, srcReg, mask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + offset, regT, mask);
    } else {
        Reg::StoreAlign(dst + offset, srcReg, mask);
    }
}
} // namespace AscendC::FloatRegisterIO3510

#endif
