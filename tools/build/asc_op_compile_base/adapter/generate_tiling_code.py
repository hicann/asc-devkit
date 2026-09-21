#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
generate tiling code
"""

from .global_storage import global_var_storage


def generate_pointer_directly_assess_data(is_dynamic: bool = True, is_micro=True, tiling_assign_str: str = ""):
    """generate code to access data directly by pointer"""

    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    # 310P can't use const qualifier due to compiler reason
    is_need_const = "" if short_soc_version == "Ascend310P" else "const"

    code = "// micro attribute for pointer assess data\n"
    if is_dynamic:
        code += f"""
#define __tiling_data_ptr__ __gm__ {is_need_const}

#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                           \
    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                             \
    __tiling_data_ptr__ tiling_struct *dst_ptr = (__tiling_data_ptr__ tiling_struct *)tiling_ptr;

#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                          \
    tiling_struct __ascendc_var##dst_ptr;                                                                                 \
    copy_data_align64((uint8_t*)(&__ascendc_var##dst_ptr), (__gm__ uint8_t*)src_ptr, sizeof(tiling_struct));              \
    {is_need_const} tiling_struct* dst_ptr = &__ascendc_var##dst_ptr;

#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                     \
    arr_type __ascendc_var##dst_ptr[arr_count];                                                                           \
    copy_data_align64((uint8_t*)(&__ascendc_var##dst_ptr), (__gm__ uint8_t*)src_ptr, sizeof(arr_type) * arr_count);       \
    {is_need_const} arr_type (*dst_ptr)[arr_count] = ({is_need_const} arr_type(*)[arr_count])&__ascendc_var##dst_ptr;

"""
    else:  # static tiling
        # tiling define by micro
        if is_micro:
            code += f"""
#define __tiling_data_ptr__ {is_need_const}

#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                                     \
    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                             \
    {tiling_assign_str % ("dst_ptr")}                                                                                           \
    {is_need_const} tiling_struct __var__##dst_ptr =                                                                            \
        convert_from_bytes<tiling_struct>(__ascendc_arr_##dst_ptr);                                                                             \
    __tiling_data_ptr__ tiling_struct *dst_ptr = &__var__##dst_ptr;

#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                                \
    {is_need_const} tiling_struct __ascendc_var##dst_ptr = *reinterpret_cast<{is_need_const} tiling_struct *>(src_ptr);         \
    {is_need_const} tiling_struct *dst_ptr = &__ascendc_var##dst_ptr;

#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                           \
    {is_need_const} struct __ascendc_struct_type##dst_ptr {{arr_type __ascendc_var_arr##dst_ptr[arr_count];}}                   \
                     __ascendc_var##dst_ptr = *({is_need_const} struct __ascendc_struct_type##dst_ptr *)src_ptr;                \
    {is_need_const} arr_type (*dst_ptr)[arr_count] =                                                                            \
                    ({is_need_const} arr_type(*)[arr_count])&__ascendc_var##dst_ptr.__ascendc_var_arr##dst_ptr;

"""

        else:
            code += f"""
#define __tiling_data_ptr__ {is_need_const}

#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                                     \
    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                             \
    {tiling_assign_str % ("dst_ptr")}                                                                                           \
    {is_need_const} tiling_struct __var__##dst_ptr =                                                                            \
        *reinterpret_cast<{is_need_const} tiling_struct *>(__ascendc_arr_##dst_ptr);                                            \
    __tiling_data_ptr__ tiling_struct *dst_ptr = &__var__##dst_ptr;

#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                                \
    {is_need_const} tiling_struct __ascendc_var##dst_ptr = *reinterpret_cast<{is_need_const} tiling_struct *>(src_ptr);         \
    {is_need_const} tiling_struct *dst_ptr = &__ascendc_var##dst_ptr;

#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                           \
    {is_need_const} struct __ascendc_struct_type##dst_ptr {{arr_type __ascendc_var_arr##dst_ptr[arr_count];}}                   \
                     __ascendc_var##dst_ptr = *({is_need_const} struct __ascendc_struct_type##dst_ptr *)src_ptr;                \
    {is_need_const} arr_type (*dst_ptr)[arr_count] =                                                                            \
                    ({is_need_const} arr_type(*)[arr_count])&__ascendc_var##dst_ptr.__ascendc_var_arr##dst_ptr;

"""
    return code


def generate_static_pointer_v1_constexpr():
    """generate code to access data directly by pointer v1 constexpr"""
    code = "// micro attribute for pointer assess data\n"
    code += """
#define __tiling_data_ptr__ const

#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                                     \\
    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                                       \\
    static constexpr tiling_struct __var__##dst_ptr;                                                                            \\
    __tiling_data_ptr__ tiling_struct *dst_ptr = &__var__##dst_ptr;

#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                                \\
    const tiling_struct __ascendc_var##dst_ptr = *reinterpret_cast<const tiling_struct *>(src_ptr);                             \\
    const tiling_struct *dst_ptr = &__ascendc_var##dst_ptr;

#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                           \\
    const struct __ascendc_struct_type##dst_ptr {arr_type __ascendc_var_arr##dst_ptr[arr_count];} \
__ascendc_var##dst_ptr = *(const struct __ascendc_struct_type##dst_ptr *)src_ptr;                                               \\
    const arr_type (*dst_ptr)[arr_count] =                                                                                      \\
                    (const arr_type(*)[arr_count])&__ascendc_var##dst_ptr.__ascendc_var_arr##dst_ptr;

"""
    return code
