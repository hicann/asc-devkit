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
 * \file kernel_operator_vec_vconv_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m300/kernel_operator_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "../../../include/basic_api/kernel_struct_vdeq.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M300_STAGE_CAST_DECLARE_FUSED_REGS(VectorType, VectorType2, sregLower, mask) \
    VectorType vreg0;                                                                    \
    VectorType vreg1;                                                                    \
    VectorType vreg2;                                                                    \
    VectorType vreg3;                                                                    \
    VectorType2 vreg4;                                                                   \
    uint32_t sregLower = static_cast<uint32_t>(mask)

#define ASC_M300_STAGE_CAST_FUSED_B16_LOOP(                                                                     \
    repeatParams, ElementCount, ElementCount2, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, vreg4, \
    VectorType3, VectorType4, dst, pregLower)                                                                   \
    uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                         \
    uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                         \
    uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                          \
    uint32_t strideOffset0 = static_cast<uint32_t>(repeatParams.src0RepStride * 256 / ElementCount);            \
    uint32_t strideOffset1 = static_cast<uint32_t>(repeatParams.src1RepStride * 256 / ElementCount);            \
    uint32_t strideOffset2 = static_cast<uint32_t>(repeatParams.dstRepStride * 256 / ElementCount2);            \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                          \
        mem_bar(VST_VLD);                                                                                       \
        vsldb(vreg0, src0 + i * strideOffset0, strideConfig0, preg);                                            \
        vsldb(vreg1, src1 + i * strideOffset1, strideConfig1, preg);                                            \
        vadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                          \
        vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                                \
        vcvt(vreg4, vreg3, preg, ROUND_R, RS_ENABLE, PART_EVEN);                                                \
        vpack((VectorType3&)vreg4, (VectorType4&)vreg4, LOWER, MODE_ZEROING);                                   \
        vsstb(vreg4, dst + i * strideOffset2, strideConfig2, pregLower);                                        \
    }

#define ASC_M300_STAGE_CAST_PREPARE_FUSED_BITMASK(VectorType, VectorType2, MaskIntrinsic) \
    VectorType vreg0;                                                                     \
    VectorType vreg1;                                                                     \
    VectorType vreg2;                                                                     \
    VectorType vreg3;                                                                     \
    VectorType2 vreg4;                                                                    \
    vector_bool pregLower;                                                                \
    vector_bool preg;                                                                     \
    preg = MaskIntrinsic();                                                               \
    ppack(pregLower, preg, LOWER)

#define ASC_M300_STAGE_CAST_FUSED_B32_LOOP(                                                                     \
    repeatParams, ElementCount, ElementCount2, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, vreg4, \
    VectorType3, VectorType4, dst, pregLower)                                                                   \
    uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                         \
    uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                         \
    uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                          \
    uint32_t strideOffset0 = static_cast<uint32_t>(repeatParams.src0RepStride * 256 / ElementCount);            \
    uint32_t strideOffset1 = static_cast<uint32_t>(repeatParams.src1RepStride * 256 / ElementCount);            \
    uint32_t strideOffset2 = static_cast<uint32_t>(repeatParams.dstRepStride * 256 / ElementCount2);            \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                          \
        mem_bar(VST_VLD);                                                                                       \
        vsldb(vreg0, src0 + i * strideOffset0, strideConfig0, preg);                                            \
        vsldb(vreg1, src1 + i * strideOffset1, strideConfig1, preg);                                            \
        vsub(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                          \
        vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                                \
        vcvt(vreg4, vreg3, preg, ROUND_R, RS_ENABLE, PART_EVEN);                                                \
        vpack((VectorType3&)vreg4, (VectorType4&)vreg4, LOWER, MODE_ZEROING);                                   \
        vsstb(vreg4, dst + i * strideOffset2, strideConfig2, pregLower);                                        \
    }

