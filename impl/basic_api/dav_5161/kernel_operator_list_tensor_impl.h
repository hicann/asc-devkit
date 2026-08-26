/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_list_tensor_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5161/kernel_operator_list_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_LIST_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_LIST_TENSOR_IMPL_H
#include "../../../include/basic_api/kernel_basic_intf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif // ASCENDC_CPU_DEBUG

namespace AscendC {
constexpr int64_t INIT_PRELOAD_OFFSET = -256;
constexpr int64_t PRELOAD_OFFSET = 256;
template <class T>
struct TensorDescImpl {
public:
    __aicore__ inline TensorDescImpl();
    __aicore__ inline void SetDim(uint32_t dim);
    __aicore__ inline void SetIndex(uint32_t offset);
    __aicore__ inline void SetShapeAddr(uint64_t* shapePtr);
    __aicore__ inline void SetDataPtr(__gm__ T* dataPtr);

    __aicore__ inline uint64_t GetDim();
    __aicore__ inline uint64_t GetIndex();
    __aicore__ inline uint64_t* GetShapeAddr();
    __aicore__ inline uint64_t GetShape(uint32_t offset);
    __aicore__ inline __gm__ T* GetDataPtr();
    __aicore__ inline GlobalTensor<T> GetDataObj();

private:
    uint32_t dim_;
    uint32_t offset_;
    uint64_t* shape_;
    __gm__ T* ptr_;
};

template <bool Sequential = true>
class ListTensorDescImpl {
public:
    __aicore__ inline ListTensorDescImpl();
    __aicore__ inline void ListTensorDecode(__gm__ void* data, uint32_t length, uint32_t shapeSize);
    template <class T>
    __aicore__ inline void GetDesc(TensorDescImpl<T>* desc, uint32_t index);
    template <class T>
    __aicore__ inline __gm__ T* GetDataPtr(uint32_t index);
    __aicore__ inline uint32_t GetSize();

private:
    __aicore__ inline void GetShape(uint64_t* shapeInfo, __gm__ uint64_t* target);

    __gm__ uint64_t* dataPtr_;
    __gm__ uint64_t* shapeStart_;
    uint32_t dim_;
    uint32_t descStructSize_;
    uint32_t count_;
    int64_t preloadOffset_;
};

template <class T>
__aicore__ inline TensorDescImpl<T>::TensorDescImpl()
{
    dim_ = 0;
    offset_ = 0;
    shape_ = nullptr;
    ptr_ = nullptr;
}

template <class T>
__aicore__ inline void TensorDescImpl<T>::SetDim(uint32_t dim)
{
    this->dim_ = dim;
}
template <class T>
__aicore__ inline uint64_t TensorDescImpl<T>::GetDim()
{
    return dim_;
}

template <class T>
__aicore__ inline void TensorDescImpl<T>::SetIndex(uint32_t offset)
{
    this->offset_ = offset;
}
template <class T>
__aicore__ inline uint64_t TensorDescImpl<T>::GetIndex()
{
    return offset_;
}

template <class T>
__aicore__ inline void TensorDescImpl<T>::SetShapeAddr(uint64_t* shapePtr)
{
    ASSERT(shapePtr != nullptr);
    this->shape_ = shapePtr;
}
template <class T>
__aicore__ inline uint64_t* TensorDescImpl<T>::GetShapeAddr()
{
    return shape_;
}
template <class T>
__aicore__ inline uint64_t TensorDescImpl<T>::GetShape(uint32_t offset)
{
    ASSERT(offset < dim_);
    return shape_[offset];
}

template <class T>
__aicore__ inline void TensorDescImpl<T>::SetDataPtr(__gm__ T* dataPtr)
{
    ASSERT(dataPtr != nullptr);
    this->ptr_ = dataPtr;
}
template <class T>
__aicore__ inline __gm__ T* TensorDescImpl<T>::GetDataPtr()
{
    return ptr_;
}

template <class T>
__aicore__ inline GlobalTensor<T> TensorDescImpl<T>::GetDataObj()
{
    GlobalTensor<T> obj;
    uint32_t size = sizeof(T);
    for (uint32_t i = 0; i < dim_; i++) {
        size *= shape_[i];
    }
    obj.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(ptr_), size);
    return obj;
}

