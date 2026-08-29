/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_SERIALIZATION
#define HCCL_SERIALIZATION

#include <vector>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <map>

#include "log.h"

namespace mc2_ops_hccl {

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

    // map序列化
    template <typename T1, typename T2>
    BinaryStream& operator<<(const std::map<T1, T2>& m)
    {
        size_t size = m.size();
        *this << size;
        for (const auto& elem : m) {
            *this << elem.first;
            *this << elem.second;
        }
        return *this;
    }

    // map反序列化
    template <typename T1, typename T2>
    BinaryStream& operator>>(std::map<T1, T2>& m)
    {
        size_t size = 0U;
        if (!ReadContainerSize(size, MAX_CONTAINER_ELEMENTS)) {
            return *this;
        }
        for (size_t i = 0; i < size; i++) {
            T1 key;
            T2 value;
            *this >> key >> value;
            if (!stream.good()) {
                return *this;
            }
            m[key] = value;
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

/*
 * 读写方向适配器。把方向从字段清单里剥离出来, 使Serialize与DeSerialize能共用同一份清单。
 */
class BinaryWriter {
public:
    explicit BinaryWriter(BinaryStream& stream) : stream_(stream) {}

    template <typename T>
    BinaryWriter& operator&(const T& t)
    {
        stream_ << t;
        return *this;
    }

private:
    BinaryStream& stream_;
};

class BinaryReader {
public:
    explicit BinaryReader(BinaryStream& stream) : stream_(stream) {}

    template <typename T>
    BinaryReader& operator&(T& t)
    {
        stream_ >> t;
        return *this;
    }

private:
    BinaryStream& stream_;
};

} // namespace mc2_ops_hccl

#endif // HCCL_SERIALIZATION