namespace AscendC {
#define VCVT_U8_TO_F16(roundMode) vcvt(vreg1, vreg0, preg, PART_EVEN)
#define VCVT_S8_TO_F16(roundMode) vcvt(vreg1, vreg0, preg, PART_EVEN)
#define VCVT_F16_TO_F32(roundMode) vcvt(vreg1, vreg0, preg, PART_EVEN)
#define VCVT_F16_TO_S32(roundMode) vcvt(vreg1, vreg0, preg, roundMode, PART_EVEN)
#define VCVT_BF16_TO_F32(roundMode) vcvt(vreg1, vreg0, preg, PART_EVEN)
#define VCVT_BF16_TO_S32(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_S16_TO_F32(roundMode) vcvt(vreg1, vreg0, preg, PART_EVEN)
#define VCVT_F32_TO_S64(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_DISABLE, PART_EVEN)
#define VCVT_S32_TO_S64(roundMode) vcvt(vreg1, vreg0, preg, PART_EVEN)

#define VCVT_F16_TO_U8(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_F16_TO_S8(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_F32_TO_F16(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_F32_TO_BF16(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_F32_TO_S16(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_S32_TO_F16(roundMode)            \
    vector_f32 vregTmpF32;                    \
    vcvt(vregTmpF32, vreg0, preg, roundMode); \
    vcvt(vreg1, vregTmpF32, preg, roundMode, RS_ENABLE, PART_EVEN)
#define VCVT_S32_TO_S16(roundMode) vcvt(vreg1, vreg0, preg, RS_ENABLE, PART_EVEN)
#define VCVT_S64_TO_F32(roundMode) vcvt(vreg1, vreg0, preg, roundMode, PART_EVEN)
#define VCVT_S64_TO_S32(roundMode) vcvt(vreg1, vreg0, preg, RS_ENABLE, PART_EVEN)

#define VCVT_F16_TO_S16(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE)
#define VCVT_S16_TO_F16(roundMode) vcvt(vreg1, vreg0, preg, roundMode)
#define VCVT_F32_TO_F32(roundMode) vtrc(vreg1, vreg0, roundMode, preg)
#define VCVT_F32_TO_S32(roundMode) vcvt(vreg1, vreg0, preg, roundMode, RS_ENABLE)
#define VCVT_S32_TO_F32(roundMode) vcvt(vreg1, vreg0, preg, roundMode)

// plt doesn't support b64
#define GEN_PLT_INSTR_B8(preg, sreg) preg = plt_b8(sreg, POST_UPDATE)
#define GEN_PLT_INSTR_B16(preg, sreg) preg = plt_b16(sreg, POST_UPDATE)
#define GEN_PLT_INSTR_B32(preg, sreg) preg = plt_b32(sreg, POST_UPDATE)
// use plt_b32 to replace b64, twice the value required
#define GEN_PLT_INSTR_B64(preg, sreg) preg = plt_b32(sreg, POST_UPDATE)

// vsts doesn't support NORM_B64
#define GEN_VSTS_INSTR_B8(vreg, base, offset, dist, preg) vsts(vreg, base, offset, dist, preg)
#define GEN_VSTS_INSTR_B16(vreg, base, offset, dist, preg) vsts(vreg, base, offset, dist, preg)
#define GEN_VSTS_INSTR_B32(vreg, base, offset, dist, preg) vsts(vreg, base, offset, dist, preg)
// use vsts_b32 to replace b64. preg is from LV2_INIT_32, can be directly used for b32
#define GEN_VSTS_INSTR_B64(vreg, base, offset, dist, preg) \
    vsts((vector_s32&)vreg, (__ubuf__ int32_t*&)base, offset, NORM_B32, preg)

// deal 128 elements each repeat. (b8->b16 / b16->b8 / b16->b16, depends on the larger data type.)
#define LV2_INIT_128(repeatSize)                            \
    uint32_t sregLower = static_cast<uint32_t>(repeatSize); \
    uint32_t sregUpper = static_cast<uint32_t>(repeatSize)

// deal 64 elements each repeat. (b16->b32 / b32->b16 / b32->b32, depends on the larger data type.)
#define LV2_INIT_64(repeatSize)                             \
    uint32_t sregLower = static_cast<uint32_t>(repeatSize); \
    uint32_t sregUpper = static_cast<uint32_t>(repeatSize)

// deal 32 elements each repeat. (b32->b64 / b64->b32, depends on the larger data type.)
#define LV2_INIT_32(repeatSize)                                 \
    uint32_t sregLower = static_cast<uint32_t>(repeatSize);     \
    uint32_t sregUpper = static_cast<uint32_t>(repeatSize) * 2; \
    sregPlt = sregPlt * 2

#define LV2_LOAD_UPPER(srcBits, dstBits)     \
    GEN_PLT_INSTR_B##dstBits(preg, sregPlt); \
    vlds(vreg0, src, i* sregLower, UNPK_B##srcBits)

#define LV2_STORE_UPPER(srcBits, dstBits) GEN_VSTS_INSTR_B##dstBits(vreg1, dst, i* sregUpper, NORM_B##dstBits, preg)

#define LV2_LOAD_LOWER(srcBits, dstBits)     \
    GEN_PLT_INSTR_B##srcBits(preg, sregPlt); \
    vlds(vreg0, src, i* sregLower, NORM)

#define LV2_STORE_LOWER(srcBits, dstBits) GEN_VSTS_INSTR_B##dstBits(vreg1, dst, i* sregLower, PK_B##srcBits, preg)

#define LV2_LOAD_EQUAL(srcBits, dstBits)     \
    GEN_PLT_INSTR_B##dstBits(preg, sregPlt); \
    vlds(vreg0, src, i* sregLower, NORM)

#define LV2_STORE_EQUAL(srcBits, dstBits) GEN_VSTS_INSTR_B##dstBits(vreg1, dst, i* sregLower, NORM_B##dstBits, preg)

// Cast::Level 2
#define REGISTER_CAST_LV2(                                                                                     \
    srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode, loadFunc, \
    castFunc, storeFunc)                                                                                       \
    __aicore__ inline void CastIntrinsicsImpl##roundStr(                                                       \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t count)                                    \
    {                                                                                                          \
        __VEC_SCOPE__                                                                                          \
        {                                                                                                      \
            vector_##srcTypeShort vreg0;                                                                       \
            vector_##dstTypeShort vreg1;                                                                       \
            uint32_t sregPlt = static_cast<uint32_t>(count);                                                   \
            LV2_INIT_##repeatSize(repeatSize);                                                                 \
            vector_bool preg;                                                                                  \
            uint16_t repeatTime = CeilDivision(count, repeatSize);                                             \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                 \
                loadFunc(srcBits, dstBits);                                                                    \
                castFunc(roundMode);                                                                           \
                storeFunc(srcBits, dstBits);                                                                   \
            }                                                                                                  \
        }                                                                                                      \
    }

// vsstb doesn't support b64
#define GEN_VSSTB_INSTR_B8(vreg, base, offset, config, preg) vsstb(vreg, base + offset, config, preg)
#define GEN_VSSTB_INSTR_B16(vreg, base, offset, config, preg) vsstb(vreg, base + offset, config, preg)
#define GEN_VSSTB_INSTR_B32(vreg, base, offset, config, preg) vsstb(vreg, base + offset, config, preg)
// use vsstb_b32 to replace b64, twice the offset required. preg is from GEN_PLT_INSTR_B64, can be directly used for b32
#define GEN_VSSTB_INSTR_B64(vreg, base, offset, config, preg) \
    vsstb((vector_s32&)vreg, (__ubuf__ int32_t*)base + offset * 2, config, preg)

