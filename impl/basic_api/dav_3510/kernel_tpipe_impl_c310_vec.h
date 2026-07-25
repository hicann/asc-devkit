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
 * \file kernel_tpipe_impl_c310_vec.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_tpipe_impl_c310_vec.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_C310_VEC_H__
#endif
#ifndef ASCENDC_MODULE_TPIPE_IMPL_C310_VEC_H
#define ASCENDC_MODULE_TPIPE_IMPL_C310_VEC_H

#include "../kernel_macros.h"
#include "../common_types.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../kernel_event.h"
#include "../kernel_log.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif
#include "../../../include/basic_api/kernel_tensor.h"
#include "../kernel_tensor_base.h"
#include "../kernel_tpipe_base.h"
#include "../kernel_utils.h"
#include "../utils/kernel_utils_ceil_oom_que.h"
#include "../utils/kernel_utils_constants.h"
#include "../utils/kernel_utils_macros.h"
#include "../utils/kernel_utils_mode_cpu.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include <map>
#include "stub_def.h"
#include "stub_fun.h"
#endif

namespace AscendC {
__aicore__ inline void PrintTimeStamp(uint32_t descId);
namespace ConstantsInternal {
const uint32_t FULL_MASK_B32 = 0xffffffff;
} // namespace ConstantsInternal

template <typename T>
__aicore__ inline __in_pipe__(V) void NopInPipeV(const T& tensor)
{
    (void)(0);
}
template <typename T>
__aicore__ inline __out_pipe__(V) void NopOutPipeV(const T& tensor)
{
    (void)(0);
}
template <const TQueConfig& config, Hardware srcType, Hardware dstType>
__aicore__ inline constexpr void ValidateQueConfig()
{
    if constexpr (config.enableStaticEvtId) {
        static_assert(
            dstType == Hardware::L1 && srcType != Hardware::UB,
            "enableStaticEvtId currently only supports GM to L1 Buffer(A1/B1/TSCM)");
    }
}

template <int32_t depth, int32_t maxBufferBlock, const TQueConfig& config>
__aicore__ inline constexpr void ValidateGlobalManageQueConfig()
{
    static_assert(depth == 1, "static queue depth must be 1");
    static_assert(maxBufferBlock >= config.bufferNumber, "bufferNumber cannot exceeds the limit.");
    static_assert((config.bufferNumber & (config.bufferNumber - 1)) == 0, "bufferNum must be power of 2");
    static_assert(
        ((config.bufferLen & (config.bufferLen - 1)) == 0) && config.bufferLen >= MIN_BUFFER_BLOCK_SIZE,
        "bufferLen must be power of 2 && equals or greater than 32k");
}
__aicore__ inline constexpr bool IsTscm(TPosition src, TPosition dst)
{
    return GetPosition(src, dst) == TPosition::TSCM;
}

// begin impl of tquebind
// TQueBind : this is used for off-standard queue
template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline TQueBind<src, dst, depth, mask>::TQueBind()
{
    ValidateQueConfig<config, srcHardType, dstHardType>();
    if constexpr (enableGlobalManageQue) {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        uint8_t* ptr;
        if (bufferType == Hardware::GM) {
            ptr = ConstDefiner::Instance().cpuGM;
        } else {
            ptr = ConstDefiner::Instance().hardwareCpuBufferMap.at(bufferType);
        }
        auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
        AscendCBufAbsAddr(
            static_cast<uint8_t>(bufferType), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ptr)),
            bufferInitLen.at(Hardware::UB));
        ASCENDC_ASSERT((config.bufferLen * config.bufferNumber <= bufferInitLen.at(bufferType)), {
            KERNEL_LOG(
                KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", config.bufferLen * config.bufferNumber,
                bufferInitLen.at(bufferType));
        });
        auto pos_ = GetPosition(src, dst);
        AscendCBufInit(
            static_cast<uint8_t>(pos_), 0, config.bufferNumber, reinterpret_cast<uint64_t>(ptr), config.bufferLen);
#endif
        // Occupied bufID with 20-27;
        Internal::g_bufId = Internal::g_bufId | 0xFF00000;
        ValidateGlobalManageQueConfig<depth, maxBufferBlock, config>();
        uint32_t baseAddr = 0;
        for (int32_t i = 0; i < config.bufferNumber; i++) {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
            bufStart.bufInfo[i].address = baseAddr;
            bufStart.bufInfo[i].dataLen = config.bufferLen;
            bufStart.bufInfo[i].bufId = i + bufIdOffset;
#else
            bufStart.bufInfo[i] = {
                .bufId = static_cast<TBufId>(i + bufIdOffset), .address = baseAddr, .dataLen = config.bufferLen};
#endif
            baseAddr += config.bufferLen;
        }
        staticHead = 0;
        staticEnqueHead = INVALID_STATIC_ENQUE_HEAD;
        freeMask = ConstantsInternal::FULL_MASK_B32 >> (maxBlockNum - config.bufferNumber);
    }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    AscendCQueCreate(static_cast<uint8_t>(src), static_cast<uint8_t>(dst), depth);
