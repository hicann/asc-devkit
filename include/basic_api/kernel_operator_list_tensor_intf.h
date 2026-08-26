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
 * \file kernel_operator_list_tensor_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_list_tensor_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_LIST_TENSOR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_LIST_TENSOR_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"

#if ASCENDC_CPU_DEBUG
#include "../../impl/basic_api/kernel_check.h"
#endif

#include "../../impl/basic_api/kernel_operator_list_tensor_intf_impl.h"

namespace AscendC {
class ListTensorDesc;
template <class T>
class TensorDesc {
#if (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || \
    (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
public:
    __aicore__ inline TensorDesc() {}
    __aicore__ inline ~TensorDesc() {}
    __aicore__ inline void SetShapeAddr(uint64_t* shapePtr) { tensorDesc.SetShapeAddr(shapePtr); }

    __aicore__ inline uint64_t GetDim() { return tensorDesc.GetDim(); }
    __aicore__ inline uint64_t GetIndex() { return tensorDesc.GetIndex(); }
    __aicore__ inline uint64_t GetShape(uint32_t offset) { return tensorDesc.GetShape(offset); }
    __aicore__ inline __gm__ T* GetDataPtr() { return tensorDesc.GetDataPtr(); }

    __aicore__ inline GlobalTensor<T> GetDataObj() { return tensorDesc.GetDataObj(); }

    friend class ListTensorDesc;

private:
    TensorDescImpl<T> tensorDesc;
#endif
};

class ListTensorDesc {
public:
    __aicore__ inline ListTensorDesc() {}
    __aicore__ inline ~ListTensorDesc() {}
    __aicore__ inline ListTensorDesc(__gm__ void* data, uint32_t length = 0xffffffff, uint32_t shapeSize = 0xffffffff)
    {
        listTensorDesc.ListTensorDecode(data, length, shapeSize);
    }
    __aicore__ inline void Init(__gm__ void* data, uint32_t length = 0xffffffff, uint32_t shapeSize = 0xffffffff)
    {
        listTensorDesc.ListTensorDecode(data, length, shapeSize);
    }
#if (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || \
    (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
    template <class T>
    __aicore__ inline void GetDesc(TensorDesc<T>& desc, uint32_t index)
    {
        listTensorDesc.GetDesc(&desc.tensorDesc, index);
    }
#endif
    template <class T>
    __aicore__ inline __gm__ T* GetDataPtr(uint32_t index)
    {
        return listTensorDesc.GetDataPtr<T>(index);
    }
    __aicore__ inline uint32_t GetSize() { return listTensorDesc.GetSize(); }

private:
#ifdef __NPU_ARCH__
    ListTensorDescImpl<true> listTensorDesc;
#endif
};
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_LIST_TENSOR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_INTF_H__
#endif