// deal 128 elements each repeat. (b8->b16 / b16->b8 / b16->b16, depends on the larger data type.)
#define BIT_INIT_128(pregLower, pregUpper)                   \
    plds(pregUpper, ((__ubuf__ uint32_t*)maskBuf), 0, NORM); \
    punpack(pregLower, pregUpper, LOWER)

// deal 64 elements each repeat. (b16->b32 / b32->b16 / b32->b32, depends on the larger data type.)
#define BIT_INIT_64(pregLower, pregUpper)                  \
    plds(pregUpper, ((__ubuf__ uint32_t*)maskBuf), 0, US); \
    punpack(pregLower, pregUpper, LOWER)

// deal 32 elements each repeat. (b32->b64 / b64->b32, depends on the larger data type.)
#define BIT_INIT_32(pregLower, pregUpper)                  \
    plds(pregUpper, ((__ubuf__ uint32_t*)maskBuf), 0, US); \
    punpack(pregUpper, pregUpper, LOWER);                  \
    punpack(pregLower, pregUpper, LOWER)

// deal 128 elements each repeat. (b8->b16 / b16->b8 / b16->b16, depends on the larger data type.)
#define COUNT_INIT_128(pregLower, pregUpper, bits_lower, bitsUpper) \
    uint32_t sregLower = static_cast<uint32_t>(mask);               \
    uint32_t sregUpper = static_cast<uint32_t>(mask);               \
    GEN_PLT_INSTR_B##bits_lower(pregLower, sregLower);              \
    GEN_PLT_INSTR_B##bitsUpper(pregUpper, sregUpper)

// deal 64 elements each repeat. (b16->b32 / b32->b16 / b32->b32, depends on the larger data type.)
#define COUNT_INIT_64(pregLower, pregUpper, bits_lower, bitsUpper) \
    uint32_t sregLower = static_cast<uint32_t>(mask);              \
    uint32_t sregUpper = static_cast<uint32_t>(mask);              \
    GEN_PLT_INSTR_B##bits_lower(pregLower, sregLower);             \
    GEN_PLT_INSTR_B##bitsUpper(pregUpper, sregUpper)

// deal 32 elements each repeat. (b32->b64 / b64->b32, depends on the larger data type.)
#define COUNT_INIT_32(pregLower, pregUpper, bits_lower, bitsUpper) \
    uint32_t sregLower = static_cast<uint32_t>(mask);              \
    uint32_t sregUpper = static_cast<uint32_t>(mask) * 2;          \
    GEN_PLT_INSTR_B##bits_lower(pregLower, sregLower);             \
    GEN_PLT_INSTR_B##bitsUpper(pregUpper, sregUpper)

#define BIT_INIT_UPPER(srcDtypeShort, dstDtypeShort, srcBits, dstBits, repeatSize) \
    vector_bool pregLower;                                                         \
    BIT_INIT_##repeatSize(preg, pregLower);                                        \
    vector_s##srcBits vregTmp

#define COUNTER_INIT_UPPER(srcDtypeShort, dstDtypeShort, srcBits, dstBits, repeatSize) \
    vector_bool pregLower;                                                             \
    COUNT_INIT_##repeatSize(pregLower, preg, srcBits, dstBits);                        \
    vector_s##srcBits vregTmp

// vintlv doesn't support bf16
#define LV0_LOAD_UPPER(srcBits)                                      \
    vsldb(vreg0, src + i * strideOffset0, strideConfig0, pregLower); \
    vintlv((vector_s##srcBits&)vreg0, vregTmp, (vector_s##srcBits&)vreg0, vregTmp)

// vsstb doesn't support b64
#define LV0_STORE_UPPER(dstBits) GEN_VSSTB_INSTR_B##dstBits(vreg1, dst, i* strideOffset1, strideConfig1, preg)

#define BIT_INIT_LOWER(srcDtypeShort, dstDtypeShort, srcBits, dstBits, repeatSize) \
    vector_bool pregLower;                                                         \
    BIT_INIT_##repeatSize(preg, pregLower);                                        \
    vector_s##dstBits vregTmp

#define COUNTER_INIT_LOWER(srcDtypeShort, dstDtypeShort, srcBits, dstBits, repeatSize) \
    vector_bool pregLower;                                                             \
    COUNT_INIT_##repeatSize(pregLower, preg, dstBits, srcBits);                        \
    vector_s##dstBits vregTmp

#define LV0_LOAD_LOWER(srcBits) vsldb(vreg0, src + i * strideOffset0, strideConfig0, preg)

// vdintlv doesn't support bf16
#define LV0_STORE_LOWER(dstBits)                                                     \
    vdintlv((vector_s##dstBits&)vreg1, vregTmp, (vector_s##dstBits&)vreg1, vregTmp); \
    GEN_VSSTB_INSTR_B##dstBits(vreg1, dst, i* strideOffset1, strideConfig1, pregLower)

#define BIT_INIT_EQUAL(srcDtypeShort, dstDtypeShort, srcBits, dstBits, repeatSize) \
    vector_bool pregLower;                                                         \
    BIT_INIT_##repeatSize(preg, pregLower)

#define COUNTER_INIT_EQUAL(srcDtypeShort, dstDtypeShort, srcBits, dstBits, repeatSize) \
    vector_bool preg1;                                                                 \
    COUNT_INIT_##repeatSize(preg1, preg, dstBits, srcBits)

#define LV0_LOAD_EQUAL(srcBits) vsldb(vreg0, src + i * strideOffset0, strideConfig0, preg)

#define LV0_STORE_EQUAL(dstBits) GEN_VSSTB_INSTR_B##dstBits(vreg1, dst, i* strideOffset1, strideConfig1, preg)

// common vf function of Cast::Level 0
#define CAST_LV0_VF(                                                                                              \
    srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundMode, initFunc, loadFunc, castFunc, storeFunc) \
    __VEC_SCOPE__                                                                                                 \
    {                                                                                                             \
        vector_##srcTypeShort vreg0;                                                                              \
        vector_##dstTypeShort vreg1;                                                                              \
        vector_bool preg;                                                                                         \
        initFunc(srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);                                       \
        uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);                      \
        uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);                      \
        uint32_t strideOffset0 = static_cast<uint32_t>(repeatParams.srcRepStride) * 256 / srcBits;                \
        uint32_t strideOffset1 = static_cast<uint32_t>(repeatParams.dstRepStride) * 256 / dstBits;                \
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                        \
            loadFunc(srcBits);                                                                                    \
            castFunc(roundMode);                                                                                  \
            storeFunc(dstBits);                                                                                   \
        }                                                                                                         \
    }

