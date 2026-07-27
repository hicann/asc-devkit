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
 * \file kernel_tensor.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_tensor.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_H__
#endif

#ifndef KERNEL_TENSOR_H
#define KERNEL_TENSOR_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/common_types.h"
#include "../../impl/basic_api/kernel_event.h"
#include "../../impl/basic_api/kernel_log.h"
#include "../../impl/basic_api/kernel_operator_tensor_trait.h"
#include "kernel_struct_data_copy.h"
#include "../../impl/basic_api/kernel_tensor_base.h"
#include "../../impl/basic_api/utils/kernel_utils_macros.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include <iosfwd>
#include <string>
#include "stub_def.h"
#endif

namespace AscendC {
/* \brief the shape info of tensor;
 * \note this struct contains the shape info of tensor;
 * info:
 * shape: the tensor's shape
 * shapeDim: the tensor's shape dim
 * originalShape: the tensor's originalShape, for example, this tensor's NZ shape is
 * {32, 32, 16, 16}, but the original shape may be {32*16, 31*16}
 * dataFormat: tensor's format, ND or NZ;
 */
struct ShapeInfo {
public:
    __aicore__ inline ShapeInfo() {}
    __aicore__ inline ShapeInfo(
        const uint8_t inputShapeDim, const uint32_t inputShape[], const uint8_t inputOriginalShapeDim,
        const uint32_t inputOriginalShape[], const DataFormat inputFormat)
        : shapeDim(inputShapeDim), originalShapeDim(inputOriginalShapeDim), dataFormat(inputFormat)
    {
        ASCENDC_ASSERT((inputShapeDim <= K_MAX_SHAPE_DIM && inputOriginalShapeDim <= K_MAX_SHAPE_DIM), {
            KERNEL_LOG(
                KERNEL_ERROR, "inputShapeDim is %d, inputOriginalShapeDim is %d, which should be less than %d both",
                inputShapeDim, inputOriginalShapeDim, K_MAX_SHAPE_DIM);
        });
        for (int index = 0; index < shapeDim; ++index) {
            shape[index] = inputShape[index];
        }
        for (int index = 0; index < originalShapeDim; ++index) {
            originalShape[index] = inputOriginalShape[index];
        }
    }
    __aicore__ inline ShapeInfo(const uint8_t inputShapeDim, const uint32_t inputShape[], const DataFormat inputFormat)
        : shapeDim(inputShapeDim), originalShapeDim(inputShapeDim), dataFormat(inputFormat)
    {
        ASCENDC_ASSERT((inputShapeDim <= K_MAX_SHAPE_DIM), {
            KERNEL_LOG(
                KERNEL_ERROR, "inputShapeDim is %u, which should be less than %d", inputShapeDim, K_MAX_SHAPE_DIM);
        });
        for (int index = 0; index < shapeDim; ++index) {
            shape[index] = inputShape[index];
            originalShape[index] = inputShape[index];
        }
    }

    __aicore__ inline ShapeInfo(const uint8_t inputShapeDim, const uint32_t inputShape[])
        : shapeDim(inputShapeDim), originalShapeDim(inputShapeDim), dataFormat(DataFormat::ND)
    {
        ASCENDC_ASSERT((inputShapeDim <= K_MAX_SHAPE_DIM), {
            KERNEL_LOG(
                KERNEL_ERROR, "inputShapeDim is %d, which should be less than %d", inputShapeDim, K_MAX_SHAPE_DIM);
        });
        for (int index = 0; index < shapeDim; ++index) {
            shape[index] = inputShape[index];
            originalShape[index] = inputShape[index];
        }
    }
    uint8_t shapeDim;
    uint8_t originalShapeDim;
    uint32_t shape[K_MAX_SHAPE_DIM];
    uint32_t originalShape[K_MAX_SHAPE_DIM];
    DataFormat dataFormat;
};

/* \brief the ShapeInfoParams of shapeInfo_;
 * \note this struct contains typename of shapeInfo_;
 * info:
 * Params: the shapeInfo_'s typename
 */
template <typename T, typename U>
struct ShapeInfoParams {
    __aicore__ ShapeInfoParams() {};
    using Params = ShapeInfo;
};
template <typename T>
struct ShapeInfoParams<TensorTrait<T>, T> {
    __aicore__ ShapeInfoParams() {};
    using Params = int8_t;
};

__aicore__ inline uint64_t GetShapeSize(const ShapeInfo& shapeInfo)
{
    uint64_t shapeSize = 1;
    for (int index = 0; index < shapeInfo.shapeDim; ++index) {
        shapeSize *= shapeInfo.shape[index];
    }
    return shapeSize;
}

template <typename T>
class SymbolOverrideAdd;
template <typename T>
class SymbolOverrideSub;
template <typename T>
class SymbolOverrideDiv;
template <typename T>
class SymbolOverrideMul;
template <typename T>
class SymbolOverrideOr;
template <typename T>
class SymbolOverrideAnd;
template <typename T>
class SymbolOverrideCompare;

template <typename T>
class LocalTensor : public BaseLocalTensor<T>, public BaseTensorTraitTensor<T> {
public:
    using PrimType = PrimT<T>;
    __aicore__ inline LocalTensor<T>(){};
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ~LocalTensor();
    explicit LocalTensor<T>(TBuffAddr& address);
    LocalTensor<T>(const LocalTensor<T>& other);
    LocalTensor<T> operator=(const LocalTensor<T>& other);

