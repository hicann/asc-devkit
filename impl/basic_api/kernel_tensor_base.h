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
 * \file kernel_tensor_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_tensor_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_BASE_H__
#endif
#ifndef ASCENDC_MODULE_TENSOR_BASE_H
#define ASCENDC_MODULE_TENSOR_BASE_H
#include <cstdint>
#include "kernel_utils.h"
#include "kernel_operator_layout.h"
#include "kernel_operator_tensor_trait.h"
#ifdef __ASCENDC_SUPER_KERNEL_DEBUG__
#include "../utils/debug/asc_aicore_printf_impl.h"
#endif

namespace AscendC {
using TBufHandle = uint8_t*;
using TEventID = int8_t;
using TTagType = int32_t;

template <typename T>
struct GetTypeFromTrait;

template <typename T, TPosition pos, typename Shape, typename Stride>
struct GetTypeFromTrait<TensorTrait<T, pos, Layout<Shape, Stride>>> {
    using LayoutType = Layout<Shape, Stride>;
    using ShapeType = Shape;
    using StrideType = Stride;
    using ShapeRows = typename Std::tuple_element<0, Shape>::type;
    using ShapeColumns = typename Std::tuple_element<1, Shape>::type;
    using StrideRows = typename Std::tuple_element<0, Stride>::type;
    using StrideColumns = typename Std::tuple_element<1, Stride>::type;
};

template <typename T, TPosition pos, typename Shape, typename Stride>
struct GetTypeFromTrait<LocalTensor<TensorTrait<T, pos, Layout<Shape, Stride>>>>
    : public GetTypeFromTrait<TensorTrait<T, pos, Layout<Shape, Stride>>> {};

template <typename T, TPosition pos, typename Shape, typename Stride>
struct GetTypeFromTrait<GlobalTensor<TensorTrait<T, pos, Layout<Shape, Stride>>>>
    : public GetTypeFromTrait<TensorTrait<T, pos, Layout<Shape, Stride>>> {};

template <typename T>
using GetLayoutType = Std::remove_cvref_t<typename GetTypeFromTrait<T>::LayoutType>;

template <typename T>
using GetShapeType = Std::remove_cvref_t<typename GetTypeFromTrait<T>::ShapeType>;

template <typename T>
using GetStrideType = Std::remove_cvref_t<typename GetTypeFromTrait<T>::StrideType>;

template <typename T>
using GetShapeRows = Std::remove_cvref_t<typename GetTypeFromTrait<T>::ShapeRows>;

template <typename T>
using GetShapeColumns = Std::remove_cvref_t<typename GetTypeFromTrait<T>::ShapeColumns>;

template <typename T>
using GetStrideRows = Std::remove_cvref_t<typename GetTypeFromTrait<T>::StrideRows>;

template <typename T>
using GetStrideColumns = Std::remove_cvref_t<typename GetTypeFromTrait<T>::StrideColumns>;

enum class TBufState : uint8_t {
    FREE = 0,
    OCCUPIED,
    ENQUE,
    DEQUE,
};

struct TBufType {
    TBufState state;
    HardEvent freeBufEvt;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    union {
        struct {
            TEventID enQueEvtID;
            TEventID freeBufEvtID;
        };
        struct {
            TBufId bufId;
            TBufId bufIdAlt;
        };
    };
#else
    TEventID enQueEvtID;
    TEventID freeBufEvtID;
#endif
    uint32_t address;
    uint32_t dataLen;
    TTagType usertag;
    DEBUG_CODE(HardEvent userEnQueEvt);
};

struct TBuffAddr {
    uint32_t dataLen;
    uint32_t bufferAddr;
    TBufHandle bufferHandle;
    uint8_t logicPos;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint8_t* absAddr;
#endif
};

template <typename T>
class BaseLocalTensor {
public:
    using PrimType = PrimT<T>;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    void SetAddr(const TBuffAddr& address) { this->address_ = address; }
    [[deprecated("NOTICE: InitBuffer has been deprecated and will be removed in the next version. "
                 "Please do not use it!")]] void
    InitBuffer(const uint32_t bufferOffset, const uint32_t bufferSize)
    {
#if (__NPU_ARCH__ == 5102)
        if constexpr (
            IsSameType<PrimType, int4b_t>::value || IsSameType<PrimType, int2b_t>::value ||
            IsSameType<PrimType, uint1b_t>::value) {
#else
        if constexpr (IsSameType<PrimType, int4b_t>::value) {
#endif
            ASCENDC_DEBUG_ASSERT(
                (bufferSize != 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "InitBuffer bufferSize must be larger than 0."));

            ASCENDC_DEBUG_ASSERT(
                (bufferOffset % ONE_BLK_SIZE == 0),
                KERNEL_LOG_INTERNAL(KERNEL_ERROR, "bufferOffset is %u, which should be 32Bytes aligned", bufferOffset));
        }

        ASCENDC_DEBUG_ASSERT(
            (TPosition(this->address_.logicPos) != TPosition::GM),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "logicPos can not be gm when init buffer"));
        auto positionHardMap = ConstDefiner::Instance().positionHardMap;
        ASCENDC_DEBUG_ASSERT(
            (positionHardMap.count(AscendC::TPosition(this->address_.logicPos)) != 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal logic pos %d", this->address_.logicPos));
        if constexpr (IsSameType<PrimType, int4b_t>::value) {
            ASCENDC_DEBUG_ASSERT(
                (bufferOffset + bufferSize * INT4_BIT_NUM / ONE_BYTE_BIT_SIZE <=
                 ConstDefiner::Instance().bufferInitLen.at(
                     positionHardMap.at(AscendC::TPosition(this->address_.logicPos)))),
                KERNEL_LOG_INTERNAL(
                    KERNEL_ERROR, "bufferOffset is %d, bufferSize is %d, buffer overflow", bufferOffset, bufferSize));

            this->address_.absAddr = ConstDefiner::Instance().hardwareCpuBufferMap.at(
                                         positionHardMap.at(AscendC::TPosition(this->address_.logicPos))) +
                                     bufferOffset;
            this->address_.dataLen = bufferSize * INT4_BIT_NUM / ONE_BYTE_BIT_SIZE;
#if (__NPU_ARCH__ == 5102)
        } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
            ASCENDC_DEBUG_ASSERT(
                (bufferOffset + bufferSize * INT2_BIT_NUM / ONE_BYTE_BIT_SIZE <=
                 ConstDefiner::Instance().bufferInitLen.at(
                     positionHardMap.at(AscendC::TPosition(this->address_.logicPos)))),
                KERNEL_LOG_INTERNAL(
                    KERNEL_ERROR, "bufferOffset is %d, bufferSize is %d, buffer overflow", bufferOffset, bufferSize));

            this->address_.absAddr = ConstDefiner::Instance().hardwareCpuBufferMap.at(
                                         positionHardMap.at(AscendC::TPosition(this->address_.logicPos))) +
                                     bufferOffset;
            this->address_.dataLen = bufferSize * INT2_BIT_NUM / ONE_BYTE_BIT_SIZE;
        } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
            ASCENDC_DEBUG_ASSERT(
                (bufferOffset + bufferSize * INT1_BIT_NUM / ONE_BYTE_BIT_SIZE <=
                 ConstDefiner::Instance().bufferInitLen.at(
                     positionHardMap.at(AscendC::TPosition(this->address_.logicPos)))),
                KERNEL_LOG_INTERNAL(
                    KERNEL_ERROR, "bufferOffset is %d, bufferSize is %d, buffer overflow", bufferOffset, bufferSize));

            this->address_.absAddr = ConstDefiner::Instance().hardwareCpuBufferMap.at(
                                         positionHardMap.at(AscendC::TPosition(this->address_.logicPos))) +
                                     bufferOffset;
            this->address_.dataLen = bufferSize * INT1_BIT_NUM / ONE_BYTE_BIT_SIZE;
#endif
        } else {
            ASCENDC_DEBUG_ASSERT(
                (bufferOffset % ONE_BLK_SIZE == 0),
                KERNEL_LOG_INTERNAL(KERNEL_ERROR, "bufferOffset is %u, which should be 32Bytes aligned", bufferOffset));
            ASCENDC_DEBUG_ASSERT(
                (bufferOffset + bufferSize * sizeof(PrimType) <=
                 ConstDefiner::Instance().bufferInitLen.at(
                     positionHardMap.at(AscendC::TPosition(this->address_.logicPos)))),
                KERNEL_LOG_INTERNAL(
                    KERNEL_ERROR, "bufferOffset is %d, bufferSize is %d, buffer overflow", bufferOffset, bufferSize));
            ASCENDC_DEBUG_ASSERT(
                (bufferSize != 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "InitBuffer bufferSize must be larger than 0."));
            this->address_.absAddr = ConstDefiner::Instance().hardwareCpuBufferMap.at(
                                         positionHardMap.at(AscendC::TPosition(this->address_.logicPos))) +
                                     bufferOffset;
            this->address_.dataLen = bufferSize * sizeof(PrimType);
        }
    }
#else
    __aicore__ inline void SetAddr(const TBuffAddr& bufferAddr) { this->address_ = bufferAddr; }
    [[deprecated("NOTICE: InitBuffer has been deprecated and will be removed in the next version. "
                 "Please do not use it!")]] __aicore__ inline void
    InitBuffer(const uint32_t bufferOffset, const uint32_t bufferSize)
    {
        this->address_.bufferAddr = get_imm(0) + bufferOffset;
        if constexpr (IsSameType<PrimType, int4b_t>::value) {
            this->address_.dataLen = bufferSize * INT4_BIT_NUM / ONE_BYTE_BIT_SIZE;
#if (__NPU_ARCH__ == 5102)
        } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
            this->address_.dataLen = bufferSize * INT2_BIT_NUM / ONE_BYTE_BIT_SIZE;
        } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
            this->address_.dataLen = bufferSize * INT1_BIT_NUM / ONE_BYTE_BIT_SIZE;
#endif
        } else {
            this->address_.dataLen = bufferSize * sizeof(PrimType);
        }
    }
