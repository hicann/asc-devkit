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
from unittest.mock import patch, MagicMock
import os
from pathlib import Path
import json
import tempfile
import sys
import shutil

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
ASC_OPC_PATH = os.path.join(TOP_PATH, "tools/build/asc_opc/python/asc_opc_tool/")
OPC_STUB_PATH = os.path.join(TOP_PATH, "tests/python/asc_opc/stub/")
sys.path.insert(0, FRAMEWORK_PATH)
sys.path.insert(0, ASC_OPC_PATH)
sys.path.insert(0, OPC_STUB_PATH)

import asc_op_compile_base.common.utils.log as logger
from asc_op_compile_base.common.context import op_context

from constant import OpcCompileMode
from op_compilation import OpCompilation, OpcOptions, CompileParam
from post_compile_base import PostCompilation
from single_op_post_compile import SingleOpPostCompile
from single_op_compile import SingleOpCompile
from op_info_store import SubOpInfoStore, load_op_info_store, load_set_op_content
from opc_common import (
    update_compile_info,
    normalize_optional_impl_mode,
    get_new_attrs_for_op_compile,
)
from op_manager import get_core_type_from_op_content
from op_compile_info_check import check_op_compilation_json, check_op_compilation_dict


class TestOpCompilationUt(unittest.TestCase):
    def set_up(self):
        pass

    def tear_down(self):
        pass

    def del_files(self, file_path):
        if not os.path.isdir(file_path):
            logger.error("file_path[%s] not exist.", file_path)
            return

        for file in os.listdir(file_path):
            to_del_file = "{}/{}".format(file_path, file)
            try:
                os.remove(to_del_file)
                logger.info("Deleta file[%s] success", to_del_file)
            except Exception as e:
                logger.error("Delete file[%s] field, reason: %s.", to_del_file, str(e))
            finally:
                pass

    @classmethod
    def tearDownClass(cls):
        # it will copy files to debug_dir/kernel_meta_matmul_custom_simplified_key, debug_dir/kernel_meta_matmul_1
        # and debug_dir/kernel_meta_matmul_2 when use /stub/files/MatMul.json as input_param, and they will be used
        # in other cases, so delete the files in the end of tests
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        if os.path.exists(
            test_root_dir + "/debug_dir/kernel_meta_matmul_1/kernel_meta/matmul_1.o"
        ):
            os.remove(
                test_root_dir + "/debug_dir/kernel_meta_matmul_1/kernel_meta/matmul_1.o"
            )
        if os.path.exists(
            test_root_dir + "/debug_dir/kernel_meta_matmul_2/kernel_meta/matmul_2.o"
        ):
            os.remove(
                test_root_dir + "/debug_dir/kernel_meta_matmul_2/kernel_meta/matmul_2.o"
            )
        if os.path.exists(
            test_root_dir + "/debug_dir/kernel_meta_matmul_custom_simplified_key"
        ):
            shutil.rmtree(
                test_root_dir + "/debug_dir/kernel_meta_matmul_custom_simplified_key"
            )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_00_op_compilation_ut(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_00_op_compilation_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        json_path = test_root_dir + "/stub/files/ascend910.json"
        with open(json_path, "r") as file_in:
            op_builtin_info_dict = json.load(file_in)
        SubOpInfoStore().set_op_content(op_builtin_info_dict)

        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        # if opc process success, op stub func(matmul) will copy json and o file from stub file dir to debug_dir dir,
        # opc will copy json and o file from debug_dir dir to output dir
        # opc will finally delete file in output dir
        ret = op_compile.op_compilation()

        # if ret is false, it means opc process failed and throw exceptions. Please check if your codes are correct
        self.assertEqual(ret, True)

        # execute again, output dir has json files, opc process will check file exist
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        ret, json_dict = check_op_compilation_json(
            OpcOptions.INPUT_PARAM, opc_compile_args
        )
        self.assertEqual(ret, True)
        ret = op_compile.check_and_update_core_type(
            OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE, json_dict
        )
        self.assertEqual(ret, True)
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")
        opc_compile_args["input_param"] = "./not_exist.json"
        ret, _ = check_op_compilation_json(OpcOptions.INPUT_PARAM, opc_compile_args)
        self.assertEqual(ret, False)
        op_params_tmp = ["a", "b"]
        ret, _ = check_op_compilation_dict(op_params_tmp, opc_compile_args)
        self.assertEqual(ret, False)
        logger.debug(
            "End to execute ============ test_00_op_compilation_ut ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_01_op_compilation_st(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_01_op_compilation_st ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/sqrt.json"
        op_path = test_root_dir + "/stub/files/Sqrt.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "sqrt",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "AiCore",
            "impl_mode": "high_precision, optional",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        opc_compile_args["impl_mode"] = ""
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        logger.debug("Testcase delete files in output dir and debug dir")
        self.del_files(test_root_dir + "/output")
        os.remove(test_root_dir + "/debug_dir/kernel_meta_Sqrt/kernel_meta/Sqrt.o")
        logger.debug(
            "End to execute ============ test_01_op_compilation_st ============"
        )

    # @patch('asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info')
    # def test_01_op_compilation(self, mock_set_current_compile_soc_info):
    #     mock_set_current_compile_soc_info.return_value = "success"
    #     """test op_compilation"""
    #     logger.debug("Start to execute ============ test_01_op_compilation ============")

    #     test_root_dir = os.path.abspath(os.path.dirname(__file__))
    #     input_param_path = test_root_dir + "/stub/files/matmul_1.json"
    #     op_path = test_root_dir + "/stub/files/MatMul.py"
    #     json_path = test_root_dir + "/stub/files/ascend910.json"
    #     with open(json_path, "r") as file_in:
    #         op_builtin_info_dict = json.load(file_in)
    #     SubOpInfoStore().set_op_content(op_builtin_info_dict)

    #     opc_compile_args = {
    #         "op_path" : op_path,
    #         "input_param" : input_param_path,
    #         "soc_version" : "Ascend910A",
    #         "output" : test_root_dir + "/output",
    #         "debug_dir" : test_root_dir + "/debug_dir",
    #         "op_compile_classify": "single_op_compile_config_file_mode",
    #         "op_debug_level": 3,
    #         "simplified_key_mode" : 0,
    #         "core_type" : "AiCore"
    #     }
    #     op_compile = OpCompilation(opc_compile_args)
    #     ret = op_compile.op_compilation()
    #     self.assertEqual(ret, True)
    #     self.del_files(test_root_dir + "/output")
    #     logger.debug("End to execute ============ test_01_op_compilation ============")

    # @patch('asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info')
    # def test_02_op_compilation(self, mock_set_current_compile_soc_info):
    #     mock_set_current_compile_soc_info.return_value = "success"
    #     """test op_compilation"""
    #     logger.debug("Start to execute ============ test_02_op_compilation ============")

    #     test_root_dir = os.path.abspath(os.path.dirname(__file__))
    #     input_param_path = test_root_dir + "/stub/files/matmul_1.json"
    #     op_path = test_root_dir + "/stub/files/MatMul.py"
    #     json_path = test_root_dir + "/stub/files/ascend910.json"
    #     with open(json_path, "r") as file_in:
    #         op_builtin_info_dict = json.load(file_in)
    #     SubOpInfoStore().set_op_content(op_builtin_info_dict)

    #     opc_compile_args = {
    #         "op_path" : op_path,
    #         "input_param" : input_param_path,
    #         "soc_version" : "Ascend910A",
    #         "output" : test_root_dir + "/output",
    #         "debug_dir" : test_root_dir + "/debug_dir",
    #         "op_compile_classify": "single_op_compile_config_file_mode",
    #         "op_debug_level": 3,
    #         "simplified_key_mode" : 0,
    #         "optional_input_mode": "gen_placeholder",
    #         "dynamic_param_mode": "folded_with_desc"
    #     }
    #     op_compile = OpCompilation(opc_compile_args)
    #     ret = op_compile.op_compilation()
    #     self.assertEqual(ret, True)
    #     self.del_files(test_root_dir + "/output")
    #     logger.debug("End to execute ============ test_02_op_compilation ============")

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_03_op_compilation(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test test_03_op_compilation_ut"""
        logger.debug(
            "Start to execute ============ test_03_op_compilation_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        json_path = test_root_dir + "/stub/files/ascend910.json"
        with open(json_path, "r") as file_in:
            op_builtin_info_dict = json.load(file_in)
        SubOpInfoStore().set_op_content(op_builtin_info_dict)

        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "optional,high_performance",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
            "simplified_key_mode": 0,
        }
        op_compile = OpCompilation(opc_compile_args)
        # if opc process success, op stub func(matmul) will copy json and o file from stub file dir to debug_dir dir,
        # opc will copy json and o file from debug_dir dir to output dir
        # opc will finally delete file in output dir
        ret = op_compile.op_compilation()

        # if ret is false, it means opc process failed and throw exceptions. Please check if your codes are correct
        self.assertEqual(ret, True)
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")
        logger.debug(
            "End to execute ============ test_03_op_compilation_ut ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_check_and_update_core_type(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        logger.debug(
            "Start to execute ============ test_check_and_update_core_type ============"
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul_no_op_type.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": None,
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)
        ret, json_dict = check_op_compilation_json(
            OpcOptions.INPUT_PARAM, opc_compile_args
        )
        self.assertEqual(ret, True)
        ret = op_compile.check_and_update_core_type(
            OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE, json_dict
        )
        self.assertEqual(ret, False)

        input_param_path = test_root_dir + "/stub/files/MatMul.json"
        opc_compile_args["input_param"] = input_param_path
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        json_path = test_root_dir + "/stub/files/ascend910.json"
        load_set_op_content(json_path)
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")
        ret, json_dict = check_op_compilation_json(
            OpcOptions.INPUT_PARAM, opc_compile_args
        )
        self.assertEqual(ret, True)
        ret = op_compile.check_and_update_core_type(
            OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE, json_dict
        )
        self.assertEqual(ret, True)

        ret = get_core_type_from_op_content("")
        self.assertEqual(ret, None)
        ret = get_core_type_from_op_content("MatMul")
        self.assertEqual(ret, "AiCore,VectorCore")
        ret = get_core_type_from_op_content("Add")
        self.assertEqual(ret, None)
        logger.debug(
            "End to execute ============ test_check_and_update_core_type ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_01_op_compilation_json_file_error_ut(
        self, mock_set_current_compile_soc_info
    ):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation json_file_error"""
        logger.debug(
            "Start to execute ============ test_01_op_compilation_json_file_error_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = (
            test_root_dir + "/stub/files/matmul_test.json"
        )  # json file is none, json load fail
        op_path = test_root_dir + "/stub/files/MatMul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "op_compile_classify": "single_op_compile_config_file_mode",
        }
        op_compile = OpCompilation(opc_compile_args)

        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)

        input_param_path = (
            test_root_dir + "/stub/files/matmul_test_notdict.json"
        )  # json file not dict
        opc_compile_args["input_param"] = input_param_path
        op_compile = OpCompilation(opc_compile_args)

        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)

        input_param_path = (
            test_root_dir + "/stub/files/MatMul_build_res.json"
        )  # json file has no op_list
        opc_compile_args["input_param"] = input_param_path
        op_compile = OpCompilation(opc_compile_args)

        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)

        logger.debug(
            "End to execute ============ test_01_op_compilation_json_file_error_ut ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_02_op_compilation_mode64_ut(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation mode 64: large shape"""
        logger.debug(
            "Start to execute ============ test_02_op_compilation_mode64_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul_mode64.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "op_compile_classify": "single_op_compile_config_file_mode",
        }
        op_compile = OpCompilation(opc_compile_args)
        # if opc process success, op stub func(matmul) will copy json and o file from stub file dir to debug_dir dir,
        # opc will copy json and o file from debug_dir dir to output dir
        # opc will finally delete file in output dir
        ret = op_compile.op_compilation()

        # if ret is false, it means opc process failed and throw exceptions. Please check if your codes are correct
        self.assertEqual(ret, True)

        # delete file in output
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")

        logger.debug(
            "End to execute ============ test_02_op_compilation_mode64_ut ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_op_compilation_input_options_ut(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation input options mode"""
        logger.debug(
            "Start to execute ============ test_op_compilation_input_options_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/apply_adam_v2.json"
        op_path = test_root_dir + "/stub/files/apply_adam_v2.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "apply_adam_v2",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "optional_input_mode": "gen_placeholder",
            "op_compile_classify": "single_op_compile_config_file_mode",
        }
        json_path = test_root_dir + "/stub/files/ascend910.json"
        with open(json_path, "r") as file_in:
            op_builtin_info_dict = json.load(file_in)
        SubOpInfoStore().set_op_content(op_builtin_info_dict)

        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)

        # delete file in output
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")

        logger.debug(
            "End to execute ============ test_op_compilation_input_options_ut ============"
        )

    def test_03_record_compile_error_info_ut(self):
        """test record_compile_error_info"""
        logger.debug(
            "Start to execute ============ test_03_record_compile_error_info_ut ============"
        )
        op_info = {"op_type": "matmul", "kernel_name": "kernel_namexxx"}
        idx = 1
        op = {"comment": "ND_float16 with attr = true", "inputs": "inputs"}
        error_info = "errorinfo"
        OpCompilation.record_compile_error_info(op_info, idx, op, error_info)

        logger.debug(
            "End to execute ============ test_03_record_compile_error_info_ut ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_04_create_kernel_meta_dir_ut(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_04_create_kernel_meta_dir_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_3.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 2,
        }
        op_compile = OpCompilation(opc_compile_args)
        # if opc process success, op stub func(matmul) will copy json and o file from stub file dir to debug_dir dir,
        # opc will copy json and o file from debug_dir dir to output dir
        # opc will finally delete file in output dir
        ret = op_compile.op_compilation()

        # try to test create kernel_mata_xxx dir by opc, so we can not stub the file under the kernel_meta dir
        # There will be a runtime error while copying file from kernel meta dir to output dir
        self.assertEqual(ret, True)

        # delete file in output and debug_dir
        logger.debug("Testcase delete files in output dir and debug dir")
        self.del_files(test_root_dir + "/output")
        shutil.rmtree(test_root_dir + "/debug_dir/kernel_meta_matmul_3")

        logger.debug(
            "End to execute ============ test_04_create_kernel_meta_dir_ut ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_05_verify_kernel_meta_lock_ut(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_05_verify_kernel_meta_lock_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/matmul_4.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 2,
        }
        json_path = test_root_dir + "/stub/files/ascend910.json"
        load_set_op_content(json_path)
        op_compile = OpCompilation(opc_compile_args)
        # if opc process success, op stub func(matmul) will copy json and o file from stub file dir to debug_dir dir,
        # opc will copy json and o file from debug_dir dir to output dir
        # opc will finally delete file in output dir
        ret = op_compile.op_compilation()

        # try to test create kernel_mata_xxx dir by opc, so we can not stub the file under the kernel_meta dir
        # There will be a runtime error while copying file from kernel meta dir to output dir
        self.assertEqual(ret, True)

        # delete file in output and debug dir
        logger.debug("Testcase delete files in output dir and debug dir")
        self.del_files(test_root_dir + "/output")
        shutil.rmtree(test_root_dir + "/debug_dir/kernel_meta_matmul_4")

        logger.debug(
            "End to execute ============ test_05_verify_kernel_meta_lock_ut ============"
        )

    def test_06_verify_opc_common_update_compile_info_ut(self):
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_06_verify_opc_common_update_compile_info_ut ============"
        )

        test_file_dir = os.path.abspath(os.path.dirname(__file__))
        test_root_dir = os.path.abspath(os.path.join(test_file_dir, "../.."))
        input_param_path = test_root_dir + "/stub/files/matmul_4.json"
        op_path = test_root_dir + "/stub/files/MatMul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir
            + "/output",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "debug_dir": test_root_dir
            + "/debug_dir",  # path will be used in llt\atc\opcompiler\opc\stub\files\MatMul.py
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "high_performace",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 2,
        }

        test_debug_dir = test_root_dir + "/debug_dir/kernel_meta/testSupportInfo.json"
        update_compile_info(None, opc_compile_args)
        update_compile_info(test_debug_dir, opc_compile_args)
        logger.debug(
            "End to execute ============ test_06_verify_opc_common_update_compile_info_ut ============"
        )

    def test_construct_op_kernel_info_ut(self):
        logger.debug(
            "Start to execute ============ test_construct_op_kernel_info_ut ============"
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        json_path = test_root_dir + "/stub/files/ascend910.json"
        with open(json_path, "r") as file_in:
            op_builtin_info_dict = json.load(file_in)
        SubOpInfoStore().set_op_content(op_builtin_info_dict)
        op_type = "GNTrainingReduce"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, True)
        op_type = "GIoU"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, True)
        op_type = "FusedMulAdd"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, True)
        op_type = "FresnelSin"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, True)
        op_type = "Fills"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, True)
        op_type = "Fills_1"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, True)
        op_type = "Fills_2"
        ret = SubOpInfoStore().construct_op_kernel_info(op_type)
        self.assertEqual(ret, False)
        logger.debug(
            "End to execute ============ test_construct_op_kernel_info_ut ============"
        )

    # def test_get_single_op_operator_ut(self):
    #     logger.debug("Start to excute ============ test_get_single_op_operator_ut ============")
    #     test_root_dir = os.path.abspath(os.path.dirname(__file__))
    #     ascend_path = test_root_dir + "/stub"
    #     os.environ["ASCEND_OPP_PATH"] = ascend_path

    #     with unittest.mock.patch('asc_op_compile_base.common.register.get_operator') as mock_get_operator:
    #         def mock_get_operator_from_ops(key):
    #             def conv2d(*args, **kwargs):
    #                 return "conv2d"

    #             def fresnel_cos(*args, **kwargs):
    #                 return "fresnel_cos"
    #             operators = {
    #                 "conv2d": Operator(None, conv2d),
    #                 "fresnel_cos": Operator(None, fresnel_cos),
    #             }
    #             if key in operators.keys():
    #                 return operators[key]
    #             else:
    #                 return None
    #         mock_get_operator.side_effect = mock_get_operator_from_ops
    #         op_type = "conv2d"
    #         ret = get_single_op_operator(op_type, "true", True)
    #         self.assertEqual(ret.__name__, "conv2d")
    #         op_type = "fresnel_cos"
    #         res = get_single_op_operator(op_type, "true", True)
    #         self.assertEqual(res.__name__, "fresnel_cos")
    #     op_type = "asdsddd"
    #     res = get_single_op_operator(op_type, "false", True)
    #     self.assertEqual(res, None)
    #     logger.debug("End to excute ============ test_get_single_op_operator_ut ============")

    # @patch('asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info')
    # def test_get_custom_op_operator_ut(self, mock_set_current_compile_soc_info):
    #     mock_set_current_compile_soc_info.return_value = "success"
    #     logger.debug("Start to excute ============ test_get_custom_op_operator_ut ============")
    #     test_root_dir = os.path.abspath(os.path.dirname(__file__))
    #     custom_path = test_root_dir + "/stub/vendors/custom"
    #     os.environ["ASCEND_CUSTOM_OPP_PATH"] = custom_path

    #     with unittest.mock.patch('asc_op_compile_base.common.platform.platform_info.get_soc_spec') as mock_get_soc_spec:
    #         mock_get_soc_spec.return_value = "Ascend310"
    #         load_op_info_store("Ascend310")
    #     with unittest.mock.patch('asc_op_compile_base.common.register.get_operator') as mock_get_operator:
    #         def mock_get_operator_from_ops(key):
    #             def conv2d(*args, **kwargs):
    #                 return "conv2d"

    #             def fresnel_cos(*args, **kwargs):
    #                 return "fresnel_cos"

    #             def flash_attentions_score(*args, **kwargs):
    #                 return "FlashAttentionScore"
    #             operators = {
    #                 "conv2d": Operator(None, conv2d),
    #                 "fresnel_cos": Operator(None, fresnel_cos),
    #                 "FlashAttentionScore": Operator(None, flash_attentions_score),
    #             }
    #             if key in operators.keys():
    #                 return operators[key]
    #             else:
    #                 return None
    #         mock_get_operator.side_effect = mock_get_operator_from_ops
    #         op_type = "conv2d"
    #         ret = get_single_op_operator(op_type, "true", True)
    #         self.assertEqual(ret.__name__, "conv2d")
    #         op_type = "fresnel_cos"
    #         res = get_single_op_operator(op_type, "true", True)
    #         self.assertEqual(res.__name__, "fresnel_cos")
    #         op_type = "FlashAttentionScore"
    #         res = get_single_op_operator(op_type, "true", True)
    #         self.assertEqual(res.__name__, "flash_attentions_score")
    #     op_type = "asdsddd"
    #     res = get_single_op_operator(op_type, "false", True)
    #     self.assertEqual(res, None)
    #     logger.debug("End to excute ============ test_get_custom_op_operator_ut ============")

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    @patch("asc_op_compile_base.common.platform.platform_info.get_soc_spec")
    def test_get_vendor_and_custom_path_ut(
        self, mock_set_current_compile_soc_info, mock_get_soc_spec
    ):
        mock_set_current_compile_soc_info.return_value = "success"
        mock_get_soc_spec.return_value = "Ascend910"
        logger.debug(
            "Start to excute ============ test_get_vendor_and_custom_path_ut ============"
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        ascend_path = test_root_dir + "/stub"
        os.environ["ASCEND_OPP_PATH"] = ascend_path
        config_path = "{}/vendors/config.ini".format(ascend_path)
        with open(config_path, "r", encoding="utf-8") as file:
            content = file.read()
        new_content = content.replace("custom,mdc", "vendor")
        with open(config_path, "w", encoding="utf-8") as file:
            file.write(new_content)
        load_op_info_store("Ascend910")
        with open(config_path, "r", encoding="utf-8") as file:
            content = file.read()
        new_content = content.replace("vendor", "custom,mdc")
        with open(config_path, "w", encoding="utf-8") as file:
            file.write(new_content)
        logger.debug(
            "End to excute ============ test_get_vendor_and_custom_path_ut ============"
        )

    def test_get_new_attrs_for_op_compile(self):
        logger.debug(
            "Start to execute ============ test_get_new_attrs_for_op_compile ============"
        )
        attrs = [
            {
                "name": "_input_name_key",
                "dtype": "list_str",
                "value": ["bias", "filter", "offset_w", "x"],
            },
            {"name": "_input_name_value", "dtype": "list_int", "value": [2, 1, 3, 0]},
            {"name": "_opt_input", "dtype": "list_str", "value": ["bias", "offset_w"]},
            {"name": "_output_name_key", "dtype": "list_str", "value": ["y"]},
            {"name": "_output_name_value", "dtype": "list_int", "value": [0]},
            {"name": "data_format", "dtype": "str", "value": "NHWC"},
            {"name": "dilations", "dtype": "list_int", "value": [1, 1, 1, 1]},
            {"name": "groups", "dtype": "int", "value": 1},
            {
                "name": "is_input_const",
                "dtype": "list_bool",
                "value": [False, True, True],
            },
            {"name": "offset_x", "dtype": "int", "value": 1},
            {"name": "pads", "dtype": "list_int", "value": [0, 0, 0, 0]},
            {"name": "strides", "dtype": "list_int", "value": [1, 1, 1, 1]},
            {
                "name": "var_attrs",
                "dtype": "list_str",
                "value": ["strides", "dilations", "groups", "data_format", "offset_x"],
            },
        ]

        def op_func(
            inputs,
            weights,
            bias,
            offset_w,
            outputs,
            strides,
            pads,
            dilations,
            groups=1,
            data_format="NHWC",
            offset_x=0,
            kernel_name="conv2d",
            dsl_flag=True,
        ):
            pass

        op_node = {"attrs": attrs}
        res = get_new_attrs_for_op_compile(
            op_node, op_func, "single_op_compile_graph_mode"
        )
        expect_res = [None, [0, 0, 0, 0], None, None, None, None]
        self.assertEqual(res, expect_res)
        logger.debug(
            "End to execute ============ test_get_new_attrs_for_op_compile ============"
        )

    def test_SingleOpCompile_will_trans_deterministic_option_to_kwargs(self):
        with unittest.mock.patch("single_op_compile.build_config") as build_config:
            test_op_func = unittest.mock.MagicMock()

            op_info = {
                OpcOptions.DETERMINISTIC: "true",
                OpcOptions.OP_FUNC_ATTR: test_op_func,
                CompileParam.INPUTS: [],
                CompileParam.OUTPUTS: [],
            }

            op = {
                "bin_filename": "Add_3",
                "inputs": [
                    {
                        "name": "x1",
                        "index": 0,
                        "dtype": "float16",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [-2],
                    },
                    {
                        "name": "x2",
                        "index": 1,
                        "dtype": "float16",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [-2],
                    },
                ],
                "outputs": [
                    {
                        "name": "y",
                        "index": 0,
                        "dtype": "float16",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [-2],
                    }
                ],
            }

            SingleOpCompile(op, op_info, {}).op_compile()

            test_op_func.assert_called_once()
            build_config.assert_called_once()
            assert build_config.call_args[1]["enable_deterministic_mode"] is True

    def test_single_op_compile_passes_kernel_spec_to_context(self):
        cases = (
            ("Normal", False, False),
            ("SK", False, True),
            ("None", True, True),
        )
        for mode, relocatable, expected_enable_sk in cases:
            with self.subTest(mode=mode), tempfile.TemporaryDirectory() as output_dir:
                captured_context = {}

                def op_func(*args, **kwargs):
                    context = op_context.get_context()
                    captured_context["mode"] = context.get_addition("kernel_spec_mode")
                    captured_context["dir"] = context.get_addition("kernel_spec_dir")
                    captured_context["sk_sub_combine"] = context.get_addition(
                        "super_kernel_sub_combine"
                    )

                op = {
                    "bin_filename": "Add_3",
                    "inputs": [],
                    "outputs": [],
                }
                op_info = {
                    OpcOptions.OP_FUNC_ATTR: op_func,
                    OpcOptions.IS_DYNAMIC: "true",
                    CompileParam.INPUTS: [],
                    CompileParam.OUTPUTS: [],
                }
                compile_args = {
                    OpcOptions.KERNEL_SPEC: mode,
                    OpcOptions.KERNEL_SPEC_DIR: output_dir,
                    OpcOptions.RELOCATABLE_BIN: relocatable,
                }

                with patch("single_op_compile.build_config") as build_config:
                    SingleOpCompile(op, op_info, compile_args).op_compile()

                self.assertEqual(captured_context["mode"], mode)
                self.assertEqual(captured_context["dir"], output_dir)
                self.assertIs(
                    captured_context["sk_sub_combine"],
                    True if mode == "SK" else None,
                )
                self.assertIs(
                    build_config.call_args.kwargs["enable_super_kernel"],
                    expected_enable_sk,
                )

    def test_kernel_spec_sk_adds_default_sub_super_kernel_info(self):
        compiler = SingleOpCompile(
            {},
            {},
            {
                OpcOptions.KERNEL_SPEC: "SK",
                OpcOptions.SPK_OPT: "early-start=1",
            },
        )
        with op_context.OpContext() as context:
            compiler.set_kernel_spec_for_context(context)
            self.assertEqual(
                context.get_addition("super_kernel_sub_info"),
                {
                    "enable_super_kernel": True,
                    "super_kernel_options": "early-start=1",
                    "super_kernel_sub_loc": "middle",
                },
            )

    def test_kernel_spec_sk_keeps_input_sub_super_kernel_info(self):
        compiler = SingleOpCompile({}, {}, {OpcOptions.KERNEL_SPEC: "SK"})
        expected_info = {
            "enable_super_kernel": True,
            "super_kernel_options": "early-start=0",
            "super_kernel_sub_loc": "start",
        }
        with op_context.OpContext() as context:
            context.add_addition("super_kernel_sub_info", expected_info)
            compiler.set_kernel_spec_for_context(context)
            self.assertIs(context.get_addition("super_kernel_sub_info"), expected_info)

    def test_SingleOpCompile_will_jit_compile_option_to_kwargs(self):
        with unittest.mock.patch("single_op_compile.build_config") as build_config:
            test_op_func = unittest.mock.MagicMock()

            op = {
                "bin_filename": "Add_3",
                "inputs": [
                    {
                        "name": "x1",
                        "index": 0,
                        "dtype": "float16",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [-2],
                    },
                    {
                        "name": "x2",
                        "index": 1,
                        "dtype": "float16",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [-2],
                    },
                ],
                "outputs": [
                    {
                        "name": "y",
                        "index": 0,
                        "dtype": "float16",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [-2],
                    }
                ],
            }

            op_info = {
                OpcOptions.DETERMINISTIC: "true",
                OpcOptions.OP_FUNC_ATTR: test_op_func,
                OpcOptions.IS_DYNAMIC: "true",
                OpcOptions.JIT_COMPILE_MODE: 2,
                CompileParam.INPUTS: [],
                CompileParam.OUTPUTS: [],
            }

            SingleOpCompile(op, op_info, {}).op_compile()

            test_op_func.assert_called_once()
            build_config.assert_called_once()

    def test_OpCompilation_will_trans_deterministic_arg_to_op_info(self):
        test_bin_name = "test"
        test_output = tempfile.TemporaryDirectory()

        opc_compile_args = {
            "soc_version": "Ascend910A",
            "op_compile_classify": "single_op_compile_config_file_mode",
            OpcOptions.DETERMINISTIC: "true",
            OpcOptions.IS_DYNAMIC: "false",
            OpcOptions.CORE_TYPE: "AiCore",
            OpcOptions.OUTPUT: test_output.name,
            OpcOptions.DEBUG_DIR: test_output.name,
        }

        op_json = {
            CompileParam.TYPE: "mat_mul",
            CompileParam.OP_LIST: [
                {
                    OpcOptions.BIN_FILENAME: test_bin_name,
                    CompileParam.INPUTS: [],
                    CompileParam.OUTPUTS: [],
                }
            ],
        }

        with patch("op_compilation.SingleOpCompile") as patchSingleOpCompile:
            with patch(
                "op_compilation.get_single_op_operator"
            ) as patch_get_single_op_operator:
                # 由于算子实际不存在，通过patch返回模拟的算子脚本main_func
                patch_get_single_op_operator.return_value = MagicMock()

                def create_op_bin():
                    # 模拟算子生成输出文件，从而使编译成功
                    meta_dir = test_output.name + "/kernel_meta_test/kernel_meta/"
                    os.mkdir(meta_dir)

                    open(meta_dir + test_bin_name + ".o", "w")
                    with open(meta_dir + test_bin_name + ".json", "w") as f:
                        f.write("{}")

                    return meta_dir + test_bin_name + ".json"

                patchSingleOpCompile.return_value = MagicMock()
                patchSingleOpCompile.return_value.op_compile = MagicMock(
                    side_effect=create_op_bin
                )

                test_op_compilation = OpCompilation(opc_compile_args)
                test_op_compilation.single_op_compilation(op_json)

                assert patchSingleOpCompile.call_args.args[1]["deterministic"] == "true"

        del test_output

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    @patch("asc_op_compile_base.common.platform.platform_info.get_soc_spec")
    def test_load_op_info(self, mock_set_current_compile_soc_info, mock_get_soc_spec):
        mock_set_current_compile_soc_info.return_value = "success"
        mock_get_soc_spec.return_value = "Ascend910"
        load_op_info_store("Ascend910")
        res = normalize_optional_impl_mode("high_performance, optional")
        self.assertEqual(res, "high_performance")

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_implmode_op_compilation_ut_01(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_implmode_op_compilation_ut_01 ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/sqrt.json"
        op_path = test_root_dir + "/stub/files/Sqrt.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "sqrt",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "high_performance,optional",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        # delete file in output and debug_dir
        logger.debug("Testcase delete files in output dir and debug dir")
        self.del_files(test_root_dir + "/output")
        os.remove(test_root_dir + "/debug_dir/kernel_meta_Sqrt/kernel_meta/Sqrt.o")
        logger.debug(
            "End to execute ============ test_implmode_op_compilation_ut_01 ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_implmode_op_compilation_ut_02(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_implmode_op_compilation_ut_02 ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul.json"
        op_path = test_root_dir + "/stub/files/mat_mul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "high_performance,optional",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, True)
        # delete file in output
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")
        logger.debug(
            "End to execute ============ test_implmode_op_compilation_ut_02 ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_implmode_op_compilation_st_03(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_implmode_op_compilation_ut_03 ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/sqrt.json"
        op_path = test_root_dir + "/stub/files/aaaaaaaaaaa.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "sqrt",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "high_performance,optional",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)
        # delete file in output
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")
        logger.debug(
            "End to execute ============ test_implmode_op_compilation_ut_03 ============"
        )

    @patch(
        "asc_op_compile_base.common.platform.platform_info.set_current_compile_soc_info"
    )
    def test_implmode_op_compilation_ut_04(self, mock_set_current_compile_soc_info):
        mock_set_current_compile_soc_info.return_value = "success"
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_implmode_op_compilation_ut_04 ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        input_param_path = test_root_dir + "/stub/files/MatMul.json"
        op_path = test_root_dir + "/stub/files/impl/dynamic/mat_mul.py"
        opc_compile_args = {
            "op_path": op_path,
            "input_param": input_param_path,
            "main_func": "mat_mul",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "debug_dir": test_root_dir + "/debug_dir",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "high_performance,optional",
            "op_compile_classify": "single_op_compile_config_file_mode",
            "op_debug_level": 1,
        }
        op_compile = OpCompilation(opc_compile_args)
        ret = op_compile.op_compilation()
        self.assertEqual(ret, False)
        # delete file in output
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")

        op_path1 = "mat_mul.py"
        opc_compile_args["op_path"] = op_path1
        op_compile1 = OpCompilation(opc_compile_args)
        ret = op_compile1.op_compilation()
        self.assertEqual(ret, False)
        # delete file in output
        logger.debug("Testcase delete files in output dir")
        self.del_files(test_root_dir + "/output")

        logger.debug(
            "End to execute ============ test_implmode_op_compilation_ut_04 ============"
        )

    def test_copy_compile_res_files_to_output_ut(self):
        """test op_compilation"""
        logger.debug(
            "Start to execute ============ test_copy_compile_res_files_to_output_ut ============"
        )

        test_root_dir = os.path.abspath(os.path.dirname(__file__))

        output_dir = str(test_root_dir) + "/stub/files/kernel_meta/"

        op_path = str(test_root_dir) + "/stub/files/matmul_add_1.json"

        opc_compile_args = {
            "debug_dir": str(test_root_dir) + "/debug_dir",
            "bin_filename": "matmul_add_1",
            "soc_version": "Ascend910B",
            "output": output_dir,
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "impl_mode": "high_performance, optional",
        }
        l1_fusion_flag = "false"
        l2_fusion_flag = "false"
        l2_mode_flag = "0"
        post_compilation = PostCompilation(
            opc_compile_args, l1_fusion_flag, l2_fusion_flag, l2_mode_flag
        )

        ret = post_compilation.copy_compile_res_files_to_output(op_path)

        check_file_suffix = [
            ".json",
            "_mix_aic.json",
            "_mix_aic.txt",
            "_mix_aiv.json",
            "_mix_aiv.txt",
        ]
        for suffix in check_file_suffix:
            self.assertEqual(
                os.path.exists(output_dir + "matmul_add_1{}".format(suffix)), True
            )
        for suffix in check_file_suffix:
            os.remove(output_dir + "matmul_add_1{}".format(suffix))

        logger.debug(
            "End to execute ============ test_copy_compile_res_files_to_output_ut ============"
        )

    def test_check_simplifiedkey_ut(self):
        logger.debug(
            "Start to execute ============ test_check_simplifiedkey_ut ============"
        )
        test_root_dir = os.path.abspath(os.path.dirname(__file__))
        test_debug_dir = test_root_dir + "/debug_dir/kernel_meta/testSupportInfo.json"
        opc_compile_args = {
            "debug_dir": test_root_dir + "/debug_dir",
            "bin_filename": "add12343",
            "soc_version": "Ascend910A",
            "output": test_root_dir + "/output",
            "h": "not_exist",
            "log": "not_exist",
            "core_type": "VectorCore",
            "optional_input_mode": "no_placeholder",
            "impl_mode": "high_performance, optional",
            "simplified_key_mode": 1,
        }
        fusion_impl_mode = dict()
        fusion_impl_mode["Conv2d"] = ("high_performance", True)
        l1_fusion_flag = "false"
        l2_fusion_flag = "false"
        l2_mode_flag = "0"
        single_post_compilation = SingleOpPostCompile(
            opc_compile_args,
            l1_fusion_flag,
            l2_fusion_flag,
            l2_mode_flag,
            fusion_impl_mode,
        )
        op = {
            "op_type": "Add",
            "kernel_name": "add12343",
            "kernel_meta_path": test_debug_dir,
            "inputs": [
                {
                    "name": "x1",
                    "index": 0,
                    "dtype": "float16",
                    "dtypeForBinQuery": ["float16", "float32"],
                    "format": "FRACTAL_NZ",
                    "formatForBinQuery": ["ND", "NCHW"],
                    "shape": [-2],
                },
                None,
            ],
            "outputs": [
                {
                    "name": "x1",
                    "index": 0,
                    "dtype": "float16",
                    "format": "FRACTAL_NZ",
                    "shape": [-2],
                }
            ],
            "attrs": [{"name": "adj_x1", "dtype": "bool", "value": False}, None],
        }
        op_info = {"op_type": "Add", "op_func_attr": "add"}
        single_post_compilation.update_info_to_json_file(op, op_info, test_debug_dir)
        with open(test_debug_dir, "r") as output_json:
            out_json = json.load(output_json)
            simplifiedKey = out_json["supportInfo"]["simplifiedKey"]
            self.assertEqual(
                simplifiedKey, ["Add/d=0,p=0/1,2/,/1,29/0", "Add/d=0,p=0/0,0/,/1,29/0"]
            )

        del op["inputs"][0]["dtypeForBinQuery"]
        single_post_compilation.update_info_to_json_file(op, op_info, test_debug_dir)
        with open(test_debug_dir, "r") as output_json:
            out_json = json.load(output_json)
            simplifiedKey = out_json["supportInfo"]["simplifiedKey"]
            self.assertEqual(
                simplifiedKey, ["Add/d=0,p=0/1,2/,/1,29/0", "Add/d=0,p=0/1,0/,/1,29/0"]
            )

        del op["inputs"][0]["formatForBinQuery"]
        single_post_compilation.update_info_to_json_file(op, op_info, test_debug_dir)
        with open(test_debug_dir, "r") as output_json:
            out_json = json.load(output_json)
            simplifiedKey = out_json["supportInfo"]["simplifiedKey"]
            self.assertEqual(simplifiedKey, ["Add/d=0,p=0/1,29/,/1,29/0"])

        logger.debug(
            "End to execute ============ test_check_simplifiedkey_ut ============"
        )


class TestOpCompilation_WithDeterministic:
    """
    确定性算子编译测试套
    通过将基本的编译参数、对算子的打桩封装起来，
    从而逻辑尽可能的聚焦在compilation模块的处理
    """

    class TempOutputDir:
        def __init__(self, bin_name="test"):
            self._output_temp = tempfile.TemporaryDirectory()
            self.output_dir = self._output_temp.name

            self.meta_dir = self.output_dir + "/kernel_meta_test/kernel_meta/"
            self.bin_name = bin_name
            self.output_bin = self.meta_dir + self.bin_name

            self.compile_args = {
                OpcOptions.OUTPUT: self.output_dir,
                OpcOptions.DEBUG_DIR: self.output_dir,
            }

    class DefaultArg:
        def __init__(self):
            self.temp_output = TestOpCompilation_WithDeterministic.TempOutputDir()
            self.compile_args = {
                "soc_version": "Ascend910A",
                "op_compile_classify": "single_op_compile_config_file_mode",
                OpcOptions.IS_DYNAMIC: "false",
                "core_type": "AiCore",
                **self.temp_output.compile_args,
            }

            self.op_json = {
                CompileParam.TYPE: "mat_mul",
                CompileParam.OP_LIST: [
                    {
                        OpcOptions.BIN_FILENAME: self.temp_output.bin_name,
                        CompileParam.INPUTS: [],
                        CompileParam.OUTPUTS: [],
                    }
                ],
            }

    class MockSingleOpCompile:
        def __init__(self, will_output):
            self._patch_SingleOpCompile = patch("op_compilation.SingleOpCompile")

            self.patch_SingleOpCompile = self._patch_SingleOpCompile.start()
            self.patch_op_compile = MagicMock(side_effect=self.create_op_bin)

            self.patch_SingleOpCompile.return_value = MagicMock()
            self.patch_SingleOpCompile.return_value.op_compile = self.patch_op_compile

            self.will_output = will_output

        def stop(self):
            self._patch_SingleOpCompile.stop()

        def create_op_bin(self):
            # 模拟算子生成输出文件，从而使编译成功
            will_output = self.will_output.pop(0)
            assert will_output is not None

            will_output_meta_dir, will_output_bin_path, will_output_json_content = (
                will_output
            )
            will_output_json_content["binFileName"] = will_output_bin_path
            will_output_json_content["binFileSuffix"] = ".o"

            if not os.path.isdir(will_output_meta_dir):
                os.mkdir(will_output_meta_dir)
            with open(will_output_bin_path + ".json", "w") as output_json:
                with open(will_output_bin_path + ".o", "w") as output_obj:
                    json.dump(will_output_json_content, output_json)
                    output_obj.write("")

            return will_output_bin_path + ".json"

    def test_will_trans_deterministic_arg_to_build_config(self):
        test_arg = self.DefaultArg()

        opc_compile_args = test_arg.compile_args
        opc_compile_args[OpcOptions.DETERMINISTIC] = "true"

        testMock = self.MockSingleOpCompile(
            [(test_arg.temp_output.meta_dir, test_arg.temp_output.output_bin, {})]
        )

        with patch("op_compilation.get_single_op_operator"):
            test_op_compilation = OpCompilation(opc_compile_args)
            test_compile_result = test_op_compilation.single_op_compilation(
                test_arg.op_json
            )
            assert test_compile_result is True

        assert (
            testMock.patch_SingleOpCompile.call_args.args[1]["deterministic"] == "true"
        )

        testMock.stop()

    def test_deterministic_true__will_insert_deterministic_to_support_info(self):
        test_arg = self.DefaultArg()

        opc_compile_args = test_arg.compile_args
        opc_compile_args[OpcOptions.DETERMINISTIC] = "true"

        testMock = self.MockSingleOpCompile(
            [
                (
                    test_arg.temp_output.meta_dir,
                    test_arg.temp_output.output_bin,
                    {"deterministic": "true"},
                )
            ]
        )

        with patch("op_compilation.get_single_op_operator"):
            test_op_compilation = OpCompilation(opc_compile_args)
            test_compile_result = test_op_compilation.single_op_compilation(
                test_arg.op_json
            )

        assert test_compile_result is True
        with open(
            test_arg.temp_output.output_bin + ".json", "r"
        ) as test_output_json_file:
            test_output_json = json.load(test_output_json_file)
            assert test_output_json["supportInfo"]["deterministic"] == "true"

        testMock.stop()

    def test_deterministic_true__when_kernelist_true__will_insert_deterministic_to_support_info(
        self,
    ):
        test_arg = self.DefaultArg()

        opc_compile_args = test_arg.compile_args
        opc_compile_args[OpcOptions.DETERMINISTIC] = "true"

        testMock = self.MockSingleOpCompile(
            [
                (
                    test_arg.temp_output.meta_dir,
                    test_arg.temp_output.output_bin,
                    {
                        "deterministic": "ignore",
                        "kernelList": [{"deterministic": "true"}],
                    },
                )
            ]
        )

        with patch("op_compilation.get_single_op_operator"):
            test_op_compilation = OpCompilation(opc_compile_args)
            test_compile_result = test_op_compilation.single_op_compilation(
                test_arg.op_json
            )

        assert test_compile_result is True
        with open(
            test_arg.temp_output.output_bin + ".json", "r"
        ) as test_output_json_file:
            test_output_json = json.load(test_output_json_file)
            assert test_output_json["supportInfo"]["deterministic"] == "true"

        testMock.stop()

    def test_deterministic_all__will_try_true__and_stop_when_bin_not_support(self):
        test_arg = self.DefaultArg()

        opc_compile_args = test_arg.compile_args
        opc_compile_args[OpcOptions.DETERMINISTIC] = "all"

        testMock = self.MockSingleOpCompile(
            [
                (
                    test_arg.temp_output.meta_dir,
                    test_arg.temp_output.output_bin,
                    {"deterministic": "ignore"},
                )
            ]
        )

        with patch("op_compilation.get_single_op_operator"):
            test_op_compilation = OpCompilation(opc_compile_args)
            test_compile_result = test_op_compilation.single_op_compilation(
                test_arg.op_json
            )

        assert test_compile_result is True
        assert (
            testMock.patch_SingleOpCompile.call_args.args[1]["deterministic"] == "true"
        )

        testMock.stop()

    def test_deterministic_all__when_op_support__will_rename_binfile__and_compile_nondeterministic(
        self,
    ):
        test_arg = self.DefaultArg()

        opc_compile_args = test_arg.compile_args
        opc_compile_args[OpcOptions.DETERMINISTIC] = "all"

        testMock = self.MockSingleOpCompile(
            [
                (
                    test_arg.temp_output.meta_dir,
                    test_arg.temp_output.output_bin,
                    {"deterministic": "true"},
                ),
                (
                    test_arg.temp_output.meta_dir,
                    test_arg.temp_output.output_bin,
                    {"deterministic": "false"},
                ),
            ]
        )

        with patch("op_compilation.get_single_op_operator"):
            test_op_compilation = OpCompilation(opc_compile_args)
            test_compile_result = test_op_compilation.single_op_compilation(
                test_arg.op_json
            )

        assert test_compile_result is True

        output_bin = (
            test_arg.temp_output.output_dir + "/" + test_arg.temp_output.bin_name
        )

        assert Path(output_bin + "_deterministic.json").is_file()
        assert Path(output_bin + "_deterministic.o").is_file()
        with open(output_bin + "_deterministic.json", "r") as output_deterministic_json:
            output_json = json.load(output_deterministic_json)
            assert output_json.get("deterministic") == "true"
            assert (
                output_json.get("binFileName")
                == test_arg.temp_output.bin_name + "_deterministic"
            )

        assert Path(output_bin + ".json").is_file()
        assert Path(output_bin + ".o").is_file()
        with open(output_bin + ".json", "r") as output_nondeterministic_json:
            output_json = json.load(output_nondeterministic_json)
            assert output_json.get("deterministic") == "false"

        testMock.stop()


class Tensor:
    def __init__(self, dtype, op):
        self.dtype = dtype
        self.op = op


class Op:
    def __init__(self, name):
        self.name = name


if __name__ == "__main__":
    unittest.main()