    PrimType* GetPhyAddr(const uint32_t offset) const;
    PrimType* GetPhyAddr() const;
    __inout_pipe__(S) PrimType GetValue(const uint32_t offset) const;
    __inout_pipe__(S) PrimType& operator()(const uint32_t offset) const;

    template <typename U>
    __aicore__ inline LocalTensor<U> ReinterpretCast() const;
    template <typename S>
    __inout_pipe__(S) void SetValue(const uint32_t index, const S value) const;
    LocalTensor operator[](const uint32_t offset) const;

    template <typename S>
    void SetAddrWithOffset(LocalTensor<S>& src, uint32_t offset);
    inline void Print();
    inline void Print(uint32_t len);
    int32_t ToFile(const std::string& fileName) const;
#else
    __aicore__ inline uint64_t GetPhyAddr() const;
    __aicore__ inline uint64_t GetPhyAddr(const uint32_t offset) const;
    __aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint32_t index) const;
    __aicore__ inline __inout_pipe__(S) __ubuf__ PrimType& operator()(const uint32_t offset) const;
    template <typename U>
    __aicore__ inline LocalTensor<U> ReinterpretCast() const;
    template <typename S>
    __aicore__ inline __inout_pipe__(S) void SetValue(const uint32_t index, const S value) const;
    __aicore__ inline LocalTensor operator[](const uint32_t offset) const;

    template <typename S>
    [[deprecated("NOTICE: SetAddrWithOffset has been deprecated and will be removed in the next version. "
                 "Please do not use it!")]] __aicore__ inline void
    SetAddrWithOffset(LocalTensor<S>& src, uint32_t offset);
#endif
    __aicore__ inline LocalTensor<T>(TPosition pos, uint32_t addr, uint32_t tileSize);
    template <typename U>
    __aicore__ inline LocalTensor<T>(uint32_t addr, const U& layout);
    __aicore__ inline LocalTensor<T>(uint32_t addr);
    __aicore__ inline int32_t GetPosition() const;
    __aicore__ inline void SetSize(const uint32_t size);
    __aicore__ inline uint32_t GetSize() const;

    [[deprecated("NOTICE: GetLength has been deprecated and will be removed in the next version. Please do not use "
                 "it!")]] __aicore__ inline uint32_t
    GetLength() const;