#endif // ASCENDC_CPU_DEBUG
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline uint64_t TQueBind<src, dst, depth, mask>::GetNext(const int32_t len)
{
    DEBUG_CODE(int loop = 0);
    uint32_t maskLen = ConstantsInternal::FULL_MASK_B32 >> (maxBlockNum - len);
    do {
        auto curMask = maskLen << staticHead;
        if ((freeMask & curMask) == curMask) {
            freeMask ^= curMask;
            auto ret = staticHead;
            staticHead = (staticHead + len) & (config.bufferNumber - 1);
            return ret;
        } else {
            staticHead = (staticHead + 1) & (config.bufferNumber - 1);
        }
        DEBUG_CODE(if (++loop > config.bufferNumber) { return static_cast<uint64_t>(-1); };);
    } while (true);
    return static_cast<uint64_t>(-1);
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TQueBind<src, dst, depth, mask>::AllocTensor()
{
    static_assert((depth != 0), "must use AllocTensor<LocalTensor&> api while tque's depth is zero");
    auto buf = AllocBuffer();
    return Buf2Tensor<T>(buf);
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TQueBind<src, dst, depth, mask>::AllocTensor(int32_t num)
{
    if constexpr (enableGlobalManageQue) {
        ASCENDC_ASSERT((num > 0 && num <= config.bufferNumber), {
            KERNEL_LOG(
                KERNEL_ERROR, "StaticQue AllocTensor size is %d, which must be with (0, %d]", static_cast<int32_t>(num),
                static_cast<int32_t>(config.bufferNumber));
        });
        const uint32_t maskLen = ConstantsInternal::FULL_MASK_B32 >> (maxBlockNum - num);
        const uint32_t pos = GetNext(num);
        TBufType* ret = this->bufStart.bufInfo + pos;
        ret->state = TBufState::OCCUPIED;
        ret->dataLen = num << shiftBits;
        ret->bufIdAlt = ret->bufId + num - 1;
        for (uint8_t i = num - 1; i > 0; i--) {
            const uint8_t tmpBufId = ret->bufId + i;
            GetBuffImpl<srcPipe, false>(tmpBufId);
            ReleaseBuffImpl<srcPipe, false>(tmpBufId);
        }
        // take usertag position for storing active mask.
        ret->usertag = maskLen << pos;
        GetBuffImpl<srcPipe, false>(ret->bufId);
        return Buf2Tensor<T>(reinterpret_cast<TBufHandle>(ret));
    } else {
        auto buf = AllocBuffer();
        return Buf2Tensor<T>(buf);
    }
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline __sync_noalias__ void TQueBind<src, dst, depth, mask>::AllocTensor(LocalTensor<T>& tensor)
{
    static_assert((depth == 0), "can not AllocTensor in place while tque's depth is non zero");
    auto buf = AllocBuffer();
    // c310 aic depth=0 will use waitflag
    if ASCEND_IS_AIC {
        WaitFlag<freeBufEvt>(reinterpret_cast<TBufType*>(buf)->freeBufEvtID);
    }
    TBuffAddr addr = GetBufferAddr(buf);
    tensor.SetAddr(addr);
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline void TQueBind<src, dst, depth, mask>::FreeTensor(LocalTensor<T>& tensor)
{
    FreeBuffer(tensor.GetBufferHandle());
    return;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::EnQue(const LocalTensor<T>& tensor)
{
    if constexpr (GetPhyType(src) == Hardware::UB || GetPhyType(dst) == Hardware::UB) {
        NopInPipeV<LocalTensor<T>>(tensor);
    }
    auto buf = tensor.GetBufferHandle();
    return EnQue(reinterpret_cast<TBufHandle>(buf));
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <TPosition srcUserPos, TPosition dstUserPos, typename T>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::EnQue(const LocalTensor<T>& tensor)
{
    if constexpr (GetPhyType(srcUserPos) == Hardware::UB || GetPhyType(dstUserPos) == Hardware::UB) {
        NopInPipeV<LocalTensor<T>>(tensor);
    }
    auto buf = tensor.GetBufferHandle();
    return EnQue<srcUserPos, dstUserPos>(reinterpret_cast<TBufHandle>(buf));
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <TPosition srcUserPos, TPosition dstUserPos>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::EnQue(TBufHandle buf)
{
    static_assert((depth != 0), "can not enque tbuf with user pos while tque's depth is zero");
    static_assert(
        ((srcUserPos == TPosition::GM) || (srcUserPos == TPosition::VECIN) || (srcUserPos == TPosition::VECOUT) ||
         (srcUserPos == TPosition::VECCALC)) &&
        "enque only support src position GM/UB(VECIN/VECOUT/VECCALC) currently.");
    static_assert(
        ((dstUserPos == TPosition::GM) || (dstUserPos == TPosition::VECIN) || (dstUserPos == TPosition::VECOUT) ||
         (dstUserPos == TPosition::VECCALC)) &&
        "enque only support dst position GM/UB(VECIN/VECOUT/VECCALC) currently.");
    static_assert(
        !((srcUserPos == TPosition::GM) && (dstUserPos == TPosition::GM)) &&
        "enque src and dst position cannot be GM at the same time.");
    constexpr pipe_t srcUserPipe = GetPipeByPos(srcUserPos, dstUserPos);
    constexpr bool useAltBufId = UseAltBufId(dst, dstUserPos, config.consumerSize);

    ASCENDC_ASSERT((this->usedCount < depth), {
        KERNEL_LOG(
            KERNEL_ERROR, "usedCount is %d, which exceed depth limits %d", static_cast<int32_t>(usedCount), depth);
    });
    auto ptr = reinterpret_cast<TBufType*>(buf);
    if constexpr (depth == 1) {
        this->que_ = buf;
    } else {
        this->que_[this->tail] = buf;
    }
    this->usedCount++;

    ASCENDC_ASSERT((this->bufStart <= ptr && ptr < this->bufStart + this->bufNum), {
        KERNEL_LOG(
            KERNEL_ERROR, "ptr is %p, which should be in range [%p, %p)", ptr, this->bufStart,
            this->bufStart + this->bufNum);
    });
    ASCENDC_ASSERT((ptr->state == TBufState::OCCUPIED) || (ptr->state == TBufState::DEQUE), {
        KERNEL_LOG(KERNEL_ERROR, "ptr state is %d, which should be OCCUPIED / DEQUE", static_cast<int32_t>(ptr->state));
    });
    DEBUG_CODE(ptr->state = TBufState::ENQUE);

    if constexpr (useAltBufId) {
        GetBuffImpl<srcUserPipe, true>(ptr->bufIdAlt);
        ReleaseBuffImpl<srcUserPipe, true>(ptr->bufIdAlt);
    } else {
        GetBuffImpl<srcUserPipe, true>(ptr->bufId);
        ReleaseBuffImpl<srcUserPipe, true>(ptr->bufId);
    }

    ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
        KERNEL_LOG(KERNEL_ERROR, "EnQue is not matched with the previous state.");
    });

    if constexpr (depth != 1) {
        if (++this->tail >= depth) {
            this->tail = 0;
        }
    }

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    constexpr Hardware bufferType = GetBufferPos(src, dst);
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
    AscendCBufEnque(
        static_cast<uint8_t>(src), static_cast<uint8_t>(dst), static_cast<uint8_t>(GetPosition(src, dst)),
        reinterpret_cast<uint64_t>(absAddr + ptr->address));
#endif // ASCENDC_CPU_DEBUG
    return true;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::EnQue(TBufHandle buf)
{
    if constexpr (enableGlobalManageQue) {
        static_assert((depth != 0), "can not EnQue tbuf using global manage while depth is non zero");
        auto ptr = reinterpret_cast<TBufType*>(buf);
        ASCENDC_ASSERT((this->staticEnqueHead == INVALID_STATIC_ENQUE_HEAD), {
            KERNEL_LOG(
                KERNEL_ERROR, "StaticQue staticEnqueHead is %d, which must be 0 before enque",
                static_cast<int32_t>(this->staticEnqueHead));
        });
        staticEnqueHead = ptr->address >> shiftBits;

        ASCENDC_ASSERT((this->staticEnqueHead < config.bufferNumber), {
            KERNEL_LOG(
                KERNEL_ERROR, "StaticQue staticEnqueHead is %d, which must be less than %d",
                static_cast<int32_t>(this->staticEnqueHead), config.bufferNumber);
        });

        ASCENDC_ASSERT((ptr->state == TBufState::OCCUPIED) || (ptr->state == TBufState::DEQUE), {
            KERNEL_LOG(
                KERNEL_ERROR, "ptr state is %d, which should be OCCUPIED / DEQUE", static_cast<int32_t>(ptr->state));
        });
        DEBUG_CODE(ptr->state = TBufState::ENQUE);

        ReleaseBuffImpl<srcPipe, false>(ptr->bufId);
        return true;
    } else {
        if constexpr (depth != 0) {
            ASCENDC_ASSERT((this->usedCount < depth), {
                KERNEL_LOG(
                    KERNEL_ERROR, "usedCount is %d, which exceed depth limits %d", static_cast<int32_t>(usedCount),
                    depth);
            });
        }
        auto ptr = reinterpret_cast<TBufType*>(buf);

        if constexpr (depth == 1) {
            this->que_ = buf;
        } else if constexpr (depth != 0) {
            this->que_[this->tail] = buf;
        }

        if constexpr (depth != 0) {
            if constexpr (config.enableStaticEvtId) {
                DEBUG_CODE(this->usedCount++);
            } else {
                this->usedCount++;
            }
        }

        ASCENDC_ASSERT((this->bufStart <= ptr && ptr < this->bufStart + this->bufNum), {
            KERNEL_LOG(
                KERNEL_ERROR, "ptr is %p, which should be in range [%p, %p)", ptr, this->bufStart,
                this->bufStart + this->bufNum);
        });
        ASCENDC_ASSERT((ptr->state == TBufState::OCCUPIED) || (ptr->state == TBufState::DEQUE), {
            KERNEL_LOG(
                KERNEL_ERROR, "ptr state is %d, which should be OCCUPIED / DEQUE", static_cast<int32_t>(ptr->state));
        });
        DEBUG_CODE(ptr->state = TBufState::ENQUE);

        // If the AIC is not entered, the AIV does not process any event ID.
        if constexpr (!IsTscm(src, dst)) {
            if ASCEND_IS_AIV {
                GetBuffImpl<srcPipe, true>(ptr->bufId);
                ReleaseBuffImpl<srcPipe, true>(ptr->bufId);
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "EnQue is not matched with the previous state.");
                });
            } else {
                // aic depth=0 will set flag
                if constexpr (depth == 0) {
                    SetFlag<enQueEvt>(ptr->enQueEvtID);
                } else if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
                    GetBuffImpl<srcPipe, true>(ptr->bufId);
                    ReleaseBuffImpl<srcPipe, true>(ptr->bufId);
                    ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                        KERNEL_LOG(KERNEL_ERROR, "EnQue is not matched with the previous state.");
                    });
                } else {
                    auto enQueEvtID = GetTPipePtr()->AllocEventID<enQueEvt>();
                    SetFlag<enQueEvt>(enQueEvtID);
                    ptr->enQueEvtID = enQueEvtID;
                }
            }
        } else if constexpr (srcHardType == Hardware::GM) {
            if ASCEND_IS_AIC {
                GetBuffImpl<srcPipe, true>(ptr->bufId);
                ReleaseBuffImpl<srcPipe, true>(ptr->bufId);
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "EnQue is not matched with the previous state.");
                });
            }
        }
        if constexpr (depth > 1) {
            if (++this->tail >= depth) {
                this->tail = 0;
            }
        }

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
        AscendCBufEnque(
            static_cast<uint8_t>(src), static_cast<uint8_t>(dst), static_cast<uint8_t>(GetPosition(src, dst)),
            reinterpret_cast<uint64_t>(absAddr + ptr->address));
