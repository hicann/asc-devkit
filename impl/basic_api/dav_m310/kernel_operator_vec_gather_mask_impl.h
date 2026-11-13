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
 * \file kernel_operator_vec_gather_mask_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m310/kernel_operator_vec_gather_mask_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_MASK_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_REDUCEV2_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_REDUCEV2_IMPL_H
#include "../../../include/basic_api/kernel_struct_gather.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_common.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M310_STAGE_GATHER_MASK_INIT_B16(dst, gatherMaskParams, mask)                        \
    sprclr(SPR_AR);                                                                             \
    uint64_t hoistLetVar0 = ((uint64_t)dst);                                                    \
    uint32_t sreg0;                                                                             \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(gatherMaskParams.src0BlockStride)) << 16); \
    uint32_t repeatElm = VECTOR_REG_WIDTH / sizeof(half);                                       \
    uint16_t counterLoop = (mask + repeatElm - 1) / repeatElm

#define ASC_M310_STAGE_GATHER_MASK_LOAD_B16(                                                                           \
    preg0, sreg0, vreg0, src0, i, gatherMaskParams, blockElm, j, strideConfig0)                                        \
    preg0 = plt_b16(sreg0, POST_UPDATE);                                                                               \
    vsldb(                                                                                                             \
        vreg0,                                                                                                         \
        src0 + i * gatherMaskParams.src0RepeatStride * blockElm + j * 8 * gatherMaskParams.src0BlockStride * blockElm, \
        strideConfig0, preg0)