    __aicore__ inline void SetBufferLen(uint32_t dataLen);
    __aicore__ inline void SetUserTag(const TTagType tag);
    __aicore__ inline TTagType GetUserTag() const;
    // symbol override
    __aicore__ inline void operator=(const SymbolOverrideAdd<T>& symbolOverride);
    __aicore__ inline void operator=(const SymbolOverrideSub<T>& symbolOverride);
    __aicore__ inline void operator=(const SymbolOverrideMul<T>& symbolOverride);
    __aicore__ inline void operator=(const SymbolOverrideDiv<T>& symbolOverride);
    __aicore__ inline void operator=(const SymbolOverrideOr<T>& symbolOverride);
    __aicore__ inline void operator=(const SymbolOverrideAnd<T>& symbolOverride);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    template <typename U>
    __aicore__ inline void operator=(const SymbolOverrideCompare<U>& symbolOverride);
#else
    __aicore__ inline void operator=(const SymbolOverrideCompare<float>& symbolOverride);
    __aicore__ inline void operator=(const SymbolOverrideCompare<half>& symbolOverride);
#endif
    __aicore__ inline SymbolOverrideAdd<T> operator+(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideSub<T> operator-(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideMul<T> operator*(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideDiv<T> operator/(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideOr<T> operator|(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideAnd<T> operator&(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideCompare<T> operator<(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideCompare<T> operator>(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideCompare<T> operator!=(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideCompare<T> operator==(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideCompare<T> operator<=(const LocalTensor<T>& src1) const;
    __aicore__ inline SymbolOverrideCompare<T> operator>=(const LocalTensor<T>& src1) const;
    __aicore__ inline void SetShapeInfo(const ShapeInfo& shapeInfo);
    __aicore__ inline ShapeInfo GetShapeInfo() const;

public:
#ifndef SPLIT_CORE_CUBE
    typename ShapeInfoParams<T, PrimType>::Params shapeInfo_;
#endif
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    std::ostringstream os_;
#endif

private:
    template <typename S>
    __aicore__ inline void CreateTensor(TPosition pos, uint32_t addr, uint32_t tileSize);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    inline void PrintTypicalFloat(uint32_t len, uint32_t dataSize);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    inline void PrintFp4E2M1(uint32_t len) const;
    inline void PrintFp4E1M2(uint32_t len) const;
#endif
#endif
};

template <typename T>
class GlobalTensor : public BaseGlobalTensor<T>, public BaseTensorTraitTensor<T> {
public:
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__)
    __aicore__ inline GlobalTensor<T>();
#else
    __aicore__ inline GlobalTensor<T>() {}
#endif
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    __aicore__ inline ~GlobalTensor<T>();
#endif
    __aicore__ inline void SetGlobalBuffer(__gm__ PrimType* buffer, uint64_t bufferSize);
    __aicore__ inline void SetGlobalBuffer(__gm__ PrimType* buffer);
    __aicore__ inline const __gm__ PrimType* GetPhyAddr() const;
    __aicore__ inline __gm__ PrimType* GetPhyAddr(const uint64_t offset) const;

    __aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint64_t offset) const;
    __aicore__ inline __inout_pipe__(S) __gm__ PrimType& operator()(const uint64_t offset) const;
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    __aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint64_t offset);
    __aicore__ inline __inout_pipe__(S) __gm__ PrimType& operator()(const uint64_t offset);
#endif
    __aicore__ inline void SetValue(const uint64_t offset, PrimType value);

    __aicore__ inline uint64_t GetSize() const;
    __aicore__ inline GlobalTensor operator[](const uint64_t offset) const;
    __aicore__ inline void SetShapeInfo(const ShapeInfo& shapeInfo);
    __aicore__ inline ShapeInfo GetShapeInfo() const;
    template <CacheRwMode rwMode = CacheRwMode::RW>
    __aicore__ inline void SetL2CacheHint(CacheMode mode);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    template <typename U>
    __aicore__ inline GlobalTensor<U> ReinterpretCast() const;
#endif

public:
    // element number of Tensor
    uint64_t bufferSize_;
#ifndef SPLIT_CORE_CUBE
    typename ShapeInfoParams<T, PrimType>::Params shapeInfo_;
#endif
    CacheMode cacheMode_ = CacheMode::CACHE_MODE_NORMAL;
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    uintptr_t lastWriteCacheAddr;
    uintptr_t lastReadCacheAddr;
#endif
private:
#ifdef __ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__
    template <typename U>
    __aicore__ inline uintptr_t AlignPtr(__gm__ U* buffer) const;
    __aicore__ inline void DcciCacheLine(uintptr_t cacheAddr) const;
    template <typename U>
    __aicore__ inline void DcciReadCache(__gm__ U* buffer);
    template <typename U>
    __aicore__ inline void DcciWriteCache(__gm__ U* buffer);
#endif
};

template <Hardware hard = Hardware::UB>
class LocalMemAllocator {
public:
    __aicore__ inline LocalMemAllocator();
    __aicore__ inline uint32_t GetCurAddr() const;
    template <TPosition pos, class DataType, uint32_t tileSize>
    __aicore__ inline LocalTensor<DataType> Alloc();
    template <TPosition pos, class DataType>
    LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize);
    template <class DataType, uint32_t tileSize>
    LocalTensor<DataType> __aicore__ inline Alloc();
    template <class DataType>
    LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize);
    template <class DataType>
    LocalTensor<DataType> __aicore__ inline Alloc();
    template <class DataType, typename LayoutType>
    typename Std::enable_if<is_layout_v<LayoutType>, LocalTensor<DataType>>::type
        __aicore__ inline Alloc(const LayoutType& layout);

private:
    uint32_t head_ = 0;
};
} // namespace AscendC

#include "../../impl/basic_api/kernel_tensor_impl.h"
#endif // KERNEL_TENSOR_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TENSOR_H__
#endif