// Cast::Level 0 - mask bit mode
#define REGISTER_CAST_BIT(                                                                                     \
    srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode, initFunc, \
    loadFunc, castFunc, storeFunc)                                                                             \
    __aicore__ inline void CastIntrinsicsImpl##roundStr(                                                       \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint64_t mask[], uint8_t repeatTime,               \
        const UnaryRepeatParams& repeatParams)                                                                 \
    {                                                                                                          \
        __ubuf__ uint64_t* maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 4);         \
        maskBuf[0] = mask[0];                                                                                  \
        maskBuf[1] = mask[1];                                                                                  \
        maskBuf[2] = 0;                                                                                        \
        maskBuf[3] = 0;                                                                                        \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));               \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                                  \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                                 \
        CAST_LV0_VF(                                                                                           \
            srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundMode, initFunc, loadFunc, castFunc, \
            storeFunc);                                                                                        \
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);                                                  \
    }

// Cast::Level 0 - mask counter mode
#define REGISTER_CAST_COUNTER(                                                                                 \
    srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode, initFunc, \
    loadFunc, castFunc, storeFunc)                                                                             \
    __aicore__ inline void CastIntrinsicsImpl##roundStr(                                                       \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint64_t mask, uint8_t repeatTime,                 \
        const UnaryRepeatParams& repeatParams)                                                                 \
    {                                                                                                          \
        CAST_LV0_VF(                                                                                           \
            srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundMode, initFunc, loadFunc, castFunc, \
            storeFunc);                                                                                        \
    }

// for dataType size: src < dst
// repeatSize: the num of elements processed in each repeat depends on the larger data type.
#define REGISTER_CAST_UPPER(                                                                                   \
    castFunc, roundStr, roundMode, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize) \
    REGISTER_CAST_LV2(                                                                                         \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        LV2_LOAD_UPPER, castFunc, LV2_STORE_UPPER);                                                            \
    REGISTER_CAST_BIT(                                                                                         \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        BIT_INIT_UPPER, LV0_LOAD_UPPER, castFunc, LV0_STORE_UPPER);                                            \
    REGISTER_CAST_COUNTER(                                                                                     \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        COUNTER_INIT_UPPER, LV0_LOAD_UPPER, castFunc, LV0_STORE_UPPER)

// for dataType size: src > dst
// repeatSize: the num of elements processed in each repeat depends on the larger data type.
#define REGISTER_CAST_LOWER(                                                                                   \
    castFunc, roundStr, roundMode, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize) \
    REGISTER_CAST_LV2(                                                                                         \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        LV2_LOAD_LOWER, castFunc, LV2_STORE_LOWER);                                                            \
    REGISTER_CAST_BIT(                                                                                         \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        BIT_INIT_LOWER, LV0_LOAD_LOWER, castFunc, LV0_STORE_LOWER);                                            \
    REGISTER_CAST_COUNTER(                                                                                     \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        COUNTER_INIT_LOWER, LV0_LOAD_LOWER, castFunc, LV0_STORE_LOWER)

// for dataType size: src == dst
// repeatSize: the num of elements processed in each repeat depends on the larger data type.
#define REGISTER_CAST_EQUAL(                                                                                   \
    castFunc, roundStr, roundMode, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize) \
    REGISTER_CAST_LV2(                                                                                         \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        LV2_LOAD_EQUAL, castFunc, LV2_STORE_EQUAL);                                                            \
    REGISTER_CAST_BIT(                                                                                         \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        BIT_INIT_EQUAL, LV0_LOAD_EQUAL, castFunc, LV0_STORE_EQUAL);                                            \
    REGISTER_CAST_COUNTER(                                                                                     \
        srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize, roundStr, roundMode,       \
        COUNTER_INIT_EQUAL, LV0_LOAD_EQUAL, castFunc, LV0_STORE_EQUAL)

// Cast::Level 2
#define REGISTER_CAST_LV2_NOT_SUPPORTED(roundStr, srcType, dstType)                                       \
    __aicore__ inline void CastIntrinsicsImpl##roundStr(                                                  \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t count)                               \
    {                                                                                                     \
        ASCENDC_ASSERT((false), {                                                                         \
            KERNEL_LOG(KERNEL_ERROR, "roundStr from srcType to dstType not supported on current device"); \
        });                                                                                               \
    }