namespace AscendC {
template <typename T>
__aicore__ inline void GatherMaskCal(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ uint16_t* src1, const bool reduceMode, const uint32_t mask,
    const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
{
    ASSERT(false && "unsupported Gather on current device");
}

template <typename T>
__aicore__ inline void GatherMaskCal(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ uint32_t* src1, const bool reduceMode, const uint32_t mask,
    const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
{
    ASSERT(false && "unsupported Gather on current device");
}

__aicore__ inline int64_t GetGatherMaskRemainCountImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetGatherMaskRemainCount on current device");
    return 0;
}

#define REGISTER_GATHER_MASK_B16(dataType, vectorType)                                                             \
    __aicore__ inline void GatherMaskCal(                                                                          \
        __ubuf__ dataType* dst, __ubuf__ dataType* src0, __ubuf__ uint16_t* src1, const bool reduceMode,           \
        const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)                          \
    {                                                                                                              \
        uint32_t blockElm = ONE_BLK_SIZE / sizeof(half);                                                           \
        if (reduceMode) {                                                                                          \
            __VEC_SCOPE__                                                                                          \
            {                                                                                                      \
                vector_align ureg0;                                                                                \
                vector_##vectorType vreg0;                                                                         \
                vector_##vectorType vreg1;                                                                         \
                vector_bool preg0;                                                                                 \
                vector_bool preg1;                                                                                 \
                vector_bool preg2;                                                                                 \
                ASC_M310_STAGE_GATHER_MASK_INIT_B16(dst, gatherMaskParams, mask);                                  \
                for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {               \
                    sreg0 = mask;                                                                                  \
                    for (uint16_t j = 0; j < static_cast<uint16_t>(counterLoop); ++j) {                            \
                        ASC_M310_STAGE_GATHER_MASK_LOAD_B16(                                                       \
                            preg0, sreg0, vreg0, src0, i, gatherMaskParams, blockElm, j, strideConfig0);           \
                        plds(                                                                                      \
                            preg1, ((__ubuf__ uint32_t*)src1), i* gatherMaskParams.src1RepeatStride * 32 + j * 16, \
                            US);                                                                                   \
                        pmov(preg2, preg1, preg0);                                                                 \
                        vsqz(vreg1, vreg0, preg2, MODE_STORED);                                                    \
                        vstur(ureg0, vreg1, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                     \
                    }                                                                                              \
                }                                                                                                  \
                vstar(ureg0, ((__ubuf__ dataType*)dst));                                                           \
            }                                                                                                      \
        } else {                                                                                                   \
            __VEC_SCOPE__                                                                                          \
            {                                                                                                      \
                vector_align ureg0;                                                                                \
                vector_##vectorType vreg0;                                                                         \
                vector_##vectorType vreg1;                                                                         \
                vector_bool preg0;                                                                                 \
                vector_bool preg1 = pset_b16(PAT_ALL);                                                             \
                sprclr(SPR_AR);                                                                                    \
                uint64_t hoistLetVar0 = ((uint64_t)dst);                                                           \
                uint32_t strideConfig0 = ((static_cast<uint32_t>(gatherMaskParams.src0BlockStride)) << 16);        \
                for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {               \
                    vsldb(vreg0, src0 + i * gatherMaskParams.src0RepeatStride * blockElm, strideConfig0, preg1);   \
                    plds(preg0, ((__ubuf__ uint32_t*)src1), i* gatherMaskParams.src1RepeatStride * 32, US);        \
                    vsqz(vreg1, vreg0, preg0, MODE_STORED);                                                        \
                    vstur(ureg0, vreg1, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                         \
                }                                                                                                  \
                vstar(ureg0, ((__ubuf__ dataType*)dst));                                                           \
            }                                                                                                      \
        }                                                                                                          \
        rsvdCnt = get_ar() / 2;                                                                                    \
    }

REGISTER_GATHER_MASK_B16(half, f16)
REGISTER_GATHER_MASK_B16(uint16_t, u16)
REGISTER_GATHER_MASK_B16(int16_t, s16)

#define REGISTER_GATHER_MASK_B32(dataType, vectorType)                                                           \
    __aicore__ inline void GatherMaskCal(                                                                        \
        __ubuf__ dataType* dst, __ubuf__ dataType* src0, __ubuf__ uint32_t* src1, const bool reduceMode,         \
        const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)                        \
    {                                                                                                            \
        uint32_t blockElm = ONE_BLK_SIZE / sizeof(float);                                                        \
        if (reduceMode) {                                                                                        \
            __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);        \
            event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));             \
            SetFlag<HardEvent::S_V>(eventIdSToV);                                                                \
            WaitFlag<HardEvent::S_V>(eventIdSToV);                                                               \
            __VEC_SCOPE__                                                                                        \
            {                                                                                                    \
                vector_align ureg0;                                                                              \
                vector_##vectorType vreg0;                                                                       \
                vector_##vectorType vreg1;                                                                       \
                vector_u8 vreg2;                                                                                 \
                vector_bool preg0;                                                                               \
                vector_bool preg1;                                                                               \
                vector_bool preg2;                                                                               \
                vector_bool preg3;                                                                               \
                vector_bool preg4 = pset_b8(PAT_VL32);                                                           \
                vector_align ureg1;                                                                              \
                sprclr(SPR_AR);                                                                                  \
                uint64_t hoistLetVar0 = ((uint64_t)dst);                                                         \
                uint64_t hoistLetVar1 = ((uint64_t)src1);                                                        \
                uint32_t sreg0 = mask;                                                                           \
                uint32_t sreg1;                                                                                  \
                uint32_t strideConfig0 = ((static_cast<uint32_t>(gatherMaskParams.src0BlockStride)) << 16);      \
                uint32_t repeatElm = VECTOR_REG_WIDTH / sizeof(float);                                           \
                uint16_t counterLoop = (mask + repeatElm - 1) / repeatElm;                                       \
                for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {             \
                    sreg0 = mask;                                                                                \
                    for (uint16_t j = 0; j < static_cast<uint16_t>(counterLoop); ++j) {                          \
                        preg0 = plt_b32(sreg0, POST_UPDATE);                                                     \
                        vsldb(                                                                                   \
                            vreg0,                                                                               \
                            src0 + i * gatherMaskParams.src0RepeatStride * blockElm +                            \
                                j * 8 * gatherMaskParams.src0BlockStride * blockElm,                             \
                            strideConfig0, preg0);                                                               \
                        sreg1 = i * gatherMaskParams.src1RepeatStride * 32 + j * 8;                              \
                        vldas(ureg1, ((__ubuf__ uint8_t*)src1 + sreg1));                                         \
                        hoistLetVar1 = ((uint64_t)src1) + (static_cast<uint64_t>(sreg1));                        \
                        vldus(vreg2, ureg1, ((__ubuf__ uint8_t*&)hoistLetVar1), 0, POST_UPDATE);                 \
                        mem_bar(VST_VST);                                                                        \
                        vsts(vreg2, ((__ubuf__ uint8_t*)tempBuf), 0, NORM_B32, preg4);                           \
                        mem_bar(VST_VLD);                                                                        \
                        plds(preg1, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                       \
                        punpack(preg3, preg1, LOWER);                                                            \
                        pmov(preg2, preg3, preg0);                                                               \
                        vsqz(vreg1, vreg0, preg2, MODE_STORED);                                                  \
                        mem_bar(VST_VST);                                                                        \
                        mem_bar(VLD_VST);                                                                        \
                        vstur(ureg0, vreg1, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                   \
                    }                                                                                            \
                }                                                                                                \
                vstar(ureg0, ((__ubuf__ dataType*)dst));                                                         \
            }                                                                                                    \
            AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                                 \
        } else {                                                                                                 \
            __VEC_SCOPE__                                                                                        \
            {                                                                                                    \
                vector_align ureg0;                                                                              \
                vector_##vectorType vreg0;                                                                       \
                vector_##vectorType vreg1;                                                                       \
                vector_bool preg0;                                                                               \
                vector_bool preg1 = pset_b32(PAT_ALL);                                                           \
                vector_bool preg2;                                                                               \
                sprclr(SPR_AR);                                                                                  \
                uint64_t hoistLetVar0 = ((uint64_t)dst);                                                         \
                uint32_t strideConfig0 = ((static_cast<uint32_t>(gatherMaskParams.src0BlockStride)) << 16);      \
                for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {             \
                    vsldb(vreg0, src0 + i * gatherMaskParams.src0RepeatStride * blockElm, strideConfig0, preg1); \
                    plds(preg0, ((__ubuf__ uint32_t*)src1), i* gatherMaskParams.src1RepeatStride * 32, US);      \
                    punpack(preg2, preg0, LOWER);                                                                \
                    vsqz(vreg1, vreg0, preg2, MODE_STORED);                                                      \
                    vstur(ureg0, vreg1, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                       \
                }                                                                                                \
                vstar(ureg0, ((__ubuf__ dataType*)dst));                                                         \
            }                                                                                                    \
        }                                                                                                        \
        rsvdCnt = get_ar() / 4;                                                                                  \
    }

REGISTER_GATHER_MASK_B32(float, f32)
REGISTER_GATHER_MASK_B32(uint32_t, u32)
REGISTER_GATHER_MASK_B32(int32_t, s32)

template <typename T>
__aicore__ inline void GatherMaskCal(
    __ubuf__ T* dst, __ubuf__ T* src0, const uint8_t src1Pattern, const bool reduceMode, const uint32_t mask,
    const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
{
    ASSERT(false && "unsupported Gather on current device");
}

#define REGISTER_GATHER_MASK_SOLID_B16(dataType, vectorType)                                                     \
    __aicore__ inline void GatherMaskCal(                                                                        \
        __ubuf__ dataType* dst, __ubuf__ dataType* src0, const uint8_t src1Pattern, const bool reduceMode,       \
        const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)                        \
    {                                                                                                            \
        __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);            \
        const uint32_t pattern[8] = {0,         1431655765, 2863311530, 286331153,                               \
                                     572662306, 1145324612, 2290649224, 4294967295};                             \
        uint32_t blockElm = ONE_BLK_SIZE / sizeof(half);                                                         \
        if (reduceMode) {                                                                                        \
            __VEC_SCOPE__                                                                                        \
            {                                                                                                    \
                vector_align ureg0;                                                                              \
                vector_##vectorType vreg0;                                                                       \
                vector_##vectorType vreg1;                                                                       \
                vector_u32 vreg2;                                                                                \
                vector_bool preg0;                                                                               \
                vector_bool preg1;                                                                               \
                vector_bool preg2;                                                                               \
                vector_bool preg3 = pset_b32(PAT_VL8);                                                           \
                vdup(vreg2, pattern[src1Pattern], preg3, MODE_ZEROING);                                          \
                vsts(vreg2, ((__ubuf__ uint32_t*)tempBuf), 0, NORM_B32, preg3);                                  \
                mem_bar(VST_VLD);                                                                                \
                plds(preg1, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                               \
                ASC_M310_STAGE_GATHER_MASK_INIT_B16(dst, gatherMaskParams, mask);                                \
                for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {             \
                    sreg0 = mask;                                                                                \
                    for (uint16_t j = 0; j < static_cast<uint16_t>(counterLoop); ++j) {                          \
                        ASC_M310_STAGE_GATHER_MASK_LOAD_B16(                                                     \
                            preg0, sreg0, vreg0, src0, i, gatherMaskParams, blockElm, j, strideConfig0);         \
                        pmov(preg2, preg1, preg0);                                                               \
                        vsqz(vreg1, vreg0, preg2, MODE_STORED);                                                  \
                        vstur(ureg0, vreg1, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                   \
                    }                                                                                            \
                }                                                                                                \
                vstar(ureg0, ((__ubuf__ dataType*)dst));                                                         \
            }                                                                                                    \
        } else {                                                                                                 \
            __VEC_SCOPE__                                                                                        \
            {                                                                                                    \
                vector_align ureg0;                                                                              \
                vector_u32 vreg0;                                                                                \
                vector_##vectorType vreg1;                                                                       \
                vector_##vectorType vreg2;                                                                       \
                vector_bool preg0 = pset_b32(PAT_VL8);                                                           \
                vector_bool preg1 = pset_b16(PAT_ALL);                                                           \
                vector_bool preg2;                                                                               \
                vdup(vreg0, pattern[src1Pattern], preg0, MODE_ZEROING);                                          \
                vsts(vreg0, ((__ubuf__ uint32_t*)tempBuf), 0, NORM_B32, preg0);                                  \
                mem_bar(VST_VLD);                                                                                \
                plds(preg2, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                               \
                sprclr(SPR_AR);                                                                                  \
                uint64_t hoistLetVar0 = ((uint64_t)dst);                                                         \
                uint32_t strideConfig0 = ((static_cast<uint32_t>(gatherMaskParams.src0BlockStride)) << 16);      \
                for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {             \
                    vsldb(vreg1, src0 + i * gatherMaskParams.src0RepeatStride * blockElm, strideConfig0, preg1); \
                    vsqz(vreg2, vreg1, preg2, MODE_STORED);                                                      \
                    vstur(ureg0, vreg2, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                       \
                }                                                                                                \
                vstar(ureg0, ((__ubuf__ dataType*)dst));                                                         \
            }                                                                                                    \
            AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                                 \
        }                                                                                                        \
        rsvdCnt = get_ar() / 2;                                                                                  \
    }

REGISTER_GATHER_MASK_SOLID_B16(half, f16)
REGISTER_GATHER_MASK_SOLID_B16(uint16_t, u16)
REGISTER_GATHER_MASK_SOLID_B16(int16_t, s16)

#define REGISTER_GATHER_MASK_SOLID_B32(dataType, vectorType)                                                 \
    __aicore__ inline void GatherMaskCal(                                                                    \
        __ubuf__ dataType* dst, __ubuf__ dataType* src0, const uint8_t src1Pattern, const bool reduceMode,   \
        const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)                    \
    {                                                                                                        \
        __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);        \
        const uint32_t pattern[8] = {0,         1431655765, 2863311530, 286331153,                           \
                                     572662306, 1145324612, 2290649224, 4294967295};                         \
        __VEC_SCOPE__                                                                                        \
        {                                                                                                    \
            vector_align ureg0;                                                                              \
            vector_u32 vreg0;                                                                                \
            vector_##vectorType vreg1;                                                                       \
            vector_##vectorType vreg2;                                                                       \
            vector_bool preg0 = pset_b32(PAT_VL8);                                                           \
            vector_bool preg1 = pset_b32(PAT_ALL);                                                           \
            vector_bool preg2;                                                                               \
            vector_bool preg3;                                                                               \
            vdup(vreg0, pattern[src1Pattern], preg0, MODE_ZEROING);                                          \
            vsts(vreg0, ((__ubuf__ uint32_t*)tempBuf), 0, NORM_B32, preg0);                                  \
            mem_bar(VST_VLD);                                                                                \
            plds(preg2, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                               \
            punpack(preg3, preg2, LOWER);                                                                    \
            sprclr(SPR_AR);                                                                                  \
            uint64_t hoistLetVar0 = ((uint64_t)dst);                                                         \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(gatherMaskParams.src0BlockStride)) << 16);      \
            uint32_t blockElm = ONE_BLK_SIZE / sizeof(float);                                                \
            for (uint16_t i = 0; i < static_cast<uint16_t>(gatherMaskParams.repeatTimes); ++i) {             \
                vsldb(vreg1, src0 + i * gatherMaskParams.src0RepeatStride * blockElm, strideConfig0, preg1); \
                vsqz(vreg2, vreg1, preg3, MODE_STORED);                                                      \
                vstur(ureg0, vreg2, ((__ubuf__ dataType*&)hoistLetVar0), POST_UPDATE);                       \
            }                                                                                                \
            vstar(ureg0, ((__ubuf__ dataType*)dst));                                                         \
        }                                                                                                    \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                                 \
        rsvdCnt = get_ar() / 4;                                                                              \
    }

REGISTER_GATHER_MASK_SOLID_B32(float, f32)
REGISTER_GATHER_MASK_SOLID_B32(uint32_t, u32)
REGISTER_GATHER_MASK_SOLID_B32(int32_t, s32)
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_REDUCEV2_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_MASK_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_MASK_IMPL_H__
#endif

#undef ASC_M310_STAGE_GATHER_MASK_LOAD_B16
#undef ASC_M310_STAGE_GATHER_MASK_INIT_B16
