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


THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)

from asc_op_compile_base.common.buildcfg import buildcfg


class TestBuildcfg(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_default_build_cfg(self):
        self.assertEqual(buildcfg.get_default_build_config("dynamic_shape"), False)
        self.assertEqual(buildcfg.get_default_build_config("bool_storage_as_1bit"), True)
        self.assertEqual(buildcfg.get_default_build_config("enable_mask_counter_mode"), "default_normal")
        self.assertEqual(buildcfg.get_default_build_config()["dump_ir"], False)
        self.assertEqual(buildcfg.get_default_build_config("all")["tik_debug_context_id"], -1)
        self.assertEqual(buildcfg.get_default_build_config("tir.test"), None)

    def test_current_build_cfg(self):
        buildcfg.set_current_build_config("dynamic_shape", True)
        buildcfg.set_current_build_config("bool_storage_as_1bit", False)
        buildcfg.set_current_build_config("early_start_mode", "enable")
        buildcfg.set_current_build_config("tir.test", "test")
        self.assertEqual(buildcfg.get_current_build_config("dynamic_shape"), True)
        self.assertEqual(buildcfg.get_current_build_config("bool_storage_as_1bit"), False)
        self.assertEqual(buildcfg.get_current_build_config("early_start_mode"), "enable")
        self.assertEqual(buildcfg.get_current_build_config("tir.test"), None)
        buildcfg.set_current_build_config("tir.is_dynamic_shape", False)
        self.assertEqual(buildcfg.get_current_build_config("tir.is_dynamic_shape"), False)
        self.assertEqual(buildcfg.get_current_build_config("dynamic_shape"), False)
        self.assertEqual(buildcfg.get_current_build_config()["is_dynamic_shape"], False)


if __name__ == "__main__":
    unittest.main()
