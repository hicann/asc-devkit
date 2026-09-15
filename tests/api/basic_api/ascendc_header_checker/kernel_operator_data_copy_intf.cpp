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
#include "kernel_operator_data_copy_intf.h"
#endif

// __aicore__ inline __inout_pipe__(MTE2) void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// Nd2NzParams& intriParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::Nd2NzParams intriParams;
    AscendC::DataCopy(dst, src, intriParams);
}

// __aicore__ inline __inout_pipe__(MTE2) void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// Nd2NzParams& intriParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::Nd2NzParams intriParams;
    AscendC::DataCopy(dst, src, intriParams);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002)
// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams&
// intriParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::Nd2NzParams intriParams;
    AscendC::DataCopy(dst, src, intriParams);
}
#endif

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline __inout_pipe__(MTE2) void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// Dn2NzParams& intriParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::Dn2NzParams intriParams;
    AscendC::DataCopy(dst, src, intriParams);
}
#endif

// __aicore__ inline __inout_pipe__(MTE3) void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// DataCopyParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy5()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams repeatParams;
    AscendC::DataCopy(dst, src, repeatParams);
}

// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams&
// repeatParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams repeatParams;
    AscendC::DataCopy(dst, src, repeatParams);
}

// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams&
// repeatParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy7()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams repeatParams;
    AscendC::DataCopy(dst, src, repeatParams);
}

#if __NPU_ARCH__ != 1001 && __NPU_ARCH__ != 2002
// __aicore__ inline __inout_pipe__(V) void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t
// mask[], const uint8_t repeatTime, const CopyRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestCopy1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::CopyRepeatParams repeatParams;
    AscendC::Copy(dst, src, mask, repeatTime, repeatParams);
}
#endif

// __aicore__ inline __inout_pipe__(V) void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t
// mask, const uint8_t repeatTime, const CopyRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestCopy2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::CopyRepeatParams repeatParams;
    AscendC::Copy(dst, src, mask, repeatTime, repeatParams);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline __inout_pipe__(V) void Copy(const LocalTensor<T> &dst, const LocalTensor<T> &src, const uint32_t
// count);
extern "C" __global__ __aicore__ void KernelTestCopy3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint32_t count = 0;
    AscendC::Copy(dst, src, count);
}
#endif

#if __NPU_ARCH__ != 3102
// __aicore__ inline __inout_pipe__(MTE2) void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1);
extern "C" __global__ __aicore__ void KernelTestDataCopy8()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::SliceInfo dstSliceInfo[2] = {{16, 70, 7, 3, 87}, {0, 2, 1, 1, 3}};
    AscendC::SliceInfo srcSliceInfo[2] = {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}};
    uint32_t dimValue = 1;
    AscendC::DataCopy(dst, src, dstSliceInfo, srcSliceInfo, 1);
}

// __aicore__ inline __inout_pipe__(MTE3) void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1);
extern "C" __global__ __aicore__ void KernelTestDataCopy9()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::SliceInfo dstSliceInfo[2] = {{16, 70, 7, 3, 87}, {0, 2, 1, 1, 3}};
    AscendC::SliceInfo srcSliceInfo[2] = {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}};
    uint32_t dimValue = 1;
    AscendC::DataCopy(dst, src, dstSliceInfo, srcSliceInfo, 1);
}
#endif

// __aicore__ inline __inout_pipe__(MTE2) void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// uint32_t count);
extern "C" __global__ __aicore__ void KernelTestDataCopy10()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    uint32_t count = 0;
    AscendC::DataCopy(dst, src, count);
}

// __aicore__ inline __inout_pipe__(MTE3) void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// uint32_t count);
extern "C" __global__ __aicore__ void KernelTestDataCopy11()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint32_t count = 0;
    AscendC::DataCopy(dst, src, count);
}

// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestDataCopy12()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint32_t count = 0;
    AscendC::DataCopy(dst, src, count);
}

#if __NPU_ARCH__ != 3102
// __aicore__ inline __inout_pipe__(MTE3) void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// Nz2NdParamsFull& intriParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy13()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::Nz2NdParamsFull intriParams;
    AscendC::DataCopy(dst, src, intriParams);
}
#endif

// __aicore__ inline __inout_pipe__(MTE2) void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy14()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline __inout_pipe__(MTE3) void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy15()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams&
// intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy16()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// // __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams&
// intriParams); extern "C" __global__ __aicore__ void KernelTestDataCopy17() {
//     AscendC::LocalTensor<float> dst;
//     AscendC::LocalTensor<float> src;
//     AscendC::DataCopyCO12DstParams intriParams;
//     AscendC::DataCopy(dst, src, intriParams);
// }

