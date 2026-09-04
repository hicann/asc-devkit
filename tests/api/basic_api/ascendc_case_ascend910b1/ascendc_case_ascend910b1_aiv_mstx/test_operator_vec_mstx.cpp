/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstdint>
extern "C" void __mstx_dfx_report_stub(int type, uint8_t size, void* desc);
#define __MSTX_DFX_REPORT__
#include <gtest/gtest.h>
#include <string.h>

#include "kernel_log.h"
static uint8_t g_testRes = 1; // 全局变量记录运行结果, 如果进入ASCENDC_ASSERT报错，会被置为0
// 重定义ASCENDC_ASSERT，不Abort，仅修改全局变量通知进入报错分支
#undef ASCENDC_ASSERT
#define ASCENDC_ASSERT(cond, behavior) \
    do {                               \
        if (!(cond)) {                 \
            g_testRes = 0;             \
            behavior;                  \
        }                              \
    } while (0)

#undef ASCENDC_REPORT_CHECK_ERROR
#define ASCENDC_REPORT_CHECK_ERROR(apiMsg, funcType) \
    do {                                             \
        g_testRes = 0;                               \
    } while (0)

#include "kernel_utils.h"
#include "kernel_operator.h"
#include <mutex>

using namespace std;
using namespace AscendC;
using namespace AscendC::MstxTensor;

MstxTensor::MstxVecUnaryDesc g_vec_unary;
MstxTensor::MstxVecBinaryDesc g_vec_binary;
MstxTensor::MstxVecBilinearInterpolation g_vec_bilinear;
MstxTensor::MstxVecCastDeqDesc g_vec_cast_deq;
MstxTensor::MstxVecSelDesc g_vec_sel;
MstxTensor::MstxVecGatherMaskDesc g_vec_gather_mask;
MstxTensor::MstxVecTranspose g_vec_transpose;
MstxTensor::MstxVecReduceDesc g_vec_reduce;
MstxTensor::MstxVecComplexReduceDesc g_vec_cpx_reduce;
MstxTensor::MstxVecDupDesc g_vec_dup;
MstxTensor::MstxVecBrcbDesc g_vec_brcb;
MstxTensor::MstxVecCopy g_vec_copy;
MstxTensor::MstxDataCopyDesc g_data_copy;
MstxTensor::MstxDataCopyPadDesc g_data_copy_pad;
MstxTensor::MstxDataCopyPadV2Desc g_data_copy_pad_v2;

extern "C" void __mstx_dfx_report_stub(int type, uint8_t size, void* desc)
{
    switch (type) {
        case static_cast<int>(MstxReportType::MSTX_VEC_UNARY):
        case static_cast<int>(MstxReportType::MSTX_VEC_BINARY_SCALAR):
        case static_cast<int>(MstxReportType::MSTX_VEC_TENARY):
        case static_cast<int>(MstxReportType::MSTX_VEC_CAST):
        case static_cast<int>(MstxReportType::MSTX_VEC_CMPS): {
            if (desc && size == sizeof(MstxTensor::MstxVecUnaryDesc)) {
                g_vec_unary = *static_cast<MstxTensor::MstxVecUnaryDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_BINARY):
        case static_cast<int>(MstxReportType::MSTX_VEC_CMP): {
            if (desc && size == sizeof(MstxTensor::MstxVecBinaryDesc)) {
                g_vec_binary = *static_cast<MstxTensor::MstxVecBinaryDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_BILINEAR_INTERPOLATION): {
            if (desc && size == sizeof(MstxTensor::MstxVecBilinearInterpolation)) {
                g_vec_bilinear = *static_cast<MstxTensor::MstxVecBilinearInterpolation*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_CASTDEQ): {
            if (desc && size == sizeof(MstxTensor::MstxVecCastDeqDesc)) {
                g_vec_cast_deq = *static_cast<MstxTensor::MstxVecCastDeqDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_SEL): {
            if (desc && size == sizeof(MstxTensor::MstxVecSelDesc)) {
                g_vec_sel = *static_cast<MstxTensor::MstxVecSelDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_GATHER_MASK): {
            if (desc && size == sizeof(MstxTensor::MstxVecGatherMaskDesc)) {
                g_vec_gather_mask = *static_cast<MstxTensor::MstxVecGatherMaskDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_TRANSDATA):
        case static_cast<int>(MstxReportType::MSTX_VEC_TRANSPOSE): {
            if (desc && size == sizeof(MstxTensor::MstxVecTranspose)) {
                g_vec_transpose = *static_cast<MstxTensor::MstxVecTranspose*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_WHOLE_REDUCE):
        case static_cast<int>(MstxReportType::MSTX_VEC_BLK_REDUCE):
        case static_cast<int>(MstxReportType::MSTX_VEC_PAIR_REDUCE):
        case static_cast<int>(MstxReportType::MSTX_VEC_REPEAT_REDUCE): {
            if (desc && size == sizeof(MstxTensor::MstxVecReduceDesc)) {
                g_vec_reduce = *static_cast<MstxTensor::MstxVecReduceDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_REDUCE): {
            if (desc && size == sizeof(MstxTensor::MstxVecComplexReduceDesc)) {
                g_vec_cpx_reduce = *static_cast<MstxTensor::MstxVecComplexReduceDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_DUP):
        case static_cast<int>(MstxReportType::MSTX_VEC_VCI): {
            if (desc && size == sizeof(MstxTensor::MstxVecDupDesc)) {
                g_vec_dup = *static_cast<MstxTensor::MstxVecDupDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_BROADCAST): {
            if (desc && size == sizeof(MstxTensor::MstxVecBrcbDesc)) {
                g_vec_brcb = *static_cast<MstxTensor::MstxVecBrcbDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_VEC_COPY): {
            if (desc && size == sizeof(MstxTensor::MstxVecCopy)) {
                g_vec_copy = *static_cast<MstxTensor::MstxVecCopy*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_DATA_COPY): {
            if (desc && size == sizeof(MstxTensor::MstxDataCopyDesc)) {
                g_data_copy = *static_cast<MstxTensor::MstxDataCopyDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_DATA_COPY_PAD): {
            if (desc && size == sizeof(MstxTensor::MstxDataCopyPadDesc)) {
                g_data_copy_pad = *static_cast<MstxTensor::MstxDataCopyPadDesc*>(desc);
            }
            break;
        }

        case static_cast<int>(MstxReportType::MSTX_DATA_COPY_PAD_V2): {
            if (desc && size == sizeof(MstxTensor::MstxDataCopyPadV2Desc)) {
                g_data_copy_pad_v2 = *static_cast<MstxTensor::MstxDataCopyPadV2Desc*>(desc);
            }
            break;
        }

        default:
            break;
    }
}

void CopyNameUt(char b[64], const char* a)
{
    uint32_t i = 0;
    for (; i < 63; ++i) {
        b[i] = a[i];
        if (a[i] == '\0') {
            break;
        }
    }
    b[i] = '\0';
}

template <typename T>
MstxTensor::MstxTensorAddressSpace GetTensorSpace(const LocalTensor<T>& dst)
{
    Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstHWPos == Hardware::GM) {
        return AscendC::MstxTensor::MSTX_TENSOR_AT_GM;
    } else if (dstHWPos == Hardware::UB) {
        return AscendC::MstxTensor::MSTX_TENSOR_AT_UB;
    } else if (dstHWPos == Hardware::L1) {
        return AscendC::MstxTensor::MSTX_TENSOR_AT_L1;
    } else if (dstHWPos == Hardware::L0A) {
        return AscendC::MstxTensor::MSTX_TENSOR_AT_L0A;
    } else if (dstHWPos == Hardware::L0B) {
        return AscendC::MstxTensor::MSTX_TENSOR_AT_L0B;
    } else if (dstHWPos == Hardware::L0C) {
        return AscendC::MstxTensor::MSTX_TENSOR_AT_L0C;
    }
}

void CheckResult(MstxTensor::MstxVecUnaryDesc& expect_desc, MstxTensor::MstxVecUnaryDesc& g_vec_unary)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_unary.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_unary.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_unary.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_unary.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_unary.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_unary.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_unary.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_unary.src.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_unary.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_unary.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_unary.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_unary.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_unary.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.blockNum, g_vec_unary.blockNum);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_unary.repeatTimes);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_unary.dstBlockStride);
    EXPECT_EQ(expect_desc.srcBlockStride, g_vec_unary.srcBlockStride);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_unary.dstRepeatStride);
    EXPECT_EQ(expect_desc.srcRepeatStride, g_vec_unary.srcRepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_unary.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_unary.name[1]);
}

void CheckResult(MstxTensor::MstxVecBinaryDesc& expect_desc, MstxTensor::MstxVecBinaryDesc& g_vec_binary)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_binary.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_binary.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_binary.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_binary.dst.dataBits);
    EXPECT_EQ(expect_desc.src0.space, g_vec_binary.src0.space);
    EXPECT_EQ(expect_desc.src0.addr, g_vec_binary.src0.addr);
    EXPECT_EQ(expect_desc.src0.size, g_vec_binary.src0.size);
    EXPECT_EQ(expect_desc.src0.dataBits, g_vec_binary.src0.dataBits);
    EXPECT_EQ(expect_desc.src1.space, g_vec_binary.src1.space);
    EXPECT_EQ(expect_desc.src1.addr, g_vec_binary.src1.addr);
    EXPECT_EQ(expect_desc.src1.size, g_vec_binary.src1.size);
    EXPECT_EQ(expect_desc.src1.dataBits, g_vec_binary.src1.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_binary.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_binary.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_binary.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_binary.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_binary.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.blockNum, g_vec_binary.blockNum);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_binary.dstBlockStride);
    EXPECT_EQ(expect_desc.src0BlockStride, g_vec_binary.src0BlockStride);
    EXPECT_EQ(expect_desc.src1BlockStride, g_vec_binary.src1BlockStride);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_binary.repeatTimes);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_binary.dstRepeatStride);
    EXPECT_EQ(expect_desc.src0RepeatStride, g_vec_binary.src0RepeatStride);
    EXPECT_EQ(expect_desc.src1RepeatStride, g_vec_binary.src1RepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_binary.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_binary.name[1]);
}

void CheckResult(MstxTensor::MstxVecReduceDesc& expect_desc, MstxTensor::MstxVecReduceDesc& g_vec_reduce)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_reduce.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_reduce.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_reduce.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_reduce.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_reduce.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_reduce.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_reduce.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_reduce.src.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_reduce.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_reduce.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_reduce.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_reduce.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_reduce.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.srcBlockStride, g_vec_reduce.srcBlockStride);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_reduce.repeatTimes);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_reduce.dstRepeatStride);
    EXPECT_EQ(expect_desc.srcRepeatStride, g_vec_reduce.srcRepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_reduce.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_reduce.name[1]);
}

