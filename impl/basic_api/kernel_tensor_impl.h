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
 * \file kernel_tensor_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_TENSOR_IMPL_H
#define ASCENDC_MODULE_TENSOR_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_H__
#endif
#include "kernel_operator_symbol_override_impl.h"

namespace AscendC {
template <Hardware pos>
__aicore__ inline uint32_t GetDynamicMemStartPos()
{
// dynamic array is not supported in cpu mode
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510 && !defined(ASCENDC_CPU_DEBUG)
    if constexpr (pos == Hardware::UB) {
        extern __ubuf__ uint32_t dynamicStartUB[];
        return (uint64_t)(&dynamicStartUB[0]);
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

#if defined(__NPU_ARCH__)
template <typename Shape, size_t... Is>
__aicore__ inline constexpr auto ProdImpl(const Shape& t, Std::index_sequence<Is...>)
{
    return (Std::get<Is>(t) * ... * 1);
}

template <typename... Args>
__aicore__ inline constexpr auto Prod(const Shape<Args...>& t)
{
    return ProdImpl(t, Std::make_index_sequence<sizeof...(Args)>{});
}

// CPU Impl
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
inline uint8_t* GetBaseAddrCpu(int8_t logicPos);
#endif
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
template <typename T>
LocalTensor<T>::LocalTensor(TBuffAddr& address)
{
    this->address_ = address;
}

template <typename T>
LocalTensor<T>::LocalTensor(const LocalTensor<T>& other)
{
    os_.str("");
    this->address_ = other.address_;
#ifndef SPLIT_CORE_CUBE
    if constexpr (IsSameType<PrimType, T>::value) {
        this->shapeInfo_ = other.shapeInfo_;
    }
#endif
    if constexpr (is_tensorTrait_v<T>) {
        this->SetTensorTrait(other.GetTensorTrait());
    }
}

template <typename T>
LocalTensor<T> LocalTensor<T>::operator=(const LocalTensor<T>& other)
{
    if (this != &other) {
        os_.str("");
        this->address_ = other.address_;
#ifndef SPLIT_CORE_CUBE
        if constexpr (IsSameType<PrimType, T>::value) {
            this->shapeInfo_ = other.shapeInfo_;
        }
#endif
    }
    if constexpr (is_tensorTrait_v<T>) {
        this->SetTensorTrait(other.GetTensorTrait());
    }
    return *this;
}

template <typename T>
typename LocalTensor<T>::PrimType* LocalTensor<T>::GetPhyAddr(const uint32_t offset) const
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
#else
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
#endif
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT4_TWO > (offset / INT4_TWO)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT4_TWO)));
        ASCENDC_DEBUG_ASSERT(
            (offset % INT4_TWO == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for int4b_t GetPhyAddr should be an even num."));
        return reinterpret_cast<PrimType*>(this->address_.absAddr) + offset / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT2_FOUR > (offset / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));
        ASCENDC_DEBUG_ASSERT(
            (offset % INT2_FOUR == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for int2b_t GetPhyAddr should be multiples of 4."));
        return reinterpret_cast<PrimType*>(this->address_.absAddr) + offset / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT1_EIGHT > (offset / INT1_EIGHT)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT1_EIGHT)));
        ASCENDC_DEBUG_ASSERT(
            (offset % INT1_EIGHT == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for uint1b_t GetPhyAddr should be multiples of 8."));
        return reinterpret_cast<PrimType*>(this->address_.absAddr) + offset / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT2_FOUR > (offset / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be large than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));
        ASCENDC_DEBUG_ASSERT(
            (offset % INT2_FOUR == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for uint2b_t GetPhyAddr should be divisible by 4."));
        return reinterpret_cast<uint2b_t*>(this->address_.absAddr) + offset / INT2_FOUR;
#endif
    } else {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset * sizeof(PrimType))),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen / sizeof(PrimType))));
        return reinterpret_cast<PrimType*>(this->address_.absAddr) + offset;
    }
}
template <typename T>
typename LocalTensor<T>::PrimType* LocalTensor<T>::GetPhyAddr() const
{
    return GetPhyAddr(0);
}

template <typename T>
__inout_pipe__(S) typename LocalTensor<T>::PrimType LocalTensor<T>::GetValue(const uint32_t offset) const
{
    if ASCEND_IS_AIC {
        if (GetPhyType(AscendC::TPosition(this->GetPosition())) == Hardware::UB) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
            if constexpr (SupportType<PrimType, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
                PrimType ret;
                return ret;
            } else {
                return PrimType(0);
            }
#else
            return 0;
#endif
        }
    }
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT4_TWO > (offset / INT4_TWO)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT4_TWO)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(offset / INT4_TWO);
        return static_cast<int4b_t>(val >> (4 * (offset % INT4_TWO)));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT2_FOUR > (offset / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(offset / INT2_FOUR);
        return static_cast<int2b_t>(val >> (2 * (offset % INT2_FOUR)));
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT1_EIGHT > (offset / INT1_EIGHT)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT1_EIGHT)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(offset / INT1_EIGHT);
        return static_cast<uint1b_t>(val >> (offset % INT1_EIGHT));
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT2_FOUR > (offset / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be large than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(offset / INT2_FOUR);
        return static_cast<uint2b_t>(val >> (2 * (offset % INT2_FOUR)));
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * ConstantsInternal::ASCENDC_B4_TWO > (offset / ConstantsInternal::ASCENDC_B4_TWO)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * ConstantsInternal::ASCENDC_B4_TWO)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(offset / ConstantsInternal::ASCENDC_B4_TWO);
        PrimType ret;
        ret.val = static_cast<uint8_t>(
            (val >> (ConstantsInternal::ASCENDC_B4_BIT_NUM * (offset % ConstantsInternal::ASCENDC_B4_TWO))) & 0xf);
        return ret;
    } else if constexpr (SupportType<PrimType, complex32>()) {
        LocalTensor<uint32_t> tmp = this->ReinterpretCast<uint32_t>();
        uint32_t val = tmp.GetValue(offset);
        return *(reinterpret_cast<PrimType*>(&val));
    } else if constexpr (SupportType<PrimType, complex64>()) {
        LocalTensor<uint64_t> tmp = this->ReinterpretCast<uint64_t>();
        uint64_t val = tmp.GetValue(offset);
        return *(reinterpret_cast<PrimType*>(&val));
#endif
    } else {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset * sizeof(PrimType))),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen / sizeof(PrimType))));
        return *(GetPhyAddr(offset));
    }
}

template <typename T>
__inout_pipe__(S) typename LocalTensor<T>::PrimType& LocalTensor<T>::operator()(const uint32_t offset) const
{
    ASCENDC_DEBUG_ASSERT(
        (this->address_.dataLen > (offset * sizeof(PrimType))),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
            static_cast<uint32_t>(this->address_.dataLen / sizeof(PrimType))));
    return *(GetPhyAddr(offset));
}
template <typename T>
template <typename U>
__aicore__ inline LocalTensor<U> LocalTensor<T>::ReinterpretCast() const
{
    LocalTensor<U> output;
    output.address_.logicPos = static_cast<uint8_t>(this->GetPosition());
    output.address_.bufferHandle = this->GetBufferHandle();
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
#else
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
#endif
        output.address_.dataLen = this->GetSize() / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        output.address_.dataLen = this->GetSize() / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        output.address_.dataLen = this->GetSize() / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        output.address_.dataLen = this->GetSize() / INT2_FOUR;
#endif
    } else {
        output.address_.dataLen = this->GetSize() * sizeof(PrimType);
    }
    output.address_.bufferAddr = this->address_.bufferAddr;
    output.address_.absAddr = this->address_.absAddr;
    if constexpr (is_tensorTrait_v<T> && is_tensorTrait_v<U>) {
        output.GetTensorTrait().SetLayout(this->GetTensorTrait().GetLayout());
    }
    return output;
}