// // __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const
// DataCopyCO12DstParams& intriParams); extern "C" __global__ __aicore__ void KernelTestDataCopy18() {
//     AscendC::GlobalTensor<float> dst;
//     AscendC::LocalTensor<float> src;
//     AscendC::DataCopyCO12DstParams intriParams;
//     AscendC::DataCopy(dst, src, intriParams);
// }

// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams&
// intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy19()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams&
// intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy20()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline __inout_pipe__(V) void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy21()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline __inout_pipe__(V) void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy22()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline __inout_pipe__(V) void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy23()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline __inout_pipe__(V) void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy24()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

// __aicore__ inline __inout_pipe__(V) void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const
// DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams);
extern "C" __global__ __aicore__ void KernelTestDataCopy25()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams intriParams;
    AscendC::DataCopyEnhancedParams enhancedParams;
    AscendC::DataCopy(dst, src, intriParams, enhancedParams);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002)
// __aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// DataCopyParams& dataCopyParams, const DataCopyPadParams& padParams);
extern "C" __global__ __aicore__ void KernelTestDataCopyPad1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::DataCopyParams dataCopyParams;
    AscendC::DataCopyPadParams padParams;
    AscendC::DataCopyPad(dst, src, dataCopyParams, padParams);
}

// __aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// DataCopyParams& dataCopyParams);
extern "C" __global__ __aicore__ void KernelTestDataCopyPad2()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyParams dataCopyParams;
    AscendC::DataCopyPad(dst, src, dataCopyParams);
}

// // __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams&
// dataCopyParams, const Nd2NzParams& nd2nzParams); extern "C" __global__ __aicore__ void KernelTestDataCopyPad5() {
//     AscendC::LocalTensor<float> dst;
//     AscendC::LocalTensor<float> src;
//     AscendC::DataCopyParams dataCopyParams;
//     AscendC::Nd2NzParams nd2nzParams;
//     AscendC::DataCopyPad(dst, src, dataCopyParams, nd2nzParams);
// }

// __aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const
// DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams);
extern "C" __global__ __aicore__ void KernelTestDataCopyPad6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::DataCopyExtParams dataCopyParams;
    AscendC::DataCopyPadExtParams<float> padParams;
    AscendC::DataCopyPad(dst, src, dataCopyParams, padParams);
}

// __aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const
// DataCopyExtParams& dataCopyParams);
extern "C" __global__ __aicore__ void KernelTestDataCopyPad9()
{
    AscendC::GlobalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyExtParams dataCopyParams;
    AscendC::DataCopyPad(dst, src, dataCopyParams);
}
#endif

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 2201)
// __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams&
// dataCopyParams, const Nd2NzParams& nd2nzParams);
extern "C" __global__ __aicore__ void KernelTestDataCopyPad11()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::DataCopyExtParams dataCopyParams;
    AscendC::Nd2NzParams nd2nzParams;
    AscendC::DataCopyPad(dst, src, dataCopyParams, nd2nzParams);
}
#endif

// __aicore__ inline void SetPadValue(T paddingValue);
extern "C" __global__ __aicore__ void KernelTestSetPadValue1()
{
    float paddingValue = 0.0f;
    AscendC::SetPadValue(paddingValue);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline void DataCopy(const LocalTensor<T> &dst, const GlobalTensor<T> &src, const MultiCopyParams<T, dim>
// &params);
extern "C" __global__ __aicore__ void KernelTestDataCopy26()
{
    AscendC::LocalTensor<float> dst;
    AscendC::GlobalTensor<float> src;
    AscendC::MultiCopyParams<float, 1> params;
    AscendC::DataCopy(dst, src, params);
}

// __aicore__ inline void NdDmaDci();
extern "C" __global__ __aicore__ void KernelTestNdDmaDci1() { AscendC::NdDmaDci(); }

// __aicore__ inline void SetLoopModePara(const LoopModeParams& loopParams, DataCopyMVType type);
extern "C" __global__ __aicore__ void KernelTestSetLoopModePara1()
{
    AscendC::LoopModeParams loopParams;
    AscendC::DataCopyMVType type;
    AscendC::SetLoopModePara(loopParams, type);
}

// __aicore__ inline void ResetLoopModePara(DataCopyMVType type);
extern "C" __global__ __aicore__ void KernelTestResetLoopModePara1()
{
    AscendC::DataCopyMVType type;
    AscendC::ResetLoopModePara(type);
}
#endif
