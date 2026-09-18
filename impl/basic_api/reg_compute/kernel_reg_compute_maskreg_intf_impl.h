/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_maskreg_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_maskreg_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_maskreg_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_INTF_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_MASKREG_INTERFACE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_MASKREG_INTERFACE_IMPL_H

#if __NPU_ARCH__ == 3003
#include "../../basic_api/reg_compute/dav_l300/kernel_reg_compute_maskreg_impl.h"
#elif __NPU_ARCH__ == 3113
#include "../../basic_api/reg_compute/dav_l311/kernel_reg_compute_maskreg_impl.h"
#elif __NPU_ARCH__ == 5102
#include "../../basic_api/reg_compute/dav_m510/kernel_reg_compute_maskreg_impl.h"
#elif __NPU_ARCH__ == 5162
#include "../../basic_api/reg_compute/dav_v516/kernel_reg_compute_maskreg_impl.h"
#else
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_maskreg_impl.h"
#endif

namespace AscendC {
namespace Reg {
template <typename T, const RegTrait& regTrait>
__simd_callee__ inline MaskReg UpdateMask(uint32_t& scalarValue)
{
    return UpdateMaskImpl<T, regTrait>(scalarValue);
}

template <typename T, MaskPattern mode, const RegTrait& regTrait>
__simd_callee__ inline MaskReg CreateMask()
{
    return CreateMaskImpl<T, mode, regTrait>();
}

__simd_callee__ inline void MaskNot(MaskReg& dst, MaskReg& src, MaskReg& mask) { MaskNotImpl(dst, src, mask); }
__simd_callee__ inline void Not(MaskReg& dst, MaskReg& src, MaskReg& mask) { MaskNotImpl(dst, src, mask); }

template <typename T, int16_t offset, typename U>
__simd_callee__ inline void MaskGenWithRegTensor(MaskReg& dst, U& srcReg)
{
    MaskGenWithRegTensorImpl<T, offset, U>(dst, srcReg);
}

__simd_callee__ inline void MaskAnd(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskAndImpl(dst, src0, src1, mask);
}
__simd_callee__ inline void And(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskAndImpl(dst, src0, src1, mask);
}

__simd_callee__ inline void MaskOr(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskOrImpl(dst, src0, src1, mask);
}
__simd_callee__ inline void Or(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskOrImpl(dst, src0, src1, mask);
}

__simd_callee__ inline void MaskXor(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskXorImpl(dst, src0, src1, mask);
}
__simd_callee__ inline void Xor(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskXorImpl(dst, src0, src1, mask);
}

__simd_callee__ inline void MaskMov(MaskReg& dst, MaskReg& src, MaskReg& mask) { MaskMovImpl(dst, src, mask); }
__simd_callee__ inline void Move(MaskReg& dst, MaskReg& src, MaskReg& mask) { MaskMovImpl(dst, src, mask); }

__simd_callee__ inline void MaskMov(MaskReg& dst, MaskReg& src) { MaskMovImpl(dst, src); }
__simd_callee__ inline void Move(MaskReg& dst, MaskReg& src) { MaskMovImpl(dst, src); }

template <typename T>
__simd_callee__ inline void MaskInterleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
{
    MaskInterleaveImpl<T>(dst0, dst1, src0, src1);
}
template <typename T>
__simd_callee__ inline void Interleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
{
    MaskInterleaveImpl<T>(dst0, dst1, src0, src1);
}

template <typename T>
__simd_callee__ inline void MaskDeInterleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
{
    MaskDeInterleaveImpl<T>(dst0, dst1, src0, src1);
}
template <typename T>
__simd_callee__ inline void DeInterleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
{
    MaskDeInterleaveImpl<T>(dst0, dst1, src0, src1);
}

__simd_callee__ inline void MaskSel(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskSelImpl(dst, src0, src1, mask);
}
__simd_callee__ inline void Select(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    MaskSelImpl(dst, src0, src1, mask);
}

template <HighLowPart part = HighLowPart::LOWEST>
__simd_callee__ inline void MaskPack(MaskReg& dst, MaskReg& src)
{
    MaskPackImpl<part>(dst, src);
}
template <HighLowPart part>
__simd_callee__ inline void Pack(MaskReg& dst, MaskReg& src)
{
    MaskPackImpl<part>(dst, src);
}

template <HighLowPart part = HighLowPart::LOWEST>
__simd_callee__ inline void MaskUnPack(MaskReg& dst, MaskReg& src)
{
    MaskUnPackImpl<part>(dst, src);
}
template <HighLowPart part>
__simd_callee__ inline void UnPack(MaskReg& dst, MaskReg& src)
{
    MaskUnPackImpl<part>(dst, src);
}

template <typename T>
__simd_callee__ inline MaskReg MoveMask()
{
    return MoveMaskImpl<T>();
}
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_MODULE_REG_COMPUTE_MASKREG_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_INTF_IMPL__
#endif