template <typename T>
template <typename U>
__inout_pipe__(S) void LocalTensor<T>::SetValue(const uint32_t index, const U value) const
{
    if ASCEND_IS_AIC {
        if (GetPhyType(AscendC::TPosition(this->GetPosition())) == Hardware::UB) {
            return;
        }
    }
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT4_TWO > (index / INT4_TWO)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "index is %u, which can not be larger than data len %u", index,
                static_cast<uint32_t>(this->address_.dataLen * INT4_TWO)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t shift = (index % INT4_TWO == 0) ? 0 : 4;
        uint32_t idx = index / INT4_TWO;
        uint8_t val = tmp.GetValue(idx) & (0xf << (INT4_BIT_NUM - shift));
        tmp.SetValue(idx, val + (value.storage << shift));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT2_FOUR > (index / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "index is %u, which can not be larger than data len %u", index,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t shift = (index % INT2_FOUR) * 2;
        uint32_t idx = index / INT2_FOUR;
        uint8_t mask = (0x3) << shift;
        uint8_t maskedVal = tmp.GetValue(idx) & (~mask);
        tmp.SetValue(idx, maskedVal + (value.storage << shift));
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT1_EIGHT > (index / INT1_EIGHT)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "index is %u, which can not be larger than data len %u", index,
                static_cast<uint32_t>(this->address_.dataLen * INT1_EIGHT)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t shift = index % INT1_EIGHT;
        uint32_t idx = index / INT1_EIGHT;
        uint8_t mask = (0x1) << shift;
        uint8_t maskedVal = tmp.GetValue(idx) & (~mask);
        tmp.SetValue(idx, maskedVal + (value.storage << shift));
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * INT2_FOUR > (index / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "index is %u, which can not be large than data len %u", index,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint32_t idx = index / INT2_FOUR;
        uint8_t shift = 2 * (index % INT2_FOUR);
        uint8_t val;
        if (index % INT2_FOUR == 0) {
            val = tmp.GetValue(idx) & 0xFC;
        } else if (index % INT2_FOUR == 1) {
            val = tmp.GetValue(idx) & 0xF3;
        } else if (index % INT2_FOUR == 2) {
            val = tmp.GetValue(idx) & 0xCF;
        } else {
            val = tmp.GetValue(idx) & 0x3F;
        }
        tmp.SetValue(idx, val + (value.storage << shift));
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen * ConstantsInternal::ASCENDC_B4_TWO > (index / ConstantsInternal::ASCENDC_B4_TWO)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "index is %u, which can not be larger than data len %u", index,
                static_cast<uint32_t>(this->address_.dataLen * ConstantsInternal::ASCENDC_B4_TWO)));

        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t shift = (index % ConstantsInternal::ASCENDC_B4_TWO == 0) ? 0 : ConstantsInternal::ASCENDC_B4_BIT_NUM;
        uint32_t idx = index / ConstantsInternal::ASCENDC_B4_TWO;
        uint8_t val = tmp.GetValue(idx) & (0xf << (ConstantsInternal::ASCENDC_B4_BIT_NUM - shift));
        tmp.SetValue(idx, val + (value << shift));
#endif
    } else {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (index * sizeof(PrimType))),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "index is %u, which can not be larger than data len %u", index,
                static_cast<uint32_t>(this->address_.dataLen / sizeof(PrimType))));
        *(GetPhyAddr(index)) = PrimType(value);
    }
}

template <typename T>
LocalTensor<T> LocalTensor<T>::operator[](const uint32_t offset) const
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
#else
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
#endif
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset / INT4_TWO)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT4_TWO)));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset / INT1_EIGHT)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT1_EIGHT)));
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset / INT2_FOUR)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be large than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen * INT2_FOUR)));
#endif
    } else {
        ASCENDC_DEBUG_ASSERT(
            (this->address_.dataLen > (offset * sizeof(PrimType))),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "offset is %u, which can not be larger than data len %u", offset,
                static_cast<uint32_t>(this->address_.dataLen / sizeof(PrimType))));
    }

    LocalTensor result = *this;
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
#else
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
#endif
        result.address_.dataLen -= (offset / INT4_TWO);
        result.address_.absAddr = result.address_.absAddr + offset / INT4_TWO;
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        result.address_.dataLen -= (offset / INT2_FOUR);
        result.address_.absAddr = result.address_.absAddr + offset / INT2_FOUR;
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        result.address_.dataLen -= (offset / INT1_EIGHT);
        result.address_.absAddr = result.address_.absAddr + offset / INT1_EIGHT;
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        result.address_.dataLen -= (offset / INT2_FOUR);
        result.address_.absAddr = result.address_.absAddr + offset / INT2_FOUR;
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT2_FOUR;
#endif
    } else {
        result.address_.dataLen -= (offset * sizeof(PrimType));
        result.address_.absAddr = result.address_.absAddr + offset * sizeof(PrimType);
        result.address_.bufferAddr = result.address_.bufferAddr + offset * sizeof(PrimType);
    }
    result.os_.str("");
    return result;
}

template <typename T>
template <typename U>
[[deprecated("NOTICE: SetAddrWithOffset has been deprecated and will be removed in the next version. "
             "Please do not use it!")]] void
