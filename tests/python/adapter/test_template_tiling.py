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
API_ROOT_PATH = os.path.join(TOP_PATH, "build/adapter_ut")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/asc_op_compile_base/")
sys.path.append(FRAMEWORK_PATH)

from adapter import template_tiling
from adapter.template_tiling import *
import tbe.common.context


class TestCompileOp(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_template_tiling(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling()
        self.assertEqual(result.get(17176852), {"paramArgs": [20, 25, 6, 1]})

    def test_template_tiling_err_tpl(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args,
            (
                "Cannot find flag in ASCENDC_TPL_UINT_SEL Z! Value should be in [ASCENDC_TPL_UI_RANGE, ASCENDC_TPL_UI_LIST, ASCENDC_TPL_UI_MIX].",
            ),
        )

    def test_template_tiling_no_flag(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {10, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args,
            (
                "Cannot find flag in ASCENDC_TPL_UINT_SEL Z! Value should be in [ASCENDC_TPL_UI_RANGE, ASCENDC_TPL_UI_LIST, ASCENDC_TPL_UI_MIX].",
            ),
        )

    def test_template_tiling_null(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"

        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling()
        self.assertIsNone(result.get(17176851112))

    def test_template_tiling_single(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"

        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling(17176852)
        self.assertEqual(result.get(17176852), {"paramArgs": [20, 25, 6, 1]})

    def test_template_tiling_single_null(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"

        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling(17176851112)
        self.assertIsNone(result.get(17176851112))

    def test_template_tiling_ui_range(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 20, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args,
            (
                "UI_RANGE declare parse failed, because the announced length of ASCENDC_TPL_UINT_DECL Z is greater than actual values' length.",
            ),
        )

    def test_template_tiling_duplicated(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 3, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args,
            ("There is duplicated number in ASCENDC_TPL_DECL_UINT Z! Duplicated List: [0, 1, 2, 3, 4, 3, 6].",),
        )

        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},"
            "@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        )
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
            result = decode_tiling()
        self.assertEqual(e.exception.args, ("ASCENDC_TPL_SELECT has duplicated definitions!",))

    def test_template_tiling_name(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S111@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(e.exception.args, ("There is missing marco define: S in ASCENDC_TPL_BOOL_SEL.",))

    def test_template_invalid_params(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},"
            "@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        )
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(e.exception.args, ("Length of ASCENDC_TPL_UINT_SEL Z is too short.",))

        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},"
            "@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = {},@@ASCENDC_TPL_UINT_SEL_Z@@ = {4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}}, };"
        )
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(e.exception.args, ("values of ASCENDC_TPL_FORMAT_SEL Y is empty!",))
        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},"
            "@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = {15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1,4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {3, 1}}, };"
        )
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args, ("There is invalid number in ASCENDC_TPL_BOOL_SEL S! Value should only be in [0, 1].",)
        )
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {1, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args,
            (
                "Bit width:1 in ASCENDC_TPL_UINT_DECL Z is not enough to represent all values: [0, 1, 2, 3, 4, 5, 6]! Please make sure 2^bitWidth is greater than or equal to the [number of values].",
            ),
        )
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {0, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(e.exception.args, ("Bit width in ASCENDC_TPL_UINT_DECL Z cannot be less than zero!",))
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},"
            "@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_FORMAT_SEL_S@@ = {0, 1}}, };"
        )
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args, ("S has different type in ASCENDC_TPL_FORMAT_SEL and ASCENDC_TPL_BOOL_DECL.",)
        )
        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@ASCENDC_TPL_BOOL_SEL_XXX@@ = {0, 1}},"
            "@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 1, 6},@@ASCENDC_TPL_BOOL_SEL_XXX@@ = {0, 1}}, };"
        )
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(e.exception.args, ("Cannot find ASCENDC_TPL_BOOL_SEL name: XXX in ASCENDC_TPL_BOOL_DECL.",))
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {64, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(e.exception.args, ("name:Z, type:UINT, Total bit width cannot be greater than 64!",))

    def test_template_tiling_struct(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1},@@ASCENDC_TPL_TILING_STRUCT_SEL_tilingDataStruct@@ = {}},};"
        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling()
        self.assertEqual(result.get(17176852).get("paramArgs"), [20, 25, 6, 1])

    def test_template_tiling_kernel(self):
        declare_param_str = "@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {@@ASCENDC_TPL_DTYPE_DECL_D_T_X@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Y@@ = {10, 20},@@ASCENDC_TPL_DTYPE_DECL_D_T_Z@@ = {10, 20},@@ASCENDC_TPL_UINT_DECL_TILE_NUM@@ = {8, 2, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8},@@ASCENDC_TPL_BOOL_DECL_IS_SPLIT@@ = {0, 1},};"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {2}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {10}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {10}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {10}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},}, @@{@@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {0}, @@ASCENDC_TPL_DTYPE_SEL_D_T_X@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Y@@ = {20}, @@ASCENDC_TPL_DTYPE_SEL_D_T_Z@@ = {20}, @@ASCENDC_TPL_UINT_SEL_TILE_NUM@@ = {1, 1, 8}, @@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {0, 1},},};"
        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling()
        self.assertEqual(result.get(17435146).get("paramArgs"), [10, 10, 10, 1, 0])
        self.assertEqual(result.get(17435146).get("kernelType"), 2)

    def test_template_tiling_datatype(self):
        input0 = int(template_tiling.ASCENDC_TPL_INPUT_BIAS)
        output0 = int(template_tiling.ASCENDC_TPL_OUTPUT_BIAS)
        input0_expr = f"({input0} + (3 * 7 - 21))"
        output0_expr = f"({output0} + ((8 // 4) - 2))"
        d_t_x_sel_expr = "(13 * 2 + 1)"
        d_t_y_sel_expr = "((5 - 2) * 1)"
        declare_param_str = (
            f"@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {{@@ASCENDC_TPL_DATATYPE_DECL_D_T_X@@ = {{C_DT_FLOAT16, C_DT_BF16, {input0_expr}}},"
            f"@@ASCENDC_TPL_DATATYPE_DECL_D_T_Y@@ = {{C_DT_FLOAT, C_DT_INT32, {output0_expr}}},"
            "@@ASCENDC_TPL_BOOL_DECL_IS_SPLIT@@ = {0, 1},};"
        )
        select_param_str = (
            f"@@ASCENDC_TPL_LISTS@@ = {{@@{{@@ASCENDC_TPL_DATATYPE_SEL_D_T_X@@ = {{{d_t_x_sel_expr}}},"
            f"@@ASCENDC_TPL_DATATYPE_SEL_D_T_Y@@ = {{{d_t_y_sel_expr}}},@@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {{1}},}},}};"
        )
        op_info = OpInfo(inputs=[{"dtype": ["float16", "bfloat16"]}], outputs=[{"dtype": ["float32", "int32"]}])

        with tbe.common.context.op_context.OpContext():
            with mock.patch.object(tbe.common.context.get_context(), "get_addition", return_value=""):
                with mock.patch.object(
                    template_tiling, "safe_parse_value", wraps=template_tiling.safe_parse_value
                ) as mock_safe_parse_value:
                    extract_template_tiling_info(declare_param_str, select_param_str)
                    dtype_options, dtype_select_indexes = extract_decl_param_options(op_info, "dtype")
                    result = decode_tiling()

        self.assertIn(mock.call("C_DT_BF16"), mock_safe_parse_value.call_args_list)
        self.assertIn(mock.call("C_DT_INT32"), mock_safe_parse_value.call_args_list)
        self.assertIn(mock.call(input0_expr), mock_safe_parse_value.call_args_list)
        self.assertIn(mock.call(output0_expr), mock_safe_parse_value.call_args_list)
        self.assertIn(mock.call(d_t_x_sel_expr), mock_safe_parse_value.call_args_list)
        self.assertIn(mock.call(d_t_y_sel_expr), mock_safe_parse_value.call_args_list)
        self.assertEqual(dtype_options, [["float16", "bfloat16"], ["float32", "int32"]])
        self.assertEqual(dtype_select_indexes, [True, True])
        decoded_info = next(iter(result.values()))
        self.assertEqual(decoded_info.get("dtypeParams"), ["bfloat16", "int32"])
        self.assertEqual(decoded_info.get("paramArgs"), ["TypeFromId<27>::type", "TypeFromId<3>::type", "1"])

    def test_template_tiling_datatype_parse_values(self):
        self.assertEqual(safe_parse_value("27"), 27)
        self.assertEqual(safe_parse_value("(13 * 2 + 1)"), 27)
        self.assertEqual(safe_parse_value("C_DT_BF16"), 27)
        self.assertEqual(safe_parse_value("C_FORMAT_ND"), 2)
        self.assertEqual(safe_parse_value("C_DT_UNKNOWN"), -1)
        self.assertEqual(extract_expr("{C_DT_FLOAT16, (13 * 2 + 1), C_DT_UNKNOWN, C_FORMAT_ND}"), [1, 27, 2])

    def test_template_tiling_datatype_invalid_select_value(self):
        declare_param_str = (
            "@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {@@ASCENDC_TPL_DATATYPE_DECL_D_T_X@@ = {C_DT_FLOAT16, C_DT_BF16},"
            "@@ASCENDC_TPL_BOOL_DECL_IS_SPLIT@@ = {0, 1},};"
        )
        select_param_str = (
            "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DATATYPE_SEL_D_T_X@@ = {(2 * 3 + 1)},"
            "@@ASCENDC_TPL_BOOL_SEL_IS_SPLIT@@ = {1},},};"
        )

        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        self.assertEqual(
            e.exception.args,
            (
                "Cannot find value {7} from ASCENDC_TPL_DATATYPE_SEL D_T_X in ASCENDC_TPL_DATATYPE_DECL,"
                " please check your macro define.",
            ),
        )

    def test_template_tiling_datatype_invalid_parse_value(self):
        declare_param_str = (
            "@@ASCENDC_TPL_ARGS_DECL_AddTemplateCustom@@ = {@@ASCENDC_TPL_DATATYPE_DECL_D_T_X@@ = {C_DT_UNKNOWN},};"
        )

        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, "")
        self.assertEqual(e.exception.args, ("values of ASCENDC_TPL_DATATYPE_DECL D_T_X is empty!",))

    def test_template_tiling_deterministic(self):
        declare_param_str = "@@structFlashAttentionScore@@ =@@ASCENDC_TPL_ARGS_DECL_FlashAttentionScore@@ = {@@ASCENDC_TPL_DTYPE_DECL_X@@ = { 10, 30, 20},@@ASCENDC_TPL_FORMAT_DECL_Y@@ = {15, 25},@@ASCENDC_TPL_UINT_DECL_Z@@ = {8, 2, 2, 0, 2, 3, 4, 5, 6},@@ASCENDC_TPL_BOOL_DECL_S@@ = {0, 1}, };"
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {false},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1},@@ASCENDC_TPL_TILING_STRUCT_SEL_tilingDataStruct@@ = {}},};"
        extract_template_tiling_info(declare_param_str, select_param_str)
        result = decode_tiling()
        self.assertEqual(result.get(17176852).get("paramArgs"), [20, 25, 6, 1])
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {0},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1},@@ASCENDC_TPL_TILING_STRUCT_SEL_tilingDataStruct@@ = {}},};"
        extract_template_tiling_info(declare_param_str, select_param_str)
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {1},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1},@@ASCENDC_TPL_TILING_STRUCT_SEL_tilingDataStruct@@ = {}},};"
        extract_template_tiling_info(declare_param_str, select_param_str)
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {3},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1},@@ASCENDC_TPL_TILING_STRUCT_SEL_tilingDataStruct@@ = {}},};"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)
        select_param_str = "@@ASCENDC_TPL_LISTS@@ = {@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 10, 30},@@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {0, 1},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15},@@ASCENDC_TPL_UINT_SEL_Z@@ = {1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1}},@@{@@ASCENDC_TPL_DTYPE_SEL_X@@ = { 20},@@ASCENDC_TPL_FORMAT_SEL_Y@@ = { 15, 25},@@ASCENDC_TPL_UINT_SEL_Z@@ = { 1, 4, 6},@@ASCENDC_TPL_BOOL_SEL_S@@ = {0, 1},@@ASCENDC_TPL_TILING_STRUCT_SEL_tilingDataStruct@@ = {}},};"
        with self.assertRaises(RuntimeError) as e:
            extract_template_tiling_info(declare_param_str, select_param_str)


if __name__ == "__main__":
    unittest.main()