template <bool Sequential>
__aicore__ inline ListTensorDescImpl<Sequential>::ListTensorDescImpl()
{
    dataPtr_ = nullptr;
    shapeStart_ = nullptr;
    dim_ = 0;
    descStructSize_ = 0;
    count_ = 0;
    preloadOffset_ = 0;
}

template <bool Sequential>
__aicore__ inline void ListTensorDescImpl<Sequential>::ListTensorDecode(
    __gm__ void* data, uint32_t length, uint32_t shapeSize)
{
    ASSERT(data != nullptr);
    __gm__ uint64_t* dataAddr = reinterpret_cast<__gm__ uint64_t*>(data);
    uint64_t dataPtrOffset = *dataAddr; // The offset between head address and data pointer address.

    shapeStart_ = dataAddr + 1; // Point to the address of first shape info address.
    dataPtr_ = dataAddr + dataPtrOffset / sizeof(uint64_t);

    dim_ = *shapeStart_ & 0xffffffff;     // 0xffffffff means get the low 32 bits.
    uint32_t offset = *shapeStart_ >> 32; // Shift right 32 means get the high 32 bits.

    // dim_ = 0 means the ListTensor only has data pointer inside, then shape info should be filled in 0xffffffff
    // which means descStructSize_ is 2.
    descStructSize_ = (dim_ == 0) ? 2 : 1 + dim_;
    // Shift right 32 means get the high 32bits.
    count_ = (offset != 0) ? offset : (*(dataPtr_ - descStructSize_) >> 32) + 1;
    // If the ListTensor only has data pointer inside, the offset between head address and data pointer address
    // should be a constant value, which is (2 + 1) * sizeof(uint64_t) = 24.
    ASSERT(
        ((dim_ != 0) && (count_ == ((dataPtrOffset - sizeof(uint64_t)) / (descStructSize_ * sizeof(uint64_t))))) ||
        ((dim_ == 0) && (*(shapeStart_ + 1) == 0xffffffff) && (dataPtrOffset == 24)));

    if ((length != 0xffffffff) && (shapeSize != 0xffffffff)) {
        ASSERT(count_ == shapeSize);
        ASSERT(length == (dataPtrOffset + count_ * sizeof(uint64_t)));
    }
    preloadOffset_ = INIT_PRELOAD_OFFSET;
}

template <bool Sequential>
template <class T>
__aicore__ inline void ListTensorDescImpl<Sequential>::GetDesc(TensorDescImpl<T>* desc, uint32_t index)
{
    ASSERT((index < count_) && (desc != nullptr) && (dim_ != 0));
    desc->SetDim(dim_);
    desc->SetDataPtr(reinterpret_cast<__gm__ T*>(*(dataPtr_ + index)));
    desc->SetIndex(index);
    int64_t curloadOffset = index * descStructSize_ * sizeof(uint64_t);
    // check the whole struct has been loaded in cache
    if (curloadOffset + descStructSize_ * sizeof(uint64_t) > preloadOffset_ + PRELOAD_OFFSET) {
#ifndef ASCENDC_CPU_DEBUG
        dc_preload(shapeStart_, curloadOffset); // Preload 256Bytes from the cache miss offset.
#endif
        preloadOffset_ = curloadOffset;
    }
    __gm__ uint64_t* curShapePtr = shapeStart_ + index * descStructSize_;
    GetShape(desc->GetShapeAddr(), curShapePtr);
}

template <bool Sequential>
template <class T>
__aicore__ inline __gm__ T* ListTensorDescImpl<Sequential>::GetDataPtr(uint32_t index)
{
    ASSERT(index < count_);
    return reinterpret_cast<__gm__ T*>(*(dataPtr_ + index));
}

template <bool Sequential>
__aicore__ inline void ListTensorDescImpl<Sequential>::GetShape(uint64_t* shapeInfo, __gm__ uint64_t* target)
{
    auto targetShape = reinterpret_cast<__gm__ uint64_t*>(target + 1);
    for (uint32_t i = 0; i < dim_; i++) {
        *(shapeInfo++) = *(targetShape++);
    }
}

template <bool Sequential>
__aicore__ inline uint32_t ListTensorDescImpl<Sequential>::GetSize()
{
    return count_;
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_LIST_TENSOR_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_IMPL_H__
#endif