#endif
    __aicore__ inline TBufHandle GetBufferHandle() const { return address_.bufferHandle; }

public:
    TBuffAddr address_;
};

template <typename T>
class BaseGlobalTensor {
public:
    using PrimType = PrimT<T>;
    __aicore__ inline void SetAddr(const uint64_t offset)
    {
        CheckGlobalBuffer("SetAddr");
        if constexpr (IsSameType<PrimType, int4b_t>::value) {
            address_ = address_ + offset / INT4_TWO;
            oriAddress_ = oriAddress_ + offset / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
        } else if constexpr (IsSameType<PrimType, int2b_t>::value) {
            address_ = address_ + offset / INT2_FOUR;
            oriAddress_ = oriAddress_ + offset / INT2_FOUR;
        } else if constexpr (IsSameType<PrimType, uint1b_t>::value) {
            address_ = address_ + offset / INT1_EIGHT;
            oriAddress_ = oriAddress_ + offset / INT1_EIGHT;
#endif
        } else {
            address_ = address_ + offset;
            oriAddress_ = oriAddress_ + offset;
        }
    }

public:
#ifdef __ASCENDC_SUPER_KERNEL_DEBUG__
    __gm__ PrimType* address_ = nullptr;
    __gm__ PrimType* oriAddress_ = nullptr;
#else
    __gm__ PrimType* address_;
    __gm__ PrimType* oriAddress_;
#endif

protected:
    __aicore__ inline void CheckGlobalBuffer(__gm__ const char* apiName) const
    {
#ifdef __ASCENDC_SUPER_KERNEL_DEBUG__
        if (address_ == nullptr || oriAddress_ == nullptr) {
            __asc_aicore::printf_impl_assert(
                "[ASSERT] GlobalTensor::%s requires a non-null GM buffer. "
                "Call SetGlobalBuffer before accessing the tensor.\n",
                apiName);
            // Keep the guard effective even when diagnostic printing is disabled.
            trap();
        }
#else
        (void)apiName;
#endif
    }
};

