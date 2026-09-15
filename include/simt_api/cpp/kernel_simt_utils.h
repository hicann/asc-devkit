/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_simt_utils.h
 * \brief
 */
#ifndef ASCENDC_MODULE_SIMT_UTILS_H
#define ASCENDC_MODULE_SIMT_UTILS_H

namespace AscendC {
namespace Simt {

using Dim3 = cce::dim3;

template <auto funcPtr, typename... Args>
__aicore__ inline void VF_CALL(Dim3 threadNums, Args&&... args)
{
#if defined(SPLIT_CORE_VEC) || defined(ASCENDC_CPU_DEBUG)
    cce::async_invoke<funcPtr>(threadNums, args...);
#endif
}

} // namespace Simt
} // namespace AscendC
#endif // ASCENDC_MODULE_SIMT_UTILS_H
