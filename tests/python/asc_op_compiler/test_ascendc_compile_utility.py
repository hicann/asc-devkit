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
import platform
import unittest
from unittest import mock

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
API_ROOT_PATH = os.path.join(TOP_PATH, "build/adapter_ut")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)

from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import *
from asc_op_compile_base.asc_op_compiler.super_kernel_option_parse import *
from asc_op_compile_base.common.context.op_context import OpContext


class TestCompileUtility(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_is_support_super_kernel(self):
        get_soc = global_var_storage.get_variable("ascendc_short_soc_version")
        global_var_storage.set_variable("ascendc_short_soc_version", "Ascend910_93")
        res = CommonUtility.is_support_super_kernel()
        self.assertEqual(res, True)
        global_var_storage.set_variable("ascendc_short_soc_version", get_soc)

    def test_code_test_align_parser(self):
        tmp = CodeTextAlignParser("func-align")
        self.assertRaises(Exception, tmp.parse_option, "test")
        self.assertRaises(Exception, tmp.parse_option, "3")
        res = tmp.parse_option("512")
        self.assertEqual(res, 512)

    def test_enum_parser(self):
        tmp = EnumParser(
            "early-start",
            {
                "0": SuperKernelEarlyStartMode.EarlyStartDisable,
                "1": SuperKernelEarlyStartMode.EarlyStartEnableV2,
                "2": SuperKernelEarlyStartMode.EarlyStartV2DisableSubKernel,
            },
        )
        self.assertRaises(Exception, tmp.parse_option, "3")

    def test_binary_parser(self):
        tmp = BinaryParser("enable-test")
        self.assertRaises(Exception, tmp.parse_option, "3")
        res = tmp.parse_option("1")
        self.assertEqual(res, "1")

    def test_number_parser(self):
        tmp = NumberParser("split-mode")
        self.assertRaises(Exception, tmp.parse_option, "test")
        self.assertRaises(Exception, tmp.parse_option, "128")
        res = tmp.parse_option("4")
        self.assertEqual(res, 4)

    def test_non_empty_parser(self):
        tmp = NonEmptyParser("compile-options")
        self.assertRaises(Exception, tmp.parse_option, "")

    def test_parse_super_kernel_options(self):
        # parse_super_kernel_options("func-align:early-start=:")
        self.assertRaises(
            Exception, parse_super_kernel_options, "func-align:early-start=:"
        )
        self.assertRaises(
            Exception,
            parse_super_kernel_options,
            "func-align:early-start=1:early-start=1",
        )
        self.assertRaises(
            Exception, parse_super_kernel_options, "func-align:early-start=1:test=1"
        )

    def test_parse_super_kernel_options_none_and_empty(self):
        # None should return {} without AttributeError
        res = parse_super_kernel_options(None)
        self.assertEqual(res, {})
        # empty string should return {}
        res = parse_super_kernel_options("")
        self.assertEqual(res, {})
        # whitespace-only string should return {}
        res = parse_super_kernel_options("   ")
        self.assertEqual(res, {})

    def test_parse_super_kernel_options_strip_quotes(self):
        res = parse_super_kernel_options('"early-start=1"')
        self.assertEqual(
            res, {"early-start": SuperKernelEarlyStartMode.EarlyStartEnableV2}
        )

    def test_aclgraph_options_accept_dcci_underscore_keys(self):
        with OpContext() as ctx:
            ctx.add_addition("super_kernel_sub_combine", True)
            res = parse_super_kernel_options(
                "early_start=1:dcci_before_kernel_start=MatMul.*:stream_fusion=1:debug_sync_all=1"
            )
        self.assertEqual(
            res,
            {
                "early-start": "1",
                "dcci-before-kernel-start": "MatMul.*",
                "debug-sync-all": "1",
            },
        )

    def test_aclgraph_options_accept_debug_per_op_max_core_num(self):
        with OpContext() as ctx:
            ctx.add_addition("super_kernel_sub_combine", True)
            res = parse_super_kernel_options("debug_per_op_max_core_num=1")
        self.assertEqual(res, {"debug-per-op-max-core-num": "1"})

    def test_aclgraph_options_reject_non_binary_early_start(self):
        with OpContext() as ctx:
            ctx.add_addition("super_kernel_sub_combine", True)
            self.assertRaises(Exception, parse_super_kernel_options, "early_start=2")

    def test_ge_options_reject_underscore_keys(self):
        self.assertRaises(Exception, parse_super_kernel_options, "early_start=1")

    def test_aclgraph_options_ignore_numeric_preload_code(self):
        for value in ("0", "1", "2"):
            with self.subTest(value=value):
                with OpContext() as ctx:
                    ctx.add_addition("super_kernel_sub_combine", True)
                    res = parse_super_kernel_options(f"preload-code={value}")
                self.assertEqual(res, {})

    def test_check_func_align(self):
        self.assertRaises(Exception, check_func_align, "test")
        self.assertRaises(Exception, check_func_align, "3")
        self.assertRaises(Exception, check_func_align, 10)
        self.assertRaises(Exception, check_func_align, -5)

    def test_gen_func_align_attribute(self):
        self.assertEqual(
            gen_func_align_attribute("512"), "__attribute__((aligned(512)))"
        )
        self.assertEqual(gen_func_align_attribute(0), "")
        self.assertRaises(Exception, gen_func_align_attribute, "xxx")
        self.assertRaises(Exception, gen_func_align_attribute, 10)

    def test_process_ascendc_api_version(self):
        cce_file = "./tmp.cpp"
        compile_options = ["-DASCENDC_API_VERSION=20250330"]
        extend_options = {}
        process_ascendc_api_version(cce_file, compile_options, extend_options)
        self.assertIn("-DASCENDC_API_VERSION=20250330", compile_options)
        compile_options = ["/tmp/testcase/../ascendc/common"]
        extend_options = {"opp_kernel_hidden_dat_path": "./test.dat"}
        process_ascendc_api_version(cce_file, compile_options, extend_options)
        self.assertIn("-DASCENDC_API_VERSION=20250330", compile_options)
        with mock.patch("os.path.exists", return_value=True):
            compile_options = ["/tmp/testcase/../ascendc/common"]
            extend_options = {"opp_kernel_hidden_dat_path": "./test.dat"}
            process_ascendc_api_version(cce_file, compile_options, extend_options)
            self.assertIn("-DASCENDC_API_VERSION=20250330", compile_options)

        mock_popen = mock.Mock()
        mock_communicate = mock.Mock(
            return_value=(
                b"test\n mock stdout\n Contents of section .ascendc.api.version: \n 0000 50000000 00000000 \n end",
                b"mock_stderr",
            )
        )
        type(mock_popen).communicate = mock_communicate
        type(mock_popen).returncode = 0
        with mock.patch("os.path.exists", return_value=True):
            with mock.patch(
                "subprocess.Popen", return_value=mock_popen
            ) as mock_popen_cls:
                compile_options = ["/tmp/testcase/../ascendc/common"]
                extend_options = {"opp_kernel_hidden_dat_path": "./test.dat"}
                process_ascendc_api_version(cce_file, compile_options, extend_options)
                self.assertIn("-DASCENDC_API_VERSION=80", compile_options)

        with mock.patch("os.path.exists", return_value=True):
            with mock.patch(
                "subprocess.Popen", return_value=mock_popen
            ) as mock_popen_cls:
                compile_options = ["/tmp/testcase/../ascendc/common"]
                extend_options = {}
                process_ascendc_api_version(cce_file, compile_options, extend_options)
                self.assertIn("-DASCENDC_API_VERSION=80", compile_options)

        with mock.patch("os.path.exists", return_value=False):
            res = get_op_tiling_so_path(
                "/usr/local/Ascend/CANN-7.8/opp/vendors/customize/op_impl/ai_core/tbe/customize_impl/dynamic/test.cpp"
            )
            self.assertIn(
                f"op_tiling/lib/linux/{platform.machine()}/liboptiling.so", res
            )

        mock_communicate = mock.Mock(
            return_value=(
                b"test\n mock stdout\n Contents of section xxxx: \n 0000 50000000 00000000 \n end",
                b"mock_stderr",
            )
        )
        type(mock_popen).communicate = mock_communicate
        type(mock_popen).returncode = 0
        with mock.patch("os.path.exists", return_value=True):
            with mock.patch(
                "subprocess.Popen", return_value=mock_popen
            ) as mock_popen_cls:
                compile_options = ["/tmp/testcase/../ascendc/common"]
                extend_options = {}
                process_ascendc_api_version(cce_file, compile_options, extend_options)
                self.assertIn("-DASCENDC_API_VERSION=20250330", compile_options)


if __name__ == "__main__":
    unittest.main()