#endif // ASCENDC_CPU_DEBUG
        return true;
    }
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline LocalTensor<T> TQueBind<src, dst, depth, mask>::DeQue()
{
    static_assert((depth != 0), "must use DeQue<LocalTensor&> api while tque's depth is zero");
    auto buf = DeQue();
    auto ret = Buf2Tensor<T>(buf);
    if constexpr (GetPhyType(src) == Hardware::UB || GetPhyType(dst) == Hardware::UB) {
        NopOutPipeV<LocalTensor<T>>(ret);
    }
    return ret;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <TPosition srcUserPos, TPosition dstUserPos, typename T>
__aicore__ inline LocalTensor<T> TQueBind<src, dst, depth, mask>::DeQue()
{
    static_assert((depth != 0), "must use DeQue<LocalTensor&> api while tque's depth is zero");
    auto buf = DeQue<srcUserPos, dstUserPos>();
    auto ret = Buf2Tensor<T>(buf);
    if constexpr (GetPhyType(src) == Hardware::UB || GetPhyType(dst) == Hardware::UB) {
        NopOutPipeV<LocalTensor<T>>(ret);
    }
    return ret;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline TBufHandle TQueBind<src, dst, depth, mask>::DeQue()
{
    if constexpr (enableGlobalManageQue) {
        TBufHandle bufHandle = reinterpret_cast<TBufHandle>(bufStart.bufInfo + staticEnqueHead);
        ASCENDC_ASSERT((this->staticEnqueHead != INVALID_STATIC_ENQUE_HEAD), {
            KERNEL_LOG(
                KERNEL_ERROR, "staticUsedCount is %d, which can only larger than 0",
                static_cast<int32_t>(this->staticUsedCount));
        });
        DEBUG_CODE(staticEnqueHead = INVALID_STATIC_ENQUE_HEAD);
        GetBuffImpl<dstPipe, false>(staticEnqueHead + bufIdOffset);
        return bufHandle;
    } else {
        TBufHandle buf;
        if constexpr (depth == 1) {
            buf = this->que_;
        } else {
            buf = this->que_[this->head];
        }
        ASCENDC_ASSERT((buf != nullptr), { KERNEL_LOG(KERNEL_ERROR, "buf can not be nullptr"); });
        auto ptr = reinterpret_cast<TBufType*>(buf);

        ASCENDC_ASSERT((ptr->state == TBufState::ENQUE), {
            KERNEL_LOG(KERNEL_ERROR, "ptr state is %d, which can only be ENQUE", static_cast<int32_t>(ptr->state));
        });
        ASCENDC_ASSERT((this->usedCount > 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "usedCount is %d, which can only larger than 0", static_cast<int32_t>(this->usedCount));
        });
        if constexpr (config.enableStaticEvtId) {
            DEBUG_CODE(this->usedCount--);
        } else {
            this->usedCount--;
        }
        /* Add for TSCM
         * for 220, aiv just send message, no need add this set/wait
         */
        DEBUG_CODE(ptr->state = TBufState::DEQUE);

        // If the AIC is not entered, the AIV does not process any event ID.
        if constexpr (!IsTscm(src, dst)) {
            if ASCEND_IS_AIV {
                GetBuffImpl<dstPipe, false>(ptr->bufId);
                ReleaseBuffImpl<dstPipe, false>(ptr->bufId);
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "DeQue is not matched with the previous state.");
                });
            } else {
                if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
                    GetBuffImpl<dstPipe, false>(ptr->bufId);
                    ReleaseBuffImpl<dstPipe, false>(ptr->bufId);
                    ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                        KERNEL_LOG(KERNEL_ERROR, "DeQue is not matched with the previous state.");
                    });
                } else {
                    if (ptr->enQueEvtID != INVALID_TEVENTID) {
                        WaitFlag<enQueEvt>(ptr->enQueEvtID);
                        GetTPipePtr()->ReleaseEventID<enQueEvt>(ptr->enQueEvtID);
                        ptr->enQueEvtID = INVALID_TEVENTID;
                    }
                }
            }
        } else if constexpr (srcHardType == Hardware::GM) {
            if ASCEND_IS_AIC {
                GetBuffImpl<dstPipe, false>(ptr->bufId);
                ReleaseBuffImpl<dstPipe, false>(ptr->bufId);
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "DeQue is not matched with the previous state.");
                });
            }
        } else if constexpr (srcHardType == Hardware::UB) {
#if defined(KFC_C310_SSBUF) && KFC_C310_SSBUF == 1
            if ASCEND_IS_AIV {
                // Use enQueEvtId for representing TSCM Que inter-core sync, since no inner-core sync
                // required for TSCM in AIV.
                ptr->freeBufEvtID = 0;
                constexpr int32_t maxAllowedTscmAndMmCount = 10;
                ASCENDC_ASSERT(
                    Internal::TSCM_CROSS_SYNC_ID_MAX - ptr->enQueEvtID + 1 + g_matmulCount <= maxAllowedTscmAndMmCount,
                    {
                        KERNEL_LOG(
                            KERNEL_ERROR,
                            "TSCM and Matmul Objects exceed the regulation that the total number should be within %d, "
                            "while Matmul count is %d, buffer count is %d",
                            maxAllowedTscmAndMmCount, g_matmulCount,
                            Internal::TSCM_CROSS_SYNC_ID_MAX - ptr->enQueEvtID + 1);
                    });
                set_intra_block(PIPE_MTE3, ptr->enQueEvtID);
            }
#endif
        }
        if constexpr (depth != 1) {
            if (++this->head >= depth) {
                this->head = 0;
            }
        }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
        AscendCBufDeque(
            static_cast<uint8_t>(src), static_cast<uint8_t>(dst), static_cast<uint8_t>(GetPosition(src, dst)),
            (uint64_t)(absAddr + ptr->address));
