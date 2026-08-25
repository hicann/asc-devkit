#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
Opc test case
"""

import unittest
import os
import sys
import json

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
ASC_OPC_PATH = os.path.join(TOP_PATH, "tools/build/asc_opc/python/asc_opc_tool/")
OPC_STUB_PATH = os.path.join(TOP_PATH, "tests/python/asc_opc/stub/")
sys.path.insert(0, FRAMEWORK_PATH)
sys.path.insert(0, ASC_OPC_PATH)
sys.path.insert(0, OPC_STUB_PATH)

from opc import OpcOptionParser
import opc as opc
import opc_api as opc_api
from constant import OpcOptions
from opc_common import get_int64_mode
import asc_op_compile_base.common.utils.log as logger
from op_compile_info_check import check_op_optional_paramtype
import tempfile


class RequiredValidArgs:
    """
    基础的参数，能确保opt_parser的check通过.
    """

    def __enter__(self):
        self.temp_op_file = tempfile.NamedTemporaryFile()
        return list(
            [
                self.temp_op_file.name,
                "--soc_version",
                "Ascend910",
                "--bin_filename",
                "test_bin",
            ]
        )

    def __exit__(self, *unused):
        # 由于是temp文件，在关闭时会自动删除
        self.temp_op_file.close()


class TestOpc01(unittest.TestCase):
    def set_up(self):
        pass

    def tear_down(self):
        pass

    def test_dir_01(self):
        parser = OpcOptionParser()
        ret = parser.check_dir_valid("xxx")
        self.assertEqual(ret, False)

    def test_dir_02(self):
        parser = OpcOptionParser()
        ret = parser.check_dir_valid(
            "C:\\Program Files\\JetBrains\\PyCharm 2019.2\\helpers\\"
        )
        self.assertEqual(ret, False)


current_dir = os.path.abspath(os.getcwd())


class TestOpc02(unittest.TestCase):
    def set_up(self):
        pass

    def tear_down(self):
        pass

    def check_options(self, options, expect_options):
        """
        "ne" means not exsit.
        """
        op_path = options.get("op_path", "")
        input_param = options.get("input_param", "not_exist")
        main_func = options.get("main_func", "not_exist")
        soc_version = options.get("soc_version", "not_exist")
        output = options.get("output", "not_exist")
        debug_dir = options.get("debug_dir", "not_exist")
        h = options.get("h", "not_exist")
        log = options.get("log", "not_exist")
        core_type = options.get("core_type", "AiCore")
        aicore_num = options.get("aicore_num", "not_exist")
        mdl_bank_path = options.get("mdl_bank_path", "not_exist")
        op_bank_path = options.get("op_bank_path", "not_exist")
        op_debug_level = options.get("op_debug_level", "not_exist")
        impl_mode = options.get("impl_mode", "not exist")

        self.assertEqual(expect_options["op_path"], op_path)
        self.assertEqual(expect_options["input_param"], input_param)
        self.assertEqual(expect_options["main_func"], main_func)
        self.assertEqual(expect_options["soc_version"], soc_version)
        logger.info("%s output %s", expect_options["output"], output)
        self.assertEqual(expect_options["output"], output)
        debug_dir_check = expect_options["debug_dir"] in debug_dir
        logger.info(
            "%s output %s, debug_dir_check %s",
            expect_options["debug_dir"],
            debug_dir,
            debug_dir_check,
        )
        self.assertEqual(debug_dir_check, True)
        logger.info("%s output %s", expect_options["h"], h)
        self.assertEqual(expect_options["h"], h)
        logger.info("%s output %s", expect_options["log"], log)
        self.assertEqual(expect_options["log"], log)
        logger.info("%s output %s", expect_options.get("core_type"), core_type)

    def test_options_parser_01(self):
        logger.info(current_dir)
        logger.info(
            "Start to execute ==============test_options_parser_01=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"

        sys.argv = [
            "xx",
            "--op_path=./op_impl/dynamic/mat_mul.py",
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": "--op_path=./op_impl/dynamic/mat_mul.py",
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": "AiCore",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_02(self):
        """
        op path use relative path, which does not exist
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_02=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"

        sys.argv = [
            "xx",
            "../../stub/files/mat_mul.py",
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": "../../stub/files/mat_mul.py",
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": "AiCore",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_03(self):
        """
        input param: matmul_test_false.json does not exist
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_03=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test_false.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"
        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]

        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": "AiCore",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_04(self):
        """
        op path is empty, --main_func=mat_mul will be
        considered as op path
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_04=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"

        sys.argv = [
            "xx",
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": "",
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_05(self):
        """
        correct case
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_05=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_06(self):
        """
        soc version is empty
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_06=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "",
            "output": "./output",
            "debug_dir": "./",
            "h": "False",
            "log": "null",
            "core_type": "AiCore",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_07(self):
        """
        main_func is empty, which is allowed
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_07=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": None,
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_08(self):
        """
        output is empty, which will use current dir as output dir
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_08=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_09(self):
        """
        debug dir is empty, which will use current dir(./) as debug dir
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_09=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_09_01(self):
        """
        debug dir is not empty
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_09_01=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--op_debug_config=dump_cce",
            "--output=./output",
            "--debug_dir=" + test_root_dir,
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "op_debug_config": "dump_cce",
            "output": current_dir + "/output",
            "debug_dir": test_root_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_09_02(self):
        """
        output not exist, opc will create
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_09_02=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./outputtemp",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/outputtemp",
            "debug_dir": current_dir,
            "h": "False",
            "log": "null",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)
        # remove current_dir + "/outputtemp" create by opc
        if os.path.exists(current_dir + "/outputtemp"):
            logger.info("Delete existed output dir created by opc.")
            os.rmdir(current_dir + "/outputtemp")

    def test_options_parser_10(self):
        """
        log is not empty
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_10=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "debug",
            "core_type": None,
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)
        self.assertEqual(opt_parser.get_option("log"), "debug")

    def test_options_parser_11(self):
        """
        core_type is not empty
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_11=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--core_type=VectorCore",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "debug",
            "core_type": "VectorCore",
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_12(self):
        """
        core_type is not empty
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_12=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--core_type=VectorCore",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "debug",
            "core_type": "VectorCore",
        }
        self.assertEqual(opt_parser.check_input_params(), True)
        self.check_options(options, expect_options)

    def test_options_parser_13(self):
        """
        no arguments
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_13=============="
        )
        sys.argv = ["xx"]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": "",
            "input_param": None,
            "main_func": None,
            "soc_version": "",
            "output": "",
            "debug_dir": "",
            "h": "False",
            "log": "null",
            "core_type": "AiCore",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_14(self):
        """
        -h and --help only
        :return:
        """
        logger.info(
            "Start to execute ==============test_options_parser_14=============="
        )
        sys.argv = ["xx"]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": "",
            "input_param": None,
            "main_func": None,
            "soc_version": "",
            "output": "",
            "debug_dir": "",
            "h": "False",
            "log": "null",
            "core_type": "AiCore",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_18(self):
        """
        test invalid core_type: ai_vector
        """
        logger.info(
            "Start to execute ==============test_options_parser_18=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--core_type=ai_vector",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        expect_options = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": current_dir + "/output",
            "debug_dir": current_dir,
            "h": "False",
            "log": "debug",
            "core_type": "ai_vector",
        }
        self.assertEqual(opt_parser.check_input_params(), False)
        self.check_options(options, expect_options)

    def test_options_parser_19(self):
        """
        test invalid aicore_num: sih
        """
        logger.info(
            "Start to execute ==============test_options_parser_19=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--aicore_num=sih",
        ]
        try:
            ret, opt_parser = opc.parse_args()
        except SystemExit as e:
            logger.info("aicore_num only support digit.")
            self.assertEqual("1", str(e))

    def test_options_parser_20(self):
        """
        test invalid mdl_bank_path
        """
        logger.info(
            "Start to execute ==============test_options_parser_20=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), True)

    def test_options_parser_21(self):
        """
        test invalid op_bank_path
        """
        logger.info(
            "Start to execute ==============test_options_parser_21=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), True)

    def test_options_parser_22(self):
        """
        test invalid op_debug_level
        """
        logger.info(
            "Start to execute ==============test_options_parser_22=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--op_debug_level=5",
            "--op_debug_config=dump_cce,dump_bin",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), False)

    def test_options_parser_23(self):
        """
        test invalid impl_mode: ascend
        """
        logger.info(
            "Start to execute ==============test_options_parser_23=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--impl_mode=high_precision,enable_hi_float_32_execution",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), True)

    def test_options_parse_24(self):
        """
        test valid core_type, aicore_num, mdl_bank_path, op_bank_path, impl_mode
        """
        logger.info(
            "Start to execute ==============test_options_parse_24=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        path = test_root_dir + "/output"
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--core_type=AiCore",
            "--aicore_num=1",
            "--impl_mode=high_performance",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), True)

    def test_options_parse_25(self):
        """
        test valid core_type, aicore_num, mdl_bank_path, op_bank_path, impl_mode
        """
        logger.info(
            "Start to execute ==============test_options_parse_25=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        path = test_root_dir + "/output"
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--core_type=AiCore,VectorCore",
            "--aicore_num=1",
            "--impl_mode=high_performance",
            "--simplified_key_mode=0",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), True)

    def test_options_parser_debug_config(self):
        """
        correct case
        :return:
        """
        logger.info(
            "Start to execute ==============ut_test_options_parser_debug_config=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--op_debug_config=None",
            "--output=./output",
        ]
        ret, opt_parser = opc.parse_args()
        opc.op_compile_classify(opt_parser)
        self.assertEqual(opt_parser.check_input_params(), False)

    def test_options__deterministic__when_not_specified__will_set_to_all_as_default(
        self,
    ):
        sys.argv = []

        _, result_parser = opc.parse_args()
        result_option = result_parser.get_all_options()

        assert "deterministic" in result_option
        assert result_option["deterministic"] == "all"

    def test_options__deterministic__when_value_invalid__will_check_failed(self):
        def do_check_input_args(args):
            with RequiredValidArgs() as incare_required_args:
                sys.argv = incare_required_args + args
                _, result_parser = opc.parse_args()
                print(sys.argv)
            return result_parser.check_input_params()

        assert do_check_input_args(["--deterministic", "all"]) == True
        assert do_check_input_args(["--deterministic", "true"]) == True
        assert do_check_input_args(["--deterministic", "false"]) == True
        assert do_check_input_args(["--deterministic", "other"]) == False

    def test_kernel_spec_defaults_to_none(self):
        with RequiredValidArgs() as required_args:
            sys.argv = required_args
            _, result_parser = opc.parse_args()

        options = result_parser.get_all_options()
        self.assertEqual(options[OpcOptions.KERNEL_SPEC], "None")
        self.assertIsNone(options[OpcOptions.KERNEL_SPEC_DIR])

    def test_kernel_spec_accepts_normal_and_sk(self):
        for mode, relocatable in (
            ("Normal", "false"),
            ("SK", "false"),
        ):
            with self.subTest(mode=mode), tempfile.TemporaryDirectory() as output_dir:
                with RequiredValidArgs() as required_args:
                    sys.argv = required_args + [
                        "--kernel-spec=" + mode,
                        "--kernel-spec-dir=" + output_dir,
                        "--op_relocatable_kernel_binary=" + relocatable,
                    ]
                    _, result_parser = opc.parse_args()

                self.assertTrue(result_parser.check_input_params())
                options = result_parser.get_all_options()
                self.assertEqual(options[OpcOptions.KERNEL_SPEC], mode)
                self.assertEqual(
                    options[OpcOptions.KERNEL_SPEC_DIR], os.path.realpath(output_dir)
                )

    def test_kernel_spec_none_accepts_relocatable_binary(self):
        with RequiredValidArgs() as required_args:
            sys.argv = required_args + [
                "--op_relocatable_kernel_binary=true",
            ]
            _, result_parser = opc.parse_args()

        self.assertTrue(result_parser.check_input_params())
        self.assertTrue(result_parser.get_option(OpcOptions.RELOCATABLE_BIN))

    def test_kernel_spec_rejects_invalid_combinations(self):
        cases = (
            ("invalid mode", ["--kernel-spec=All"]),
            ("normal without dir", ["--kernel-spec=Normal"]),
            ("sk without dir", ["--kernel-spec=SK"]),
            (
                "normal with relocatable binary",
                [
                    "--kernel-spec=Normal",
                    "--kernel-spec-dir={}",
                    "--op_relocatable_kernel_binary=true",
                ],
            ),
            (
                "sk with relocatable binary",
                [
                    "--kernel-spec=SK",
                    "--kernel-spec-dir={}",
                    "--op_relocatable_kernel_binary=true",
                ],
            ),
        )
        for name, args in cases:
            with self.subTest(name=name):
                with tempfile.TemporaryDirectory() as output_dir:
                    args = [arg.format(output_dir) for arg in args]
                    with RequiredValidArgs() as required_args:
                        sys.argv = required_args + args
                        _, result_parser = opc.parse_args()

                    self.assertFalse(result_parser.check_input_params())

    def test_options_parse_24(self):
        """
        test valid core_type, aicore_num, mdl_bank_path, op_bank_path, impl_mode
        """
        logger.info(
            "Start to execute ==============test_options_parse_24=============="
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        path = test_root_dir + "/output"
        input_param_path = test_root_dir + "/stub/files/matmul_test.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"

        sys.argv = [
            "xx",
            op_path,
            "--main_func=mat_mul",
            "--input_param=" + input_param_path,
            "--soc_version=Ascend910A",
            "--output=./output",
            "--log=debug",
            "--core_type=AiCore",
            "--aicore_num=1",
            "--impl_mode=high_performance",
            "--optional_input_mode=gen_placeholder",
        ]
        ret, opt_parser = opc.parse_args()
        self.assertEqual(ret, 0)
        opc.op_compile_classify(opt_parser)
        options = opt_parser.get_all_options()
        self.assertEqual(opt_parser.check_input_params(), True)

    # def test_op_compilation_config_file(self):
    #     """test test_op_compilation_config_file"""
    #     logger.debug("Start to execute ============ test_op_compilation_config_file ============")

    #     def del_files(file_path):
    #         for file_ in os.listdir(file_path):
    #             to_del_file = "{}/{}".format(file_path, file_)
    #             os.remove(to_del_file)

    #     test_root_dir = os.path.abspath(os.path.dirname(__file__))
    #     # make stub .o file
    #     with open(test_root_dir + "/debug_dir/kernel_meta_matmul_1/kernel_meta/matmul_1.o", "wb") as file_:
    #         pass
    #     input_param_path = test_root_dir + "/stub/files/matmul_1.json"
    #     opc_compile_args = {
    #         "input_param" : input_param_path,
    #         "soc_version" : "Ascend910A",
    #         "output" : test_root_dir + "/output",
    #         "debug_dir" : test_root_dir + "/debug_dir",
    #         "op_compile_classify": "single_op_compile_config_file_mode",
    #         "op_debug_level": 3,
    #         "op_debug_config": "dump_bin",
    #         "core_type" : "AiCore"
    #     }
    #     json_path = test_root_dir + "/stub/files/ascend910.json"
    #     load_set_op_content(json_path)
    #     op_compile = OpCompilation(opc_compile_args)
    #     ret = op_compile.op_compilation()
    #     self.assertEqual(ret, True)

    #     # delete stub .o file and outputs
    #     os.remove(test_root_dir + "/debug_dir/kernel_meta_matmul_1/kernel_meta/matmul_1.o")
    #     del_files(test_root_dir + "/output")

    #     logger.debug("End to execute ============ test_op_compilation_config_file ============")

    def test_check_op_optional_paramtype(self):
        """test check_op_optional_paramtype"""
        logger.debug(
            "Start to execute ============ test_check_op_optional_paramtype ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/apply_adam_v2.json"
        op_path = test_root_dir + "/stub/files/apply_adam_v2.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 3,
            "optional_input_mode": "gen_placeholder",
        }
        try:
            with open(input_param_path, "r") as file_handle:
                json_dict = json.load(file_handle)
        except Exception as e:
            logger.error("load file[%s] failed, reason: %s.", input_param_path, str(e))
            return False, None
        finally:
            pass
        ret = check_op_optional_paramtype(json_dict, opc_compile_args)
        self.assertEqual(ret, True)
        logger.debug(
            "End to execute ============ test_check_op_optional_paramtype ============"
        )

    def test_get_int64_mode(self):
        """test_get_int64_mode"""
        logger.debug("Start to execute ============ test_get_int64_mode ============")

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/ConcatD.json"
        with open(input_param_path, "r") as file_in:
            json_data = json.load(file_in)
        op_list = json_data.get("op_list")
        for op_operator in op_list:
            ret = get_int64_mode(op_operator.get("inputs"))
            self.assertEqual(ret, False)
            ret = get_int64_mode(op_operator.get("outputs"))
            self.assertEqual(ret, False)

        logger.debug("End to execute ============ test_get_int64_mode ============")

    def test_compile_op_api(self):
        """test_compile_op_api"""
        logger.debug("Start to execute ============ test_compile_op_api ============")

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/sqrt.json"
        output_dir = test_root_dir + "/output"
        debug_dir = test_root_dir + "/debug_dir"
        op_path = test_root_dir + "/stub/files/Sqrt.py"
        with open(input_param_path, "r") as file_in:
            json_data = json.load(file_in)

        build_options = {
            "main_func": "sqrt",
            "op_debug_level": 2,
            "op_path": op_path,
            "soc_version": "Ascend910A",
            "output": output_dir,
            "debug_dir": debug_dir,
            "optional_input_mode": "gen_placeholder",
            "bin_filename": "Sqrt",
        }
        res = opc_api.compile_op(json_data, build_options)
        self.assertEqual(res, True)

        os.remove(test_root_dir + "/debug_dir/kernel_meta_Sqrt/kernel_meta/Sqrt.o")
        logger.debug("End to execute ============ test_compile_op_api ============")

    def test_compile_op_api_fail(self):
        """test_compile_op_api"""
        logger.debug(
            "Start to execute ============ test_compile_op_api_fail ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul_no_op_type.json"
        output_dir = test_root_dir + "/output"
        debug_dir = test_root_dir + "/debug_dir"
        op_path = test_root_dir + "/stub/files/sqrt.py"
        with open(input_param_path, "r") as file_in:
            json_data = json.load(file_in)

        build_options = {
            "op_path": op_path,
            "soc_version": "Ascend910A",
            "output": output_dir,
            "debug_dir": debug_dir,
            "optional_input_mode": "gen_placeholder",
            "bin_filename": "ConcatD",
            "test": "Not exsit",
        }
        res = opc_api.compile_op(json_data, build_options)
        self.assertEqual(res, False)

        logger.debug(
            "End to execute ============ test_compile_op_api_fail ============"
        )


if __name__ == "__main__":
    unittest.main()