void CheckResult(MstxTensor::MstxVecBrcbDesc& expect_desc, MstxTensor::MstxVecBrcbDesc& g_vec_brcb)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_brcb.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_brcb.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_brcb.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_brcb.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_brcb.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_brcb.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_brcb.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_brcb.src.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_brcb.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_brcb.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_brcb.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_brcb.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_brcb.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_brcb.dstBlockStride);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_brcb.repeatTimes);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_brcb.dstRepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_brcb.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_brcb.name[1]);
}

void CheckResult(
    MstxTensor::MstxVecComplexReduceDesc& expect_desc, MstxTensor::MstxVecComplexReduceDesc& g_vec_cpx_reduce)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_cpx_reduce.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_cpx_reduce.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_cpx_reduce.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_cpx_reduce.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_cpx_reduce.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_cpx_reduce.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_cpx_reduce.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_cpx_reduce.src.dataBits);
    EXPECT_EQ(expect_desc.tmp.space, g_vec_cpx_reduce.tmp.space);
    EXPECT_EQ(expect_desc.tmp.addr, g_vec_cpx_reduce.tmp.addr);
    EXPECT_EQ(expect_desc.tmp.size, g_vec_cpx_reduce.tmp.size);
    EXPECT_EQ(expect_desc.tmp.dataBits, g_vec_cpx_reduce.tmp.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_cpx_reduce.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_cpx_reduce.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_cpx_reduce.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_cpx_reduce.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_cpx_reduce.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_cpx_reduce.repeatTimes);
    EXPECT_EQ(expect_desc.srcRepeatStride, g_vec_cpx_reduce.srcRepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_cpx_reduce.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_cpx_reduce.name[1]);
}

void CheckResult(MstxTensor::MstxVecDupDesc& expect_desc, MstxTensor::MstxVecDupDesc& g_vec_dup)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_dup.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_dup.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_dup.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_dup.dst.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_dup.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_dup.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_dup.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_dup.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_dup.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_dup.dstBlockStride);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_dup.repeatTimes);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_dup.dstRepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_dup.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_dup.name[1]);
}

void CheckResult(MstxTensor::MstxVecCopy& expect_desc, MstxTensor::MstxVecCopy& g_vec_copy)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_copy.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_copy.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_copy.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_copy.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_copy.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_copy.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_copy.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_copy.src.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_copy.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_copy.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_copy.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_copy.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_copy.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_copy.repeatTimes);
    EXPECT_EQ(expect_desc.dstStride, g_vec_copy.dstStride);
    EXPECT_EQ(expect_desc.srcStride, g_vec_copy.srcStride);
    EXPECT_EQ(expect_desc.dstRepeatSize, g_vec_copy.dstRepeatSize);
    EXPECT_EQ(expect_desc.srcRepeatSize, g_vec_copy.srcRepeatSize);
    // EXPECT_EQ(expect_desc.name[0], g_vec_copy.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_copy.name[1]);
}

void CheckResult(MstxTensor::MstxVecCastDeqDesc& expect_desc, MstxTensor::MstxVecCastDeqDesc& g_vec_cast_deq)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_cast_deq.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_cast_deq.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_cast_deq.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_cast_deq.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_cast_deq.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_cast_deq.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_cast_deq.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_cast_deq.src.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_cast_deq.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_cast_deq.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_cast_deq.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_cast_deq.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_cast_deq.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.blockNum, g_vec_cast_deq.blockNum);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_cast_deq.dstBlockStride);
    EXPECT_EQ(expect_desc.srcBlockStride, g_vec_cast_deq.srcBlockStride);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_cast_deq.repeatTimes);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_cast_deq.dstRepeatStride);
    EXPECT_EQ(expect_desc.srcRepeatStride, g_vec_cast_deq.srcRepeatStride);
    // EXPECT_EQ(expect_desc.halfBlock, g_vec_cast_deq.halfBlock);
    // EXPECT_EQ(expect_desc.name[0], g_vec_cast_deq.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_cast_deq.name[1]);
}

void CheckResult(MstxTensor::MstxDataCopyDesc& expect_desc, MstxTensor::MstxDataCopyDesc& g_data_copy)
{
    EXPECT_EQ(expect_desc.dst.space, g_data_copy.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_data_copy.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_data_copy.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_data_copy.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_data_copy.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_data_copy.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_data_copy.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_data_copy.src.dataBits);
    EXPECT_EQ(expect_desc.lenBurst, g_data_copy.lenBurst);
    EXPECT_EQ(expect_desc.nBurst, g_data_copy.nBurst);
    EXPECT_EQ(expect_desc.srcGap, g_data_copy.srcGap);
    EXPECT_EQ(expect_desc.dstGap, g_data_copy.dstGap);
    // EXPECT_EQ(expect_desc.name[0], g_data_copy.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_data_copy.name[1]);
}

