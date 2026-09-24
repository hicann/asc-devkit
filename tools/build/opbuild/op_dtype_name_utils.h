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
 * \file op_dtype_name_utils.h
 * \brief
 */

#ifndef STUB_OP_DTYPE_NAME_UTILS_H
#define STUB_OP_DTYPE_NAME_UTILS_H

#include "register/op_def.h"

namespace ops {
struct DataTypeName {
    ge::DataType type;
    const char* cfgName;
    const char* cpuCfgName;
};

enum class DataTypeNameMode { CFG, CPU_CFG };

constexpr DataTypeName DTYPE_NAMES[] = {
    {ge::DT_FLOAT, "float32", "DT_FLOAT"},
    {ge::DT_FLOAT16, "float16", "DT_FLOAT16"},
    {ge::DT_INT8, "int8", "DT_INT8"},
    {ge::DT_INT16, "int16", "DT_INT16"},
    {ge::DT_INT32, "int32", "DT_INT32"},
    {ge::DT_INT64, "int64", "DT_INT64"},
    {ge::DT_UINT1, "uint1", "DT_UINT1"},
    {ge::DT_UINT8, "uint8", "DT_UINT8"},
    {ge::DT_UINT16, "uint16", "DT_UINT16"},
    {ge::DT_UINT32, "uint32", "DT_UINT32"},
    {ge::DT_UINT64, "uint64", "DT_UINT64"},
    {ge::DT_BOOL, "bool", "DT_BOOL"},
    {ge::DT_DOUBLE, "double", "DT_DOUBLE"},
    {ge::DT_DUAL, "dual", "DT_DUAL"},
    {ge::DT_DUAL_SUB_INT8, "dual_sub_int8", "DT_DUAL_SUB_INT8"},
    {ge::DT_DUAL_SUB_UINT8, "dual_sub_uint8", "DT_DUAL_SUB_UINT8"},
    {ge::DT_STRING, "string", "DT_STRING"},
    {ge::DT_COMPLEX64, "complex64", "DT_COMPLEX64"},
    {ge::DT_COMPLEX128, "complex128", "DT_COMPLEX128"},
    {ge::DT_QINT8, "qint8", "DT_QINT8"},
    {ge::DT_QINT16, "qint16", "DT_QINT16"},
    {ge::DT_QINT32, "qint32", "DT_QINT32"},
    {ge::DT_QUINT8, "quint8", "DT_QUINT8"},
    {ge::DT_QUINT16, "quint16", "DT_QUINT16"},
    {ge::DT_RESOURCE, "resource", "DT_RESOURCE"},
    {ge::DT_STRING_REF, "string_ref", "DT_STRING_REF"},
    {ge::DT_INT4, "int4", "DT_INT4"},
    {ge::DT_INT2, "int2", nullptr},
    {ge::DT_BF16, "bfloat16", "DT_BF16"},
    {ge::DT_COMPLEX32, "complex32", "DT_COMPLEX32"},
    {ge::DT_HIFLOAT8, "hifloat8", "DT_HIFLOAT8"},
    {ge::DT_FLOAT8_E4M3FN, "float8_e4m3fn", "DT_FLOAT8_E4M3FN"},
    {ge::DT_FLOAT8_E5M2, "float8_e5m2", "DT_FLOAT8_E5M2"},
    {ge::DT_FLOAT8_E8M0, "float8_e8m0", "DT_FLOAT8_E8M0"},
    {ge::DT_FLOAT6_E3M2, "float6_e3m2", "DT_FLOAT6_E3M2"},
    {ge::DT_FLOAT6_E2M3, "float6_e2m3", "DT_FLOAT6_E2M3"},
    {ge::DT_FLOAT4_E2M1, "float4_e2m1", "DT_FLOAT4_E2M1"},
    {ge::DT_FLOAT4_E1M2, "float4_e1m2", "DT_FLOAT4_E1M2"},
    {ge::DT_HIFLOAT4, "hifloat4", "DT_HIFLOAT4"},
    {ge::DT_HIFLOAT4_SCALE, "hifloat4_scale", "DT_HIFLOAT4_SCALE"}};

inline const char* FindDataTypeName(const ge::DataType& type, const DataTypeNameMode mode)
{
    for (const auto& item : DTYPE_NAMES) {
        if (item.type == type) {
            const char* name = (mode == DataTypeNameMode::CFG) ? item.cfgName : item.cpuCfgName;
            return (name == nullptr) ? "unknow" : name;
        }
    }
    return "unknow";
}

inline const char* FindCfgDataTypeName(const ge::DataType& type)
{
    return FindDataTypeName(type, DataTypeNameMode::CFG);
}

inline const char* FindCpuCfgDataTypeName(const ge::DataType& type)
{
    return FindDataTypeName(type, DataTypeNameMode::CPU_CFG);
}
} // namespace ops

#endif
