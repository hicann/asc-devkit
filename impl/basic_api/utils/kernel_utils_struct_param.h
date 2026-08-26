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
 * \file kernel_utils_struct_param.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_utils_struct_param.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_PARAM_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_STRUCT_PARAM_H
#define ASCENDC_MODULE_UTILS_STRUCT_PARAM_H
#include "kernel_utils_mode.h"

namespace AscendC {
struct ReduceRepeatParams {
    __aicore__ ReduceRepeatParams()
    {
        highMask = FULL_MASK;
        lowMask = FULL_MASK;
        repeatTimes = 0;
        dstRepStride = DEFAULT_REDUCE_DST_REP_STRIDE; // dst Stride Unit is 2B(fp16)/4B(fp32)
        srcBlkStride = DEFAULT_BLK_STRIDE;
        srcRepStride = DEFAULT_REPEAT_STRIDE; // src Stride Unit is 32B
    }

    __aicore__ ReduceRepeatParams(
        const int32_t mask, const int32_t repeatTimesIn, const int32_t dstRepStrideIn, const int32_t srcBlkStrideIn,
        const int32_t srcRepStrideIn)
    {
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3113) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        normalMask = mask;
        maskMode = 1;
#else
        if (mask == HALF_MASK_LEN) {
            highMask = 0;
            lowMask = FULL_MASK;
        } else if (mask == HALF_MASK_LEN * DOUBLE_FACTOR) {
            highMask = FULL_MASK;
            lowMask = FULL_MASK;
        } else {
            highMask = (mask > HALF_MASK_LEN) ?
                           (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(mask - HALF_MASK_LEN)) - 1) :
                           0;
            lowMask =
                (mask > HALF_MASK_LEN) ? FULL_MASK : (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(mask)) - 1);
        }
#endif
        repeatTimes = repeatTimesIn;
        dstRepStride = dstRepStrideIn;
        srcBlkStride = srcBlkStrideIn;
        srcRepStride = srcRepStrideIn;
    }

    __aicore__ ReduceRepeatParams(
        const uint64_t mask[2], const int32_t repeatTimesIn, const int32_t dstRepStrideIn, const int32_t srcBlkStrideIn,
        const int32_t srcRepStrideIn)
    {
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3113) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        bitMask[0] = mask[0];
        bitMask[1] = mask[1];
#else
        highMask = mask[1];
        lowMask = mask[0];
#endif
        repeatTimes = repeatTimesIn;
        dstRepStride = dstRepStrideIn;
        srcBlkStride = srcBlkStrideIn;
        srcRepStride = srcRepStrideIn;
    }

    uint64_t highMask = 0;
    uint64_t lowMask = 0;
    uint64_t bitMask[2] = {0, 0};
    int32_t normalMask = 0;
    int32_t maskMode = 0;
    int32_t repeatTimes = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
};

struct DumpMessageHead {
    __aicore__ DumpMessageHead()
    {
        type = 0;
        length = 0;
        addr = 0;
        dataType = 0;
        desc = 0;
        bufferId = 0;
        position = 0;
        dumpSize = 0;
    }

    __aicore__ DumpMessageHead(
        uint32_t typeIn, uint32_t lengthIn, uint32_t addrIn, uint32_t dataTypeIn, uint32_t descIn, uint32_t bufferIdIn,
        uint32_t positionIn, uint32_t dumpSizeIn)
    {
        type = typeIn;
        length = lengthIn;
        addr = addrIn;
        dataType = dataTypeIn;
        desc = descIn;
        bufferId = bufferIdIn;
        position = positionIn;
        dumpSize = dumpSizeIn;
    }

    uint32_t type = 0; // Dump Type 1:DumpScalar(DUMP_SCALAR), 2:DumpTensor (DUMP_TENSOR)
    uint32_t length = 0;
    uint32_t addr = 0;     // Dumptensor address, DumpScalar:0
    uint32_t dataType = 0; // data type: int32_t/half/...
    uint32_t desc = 0;     // for usr to add info or tag
    uint32_t bufferId = 0; // DumpScalar: Blockid, DumpTensor: UB adddr ()
    uint32_t position = 0; // DumpScalar: 0: MIX, 1: AIC 2: AIV; DumpTensor: 1:UB, 2:L1
    uint32_t dumpSize = 0; // actual dump size
};

struct DumpShapeMessageHead {
    __aicore__ DumpShapeMessageHead()
    {
        dim = 0;
        rsv = 0;
        for (uint32_t idx = 0; idx < K_MAX_SHAPE_DIM; ++idx) {
            shape[idx] = 0;
        }
    }

    __aicore__ DumpShapeMessageHead(uint32_t dimIn, uint32_t shapeIn[], uint32_t rsvIn = 0)
    {
        ASCENDC_ASSERT((dimIn <= K_MAX_SHAPE_DIM), {
            KERNEL_LOG(KERNEL_ERROR, "dim is %u, which should be less than %d", dimIn, K_MAX_SHAPE_DIM);
        });
        dim = dimIn;
        rsv = rsvIn;
        for (uint32_t idx = 0; idx < K_MAX_SHAPE_DIM; ++idx) {
            if (idx < dim) {
                shape[idx] = shapeIn[idx];
            } else {
                shape[idx] = 0;
            }
        }
    }

    uint32_t dim = 0;
    uint32_t shape[K_MAX_SHAPE_DIM];
    uint32_t rsv = 0; // reserved information
};

// dump TLV for L1 to UB
struct L12UBDumpCtrlMessage {
    volatile uint32_t enDumpFlag = 0;
    volatile uint32_t sig = 0;
    volatile uint32_t len = 0;
};