void CheckResult(MstxTensor::MstxDataCopyPadDesc& expect_desc, MstxTensor::MstxDataCopyPadDesc& g_data_copy_pad)
{
    EXPECT_EQ(expect_desc.dst.space, g_data_copy_pad.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_data_copy_pad.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_data_copy_pad.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_data_copy_pad.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_data_copy_pad.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_data_copy_pad.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_data_copy_pad.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_data_copy_pad.src.dataBits);
    EXPECT_EQ(expect_desc.lenBurst, g_data_copy_pad.lenBurst);
    EXPECT_EQ(expect_desc.nBurst, g_data_copy_pad.nBurst);
    EXPECT_EQ(expect_desc.srcGap, g_data_copy_pad.srcGap);
    EXPECT_EQ(expect_desc.dstGap, g_data_copy_pad.dstGap);
    EXPECT_EQ(expect_desc.leftPad, g_data_copy_pad.leftPad);
    EXPECT_EQ(expect_desc.rightPad, g_data_copy_pad.rightPad);
    // EXPECT_EQ(expect_desc.name[0], g_data_copy_pad.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_data_copy_pad.name[1]);
}

void CheckResult(MstxTensor::MstxDataCopyPadV2Desc& expect_desc, MstxTensor::MstxDataCopyPadV2Desc& g_data_copy_pad_v2)
{
    EXPECT_EQ(expect_desc.dst.space, g_data_copy_pad_v2.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_data_copy_pad_v2.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_data_copy_pad_v2.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_data_copy_pad_v2.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_data_copy_pad_v2.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_data_copy_pad_v2.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_data_copy_pad_v2.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_data_copy_pad_v2.src.dataBits);
    EXPECT_EQ(expect_desc.padMode, g_data_copy_pad_v2.padMode);
    EXPECT_EQ(expect_desc.lenBurst, g_data_copy_pad_v2.lenBurst);
    EXPECT_EQ(expect_desc.nBurst, g_data_copy_pad_v2.nBurst);
    EXPECT_EQ(expect_desc.srcGap, g_data_copy_pad_v2.srcGap);
    EXPECT_EQ(expect_desc.dstGap, g_data_copy_pad_v2.dstGap);
    EXPECT_EQ(expect_desc.leftPad, g_data_copy_pad_v2.leftPad);
    EXPECT_EQ(expect_desc.rightPad, g_data_copy_pad_v2.rightPad);
}

void CheckResult(
    MstxTensor::MstxVecBilinearInterpolation& expect_desc, MstxTensor::MstxVecBilinearInterpolation& g_vec_bilinear)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_bilinear.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_bilinear.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_bilinear.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_bilinear.dst.dataBits);
    EXPECT_EQ(expect_desc.src0.space, g_vec_bilinear.src0.space);
    EXPECT_EQ(expect_desc.src0.addr, g_vec_bilinear.src0.addr);
    EXPECT_EQ(expect_desc.src0.size, g_vec_bilinear.src0.size);
    EXPECT_EQ(expect_desc.src0.dataBits, g_vec_bilinear.src0.dataBits);
    EXPECT_EQ(expect_desc.src1.space, g_vec_bilinear.src1.space);
    EXPECT_EQ(expect_desc.src1.addr, g_vec_bilinear.src1.addr);
    EXPECT_EQ(expect_desc.src1.size, g_vec_bilinear.src1.size);
    EXPECT_EQ(expect_desc.src1.dataBits, g_vec_bilinear.src1.dataBits);
    EXPECT_EQ(expect_desc.src0Offset.space, g_vec_bilinear.src0Offset.space);
    EXPECT_EQ(expect_desc.src0Offset.addr, g_vec_bilinear.src0Offset.addr);
    EXPECT_EQ(expect_desc.src0Offset.size, g_vec_bilinear.src0Offset.size);
    EXPECT_EQ(expect_desc.src0Offset.dataBits, g_vec_bilinear.src0Offset.dataBits);
    EXPECT_EQ(expect_desc.shared.space, g_vec_bilinear.shared.space);
    EXPECT_EQ(expect_desc.shared.addr, g_vec_bilinear.shared.addr);
    EXPECT_EQ(expect_desc.shared.size, g_vec_bilinear.shared.size);
    EXPECT_EQ(expect_desc.shared.dataBits, g_vec_bilinear.shared.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_bilinear.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_bilinear.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_bilinear.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_bilinear.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_bilinear.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.hRepeat, g_vec_bilinear.hRepeat);
    EXPECT_EQ(expect_desc.repeatMode, g_vec_bilinear.repeatMode);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_bilinear.dstBlockStride);
    EXPECT_EQ(expect_desc.vROffset, g_vec_bilinear.vROffset);
    EXPECT_EQ(expect_desc.vRepeat, g_vec_bilinear.vRepeat);
    // EXPECT_EQ(expect_desc.name[0], g_vec_bilinear.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_bilinear.name[1]);
}

void CheckResult(MstxTensor::MstxVecTranspose& expect_desc, MstxTensor::MstxVecTranspose& g_vec_transpose)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_transpose.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_transpose.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_transpose.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_transpose.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_transpose.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_transpose.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_transpose.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_transpose.src.dataBits);
    EXPECT_EQ(expect_desc.shared.space, g_vec_transpose.shared.space);
    EXPECT_EQ(expect_desc.shared.addr, g_vec_transpose.shared.addr);
    EXPECT_EQ(expect_desc.shared.size, g_vec_transpose.shared.size);
    EXPECT_EQ(expect_desc.shared.dataBits, g_vec_transpose.shared.dataBits);
    EXPECT_EQ(expect_desc.temp, g_vec_transpose.temp);
    // EXPECT_EQ(expect_desc.name[0], g_vec_transpose.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_transpose.name[1]);
}

void CheckResult(MstxTensor::MstxVecSelDesc& expect_desc, MstxTensor::MstxVecSelDesc& g_vec_sel)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_sel.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_sel.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_sel.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_sel.dst.dataBits);
    EXPECT_EQ(expect_desc.src0.space, g_vec_sel.src0.space);
    EXPECT_EQ(expect_desc.src0.addr, g_vec_sel.src0.addr);
    EXPECT_EQ(expect_desc.src0.size, g_vec_sel.src0.size);
    EXPECT_EQ(expect_desc.src0.dataBits, g_vec_sel.src0.dataBits);
    EXPECT_EQ(expect_desc.src1.space, g_vec_sel.src1.space);
    EXPECT_EQ(expect_desc.src1.addr, g_vec_sel.src1.addr);
    EXPECT_EQ(expect_desc.src1.size, g_vec_sel.src1.size);
    EXPECT_EQ(expect_desc.src1.dataBits, g_vec_sel.src1.dataBits);
    EXPECT_EQ(expect_desc.mask.space, g_vec_sel.mask.space);
    EXPECT_EQ(expect_desc.mask.addr, g_vec_sel.mask.addr);
    EXPECT_EQ(expect_desc.mask.size, g_vec_sel.mask.size);
    EXPECT_EQ(expect_desc.mask.dataBits, g_vec_sel.mask.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_sel.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_sel.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_sel.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_sel.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_sel.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.scalarMode, g_vec_sel.scalarMode);
    EXPECT_EQ(expect_desc.blockNum, g_vec_sel.blockNum);
    EXPECT_EQ(expect_desc.dstBlockStride, g_vec_sel.dstBlockStride);
    EXPECT_EQ(expect_desc.src0BlockStride, g_vec_sel.src0BlockStride);
    EXPECT_EQ(expect_desc.src1BlockStride, g_vec_sel.src1BlockStride);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_sel.repeatTimes);
    EXPECT_EQ(expect_desc.dstRepeatStride, g_vec_sel.dstRepeatStride);
    EXPECT_EQ(expect_desc.src0RepeatStride, g_vec_sel.src0RepeatStride);
    EXPECT_EQ(expect_desc.src1RepeatStride, g_vec_sel.src1RepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_sel.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_sel.name[1]);
}

