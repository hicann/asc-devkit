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
 * \file asc_debug_types.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_DEBUG_TYPES_H
#define IMPL_UTILS_DEBUG_ASC_DEBUG_TYPES_H

#ifndef ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE
// Reserved UB space is 2K in total. 20 bytes for BlockVFBufInfo header, leaving 2028 bytes for buffer.
#define ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE 2028
#endif

#define ASCENDC_SIMD_VF_MAGIC_NUMBER 0xF0A00B0F

struct BlockVFBufInfo {
    uint32_t magic = ASCENDC_SIMD_VF_MAGIC_NUMBER;
    uint32_t length = ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE;
    uint32_t writeLen = 0;
    uint16_t pidx = 0;
    uint8_t flag = 0; // 0: normal, nonzero: overflow
    uint8_t resv1 = 0;
    uint16_t blockIdx = 0;
    uint16_t resv2 = 0;
    uint8_t buffer[ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE];
};

namespace __asc_aicore {

enum class DumpTensorDataType : uint32_t {
    ACL_FLOAT = 0,
    ACL_FLOAT16 = 1,
    ACL_INT8 = 2,
    ACL_INT32 = 3,
    ACL_UINT8 = 4,
    ACL_INT16 = 6,
    ACL_UINT16 = 7,
    ACL_UINT32 = 8,
    ACL_INT64 = 9,
    ACL_UINT64 = 10,
    ACL_DOUBLE = 11,
    ACL_BOOL = 12,
    ACL_STRING = 13,
    ACL_COMPLEX64 = 16,
    ACL_COMPLEX128 = 17,
    ACL_BF16 = 27,
    ACL_INT4 = 29,
    ACL_UINT1 = 30,
    ACL_COMPLEX32 = 33,
    ACL_HIFLOAT8 = 34,
    ACL_FLOAT8_E5M2 = 35,
    ACL_FLOAT8_E4M3FN = 36,
    ACL_FLOAT8_E8M0 = 37,
    ACL_FLOAT6_E3M2 = 38,
    ACL_FLOAT6_E2M3 = 39,
    ACL_FLOAT4_E2M1 = 40,
    ACL_FLOAT4_E1M2 = 41,
    ACL_MAX = 42,
};

enum class DumpType : uint8_t {
    DUMP_DEFAULT = 0,
    DUMP_SCALAR,
    DUMP_TENSOR,
    DUMP_SHAPE,
    DUMP_ASSERT,
    DUMP_META,
    DUMP_TIME_STAMP,
    DUMP_SIMT,
    DUMP_BUFI,
    DUMP_BUFO,
    DUMP_SKIP
};

struct DebugBlockHeadInfo {
    uint32_t length = 0U;        // total size per block (include head and r/w info)
    uint32_t coreId = 0U;        // current core id
    uint32_t blockNum = 0U;      // total core num
    uint32_t ringBufLen = 0U;    // fifo buff size (print tlv storage)
    uint16_t magic = 0U;         // magic number
    uint16_t flag = 0U;          // 0: aic, 1: aiv, 2: simt
    uint32_t rsv = 0U;           // reserve
    uint64_t ringBufAddr = 0U;   // start addr of fifo buff
    uint64_t debugBusAddr = 0U;  // debug-bus base addr for L1/L0A/L0B dump
    uint32_t resvMem[4];         // reserved
};

struct DebugBlockWriteInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFI); // DumpType = DUMP_BUFI
    uint32_t length = 0U;       // u64 + u64
    uint64_t bufOffset = 0U;    // the offset of write addr relative to ringBufAddr
    uint64_t packIdx = 0U;      // print pack counter
};

struct DebugBlockReadInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFO); // DumpType = DUMP_BUFO
    uint32_t length = 0U;       // u64 + u64
    uint64_t bufOffset = 0U;    // the offset of read addr relative to ringBufAddr
    uint64_t resv = 0U;
};

struct SkipTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SKIP); // DumpType = DUMP_SKIP
    uint32_t length = 0U;
};

struct PrintTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SCALAR);
    uint32_t length = 0U;
    uint32_t blockIdx = 0U;             // blockIdx
    uint32_t resv = 0U;                 // reserved
    uint64_t fmtOffset = 0U;            // offset of fmt string from the start of fmtOffset addr
};

struct DumpTensorTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_TENSOR); // DumpType = DUMP_TENSOR
    uint32_t length = 0U;            // Length of (addr dataType desc bufferId position dumpSize dumpData align)
    uint32_t tensorAddr = 0U;        // Address of Tensor
    uint32_t dataType = 0U;          // Data type: int32_t/half/...
    uint32_t desc = 0U;              // Usr id
    uint32_t bufferId = 0U;          // 0
    uint16_t position = 0U;          // Position GM, UB, L1, L0A, L0B, L0C
    uint16_t blockIdx = 0U;          // blockIdx
    uint32_t dim = 0U;               // shape dim
    uint32_t shape[8];              // dim <= 8
    uint32_t resv1 = 0U;             // reserved
    uint32_t dumpSize = 0U;          // Length of dumpData
                                     // dumpData[dumpSize], Tensor data
};

struct DumpShapeTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SHAPE); // DumpType = DUMP_SHAPE
    uint32_t length = 0U;           // Length of (dim shape rsv)
    uint32_t dim = 0U;                  // shapeInfo.dim
    uint32_t shape[8];              // dim <= 8
    uint32_t resv;
};

