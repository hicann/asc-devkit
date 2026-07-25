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
 * \file kernel_tpipe_impl_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l300/kernel_tpipe_impl_l300_vec.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_L300_VEC_H__
#endif
#ifndef ASCENDC_MODULE_TPIPE_IMPL_L300_H
#define ASCENDC_MODULE_TPIPE_IMPL_L300_H
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_prof_trace_intf.h"

namespace AscendC {
namespace ConstantsInternal {
const uint32_t FULL_MASK_B32 = 0xffffffff;
} // namespace ConstantsInternal
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
inline uint8_t* GetBaseAddrCpu(int8_t logicPos) { return GetTPipePtr()->GetBaseAddr(logicPos); }
#endif

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
    static_assert(maxBufferBlock >= config.bufferNumber, "bufferNumber couldn't exceed limits.");
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
                KERNEL_ERROR, "buffer size is %d, exceed limits %d", config.bufferLen * config.bufferNumber,
                bufferInitLen.at(bufferType));
        });
        auto pos_ = GetPosition(src, dst);
        AscendCBufInit(
            static_cast<uint8_t>(pos_), 0, config.bufferNumber, reinterpret_cast<uint64_t>(ptr), config.bufferLen);
#endif
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
    constexpr pipe_t srcUserPipe = GetPipeByPos(srcUserPos);
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
        if constexpr (config.enableStaticEvtId) {
            DEBUG_CODE(this->usedCount++);
        } else {
            this->usedCount++;
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
                if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
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
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
template <typename T>
__aicore__ inline LocalTensor<T> TQueBind<src, dst, depth, mask>::DeQue()
{
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
template <TPosition srcUserPos, TPosition dstUserPos>
__aicore__ inline TBufHandle TQueBind<src, dst, depth, mask>::DeQue()
{
    static_assert(
        ((srcUserPos == TPosition::GM) || (srcUserPos == TPosition::VECIN) || (dstUserPos == TPosition::VECOUT) ||
         (dstUserPos == TPosition::VECCALC)),
        "DeQue only support src position GM/UB(VECIN/VECOUT/VECCALC) currently.");
    static_assert(
        ((dstUserPos == TPosition::GM) || (dstUserPos == TPosition::VECIN) || (dstUserPos == TPosition::VECOUT) ||
         (dstUserPos == TPosition::VECCALC)) &&
        "DeQue only support dst position GM/UB(VECIN/VECOUT/VECCALC) currently.");
    static_assert(
        !((srcUserPos == TPosition::GM) && (dstUserPos == TPosition::GM)) &&
        "DeQue src and dst position cannot be GM at the same time.");
    constexpr pipe_t dstUserPipe = GetPipeByPos(dstUserPos);
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
                    GetBuffImpl<GetPipeByPos(dst), true>(ptr->bufId);
                    ReleaseBuffImpl<GetPipeByPos(dst), true>(ptr->bufId);
                    GetBuffImpl<GetPipeByPos(config.consumer[idx]), true>(ptr->bufIdAlt);
                    ReleaseBuffImpl<GetPipeByPos(config.consumer[idx]), true>(ptr->bufIdAlt);
                } else {
                    GetBuffImpl<dstPipe, true>(ptr->bufId);
                    ReleaseBuffImpl<dstPipe, true>(ptr->bufId);
                }
                ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                    KERNEL_LOG(KERNEL_ERROR, "FreeBuffer is not matched with the previous state.");
                });
            } else {
                if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
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
                    KERNEL_ERROR, "size is %d, which exceed limits %d", size, static_cast<int32_t>(this->bufNum));
            });
        } while (true);
    }
    if constexpr (config.enableStaticEvtId && GetBufferPos(src, dst) == Hardware::L1) {
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
            if constexpr (srcHardType == Hardware::GM) {
                if ASCEND_IS_AIC {
                    GetBuffImpl<srcPipe, false>(ret->bufId);
                    ReleaseBuffImpl<srcPipe, false>(ret->bufId);
                    ASCENDC_ASSERT(BufIdTracker::GetInstance().GetState(), {
                        KERNEL_LOG(KERNEL_ERROR, "AllocBuffer is not matched with the previous state.");
                    });
                }
            } else if constexpr (srcHardType == Hardware::UB) {
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
                if (ret->freeBufEvtID != INVALID_TEVENTID) {
                    WaitFlag<freeBufEvt>(ret->freeBufEvtID);
                    GetTPipePtr()->ReleaseEventID<freeBufEvt>(ret->freeBufEvtID);
                    ret->freeBufEvtID = INVALID_TEVENTID;
                }
            }
        }
        this->bufUsedCount++;
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
    return enableGlobalManageQue ? staticEnqueHead == INVALID_STATIC_ENQUE_HEAD : usedCount < depth;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::HasTensorInQue()
{
    return enableGlobalManageQue ? staticEnqueHead != INVALID_STATIC_ENQUE_HEAD : usedCount > 0;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline int32_t TQueBind<src, dst, depth, mask>::GetTensorCountInQue()
{
    return enableGlobalManageQue ? static_cast<int32_t>(staticEnqueHead == INVALID_STATIC_ENQUE_HEAD) : usedCount;
}
template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline bool TQueBind<src, dst, depth, mask>::HasIdleBuffer()
{
    return enableGlobalManageQue ? freeMask != 0 : bufUsedCount < bufNum;
}

template <TPosition src, TPosition dst, int32_t depth, auto mask>
__aicore__ inline void TQueBind<src, dst, depth, mask>::FreeAllEvent()
{
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
    ASCENDC_ASSERT((startBufhandle != nullptr), { KERNEL_LOG(KERNEL_ERROR, "bufhandle couldn't be nullptr"); });
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
    uint8_t bufId = bufPool->tBufPoolImpl.bufIdPool_ + index;
    ASCENDC_ASSERT((bufhandle != nullptr), { KERNEL_LOG(KERNEL_ERROR, "bufhandle couldn't be nullptr"); });
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
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    ASCENDC_ASSERT((len * sizeof(T) % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be times of 32 Bytes", len);
    });
    ASCENDC_ASSERT(((len * sizeof(T)) <= bufLen), {
        KERNEL_LOG(KERNEL_ERROR, "len is %u, max buffer len is %u", len * sizeof(T), bufLen);
    });
#endif
    auto ptr = this->bufStart;
    ptr->dataLen = len * sizeof(T);
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
    return Get<T>(bufLen / sizeof(T));
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
    ASCENDC_ASSERT((startBufhandle != nullptr), { KERNEL_LOG(KERNEL_ERROR, "bufhandle couldn't be nullptr"); });
    auto ptr = reinterpret_cast<TBufType*>(startBufhandle);
    this->bufStart = ptr;
    this->bufLen = len;
    this->offset = 0;
    return;
}
// begin impl of tpipe
__aicore__ inline TPipe::TPipe()
{
    InitSocState();
    Init();
}

__aicore__ inline TPipe::~TPipe()
{
    if (g_tpipeImpl.isDestroy) {
        return;
    }
    Destroy();
};

__aicore__ inline void TPipe::Init()
{
    ResetPool();
    // for matmul macro, set flag M_MTE1 at the begining of operator, and also wait flag at the end.
    // matmul macro only use M_MTE1 event id 0 1 currently.
    if ASCEND_IS_AIC {
        auto enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_ASSERT((enQueEvtID == 0), { KERNEL_LOG(KERNEL_ERROR, "enQueEvtID should be 0"); });
        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
        enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_ASSERT((enQueEvtID == 1), { KERNEL_LOG(KERNEL_ERROR, "enQueEvtID should be 1"); });
        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
        // For load Bias
        enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_ASSERT((enQueEvtID == 2), { KERNEL_LOG(KERNEL_ERROR, "enQueEvtID should be 2"); });

        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
    }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++) {
        SetBufferCtx((Hardware)i, &g_tpipeImpl.bufPoolBaseAddr_[i]);
    }
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    AscendCBufAbsAddr(
        uint8_t(Hardware::UB), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuUB)),
        bufferInitLen.at(Hardware::UB));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L1), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL1)),
        bufferInitLen.at(Hardware::L1));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L0A), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0A)),
        bufferInitLen.at(Hardware::L0A));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L0B), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0B)),
        bufferInitLen.at(Hardware::L0B));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L0C), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0C)),
        bufferInitLen.at(Hardware::L0C));
    AscendCBufAbsAddr(
        uint8_t(Hardware::BIAS), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuBIAS)),
        bufferInitLen.at(Hardware::BIAS));
    AscendCBufAbsAddr(
        uint8_t(Hardware::FIXBUF),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuFIXBUF)),
        bufferInitLen.at(Hardware::FIXBUF));