void CheckResult(MstxTensor::MstxVecGatherMaskDesc& expect_desc, MstxTensor::MstxVecGatherMaskDesc& g_vec_gather_mask)
{
    EXPECT_EQ(expect_desc.dst.space, g_vec_gather_mask.dst.space);
    EXPECT_EQ(expect_desc.dst.addr, g_vec_gather_mask.dst.addr);
    EXPECT_EQ(expect_desc.dst.size, g_vec_gather_mask.dst.size);
    EXPECT_EQ(expect_desc.dst.dataBits, g_vec_gather_mask.dst.dataBits);
    EXPECT_EQ(expect_desc.src.space, g_vec_gather_mask.src.space);
    EXPECT_EQ(expect_desc.src.addr, g_vec_gather_mask.src.addr);
    EXPECT_EQ(expect_desc.src.size, g_vec_gather_mask.src.size);
    EXPECT_EQ(expect_desc.src.dataBits, g_vec_gather_mask.src.dataBits);
    EXPECT_EQ(expect_desc.wrapper.maskMode, g_vec_gather_mask.wrapper.maskMode);
    EXPECT_EQ(expect_desc.wrapper.mask.mask0, g_vec_gather_mask.wrapper.mask.mask0);
    EXPECT_EQ(expect_desc.wrapper.mask.mask1, g_vec_gather_mask.wrapper.mask.mask1);
    EXPECT_EQ(expect_desc.wrapper.useMask, g_vec_gather_mask.wrapper.useMask);
    EXPECT_EQ(expect_desc.wrapper.reserveBufSize, g_vec_gather_mask.wrapper.reserveBufSize);
    EXPECT_EQ(expect_desc.mode, g_vec_gather_mask.mode);
    EXPECT_EQ(expect_desc.repeatTimes, g_vec_gather_mask.repeatTimes);
    EXPECT_EQ(expect_desc.src0BlockStride, g_vec_gather_mask.src0BlockStride);
    EXPECT_EQ(expect_desc.src0RepeatStride, g_vec_gather_mask.src0RepeatStride);
    EXPECT_EQ(expect_desc.src1RepeatStride, g_vec_gather_mask.src1RepeatStride);
    // EXPECT_EQ(expect_desc.name[0], g_vec_gather_mask.name[0]);
    // EXPECT_EQ(expect_desc.name[1], g_vec_gather_mask.name[1]);
}

