/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_SERIALIZATION_H
#define HCCL_SERIALIZATION_H

#include <cstdint>
#include <algorithm>
#include <map>

#include "hccl/base.h"
#include "exception_util.h"
#include "log.h"

namespace Hccl {

class BinaryStream {
public:
    static constexpr std::ios_base::openmode DEFAULT_IOS_MODE = std::ios_base::in | std::ios_base::out;

    explicit BinaryStream(std::ios_base::openmode mode = DEFAULT_IOS_MODE) : stream(mode | std::ios_base::binary) {};

    explicit BinaryStream(std::vector<char>& buf, std::ios_base::openmode mode = DEFAULT_IOS_MODE)
        : stream(mode | std::ios_base::binary)
    {
        stream.rdbuf()->pubsetbuf(buf.data(), buf.size());
    }

    template <typename T>
    BinaryStream& operator<<(const T& t)
    {
        stream.write(reinterpret_cast<const char*>(&t), sizeof(T));
        return *this;
    }

    // 多级vector递归序列化
    template <typename T>
    BinaryStream& operator<<(const std::vector<T>& vec)
    {
        size_t size = vec.size();
        *this << size;
        for (const auto& elem : vec) {
            *this << elem;
        }
        return *this;
    }

    // 对string的输入函数
    BinaryStream& operator<<(const std::string& s)
    {
        size_t size = s.size();
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size_t)); // 写入长度
        stream.write(s.data(), size);                                       // 写入字符数据
        return *this;
    }

    template <typename T>
    BinaryStream& operator>>(T& t)
    {
        stream.read(reinterpret_cast<char*>(&t), sizeof(T));
        return *this;
    }

    // 对string的读取函数
    BinaryStream& operator>>(std::string& s)
    {
        size_t size = 0U;
        if (!ReadContainerSize(size, MAX_CONTAINER_BYTES)) {
            return *this;
        }
        s.resize(size); // 为string分配足够空间
        if (size > 0U) {
            stream.read(&s[0], size);
        }
        return *this;
    }

    // 多级vector递归反序列化
    template <typename T>
    BinaryStream& operator>>(std::vector<T>& vec)
    {
        size_t size = 0U;
        if (!ReadContainerSize(size, MAX_CONTAINER_ELEMENTS, sizeof(T))) {
            return *this;
        }
        vec.resize(size);
        for (auto& elem : vec) {
            *this >> elem;
            if (!stream.good()) {
                return *this;
            }
        }
        return *this;
    }

    void Dump(std::vector<char>& vec)
    {
        std::for_each(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), [&vec](const char c) {
            vec.push_back(c);
        });
    }

    void DumpWithRevert(std::vector<char>& vec)
    {
        std::streampos originalPos = stream.tellg(); // 保存原始位置
        std::for_each(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), [&vec](const char c) {
            vec.push_back(c);
        });
        stream.seekg(originalPos); // 恢复原始位置
    }

    std::uint64_t GetSize() { return stream.str().size(); }

    std::string GetString() { return stream.str(); }

    std::string SplictStream(u64& start, u64& end)
    {
        std::string temp = stream.str();
        if (start >= temp.length()) {
            HCCL_ERROR("[SplictStream]start[%llu] is bigger than stream length[%llu]", start, temp.length());
            return "";
        }

        // 截取子串
        std::string result = temp.substr(start, end - start);

        // 返回新的 string
        return result;
    }

    void Clear() { stream.clear(); }

private:
    bool ReadContainerSize(size_t& size, size_t maxSize, size_t elementSize = 1U)
    {
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (!stream.good() || elementSize == 0U || size > maxSize || size > MAX_CONTAINER_BYTES / elementSize ||
            stream.rdbuf()->in_avail() < static_cast<std::streamsize>(size)) {
            stream.setstate(std::ios_base::failbit);
            return false;
        }
        return true;
    }

    static constexpr size_t MAX_CONTAINER_ELEMENTS = 1024U * 1024U;
    static constexpr size_t MAX_CONTAINER_BYTES = 64U * 1024U * 1024U;
    std::stringstream stream;
};

} // namespace Hccl

#endif // HCCL_SERIALIZATION_H