#endif // ASCENDC_CPU_DEBUG
        return reinterpret_cast<TBufHandle>(buf);
    }
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline void TQueBind<src, dst, depth, mask>::DeQue(LocalTensor<T>& tensor)
{
    static_assert((depth == 0), "can not DeQue tensor in place while tque's depth is non zero");
    TBufHandle buf = tensor.GetBufferHandle();

    ASCENDC_ASSERT((buf != nullptr), { KERNEL_LOG(KERNEL_ERROR, "buf can not be nullptr"); });
    auto ptr = reinterpret_cast<TBufType*>(buf);

    ASCENDC_ASSERT((ptr->state == TBufState::ENQUE), {
        KERNEL_LOG(KERNEL_ERROR, "ptr state is %d, which can only be ENQUE", static_cast<int32_t>(ptr->state));
    });
    /* Add for TSCM
     * for 220, aiv just send message, no need add this set/wait
     */
    DEBUG_CODE(ptr->state = TBufState::DEQUE);

    // If the AIC is not entered, the AIV does not process any event ID.
    if ASCEND_IS_AIV {
        GetBuffImpl<dstPipe, false>(ptr->bufId);
        ReleaseBuffImpl<dstPipe, false>(ptr->bufId);
        ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
            KERNEL_LOG(KERNEL_ERROR, "DeQue is not matched with the previous state.");
        });
    } else {
        auto ptr = reinterpret_cast<TBufType*>(buf);
        WaitFlag<enQueEvt>(ptr->enQueEvtID);
    }

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    constexpr Hardware bufferType = GetBufferPos(src, dst);
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
    AscendCBufDeque(
        static_cast<uint8_t>(src), static_cast<uint8_t>(dst), static_cast<uint8_t>(GetPosition(src, dst)),
        (uint64_t)(absAddr + ptr->address));
#endif // ASCENDC_CPU_DEBUG

    if constexpr (GetPhyType(src) == Hardware::UB || GetPhyType(dst) == Hardware::UB) {
        NopOutPipeV<LocalTensor<T>>(tensor);
    }
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <TPosition srcUserPos, TPosition dstUserPos>
__aicore__ inline TBufHandle TQueBind<src, dst, depth, mask>::DeQue()
{
    static_assert(
        ((srcUserPos == TPosition::GM) || (srcUserPos == TPosition::VECIN) || (srcUserPos == TPosition::VECOUT) ||
         (srcUserPos == TPosition::VECCALC)),
        "DeQue only support src position GM/UB(VECIN/VECOUT/VECCALC) currently.");
    static_assert(
        ((dstUserPos == TPosition::GM) || (dstUserPos == TPosition::VECIN) || (dstUserPos == TPosition::VECOUT) ||
         (dstUserPos == TPosition::VECCALC)) &&
        "DeQue only support dst position GM/UB(VECIN/VECOUT/VECCALC) currently.");
    static_assert(
        !((srcUserPos == TPosition::GM) && (dstUserPos == TPosition::GM)) &&
        "DeQue src and dst position cannot be GM at the same time.");
    constexpr pipe_t dstUserPipe = GetPipeByPos(dstUserPos, srcUserPos);
    constexpr bool useAltBufId = UseAltBufId(dst, dstUserPos, config.consumerSize);

    TBufHandle buf;
    if constexpr (depth == 1) {
        buf = this->que_;
    } else {
        buf = this->que_[this->head];
    }
    ASCENDC_ASSERT((buf != nullptr), { KERNEL_LOG(KERNEL_ERROR, "buf can not be nullptr"); });
    auto ptr = reinterpret_cast<TBufType*>(buf);

    ASCENDC_ASSERT((ptr->state == TBufState::ENQUE), {
        KERNEL_LOG(KERNEL_ERROR, "ptr state is %d, which can only be ENQUE", static_cast<int32_t>(ptr->state));
    });
    ASCENDC_ASSERT((this->usedCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "usedCount is %d, which can only larger than 0", static_cast<int32_t>(this->usedCount));
    });
    this->usedCount--;
    DEBUG_CODE(ptr->state = TBufState::DEQUE);
    if constexpr (useAltBufId) {
        GetBuffImpl<dstUserPipe, false>(ptr->bufIdAlt);
        ReleaseBuffImpl<dstUserPipe, false>(ptr->bufIdAlt);
    } else {
        GetBuffImpl<dstUserPipe, false>(ptr->bufId);
        ReleaseBuffImpl<dstUserPipe, false>(ptr->bufId);
    }

    ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
        KERNEL_LOG(KERNEL_ERROR, "DeQue is not matched with the previous state.");
    });

    if constexpr (depth != 1) {
        if (++this->head >= depth) {
            this->head = 0;
        }
    }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    constexpr Hardware bufferType = GetBufferPos(src, dst);
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
    AscendCBufDeque(
        static_cast<uint8_t>(src), static_cast<uint8_t>(dst), static_cast<uint8_t>(GetPosition(src, dst)),
        (uint64_t)(absAddr + ptr->address));
#endif // ASCENDC_CPU_DEBUG
    return reinterpret_cast<TBufHandle>(buf);
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline void TQueBind<src, dst, depth, mask>::FreeBuffer(TBufHandle buf)
{
    if constexpr (enableGlobalManageQue) {
        static_assert((depth != 0), "can not FreeBuffer using global manage while depth is non zero");
        auto ptr = reinterpret_cast<TBufType*>(buf);
        ptr->state = TBufState::FREE;
        freeMask |= static_cast<uint32_t>(ptr->usertag);

        ReleaseBuffImpl<dstPipe, false>(ptr->bufId);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        uint8_t* cpuPtr;
        if (bufferType == Hardware::GM) {
            cpuPtr = ConstDefiner::Instance().cpuGM;
        } else {
            cpuPtr = ConstDefiner::Instance().hardwareCpuBufferMap.at(bufferType);
        }
        AscendCBufFree(
            static_cast<uint8_t>(bufferType), static_cast<uint8_t>(GetPosition(src, dst)),
            (uint64_t)(cpuPtr + ptr->address), static_cast<uint64_t>(ptr->dataLen));
#endif // ASCENDC_CPU_DEBUG
        return;
    } else {
        auto ptr = reinterpret_cast<TBufType*>(buf);
        ASCENDC_ASSERT((this->bufStart <= ptr && ptr < this->bufStart + this->bufNum), {
            KERNEL_LOG(
                KERNEL_ERROR, "ptr is %p, which should be in range [%p, %p)", ptr, this->bufStart,
                this->bufStart + this->bufNum);
        });
        ASCENDC_ASSERT((ptr->state != TBufState::FREE), {
            KERNEL_LOG(KERNEL_ERROR, "ptr state is %d, which can not be FREE", static_cast<int32_t>(ptr->state));
        });
        if constexpr (!IsTscm(src, dst)) {
            if ASCEND_IS_AIV {
                if constexpr (config.consumerSize > 1) {
                    constexpr uint8_t idx = config.consumer[0] == dst ? 1 : 0;
                    GetBuffImpl<GetPipeByPos(dst, TPosition::VECCALC), true>(ptr->bufId);
                    ReleaseBuffImpl<GetPipeByPos(dst, TPosition::VECCALC), true>(ptr->bufId);
                    GetBuffImpl<GetPipeByPos(config.consumer[idx], TPosition::VECCALC), true>(ptr->bufIdAlt);
                    ReleaseBuffImpl<GetPipeByPos(config.consumer[idx], TPosition::VECCALC), true>(ptr->bufIdAlt);
                } else {
                    GetBuffImpl<dstPipe, true>(ptr->bufId);
                    ReleaseBuffImpl<dstPipe, true>(ptr->bufId);
                }
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "FreeBuffer is not matched with the previous state.");
                });
            } else {
                // aic depth=0 will set flag
                if constexpr (depth == 0) {
                    SetFlag<freeBufEvt>(ptr->freeBufEvtID);
                } else if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
                    GetBuffImpl<dstPipe, true>(ptr->bufId);
                    ReleaseBuffImpl<dstPipe, true>(ptr->bufId);
                } else {
                    ptr->freeBufEvtID = GetTPipePtr()->AllocEventID<freeBufEvt>();
                    SetFlag<freeBufEvt>(ptr->freeBufEvtID);
                }
            }
        } else if constexpr (srcHardType == Hardware::GM) {
            if ASCEND_IS_AIC {
                GetBuffImpl<dstPipe, true>(ptr->bufId);
                ReleaseBuffImpl<dstPipe, true>(ptr->bufId);
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "FreeBuffer is not matched with the previous state.");
                });
            }
        }
        ptr->state = TBufState::FREE;
        if constexpr (config.enableStaticEvtId) {
            DEBUG_CODE(this->bufUsedCount--);
        } else {
            this->bufUsedCount--;
        }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
        AscendCBufFree(
            static_cast<uint8_t>(bufferType), static_cast<uint8_t>(GetPosition(src, dst)),
            (uint64_t)(absAddr + ptr->address), static_cast<uint64_t>(ptr->dataLen));
