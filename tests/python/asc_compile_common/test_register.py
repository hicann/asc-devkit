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
import functools


THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)

from asc_op_compile_base.common.register import (
    OpCompute,
    register_operator,
    register_param_generalization,
    set_fusion_buildcfg,
    get_op_compute,
    get_operator,
    get_param_generalization,
    get_fusion_buildcfg,
)
from asc_op_compile_base.common.register.operation_func_mgr import _generalization, _op_computes


class TestRegister(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_operator(self):
        def custom_add_v2(*args, **kwargs):
            return "success"

        deco = register_operator("custom_add_v2", None, False)
        custom_add_v2_deco = deco(custom_add_v2)
        op = get_operator("custom_add_v2")
        self.assertEqual(op.get_func().__name__, "custom_add_v2")

    def test_op_compute(self):
        def conv_compute(inputs, weights):
            return f"conv({inputs}, {weights})"

        @functools.wraps(conv_compute)
        def wrapper(*args, **kwargs):
            return conv_compute(*args, **kwargs)

        global _op_computes
        _op_computes[("conv2d", "dynamic")] = OpCompute(True, wrapper)
        op_compute = get_op_compute("conv2d")
        self.assertIsNotNone(op_compute)
        self.assertTrue(op_compute.if_support_fusion())
        self.assertEqual((op_compute.get_func())([1, 2, 3], [4, 5, 6]), "conv([1, 2, 3], [4, 5, 6])")

    def test_fusion_buildcfg(self):
        # test set
        conv_config = {"kernel_size": 3, "stride": 1}
        set_fusion_buildcfg("conv2d", conv_config)
        # test get
        result = get_fusion_buildcfg("conv2d")
        self.assertIsNotNone(result)
        self.assertEqual(result["kernel_size"], 3)
        self.assertEqual(result["stride"], 1)
        # test update
        conv_update = {"padding": 1, "dilation": 1}
        set_fusion_buildcfg("conv2d", conv_update)
        result = get_fusion_buildcfg("conv2d")
        self.assertEqual(result["kernel_size"], 3)
        self.assertEqual(result["stride"], 1)
        self.assertEqual(result["padding"], 1)
        self.assertEqual(result["dilation"], 1)
        # border values
        nonexistent = get_fusion_buildcfg("nonexistent")
        self.assertIsNone(nonexistent)
        with self.assertRaises(RuntimeError):
            set_fusion_buildcfg(None, {"key": "value"})
        set_fusion_buildcfg("empty_op", {})
        empty_result = get_fusion_buildcfg("empty_op")
        self.assertEqual(empty_result, {})

    def test_param_generalization(self):
        @register_param_generalization("conv2d")
        def conv_generalization(params):
            return params.get("kernel_size", 3)

        global _generalization
        self.assertIn("conv2d", _generalization)
        func = get_param_generalization("conv2d")
        self.assertIsNotNone(func)
        test_params = {"kernel_size": 5}
        result = func(test_params)
        self.assertEqual(result, 5)
        test_params2 = {}
        result2 = func(test_params2)
        self.assertEqual(result2, 3)


if __name__ == "__main__":
    unittest.main()
