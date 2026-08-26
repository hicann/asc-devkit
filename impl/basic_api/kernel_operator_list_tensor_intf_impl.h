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
 * \file kernel_operator_list_tensor_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_list_tensor_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_list_tensor_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_LIST_TENSOR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_LIST_TENSOR_INTERFACE_IMPL_H

#if __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_list_tensor_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_list_tensor_impl.h"
#elif __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_list_tensor_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_list_tensor_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_list_tensor_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_list_tensor_impl.h"
#elif __NPU_ARCH__ == 5102
#include "dav_m510/kernel_operator_list_tensor_impl.h"
#elif (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "dav_5161/kernel_operator_list_tensor_impl.h"
#endif

#endif // ASCENDC_MODULE_OPERATOR_LIST_TENSOR_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIST_TENSOR_INTF_IMPL_H__
#endif
