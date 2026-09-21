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
op compile
"""

import os
import sys
import stat
import shutil
import importlib
import inspect
import re

from asc_op_compile_base.common.buildcfg import build_config
from asc_op_compile_base.common.context import op_context
from asc_op_compile_base.common.platform import platform_info
from asc_op_compile_base.common.utils import log as logger
from constant import CompileParam, OpcOptions, OpcCompileMode, GraphDefParam, OptionalInOutMode, OpImplType

from single_op_post_compile import SingleOpPostCompile
from op_manager import (
    get_single_op_operator,
    get_core_type_from_op_content,
    get_dynamic_compile_static,
    get_op_impl_switch,
    is_valid_module_path,
    get_enable_vector_core_from_opstore,
)
from opc_common import (
    check_is_dynamic,
    get_except_msg,
    get_int64_mode,
    LogLevel,
    record_log_list,
    opc_log_full,
    check_and_normalize_impl_mode,
    read_json_file,
    update_json_file,
    get_file_real_path,
    normalize_optional_impl_mode,
)
from op_info_parser import OpInfoParser
from single_op_compile import SingleOpCompile
from op_compile_info_check import check_op_compilation_json, check_op_compilation_dict


class OpCompilation:
    """
    class for opc to compile tbe op
    """

    def __init__(self, opc_compile_args):
        """
        init
        :param opc_compile_args: type is dict, key defined in compiling_option_keys of opc_common
        """
        self.__opc_compile_args = opc_compile_args
        self.__l1_fusion_flag = True
        self.__l2_fusion_flag = True
        self.__l2_mode_flag = "0"
        self.__compile_res = True
        self.__debug_dir_vec = []
        self.__pid = os.getpid()
        self.__is_ascendc = False

    def set_l1_fusion_flag(self, l1_fusion_flag):
        """
        set l1 fusion flag
        """
        self.__l1_fusion_flag = l1_fusion_flag

    def set_l2_fusion_flag(self, l2_fusion_flag):
        """
        set l2 fusion flag
        """
        self.__l2_fusion_flag = l2_fusion_flag

    def set_l2_mode_flag(self, l2_mode_flag):
        """
        set l2 mode flag
        """
        self.__l2_mode_flag = l2_mode_flag

    def __generate_debug_dir(self, bin_filename):
        opc_debug_dir = self.__opc_compile_args.get(OpcOptions.DEBUG_DIR, "./")
        opc_debug_dir = opc_debug_dir.strip()
        return "{}/kernel_meta_{}".format(opc_debug_dir, bin_filename)

    def __create_lock_file(self, debug_dir):
        lock_file = os.path.join(debug_dir, "kernel_meta.lock")
        if os.path.exists(lock_file):
            try:
                logger.debug("Lock file[%s] is existed, try to read pid from it", lock_file)
                with open(lock_file, "r") as f:
                    lock_pid = f.readline()
                    logger.debug("Pid of lock file[%s] is [%s]", lock_file, lock_pid)
                    if lock_pid != str(self.__pid):
                        logger.error(
                            "The pid[%s] of lock file[%s] is different from current pid[%s]",
                            lock_pid,
                            lock_file,
                            str(self.__pid),
                        )
                        raise RuntimeError("Another process is using this dir")
            except Exception as e:
                logger.error("Fail to open and read lock file[%s]", lock_file)
                raise RuntimeError("Another process is using this dir") from e
            finally:
                pass
        else:
            try:
                logger.debug("Lock file[%s] is not existed, try to create it", lock_file)
                with open(lock_file, "w") as f:
                    f.write(str(self.__pid))
                logger.debug("Lock file[%s] has been created", lock_file)
            except Exception as e:
                logger.error("Fail to create lock file[%s].", lock_file)
                raise RuntimeError("Another process is using this dir.") from e
            finally:
                pass

    def __create_debug_dir(self, bin_filename):
        """
        create kernel_meta dir and lock file
        :param bin_filename: bin file name
        :return: debug dir path
        """
        debug_dir = self.__generate_debug_dir(bin_filename)

        if not os.path.exists(debug_dir):
            try:
                os.makedirs(debug_dir, stat.S_IRWXU + stat.S_IRGRP + stat.S_IXGRP, exist_ok=True)
                logger.debug("Debug dir[%s] has been created.", debug_dir)
            except Exception as e:
                logger.error("Fail to create debug dir[%s].", debug_dir)
                raise RuntimeError("Another process is using this dir.") from e
            finally:
                pass

        self.__create_lock_file(debug_dir)
        if self.__debug_dir_vec.count(debug_dir) == 0:
            self.__debug_dir_vec.append(debug_dir)
        return debug_dir

    def __single_op_post_compile(self, op, op_info, json_file_path):
        """
        after compile, write support info and compile info to json file
        """
        logger.debug("Single op post compile process, json_file_path is %s.", json_file_path)
        post_compile = SingleOpPostCompile(
            self.__opc_compile_args, self.__l1_fusion_flag, self.__l2_fusion_flag, self.__l2_mode_flag
        )
        post_compile.update_info_to_json_file(op, op_info, json_file_path)
        post_compile.copy_compile_res_files_to_output(json_file_path)

    def __del_compile_res_files(self, op_info):
        """
        finally, delete compile res if op_debug_level is 0
        """
        if self.__opc_compile_args.get(OpcOptions.OP_DEBUG_LEVEL, 0) != 0:
            return
        if self.__opc_compile_args.get(OpcOptions.KEEP_DIR, False):
            return

        file_path = op_info.get(OpcOptions.KERNEL_META_PATH)
        if file_path is None:
            return
        if not os.path.exists(file_path):
            logger.info("File path[%s] is not existed.", file_path)
            return
        if not os.path.isdir(file_path):
            logger.error("File path[%s] is not dir.", file_path)
            return

        kernel_name = op_info.get(OpcOptions.KERNEL_NAME)
        if not kernel_name:
            logger.error("kernel_name not exist.")
            return

        for file in os.listdir(file_path):
            if kernel_name in file:
                to_del_file = "{}/{}".format(file_path, file)
                try:
                    os.remove(to_del_file)
                    logger.info("Delete file[%s] success", to_del_file)
                except Exception as e:
                    logger.error("Delete file[%s] failed, reason: %s.", to_del_file, str(e))
                finally:
                    pass

    @staticmethod
    def record_compile_error_info(op_info, idx, op, error_info):
        """
        record compile error info, include traceback message
        """
        record_log_list(get_except_msg(), LogLevel.ERROR)
        logger.error(
            "Op[%s] of index[%d] compile failed, kernelName: %s.",
            op_info.get(CompileParam.OP_TYPE),
            idx,
            op_info.get(OpcOptions.KERNEL_NAME),
        )
        opc_log_full(LogLevel.ERROR, "reason is:[%s].", error_info)

    @staticmethod
    def __set_int64_mode(op_info):
        """
        check is dynamic op by shape
        """
        int64_mode = False
        if not op_info[OpcOptions.IS_DYNAMIC]:
            int64_mode = get_int64_mode(op_info.get(CompileParam.INPUTS)) or get_int64_mode(
                op_info.get(CompileParam.OUTPUTS)
            )
        op_info[OpcOptions.INT64_MODE] = int64_mode
        logger.debug("int64_mode of Op[%s] is %d.", op_info.get(CompileParam.OP_TYPE), int64_mode)

    @staticmethod
    def __check_update_impl_mode(impl_mode_str, impl_mode_default):
        impl_mode_str = "".join(impl_mode_str.split())
        opt_impl_mode_flag = False
        if OpImplType.OPTIONAL in impl_mode_str:
            impl_mode_str = normalize_optional_impl_mode(impl_mode_str)
            opt_impl_mode_flag = True
        elif impl_mode_default and (impl_mode_default.default != "") and (impl_mode_str == impl_mode_default.default):
            opt_impl_mode_flag = True
        return impl_mode_str, opt_impl_mode_flag

    def __set_impl_mode(self, op_info):
        """
        set real impl mode
        """
        # if op func not contain param "imple_mode", set impl_mode in opc_compile_args to None
        # else if user not set imple_mode, use op func param default value
        # otherwise use user configed value
        op_func = op_info.get(OpcOptions.OP_FUNC_ATTR)
        impl_mode_default = inspect.signature(op_func).parameters.get(OpcOptions.IMPL_MODE, None)
        if impl_mode_default is None:
            logger.debug("Op func[%s] not contain arg impl_mode", self.__opc_compile_args.get(OpcOptions.MAIN_FUNC))
        elif impl_mode_default.kind not in (inspect.Parameter.KEYWORD_ONLY, inspect.Parameter.POSITIONAL_OR_KEYWORD):
            raise RuntimeError(
                "impl_mode of op func[%s] is %s", self.__opc_compile_args.get(OpcOptions.MAIN_FUNC), impl_mode_default
            )
        impl_mode_cfg = op_info.get(OpcOptions.IMPL_MODE, None)
        if impl_mode_cfg is None:
            impl_mode_cfg = self.__opc_compile_args.get(OpcOptions.IMPL_MODE)
            if impl_mode_cfg is None:
                del op_info[OpcOptions.IMPL_MODE]
                logger.info(
                    "del IMPL_MODE. impl_mode_cfg [%s]. impl_mode_default: [%s].", impl_mode_cfg, impl_mode_default
                )
                return
        valid_flag, _ = check_and_normalize_impl_mode(impl_mode_cfg)
        if impl_mode_default is None and valid_flag and not self.__is_ascendc:
            logger.info("Op func[%s] not contain arg impl_mode", self.__opc_compile_args.get(OpcOptions.MAIN_FUNC))
            return
        if impl_mode_default is not None:
            logger.debug("impl_mode_cfg [%s]. impl_mode_default: [%s].", impl_mode_cfg, impl_mode_default)
        op_info[OpcOptions.IMPL_MODE], opt_impl_mode_flag = self.__check_update_impl_mode(
            impl_mode_cfg, impl_mode_default
        )
        op_info["optional_impl_mode_flag"] = opt_impl_mode_flag
        logger.debug(
            "ImplMode of Op[%s] is %s, opt_impl_mode_flag is %d.",
            op_info.get(CompileParam.OP_TYPE),
            op_info[OpcOptions.IMPL_MODE],
            opt_impl_mode_flag,
        )
        return

    @staticmethod
    def __get_single_op_operator(op_type, op_info):
        """
        single op compile
        """
        dynamic_compile_static = get_dynamic_compile_static(op_type, op_info)
        if not dynamic_compile_static:
            logger.warn("{} dynamic_compile_static is None".format(op_type))

        op_impl_switch = get_op_impl_switch(op_type, op_info)
        if op_impl_switch:
            op_info[OpcOptions.OP_IMPL_SWITCH] = op_impl_switch

        is_dynamic = op_info.get(OpcOptions.IS_DYNAMIC)
        return get_single_op_operator(op_type, dynamic_compile_static, is_dynamic)

    def __single_op_compile(self, op, op_info, idx):
        """
        single op compile
        """
        try:
            op_info_parser = OpInfoParser(op, op_info, self.__opc_compile_args)
            op_info_parser.get_op_info(op_info.get(CompileParam.OP_TYPE))

            op_info[OpcOptions.IS_DYNAMIC] = check_is_dynamic(op_info.get(CompileParam.INPUTS)) or check_is_dynamic(
                op_info.get(CompileParam.OUTPUTS)
            )
            op_func_attr = op_info.get(OpcOptions.OP_FUNC_ATTR)
            if not op_func_attr:
                op_func_attr = self.__get_single_op_operator(op_info.get(CompileParam.OP_TYPE), op_info)
                if op_func_attr:
                    op_info[OpcOptions.OP_FUNC_ATTR] = op_func_attr
                else:
                    logger.error("op %s op_func_attr is None.", op_info.get(CompileParam.OP_TYPE))
                    self.__compile_res = False
                    return

            self.__set_int64_mode(op_info)
            self.__set_impl_mode(op_info)
            op_info[OpcOptions.KERNEL_NAME] = op_info_parser.generate_kernel_name()

            # set kernel meta path
            debug_dir = self.__create_debug_dir(op_info[OpcOptions.KERNEL_NAME])
            op_info[OpcOptions.KERNEL_META_PATH] = debug_dir

            logger.debug("call vector_random_buff, debug_dir is %s", debug_dir)
            tbe_debug_level_value = int(self.__opc_compile_args.get(OpcOptions.OP_DEBUG_LEVEL, 0))
            if tbe_debug_level_value == 3:
                tbe_debug_level_value = 0

            single_op_obj = SingleOpCompile(op, op_info, self.__opc_compile_args)
            logger.debug("Call SingleOpCompile.")
            json_file_path = single_op_obj.op_compile()
            self.__single_op_post_compile(op, op_info, json_file_path)

            logger.debug(
                "Op[%s] of index[%d] compile success, kernelName:[%s].",
                op_info.get(CompileParam.OP_TYPE),
                idx,
                op_info.get(OpcOptions.KERNEL_NAME),
            )

            self.__compile_res = True
        except Exception as e:
            logger.error("Exception occured, %s", str(e))
            self.record_compile_error_info(op_info, idx, op, str(e))
            self.__compile_res = False
        finally:
            # delete compile result(.o\.json) files by kernel_name if necessary
            self.__del_compile_res_files(op_info)

    def __single_op_compile_all_deterministic(self, op, op_info, idx):
        def rename_output_op(output_name, new_output_name):
            logger.debug("Rename output kernel name %s to %s.", output_name, new_output_name)

            output_path = self.__opc_compile_args[OpcOptions.OUTPUT]

            for suffix in ["o", "json"]:
                os.rename(
                    get_file_real_path(output_path, output_name, suffix),
                    get_file_real_path(output_path, new_output_name, suffix),
                )

            new_output_json = get_file_real_path(output_path, new_output_name, "json")
            update_json_file("binFileName", new_output_name, new_output_json)

        deterministic_op_info = op_info.copy()
        deterministic_op_info[OpcOptions.DETERMINISTIC] = "true"
        self.__single_op_compile(op, deterministic_op_info, idx)

        if self.__compile_res is not True:
            return

        # If the value of deterministic in the compilation result is true,
        # the operator supports deterministic compilation.
        # In this case, the non-deterministic ones are also compiled.
        # Whether the operator supports determinism is identified in the pass.
        # Therefore, the operator cannot determine whether to support determinism in advance,
        # So, the judgment can only be based on the compilation result.

        # Use output_name instead of kernel_name because only the file name is changed in subsequent renaming,
        # the kernel list and kernel_name in JSON do not need to be changed.
        output_name = deterministic_op_info[OpcOptions.KERNEL_NAME]
        output_path = self.__opc_compile_args[OpcOptions.OUTPUT]
        output_json = get_file_real_path(output_path, output_name, "json")
        output_deterministic = read_json_file(output_json)["supportInfo"].get("deterministic")
        if output_deterministic == "ignore" or output_deterministic is None:
            # Operators do not focus on deterministic.
            # In this case, non-deterministic operators are not compiled.
            return

        elif output_deterministic != "true":
            self.__compile_res = False
            logger.error(
                "Compile operator with deterministic=all failed,"
                " output has unexpected deterministic %s." % output_deterministic
            )
            return

        else:  # output_json['deterministic'] == 'true'
            # Operators that support determinism are renamed to op_deterministic,
            # and non-deterministic operators are compiled.

            rename_output_op(output_name, output_name + "_deterministic")

            nondeterministic_op_info = op_info
            nondeterministic_op_info[OpcOptions.DETERMINISTIC] = "false"
            self.__single_op_compile(op, nondeterministic_op_info, idx)

    @staticmethod
    def get_op_func_attr(op_type, main_func, op_path):
        """
        get op func attr from tbe
        """
        logger.debug("op_type is {}, main_func is {}, op_path is {}".format(op_type, main_func, op_path))
        opm = is_valid_module_path(op_path)
        if opm is None:
            if "/impl/dynamic/" in op_path:
                op_module = "impl.dynamic.{}".format(os.path.splitext(os.path.basename(op_path))[0])
            else:
                last_folder = os.path.basename(os.path.dirname(op_path))
                op_module = "{}.{}".format(last_folder, op_type)
                py_module_path = "/".join(op_path.split("/")[:-2])
                if py_module_path != "" and py_module_path not in sys.path:
                    logger.debug("py module path is {}.".format(py_module_path))
                    sys.path.append(py_module_path)
            logger.debug("op_module is {}, op_path is {}.".format(op_module, op_path))

            try:
                opm = importlib.import_module(op_module)
            except ImportError as e:
                raise RuntimeError("import %s error, reason:%s." % (op_module, str(e))) from e
            finally:
                pass

        return getattr(opm, main_func)

    def set_flag_before_compile(self, l1_fusion, l2_fusion):
        if l1_fusion == "false":
            self.set_l1_fusion_flag(False)

        if l2_fusion == "false":
            self.set_l2_fusion_flag(False)

    @staticmethod
    def is_ascendc_op(file_path):
        # match "ascendc" or "tikcfw" in py
        pattern = r"ascendc|tikcfw"
        try:
            with open(file_path, "r", encoding="utf-8") as file:
                for line in file:
                    if re.search(pattern, line):
                        return True
                return False
        except FileNotFoundError:
            print(f"cannot find {file_path}")
            return False

    def __op_compile_by_json_info(self, json_dict):
        """
        parse op_list of json file to compile op
        """
        comment = json_dict.get("comment")
        l1_fusion = json_dict.get(CompileParam.SOC_INFO, {CompileParam.L1_FUSION: "false"}).get(
            CompileParam.L1_FUSION, "false"
        )
        l2_fusion = json_dict.get(CompileParam.SOC_INFO, {CompileParam.L2_FUSION: "false"}).get(
            CompileParam.L2_FUSION, "false"
        )
        py_op_path = self.__opc_compile_args.get(OpcOptions.OP_PATH)
        if py_op_path is not None:
            self.__is_ascendc = self.is_ascendc_op(py_op_path)
            logger.debug(f"__is_ascendc is {self.__is_ascendc}")

        if comment == "single ops" or comment is None:
            op_type = json_dict.get(CompileParam.OP_TYPE)
            status_check = json_dict.get(CompileParam.STATUS_CHECK, "true").lower()
            enable_vector_core = get_enable_vector_core_from_opstore(op_type)
            op_func_attr = None
            if (
                self.__opc_compile_args.get(OpcOptions.MAIN_FUNC) is not None
                and self.__opc_compile_args.get(OpcOptions.OP_PATH) is not None
            ):
                op_func_attr = self.get_op_func_attr(
                    op_type,
                    self.__opc_compile_args.get(OpcOptions.MAIN_FUNC),
                    self.__opc_compile_args.get(OpcOptions.OP_PATH),
                )

            self.set_flag_before_compile(l1_fusion, l2_fusion)
            self.set_l2_mode_flag(self.get_l2_mode())
            logger.info("End set_flag_before_compile")

            for idx, op in enumerate(json_dict.get(CompileParam.OP_LIST)):
                op_info = {
                    CompileParam.OP_TYPE: op_type,
                    OpcOptions.OP_FUNC_ATTR: op_func_attr,
                    GraphDefParam.EXTRA_PARAMS: op.get(GraphDefParam.EXTRA_PARAMS, {}),
                    CompileParam.STATUS_CHECK: status_check,
                    CompileParam.ENABLE_VECTOR_CORE: enable_vector_core,
                    OpcOptions.IMPL_MODE: op.get(OpcOptions.IMPL_MODE, None),
                    OpcOptions.JIT_COMPILE_MODE: op.get(OpcOptions.JIT_COMPILE_MODE, 0),
                    CompileParam.EXTRA_SETTINGS: op.get(CompileParam.EXTRA_SETTINGS, None),
                    OpcOptions.DETERMINISTIC_LEVEL: op.get(OpcOptions.DETERMINISTIC_LEVEL, ""),
                }
                deterministic = self.__opc_compile_args.get(OpcOptions.DETERMINISTIC)
                if deterministic is None:
                    self.__single_op_compile(op, op_info, idx)
                elif deterministic in {"true", "false"}:
                    op_info[OpcOptions.DETERMINISTIC] = deterministic
                    self.__single_op_compile(op, op_info, idx)
                elif deterministic == "all":
                    self.__single_op_compile_all_deterministic(op, op_info, idx)
        else:
            logger.warn("Current opc only support single op compile, comment is %s.", str(comment))
            self.__compile_res = False

    def set_optional_and_dynamic_mode(self, json_dict):
        optional_input_mode = json_dict.get(OpcOptions.OPTIONAL_INPUT_MODE)
        if optional_input_mode in {OptionalInOutMode.DEFAULT, OptionalInOutMode.GEN_PLACEHOLDER}:
            self.__opc_compile_args["optional_input_mode"] = optional_input_mode

        optional_output_mode = json_dict.get(OpcOptions.OPTIONAL_OUTPUT_MODE)
        if optional_output_mode in {OptionalInOutMode.DEFAULT, OptionalInOutMode.GEN_PLACEHOLDER}:
            self.__opc_compile_args["optional_output_mode"] = optional_output_mode

        dynamic_param_mode = json_dict.get(OpcOptions.DYNAMIC_PARAM_MODE)
        if dynamic_param_mode in {"unfolded", "folded_with_desc"}:
            self.__opc_compile_args[OpcOptions.DYNAMIC_PARAM_MODE] = dynamic_param_mode

    def set_current_compile_soc_info_before_compile(self):
        if self.__opc_compile_args.get(OpcOptions.AICORE_NUM) is None:
            platform_info.set_current_compile_soc_info(
                self.__opc_compile_args.get(OpcOptions.SOC_VERSION), self.__opc_compile_args.get(OpcOptions.CORE_TYPE)
            )
        else:
            platform_info.set_current_compile_soc_info(
                self.__opc_compile_args.get(OpcOptions.SOC_VERSION),
                self.__opc_compile_args.get(OpcOptions.CORE_TYPE),
                self.__opc_compile_args.get(OpcOptions.AICORE_NUM),
            )

    def single_op_compilation(self, json_dict):
        """
        single op compilation
        """
        try:
            # set soc version to platform
            self.set_current_compile_soc_info_before_compile()
            self.set_optional_and_dynamic_mode(json_dict)
            with op_context.OpContext("static"):
                # compile op
                self.__op_compile_by_json_info(json_dict)
        except Exception as e:
            record_log_list(get_except_msg(), LogLevel.ERROR)
            logger.error("Compile failed, reason is: %s", str(e))
            self.__compile_res = False
        finally:
            pass
        return self.__compile_res

    def get_l2_mode(self):
        """
        get l2 mode
        """
        if self.__opc_compile_args[OpcOptions.SOC_VERSION] != "ascend310":
            return "0"  # "no-append-args"

        if self.__l1_fusion_flag or self.__l2_fusion_flag:
            return "1"  # "L2-buffer-append-args"

        return "0"  # "no-append-args"

    def single_op_config_file_compile(self):
        """
        single_op_config_file_compile
        """
        if self.__opc_compile_args.get(OpcOptions.INPUT_PARAM):
            res, json_dict = check_op_compilation_json(OpcOptions.INPUT_PARAM, self.__opc_compile_args)
            if res is False:
                return res
            res = self.check_and_update_core_type(OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE, json_dict)
            if res is False:
                return res
            return self.single_op_compilation(json_dict)

        logger.error("opc command %s is empty.", OpcOptions.INPUT_PARAM)
        return False

    def single_op_dict_compile(self):
        """
        single_op_dict_compile
        """
        op_params = self.__opc_compile_args.get(OpcOptions.OP_PARAMS)
        if op_params:
            res, json_dict = check_op_compilation_dict(op_params, self.__opc_compile_args)
            if res is False:
                return res
            res = self.check_and_update_core_type(OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE, json_dict)
            if res is False:
                return res
            return self.single_op_compilation(json_dict)

        logger.error("opc command %s is empty.", OpcOptions.OP_PARAM)
        return False

    def op_compilation(self) -> bool:
        """
        Operator compiler entry in opc tools
        """
        try:
            op_compile_func = {
                OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE: self.single_op_config_file_compile,
                OpcCompileMode.SINGLE_OP_DICT_MODE: self.single_op_dict_compile,
            }
            op_compile_mode = self.__opc_compile_args.get(OpcOptions.OP_COMPILE_MODE)  # Validity has been checked
            if op_compile_mode in op_compile_func.keys():
                compile_func = op_compile_func.get(op_compile_mode)
                return compile_func()
            return False

        except Exception as e:
            raise RuntimeError("Opc Compile fail, : %s.", str(e)) from e

        finally:
            self.clear_debug_dir()

    def clear_debug_dir(self):
        """
        clear_debug_dir
        """
        for op_debug_dir in self.__debug_dir_vec:
            try:
                lock_file = os.path.join(op_debug_dir, "kernel_meta.lock")
                if os.path.exists(lock_file):
                    os.remove(lock_file)
                    logger.debug("Lock file[%s] has been removed.", lock_file)
            except Exception as e:
                raise RuntimeError("Del lock file [%s] field, reason: %s." % (lock_file, str(e))) from e
            finally:
                pass

        op_debug_level = self.__opc_compile_args.get(OpcOptions.OP_DEBUG_LEVEL)
        if op_debug_level != "" and op_debug_level != 0:
            return
        if self.__opc_compile_args.get(OpcOptions.KEEP_DIR, False):
            return
        for op_debug_dir in self.__debug_dir_vec:
            if os.path.isdir(op_debug_dir):
                try:
                    shutil.rmtree(op_debug_dir, True)
                    logger.info("debug_dir [%s] removed.", op_debug_dir)
                except Exception as e:
                    raise RuntimeError("Del debug_dir [%s] failed, reason: %s." % (op_debug_dir, str(e))) from e
                finally:
                    pass
            else:
                logger.error("debug_dir [%s] is not dir, can't be removed!", op_debug_dir)

    def get_core_type_from_fusion_op_graph(self, json_dict):
        core_type_list = []
        for op in json_dict.get(CompileParam.OP_LIST):
            op_type = op.get(CompileParam.TYPE, "")
            if op_type != "Data" and op_type != "Constant":
                core_type = self.get_core_type_by_op_type(op_type)
                core_type_list.append(core_type)
                logger.debug("add op_type: %s core_type:[%s] op_type_list", op_type, core_type)

        if len(core_type_list) == 0:
            logger.error("op_type_list is null, json_dict: %s", json_dict)
            return False
        else:
            if "AiCore" in core_type_list:
                self.__opc_compile_args[OpcOptions.CORE_TYPE] = "AiCore"
                logger.debug("set fusion core type is AiCore")
                return True
            else:
                self.__opc_compile_args[OpcOptions.CORE_TYPE] = "VectorCore"
                logger.debug("set current core type VectorCore")
                return True

    @staticmethod
    def get_core_type_by_op_type(op_type):
        core_type = get_core_type_from_op_content(op_type)
        if core_type in ("AiCore", "VectorCore", None):
            if not core_type:
                vector_combine = platform_info.get_soc_spec("cube_vector_combine")
                if vector_combine == "split":
                    logger.debug(
                        "return op_type:[%s] core type: VectorCore, vector_combine: %s", op_type, vector_combine
                    )
                    return "VectorCore"
                else:
                    logger.debug("return op_type:[%s] core type: AiCore, vector_combine: %s", op_type, vector_combine)
                    return "AiCore"
            else:
                logger.debug("return op_type:[%s] core type: %s", op_type, core_type)
                return core_type
        else:
            logger.info("Get op_type [%s] core_type is:[%s], OPC set core_type is None", op_type, core_type)
            return None

    def check_and_update_core_type(self, compile_type, json_dict):
        core_type = self.__opc_compile_args.get(OpcOptions.CORE_TYPE)
        if core_type:
            return True
        else:
            if compile_type == OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE:
                op_type = json_dict.get("op_type")
                if op_type is None:
                    logger.error("op_type is None. json_dict: %s", json_dict)
                    return False
                core_type = self.get_core_type_by_op_type(op_type)
                self.__opc_compile_args[OpcOptions.CORE_TYPE] = core_type
                logger.debug("set op_type: [%s] core type [%s]", op_type, core_type)
                return True
            else:
                return self.get_core_type_from_fusion_op_graph(json_dict)


def asc_op_compile(opc_compile_args) -> bool:
    from op_info_store import load_op_info_store

    load_op_info_store(opc_compile_args.get(OpcOptions.SOC_VERSION))
    op_compile = OpCompilation(opc_compile_args)
    return op_compile.op_compilation()
