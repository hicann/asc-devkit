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

from asc_op_compile_base.common.error_mgr import TBEPythonError, raise_tbe_python_err
from asc_op_compile_base.common.error_mgr.error_manager_util import get_error_message


class TestErrMgr(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_tbe_python_error_init(self):
        error_info = {
            "errCode": "EB0001",
            "errClass": "RuntimeError",
            "errPcause": "params error",
            "errSolution": "check param formats",
            "message": "invalid param format",
        }
        error = TBEPythonError(error_info)
        self.assertEqual(error.args[0], "EB0001")
        self.assertEqual(error.args[1], "RuntimeError")
        self.assertEqual(error.args[2], "params error")
        self.assertEqual(error.args[3], "check param formats")
        self.assertEqual(error.args[4], "invalid param format")
        self.assertEqual(error.errorinfo, error_info)

    def test_raise_tbe_python_err_basic(self):
        err_code = "EB0003"
        err_msg = "basic error msg"

        with self.assertRaises(TBEPythonError) as context:
            raise_tbe_python_err(err_code, err_msg)

        exception = context.exception
        self.assertEqual(exception.args[0], err_code)
        self.assertEqual(exception.args[4], err_msg)
        self.assertEqual(exception.errorinfo["errCode"], err_code)
        self.assertEqual(exception.errorinfo["message"], err_msg)
        self.assertEqual(exception.errorinfo["errClass"], "")
        self.assertEqual(exception.errorinfo["errPcause"], "")
        self.assertEqual(exception.errorinfo["errSolution"], "")

    def test_raise_tbe_python_err_with_tuple_msg(self):
        err_code = "EB0004"
        inner_error_info = {
            "errCode": "EB0005",
            "errClass": "InnerError",
            "errPcause": "inner reason",
            "errSolution": "inner solution",
            "message": "inner error msg",
        }
        inner_error = TBEPythonError(inner_error_info)
        tuple_msg = ("outter error", inner_error)

        with self.assertRaises(TBEPythonError) as context:
            raise_tbe_python_err(err_code, tuple_msg)
        exception = context.exception
        expected_message = "outter error\ninner error msg"
        self.assertEqual(exception.args[0], err_code)
        self.assertEqual(exception.errorinfo["message"], expected_message)

    def test_get_error_message(self):
        self.mock_error_data = [
            {
                "errCode": "TEST001",
                "argList": "arg1,arg2,arg3",
                "errMessage": "error type: %s, reason: %s, soulution: %s",
            }
        ]

        mock_json_data = self.mock_error_data
        args = {
            "errCode": "TEST001",
            "arg1": "data type not match",
            "arg2": "wrong param types",
            "arg3": "check param types",
        }
        expected_message = "error type: data type not match, reason: wrong param types, soulution: check param types"

        with mock.patch("builtins.open", mock.mock_open()) as mock_file:
            with mock.patch("json.load", return_value=mock_json_data):
                result = get_error_message(args)
                self.assertEqual(result, expected_message)
                mock_file.assert_called_once()


if __name__ == "__main__":
    unittest.main()