template <typename T, bool USE_CAST_DEQ>
void MainVecUnaryMstx01(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> inputLocal2 = tbuf1.Get<int16_t>();

    TBuf<TPosition::VECCALC> tbuf2;
    tpipe.InitBuffer(tbuf2, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf2.Get<T>();

    TBuf<TPosition::VECCALC> tbuf3;
    tpipe.InitBuffer(tbuf3, 5 * dataSize * sizeof(int8_t));
    LocalTensor<int8_t> outputLocal2 = tbuf3.Get<int8_t>();

    TBuf<TPosition::VECCALC> tbuf4;
    tpipe.InitBuffer(tbuf4, 5 * dataSize * sizeof(uint8_t));
    LocalTensor<uint8_t> outputLocal3 = tbuf4.Get<uint8_t>();

    TBuf<TPosition::VECCALC> tbuf5;
    tpipe.InitBuffer(tbuf5, 5 * dataSize * sizeof(half));
    LocalTensor<half> outputLocalHalf = tbuf5.Get<half>();

    TBuf<TPosition::VECCALC> tbuf6;
    tpipe.InitBuffer(tbuf6, 5 * dataSize * sizeof(int32_t));
    LocalTensor<int32_t> outputLocalInt32 = tbuf6.Get<int32_t>();

    TBuf<TPosition::VECCALC> tbuf7;
    tpipe.InitBuffer(tbuf7, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> outputLocalInt16 = tbuf7.Get<int16_t>();

    TBuf<TPosition::VECCALC> tbuf8;
    tpipe.InitBuffer(tbuf8, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> inputLocalInt16 = tbuf8.Get<int16_t>();

    TBuf<TPosition::VECCALC> tbuf9;
    tpipe.InitBuffer(tbuf9, 5 * dataSize * sizeof(int32_t));
    LocalTensor<int32_t> inputLocalInt32 = tbuf9.Get<int32_t>();

    TBuf<TPosition::VECCALC> tbuf10;
    tpipe.InitBuffer(tbuf10, 5 * dataSize * sizeof(half));
    LocalTensor<half> inputLocalHalf = tbuf10.Get<half>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123; // mask相等时，mask0是18446744073709551615，mask1是576460752303423487
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;
    MstxTensor::MstxVecUnaryDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.blockNum = repeatParams.blockNumber;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstBlockStride = repeatParams.dstBlkStride;
    expect_desc.srcBlockStride = repeatParams.srcBlkStride;
    expect_desc.dstRepeatStride = repeatParams.dstRepStride;
    expect_desc.srcRepeatStride = repeatParams.srcRepStride;

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "Relu");
    Relu(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Exp");
    Exp(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Ln");
    Ln(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Abs");
    Abs(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Reciprocal");
    Reciprocal(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Sqrt");
    Sqrt(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Rsqrt");
    Rsqrt(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Adds");
    Adds(outputLocal, inputLocal, scalar, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Muls");
    Muls(outputLocal, inputLocal, scalar, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Maxs");
    Maxs(outputLocal, inputLocal, scalar, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Mins");
    Mins(outputLocal, inputLocal, scalar, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "LeakyRelu");
    LeakyRelu(outputLocal, inputLocal, scalar, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Axpy");
    Axpy(outputLocal, inputLocal, scalar, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src.size = inputLocalInt16.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "Not");
    Not(outputLocalInt16, inputLocalInt16, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "ShiftLeft");
    ShiftLeft(outputLocalInt16, inputLocalInt16, static_cast<int16_t>(scalar), maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "ShiftRight");
    ShiftRight(outputLocalInt16, inputLocalInt16, static_cast<int16_t>(scalar), maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    CopyNameUt(expect_desc.name, "Relu");
    Relu(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Exp");
    Exp(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Ln");
    Ln(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Abs");
    Abs(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Reciprocal");
    Reciprocal(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Sqrt");
    Sqrt(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Rsqrt");
    Rsqrt(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Adds");
    Adds(outputLocal, inputLocal, scalar, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Muls");
    Muls(outputLocal, inputLocal, scalar, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Maxs");
    Maxs(outputLocal, inputLocal, scalar, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Mins");
    Mins(outputLocal, inputLocal, scalar, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "LeakyRelu");
    LeakyRelu(outputLocal, inputLocal, scalar, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Axpy");
    Axpy(outputLocal, inputLocal, scalar, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src.size = inputLocalInt16.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "Not");
    Not(outputLocalInt16, inputLocalInt16, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "ShiftLeft");
    ShiftLeft(outputLocalInt16, inputLocalInt16, static_cast<int16_t>(scalar), maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "ShiftRight");
    ShiftRight(outputLocalInt16, inputLocalInt16, static_cast<int16_t>(scalar), maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);
    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src.size = inputLocalInt32.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));
    CopyNameUt(expect_desc.name, "Cast");
    Cast(outputLocal, inputLocalInt32, roundMode, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_unary);

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    // //count
    expect_desc.wrapper.mask.mask0 = count;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.blockNum = 8;
    expect_desc.repeatTimes = 1;
    expect_desc.dstBlockStride = 1;
    expect_desc.srcBlockStride = 1;
    expect_desc.dstRepeatStride = 8;
    expect_desc.srcRepeatStride = 8;
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_COUNT;
    CopyNameUt(expect_desc.name, "Relu");
    Relu(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Exp");
    Exp(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Ln");
    Ln(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Abs");
    Abs(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Reciprocal");
    Reciprocal(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Sqrt");
    Sqrt(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Rsqrt");
    Rsqrt(outputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Adds");
    Adds(outputLocal, inputLocal, scalar, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Muls");
    Muls(outputLocal, inputLocal, scalar, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Maxs");
    Maxs(outputLocal, inputLocal, scalar, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Mins");
    Mins(outputLocal, inputLocal, scalar, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "LeakyRelu");
    LeakyRelu(outputLocal, inputLocal, scalar, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "Axpy");
    Axpy(outputLocal, inputLocal, scalar, count);
    CheckResult(expect_desc, g_vec_unary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src.size = inputLocalInt16.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "Not");
    Not(outputLocalInt16, inputLocalInt16, count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "ShiftLeft");
    ShiftLeft(outputLocalInt16, inputLocalInt16, static_cast<int16_t>(scalar), count);
    CheckResult(expect_desc, g_vec_unary);
    CopyNameUt(expect_desc.name, "ShiftRight");
    ShiftRight(outputLocalInt16, inputLocalInt16, static_cast<int16_t>(scalar), count);
    CheckResult(expect_desc, g_vec_unary);

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src.size = inputLocalInt32.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));
    CopyNameUt(expect_desc.name, "Cast");
    expect_desc.dstRepeatStride = 4;
    Cast(outputLocal, inputLocalInt32, roundMode, count);
    CheckResult(expect_desc, g_vec_unary);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecBinaryMstx02(
    __gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    TBuf<TPosition::VECCALC> tbuf2;
    tpipe.InitBuffer(tbuf2, 5 * dataSize * sizeof(half));
    LocalTensor<half> outputLocalHalf = tbuf2.Get<half>();

    TBuf<TPosition::VECCALC> tbuf3;
    tpipe.InitBuffer(tbuf3, 5 * dataSize * sizeof(int8_t));
    LocalTensor<int8_t> outputLocalInt8 = tbuf3.Get<int8_t>();

    TBuf<TPosition::VECCALC> tbuf4;
    tpipe.InitBuffer(tbuf4, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> outputLocalInt16 = tbuf4.Get<int16_t>();

    TBuf<TPosition::VECCALC> tbuf5;
    tpipe.InitBuffer(tbuf5, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> inputLocalInt16 = tbuf5.Get<int16_t>();

    TBuf<TPosition::VECCALC> tbuf6;
    tpipe.InitBuffer(tbuf6, 5 * dataSize * sizeof(int32_t));
    LocalTensor<int32_t> inputLocalInt32 = tbuf6.Get<int32_t>();

    TBuf<TPosition::VECCALC> tbuf7;
    tpipe.InitBuffer(tbuf7, 5 * dataSize * sizeof(half));
    LocalTensor<half> inputLocalHalf = tbuf7.Get<half>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    BinaryRepeatParams repeatParams = {2, 2, 2, 6, 6, 6};
    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123; // mask相等时，mask0是18446744073709551615，mask1是576460752303423487
    uint8_t repeatTimes = 5;
    int32_t count = 456;
    MstxTensor::MstxVecBinaryDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src0.space = GetTensorSpace(inputLocal);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src0.size = inputLocal.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src1.space = GetTensorSpace(inputLocal);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src1.size = inputLocal.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.blockNum = repeatParams.blockNumber;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstBlockStride = repeatParams.dstBlkStride;
    expect_desc.src0BlockStride = repeatParams.src0BlkStride;
    expect_desc.src1BlockStride = repeatParams.src1BlkStride;
    expect_desc.dstRepeatStride = repeatParams.dstRepStride;
    expect_desc.src0RepeatStride = repeatParams.src0RepStride;
    expect_desc.src1RepeatStride = repeatParams.src1RepStride;

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "Add");
    Add(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Sub");
    Sub(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Mul");
    Mul(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Max");
    Max(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Min");
    Min(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src0.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt16.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src1.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt16.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "And");
    And(outputLocalInt16, inputLocalInt16, inputLocalInt16, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Or");
    Or(outputLocalInt16, inputLocalInt16, inputLocalInt16, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalHalf);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalHalf.GetPhyAddr());
    expect_desc.dst.size = outputLocalHalf.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    expect_desc.src0.space = GetTensorSpace(inputLocalHalf);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalHalf.GetPhyAddr());
    expect_desc.src0.size = inputLocalHalf.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    expect_desc.src1.space = GetTensorSpace(inputLocalHalf);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalHalf.GetPhyAddr());
    expect_desc.src1.size = inputLocalHalf.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    CopyNameUt(expect_desc.name, "Div");
    Div(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "AddRelu");
    AddRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "SubRelu");
    SubRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "MulAddDst");
    MulAddDst(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "FusedMulAdd");
    FusedMulAdd(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "FusedMulAddRelu");
    FusedMulAddRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "MulCast");
    MulCast(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    expect_desc.wrapper.reserveBufSize = 0;

    expect_desc.dst.space = GetTensorSpace(outputLocalInt8);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt8.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt8.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int8_t));

    expect_desc.src0.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt16.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));

    expect_desc.src1.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt16.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "AddReluCast");
    AddReluCast(outputLocalInt8, inputLocalInt16, inputLocalInt16, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "SubReluCast");
    SubReluCast(outputLocalInt8, inputLocalInt16, inputLocalInt16, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalHalf);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalHalf.GetPhyAddr());
    expect_desc.dst.size = outputLocalHalf.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(half));

    expect_desc.src0.space = GetTensorSpace(inputLocalInt32);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt32.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));

    expect_desc.src1.space = GetTensorSpace(inputLocalInt32);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt32.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));
    CopyNameUt(expect_desc.name, "AddDeqRelu");
    expect_desc.wrapper.reserveBufSize = 8192;
    AddDeqRelu(outputLocalHalf, inputLocalInt32, inputLocalInt32, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    expect_desc.wrapper.reserveBufSize = 0;

    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src0.space = GetTensorSpace(inputLocal);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src0.size = inputLocal.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src1.space = GetTensorSpace(inputLocal);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src1.size = inputLocal.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    CopyNameUt(expect_desc.name, "Add");
    Add(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Sub");
    Sub(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Mul");
    Mul(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Max");
    Max(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Min");
    Min(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "MulCast");
    MulCast(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src0.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt16.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src1.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt16.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "And");
    And(outputLocalInt16, inputLocalInt16, inputLocalInt16, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Or");
    Or(outputLocalInt16, inputLocalInt16, inputLocalInt16, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalHalf);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalHalf.GetPhyAddr());
    expect_desc.dst.size = outputLocalHalf.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    expect_desc.src0.space = GetTensorSpace(inputLocalHalf);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalHalf.GetPhyAddr());
    expect_desc.src0.size = inputLocalHalf.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    expect_desc.src1.space = GetTensorSpace(inputLocalHalf);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalHalf.GetPhyAddr());
    expect_desc.src1.size = inputLocalHalf.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    CopyNameUt(expect_desc.name, "Div");
    Div(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "AddRelu");
    AddRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "SubRelu");
    SubRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "MulAddDst");
    MulAddDst(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "FusedMulAdd");
    FusedMulAdd(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "FusedMulAddRelu");
    FusedMulAddRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt8);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt8.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt8.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int8_t));

    expect_desc.src0.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt16.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));

    expect_desc.src1.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt16.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "AddReluCast");
    AddReluCast(outputLocalInt8, inputLocalInt16, inputLocalInt16, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "SubReluCast");
    SubReluCast(outputLocalInt8, inputLocalInt16, inputLocalInt16, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalHalf);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalHalf.GetPhyAddr());
    expect_desc.dst.size = outputLocalHalf.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(half));

    expect_desc.src0.space = GetTensorSpace(inputLocalInt32);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt32.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));

    expect_desc.src1.space = GetTensorSpace(inputLocalInt32);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt32.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));
    CopyNameUt(expect_desc.name, "AddDeqRelu");
    expect_desc.wrapper.reserveBufSize = 8192;
    AddDeqRelu(outputLocalHalf, inputLocalInt32, inputLocalInt32, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_binary);
    expect_desc.wrapper.reserveBufSize = 0;

    // count
    expect_desc.wrapper.mask.mask0 = count;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.blockNum = 8;
    expect_desc.repeatTimes = 1;
    expect_desc.dstBlockStride = 1;
    expect_desc.src0BlockStride = 1;
    expect_desc.src1BlockStride = 1;
    expect_desc.dstRepeatStride = 8;
    expect_desc.src0RepeatStride = 8;
    expect_desc.src1RepeatStride = 8;
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_COUNT;
    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src0.space = GetTensorSpace(inputLocal);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src0.size = inputLocal.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    expect_desc.src1.space = GetTensorSpace(inputLocal);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src1.size = inputLocal.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    CopyNameUt(expect_desc.name, "Add");
    Add(outputLocal, inputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Sub");
    Sub(outputLocal, inputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Mul");
    Mul(outputLocal, inputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Max");
    Max(outputLocal, inputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Min");
    Min(outputLocal, inputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_binary);
    // CopyNameUt(expect_desc.name, "MulCast");
    // MulCast(outputLocal, inputLocal, inputLocal, count);
    // CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src0.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt16.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    expect_desc.src1.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt16.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "And");
    And(outputLocalInt16, inputLocalInt16, inputLocalInt16, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "Or");
    Or(outputLocalInt16, inputLocalInt16, inputLocalInt16, count);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalHalf);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalHalf.GetPhyAddr());
    expect_desc.dst.size = outputLocalHalf.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    expect_desc.src0.space = GetTensorSpace(inputLocalHalf);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalHalf.GetPhyAddr());
    expect_desc.src0.size = inputLocalHalf.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    expect_desc.src1.space = GetTensorSpace(inputLocalHalf);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalHalf.GetPhyAddr());
    expect_desc.src1.size = inputLocalHalf.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(half));
    CopyNameUt(expect_desc.name, "Div");
    Div(outputLocalHalf, inputLocalHalf, inputLocalHalf, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "AddRelu");
    AddRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "SubRelu");
    SubRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "MulAddDst");
    MulAddDst(outputLocalHalf, inputLocalHalf, inputLocalHalf, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "FusedMulAdd");
    FusedMulAdd(outputLocalHalf, inputLocalHalf, inputLocalHalf, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "FusedMulAddRelu");
    FusedMulAddRelu(outputLocalHalf, inputLocalHalf, inputLocalHalf, count);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalInt8);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt8.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt8.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int8_t));

    expect_desc.src0.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt16.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));

    expect_desc.src1.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt16.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));
    CopyNameUt(expect_desc.name, "AddReluCast");
    AddReluCast(outputLocalInt8, inputLocalInt16, inputLocalInt16, count);
    CheckResult(expect_desc, g_vec_binary);
    CopyNameUt(expect_desc.name, "SubReluCast");
    SubReluCast(outputLocalInt8, inputLocalInt16, inputLocalInt16, count);
    CheckResult(expect_desc, g_vec_binary);

    expect_desc.dst.space = GetTensorSpace(outputLocalHalf);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalHalf.GetPhyAddr());
    expect_desc.dst.size = outputLocalHalf.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(half));

    expect_desc.src0.space = GetTensorSpace(inputLocalInt32);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src0.size = inputLocalInt32.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));

    expect_desc.src1.space = GetTensorSpace(inputLocalInt32);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocalInt32.GetPhyAddr());
    expect_desc.src1.size = inputLocalInt32.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(int32_t));
    CopyNameUt(expect_desc.name, "AddDeqRelu");
    expect_desc.wrapper.reserveBufSize = 8192;
    AddDeqRelu(outputLocalHalf, inputLocalInt32, inputLocalInt32, count);
    CheckResult(expect_desc, g_vec_binary);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx03(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    TBuf<TPosition::VECCALC> tbuf2;
    tpipe.InitBuffer(tbuf2, 5 * dataSize * sizeof(unsigned int));
    LocalTensor<unsigned int> tmpLocal = tbuf2.Get<unsigned int>();

    TBuf<TPosition::VECCALC> tbuf3;
    tpipe.InitBuffer(tbuf3, 5 * dataSize * sizeof(unsigned char));
    LocalTensor<unsigned char> tmpLocalChar = tbuf3.Get<unsigned char>();

    TBuf<TPosition::VECCALC> tbuf4;
    tpipe.InitBuffer(tbuf4, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> outputLocalInt16 = tbuf4.Get<int16_t>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;

    MstxTensor::MstxVecBilinearInterpolation expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocalInt16);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt16.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt16.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));

    expect_desc.src0.space = GetTensorSpace(inputLocal);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src0.size = inputLocal.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src1.space = GetTensorSpace(tmpLocal);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(tmpLocal.GetPhyAddr());
    expect_desc.src1.size = tmpLocal.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(unsigned int));

    expect_desc.src0Offset.space = GetTensorSpace(inputLocal);
    expect_desc.src0Offset.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src0Offset.size = inputLocal.GetSize();
    expect_desc.src0Offset.dataBits = static_cast<uint8_t>(8 * sizeof(uint32_t));

    expect_desc.shared.space = GetTensorSpace(tmpLocalChar);
    expect_desc.shared.addr = reinterpret_cast<uint64_t>(tmpLocalChar.GetPhyAddr());
    expect_desc.shared.size = tmpLocalChar.GetSize();
    expect_desc.shared.dataBits = static_cast<uint8_t>(8 * sizeof(unsigned char));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.hRepeat = 6;
    expect_desc.repeatMode = false;
    expect_desc.dstBlockStride = 9;
    expect_desc.vROffset = 3;
    expect_desc.vRepeat = 3;

    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    CopyNameUt(expect_desc.name, "BilinearInterpolation");
    BilinearInterpolation(outputLocalInt16, inputLocal, tmpLocal, inputLocal, maskBit, 6, false, 9, 3, 3, tmpLocalChar);
    CheckResult(expect_desc, g_vec_bilinear);

    BilinearInterpolation(
        outputLocalInt16, inputLocal, tmpLocal, inputLocal, maskCounter, 6, false, 9, 3, 3, tmpLocalChar);
    CheckResult(expect_desc, g_vec_bilinear);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx04(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    TBuf<TPosition::VECCALC> tbuf3;
    tpipe.InitBuffer(tbuf3, 5 * dataSize * sizeof(int8_t));
    LocalTensor<int8_t> outputLocalInt8 = tbuf3.Get<int8_t>();

    TBuf<TPosition::VECCALC> tbuf5;
    tpipe.InitBuffer(tbuf5, 5 * dataSize * sizeof(int16_t));
    LocalTensor<int16_t> inputLocalInt16 = tbuf5.Get<int16_t>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecCastDeqDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocalInt8);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocalInt8.GetPhyAddr());
    expect_desc.dst.size = outputLocalInt8.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(int8_t));

    expect_desc.src.space = GetTensorSpace(inputLocalInt16);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocalInt16.GetPhyAddr());
    expect_desc.src.size = inputLocalInt16.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(int16_t));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.blockNum = repeatParams.blockNumber;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstBlockStride = repeatParams.dstBlkStride;
    expect_desc.srcBlockStride = repeatParams.srcBlkStride;
    expect_desc.dstRepeatStride = repeatParams.dstRepStride;
    expect_desc.srcRepeatStride = repeatParams.srcRepStride;
    expect_desc.halfBlock = false;

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "CastDeq");
    CastDeq(outputLocalInt8, inputLocalInt16, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_cast_deq);

    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    CopyNameUt(expect_desc.name, "CastDeq");
    CastDeq(outputLocalInt8, inputLocalInt16, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_cast_deq);

    // count
    expect_desc.wrapper.mask.mask0 = count;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.blockNum = 8;
    expect_desc.repeatTimes = 1;
    expect_desc.dstBlockStride = 1;
    expect_desc.srcBlockStride = 1;
    expect_desc.dstRepeatStride = 4;
    expect_desc.srcRepeatStride = 8;
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_COUNT;
    CopyNameUt(expect_desc.name, "CastDeq");
    CastDeq(outputLocalInt8, inputLocalInt16, count);
    CheckResult(expect_desc, g_vec_cast_deq);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx05(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    BinaryRepeatParams repeatParams = {2, 2, 2, 6, 6, 6};
    int32_t count = 456;

    MstxTensor::MstxVecSelDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src0.space = GetTensorSpace(inputLocal);
    expect_desc.src0.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src0.size = inputLocal.GetSize();
    expect_desc.src0.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src1.space = GetTensorSpace(inputLocal);
    expect_desc.src1.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src1.size = inputLocal.GetSize();
    expect_desc.src1.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.mask.space = GetTensorSpace(inputLocal);
    expect_desc.mask.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.mask.size = inputLocal.GetSize();
    expect_desc.mask.dataBits = static_cast<uint8_t>(8 * sizeof(uint8_t));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.scalarMode = true;
    expect_desc.blockNum = repeatParams.blockNumber;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstBlockStride = repeatParams.dstBlkStride;
    expect_desc.src0BlockStride = repeatParams.src0BlkStride;
    expect_desc.src1BlockStride = repeatParams.src1BlkStride;
    expect_desc.dstRepeatStride = repeatParams.dstRepStride;
    expect_desc.src0RepeatStride = repeatParams.src0RepStride;
    expect_desc.src1RepeatStride = repeatParams.src1RepStride;

    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    CopyNameUt(expect_desc.name, "Select");
    Select(
        outputLocal, inputLocal, inputLocal, inputLocal, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, maskBit,
        repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_sel);

    // mask是0
    expect_desc.wrapper.mask.mask0 = 0;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.wrapper.useMask = false;
    CopyNameUt(expect_desc.name, "Select");
    Select(outputLocal, inputLocal, inputLocal, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_sel);
    expect_desc.wrapper.useMask = true;

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "Select");
    Select(
        outputLocal, inputLocal, inputLocal, inputLocal, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, maskCounter,
        repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_sel);

    expect_desc.wrapper.mask.mask0 = count;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_COUNT;
    expect_desc.blockNum = 8;
    expect_desc.repeatTimes = 1;
    expect_desc.dstBlockStride = 1;
    expect_desc.src0BlockStride = 1;
    expect_desc.src1BlockStride = 1;
    expect_desc.dstRepeatStride = 8;
    expect_desc.src0RepeatStride = 8;
    expect_desc.src1RepeatStride = 8;
    CopyNameUt(expect_desc.name, "Select");
    Select(outputLocal, inputLocal, inputLocal, inputLocal, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, count);
    CheckResult(expect_desc, g_vec_sel);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx06(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    TBuf<TPosition::VECCALC> tbuf5;
    tpipe.InitBuffer(tbuf5, 5 * dataSize * sizeof(uint8_t));
    LocalTensor<uint8_t> inputLocalInt8 = tbuf5.Get<uint8_t>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint32_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecGatherMaskDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.mode = MstxTensor::MstxGatherMaskMode::V2;
    expect_desc.repeatTimes = 1;
    expect_desc.src0BlockStride = 1;
    expect_desc.src0RepeatStride = 0;
    expect_desc.src1RepeatStride = 0;

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "GatherMask");
    uint64_t rsvCnt = 0;
    GatherMask(outputLocal, inputLocal, 1, false, maskCounter, {1, 1, 0, 0}, rsvCnt);
    CheckResult(expect_desc, g_vec_gather_mask);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx07(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecTranspose expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.temp = false;
    CopyNameUt(expect_desc.name, "Transpose");
    Transpose(outputLocal, inputLocal);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx08(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint32_t maskBit[2] = {0x77, 0x88};
    uint32_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecReduceDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.srcBlockStride = 1;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstRepeatStride = 1;
    expect_desc.srcRepeatStride = 1;

    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "WholeReduceSum");
    WholeReduceSum<T, true>(outputLocal, inputLocal, maskCounter, repeatTimes, 1, 1, 1);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "WholeReduceMax");
    WholeReduceMax<T, true>(outputLocal, inputLocal, maskCounter, repeatTimes, 1, 1, 1, ReduceOrder::ORDER_VALUE_INDEX);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "WholeReduceMin");
    WholeReduceMin<T, true>(outputLocal, inputLocal, maskCounter, repeatTimes, 1, 1, 1, ReduceOrder::ORDER_VALUE_INDEX);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "BlockReduceSum");
    BlockReduceSum<T, true>(outputLocal, inputLocal, repeatTimes, maskCounter, 1, 1, 1);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "BlockReduceMax");
    BlockReduceMax<T, true>(outputLocal, inputLocal, repeatTimes, maskCounter, 1, 1, 1);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "BlockReduceMin");
    BlockReduceMin<T, true>(outputLocal, inputLocal, repeatTimes, maskCounter, 1, 1, 1);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "PairReduceSum");
    PairReduceSum<T, true>(outputLocal, inputLocal, repeatTimes, maskCounter, 1, 1, 1);
    CheckResult(expect_desc, g_vec_reduce);
    CopyNameUt(expect_desc.name, "RepeatReduceSum");
    RepeatReduceSum<T, true>(outputLocal, inputLocal, repeatTimes, maskCounter, 1, 1, 1, 1);
    CheckResult(expect_desc, g_vec_reduce);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx09(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecComplexReduceDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.tmp.space = GetTensorSpace(inputLocal);
    expect_desc.tmp.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.tmp.size = inputLocal.GetSize();
    expect_desc.tmp.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 8192;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.repeatTimes = repeatTimes;
    expect_desc.srcRepeatStride = 1;

    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    CopyNameUt(expect_desc.name, "ReduceMax");
    ReduceMax<T>(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, 1, true);
    CheckResult(expect_desc, g_vec_cpx_reduce);
    CopyNameUt(expect_desc.name, "ReduceMin");
    ReduceMin<T>(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, 1, true);
    CheckResult(expect_desc, g_vec_cpx_reduce);
    CopyNameUt(expect_desc.name, "ReduceSum");
    ReduceSum<T>(outputLocal, inputLocal, inputLocal, maskBit, repeatTimes, 1);
    CheckResult(expect_desc, g_vec_cpx_reduce);

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "ReduceMax");
    ReduceMax<T>(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, 1, true);
    CheckResult(expect_desc, g_vec_cpx_reduce);
    CopyNameUt(expect_desc.name, "ReduceMin");
    ReduceMin<T>(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, 1, true);
    CheckResult(expect_desc, g_vec_cpx_reduce);
    CopyNameUt(expect_desc.name, "ReduceSum");
    ReduceSum<T>(outputLocal, inputLocal, inputLocal, maskCounter, repeatTimes, 1);
    CheckResult(expect_desc, g_vec_cpx_reduce);

    expect_desc.wrapper.mask.mask0 = count;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_COUNT;
    expect_desc.repeatTimes = 1;
    expect_desc.srcRepeatStride = 8;
    CopyNameUt(expect_desc.name, "ReduceMax");
    ReduceMax<T>(outputLocal, inputLocal, inputLocal, count, true);
    CheckResult(expect_desc, g_vec_cpx_reduce);
    CopyNameUt(expect_desc.name, "ReduceMin");
    ReduceMin<T>(outputLocal, inputLocal, inputLocal, count, true);
    CheckResult(expect_desc, g_vec_cpx_reduce);
    CopyNameUt(expect_desc.name, "ReduceSum");
    ReduceSum<T>(outputLocal, inputLocal, inputLocal, count);
    CheckResult(expect_desc, g_vec_cpx_reduce);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx10(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    UnaryRepeatParams repeatParams = {2, 2, 6, 6};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecDupDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.dstBlockStride = 1;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstRepeatStride = 1;

    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    CopyNameUt(expect_desc.name, "Duplicate");
    Duplicate(outputLocal, static_cast<T>(0), maskBit, repeatTimes, 1, 1);
    CheckResult(expect_desc, g_vec_dup);
    expect_desc.wrapper.reserveBufSize = 8192;
    CopyNameUt(expect_desc.name, "CreateVecIndex");
    CreateVecIndex(outputLocal, static_cast<T>(0), maskBit, repeatTimes, 1, 1);
    CheckResult(expect_desc, g_vec_dup);

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    expect_desc.wrapper.reserveBufSize = 0;
    CopyNameUt(expect_desc.name, "Duplicate");
    Duplicate(outputLocal, static_cast<T>(0), maskCounter, repeatTimes, 1, 1);
    CheckResult(expect_desc, g_vec_dup);
    expect_desc.wrapper.reserveBufSize = 8192;
    CopyNameUt(expect_desc.name, "CreateVecIndex");
    CreateVecIndex(outputLocal, static_cast<T>(0), maskCounter, repeatTimes, 1, 1);
    CheckResult(expect_desc, g_vec_dup);

    expect_desc.wrapper.mask.mask0 = count;
    expect_desc.wrapper.mask.mask1 = 0;
    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_COUNT;
    expect_desc.repeatTimes = 1;
    expect_desc.dstBlockStride = 1;
    expect_desc.dstRepeatStride = 8;
    expect_desc.wrapper.reserveBufSize = 0;
    CopyNameUt(expect_desc.name, "Duplicate");
    Duplicate(outputLocal, static_cast<T>(0), count);
    CheckResult(expect_desc, g_vec_dup);
    expect_desc.wrapper.reserveBufSize = 8192;
    CopyNameUt(expect_desc.name, "CreateVecIndex");
    CreateVecIndex(outputLocal, static_cast<T>(0), count);
    CheckResult(expect_desc, g_vec_dup);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx11(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    BrcbRepeatParams repeatParams{1, 8};
    int32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecBrcbDesc expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = false;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.dstBlockStride = 1;
    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstRepeatStride = 8;

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 0;
    expect_desc.wrapper.mask.mask1 = 0;
    CopyNameUt(expect_desc.name, "Broadcast");
    Brcb(outputLocal, inputLocal, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_brcb);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

template <typename T>
void MainVecUnaryMstx12(__gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 5 * dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, 5 * dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf1.Get<T>();

    AscendCUtils::SetMask<uint8_t>(256);

    DataCopy(inputLocal, inputGlobal, dataSize);

    set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
    AscendCUtils::SetMask<uint8_t>(128);
    wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint64_t maskBit[2] = {0x77, 0x88};
    uint64_t maskCounter = 123;
    uint8_t repeatTimes = 5;
    CopyRepeatParams repeatParams = {0, 0, 8, 8};
    uint32_t count = 456;
    T scalar = 2;
    RoundMode roundMode = RoundMode::CAST_ROUND;

    MstxTensor::MstxVecCopy expect_desc;

    expect_desc.dst.space = GetTensorSpace(outputLocal);
    expect_desc.dst.addr = reinterpret_cast<uint64_t>(outputLocal.GetPhyAddr());
    expect_desc.dst.size = outputLocal.GetSize();
    expect_desc.dst.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.src.space = GetTensorSpace(inputLocal);
    expect_desc.src.addr = reinterpret_cast<uint64_t>(inputLocal.GetPhyAddr());
    expect_desc.src.size = inputLocal.GetSize();
    expect_desc.src.dataBits = static_cast<uint8_t>(8 * sizeof(PrimT<T>));

    expect_desc.wrapper.maskMode = MstxTensor::MSTX_MASK_FROM_REG;
    expect_desc.wrapper.useMask = true;
    expect_desc.wrapper.reserveBufSize = 0;
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];

    expect_desc.repeatTimes = repeatTimes;
    expect_desc.dstStride = 0;
    expect_desc.srcStride = 0;
    expect_desc.dstRepeatSize = 8;
    expect_desc.srcRepeatSize = 8;

    // mask是[]
    expect_desc.wrapper.mask.mask0 = maskBit[0];
    expect_desc.wrapper.mask.mask1 = maskBit[1];
    CopyNameUt(expect_desc.name, "Copy");
    Copy(outputLocal, inputLocal, maskBit, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_copy);

    // mask是uint64
    expect_desc.wrapper.mask.mask0 = 18446744073709551615;
    expect_desc.wrapper.mask.mask1 = 576460752303423487;
    CopyNameUt(expect_desc.name, "Copy");
    Copy(outputLocal, inputLocal, maskCounter, repeatTimes, repeatParams);
    CheckResult(expect_desc, g_vec_copy);

    set_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    pipe_barrier(PIPE_ALL);
}

struct UnaryTestParamsMstx {
    int32_t dataSize;
    int32_t dataBitSize;
    void (*CalFunc)(uint8_t*, uint8_t*, int32_t);
};

class UnarySimpleMstxTestsuite : public testing::Test, public testing::WithParamInterface<UnaryTestParamsMstx> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_UNARY_SIMPLE_MSTX, UnarySimpleMstxTestsuite,
    ::testing::Values(
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx01<half, true>},
        UnaryTestParamsMstx{208, 4, MainVecBinaryMstx02<int32_t>},
        // UnaryTestParamsMstx { 208, 2, MainVecUnaryMstx03<int16_t> },
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx04<int16_t>},
        // UnaryTestParamsMstx { 208, 2, MainVecUnaryMstx05<half> },
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx06<int32_t>},
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx07<int16_t>}, UnaryTestParamsMstx{208, 2, MainVecUnaryMstx08<half>},
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx09<half>}, UnaryTestParamsMstx{208, 2, MainVecUnaryMstx10<int16_t>},
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx11<uint32_t>},
        UnaryTestParamsMstx{208, 2, MainVecUnaryMstx12<int16_t>}));

TEST_P(UnarySimpleMstxTestsuite, UnarySimpleMstxTestCase)
{
    auto param = GetParam();
    uint8_t srcGm[param.dataSize * param.dataBitSize];
    uint8_t dstGm[param.dataSize * param.dataBitSize];

    param.CalFunc(srcGm, dstGm, param.dataSize);
    for (int32_t i = 0; i < param.dataSize; i++) {
        EXPECT_EQ(dstGm[i], 0x00);
    }
}