// Cast::Level 0 - mask counter mode
#define REGISTER_CAST_COUNTER_NOT_SUPPORTED(roundStr, srcType, dstType)                                   \
    __aicore__ inline void CastIntrinsicsImpl##roundStr(                                                  \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint64_t mask, uint8_t repeatTime,            \
        const UnaryRepeatParams& repeatParams)                                                            \
    {                                                                                                     \
        ASCENDC_ASSERT((false), {                                                                         \
            KERNEL_LOG(KERNEL_ERROR, "roundStr from srcType to dstType not supported on current device"); \
        });                                                                                               \
    }

// Cast::Level 0 - mask bit mode
#define REGISTER_CAST_BIT_NOT_SUPPORTED(roundStr, srcType, dstType)                                       \
    __aicore__ inline void CastIntrinsicsImpl##roundStr(                                                  \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint64_t mask[], uint8_t repeatTime,          \
        const UnaryRepeatParams& repeatParams)                                                            \
    {                                                                                                     \
        ASCENDC_ASSERT((false), {                                                                         \
            KERNEL_LOG(KERNEL_ERROR, "roundStr from srcType to dstType not supported on current device"); \
        });                                                                                               \
    }

#define REGISTER_CAST_NOT_SUPPORTED(roundStr, srcType, dstType)      \
    REGISTER_CAST_LV2_NOT_SUPPORTED(roundStr, srcType, dstType);     \
    REGISTER_CAST_COUNTER_NOT_SUPPORTED(roundStr, srcType, dstType); \
    REGISTER_CAST_BIT_NOT_SUPPORTED(roundStr, srcType, dstType)

#define REGISTER_CAST_ROUND_MODE_NOT_SUPPORTED(srcType, dstType) \
    REGISTER_CAST_NOT_SUPPORTED(CastRint, srcType, dstType);     \
    REGISTER_CAST_NOT_SUPPORTED(CastRound, srcType, dstType);    \
    REGISTER_CAST_NOT_SUPPORTED(CastFloor, srcType, dstType);    \
    REGISTER_CAST_NOT_SUPPORTED(CastCeil, srcType, dstType);     \
    REGISTER_CAST_NOT_SUPPORTED(CastTrunc, srcType, dstType);    \
    REGISTER_CAST_NOT_SUPPORTED(CastOdd, srcType, dstType)

#define REGISTER_CAST_NONE_MODE_NOT_SUPPORTED(srcType, dstType) REGISTER_CAST_NOT_SUPPORTED(CastNone, srcType, dstType)

#define REGISTER_CAST_ODD_MODE_NOT_SUPPORTED(srcType, dstType) REGISTER_CAST_NOT_SUPPORTED(CastOdd, srcType, dstType)

// support CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC, CAST_NONE
#define REGISTER_CAST_ROUND_NONE(                                                                                  \
    sizeMode, castFunc, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize)                \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastRound, ROUND_A, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastRint, ROUND_R, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastFloor, ROUND_F, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastTrunc, ROUND_Z, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastCeil, ROUND_C, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastNone, ROUND_R, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_ODD_MODE_NOT_SUPPORTED(srcType, dstType)

// support CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC, CAST_NONE, CAST_ODD
#define REGISTER_CAST_ALL(                                                                                         \
    sizeMode, castFunc, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize)                \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastCeil, ROUND_C, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastRound, ROUND_A, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastOdd, ROUND_O, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);   \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastFloor, ROUND_F, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastRint, ROUND_R, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastTrunc, ROUND_Z, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastNone, ROUND_R, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize)

// support CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC
// not support CAST_NONE
#define REGISTER_CAST_ONLY_ROUND(                                                                                  \
    sizeMode, castFunc, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize)                \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastRint, ROUND_R, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastRound, ROUND_A, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastFloor, ROUND_F, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastCeil, ROUND_C, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize);  \
    REGISTER_CAST_##sizeMode(                                                                                      \
        castFunc, CastTrunc, ROUND_Z, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize); \
    REGISTER_CAST_NONE_MODE_NOT_SUPPORTED(srcType, dstType);                                                       \
    REGISTER_CAST_ODD_MODE_NOT_SUPPORTED(srcType, dstType)

// support CAST_NONE
// not support CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC
#define REGISTER_CAST_ONLY_NONE(                                                                    \
    sizeMode, castFunc, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize) \
    REGISTER_CAST_ROUND_MODE_NOT_SUPPORTED(srcType, dstType);                                       \
    REGISTER_CAST_##sizeMode(                                                                       \
        castFunc, CastNone, ROUND_R, srcType, dstType, srcTypeShort, dstTypeShort, srcBits, dstBits, repeatSize)

REGISTER_CAST_ONLY_NONE(UPPER, VCVT_U8_TO_F16, uint8_t, half, u8, f16, 8, 16, 128);
REGISTER_CAST_ONLY_NONE(UPPER, VCVT_S8_TO_F16, int8_t, half, s8, f16, 8, 16, 128);
REGISTER_CAST_ONLY_NONE(UPPER, VCVT_F16_TO_F32, half, float, f16, f32, 16, 32, 64);
REGISTER_CAST_ONLY_ROUND(UPPER, VCVT_F16_TO_S32, half, int32_t, f16, s32, 16, 32, 64);
REGISTER_CAST_ONLY_NONE(UPPER, VCVT_BF16_TO_F32, bfloat16_t, float, bf16, f32, 16, 32, 64);
REGISTER_CAST_ONLY_ROUND(UPPER, VCVT_BF16_TO_S32, bfloat16_t, int32_t, bf16, s32, 16, 32, 64);
REGISTER_CAST_ONLY_NONE(UPPER, VCVT_S16_TO_F32, int16_t, float, s16, f32, 16, 32, 64);
REGISTER_CAST_ONLY_ROUND(UPPER, VCVT_F32_TO_S64, float, int64_t, f32, s64, 32, 64, 32);
REGISTER_CAST_ONLY_NONE(UPPER, VCVT_S32_TO_S64, int32_t, int64_t, s32, s64, 32, 64, 32);