#endif // ASCENDC_CPU_DEBUG
        return;
    }
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline TBufHandle TQueBind<src, dst, depth, mask>::AllocBuffer()
{
    DEBUG_CODE(int32_t size = 0);
    ASCENDC_ASSERT((bufNum > 0), {
        KERNEL_LOG(KERNEL_ERROR, "bufNum is %d, which must be larger than 0", static_cast<int32_t>(bufNum));
    });
    TBufType* ret;
    if constexpr (config.bufferNumber == 1) {
        ret = this->bufStart;
        ASCENDC_ASSERT((ret->state == TBufState::FREE), {
            KERNEL_LOG(KERNEL_ERROR, "ptr state is %d, which should be Free", static_cast<int32_t>(ret->state));
        });
        DEBUG_CODE(ret->state = TBufState::OCCUPIED);
    } else {
        do {
            ret = this->bufStart + this->bufCursor;
            this->bufCursor += 1;
            if (this->bufCursor == this->bufNum) {
                this->bufCursor = 0;
            }
            if (ret->state == TBufState::FREE) {
                ret->state = TBufState::OCCUPIED;
                break;
            }
            ASCENDC_ASSERT((++size <= this->bufNum), {
                KERNEL_LOG(
                    KERNEL_ERROR, "size is %d, which exceeds the limit %d", size, static_cast<int32_t>(this->bufNum));
            });
        } while (true);
    }
    if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
        static_assert(depth != 0, "cannot alloc L1 buffer if static event id configured while depth is zero");
        ASCENDC_ASSERT((config.bufferNumber <= 2), {
            KERNEL_LOG(
                KERNEL_ERROR, "bufferNumber %d must be <= 2 for que with staticEvtId enabled",
                static_cast<int32_t>(config.bufferNumber));
        });
        if ASCEND_IS_AIC {
            GetBuffImpl<srcPipe, false>(ret->bufId);
            ReleaseBuffImpl<srcPipe, false>(ret->bufId);
        }
        DEBUG_CODE(this->bufUsedCount++);
    } else {
        if constexpr (IsTscm(src, dst)) {
            static_assert(depth != 0, "cannot alloc buffer at tscm while depth is zero");
            if constexpr (srcHardType == Hardware::GM) {
                if ASCEND_IS_AIC {
                    GetBuffImpl<srcPipe, false>(ret->bufId);
                    ReleaseBuffImpl<srcPipe, false>(ret->bufId);
                    ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                        KERNEL_LOG(KERNEL_ERROR, "AllocBuffer is not matched with the previous state.");
                    });
                }
            } else if constexpr (srcHardType == Hardware::UB) {
#if defined(KFC_C310_SSBUF) && KFC_C310_SSBUF == 1
                if ASCEND_IS_AIV {
                    // Use enQueEvtId for representing TSCM Que inter-core sync, since no inner-core sync
                    // required for TSCM in AIV.
                    if (ret->freeBufEvtID != INVALID_TEVENTID) {
                        constexpr int32_t maxAllowedTscmAndMmCount = 10;
                        ASCENDC_ASSERT(
                            Internal::TSCM_CROSS_SYNC_ID_MAX - ret->enQueEvtID + 1 + g_matmulCount <=
                                maxAllowedTscmAndMmCount,
                            {
                                KERNEL_LOG(
                                    KERNEL_ERROR,
                                    "TSCM and Matmul Objects exceed the regulation that the total number "
                                    "should be within %d, "
                                    "while Matmul count is %d, buffer count is %d",
                                    maxAllowedTscmAndMmCount, g_matmulCount,
                                    Internal::TSCM_CROSS_SYNC_ID_MAX - ret->enQueEvtID + 1);
                            });
                        wait_intra_block(PIPE_MTE3, ret->enQueEvtID);
                    }
                }
#endif
            }
        } else {
            if ASCEND_IS_AIV {
                GetBuffImpl<srcPipe, false>(ret->bufId);
                ReleaseBuffImpl<srcPipe, false>(ret->bufId);
                if constexpr (config.consumerSize > 1) {
                    GetBuffImpl<srcPipe, false>(ret->bufIdAlt);
                    ReleaseBuffImpl<srcPipe, false>(ret->bufIdAlt);
                }
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "AllocBuffer is not matched with the previous state.");
                });
            } else {
                if constexpr (depth != 0) {
                    if (ret->freeBufEvtID != INVALID_TEVENTID) {
                        WaitFlag<freeBufEvt>(ret->freeBufEvtID);
                        GetTPipePtr()->ReleaseEventID<freeBufEvt>(ret->freeBufEvtID);
                        ret->freeBufEvtID = INVALID_TEVENTID;
                    }
                }
            }
        }
        if constexpr (depth != 0) {
            this->bufUsedCount++;
        }
    }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    constexpr Hardware bufferType = GetBufferPos(src, dst);
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
    AscendCBufAlloc(
        static_cast<uint8_t>(bufferType), static_cast<uint8_t>(GetPosition(src, dst)),
        reinterpret_cast<uint64_t>(absAddr + ret->address), static_cast<uint64_t>(ret->dataLen));
    if (this->bufPoolHandle != 0U) {
        AscendCUpdateTbufPoolStatus(this->bufPoolHandle, false);
        AscendCTBufPoolResetCheck(
            static_cast<uint8_t>(GetPosition(srcPosition, dstPosition)),
            reinterpret_cast<uint64_t>(absAddr + ret->address), static_cast<uint64_t>(ret->dataLen),
            this->bufPoolHandle);
    }
