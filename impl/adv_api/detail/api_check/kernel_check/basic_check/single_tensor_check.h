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
 * \file single_tensor_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/basic_check/single_tensor_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SINGLE_TENSOR_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_SINGLE_TENSOR_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_SINGLE_TENSOR_CHECK_H_

#include "basic_check_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

class SingleTensorCheckFuncBasicClass {
public:
    __aicore__ inline SingleTensorCheckFuncBasicClass(){};
    __aicore__ inline SingleTensorCheckFuncBasicClass(__gm__ const char* apiName) { this->apiName = apiName; };

public:
    template <typename T, typename U, typename W>
    __aicore__ inline void TensorVerifyingParameters(
        T tensorTuple, U tensorStringTuple, W posTuple, __gm__ const char* posString)
    {
        TPositionVerifyingParameters<T, U, W>(tensorTuple, tensorStringTuple, posTuple, posString);
        TensorSizeVerifyingParameters<T, U>(tensorTuple, tensorStringTuple);
        TensorPhyAddrVerifyingParameters<T, U>(tensorTuple, tensorStringTuple);
    }

    template <typename T, typename U, typename W>
    __aicore__ inline void TPositionVerifyingParameters(
        T tensorTuple, U tensorStringTuple, W posTuple, __gm__ const char* posString)
    {
        static_assert(
            (Std::is_tuple_v<T> && Std::is_tuple_v<U> && Std::is_tuple_v<W>),
            "Input template T or U or W is not tuple!");
        static_assert(
            (Std::tuple_size_v<decltype(tensorTuple)> == Std::tuple_size_v<decltype(tensorStringTuple)>),
            "TPositionVerifyingParameters func, tensorTuple.Size != tensorStringTuple.Size !");

        TPositionAllTensorLoopCheck<T, U, W>(tensorTuple, tensorStringTuple, posTuple, posString, tuple_sequence<T>{});
    }

    template <typename T, typename U>
    __aicore__ inline void TensorSizeVerifyingParameters(T tensorTuple, U tensorStringTuple)
    {
        static_assert((Std::is_tuple_v<T> && Std::is_tuple_v<U>), "Input template T or U is not tuple!");
        static_assert(
            (Std::tuple_size_v<decltype(tensorTuple)> == Std::tuple_size_v<decltype(tensorStringTuple)>),
            "TensorSizeVerifyingParameters func, tensorTuple.Size != tensorStringTuple.Size !");

        TensorSizeAllTensorLoopCheck<T, U>(tensorTuple, tensorStringTuple, tuple_sequence<T>{});
    }

    template <typename T, typename U>
    __aicore__ inline void TensorPhyAddrVerifyingParameters(T tensorTuple, U tensorStringTuple)
    {
        static_assert((Std::is_tuple_v<T> && Std::is_tuple_v<U>), "Input template T or U is not tuple!");
        static_assert(
            (Std::tuple_size_v<decltype(tensorTuple)> == Std::tuple_size_v<decltype(tensorStringTuple)>),
            "TensorPhyAddrVerifyingParameters func, tensorTuple.Size != tensorStringTuple.Size !");

        TensorPhyAddrAllTensorLoopCheck<T, U>(tensorTuple, tensorStringTuple, tuple_sequence<T>{});
    }

private:
    template <typename T, typename U, typename W, size_t... Is>
    __aicore__ inline void TPositionAllTensorLoopCheck(
        T tensorTuple, U tensorStringTuple, W posTuple, __gm__ const char* posString, Std::index_sequence<Is...>)
    {
        (TPositionOneTensorCheck(Std::get<Is>(tensorTuple), Std::get<Is>(tensorStringTuple), posTuple, posString), ...);
    }

    template <typename T, typename U>
    __aicore__ inline void TPositionOneTensorCheck(
        const LocalTensor<T>& checkTensor, __gm__ const char* tensorName, U posTuple, __gm__ const char* posString)
    {
        static_assert((Std::is_tuple_v<U>), "Input template U is not tuple!");
        TPositionOneTensorLoopCheck<T, U>(checkTensor, tensorName, posTuple, posString, tuple_sequence<U>{});
        if constexpr (HighLevelAPIParametersPrint) {
            PrintTensorTPosition(checkTensor, tensorName);
        }
    }

