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
opc single op compile
"""

import copy
import inspect

import asc_op_compile_base.common.utils.log as logger

from asc_op_compile_base.common.buildcfg import build_config
from asc_op_compile_base.common.context import op_context
from asc_op_compile_base.common.context import op_info as operator_info

from simplified_key_utils import infer_simplified_key_mode
from constant import OpcOptions, CompileParam, GraphDefParam, OpModeType
from op_manager import get_dynamic_compile_static_from_opstore
from opc_common import (
    get_new_attrs_for_op_compile,
    update_compile_info,
    get_json_file_path,
    LogLevel,
    opc_log_full,
    attrs_from_string_to_str,
)


class SingleOpCompile:
    """
    class for opc to compile single tbe op
    """

    def __init__(self, op, op_info, opc_compile_args):
        """
        init
        :param op_info: type is namedtuple of OP_INFO, defined in opc_common
        :param opc_compile_args: type is namedtuple of COMPILE_PARAS, defined in opc_common
        """
        self.__opc_compile_args = opc_compile_args
        self.__op_info = op_info
        self.__op = op

    @staticmethod
    def get_op_info_for_context(op_info, kwargs, context_op_info):
        """
        set op info to context
        """
        context_op_info.pattern = "Opaque"
        context_op_info.inputs = op_info.get(CompileParam.INPUTS)
        context_op_info.outputs = op_info.get(CompileParam.OUTPUTS)
        context_op_info.attrs = attrs_from_string_to_str(
            op_info.get(CompileParam.ATTRS)
        )
        params = {}
        for key, value in kwargs.items():
            params[key] = value
        extra_params = op_info.get(GraphDefParam.EXTRA_PARAMS, {})
        params.update(extra_params)
        context_op_info.extra_params = params

    @staticmethod
    def set_extra_settings_for_context(extra_settings, context, context_op_info):
        if extra_settings is None or not isinstance(extra_settings, list):
            return
        for setting in extra_settings:
            if not isinstance(setting, dict):
                continue
            setting_key = setting.get("key", "")
            setting_value = setting.get("value", "")
            setting_type = setting.get("type", "")
            if setting_type == "addition":
                logger.debug(
                    "[asc_opc] add key: {}, value {} to context".format(
                        setting_key, setting_value
                    )
                )
                context.add_addition(setting_key, setting_value)
            if setting_type == "op_info.extra_params":
                extra_params = context_op_info.extra_params
                extra_params.update(setting_value)
                logger.debug(
                    "[asc_opc] extra_params is {}".format(context_op_info.extra_params)
                )

    def set_kernel_spec_for_context(self, context):
        """Forward Kernel Spec settings to the AscendC compile context."""

        kernel_spec_mode = self.__opc_compile_args.get(OpcOptions.KERNEL_SPEC, "None")
        kernel_spec_dir = self.__opc_compile_args.get(OpcOptions.KERNEL_SPEC_DIR)
        context.add_addition("kernel_spec_mode", kernel_spec_mode)
        context.add_addition("kernel_spec_dir", kernel_spec_dir)
        if kernel_spec_mode == "SK":
            context.add_addition("super_kernel_sub_combine", True)
            if context.get_addition("super_kernel_sub_info") is None:
                context.add_addition(
                    "super_kernel_sub_info",
                    {
                        "enable_super_kernel": True,
                        "super_kernel_options": self.__opc_compile_args.get(
                            OpcOptions.SPK_OPT, ""
                        ),
                        "super_kernel_sub_loc": "middle",
                    },
                )

    def __get_single_check_impl_mode(self, op_info, impl_mode):
        op_func = op_info.get(OpcOptions.OP_FUNC_ATTR)
        impl_mode_default = inspect.signature(op_func).parameters.get(
            OpcOptions.IMPL_MODE, None
        )
        if impl_mode_default is None:
            logger.info(
                "Op func[%s] not contain arg impl_mode.",
                self.__opc_compile_args.get(OpcOptions.MAIN_FUNC),
            )
            return {}
        elif impl_mode_default.kind not in (
            inspect.Parameter.KEYWORD_ONLY,
            inspect.Parameter.POSITIONAL_OR_KEYWORD,
        ):
            logger.error(
                "impl_mode of op func[%s] is %s.",
                self.__opc_compile_args.get(OpcOptions.MAIN_FUNC),
                impl_mode_default,
            )
            return {}
        return impl_mode

    def __get_kwargs(self):
        """
        kwargs is dict, current only contain impl_mode, key is 'impl_mode'
        """
        kwargs = {}
        if OpcOptions.IMPL_MODE in self.__op_info:
            kwargs[OpcOptions.IMPL_MODE] = self.__op_info.get(OpcOptions.IMPL_MODE)
            kwargs = self.__get_single_check_impl_mode(self.__op_info, kwargs)
            logger.debug("impl_mode: %s", kwargs.get(OpcOptions.IMPL_MODE, ""))

        op_impl_switch = self.__op_info.get(OpcOptions.OP_IMPL_SWITCH, None)
        if op_impl_switch:
            kwargs[OpcOptions.OP_IMPL_SWITCH] = op_impl_switch
            logger.debug(
                "op_impl_switch: %s", kwargs.get(OpcOptions.OP_IMPL_SWITCH, "")
            )
        return kwargs

    def __call_op(self):
        """
        call tbe build function
        """
        logger.info("__call_op")
        context_op_info = operator_info.OpInfo(
            self.__opc_compile_args.get(OpcOptions.MAIN_FUNC),
            self.__op_info.get(CompileParam.OP_TYPE),
        )
        # get kwargs
        kwargs = self.__get_kwargs()
        self.get_op_info_for_context(self.__op_info, kwargs, context_op_info)
        # get compile func obj
        op_func = self.__op_info.get(OpcOptions.OP_FUNC_ATTR)
        # attrs for tbe compile is list, only contain value, no name and dtype
        new_attrs = get_new_attrs_for_op_compile(
            self.__op_info,
            op_func,
            self.__opc_compile_args.get(OpcOptions.OP_COMPILE_MODE),
        )
        inputs = copy.deepcopy(self.__op_info.get(CompileParam.INPUTS))
        outputs = copy.deepcopy(self.__op_info.get(CompileParam.OUTPUTS))
        context_op_info.precision_mode = self.__op_info.get(OpcOptions.IMPL_MODE)
        logger.debug(
            "context_op_info op_type [%s], precision_mode: %s.",
            self.__op_info.get(CompileParam.OP_TYPE),
            context_op_info.precision_mode,
        )
        dynamic_compile_static = get_dynamic_compile_static_from_opstore(
            self.__op_info.get(CompileParam.OP_TYPE)
        )
        res_list = infer_simplified_key_mode(self.__op, self.__opc_compile_args)
        opt_param_mode = res_list[1]
        dyn_param_mode = res_list[3]
        opt_input_mode = opt_param_mode
        opt_output_mode = opt_param_mode
        if (
            opt_param_mode is None
            and self.__opc_compile_args.get(OpcOptions.OPTIONAL_INPUT_MODE) is not None
        ):
            opt_input_mode = self.__opc_compile_args.get(OpcOptions.OPTIONAL_INPUT_MODE)
        if (
            opt_param_mode is None
            and self.__opc_compile_args.get(OpcOptions.OPTIONAL_OUTPUT_MODE) is not None
        ):
            opt_output_mode = self.__opc_compile_args.get(
                OpcOptions.OPTIONAL_OUTPUT_MODE
            )
        if (
            dyn_param_mode is None
            and self.__opc_compile_args.get(OpcOptions.DYNAMIC_PARAM_MODE) is not None
        ):
            dyn_param_mode = self.__opc_compile_args.get(OpcOptions.DYNAMIC_PARAM_MODE)
        sub_kernel_option = self.__opc_compile_args.get(OpcOptions.SPK_OPT, "")
        output_path = self.__opc_compile_args.get(OpcOptions.OUTPUT, ".")
        opc_log_full(
            LogLevel.INFO,
            "Call op func, inputs: %s, outputs: %s, attrs: %s, kernel_name: %s, \
                     kwargs: %s, optional_input_mode: %s, optional_output_mode: %s, dyn_param_mode %s, SPK_OPT[%s], \
                     output_path[%s].",
            str(inputs),
            str(outputs),
            str(new_attrs),
            str(self.__op_info.get(OpcOptions.KERNEL_NAME)),
            str(kwargs),
            str(opt_input_mode),
            str(opt_output_mode),
            str(dyn_param_mode),
            str(sub_kernel_option),
            str(output_path),
        )
        tiling_key_list = self.__opc_compile_args.get(OpcOptions.TILING_KEY)
        extra_settings = self.__op_info.get(CompileParam.EXTRA_SETTINGS, None)
        kernel_template_input = self.__opc_compile_args.get(
            OpcOptions.KERNEL_TEMPLATE_INPUT
        )
        if (
            self.__op_info.get(OpcOptions.IS_DYNAMIC)
            or self.__opc_compile_args.get(OpcOptions.OP_MODE) == OpModeType.DYNAMIC
        ):
            with op_context.OpContext("dynamic"):
                context = op_context.get_context()
                self.set_extra_settings_for_context(
                    extra_settings, context, context_op_info
                )
                self.set_kernel_spec_for_context(context)
                context.add_op_info(context_op_info)
                context.add_addition(OpcOptions.OPTIONAL_INPUT_MODE, opt_input_mode)
                context.add_addition(OpcOptions.OPTIONAL_OUTPUT_MODE, opt_output_mode)
                context.add_addition(OpcOptions.DYNAMIC_PARAM_MODE, dyn_param_mode)
                context.add_addition(OpcOptions.TILING_KEY, tiling_key_list)
                context.add_addition(OpcOptions.SPK_OPT, sub_kernel_option)
                context.add_addition(OpcOptions.OUTPUT, output_path)
                context.add_addition(
                    OpcOptions.KERNEL_TEMPLATE_INPUT, kernel_template_input
                )
                op_func(
                    *inputs,
                    *outputs,
                    *new_attrs,
                    self.__op_info.get(OpcOptions.KERNEL_NAME),
                    **kwargs,
                )

                # fetch json file path for build result.
                json_file_path = context.get_build_res("json_file_path")
                logger.debug("json_file_path is %s.", json_file_path)

                if not json_file_path:
                    json_file_path = get_json_file_path(self.__op_info)
                    logger.debug("json_file_path is %s.", json_file_path)
                self.update_jit_compile_info()
                update_compile_info(json_file_path, context.get_compile_info(None))
        elif dynamic_compile_static == "true":
            with op_context.OpContext("static"):
                context = op_context.get_context()
                self.set_extra_settings_for_context(
                    extra_settings, context, context_op_info
                )
                self.set_kernel_spec_for_context(context)
                context.add_op_info(context_op_info)
                context.add_addition(OpcOptions.OPTIONAL_OUTPUT_MODE, opt_output_mode)
                context.add_addition(OpcOptions.OPTIONAL_INPUT_MODE, opt_input_mode)
                context.add_addition(OpcOptions.DYNAMIC_PARAM_MODE, dyn_param_mode)
                context.add_addition(OpcOptions.TILING_KEY, tiling_key_list)
                context.add_addition(OpcOptions.SPK_OPT, sub_kernel_option)
                context.add_addition(OpcOptions.OUTPUT, output_path)
                context.add_addition(
                    OpcOptions.KERNEL_TEMPLATE_INPUT, kernel_template_input
                )
                op_func(
                    *inputs,
                    *outputs,
                    *new_attrs,
                    self.__op_info.get(OpcOptions.KERNEL_NAME),
                    **kwargs,
                )

                # fetch json file path for build result.
                json_file_path = context.get_build_res("json_file_path")
                logger.debug("json_file_path is %s.", json_file_path)

                if not json_file_path:
                    json_file_path = get_json_file_path(self.__op_info)
                    logger.debug("json_file_path is %s.", json_file_path)
        else:
            with op_context.OpContext("prestatic"):
                context = op_context.get_context()
                self.set_extra_settings_for_context(
                    extra_settings, context, context_op_info
                )
                self.set_kernel_spec_for_context(context)
                context.add_op_info(context_op_info)
                context.add_addition(OpcOptions.OPTIONAL_OUTPUT_MODE, opt_output_mode)
                context.add_addition(OpcOptions.OPTIONAL_INPUT_MODE, opt_input_mode)
                context.add_addition(OpcOptions.DYNAMIC_PARAM_MODE, dyn_param_mode)
                context.add_addition(OpcOptions.TILING_KEY, tiling_key_list)
                context.add_addition(OpcOptions.SPK_OPT, sub_kernel_option)
                context.add_addition(OpcOptions.OUTPUT, output_path)
                context.add_addition(
                    OpcOptions.KERNEL_TEMPLATE_INPUT, kernel_template_input
                )
                op_func(
                    *inputs,
                    *outputs,
                    *new_attrs,
                    self.__op_info.get(OpcOptions.KERNEL_NAME),
                    **kwargs,
                )

                # fetch json file path for build result.
                json_file_path = context.get_build_res("json_file_path")
                logger.debug("json_file_path is %s.", json_file_path)

                if not json_file_path:
                    json_file_path = get_json_file_path(self.__op_info)
                    logger.debug("json_file_path is %s.", json_file_path)
        return json_file_path

    def update_jit_compile_info(self):
        jit_compile_mode = self.__op_info.get(OpcOptions.JIT_COMPILE_MODE)
        # if mode isnt 2, ignore jit compile
        if jit_compile_mode != 2:
            return
        op_func = self.__op_info.get(OpcOptions.OP_FUNC_ATTR)
        context = op_context.get_context()
        context.add_compile_info(
            "jit_compile_attrs", self.__op_info.get(CompileParam.ATTRS)
        )
        if hasattr(op_func, "__name__"):
            context.add_compile_info("jit_compile_func", op_func.__name__)

    def op_compile(self):
        """
        singe op compile
        """
        logger.debug("op_compile.")
        dump_cce_switch = False
        if self.__opc_compile_args.get(OpcOptions.OP_DEBUG_LEVEL) in {1, 2}:
            dump_cce_switch = True
        tbe_debug_level_value = int(
            self.__opc_compile_args.get(OpcOptions.OP_DEBUG_LEVEL, 0)
        )
        if tbe_debug_level_value == 3:
            tbe_debug_level_value = 0
        debug_config = self.__opc_compile_args.get(OpcOptions.OP_DEBUG_CONFIG)
        status_check = self.__op_info.get(CompileParam.STATUS_CHECK) != "false"
        enable_vector_core = self.__op_info.get(CompileParam.ENABLE_VECTOR_CORE)
        enable_vector_core = False if enable_vector_core is None else enable_vector_core
        deterministic = self.__op_info.get(OpcOptions.DETERMINISTIC) == "true"
        deterministic_level = self.__op_info.get(OpcOptions.DETERMINISTIC_LEVEL, "")
        jit_compile_mode = self.__op_info.get(OpcOptions.JIT_COMPILE_MODE)
        op_relocatable_bin = self.__opc_compile_args.get(
            OpcOptions.RELOCATABLE_BIN, False
        )
        kernel_spec_mode = self.__opc_compile_args.get(OpcOptions.KERNEL_SPEC, "None")
        # Explicit SK enables the existing dynamic SK preparation path.
        enable_super_kernel = op_relocatable_bin or (kernel_spec_mode == "SK")
        logger.debug(
            "debug_config {}, enable_super_kernel {}.".format(
                debug_config, enable_super_kernel
            )
        )

        build_config_kwargs = {
            "tbe_debug_level": tbe_debug_level_value,
            "op_debug_config": debug_config,
            "kernel_meta_parent_dir": self.__op_info.get(
                OpcOptions.KERNEL_META_PATH, "./"
            ),
            "compatible": True,
            "enable_op_prebuild": False,
            "save_temp_cce_file": dump_cce_switch,
            "random_cce_file_location": False,
            "status_check": status_check,
            "enable_deterministic_mode": deterministic,
            "jit_compile_mode": jit_compile_mode,
            "enable_vector_core": enable_vector_core,
            "enable_super_kernel": enable_super_kernel,
        }

        if deterministic_level != "":
            build_config_kwargs["deterministic_level"] = deterministic_level
            logger.debug("deterministic_level is {}".format(deterministic_level))

        with build_config(**build_config_kwargs):
            json_file_path = self.__call_op()

            return json_file_path
