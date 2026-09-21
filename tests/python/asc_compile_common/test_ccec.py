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
from unittest.mock import patch


THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)

from asc_op_compile_base.common.ccec import (
    current_build_config,
    switching_compilation_mode,
    _set_vector_fp_ceiling,
    _set_cce_overflow,
    check_is_regbase_v2,
    enable_sanitizer,
    _build_aicore_compile_cmd,
)
from asc_op_compile_base.common.buildcfg.buildcfg_mapping import dynamic_shape
from asc_op_compile_base.common.platform.platform_info import COMPILER_ARCH, ASCEND_031, ASCEND_910B


class TestCcec(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    @patch("asc_op_compile_base.common.platform.platform_info.get_soc_spec")
    def test_get_and_set(self, get_soc_spec_mock):
        self.assertEqual(current_build_config()[dynamic_shape], False)
        switching_compilation_mode()
        self.assertEqual(_set_vector_fp_ceiling([]), ["-mllvm", "-cce-aicore-fp-ceiling=2"])
        self.assertEqual(_set_cce_overflow([]), ["-mllvm", "-cce-aicore-record-overflow=false"])
        get_soc_spec_mock.return_value = ASCEND_031
        self.assertEqual(check_is_regbase_v2(), True)
        get_soc_spec_mock.return_value = ASCEND_910B
        self.assertEqual(enable_sanitizer(), False)

    @patch("asc_op_compile_base.common.platform.platform_info.get_soc_spec")
    def test_build_aicore_compile_cmd(self, mock_get_soc_spec):
        def get_soc_spec_mock(key):
            mock_map = {
                COMPILER_ARCH: "dav-l300",
                "SHORT_SOC_VERSION": "Ascend910B",
                "AICORE_TYPE": "AiCore",
                "VECTOR_REG_WIDTH": 128,
            }
            if key in mock_map.keys():
                return mock_map[key]
            return None

        mock_get_soc_spec.side_effect = get_soc_spec_mock
        expect_cmd = [
            "ccec",
            "-c",
            "-O3",
            "test_src",
            "--cce-aicore-arch=dav-l300",
            "--cce-aicore-only",
            "-o",
            "test_dst",
            "-mllvm",
            "-cce-aicore-stack-size=32768",
            "-mllvm",
            "-cce-aicore-function-stack-size=32768",
            "-mllvm",
            "-cce-aicore-record-overflow=false",
            "-mllvm",
            "-cce-aicore-addr-transform",
            "--cce-auto-sync=off",
            "-mllvm",
            "-cce-aicore-jump-expand=false",
            "-mllvm",
            "-cce-aicore-mask-opt=false",
        ]
        cmd = _build_aicore_compile_cmd("test_src", "test_dst", "testname_mix_aic")
        self.assertEqual(cmd, expect_cmd)


if __name__ == "__main__":
    unittest.main()