#endif
#ifdef SPLIT_CORE_CUBE
    g_cubeTPipePtr = this;
#elif defined(SPLIT_CORE_VEC)
    g_vecTPipePtr = this;
#else
    g_tPipePtr = this;
#endif
    g_tpipeImpl.isDestroy = false;
}

template <class T>
__aicore__ inline bool TPipe::InitBuffer(T& que, uint8_t num, uint32_t len)
{
    static_assert((T::isTQue), "TPipe::InitBuffer(T& que, uint8_t num, uint32_t len) not supports T as TBuf");

    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    if constexpr (T::enableGlobalManageQue) {
        return true;
    } else {
        if constexpr (T::dstPosition == TPosition::TSCM) {
            return TscmInitBuffer(que, num, len);
        }
        constexpr bool useAltBufId = T::config.consumerSize > 1;
        len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        ASCENDC_ASSERT((T::config.bufferLen == 0 || T::config.bufferLen == len), {
            KERNEL_LOG(
                KERNEL_ERROR, "init buffer len %d must > 0 and <= bufferLen %d if bufferLen is > 0",
                static_cast<int32_t>(len), static_cast<int32_t>(T::config.bufferLen));
        });
        ASCENDC_ASSERT((num > 0 && (T::config.bufferNumber == 0 || T::config.bufferNumber == num)), {
            KERNEL_LOG(
                KERNEL_ERROR, "init buffer num %d must > 0 and <= bufferNumber %d if bufferNumber is > 0",
                static_cast<int32_t>(num), static_cast<int32_t>(T::config.bufferNumber));
        });

        que.value = num;
        que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
        DEBUG_CODE(que.bufLen = num * len);

        Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
        ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });
        ASCENDC_ASSERT((pool != Hardware::MAX), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::MAX"); });
        auto curPoolAddr = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
        auto ptr = que.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
        ASCENDC_ASSERT((num * len <= bufferInitLen.at(pool)), {
            KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceed limits %d", num * len, bufferInitLen.at(pool));
        });
        auto pos_ = GetPosition(T::srcPosition, T::dstPosition);
        auto absAddr = GetBaseAddr(static_cast<int8_t>(pos_));
        AscendCBufInit(static_cast<uint8_t>(pos_), 0, num, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
#endif
        for (int32_t i = 0; i < num; i++, ptr++) {
            ptr->state = TBufState::FREE;
            ptr->freeBufEvt = T::freeBufEvt;
            if ASCEND_IS_AIV {
                ptr->bufId = AllocBufId();
                if constexpr (useAltBufId) {
                    ptr->bufIdAlt = AllocBufId();
                } else {
                    ptr->bufIdAlt = INVALID_TBUFID;
                }
            } else {
                if constexpr (T::config.enableStaticEvtId) {
                    ptr->bufId = AllocBufId();
                    ptr->bufIdAlt = INVALID_TBUFID;
                } else {
                    ptr->enQueEvtID = INVALID_TEVENTID;
                    ptr->freeBufEvtID = INVALID_TEVENTID;
                }
            }
            ptr->address = curPoolAddr;
            ptr->dataLen = len;
            ptr->usertag = -1;
            curPoolAddr += len;
        }
        ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
            KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool));
        });
        this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
        this->g_tpipeImpl.curBufSize_ += num;
        ASCENDC_ASSERT((this->g_tpipeImpl.curBufSize_ < QBUF_MAX_LEN), {
            KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, limits is %d", this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN);
        });
        ASCENDC_ASSERT(
            (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr <=
             this->g_tpipeImpl.tscmBufferPtr_),
            {
                KERNEL_LOG(
                    KERNEL_ERROR, "tscm addr is %d, limits is %d", this->g_tpipeImpl.tscmBufferPtr_,
                    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr);
            });
        return true;
    }
}

