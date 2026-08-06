/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef ASCENDC_MODULE_SIMT_PRINT_INTERFACE_H
#define ASCENDC_MODULE_SIMT_PRINT_INTERFACE_H

#include "simt_api/device_types.h"
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#include "impl/utils/debug/asc_printf_simt_impl.h"
#endif

namespace AscendC {
namespace Simt {

#ifndef ASCENDC_CPU_DEBUG
#if !defined(__CHECK_FEATURE_AT_PRECOMPILE) || (defined(__CHECK_FEATURE_AT_PRECOMPILE) && defined(__NPU_DEVICE__))
template <class... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void PRINTF(const __gm__ char* fmt, Args&&... args);

template <class... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void printf(const __gm__ char* fmt, Args&&... args);

#endif
#endif

}  // namespace Simt
}  // namespace AscendC

#include "impl/simt_api/cpp/kernel_simt_print_intf_impl.h"
#endif  // ASCENDC_MODULE_SIMT_PRINT_INTERFACE_H
