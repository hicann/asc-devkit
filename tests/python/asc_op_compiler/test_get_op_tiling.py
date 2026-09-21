#!/usr/bin/python3
# coding=utf-8
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
import os
import sys
import unittest
from unittest import mock

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)

import asc_op_compile_base
from asc_op_compile_base.common.platform import set_current_compile_soc_info, get_soc_spec
from asc_op_compile_base.common import register
from asc_op_compile_base.common import buildcfg
from asc_op_compile_base.asc_op_compiler.get_op_tiling import *
import importlib

compile_op_module = importlib.import_module("asc_op_compile_base.asc_op_compiler.compile_op")


from asc_op_compile_base.asc_op_compiler.global_storage import global_var_storage


def SetCurrentSocInfo(soc: str):
    set_current_compile_soc_info(soc)
    global_var_storage.set_variable("ascendc_short_soc_version", get_soc_spec("SHORT_SOC_VERSION"))


class TestGetOpTiling(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    @mock.patch("ctypes.CDLL")
    @mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.get_custom_opp_pathlist")
    @mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.get_default_optiling_pathlist")
    @mock.patch("os.path.exists")
    def test_load_lib(self, mock_exist, mock_get_dft_pathlist, mock_get_opp_pathlist, mock_cdll):
        mock_exist.return_value = True
        mock_get_dft_pathlist.return_value = []
        mock_get_opp_pathlist.return_value = []
        mock_cdll.return_value = None
        self.assertIsNone(load_lib())

    @mock.patch("os.environ", {"ASCEND_CUSTOM_OPP_PATH": ["path_1", "path_2", "path_3"]})
    @mock.patch("os.path.exists")
    def test_get_custom_opp_pathlist(self, mock_exist):
        mock_exist.return_value = True
        self.assertEqual(get_custom_opp_pathlist(), ["['path_1', 'path_2', 'path_3']"])

    def test_gen_static_shape_v2(self):
        compile_op_module.global_var_storage.set_variable("ascendc_tiling_no_register", True)
        optype = "AddCustom"
        tiling_struct = "optiling::TilingData"
        tiling_raw_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00W\x04\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import gen_static_shape_v2

        SetCurrentSocInfo("Ascend910B1")
        class_body = gen_static_shape_v2(optype, tiling_struct, tiling_raw_data)
        compare_str = """#ifndef __ADDCUSTOM_HEADER__\n#define __ADDCUSTOM_HEADER__\n#include "kernel_tiling/kernel_tiling.h"\n#ifdef ASCENDC_CPU_DEBUG\n#include "kernel_log.h"\n#else\n#ifndef __aicore__\n#define __aicore__ [aicore]\n#endif\n#endif\n#define ASCENDC_INTERNAL_STR(x) #x \n#define ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(x) ASCENDC_INTERNAL_STR(x) \n#define ASCENDC_INTERNAL_CONCAT_IMPL(x, y) x##y \n#define ASCENDC_INTERNAL_CONCAT(x, y) ASCENDC_INTERNAL_CONCAT_IMPL(x, y) \n#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \\\n    static constexpr uint64_t ASCENDC_INTERNAL_CONCAT(__ascend_tiling_struct_,             ASCENDC_INTERNAL_CONCAT(TILING_KEY_VAR, counter)) \\\n__attribute__((used, section( \\\n        ".ascendc_tiling." \\\n        ASCENDC_INTERNAL_STR(tiling_struct) "_" \\\n        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(TILING_KEY_VAR) "." \\\n        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(counter) \\\n    ))) = sizeof(tiling_struct); \n#define GET_TILING_DATA(tiling_data, tiling_arg)                                         \n#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                  \\\n    const uint8_t __ascendc_arr_##tiling_data[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    \\\n    static constexpr tiling_struct tiling_data = convert_from_bytes<tiling_struct>(__ascendc_arr_##tiling_data); \n\n#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                            \\\n    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                                  \\\n    const uint8_t __ascendc_arr_##var[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; \\\n    static constexpr tiling_type __ascendc_point##var = convert_from_bytes<tiling_type>(__ascendc_arr_##var);\\\n    auto& var = __ascendc_point##var.member;\n\n// micro attribute for pointer assess data\n\n#define __tiling_data_ptr__ const\n\n#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                                         REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                                     const uint8_t __ascendc_arr_##dst_ptr[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                                                                                               const tiling_struct __var__##dst_ptr =                                                                                    *reinterpret_cast<const tiling_struct *>(__ascendc_arr_##dst_ptr);                                                __tiling_data_ptr__ tiling_struct *dst_ptr = &__var__##dst_ptr;\n\n#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                                    const tiling_struct __ascendc_var##dst_ptr = *reinterpret_cast<const tiling_struct *>(src_ptr);             const tiling_struct *dst_ptr = &__ascendc_var##dst_ptr;\n\n#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                               const struct __ascendc_struct_type##dst_ptr {arr_type __ascendc_var_arr##dst_ptr[arr_count];}                                        __ascendc_var##dst_ptr = *(const struct __ascendc_struct_type##dst_ptr *)src_ptr;                    const arr_type (*dst_ptr)[arr_count] =                                                                                                (const arr_type(*)[arr_count])&__ascendc_var##dst_ptr.__ascendc_var_arr##dst_ptr;\n\n#endif // __ADDCUSTOM_HEADER__\n"""

        self.assertEqual(class_body, compare_str)

        SetCurrentSocInfo("Ascend310P1")
        class_body = gen_static_shape_v2(optype, tiling_struct, tiling_raw_data)
        compare_str = """#ifndef __ADDCUSTOM_HEADER__\n#define __ADDCUSTOM_HEADER__\n#include "kernel_tiling/kernel_tiling.h"\n#ifdef ASCENDC_CPU_DEBUG\n#include "kernel_log.h"\n#else\n#ifndef __aicore__\n#define __aicore__ [aicore]\n#endif\n#endif\n#define ASCENDC_INTERNAL_STR(x) #x \n#define ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(x) ASCENDC_INTERNAL_STR(x) \n#define ASCENDC_INTERNAL_CONCAT_IMPL(x, y) x##y \n#define ASCENDC_INTERNAL_CONCAT(x, y) ASCENDC_INTERNAL_CONCAT_IMPL(x, y) \n#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \\\n    static constexpr uint64_t ASCENDC_INTERNAL_CONCAT(__ascend_tiling_struct_,             ASCENDC_INTERNAL_CONCAT(TILING_KEY_VAR, counter)) \\\n__attribute__((used, section( \\\n        ".ascendc_tiling." \\\n        ASCENDC_INTERNAL_STR(tiling_struct) "_" \\\n        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(TILING_KEY_VAR) "." \\\n        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(counter) \\\n    ))) = sizeof(tiling_struct); \n#define GET_TILING_DATA(tiling_data, tiling_arg)                                           \\\n    uint8_t __ascendc_arr_##tiling_data[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    \\\n    optiling::TilingData tiling_data = convert_from_bytes<optiling::TilingData>(__ascendc_arr_##tiling_data);\n\n#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n    uint8_t __ascendc_arr_##tiling_data[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    \\\n    tiling_struct tiling_data = convert_from_bytes<tiling_struct>(__ascendc_arr_##tiling_data);\n\n#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                           \\\n    uint8_t __ascendc_arr_##var[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    \\\n    tiling_type __ascendc_point##var = convert_from_bytes<tiling_type>(__ascendc_arr_##var);\\\n    auto& var = __ascendc_point##var.member;\n\n// micro attribute for pointer assess data\n\n#define __tiling_data_ptr__ \n\n#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                                         REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                                     uint8_t __ascendc_arr_##dst_ptr[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                                                                                                tiling_struct __var__##dst_ptr =                                                                                    *reinterpret_cast< tiling_struct *>(__ascendc_arr_##dst_ptr);                                                __tiling_data_ptr__ tiling_struct *dst_ptr = &__var__##dst_ptr;\n\n#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                                     tiling_struct __ascendc_var##dst_ptr = *reinterpret_cast< tiling_struct *>(src_ptr);              tiling_struct *dst_ptr = &__ascendc_var##dst_ptr;\n\n#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                                struct __ascendc_struct_type##dst_ptr {arr_type __ascendc_var_arr##dst_ptr[arr_count];}                                        __ascendc_var##dst_ptr = *( struct __ascendc_struct_type##dst_ptr *)src_ptr;                     arr_type (*dst_ptr)[arr_count] =                                                                                                ( arr_type(*)[arr_count])&__ascendc_var##dst_ptr.__ascendc_var_arr##dst_ptr;\n\n#endif // __ADDCUSTOM_HEADER__\n"""

        self.assertEqual(class_body, compare_str)

        SetCurrentSocInfo("Ascend950PR_9599")
        class_body = gen_static_shape_v2(optype, tiling_struct, tiling_raw_data)
        compare_str = """#ifndef __ADDCUSTOM_HEADER__\n#define __ADDCUSTOM_HEADER__\n#include "kernel_tiling/kernel_tiling.h"\n#ifdef ASCENDC_CPU_DEBUG\n#include "kernel_log.h"\n#else\n#ifndef __aicore__\n#define __aicore__ [aicore]\n#endif\n#endif\n#define ASCENDC_INTERNAL_STR(x) #x \n#define ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(x) ASCENDC_INTERNAL_STR(x) \n#define ASCENDC_INTERNAL_CONCAT_IMPL(x, y) x##y \n#define ASCENDC_INTERNAL_CONCAT(x, y) ASCENDC_INTERNAL_CONCAT_IMPL(x, y) \n#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \\\n    static constexpr uint64_t ASCENDC_INTERNAL_CONCAT(__ascend_tiling_struct_,             ASCENDC_INTERNAL_CONCAT(TILING_KEY_VAR, counter)) \\\n__attribute__((used, section( \\\n        ".ascendc_tiling." \\\n        ASCENDC_INTERNAL_STR(tiling_struct) "_" \\\n        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(TILING_KEY_VAR) "." \\\n        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(counter) \\\n    ))) = sizeof(tiling_struct); \n#define GET_TILING_DATA(tiling_data, tiling_arg)                                         \n#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                  \\\n    const uint8_t __ascendc_arr_##tiling_data[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    \\\n    static constexpr tiling_struct tiling_data = convert_from_bytes<tiling_struct>(__ascendc_arr_##tiling_data); \n\n#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                            \\\n    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                                  \\\n    const uint8_t __ascendc_arr_##var[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; \\\n    static constexpr tiling_type __ascendc_point##var = convert_from_bytes<tiling_type>(__ascendc_arr_##var);\\\n    auto& var = __ascendc_point##var.member;\n\n// micro attribute for pointer assess data\n\n#define __tiling_data_ptr__ const\n\n#define GET_TILING_DATA_PTR_WITH_STRUCT(tiling_struct, dst_ptr, tiling_ptr)                                                         REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                                                     const uint8_t __ascendc_arr_##dst_ptr[88] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 4, 0, 0, 0, 64, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                                                                                               const tiling_struct __var__##dst_ptr =                                                                                    *reinterpret_cast<const tiling_struct *>(__ascendc_arr_##dst_ptr);                                                __tiling_data_ptr__ tiling_struct *dst_ptr = &__var__##dst_ptr;\n\n#define COPY_TILING_WITH_STRUCT(tiling_struct, src_ptr, dst_ptr)                                                                    const tiling_struct __ascendc_var##dst_ptr = *reinterpret_cast<const tiling_struct *>(src_ptr);             const tiling_struct *dst_ptr = &__ascendc_var##dst_ptr;\n\n#define COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)                                                               const struct __ascendc_struct_type##dst_ptr {arr_type __ascendc_var_arr##dst_ptr[arr_count];}                                        __ascendc_var##dst_ptr = *(const struct __ascendc_struct_type##dst_ptr *)src_ptr;                    const arr_type (*dst_ptr)[arr_count] =                                                                                                (const arr_type(*)[arr_count])&__ascendc_var##dst_ptr.__ascendc_var_arr##dst_ptr;\n\n#endif // __ADDCUSTOM_HEADER__\n"""

        self.assertEqual(class_body, compare_str)

        compile_op_module.global_var_storage.set_variable("ascendc_tiling_no_register", False)
        class_body = gen_static_shape_v2(optype, tiling_struct, tiling_raw_data)

    def test_get_tiling_info_v2(self):
        op_info = OpInfo(
            kernel_name="test",
            op_type="AddCustomUnalign",
            inputs=[
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "x_in__",
                },
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "y_in__",
                },
            ],
            outputs=[
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "z_out_",
                }
            ],
            attrs=[
                {"name": "testAttr1", "dtype": "float", "value": 0.0},
                {"name": "testAttr2", "dtype": "int", "value": 0},
            ],
            impl_mode="high_performance",
            origin_inputs=[
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "x_in__",
                },
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "y_in__",
                },
            ],
            origin_outputs=[
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "z_out_",
                }
            ],
            param_type_dynamic=False,
            mc2_ctx=[],
            param_type_list=["required", "required", "required"],
            init_value_list=[None],
        )
        tiling_key_list = ["1"]
        default_tiling_struct = "tiling"
        tiling_struct_expr_map = {"1": "tiling"}

        from asc_op_compile_base.asc_op_compiler.get_op_tiling import get_tiling_info_v2, gen_dynamic_shape

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.get_struct_shape", return_value=""),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.get_dynamic_tiling_struct", return_value=""),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_copy_func_and_micro", return_value=""
            ),
        ):
            tiling_def = TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": []})
            gen_dynamic_shape(tiling_def, {})
            get_header_and_sub_struct_def(tiling_def, {})

        with asc_op_compile_base.common.context.op_context.OpContext(), buildcfg.build_config():
            tiling_info = get_tiling_info_v2(op_info, tiling_key_list, default_tiling_struct, tiling_struct_expr_map)
            self.assertEqual(tiling_info.static_shape_flag, False)

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.is_static_shape", return_value=True),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.gen_static_shape_v2", return_value=""),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.do_op_tiling",
                return_value={
                    "tiling_data": b"\x00",
                    "tiling_key": "1",
                    "block_dim": 1,
                    "clear_atomic": 0,
                    "schedule_mode": 0,
                    "workspaces": [1],
                    "ret_code": False,
                    "local_memory_size": 1024,
                },
            ),
        ):
            tiling_info = get_tiling_info_v2(op_info, tiling_key_list, default_tiling_struct, tiling_struct_expr_map)
            self.assertEqual(tiling_info.static_shape_flag, True)

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.is_static_shape", return_value=True),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.gen_static_shape_v2", return_value=""),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.do_op_tiling",
                return_value={
                    "tiling_data": b"\x00",
                    "tiling_key": "1",
                    "block_dim": 1,
                    "clear_atomic": 0,
                    "schedule_mode": 0,
                    "workspaces": [1, 2],
                    "ret_code": False,
                    "local_memory_size": 1024,
                },
            ),
            mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.raise_tbe_python_err", return_value=None),
        ):
            tiling_info = get_tiling_info_v2(op_info, tiling_key_list, default_tiling_struct, tiling_struct_expr_map)
            self.assertEqual(tiling_info.static_shape_flag, True)

            self.assertIsNotNone(tiling_info.local_memory_size)
            self.assertEqual(tiling_info.local_memory_size, 1024)

    class MockClass:
        def TbeLoadSoAndSaveToRegistry(input):
            pass

    def test_load_so_fail(self):
        with mock.patch("os.environ", {"_TILING_SO_PATH": "abcd"}):
            with mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_default_optiling_pathlist",
                return_value=list("aabbcc"),
            ):
                self.assertIsNotNone(load_lib())
        with mock.patch("os.environ", {"_TILING_SO_PATH": "abcd"}):
            with mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_default_optiling_pathlist",
                return_value=list("aabbcc"),
            ):
                with mock.patch("os.path.exists", return_value=True):
                    with mock.patch("ctypes.CDLL") as MockClass:
                        self.assertIsNotNone(load_lib())

    def test_gen_static_shape(self):
        field_0 = {"classType": "0", "name": "testDefault", "dtype": "uint8_t"}
        field_1 = {"classType": "1", "name": "testArr", "dtype": "uint8_t", "arrSize": "256"}
        field_2 = {
            "classType": "2",
            "name": "testStruct",
            "dtype": "struct",
            "structType": "uint8_t",
            "structSize": "256",
        }
        tiling_def = TilingDef({"class_name": "add_custiom", "data_size": 4096, "fields": [field_0, field_1, field_2]})
        all_dynamic_struct_def_except_self = gen_all_dynamic_struct_def_except_self(
            False, None, None, "AddCustom", None
        )
        with mock.patch(
            "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def",
            return_value=TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": [field_0, field_1]}),
        ) as mock_get_tiling_def:
            struct_base = get_struct_tiling_info(tiling_def, {})
        self.assertRaises(
            Exception, gen_static_shape, tiling_def, {""}, struct_base, all_dynamic_struct_def_except_self
        )
        tiling_data = {
            "testDefault": b"\x01\x00\x02\x00\n\x00\x02\x00\x08\x00",
            "testArr": b"\x01\x00\x02\x00\n\x00\x02\x00\x08\x00" * 128,
            "testStruct": {
                "testDefault": b"\x01\x00\x02\x00\n\x00\x02\x00\x08\x00",
                "testArr": b"\x01\x00\x02\x00\n\x00\x02\x00\x08\x00" * 128,
            },
        }
        source = " "

        tiling_raw_data = tiling_data["testDefault"]
        code = gen_static_shape(tiling_def, tiling_raw_data, struct_base, all_dynamic_struct_def_except_self)
        self.assertNotEqual(source, code)

        tiling_raw_data = tiling_data["testArr"]
        code = gen_static_shape(tiling_def, tiling_raw_data, struct_base, all_dynamic_struct_def_except_self)
        self.assertNotEqual(source, code)

        SetCurrentSocInfo("Ascend310P1")
        tiling_raw_data = tiling_data["testDefault"]
        gen_static_shape(tiling_def, tiling_raw_data, struct_base, all_dynamic_struct_def_except_self)
        self.assertNotEqual(source, code)

        tiling_raw_data = tiling_data["testArr"]
        gen_static_shape(tiling_def, tiling_raw_data, struct_base, all_dynamic_struct_def_except_self)
        self.assertNotEqual(source, code)

    def test_get_struct_shape(self):
        field_0 = {"classType": "0", "name": "testDefault", "dtype": "uint8_t"}
        field_1 = {"classType": "1", "name": "testArr", "dtype": "uint8_t", "arrSize": "256"}
        field_2 = {
            "classType": "2",
            "name": "testStruct",
            "dtype": "struct",
            "structType": "CustStruct",
            "structSize": "256",
        }
        tiling_def = TilingDef({"class_name": "add_custiom", "data_size": 4096, "fields": [field_2]})
        with mock.patch("asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def") as mock_get_tiling_def:
            mock_get_tiling_def.side_effect = [
                None,
                TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": [field_0]}),
            ]
            struct_base = get_struct_tiling_info(tiling_def, {})
            source = " "
            self.assertNotEqual(source, get_struct_shape(struct_base))

        compare_str = """class CustStruct
{
public:
    uint8_t testDefault = 0;
    AscendC::tiling::ApiStruct testApiStruct;
};

"""
        field_3 = {
            "classType": "2",
            "name": "testApiStruct",
            "dtype": "struct",
            "structType": "ApiStruct",
            "structSize": "256",
        }
        cust_tiling_def = TilingDef({"class_name": "CustStruct", "data_size": 0, "fields": [field_0, field_3]})
        cust_tiling_def.depth = 1

        api_tiling_def = TilingDef({"class_name": "ApiStruct", "data_size": 256, "fields": [field_0]})
        api_tiling_def.is_api = True
        api_tiling_def.depth = 2
        struct_base = {"CustStruct": cust_tiling_def, "ApiStruct": api_tiling_def}
        self.assertEqual(compare_str, get_struct_shape(struct_base))

    def test_get_dynamic_tiling_struct(self):
        field_0 = {"classType": "0", "name": "testDefault", "dtype": "uint8_t"}
        field_1 = {"classType": "1", "name": "testArr", "dtype": "uint8_t", "arrSize": "256"}
        field_2 = {
            "classType": "2",
            "name": "testStruct",
            "dtype": "struct",
            "structType": "CustStruct",
            "structSize": "256",
        }
        field_3 = {
            "classType": "2",
            "name": "testApiStruct",
            "dtype": "struct",
            "structType": "ApiStruct",
            "structSize": "256",
        }
        tiling_def = TilingDef({"class_name": "add_custiom", "data_size": 4096, "fields": [field_0, field_2, field_3]})
        compare_str = """class add_custiom
{
public:
    uint8_t testDefault;
    CustStruct testStruct;
    AscendC::tiling::ApiStruct testApiStruct;
}__attribute__((__may_alias__));

"""
        cust_tiling_def = TilingDef({"class_name": "CustStruct", "data_size": 0, "fields": [field_0, field_3]})
        cust_tiling_def.depth = 1

        api_tiling_def = TilingDef({"class_name": "ApiStruct", "data_size": 256, "fields": [field_0]})
        api_tiling_def.is_api = True
        api_tiling_def.depth = 2

        struct_base = {"CustStruct": cust_tiling_def, "ApiStruct": api_tiling_def}
        self.assertEqual(compare_str, get_dynamic_tiling_struct(tiling_def, struct_base))

    def test_get_tiling_info(self):
        SetCurrentSocInfo("Ascend910B1")
        op_info = OpInfo(
            kernel_name="te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5",
            op_type="AddCustom",
            inputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "y_in__",
                },
            ],
            outputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            attrs=[],
            impl_mode="",
            origin_inputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "y_in__",
                },
            ],
            origin_outputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            param_type_dynamic=False,
            mc2_ctx=[],
            param_type_list=["required", "required", "required"],
            init_value_list=["0"],
            output_shape_depend_on_compute=[],
        )

        op_info_no_static = OpInfo(
            kernel_name="te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5",
            op_type="AddCustom",
            inputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                }
            ],
            outputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            attrs=[],
            impl_mode="",
            origin_inputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                }
            ],
            origin_outputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            param_type_dynamic=False,
            mc2_ctx=[],
            param_type_list=["required", "required", "required"],
            init_value_list=["0"],
            output_shape_depend_on_compute=[],
        )

        field_0 = {"classType": "0", "name": "testDefault", "dtype": "uint8_t"}
        field_1 = {"classType": "1", "name": "testArr", "dtype": "uint8_t", "arrSize": "256"}
        field_2 = {
            "classType": "2",
            "name": "testStruct",
            "dtype": "struct",
            "structType": "uint8_t",
            "structSize": "256",
        }

        from asc_op_compile_base.asc_op_compiler.get_op_tiling import TilingDef
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import get_tiling_info

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def", return_value=None
            ) as mock_get_tiling_def,
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.do_op_tiling",
                return_value={
                    "tiling_data": b"\x00",
                    "tiling_key": "0",
                    "block_dim": 1,
                    "clear_atomic": 0,
                    "schedule_mode": 0,
                    "workspaces": [],
                    "ret_code": False,
                },
            ) as mock_do_op_tiling,
        ):
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, -1)
            self.assertEqual(tiling_info.clear_atomic, 1)
            self.assertEqual(tiling_info.schedule_mode, 0)

            mock_get_tiling_def.return_value = TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": []})
            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x00",
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [4096],
                "ret_code": False,
            }
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, 1)
            self.assertEqual(tiling_info.clear_atomic, 0)
            self.assertEqual(tiling_info.schedule_mode, 0)

            mock_get_tiling_def.return_value = TilingDef(
                {"class_name": "add_custiom", "data_size": 4096, "fields": [field_0, field_1]}
            )
            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x01\x00\x02\x00\n\x00\x02\x00\x08\x00" * 1024,
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [8192],
                "ret_code": False,
            }
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, 1)
            self.assertEqual(tiling_info.clear_atomic, 0)
            self.assertEqual(tiling_info.schedule_mode, 0)

            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x00",
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [1024, 1024],
                "ret_code": False,
            }
            self.assertRaises(Exception, get_tiling_info, op_info, ["1"])

            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x00",
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [1024],
                "ret_code": False,
            }
            tiling_info = get_tiling_info(op_info_no_static, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, -1)
            self.assertEqual(tiling_info.clear_atomic, 1)
            self.assertEqual(tiling_info.schedule_mode, 0)
            tiling_info = get_tiling_info(op_info_no_static, [], None, False, {})
            self.assertEqual(tiling_info.block_num, -1)
            self.assertEqual(tiling_info.clear_atomic, 1)
            self.assertEqual(tiling_info.schedule_mode, 0)

    def test_get_tiling_info_c310(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        op_info = OpInfo(
            kernel_name="te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5",
            op_type="AddCustom",
            inputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "y_in__",
                },
            ],
            outputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            attrs=[],
            impl_mode="",
            origin_inputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "y_in__",
                },
            ],
            origin_outputs=[
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            param_type_dynamic=False,
            mc2_ctx=[],
            param_type_list=["required", "required", "required"],
            init_value_list=["0"],
            output_shape_depend_on_compute=[],
        )

        op_info_no_static = OpInfo(
            kernel_name="te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5",
            op_type="AddCustom",
            inputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                }
            ],
            outputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            attrs=[],
            impl_mode="",
            origin_inputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                }
            ],
            origin_outputs=[
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "z_out_",
                }
            ],
            param_type_dynamic=False,
            mc2_ctx=[],
            param_type_list=["required", "required", "required"],
            init_value_list=["0"],
            output_shape_depend_on_compute=[],
        )

        field_0 = {"classType": "0", "name": "testDefault", "dtype": "uint8_t"}
        field_1 = {"classType": "1", "name": "testArr", "dtype": "uint8_t", "arrSize": "256"}
        field_2 = {
            "classType": "2",
            "name": "testStruct",
            "dtype": "struct",
            "structType": "uint8_t",
            "structSize": "256",
        }

        from asc_op_compile_base.asc_op_compiler.get_op_tiling import TilingDef
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import get_tiling_info

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def", return_value=None
            ) as mock_get_tiling_def,
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.do_op_tiling",
                return_value={
                    "tiling_data": b"\x00",
                    "tiling_key": "0",
                    "local_memory_size": 1,
                    "block_dim": 1,
                    "clear_atomic": 0,
                    "schedule_mode": 0,
                    "workspaces": [],
                    "ret_code": False,
                },
            ) as mock_do_op_tiling,
        ):
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, -1)
            self.assertEqual(tiling_info.clear_atomic, 1)
            self.assertEqual(tiling_info.schedule_mode, 0)

            mock_get_tiling_def.return_value = TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": []})
            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x00",
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [4096],
                "ret_code": False,
            }
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, 1)
            self.assertEqual(tiling_info.clear_atomic, 0)
            self.assertEqual(tiling_info.schedule_mode, 0)

            mock_get_tiling_def.return_value = TilingDef(
                {"class_name": "add_custiom", "data_size": 4096, "fields": [field_0, field_1]}
            )
            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x01\x00\x02\x00\n\x00\x02\x00\x08\x00" * 1024,
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [8192],
                "ret_code": False,
            }
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, 1)
            self.assertEqual(tiling_info.clear_atomic, 0)
            self.assertEqual(tiling_info.schedule_mode, 0)

            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x00",
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [1024, 1024],
                "ret_code": False,
            }
            self.assertRaises(Exception, get_tiling_info, op_info, ["1"])

            mock_do_op_tiling.return_value = {
                "tiling_data": b"\x00",
                "tiling_key": "0",
                "block_dim": 1,
                "clear_atomic": 0,
                "schedule_mode": 0,
                "workspaces": [1024],
                "ret_code": False,
            }
            tiling_info = get_tiling_info(op_info_no_static, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, -1)
            self.assertEqual(tiling_info.clear_atomic, 1)
            self.assertEqual(tiling_info.schedule_mode, 0)
            tiling_info = get_tiling_info(op_info_no_static, [])
            self.assertEqual(tiling_info.block_num, -1)
            self.assertEqual(tiling_info.clear_atomic, 1)
            self.assertEqual(tiling_info.schedule_mode, 0)

    def test_get_tiling_declaration(self):
        get_tiling_declaration("AddCustom")
        with mock.patch(
            "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def",
            return_value=TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": []}),
        ) as mock_get_tiling_def:
            tiling_info = TilingInfo()
            self.assertNotEqual(tiling_info, get_tiling_declaration("AddCustom"))

    def test_get_bytes_by_type(self):
        self.assertEqual(get_bytes_by_type("int8_t"), 1)
        self.assertRaises(Exception, get_bytes_by_type, "test")

    # inf + nan 场景
    def test_decode_inf(self):
        tiling_data = b"\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x80\x7f\x00\x00\xc0\x7f\x00\x00\x80\x7f\x00\x00\x80\xff"
        fmt = {"totalLength": "uint", "tileNum": "uint", "testValue": "float", "testValueArray": [3, "float"]}
        res, offset = decode(tiling_data, fmt)
        # dict_res = {'totalLength': 16384, 'tileNum': 8, 'testValue': 'float(1.0 / 0.0)',
        #             'testValueArray': ('float(0.0 / 0.0)', 'float(1.0 / 0.0)', 'float(-1.0 / 0.0)')}
        self.assertEqual(res["testValue"], "float(1.0 / 0.0)")  # inf
        self.assertEqual(
            res["testValueArray"], ("float(0.0 / 0.0)", "float(1.0 / 0.0)", "float(-1.0 / 0.0)")
        )  # nan, inf, -inf

    def test_get_dynamic_assign_tiling_data_by_bytes(self):
        total_bytes = 19 * 5
        prefix_0 = "__test_0__"
        prefix_1 = "__test_1__"
        source = ""
        self.assertNotEqual(source, get_dynamic_assign_tiling_data_by_bytes(total_bytes, prefix_0, prefix_1))

    def test_get_dynamic_assign_tiling_data(self):
        field_0 = {"classType": "0", "name": "testDefault", "dtype": "uint8_t"}
        field_1 = {"classType": "1", "name": "testArr", "dtype": "uint8_t", "arrSize": "4096"}
        field_2 = {
            "classType": "2",
            "name": "testStruct",
            "dtype": "struct",
            "structType": "uint32_t",
            "structSize": "4096",
        }
        Field0 = Field(field_0)
        Field1 = Field(field_1)
        Field2 = Field(field_2)

        tiling_def = TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": [field_0, field_1, field_2]})
        with mock.patch(
            "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def",
            return_value=TilingDef({"class_name": "add_custiom", "data_size": 0, "fields": [field_0, field_1]}),
        ) as mock_get_tiling_def:
            struct_base = get_struct_tiling_info(tiling_def, {})
        _, offset_npu = get_dynamic_npu_assign_tiling_data(struct_base, [Field0, Field1, Field2], "__test__")
        _, offset_cpu = get_dynamic_cpu_assign_tiling_data(struct_base, [Field0, Field1, Field2], "__test__")
        self.assertEqual(8194, offset_npu)
        self.assertEqual(8194, offset_cpu)

    def test_is_static_shape(self):
        origin_inputs = [
            [
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
                {
                    "shape": (-1, 2048),
                    "ori_shape": (8, -2),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
            ],
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        outputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "z_out_",
            }
        ]
        res = is_static_shape(origin_inputs, outputs)
        self.assertEqual(res, False)

        origin_inputs = [
            [
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
                {
                    "shape": (7, 0),
                    "ori_shape": (8, 0),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "x_in__",
                },
            ],
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        outputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "z_out_",
            }
        ]
        res = is_static_shape(origin_inputs, outputs)
        self.assertEqual(res, True)

        origin_inputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "x_in__",
            },
            {
                "shape": (-2, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        outputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "z_out_",
            }
        ]
        res = is_static_shape(origin_inputs, outputs)
        self.assertEqual(res, False)

        origin_inputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "x_in__",
            },
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        outputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "z_out_",
            }
        ]
        res = is_static_shape(origin_inputs, outputs)
        self.assertEqual(res, True)

    def test_is_static_shape_with_value_depend(self):
        origin_inputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "x_in__",
            },
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        outputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "z_out_",
            }
        ]
        param_list = ["optional", "optional"]
        value_depends = {0: "optional"}
        res = is_static_shape(origin_inputs, outputs, value_depends, param_list, True)
        self.assertEqual(res, False)

        # optional with const value
        origin_inputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "const_value": (1, 2, 3),
                "param_name": "x_in__",
            },
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        param_list = ["optional", "optional"]
        value_depends = {0: "optional"}
        res = is_static_shape(origin_inputs, outputs, value_depends, param_list, True)
        self.assertEqual(res, True)

        # required with const value
        origin_inputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "const_value": (1, 2, 3),
                "param_name": "x_in__",
            },
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "const_value": (1, 2, 3),
                "param_name": "y_in__",
            },
        ]
        param_list = ["required", "optional"]
        value_depends = {0: "required", 1: "required"}
        res = is_static_shape(origin_inputs, outputs, value_depends, param_list, True)
        self.assertEqual(res, True)

        param_list = ["others"]
        value_depends = {0: "optional"}
        res = is_static_shape(origin_inputs, outputs, value_depends, param_list, True)
        self.assertEqual(res, True)

        # required with const value
        origin_inputs = [
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "const_value": (1, 2, 3),
                "param_name": "x_in__",
            },
            {
                "shape": (8, 2048),
                "ori_shape": (8, 2048),
                "format": "ND",
                "sub_format": 0,
                "ori_format": "ND",
                "dtype": "float16",
                "addr_type": 0,
                "total_shape": [8, 2048],
                "slice_offset": (),
                "L1_addr_offset": 0,
                "L1_fusion_type": -1,
                "L1_workspace_size": -1,
                "valid_shape": (),
                "split_index": 0,
                "atomic_type": "",
                "input_c_values": -1,
                "range": [[8, 8], [2048, 2048]],
                "param_name": "y_in__",
            },
        ]
        param_list = ["required", "optional"]
        value_depends = {0: "required", 1: "optional"}
        res = is_static_shape(origin_inputs, outputs, value_depends, param_list, True)
        self.assertEqual(res, False)

        # test TensorList
        print("test tensorList")
        origin_inputs = [
            [
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "const_value": (1, 2, 3),
                    "param_name": "x_in__",
                },
                {
                    "shape": (8, 2048),
                    "ori_shape": (8, 2048),
                    "format": "ND",
                    "sub_format": 0,
                    "ori_format": "ND",
                    "dtype": "float16",
                    "addr_type": 0,
                    "total_shape": [8, 2048],
                    "slice_offset": (),
                    "L1_addr_offset": 0,
                    "L1_fusion_type": -1,
                    "L1_workspace_size": -1,
                    "valid_shape": (),
                    "split_index": 0,
                    "atomic_type": "",
                    "input_c_values": -1,
                    "range": [[8, 8], [2048, 2048]],
                    "param_name": "y_in__",
                },
            ]
        ]
        param_list = ["required"]
        value_depends = {0: "optional"}
        res = is_static_shape(origin_inputs, outputs, value_depends, param_list, True)
        self.assertEqual(res, False)

        value_depends = {0: "required"}
        self.assertRaises(Exception, is_static_shape, origin_inputs, outputs, value_depends, param_list, True)

        param_list = ["optional"]
        value_depends = {0: "required"}
        self.assertRaises(Exception, is_static_shape, origin_inputs, outputs, value_depends, param_list, True)

    @mock.patch("ctypes.CDLL")
    @mock.patch("os.path.exists")
    @mock.patch("glob.glob")
    def test_load_op_host_tiling_lib(self, mock_glob, mock_path, mock_cdll):
        class MockClass1:
            @staticmethod
            def TbeLoadSoAndSaveToRegistry(input):
                pass

        mock_tmp = MockClass1()
        mock_glob.return_value = ["test"]
        mock_path.return_value = True
        mock_cdll.return_value = mock_tmp
        load_op_host_tiling_lib()
        mock_path.side_effect = OSError(13, "Permission denied", "test")
        self.assertRaises(Exception, load_op_host_tiling_lib)
        mock_path.side_effect = Exception("error msg")
        self.assertRaises(Exception, load_op_host_tiling_lib)

    def test_get_tiling_data_without_time_stamp(self):
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import _get_tiling_data_without_time_stamp

        compile_op_module.global_var_storage.set_variable("ascendc_tiling_no_register", True)
        class_name = "AddCustomTiling"
        _get_tiling_data_without_time_stamp(class_name)
        compile_op_module.global_var_storage.set_variable("ascendc_tiling_no_register", False)
        _get_tiling_data_without_time_stamp(class_name)

    def test_get_tiling_data_with_time_stamp(self):
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import _get_tiling_data_with_time_stamp

        compile_op_module.global_var_storage.set_variable("ascendc_tiling_no_register", False)
        class_name = "AddCustomTiling"
        _get_tiling_data_with_time_stamp(class_name)

    def test_gen_dynamic_shape_v2(self):
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import gen_dynamic_shape_v2

        compile_op_module.global_var_storage.set_variable("ascendc_tiling_no_register", True)
        optype = "AddCustom"
        tiling_struct = "optiling::TilingData"
        gen_dynamic_shape_v2(optype, tiling_struct)


if __name__ == "__main__":
    unittest.main()
