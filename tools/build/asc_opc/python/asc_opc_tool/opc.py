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
opc
"""

import getopt
import os
import sys

from asc_op_compile_base.common.utils import log as logger
from asc_op_compile_base.common.utils.AscendLog import AscendLog

from constant import OpcOptions, OpcCompileMode, OptionalInOutMode
from opc_common import compiling_option_keys, check_and_normalize_impl_mode
from op_compilation import OpCompilation
from op_info_store import load_op_info_store

log_level_dict = {
    "debug": 0,
    "info": 1,
    "warning": 2,
    "error": 3,
}

FAILED = -1
SUCCESS = 0


class Option:
    """
    define a option.
    no_args means the option does not need a value. e.g.
    -h or --help
    """

    def __init__(self, name, default_value="", help_message="", no_args=False):
        """
        init class
        """
        self.__name = name
        self.__default_value = default_value
        self.__help_message = help_message
        self.__no_args = no_args
        self.__value = default_value

    def get_name(self):
        """
        return the name of this option
        """
        return self.__name

    def get_value(self):
        """
        return the value of this option
        """
        return self.__value

    def set_value(self, value):
        """
        Set the value of this option
        """
        self.__value = value


class OpcOptionParser:
    """
    This class with parse the user's input arguments
    with getopt.
    """

    def __init__(self):
        """
        init class
        """
        self.__full_options = {}
        self.__compiling_options = {}
        self.__short_options = ""
        self.__long_options = []

    def __set_option(self, option, value):
        """
        :param option:
        :param value:
        :return:
        """
        self.__compiling_options[option] = value

    def set_option(self, option, value):
        """
        set option
        """
        self.__compiling_options[option] = value

    def get_option(self, option):
        """
        check option in __compiling_options first.
        __compiling_options are those will be passed to the compilation.
        Then check it in full options.
        """
        if option == "":
            logger.warning("should not get empty option")
            return ""
        if option in self.__compiling_options.keys():
            return self.__compiling_options.get(option, "")

        if len(option) == 1:
            full_option = "-" + option
        else:
            full_option = "--" + option

        full_option_obj = self.__full_options.get(full_option, None)
        if full_option_obj is not None:
            return full_option_obj.get_value()

        logger.warn("option %s is not valid", option)
        return ""

    def get_all_options(self):
        """
        :return:
        """
        return self.__compiling_options

    def __define_option(self, real_name, default_value, help_message, no_args=False):
        """
        :param real_name:
        :param default_value:
        :param help_message:
        :param no_args:
        :return:
        """
        option_object = Option(real_name, default_value, help_message, no_args)
        self.__compiling_options[real_name] = default_value
        # full name contains dash, for example
        # option "help" has full name "--help"
        # option "h" has full name "-h"
        if len(real_name) == 1:
            self.__short_options = self.__short_options + real_name
            if not no_args:
                self.__short_options = self.__short_options + ":"
            full_name = "-" + real_name
        else:
            name_with_equal = real_name
            if not no_args:
                name_with_equal = name_with_equal + "="
            self.__long_options.append(name_with_equal)
            full_name = "--" + real_name
        self.__full_options[full_name] = option_object

    def define_options(self):
        """
        Register all options with:
        (option key, default_value, help message, do not need value).
        :return:
        """
        self.__define_option(OpcOptions.HELP_H, "False", "show help messages", True)
        self.__define_option(OpcOptions.HELP, "False", "show help messages", True)
        self.__define_option(OpcOptions.OUTPUT, "", "output directory of binary files")
        self.__define_option(
            OpcOptions.SOC_VERSION, "", "soc version of current hardware"
        )
        self.__define_option(OpcOptions.LOG, "null", "log level")
        self.__define_option(
            OpcOptions.INPUT_PARAM, None, "the path of input json file"
        )
        self.__define_option(
            OpcOptions.MAIN_FUNC,
            None,
            'the main func name in operator"s implemenatation',
        )
        self.__define_option(
            OpcOptions.CORE_TYPE, None, "the core type of compiling environment"
        )
        self.__define_option(
            OpcOptions.DEBUG_DIR,
            "./",
            "Set the save path of operator compilation \
                             intermediate files. Default value: ./",
        )
        self.__define_option(
            OpcOptions.GRAPH, "", "the path of input fusion op file. Default value: ./"
        )
        self.__define_option(
            OpcOptions.BIN_FILENAME, "", "the name of op_kernel. Default valuen: ./"
        )
        self.__define_option(
            OpcOptions.IMPL_MODE,
            None,
            "Impl_mode of cur op. \
                             Default value: None",
        )
        self.__define_option(OpcOptions.AICORE_NUM, None, "the number of aicore.")
        self.__define_option(
            OpcOptions.OP_DEBUG_LEVEL,
            0,
            "tbe operator compile debug function switch. \
                             Default value: 0",
        )
        self.__define_option(
            OpcOptions.OP_DEBUG_CONFIG, "", "tbe operator compile debug config."
        )
        self.__define_option(
            OpcOptions.OPTIONAL_INPUT_MODE,
            None,
            "tbe operator compile optional input mode. \
                             Default value: no_placeholder",
        )
        self.__define_option(
            OpcOptions.OPTIONAL_OUTPUT_MODE,
            None,
            "tbe operator compile optional output mode. \
                             Default value: no_placeholder",
        )
        self.__define_option(OpcOptions.OP_MODE, "", "tbe operator compile mode.")
        self.__define_option(
            OpcOptions.DETERMINISTIC,
            "all",
            "whether to compile operators in deterministic mode. \
                             Default value: all",
        )
        self.__define_option(OpcOptions.SIMPLE_KEY_MODE, None, "simplified key mode.")
        self.__define_option(OpcOptions.DYNAMIC_PARAM_MODE, None, "dynamic param mode.")
        self.__define_option(OpcOptions.TILING_KEY, None, "tiling key list.")
        self.__define_option(
            OpcOptions.RELOCATABLE_BIN, "False", "op_relocatable_kernel_binary mode."
        )
        self.__define_option(OpcOptions.SPK_OPT, "", "SPK sub kernel compile options.")
        self.__define_option(
            OpcOptions.KERNEL_TEMPLATE_INPUT, "", "kernel template input."
        )
        self.__define_option(
            OpcOptions.KERNEL_SPEC,
            "None",
            "kernel spec mode. Support None, Normal and SK.",
        )
        self.__define_option(
            OpcOptions.KERNEL_SPEC_DIR, None, "kernel spec output directory."
        )

    @staticmethod
    def usage():
        """
        print the help message
        :return:
        """
        print(
            "usage: opc <args>\n"
            "example of compiling binary files by input jsons:\n"
            "opc op_path --input_param=./operator.json \n"
            "--main_func=operator_main_func \n"
            "--output=./output_dir --soc_version=Ascend910 \n"
            "op path is the second argument. \n"
            "============ Basic Functionality ===========\n"
            "[General]\n"
            "  -h/--help           Show this help message.\n"
            "\n[Input]\n"
            "  op_path             The path of operator file.\n"
            "  --input_param       The path of input json file.\n"
            "  --main_func         The compilation function in operator's implementation file.\n"
            "  --graph             The path of input graph file.\n"
            "  --bin_filename      The name of op kernel.\n"
            "\n[Output]\n"
            "  --output            Output file path(needn't suffix, will add .o and .json automatically).\n"
            "\n[Target]\n"
            "  --soc_version       The soc version.\n"
            "  --core_type         Set core type AiCore or VectorCore. VectorCore: use vector core.\n"
            "  --aicore_num        Set aicore num.\n"
            "\n[Debug]\n"
            "  --log               Generate log with level. Support debug, info, warning, error, null.\n"
            "  --debug_dir         Set the save path of operator compilation intermediate files. Default value: ./.\n"
            "\n[Operator Tuning]\n"
            "  --impl_mode         Set op impl_mode. Support high_precision, high_performance,\n"
            "                      enable_hi_float_32_execution, enable_float_32_execution, optional,\n"
            "                      or their combination.  Default: None.\n"
            "  --deterministic     Whether to compile operators in deterministic mode.\n"
            "                      all(default): Compiles operators both in deterministic and non-deterministic.\n"
            "                      true: Compiles operators in deterministic mode.\n"
            "                      false: Compiles operators in non-deterministic mode.\n"
            "Auto Tune.\n"
            "  --op_debug_level    Debug enable for TBE operator building.\n"
            "                      0 (default): Disable debug;\n"
            "                      1: Enable TBE pipe_all, and generate the operator CCE file and Python-CCE "
            "mapping file (.json);\n"
            "                      2: Enable TBE pipe_all, generate the operator CCE file and Python-CCE "
            "mapping file (.json), and enable the CCE compiler -O0-g;\n"
            "                      3: Disable debug, and keep generating kernel file (.o and .json);\n"
            "  --op_debug_config   Set op debug config .\n"
            "                      support (oom, dump_cce, dump_bin, dump_loc, ccec_O0, ccec_g, \n"
            "                               check_flag, sanitizer)\n"
            "  --op_mode           Set op mode (Options are dynamic, static and pre-static). \n"
            "  --optional_input_mode       Set optional input mode (gen_placeholder or no_placeholder). \n"
            "                              Default: no_placeholder.\n"
            "  --optional_output_mode       Set optional input mode (gen_placeholder or no_placeholder). \n"
            "                              Default: no_placeholder.\n"
            "  --simplified_key_mode       Set how to generate simplified_key. \n"
            "                              None(default): Don't generate simplified_key.\n"
            "                              0: generate simplified_key, simple mode\n"
            "                              1: generate simplified key, compitable mode\n"
            "  --dynamic_param_mode        Whether to add count for dynamic input/output in simplified key. \n"
            "                              None: default\n"
            "                              unfolded: add count\n"
            "                              folded_with_desc: with shape, don't add count\n"
            "  --tiling_key        Set tiling key list for op, default is None. \n"
            "                      For expl: --tiling_key=1,2,3,4  \n"
            "  --op_relocatable_kernel_binary    Set relocatable bin mode. \n"
            "                      support(True, true, False, false)\n"
            "                      if not set, use default False. \n"
            "  --op_super_kernel_options         Set super kernel options. \n"
            "                      For expl: --op_super_kernel_options=aaa,bbb \n"
            "                      if not set, use empty str as default. \n"
            "  --kernel-template-input         Set specific kernel compilation. \n"
            "  --kernel-spec                   Generate static compile resources. \n"
            "                                  Support None(default), Normal and SK. \n"
            "  --kernel-spec-dir               Set the kernel spec output directory. \n"
        )

    @staticmethod
    def check_file_valid(file_path):
        """
        check whether the file exists or not.
        file path could be a relative path.
        :param file_path:
        :return:
        """
        abs_path = os.path.realpath(file_path)
        logger.debug("absolute json file path is %s.", abs_path)
        return os.path.isfile(abs_path)

    @staticmethod
    def check_dir_valid(dir_path):
        """
        check whether the directory exists or not.
        dir path could be a relative path.
        :param dir_path:
        :return:
        """
        abs_path = os.path.realpath(dir_path)
        logger.debug("absolute output dir path is %s.", abs_path)
        return os.path.exists(abs_path)

    @staticmethod
    def set_log_level(log_level_str):
        """
        Set the environment argument of log.
        :param log_level_str:
        :return:
        """
        log_level = log_level_dict.get(log_level_str, 3)
        ascend_logger = AscendLog()
        # 57 is TBE, 8 is CCE
        ascend_logger.set_level(57, log_level, 0)
        ascend_logger.set_level(8, log_level, 0)

    @staticmethod
    def strip_key(key):
        """
        original key is --xxx or -x, we get rid of -- or - in this function
        :param key: input key of option
        :return:
        """
        length = len(key)
        if length == 2:
            ret_key = key[1:]
        elif length >= 3:
            ret_key = key[2:]
        else:
            ret_key = key

        return ret_key

    def __parse(self, opts):
        """
        parse options
        """
        help_only = False
        for key, value in opts:
            striped_key = self.strip_key(key)
            if striped_key in {
                OpcOptions.AICORE_NUM,
                OpcOptions.OP_DEBUG_LEVEL,
                OpcOptions.SIMPLE_KEY_MODE,
            }:
                if not value.isdigit():
                    logger.error("Invalid params: %s should be digit.", striped_key)
                    sys.exit(1)
                else:
                    value = int(value)
            if striped_key in compiling_option_keys:
                self.__compiling_options[striped_key] = value

            try:
                self.__full_options[key].set_value(value)
            except KeyError as e:
                logger.error("%s", e)
            finally:
                pass
            logger.debug("%s = %s", key, str(value))
            if key in ("-h", "--help"):
                help_only = True
            if key == "--log":
                self.set_log_level(value)

        if help_only:
            self.usage()
            sys.exit(1)

    def parse_build_options(self, build_options):
        """
        parse build_options.
        :return:
        """
        for key, value in OpcOptions.OpcOptionDefaultValueDict.items():
            self.__set_option(key, value)

        for key, value in build_options.items():
            if key not in compiling_option_keys:
                logger.error("build_options %s not supported.", key)
                continue
            else:
                self.__set_option(key, value)

            try:
                self.__full_options[key].set_value(value)
            except KeyError as e:
                logger.warn("%s", e)
            finally:
                pass
            logger.debug("%s = %s", key, str(value))
        return True

    def parse_options(self):
        """
        parse options using getopt.
        __compiling_options are those will be passed to the
        down layer compiler
        :return:
        """
        if len(sys.argv) <= 1:
            logger.error("Args is empty. Please enter correct compiling arguments.")
            self.usage()
            return FAILED

        if len(sys.argv) == 2:
            arg = sys.argv[1]
            if arg in ("-h", "--h", "--help"):
                self.usage()
                return FAILED
        start_opt = 1
        if ".py" in sys.argv[1]:
            self.__compiling_options[OpcOptions.OP_PATH] = sys.argv[1]
            self.__full_options["--op_path"] = sys.argv[1]
            start_opt = 2

        try:
            opts, _ = getopt.getopt(
                sys.argv[start_opt:], self.__short_options, self.__long_options
            )
        except getopt.GetoptError as err:
            logger.error("Failed to get option. %s", err)
            self.usage()
            return FAILED
        finally:
            pass

        self.__parse(opts)
        return SUCCESS

    def __check_and_create_dir(self, directory):
        """
        If the dir is empty, use current directory.
        If the dir does not exist, create it.
        :param directory: directory to check
        :return: True of False
        """
        if directory == "":
            return True, os.path.realpath(".")

        output_dir_valid_flag = self.check_dir_valid(directory)
        abs_dir = os.path.realpath(directory)
        if output_dir_valid_flag is not True:
            logger.debug("The dir %s is not exist.", abs_dir)
            try:
                os.makedirs(abs_dir, 750)
            except OSError as error:
                logger.error("%s", str(error))
                return False, ""
            finally:
                pass
        return True, abs_dir

    def __check_and_update_file_path(self, option):
        """
        check and update the file with absolute path
        :param option:
        :return:
        """
        path = self.get_option(option)
        logger.debug("path = %s", path)
        path_valid_flag = self.check_file_valid(path)
        if path == "" or path_valid_flag is False:
            logger.error("The %s: %s is not exist.", option, path)
            return False
        abs_path = os.path.realpath(path)
        self.__set_option(option, abs_path)
        return True

    def __check_and_update_dir(self, option):
        """
        check and update the dir with absolute path
        :param option:
        :return:
        """
        directory = self.get_option(option)
        ret, abs_path = self.__check_and_create_dir(directory)
        if not ret:
            return False
        logger.debug("set abs dir path %s for %s.", abs_path, option)
        self.__set_option(option, abs_path)
        return True

    def check_opt_param_mode(self):
        """
        if simplified_key_mode is 0, set OptionalInOutMode is gen_placeholder
        if simplifile_key_mode not set. set default OptionalInOutMode is no_placeholder.
        """
        simplified_key_mode = self.get_option(OpcOptions.SIMPLE_KEY_MODE)
        if simplified_key_mode == 0:
            self.set_option(
                OpcOptions.OPTIONAL_INPUT_MODE, OptionalInOutMode.GEN_PLACEHOLDER
            )
            self.set_option(
                OpcOptions.OPTIONAL_OUTPUT_MODE, OptionalInOutMode.GEN_PLACEHOLDER
            )
            logger.warn(
                "simplified_key_mode is [%d], set optional param mode gen_placeholder.",
                simplified_key_mode,
            )
            return
        opt_input_mode = self.get_option(OpcOptions.OPTIONAL_INPUT_MODE)
        if opt_input_mode not in {
            OptionalInOutMode.DEFAULT,
            OptionalInOutMode.GEN_PLACEHOLDER,
            None,
        }:
            logger.warn(
                "Invalid optional_input_mode[%s], will be ignored.", str(opt_input_mode)
            )
            self.set_option(OpcOptions.OPTIONAL_INPUT_MODE, None)
        opt_output_mode = self.get_option(OpcOptions.OPTIONAL_OUTPUT_MODE)
        if opt_output_mode not in {
            OptionalInOutMode.DEFAULT,
            OptionalInOutMode.GEN_PLACEHOLDER,
            None,
        }:
            logger.warn(
                "Invalid optional_output_mode[%s], will be ignored.",
                str(opt_output_mode),
            )
            self.set_option(OpcOptions.OPTIONAL_OUTPUT_MODE, None)

    def check_op_debug_config_valid(self):
        """
        check_op_debug_config_valid
        """
        op_debug_config = self.get_option(OpcOptions.OP_DEBUG_CONFIG)
        if op_debug_config is None:
            return False
        op_debug_tuple = (
            "oom",
            "dump_cce",
            "dump_bin",
            "dump_loc",
            "ccec_O0",
            "ccec_g",
            "check_flag",
            "sanitizer",
        )
        if op_debug_config == "":
            return True
        keep_kernel_meta_config = ("dump_cce", "dump_bin", "dump_loc")
        keep_kernel_meta = False
        op_debug_config_list = op_debug_config.split(",")
        for op_debug in op_debug_config_list:
            if op_debug.strip() not in op_debug_tuple:
                logger.error("op_debug {} is invalid.".format(op_debug))
                return False
            if op_debug.strip() in keep_kernel_meta_config:
                keep_kernel_meta = True
        self.set_option(OpcOptions.KEEP_DIR, keep_kernel_meta)
        logger.info("Set keep_kernel_meta_dir flag {}.".format(keep_kernel_meta))
        return True

    def check_and_save_tiling_key(self):
        """
        check_tiling_key
        """
        tiling_key_str = self.get_option(OpcOptions.TILING_KEY)
        if tiling_key_str is not None:
            tiling_key_list = list(filter(None, tiling_key_str.split(",")))
            if not tiling_key_list:
                logger.warn("Invalid tiling_key list {}.".format(tiling_key_str))
            self.set_option(OpcOptions.TILING_KEY, tiling_key_list)
            logger.info("Save tiling_key list {}.".format(tiling_key_list))

    def check_and_save_kernel_template_input(self):
        kernel_template_input_str = self.get_option(OpcOptions.KERNEL_TEMPLATE_INPUT)
        if kernel_template_input_str is not None:
            self.set_option(OpcOptions.KERNEL_TEMPLATE_INPUT, kernel_template_input_str)

    def check_op_relocatable_cfg(self):
        """
        check_op_relocatable_cfg
        """
        op_relocatable_bin = self.get_option(OpcOptions.RELOCATABLE_BIN)
        op_relocatable_bin_bool = (
            True if op_relocatable_bin in ("True", "true") else False
        )
        self.set_option(OpcOptions.RELOCATABLE_BIN, op_relocatable_bin_bool)
        logger.info("Save op_relocatable_bin {}.".format(op_relocatable_bin_bool))
        if not op_relocatable_bin_bool:
            return
        sub_kernel_option = self.get_option(OpcOptions.SPK_OPT)
        self.set_option(OpcOptions.SPK_OPT, sub_kernel_option)
        logger.info("Save op_super_kernel_options {}.".format(sub_kernel_option))

    def check_kernel_spec_cfg(self):
        """Validate Kernel Spec mode and normalize its publication directory."""

        kernel_spec = self.get_option(OpcOptions.KERNEL_SPEC)
        if kernel_spec not in {"None", "Normal", "SK"}:
            logger.error(
                "Invalid kernel-spec option {}, support None/Normal/SK.".format(
                    kernel_spec
                )
            )
            return False
        if kernel_spec == "None":
            return True

        if self.get_option(OpcOptions.RELOCATABLE_BIN):
            logger.error(
                "kernel-spec cannot be used with op_relocatable_kernel_binary."
            )
            return False

        kernel_spec_dir = self.get_option(OpcOptions.KERNEL_SPEC_DIR)
        if not kernel_spec_dir:
            logger.error("kernel-spec-dir is required when kernel-spec is enabled.")
            return False
        if not self.__check_and_update_dir(OpcOptions.KERNEL_SPEC_DIR):
            logger.error("Failed to check and update kernel spec output dir.")
            return False
        return True

    def check_op_compile_mode(self):
        if (
            self.get_option(OpcOptions.OP_COMPILE_MODE)
            == OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE
        ):
            # check op path: op path must be a valid path
            if self.get_option(
                OpcOptions.OP_PATH
            ) != "" and not self.__check_and_update_file_path(OpcOptions.OP_PATH):
                logger.error("Failed to check and update op_path.")
                return False
            # check input_param: input_param must be a valid path
            if not self.__check_and_update_file_path(OpcOptions.INPUT_PARAM):
                logger.error("Failed to check and update input_param.")
                return False
        else:
            if self.get_option(OpcOptions.BIN_FILENAME) == "":
                logger.error("bin_filename is empty.")
                return False
        return True

    def check_simplified_mode_params(self):
        simplified_key_mode = self.get_option(OpcOptions.SIMPLE_KEY_MODE)
        if simplified_key_mode not in {0, 1, None}:
            logger.error(
                "Invalid simplified_key_mode option {}, support 0, 1.".format(
                    simplified_key_mode
                )
            )
            return False

        dynamic_param_mode = self.get_option(OpcOptions.DYNAMIC_PARAM_MODE)
        if dynamic_param_mode not in {"unfolded", "folded_with_desc", None}:
            logger.error(
                "Invalid dynamic_param_mode option {}, \
                    support 'unfolded', 'folded_with_desc'".format(dynamic_param_mode)
            )
            return False
        return True

    def check_all_input_path(self):
        # use current directory if the parameter output_dir is not configured
        if not self.__check_and_update_dir(OpcOptions.OUTPUT):
            logger.error("Failed to check and update output dir.")
            return False

        # use current directory if the parameter debug_dir is not configured
        if not self.__check_and_update_dir(OpcOptions.DEBUG_DIR):
            logger.error("Failed to check and update debug dir.")
            return False

        return True

    def check_input_params(self):
        """
        check input arguments:
        """
        # check soc version: soc version must not be empty
        soc_version = self.get_option(OpcOptions.SOC_VERSION)
        if soc_version == "":
            logger.error("Soc version is empty.")
            return False
        if not self.check_all_input_path():
            return False

        if not self.check_op_compile_mode():
            return False

        impl_mode = self.get_option(OpcOptions.IMPL_MODE)
        if impl_mode is not None:
            res, _ = check_and_normalize_impl_mode(impl_mode)
            if not res:
                logger.info(
                    "impl mode {}, not in high_precision,high_performance.".format(
                        impl_mode
                    )
                )

        deterministic = self.get_option(OpcOptions.DETERMINISTIC)
        if deterministic not in {"all", "true", "false"}:
            logger.error(
                "Invalid deterministic option {}, support all/true/false.".format(
                    deterministic
                )
            )
            return False

        if not self.check_simplified_mode_params():
            return False

        core_type = self.get_option(OpcOptions.CORE_TYPE)
        if core_type not in {
            "AiCore",
            "VectorCore",
            None,
            "AiCore,VectorCore",
            "VectorCore,AiCore",
        }:
            logger.info("core type  is {}.".format(core_type))
            return False

        self.check_opt_param_mode()
        op_debug_level = self.get_option(OpcOptions.OP_DEBUG_LEVEL)
        if op_debug_level not in {0, 1, 2, 3}:
            logger.error(
                "Invalid op debug level {}, support values: 0 1 2 3.".format(
                    op_debug_level
                )
            )
            return False

        res = self.check_op_debug_config_valid()
        if not res:
            logger.error(
                "Invalid op debug config {}.".format(
                    self.get_option(OpcOptions.OP_DEBUG_CONFIG)
                )
            )
            return False

        self.check_and_save_tiling_key()
        self.check_op_relocatable_cfg()
        if not self.check_kernel_spec_cfg():
            return False
        self.check_and_save_kernel_template_input()
        return True


def parse_args():
    """
    parse all arguments from use's input
    :return:
    """
    print("Opc tool start working now, please wait for a moment.")

    opt_parser_temp = OpcOptionParser()
    opt_parser_temp.define_options()
    ret = opt_parser_temp.parse_options()
    return ret, opt_parser_temp


def op_compile_classify(opt_parser):
    """
    set op compile classify
    """
    graph_path = opt_parser.get_option(OpcOptions.GRAPH)
    if graph_path == "":
        opt_parser.set_option(
            OpcOptions.OP_COMPILE_MODE, OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE
        )
        logger.debug("set compile mode: %s.", OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE)
        return
    logger.error("Cannot set GRAPH.\n")


def main():
    """
    main function
    :return:
    """
    ret, opt_parser = parse_args()
    if ret is FAILED:
        return

    op_compile_classify(opt_parser)
    op_compile_mode = opt_parser.get_option(OpcOptions.OP_COMPILE_MODE)
    if op_compile_mode == OpcCompileMode.UNKOWN_MODE:
        raise RuntimeError(
            "Input command error! opc confuse single or fusion op compile."
        )

    if not opt_parser.check_input_params():
        logger.error("Opc tool compile failed.")
        sys.exit(1)

    load_op_info_store(opt_parser.get_option(OpcOptions.SOC_VERSION))
    if op_compile_mode != OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE:
        raise RuntimeError("OP_COMPILE_MODE is incorrect!")

    op_compile = OpCompilation(opt_parser.get_all_options())
    if op_compile.op_compilation():
        logger.info("Opc tool compile success.")
    else:
        logger.error("Opc tool compile failed.")
        sys.exit(1)


if __name__ == "__main__":
    main()
