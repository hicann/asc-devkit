/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if __NPU_ARCH__ == 3510 && !defined(ASCENDC_CPU_DEBUG)
#include "impl/utils/debug/asc_aicore_dump_impl.h"

using AscendC::Hardware;

__aicore__ inline void CheckSuperTensorDumpInstantiation(__gm__ __asc_aicore::DebugBlockHeadInfo* blockInfo)
{
    __asc_aicore::asc_dump_super_tensor_impl<Hardware::GM, uint32_t>(
        static_cast<__gm__ uint32_t*>(nullptr), 0, 64, nullptr, 0, blockInfo);
#if defined(__DAV_VEC__)
    __asc_aicore::asc_dump_super_tensor_impl<Hardware::UB, uint32_t>(
        static_cast<__ubuf__ uint32_t*>(nullptr), 0, 64, nullptr, 0, blockInfo);
#elif defined(__DAV_CUBE__)
    __asc_aicore::asc_dump_super_tensor_impl<Hardware::L1, uint32_t>(
        static_cast<__cbuf__ uint32_t*>(nullptr), 0, 64, nullptr, 0, blockInfo);
    __asc_aicore::asc_dump_super_tensor_impl<Hardware::L0C, uint32_t>(
        static_cast<__cc__ uint32_t*>(nullptr), 0, 1024, nullptr, 0, blockInfo);
    __asc_aicore::asc_dump_super_tensor_impl<Hardware::BIAS, uint32_t>(
        static_cast<__biasbuf__ uint32_t*>(nullptr), 0, 64, nullptr, 0, blockInfo);
    __asc_aicore::asc_dump_super_tensor_impl<Hardware::FIXBUF, uint32_t>(
        static_cast<__fbuf__ uint32_t*>(nullptr), 0, 64, nullptr, 0, blockInfo);
#endif
}
#endif

#if __NPU_ARCH__ == 2002 && !defined(ASCENDC_CPU_DEBUG)
#include "kernel_operator.h"

__aicore__ inline void CheckSuperTensorDump2002Instantiation(
    const AscendC::GlobalTensor<uint32_t>& gm, const AscendC::LocalTensor<uint32_t>& local,
    __gm__ AscendC::BlockRingBufInfo* blockInfo)
{
    AscendC::DumpSuperTensorRingBufImpl(gm, 0, 8192, nullptr, 0, blockInfo);
    AscendC::DumpSuperTensorRingBufImpl(local, 0, 8192, nullptr, 0, blockInfo);
}
#endif
