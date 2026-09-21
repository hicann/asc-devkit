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
from tbe.common.platform import set_current_compile_soc_info, get_soc_spec

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
API_ROOT_PATH = os.path.join(TOP_PATH, "build/adapter_ut")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/asc_op_compile_base/")
sys.path.append(FRAMEWORK_PATH)

from adapter.log_utils import LogUtil, AscendCLogLevel
from adapter.global_storage import global_var_storage


def SetCurrentSocInfo(soc: str):
    set_current_compile_soc_info(soc)
    global_var_storage.set_variable("ascendc_short_soc_version", get_soc_spec("SHORT_SOC_VERSION"))


class TestLogUtils(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_dump_log_fail(self):
        emptystr = ""
        LogUtil.dump_log(emptystr)
        self.assertEqual(emptystr, "")

    def test_print_compile_log_lowlevel(self):
        op_name = "test_op"
        output = "a debug info"
        with mock.patch("os.environ", {"ASCEND_GLOBAL_LOG_LEVEL": 3}):
            LogUtil().print_compile_log(op_name, output, AscendCLogLevel.LOG_DEBUG)

    def test_print_compile_plog_lowlevel(self):
        op_name = "test_op"
        output = "a debug info"
        with mock.patch("os.environ", {"ASCEND_GLOBAL_LOG_LEVEL": 3}):
            with mock.patch("os.environ", {"ASCEND_SLOG_PRINT_TO_STDOUT": 1}):
                LogUtil().plog_print(op_name, output, AscendCLogLevel.LOG_DEBUG)
                LogUtil().plog_print(op_name, output, AscendCLogLevel.LOG_INFO)
                LogUtil().plog_print(op_name, output, AscendCLogLevel.LOG_WARNING)
                LogUtil().plog_print(op_name, output, AscendCLogLevel.LOG_ERROR)
                self.assertEqual(op_name, "test_op")

    def test_detail_log_print(self):
        op_name = "test_op"
        output = "a detail info"
        with mock.patch("os.environ", {"ASCEND_GLOBAL_EVENT_ENABLE": 1}):
            LogUtil().detail_log_print(op_name, output, AscendCLogLevel.LOG_INFO)

    def test_fix_string_escapes(self):
        # test simple texts
        self.assertEqual(LogUtil.fix_string_escapes("Hellow World"), "Hellow World")
        self.assertEqual(LogUtil.fix_string_escapes("中文字符！！！"), "中文字符！！！")
        # test \n \r \t
        self.assertEqual(LogUtil.fix_string_escapes("\n"), "\n")
        self.assertEqual(LogUtil.fix_string_escapes("\t"), "\t")
        self.assertEqual(LogUtil.fix_string_escapes("\r\n"), "\r\n")
        # test C++ format symbols
        self.assertEqual(LogUtil.fix_string_escapes("Process 50% complete"), "Process 50%% complete")
        self.assertEqual(LogUtil.fix_string_escapes("%% in config"), "%%%% in config")
        self.assertEqual(LogUtil.fix_string_escapes("%s %d"), "%%s %%d")
        self.assertEqual(LogUtil.fix_string_escapes(r"%A%%"), "%%A%%%%")
        # test backslash
        self.assertEqual(LogUtil.fix_string_escapes("Path\\to\\file"), "Path\\to\\file")
        self.assertEqual(LogUtil.fix_string_escapes(r"C:\\Users\test"), "C:\\\\Users\\test")
        self.assertEqual(LogUtil.fix_string_escapes(r"\n"), "\\n")
        # test control symbols
        self.assertEqual(LogUtil.fix_string_escapes("\x07\a\x00\0\x0c\f\x05"), "\\a\\a\\0\\0\\f\\f\\x05")
        self.assertEqual(LogUtil.fix_string_escapes(r"\x07\a\x00\0\x0c\f\x05"), "\\x07\\a\\x00\\0\\x0c\\f\\x05")
        self.assertEqual(LogUtil.fix_string_escapes("\\x00\\0\\x0c\\f\\x05\\r\\n"), "\\x00\\0\\x0c\\f\\x05\\r\\n")
        # test mix
        self.assertEqual(LogUtil.fix_string_escapes('%\n%s\\n"100%完成\n结果："'), '%%\n%%s\\n"100%%完成\n结果："')
