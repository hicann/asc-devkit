/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include "queue.h"
#include <vector>
namespace Hccl {

template <typename T> class CircularQueue : public QueueWithSize<T> {
private:
    std::vector<T> elems_;
    size_t         head_;
    size_t         tail_;
    size_t         capacity_;

public:
    class Iterator : public Queue<T>::Iterator {
    private:
        CircularQueue *queue_{nullptr};

    protected:
        void check() override
        {
            if (queue_ == nullptr) {
                THROW<InternalException>(StringFormat("CircularQueue::Iterator queue_ is nullptr"));
            }
            if ((this->it_) == (queue_->elems_.begin() + queue_->tail_)) {
                this->it_ = queue_->elems_.end();
                return;
            }

            if (queue_->size_ == 0) {
                THROW<InternalException>(StringFormat("CircularQueue::Iterator queue_ is empty"));
            }
            size_t now = this->it_ - queue_->elems_.begin();
            if ((queue_->head_ < queue_->tail_) && !(queue_->head_ <= now && now < queue_->tail_)) {
                THROW<InternalException>(StringFormat("CircularQueue<T>::Iterator out of range"));
            }
            const size_t start_point = 0;
            if ((queue_->tail_ <= queue_->head_)
                && !((start_point <= now && now < queue_->tail_)
                     || (queue_->head_ <= now && now < queue_->capacity_))) {
                THROW<InternalException>(StringFormat("CircularQueue<T>::Iterator out of range"));
            }
        }

    public:
        Iterator(typename std::vector<T>::iterator it, CircularQueue *queue) : Queue<T>::Iterator(it), queue_(queue)
        {
            check();
        }

        ~Iterator() override = default;

        typename Queue<T>::Iterator &operator++() override
        {
            (this->it_) = ((this->it_) - queue_->elems_.begin() + 1) % queue_->capacity_ + queue_->elems_.begin();
            check();
            return *this;
        }

        typename Queue<T>::Iterator operator++(int) override
        {
            Iterator temp = *this;
            (this->it_)   = ((this->it_) - queue_->elems_.begin() + 1) % queue_->capacity_ + queue_->elems_.begin();
            check();
            return temp;
        }

        typename Queue<T>::Iterator &operator--() override
        {
            if (this->it_  == queue_->elems_.begin() + queue_->head_) {
                THROW<InternalException>(StringFormat("CircularQueue<T>::Iterator out of range"));
            }
            if (this->it_  == this->queue_->elems_.end()) {
                (this->it_) = queue_->elems_.begin() + queue_->tail_;
            }
            (this->it_) = ((this->it_) - queue_->elems_.begin() - 1 + queue_->capacity_) % queue_->capacity_
                          + queue_->elems_.begin();
            check();
            return *this;
        }

        typename Queue<T>::Iterator operator--(int) override
        {
            if (this->it_  == queue_->elems_.begin() + queue_->head_) {
                THROW<InternalException>(StringFormat("CircularQueue<T>::Iterator out of range"));
            }
            Iterator temp = *this;
            if (this->it_  == this->queue_->elems_.end()) {
                (this->it_) = queue_->elems_.begin() + queue_->tail_;
            }
            (this->it_)   = ((this->it_) - queue_->elems_.begin() - 1 + queue_->capacity_) % queue_->capacity_
                          + queue_->elems_.begin();
            check();
            return temp;
        }
    };

    explicit CircularQueue(size_t capacity)
        : elems_(capacity + 1), head_(0), tail_(0), capacity_(capacity + 1)
    {
        if (capacity_ == 0) {
            THROW<InternalException>(StringFormat("CircularQueue capacity cannot be zero"));
        }
    }

    ~CircularQueue() override
    {
        HCCL_INFO("[CircularQueue]Destroy");
    }

    void Append(T &&value) override
    {
        if (IsFull()) {
            head_ = (head_ + 1) % capacity_;
            this->size_--;
        }
        elems_[tail_] = std::move(value);
        tail_         = (tail_ + 1) % capacity_;
        this->size_++;
    }

    T& GetAndUpdate() override
    {
        if (IsFull()) {
            head_ = (head_ + 1) % capacity_;
            this->size_--;
        }
        auto curTail = tail_;
        tail_ = (tail_ + 1) % capacity_;
        this->size_++;
        return elems_[curTail];
    }

    void PopFront() override
    {
        if (this->IsEmpty()) {
            THROW<InternalException>(StringFormat("CircularQueue<T>::PopFront Queue is empty!"));
        }
        head_ = (head_ + 1) % capacity_;
        this->size_--;
    }

    void Traverse(std::function<void(const T &)> action) override
    {
        size_t i     = head_;
        size_t count = 0;
        while (count < this->size_) {
            action(elems_[i]);
            i = (i + 1) % capacity_;
            count++;
        }
    }

    bool IsFull() const override
    {
        return this->size_ == Capacity();
    }

    size_t Capacity() const override
    {
        return capacity_ - 1;
    }

    std::shared_ptr<typename Queue<T>::Iterator> Find(std::function<bool(const T &)> cond) override
    {
        size_t i     = head_;
        size_t count = 0;
        while (count < this->size_) {
            if (cond(elems_[i])) {
                return std::make_shared<Iterator>(elems_.begin() + i, this);
            }
            i = (i + 1) % capacity_;
            count++;
        }
        return std::make_shared<Iterator>(elems_.begin() + tail_, this);
    }

    std::shared_ptr<typename Queue<T>::Iterator> Begin() override
    {
        if (this->IsEmpty()) {
            HCCL_WARNING("[CircularQueue][Begin] Queue is empty!");
            return std::make_shared<Iterator>(elems_.begin() + tail_, this);
        }
        return std::make_shared<Iterator>(elems_.begin() + head_, this);
    }

    std::shared_ptr<typename Queue<T>::Iterator> Tail() override
    {
        if (this->IsEmpty()) {
            HCCL_WARNING("[CircularQueue][Tail] Queue is empty!");
            return std::make_shared<Iterator>(elems_.begin() + tail_, this);
        }
        return std::make_shared<Iterator>(elems_.begin() + (tail_ - 1 + capacity_) % capacity_, this);
    }

    std::shared_ptr<typename Queue<T>::Iterator> End() override
    {
        return std::make_shared<Iterator>(elems_.begin() + tail_, this);
    }
};

} // namespace Hccl
#endif // CIRCULAR_QUEUE_H
