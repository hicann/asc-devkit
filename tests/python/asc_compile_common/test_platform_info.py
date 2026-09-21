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

from asc_op_compile_base.common.platform import get_soc_spec, set_current_compile_soc_info, get_block_size
from asc_op_compile_base.common.platform.platform_info import (
    set_soc_spec,
    te_update_version,
    set_platform_info_res,
    set_core_num_by_core_type,
    CORE_NUM,
    CUBE_SIZE,
    CORE_TYPE_LIST,
    L0A_LAYOUT_IS_zN,
    UB_BLOCK_SIZE,
)


def _get_soc_spec_mock(key):
    mock_map = {
        CORE_NUM: "4",
        CUBE_SIZE: "3,4,5",
        CORE_TYPE_LIST: "VectorCore,AiCore",
        L0A_LAYOUT_IS_zN: "1",
        UB_BLOCK_SIZE: "2048",
    }
    if key in mock_map.keys():
        return mock_map[key]
    return None


class TestPlatformInfo(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    @patch("asc_op_compile_base.common.platform.platform_info._get_soc_spec")
    def test_get_soc_spec(self, mock_get_soc_spec):
        mock_get_soc_spec.side_effect = _get_soc_spec_mock
        self.assertEqual(get_soc_spec(CORE_NUM), 4)
        self.assertEqual(get_soc_spec(CUBE_SIZE), [3, 4, 5])
        self.assertEqual(get_soc_spec(CORE_TYPE_LIST), ["VectorCore", "AiCore"])
        self.assertEqual(get_soc_spec(L0A_LAYOUT_IS_zN), True)
        self.assertEqual(get_block_size(), 2048)

    @patch("asc_op_compile_base.common.platform.platform_info._init_soc_spec")
    @patch("asc_op_compile_base.common.platform.platform_info._set_soc_spec")
    def test_set_soc_info(self, _set_soc_spec_mock, _init_soc_spec_mock):
        _init_soc_spec_mock.return_value = "success"
        _set_soc_spec_mock.return_value = "success"
        set_current_compile_soc_info("Ascend910B1")
        set_current_compile_soc_info("Ascend910B1", None, None, None)
        set_current_compile_soc_info("Ascend910B1", "VectorCore", 4, "Test")
        set_soc_spec(False)
        set_soc_spec(True)
        set_soc_spec("Test123")
        set_soc_spec(225)
        set_soc_spec(CORE_NUM)

    @patch("asc_op_compile_base.common.platform.platform_info._te_update_version")
    @patch("asc_op_compile_base.common.platform.platform_info._set_platform_info_res")
    @patch("asc_op_compile_base.common.platform.platform_info._set_core_num_by_core_type")
    def test_update_soc_infos(
        self, _set_core_num_by_core_type_mock, _set_platform_info_res_mock, _te_update_version_mock
    ):
        _te_update_version_mock.return_value = "success"
        _set_platform_info_res_mock.return_value = "success"
        _set_core_num_by_core_type_mock.return_value = "success"
        te_update_version()
        te_update_version("", "", 0, "")
        te_update_version("Ascend910B1", "VectorCore", 4, "Test")
        set_platform_info_res("", None)
        set_platform_info_res("3", "test_res")
        set_core_num_by_core_type("")
        set_core_num_by_core_type(None)
        set_core_num_by_core_type("AiCore")


if __name__ == "__main__":
    unittest.main()
