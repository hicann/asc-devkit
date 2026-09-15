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
 * \file kernel_operator_symbol_override_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_symbol_override_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYMBOL_OVERRIDE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_SYM_OVERRIDE_H
#define ASCENDC_MODULE_OPERATOR_SYM_OVERRIDE_H
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "kernel_check.h"
#endif

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_cmpsel_impl.h"
#include "dav_c100/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_cmpsel_impl.h"
#include "dav_m200/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_cmp_impl.h"
#include "dav_c220/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_binary_continuous_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_binary_continuous_impl.h"
#elif __NPU_ARCH__ == 3510
#include "../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"
#include "dav_3510/kernel_operator_vec_binary_continuous_impl.h"
#include "dav_3510/kernel_operator_vec_compare_continuous_impl.h"
#endif
#pragma begin_pipe(V)
namespace AscendC {
template <typename T>
class LocalTensor;

// Addition symbol overload
template <typename T>
class SymbolOverrideAdd {
public:
    __aicore__ inline SymbolOverrideAdd(const LocalTensor<T>& src0, const LocalTensor<T>& src1)
        : src0_(src0), src1_(src1)
    {}

    __aicore__ inline void Process(const LocalTensor<T>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinary(dst, this->src0_, this->src1_, dst.GetSize(), "Add operator")), {
            ASCENDC_REPORT_CHECK_ERROR("Add operator", KernelFuncType::NONE_MODE);
        });
#endif
        AddImpl(
            (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
            (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize());
    }

private:
    const LocalTensor<T>& src0_;
    const LocalTensor<T>& src1_;
};
// Subtract Symbol Overload
template <typename T>
class SymbolOverrideSub {
public:
    __aicore__ inline SymbolOverrideSub(const LocalTensor<T>& src0, const LocalTensor<T>& src1)
        : src0_(src0), src1_(src1)
    {}

    __aicore__ inline void Process(const LocalTensor<T>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinary(dst, this->src0_, this->src1_, dst.GetSize(), "Sub operator")), {
            ASCENDC_REPORT_CHECK_ERROR("Sub operator", KernelFuncType::NONE_MODE);
        });
#endif
        SubImpl(
            (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
            (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize());
    }

private:
    const LocalTensor<T>& src0_;
    const LocalTensor<T>& src1_;
};
// Multiplication symbol overload
template <typename T>
class SymbolOverrideMul {
public:
    __aicore__ inline SymbolOverrideMul(const LocalTensor<T>& src0, const LocalTensor<T>& src1)
        : src0_(src0), src1_(src1)
    {}

    __aicore__ inline void Process(const LocalTensor<T>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinary(dst, this->src0_, this->src1_, dst.GetSize(), "Mul operator")), {
            ASCENDC_REPORT_CHECK_ERROR("Mul operator", KernelFuncType::NONE_MODE);
        });
#endif
        MulImpl(
            (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
            (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize());
    }

private:
    const LocalTensor<T>& src0_;
    const LocalTensor<T>& src1_;
};
// Division symbol overload
template <typename T>
class SymbolOverrideDiv {
public:
    __aicore__ inline SymbolOverrideDiv(const LocalTensor<T>& src0, const LocalTensor<T>& src1)
        : src0_(src0), src1_(src1)
    {}

    __aicore__ inline void Process(const LocalTensor<T>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinary(dst, this->src0_, this->src1_, dst.GetSize(), "Div operator")), {
            ASCENDC_REPORT_CHECK_ERROR("Div operator", KernelFuncType::NONE_MODE);
        });
#endif
        DivImpl(
            (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
            (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize());
    }

private:
    const LocalTensor<T>& src0_;
    const LocalTensor<T>& src1_;
};

// bitwise and symbol overloads
template <typename T>
class SymbolOverrideAnd {
public:
    __aicore__ inline SymbolOverrideAnd(const LocalTensor<T>& src0, const LocalTensor<T>& src1)
        : src0_(src0), src1_(src1)
    {}

    __aicore__ inline void Process(const LocalTensor<T>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinary(dst, this->src0_, this->src1_, dst.GetSize(), "And operator")), {
            ASCENDC_REPORT_CHECK_ERROR("And operator", KernelFuncType::NONE_MODE);
        });
#endif
        if constexpr (SupportType<T, int32_t, uint32_t>()) {
            AndImpl(
                (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
                (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize() * 2);
        } else { // mainly for uint16_t  + int16_t
            AndImpl(
                (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
                (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize());
        }
    }

private:
    const LocalTensor<T>& src0_;
    const LocalTensor<T>& src1_;
};
// bitwise or symbol overloads
template <typename T>
class SymbolOverrideOr {
public:
    __aicore__ inline SymbolOverrideOr(const LocalTensor<T>& src0, const LocalTensor<T>& src1)
        : src0_(src0), src1_(src1)
    {}

    __aicore__ inline void Process(const LocalTensor<T>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinary(dst, this->src0_, this->src1_, dst.GetSize(), "Or operator")), {
            ASCENDC_REPORT_CHECK_ERROR("Or operator", KernelFuncType::NONE_MODE);
        });
#endif
        if constexpr (SupportType<T, int32_t, uint32_t>()) {
            OrImpl(
                (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
                (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize() * 2);
        } else { // mainly for uint16_t  + int16_t
            OrImpl(
                (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
                (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), dst.GetSize());
        }
    }

private:
    const LocalTensor<T>& src0_;
    const LocalTensor<T>& src1_;
};

// Compare symbol overloads
template <typename T>
class SymbolOverrideCompare {
public:
    __aicore__ inline SymbolOverrideCompare(const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode)
        : src0_(src0), src1_(src1), cmpMode_(cmpMode)
    {}

    template <typename U>
    __aicore__ inline void Process(const LocalTensor<U>& dst) const
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((CheckFuncVecBinaryCmp(dst, this->src0_, this->src1_, dst.GetSize(), "Compare operator")), {
            ASCENDC_REPORT_CHECK_ERROR("Compare operator", KernelFuncType::NONE_MODE);
        });
#endif
        VcmpvImpl(
            (__ubuf__ PrimT<U>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)this->src0_.GetPhyAddr(),
            (__ubuf__ PrimT<T>*)this->src1_.GetPhyAddr(), cmpMode_, this->src0_.GetSize());
    }

private:
    const LocalTensor<T> src0_;
    const LocalTensor<T> src1_;
    CMPMODE cmpMode_;
};
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_SYM_OVERRIDE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYMBOL_OVERRIDE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYMBOL_OVERRIDE_IMPL_H__
#endif
