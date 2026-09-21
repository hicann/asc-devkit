#!/usr/bin/env python
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
post_compile
"""

import json
import os
import stat

from shutil import copy as file_copy
from asc_op_compile_base.common.utils import log as logger
from opc_common import opc_log_full, LogLevel
from constant import CompileParam, SupportInfo, OpcOptions, GraphDefParam


class PostCompilation:
    """
    class PostCompilation
    """

    def __init__(self, opc_compile_args, l1_fusion_flag, l2_fusion_flag, l2_mode_flag, fusion_op_impl_mode=None):
        self._opc_compile_args = opc_compile_args
        self._l1_fusion_flag = l1_fusion_flag
        self._l2_fusion_flag = l2_fusion_flag
        self._l2_mode_flag = l2_mode_flag
        self._fusion_op_impl_mode = fusion_op_impl_mode
        self._simplified_key_mode = None

    @staticmethod
    def gen_notnone_param(support_info, key, value):
        """
        gen not none param to supportInfo
        """
        if value is None:
            logger.debug("Value of %s is none.", str(key))
            return
        support_info[key] = value

    def get_externel_input(self, cur_op, op_list, input, non_opt_input):
        inputs = cur_op.get(CompileParam.INPUTS, None)
        logger.debug("externel inputs is %s", str(inputs))
        if inputs is None:
            return False
        for input_desc in inputs:
            if CompileParam.NAME not in input_desc or input_desc[CompileParam.NAME] != "":
                return False

        outputs = cur_op.get(CompileParam.OUTPUTS, None)
        if outputs is None:
            return False

        is_input = False
        for output_desc in outputs:
            name = output_desc.get(CompileParam.NAME, None)
            if name is None:
                continue
            is_used_input, is_opt_input = self.__is_used_by_input(cur_op, op_list, name)
            if is_used_input:
                is_input = True
                input.append(output_desc)
                if not is_opt_input:
                    non_opt_input.append(output_desc)
                else:
                    non_opt_input.append(None)
        return is_input

    @staticmethod
    def __is_used_by_input(cur_op, op_list, name):
        for op in op_list:
            if op == cur_op or CompileParam.INPUTS not in op:
                continue
            for input_desc in op[CompileParam.INPUTS]:
                if name == input_desc.get(CompileParam.NAME, None):
                    para_type = input_desc.get(CompileParam.PARAM_TYPE, None)
                    is_opt_input = True if para_type == "optional" else False
                    return True, is_opt_input
        return False, False

    def get_externel_output(self, cur_op, op_list, output):
        outputs = cur_op.get(CompileParam.OUTPUTS, None)
        logger.debug("externel outputs is %s", str(outputs))
        if outputs is None:
            return False
        is_output = False
        for output_desc in outputs:
            name = output_desc.get(CompileParam.NAME, None)
            if name is None:
                continue
            output_been_used, _ = self.__is_used_by_input(cur_op, op_list, name)
            if not output_been_used:
                is_output = True
                output.append(output_desc)
        return is_output

    @staticmethod
    def get_static_key_build_options(json_dict):
        """
        get build_options from graph as statickey format
        """
        logger.debug("Generate graph support json_dict static[%s] key build options.", str(json_dict))
        build_options = {}
        soc_info = json_dict.get(CompileParam.SOC_INFO)
        logger.debug("Current soc_info is [%s]", str(soc_info))
        if soc_info is not None:
            l1_fusion = soc_info.get(CompileParam.L1_FUSION)
            if l1_fusion is not None and l1_fusion:
                build_options[SupportInfo.L1_FUSION] = "true"

            l2_fusion = soc_info.get(CompileParam.L2_FUSION)
            if l2_fusion is not None and l2_fusion:
                build_options[SupportInfo.L2_FUSION] = "true"

            l2_mode = soc_info.get(CompileParam.L2_MODE)
            if l2_mode is not None and l2_mode != "0":
                build_options[SupportInfo.L2_MODE] = l2_mode

            status_check = json_dict.get(CompileParam.STATUS_CHECK)
            build_options[CompileParam.STATUS_CHECK] = status_check

        else:
            logger.debug("graph {}'s soc_info is null.".format(json_dict.get(GraphDefParam.GRAPH_NAME)))

        if len(build_options) == 0:
            return None
        return build_options

    @staticmethod
    def adjust_support_info_inputoutput(inputoutput):
        def _adjust_ori_format(tensor_info):
            """delete ori format when its value is ALL"""
            if tensor_info.get(CompileParam.ORI_FORMAT) == "ALL":
                del tensor_info[CompileParam.ORI_FORMAT]
                logger.debug("Delete support info ori format key when its value is ALL.")

        if inputoutput is None:
            return

        for tensor in inputoutput:
            if tensor is None:
                continue
            if isinstance(tensor, list):
                for item in tensor:
                    _adjust_ori_format(item)
            else:
                _adjust_ori_format(tensor)

    @staticmethod
    def copy_one_json_file_with_one_obj_file(json_file_path, output_path):
        """
        copy both json file and object file to destination dir.
        """
        logger.debug("json_file_path is %s.", json_file_path)
        logger.debug("output_path is %s.", output_path)

        try:
            with open(json_file_path, "r") as f:
                json_file = json.load(f)

                opc_log_full(LogLevel.DEBUG, "json_file is %s.", str(json_file))

                # copy .json file
                os.chmod(json_file_path, stat.S_IWUSR + stat.S_IRGRP + stat.S_IRUSR)
                file_copy(os.path.realpath(json_file_path), output_path)

                # copy .o file
                object_file_path = os.path.join(
                    os.path.dirname(json_file_path), json_file["binFileName"] + json_file["binFileSuffix"]
                )
                logger.debug("object_file_path is %s.", object_file_path)
                os.chmod(object_file_path, stat.S_IWUSR + stat.S_IRGRP + stat.S_IRUSR)
                file_copy(os.path.realpath(object_file_path), output_path)
        except Exception as e:
            raise RuntimeError("Copy [%s] to [%s] field, reason: %s." % (json_file_path, output_path, str(e))) from e
        finally:
            pass

    def copy_compile_res_files_to_output(self, json_file_path):
        """
        single build result files(.json and .o files) to output path
        """
        # if output path not exist, creat it
        output_path = self._opc_compile_args.get(OpcOptions.OUTPUT)
        logger.debug("The compile result files copy to [%s]", str(output_path))

        try:
            with open(json_file_path, "r") as f:
                build_res = json.load(f)
                if "jsonList" not in build_res:  # traditional pattern with one json file and one object file
                    PostCompilation.copy_one_json_file_with_one_obj_file(json_file_path, output_path)
                    return

                # three .json files with two .o files
                # copy main .json file
                try:
                    os.chmod(json_file_path, stat.S_IWUSR + stat.S_IRGRP + stat.S_IRUSR)
                    file_copy(os.path.realpath(json_file_path), output_path)
                except Exception as e:
                    raise RuntimeError(
                        "Failed to copy [%s] to [%s], reason: %s." % (json_file_path, output_path, str(e))
                    ) from e
                finally:
                    pass

                # copy sub .json/.o files
                for item in build_res["jsonList"]:
                    sub_json_file_path = os.path.join(os.path.dirname(json_file_path), item["jsonFileName"])
                    PostCompilation.copy_one_json_file_with_one_obj_file(sub_json_file_path, output_path)
        except Exception as e:
            raise RuntimeError("File [%s] doesn't exist, reason: %s." % (json_file_path, str(e))) from e
        finally:
            pass
