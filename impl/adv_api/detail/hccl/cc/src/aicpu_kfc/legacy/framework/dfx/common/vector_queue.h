/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef VECTOR_QUEUE_H
#define VECTOR_QUEUE_H

#include "queue.h"
#include <vector>

namespace Hccl {
constexpr uint32_t VECTOR_QUEUE_SIZE = 2048;

template <typename T> class VectorQueue : public QueueWithSize<T> {
private:
    std::vector<T> elems_;

public:
    class Iterator : public Queue<T>::Iterator {
    private:
        VectorQueue *queue_{nullptr};
        u32          index_ = 0;

    protected:
        void check() override
        {
            if ((this->index_) > queue_->size_) {
                THROW<InternalException>(StringFormat("VectorQueue<T>::Iterator out of range"));
            }
        }

    public:
        using pointer   = T *;
        using reference = T &;

        Iterator(VectorQueue *queue, u32 index) : queue_(queue), index_(index)
        {
            check();
        }

        ~Iterator() override = default;

        reference operator*() const override
        {
            if (this->index_ >= this->queue_->size_) {
                THROW<InternalException>(StringFormat("VectorQueue<T>::Iterator dereference out of range, index[%u], size[%zu]",
                                                       this->index_, this->queue_->size_));
            }
            return (this->queue_->elems_[this->index_]);
        }

        pointer operator->() const override
        {
            if (this->index_ >= this->queue_->size_) {
                THROW<InternalException>(StringFormat("VectorQueue<T>::Iterator dereference out of range, index[%u], size[%zu]",
                                                       this->index_, this->queue_->size_));
            }
            return &(this->queue_->elems_[this->index_]);
        }

        typename Queue<T>::Iterator &operator++() override
        {
            (this->index_)++;
            check();
            return *this;
        }

        typename Queue<T>::Iterator operator++(int) override
        {
            Iterator temp = *this;
            (this->index_)++;
            check();
            return temp;
        }

        typename Queue<T>::Iterator &operator--() override
        {
            (this->index_)--;
            check();
            return *this;
        }

        typename Queue<T>::Iterator operator--(int) override
        {
            Iterator temp = *this;
            (this->index_)--;
            check();
            return temp;
        }

        bool operator==(const typename Queue<T>::Iterator &other) const override
        {
            return this->index_ == static_cast<const Iterator&>(other).index_;
        }

        bool operator!=(const typename Queue<T>::Iterator &other) const override
        {
            return this->index_ != static_cast<const Iterator&>(other).index_;
        }
    };

    VectorQueue() : elems_(VECTOR_QUEUE_SIZE)
    {
    }

    ~VectorQueue() override
    {
        HCCL_INFO("[VectorQueue]Destroy");
    }

    void Append(T &&value) override
    {
        if (UNLIKELY(this->size_ >= VECTOR_QUEUE_SIZE)) {
            THROW<InternalException>(StringFormat("VectorQueue<T>::Append size[%zu] is full", this->size_));
        }
        elems_[this->size_] = std::move(value);
        this->size_++;
    }

    T& GetAndUpdate() override
    {
        if (UNLIKELY(this->size_ >= VECTOR_QUEUE_SIZE)) {
            THROW<InternalException>(StringFormat("VectorQueue<T>::GetAndUpdate size[%zu] is full", this->size_));
        }
        return elems_[this->size_++];
    }

    void Traverse(std::function<void(const T &)> action) override
    {
        for (size_t i = 0; i < this->size_; ++i) {
            action(elems_[i]);
        }
    }

    bool IsFull() const override
    {
        return this->size_ >= VECTOR_QUEUE_SIZE;
    }

    size_t Capacity() const override
    {
        return VECTOR_QUEUE_SIZE;
    }

    std::shared_ptr<typename Queue<T>::Iterator> Find(std::function<bool(const T &)> cond) override
    {
        for (size_t i = 0; i < this->size_; ++i) {
            if (cond(elems_[i])) {
                return std::make_shared<Iterator>(this, static_cast<u32>(i));
            }
        }
        return std::make_shared<Iterator>(this, static_cast<u32>(this->size_));
    }

    std::shared_ptr<typename Queue<T>::Iterator> Begin() override
    {
        return std::make_shared<Iterator>(this, 0);
    }

    std::shared_ptr<typename Queue<T>::Iterator> Tail() override
    {
        if (this->IsEmpty()) {
            HCCL_WARNING("[VectorQueue][Tail] Queue is empty!");
            return std::make_shared<Iterator>(this, 0);
        }
        return std::make_shared<Iterator>(this, static_cast<u32>(this->size_ - 1));
    }

    std::shared_ptr<typename Queue<T>::Iterator> End() override
    {
        return std::make_shared<Iterator>(this, static_cast<u32>(this->size_));
    }
};

} // namespace Hccl
#endif // VECTOR_QUEUE_H