template <TPosition pos>
__aicore__ inline bool TPipe::InitBuffer(TBuf<pos>& buf, uint32_t len)
{
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    buf.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    buf.bufLen = len;
    buf.offset = 0;

    constexpr auto pool = GetPhyType(pos);
    ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });

    auto curPoolAddr = g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
    auto ptr = buf.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "len is %u, exceed limits %d", len, bufferInitLen.at(pool));
    });
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCBufInit(static_cast<uint8_t>(pos), 1, 1, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
#endif
    ptr->state = TBufState::FREE;
    ptr->enQueEvtID = INVALID_TEVENTID;
    ptr->freeBufEvtID = INVALID_TEVENTID;
    ptr->address = curPoolAddr;
    ptr->dataLen = len;
    ptr->usertag = -1;
    curPoolAddr += len;
    ptr++;
    ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, exceed limits %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    this->g_tpipeImpl.curBufSize_ += 1;
    ASCENDC_ASSERT((this->g_tpipeImpl.curBufSize_ < QBUF_MAX_LEN), {
        KERNEL_LOG(
            KERNEL_ERROR, "current total buffer num is %d, exceed limits %d", this->g_tpipeImpl.curBufSize_,
            QBUF_MAX_LEN);
    });
    return true;
}

template <class T>
__aicore__ inline bool TPipe::InitBufPool(T& bufPool, uint32_t len)
{
    static_assert(
        (T::isTbufPool), "TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T as TbufPool");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr auto pool = GetPhyType(T::poolPos);
    constexpr uint32_t bufIdSize = T::bufSize;
    bufPool.tBufPoolImpl.startAddr_ = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = len;
    bufPool.tBufPoolImpl.bufIdPool_ = this->g_tpipeImpl.bufIdPool_;
    this->g_tpipeImpl.bufIdPool_ += bufIdSize;
    auto curPoolAddr = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceed limits %d", len, bufferInitLen.at(pool));
    });
    auto pos = T::poolPos;
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(pos), reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    curPoolAddr += len;
    ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    ASCENDC_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_), {
            KERNEL_LOG(
                KERNEL_ERROR, "tscm addr is %d, limits is %d", this->g_tpipeImpl.tscmBufferPtr_,
                this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr);
        });
    return true;
}

