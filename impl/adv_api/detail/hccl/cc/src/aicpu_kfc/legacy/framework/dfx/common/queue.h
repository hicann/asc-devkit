/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <functional>
#include <iterator>
#include <vector>
#include <memory>
#include <task_info.h>
#include "exception_util.h"
#include "internal_exception.h"
#include "log.h"
namespace Hccl {

template <typename T>
class Queue {
public:
    class Iterator {
    protected:
        typename std::vector<T>::iterator it_;

    protected:
        virtual void check() { return; }

    public:
        using pointer = T*;
        using reference = T&;

        explicit Iterator(typename std::vector<T>::iterator it) : it_(it) {}

        Iterator() {}

        virtual ~Iterator() = default;

        virtual reference operator*() const { return *(this->it_); }

        virtual pointer operator->() const { return &*(this->it_); }

        virtual Iterator& operator++()
        {
            (this->it_)++;
            check();
            return *this;
        }

        virtual Iterator operator++(int)
        {
            Iterator temp(*this);
            ++it_;
            check();
            return temp;
        }

        virtual Iterator& operator--()
        {
            (this->it_)--;
            check();
            return *this;
        }

        virtual Iterator operator--(int)
        {
            Iterator temp(*this);
            --it_;
            check();
            return temp;
        }

        virtual bool operator==(const Iterator& other) const { return it_ == other.it_; }

        virtual bool operator!=(const Iterator& other) const { return it_ != other.it_; }
    };

    virtual ~Queue() = default;

    virtual void Append(T&& value) = 0;
    virtual T& GetAndUpdate() = 0; // 返回当前元素并更新index
    virtual void Traverse(std::function<void(const T&)> action) = 0;
    virtual size_t Size() const = 0;
    virtual bool IsEmpty() const = 0;
    virtual bool IsFull() const = 0;
    virtual size_t Capacity() const = 0;
    virtual std::shared_ptr<Iterator> Find(std::function<bool(const T&)> cond) = 0;
    virtual std::shared_ptr<Iterator> Begin() = 0;
    virtual std::shared_ptr<Iterator> Tail() = 0;
    virtual std::shared_ptr<Iterator> End() = 0;
    virtual void PopFront() { THROW<InternalException>(StringFormat("Queue<T>::PopFront () is not supported")); }
};

template <typename T>
class QueueWithSize : public Queue<T> {
protected:
    size_t size_ = 0;

public:
    size_t Size() const override { return size_; }

    bool IsEmpty() const override { return size_ == 0; }
};

} // namespace Hccl
#endif // QUEUE_H
