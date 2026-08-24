/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MEMORY_ALLOC_RING_H
#define MEMORY_ALLOC_RING_H
#include <atomic>
#include <mutex>
#include <semaphore.h>
#include "log.h"
#include "hccl/base.h"

namespace hccl {
constexpr u32 EXPANSION_MULTIPLES = 2;
constexpr u32 RING_MEMORY_CAPACITY = 4096;

template <typename T>
class LocklessRingMemoryAllocate {
public:
    enum class OperateState {
        MEMORY_NULL = 0,    // 未申请内存
        MEMORY_PUTTING = 1, // 正在归还内存块
        MEMORY_VALID = 2,   // 可用的内存块
        MEMORY_TAKING = 3   // 正在取出内存块
    };

    explicit LocklessRingMemoryAllocate(size_t maxCapacity)
        : capacity_(maxCapacity), ringQueue_(nullptr), recordQueue_(nullptr), status_(nullptr), head_(0), tail_(0)
    {}

    void ResourseClear()
    {
        // 之前未加锁 多线程访问可能存在double free 另外本类内存管理过于复杂 后续考虑重构
        std::unique_lock<std::mutex> lock(initDesMutex_);
        if (recordQueue_ != nullptr) {
            for (size_t i = 0; i < capacity_; i++) {
                if (recordQueue_[i] != nullptr) {
                    delete reinterpret_cast<T*>(recordQueue_[i]);
                    recordQueue_[i] = nullptr;
                }
            }
            delete[] recordQueue_;
            recordQueue_ = nullptr;
        }

        if (ringQueue_ != nullptr) {
            delete[] ringQueue_;
            ringQueue_ = nullptr;
        }
        if (status_ != nullptr) {
            delete[] status_;
            status_ = nullptr;
        }
    }

    ~LocklessRingMemoryAllocate()
    {
        ResourseClear();
        sem_destroy(&allocAvailable_);
        sem_destroy(&freeAvailable_);
    }

    HcclResult Init()
    {
        std::unique_lock<std::mutex> lock(initDesMutex_);
        if (recordQueue_ != nullptr) {
            return HCCL_SUCCESS;
        }
        if (capacity_ > 0) {
            ringQueue_ = new (std::nothrow) T*[capacity_];
            recordQueue_ = new (std::nothrow) T*[capacity_];
            CHK_PTR_NULL(ringQueue_);
            CHK_PTR_NULL(recordQueue_);
            status_ = new (std::nothrow) std::atomic<OperateState>[capacity_];
            CHK_PTR_NULL(status_);
            for (size_t i = 0; i < capacity_; i++) {
                ringQueue_[i] = new (std::nothrow) T;
                CHK_PTR_NULL(ringQueue_[i]);
                status_[i] = OperateState::MEMORY_VALID;
                tail_++;
                recordQueue_[i] = ringQueue_[i];
            }
        } else {
            HCCL_ERROR("[LocklessRingMemoryAllocate]Capacity incorrect setting [%u]", capacity_);
            return HCCL_E_PARA;
        }

        auto allocRet = sem_init(&allocAvailable_, 0, capacity_);
        auto freerRet = sem_init(&freeAvailable_, 0, 0);
        if ((allocRet != 0) || (freerRet != 0)) {
            HCCL_ERROR("[LocklessRingMemoryAllocate] sem_init fail! allocRet[%u] freerRet[%u] ", allocRet, freerRet);
            ResourseClear();
            return HCCL_E_PARA;
        }
        return HCCL_SUCCESS;
    }

    T* Alloc()
    {
        if (Init() != HCCL_SUCCESS) {
            HCCL_ERROR("Init fail.");
            return nullptr;
        }
        HCCL_DEBUG("LocklessRingMemoryAllocate::Alloc Start");
        while (sem_trywait(&allocAvailable_) != 0) {
            HCCL_INFO("Alloc limited! head_[%u] tail_[%u]", head_ - 0, tail_ - 0);
            std::unique_lock<std::mutex> lock(expansionMutex_);
            int value;
            sem_getvalue(&freeAvailable_, &value);
            if ((head_ == tail_) && (static_cast<size_t>(value) == capacity_)) {
                sem_init(&freeAvailable_, 0, 0);
                CapacityExpansion();
            }
            lock.unlock();
        }
        T** position = nullptr;
        std::atomic<OperateState>* state = nullptr;
        while (true) {
            size_t index = (head_++) % capacity_;
            position = ringQueue_ + index;
            state = status_ + index;
            OperateState memoryValid = OperateState::MEMORY_VALID;
            if (!(state->compare_exchange_strong(memoryValid, OperateState::MEMORY_TAKING))) {
                HCCL_WARNING("[LocklessRingMemoryAllocate] Alloc fail!");
                continue;
            }
            break;
        }
        T* memoryBlock = *position;
        *position = nullptr;
        *state = OperateState::MEMORY_NULL;
        sem_post(&freeAvailable_);
        return memoryBlock;
    }