template <class T, class U>
__aicore__ inline bool TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
{
    static_assert(
        (T::isTbufPool && U::isTbufPool),
        "TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T and U as TBufPool");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr auto pool = GetPhyType(T::poolPos);
    ASCENDC_ASSERT((pool == GetPhyType(U::poolPos)), {
        KERNEL_LOG(KERNEL_ERROR, "Hardware type of input bufPool should be same with shareBuf");
    });
    static_assert((T::bufSize <= U::bufSize), "U bufIDSize must be > T bufIDSize");

    bufPool.tBufPoolImpl.startAddr_ = shareBuf.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = shareBuf.tBufPoolImpl.maxLen_;
    bufPool.tBufPoolImpl.bufIdPool_ = shareBuf.tBufPoolImpl.bufIdPool_;
    ASCENDC_ASSERT((len <= shareBuf.tBufPoolImpl.maxLen_), {
        KERNEL_LOG(
            KERNEL_ERROR, "Length of input bufPool should be shorter than len of shareBuf, which is %u",
            shareBuf.tBufPoolImpl.maxLen_);
    });
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceed limits %d", len, bufferInitLen.at(pool));
    });
    auto pos = T::poolPos;
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(pos), reinterpret_cast<uint64_t>(bufPool.tBufPoolImpl.startAddr_ + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    return true;
}

template <HardEvent evt>
__aicore__ inline TEventID TPipe::AllocEventID()
{
    ASCENDC_ASSERT(
        (evt < HardEvent::MAX), { KERNEL_LOG(KERNEL_ERROR, "illegal event %d", static_cast<int32_t>(evt)); });
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_ASSERT((lastId < QUE_MAX_EVENT && lastId >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT);
    });
    ptr->eventOccupy = sbitset1(ptr->eventOccupy, lastId);
    return lastId;
}

__aicore__ inline TBufId TPipe::AllocBufId()
{
    TBufId bufId = this->g_tpipeImpl.bufIdPool_++;
    if ASCEND_IS_AIV {
        ASCENDC_ASSERT((bufId <= MAX_TBUFID), {
            KERNEL_LOG(
                KERNEL_ERROR, "current id is %u, max buffer ID allocated is %u", static_cast<uint32_t>(bufId),
                static_cast<uint32_t>(MAX_TBUFID));
        });
    } else {
        ASCENDC_ASSERT((bufId < this->g_tpipeImpl.tscmBufIdPool_), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "L1 Buffer(TSCM) source from GM and L1 Buffer(A1/B1) with staticEvtID used is out of limits 20, "
                "current L1 Buffer(A1/B1) "
                "uses %u buffer",
                static_cast<uint32_t>(bufId + 1));
        });
    }
    return bufId;
}

__aicore__ inline int8_t TPipe::AllocCrossSyncId()
{
    int8_t syncId = this->g_tpipeImpl.crossSyncId_;
    this->g_tpipeImpl.crossSyncId_--;
    ASCENDC_ASSERT((syncId >= 0), { KERNEL_LOG(KERNEL_ERROR, "current id is %d, which must be >= 0", syncId); });
    return syncId;
}

__aicore__ inline TBufId TPipe::AllocTscmBufId()
{
    this->g_tpipeImpl.tscmBufIdPool_--;
    TBufId bufId = this->g_tpipeImpl.tscmBufIdPool_;
    ASCENDC_ASSERT((bufId <= MAX_TBUFID), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "L1 Buffer(TSCM) source from GM with staticEvtID used is out of limits, allocates over 20 buffers");
    });
    ASCENDC_ASSERT((bufId >= this->g_tpipeImpl.bufIdPool_), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "L1 Buffer(TSCM) source from GM and L1 Buffer(A1/B1) with staticEvtID used is out of limits 20, current L1 "
            "Buffer(TSCM) uses "
            "%u buffer",
            static_cast<uint32_t>(TSCM_BUFID_MAX - bufId));
    });
    return bufId;
}

template <HardEvent evt>
__aicore__ inline void TPipe::ReleaseEventID(TEventID id)
{
    ASCENDC_ASSERT((id >= 0 && id < QUE_MAX_EVENT), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %d, which should be larger than 0, and smaller than %d",
            static_cast<int32_t>(id), QUE_MAX_EVENT);
    });
    ASCENDC_ASSERT((evt != HardEvent::MAX), { KERNEL_LOG(KERNEL_ERROR, "evt cannot be HardEvent::MAX"); });
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    ptr->eventOccupy = sbitset0(ptr->eventOccupy, id);
    return;
}

__aicore__ inline TEventID TPipe::FetchEventID(HardEvent evt)
{
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_ASSERT((lastId < QUE_MAX_EVENT && lastId >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT);
    });
    return lastId;
}

template <HardEvent evt>
__aicore__ inline TEventID TPipe::FetchEventID()
{
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_ASSERT((lastId < QUE_MAX_EVENT && lastId >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT);
    });
    return lastId;
}