    template <typename T, typename U, size_t... Is>
    __aicore__ inline void TPositionOneTensorLoopCheck(
        const LocalTensor<T>& checkTensor, __gm__ const char* tensorName, U posTuple, __gm__ const char* posString,
        Std::index_sequence<Is...>)
    {
        TPosition pos = (TPosition)checkTensor.GetPosition();
        bool status = ((pos == Std::get<Is>(posTuple)) || ...);
        ASCENDC_ASSERT((status || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] Failed to check tensor position of %s, current api support positions are %s, "
                "current position is %s.",
                apiName, tensorName, posString, GetPositionDisplay(pos).c_str());
        });
    }

    template <typename T>
    __aicore__ inline void PrintTensorTPosition(const LocalTensor<T>& checkTensor, __gm__ const char* tensorInfo)
    {
        TPosition pos = (TPosition)checkTensor.GetPosition();
        KERNEL_LOG(
            KERNEL_INFO, "[%s] The tensor position of %s is %s.", apiName, tensorInfo, GetPositionDisplay(pos).c_str());
    }

    template <typename T, typename U, size_t... Is>
    __aicore__ inline void TensorSizeAllTensorLoopCheck(T tensorTuple, U tensorStringTuple, Std::index_sequence<Is...>)
    {
        (TensorSizeOneTensorCheck(Std::get<Is>(tensorTuple), Std::get<Is>(tensorStringTuple)), ...);
    }

    template <typename T>
    __aicore__ inline void TensorSizeOneTensorCheck(const LocalTensor<T>& checkTensor, __gm__ const char* tensorInfo)
    {
        uint32_t size = checkTensor.GetSize();
        uint32_t ubSize = static_cast<uint32_t>(check::GetHardWarebufferSize(static_cast<uint8_t>(HardWareIndex::UB)));
        ubSize = ubSize / sizeof(T);

        ASCENDC_ASSERT(((size > 0 && size < ubSize) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] Failed to check tensor size of %s, current tensor size is %u, should be greater than 0 and "
                "less than the hardware memory size.",
                apiName, tensorInfo, size);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            PrintTensorSize(checkTensor, tensorInfo);
        }
    }

    template <typename T>
    __aicore__ inline void PrintTensorSize(const LocalTensor<T>& checkTensor, __gm__ const char* tensorInfo)
    {
        KERNEL_LOG(KERNEL_INFO, "[%s] The size of %s is %u.", apiName, tensorInfo, checkTensor.GetSize());
    }

    template <typename T, typename U, size_t... Is>
    __aicore__ inline void TensorPhyAddrAllTensorLoopCheck(
        T tensorTuple, U tensorStringTuple, Std::index_sequence<Is...>)
    {
        (TensorPhyAddrOneTensorCheck(Std::get<Is>(tensorTuple), Std::get<Is>(tensorStringTuple)), ...);
    }

    template <typename T>
    __aicore__ inline void TensorPhyAddrOneTensorCheck(const LocalTensor<T>& checkTensor, __gm__ const char* tensorInfo)
    {
        uint64_t phyAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(checkTensor.GetPhyAddr()));
        uint64_t baseAddr = GetBaseAddr(static_cast<int8_t>(checkTensor.GetPosition()));
        uint64_t tensorAddr = phyAddr - baseAddr;

        ASCENDC_ASSERT((tensorAddr % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] Failed to check %s physical address, "
                "address must be 32-byte aligned, current physical address is %lu.",
                apiName, tensorInfo, tensorAddr);
        });

        if constexpr (HighLevelAPIParametersPrint) {
            PrintTensorPhyAddr(checkTensor, tensorInfo);
        }
    }

    template <typename T>
    __aicore__ inline void PrintTensorPhyAddr(const LocalTensor<T>& checkTensor, __gm__ const char* tensorInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[%s] The physical address of %s is %u.", apiName, tensorInfo, checkTensor.GetPhyAddr());
    }

    __aicore__ inline uint64_t GetBaseAddr(int8_t logicPos)
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(GetBaseAddrCpu(logicPos)));
#else
        return static_cast<uint64_t>(0);
#endif
    }

private:
    __gm__ const char* apiName = nullptr;
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_SINGLE_TENSOR_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SINGLE_TENSOR_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SINGLE_TENSOR_CHECK_H__
#endif
