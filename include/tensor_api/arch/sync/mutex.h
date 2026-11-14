/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MUTEX_H
#endif

/*!
 * \file mutex.h
 * \brief Object-style pipeline synchronization interfaces.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_SYNC_MUTEX_H
#define INCLUDE_TENSOR_API_ARCH_SYNC_MUTEX_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace asc {
namespace te {

using mutex_id = uint8_t;

/**
 * @brief Pipeline synchronization handle. The caller manages the hardware ID.
 *
 * @tparam Pipe Pipeline associated with the hardware lock.
 */
template <pipe_t Pipe>
class mutex {
public:
    /**
     * @brief Constructs a pipeline synchronization handle.
     *
     * @param id Hardware lock ID.
     */
    __aicore__ inline explicit mutex(mutex_id id);
    __aicore__ inline void lock();
    __aicore__ inline void unlock();

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;
    mutex(mutex&&) = delete;
    mutex& operator=(mutex&&) = delete;

private:
    mutex_id id;
};

/**
 * @brief Scope guard that locks a mutex on construction and unlocks it on destruction.
 *
 * @tparam Mutex Mutex type managed by the guard.
 */
template <typename Mutex>
class lock_guard {
public:
    /**
     * @brief Constructs the guard and locks the referenced mutex.
     *
     * @param mutex Mutex object to manage.
     */
    __aicore__ inline explicit lock_guard(Mutex& mutex);
    __aicore__ inline ~lock_guard();

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;
    lock_guard(lock_guard&&) = delete;
    lock_guard& operator=(lock_guard&&) = delete;

private:
    Mutex& mutex;
};

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/sync/mutex_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_SYNC_MUTEX_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MUTEX_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MUTEX_H
#endif