template <TPosition pos>
__aicore__ inline TBuffAddr TPipe::GetAbsAddr(int32_t offset, int32_t len) const
{
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = nullptr;
    addr.bufferAddr = offset;
    addr.dataLen = len;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    constexpr auto pool = GetPhyType(pos);
    ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });
    ASCENDC_ASSERT(((offset + len) <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "offset is %d, len is %d, exceed limits %d", offset, len, bufferInitLen.at(pool));
    });
    auto absAddr = this->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(pool)].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    return addr;
}

template <TPosition pos, typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TPipe::GetAbsAddr(int32_t offset, int32_t size) const
{
    TBuffAddr addr = GetAbsAddr<pos>(offset, static_cast<int32_t>((size * sizeof(T))));
    LocalTensor<T> tensor;
    tensor.SetAddr(addr);
    return tensor;
}

__aicore__ inline void InitShareBufStart(
    TPipe* tpipe, uint32_t mode, uint32_t* shareLens, uint32_t lens, uint8_t subBlockIdx)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((lens == static_cast<uint32_t>(TShareBuf::ShareHard::MAX)), {
        KERNEL_LOG(
            KERNEL_ERROR, "lens is %d, which should be %d", lens, static_cast<uint32_t>(TShareBuf::ShareHard::MAX));
    });
#else
    (void)(lens);
#endif

    ASCENDC_ASSERT((subBlockIdx == 0 || subBlockIdx == 1), {
        KERNEL_LOG(KERNEL_ERROR, "subBlockIdx is %d, which should only be 0/1", subBlockIdx);
    });
    tpipe->AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L1), Hardware::L1, subBlockIdx);
    tpipe->AuxShareBufStart(
        mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L0C), Hardware::L0C, subBlockIdx);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ = 2002)
    tpipe->AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::UB), Hardware::UB, subBlockIdx);
#endif
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0A)].maxAddr = 0;
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0B)].maxAddr = 0;
    // v100 Shouldn't Use Bias Table
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::BIAS)].maxAddr = 0;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    tpipe->g_tpipeImpl.sharedEvtId_ = tpipe->g_tpipeImpl.bufIdPool_;
#endif
    return;
}

__aicore__ inline void InitShareBufEnd(TPipe* tpipe)
{
    // debug methods need to be added.
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::L1)];
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0C)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::L0C)];
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ = 2002)
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::UB)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::UB)];
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    tpipe->g_tpipeImpl.bufIdPool_ = tpipe->g_tpipeImpl.sharedEvtId_;
#endif
    return;
}

template <typename T>
__aicore__ inline void TPipe::InitSpmBuffer(const GlobalTensor<T>& workspace, const int32_t bufferSize)
{
    g_tpipeImpl.spmInfo_.spmBuffSize = bufferSize;
    g_tpipeImpl.spmInfo_.spmAddr = reinterpret_cast<uint64_t>(workspace.GetPhyAddr());
    g_tpipeImpl.spmInfo_.spmBufType = static_cast<uint8_t>(Hardware::GM);
}

__aicore__ inline void TPipe::InitSpmBuffer(const int32_t bufferSize)
{
    (void)(bufferSize);
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "only support platform ascend910, ascend310p"); });
}