template <typename T>
class BaseTensor {};

template <typename T>
class BaseTensorTraitTensor {};

template <typename T, TPosition pos, typename LayoutType>
class BaseTensorTraitTensor<TensorTrait<T, pos, LayoutType>> {
public:
    __aicore__ inline TensorTrait<T, pos, LayoutType>& GetTensorTrait();
    __aicore__ inline const TensorTrait<T, pos, LayoutType>& GetTensorTrait() const;
    __aicore__ inline void SetTensorTrait(const TensorTrait<T, pos, LayoutType>& newTrait);
    __aicore__ inline decltype(auto) GetLayout() const { return trait.GetLayout(); }
    __aicore__ inline decltype(auto) GetShape() const { return trait.GetShape(); }
    __aicore__ inline decltype(auto) GetStride() const { return trait.GetStride(); }

private:
    TensorTrait<T, pos, LayoutType> trait = {};
};

template <typename T, TPosition pos, typename LayoutType>
__aicore__ inline TensorTrait<T, pos, LayoutType>&
BaseTensorTraitTensor<TensorTrait<T, pos, LayoutType>>::GetTensorTrait()
{
    return this->trait;
}

template <typename T, TPosition pos, typename LayoutType>
__aicore__ inline const TensorTrait<T, pos, LayoutType>&
BaseTensorTraitTensor<TensorTrait<T, pos, LayoutType>>::GetTensorTrait() const
{
    return this->trait;
}

template <typename T, TPosition pos, typename LayoutType>
__aicore__ inline void BaseTensorTraitTensor<TensorTrait<T, pos, LayoutType>>::SetTensorTrait(
    const TensorTrait<T, pos, LayoutType>& newTrait)
{
    this->trait = newTrait;
}

} // namespace AscendC
#endif // ASCENDC_MODULE_TPIPE_BASE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_BASE_H__
#endif
