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
single_op_post_compile
"""

import json
import copy

from asc_op_compile_base.common.utils import log as logger
from opc_common import str_to_sha256_hash, opc_log_full, LogLevel
from opc_common import read_json_file, update_json_file
from simplified_key_utils import generate_simplified_key
from constant import CompileParam, SupportInfo, OpcOptions, OptionalInOutMode, GraphDtype, OpcCompileMode
from post_compile_base import PostCompilation


class SingleOpPostCompile(PostCompilation):
    def update_info_to_json_file(self, op, op_info, json_file_path):
        """
        single op after compile, write support info and compile info to json file
        """
        logger.debug("update_info to json_file_path is %s.", json_file_path)
        support_info = self.__generate_op_support_info(op, op_info, json_file_path)
        update_json_file("supportInfo", support_info, json_file_path)

    @staticmethod
    def parse_deterministic_from_json(compiled_json):
        deterministic = None
        if "kernelList" not in compiled_json:
            deterministic = compiled_json.get("deterministic")
        else:  # kernelList in compiled_json
            kernels_deterministic = list(
                map(lambda kernel: kernel.get("deterministic"), compiled_json.get("kernelList"))
            )
            has_true = "true" in kernels_deterministic
            has_false = "false" in kernels_deterministic
            if has_true and has_false:
                logger.error(
                    "Both deterministic and non-deterministic results exist in the file kernel list (%s).",
                    compiled_json,
                )
                return None

            if has_true:
                deterministic = "true"
            elif has_false:
                deterministic = "false"
            else:
                deterministic = "ignore"

        return deterministic

    @staticmethod
    def parse_info_from_compiled_json(compiled_json_path):
        compiled_json = read_json_file(compiled_json_path)
        deterministic = SingleOpPostCompile.parse_deterministic_from_json(compiled_json)
        optional_input_mode = compiled_json.get(SupportInfo.OPTIONAL_INPUT_MODE)
        optional_output_mode = compiled_json.get(SupportInfo.OPTIONAL_OUTPUT_MODE)
        dynamic_param_mode = compiled_json.get(SupportInfo.DYNAMIC_PARAM_MODE)
        res_tuple = (deterministic, optional_input_mode, optional_output_mode, dynamic_param_mode)
        return res_tuple

    @staticmethod
    def __generate_op_attrs_static_key(op_info, op_compile_mode):
        """
        generate single op attrs static key
        """
        attrs_list = []
        ori_attrs = op_info.get(CompileParam.OP_ATTRS)
        if op_compile_mode == OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE:
            ori_attrs = op_info.get(CompileParam.ATTRS)
        logger.debug("static key ori_attrs is [%s]", str(ori_attrs))
        if ori_attrs is None:
            return None
        for attr_val in ori_attrs:
            if attr_val is None:
                logger.debug("Attr is none. Add none to attrs_list.")
                attrs_list.append(None)
                continue
            attr = {}
            dtype = attr_val.get(CompileParam.DTYPE)
            value = attr_val.get(CompileParam.VALUE)
            value_range = attr_val.get(CompileParam.VALUE_RANGE)
            value_list = attr_val.get(CompileParam.VALUE_LIST)
            if dtype is not None:
                attr[CompileParam.DTYPE] = dtype
            # value_range and value_list not take part in compute static_key, set attr.vaule = None
            if value_range or value_list or value is None:
                attr[CompileParam.VALUE] = None
                attrs_list.append(attr)
                continue
            if dtype not in (GraphDtype.FLOAT, GraphDtype.LIST_FLOAT):
                attr[CompileParam.VALUE] = value
            else:
                attr[CompileParam.VALUE] = None

            attrs_list.append(attr)
        logger.debug("Generate op static key attrs is [%s].", str(attrs_list))
        return attrs_list

    def __generate_op_build_options(self, op_info, impl_mode):
        """
        generate single op build options
        """
        build_options = {}
        if impl_mode:
            super().gen_notnone_param(build_options, "implMode", impl_mode)

        if self._l1_fusion_flag:
            build_options[SupportInfo.L1_FUSION] = "true"

        if self._l2_fusion_flag:
            build_options[SupportInfo.L2_FUSION] = "true"

        if self._l2_mode_flag != "0":
            build_options[SupportInfo.L2_MODE] = self._l2_mode_flag

        status_check = op_info.get(CompileParam.STATUS_CHECK)
        build_options[CompileParam.STATUS_CHECK] = status_check

        logger.debug("Generate op build options is %s.", str(build_options))
        if len(build_options) == 0:
            logger.debug("Generate no build_options.")
            return None
        return build_options

    @staticmethod
    def __generate_single_tensor_supportinfo(index, desc, is_support_info):
        """
        generate single tensor supportinfo
        """
        op = {}
        if index != "" and is_support_info:
            op["id"] = index
        # "tensor field": if need to check is_support_info
        tensor_list = {
            CompileParam.DTYPE: False,
            CompileParam.FORMAT: False,
            CompileParam.ORI_FORMAT: True,
            CompileParam.SHAPE: False,
            CompileParam.ORI_SHAPE: True,
            CompileParam.RANGE: True,
            CompileParam.ORI_RANGE: True,
            CompileParam.ADDR_TYPE: True,
            CompileParam.USE_L1_WORKSPACE: True,
            CompileParam.L1_ADDR_FLAG: True,
            CompileParam.L1_FUSION_TYPE: True,
            CompileParam.SPLIT_INDEX: True,
            CompileParam.L1_WORKSPACE_SIZE: True,
            CompileParam.L1_ADDR_OFFSET: True,
            CompileParam.L1_VALID_SIZE: True,
            CompileParam.IS_FIRST_LAYER: True,
            CompileParam.SLICE_OFFSET: True,
            CompileParam.VALID_SHAPE: True,
            CompileParam.TOTAL_SHAPE: True,
        }
        for key in tensor_list:
            value = desc.get(key)
            if value is None:
                continue
            need_check_is_support_info = tensor_list.get(key)
            if need_check_is_support_info:
                if is_support_info:
                    op[key] = value
            else:
                op[key] = value

        if is_support_info is True and op.get(CompileParam.ORI_FORMAT) == "ALL":
            logger.debug("Delete support info ori format key when its value is ALL.")
            del op[CompileParam.ORI_FORMAT]

        logger.debug("Generate single tensor: [%s].", str(op))
        return op

    def __get_op_inputs_or_outputs_supportinfo(self, op_info, is_support_info):
        """
        generate single op inputs or outputs supportinfo
        """

        def _get_tensor_info(tensor, is_support, inputs_or_output):
            index = ""
            info = self.__generate_single_tensor_supportinfo(index, tensor, is_support)
            inputs_or_output.append(info)

        if op_info is None:
            return None
        inputs_or_outputs = []
        for op in op_info:
            if op is None:
                logger.debug("Op is none. Add none to inputs_or_outputs.")
                inputs_or_outputs.append(None)
                continue
            if isinstance(op, list):
                inputs_or_output = []
                for item in op:
                    _get_tensor_info(item, is_support_info, inputs_or_output)
                inputs_or_outputs.append(inputs_or_output)
            else:
                _get_tensor_info(op, is_support_info, inputs_or_outputs)
        return inputs_or_outputs

    @staticmethod
    def __generate_optional_input_palceholder(op, op_type, inputs):
        inputs_info = op.get("inputs")
        if len(inputs_info) != len(inputs):
            logger.error(
                "Generate op[Binfilename=%s] support info fail, the lenth of inputs_info[%d] should be \
                         equal to len inputs[%d]",
                op.get(OpcOptions.BIN_FILENAME),
                len(inputs_info),
                len(inputs),
            )
            return

        for i, input_i in enumerate(inputs_info):
            if isinstance(input_i, (tuple, list)):
                for input in input_i:
                    param_type = input.get(CompileParam.PARAM_TYPE, None)
                    if param_type is not None and param_type == "optional":
                        input = None
            elif inputs[i] is None:
                continue
            else:
                param_type = input_i.get(CompileParam.PARAM_TYPE, None)
                if param_type is not None and param_type == "optional":
                    inputs[i] = None
        return

    def __generate_single_op_static_key(self, op, op_info, impl_mode):
        """
        generate single op static key
        """
        static_key_json = {}
        # should be in alphabetical order to add in static_key_json, because reuse json is in this order
        attrs = self.__generate_op_attrs_static_key(op_info, self._opc_compile_args.get(OpcOptions.OP_COMPILE_MODE))
        super().gen_notnone_param(static_key_json, "attrs", attrs)

        build_options = self.__generate_op_build_options(op_info, impl_mode)
        super().gen_notnone_param(static_key_json, "buildOptions", build_options)

        inputs = self.__get_op_inputs_or_outputs_supportinfo(op_info.get(CompileParam.INPUTS), False)
        opt_input_mode = self._opc_compile_args.get(OpcOptions.OPTIONAL_INPUT_MODE)
        if opt_input_mode == OptionalInOutMode.GEN_PLACEHOLDER:
            op_type = op_info.get(CompileParam.OP_TYPE)
            self.__generate_optional_input_palceholder(op, op_type, inputs)
        super().gen_notnone_param(static_key_json, "inputs", inputs)

        outputs = self.__get_op_inputs_or_outputs_supportinfo(op_info.get(CompileParam.OUTPUTS), False)
        super().gen_notnone_param(static_key_json, "outputs", outputs)

        # remove extra space in dict to json
        static_key_json_str = json.dumps(static_key_json, separators=(",", ":"))

        static_key = str_to_sha256_hash(static_key_json_str)
        logger.info(
            "Generate op(Binfilename=%s) static key is [%s]. Static key json is [%s]",
            op_info.get(OpcOptions.BIN_FILENAME),
            str(static_key),
            str(static_key_json_str),
        )
        return static_key

    def __generate_static_key(self, op, op_info):
        """
        __generate static_key
        """
        static_key = str()
        impl_mode_str = op_info.get(OpcOptions.IMPL_MODE, None)
        if not impl_mode_str:
            static_key = self.__generate_single_op_static_key(op, op_info, None)
            return static_key
        else:
            impl_mode_list = impl_mode_str.split(",", -1)
            for impl_mode in impl_mode_list:
                static_key_impl_mode = self.__generate_single_op_static_key(op, op_info, impl_mode)
                if len(static_key) == 0:
                    static_key = static_key_impl_mode
                else:
                    static_key = static_key + "," + static_key_impl_mode
                logger.debug("impl mode static_key: [%s].", static_key_impl_mode)
        logger.debug("static_key: [%s].", static_key)
        return static_key

    def __generate_simplified_key(self, op, op_info, deterministic):
        simplified_key, simplified_key_mode = generate_simplified_key(
            self._opc_compile_args, op, op_info, deterministic, self._simplified_key_mode
        )
        self._simplified_key_mode = simplified_key_mode
        return simplified_key

    def __change_inoroutputs(self, inputoutput):
        def _range_correct(tensor):
            if tensor.get(CompileParam.RANGE) is not None:
                tensor[CompileParam.RANGE] = self.__adjust_null_range(tensor[CompileParam.RANGE])
            if tensor.get(CompileParam.ORI_RANGE) is not None:
                tensor[CompileParam.ORI_RANGE] = self.__adjust_null_range(tensor[CompileParam.ORI_RANGE])

        if inputoutput is None:
            return inputoutput

        for inorout in inputoutput:
            if inorout is None:
                continue
            if isinstance(inorout, list):
                for item in inorout:
                    _range_correct(item)
            else:
                _range_correct(inorout)
        return inputoutput

    @staticmethod
    def __adjust_null_range(cur_range):
        if cur_range is None:
            return cur_range

        for row_index, row in enumerate(cur_range):
            for col_index, value in enumerate(row):
                if value is None:
                    cur_range[row_index][col_index] = -1
        return cur_range

    @staticmethod
    def __add_mode_in_attrs(support_info):
        attrs = support_info.get("attrs", None)
        logger.debug("add mode attrs is [%s]", str(attrs))
        if attrs is not None and len(attrs) > 0:
            for attr in attrs:
                if attr is not None and attr["value"] is not None:
                    attr["mode"] = "value"
                elif attr is not None:
                    attr["mode"] = "null"

    def __generate_op_support_info(self, op, op_info, json_file_path):
        """
        creat support info from op info
        """
        support_info = {}
        super().gen_notnone_param(support_info, "implMode", op_info.get(OpcOptions.IMPL_MODE))
        super().gen_notnone_param(support_info, "int64Mode", op_info.get(OpcOptions.INT64_MODE))
        # Whether the operator supports determinism is identified in the pass,
        # and then sets the output deterministic field.
        # Therefore, even if deterministic is set to true for CompileArgs,
        # the final result is subject to the output of the compiled operator.
        static_key = self.__generate_static_key(op, op_info)
        opt_impl_mode_flag = op_info.get("optional_impl_mode_flag", False)
        if opt_impl_mode_flag:
            static_key_impl_mode_optional = self.__generate_single_op_static_key(op, op_info, None)
            static_key = static_key + "," + static_key_impl_mode_optional
        logger.info("Final static_key %s.", static_key)

        deterministic, optional_input_mode, optional_output_mode, dynamic_param_mode = (
            self.parse_info_from_compiled_json(json_file_path)
        )
        simplified_key = self.__generate_simplified_key(op, op_info, deterministic)
        if self._simplified_key_mode is not None and len(simplified_key) > 0:
            super().gen_notnone_param(support_info, "simplifiedKeyMode", self._simplified_key_mode)
            super().gen_notnone_param(support_info, "simplifiedKey", simplified_key)
            logger.info(
                "Op's simplified_key_mode is %d, simplified_key is [%s].", self._simplified_key_mode, simplified_key
            )

        if dynamic_param_mode is not None:
            support_info[SupportInfo.DYNAMIC_PARAM_MODE] = dynamic_param_mode
            logger.info("Op's dynamicParamMode is %s.", dynamic_param_mode)
        if optional_input_mode is not None:
            support_info[SupportInfo.OPTIONAL_INPUT_MODE] = optional_input_mode
            logger.info("Op's optionalInputMode is %s.", optional_input_mode)
        if optional_output_mode is not None:
            support_info[SupportInfo.OPTIONAL_OUTPUT_MODE] = optional_output_mode
            logger.info("Op's optionalOutputMode is %s.", optional_output_mode)

        super().gen_notnone_param(support_info, "staticKey", static_key)
        inputs = self.__change_inoroutputs(op.get(CompileParam.INPUTS))
        super().gen_notnone_param(support_info, "inputs", inputs)
        self.adjust_support_info_inputoutput(support_info.get(SupportInfo.INPUTS))
        outputs = self.__change_inoroutputs(op.get(CompileParam.OUTPUTS))
        super().gen_notnone_param(support_info, "outputs", outputs)
        self.adjust_support_info_inputoutput(support_info.get(SupportInfo.OUTPUTS))
        attrs = copy.deepcopy(op.get(CompileParam.ATTRS))
        super().gen_notnone_param(support_info, "attrs", attrs)
        super().gen_notnone_param(support_info, "opMode", self._opc_compile_args.get(OpcOptions.OP_MODE, None))
        if self._simplified_key_mode == 1:
            self.__add_mode_in_attrs(support_info)
        super().gen_notnone_param(support_info, "tilingKey", self._opc_compile_args.get(OpcOptions.TILING_KEY, None))
        if deterministic is not None:
            support_info["deterministic"] = deterministic
        op_debug_config = self._opc_compile_args.get(OpcOptions.OP_DEBUG_CONFIG)
        if op_debug_config != "":
            support_info["op_debug_config"] = op_debug_config

        opc_log_full(LogLevel.DEBUG, "Generate op supportInfo is %s", str(support_info))
        return support_info