template <typename T>
__aicore__ inline void TPipe::WriteSpmBuffer(
    const LocalTensor<T>& writeLocal, const DataCopyParams& copyParams, int32_t writeOffset)
{
    /*
     * before write, the local may come frome MTE2/V, so need insert MTE3 wait V/MTE2
     * after write, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
    SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        DataCopyUB2GMImpl(
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), copyParams);
        event_t eventIDMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
        WaitFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((writeOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "writeOffset is %d, which must be 32B aligned", writeOffset);
        });
        DataCopyUB2L1Impl(
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), copyParams);
        event_t eventIDMTE3ToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
        SetFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
        WaitFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
    }
    event_t eventIDMTE3ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_V));
    SetFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
    WaitFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
}

template <typename T>
__aicore__ inline void TPipe::ReadSpmBuffer(
    const LocalTensor<T>& readLocal, const DataCopyParams& copyParams, int32_t readOffset)
{
    /*
     * before read, the local may be calculate, so need insert MTE wait V
     * after read, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        event_t eventIDVToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE2));
        event_t eventIDMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
        SetFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        WaitFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        DataCopyGM2UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, copyParams);
        SetFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);

        SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
        WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((readOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "readOffset is %d, which must be 32B aligned", readOffset);
        });
        event_t eventIDVToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE1));
        event_t eventIDMTE1ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_V));
        event_t eventIDMTE1ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_MTE3));
        SetFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        WaitFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        DataCopyL12UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, copyParams);

        SetFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);
        WaitFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);

        SetFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
        WaitFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
    }
}

template <typename T>
__aicore__ inline void TPipe::WriteSpmBuffer(
    const LocalTensor<T>& writeLocal, const int32_t writeSize, int32_t writeOffset)
{
    /*
     * before write, the local may come frome MTE2/V, so need insert MTE3 wait V/MTE2
     * after write, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    int computeSize = writeSize != 0 ? writeSize : GetShapeSize(writeLocal.GetShapeInfo());
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = computeSize / AscendCUtils::GetC0Count(sizeof(T));
    event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
    event_t eventIDMTE3ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_V));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMTE3);

    SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        DataCopyUB2GMImpl(
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), repeatParams);
        event_t eventIDMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
        WaitFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((writeOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "writeOffset is %d, which must be 32B aligned", writeOffset);
        });
        ASCENDC_ASSERT((writeSize % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "writeSize is %d, which must be 32B aligned", writeSize);
        });
        DataCopyUB2L1Impl(
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), repeatParams);
        event_t eventIDMTE3ToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
        SetFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
        WaitFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
    }

    SetFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
    WaitFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
}

template <typename T>
__aicore__ inline void TPipe::ReadSpmBuffer(const LocalTensor<T>& readLocal, const int32_t readSize, int32_t readOffset)
{
    /*
     * before read, the local may be calculate, so need insert MTE wait V
     * after read, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    int computeSize = readSize != 0 ? readSize : GetShapeSize(readLocal.GetShapeInfo());
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = computeSize / AscendCUtils::GetC0Count(sizeof(T));
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        event_t eventIDVToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE2));
        event_t eventIDMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
        SetFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        WaitFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        DataCopyGM2UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, repeatParams);

        SetFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);

        SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
        WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((readOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "readOffset is %d, which must be 32B aligned", readOffset);
        });
        ASCENDC_ASSERT((readSize % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "readSize is %d, which must be 32B aligned", readSize);
        });
        event_t eventIDVToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE1));
        event_t eventIDMTE1ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_V));
        event_t eventIDMTE1ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_MTE3));
        SetFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        WaitFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        DataCopyL12UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, repeatParams);

        SetFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);
        WaitFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);

        SetFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
        WaitFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
    }
}

template <TPosition pos>
__aicore__ inline uint64_t TPipe::GetQueueEndAddress()
{
    Hardware hardType = GetPhyType(pos);
    ASCENDC_ASSERT((hardType == Hardware::UB), { KERNEL_LOG(KERNEL_ERROR, "hardType should be UB"); });
    return this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardType)].maxAddr;
}

__aicore__ inline void TPipe::Destroy()
{
    if ASCEND_IS_AIC {
        g_tpipeImpl.isDestroy = true;
        auto ptr = this->g_tpipeImpl.buf_;
        for (uint8_t i = 0; i < this->g_tpipeImpl.curBufSize_; i++, ptr++) {
            if (ptr->freeBufEvtID != INVALID_TEVENTID) {
                WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
                ptr->freeBufEvtID = INVALID_TEVENTID;
            }
        }
        WaitFlag<HardEvent::M_MTE1>(0);
        ReleaseEventID<HardEvent::M_MTE1>(0);
        WaitFlag<HardEvent::M_MTE1>(1);
        ReleaseEventID<HardEvent::M_MTE1>(1);
        // For Bias
        WaitFlag<HardEvent::M_MTE1>(2);
        ReleaseEventID<HardEvent::M_MTE1>(2);
    }
}

__aicore__ inline void TPipe::Reset()
{
    if ASCEND_IS_AIC {
        auto ptr = this->g_tpipeImpl.buf_;
        for (uint8_t i = 0; i < this->g_tpipeImpl.curBufSize_; i++, ptr++) {
            if (ptr->freeBufEvtID != INVALID_TEVENTID) {
                WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
                ptr->freeBufEvtID = INVALID_TEVENTID;
            }
        }
    } else {
        GetBuffImpl<PIPE_V, true>(0);
        ReleaseBuffImpl<PIPE_V, true>(0);
        GetBuffImpl<PIPE_MTE3, false>(0);
        ReleaseBuffImpl<PIPE_MTE3, false>(0);
        pipe_barrier(PIPE_MTE2);
        pipe_barrier(PIPE_MTE3);
    }
    InitSocState();
    ResetPool();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++) {
        SetBufferCtx((Hardware)i, &g_tpipeImpl.bufPoolBaseAddr_[i]);
    }
#endif
}

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
template <typename T>
[[deprecated("NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use "
             "it!")]] inline uint64_t
TPipe::GetAbsAddr(const LocalTensor<T>& tensor)
{
    // Translates the CPU address to the actual physical address.
    // Currently, only L1 or UB address translation is supported.
    int8_t logicPos = tensor.GetPosition();
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", static_cast<int32_t>(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT(((hardType == Hardware::UB) || (hardType == Hardware::L1)), {
        KERNEL_LOG(KERNEL_ERROR, "illegal hardType %d ", static_cast<int32_t>(hardType));
    });
    uint8_t* phyAddr = reinterpret_cast<uint8_t*>(tensor.GetPhyAddr());
    uint8_t* baseAddr = static_cast<uint8_t*>(g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    ASCENDC_ASSERT((phyAddr >= baseAddr), {
        KERNEL_LOG(
            KERNEL_ERROR, "phyAddr is %p, baseAddr is %p, phyAddr should be larger than baseAddr", phyAddr, baseAddr);
    });
    uint64_t delta = phyAddr - baseAddr;
    if (hardType == Hardware::UB) {
        ASCENDC_ASSERT((delta < TMP_UB_OFFSET), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed ub limits %lu ", delta, TMP_UB_OFFSET);
        });
    } else {
        ASCENDC_ASSERT((delta < TOTAL_L1_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed l1 limits %lu", delta, TOTAL_L1_SIZE);
        });
    }
    return delta;
}

template <typename T>
inline uint64_t GetAbsAddr(TPipe* tpipe, const LocalTensor<T>& tensor)
{
    // Translates the CPU address to the actual physical address.
    // Currently, only L1 or UB address translation is supported.
    int8_t logicPos = tensor.GetPosition();
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", static_cast<int32_t>(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT(((hardType == Hardware::UB) || (hardType == Hardware::L1)), {
        KERNEL_LOG(KERNEL_ERROR, "illegal hardType %d ", static_cast<int32_t>(hardType));
    });
    uint8_t* phyAddr = reinterpret_cast<uint8_t*>(tensor.GetPhyAddr());
    uint8_t* baseAddr =
        static_cast<uint8_t*>(tpipe->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    ASCENDC_ASSERT((phyAddr >= baseAddr), {
        KERNEL_LOG(
            KERNEL_ERROR, "phyAddr is %p, baseAddr is %p, phyAddr should be larger than baseAddr", phyAddr, baseAddr);
    });
    uint64_t delta = phyAddr - baseAddr;
    if (hardType == Hardware::UB) {
        ASCENDC_ASSERT((delta < TMP_UB_OFFSET), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed ub limits %lu ", delta, TMP_UB_OFFSET);
        });
    } else {
        ASCENDC_ASSERT((delta < TOTAL_L1_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed l1 limits %lu", delta, TOTAL_L1_SIZE);
        });
    }
    return delta;
}

inline uint8_t* TPipe::GetBaseAddr(int8_t logicPos)
{
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", int32_t(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT((hardType != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "hardware position can not be gm"); });
    uint8_t* baseAddr = static_cast<uint8_t*>(g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    return baseAddr;
}

void inline TPipe::SetBufferCtx(Hardware hard, struct BufPoolExtra* bufPool)
{
    ASCENDC_ASSERT((hard != Hardware::MAX), { KERNEL_LOG(KERNEL_ERROR, "hard type can not be Hardware::MAX"); });
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((bufferInitLen.find(hard) != bufferInitLen.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal hard type %d", static_cast<int32_t>(hard));
    });
    uint8_t* ptr;
    if (hard == Hardware::GM) {
        ptr = ConstDefiner::Instance().cpuGM;
    } else {
        ptr = ConstDefiner::Instance().hardwareCpuBufferMap.at(hard);
    }
    {
        // init memory with random value
        std::default_random_engine e;
        int32_t* p = reinterpret_cast<int32_t*>(ptr);
        for (uint64_t i = 0; i < bufferInitLen.at(hard) / sizeof(int32_t); i++) {
            p[i] = e();
        }
    }
    bufPool->phySpace = bufferInitLen.at(hard);
    bufPool->absAddr = ptr;
    return;
}
#endif

__aicore__ inline void TPipe::InitSocState() const
{
    set_atomic_none();
    set_mask_norm();
    if ASCEND_IS_AIC {
        set_padding(static_cast<uint64_t>(0));
    } else {
        set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
    }
}

__aicore__ inline void TPipe::ResetPool()
{
    g_tpipeImpl.tscmBufferPtr_ = TOTAL_L1_SIZE;
    g_tpipeImpl.curBufSize_ = 0;
    g_tpipeImpl.bufIdPool_ = 0;
    g_tpipeImpl.tscmBufIdPool_ = TSCM_BUFID_MAX;
    g_tpipeImpl.crossSyncId_ = Internal::TSCM_CROSS_SYNC_ID_MAX;
    auto buf = g_tpipeImpl.bufPool_;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++, buf++) {
        buf->maxAddr = 0;
    }
#else
    if ASCEND_IS_AIV {
        buf[static_cast<int32_t>(Hardware::UB)].maxAddr = 0;
        buf[static_cast<int32_t>(Hardware::L1)].maxAddr = 0;
    } else {
        for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++, buf++) {
            buf->maxAddr = 0;
        }
    }
#endif
    auto evt = g_tpipeImpl.eventPool_;
    for (int32_t i = 0; i < EVENT_NUM; i++, evt++) {
        evt->eventOccupy = 0;
    }
    g_tpipeImpl.shareBufPool_.start[static_cast<uint8_t>(TShareBuf::ShareHard::L1)] = -1;
    g_tpipeImpl.shareBufPool_.start[static_cast<uint8_t>(TShareBuf::ShareHard::UB)] = -1;
    g_tpipeImpl.shareBufPool_.start[static_cast<uint8_t>(TShareBuf::ShareHard::L0C)] = -1;
}

template <class T>
__aicore__ inline bool TPipe::TscmInitBuffer(T& que, uint8_t num, uint32_t len)
{
    ASCENDC_ASSERT(((num * len) < TOTAL_L1_SIZE), {
        KERNEL_LOG(
            KERNEL_ERROR, "tscm buffer length is %u bytes, which is larger than total l1 size %u bytes", len * num,
            TOTAL_L1_SIZE);
    });
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    que.value = num;
    que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    DEBUG_CODE(que.bufLen = num * len);
    // Assign l1
    constexpr Hardware pool = Hardware::L1;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((num * len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer length %d is too large, the limit is %d", num * len, bufferInitLen.at(pool));
    });
#endif
    uint32_t curPoolAddr;
    if constexpr (T::scmBlockGroup) {
        curPoolAddr = g_tpipeImpl.tscmBufferPtr_ - num * len;
        g_tpipeImpl.tscmBufferPtr_ -= num * len;
    } else {
        curPoolAddr = g_tpipeImpl.tscmBufferPtr_ - (GetTaskRationImpl() - GetSubBlockIdxImpl()) * len * num;
        g_tpipeImpl.tscmBufferPtr_ -= GetTaskRationImpl() * num * len;
    }

    auto ptr = que.bufStart;
    for (int32_t i = 0; i < num; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->freeBufEvt = T::freeBufEvt;
        if constexpr (T::srcHardType == Hardware::GM) {
            ptr->bufId = AllocTscmBufId();
            ptr->bufIdAlt = INVALID_TBUFID;
        } else {
            ptr->enQueEvtID = AllocCrossSyncId();
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }
    ASCENDC_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_), {
            KERNEL_LOG(
                KERNEL_ERROR, "tscm addr %d overlapped with maxAddr %d", this->g_tpipeImpl.tscmBufferPtr_,
                this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr);
        });
    this->g_tpipeImpl.curBufSize_ += num;
    ASCENDC_ASSERT((this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN), {
        KERNEL_LOG(
            KERNEL_ERROR, "max buffer num is %d, current buf size %d exceed this limits", QBUF_MAX_LEN,
            this->g_tpipeImpl.curBufSize_);
    });
    return true;
}

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
        (pool == GetPhyType(pos)), { KERNEL_LOG(KERNEL_ERROR, "buffer pos should be same with pos of TbufPool"); });
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((num * len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceed limits %d", num * len, bufferInitLen.at(pool));
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
            ptr->bufId = this->tBufPoolImpl.bufIdPool_ + i;
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
        KERNEL_LOG(KERNEL_ERROR, "buffer pos should be same with pos of TBufPool");
    });
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
    auto ptr = buf.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "len is %u, exceed limits %d", len, bufferInitLen.at(pool));
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
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, exceed limits %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    this->tBufPoolImpl.curBufSize_ += bufHandleSize;
    ASCENDC_ASSERT((this->tBufPoolImpl.curBufSize_ <= bufIDSize), {
        KERNEL_LOG(
            KERNEL_ERROR, "current total buffer num is %d, exceed limits %d", this->tBufPoolImpl.curBufSize_,
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
    bufPool.tBufPoolImpl.bufIdPool_ = this->tBufPoolImpl.bufIdPool_ + this->tBufPoolImpl.curBufSize_;
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
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceed limits %d", len, bufferInitLen.at(pool));
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
        KERNEL_LOG(KERNEL_ERROR, "Position of input bufPool should be same with position of shareBuf");
    });
    bufPool.tBufPoolImpl.startAddr_ = shareBuf.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = shareBuf.tBufPoolImpl.maxLen_;
    bufPool.tBufPoolImpl.bufIdPool_ = shareBuf.tBufPoolImpl.bufIdPool_;
    ASCENDC_ASSERT((U::bufSize >= T::bufSize), {
        KERNEL_LOG(KERNEL_ERROR, "InitBufferSize couldn't larger than shared TBufPool");
    });
    ASCENDC_ASSERT((len <= shareBuf.tBufPoolImpl.maxLen_), {
        KERNEL_LOG(
            KERNEL_ERROR, "Length of input bufPool should be no longer than length of shareBuf, which is %u",
            shareBuf.tBufPoolImpl.maxLen_);
    });
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceed limits %d", len, bufferInitLen.at(pool));
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
} // namespace AscendC
#endif // ASCENDC_MODULE_TPIPE_IMPL_L300_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_L300_VEC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_L300_VEC_H__
#endif