    HcclResult Free(T* memoryBlock)
    {
        while (sem_trywait(&freeAvailable_) != 0) {
            int value;
            sem_getvalue(&allocAvailable_, &value);
            if (static_cast<size_t>(value) == capacity_) {
                HCCL_WARNING("[LocklessRingMemoryAllocate] Free limited!");
                return HCCL_SUCCESS;
            }
        }
        T** position = nullptr;
        std::atomic<OperateState>* state = nullptr;
        while (true) {
            size_t index = (tail_++) % capacity_;
            position = ringQueue_ + index;
            state = status_ + index;
            OperateState memoryNull = OperateState::MEMORY_NULL;
            if (!(state->compare_exchange_strong(memoryNull, OperateState::MEMORY_PUTTING))) {
                HCCL_WARNING("[LocklessRingMemoryAllocate] Free fail!");
                continue;
            }
            break;
        }
        *position = memoryBlock;
        *state = OperateState::MEMORY_VALID;
        sem_post(&allocAvailable_);
        return HCCL_SUCCESS;
    }

private:
    size_t Length() const
    {
        size_t headPos = head_.load();
        size_t tailPos = tail_.load();
        if (headPos < tailPos) {
            return tailPos - headPos;
        } else {
            return 0;
        }
    }

    HcclResult CapacityExpansion()
    {
        size_t newCapacity = capacity_ * EXPANSION_MULTIPLES;
        size_t newHead = 0;
        T** newRingQueue = new (std::nothrow) T*[newCapacity];
        if (newRingQueue == nullptr) {
            ResourseClear();
            return HCCL_E_MEMORY;
        }

        T** newRecordQueue = new (std::nothrow) T*[newCapacity];
        if (newRecordQueue == nullptr) {
            delete[] newRingQueue;
            ResourseClear();
            return HCCL_E_MEMORY;
        }

        std::atomic<OperateState>* newStatus = new (std::nothrow) std::atomic<OperateState>[newCapacity];
        if (newStatus == nullptr) {
            delete[] newRingQueue;
            delete[] newRecordQueue;
            ResourseClear();
            return HCCL_E_MEMORY;
        }

        for (size_t i = tail_ - capacity_; i < tail_; i++) {
            newRingQueue[newHead] = nullptr;
            newStatus[newHead].store(status_[i % capacity_]);
            newRecordQueue[newHead] = recordQueue_[newHead];
            newHead++;
        }

        for (size_t i = newHead; i < newCapacity; i++) {
            newRingQueue[i] = new (std::nothrow) T;
            if (newRingQueue[i] == nullptr) {
                for (size_t j = newHead; j < i; j++) {
                    delete newRingQueue[j];
                }
                delete[] newStatus;
                delete[] newRingQueue;
                delete[] newRecordQueue;
                ResourseClear();
                return HCCL_E_MEMORY;
            }
            newRecordQueue[i] = newRingQueue[i];
            newStatus[i] = OperateState::MEMORY_VALID;
        }

        delete[] ringQueue_;
        ringQueue_ = newRingQueue;
        delete[] recordQueue_;
        recordQueue_ = newRecordQueue;
        delete[] status_;
        status_ = newStatus;
        head_ = newHead;
        tail_ = newCapacity;
        capacity_ = newCapacity;
        sem_init(&allocAvailable_, 0, newCapacity / EXPANSION_MULTIPLES);
        sem_init(&freeAvailable_, 0, newCapacity / EXPANSION_MULTIPLES);
        return HCCL_SUCCESS;
    }

    size_t capacity_ = 0;                         // 容量
    T** ringQueue_ = nullptr;                     // 内存块数组
    T** recordQueue_ = nullptr;                   // 内存记录
    std::atomic<OperateState>* status_ = nullptr; // 每一个内存块的状态
    std::atomic<size_t> head_;                    // 逻辑上的头
    std::atomic<size_t> tail_;                    // 逻辑上的尾
    sem_t allocAvailable_;                        // 可以申请的内存块个数
    sem_t freeAvailable_;                         // 可以释放的内存块个数
    std::mutex expansionMutex_;                   // 扩容锁
    std::mutex initDesMutex_;                     // 初始化析构锁
};
} // namespace hccl
#endif