REGISTER_CAST_ROUND_NONE(LOWER, VCVT_F16_TO_U8, half, uint8_t, f16, u8, 16, 8, 128);
REGISTER_CAST_ROUND_NONE(LOWER, VCVT_F16_TO_S8, half, int8_t, f16, s8, 16, 8, 128);
REGISTER_CAST_ALL(LOWER, VCVT_F32_TO_F16, float, half, f32, f16, 32, 16, 64);
REGISTER_CAST_ONLY_ROUND(LOWER, VCVT_F32_TO_BF16, float, bfloat16_t, f32, bf16, 32, 16, 64);
REGISTER_CAST_ONLY_ROUND(LOWER, VCVT_F32_TO_S16, float, int16_t, f32, s16, 32, 16, 64);
REGISTER_CAST_ONLY_ROUND(LOWER, VCVT_S32_TO_F16, int32_t, half, s32, f16, 32, 16, 64);
REGISTER_CAST_ONLY_NONE(LOWER, VCVT_S32_TO_S16, int32_t, int16_t, s32, s16, 32, 16, 64);
REGISTER_CAST_ONLY_ROUND(LOWER, VCVT_S64_TO_F32, int64_t, float, s64, f32, 64, 32, 32);
REGISTER_CAST_ONLY_NONE(LOWER, VCVT_S64_TO_S32, int64_t, int32_t, s64, s32, 64, 32, 32);

REGISTER_CAST_ONLY_ROUND(EQUAL, VCVT_F16_TO_S16, half, int16_t, f16, s16, 16, 16, 128);
REGISTER_CAST_ROUND_NONE(EQUAL, VCVT_S16_TO_F16, int16_t, half, s16, f16, 16, 16, 128);
REGISTER_CAST_ONLY_ROUND(EQUAL, VCVT_F32_TO_F32, float, float, f32, f32, 32, 32, 64);
REGISTER_CAST_ONLY_ROUND(EQUAL, VCVT_F32_TO_S32, float, int32_t, f32, s32, 32, 32, 64);
REGISTER_CAST_ROUND_NONE(EQUAL, VCVT_S32_TO_F32, int32_t, float, s32, f32, 32, 32, 64);

