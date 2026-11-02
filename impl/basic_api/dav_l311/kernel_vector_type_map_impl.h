/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_MODULE_DAV_L311_VECTOR_TYPE_MAP_IMPL_H
#define ASCENDC_MODULE_DAV_L311_VECTOR_TYPE_MAP_IMPL_H

namespace AscendC {
namespace Internal {
template <typename T>
struct VectorTypeMap;
template <>
struct VectorTypeMap<uint32_t> {
    using T = vector_u32;
};
template <>
struct VectorTypeMap<int32_t> {
    using T = vector_s32;
};
template <>
struct VectorTypeMap<float> {
    using T = vector_f32;
};
template <>
struct VectorTypeMap<uint16_t> {
    using T = vector_u16;
};
template <>
struct VectorTypeMap<half> {
    using T = vector_f16;
};
template <>
struct VectorTypeMap<int16_t> {
    using T = vector_s16;
};
template <>
struct VectorTypeMap<uint8_t> {
    using T = vector_u8;
};
template <>
struct VectorTypeMap<int8_t> {
    using T = vector_s8;
};

template <>
struct VectorTypeMap<bool> {
    using T = vector_s8;
};

template <>
struct VectorTypeMap<int4x2_t> {
    using T = vector_s4x2;
};
} // namespace Internal
} // namespace AscendC

#endif // ASCENDC_MODULE_DAV_L311_VECTOR_TYPE_MAP_IMPL_H
