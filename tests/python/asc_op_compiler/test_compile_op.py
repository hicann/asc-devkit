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
import shutil
import unittest
from unittest import mock

import json
from collections import namedtuple


THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
API_ROOT_PATH = os.path.join(TOP_PATH, "build/adapter_ut")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)
import asc_op_compile_base
from asc_op_compile_base.common.platform import (
    set_current_compile_soc_info,
    get_soc_spec,
)
from asc_op_compile_base.common import register
from asc_op_compile_base.common import buildcfg
from asc_op_compile_base.asc_op_compiler.global_storage import global_var_storage
from asc_op_compile_base.common.context import get_context


def SetCurrentSocInfo(soc: str):
    set_current_compile_soc_info(soc)
    global_var_storage.set_variable(
        "ascendc_short_soc_version", get_soc_spec("SHORT_SOC_VERSION")
    )


from asc_op_compile_base.asc_op_compiler.compile_op import *
from asc_op_compile_base.asc_op_compiler.compile_op import (
    _gen_kernel_func_declare_head,
    _compile_ascendc_cce,
    _generate_section_content,
    _get_sub_kernel_name,
    _compile_ascendc_cce_v200_with_kernel_type,
    _dynamic_kernel_list_to_json,
    _compile_ascendc_cce_v200_with_kernel_type_for_dynamic,
    _gen_dynamic_json_for_v200,
    _gen_static_json_for_mix_v200,
    _gen_static_json_for_no_mix_v200,
    _gen_non_mix_sub_json,
    _gen_mix_json_from_seperate_json,
    _gen_mix_sub_json,
    _update_compile_option,
    gen_meta_info_section,
    _match_regex,
    _get_dcci_disable_cap_bitmap,
)
from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
    call_bisheng_v220,
    get_ktype_section_variable,
    decode_mode,
    v220_mode,
    v310_mode,
)
from asc_op_compile_base.asc_op_compiler.ascendc_compile_dfx import DFXSectionGenerator
from asc_op_compile_base.asc_op_compiler.ascendc_compile_v200 import (
    call_bisheng_v200_static,
)
from asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_code import (
    skip_mc2_context_size,
    add_op_param_to_workspace,
    get_value,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_constants import (
    SuperKernelStreamFusionMode,
    SuperKernelLinkMode,
)
from asc_op_compile_base.asc_op_compiler.ascendc_constants import (
    InferChannelParamsFromIFile,
    KernelMetaType,
)

import importlib

compile_op_module = importlib.import_module(
    "asc_op_compile_base.asc_op_compiler.compile_op"
)
super_kernel_compile_module = importlib.import_module(
    "asc_op_compile_base.asc_op_compiler.super_kernel_op_compile"
)
from unittest.mock import Mock, patch
from tempfile import TemporaryDirectory


class MockMatch:
    """Custom Mock re.Match on test purpose"""

    def __init__(self, string, groups=None):
        self.string = string
        self._groups = groups or ()

    def group(self, idx=0):
        if idx >= len(self._groups):
            raise IndexError("no such group")
        return self._groups[idx]

    def __bool__(self):
        return True


class TestCompileOp(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")
        self.reset_global_var()

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")
        self.reset_global_var()

    @classmethod
    def tearDownClass(cls):
        generated_file_path = os.path.join(
            os.path.abspath(os.path.dirname(__file__)), "../../../kernel_meta"
        )
        if os.path.isdir(generated_file_path):
            shutil.rmtree(generated_file_path)

    def reset_global_var(self):
        compile_op_module.global_var_storage.set_variable(
            "ascendc_compile_debug_config", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_dump_disable_compile_options", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_debug_compile_options", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", False
        )
        compile_op_module.global_var_storage.set_variable("ascendc_compiler_path", None)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        compile_op_module.global_var_storage.set_variable("ascendc_asan_obj_path", {})
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_build_log", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_coverage", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_build_log_path", None
        )
        compile_op_module.global_var_storage.set_variable("ascendc_build_log_list", [])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_time_stamp_compile_options", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_sub_super_kernel_params", []
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_sub_super_kernel_type", ""
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_sub_super_kernel_fun_names", {}
        )
        compile_op_module.global_var_storage.set_variable(
            "super_kenel_save_sub_op_files", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_recognize_simtvf", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_tiling_no_register", False
        )

    def test_decode_mode(self):
        mode1 = decode_mode(10)
        mode2 = decode_mode(3)
        mode3 = decode_mode(0)
        self.assertEqual(mode1, 10)
        self.assertEqual(mode2, CORE_TYPE_MIX)
        self.assertEqual(mode3, CORE_TYPE_VEC)

    def test_global_storage(self):
        self.assertRaises(
            Exception,
            compile_op_module.global_var_storage.set_variable,
            "test_var",
            True,
        )
        self.assertRaises(
            Exception, compile_op_module.global_var_storage.get_variable, "test_var"
        )

    def test_compile_op_dynamic(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"
        ascendc_common_utilityop_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.ascendc_common_utility"
        )
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with mock.patch.object(
                    ascendc_common_utilityop_module,
                    "is_enable_ascendc_cov",
                    return_value=True,
                ):
                    compile_op(
                        cce_file,
                        origin_func_name,
                        op_info,
                        compile_options,
                        code_channel,
                        op_compile_option,
                    )

        binary_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".o")
        json_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".json")
        self.assertTrue(os.path.exists(binary_file))
        os.remove(binary_file)
        os.remove(json_file)

    def test_compile_op_dynamic_with_inferinfo(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        tiling_key_list = ["1", "2"]
        hard_sync = False
        no_kfc_server_flag = False
        enable_deterministic = False
        no_set_kernel_type = False
        decode_tiling_result = {}
        default_tiling_struct = ""
        tiling_struct_expr_map = {}
        set_task_bar = False
        wait_task_bar = False
        tiling_key_kernel_type = {
            "1": KernelMetaType.KERNEL_TYPE_AIV_ONLY,
            "2": KernelMetaType.KERNEL_TYPE_AIV_ONLY,
        }
        tiling_key_deterministic = {}
        default_kernel_type = KernelMetaType.KERNEL_TYPE_MAX
        dump_info = {}
        decode_tiling_result = {}

        tiling_key_struct_map = {}
        tiling_key_group_map = {}

        from asc_op_compile_base.asc_op_compiler.ascendc_constants import (
            TilingKeyConfig,
        )

        tiling_key_infos = {
            "1": TilingKeyConfig(kernel_type=KernelMetaType.KERNEL_TYPE_AIV_ONLY)
        }
        customized_config = CustomizedConfig(
            default_kernel_type, default_tiling_struct, tiling_key_infos
        )

        op_compile_option = "{}"
        extend_options = {"customized_tiling_key_list": ["1"]}
        ascendc_common_utilityop_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.ascendc_common_utility"
        )
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with mock.patch.object(
                    ascendc_common_utilityop_module,
                    "is_enable_ascendc_cov",
                    return_value=True,
                ):
                    compile_op_with_customized_config(
                        cce_file,
                        origin_func_name,
                        op_info,
                        compile_options,
                        code_channel,
                        op_compile_option,
                        extend_options,
                        customized_config,
                    )

        binary_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".o")
        json_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".json")
        self.assertTrue(os.path.exists(binary_file))
        os.remove(binary_file)
        os.remove(json_file)

    def test_compile_op_dynamic_c310(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d96",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"
        ascendc_common_utilityop_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.ascendc_common_utility"
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_recognize_simtvf", True
        )
        original_search = re.search
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with mock.patch.object(
                    ascendc_common_utilityop_module,
                    "is_enable_ascendc_cov",
                    return_value=True,
                ):
                    with mock.patch("re.search") as mock_search:

                        def custom_search(pattern, string, *args, **kwargs):
                            if r"cce_simt_entry" in pattern:
                                return MockMatch(string, (string,))
                            return original_search(pattern, string, *args, **kwargs)

                        mock_search.side_effect = custom_search
                        compile_op(
                            cce_file,
                            origin_func_name,
                            op_info,
                            compile_options,
                            code_channel,
                            op_compile_option,
                            {},
                        )

    def test_compile_op_dynamic_m510(self):
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d96",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"
        ascendc_common_utilityop_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.ascendc_common_utility"
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_recognize_simtvf", True
        )
        original_search = re.search
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with mock.patch.object(
                    ascendc_common_utilityop_module,
                    "is_enable_ascendc_cov",
                    return_value=True,
                ):
                    with mock.patch("re.search") as mock_search:
                        with mock.patch.object(
                            CommonUtility, "is_c310", return_value=False
                        ):
                            with mock.patch.object(
                                CommonUtility, "is_m510", return_value=True
                            ):
                                with mock.patch.object(
                                    CommonUtility, "is_v220", return_value=False
                                ):
                                    with mock.patch.object(
                                        compile_op_module,
                                        "_compile_ascendc_cce_m510",
                                        return_value=None,
                                    ):
                                        with mock.patch.object(
                                            compile_op_module,
                                            "link_relocatable",
                                            return_value=None,
                                        ):
                                            with mock.patch.object(
                                                compile_op_module,
                                                "_json_post_process",
                                                return_value=None,
                                            ):
                                                with mock.patch.object(
                                                    CommonUtility,
                                                    "run_cmd_inner",
                                                    return_value=None,
                                                ):

                                                    def custom_search(
                                                        pattern, string, *args, **kwargs
                                                    ):
                                                        if r"cce_simt_entry" in pattern:
                                                            return MockMatch(
                                                                string, (string,)
                                                            )
                                                        return original_search(
                                                            pattern,
                                                            string,
                                                            *args,
                                                            **kwargs,
                                                        )

                                                    mock_search.side_effect = (
                                                        custom_search
                                                    )
                                                    # currently build/bin/toolchain/x86/ubuntu/ccec_libs/
                                                    # ccec_x86_ubuntu_20_04_adk/bin/bisheng
                                                    # not support dav-510r2 -cce-aicore-stack-size
                                                    self.assertRaises(
                                                        Exception,
                                                        compile_op,
                                                        cce_file,
                                                        origin_func_name,
                                                        op_info,
                                                        compile_options,
                                                        code_channel,
                                                        op_compile_option,
                                                    )

    def test_compile_op_dynamic_no_tiling_register(self):
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d96",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"

        tiling_key_list = []
        declare_param_str = ""
        select_param_str = ""
        decode_tiling_result = {}
        code_channel: int = 0
        hard_sync = False
        no_kfc_server_flag = False
        find_kfc_server = False
        default_tiling_struct = ""
        tiling_struct_expr_map = {}
        enable_deterministic = False
        tiling_key_kernel_type = {}
        tiling_key_deterministic = {}
        tiling_key_group_map = {}
        default_kernel_type = KernelMetaType.KERNEL_TYPE_MAX
        dump_info = {"dump_type": "", "dump_size": 1048576}
        func_name_exist = False
        expect_tilingkey_set = set()
        need_find_kernel_type = False
        tiling_no_register_flag = False
        tiling_key_struct_map = {}
        register_tiling_struct = set()
        tpl_tiling_struct = set()
        set_task_bar, wait_task_bar = False, False
        no_set_kernel_type = False
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch.object(CommonUtility, "is_c310", return_value=False),
            mock.patch.object(CommonUtility, "is_m510", return_value=False),
            mock.patch.object(
                compile_op_module, "_compile_ascendc_cce", return_value=None
            ),
            mock.patch.object(CommonUtility, "is_v220", return_value=False),
            mock.patch.object(
                compile_op_module,
                "get_tiling_info_by_tiling",
                return_value=TilingInfo(),
            ),
            mock.patch.object(
                compile_op_module, "_compile_ascendc_cce_m510", return_value=None
            ),
            mock.patch.object(
                KernelInfoInfer,
                "get_tiling_key_list_and_simple_infer_code_channel",
                return_value=InferChannelParamsFromIFile(
                    tiling_key_list,
                    code_channel,
                    hard_sync,
                    no_kfc_server_flag,
                    enable_deterministic,
                    tiling_key_kernel_type,
                    no_set_kernel_type,
                    default_kernel_type,
                    dump_info,
                    decode_tiling_result,
                    default_tiling_struct,
                    tiling_struct_expr_map,
                    tiling_key_struct_map,
                    register_tiling_struct,
                    tpl_tiling_struct,
                    set_task_bar,
                    wait_task_bar,
                    tiling_key_deterministic,
                    tiling_key_group_map,
                ),
            ),
            mock.patch.object(
                compile_op_module,
                "_get_tiling_struct_without_register_size",
                return_value=None,
            ),
            mock.patch.object(
                compile_op_module,
                "gen_tiling_struct_size_and_dfx_section_file",
                return_value=None,
            ),
            mock.patch.object(
                compile_op_module, "link_relocatable_meta_file", return_value=None
            ),
            mock.patch.object(
                compile_op_module, "gen_compile_cmd_for_meta_info", return_value=None
            ),
            mock.patch.object(
                compile_op_module, "_json_post_process", return_value=None
            ),
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=None),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.global_storage.global_var_storage.global_storage_reset"
            ),
        ):
            compile_op_module.global_var_storage.set_variable(
                "ascendc_tiling_no_register", True
            )
            compile_op(
                cce_file,
                origin_func_name,
                op_info,
                compile_options,
                code_channel,
                op_compile_option,
            )

    def test_sk_sub_combine_links_no_register_meta_file(self):
        op_info = OpInfo(
            kernel_name="TestKernel", op_type="TestOp", inputs=[], outputs=[]
        )
        compile_option_tuple = CompileOptionTuple([], [])
        infered_info = InferChannelParamsFromIFile(
            tiling_key_list=[0],
            code_channel=CORE_TYPE_VEC,
            hard_sync=False,
            no_kfc_server_flag=False,
            enable_deterministic=False,
            tiling_key_kernel_type={},
            no_set_kernel_type=False,
            default_kernel_type=KernelMetaType.KERNEL_TYPE_MAX,
            dump_info={},
            template_tiling_info={},
            default_tiling_struct="",
            tiling_struct_expr_map={},
            tiling_key_struct_map={},
            register_tiling_struct=set(),
            tpl_tiling_struct=set(),
            super_kernel_early_start_set_flag=False,
            super_kernel_early_start_wait_flag=False,
            tiling_key_deterministic={},
            tiling_key_group_map={},
        )
        tiling_info = TilingInfo()
        dfx_generator = DFXSectionGenerator()

        global_var_storage.set_variable("ascendc_tiling_no_register", True)
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        try:
            with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
                ctx.add_addition("super_kernel_sub_combine", True)
                with (
                    mock.patch.object(
                        CommonUtility,
                        "get_kernel_meta_dir",
                        return_value="/tmp/kernel_meta",
                    ),
                    mock.patch.object(
                        CommonUtility,
                        "get_distinct_filename_tag",
                        return_value="_tag",
                    ),
                    mock.patch.object(
                        compile_op_module,
                        "check_if_gen_placehoder",
                        return_value=False,
                    ),
                    mock.patch.object(
                        compile_op_module,
                        "get_tiling_info_by_tiling",
                        return_value=tiling_info,
                    ),
                    mock.patch.object(tiling_info, "save_file"),
                    mock.patch.object(tiling_info, "remove_file"),
                    mock.patch.object(compile_op_module, "handle_sk_codegen_options"),
                    mock.patch.object(
                        compile_op_module,
                        "gen_op_stub_kernel_func",
                        return_value=0,
                    ),
                    mock.patch.object(compile_op_module, "handle_compile_options"),
                    mock.patch.object(compile_op_module, "compile_kernel_and_meta"),
                    mock.patch.object(compile_op_module, "link_kernel_obj"),
                    mock.patch.object(
                        compile_op_module,
                        "compile_sk_bind",
                        return_value="/tmp/kernel_meta/sk_bind.o",
                    ),
                    mock.patch.object(
                        compile_op_module, "link_sk_norm_combine"
                    ) as mock_link,
                    mock.patch.object(compile_op_module, "_json_post_process"),
                    mock.patch.object(dfx_generator, "dfx_info_reset"),
                    mock.patch.object(dfx_generator, "update_is_support"),
                ):
                    compile_op_common_part(
                        "test.cpp",
                        "test",
                        op_info,
                        compile_option_tuple,
                        infered_info,
                        {},
                    )
        finally:
            global_var_storage.set_variable("ascendc_tiling_no_register", False)
            global_var_storage.set_variable("ascendc_enable_super_kernel", False)

        mock_link.assert_called_once_with(
            "/tmp/kernel_meta/TestKernel.o",
            "/tmp/kernel_meta/TestKernel_norm.o",
            "/tmp/kernel_meta/sk_bind.o",
            "/tmp/kernel_meta/TestKernel_tag_meta_info.o",
            None,
        )

    def test_compile_op_dynamic_c310_cube(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/cube_custom.cpp"
        )
        origin_func_name = "cube_custom"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="CubeCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="CubeCustom",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/cube_custom_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"
        ascendc_common_utilityop_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.ascendc_common_utility"
        )
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with mock.patch.object(
                    ascendc_common_utilityop_module,
                    "is_enable_ascendc_cov",
                    return_value=True,
                ):
                    compile_op(
                        cce_file,
                        origin_func_name,
                        op_info,
                        compile_options,
                        code_channel,
                        op_compile_option,
                    )

        binary_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".o")
        json_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".json")
        self.assertTrue(os.path.exists(binary_file))
        os.remove(binary_file)
        os.remove(json_file)

    def test_add_op_param_to_workspace(self):
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
            output_shape_depend_on_compute=[0],
        )

        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        compile_options = []
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {"1": 96, "2": 100}
        tiling_info.static_shape_flag = False

        compile_info = CompileInfo()
        compile_info.tiling_key_struct_map = {}

        dump_size = 1024
        old_source = ""
        new_source = add_op_param_to_workspace(
            op_info, tiling_info, old_source, compile_options, compile_info
        )
        self.assertNotEqual(new_source, old_source)
        tiling_info.static_shape_flag = True
        compile_info.tiling_key_struct_map = {"1": 96, "2": 100}
        new_source = add_op_param_to_workspace(
            op_info, tiling_info, old_source, compile_options, compile_info
        )
        self.assertNotEqual(new_source, old_source)
        tiling_info.tiling_key_data_size = {}
        new_source = add_op_param_to_workspace(
            op_info, tiling_info, old_source, compile_options, compile_info
        )
        self.assertNotEqual(new_source, old_source)
        tiling_info.tiling_key_data_size = {"0": 96}
        new_source = add_op_param_to_workspace(
            op_info, tiling_info, old_source, compile_options, compile_info
        )
        self.assertNotEqual(new_source, old_source)

        op_info_neg = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
                None,
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
                },
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "int",
                    "range": [(0, None)],
                    "param_name": "z_out_",
                },
                None,
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
            mc2_ctx=None,
            param_type_list=[
                "dynamic",
                "required",
                "dynamic",
                "required",
                "dynamic",
                "dynamic",
            ],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )

        new_source = add_op_param_to_workspace(
            op_info_neg, tiling_info, old_source, compile_options, compile_info
        )
        self.assertNotEqual(new_source, old_source)

        value = get_value("DT_INT8")
        self.assertEqual(value, 1)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    def test_get_ascendc_compiler_path(self):
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        self.assertRaises(Exception, CommonUtility.get_ascendc_compiler_path)
        compile_op_module.global_var_storage.set_variable("ascendc_compiler_path", ".")
        CommonUtility.get_ascendc_compiler_path()

    def test_trans_compile_cmds_to_precompile(self):
        cmd_i = [
            "ccec",
            "-c",
            "-O3",
            "--cce-aicore-arch=dav-c220-cube",
            "--cce-aicore-only",
            "/tmp/add_custom.o",
        ]
        from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import (
            _trans_compile_cmds_to_precompile,
        )

        _trans_compile_cmds_to_precompile(cmd_i)
        self.assertEqual(len(cmd_i), 7)
        cmd_i = ["ccec"]
        self.assertRaises(Exception, _trans_compile_cmds_to_precompile, cmd_i)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd_v200(self, mock_shutil):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v200 import (
            gen_compile_cmd_v200,
        )

        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", True
        )
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        compile_cmd = gen_compile_cmd_v200(
            src_file, dst_file, compile_option_tuple, "dav-m200", tiling_file
        )
        self.assertEqual(compile_cmd[0], "/usr/bin/ccache")
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", False
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        compile_cmd = gen_compile_cmd_v200(
            src_file, dst_file, compile_option_tuple, "dav-m200", tiling_file
        )
        self.assertEqual(
            compile_cmd[0],
            compile_op_module.global_var_storage.get_variable("ascendc_compiler_path"),
        )
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
            gen_compile_cmd_v200(
                src_file, dst_file, compile_option_tuple, "dav-m200", tiling_file
            )

    def test_gen_compile_ascend_cmd_m510(self):
        from asc_op_compile_base.asc_op_compiler.compile_op import gen_compile_cmd_v220

        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", True
        )
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_m510,
        )

        compile_info = CompileInfo()
        tiling_info = TilingInfo()
        compile_info.src_file = src_file
        compile_info.dst_file = dst_file
        compile_info.gen_kernel_func_file = src_file
        compile_info.kernel_name = op_info.kernel_name
        tiling_info.static_shape_flag = True
        with mock.patch.object(
            compile_op_module, "call_bisheng_v220", return_value=["1"]
        ):
            with mock.patch.object(
                compile_op_module, "_gen_non_mix_sub_json", return_value=None
            ):
                with mock.patch.object(
                    compile_op_module, "_dynamic_kernel_list_to_json", return_value=None
                ):
                    _compile_ascendc_cce_m510(
                        compile_info, compile_option_tuple, tiling_info
                    )

    def test_gen_compile_ascend_cmd_m5101(self):
        from asc_op_compile_base.asc_op_compiler.compile_op import gen_compile_cmd_v220

        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", True
        )
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_m510,
        )

        compile_info = CompileInfo()
        tiling_info = TilingInfo()
        compile_info.src_file = src_file
        compile_info.dst_file = dst_file
        compile_info.gen_kernel_func_file = src_file
        compile_info.kernel_name = op_info.kernel_name
        tiling_info.static_shape_flag = False
        with mock.patch.object(
            compile_op_module, "call_bisheng_v220", return_value=["1"]
        ):
            with mock.patch.object(
                compile_op_module, "_gen_non_mix_sub_json", return_value=None
            ):
                with mock.patch.object(
                    compile_op_module, "_dynamic_kernel_list_to_json", return_value=None
                ):
                    _compile_ascendc_cce_m510(
                        compile_info, compile_option_tuple, tiling_info
                    )

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd_regbase_m510(self, mock_shutil):
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _gen_compile_cmd_regbase,
        )

        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple([], [])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
            from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
                _gen_compile_cmd_m510,
            )

            compile_cmd = _gen_compile_cmd_m510(
                src_file, dst_file, compile_option_tuple, "dav-m510", tiling_file
            )
            self.reset_global_var()
            compile_cmd = _gen_compile_cmd_m510(
                src_file, dst_file, compile_option_tuple, "dav-m510", tiling_file
            )

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd_m510(self, mock_shutil):
        # SetCurrentSocInfo("MC62CM12AA")
        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", True
        )
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
            _gen_compile_cmd_m510,
        )

        compile_cmd_bak = _gen_compile_cmd_m510(
            src_file, dst_file, compile_option_tuple, tiling_file, True
        )
        from asc_op_compile_base.asc_op_compiler.compile_op import gen_compile_cmd_v220

        with mock.patch.object(CommonUtility, "is_c310", return_value=False):
            with mock.patch.object(CommonUtility, "is_m510", return_value=True):
                compile_cmd = gen_compile_cmd_v220(
                    src_file,
                    dst_file,
                    compile_option_tuple,
                    "dav-510r2",
                    tiling_file,
                    True,
                )
                self.assertIn("--cce-enable-sanitizer", compile_cmd)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        with mock.patch.object(CommonUtility, "is_c310", return_value=False):
            with mock.patch.object(CommonUtility, "is_m510", return_value=True):
                compile_cmd = gen_compile_cmd_v220(
                    src_file,
                    dst_file,
                    compile_option_tuple,
                    "dav-510r2",
                    tiling_file,
                    True,
                )

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_is_m510(self, mock_shutil):
        mock_shutil.return_value = "/tmp/ascendc_compiler"
        a_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.ascendc_common_utility"
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
        from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import (
            CommonUtility as utility,
        )

        get_soc = global_var_storage.get_variable("ascendc_short_soc_version")
        global_var_storage.set_variable("ascendc_short_soc_version", "MC62")
        self.assertTrue(utility.is_m510())
        global_var_storage.set_variable("ascendc_short_soc_version", get_soc)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd_v220(self, mock_shutil):
        from asc_op_compile_base.asc_op_compiler.compile_op import gen_compile_cmd_v220

        SetCurrentSocInfo("Ascend910B1")
        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", True
        )
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        compile_cmd = gen_compile_cmd_v220(
            src_file, dst_file, compile_option_tuple, "dav-c220", tiling_file, True
        )
        self.assertIn("--cce-enable-sanitizer", compile_cmd)
        self.assertEqual(compile_cmd[0], "/usr/bin/ccache")
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        compile_cmd = gen_compile_cmd_v220(
            src_file, dst_file, compile_option_tuple, "dav-c220", tiling_file, True
        )
        self.assertEqual(
            compile_cmd[0],
            compile_op_module.global_var_storage.get_variable("ascendc_compiler_path"),
        )

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd_c310(self, mock_shutil):
        from asc_op_compile_base.asc_op_compiler.compile_op import gen_compile_cmd_v220

        SetCurrentSocInfo("Ascend950PR_9599")
        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_sanitizer", True
        )
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        compile_cmd = gen_compile_cmd_v220(
            src_file, dst_file, compile_option_tuple, "dav-c310", tiling_file, True
        )
        self.assertIn("--cce-enable-sanitizer", compile_cmd)
        self.assertEqual(compile_cmd[0], "/usr/bin/ccache")
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_ccache", False
        )
        compile_cmd = gen_compile_cmd_v220(
            src_file, dst_file, compile_option_tuple, "dav-c310", tiling_file, True
        )
        self.assertEqual(
            compile_cmd[0],
            compile_op_module.global_var_storage.get_variable("ascendc_compiler_path"),
        )
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
            compile_cmd = gen_compile_cmd_v220(
                src_file, dst_file, compile_option_tuple, "dav-c310", tiling_file, True
            )
            self.assertIn("-cce-block-local-relocate=false", compile_cmd)
            self.assertIn("-DASCENDC_OOM=1", compile_cmd)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    def test_gen_compile_cmd_for_meta_info(self):
        global_var_storage.set_variable("ascendc_enable_ccache", True)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_compiler_path", "/usr/local/ASC"
        )

        original_is_c310 = CommonUtility.is_c310

        def mock_is_c310():
            return True

        CommonUtility.is_c310 = mock_is_c310

        src_file = "test_kernel.cpp"
        dst_file = "test_kernel.o"
        compile_option_tuple = CompileOptionTuple(["opt"], ["opt"])
        sub_arch = "dav-c310"

        cmd = gen_compile_cmd_for_meta_info(
            src_file, dst_file, compile_option_tuple, sub_arch
        )

        expected_cmd = [
            os.environ.get("ASCENDC_CCACHE_EXECUTABLE", "ccache"),
            "/usr/local/ASC",
            "-c",
            "-O3",
            "opt",
            src_file,
            "--cce-aicore-arch=dav-c310",
            "--cce-aicore-only",
            "-o",
            dst_file,
            "-D__DAV_C310__",
            "-std=c++17",
        ]

        print("Generated command:", cmd)
        print("Expected command:", expected_cmd)
        self.assertEqual(cmd, expected_cmd)

        global_var_storage.set_variable("ascendc_enable_ccache", False)

        def mock_is_c310():
            return False

        CommonUtility.is_c310 = mock_is_c310
        sub_arch = "dav-c220"

        cmd = gen_compile_cmd_for_meta_info(
            src_file, dst_file, compile_option_tuple, sub_arch
        )

        expected_cmd = [
            "/usr/local/ASC",
            "-c",
            "-O3",
            "opt",
            src_file,
            "--cce-aicore-arch=dav-c220",
            "--cce-aicore-only",
            "-o",
            dst_file,
            "-std=c++17",
        ]

        print("Generated command (no ccache, no opts):", cmd)
        print("Expected command:", expected_cmd)
        self.assertEqual(cmd, expected_cmd)

        CommonUtility.is_c310 = original_is_c310
        compile_op_module.global_var_storage.set_variable("ascendc_compiler_path", None)
        self.reset_global_var()

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd_regbase(self, mock_shutil):
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _gen_compile_cmd_regbase,
        )

        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple([], [])
        compile_cmd = _gen_compile_cmd_regbase(
            src_file, dst_file, compile_option_tuple, "dav-m300", tiling_file
        )
        self.assertEqual(compile_cmd[0], "/usr/bin/ccache")
        self.reset_global_var()
        compile_cmd = _gen_compile_cmd_regbase(
            src_file, dst_file, compile_option_tuple, "dav-m300", tiling_file
        )
        self.assertEqual(compile_cmd[0], None)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("shutil.which")
    def test_gen_compile_cmd(self, mock_shutil):
        from asc_op_compile_base.asc_op_compiler.compile_op import _gen_compile_cmd

        mock_shutil.return_value = "/tmp/ascendc_compiler"
        op_info = OpInfo(
            kernel_name="AddCustom_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustom",
        )
        CommonUtility.get_ascendc_compiler_path()
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        tiling_file = "/tmp/tiling_data.h"
        compile_option_tuple = CompileOptionTuple([], [])
        with mock.patch.object(
            CommonUtility,
            "ascendc_build_aicore_compile_cmd",
            return_value=["ccec", "-x", "-c"],
        ):
            compile_cmd = _gen_compile_cmd(
                src_file, dst_file, compile_option_tuple, tiling_file
            )
        self.assertEqual(compile_cmd[0], "/usr/bin/ccache")
        with mock.patch.object(
            CommonUtility,
            "ascendc_build_aicore_compile_cmd",
            return_value=["ccec", "-x", "-c"],
        ):
            with buildcfg.build_config() as cfg:
                cfg.current()["tir.op_debug_config"] = ["oom"]
                _gen_compile_cmd(src_file, dst_file, compile_option_tuple, tiling_file)

    def test_gen_kernel_func_declare_head(self):
        kernel_func_desc = ""
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                None,
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
                None,
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
            mc2_ctx=["mc2_test"],
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )
        tiling_info = TilingInfo()
        kernel_func_desc, idx, called_func_params, called_func_params_type = (
            _gen_kernel_func_declare_head(True, True, op_info, tiling_info)
        )
        print(kernel_func_desc)
        self.assertEqual(
            "GM_ADDR ffts_addr, GM_ADDR mc2_test, GM_ADDR y_in__, GM_ADDR z_out_, GM_ADDR __ascendc_output_shape, GM_ADDR workspace) {\n",
            kernel_func_desc,
        )
        self.assertEqual(idx, 5)

    def test_compile_ascendc_cce(self):
        SetCurrentSocInfo("Ascend310P1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/test.cpp"
        )
        compile_info = CompileInfo()
        compile_info.dst_file = os.path.join(TOP_PATH, "kernel_meta", "test.o")
        compile_info.kernel_name = "test"
        compile_info.origin_func_name = "test"
        compile_info.dump_info = {"dump_type": "", "dump_size": 1048576}
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel_ascendc_cce.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, "test" + file_name_tag
        )
        compile_option_tuple = CompileOptionTuple([], [])
        compile_info.tiling_key_list = ["1"]

        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                None,
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
                None,
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
            mc2_ctx=["mc2_test"],
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.tiling_data_file_path = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_tiling.h"
        )
        tiling_info.static_shape_flag = False
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with mock.patch.object(
                    CommonUtility,
                    "ascendc_build_aicore_compile_cmd",
                    return_value=[
                        "bisheng",
                        "-c",
                        "-O3",
                        "-x",
                        "cce",
                        cce_file,
                        "-I" + API_ROOT_PATH,
                        "-I" + os.path.join(API_ROOT_PATH, "include"),
                        "-I" + os.path.join(API_ROOT_PATH, "impl"),
                        "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
                        "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
                        "-I"
                        + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
                        "-I"
                        + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
                        "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
                        "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
                        "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
                        "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
                        "-I" + os.path.join(TOP_PATH, "build"),
                        "-DHIGH_PERFORMANCE=1",
                        "-DDETERMINISTIC_MODE=1--cce-auto-sync",
                        "-mllvm",
                        "-api-deps-filter",
                        "-DONE_CORE_DUMP_SIZE=1048576",
                        "-DASCENDC_DUMP=0",
                        "-DL2_CACHE_HINT",
                        "--cce-aicore-arch=dav-c220-vec",
                        "--cce-aicore-only",
                        "-o",
                        os.path.join(TOP_PATH, "kernel_meta", "test_1.o"),
                    ],
                ):
                    CommonUtility.get_kernel_meta_dir()
                    DFXSectionGenerator().dfx_info_reset(op_info)
                    gen_kernel_fun(
                        compile_info,
                        compile_info.origin_func_name,
                        op_info,
                        tiling_info,
                        CompileOptionTuple([], []),
                    )
                    _compile_ascendc_cce(
                        compile_info, compile_option_tuple, tiling_info
                    )
        self.assertTrue(os.path.exists(compile_info.dst_file))
        os.remove(compile_info.dst_file)
        os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))

    def test_get_ktype_section_variable(self):
        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_AIV_ONLY
        )
        self.assertTrue("K_TYPE_AIV" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_AIC_ONLY
        )
        self.assertTrue("K_TYPE_AIC" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC
        )
        self.assertTrue("K_TYPE_MIX_AIV_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
        )
        self.assertTrue("K_TYPE_MIX_AIV_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test", "test", KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test_mix_aic", "test_mix_aic", KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

        section_context = get_ktype_section_variable(
            "test_mix_aiv", "test_mix_aiv", KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

    def test_gen_json(self):
        SetCurrentSocInfo("Ascend310P1")
        compile_info = CompileInfo()
        compile_info.kernel_name = "test"
        compile_info.origin_func_name = "test"
        compile_info.tiling_key_list = []

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.static_shape_flag = False

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config() as cfg:
                _gen_dynamic_json_for_v200(compile_info, tiling_info, "VectorCore")
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_static_json_for_mix_v200(
                    compile_info,
                    tiling_info,
                    KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                )
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_static_json_for_no_mix_v200(
                    compile_info,
                    tiling_info,
                    KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                )
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_non_mix_sub_json(compile_info, tiling_info, "VectorCore")
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_mix_sub_json(compile_info, tiling_info)
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                cfg.current()["tir.enable_vector_core"] = 1
                _gen_static_json_for_mix_v200(
                    compile_info,
                    tiling_info,
                    KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                )
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))

    def test_gen_meta_info_section(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        compile_info = CompileInfo()
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d96",
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
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config() as cfg:
                cfg.current()["tir.op_debug_config"] = ["oom"]
                gen_meta_info_section(compile_info, op_info)

    def test_gen_kernel_fun(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"
        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = ["1", "2", "3"]

        tiling_key_kernel_type = infered_info_from_ifile.tiling_key_kernel_type
        tiling_key_kernel_type["1"] = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0
        tiling_key_kernel_type["2"] = KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
        tiling_key_kernel_type["3"] = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2

        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = True
        tiling_info.tiling_key_list = tiling_key_list

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = tiling_key_kernel_type
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_aicore_exception_restart", True
        )

        if os.path.exists(compile_info.gen_kernel_func_file):
            os.remove(compile_info.gen_kernel_func_file)
        assert os.path.exists(compile_info.gen_kernel_func_file) == False
        self.assertFalse(os.path.exists(compile_info.gen_kernel_func_file))
        global_var_storage.set_variable("ascendc_enable_super_kernel", False)
        global_var_storage.set_variable("ascendc_recognize_simtvf", True)
        DFXSectionGenerator().dfx_info_reset(op_info)
        gen_kernel_fun(
            compile_info,
            origin_func_name,
            op_info,
            tiling_info,
            CompileOptionTuple(compile_options, []),
        )
        global_var_storage.set_variable("ascendc_recognize_simtvf", False)
        assert os.path.exists(compile_info.gen_kernel_func_file) == True, (
            "Problems Occurred during Kernel Function Generation!!!"
        )
        self.assertTrue(os.path.exists(compile_info.gen_kernel_func_file))
        with open(compile_info.gen_kernel_func_file, "r") as generated_kernel_file:
            self.assertNotIn(
                "__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__",
                generated_kernel_file.read(),
            )
        os.remove(compile_info.gen_kernel_func_file)
        # compile_info.dump_info["dump_type"]= "assert"
        # assert global_var_storage.get_variable("ascendc_dump_assert_only") is True
        assert "-DASCENDC_DUMP=0" not in compile_option_tuple.compile_options
        gen_kernel_fun(
            compile_info,
            origin_func_name,
            op_info,
            tiling_info,
            CompileOptionTuple(compile_options, []),
        )
        assert os.path.exists(compile_info.gen_kernel_func_file) == True, (
            "Problems Occurred during Kernel Function Generation!!!"
        )
        self.assertTrue(os.path.exists(compile_info.gen_kernel_func_file))
        os.remove(compile_info.gen_kernel_func_file)
        # global_var_storage.set_variable("ascendc_dump_assert_only", True)
        compile_info.code_channel = 1
        gen_kernel_fun(
            compile_info,
            origin_func_name,
            op_info,
            tiling_info,
            CompileOptionTuple(compile_options, []),
        )
        # global_var_storage.set_variable("ascendc_dump_assert_only", False)
        compile_info.dump_info = infered_info_from_ifile.dump_info
        assert os.path.exists(compile_info.gen_kernel_func_file) == True, (
            "Problems Occurred during Kernel Function Generation!!!"
        )
        self.assertTrue(os.path.exists(compile_info.gen_kernel_func_file))
        os.remove(compile_info.gen_kernel_func_file)
        compile_info.dump_info = infered_info_from_ifile.dump_info
        compile_info.code_channel = code_channel
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_aicore_exception_restart", False
        )

    def test_gen_tiling_struct_size_and_dfx_section_file(self):
        compile_info_old = CompileInfo()
        tiling_info_old = TilingInfo()
        compile_info_old.tiling_key_list = ["1", "2"]
        compile_info_old.no_set_kernel_type = False
        tiling_key_struct_size_map = {"tiling_str": "1"}
        mock_dfx_return = "// DFX section generated"

        with (
            mock.patch("os.open", return_value=3) as mock_open,
            mock.patch("os.fdopen", return_value=mock.MagicMock()) as mock_fdopen,
        ):
            with mock.patch.object(
                DFXSectionGenerator(),
                "generate_dfx_section_without_tiling_register",
                return_value=mock_dfx_return,
            ) as mock_method:
                # static_shape_flag = True
                tiling_info_old.static_shape_flag = True
                tiling_info_old.tiling_key = "1"
                compile_info_old.tiling_key_kernel_type["1"] = (
                    KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1
                )
                # KERNEL_TYPE_MIX_AIC_1_1
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )

                compile_info_old.tiling_key_kernel_type["1"] = (
                    KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0
                )
                # KERNEL_TYPE_MIX_AIC_1_0
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )

                # static_shape_flag = False
                tiling_info_old.static_shape_flag = False
                compile_info_old.tiling_key_kernel_type["2"] = (
                    KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
                )
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )
                compile_info_old.tiling_key_kernel_type["1"] = (
                    KernelMetaType.KERNEL_TYPE_AIV_ONLY
                )
                compile_info_old.tiling_key_kernel_type["2"] = (
                    KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
                )
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )

                compile_info_old.no_set_kernel_type = True
                # no_set_kernel_type
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )

                compile_info_old.hard_sync = True
                compile_info_old.code_channel = CORE_TYPE_VEC
                # hard_sync
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )

                # no_set_kernel_type = True, static_shape_flag = True
                compile_info_old.no_set_kernel_type = True
                tiling_info_old.static_shape_flag = True
                compile_info_old.code_channel = CORE_TYPE_MIX
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )
                compile_info_old.hard_sync = True
                compile_info_old.code_channel = CORE_TYPE_VEC
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )
                compile_info_old.hard_sync = False
                gen_tiling_struct_size_and_dfx_section_file(
                    compile_info=compile_info_old,
                    tiling_info=tiling_info_old,
                    tiling_key_struct_size_map=tiling_key_struct_size_map,
                )
                assert mock_method.call_count > 0, (
                    "DFXSectionGenerator.generate_dfx_section_without_tiling_register should be called"
                )

    @patch("subprocess.Popen")
    def test_get_tiling_struct_without_register_size(self, mock_popen):
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _get_tiling_struct_without_register_size,
        )

        mock_output = """
Contents of section
0000 50000000 00000000 00000000 .ascendc_tiling.struct1_1234UL
0000 60000000 00000000 00000000 .ascendc_tiling.struct2_5678
0000 70000000 00000000 00000000 .ascendc_tiling.struct3
"""
        mock_proc = mock.MagicMock()
        mock_proc.communicate.return_value = (mock_output.encode("utf-8"), None)
        mock_popen.return_value = mock_proc
        compile_info_old = mock.MagicMock()
        tiling_key_struct_size_map = _get_tiling_struct_without_register_size(
            compile_info_old
        )
        self.assertEqual(
            tiling_key_struct_size_map, {"1234": ("struct1", 0), "5678": ("struct2", 0)}
        )

    def test_get_code_channel_v220_by_first_tiling_key(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]
        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        ret_code_channel, _ = get_code_channel_v220_by_first_tiling_key(
            InferChannelParams(
                cce_file,
                dst_file_header,
                compile_option_tuple,
                tiling_key_list[0],
                tiling_info,
                compile_log_path,
                infered_info_from_ifile.no_kfc_server_flag,
            )
        )
        self.assertEqual(ret_code_channel, 2)

    def test_v220_mode(self):
        cases = {
            "abcdefghi": 0,
            "000000f0": CORE_TYPE_CUBE,
            "000000c0": CORE_TYPE_CUBE,
            "00004080": 0,
            "00000090": CORE_TYPE_VEC,
            "0000a06b": CORE_TYPE_VEC,
            "00007060": CORE_TYPE_CUBE,
            "0000706b": CORE_TYPE_CUBE,
            "0000a070": CORE_TYPE_VEC,
            "8a000070": CORE_TYPE_VEC,
            "00000070": CORE_TYPE_CUBE,
            "000000b0": 0,
        }
        for inst, golden_result in cases.items():
            result = v220_mode(inst)
            self.assertEqual(result, golden_result)

    def test_v310_mode(self):
        casescube = {
            "abcdefghi": 0,
            "000000f0": CORE_TYPE_CUBE,
            "000000c0": CORE_TYPE_CUBE,
            "00004080": 0,
            "00000015": 0,
            "0000f015": 0,
            "28003072": CORE_TYPE_CUBE,
            "0000806e": CORE_TYPE_CUBE,
            "0000a070": 0,
            "8a000070": 0,
            "0000006b": CORE_TYPE_CUBE,
            "000000e5": CORE_TYPE_CUBE,
        }
        for inst, golden_result in casescube.items():
            result = v310_mode(inst, True)
            self.assertEqual(result, golden_result)
        casesvec = {
            "abcdefghi": 0,
            "000000f0": 0,
            "000000c0": 0,
            "00004080": 0,
            "00000015": CORE_TYPE_VEC,
            "0000f015": CORE_TYPE_VEC,
            "28003072": 0,
            "0000806e": 0,
            "00004242": CORE_TYPE_VEC,
            "0000e015": CORE_TYPE_VEC,
            "00000070": 0,
            "000000b0": 0,
        }
        for inst, golden_result in casesvec.items():
            result = v310_mode(inst, False)
            self.assertEqual(result, golden_result)

    def test_is_hard_sync_instr(self):
        cases = {
            "00000000": False,
            "0000e541": True,
            "0000f741": True,
            "00000041": False,
        }
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
            _is_hard_sync_instr,
        )

        for inst, golden_result in cases.items():
            result = _is_hard_sync_instr(inst)
            self.assertEqual(result, golden_result)

    def test_dynamic_kernel_list_to_json_for_kernel_type(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        compile_info.tiling_key_deterministic = {}
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        final_kernel_type = 1
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_json import (
            _dynamic_kernel_list_to_json_for_kernel_type,
        )
        from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
            enable_deterministic_mode,
        )

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config() as cfg,
            mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open,
            mock.patch("json.dump") as mock_dump,
            mock.patch("os.chmod"),
        ):
            enable_deterministic = False
            compile_info.tiling_key_deterministic = {"1": True}
            _dynamic_kernel_list_to_json_for_kernel_type(
                compile_info,
                op_info.kernel_name,
                tiling_key_list,
                enable_deterministic,
                final_kernel_type,
            )
            _dynamic_kernel_list_to_json(
                op_info.kernel_name, tiling_key_list, enable_deterministic, {"1": True}
            )
            enable_deterministic = True
            cfg.current()[enable_deterministic_mode] = 1
            _dynamic_kernel_list_to_json_for_kernel_type(
                compile_info,
                op_info.kernel_name,
                tiling_key_list,
                enable_deterministic,
                final_kernel_type,
            )
            _dynamic_kernel_list_to_json(
                op_info.kernel_name, tiling_key_list, enable_deterministic, {}
            )
            cfg.current()[enable_deterministic_mode] = False
            _dynamic_kernel_list_to_json_for_kernel_type(
                compile_info,
                op_info.kernel_name,
                tiling_key_list,
                enable_deterministic,
                final_kernel_type,
            )
            _dynamic_kernel_list_to_json(
                op_info.kernel_name, tiling_key_list, enable_deterministic, {}
            )
            self.assertEqual(compile_info.hard_sync, False)
            mock_dump.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _dynamic_kernel_list_to_json,
                op_info.kernel_name,
                tiling_key_list,
                enable_deterministic,
                {},
            )
            self.assertRaises(
                Exception,
                _dynamic_kernel_list_to_json_for_kernel_type,
                compile_info,
                op_info.kernel_name,
                tiling_key_list,
                enable_deterministic,
                final_kernel_type,
            )
        self.assertRaises(
            Exception,
            _dynamic_kernel_list_to_json_for_kernel_type,
            compile_info,
            op_info.kernel_name,
            tiling_key_list,
            enable_deterministic,
            final_kernel_type,
        )
        self.assertRaises(
            Exception,
            _dynamic_kernel_list_to_json,
            op_info.kernel_name,
            tiling_key_list,
            enable_deterministic,
            {},
        )

    def test_call_bisheng_regbase(self):
        SetCurrentSocInfo("Ascend310B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        with buildcfg.build_config():
            infered_info_from_ifile = (
                KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                    op_info,
                    cce_file,
                    os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                    compile_option_tuple,
                    compile_log_path,
                    origin_func_name,
                )
            )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        arch = "dav-m300"

        from asc_op_compile_base.asc_op_compiler.compile_op import _call_bisheng_regbase

        os.mknod(compile_info.gen_kernel_func_file)
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch.object(
                CommonUtility,
                "ascendc_build_aicore_compile_cmd",
                return_value=["ccec", "-x", "-c"],
            ),
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
            mock.patch("os.remove"),
        ):
            tiling_info.static_shape_flag = False
            DFXSectionGenerator().dfx_info_reset(op_info)
            gen_kernel_fun(
                compile_info,
                compile_info.origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )

            ret = _call_bisheng_regbase(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, ["1"])

            with open(compile_info.gen_kernel_func_file, "w") as temp_file:
                temp_file.write("")

            tiling_info.static_shape_flag = True
            DFXSectionGenerator().dfx_info_reset(op_info)
            gen_kernel_fun(
                compile_info,
                compile_info.origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            ret = _call_bisheng_regbase(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, None)
        os.remove(compile_info.gen_kernel_func_file)

    def test_call_bisheng_v220(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d86",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        arch = "dav-c220-vec"
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
            call_bisheng_v220,
        )

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
            mock.patch("os.remove"),
        ):
            DFXSectionGenerator().dfx_info_reset(op_info)
            tiling_info.static_shape_flag = True
            gen_kernel_fun(
                compile_info,
                origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            code_channel = 0
            ret = call_bisheng_v220(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, ["0"])
            code_channel = 1
            ret = call_bisheng_v220(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, ["0"])
            DFXSectionGenerator().dfx_info_reset(op_info)
            os.system(f"cat {compile_info.gen_kernel_func_file}")
            os.remove(compile_info.gen_kernel_func_file)
            tiling_info.static_shape_flag = False
            gen_kernel_fun(
                compile_info,
                origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            ret = call_bisheng_v220(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, ["1"])

    def test_call_bisheng_c310(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d86",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        arch = "dav-c310-vec"
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
            call_bisheng_v220,
        )

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
            mock.patch("os.remove"),
        ):
            DFXSectionGenerator().dfx_info_reset(op_info)
            tiling_info.static_shape_flag = True
            gen_kernel_fun(
                compile_info,
                origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            code_channel = 0
            ret = call_bisheng_v220(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, ["0"])
            tiling_info.static_shape_flag = False
            gen_kernel_fun(
                compile_info,
                origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            ret = call_bisheng_v220(
                compile_info, compile_option_tuple, tiling_info, arch, code_channel
            )
            self.assertEqual(ret, ["1"])

    def test_mssanitizer_link(self):
        SetCurrentSocInfo("Ascend910B1")
        src_file = "/tmp/add_custom.cpp"
        dst_file = "/tmp/add_custom.o"
        compile_log_path = None
        from asc_op_compile_base.asc_op_compiler.compile_op import _mssanitizer_link

        compile_op_module.global_var_storage.set_variable(
            "ascendc_asan_obj_path", {"Ascend910B": ""}
        )
        with mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""):
            _mssanitizer_link(src_file, dst_file, compile_log_path)
        self.assertEqual(src_file, "/tmp/add_custom.cpp")

    def test_get_sub_compile_info(self):
        from asc_op_compile_base.asc_op_compiler.compile_op import _get_sub_compile_info

        compile_info = CompileInfo()
        compile_info.kernel_name = "test"
        compile_info.dst_file = "test.o"
        kernel_type = 0
        sub_compile_info = _get_sub_compile_info(compile_info, kernel_type)
        self.assertEqual(sub_compile_info.dst_file, "test_mix_aiv.o")

    def test_compile_ascendc_cce_regbase(self):
        SetCurrentSocInfo("Ascend310B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        with buildcfg.build_config():
            infered_info_from_ifile = (
                KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                    op_info,
                    cce_file,
                    os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                    compile_option_tuple,
                    compile_log_path,
                    origin_func_name,
                )
            )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        arch = "dav-m300"

        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_regbase,
        )

        os.mknod(compile_info.gen_kernel_func_file)
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch.object(
                CommonUtility,
                "ascendc_build_aicore_compile_cmd",
                return_value=["ccec", "-x", "-c"],
            ),
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
            mock.patch("os.remove"),
        ):
            tiling_info.static_shape_flag = True
            DFXSectionGenerator().dfx_info_reset(op_info)
            gen_kernel_fun(
                compile_info,
                compile_info.origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            _compile_ascendc_cce_regbase(
                compile_info, compile_option_tuple, tiling_info
            )

            with open(compile_info.gen_kernel_func_file, "w") as temp_file:
                temp_file.write("")

            tiling_info.static_shape_flag = False
            DFXSectionGenerator().dfx_info_reset(op_info)
            gen_kernel_fun(
                compile_info,
                compile_info.origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            _compile_ascendc_cce_regbase(
                compile_info, compile_option_tuple, tiling_info
            )
        os.remove(compile_info.gen_kernel_func_file)
        self.assertEqual(compile_info.tiling_key_list, ["1"])

    def test_dynamic_regbase_kernel_list_to_json(self):
        SetCurrentSocInfo("Ascend310P1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom.cpp"
        )
        origin_func_name = "add_custom"
        code_channel = 0
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

        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.task_ration = 2  # AscendC only support 1:2
        tiling_info.file_content = ""
        tiling_info.tiling_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        tiling_info.tiling_data_file_path: str = ""
        tiling_info.tiling_data_size = 96  # TilingDef::getDataSize
        tiling_info.static_shape_flag: bool = True
        tiling_info.tiling_key = 1
        tiling_info.static_workspace_size = 104857600
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {"1": 96, "2": 100}
        tiling_info.default_tiling_size = 0
        tiling_info.clear_atomic = True
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _dynamic_regbase_kernel_list_to_json,
        )

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                CommonUtility.get_kernel_meta_dir()
                enable_mix_for_profiling = True
                enable_deterministic = True
                self.assertRaises(
                    Exception,
                    _dynamic_regbase_kernel_list_to_json,
                    op_info.kernel_name,
                    tiling_info.tiling_key_list,
                    enable_deterministic,
                    enable_mix_for_profiling,
                    {},
                )
                with mock.patch(
                    "builtins.open", new_callable=mock.mock_open, read_data="{}"
                ):
                    with mock.patch("json.dump") as mock_dump:
                        with mock.patch("os.chmod"):
                            mock_dump.side_effect = RuntimeError()
                            self.assertRaises(
                                Exception,
                                _dynamic_regbase_kernel_list_to_json,
                                op_info.kernel_name,
                                tiling_info.tiling_key_list,
                                enable_deterministic,
                                enable_mix_for_profiling,
                                {},
                            )
                            mock_dump.side_effect = None
                            _dynamic_regbase_kernel_list_to_json(
                                op_info.kernel_name,
                                tiling_info.tiling_key_list,
                                enable_deterministic,
                                enable_mix_for_profiling,
                                {"1": True, "2": False},
                            )
                            enable_deterministic = False
                            _dynamic_regbase_kernel_list_to_json(
                                op_info.kernel_name,
                                tiling_info.tiling_key_list,
                                enable_deterministic,
                                enable_mix_for_profiling,
                                {},
                            )

    def test_compile_ascendc_cce_v220(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )

        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_v220,
        )

        with mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""):
            with asc_op_compile_base.common.context.op_context.OpContext():
                with buildcfg.build_config():
                    CommonUtility.get_kernel_meta_dir()
                    DFXSectionGenerator().dfx_info_reset(op_info)
                    gen_kernel_fun(
                        compile_info,
                        origin_func_name,
                        op_info,
                        tiling_info,
                        CompileOptionTuple([], []),
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic.o')}"
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv.o')}"
                    )
                    _compile_ascendc_cce_v220(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic.o")
                        )
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv.o")
                        )
                    )
                    compile_info.hard_sync = True
                    compile_info.code_channel = 1
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic.o')}"
                    )
                    _compile_ascendc_cce_v220(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic.o")
                        )
                    )
                    compile_info.hard_sync = False
                    _compile_ascendc_cce_v220(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    with mock.patch.object(
                        compile_op_module, "call_bisheng_v220", return_value=["1"]
                    ):
                        with mock.patch.object(
                            compile_op_module, "fatbin_objs", return_value=["1"]
                        ):
                            tiling_info.static_shape_flag = False
                            compile_info.code_channel = CORE_TYPE_MIX
                            _compile_ascendc_cce_v220(
                                compile_info, compile_option_tuple, tiling_info
                            )
                            compile_info.hard_sync = True
                            compile_info.code_channel = CORE_TYPE_VEC
                            _compile_ascendc_cce_v220(
                                compile_info, compile_option_tuple, tiling_info
                            )

    @mock.patch("shutil.which")
    def test_compile_ascendc_cce_v200_with_kernel_type_for_static(self, mock_shutil):
        SetCurrentSocInfo("Ascend310P1")
        mock_shutil.return_value = "bisheng"
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/test.cpp"
        )
        compile_info = CompileInfo()
        compile_info.dst_file = os.path.join(TOP_PATH, "kernel_meta", "test.o")
        compile_info.kernel_name = "test"
        compile_info.origin_func_name = "test"
        compile_info.dump_info = {"dump_type": "", "dump_size": 1048576}
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = (
            distinct_tag + "_kernel_ascendc_cce_v200_with_kernel_type_for_static.cpp"
        )
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, "test" + file_name_tag
        )
        compile_option_tuple = CompileOptionTuple([], [])
        compile_info.tiling_key_list = ["1"]
        compile_info.tiling_key_kernel_type["1"] = (
            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE
        )

        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "y_in__",
                }
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
                    "param_name": "y_in__",
                }
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
            mc2_ctx=["mc2_test"],
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.tiling_data_file_path = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_tiling.h"
        )
        tiling_info.static_shape_flag = True
        tiling_info.tiling_key = 1

        compile_option_tuple = CompileOptionTuple([], [])
        CommonUtility.get_ascendc_compiler_path()
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_v200_with_kernel_type_for_static,
            _compile_ascendc_cce_v200_with_kernel_type,
        )

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
        ):
            CommonUtility.get_kernel_meta_dir()
            DFXSectionGenerator().dfx_info_reset(op_info)
            gen_kernel_fun(
                compile_info,
                compile_info.origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple([], []),
            )
            os.system(
                f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic.o')}"
            )
            os.system(
                f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv.o')}"
            )
            compile_info.tiling_key_kernel_type["1"] = (
                KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE
            )
            # _compile_ascendc_cce_v200_with_kernel_type_for_static(
            #     compile_info, compile_option_tuple, tiling_info)
            _compile_ascendc_cce_v200_with_kernel_type(
                compile_info, compile_option_tuple, tiling_info
            )
            self.assertFalse(
                os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic.o"))
            )
            self.assertFalse(
                os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv.o"))
            )
            compile_info.tiling_key_kernel_type["1"] = KernelMetaType.KERNEL_TYPE_AICORE
            _compile_ascendc_cce_v200_with_kernel_type_for_static(
                compile_info, compile_option_tuple, tiling_info
            )
            compile_info.tiling_key_kernel_type["1"] = (
                KernelMetaType.KERNEL_TYPE_AIV_ONLY
            )
            self.assertRaises(
                Exception,
                _compile_ascendc_cce_v200_with_kernel_type_for_static,
                compile_info,
                compile_option_tuple,
                tiling_info,
            )

    def test_gen_static_json_for_no_mix_v200(self):
        SetCurrentSocInfo("Ascend310P1")
        compile_info = CompileInfo()
        compile_info.kernel_name = "test"
        compile_info.origin_func_name = "test"
        compile_info.tiling_key_list = []

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.static_shape_flag = False

        from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
            enable_vector_core,
        )

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config() as cfg:
                with mock.patch("os.rename"):
                    cfg.current()[enable_vector_core] = True
                    _gen_static_json_for_no_mix_v200(
                        compile_info,
                        tiling_info,
                        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                    )
        self.assertEqual(tiling_info.task_ration, 2)

    def test_compile_ascendc_cce_v220_with_kernel_type(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )

        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_v220_with_kernel_type,
        )

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config() as cfg:
                with (
                    mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
                    mock.patch("os.system", return_value=0),
                    mock.patch("os.remove", return_value=0),
                ):
                    DFXSectionGenerator().dfx_info_reset(op_info)
                    gen_kernel_fun(
                        compile_info,
                        origin_func_name,
                        op_info,
                        tiling_info,
                        CompileOptionTuple([], []),
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', op_info.kernel_name + file_name_tag)}"
                    )
                    # _compile_ascendc_cce_v220_with_kernel_type_for_static
                    tiling_info.static_shape_flag = True
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic.o')}"
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv.o')}"
                    )
                    compile_info.tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
                    )
                    compile_info.raw_tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic.o")
                        )
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv.o")
                        )
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv.o')}"
                    )
                    compile_info.tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv.o")
                        )
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic.o')}"
                    )
                    compile_info.tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic.o")
                        )
                    )

                    compile_info.tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_AIV_ONLY
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )

                    # _compile_ascendc_cce_v220_with_kernel_type_for_dynamic
                    tiling_info.static_shape_flag = False
                    DFXSectionGenerator().dfx_info_reset(op_info)
                    gen_kernel_fun(
                        compile_info,
                        origin_func_name,
                        op_info,
                        tiling_info,
                        CompileOptionTuple([], []),
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic_1.o')}"
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv_1.o')}"
                    )
                    compile_info.tiling_key_kernel_type["1"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic_1.o")
                        )
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv_1.o")
                        )
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv_1.o')}"
                    )
                    compile_info.tiling_key_kernel_type["1"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv_1.o")
                        )
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic_1.o')}"
                    )
                    compile_info.tiling_key_kernel_type["1"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic_1.o")
                        )
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_1.o')}"
                    )
                    compile_info.tiling_key_kernel_type["1"] = (
                        KernelMetaType.KERNEL_TYPE_AIV_ONLY
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_1.o')}"
                    )
                    compile_info.tiling_key_kernel_type["1"] = (
                        KernelMetaType.KERNEL_TYPE_AIC_ONLY
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )

                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_1.o')}"
                    )
                    compile_info.tiling_key_kernel_type["1"] = (
                        KernelMetaType.KERNEL_TYPE_MAX
                    )
                    self.assertRaises(
                        Exception,
                        _compile_ascendc_cce_v220_with_kernel_type,
                        compile_info,
                        compile_option_tuple,
                        tiling_info,
                    )

                    os.remove(
                        os.path.join(
                            TOP_PATH, "kernel_meta", op_info.kernel_name + file_name_tag
                        )
                    )

    def test_compile_ascendc_cce_c310_with_kernel_type(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )

        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _compile_ascendc_cce_v220_with_kernel_type,
        )

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config() as cfg:
                with mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""):
                    DFXSectionGenerator().dfx_info_reset(op_info)
                    gen_kernel_fun(
                        compile_info,
                        origin_func_name,
                        op_info,
                        tiling_info,
                        CompileOptionTuple([], []),
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', op_info.kernel_name + file_name_tag)}"
                    )
                    tiling_info.static_shape_flag = True
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic.o')}"
                    )
                    os.system(
                        f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv.o')}"
                    )
                    compile_info.tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
                    )
                    compile_info.raw_tiling_key_kernel_type["0"] = (
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
                    )
                    _compile_ascendc_cce_v220_with_kernel_type(
                        compile_info, compile_option_tuple, tiling_info
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aic.o")
                        )
                    )
                    self.assertFalse(
                        os.path.exists(
                            os.path.join(TOP_PATH, "kernel_meta", "test_mix_aiv.o")
                        )
                    )

    def test_get_kernel_type_dict(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_json import (
            _get_kernel_type_dict,
        )

        compile_info = CompileInfo()
        tiling_key = "0"
        for kernel_type in list(KernelMetaType):
            compile_info.tiling_key_kernel_type["0"] = kernel_type
            if kernel_type == KernelMetaType.KERNEL_TYPE_MAX:
                self.assertRaises(
                    Exception, _get_kernel_type_dict, compile_info, tiling_key
                )
            else:
                _get_kernel_type_dict(compile_info, tiling_key)

    def test_gen_mix_json_from_seperate_json(self):
        kernel_name = "test"
        task_ration_str = ""
        core_type = 0
        no_set_kernel_type = True
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _gen_mix_json_from_seperate_json_for_kernel_type,
        )

        self.assertRaises(
            Exception,
            _gen_mix_json_from_seperate_json_for_kernel_type,
            kernel_name,
            task_ration_str,
            core_type,
            no_set_kernel_type,
        )
        with mock.patch("os.rename", return_value=0):
            self.assertRaises(
                Exception,
                _gen_mix_json_from_seperate_json,
                kernel_name,
                task_ration_str,
                core_type,
                no_set_kernel_type,
            )

        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.dump") as mock_dump,
            mock.patch("os.chmod"),
        ):
            _gen_mix_json_from_seperate_json_for_kernel_type(
                kernel_name, task_ration_str, core_type, no_set_kernel_type
            )
            mock_dump.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _gen_mix_json_from_seperate_json_for_kernel_type,
                kernel_name,
                task_ration_str,
                core_type,
                no_set_kernel_type,
            )
            with mock.patch("os.rename", return_value=0):
                self.assertRaises(
                    Exception,
                    _gen_mix_json_from_seperate_json,
                    kernel_name,
                    task_ration_str,
                    core_type,
                    no_set_kernel_type,
                )

    def test_compile_multi_tilingkey(self):
        tiling_key_list = ["1"]
        cmds_list = ["cmd"]
        dstfile_name = "test"
        compile_log_path = None
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            compile_multi_tilingkey,
        )

        with (
            mock.patch.object(
                CommonUtility, "get_build_file_name", return_value=["", "", ""]
            ),
            mock.patch("builtins.open", new_callable=mock.mock_open),
            mock.patch("os.chmod"),
            mock.patch("os.system", return_value=1),
            mock.patch("os.rename"),
        ):
            compile_op_module.global_var_storage.set_variable(
                "ascendc_enable_build_log", True
            )
            with mock.patch(
                "os.environ",
                {
                    "ASCENDC_PAR_COMPILE_JOB": "1",
                    "TILINGKEY_PAR_COMPILE": "1",
                },
            ):
                self.assertRaises(
                    Exception,
                    compile_multi_tilingkey,
                    tiling_key_list,
                    cmds_list,
                    dstfile_name,
                    compile_log_path,
                )

            with mock.patch(
                "os.environ",
                {
                    "ASCENDC_PAR_COMPILE_JOB": "1",
                    "TILINGKEY_PAR_COMPILE": "0",
                },
            ):
                self.assertRaises(
                    Exception,
                    compile_multi_tilingkey,
                    tiling_key_list,
                    cmds_list,
                    dstfile_name,
                    compile_log_path,
                )

            with mock.patch(
                "os.environ",
                {
                    "ASCENDC_PAR_COMPILE_JOB": "1",
                    "TILINGKEY_PAR_COMPILE": "0",
                },
            ):
                compile_op_module.global_var_storage.set_variable(
                    "ascendc_compile_debug_config", True
                )
                self.assertRaises(
                    Exception,
                    compile_multi_tilingkey,
                    tiling_key_list,
                    cmds_list,
                    dstfile_name,
                    compile_log_path,
                )

            with mock.patch(
                "os.environ",
                {
                    "ASCENDC_PAR_COMPILE_JOB": "0",
                    "TILINGKEY_PAR_COMPILE": "0",
                },
            ):
                self.assertRaises(
                    Exception,
                    compile_multi_tilingkey,
                    tiling_key_list,
                    cmds_list,
                    dstfile_name,
                    compile_log_path,
                )
        with (
            mock.patch.object(
                CommonUtility, "get_build_file_name", return_value=["", "", ""]
            ),
            mock.patch("builtins.open", new_callable=mock.mock_open),
            mock.patch("os.chmod"),
            mock.patch("os.system", return_value=0),
            mock.patch("os.rename"),
        ):
            compile_op_module.global_var_storage.set_variable(
                "ascendc_compile_debug_config", False
            )
            with mock.patch(
                "os.environ",
                {
                    "ASCENDC_PAR_COMPILE_JOB": "1",
                    "TILINGKEY_PAR_COMPILE": "0",
                },
            ):
                compile_multi_tilingkey(
                    tiling_key_list, [["cmd", "test"]], dstfile_name, compile_log_path
                )
        with (
            mock.patch.object(
                CommonUtility, "get_build_file_name", return_value=["", "", ""]
            ),
            mock.patch("builtins.open", new_callable=mock.mock_open),
            mock.patch("os.chmod"),
            mock.patch("os.system", return_value=0),
            mock.patch("os.rename"),
        ):
            compile_op_module.global_var_storage.set_variable(
                "ascendc_compile_debug_config", False
            )
            with mock.patch(
                "os.environ",
                {
                    "ASCENDC_PAR_COMPILE_JOB": "1",
                    "TILINGKEY_PAR_COMPILE": "2",
                },
            ):
                compile_multi_tilingkey(
                    tiling_key_list, [["cmd", "test"]], dstfile_name, compile_log_path
                )

    def test_judge_valid_for_v200(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v200 import (
            judge_valid_for_v200,
        )

        ValidKernelMetaType = [
            KernelMetaType.KERNEL_TYPE_AICORE,
            KernelMetaType.KERNEL_TYPE_VECTORCORE,
            KernelMetaType.KERNEL_TYPE_MIX_AICORE,
            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
        ]
        for kernel_type in list(ValidKernelMetaType):
            tiling_key_kernel_type = {"0": kernel_type}
            judge_valid_for_v200(tiling_key_kernel_type)

        InvalidKernelMetaType = [KernelMetaType.KERNEL_TYPE_AIV_ONLY]
        for kernel_type in list(InvalidKernelMetaType):
            tiling_key_kernel_type = {"0": kernel_type}
            self.assertRaises(Exception, judge_valid_for_v200, tiling_key_kernel_type)

    @mock.patch("os.environ", {"ASCENDC_COV": "1"})
    def test_is_enable_ascendc_cov(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import (
            is_enable_ascendc_cov,
        )

        self.assertTrue(is_enable_ascendc_cov())

    @mock.patch("os.environ", {"ASCEND_HOME_PATH": ""})
    def test_is_enable_sanitizer(self):
        set_current_compile_soc_info("Ascend910B")
        from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import (
            is_enable_sanitizer,
        )

        compile_options = []
        self.assertFalse(is_enable_sanitizer(compile_options))
        with mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": None}):
            compile_options = ["-sanitizer"]
            self.assertFalse(is_enable_sanitizer(compile_options))
        with mock.patch("os.path.exists") as mock_exists:
            with mock.patch.dict(
                "os.environ", {"ASCEND_HOME_PATH": os.environ.get("ASCEND_HOME_PATH")}
            ):
                mock_exists.return_value = True
                compile_options = ["-sanitizer"]
                self.assertTrue(is_enable_sanitizer(compile_options))
                with mock.patch(
                    "asc_op_compile_base.asc_op_compiler.global_storage.global_var_storage.get_variable"
                ) as mock_soc:
                    mock_soc.return_value = "Ascend950"
                    self.assertFalse(is_enable_sanitizer(compile_options))
        with mock.patch("os.path.exists") as mock_exists:
            with mock.patch.dict(
                "os.environ", {"ASCEND_HOME_PATH": "/usr/local/Ascend/cann"}
            ):
                mock_exists.return_value = False
                compile_options = ["-sanitizer"]
                self.assertRaises(Exception, is_enable_sanitizer, compile_options)

    def test_add_op_compile_options_by_customized_json(self):
        SetCurrentSocInfo("Ascend910B1")
        compile_option_tuple = CompileOptionTuple([], [])
        op_compile_option = ""
        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _add_op_compile_options_by_customized_json,
        )

        with mock.patch("json.loads") as mock_json:
            js = {
                "compile_options": {
                    "__ALL__": ["-mllvm test", "test"],
                    "ascend910b": ["-mllvm test", "test"],
                }
            }
            mock_json.return_value = js
            _add_op_compile_options_by_customized_json(
                op_compile_option, compile_option_tuple
            )
        self.assertEqual(
            compile_option_tuple.mllvm_options, ["-mllvm", "test", "-mllvm", "test"]
        )

    def test_get_code_for_l2_cache(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_code import (
            get_code_for_l2_cache,
        )

        compile_info = CompileInfo()
        compile_info.tiling_key_list = ["0"]
        tiling_info = TilingInfo()
        tiling_info.static_shape_flag = True
        tiling_info.tiling_key = 0

        source = ""
        result = get_code_for_l2_cache(compile_info, source, tiling_info)
        self.assertNotEqual(source, result)

        with mock.patch.object(CommonUtility, "is_v220", return_value=True):
            compile_info.no_set_kernel_type = False
            compile_info.tiling_key_kernel_type["0"] = (
                KernelMetaType.KERNEL_TYPE_AIC_ONLY
            )
            source = ""
            result = get_code_for_l2_cache(compile_info, source, tiling_info)
            self.assertNotEqual(source, result)

            compile_info.no_set_kernel_type = True
            compile_info.code_channel = 1
            compile_info.tiling_key_kernel_type["0"] = (
                KernelMetaType.KERNEL_TYPE_AIC_ONLY
            )
            source = ""
            result = get_code_for_l2_cache(compile_info, source, tiling_info)
            self.assertNotEqual(source, result)

        with mock.patch.object(CommonUtility, "is_v200", return_value=True):
            compile_info.no_set_kernel_type = False
            compile_info.tiling_key_kernel_type["0"] = (
                KernelMetaType.KERNEL_TYPE_VECTORCORE
            )
            source = ""
            result = get_code_for_l2_cache(compile_info, source, tiling_info)
            self.assertNotEqual(source, result)

            compile_info.no_set_kernel_type = True
            compile_info.code_channel = 1
            compile_info.tiling_key_kernel_type["0"] = (
                KernelMetaType.KERNEL_TYPE_AIC_ONLY
            )
            source = ""
            result = get_code_for_l2_cache(compile_info, source, tiling_info)
            self.assertNotEqual(source, result)

        with mock.patch.object(CommonUtility, "is_v220", return_value=False):
            with mock.patch.object(CommonUtility, "is_v200", return_value=False):
                compile_info.no_set_kernel_type = False
                compile_info.tiling_key_kernel_type["0"] = (
                    KernelMetaType.KERNEL_TYPE_VECTORCORE
                )
                source = ""
                result = get_code_for_l2_cache(compile_info, source, tiling_info)
                self.assertNotEqual(source, result)

    def test_gen_usr_origin_kernel_function_call(self):
        func_name = "test"
        tiling_info = TilingInfo()
        op_info = OpInfo(
            origin_inputs=[
                None,
                [],
                [{"param_name": "test"}],
                {
                    "shape": [-2],
                    "ori_shape": [-2],
                    "format": "ND",
                    "ori_format": "ND",
                    "dtype": "float32",
                    "range": [(0, None)],
                    "param_name": "x_in__",
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
            output_shape_depend_on_compute=[],
        )
        origin_input = op_info.origin_inputs
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_code import (
            gen_usr_origin_kernel_function_call,
        )

        tiling_info.static_shape_flag = True
        self.assertNotEqual(
            gen_usr_origin_kernel_function_call(func_name, op_info, tiling_info), ""
        )
        tiling_info.static_shape_flag = False
        self.assertNotEqual(
            gen_usr_origin_kernel_function_call(func_name, op_info, tiling_info), ""
        )

    def test_gen_set_workspace_codes(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )

        from asc_op_compile_base.asc_op_compiler.compile_op import (
            _gen_set_workspace_codes,
        )

        is_mix = True
        is_single_and_using_hard_sync = True
        dump_size = 0
        compile_info.dump_info["dump_type"] = ["printf"]
        result = _gen_set_workspace_codes(
            is_mix,
            is_single_and_using_hard_sync,
            op_info,
            tiling_info,
            compile_options,
            compile_info,
        )
        self.assertNotEqual(result, "")
        self.assertIn(
            "    GM_ADDR usrWorkspace = AscendC::GetUserWorkspace(workspace);\n",
            result,
        )
        with mock.patch.object(
            CommonUtility, "is_support_workspace_offset", return_value=True
        ):
            result = _gen_set_workspace_codes(
                is_mix,
                is_single_and_using_hard_sync,
                op_info,
                tiling_info,
                compile_options,
                compile_info,
            )
        self.assertIn(
            "#if ENABLE_CV_COMM_VIA_SSBUF != 0 && __MIX_CORE_AIC_RATION__ != 1\n"
            "    GM_ADDR usrWorkspace = workspace;\n"
            "#else\n"
            "    GM_ADDR usrWorkspace = workspace + AscendC::RESERVED_WORKSPACE;\n"
            "#endif\n",
            result,
        )
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
            result = _gen_set_workspace_codes(
                is_mix,
                is_single_and_using_hard_sync,
                op_info,
                tiling_info,
                compile_options,
                compile_info,
            )
        self.assertNotEqual(result, "")

    def test_dump_compile_log(self):
        with open(os.devnull, "a") as file:
            with mock.patch("os.open", return_value=file):
                with mock.patch("os.fdopen") as mock_fdopen:
                    cmd = []
                    stage = "test"
                    log_file = ""
                    CommonUtility.dump_compile_log(cmd, stage, log_file)
                    mock_fdopen.side_effect = RuntimeError()
                    self.assertRaises(
                        Exception, CommonUtility.dump_compile_log, cmd, stage, log_file
                    )

    def test_dump_log(self):
        log_str = "test"
        log_file = ""
        with open(os.devnull, "a") as file:
            with mock.patch("os.open", return_value=file):
                with mock.patch("os.fdopen") as mock_fdopen:
                    CommonUtility.dump_log(log_str, log_file)
                    mock_fdopen.side_effect = RuntimeError()
                    self.assertRaises(
                        Exception, CommonUtility.dump_log, log_str, log_file
                    )

    def test_run_cmd_stack_spill_case_ascendc(self):
        cmd = []
        cmd_type = ""
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ):
                result = CommonUtility.run_cmd_stack_spill_case_ascendc(cmd, cmd_type)
                self.assertEqual(result[0], None)

    def test_run_cmd_stackoverflow_ascnendc(self):
        cmd = []
        cmd_type = ""
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ):
                SetCurrentSocInfo("Ascend310P1")
                result = CommonUtility.run_cmd_stackoverflow_ascnendc(cmd, cmd_type)
                self.assertEqual(result[0], None)

    def test_run_cmd_ascendc(self):
        cmd_type = "compile"
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ):
                SetCurrentSocInfo("Ascend310P1")
                cmd = ["--cce-aicore-only", "-O0", "-g"]
                result = CommonUtility.run_cmd_ascendc(cmd, cmd_type)
                self.assertEqual(result[0], None)
                cmd = ["--cce-aicore-only"]
                result = CommonUtility.run_cmd_ascendc(cmd, cmd_type)
                self.assertEqual(result[0], None)

    def test_run_cmd_inner(self):
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ):
                SetCurrentSocInfo("Ascend310P1")
                cmd = ["--cce-aicore-only", "-O0", "-g", ".o"]
                compile_op_module.global_var_storage.set_variable(
                    "ascendc_compile_debug_config", True
                )
                s = "unsupported API".encode("utf-8")
                with mock.patch.object(
                    CommonUtility, "run_cmd_ascendc", return_value=[s, 0]
                ):
                    with mock.patch.object(
                        CommonUtility, "dump_build_log", return_value=[s, 0]
                    ):
                        result = CommonUtility.run_cmd_inner(
                            cmd, CompileStage.COMPILE, None
                        )
                        self.assertEqual(result, None)

    def test_run_cmd_inner_c310(self):
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ):
                SetCurrentSocInfo("Ascend950PR_9599")
                cmd = ["--cce-aicore-only", "-O0", "-g", ".o"]
                compile_op_module.global_var_storage.set_variable(
                    "ascendc_compile_debug_config", True
                )
                s = "unsupported API".encode("utf-8")
                with mock.patch.object(
                    CommonUtility, "run_cmd_ascendc", return_value=[s, 0]
                ):
                    with mock.patch.object(
                        CommonUtility, "dump_build_log", return_value=[s, 0]
                    ):
                        result = CommonUtility.run_cmd_inner(
                            cmd, CompileStage.COMPILE, None
                        )
                        self.assertEqual(result, None)

    def test_run_cmd_inner_jump(self):
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ):
                SetCurrentSocInfo("Ascend310P1")
                cmd = ["--cce-aicore-only", "-O0", "-g", ".o"]
                compile_op_module.global_var_storage.set_variable(
                    "ascendc_compile_debug_config", True
                )
                s = "unsupported API".encode("utf-8")
                with mock.patch.object(
                    CommonUtility, "run_cmd_ascendc"
                ) as mock_run_cmd_ascendc:
                    mock_run_cmd_ascendc.side_effect = [(s, 1), (s, 1), (s, 0)]
                    with mock.patch.object(
                        CommonUtility, "dump_build_log", return_value=[s, 0]
                    ):
                        result = CommonUtility.run_cmd_inner(
                            cmd, CompileStage.COMPILE, None
                        )
                        self.assertEqual(result, None)

    def test_write_mk_v200(self):
        SetCurrentSocInfo("Ascend310P1")
        tiling_key_list = ["1"]
        cmds_list = [["cce"]]
        dstfile_name = "test"
        compile_log_path = None
        s = "unsupported API".encode("utf-8")
        from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import write_mk

        with (
            mock.patch("subprocess.Popen"),
            mock.patch.object(
                CommonUtility, "get_build_file_name", return_value=["", "", ""]
            ),
            mock.patch.object(CommonUtility, "is_v200", return_value=True),
            mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[None, None]
            ),
            mock.patch.object(CommonUtility, "dump_build_log", return_value=[s, 0]),
            mock.patch("builtins.open", new_callable=mock.mock_open),
            mock.patch("os.chmod"),
            mock.patch("os.system", return_value=1),
            mock.patch("os.rename"),
        ):
            result = write_mk(tiling_key_list, cmds_list, dstfile_name, None)
            self.assertEqual(result, None)

    def test_get_distinct_filename_tag(self):
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        tag = CommonUtility.get_distinct_filename_tag()
        self.assertEqual("", tag)

    def test_ascendc_raise_python_err(self):
        self.assertRaises(Exception, CommonUtility.ascendc_raise_python_err, 1, "error")

    def test_check_debug_options(self):
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["ccec_O0", "ccec_g"]
            self.assertTrue(CommonUtility.check_debug_options(["-O2", "-O3"]))

    def test_ascendc_read_file(self):
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open) as mock_open,
            mock.patch("os.chmod"),
            mock.patch("os.rename"),
        ):
            mock_open.side_effect = RuntimeError()
            self.assertRaises(Exception, CommonUtility.ascendc_read_file, " ")

    def test_ascendc_write_file(self):
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open) as mock_open,
            mock.patch("os.chmod"),
            mock.patch("os.rename"),
        ):
            mock_open.side_effect = RuntimeError()
            self.assertRaises(Exception, CommonUtility.ascendc_write_file, " ", " ")

    def test_get_kernel_meta_dir(self):
        with mock.patch("os.path.exists", return_value=False):
            with mock.patch("os.makedirs") as mock_mkdir:
                result = CommonUtility.get_kernel_meta_dir()
                self.assertTrue(result)
                mock_mkdir.side_effect = RuntimeError()
                self.assertRaises(Exception, CommonUtility.get_kernel_meta_dir)

    def test_dump_build_log(self):
        with (
            mock.patch.object(CommonUtility, "print_compile_log"),
            mock.patch("builtins.open", new_callable=mock.mock_open) as mock_open,
            mock.patch("os.chmod"),
            mock.patch("os.rename"),
        ):
            cmds = ["test.o"]
            stage = CompileStage.PRECOMPILE
            output = "ret_code=1, ascendc_enable_build_log is False"
            compile_op_module.global_var_storage.set_variable(
                "ascendc_enable_build_log", False
            )
            ret_code = 1
            self.assertRaises(
                Exception, CommonUtility.dump_build_log, output, cmds, stage, ret_code
            )
            output = "ret_code=1, ascendc_enable_build_log is True"
            compile_op_module.global_var_storage.set_variable(
                "ascendc_enable_build_log", True
            )
            self.assertRaises(
                Exception, CommonUtility.dump_build_log, output, cmds, stage, ret_code
            )
            output = "ret_code=0, ascendc_enable_build_log is True"
            compile_op_module.global_var_storage.set_variable(
                "ascendc_enable_build_log", True
            )
            ret_code = 0
            CommonUtility.dump_build_log(output, cmds, stage, ret_code)
            output = "WARNING: ret_code=0 ascendc_enable_build_log is True"
            CommonUtility.dump_build_log(output, cmds, stage, ret_code)
            stage = CompileStage.FATBIN
            output = (
                "stage=CompileStage.FATBIN, ret_code=0 ascendc_enable_build_log is True"
            )
            CommonUtility.dump_build_log(output, cmds, stage, ret_code)
            stage = CompileStage.LINKRELOCATE
            output = "stage=CompileStage.LINKRELOCATE, ret_code=0 ascendc_enable_build_log is True"
            CommonUtility.dump_build_log(output, cmds, stage, ret_code)
            mock_open.side_effect = RuntimeError()
            self.assertRaises(
                Exception, CommonUtility.dump_build_log, output, cmds, stage, ret_code
            )

    def test_get_v220_kernel_type_mix_flag(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        origin_func_name = "add_custom_unalign"
        code_channel = 0
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
            "-mllvm",
            "-api-deps-filter",
        ]

        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = infered_info_from_ifile.tiling_key_list
        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = False
        tiling_info.static_shape_flag = True
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_tiling_data.h"
        tiling_data_file_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        tiling_info.save_file(tiling_data_file_path)
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = (
            infered_info_from_ifile.tiling_key_kernel_type
        )
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )

        from asc_op_compile_base.asc_op_compiler.compile_op import (
            get_v220_kernel_type_mix_flag,
        )

        with mock.patch.object(CommonUtility, "is_v220", return_value=False):
            result = get_v220_kernel_type_mix_flag(compile_info, tiling_info)
            self.assertEqual(result, (False, compile_info.hard_sync))
        compile_info.no_set_kernel_type = False
        tiling_info.tiling_key = 0
        compile_info.tiling_key_list = ["0"]
        compile_info.tiling_key_kernel_type["0"] = KernelMetaType.KERNEL_TYPE_AIV_ONLY
        result = get_v220_kernel_type_mix_flag(compile_info, tiling_info)
        self.assertEqual(result, (False, False))
        compile_info.tiling_key_kernel_type["0"] = KernelMetaType.KERNEL_TYPE_AIC_ONLY
        result = get_v220_kernel_type_mix_flag(compile_info, tiling_info)
        self.assertEqual(result, (False, False))
        compile_info.tiling_key_kernel_type["0"] = (
            KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC
        )
        result = get_v220_kernel_type_mix_flag(compile_info, tiling_info)
        self.assertEqual(result, (True, compile_info.hard_sync))
        compile_info.tiling_key_kernel_type["0"] = (
            KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC
        )
        result = get_v220_kernel_type_mix_flag(compile_info, tiling_info)
        self.assertEqual(result, (True, compile_info.hard_sync))

    def test_get_kernel_type_enum(self):
        compile_log_path = None
        with mock.patch.object(CommonUtility, "is_v220", return_value=True):
            kernel_type = "KERNEL_TYPE_AIV_ONLY"
            kernel_type_enum = KernelInfoInfer.get_kernel_type_enum(
                kernel_type, compile_log_path
            )
            self.assertEqual(kernel_type_enum, KernelMetaType.KERNEL_TYPE_AIV_ONLY)
            kernel_type = "KERNEL_TYPE_AICORE"
            kernel_type_enum = KernelInfoInfer.get_kernel_type_enum(
                kernel_type, compile_log_path
            )
            self.assertEqual(kernel_type_enum, None)
        with mock.patch.object(CommonUtility, "is_v220", return_value=False):
            with mock.patch.object(CommonUtility, "is_v200", return_value=True):
                kernel_type = "KERNEL_TYPE_AICORE"
                kernel_type_enum = KernelInfoInfer.get_kernel_type_enum(
                    kernel_type, compile_log_path
                )
                self.assertEqual(kernel_type_enum, KernelMetaType.KERNEL_TYPE_AICORE)
                kernel_type = "KERNEL_TYPE_AIV_ONLY"
                kernel_type_enum = KernelInfoInfer.get_kernel_type_enum(
                    kernel_type, compile_log_path
                )
                self.assertEqual(kernel_type_enum, None)
        with mock.patch.object(CommonUtility, "is_v220", return_value=False):
            with mock.patch.object(CommonUtility, "is_v200", return_value=False):
                self.assertRaises(
                    Exception,
                    KernelInfoInfer.get_kernel_type_enum,
                    kernel_type,
                    compile_log_path,
                )

    def test_is_enable_build_log(self):
        SetCurrentSocInfo("Ascend910B1")
        from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import (
            is_enable_build_log,
        )

        with mock.patch("os.environ", {"ASCENDC_BUILD_LOG_DIR": None}):
            result = is_enable_build_log()
            self.assertFalse(result)
        with mock.patch("os.environ", {"ASCENDC_BUILD_LOG_DIR": "None"}):
            with mock.patch("os.path.exists", return_value=False):
                with mock.patch("os.makedirs") as mock_mkdir:
                    result = is_enable_build_log()
                    self.assertTrue(result)
                    mock_mkdir.side_effect = RuntimeError()
                    self.assertRaises(Exception, is_enable_build_log)
        with mock.patch("os.environ", {"ASCENDC_BUILD_LOG_DIR": "."}):
            get_soc = global_var_storage.get_variable("ascendc_short_soc_version")
            global_var_storage.set_variable("ascendc_short_soc_version", "ASCEND_310P")
            with mock.patch("os.path.exists", return_value=False):
                with mock.patch("os.makedirs") as mock_mkdir:
                    result = is_enable_build_log()
                    self.assertTrue(result)
                    mock_mkdir.side_effect = RuntimeError()
                    self.assertRaises(Exception, is_enable_build_log)
            global_var_storage.set_variable("ascendc_short_soc_version", get_soc)

    def test_check_if_gen_placehoder(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_utils import (
            check_if_gen_placehoder,
            GEN_PLACE_HOLDER_STR,
        )

        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                op_info = OpInfo(
                    kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
                    op_type="AddCustomUnalign",
                    inputs=[],
                )
                result = check_if_gen_placehoder(op_info, True)
                self.assertFalse(result)
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value=GEN_PLACE_HOLDER_STR,
                ):
                    result = check_if_gen_placehoder(op_info, True)
                    self.assertFalse(result)
                    op_info = OpInfo(
                        kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
                        op_type="AddCustomUnalign",
                        inputs=[
                            {
                                "shape": [-2],
                                "ori_shape": [-12],
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
                    )
                    result = check_if_gen_placehoder(op_info, True)
                    self.assertTrue(result)
                    op_info = OpInfo(
                        kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
                        op_type="AddCustomUnalign",
                        inputs=[None],
                    )
                    self.assertRaises(Exception, check_if_gen_placehoder, op_info, True)

    def test_json_post_process(self):
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
                    "param_name": "a_out_",
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
                    "param_name": "b_out_",
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
                    "param_name": "c_out_",
                },
            ],
            attrs=[],
            impl_mode="",
            param_type_dynamic=True,
            mc2_ctx=["aaa"],
            param_type_list=[
                "required",
                "required",
                "required",
                "required",
                "required",
            ],
            init_value_list=["1", "a", None],
            output_shape_depend_on_compute=[],
        )
        from asc_op_compile_base.asc_op_compiler.compile_op import _json_post_process

        kernel_name = "te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5"
        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.task_ration = 2  # AscendC only support 1:2
        tiling_info.file_content = ""
        tiling_info.tiling_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        tiling_info.tiling_data_file_path: str = ""
        tiling_info.tiling_data_size = 96  # TilingDef::getDataSize
        tiling_info.static_shape_flag: bool = True
        tiling_info.tiling_key = 1
        tiling_info.static_workspace_size = 104857600
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {"1": 96, "2": 100}
        tiling_info.default_tiling_size = 0
        tiling_info.clear_atomic = True
        tiling_info.schedule_mode = 1
        need_gen_placehoder = True
        enable_deterministic = True
        dump_info = {"dump_type": "test", "dump_size": 0}
        from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
            enable_deterministic_mode,
        )

        compile_info = CompileInfo()
        compile_info.kernel_name = kernel_name
        compile_info.enable_deterministic = enable_deterministic
        compile_info.dump_info = {"dump_type": ""}
        compile_info.super_kernel_info["timestamp_option"] = True
        compile_info.super_kernel_info["kernel_name"] = kernel_name
        compile_info.super_kernel_info["sp_options"] = {
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionDisable
        }
        compile_info.super_kernel_info["op_list"] = [
            {
                "bin_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.o",
                "json_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.json",
                "stream_id": 0,
                "task_type": "normal",
            },
            {
                "bin_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.o",
                "json_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.json",
                "stream_id": 0,
                "task_type": "normal",
            },
        ]
        compile_info.super_kernel_info["param_offset"] = [1, 8]
        compile_info.super_kernel_info["notify_param_offset"] = [8, 13]
        compile_info.super_kernel_info["wait_param_offset"] = [8, 13]
        compile_info.super_kernel_info["send_event_list"] = [1, 2]
        compile_info.super_kernel_info["recv_event_list"] = [3, 4]
        compile_info.super_kernel_info["debug_option"] = "printf"
        compile_info.super_kernel_info["debug_size"] = 1024
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config() as cfg,
            mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open,
            mock.patch("json.load") as mock_load,
            mock.patch("json.loads") as mock_loads,
            mock.patch("hashlib.sha256", return_value=hashlib.sha256()) as mock_hash,
            mock.patch("os.chmod"),
            mock.patch("json.dump") as mock_dump,
        ):
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_load.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_load.side_effect = None
            cfg.current()[enable_deterministic_mode] = True
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            cfg.current()["tir.op_debug_config"] = ["oom"]
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_loads.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_loads.side_effect = None
            tiling_info.clear_atomic = False
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_hash.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_hash.side_effect = None
            mock_dump.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )

    def test_json_post_process_sigle_stream(self):
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
                    "param_name": "a_out_",
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
                    "param_name": "b_out_",
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
                    "param_name": "c_out_",
                },
            ],
            attrs=[],
            impl_mode="",
            param_type_dynamic=True,
            mc2_ctx=["aaa"],
            param_type_list=[
                "required",
                "required",
                "required",
                "required",
                "required",
            ],
            init_value_list=["1", "a", None],
            output_shape_depend_on_compute=[],
        )
        from asc_op_compile_base.asc_op_compiler.compile_op import _json_post_process

        kernel_name = "te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5"
        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.task_ration = 2  # AscendC only support 1:2
        tiling_info.file_content = ""
        tiling_info.tiling_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        tiling_info.tiling_data_file_path: str = ""
        tiling_info.tiling_data_size = 96  # TilingDef::getDataSize
        tiling_info.static_shape_flag: bool = True
        tiling_info.tiling_key = 1
        tiling_info.static_workspace_size = 104857600
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {"1": 96, "2": 100}
        tiling_info.default_tiling_size = 0
        tiling_info.clear_atomic = True
        tiling_info.schedule_mode = 1
        need_gen_placehoder = True
        enable_deterministic = True
        dump_info = {"dump_type": "test", "dump_size": 0}
        from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
            enable_deterministic_mode,
        )

        compile_info = CompileInfo()
        compile_info.kernel_name = kernel_name
        compile_info.enable_deterministic = enable_deterministic
        compile_info.dump_info = {"dump_type": ""}
        compile_info.super_kernel_info["timestamp_option"] = True
        compile_info.super_kernel_info["kernel_name"] = kernel_name
        compile_info.super_kernel_info["sp_options"] = {
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable
        }
        compile_info.super_kernel_info["op_list"] = [
            {
                "bin_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.o",
                "json_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.json",
                "stream_id": 0,
                "task_type": "normal",
            },
            {
                "bin_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.o",
                "json_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.json",
                "stream_id": 0,
                "task_type": "normal",
            },
        ]
        compile_info.super_kernel_info["param_offset"] = [1, 8]
        compile_info.super_kernel_info["notify_param_offset"] = [8, 13]
        compile_info.super_kernel_info["wait_param_offset"] = [8, 13]
        compile_info.super_kernel_info["send_event_list"] = [1, []]
        compile_info.super_kernel_info["recv_event_list"] = [3, []]
        compile_info.super_kernel_info["debug_option"] = "printf"
        compile_info.super_kernel_info["debug_size"] = 1024
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config() as cfg,
            mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open,
            mock.patch("json.load") as mock_load,
            mock.patch("json.loads") as mock_loads,
            mock.patch("hashlib.sha256", return_value=hashlib.sha256()) as mock_hash,
            mock.patch("os.chmod"),
            mock.patch("json.dump") as mock_dump,
        ):
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_load.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_load.side_effect = None
            cfg.current()[enable_deterministic_mode] = True
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            cfg.current()["tir.op_debug_config"] = ["oom"]
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_loads.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_loads.side_effect = None
            tiling_info.clear_atomic = False
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_hash.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )
            mock_hash.side_effect = None
            mock_dump.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                _json_post_process,
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )

    def test_json_post_process_no_tiling_register(self):
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
                    "param_name": "a_out_",
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
                    "param_name": "b_out_",
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
                    "param_name": "c_out_",
                },
            ],
            attrs=[],
            impl_mode="",
            param_type_dynamic=True,
            mc2_ctx=["aaa"],
            param_type_list=[
                "required",
                "required",
                "required",
                "required",
                "required",
            ],
            init_value_list=["1", "a", None],
            output_shape_depend_on_compute=[],
        )
        from asc_op_compile_base.asc_op_compiler.compile_op import _json_post_process

        kernel_name = "te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5"
        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.task_ration = 2  # AscendC only support 1:2
        tiling_info.file_content = ""
        tiling_info.tiling_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        tiling_info.tiling_data_file_path: str = ""
        tiling_info.tiling_data_size = 96  # TilingDef::getDataSize
        tiling_info.static_shape_flag: bool = True
        tiling_info.tiling_key = 1
        tiling_info.static_workspace_size = 104857600
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {"1": 96, "2": 100}
        tiling_info.default_tiling_size = 0
        tiling_info.clear_atomic = True
        tiling_info.schedule_mode = 1
        need_gen_placehoder = True
        enable_deterministic = True
        dump_info = {"dump_type": "test", "dump_size": 0}
        from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
            enable_deterministic_mode,
        )

        compile_info = CompileInfo()
        compile_info.kernel_name = kernel_name
        compile_info.enable_deterministic = enable_deterministic
        compile_info.dump_info = {"dump_type": ""}
        compile_info.super_kernel_info["timestamp_option"] = True
        compile_info.super_kernel_info["kernel_name"] = kernel_name
        compile_info.super_kernel_info["sp_options"] = {
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionDisable
        }
        compile_info.super_kernel_info["op_list"] = [
            {
                "bin_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.o",
                "json_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.json",
                "stream_id": 0,
                "task_type": "normal",
            },
            {
                "bin_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.o",
                "json_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.json",
                "stream_id": 0,
                "task_type": "normal",
            },
        ]
        compile_info.super_kernel_info["param_offset"] = [1, 8]
        compile_info.super_kernel_info["notify_param_offset"] = [8, 13]
        compile_info.super_kernel_info["wait_param_offset"] = [8, 13]
        compile_info.super_kernel_info["send_event_list"] = [1, 2]
        compile_info.super_kernel_info["recv_event_list"] = [3, 4]
        compile_info.super_kernel_info["debug_option"] = "printf"
        compile_info.super_kernel_info["debug_size"] = 1024
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config() as cfg,
            mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open,
            mock.patch("json.load") as mock_load,
            mock.patch("json.loads") as mock_loads,
            mock.patch("hashlib.sha256", return_value=hashlib.sha256()) as mock_hash,
            mock.patch("os.chmod"),
            mock.patch("json.dump") as mock_dump,
        ):
            global_var_storage.set_variable("ascendc_tiling_no_register", True)
            _json_post_process(
                compile_info,
                op_info,
                tiling_info,
                need_gen_placehoder,
                need_gen_placehoder,
                dump_info,
            )

    def test_json_except_info(self):
        compile_info = CompileInfo()
        compile_info.kernel_name = "te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5"
        compile_info.dump_info = {"dump_type": ""}
        compile_info.super_kernel_info["timestamp_option"] = True
        compile_info.super_kernel_info["kernel_name"] = (
            "te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5"
        )
        compile_info.super_kernel_info["sp_options"] = {
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable
        }
        compile_info.super_kernel_info["op_list"] = [
            {
                "bin_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.o",
                "json_path": "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.json",
                "stream_id": 0,
                "task_type": "normal",
            },
            {
                "bin_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.o",
                "json_path": "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.json",
                "stream_id": 0,
                "task_type": "normal",
            },
        ]
        compile_info.super_kernel_info["param_offset"] = [1, 8]
        compile_info.super_kernel_info["notify_param_offset"] = [8, 13]
        compile_info.super_kernel_info["wait_param_offset"] = [8, 13]
        compile_info.super_kernel_info["send_event_list"] = [1, []]
        compile_info.super_kernel_info["recv_event_list"] = [3, []]
        compile_info.super_kernel_info["debug_option"] = "printf"
        compile_info.super_kernel_info["debug_size"] = 1024
        from asc_op_compile_base.asc_op_compiler.compile_op import _json_except_info

        json_file_path_1 = "./te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5.json"
        json_file_path_2 = "./te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e.json"
        os.makedirs(os.path.dirname(json_file_path_1), exist_ok=True)
        os.makedirs(os.path.dirname(json_file_path_2), exist_ok=True)
        content_1 = {
            "binFileName": "te_addcustom_c3ddf0b6b7cfcb0fa81511155a4d588722c7b8412920ecfe1b049dec430667c5",
            "blockDim": 48,
            "coreType": "VectorCore",
            "deterministic": "ignore",
            "intercoreSync": 0,
            "kernelName": "te_transdata_460196a1c24e1a897faaef38129e351235b9cc6fc0a2b8af693296d2087a81f0__kernel0",
            "magic": "RT_DEV_BINARY_MAGIC_ELF_AIVEC",
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "split_mode": 4,
            "debug_option": "printf",
            "debug_size": 1024,
        }
        content_2 = {
            "binFileName": "te_dequantswigluquant_c15e5bac0a77649656269990f41afcf1d0bf6e714342f104a6682f80963ca17e",
            "blockDim": 48,
            "coreType": "VectorCore",
            "deterministic": "ignore",
            "intercoreSync": 0,
            "kernelName": "te_transdata_460196a1c24e1a897faaef38129e351235b9cc6fc0a2b8af693296d2087a81f0__kernel0",
            "magic": "RT_DEV_BINARY_MAGIC_ELF_AIVEC",
            "sub_operator_kernel_type": "KERNEL_TYPE_MIX_AIC_1_1",
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "split_mode": 4,
            "debug_option": "printf",
            "debug_size": 1024,
        }
        with open(json_file_path_1, "w", encoding="utf-8") as file:
            json.dump(content_1, file, ensure_ascii=False, indent=4)

        with open(json_file_path_2, "w", encoding="utf-8") as file:
            json.dump(content_2, file, ensure_ascii=False, indent=4)

        super_dfx_list = _json_except_info(compile_info)
        self.assertTrue(len(super_dfx_list) > 0)

        os.remove(json_file_path_1)
        os.remove(json_file_path_2)

    def test_is_static_shape(self):
        inputs = [
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
            }
        ]
        outputs_static = [
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
            }
        ]
        outputs_dynamic = [
            {
                "shape": (-1,),
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
        ]
        outputs_dynamic_none = [None]
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import is_static_shape

        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                flag = is_static_shape(inputs, outputs_static)
                self.assertTrue(flag)
                flag = is_static_shape(inputs, outputs_dynamic)
                self.assertFalse(flag)
                flag = is_static_shape(inputs, outputs_dynamic_none)
                self.assertTrue(flag)

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
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import TilingDef
        from asc_op_compile_base.asc_op_compiler.get_op_tiling import get_tiling_info

        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            buildcfg.build_config(),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.get_op_tiling.get_tiling_def",
                return_value=TilingDef(
                    {"class_name": "add_custiom", "data_size": 0, "fields": []}
                ),
            ),
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
            ),
        ):
            tiling_info = get_tiling_info(op_info, ["1"], None, False, {})
            self.assertEqual(tiling_info.block_num, 1)
            self.assertEqual(tiling_info.clear_atomic, 0)
            self.assertEqual(tiling_info.schedule_mode, 0)

    @mock.patch("os.path.exists")
    def test_update_compile_info(self, mock_exist):
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        extend_options = {"opp_kernel_hidden_dat_path": os.path.join(TOP_PATH, "tmp")}

        def custom_exists(path):
            if (
                path
                == "/usr/local/Ascend/cann/compiler/tikcpp/../../include/version/asc_devkit_version.h"
            ):
                return True
            else:
                return mock.DEFAULT

        mock_exist.side_effect = custom_exists

        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        _update_compile_option(
            "my_test", compile_option_tuple.compile_options, extend_options
        )
        self.assertTrue("-cce-vfs" in compile_option_tuple.compile_options)

    def test_compile_op_template(self):
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH,
            "tests/python/asc_op_compiler/stub_kernels/add_custom_template.cpp",
        )
        origin_func_name = "add_custom_template"
        code_channel = 0
        op_info = OpInfo(
            kernel_name="AddCustomTemplate_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomTemplate",
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
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                compile_op(
                    cce_file,
                    origin_func_name,
                    op_info,
                    compile_options,
                    code_channel,
                    op_compile_option,
                )

        binary_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".o")
        json_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".json")
        self.assertTrue(os.path.exists(binary_file))
        os.remove(binary_file)
        os.remove(json_file)
        for item in os.listdir(os.path.join(TOP_PATH, "kernel_meta")):
            file_path = os.path.join(TOP_PATH, "kernel_meta", item)
            if os.path.exists(file_path) and os.path.isfile(file_path):
                os.remove(file_path)

    def test_compile_op_static(self):
        SetCurrentSocInfo("Ascend310P1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom.cpp"
        )
        origin_func_name = "add_custom"
        code_channel = 0
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

        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        op_compile_option = "{}"

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.task_ration = 2  # AscendC only support 1:2
        tiling_info.file_content = ""
        tiling_info.tiling_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        tiling_info.tiling_data_file_path: str = ""
        tiling_info.tiling_data_size = 96  # TilingDef::getDataSize
        tiling_info.static_shape_flag: bool = True
        tiling_info.tiling_key = 1
        tiling_info.static_workspace_size = 104857600
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {}
        tiling_info.default_tiling_size = 0
        tiling_info.clear_atomic = True
        module_name = "asc_op_compile_base.asc_op_compiler.compile_op"
        compile_op_module = importlib.import_module(module_name)
        with mock.patch.object(
            compile_op_module, "get_tiling_info_by_tiling", return_value=tiling_info
        ):
            with asc_op_compile_base.common.context.op_context.OpContext():
                with buildcfg.build_config():
                    compile_op(
                        cce_file,
                        origin_func_name,
                        op_info,
                        compile_options,
                        code_channel,
                        op_compile_option,
                    )

        binary_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".o")
        json_file = os.path.join(TOP_PATH, "kernel_meta", op_info.kernel_name + ".json")
        self.assertTrue(os.path.exists(binary_file))
        os.remove(binary_file)
        os.remove(json_file)

        with mock.patch.object(
            compile_op_module, "get_tiling_info_by_tiling", return_value=tiling_info
        ):
            with asc_op_compile_base.common.context.op_context.OpContext():
                with buildcfg.build_config():
                    with mock.patch.object(
                        asc_op_compile_base.common.context.get_context(),
                        "get_addition",
                        return_value=["2"],
                    ):
                        self.assertRaises(
                            Exception,
                            compile_op,
                            cce_file,
                            origin_func_name,
                            op_info,
                            compile_options,
                            code_channel,
                            op_compile_option,
                        )
                    with mock.patch.object(
                        asc_op_compile_base.common.context.get_context(),
                        "get_addition",
                        return_value=["3"],
                    ):
                        self.assertRaises(
                            SystemExit,
                            compile_op,
                            cce_file,
                            origin_func_name,
                            op_info,
                            compile_options,
                            code_channel,
                            op_compile_option,
                        )

    def test_dfx_generate_binary_section_for_dynamic(self):
        SetCurrentSocInfo("Ascend910B1")
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                None,
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
                None,
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
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )
        DFXSectionGenerator().dfx_info_reset(op_info)
        compile_info = CompileInfo()
        tiling_info = TilingInfo()
        tiling_info.static_shape_flag = False

        y_in = DFXArgInfo("y_in__", DFXParamType.INPUT)
        z_out = DFXArgInfo("z_out_", DFXParamType.OUTPUT)
        tiling = DFXArgInfo("tiling", DFXParamType.TILING)
        DFXSectionGenerator().insert_param(y_in)
        DFXSectionGenerator().insert_param(z_out)
        DFXSectionGenerator().insert_param(tiling)
        DFXSectionGenerator()._generate_binary_section_for_dynamic(
            compile_info, op_info, tiling_info
        )

        self.assertEqual(
            DFXSectionGenerator().get_param("z_out_").param_type, DFXParamType.OUTPUT
        )
        self.assertEqual(
            DFXSectionGenerator().get_param("z_out_").point_type,
            DFXPointType.LEVEL_2_WITH_SHAPE,
        )

        DFXSectionGenerator()._generate_binary_for_input_and_output(
            DFXSectionGenerator().get_param("z_out_")
        )
        golden_dfx_info = [
            0,
            0,
            0,
            0,
            0,
            3,
            0,
            3,
            255,
            255,
            255,
            255,
            255,
            255,
            255,
            255,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            4,
            255,
            255,
            255,
            255,
            255,
            255,
            255,
            255,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            4,
        ]
        self.assertEqual(
            DFXSectionGenerator().get_param("z_out_").args_dfx_info, golden_dfx_info
        )

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.task_ration = 2  # AscendC only support 1:2
        tiling_info.file_content = ""
        tiling_info.tiling_data = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        tiling_info.tiling_data_file_path: str = ""
        tiling_info.tiling_data_size = 96  # TilingDef::getDataSize
        tiling_info.static_shape_flag: bool = True
        tiling_info.tiling_key = 1
        tiling_info.static_workspace_size = 104857600
        tiling_info.tiling_key_list = ["1", "2"]
        tiling_info.tiling_key_data_size = {"1": 100, "2": 96}
        tiling_info.default_tiling_size = 0
        tiling_info.clear_atomic = True

        compile_info = CompileInfo()
        compile_info.tiling_key_struct_map = {}
        DFXSectionGenerator()._generate_binary_for_tiling(
            "1", tiling_info, compile_info
        )

        # check tiling size of tilingkey 1
        golden_dfx_info = [0, 0, 0, 0, 0, 1, 0, 7, 0, 0, 0, 0, 0, 0, 0, 108]
        self.assertEqual(
            DFXSectionGenerator().get_param("tiling").args_dfx_info, golden_dfx_info
        )

        module_name = "asc_op_compile_base.asc_op_compiler.compile_op"
        compile_op_module = importlib.import_module(module_name)
        with mock.patch.object(
            compile_op_module, "get_current_build_config", return_value=["oom"]
        ):
            DFXSectionGenerator()._generate_binary_for_tiling(
                "1", tiling_info, compile_info
            )

        compile_info = CompileInfo()
        compile_info.sub_core_type = CORE_TYPE_CUBE
        DFXSectionGenerator().generate_dfx_section(
            "1", tiling_info, "test", compile_info
        )
        self.assertEqual(DFXSectionGenerator().param_placeholder_num, 2)

        compile_info.sub_core_type = CORE_TYPE_VEC
        DFXSectionGenerator().generate_dfx_section(
            "1", tiling_info, "test", compile_info
        )
        _generate_section_content(
            "test",
            "1",
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
            tiling_info,
            compile_info,
        )
        self.assertEqual(DFXSectionGenerator().param_placeholder_num, 2)

    def test_dfx_generate_kernel_type_section(self):
        SetCurrentSocInfo("Ascend910B1")
        compile_info = CompileInfo()
        compile_info.code_channel = CORE_TYPE_MIX
        section_context = DFXSectionGenerator().generate_kernel_type_section(
            compile_info, "test"
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

        compile_info.code_channel = CORE_TYPE_CUBE
        section_context = DFXSectionGenerator().generate_kernel_type_section(
            compile_info, "test"
        )
        self.assertTrue("K_TYPE_AIC" in section_context)

        compile_info.hard_sync = True
        compile_info.code_channel = CORE_TYPE_VEC
        section_context = DFXSectionGenerator().generate_kernel_type_section(
            compile_info, "test"
        )
        self.assertTrue("K_TYPE_MIX_AIV_MAIN" in section_context)

        compile_info.code_channel = CORE_TYPE_CUBE
        section_context = DFXSectionGenerator().generate_kernel_type_section(
            compile_info, "test"
        )
        self.assertTrue("K_TYPE_MIX_AIC_MAIN" in section_context)

    def test_get_sub_kernel_name(self):
        compile_info = CompileInfo()
        core_type = 0
        sub_kernel_name = _get_sub_kernel_name(compile_info, core_type)
        self.assertEqual(sub_kernel_name, "_mix_aiv")

    def test_gen_json(self):
        SetCurrentSocInfo("Ascend310P1")
        compile_info = CompileInfo()
        compile_info.kernel_name = "test"
        compile_info.origin_func_name = "test"
        compile_info.tiling_key_list = []

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.static_shape_flag = False

        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                _gen_dynamic_json_for_v200(compile_info, tiling_info, "VectorCore")
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_static_json_for_mix_v200(
                    compile_info,
                    tiling_info,
                    KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                )
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_static_json_for_no_mix_v200(
                    compile_info,
                    tiling_info,
                    KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                )
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_non_mix_sub_json(compile_info, tiling_info, "VectorCore")
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                _gen_mix_sub_json(compile_info, tiling_info)
                self.assertTrue(
                    os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                )
                os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
                    enable_vector_core,
                )

                with asc_op_compile_base.common.context.op_context.OpContext():
                    with buildcfg.build_config() as cfg:
                        cfg.current()[enable_vector_core] = True
                        self.assertRaises(
                            Exception,
                            _gen_dynamic_json_for_v200,
                            compile_info,
                            tiling_info,
                            "VectorCore",
                        )
                        _gen_static_json_for_mix_v200(
                            compile_info,
                            tiling_info,
                            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                        )
                        self.assertTrue(
                            os.path.exists(
                                os.path.join(TOP_PATH, "kernel_meta", "test.json")
                            )
                        )
                        os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
                with mock.patch(
                    "builtins.open", new_callable=mock.mock_open, read_data="{}"
                ) as mock_open:
                    with mock.patch("json.load") as mock_load:
                        with mock.patch("os.chmod"):
                            mock_load.side_effect = RuntimeError()
                            self.assertRaises(
                                Exception,
                                _gen_dynamic_json_for_v200,
                                compile_info,
                                tiling_info,
                                "VectorCore",
                            )
                            self.assertRaises(
                                Exception,
                                _gen_static_json_for_mix_v200,
                                compile_info,
                                tiling_info,
                                KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                            )
                            mock_load.side_effect = None
                        with mock.patch("os.chmod"):
                            self.assertRaises(
                                Exception,
                                _gen_dynamic_json_for_v200,
                                compile_info,
                                tiling_info,
                                "VectorCore",
                            )
                            self.assertRaises(
                                Exception,
                                _gen_static_json_for_mix_v200,
                                compile_info,
                                tiling_info,
                                KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                            )

    def test_raise_python_error(self):
        with mock.patch.object(
            CommonUtility, "ascendc_raise_python_err", return_value=""
        ):
            arg_info = DFXArgInfo("test", DFXParamType.DEFAULT)
            DFXSectionGenerator().get_param("test")
            SetCurrentSocInfo("Ascend910B1")
            compile_info = CompileInfo()
            compile_info.code_channel = 3
            DFXSectionGenerator().generate_kernel_type_section(compile_info, "test")
            DFXSectionGenerator().section_size = 65536
            DFXSectionGenerator().check_section_size()
            self.assertTrue(DFXSectionGenerator().section_size, 65536)

    @mock.patch("shutil.which")
    def test_compile_ascendc_cce_v200_with_kernel_type_for_dynamic(self, mock_shutil):
        SetCurrentSocInfo("Ascend310P1")
        mock_shutil.return_value = "bisheng"
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/test.cpp"
        )
        compile_info = CompileInfo()
        compile_info.dst_file = os.path.join(TOP_PATH, "kernel_meta", "test.o")
        compile_info.kernel_name = "test"
        compile_info.origin_func_name = "test"
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, "test" + file_name_tag
        )
        compile_option_tuple = CompileOptionTuple([], [])
        compile_info.tiling_key_list = ["1"]
        compile_info.tiling_key_kernel_type["1"] = (
            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE
        )
        compile_info.dump_info = {"dump_type": "", "dump_size": 1048576}

        tiling_info = TilingInfo()
        tiling_info.block_dim = 8
        tiling_info.tiling_data_file_path = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_tiling.h"
        )
        tiling_info.static_shape_flag = False

        compile_option_tuple = CompileOptionTuple([], [])
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                None,
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
                None,
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
            mc2_ctx=["mc2_test"],
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )
        CommonUtility.get_ascendc_compiler_path()
        with asc_op_compile_base.common.context.op_context.OpContext():
            with buildcfg.build_config():
                with (
                    mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
                    mock.patch("os.system", return_value=0),
                    mock.patch("os.remove", return_value=0),
                ):
                    with mock.patch.object(
                        DFXSectionGenerator(), "generate_dfx_section", return_value=""
                    ):
                        CommonUtility.get_kernel_meta_dir()
                        DFXSectionGenerator().dfx_info_reset(op_info)
                        gen_kernel_fun(
                            compile_info,
                            compile_info.origin_func_name,
                            op_info,
                            tiling_info,
                            CompileOptionTuple([], []),
                        )
                        os.system(
                            f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aic_1.o')}"
                        )
                        os.system(
                            f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_mix_aiv_1.o')}"
                        )
                        _compile_ascendc_cce_v200_with_kernel_type(
                            compile_info, compile_option_tuple, tiling_info
                        )
                        compile_info.tiling_key_kernel_type["1"] = (
                            KernelMetaType.KERNEL_TYPE_AICORE
                        )
                        os.system(
                            f"touch {os.path.join(TOP_PATH, 'kernel_meta', 'test_1.o')}"
                        )
                        tiling_info.static_shape_flag = True
                        _compile_ascendc_cce_v200_with_kernel_type_for_dynamic(
                            compile_info, compile_option_tuple, tiling_info, "AiCore"
                        )
                        call_bisheng_v200_static(
                            compile_info,
                            compile_option_tuple,
                            tiling_info,
                            "dav-m200",
                            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
                        )
                        call_bisheng_v200_static(
                            compile_info,
                            compile_option_tuple,
                            tiling_info,
                            "dav-m200",
                            KernelMetaType.KERNEL_TYPE_AICORE,
                        )
                        call_bisheng_v220(
                            compile_info,
                            compile_option_tuple,
                            tiling_info,
                            "dav-c220-vec",
                            CORE_TYPE_MIX,
                        )
                        call_bisheng_v220(
                            compile_info,
                            compile_option_tuple,
                            tiling_info,
                            "dav-c220-vec",
                            CORE_TYPE_VEC,
                        )
        self.assertTrue(
            os.path.exists(os.path.join(TOP_PATH, "kernel_meta", "test.json"))
        )
        os.remove(os.path.join(TOP_PATH, "kernel_meta", "test.json"))

    def test_parser_uint64_hex_num(self):
        hex_num = ["20000000", "00000000"]
        hex_num_str = CommonUtility.parser_uint64_hex_num(hex_num)
        self.assertTrue(hex_num_str, "0000000000000020")

    def test_find_tiling_struct_and_expression(self):
        re_str = (
            'auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";'
        )
        struct, experssion = KernelInfoInfer.find_tiling_struct_and_expression(re_str)
        self.assertTrue(struct, "optiling::TilingData")
        self.assertTrue(experssion, "TILING_KEY_VAR == 1")

        re_str = "auto __enable_custom_tiling optiling::TilingData_A = default;"
        struct, experssion = KernelInfoInfer.find_tiling_struct_and_expression(re_str)
        self.assertTrue(struct, "optiling::TilingData_A")
        self.assertEqual(experssion, None)

        module_name = "asc_op_compile_base.asc_op_compiler.kernel_info_infer"
        compile_op_module = importlib.import_module(module_name)
        with mock.patch.object(
            compile_op_module, "raise_tbe_python_err", return_value=None
        ):
            re_str = "auto __enable_custom_tiling optiling::TilingData_A = ;"
            struct, experssion = KernelInfoInfer.find_tiling_struct_and_expression(
                re_str
            )
            self.assertEqual(struct, None)
            self.assertEqual(experssion, None)

    def test_gen_tiling_struct_macro_src_file(self):
        src_file = "/tmp/add_custom.cpp"
        tiling_key_list = ["1"]
        tiling_struct_expr_map = {"optiling::TilingData_B": {"(TILING_KEY_VAR == 3)"}}

        KernelInfoInfer.gen_tiling_struct_macro_src_file(
            tiling_key_list, tiling_struct_expr_map, src_file
        )
        with open(src_file) as file:
            content = file.read()
            compare_str = """#if defined(TILING_KEY_VAR_1) && (TILING_KEY_VAR_1 == 3)
    auto __ascendc_custom_tiling_struct = (1, optiling::TilingData_B);
#endif
"""
            self.assertEqual(content, compare_str)
        os.remove(src_file)

        with mock.patch.object(
            compile_op_module, "raise_tbe_python_err", return_value=None
        ):
            KernelInfoInfer.gen_tiling_struct_macro_src_file(
                tiling_key_list, tiling_struct_expr_map, src_file
            )

    def test_get_tiling_key_corresponding_struct(self):
        tiling_key_list = ["1"]
        default_tiling_struct = "optiling::TilingData_A"
        src_tiling_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.cpp")
        os.system(f"touch {src_tiling_file}")
        with open(src_tiling_file, "w") as file:
            context = """
#
#
#
auto __ascendc_custom_tiling_struct = (3, optiling::TilingData_B);
auto __ascendc_custom_tiling_struct = (4, optiling::TilingData_B);
auto __ascendc_custom_tiling_struct = (1, optiling::TilingData_A);
"""
            file.write(context)

        compile_log_path = "/tmp/log.txt"
        with (
            mock.patch.object(CommonUtility, "run_cmd_inner", return_value=""),
            mock.patch.object(
                compile_op_module, "raise_tbe_python_err", return_value=None
            ),
        ):
            tiling_key_struct_map = KernelInfoInfer.get_tiling_key_corresponding_struct(
                tiling_key_list,
                default_tiling_struct,
                src_tiling_file,
                src_tiling_file,
                compile_log_path,
            )
            self.assertEqual(
                tiling_key_struct_map,
                {
                    "1": "optiling::TilingData_A",
                    "3": "optiling::TilingData_B",
                    "4": "optiling::TilingData_B",
                },
            )

    def test_infer_info_from_ifile(self):
        src_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {src_file}")
        with open(src_file, "w") as file:
            context = """
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR >= 1";
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";
auto __enable_custom_tiling optiling::TilingData_A = default;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR != 1";
auto __enable_feature_for_compile_0 = KERNEL_TYPE_MIX_AIV_1_0;
void add_custom();
"""
            file.write(context)

        get_soc = global_var_storage.get_variable("ascendc_short_soc_version")
        global_var_storage.set_variable("ascendc_short_soc_version", "Ascend910B")
        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            kernel_info = KernelInfoInfer.infer_info_from_ifile(
                None, src_file, src_file, src_file, "add_custom"
            )
            self.assertEqual(
                kernel_info.default_tiling_struct, "optiling::TilingData_A"
            )

        with open(src_file, "w") as file:
            context = """
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR >= 1";
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";
auto __enable_custom_tiling optiling::TilingData_A = default;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR != 1";
void add_custom();
"""
            file.write(context)

        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            with buildcfg.build_config() as cfg:
                cfg.current()["tir.enable_vector_core"] = True
                kernel_info = KernelInfoInfer.infer_info_from_ifile(
                    None, src_file, src_file, src_file, "add_custom"
                )
                self.assertEqual(
                    kernel_info.default_tiling_struct, "optiling::TilingData_A"
                )
        global_var_storage.set_variable("ascendc_short_soc_version", get_soc)

    def test_infer_info_from_ifile_template_tiling(self):
        src_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {src_file}")
        with open(src_file, "w") as file:
            context = """
void add_custom();
@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {@@ASCENDC_TPL_DTYPE_DECL_D_T_X@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Y@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Z@@ = {10, 20},@@ASCENDC_TPL_UINT_DECL_TILE_NUM@@ = {8, 2, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8},@@ASCENDC_TPL_BOOL_DECL_IS_SPLIT@@ = {0, 1},};
@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {7}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {10}, @@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {false}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {10}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {10}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},}, @@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {7}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {20}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},},};
"""
            file.write(context)

        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            with asc_op_compile_base.common.context.op_context.OpContext():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value="",
                ):
                    kernel_info = KernelInfoInfer.infer_info_from_ifile(
                        None, src_file, src_file, src_file, "add_custom"
                    )
                    self.assertEqual(
                        kernel_info.tiling_key_kernel_type["17435146"],
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
                    )
                    self.assertEqual(kernel_info.no_set_kernel_type, False)

        with open(src_file, "w") as file:
            context = """
void add_custom();
@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {@@ASCENDC_TPL_DTYPE_DECL_D_T_X@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Y@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Z@@ = {10, 20},@@ASCENDC_TPL_UINT_DECL_TILE_NUM@@ = {8, 2, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8},@@ASCENDC_TPL_BOOL_DECL_IS_SPLIT@@ = {0, 1},};
@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {17}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {10}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {10}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {10}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},}, @@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {17}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {20}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},},};
"""
            file.write(context)

        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            with asc_op_compile_base.common.context.op_context.OpContext():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value="",
                ):
                    kernel_info = KernelInfoInfer.infer_info_from_ifile(
                        None, src_file, src_file, src_file, "add_custom"
                    )
                    self.assertEqual(kernel_info.no_set_kernel_type, True)

        with open(src_file, "w") as file:
            context = """
void add_custom();
@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {@@ASCENDC_TPL_DTYPE_DECL_D_T_X@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Y@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Z@@ = {10, 20},@@ASCENDC_TPL_UINT_DECL_TILE_NUM@@ = {8, 2, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8},@@ASCENDC_TPL_BOOL_DECL_IS_SPLIT@@ = {0, 1},};
@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {10}, ASCENDC_TPL_DETERMINISTIC_SEL@@ = {false}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {10}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {10}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},}, @@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {7}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {20}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},},};
"""
            file.write(context)

        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            with asc_op_compile_base.common.context.op_context.OpContext():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value="",
                ):
                    try:
                        KernelInfoInfer.infer_info_from_ifile(
                            None, src_file, src_file, src_file, "add_custom"
                        )
                    except Exception as e:
                        assert str(e) == "must provide default kernel type", (
                            f"msg is :{e}"
                        )

    def test_infer_info_from_ifile_key_none_type_exists(self):
        get_soc = global_var_storage.get_variable("ascendc_short_soc_version")
        global_var_storage.set_variable("ascendc_short_soc_version", "Ascend910B")
        src_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {src_file}")
        with open(src_file, "w") as file:
            context = """
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR >= 1";
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";
auto __enable_custom_tiling optiling::TilingData_A = default;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR != 1";
auto __enable_feature_for_compile_default = KERNEL_TYPE_MIX_AIV_1_0;
void add_custom();
"""
            file.write(context)

        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            kernel_info = KernelInfoInfer.infer_info_from_ifile(
                None, src_file, src_file, src_file, "add_custom"
            )
            self.assertEqual(
                kernel_info.default_tiling_struct, "optiling::TilingData_A"
            )
        global_var_storage.set_variable("ascendc_short_soc_version", get_soc)

    def test_infer_info_from_ifile_no_tiling_register(self):
        src_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {src_file}")
        with open(src_file, "w") as file:
            context = """
auto __enable_no_register_custom_tiling ascendc_trigger_tiling_struct = default
void add_custom();
"""
            file.write(context)

        with mock.patch.object(
            KernelInfoInfer, "get_tiling_key_corresponding_struct", return_value={}
        ):
            kernel_info = KernelInfoInfer.infer_info_from_ifile(
                None, src_file, src_file, src_file, "add_custom"
            )
            ascendc_tiling_no_register = global_var_storage.get_variable(
                "ascendc_tiling_no_register"
            )
            self.assertEqual(ascendc_tiling_no_register, True)

    def test_get_hard_sync_instr_from_i_file(self):
        dst_i_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {dst_i_file}")
        with open(dst_i_file, "w") as file:
            context = """
auto __enable_feature_for_compile_assert = 1;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";
auto __enable_custom_tiling optiling::TilingData_A = default;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR != 1";
void add_custom();
SyncAll<false>();
"""
            file.write(context)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_time_stamp_compile_options", True
        )
        res = KernelInfoInfer.get_hard_sync_instr_from_i_file(context)
        self.assertEqual(res, True)
        os.remove(os.path.join(TOP_PATH, "kernel_meta", "add_custom.i"))

    def test_get_hard_sync_instr_from_i_file_2(self):
        dst_i_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {dst_i_file}")
        with open(dst_i_file, "w") as file:
            context = """
auto __enable_feature_for_compile_assert = 1;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";
auto __enable_custom_tiling optiling::TilingData_A = default;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR != 1";
void add_custom();
SyncAll();
SyncAll();
SyncAll();
"""
            file.write(context)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_time_stamp_compile_options", True
        )
        res = KernelInfoInfer.get_hard_sync_instr_from_i_file(context)
        self.assertEqual(res, True)
        os.remove(os.path.join(TOP_PATH, "kernel_meta", "add_custom.i"))

    def test_get_enable_deterministic_var_from_i_file(self):
        dst_i_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.system(f"touch {dst_i_file}")
        with open(dst_i_file, "w") as file:
            context = """
auto __enable_feature_for_compile_deterministic = 1;
"""
            file.write(context)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_time_stamp_compile_options", True
        )
        res = KernelInfoInfer.get_enable_deterministic_var_from_i_file(context)
        self.assertEqual(res, True)
        os.remove(os.path.join(TOP_PATH, "kernel_meta", "add_custom.i"))

    def test_get_sync_task_start_end_instr_from_i_file(self):
        dst_i_file = os.path.join(TOP_PATH, "kernel_meta", "add_custom.i")
        os.makedirs(os.path.dirname(dst_i_file), exist_ok=True)
        os.system(f"touch {dst_i_file}")
        with open(dst_i_file, "w") as file:
            context = """
auto __enable_feature_for_compile_printf = 1";
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR == 1";
auto __enable_custom_tiling optiling::TilingData_A = default;
auto __enable_custom_tiling optiling::TilingData = "TILING_KEY_VAR != 1";
void add_custom();
SetNextTaskStart();
WaitPreTaskEnd();
SetNextTaskStart();
WaitPreTaskEnd();
"""
            file.write(context)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_time_stamp_compile_options", True
        )
        res1, res2 = KernelInfoInfer.get_sync_task_start_end_instr_from_i_file(context)
        self.assertEqual(res1, False)
        self.assertEqual(res2, False)
        os.remove(os.path.join(TOP_PATH, "kernel_meta", "add_custom.i"))

    def test_get_sync_task_start_end_instr_from_i_file_with_template_args(self):
        context = """
__aicore__ inline void SetNextTaskStart();
__aicore__ inline void WaitPreTaskEnd();
AscendC::SetNextTaskStart<PIPE_MTE3, PIPE_FIX, true>();
AscendC::WaitPreTaskEnd<true>();
AscendC::SetNextTaskStart<PIPE_MTE3, PIPE_FIX, true>();
AscendC::WaitPreTaskEnd<true>();
"""
        res1, res2 = KernelInfoInfer.get_sync_task_start_end_instr_from_i_file(context)
        self.assertEqual(res1, True)
        self.assertEqual(res2, True)

    def test_infer_info_from_ifile_uses_shared_scan_for_aclgraph_early_start(self):
        dst_i_file = os.path.join(TOP_PATH, "kernel_meta", "aclgraph_early_start.i")
        os.makedirs(os.path.dirname(dst_i_file), exist_ok=True)
        os.system(f"touch {dst_i_file}")
        context = """
template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
__aicore__ inline void SetNextTaskStart();
template<pipe_t AIV_PIPE, pipe_t AIC_PIPE, bool FORCE>
__aicore__ inline void SetNextTaskStart()
{
    SetNextTaskStartImpl<AIV_PIPE, AIC_PIPE, true>();
}
template<bool FORCE = false>
__aicore__ inline void WaitPreTaskEnd();
template<bool FORCE>
__aicore__ inline void WaitPreTaskEnd()
{
    WaitPreTaskEndImpl();
}
void add_custom();
void add_custom()
{
    AscendC::SetNextTaskStart<PIPE_MTE3, PIPE_FIX, true>();
    AscendC::WaitPreTaskEnd<true>();
    AscendC::SetNextTaskStart<PIPE_MTE3, PIPE_FIX, true>();
    AscendC::WaitPreTaskEnd<true>();
}
"""
        with open(dst_i_file, "w") as file:
            file.write(context)
        op_info = OpInfo(
            kernel_name="AclgraphForceOp",
            op_type="AclgraphForceOp",
            inputs=[],
            outputs=[],
        )
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        global_var_storage.set_variable("ascendc_compile_debug_config", True)
        try:
            with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
                ctx.add_addition("super_kernel_sub_combine", True)
                with (
                    mock.patch.object(CommonUtility, "is_v220", return_value=False),
                    mock.patch.object(CommonUtility, "is_c310", return_value=False),
                    mock.patch.object(CommonUtility, "is_m510", return_value=True),
                    mock.patch.object(
                        KernelInfoInfer, "_gen_tiling_key_struct_map", return_value={}
                    ),
                ):
                    result = KernelInfoInfer.infer_info_from_ifile(
                        op_info, dst_i_file, None, "add_custom.cpp", "add_custom"
                    )
            self.assertTrue(result.super_kernel_early_start_set_flag)
            self.assertTrue(result.super_kernel_early_start_wait_flag)
        finally:
            global_var_storage.set_variable("ascendc_enable_super_kernel", False)
            global_var_storage.set_variable("ascendc_compile_debug_config", False)
            if os.path.exists(dst_i_file):
                os.remove(dst_i_file)

    def test_get_tiling_struct_size(self):
        compile_info = CompileInfo()
        compile_info.dst_file = "/tmp/add_custom.o"
        compile_info.tiling_key_struct_map = {
            "3": "optiling::TilingData_B",
            "1": "optiling::TilingData",
            "6": "optiling::TilingData_C",
        }
        binary = b"\n/home/y00611406/tikcpp_smoke/external_tik2_demo/acl_invocation/aclnn_online_model/custom_op/build_out/op_kernel/binary/ascend910b/kernel_meta_AddCustom_6b4edd934cf3a660c0cd2e16b3659524/kernel_meta/AddCustom_6b4edd934cf3a660c0cd2e16b3659524.o:\tfile format elf64-hiipu\nContents of section .ascendc_tiling.TilingData_C:\n 1790 28000000 00000000                    (.......\n"
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[binary, None]
            ):
                from asc_op_compile_base.asc_op_compiler.compile_op import (
                    _get_tiling_struct_size,
                )

                max_tiling_size = _get_tiling_struct_size(compile_info)
                self.assertEqual(max_tiling_size, 40)

    def test_get_tiling_struct_size_c310(self):
        compile_info = CompileInfo()
        compile_info.dst_file = "/tmp/add_custom.o"
        compile_info.tiling_key_struct_map = {
            "3": "optiling::TilingData_B",
            "1": "optiling::TilingData",
            "6": "optiling::TilingData_C",
        }
        binary = b"\n/home/y00611406/tikcpp_smoke/external_tik2_demo/acl_invocation/aclnn_online_model/custom_op/build_out/op_kernel/binary/ascend910b/kernel_meta_AddCustom_6b4edd934cf3a660c0cd2e16b3659524/kernel_meta/AddCustom_6b4edd934cf3a660c0cd2e16b3659524.o:\tfile format elf64-hiipu\nContents of section .ascendc_tiling.TilingData_C:\n 1790 28000000 00000000                    (.......\n"
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[binary, None]
            ):
                with mock.patch.object(CommonUtility, "is_c310", return_value=True):
                    from asc_op_compile_base.asc_op_compiler.compile_op import (
                        _get_tiling_struct_size,
                    )

                    max_tiling_size = _get_tiling_struct_size(compile_info)
                    self.assertEqual(max_tiling_size, 40)

    def test_get_tiling_struct_size_in_sk(self):
        compile_info = CompileInfo()
        compile_info.dst_file = "/tmp/add_custom.o"
        compile_info.tiling_key_struct_map = {
            "3": "optiling::TilingData_B",
            "1": "optiling::TilingData",
            "6": "optiling::TilingData_C",
        }
        compile_info.is_super_kernel_compile = False
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", True
        )
        binary = b"\n/home/y00611406/tikcpp_smoke/external_tik2_demo/acl_invocation/aclnn_online_model/custom_op/build_out/op_kernel/binary/ascend910b/kernel_meta_AddCustom_6b4edd934cf3a660c0cd2e16b3659524/kernel_meta/AddCustom_6b4edd934cf3a660c0cd2e16b3659524.o:\tfile format elf64-hiipu\nContents of section .ascendc_tiling.TilingData_C:\n 1790 28000000 00000000                    (.......\n"
        with mock.patch("subprocess.Popen"):
            with mock.patch.object(
                subprocess.Popen(), "communicate", return_value=[binary, None]
            ):
                with mock.patch.object(CommonUtility, "is_c310", return_value=True):
                    from asc_op_compile_base.asc_op_compiler.compile_op import (
                        _get_tiling_struct_size,
                    )

                    max_tiling_size = _get_tiling_struct_size(compile_info)
                    self.assertEqual(max_tiling_size, 40)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", False
        )

    def test_tran_dfx_info_to_string(self):
        dfx_string = DFXSectionGenerator()._tran_dfx_info_to_string("tiling")
        compare_string = [
            "(((sizeof(tiling) + 8) >> 56) & 0xff)",
            "(((sizeof(tiling) + 8) >> 48) & 0xff)",
            "(((sizeof(tiling) + 8) >> 40) & 0xff)",
            "(((sizeof(tiling) + 8) >> 32) & 0xff)",
            "(((sizeof(tiling) + 8) >> 24) & 0xff)",
            "(((sizeof(tiling) + 8) >> 16) & 0xff)",
            "(((sizeof(tiling) + 8) >> 8) & 0xff)",
            "(((sizeof(tiling) + 8) >> 0) & 0xff)",
        ]
        print("dfx_string:", dfx_string)
        print("compare_string:", compare_string)
        self.assertEqual(dfx_string, compare_string)
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                None,
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
                None,
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
            mc2_ctx=["mc2_test"],
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )
        DFXSectionGenerator().dfx_info_reset(op_info)
        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
            dfx_string = DFXSectionGenerator()._tran_dfx_info_to_string("tiling")
            compare_string = [
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 56) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 48) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 40) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 32) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 24) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 16) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 8) & 0xff)",
                "(((((sizeof(tiling) + 7) / 8) * 8 + 8 + 8 * 0 ) >> 0) & 0xff)",
            ]
            self.assertEqual(dfx_string, compare_string)

    def test_tran_dfx_info_to_value_string(self):
        size_value = 16
        value_string = DFXSectionGenerator()._tran_dfx_info_to_value_string(size_value)
        expected_value = 16 + 8  # 24
        expected_string = [
            str((expected_value >> 56) & 0xFF),
            str((expected_value >> 48) & 0xFF),
            str((expected_value >> 40) & 0xFF),
            str((expected_value >> 32) & 0xFF),
            str((expected_value >> 24) & 0xFF),
            str((expected_value >> 16) & 0xFF),
            str((expected_value >> 8) & 0xFF),
            str((expected_value >> 0) & 0xFF),
        ]
        print("value_string:", value_string)
        print("expected_string:", expected_string)
        self.assertEqual(value_string, expected_string)

        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
            op_type="AddCustomUnalign",
            inputs=[
                None,
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
                None,
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
            mc2_ctx=["mc2_test"],
            param_type_list=["required", "required", "dynamic"],
            init_value_list=[None],
            output_shape_depend_on_compute=[0],
        )
        DFXSectionGenerator().dfx_info_reset(op_info)

        with buildcfg.build_config() as cfg:
            cfg.current()["tir.op_debug_config"] = ["oom"]
            size_value = 16
            value_string = DFXSectionGenerator()._tran_dfx_info_to_value_string(
                size_value
            )

            aligned_size = ((size_value + 7) // 8) * 8  # 16 -> 16
            total_size = aligned_size + 8 + 8 * 0  # 16 + 8 + 0 = 24
            expected_string = [
                str((total_size >> 56) & 0xFF),
                str((total_size >> 48) & 0xFF),
                str((total_size >> 40) & 0xFF),
                str((total_size >> 32) & 0xFF),
                str((total_size >> 24) & 0xFF),
                str((total_size >> 16) & 0xFF),
                str((total_size >> 8) & 0xFF),
                str((total_size >> 0) & 0xFF),
            ]
            print("value_string (oom mode):", value_string)
            print("expected_string (oom mode):", expected_string)
            self.assertEqual(value_string, expected_string)

    def test_get_tiling_info_by_tiling(self):
        tiling_key = "1"
        tiling_info = TilingInfo()
        compile_info = CompileInfo()
        compile_info.tiling_key_struct_map = {"1": "tiling"}
        DFXSectionGenerator()._generate_binary_for_tiling(
            tiling_key, tiling_info, compile_info
        )

        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97"
        )
        InferedInfoFromIfile = namedtuple(
            "InferedInfoFromIfile",
            ["default_tiling_struct", "tiling_key_list", "tiling_key_struct_map"],
        )
        infered_info_from_ifile = InferedInfoFromIfile(
            default_tiling_struct="tiling",
            tiling_key_list=["1"],
            tiling_key_struct_map={"1": "tiling"},
        )
        module_name = "asc_op_compile_base.asc_op_compiler.compile_op"
        compile_op_module = importlib.import_module(module_name)
        self.assertRaises(
            Exception,
            compile_op_module.get_tiling_info_by_tiling,
            op_info,
            infered_info_from_ifile,
            "add_custom",
        )

    def test_check_func_name_exist(self):
        line = 'extern "C" __attribute__((cce_kernel)) [aicore] void add_custom('
        res = KernelInfoInfer.check_func_name_exist("add_custom", line)
        self.assertEqual(res, True)
        line = 'extern "C" __attribute__((cce_kernel)) [aicore] void void add_custom('
        res = KernelInfoInfer.check_func_name_exist("Add_custom", line)
        self.assertEqual(res, False)

    def test_skip_mc2_context_size(self):
        op_info = OpInfo(
            kernel_name="TestOpInfo",
            op_type="TestOpInfo",
            inputs=[],
            outputs=[],
            attrs=[],
            impl_mode="high_performance",
            origin_inputs=[],
            origin_outputs=[],
            param_type_dynamic=False,
            mc2_ctx=["context"],
            param_type_list=["required", "required", "required"],
            init_value_list=[None],
        )
        context = skip_mc2_context_size(op_info)
        self.assertEqual(context, "    tmpTilingSizeForOOM += 8;\n")

    def test_dfx_for_func_name(self):
        cce_file = "tmp.cpp"
        origin_func_name = "add_custom"
        func_name_exist = False
        self.assertRaises(
            Exception,
            KernelInfoInfer.dfx_for_func_name,
            cce_file,
            origin_func_name,
            func_name_exist,
        )

    def test_get_kernel_type_enum(self):
        kernel_type = "TEST_KERNEL_TYPE"
        compile_log_path = "test.log"
        with mock.patch.object(CommonUtility, "is_v220", return_value=True):
            with mock.patch.object(CommonUtility, "is_v200", return_value=False):
                res = KernelInfoInfer.get_kernel_type_enum(
                    kernel_type, compile_log_path
                )
                self.assertEqual(res, None)
        with mock.patch.object(CommonUtility, "is_v220", return_value=False):
            with mock.patch.object(CommonUtility, "is_v200", return_value=True):
                res = KernelInfoInfer.get_kernel_type_enum(
                    kernel_type, compile_log_path
                )
                self.assertEqual(res, None)

    def test_find_tilingkey(self):
        tiling_str = "g_tilingKey == (aaa)"
        re = KernelInfoInfer.find_tilingkey(tiling_str)
        self.assertEqual(re, (None, False))

    def test_set_dynamic_sub_func_names_of_super_kernel(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_v220 import (
            set_dynamic_sub_func_names_of_super_kernel,
        )

        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", False
        )
        compile_info = CompileInfo()
        compile_info.code_channel = CORE_TYPE_MIX
        tiling_key = "0"
        arch = "AiCore"
        kernel_func_name = "test"
        set_dynamic_sub_func_names_of_super_kernel(
            tiling_key, compile_info, arch, kernel_func_name
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", True
        )
        set_dynamic_sub_func_names_of_super_kernel(
            tiling_key, compile_info, arch, kernel_func_name
        )
        compile_info.code_channel = CORE_TYPE_CUBE
        compile_info.hard_sync = 0
        set_dynamic_sub_func_names_of_super_kernel(
            tiling_key, compile_info, arch, kernel_func_name
        )
        res = compile_op_module.global_var_storage.get_variable(
            "ascendc_sub_super_kernel_fun_names"
        )
        self.assertEqual(res["dynamic_func_names"]["0"]["AiCore"], "test")

    def test_super_kernel_compile(self):
        kernel_info = {
            "compile_option": ["-g"],
            "kernel_file": "test.cpp",
            "kernel_name": "test",
            "block_num": 1,
            "kernel_type": KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
            "sub_operator": "test1",
            "link_mode": SuperKernelLinkMode.PerVecHerCube,
            "timestamp_option": "True",
            "op_list": "test",
            "split_mode": 4,
            "debug_size": 75,
        }
        with (
            asc_op_compile_base.common.context.op_context.OpContext(),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.ascendc_compile_base",
                "gen_super_kernel_compile_info",
            ),
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.ascendc_compile_base",
                "gen_super_kernel_link_obj_sequence",
            ),
            mock.patch.object(super_kernel_compile_module, "fatbin_objs"),
            mock.patch.object(super_kernel_compile_module, "link_relocatable"),
            mock.patch.object(
                super_kernel_compile_module, "localization_sub_op_func_sym"
            ),
            mock.patch.object(
                super_kernel_compile_module,
                "_compile_ascendc_cce_v220_with_kernel_type_for_static",
            ),
            mock.patch.object(super_kernel_compile_module, "_json_post_process"),
        ):
            with open(os.devnull, "a") as file:
                with mock.patch("os.open", return_value=file):
                    with mock.patch("os.fdopen") as mock_fdopen:
                        compile_log_path = "./tmp"
                        super_kernel_compile_module.super_kernel_compile(
                            kernel_info, compile_log_path
                        )

    def test_infer_info_from_ifile_spk(self):
        global_var_storage.global_storage_reset()
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        src_file = "a.i"
        data = """
# 27 "/usr/local/Ascend/CANN-7.3/opp/built-in/op_impl/ai_core/tbe/impl/dynamic/../ascendc/common/../conv_forward/conv2d/../conv_common/conv_util.h"
const static uint64_t L0A_SIZE = 65536 * block_idx;
"""
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data=data
        ) as mock_open:
            dst_i_file = "./a.i"
            self.assertRaises(
                Exception,
                KernelInfoInfer.infer_info_from_ifile,
                None,
                src_file,
                src_file,
                src_file,
                "add_custom",
            )
        global_var_storage.global_storage_reset()

    def test_gen_kernel_func_enable_sk_mc2(self):
        global_var_storage.global_storage_reset()
        SetCurrentSocInfo("Ascend910B1")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
            "--cce-auto-sync",
        ]
        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options, []
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = ["1", "2", "3"]

        tiling_key_kernel_type = infered_info_from_ifile.tiling_key_kernel_type
        tiling_key_kernel_type["1"] = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0
        tiling_key_kernel_type["2"] = KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
        tiling_key_kernel_type["3"] = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2

        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = True
        tiling_info.tiling_key_list = tiling_key_list

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = tiling_key_kernel_type
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        compile_info.super_kernel_info["sp_options"] = {"func-align": 512}

        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_aicore_exception_restart", True
        )

        if os.path.exists(compile_info.gen_kernel_func_file):
            os.remove(compile_info.gen_kernel_func_file)
        assert os.path.exists(compile_info.gen_kernel_func_file) == False
        self.assertFalse(os.path.exists(compile_info.gen_kernel_func_file))
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", True
        )

        DFXSectionGenerator().dfx_info_reset(op_info)
        gen_kernel_fun(
            compile_info,
            origin_func_name,
            op_info,
            tiling_info,
            CompileOptionTuple(compile_options, []),
        )
        assert os.path.exists(compile_info.gen_kernel_func_file) == True, (
            "Problems Occurred during Kernel Function Generation!!!"
        )
        self.assertTrue(os.path.exists(compile_info.gen_kernel_func_file))
        with open(compile_info.gen_kernel_func_file, "r") as file:
            self.assertNotIn(
                "__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__", file.read()
            )
        os.remove(compile_info.gen_kernel_func_file)
        compile_info.super_kernel_info["sp_options"]["debug-per-op-max-core-num"] = "1"
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            ctx.add_addition("super_kernel_sub_combine", True)
            gen_kernel_fun(
                compile_info,
                origin_func_name,
                op_info,
                tiling_info,
                CompileOptionTuple(compile_options, []),
            )
        self.assertTrue(os.path.exists(compile_info.gen_kernel_func_file))
        with open(compile_info.gen_kernel_func_file, "r") as file:
            lines = file.readlines()  # 读取所有行，返回列表
            lines = " ".join(lines)
            self.assertNotEqual(lines.find(" __attribute__((aligned(512))) "), -1)
            self.assertNotEqual(lines.find(" __sk__"), -1)
            self.assertNotIn("__ENABLE_SUPER_KERNEL_INNER_CORE_SYNC_CHECK__", lines)
            self.assertIn(
                "AscendC::g_superKernelSetWaitFlagCountDifference = "
                "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE;",
                lines,
            )
            self.assertIn(
                "if (AscendC::g_superKernelSetWaitFlagCountDifference > "
                "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE)",
                lines,
            )
            self.assertIn(
                "else if (AscendC::g_superKernelSetWaitFlagCountDifference < "
                "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE)",
                lines,
            )
            self.assertIn(
                'assert(false, "SuperKernel: current operator has %d more SetFlag '
                'calls than WaitFlag calls; Please check synchronization within the current operator.\\n", '
                "AscendC::g_superKernelSetWaitFlagCountDifference - "
                "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE);",
                lines,
            )
            self.assertIn(
                'assert(false, "SuperKernel: current operator has %d more WaitFlag '
                'calls than SetFlag calls; Please check synchronization within the current operator.\\n", '
                "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE - "
                "AscendC::g_superKernelSetWaitFlagCountDifference);",
                lines,
            )
        os.remove(compile_info.gen_kernel_func_file)
        global_var_storage.global_storage_reset()

    def test_compile_pre_process(self):
        compile_op_module.global_var_storage.set_variable("ascendc_enable_ccache", True)
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        compile_options = ["-x", "cce"]
        compile_pre_process(op_info, compile_options)
        self.assertTrue("--cce-aicore-lang" in compile_options)
        self.assertTrue("cce" not in compile_options)

    def test_is_v300(self):
        SetCurrentSocInfo("Ascend310B1")
        self.assertTrue(CommonUtility.is_v300())

        SetCurrentSocInfo("Ascend910B1")
        self.assertFalse(CommonUtility.is_v300())

    def test_is_has_ffts_mode(self):
        SetCurrentSocInfo("Ascend910B1")
        self.assertTrue(CommonUtility.is_has_ffts_mode())

        SetCurrentSocInfo("Ascend950PR_9599")
        self.assertFalse(CommonUtility.is_has_ffts_mode())

    def test_gen_kernel_function_c310_dcci_false(self):
        global_var_storage.global_storage_reset()
        SetCurrentSocInfo("Ascend950PR_9599")
        cce_file = os.path.join(
            TOP_PATH, "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign.cpp"
        )
        kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        origin_func_name = "add_custom_unalign"
        code_channel = 0
        compile_options = [
            "-DDTYPE_X=float",
            "-DORIG_DTYPE_X=DT_FLOAT",
            "-DFORMAT_X=FORMAT_ND",
            "-DDTYPE_Y=float",
            "-DORIG_DTYPE_Y=DT_FLOAT",
            "-DFORMAT_Y=FORMAT_ND",
            "-DDTYPE_Z=float",
            "-DORIG_DTYPE_Z=DT_FLOAT",
            "-DFORMAT_Z=FORMAT_ND",
            "-x",
            "cce",
            "-I" + API_ROOT_PATH,
            "-I" + os.path.join(API_ROOT_PATH, "include"),
            "-I" + os.path.join(API_ROOT_PATH, "impl"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/basic_api/reg_compute"),
            "-I" + os.path.join(API_ROOT_PATH, "include/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/simt_api"),
            "-I" + os.path.join(API_ROOT_PATH, "include/adv_api"),
            "-I" + os.path.join(API_ROOT_PATH, "impl/adv_api"),
            "-I" + os.path.join(TOP_PATH, "build"),
            "-include"
            + os.path.join(
                TOP_PATH,
                "tests/python/asc_op_compiler/stub_kernels/add_custom_unalign_tiling.h",
            ),
            "-DHIGH_PERFORMANCE=1",
            "-DDETERMINISTIC_MODE=1",
        ]
        CommonUtility.get_ascendc_compiler_path()
        compile_option_tuple = CompileOptionTuple(
            [] if compile_options is None else compile_options,
            ["-mllvm", "-cce-aicore-dcci-before-kernel-end=false"],
        )
        compile_log_path = None
        infered_info_from_ifile = (
            KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
                op_info,
                cce_file,
                os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
                compile_option_tuple,
                compile_log_path,
                origin_func_name,
            )
        )
        tiling_key_list = ["1", "2", "3"]

        tiling_key_kernel_type = infered_info_from_ifile.tiling_key_kernel_type
        tiling_key_kernel_type["1"] = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0
        tiling_key_kernel_type["2"] = KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
        tiling_key_kernel_type["3"] = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2

        tiling_info = TilingInfo()
        tiling_info.tiling_data_size = 64
        tiling_info.tiling_key_data_size = []
        tiling_info.static_shape_flag = True
        tiling_info.tiling_key_list = tiling_key_list

        compile_info = CompileInfo()
        compile_info.src_file = cce_file
        compile_info.dst_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + ".o"
        )
        compile_info.kernel_name = op_info.kernel_name
        compile_info.origin_func_name = origin_func_name
        compile_info.op_type = op_info.op_type
        compile_info.arg_size = tiling_info.tiling_data_size
        compile_info.code_channel = code_channel
        compile_info.tiling_key_list = tiling_key_list
        compile_info.compile_log_path = compile_log_path
        compile_info.hard_sync = infered_info_from_ifile.hard_sync
        compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
        compile_info.tiling_key_kernel_type = tiling_key_kernel_type
        compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
        compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
        compile_info.dump_info = infered_info_from_ifile.dump_info
        distinct_tag = CommonUtility.get_distinct_filename_tag()
        file_name_tag = distinct_tag + "_kernel.cpp"
        compile_info.gen_kernel_func_file = os.path.join(
            kernel_meta_dir, op_info.kernel_name + file_name_tag
        )
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_aicore_exception_restart", True
        )

        if os.path.exists(compile_info.gen_kernel_func_file):
            os.remove(compile_info.gen_kernel_func_file)
        assert os.path.exists(compile_info.gen_kernel_func_file) == False
        self.assertFalse(os.path.exists(compile_info.gen_kernel_func_file))
        global_var_storage.set_variable("ascendc_recognize_simtvf", True)
        gen_kernel_fun(
            compile_info, origin_func_name, op_info, tiling_info, compile_option_tuple
        )
        compile_option_tuple.mllvm_options.append("-mllvm")
        compile_option_tuple.mllvm_options.append(
            "-cce-aicore-dcci-before-kernel-end=true"
        )
        gen_kernel_fun(
            compile_info, origin_func_name, op_info, tiling_info, compile_option_tuple
        )
        compile_option_tuple.mllvm_options.append("-mllvm")
        compile_option_tuple.mllvm_options.append("-cce-aicore-dcci-before-kernel-end=")
        gen_kernel_fun(
            compile_info, origin_func_name, op_info, tiling_info, compile_option_tuple
        )
        global_var_storage.global_storage_reset()

    def test_delete_tiling_section(self):
        from asc_op_compile_base.asc_op_compiler.compile_op import delete_tiling_section

        compile_info = CompileInfo()
        compile_info.is_super_kernel_compile = False
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", True
        )
        delete_tiling_section(compile_info)
        compile_op_module.global_var_storage.set_variable(
            "ascendc_enable_super_kernel", False
        )
        delete_tiling_section(compile_info)

    def test_update_tiling_size_for_oom(self):
        from asc_op_compile_base.asc_op_compiler.ascendc_compile_gen_code import (
            update_tiling_size_for_oom,
        )

        compile_op_module.global_var_storage.set_variable(
            "ascendc_tiling_no_register", True
        )
        compile_info = CompileInfo()
        tiling_info = TilingInfo()
        tiling_info.tiling_key_list = ["1", "2"]
        dyn_input_shape_offset = 0
        update_tiling_size_for_oom(compile_info, tiling_info, dyn_input_shape_offset)

    def test_generate_binary_for_tiling_without_register(self):
        tiling_info = TilingInfo()
        tiling_info.static_shape_flag = False
        tiling_key = "1"
        tiling_key_struct_size_map = {"1": ("struct1", 0), "2": ("struct2", 0)}
        kernel_name = "struct1"
        op_info = OpInfo(
            kernel_name="AddCustomUnalign_0904bc1781946e62d385bfc6e6f99d97",
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
        DFXSectionGenerator().dfx_info_reset(op_info)
        tiling = DFXArgInfo("tiling", DFXParamType.TILING)
        DFXSectionGenerator().insert_param(tiling)
        DFXSectionGenerator().generate_dfx_section_without_tiling_register(
            tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
        )

    def test_match_regex_basic(self):
        self.assertFalse(_match_regex("*abc", "abc"))
        self.assertFalse(_match_regex("*", "test"))

    def test_match_regex_exact_match(self):
        self.assertTrue(_match_regex("abc", "abc"))
        self.assertFalse(_match_regex("abc", "abd"))
        self.assertFalse(_match_regex("abc", "abcd"))

    def test_match_regex_dot(self):
        self.assertTrue(_match_regex("a.c", "abc"))
        self.assertTrue(_match_regex("a.c", "aac"))
        self.assertTrue(_match_regex(".", "x"))
        self.assertFalse(_match_regex("a.c", "ac"))

    def test_match_regex_star(self):
        self.assertTrue(_match_regex("ab*c", "ac"))
        self.assertTrue(_match_regex("ab*c", "abc"))
        self.assertTrue(_match_regex("ab*c", "abbbc"))
        self.assertTrue(_match_regex("a*", ""))
        self.assertTrue(_match_regex("a*", "aaa"))
        self.assertFalse(_match_regex("ab*c", "adc"))

    def test_match_regex_wildcard(self):
        self.assertTrue(_match_regex(".*", "anything"))
        self.assertTrue(_match_regex(".*", ""))
        self.assertTrue(_match_regex("a.*b", "acb"))
        self.assertTrue(_match_regex("a.*b", "a123b"))
        self.assertFalse(_match_regex("a.*b", "abx"))

    def test_match_regex_prefix_suffix(self):
        self.assertTrue(_match_regex("abc.*", "abc"))
        self.assertTrue(_match_regex("abc.*", "abcdef"))
        self.assertTrue(_match_regex(".*abc", "abc"))
        self.assertTrue(_match_regex(".*abc", "xyzabc"))
        self.assertFalse(_match_regex("abc.*", "ab"))

    def test_match_regex_complex(self):
        self.assertTrue(_match_regex("a.*b.*c", "abc"))
        self.assertTrue(_match_regex("a.*b.*c", "aXbYc"))
        self.assertTrue(_match_regex("a*b*c*", "abc"))
        self.assertTrue(_match_regex("a*b*c*", "aaabbbccc"))

    def test_match_regex_empty(self):
        self.assertTrue(_match_regex("", ""))
        self.assertFalse(_match_regex("", "a"))
        self.assertFalse(_match_regex("a", ""))

    def test_get_dcci_disable_cap_bitmap_empty_patterns(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {}
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_a"])
        self.assertEqual(result, 0)

    def test_get_dcci_disable_cap_bitmap_patterns_not_list(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {"dcci-disable-on-kernel": "not_a_list"}
        }
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_a"])
        self.assertEqual(result, 0)

    def test_get_dcci_disable_cap_bitmap_patterns_empty_list(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {"sp_options": {"dcci-disable-on-kernel": []}}
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_a"])
        self.assertEqual(result, 0)

    def test_get_dcci_disable_cap_bitmap_matched(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {"dcci-disable-on-kernel": ["kernel_a", "kernel_b"]}
        }
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_a"])
        self.assertEqual(result, 4)

    def test_get_dcci_disable_cap_bitmap_not_matched(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {"dcci-disable-on-kernel": ["kernel_x", "kernel_y"]}
        }
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_a"])
        self.assertEqual(result, 0)

    def test_get_dcci_disable_cap_bitmap_wildcard_match(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {"dcci-disable-on-kernel": ["kernel_.*", ".*_test"]}
        }
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_abc"])
        self.assertEqual(result, 4)
        result = _get_dcci_disable_cap_bitmap(compile_info, ["op_test"])
        self.assertEqual(result, 4)

    def test_get_dcci_disable_cap_bitmap_multi_symbols_first_match(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {"dcci-disable-on-kernel": ["kernel_b"]}
        }
        result = _get_dcci_disable_cap_bitmap(
            compile_info, ["kernel_a", "kernel_b", "kernel_c"]
        )
        self.assertEqual(result, 4)

    def test_get_dcci_disable_cap_bitmap_multi_symbols_no_match(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {"dcci-disable-on-kernel": ["kernel_x"]}
        }
        result = _get_dcci_disable_cap_bitmap(
            compile_info, ["kernel_a", "kernel_b", "kernel_c"]
        )
        self.assertEqual(result, 0)

    def test_get_dcci_disable_cap_bitmap_no_sp_options(self):
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {"other_option": "value"}
        result = _get_dcci_disable_cap_bitmap(compile_info, ["kernel_a"])
        self.assertEqual(result, 0)


if __name__ == "__main__":
    unittest.main()