#endif // ASCENDC_CPU_DEBUG
    return reinterpret_cast<TBufHandle>(ret);
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::VacantInQue()
{
    static_assert((depth != 0), "VacantInQue api is not supported while depth is zero");
    return enableGlobalManageQue ? staticEnqueHead == INVALID_STATIC_ENQUE_HEAD : usedCount < depth;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::HasTensorInQue()
{
    static_assert((depth != 0), "HasTensorInQue api is not supported while depth is zero");
    return enableGlobalManageQue ? staticEnqueHead != INVALID_STATIC_ENQUE_HEAD : usedCount > 0;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline int32_t TQueBind<src, dst, depth, mask>::GetTensorCountInQue()
{
    static_assert((depth != 0), "GetTensorCountInQue api is not supported while depth is zero");
    return enableGlobalManageQue ? static_cast<int32_t>(staticEnqueHead == INVALID_STATIC_ENQUE_HEAD) : usedCount;
}
template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::HasIdleBuffer()
{
    static_assert((depth != 0), "HasIdleBuffer api is not supported while depth is zero");
    return enableGlobalManageQue ? freeMask != 0 : bufUsedCount < bufNum;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline void TQueBind<src, dst, depth, mask>::FreeAllEvent()
{
    static_assert((depth != 0), "can not use FreeAllEvent api while depth is zero");
    if ASCEND_IS_AIC {
        if constexpr (enableGlobalManageQue) {
            staticHead = 0;
            freeMask = ConstantsInternal::FULL_MASK_B32 >> (maxBlockNum - config.bufferNumber);
        } else {
            if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
                if ASCEND_IS_AIC {
                    constexpr uint8_t reservedBufId = 31;
                    GetBuffImpl<PIPE_MTE1, true>(reservedBufId);
                    ReleaseBuffImpl<PIPE_MTE1, true>(reservedBufId);
                    GetBuffImpl<PIPE_MTE2, false>(reservedBufId);
                    ReleaseBuffImpl<PIPE_MTE2, false>(reservedBufId);
                }
            } else {
                auto ptr = this->bufStart;
                for (int i = 0; i < this->bufNum; i++, ptr++) {
                    // should be in deque status
                    ASCENDC_ASSERT((ptr->enQueEvtID == INVALID_TEVENTID), {
                        KERNEL_LOG(KERNEL_ERROR, "enque event id can not be -1");
                    });
                    if (ptr->freeBufEvtID != INVALID_TEVENTID) {
                        WaitFlag<freeBufEvt>(ptr->freeBufEvtID);
                        GetTPipePtr()->ReleaseEventID<freeBufEvt>(ptr->freeBufEvtID);
                        ptr->freeBufEvtID = INVALID_TEVENTID;
                    }
                }
            }
        }
    }
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline TBuffAddr TQueBind<src, dst, depth, mask>::GetBufferAddr(TBufHandle buf)
{
    ASCENDC_ASSERT((GetPosition(src, dst) != TPosition::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be GM"); });
    auto ptr = reinterpret_cast<TBufType*>(buf);
    if constexpr (!enableGlobalManageQue) {
        ASCENDC_ASSERT((this->bufStart <= ptr && ptr < this->bufStart + this->bufNum), {
            KERNEL_LOG(
                KERNEL_ERROR, "ptr is %p, which should be in range [%p, %p)", ptr, this->bufStart,
                this->bufStart + this->bufNum);
        });
    }

    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(GetPosition(src, dst));
    addr.bufferHandle = buf;
    addr.bufferAddr = ptr->address;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (!enableGlobalManageQue) {
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(bufferType)].absAddr;
        addr.absAddr = absAddr + addr.bufferAddr;
    } else {
        constexpr Hardware bufferType = GetBufferPos(src, dst);
        uint8_t* ptr;
        if (bufferType == Hardware::GM) {
            ptr = ConstDefiner::Instance().cpuGM;
        } else {
            ptr = ConstDefiner::Instance().hardwareCpuBufferMap.at(bufferType);
        }
        addr.absAddr = ptr + addr.bufferAddr;
    }
#endif
    return addr;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline TBufState TQueBind<src, dst, depth, mask>::GetState(const LocalTensor<T>& tensor) const
{
    return GetState(tensor.GetBufferHandle());
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TQueBind<src, dst, depth, mask>::Buf2Tensor(TBufHandle buf)
{
    TBuffAddr addr = GetBufferAddr(buf);
    LocalTensor<T> tensor;
    tensor.SetAddr(addr);
    return tensor;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline TBufState TQueBind<src, dst, depth, mask>::GetState(const TBufHandle& handle) const
{
    if (handle == nullptr) {
        return TBufState::FREE;
    }
    auto ptr = reinterpret_cast<TBufType*>(handle);
    ASCENDC_ASSERT((this->bufStart <= ptr && ptr < this->bufStart + this->bufNum), {
        KERNEL_LOG(
            KERNEL_ERROR, "ptr is %p, which should be in range [%p, %p)", ptr, this->bufStart,
            this->bufStart + this->bufNum);
    });
    return ptr->state;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline void TQueBind<src, dst, depth, mask>::SetTBufPoolHandle(uint64_t bufPoolHandle)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    this->bufPoolHandle = bufPoolHandle;
#else
    (void)(bufPoolHandle);
#endif
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline void TQueBind<src, dst, depth, mask>::InitStartBufHandle(
    TBufHandle startBufhandle, uint8_t num, uint32_t len)
{
    static_assert(isTQue, "InitTQueAddr only support TQue class");
    ASCENDC_ASSERT((startBufhandle != nullptr), { KERNEL_LOG(KERNEL_ERROR, "bufhandle cannot be nullptr"); });
    auto ptr = reinterpret_cast<TBufType*>(startBufhandle);
    this->value = num;
    this->bufStart = ptr;
    DEBUG_CODE(this->bufLen = num * len);
    return;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline void TQueBind<src, dst, depth, mask>::InitBufHandle(
    T* bufPool, uint32_t index, TBufHandle bufhandle, uint32_t curPoolAddr, uint32_t len)
{
    static_assert(T::isTbufPool, "only Supports for TBufPool");
    auto ptr = reinterpret_cast<TBufType*>(bufhandle);
    uint8_t bufId = MAX_TBUFID + 1;
    uint32_t foundId = 0;
    while (foundId <= index) {
        bufId = sff1(bufPool->tBufPoolImpl.availableIdMask_);
        bufPool->tBufPoolImpl.availableIdMask_ = sbitset0(bufPool->tBufPoolImpl.availableIdMask_, bufId);
        foundId++;
    }
    bufPool->tBufPoolImpl.availableIdMask_ = bufPool->tBufPoolImpl.bufIdPool_;
    ASCENDC_ASSERT((bufId <= MAX_TBUFID), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %u, max buffer ID allocated is %u", static_cast<uint32_t>(bufId),
            static_cast<uint32_t>(MAX_TBUFID));
    });
    ptr->bufId = bufId;
    ASCENDC_ASSERT((bufhandle != nullptr), { KERNEL_LOG(KERNEL_ERROR, "bufhandle cannot be nullptr"); });
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    ptr->state = TBufState::FREE;
    ptr->freeBufEvt = freeBufEvt;
    if ASCEND_IS_AIV {
        ptr->bufId = bufId;
        ptr->bufIdAlt = INVALID_TBUFID;
    } else if constexpr (config.enableStaticEvtId) {
        ptr->bufId = bufId;
        ptr->bufIdAlt = INVALID_TBUFID;
    } else {
        ptr->enQueEvtID = INVALID_TEVENTID;
        ptr->freeBufEvtID = INVALID_TEVENTID;
    }
    ptr->address = curPoolAddr;
    ptr->dataLen = len;
    ptr->usertag = -1;
}

// begin impl of tbuf
template <TPosition pos>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TBuf<pos>::Get(uint32_t len)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    ASCENDC_ASSERT((len * sizeof(PrimType) % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be times of 32 Bytes", len);
    });
    ASCENDC_ASSERT(((len * sizeof(PrimType)) <= bufLen), {
        KERNEL_LOG(KERNEL_ERROR, "len is %lu, max buffer len is %u", len * sizeof(PrimType), bufLen);
    });
#endif
    auto ptr = this->bufStart;
    ptr->dataLen = len * sizeof(PrimType);
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = reinterpret_cast<TBufHandle>(ptr);
    addr.bufferAddr = ptr->address;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(GetPhyType(pos))].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
    AscendCBufGet(addr.logicPos, static_cast<uint8_t>(GetPhyType(pos)), reinterpret_cast<uint64_t>(addr.absAddr), len);
    if (this->bufPoolHandle != 0U) {
        AscendCUpdateTbufPoolStatus(this->bufPoolHandle, false);
        AscendCTBufPoolResetCheck(
            static_cast<uint8_t>(GetPhyType(pos)), reinterpret_cast<uint64_t>(absAddr + ptr->address),
            static_cast<uint64_t>(ptr->dataLen), this->bufPoolHandle);
    }
#endif
    LocalTensor<T> tensor;
    tensor.SetAddr(addr);
    return tensor;
}

template <TPosition pos>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TBuf<pos>::Get()
{
    return Get<T>(bufLen / sizeof(PrimT<T>));
}

template <TPosition pos>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TBuf<pos>::GetWithOffset(uint32_t size, uint32_t bufOffset)
{
    auto ptr = this->bufStart;
    ptr->dataLen = size * sizeof(T);
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = reinterpret_cast<TBufHandle>(ptr);
    addr.bufferAddr = ptr->address + bufOffset;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(pos));
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    LocalTensor<T> tensor;
    tensor.SetAddr(addr);
    return tensor;
}

template <TPosition pos>
__aicore__ inline void TBuf<pos>::SetTpipeBuf(TBufType* bufStartIn, uint32_t bufLenIn)
{
    this->bufStart = bufStartIn;
    this->bufLen = bufLenIn;
    this->offset = 0;
}

template <TPosition pos>
template <typename T>
__aicore__ inline void TBuf<pos>::EnQue(const LocalTensor<T>& tensor)
{
    (void)(0);
}

template <TPosition pos>
template <typename T>
__aicore__ inline LocalTensor<T> TBuf<pos>::DeQue()
{
    return Get<T>();
}

template <TPosition pos>
template <typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TBuf<pos>::AllocTensor()
{
    return Get<T>();
}

template <TPosition pos>
template <typename T>
__aicore__ inline void TBuf<pos>::FreeTensor(LocalTensor<T>& tensor)
{
    (void)(0);
}

template <TPosition pos>
template <typename T>
__aicore__ inline TBufState TBuf<pos>::GetState(const LocalTensor<T>& tensor) const
{
    TBufHandle handle = tensor.GetBufferHandle();
    if (handle == nullptr) {
        return TBufState::FREE;
    }
    auto ptr = reinterpret_cast<TBufType*>(handle);
    return ptr->state;
}

template <TPosition pos>
__aicore__ inline bool TBuf<pos>::EnQue(TBufHandle buf)
{
    return true;
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::DeQue()
{
    return Get();
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::AllocBuffer()
{
    return Get();
}

template <TPosition pos>
__aicore__ inline void TBuf<pos>::FreeBuffer(TBufHandle buf)
{
    (void)(0);
}

template <TPosition pos>
__aicore__ inline TBuffAddr TBuf<pos>::GetBufferAddr(TBufHandle buf)
{
    auto ptr = reinterpret_cast<TBufType*>(buf);
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = buf;
    addr.bufferAddr = ptr->address;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(GetPhyType(pos))].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    return addr;
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::Get(uint32_t len)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((len <= bufLen), { KERNEL_LOG(KERNEL_ERROR, "len is %u, max buffer len is %u", len, bufLen); });
#endif
    this->bufStart->dataLen = len;
    return reinterpret_cast<TBufHandle>(this->bufStart);
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::Get()
{
    return Get(bufLen);
}

template <TPosition pos>
__aicore__ inline uint32_t TBuf<pos>::GetBufLen() const
{
    return bufLen;
}

template <TPosition pos>
__aicore__ inline void TBuf<pos>::InitStartBufHandle(TBufHandle startBufhandle, uint8_t num, uint32_t len)
{
    ASCENDC_ASSERT((!isTQue), { KERNEL_LOG(KERNEL_ERROR, "InitStartBufHandle only support TBuf class"); });
    ASCENDC_ASSERT((startBufhandle != nullptr), { KERNEL_LOG(KERNEL_ERROR, "bufhandle cannot be nullptr"); });
    auto ptr = reinterpret_cast<TBufType*>(startBufhandle);
    this->bufStart = ptr;
    this->bufLen = len;
    this->offset = 0;
    return;
}

__aicore__ inline MutexID TPipe::AllocMutexID() { return ::AscendC::AllocMutexID(); }

__aicore__ inline void TPipe::ReleaseMutexID(MutexID id) { ::AscendC::ReleaseMutexID(id); }

__aicore__ inline int8_t TPipe::AllocCrossSyncId()
{
    int8_t syncId = this->g_tpipeImpl.crossSyncId_;
    this->g_tpipeImpl.crossSyncId_--;
    ASCENDC_ASSERT((syncId >= 0), { KERNEL_LOG(KERNEL_ERROR, "current id is %d, which must be >= 0", syncId); });
    return syncId;
}

__aicore__ inline TBufId TPipe::AllocTscmBufId() { return ::AscendC::AllocMutexID(); }

// begin impl of tBufPool
template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline TBufPoolExtImpl<pos, bufIDSize>::TBufPoolExtImpl()
{
    constexpr auto pool = GetPhyType(pos);
    static_assert(
        (pool == Hardware::L1 || pool == Hardware::UB || pool == Hardware::L0C),
        "TbufPool Position should be one of L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT/VECCALC)");
    ResetPool(); // init buf size and other variables
    tBufPoolImpl.isReset_ = false;
}

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline void TBufPoolExtImpl<pos, bufIDSize>::ResetPool()
{
    tBufPoolImpl.curBufSize_ = 0;
    tBufPoolImpl.startAddr_ = 0;
    tBufPoolImpl.maxAddr_ = 0;
    tBufPoolImpl.maxLen_ = 0;
}

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline void TBufPoolExtImpl<pos, bufIDSize>::Reset()
{
    auto ptr = this->tBufPoolImpl.buf_;
    if constexpr (GetPhyType(poolPos) == Hardware::UB) {
        PipeBarrier<PIPE_MTE2>();
        PipeBarrier<PIPE_MTE3>();
        PipeBarrier<PIPE_V>();
    } else {
        uint8_t i = 0;
        do {
            if (ptr->freeBufEvtID != INVALID_TEVENTID) {
                WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
                GetTPipePtr()->ReleaseEventID<freeBufEvt>(ptr->freeBufEvtID);
                ptr->freeBufEvtID = INVALID_TEVENTID;
            }
            i++;
            ptr++;
        } while (i < this->tBufPoolImpl.curBufSize_);
        auto bufId = this->tBufPoolImpl.bufIdPool_;
        GetBuffImpl<PIPE_MTE1, true>(bufId);
        ReleaseBuffImpl<PIPE_MTE1, true>(bufId);
        GetBuffImpl<PIPE_MTE2, false>(bufId);
        ReleaseBuffImpl<PIPE_MTE2, false>(bufId);
    }
    tBufPoolImpl.curBufSize_ = 0;
    tBufPoolImpl.maxAddr_ = tBufPoolImpl.startAddr_;

    tBufPoolImpl.isReset_ = true;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    AscendCUpdateTbufPoolStatus(reinterpret_cast<uint64_t>(&tBufPoolImpl), true);
#endif
}

template <TPosition pos, uint32_t bufIDSize>
template <class T>
__aicore__ inline bool TBufPoolExtImpl<pos, bufIDSize>::InitBuffer(T& que, uint8_t num, uint32_t len)
{
    static_assert((T::isTQue), "TBufPool::InitBuffer(T& que, uint8_t num, uint32_t len) not supports T as TBuf");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    que.value = num;
    que.bufStart = this->tBufPoolImpl.buf_ + this->tBufPoolImpl.curBufSize_;
    constexpr bool enableBufId = GetPhyType(poolPos) == Hardware::UB || T::config.enableStaticEvtId;
    DEBUG_CODE(que.bufLen = num * len);
    ASCENDC_ASSERT(
        (this->tBufPoolImpl.maxAddr_ + num * len <= this->tBufPoolImpl.startAddr_ + this->tBufPoolImpl.maxLen_), {
            KERNEL_LOG(
                KERNEL_ERROR, "Buffer Init length exceeds limit of BufPool. Max Length of BufPool is %u",
                this->tBufPoolImpl.maxLen_);
        });
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
    auto ptr = que.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
    ASCENDC_ASSERT(
        (pool == GetPhyType(pos)), { KERNEL_LOG(KERNEL_ERROR, "buffer pos should be same as pos of TbufPool"); });
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((num * len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", num * len, bufferInitLen.at(pool));
    });
    auto bufPos = GetPosition(T::srcPosition, T::dstPosition);
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCBufInit(static_cast<uint8_t>(bufPos), 0, num, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
    que.SetTBufPoolHandle(reinterpret_cast<uint64_t>(&tBufPoolImpl));
    ASCENDC_ASSERT((curPoolAddr + num * len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool));
    });
#endif
    for (int32_t i = 0; i < num; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->freeBufEvt = T::freeBufEvt;
        if constexpr (enableBufId) {
            uint8_t bufId = sff1(this->tBufPoolImpl.availableIdMask_);
            ASCENDC_ASSERT((bufId <= MAX_TBUFID), {
                KERNEL_LOG(
                    KERNEL_ERROR, "current id is %u, max buffer ID allocated is %u", static_cast<uint32_t>(bufId),
                    static_cast<uint32_t>(MAX_TBUFID));
            });
            ptr->bufId = bufId;
            this->tBufPoolImpl.availableIdMask_ = sbitset0(this->tBufPoolImpl.availableIdMask_, bufId);
            ptr->bufIdAlt = INVALID_TBUFID;
        } else {
            ptr->enQueEvtID = INVALID_TEVENTID;
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    this->tBufPoolImpl.curBufSize_ += num;
    ASCENDC_ASSERT((this->tBufPoolImpl.curBufSize_ <= bufIDSize), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, limits is %d", this->tBufPoolImpl.curBufSize_, bufIDSize);
    });
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
template <TPosition bufPos>
__aicore__ inline bool TBufPoolExtImpl<pos, bufIDSize>::InitBuffer(TBuf<bufPos>& buf, uint32_t len)
{
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr int32_t bufHandleSize = 1;
    buf.bufStart = this->tBufPoolImpl.buf_ + this->tBufPoolImpl.curBufSize_;
    buf.bufLen = len;
    buf.offset = 0;
    ASCENDC_ASSERT((this->tBufPoolImpl.maxAddr_ + len <= this->tBufPoolImpl.startAddr_ + this->tBufPoolImpl.maxLen_), {
        KERNEL_LOG(
            KERNEL_ERROR, "Buffer Init length exceeds limit of BufPool. Max Length of BufPool is %u",
            this->tBufPoolImpl.maxLen_);
    });
    constexpr auto pool = GetPhyType(bufPos);
    ASCENDC_ASSERT((GetPhyType(bufPos) == GetPhyType(pos)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer pos should be same as pos of TBufPool");
    });
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
    auto ptr = buf.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "len is %u, exceeds the limit %d", len, bufferInitLen.at(pool));
    });
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCBufInit(static_cast<uint8_t>(bufPos), 1, 1, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
    buf.SetTBufPoolHandle(reinterpret_cast<uint64_t>(&tBufPoolImpl));
#endif
    for (uint8_t i = 0; i < bufHandleSize; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->enQueEvtID = INVALID_TEVENTID;
        ptr->freeBufEvtID = INVALID_TEVENTID;
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }
    ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, exceeds the limit %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    this->tBufPoolImpl.curBufSize_ += bufHandleSize;
    ASCENDC_ASSERT((this->tBufPoolImpl.curBufSize_ <= bufIDSize), {
        KERNEL_LOG(
            KERNEL_ERROR, "current total buffer num is %d, exceeds the limit %d", this->tBufPoolImpl.curBufSize_,
            bufIDSize);
    });
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
template <class T>
__aicore__ inline bool TBufPoolExtImpl<pos, bufIDSize>::InitBufPool(T& bufPool, uint32_t len)
{
    static_assert(
        (T::isTbufPool), "TBufPool::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T as TbufPool");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr auto pool = GetPhyType(T::poolPos);
    bufPool.tBufPoolImpl.startAddr_ = this->tBufPoolImpl.maxAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = len;
    bufPool.tBufPoolImpl.bufIdPool_ = this->tBufPoolImpl.bufIdPool_ & this->tBufPoolImpl.availableIdMask_;
    bufPool.tBufPoolImpl.availableIdMask_ = bufPool.tBufPoolImpl.bufIdPool_;
    this->tBufPoolImpl.curBufSize_ += T::bufSize;
    ASCENDC_ASSERT((this->tBufPoolImpl.curBufSize_ <= bufIDSize), {
        KERNEL_LOG(KERNEL_ERROR, "InitBuffer numbers exceeds limits");
    });
    ASCENDC_ASSERT((this->tBufPoolImpl.maxAddr_ + len <= this->tBufPoolImpl.startAddr_ + this->tBufPoolImpl.maxLen_), {
        KERNEL_LOG(
            KERNEL_ERROR, "Buffer Init length exceeds limit of BufPool. Max Length of BufPool is %u",
            this->tBufPoolImpl.maxLen_);
    });
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", len, bufferInitLen.at(pool));
    });
    auto bufPos = T::poolPos;
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(bufPos), reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
    AscendCRecordPoolHierarchy(
        reinterpret_cast<uint64_t>(&this->tBufPoolImpl), reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    curPoolAddr += len;
    ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
template <class T, class U>
__aicore__ inline bool TBufPoolExtImpl<pos, bufIDSize>::InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
{
    static_assert(
        (T::isTbufPool && U::isTbufPool),
        "TBufPool::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T and U as TBufPool");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr auto pool = GetPhyType(T::poolPos);
    constexpr auto sharedPool = GetPhyType(U::poolPos);
    ASCENDC_ASSERT((pool == sharedPool), {
        KERNEL_LOG(KERNEL_ERROR, "Position of input bufPool should be same as position of shareBuf");
    });
    bufPool.tBufPoolImpl.startAddr_ = shareBuf.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = shareBuf.tBufPoolImpl.maxLen_;
    bufPool.tBufPoolImpl.bufIdPool_ = shareBuf.tBufPoolImpl.bufIdPool_ & shareBuf.tBufPoolImpl.availableIdMask_;
    bufPool.tBufPoolImpl.availableIdMask_ = bufPool.tBufPoolImpl.bufIdPool_;
    ASCENDC_ASSERT(
        (U::bufSize >= T::bufSize), { KERNEL_LOG(KERNEL_ERROR, "InitBufferSize cannot larger than shared TBufPool"); });
    ASCENDC_ASSERT((len <= shareBuf.tBufPoolImpl.maxLen_), {
        KERNEL_LOG(
            KERNEL_ERROR, "Length of input bufPool should be no longer than length of shareBuf, which is %u",
            shareBuf.tBufPoolImpl.maxLen_);
    });
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", len, bufferInitLen.at(pool));
    });
    auto bufPos = T::poolPos;
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(bufPos), reinterpret_cast<uint64_t>(bufPool.tBufPoolImpl.startAddr_ + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
    AscendCRecordPoolHierarchy(
        reinterpret_cast<uint64_t>(&this->tBufPoolImpl), reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline TBufPool<pos, bufIDSize>::~TBufPool()
{
    if (this->tBufPoolImpl.isReset_) {
        return;
    }
    if ASCEND_IS_AIC {
        auto ptr = this->tBufPoolImpl.buf_;
        for (uint8_t i = 0; i < this->tBufPoolImpl.curBufSize_; i++, ptr++) {
            if (ptr->freeBufEvtID != INVALID_TEVENTID) {
                WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
                ptr->freeBufEvtID = INVALID_TEVENTID;
            }
        }
    }
};

template <typename T>
__aicore__ inline uint64_t GetTQueHeadAddr(const T& que)
{
    static_assert(T::isTQue, "input Type must be a TQue relevant type");
    if constexpr (T::enableGlobalManageQue) {
        return 0;
    } else {
        auto ptr = que.bufStart;
        return ptr->address;
    }
}

template <TPosition pos>
__aicore__ inline uint64_t TransUBAddr(uint64_t addr)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    addr = addr - reinterpret_cast<uint64_t>(ConstDefiner::Instance().hardwareCpuBufferMap.at(positionHardMap.at(pos)));
#endif
    return addr;
}
} // namespace AscendC
#endif // ASCENDC_MODULE_TPIPE_IMPL_C310_VEC_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_C310_VEC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_C310_VEC_H__
#endif