struct ProposalIntriParams {
    __aicore__ ProposalIntriParams()
    {
        repeat = 0;
        modeNumber = 0;
    }

    __aicore__ ProposalIntriParams(const int32_t repeatTime, const int32_t modeNumberIn)
    {
        repeat = repeatTime;       // [1,255]
        modeNumber = modeNumberIn; // modeNumberIn: 0: x1, 1: y1, 2: x2, 3: y2, 4: score, 5:label
    }

    int32_t repeat = 0;
    int32_t modeNumber = 0;
};

struct BlockInfo {
    __aicore__ BlockInfo()
    {
        len = 0;
        core = 0;
        blockNum = 0;
        dumpOffset = 0;
        magic = 0;
        rsv = 0;
        dumpAddr = 0;
    }
    __aicore__ BlockInfo(
        uint64_t dumpAddrIn, uint32_t lenIn, uint32_t coreIn, uint32_t blockNumIn, uint32_t dumpOffsetIn,
        uint32_t magicIn, uint32_t rsvIn)
    {
        len = lenIn;
        core = coreIn;
        blockNum = blockNumIn;
        dumpOffset = dumpOffsetIn;
        magic = magicIn;
        rsv = rsvIn;
        dumpAddr = dumpAddrIn;
    }
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    volatile uint32_t len = 0;
    volatile uint32_t core = 0;       // current core id
    volatile uint32_t blockNum = 0;   // total core num
    volatile uint32_t dumpOffset = 0; // size used by current core
    volatile uint32_t magic = 0;      // magic number
    volatile uint32_t rsv = 0;
    volatile uint64_t dumpAddr = 0; // start addr of dump
#else
    uint32_t len = 0;
    uint32_t core = 0;       // current core id
    uint32_t blockNum = 0;   // total core num
    uint32_t dumpOffset = 0; // size used by current core
    uint32_t magic = 0;      // magic number
    uint32_t rsv = 0;
    uint64_t dumpAddr = 0; // start addr of dump
#endif
};

struct BlockRingBufInfo {
    uint32_t length = 0U;      // total size per block (include head and r/w info)
    uint32_t coreId = 0U;      // current core id
    uint32_t blockNum = 0U;    // total core num
    uint32_t ringBufLen = 0U;  // fifo buff size (print tlv storage)
    uint16_t magic = 0U;       // magic number
    uint16_t flag = 0U;        // 0: aic, 1: aiv, 2: simt
    uint32_t rsv = 0U;         // reserve
    uint64_t ringBufAddr = 0U; // start addr of fifo buff
    uint32_t resvMem[6];       // reserved
};

struct RingBufWriteInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFI); // DumpType = DUMP_BUFI
    uint32_t length = 0U;                                       // u64 + u64
    uint64_t bufOffset = 0U;                                    // the offset of write addr relative to ringBufAddr
    uint64_t packIdx = 0U;                                      // print pack counter
};

struct RingBufReadInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFO); // DumpType = DUMP_BUFO
    uint32_t length = 0U;                                       // u64 + u64
    uint64_t bufOffset = 0U;                                    // the offset of read addr relative to ringBufAddr
    uint64_t resv = 0U;
};

struct SkipTlvInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SKIP); // DumpType = DUMP_SKIP
    uint32_t length = 0U;
};

struct PrintTlvInfoHead {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SCALAR);
    uint32_t length = 0U;
    uint32_t blockIdx = 0U;  // blockIdx
    uint32_t resv = 0U;      // reserved
    uint64_t fmtOffset = 0U; // offset of fmt string from the start of fmtOffset addr
};

struct DumpTensorTlvInfoHead {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_TENSOR); // DumpType = DUMP_TENSOR
    uint32_t length = 0U;            // Length of (addr dataType desc bufferId position dumpSize dumpData align)
    uint32_t tensorAddr = 0U;        // Address of Tensor
    uint32_t dataType = 0U;          // Data type: int32_t/half/...
    uint32_t desc = 0U;              // Usr id
    uint32_t bufferId = 0U;          // 0
    uint16_t position = 0U;          // Position GM,UB,L1,L0C
    uint16_t blockIdx = 0U;          // blockIdx
    uint32_t dim = 0U;               // shape dim
    uint32_t shape[K_MAX_SHAPE_DIM]; // dim <= 8
    uint32_t resv1 = 0U;             // reserved
    uint32_t dumpSize = 0U;          // Length of dumpData
                                     // dumpData[dumpSize], Tensor data
};

struct DumpShapeTlvInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SHAPE); // DumpType = DUMP_SHAPE
    uint32_t length = 0U;                                        // Length of (dim shape rsv)
    uint32_t dim = 0U;                                           // shapeInfo.dim
    uint32_t shape[K_MAX_SHAPE_DIM];                             // dim <= 8
    uint32_t resv;
};

struct DumpMeta {
    uint32_t typeId = static_cast<uint32_t>(DumpType::DUMP_META);
    uint32_t len = 8;
    uint16_t numBlocks = 0;
    uint8_t coreType = 0;
    uint8_t taskRation = 0;
    uint32_t rsv = 0;
};

struct SimtDumpMeta {
    uint32_t typeId = static_cast<uint32_t>(DumpType::DUMP_META);
    uint32_t len = 8;
    uint32_t threadId = 0;
    uint32_t rsv = 0;
};

struct DumpTimeStamp {
    uint32_t typeId = static_cast<uint32_t>(DumpType::DUMP_TIME_STAMP);
    uint32_t len = 24;
    uint32_t descId = 0;
    uint32_t rsv = 0;
    uint64_t systemCycle = 0;
    uint64_t pcPtr = 0;
};
} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_STRUCT_PARAM_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_PARAM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_PARAM_H__
#endif
