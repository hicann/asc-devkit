/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#else
#include "kernel_operator_fixpipe_intf.h"
#endif

// __aicore__ inline void SetFixPipeConfig(const LocalTensor<T> &reluPre, const LocalTensor<T> &quantPre, bool
// isUnitFlag = false);
extern "C" __global__ __aicore__ void KernelTestSetFixPipeConfig1()
{
    AscendC::LocalTensor<float> reluPre;
    AscendC::LocalTensor<float> quantPre;
    bool isUnitFlag = false;
    AscendC::SetFixPipeConfig(reluPre, quantPre, false);
}

// __aicore__ inline void SetFixPipeConfig(const LocalTensor<T> &preData, bool isUnitFlag = false);
extern "C" __global__ __aicore__ void KernelTestSetFixPipeConfig2()
{
    AscendC::LocalTensor<float> preData;
    bool isUnitFlag = false;
    AscendC::SetFixPipeConfig(preData, false);
}

// __aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride);
extern "C" __global__ __aicore__ void KernelTestSetFixpipeNz2ndFlag1()
{
    uint16_t ndNum = 1;
    uint16_t srcNdStride = 1;
    uint32_t dstNdStride = 0;
    AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride);
}

// __aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint16_t dstNdStride);
extern "C" __global__ __aicore__ void KernelTestSetFixpipeNz2ndFlag2()
{
    uint16_t ndNum = 1;
    uint16_t srcNdStride = 1;
    uint16_t dstNdStride = 1;
    AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride);
}

// __aicore__ inline void SetFixpipePreQuantFlag(uint64_t config);
extern "C" __global__ __aicore__ void KernelTestSetFixpipePreQuantFlag1()
{
    uint64_t config = 0xFFFFFFFFFFFFFFFF;
    AscendC::SetFixpipePreQuantFlag(config);
}

// __aicore__ inline void SetFixPipeClipRelu(uint64_t config);
extern "C" __global__ __aicore__ void KernelTestSetFixPipeClipRelu1()
{
    uint64_t config = 0xFFFFFFFFFFFFFFFF;
    AscendC::SetFixPipeClipRelu(config);
}

// // __aicore__ inline void SetFixPipeAddr(const LocalTensor<T> &eleWiseData, uint16_t c0ChStride);
// extern "C" __global__ __aicore__ void KernelTestSetFixPipeAddr1() {
//     AscendC::LocalTensor<half> eleWiseData;
//     uint16_t c0ChStride = 1;
//     AscendC::SetFixPipeAddr(eleWiseData, c0ChStride);
// }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || \
                              __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
// // __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220&
// intriParams); extern "C" __global__ __aicore__ void KernelTestFixpipe1() {
//     AscendC::LocalTensor<float> dst;
//     AscendC::LocalTensor<float> src;
//     AscendC::FixpipeParamsV220 intriParams;
//     AscendC::Fixpipe(dst, src, intriParams);
// }

// // __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>&
// cbufWorkspace, const FixpipeParamsV220& intriParams); extern "C" __global__ __aicore__ void KernelTestFixpipe2() {
//     AscendC::LocalTensor<float> dst;
//     AscendC::LocalTensor<float> src;
//     AscendC::LocalTensor<uint64_t> cbufWorkspace;
//     AscendC::FixpipeParamsV220 intriParams;
//     AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
// }

#if __NPU_ARCH__ != 5102 // has a bug
// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220&
// intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe3()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsV220 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>&
// cbufWorkspace, const FixpipeParamsV220& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe4()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsV220 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}
#endif // __NPU_ARCH__ != 5102
#endif

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
// __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300&
// intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsM300 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>&
// cbufWorkspace, const FixpipeParamsM300& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsM300 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300&
// intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe7()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsM300 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>&
// cbufWorkspace, const FixpipeParamsM300& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe8()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsM300 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}
#endif

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3102
// __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM310&
// intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe9()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsM310 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>&
// cbufWorkspace, const FixpipeParamsM310& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe10()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsM310 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM310&
// intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe11()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsM310 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>&
// cbufWorkspace, const FixpipeParamsM310& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe12()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsM310 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}
#endif

#if __NPU_ARCH__ == 3510
// __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const
// FixpipeParamsArch3510<config.format>& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe13()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsArch3510 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>&
// cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe14()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsArch3510 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const
// FixpipeParamsArch3510<config.format>& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe15()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::FixpipeParamsArch3510 intriParams;
    AscendC::Fixpipe(dst, src, intriParams);
}

// __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>&
// cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams);
extern "C" __global__ __aicore__ void KernelTestFixpipe16()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<uint64_t> cbufWorkspace;
    AscendC::FixpipeParamsArch3510 intriParams;
    AscendC::Fixpipe(dst, src, cbufWorkspace, intriParams);
}
#endif