struct TimeStampTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_TIME_STAMP); // DumpType = DUMP_TIME_STAMP
    uint32_t length = 0U;      // Length of (descId resv cycle pc entry)
    uint32_t descId = 0U;          // Usr id
    uint16_t blockIdx = 0U;        // blockIdx
    uint16_t resv = 0U;            // reserved
    uint64_t cycle = 0U;           // system cycle
    uint64_t pc = 0U;              // get pc
    uint64_t entry = 0U;           // entry system cycle
    uint32_t resvMem[2];          // reserved
};

#ifdef ASCENDC_TRACE_ON
enum class TraceId : uint32_t {
    KFC_CLIENT_POST_MSG = 0x7001,
    KFC_CLIENT_REV_MSG_GM = 0x7002,
    KFC_CLIENT_REV_MSG_UB = 0x7003,
    KFC_SERVER_RUN = 0x7101,
    KFC_SERVER_REV_MSG = 0x7102,
    KFC_SERVER_PROCESS_MSG = 0x7103,
    MatMul_PROCESS_MSG = 0x8001,
    MatMul_CALC,
    Conv = 0x8101,
    DropOut = 0x8201,
    SoftMax = 0x8301,
    SoftmaxGrad,
    SoftmaxFlash,
    SoftmaxFlashV2,
    LogSoftMax,
    SoftmaxFlashV3,
    LayerNorm = 0x8401,
    LayerNormGrad,
    LayerNormGradBeta,
    Pad = 0x8501,
    UnPad,
    BroadCast = 0x8601,
};
#endif

struct BlockRingBufInfo {
    uint32_t length = 0U;      // total size per block (include head and r/w info)
    uint32_t coreId = 0U;      // current core id
    uint32_t blockNum = 0U;    // total core num
    uint32_t ringBufLen = 0U;  // fifo buff size (print tlv storage)
    uint16_t magic = 0U;       // magic number
    uint16_t flag = 0U;        // 0: simd, 1: simt
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
} // namespace __asc_aicore

namespace __asc_simd_vf {
enum class DumpTensorDataType : uint32_t {
    ACL_FLOAT = 0,
    ACL_FLOAT16 = 1,
    ACL_INT8 = 2,
    ACL_INT32 = 3,
    ACL_UINT8 = 4,
    ACL_INT16 = 6,
    ACL_UINT16 = 7,
    ACL_UINT32 = 8,
    ACL_INT64 = 9,
    ACL_UINT64 = 10,
    ACL_DOUBLE = 11,
    ACL_BOOL = 12,
    ACL_STRING = 13,
    ACL_COMPLEX64 = 16,
    ACL_COMPLEX128 = 17,
    ACL_BF16 = 27,
    ACL_INT4 = 29,
    ACL_UINT1 = 30,
    ACL_COMPLEX32 = 33,
    ACL_HIFLOAT8 = 34,
    ACL_FLOAT8_E5M2 = 35,
    ACL_FLOAT8_E4M3FN = 36,
    ACL_FLOAT8_E8M0 = 37,
    ACL_FLOAT6_E3M2 = 38,
    ACL_FLOAT6_E2M3 = 39,
    ACL_FLOAT4_E2M1 = 40,
    ACL_FLOAT4_E1M2 = 41,
    ACL_MAX = 42,
};

enum class DumpType : uint8_t {
    DUMP_DEFAULT = 0,
    DUMP_SCALAR,
    DUMP_TENSOR,
    DUMP_SHAPE,
    DUMP_ASSERT,
    DUMP_META,
    DUMP_TIME_STAMP,
    DUMP_SIMT,
    DUMP_BUFI,
    DUMP_BUFO,
    DUMP_SKIP
};

struct BlockRingBufInfo {
    uint32_t length = 0U;      // total size per block (include head and r/w info)
    uint32_t coreId = 0U;      // current core id
    uint32_t blockNum = 0U;    // total core num
    uint32_t ringBufLen = 0U;  // fifo buff size (print tlv storage)
    uint16_t magic = 0U;       // magic number
    uint16_t flag = 0U;        // 0: simd, 1: simt
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

struct PrintTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_SCALAR);
    uint32_t length = 0U;
    uint32_t blockIdx = 0U;             // blockIdx
    uint32_t resv = 0U;                 // reserved
    uint64_t fmtOffset = 0U;            // offset of fmt string from the start of fmtOffset addr
};

struct DumpTensorTlv {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_TENSOR); // DumpType = DUMP_TENSOR
    uint32_t length = 0U;            // Length of (addr dataType desc bufferId position dumpSize dumpData align)
    uint32_t tensorAddr = 0U;        // Address of Tensor
    uint32_t dataType = 0U;          // Data type: int32_t/half/...
    uint32_t desc = 0U;              // Usr id
    uint32_t bufferId = 0U;          // 0
    uint16_t position = 0U;          // Position GM,UB,L1,L0C
    uint16_t blockIdx = 0U;          // blockIdx
    uint32_t dim = 0U;               // shape dim
    uint32_t shape[8];              // dim <= 8
    uint32_t resv1 = 0U;             // reserved
    uint32_t dumpSize = 0U;          // Length of dumpData
                                     // dumpData[dumpSize], Tensor data
};
} // namespace __asc_simd_vf

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_TYPES__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_TYPES__
#endif

#endif // IMPL_UTILS_DEBUG_ASC_DEBUG_TYPES_H