LocalTensor<T>::SetAddrWithOffset(LocalTensor<U>& src, uint32_t offset)
{
    this->address_ = src.address_;
    this->address_.bufferAddr += offset * sizeof(PrimT<U>);
    this->address_.absAddr += offset * sizeof(PrimT<U>);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
inline std::string GetComplexStr(const T& val)
{
    static constexpr uint16_t all16BitsOne = 0xFFFF;
    static constexpr uint32_t all32BitsOne = 0xFFFFFFFF;
    static_assert(SupportType<T, complex32, complex64>(), "Unsupported complex type.");
    std::string str = "";
    if constexpr (Std::is_same_v<T, complex32>) {
        if (val.imag.ToFloat() > 0 || !(*(reinterpret_cast<const uint16_t*>(&val.imag)) & all16BitsOne)) {
            str += std::to_string(val.real.ToFloat()) + "+" + std::to_string(val.imag.ToFloat()) + "j ";
        } else {
            str += std::to_string(val.real.ToFloat()) + std::to_string(val.imag.ToFloat()) + "j ";
        }
    } else {
        if (val.imag > 0 || !(*(reinterpret_cast<const uint32_t*>(&val.imag)) & all32BitsOne)) {
            str += std::to_string(val.real) + "+" + std::to_string(val.imag) + "j ";
        } else {
            str += std::to_string(val.real) + std::to_string(val.imag) + "j ";
        }
    }
    return str;
}
#endif

template <typename T>
[[deprecated("NOTICE: Print has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline void
LocalTensor<T>::Print(uint32_t len)
{
    if constexpr (IsSameType<PrimType, half>::value) {
        PrintTypicalFloat(len, sizeof(half));
        return;
    }
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
    (__NPU_ARCH__ == 5102)
    if constexpr (IsSameType<PrimType, bfloat16_t>::value) {
        PrintTypicalFloat(len, sizeof(bfloat16_t));
        return;
    }
    if constexpr (!IsSameType<PrimType, half>::value && !IsSameType<PrimType, bfloat16_t>::value) {
#else
    if constexpr (!IsSameType<PrimType, half>::value) {
#endif
        os_.str("");
        uint32_t printLen = std::min(len, GetSize());
        uint32_t blockNum = ONE_BLK_SIZE / sizeof(PrimType);
        uint32_t rowNum = printLen / blockNum;
        uint32_t residualNum = printLen % blockNum;
        const int32_t width = 4;
        for (uint32_t i = 0; i < rowNum; i++) {
            os_ << std::setw(width) << std::setfill('0') << i * blockNum << " : ";
            for (uint32_t j = 0; j < blockNum; j++) {
                if constexpr ((sizeof(PrimType) == sizeof(int8_t)) || (sizeof(PrimType) == sizeof(bool))) {
                    os_ << static_cast<int32_t>(GetValue(i * blockNum + j)) << " ";
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
                } else if constexpr (
                    Std::is_same_v<PrimType, fp8_e4m3fn_t> || Std::is_same_v<PrimType, fp8_e5m2_t> ||
                    Std::is_same_v<PrimType, hifloat8_t>) {
                    os_ << GetValue(i * blockNum + j).ToFloat() << " ";
                } else if constexpr (Std::is_same_v<PrimType, complex32> || Std::is_same_v<PrimType, complex64>) {
                    PrimType val = GetValue(i * blockNum + j);
                    os_ << GetComplexStr(val);
#endif
                } else {
                    os_ << GetValue(i * blockNum + j) << " ";
                }
            }
            os_ << std::endl;
        }
        if (residualNum != 0) {
            os_ << std::setw(width) << std::setfill('0') << rowNum * blockNum << " : ";
            for (uint32_t i = 0; i < residualNum; i++) {
                if constexpr ((sizeof(PrimType) == sizeof(int8_t)) || (sizeof(PrimType) == sizeof(bool))) {
                    os_ << static_cast<int32_t>(GetValue(rowNum * blockNum + i)) << " ";
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
                } else if constexpr (
                    Std::is_same_v<PrimType, fp8_e4m3fn_t> || Std::is_same_v<PrimType, fp8_e5m2_t> ||
                    Std::is_same_v<PrimType, hifloat8_t>) {
                    os_ << GetValue(rowNum * blockNum + i).ToFloat() << " ";
                } else if constexpr (Std::is_same_v<PrimType, complex32> || Std::is_same_v<PrimType, complex64>) {
                    PrimType val = GetValue(rowNum * blockNum + i);
                    os_ << GetComplexStr(val);
#endif
                } else {
                    os_ << GetValue(rowNum * blockNum + i) << " ";
                }
            }
            os_ << std::endl;
        }
        std::cout << os_.str();
    }
}

template <typename T>
inline void LocalTensor<T>::PrintTypicalFloat(uint32_t len, uint32_t dataSize)
{
    os_.str("");
    uint32_t printLen = std::min(len, GetSize());
    uint32_t blockNum = ONE_BLK_SIZE / dataSize;
    uint32_t rowNum = printLen / blockNum;
    uint32_t residualNum = printLen % blockNum;
    const int32_t width = 4;
    for (uint32_t i = 0; i < rowNum; i++) {
        os_ << std::setw(width) << std::setfill('0') << i * blockNum << " : ";
        for (uint32_t j = 0; j < blockNum; j++) {
            os_ << GetValue(i * blockNum + j).ToFloat() << " ";
        }
        os_ << std::endl;
    }
    if (residualNum != 0) {
        os_ << std::setw(width) << std::setfill('0') << rowNum * blockNum << " : ";
        for (uint32_t i = 0; i < residualNum; i++) {
            os_ << GetValue(rowNum * blockNum + i).ToFloat() << " ";
        }
        os_ << std::endl;
    }
    std::cout << os_.str();
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
inline void LocalTensor<T>::PrintFp4E1M2(uint32_t len) const
{
    std::ostringstream os;
    os.str("");
    uint32_t printLen = std::min(len, GetSize());
    constexpr uint32_t blockNum = ONE_BLK_SIZE * ConstantsInternal::ASCENDC_B4_TWO;
    uint32_t rowNum = printLen / blockNum;
    uint32_t residualNum = printLen % blockNum;
    const int32_t width = 4;
    for (uint32_t i = 0; i < rowNum; i++) {
        os << std::setw(width) << std::setfill('0') << i * blockNum << " : ";
        for (uint32_t j = 0; j < blockNum; j++) {
            fp4x2_e1m2_t fp4Val = GetValue(i * blockNum + j);
            bfloat16_t newbfval = bfloat16::Bf16T(fp4Val);
            os << newbfval.ToFloat() << " ";
        }
        os << std::endl;
    }
    if (residualNum != 0) {
        os << std::setw(width) << std::setfill('0') << rowNum * blockNum << " : ";
        for (uint32_t i = 0; i < residualNum; i++) {
            fp4x2_e1m2_t fp4Val = GetValue(rowNum * blockNum + i);
            bfloat16_t newbfval = bfloat16::Bf16T(fp4Val);
            os << newbfval.ToFloat() << " ";
        }
        os << std::endl;
    }
    std::cout << os.str();
}

template <typename T>
inline void LocalTensor<T>::PrintFp4E2M1(uint32_t len) const
{
    std::ostringstream os;
    os.str("");
    uint32_t printLen = std::min(len, GetSize());
    constexpr uint32_t blockNum = ONE_BLK_SIZE * ConstantsInternal::ASCENDC_B4_TWO;
    uint32_t rowNum = printLen / blockNum;
    uint32_t residualNum = printLen % blockNum;
    const int32_t width = 4;
    for (uint32_t i = 0; i < rowNum; i++) {
        os << std::setw(width) << std::setfill('0') << i * blockNum << " : ";
        for (uint32_t j = 0; j < blockNum; j++) {
            fp4x2_e2m1_t fp4Val = GetValue(i * blockNum + j);
            bfloat16_t newbfval = bfloat16::Bf16T(fp4Val);
            os << newbfval.ToFloat() << " ";
        }
        os << std::endl;
    }
    if (residualNum != 0) {
        os << std::setw(width) << std::setfill('0') << rowNum * blockNum << " : ";
        for (uint32_t i = 0; i < residualNum; i++) {
            fp4x2_e2m1_t fp4Val = GetValue(rowNum * blockNum + i);
            bfloat16_t newbfval = bfloat16::Bf16T(fp4Val);
            os << newbfval.ToFloat() << " ";
        }
        os << std::endl;
    }
    std::cout << os.str();
}
#endif

template <>
[[deprecated("NOTICE: Print has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline void
LocalTensor<half>::Print(uint32_t len)
{
    PrintTypicalFloat(len, sizeof(half));
}

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
    (__NPU_ARCH__ == 5102)
template <>
[[deprecated("NOTICE: Print has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline void
LocalTensor<bfloat16_t>::Print(uint32_t len)
{
    PrintTypicalFloat(len, sizeof(bfloat16_t));
}
#endif

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <>
[[deprecated("NOTICE: Print has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline void
LocalTensor<fp4x2_e2m1_t>::Print(uint32_t len)
{
    PrintFp4E2M1(len);
}

template <>
[[deprecated("NOTICE: Print has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline void
LocalTensor<fp4x2_e1m2_t>::Print(uint32_t len)
{
    PrintFp4E1M2(len);
}
#endif

template <typename T>
[[deprecated("NOTICE: Print has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline void
LocalTensor<T>::Print()
{
    Print(GetSize());
}

template <typename T>
LocalTensor<T>::~LocalTensor()
{}

template <typename T>
[[deprecated("NOTICE: ToFile has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] int32_t
LocalTensor<T>::ToFile(const std::string& fileName) const
{
    return TensorWriteFile(fileName, reinterpret_cast<const PrimType*>(GetPhyAddr()), GetSize() * sizeof(PrimType));
}
// Npu Impl
#else
template <typename T>
__aicore__ inline uint64_t LocalTensor<T>::GetPhyAddr() const
{
    return GetPhyAddr(0);
}
template <typename T>
__aicore__ inline uint64_t LocalTensor<T>::GetPhyAddr(const uint32_t offset) const
{
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        return this->address_.bufferAddr + offset / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        return this->address_.bufferAddr + offset / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        return this->address_.bufferAddr + offset / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<T, uint2b_t>::value) {
        return this->address_.bufferAddr + offset / INT2_FOUR;
#endif
    } else {
        return this->address_.bufferAddr + offset * sizeof(PrimType);
    }
}
template <typename T>
__aicore__ inline __inout_pipe__(S)
    typename LocalTensor<T>::PrimType LocalTensor<T>::GetValue(const uint32_t index) const
{
    if ASCEND_IS_AIC {
        if (GetPhyType(AscendC::TPosition(this->GetPosition())) == Hardware::UB) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
            if constexpr (SupportType<PrimType, fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>()) {
                PrimType ret;
                return ret;
            } else {
                return PrimType(0);
            }
#else
            return PrimType(0);
#endif
        }
    }
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(index / INT4_TWO);
        return static_cast<int4b_t>(val >> (INT4_BIT_NUM * (index % INT4_TWO)));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(index / INT2_FOUR);
        return static_cast<int2b_t>(val >> (INT2_BIT_NUM * (index % INT2_FOUR)));
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(index / INT1_EIGHT);
        return static_cast<uint1b_t>(val >> (INT1_BIT_NUM * (index % INT1_EIGHT)));
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<T, uint2b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t val = tmp.GetValue(index / INT2_FOUR);
        return static_cast<uint2b_t>(val >> (INT2_BIT_NUM * (index % INT2_FOUR)));
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, complex32>()) {
        LocalTensor<uint32_t> tmp = this->ReinterpretCast<uint32_t>();
        uint32_t val = tmp.GetValue(index);
        return *(reinterpret_cast<PrimType*>(&val));
    } else if constexpr (SupportType<PrimType, complex64>()) {
        LocalTensor<uint64_t> tmp = this->ReinterpretCast<uint64_t>();
        uint64_t val = tmp.GetValue(index);
        return *(reinterpret_cast<PrimType*>(&val));
#endif
    } else {
        return *(reinterpret_cast<__ubuf__ PrimType*>(GetPhyAddr(index)));
    }
}
template <typename T>
__aicore__ inline __inout_pipe__(S) __ubuf__
typename LocalTensor<T>::PrimType& LocalTensor<T>::operator()(const uint32_t offset) const
{
    return *(reinterpret_cast<__ubuf__ PrimType*>(GetPhyAddr(offset)));
}

template <typename T>
template <typename U>
__aicore__ inline __sync_alias__ LocalTensor<U> LocalTensor<T>::ReinterpretCast() const
{
    LocalTensor<U> output;
    output.address_.logicPos = static_cast<uint8_t>(this->GetPosition());
    output.address_.bufferHandle = this->GetBufferHandle();
    if constexpr (IsHalfByteDataType<PrimType>()) {
        output.address_.dataLen = this->GetSize() / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        output.address_.dataLen = this->GetSize() / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        output.address_.dataLen = this->GetSize() / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<T, uint2b_t>::value) {
        output.address_.dataLen = this->GetSize() / INT2_FOUR;
#endif
    } else {
        output.address_.dataLen = this->GetSize() * sizeof(PrimType);
    }
    output.address_.bufferAddr = this->address_.bufferAddr;
    if constexpr (is_tensorTrait_v<T> && is_tensorTrait_v<U>) {
        output.SetTensorTrait(this->GetTensorTrait());
    }
    return output;
}

template <typename T>
template <typename U>
__aicore__ inline __inout_pipe__(S) void LocalTensor<T>::SetValue(const uint32_t index, const U value) const
{
    if ASCEND_IS_AIC {
        if (GetPhyType(AscendC::TPosition(this->GetPosition())) == Hardware::UB) {
            return;
        }
    }
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t mask = (index % INT4_TWO == 0) ? 0xf0 : 0xf;
        uint32_t idx = index / INT4_TWO;
        uint8_t val = tmp.GetValue(idx) & mask;
        uint8_t shift = (index % INT4_TWO == 0) ? 0 : INT4_BIT_NUM;
        tmp.SetValue(idx, val + (value.storage << shift));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t shift = (index % INT2_FOUR) * 2;
        uint32_t idx = index / INT2_FOUR;
        uint8_t mask = (0x3) << shift;
        uint8_t maskedVal = tmp.GetValue(idx) & (~mask);
        tmp.SetValue(idx, maskedVal + (value.storage << shift));
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t shift = index % INT1_EIGHT;
        uint32_t idx = index / INT1_EIGHT;
        uint8_t mask = (0x1) << shift;
        uint8_t maskedVal = tmp.GetValue(idx) & (~mask);
        tmp.SetValue(idx, maskedVal + (value.storage << shift));
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint32_t idx = index / INT2_FOUR;
        uint8_t shift = 2 * (index % INT2_FOUR);
        uint8_t val;
        if (index % INT2_FOUR == 0) {
            val = tmp.GetValue(idx) & 0xFC;
        } else if (index % INT2_FOUR == 1) {
            val = tmp.GetValue(idx) & 0xF3;
        } else if (index % INT2_FOUR == 2) {
            val = tmp.GetValue(idx) & 0xCF;
        } else {
            val = tmp.GetValue(idx) & 0x3F;
        }
        tmp.SetValue(idx, val + (value.storage << shift));
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        LocalTensor<uint8_t> tmp = this->ReinterpretCast<uint8_t>();
        uint8_t mask = (index % ConstantsInternal::ASCENDC_B4_TWO == 0) ? 0xf0 : 0xf;
        uint32_t idx = index / ConstantsInternal::ASCENDC_B4_TWO;
        uint8_t val = tmp.GetValue(idx) & mask;
        uint8_t shift = (index % ConstantsInternal::ASCENDC_B4_TWO == 0) ? 0 : ConstantsInternal::ASCENDC_B4_BIT_NUM;
        tmp.SetValue(idx, val + (value << shift));
    } else if constexpr (SupportType<PrimType, complex32, complex64>()) {
        __ubuf__ PrimType* tmpAddr =
            reinterpret_cast<__ubuf__ T*>(static_cast<uint64_t>(this->address_.bufferAddr)) + index;
        PrimType valueAux(value);
        *(reinterpret_cast<__ubuf__ typename PrimType::EleType*>(tmpAddr)) = valueAux.real;
        *(reinterpret_cast<__ubuf__ typename PrimType::EleType*>(tmpAddr) + 1) = valueAux.imag;
#endif
    } else {
        *(reinterpret_cast<__ubuf__ PrimType*>(static_cast<uint64_t>(this->address_.bufferAddr)) + index) =
            static_cast<PrimType>(value);
    }
}

template <typename T>
__aicore__ inline LocalTensor<T> LocalTensor<T>::operator[](const uint32_t offset) const
{
    LocalTensor result = *this;
    if constexpr (IsHalfByteDataType<PrimType>()) {
        result.address_.dataLen -= (offset / INT4_TWO);
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        result.address_.dataLen -= (offset / INT2_FOUR);
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        result.address_.dataLen -= (offset / INT1_EIGHT);
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        result.address_.dataLen -= (offset / INT2_FOUR);
        result.address_.bufferAddr = result.address_.bufferAddr + offset / INT2_FOUR;
#endif
    } else {
        result.address_.dataLen -= (offset * sizeof(PrimType));
        result.address_.bufferAddr = result.address_.bufferAddr + offset * sizeof(PrimType);
    }
    return result;
}

template <typename T>
template <typename U>
[[deprecated("NOTICE: SetAddrWithOffset has been deprecated and will be removed in the next version. "
             "Please do not use it!")]] __aicore__ inline void
LocalTensor<T>::SetAddrWithOffset(LocalTensor<U>& src, uint32_t offset)
{
    this->address_ = src.address_;
    this->address_.bufferAddr += offset * sizeof(PrimT<U>);
}
#endif
template <typename T>
__aicore__ inline int32_t LocalTensor<T>::GetPosition() const
{
    return this->address_.logicPos;
}

template <typename T>
__aicore__ inline void LocalTensor<T>::SetSize(const uint32_t size)
{
#if ASCENDC_CPU_DEBUG
    uint32_t len = IsSameType<PrimType, int4b_t>::value ? size / INT4_TWO : size * sizeof(PrimType);
    ASCENDC_DEBUG_ASSERT(
        ((this->address_.absAddr - (uint8_t*)(GetBaseAddrCpu(int8_t(AscendC::TPosition(this->address_.logicPos)))) +
          len) <=
         ConstDefiner::Instance().bufferInitLen.at(
             ConstDefiner::Instance().positionHardMap.at(AscendC::TPosition(this->address_.logicPos)))),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check param size value in SetSize, current value is %d, buffer overflow", len));
#endif
    if constexpr (IsHalfByteDataType<PrimType>()) {
        this->address_.dataLen = size / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        this->address_.dataLen = size / INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        this->address_.dataLen = size / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        this->address_.dataLen = size / INT2_FOUR;
#endif
    } else {
        this->address_.dataLen = size * sizeof(PrimType);
    }
}
template <typename T>
__aicore__ inline uint32_t LocalTensor<T>::GetSize() const
{
    if constexpr (IsHalfByteDataType<PrimType>()) {
        return this->address_.dataLen * INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
        return this->address_.dataLen * INT2_FOUR;
    } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
        return this->address_.dataLen * INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        return this->address_.dataLen * INT2_FOUR;
#endif
    } else {
        return this->address_.dataLen / sizeof(PrimType);
    }
}

template <typename T>
[[deprecated("NOTICE: GetLength has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] __aicore__ inline uint32_t
LocalTensor<T>::GetLength() const
{
    return this->address_.dataLen;
}

template <typename T>
__aicore__ inline void LocalTensor<T>::SetBufferLen(uint32_t dataLen)
{
    this->address_.dataLen = dataLen;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if ASCEND_IS_AIV {
        this->address_.bufferAddr = set_ub_addr_upper_bound(this->address_.bufferAddr, dataLen * sizeof(T));
    }
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    if (GetPhyType(AscendC::TPosition(this->GetPosition())) == Hardware::UB) {
        this->address_.bufferAddr = set_ub_addr_upper_bound(this->address_.bufferAddr, dataLen * sizeof(T));
    }
#endif
}
template <typename T>
__aicore__ inline void LocalTensor<T>::SetUserTag(const TTagType tag)
{
    auto ptr = reinterpret_cast<TBufType*>(this->address_.bufferHandle);
    ASCENDC_DEBUG_ASSERT((ptr != nullptr), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "ptr can not be nullptr"));
    ptr->usertag = tag;
}
template <typename T>
__aicore__ inline TTagType LocalTensor<T>::GetUserTag() const
{
    auto ptr = reinterpret_cast<TBufType*>(this->address_.bufferHandle);
    ASCENDC_DEBUG_ASSERT((ptr != nullptr), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "ptr can not be nullptr"));
    return ptr->usertag;
}

template <typename T>
template <typename U>
__aicore__ inline void LocalTensor<T>::CreateTensor(AscendC::TPosition pos, uint32_t addr, uint32_t tileSize)
{
#if ASCENDC_CPU_DEBUG
    ASCENDC_DEBUG_ASSERT(
        (pos != AscendC::TPosition::GM) && (pos != AscendC::TPosition::MAX),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "position input should not be GM or MAX"));
    AscendC::Hardware hardPos = ConstDefiner::Instance().positionHardMap.at(pos);
    uint32_t maxLen = ConstDefiner::Instance().bufferInitLen.at(hardPos);
    ASCENDC_DEBUG_ASSERT(
        (addr % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "addr input is %u, which should be 32 bytes align", addr));
    if constexpr (IsHalfByteDataType<PrimType>()) {
        ASCENDC_DEBUG_ASSERT(
            ((tileSize > 0) && ((tileSize % INT4_TWO) == 0) && (tileSize / INT4_TWO) <= maxLen),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "tensor size input is %u, which should be even number in range (0, %u]", tileSize,
                maxLen * INT4_TWO));
    } else {
        ASCENDC_DEBUG_ASSERT(
            ((tileSize > 0) && (tileSize * sizeof(U)) <= maxLen),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "tensor size input is %u, which should be in range (0, %u]", tileSize,
                maxLen / sizeof(U)));
    }
    uint32_t tensorLength = tileSize * SizeOfBits<U>::value / SizeOfBits<uint8_t>::value;
    ASCENDC_DEBUG_ASSERT(
        (tensorLength % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "tensor length is %u bytes, which should be 32 bytes align", tensorLength));
    ASCENDC_DEBUG_ASSERT(
        ((addr >= 0) && ((addr + tensorLength) <= maxLen)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "addr input is %u, tensor length is %u bytes, which exceeds max len %u bytes", addr,
            tensorLength, maxLen));
    this->address_.bufferHandle = nullptr;
    this->address_.absAddr = GetBaseAddrCpu(static_cast<int8_t>(pos)) + addr;
#endif
    this->address_.dataLen = SizeOfBits<U>::value * tileSize / SizeOfBits<uint8_t>::value;
    this->address_.bufferAddr = addr;
    this->address_.logicPos = static_cast<uint8_t>(pos);
}

template <typename T>
__aicore__ inline LocalTensor<T>::LocalTensor(AscendC::TPosition pos, uint32_t addr, uint32_t tileSize)
{
    static_assert(!is_tensorTrait_v<T>, "currently not support TensorTrait type!!");
    CreateTensor<T>(pos, addr, tileSize);
}

template <typename T>
__aicore__ inline LocalTensor<T>::LocalTensor(uint32_t addr)
{
    static_assert(is_tensorTrait_v<T>, "only support TensorTrait type Tensor!");
    static_assert(
        (T::tPos != TPosition::GM) && (T::tPos != TPosition::MAX), "TensorTrait position should not be GM or MAX!");
    using LayoutType = GetLayoutType<T>;
    using type = typename T::LiteType;
    constexpr uint32_t tensorSize = LayoutType::size;
    static_assert((tensorSize * sizeof(type)) % ONE_BLK_SIZE == 0, "The size of localTensor must be align to 32Bytes");
    CreateTensor<typename T::LiteType>(T::tPos, addr, tensorSize);
}

template <typename T>
template <typename U>
__aicore__ inline LocalTensor<T>::LocalTensor(uint32_t addr, const U& layout)
{
    static_assert(is_tensorTrait_v<T>, "only support TensorTrait type Tensor!");
    static_assert(
        (T::tPos != TPosition::GM) && (T::tPos != TPosition::MAX), "TensorTrait position should not be GM or MAX!");
    uint32_t tensorSize = layout.GetSize();
    CreateTensor<typename T::LiteType>(T::tPos, addr, tensorSize);
}

// symbol override
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideAdd<T>& symbolOverride)
{
    symbolOverride.Process(*this);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideSub<T>& symbolOverride)
{
    symbolOverride.Process(*this);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideMul<T>& symbolOverride)
{
    symbolOverride.Process(*this);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideDiv<T>& symbolOverride)
{
    symbolOverride.Process(*this);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideOr<T>& symbolOverride)
{
    symbolOverride.Process(*this);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideAnd<T>& symbolOverride)
{
    symbolOverride.Process(*this);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
template <typename U>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideCompare<U>& symbolOverride)
{
    symbolOverride.Process(*this);
}
#else
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideCompare<float>& symbolOverride)
{
    symbolOverride.Process(*this);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::operator=(const SymbolOverrideCompare<half>& symbolOverride)
{
    symbolOverride.Process(*this);
}
#endif

template <typename T>
__aicore__ inline SymbolOverrideAdd<T> LocalTensor<T>::operator+(const LocalTensor<T>& src1) const
{
    return SymbolOverrideAdd<T>(*this, src1);
}
template <typename T>
__aicore__ inline SymbolOverrideSub<T> LocalTensor<T>::operator-(const LocalTensor<T>& src1) const
{
    return SymbolOverrideSub<T>(*this, src1);
}
template <typename T>
__aicore__ inline SymbolOverrideMul<T> LocalTensor<T>::operator*(const LocalTensor<T>& src1) const
{
    return SymbolOverrideMul<T>(*this, src1);
}
template <typename T>
__aicore__ inline SymbolOverrideDiv<T> LocalTensor<T>::operator/(const LocalTensor<T>& src1) const
{
    return SymbolOverrideDiv<T>(*this, src1);
}
template <typename T>
__aicore__ inline SymbolOverrideOr<T> LocalTensor<T>::operator|(const LocalTensor<T>& src1) const
{
    return SymbolOverrideOr<T>(*this, src1);
}
template <typename T>
__aicore__ inline SymbolOverrideAnd<T> LocalTensor<T>::operator&(const LocalTensor<T>& src1) const
{
    return SymbolOverrideAnd<T>(*this, src1);
}
template <typename T>
__aicore__ inline SymbolOverrideCompare<T> LocalTensor<T>::operator<(const LocalTensor<T>& src1) const
{
    return SymbolOverrideCompare<T>(*this, src1, CMPMODE::LT);
}
template <typename T>
__aicore__ inline SymbolOverrideCompare<T> LocalTensor<T>::operator>(const LocalTensor<T>& src1) const
{
    return SymbolOverrideCompare<T>(*this, src1, CMPMODE::GT);
}
template <typename T>
__aicore__ inline SymbolOverrideCompare<T> LocalTensor<T>::operator!=(const LocalTensor<T>& src1) const
{
    return SymbolOverrideCompare<T>(*this, src1, CMPMODE::NE);
}
template <typename T>
__aicore__ inline SymbolOverrideCompare<T> LocalTensor<T>::operator==(const LocalTensor<T>& src1) const
{
    return SymbolOverrideCompare<T>(*this, src1, CMPMODE::EQ);
}
template <typename T>
__aicore__ inline SymbolOverrideCompare<T> LocalTensor<T>::operator<=(const LocalTensor<T>& src1) const
{
    return SymbolOverrideCompare<T>(*this, src1, CMPMODE::LE);
}
template <typename T>
__aicore__ inline SymbolOverrideCompare<T> LocalTensor<T>::operator>=(const LocalTensor<T>& src1) const
{
    return SymbolOverrideCompare<T>(*this, src1, CMPMODE::GE);
}
template <typename T>
__aicore__ inline void LocalTensor<T>::SetShapeInfo(const ShapeInfo& shapeInfo)
{
    static_assert(IsSameType<T, PrimType>::value, "only primitive type Tensor has shape info!");
#ifndef SPLIT_CORE_CUBE
    shapeInfo_ = shapeInfo;
#endif
}
template <typename T>
__aicore__ inline ShapeInfo LocalTensor<T>::GetShapeInfo() const
{
    static_assert(IsSameType<T, PrimType>::value, "only primitive type Tensor has shape info!");
#ifndef SPLIT_CORE_CUBE
    return shapeInfo_;
#else
    ShapeInfo tmp;
    return tmp;
#endif
}

template <typename T>
__aicore__ inline void GlobalTensor<T>::SetGlobalBuffer(
    __gm__ typename GlobalTensor<T>::PrimType* buffer, uint64_t bufferSize)
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    this->oriAddress_ = buffer;
    // For the case GlobalTensor set through KFC message, origin cacheMode is lost.
    if (this->cacheMode_ == CacheMode::CACHE_MODE_NORMAL) {
        this->cacheMode_ = ToCacheModeEnum(ExtractCacheMode(buffer));
        this->oriAddress_ = ExtractL2CacheGmAddr(buffer);
        this->address_ = buffer;
    } else {
        this->address_ = L2CacheAlter<PrimType, CacheRwMode::RW>(buffer, this->cacheMode_);
    }
#else
    if (this->cacheMode_ == CacheMode::CACHE_MODE_NORMAL) {
        this->address_ = buffer;
    } else {
        this->address_ = L2CacheAlter<PrimType, CacheRwMode::RW>(buffer, cacheMode_);
    }
    this->oriAddress_ = buffer;
#endif
    bufferSize_ = bufferSize;
}

template <typename T>
__aicore__ inline void GlobalTensor<T>::SetGlobalBuffer(__gm__ typename GlobalTensor<T>::PrimType* buffer)
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    this->oriAddress_ = buffer;
    // For the case GlobalTensor set through KFC message, origin cacheMode is lost.
    if (this->cacheMode_ == CacheMode::CACHE_MODE_NORMAL) {
        this->cacheMode_ = ToCacheModeEnum(ExtractCacheMode(buffer));
        this->oriAddress_ = ExtractL2CacheGmAddr(buffer);
        this->address_ = buffer;
    } else {
        this->address_ = L2CacheAlter<PrimType, CacheRwMode::RW>(buffer, this->cacheMode_);
    }
#else
    if (this->cacheMode_ == CacheMode::CACHE_MODE_NORMAL) {
        this->address_ = buffer;
    } else {
        this->address_ = L2CacheAlter<PrimType, CacheRwMode::RW>(buffer, cacheMode_);
    }
    this->oriAddress_ = buffer;
#endif
#if ASCENDC_CPU_DEBUG
    bufferSize_ = 0;
#endif
}

template <typename T>
__aicore__ inline const __gm__ typename GlobalTensor<T>::PrimType* GlobalTensor<T>::GetPhyAddr() const
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    return ExtractL2CacheGmAddr(this->address_);
#else
    return this->address_;
#endif
}

template <typename T>
__aicore__ inline __gm__ typename GlobalTensor<T>::PrimType* GlobalTensor<T>::GetPhyAddr(const uint64_t offset) const
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (offset % 2 == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for int4b_t GetPhyAddr should be an even num."));
        return ExtractL2CacheGmAddr(this->address_ + offset / INT4_TWO);
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, int2b_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (offset % 4 == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for int2b_t GetPhyAddr should be multiples of 4."));
        return ExtractL2CacheGmAddr(this->address_ + offset / INT2_FOUR);
    } else if constexpr (SupportType<PrimType, uint1b_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (offset % 8 == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for uint1b_t GetPhyAddr should be multiples of 8."));
        return ExtractL2CacheGmAddr(this->address_ + offset / INT1_EIGHT);
#endif
    } else {
        return ExtractL2CacheGmAddr(this->address_ + offset);
    }
#else
    if constexpr (IsHalfByteDataType<PrimType>()) {
        ASCENDC_DEBUG_ASSERT(
            (offset % 2 == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for int4b_t GetPhyAddr should be an even num."));
        return this->address_ + offset / INT4_TWO;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint4b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (offset % 2 == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for uint4b_t GetPhyAddr should be an even num."));
        return this->address_ + offset / INT4_TWO;
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (offset % 4 == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "The offset for uint2b_t GetPhyAddr should be divisible by 4."));
        return this->address_ + offset / INT2_FOUR;
#endif
    } else {
        return this->address_ + offset;
    }
#endif
}

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
template <typename T>
template <typename U>
__aicore__ inline uintptr_t GlobalTensor<T>::AlignPtr(__gm__ U* buffer) const
{
    uintptr_t addr = (reinterpret_cast<uintptr_t>(buffer)) & (~0b111111);
    return addr;
}

template <typename T>
__aicore__ inline void GlobalTensor<T>::DcciCacheLine(uintptr_t cacheAddr) const
{
    dcci(reinterpret_cast<__gm__ uint64_t*>(cacheAddr), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
}

template <typename T>
template <typename U>
__aicore__ inline void GlobalTensor<T>::DcciReadCache(__gm__ U* buffer)
{
    uintptr_t tmpAddr = AlignPtr<U>(buffer);
    if (tmpAddr != lastReadCacheAddr) {
        lastReadCacheAddr = tmpAddr;
        DcciCacheLine(lastReadCacheAddr);
    }
}

template <typename T>
template <typename U>
__aicore__ inline void GlobalTensor<T>::DcciWriteCache(__gm__ U* buffer)
{
    uintptr_t tmpAddr = AlignPtr<U>(buffer);
    if (lastWriteCacheAddr == 0) {
        lastWriteCacheAddr = tmpAddr;
    } else if (tmpAddr != lastWriteCacheAddr) {
        DcciCacheLine(lastWriteCacheAddr);
        lastWriteCacheAddr = tmpAddr;
    }
}
#endif

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
template <typename T>
__aicore__ inline __inout_pipe__(S) typename GlobalTensor<T>::PrimType GlobalTensor<T>::GetValue(const uint64_t offset)
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT4_TWO;
        DcciReadCache(addr);
        return static_cast<T>((*addr) >> (INT4_BIT_NUM * (offset % INT4_TWO)));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, int2b_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT2_FOUR;
        DcciReadCache(addr);
        return static_cast<T>((*addr) >> (INT2_BIT_NUM * (offset % INT2_FOUR)));
    } else if constexpr (SupportType<PrimType, uint1b_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT1_EIGHT;
        DcciReadCache(addr);
        return static_cast<T>((*addr) >> (INT1_BIT_NUM * (offset % INT1_EIGHT)));
#endif
    } else {
        __gm__ T* addr = ExtractL2CacheGmAddr(this->address_ + offset);
        DcciReadCache(addr);
        if constexpr (SupportType<PrimType, complex32, complex64>()) {
            return T(addr->real, addr->imag);
        } else {
            return *addr;
        }
    }
#else
    if constexpr (IsHalfByteDataType<PrimType>()) {
        __gm__ uint8_t* addr = reinterpret_cast<__gm__ uint8_t*>(this->oriAddress_) + offset / INT4_TWO;
        DcciReadCache(addr);
        return static_cast<PrimType>((*addr) >> (INT4_BIT_NUM * (offset % INT4_TWO)));
    } else {
        DcciReadCache(this->oriAddress_ + offset);
        return this->oriAddress_[offset];
    }
#endif
}
#endif

template <typename T>
__aicore__ inline __inout_pipe__(S)
    typename GlobalTensor<T>::PrimType GlobalTensor<T>::GetValue(const uint64_t offset) const
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT4_TWO;
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciCacheLine(AlignPtr<uint8_t>(addr));
#endif
        return static_cast<T>((*addr) >> (INT4_BIT_NUM * (offset % INT4_TWO)));
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, int2b_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT2_FOUR;
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciCacheLine(AlignPtr<uint8_t>(addr));
#endif
        return static_cast<T>((*addr) >> (INT2_BIT_NUM * (offset % INT2_FOUR)));
    } else if constexpr (SupportType<PrimType, uint1b_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT1_EIGHT;
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciCacheLine(AlignPtr<uint8_t>(addr));
#endif
        return static_cast<T>((*addr) >> (INT1_BIT_NUM * (offset % INT1_EIGHT)));
#endif
    } else {
        __gm__ T* addr = ExtractL2CacheGmAddr(this->address_ + offset);
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciCacheLine(AlignPtr<T>(addr));
#endif
        if constexpr (SupportType<PrimType, complex32, complex64>()) {
            return T(addr->real, addr->imag);
        } else {
            return *addr;
        }
    }
#else
    if constexpr (IsHalfByteDataType<PrimType>()) {
        __gm__ uint8_t* addr = reinterpret_cast<__gm__ uint8_t*>(this->oriAddress_) + offset / INT4_TWO;
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciCacheLine(AlignPtr<uint8_t>(addr));
#endif
        return static_cast<PrimType>((*addr) >> (INT4_BIT_NUM * (offset % INT4_TWO)));
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        __gm__ uint8_t* addr = reinterpret_cast<__gm__ uint8_t*>(this->oriAddress_) + offset / INT2_FOUR;
        return static_cast<T>((*addr) >> (INT2_BIT_NUM * (offset % INT2_FOUR)));
#endif
    } else {
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciCacheLine(AlignPtr<PrimType>(this->oriAddress_ + offset));
#endif
        return this->oriAddress_[offset];
    }
#endif
}

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
template <typename T>
__aicore__ inline __inout_pipe__(S) __gm__
typename GlobalTensor<T>::PrimType& GlobalTensor<T>::operator()(const uint64_t offset)
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    __gm__ PrimType* addr = ExtractL2CacheGmAddr(this->address_);
    DcciReadCache(addr + offset);
    return addr[offset];
#else
    DcciReadCache(this->oriAddress_ + offset);
    return this->oriAddress_[offset];
#endif
}
#endif

template <typename T>
__aicore__ inline __inout_pipe__(S) __gm__
typename GlobalTensor<T>::PrimType& GlobalTensor<T>::operator()(const uint64_t offset) const
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    __gm__ PrimType* addr = ExtractL2CacheGmAddr(this->address_);
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    DcciCacheLine(AlignPtr<PrimType>(addr + offset));
#endif
    return addr[offset];
#else
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    DcciCacheLine(AlignPtr<PrimType>(this->oriAddress_ + offset));
#endif
    return this->oriAddress_[offset];
#endif
}

template <typename T>
__aicore__ inline void GlobalTensor<T>::SetValue(const uint64_t offset, typename GlobalTensor<T>::PrimType value)
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (SupportType<PrimType, int4b_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT4_TWO;
        uint8_t mask = (offset % INT4_TWO == 0) ? 0xf0 : 0xf;

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciWriteCache(addr);
#endif
        uint8_t val = (*addr) & mask;
        uint8_t shift = (offset % INT4_TWO == 0) ? 0 : INT4_BIT_NUM;
        *addr = val + (value.storage << shift);
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, int2b_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT2_FOUR;

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciWriteCache(addr);
#endif
        uint8_t shift = (offset % INT2_FOUR) * 2;
        uint8_t mask = (0x3) << shift;
        uint8_t maskedVal = (*addr) & (~mask);
        *addr = maskedVal + (value.storage << shift);
    } else if constexpr (SupportType<PrimType, uint1b_t>()) {
        __gm__ uint8_t* addr =
            reinterpret_cast<__gm__ uint8_t*>(ExtractL2CacheGmAddr(this->address_)) + offset / INT1_EIGHT;

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciWriteCache(addr);
#endif
        uint8_t shift = offset % INT1_EIGHT;
        uint8_t mask = (0x1) << shift;
        uint8_t maskedVal = (*addr) & (~mask);
        *addr = maskedVal + (value.storage << shift);
#endif
    } else {
        __gm__ PrimType* addr = ExtractL2CacheGmAddr(this->address_ + offset);
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciWriteCache(addr);
#endif
        *addr = value;
    }
#else
    if constexpr (IsHalfByteDataType<PrimType>()) {
        __gm__ uint8_t* addr = reinterpret_cast<__gm__ uint8_t*>(this->oriAddress_) + offset / INT4_TWO;
        uint8_t mask = (offset % INT4_TWO == 0) ? 0xf0 : 0xf;

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciWriteCache(addr);
#endif
        uint8_t val = (*addr) & mask;
        uint8_t shift = (offset % INT4_TWO == 0) ? 0 : INT4_BIT_NUM;
        *addr = val + (value.storage << shift);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<T, uint2b_t>::value) {
        __gm__ uint8_t* addr = reinterpret_cast<__gm__ uint8_t*>(this->oriAddress_) + offset / INT2_FOUR;
        // example: origin data is 0b10110110, need to set higheset 2bit to 0b11, offset is 3
        // mask is 0xCF(0b00111111), after mask operation, origin data becomes 0b00110110
        // shift is 6, findal val is 0b00110110 + 0b11 << 6 = 0b11110110
        uint8_t mask = 0;
        if (offset % INT2_FOUR == 0) {
            mask = 0xFC;
        } else if (offset % INT2_FOUR == 1) {
            mask = 0xF3;
        } else if (offset % INT2_FOUR == 2) {
            mask = 0xCF;
        } else {
            mask = 0x3F;
        }

        uint8_t val = (*addr) & mask;
        uint8_t shift = 2 * (offset % INT2_FOUR);
        *addr = val + (value.storage << shift);
#endif
    } else {
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
        DcciWriteCache(this->oriAddress_ + offset);
#endif
        this->oriAddress_[offset] = value;
    }
#endif
}

template <typename T>
__aicore__ inline GlobalTensor<T>::GlobalTensor()
{
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    this->lastWriteCacheAddr = 0;
    this->lastReadCacheAddr = 0;
#endif
}

#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
template <typename T>
__aicore__ inline GlobalTensor<T>::~GlobalTensor()
{
    if (lastWriteCacheAddr != 0) {
        DcciCacheLine(lastWriteCacheAddr);
    }
}
#endif

template <typename T>
__aicore__ inline uint64_t GlobalTensor<T>::GetSize() const
{
    return bufferSize_;
}

template <typename T>
__aicore__ inline GlobalTensor<T> GlobalTensor<T>::operator[](const uint64_t offset) const
{
    GlobalTensor result = *this;
    if constexpr (IsHalfByteDataType<PrimType>()) {
        result.address_ = result.address_ + offset / INT4_TWO;
        result.oriAddress_ = result.oriAddress_ + offset / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (SupportType<PrimType, int2b_t>()) {
        result.address_ = result.address_ + offset / INT2_FOUR;
        result.oriAddress_ = result.oriAddress_ + offset / INT2_FOUR;
    } else if constexpr (SupportType<PrimType, uint1b_t>()) {
        result.address_ = result.address_ + offset / INT1_EIGHT;
        result.oriAddress_ = result.oriAddress_ + offset / INT1_EIGHT;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    } else if constexpr (IsSameType<PrimType, uint2b_t>::value) {
        result.address_ = result.address_ + offset / INT2_FOUR;
        result.oriAddress_ = result.oriAddress_ + offset / INT2_FOUR;
#endif
    } else {
        result.address_ = result.address_ + offset;
        result.oriAddress_ = result.oriAddress_ + offset;
    }
    return result;
}

template <typename T>
__aicore__ inline void GlobalTensor<T>::SetShapeInfo(const ShapeInfo& shapeInfo)
{
    static_assert(IsSameType<T, PrimType>::value, "only primitive type Tensor has shape info!");
#ifndef SPLIT_CORE_CUBE
    shapeInfo_ = shapeInfo;
#endif
}

template <typename T>
__aicore__ inline ShapeInfo GlobalTensor<T>::GetShapeInfo() const
{
    static_assert(IsSameType<T, PrimType>::value, "only primitive type Tensor has shape info!");
#ifndef SPLIT_CORE_CUBE
    return shapeInfo_;
#else
    ShapeInfo tmp;
    return tmp;
#endif
}

template <typename T>
template <CacheRwMode rwMode>
__aicore__ inline void GlobalTensor<T>::SetL2CacheHint(CacheMode mode)
{
    this->cacheMode_ = mode;
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    this->address_ = L2CacheAlter<PrimType, rwMode>(this->address_, mode);
#else
    if (mode == CacheMode::CACHE_MODE_NORMAL) {
        this->address_ = this->oriAddress_;
    } else {
        this->address_ = L2CacheAlter<PrimType, rwMode>(this->oriAddress_, mode);
    }
#if defined(ASCENDC_OOM) && ASCENDC_OOM == 1
    AscendC::OOMAddAddrForL2Cache<PrimType>(this->address_, this->oriAddress_);
#endif // defined(ASCENDC_OOM) && ASCENDC_OOM == 1
#endif
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
template <typename U>
__aicore__ inline GlobalTensor<U> GlobalTensor<T>::ReinterpretCast() const
{
    GlobalTensor<U> output;
    output.address_ = reinterpret_cast<__gm__ U*>(reinterpret_cast<__gm__ int64_t*>(this->address_));
    output.oriAddress_ = reinterpret_cast<__gm__ U*>(reinterpret_cast<__gm__ int64_t*>(this->oriAddress_));
    output.bufferSize_ = this->GetSize() * SizeOfBits<T>::value / SizeOfBits<U>::value;
    output.cacheMode_ = this->cacheMode_;
#ifndef SPLIT_CORE_CUBE
    output.shapeInfo_ = this->shapeInfo_;
#endif
    return output;
}
#endif

template <Hardware hard>
__aicore__ inline LocalMemAllocator<hard>::LocalMemAllocator()
{
    static_assert((hard != Hardware::GM) && (hard != Hardware::MAX) && "illegal hardware position GM or MAX");
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    ASCENDC_DEBUG_ASSERT(
        ConstDefiner::Instance().CheckAllocatorUsed(hard) != true,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "only one LocalMemAllocator can exist at the same hardware position at any given time."));
#endif
    if constexpr (hard == Hardware::UB) {
        head_ = GetDynamicMemStartPos<hard>();
    }
}

template <Hardware hard>
__aicore__ inline uint32_t LocalMemAllocator<hard>::GetCurAddr() const
{
    return head_;
}

template <Hardware hard>
template <TPosition pos, class DataType, uint32_t tileSize>
__aicore__ inline LocalTensor<DataType> LocalMemAllocator<hard>::Alloc()
{
    static_assert(!is_tensorTrait_v<DataType>, "currently not support TensorTrait type!");
    static_assert(GetPhyType(pos) == hard, "logic pos and hardware pos not matched.");
    LocalTensor<DataType> output(pos, head_, tileSize);
    head_ += SizeOfBits<DataType>::value * tileSize / SizeOfBits<uint8_t>::value;
    return output;
}

template <Hardware hard>
template <class DataType, uint32_t tileSize>
__aicore__ inline LocalTensor<DataType> LocalMemAllocator<hard>::Alloc()
{
    static_assert(!is_tensorTrait_v<DataType>, "currently not support TensorTrait type!");
    LocalTensor<DataType> output(GetDefaultPosition(hard), head_, tileSize);
    head_ += SizeOfBits<DataType>::value * tileSize / SizeOfBits<uint8_t>::value;
    return output;
}

template <Hardware hard>
template <TPosition pos, class DataType>
__aicore__ inline LocalTensor<DataType> LocalMemAllocator<hard>::Alloc(uint32_t tileSize)
{
    static_assert(!is_tensorTrait_v<DataType>, "currently not support TensorTrait type!");
    static_assert(GetPhyType(pos) == hard, "logic pos and hardware pos not matched.");
    LocalTensor<DataType> output(pos, head_, tileSize);
    head_ += SizeOfBits<DataType>::value * tileSize / SizeOfBits<uint8_t>::value;
    return output;
}

template <Hardware hard>
template <class DataType>
__aicore__ inline LocalTensor<DataType> LocalMemAllocator<hard>::Alloc(uint32_t tileSize)
{
    static_assert(!is_tensorTrait_v<DataType>, "currently not support TensorTrait type!");
    LocalTensor<DataType> output(GetDefaultPosition(hard), head_, tileSize);
    head_ += SizeOfBits<DataType>::value * tileSize / SizeOfBits<uint8_t>::value;
    return output;
}
template <Hardware hard>
template <class DataType>
__aicore__ inline LocalTensor<DataType> LocalMemAllocator<hard>::Alloc()
{
    static_assert(is_tensorTrait_v<DataType>, "only support TensorTrait type!");
    static_assert(GetPhyType(DataType::tPos) == hard, "logic pos and hardware pos not matched.");
    using liteType = typename DataType::LiteType;
    static_assert(
        SupportBytes<liteType, B8_BYTE_SIZE, B16_BYTE_SIZE, B32_BYTE_SIZE, B64_BYTE_SIZE>(),
        "Only support B8/B16/B32/B64 datatype");
    LocalTensor<DataType> tensorOut(head_);
    head_ += SizeOfBits<liteType>::value * tensorOut.GetSize() / SizeOfBits<uint8_t>::value;
    return tensorOut;
}

template <Hardware hard>
template <class DataType, typename LayoutType>
__aicore__ inline typename Std::enable_if<is_layout_v<LayoutType>, LocalTensor<DataType>>::type
LocalMemAllocator<hard>::Alloc(const LayoutType& layout)
{
    static_assert(is_tensorTrait_v<DataType>, "only support TensorTrait type!");
    static_assert(GetPhyType(DataType::tPos) == hard, "logic pos and hardware pos not matched.");
    using liteType = typename DataType::LiteType;
    static_assert(
        SupportBytes<liteType, B8_BYTE_SIZE, B16_BYTE_SIZE, B32_BYTE_SIZE, B64_BYTE_SIZE>(),
        "Only support B8/B16/B32/B64 datatype");
    LocalTensor<DataType> tensorOut(head_, layout);
    head_ += SizeOfBits<liteType>::value * tensorOut.GetSize() / SizeOfBits<uint8_t>::value;
    return tensorOut;
}
#endif // defined(__NPU_ARCH__)
} // namespace AscendC
#endif // KERNEL_TENSOR_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_IMPL_H__
#endif