// Cast::Level 2
template <typename T, typename U>
__aicore__ inline void CastImpl(__ubuf__ T* dst, __ubuf__ U* src, const RoundMode& roundMode, const uint32_t count)
{
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            CastIntrinsicsImplCastRint(dst, src, count);
            break;
        case RoundMode::CAST_FLOOR:
            CastIntrinsicsImplCastFloor(dst, src, count);
            break;
        case RoundMode::CAST_CEIL:
            CastIntrinsicsImplCastCeil(dst, src, count);
            break;
        case RoundMode::CAST_ROUND:
            CastIntrinsicsImplCastRound(dst, src, count);
            break;
        case RoundMode::CAST_TRUNC:
            CastIntrinsicsImplCastTrunc(dst, src, count);
            break;
        case RoundMode::CAST_ODD:
            CastIntrinsicsImplCastOdd(dst, src, count);
            break;
        case RoundMode::CAST_NONE:
            CastIntrinsicsImplCastNone(dst, src, count);
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

// Cast::Level 0 - mask bit mode
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ T* dst, __ubuf__ U* src, const RoundMode& roundMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    switch (roundMode) {
        case RoundMode::CAST_NONE:
            CastIntrinsicsImplCastNone(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_ROUND:
            CastIntrinsicsImplCastRound(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_RINT:
            CastIntrinsicsImplCastRint(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_FLOOR:
            CastIntrinsicsImplCastFloor(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_TRUNC:
            CastIntrinsicsImplCastTrunc(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_ODD:
            CastIntrinsicsImplCastOdd(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_CEIL:
            CastIntrinsicsImplCastCeil(dst, src, mask, repeatTime, repeatParams);
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

// Cast::Level 0 - mask count mode
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ T* dst, __ubuf__ U* src, const RoundMode& roundMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            CastIntrinsicsImplCastRint(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_FLOOR:
            CastIntrinsicsImplCastFloor(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_CEIL:
            CastIntrinsicsImplCastCeil(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_ROUND:
            CastIntrinsicsImplCastRound(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_TRUNC:
            CastIntrinsicsImplCastTrunc(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_ODD:
            CastIntrinsicsImplCastOdd(dst, src, mask, repeatTime, repeatParams);
            break;
        case RoundMode::CAST_NONE:
            CastIntrinsicsImplCastNone(dst, src, mask, repeatTime, repeatParams);
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

template <typename T, typename U, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(__ubuf__ T* dst, __ubuf__ U* src, const uint32_t count)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "CastDeq");
}

template <typename T, typename U, bool isSetMask, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(
    __ubuf__ T* dst, __ubuf__ U* src, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "CastDeq");
}

template <typename T, typename U, bool isSetMask, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(
    __ubuf__ T* dst, __ubuf__ U* src, const int32_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "CastDeq");
}

// AddReluCast::Level 0 - mask count mode
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void AddReluCastImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M300_DEFINE_CAST_1(                                                                                      \
    DataType, DataType2, VectorType, VectorType2, MaskIntrinsic, MaskIntrinsic2, ElementCount, ElementCount2,        \
    VectorType3, VectorType4)                                                                                        \
    template <typename T = DataType, typename U = DataType2, bool isSetMask = true>                                  \
    __aicore__ inline void AddReluCastImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType2* src0, __ubuf__ DataType2* src1, const uint64_t mask,             \
        uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                                  \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<U, DataType2>(), "AddReluCast level-0 api only support half/float on current device");       \
        static_assert(                                                                                               \
            SupportType<T, DataType>(), "AddReluCast level-0 api only support half/float on current device");        \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M300_STAGE_CAST_DECLARE_FUSED_REGS(VectorType, VectorType2, sregLower, mask);                        \
            uint32_t sregUpper = static_cast<uint32_t>(mask);                                                        \
            vector_bool pregLower = MaskIntrinsic(sregLower, POST_UPDATE);                                           \
            vector_bool preg = MaskIntrinsic2(sregUpper, POST_UPDATE);                                               \
            ASC_M300_STAGE_CAST_FUSED_B16_LOOP(                                                                      \
                repeatParams, ElementCount, ElementCount2, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, \
                vreg4, VectorType3, VectorType4, dst, pregLower)                                                     \
        }                                                                                                            \
    }

ASC_M300_DEFINE_CAST_1(int8_t, half, vector_f16, vector_s8, plt_b8, plt_b16, 16, 8, vector_u8, vector_u16)
ASC_M300_DEFINE_CAST_1(half, float, vector_f32, vector_f16, plt_b16, plt_b32, 32, 16, vector_u16, vector_u32)

#undef ASC_M300_DEFINE_CAST_1

// AddReluCast::Level 0 - mask bit mode
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void AddReluCastImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask[], uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CAST_2(                                                                                      \
    DataType, DataType2, VectorType, VectorType2, MaskIntrinsic, ElementCount, ElementCount2, VectorType3,           \
    VectorType4)                                                                                                     \
    template <typename T = DataType, typename U = DataType2, bool isSetMask = true>                                  \
    __aicore__ inline void AddReluCastImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType2* src0, __ubuf__ DataType2* src1, const uint64_t mask[],           \
        uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                                  \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<T, DataType>(), "AddReluCast level-2 api only support half/int8_t on current device");       \
        static_assert(                                                                                               \
            SupportType<U, DataType2>(), "AddReluCast level-2 api only support float/half on current device");       \
        if constexpr (isSetMask) {                                                                                   \
            SetVectorMask<U>(mask[1], mask[0]);                                                                      \
        }                                                                                                            \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M300_STAGE_CAST_PREPARE_FUSED_BITMASK(VectorType, VectorType2, MaskIntrinsic);                       \
            ASC_M300_STAGE_CAST_FUSED_B16_LOOP(                                                                      \
                repeatParams, ElementCount, ElementCount2, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, \
                vreg4, VectorType3, VectorType4, dst, pregLower)                                                     \
        }                                                                                                            \
    }

ASC_M300_DEFINE_CAST_2(int8_t, half, vector_f16, vector_s8, movp_b16, 16, 8, vector_u8, vector_u16)
ASC_M300_DEFINE_CAST_2(half, float, vector_f32, vector_f16, movp_b32, 32, 16, vector_u16, vector_u32)

#undef ASC_M300_DEFINE_CAST_2

// AddReluCast::Level 2
template <typename T, typename U>
__aicore__ inline void AddReluCastImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CAST_3(DataType, DataType2, VectorType, VectorType2, ElementCount, MaskIntrinsic, PK_B16Arg) \
    template <typename T = DataType, typename U = DataType2>                                                         \
    __aicore__ inline void AddReluCastImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType2* src0, __ubuf__ DataType2* src1, const uint32_t count)            \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<T, DataType>(), "AddReluCast level-2 api only support half/int8_t on current device");       \
        static_assert(                                                                                               \
            SupportType<U, DataType2>(), "AddReluCast level-2 api only support float/half on current device");       \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M300_STAGE_CAST_DECLARE_FUSED_REGS(VectorType, VectorType2, sreg, count);                            \
            vector_bool preg;                                                                                        \
            uint32_t sregLower = static_cast<uint32_t>(ElementCount);                                                \
            uint16_t repeatTime = CeilDivision(count, ElementCount);                                                 \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                              \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                             \
                vlds(vreg0, src0, i* sregLower, NORM);                                                               \
                vlds(vreg1, src1, i* sregLower, NORM);                                                               \
                vadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                       \
                vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                             \
                vcvt(vreg4, vreg3, preg, ROUND_R, RS_ENABLE, PART_EVEN);                                             \
                vsts(vreg4, dst, i* sregLower, PK_B16Arg, preg);                                                     \
            }                                                                                                        \
        }                                                                                                            \
    }

ASC_M300_DEFINE_CAST_3(int8_t, half, vector_f16, vector_s8, 128, plt_b16, PK_B16)
ASC_M300_DEFINE_CAST_3(half, float, vector_f32, vector_f16, 64, plt_b32, PK_B32)

#undef ASC_M300_DEFINE_CAST_3

