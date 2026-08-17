/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef CCU_KERNEL_SIGNATURE_H
#define CCU_KERNEL_SIGNATURE_H

#include <sstream>
#include <string>

namespace hcomm {

class CcuKernelSignature {
public:
    CcuKernelSignature() = default;
    ~CcuKernelSignature() = default;
    CcuKernelSignature(const CcuKernelSignature& other)
    {
        // 实现复制构造函数
        data << other.data.str();
    }

    void operator=(const CcuKernelSignature& other)
    {
        // 实现赋值操作
        data << other.data.str();
    }

    bool operator==(const CcuKernelSignature& rhs) const { return this == &rhs || data.str() == rhs.data.str(); }

    // 用法Append<T>(t)
    template <typename T>
    void Append(T t)
    {
        data << t;
    }

    void Append(const CcuKernelSignature& other) { data << other.data.str(); }

    std::string Describe() const { return "CcuKernelSignature[data=" + data.str() + "]"; }

    // 下掉CcuContext GetSignature
    std::string GetData() const { return data.str(); }

    friend class std::hash<hcomm::CcuKernelSignature>;

private:
    std::ostringstream data;
};

} // namespace hcomm

namespace std {

template <>
class hash<hcomm::CcuKernelSignature> {
public:
    size_t operator()(const hcomm::CcuKernelSignature& signature) const
    {
        auto dataHash = hash<string>{}(signature.GetData());
        constexpr size_t res = 17;
        constexpr size_t padding = 31;
        constexpr size_t preHash = res * padding;
        return preHash + dataHash;
    }
};

} // namespace std

#endif // _CCU_KERNEL_SIGNATURE_H