// SubReluCast::Level 0 - mask count mode
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void SubReluCastImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CAST_4(                                                                                      \
    DataType, DataType2, VectorType, VectorType2, MaskIntrinsic, MaskIntrinsic2, ElementCount, ElementCount2,        \
    VectorType3, VectorType4)                                                                                        \
    template <typename T = DataType, typename U = DataType2, bool isSetMask = true>                                  \
    __aicore__ inline void SubReluCastImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType2* src0, __ubuf__ DataType2* src1, const uint64_t mask,             \
        uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                                  \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<U, DataType2>(), "SubReluCast level-0 api only support half/float on current device");       \
        static_assert(                                                                                               \
            SupportType<T, DataType>(), "SubReluCast level-0 api only support half/float on current device");        \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M300_STAGE_CAST_DECLARE_FUSED_REGS(VectorType, VectorType2, sregLower, mask);                        \
            uint32_t sregUpper = static_cast<uint32_t>(mask);                                                        \
            vector_bool pregLower = MaskIntrinsic(sregLower, POST_UPDATE);                                           \
            vector_bool preg = MaskIntrinsic2(sregUpper, POST_UPDATE);                                               \
            ASC_M300_STAGE_CAST_FUSED_B32_LOOP(                                                                      \
                repeatParams, ElementCount, ElementCount2, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, \
                vreg4, VectorType3, VectorType4, dst, pregLower)                                                     \
        }                                                                                                            \
    }

ASC_M300_DEFINE_CAST_4(int8_t, half, vector_f16, vector_s8, plt_b8, plt_b16, 16, 8, vector_u8, vector_u16)
ASC_M300_DEFINE_CAST_4(half, float, vector_f32, vector_f16, plt_b16, plt_b32, 32, 16, vector_u16, vector_u32)

#undef ASC_M300_DEFINE_CAST_4

// SubReluCast::Level 0 - mask bit mode
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void SubReluCastImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask[], uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CAST_5(                                                                                      \
    DataType, DataType2, VectorType, VectorType2, MaskIntrinsic, ElementCount, ElementCount2, VectorType3,           \
    VectorType4)                                                                                                     \
    template <typename T = DataType, typename U = DataType2, bool isSetMask = true>                                  \
    __aicore__ inline void SubReluCastImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType2* src0, __ubuf__ DataType2* src1, const uint64_t mask[],           \
        uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                                  \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<U, DataType2>(), "SubReluCast level-0 api only support half/float on current device");       \
        static_assert(                                                                                               \
            SupportType<T, DataType>(), "SubReluCast level-0 api only support half/float on current device");        \
        if constexpr (isSetMask) {                                                                                   \
            SetVectorMask<U>(mask[1], mask[0]);                                                                      \
        }                                                                                                            \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M300_STAGE_CAST_PREPARE_FUSED_BITMASK(VectorType, VectorType2, MaskIntrinsic);                       \
            ASC_M300_STAGE_CAST_FUSED_B32_LOOP(                                                                      \
                repeatParams, ElementCount, ElementCount2, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, \
                vreg4, VectorType3, VectorType4, dst, pregLower)                                                     \
        }                                                                                                            \
    }

ASC_M300_DEFINE_CAST_5(int8_t, half, vector_f16, vector_s8, movp_b16, 16, 8, vector_u8, vector_u16)
ASC_M300_DEFINE_CAST_5(half, float, vector_f32, vector_f16, movp_b32, 32, 16, vector_u16, vector_u32)

#undef ASC_M300_DEFINE_CAST_5

// SubReluCast::Level 2
template <typename T, typename U>
__aicore__ inline void SubReluCastImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CAST_6(DataType, DataType2, VectorType, VectorType2, ElementCount, MaskIntrinsic, PK_B16Arg) \
    template <typename T = DataType, typename U = DataType2>                                                         \
    __aicore__ inline void SubReluCastImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType2* src0, __ubuf__ DataType2* src1, const uint32_t count)            \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<T, DataType>(), "SubReluCast level-2 api only support half/int8_t on current device");       \
        static_assert(                                                                                               \
            SupportType<U, DataType2>(), "SubReluCast level-2 api only support float/half on current device");       \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M300_STAGE_CAST_DECLARE_FUSED_REGS(VectorType, VectorType2, sreg, count);                            \
            vector_bool preg;                                                                                        \
            uint32_t sregLower = static_cast<uint32_t>(ElementCount);                                                \
            uint16_t repeatTime = CeilDivision(count, ElementCount);                                                 \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                              \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                             \
                vlds(vreg0, src0, i* sregLower, NORM);                                                               \
                vlds(vreg1, src1, i* sregLower, NORM);                                                               \
                vsub(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                       \
                vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                             \
                vcvt(vreg4, vreg3, preg, ROUND_R, RS_ENABLE, PART_EVEN);                                             \
                vsts(vreg4, dst, i* sregLower, PK_B16Arg, preg);                                                     \
            }                                                                                                        \
        }                                                                                                            \
    }

ASC_M300_DEFINE_CAST_6(int8_t, half, vector_f16, vector_s8, 128, plt_b16, PK_B16)
ASC_M300_DEFINE_CAST_6(half, float, vector_f32, vector_f16, 64, plt_b32, PK_B32)

#undef ASC_M300_DEFINE_CAST_6

__aicore__ inline void SetDeqScaleImpl(float scale, int16_t offset, bool signMode)
{
    ASCENDC_ASSERT((false), "SetDeqScale is not supported on current device");
}

template <typename T>
__aicore__ inline void SetDeqScaleImpl(const LocalTensor<T>& vdeq, const VdeqInfo& vdeqInfo)
{
    ASCENDC_ASSERT((false), "SetDeqScale is not supported on current device");
}

template <typename T>
__aicore__ inline void SetDeqScaleImpl(T config)
{
    ASCENDC_ASSERT((false), "SetDeqScale is not supported on current device");
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif

#undef ASC_M300_STAGE_CAST_FUSED_B32_LOOP
#undef ASC_M300_STAGE_CAST_PREPARE_FUSED_BITMASK
#undef ASC_M300_STAGE_CAST_FUSED_B16_LOOP
#undef ASC_M300_STAGE_CAST_DECLARE_FUSED_REGS
