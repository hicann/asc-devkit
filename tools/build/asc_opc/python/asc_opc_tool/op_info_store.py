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
Parse ops-info and store op info
"""

import os
import json
from pathlib import Path
from asc_op_compile_base.common.platform import platform_info
from asc_op_compile_base.common.utils import log as logger
from constant import OpParamType, AttrtypeMapDict
from opc_common import read_json_file


class InOrOutputInfo:
    def __init__(self, inout_i):
        self.param_type = OpParamType.DEFAULT
        self.inout_i = inout_i
        self.name = ""


class AttrInfo:
    def __init__(self, attr_name):
        self.param_type = OpParamType.DEFAULT
        self.name = attr_name
        self.default_value = None
        self.type = None
        self.index = 0
        self.value = None


class OpKernelInfo:
    def __init__(self, op_type):
        self.init_flag = False  # type: bool
        self.op_type = op_type  # type: str
        self.op_imp_path = ""  # type:str
        self.op_pattern = []
        self.is_support_dynamic_shape = False
        self.is_support_dynamic_rank = False
        self.input_mem_continues = False
        self.output_mem_continues = False
        self.core_type = []
        self.op_info = {}
        self.is_heavy_o_p = False  # type bool
        self.input_infos_ = []  # type: list[InOrOutputInfo]
        self.output_infos_ = []  # type: list[InOrOutputInfo]
        self.attr_infos_ = []  # type: list[attr]
        self.enable_vector_core = False

    @staticmethod
    def get_str_from_op_content(op_type, op_content, key1, key2):
        key1_pos = op_content.get(key1)
        if key1_pos is None:
            if key1 != "reshapeType" and key2 != "defaultValue":
                logger.debug("Op {} not found {} in OpContent!".format(op_type, key1))
            return False, key1_pos

        key2_pos = key1_pos.get(key2)
        if key2_pos is None:
            if key1 != "reshapeType" and key2 != "defaultValue":
                logger.debug(
                    "Op {} not found {}.{} in OpContent!".format(op_type, key1, key2)
                )
            return False, key2_pos

        return True, key2_pos

    @staticmethod
    def get_inout_flags(op_content, input_list, output_list):
        for key, _ in op_content.items():
            if key.startswith("input") or key.startswith("Input"):
                input_list.append(key)
            elif key.startswith("output") or key.startswith("Output"):
                output_list.append(key)

    @staticmethod
    def get_attr_list(op_type, op_content, attr_list):
        if "attr" not in op_content:
            return
        attrs = op_content.get("attr")
        if not isinstance(attrs, dict):
            logger.error("op_type: {} has wrong type. {}".format(op_type, str(attrs)))
            return
        attr_str = attrs.get("list", "")
        split_list = attr_str.split(",")
        logger.debug("Op {} attr_str in OpContent is {}".format(op_type, attr_str))
        for attr in split_list:
            attr_list.append(attr)

    def __feed_inout_info(self, op_content, inout_list, op_kernel_info, is_input):
        def sort_input_key(x):
            return int(x[5:])

        def sort_output_key(x):
            return int(x[6:])

        inout_list.sort(key=sort_input_key) if is_input else inout_list.sort(
            key=sort_output_key
        )
        inout_infos = list()
        for inouti in inout_list:
            inout_info = InOrOutputInfo(inouti)
            inout_content = op_content.get(inouti, None)
            if inout_content is None:
                logger.error("wrong key %s.", inouti)
                return False
            inout_info.param_type = inout_content.get("paramType", None)
            inout_info.name = inout_content.get("name", None)
            inout_infos.append(inout_info)
        if is_input:
            op_kernel_info.input_infos_ = inout_infos
        else:
            op_kernel_info.output_infos_ = inout_infos
        return True

    def parse_input_and_output_from_content(self, op_type, op_content, op_kernel_info):
        input_list = list()
        output_list = list()
        self.get_inout_flags(op_content, input_list, output_list)

        if not self.__feed_inout_info(op_content, input_list, op_kernel_info, True):
            logger.error("op_type:%s has wrong input key.", op_type)
            return False
        if not self.__feed_inout_info(op_content, output_list, op_kernel_info, False):
            logger.error("op_type:%s has wrong output key.", op_type)
            return False
        return True

    @staticmethod
    def get_default_attr_value(param_type, default_value):
        if default_value is None:
            return None
        if param_type == "int":
            return int(default_value)
        return default_value

    def parse_attrs_from_content(self, op_type, op_content, op_kernel_info):
        attrs = list()
        self.get_attr_list(op_type, op_content, attrs)

        attr_infos = list()
        for index, attri in enumerate(attrs):
            attr_name = "attr_" + attri
            attr_info = AttrInfo(attri)
            attr_content = op_content.get(attr_name, None)
            if attr_content is None:
                logger.error("op_type:%s has wrong key %s.", op_type, attr_name)
                return False
            attr_info.index = index
            attr_info.param_type = attr_content.get("paramType", None)
            attr_info.type = attr_content.get("type", None)
            attr_type = AttrtypeMapDict.get(attr_info.type, None)
            if attr_type is None:
                logger.warn("attr_info.type:%s may not supported.", attr_info.type)
            else:
                attr_info.type = attr_type
            attr_info.default_value = self.get_default_attr_value(
                attr_info.type, attr_content.get("defaultValue", None)
            )
            attr_info.value = attr_content.get("value", None)
            attr_infos.append(attr_info)
            logger.debug(
                "op_type: {} add attr_info: {}.".format(op_type, attr_info.name)
            )
        op_kernel_info.attr_infos_ = attr_infos
        return True

    def parse_basic_parameter(self, op_type, op_content, op_kernel_info):
        op_pattern_dict = {
            "formatAgnostic": 0,
            "broadcast": 1,
            "reduce": 2,
            "dynamic": 3,
        }

        # parse the op.pattern of the op
        result, op_pattern_str = self.get_str_from_op_content(
            op_type, op_content, "op", "pattern"
        )
        if result and op_pattern_str is not None:
            op_pattern_iter = op_pattern_dict.get(op_pattern_str)
            if op_pattern_iter is not None:
                op_kernel_info.op_pattern.append(op_pattern_iter)

        # parse the imp_path.path of the op
        result, op_imp_path_str = self.get_str_from_op_content(
            op_type, op_content, "imp_path", "path"
        )
        res_status = result and op_imp_path_str is not None
        if res_status:
            op_kernel_info.op_imp_path = op_imp_path_str

        # parse the dynamic_format.flag of the op
        dynamic_format_str = ""
        result, dynamic_format_str = self.get_str_from_op_content(
            op_type, op_content, "dynamicFormat", "flag"
        )
        res_status = (
            result
            and dynamic_format_str is not None
            and dynamic_format_str.lower() == "true"
        )
        if res_status:
            op_kernel_info.op_pattern.append(op_pattern_dict.get("dynamic"))

        # parse the dynamicCompileStatic.flag of the op
        result, dynamic_compile_static_str = self.get_str_from_op_content(
            op_type, op_content, "dynamicCompileStatic", "flag"
        )
        res_status = result and dynamic_compile_static_str is not None
        if res_status:
            op_kernel_info.dynamic_compile_static = dynamic_compile_static_str
            logger.debug(
                "op_type:{} support dynamic_compile_static.".format(
                    op_kernel_info.op_type
                )
            )
        else:
            op_kernel_info.dynamic_compile_static = "false"
            logger.debug(
                "op_type:{} not support dynamic_compile_static.".format(
                    op_kernel_info.op_type
                )
            )

        # parse the dynamic_shape_support of the op
        result, dynamic_shape_support_str = self.get_str_from_op_content(
            op_type, op_content, "dynamicShapeSupport", "flag"
        )
        res_status = (
            result
            and dynamic_shape_support_str is not None
            and dynamic_shape_support_str.lower() == "true"
        )
        if res_status:
            op_kernel_info.is_support_dynamic_shape = True

        return True

    def parse_basic_parameter_arg(self, op_type, op_content, op_kernel_info):
        kcore_type_dict = {
            "Aicore": 0,
            "VectorCore": 1,
            "Mix": 2,
            "mix": 2,
            "Dynamic": 3,
            "dynamic": 3,
        }

        # parse the dynamic_rank_support of the op
        result, dynamic_rank_support_str = self.get_str_from_op_content(
            op_type, op_content, "dynamicRankSupport", "flag"
        )
        res_status = (
            result
            and dynamic_rank_support_str is not None
            and dynamic_rank_support_str.lower() == "true"
        )
        if res_status:
            logger.debug(
                "op_type:{} is support dynamic rank.".format(op_kernel_info.op_type)
            )
            op_kernel_info.is_support_dynamic_rank = True

        # parse the input_mem_continues.flag
        result, input_mem_continues_str = self.get_str_from_op_content(
            op_type, op_content, "inputMemContinues", "flag"
        )
        res_status = (
            result
            and input_mem_continues_str is not None
            and input_mem_continues_str.lower() == "true"
        )
        if res_status:
            op_kernel_info.input_mem_continues = True

        # parse the out_mem_continues.flag
        result, output_mem_continues_str = self.get_str_from_op_content(
            op_type, op_content, "outputMemContinues", "value"
        )
        res_status = (
            result
            and output_mem_continues_str is not None
            and output_mem_continues_str.lower() == "true"
        )
        if res_status:
            op_kernel_info.output_mem_continues = True

        # parse the core_type
        result, core_type_str = self.get_str_from_op_content(
            op_type, op_content, "coreType", "flag"
        )
        core_type_iter = kcore_type_dict.get(core_type_str)
        if core_type_iter is not None:
            op_kernel_info.core_type.append(core_type_iter)

        # parse the enable_vector_core of the op
        result, enable_vector_core_str = self.get_str_from_op_content(
            op_type, op_content, "enableVectorCore", "flag"
        )
        res_status = (
            result
            and enable_vector_core_str is not None
            and enable_vector_core_str.lower() == "true"
        )
        if res_status:
            logger.debug(
                "op_type:{} is support customized vector core.".format(
                    op_kernel_info.op_type
                )
            )
            op_kernel_info.enable_vector_core = True

        return True

    def init_op_info(self, op_type, op_content, op_kernel_info):
        """
        parse op_info
        """
        op_kernel_info.op_info["flagPartial"] = False
        op_kernel_info.op_info["flagAsync"] = False
        op_kernel_info.op_info["computeCost"] = 10

        # parse op_file
        result, op_file = self.get_str_from_op_content(
            op_type, op_content, "opFile", "value"
        )
        if result and op_file is not None:
            logger.debug("Op {} get op_file value is {}.".format(op_type, op_file))
            op_kernel_info.op_info["opFileName"] = op_file
        else:
            logger.debug("Op {} can't {} get op_file value".format(op_type, op_type))
            op_kernel_info.op_info["opFileName"] = ""

        # parse op_func
        result, op_interface = self.get_str_from_op_content(
            op_type, op_content, "opInterface", "value"
        )
        if result and op_interface is not None:
            logger.debug(
                "Op {} get op_interface value is {}.".format(op_type, op_interface)
            )
            op_kernel_info.op_info["opFuncName"] = op_interface
        else:
            logger.debug(
                "Op {} can't {} get op_interface value".format(op_type, op_type)
            )
            op_kernel_info.op_info["opFuncName"] = ""

        # parse op_impl_switch
        result, op_impl_switch = self.get_str_from_op_content(
            op_type, op_content, "opImplSwitch", "value"
        )
        if result and op_impl_switch is not None:
            logger.debug(
                "Op {} get op_impl_switch value is {}.".format(op_type, op_impl_switch)
            )
            op_kernel_info.op_info["opImplSwitch"] = op_impl_switch

        return True

    def initialize_op_kernel_info(self, op_type, op_content, op_kernel_info):
        """
        parse single op
        """
        if op_kernel_info.init_flag:
            logger.debug("op_kernel_info has been initialized.")
            return True
        op_kernel_info.op_type = op_type

        if not self.parse_basic_parameter(op_type, op_content, op_kernel_info):
            logger.debug("parse basic parameter did not succeed.")
            return False

        if not self.parse_basic_parameter_arg(op_type, op_content, op_kernel_info):
            logger.debug("parse basic parameter arg did not succeed.")
            return False

        if not self.init_op_info(op_type, op_content, op_kernel_info):
            logger.debug("init op info did not succeed.")
            return False

        if not self.parse_input_and_output_from_content(
            op_type, op_content, op_kernel_info
        ):
            logger.debug("init op input info did not succeed.")
            return False

        if not self.parse_attrs_from_content(op_type, op_content, op_kernel_info):
            logger.debug("init attrs did not succeed.")
            return False
        op_kernel_info.init_flag = True
        return True


class SubOpInfoStore:
    """
    save op store info with instance
    """

    op_kernel_info_dict = {}
    op_builtin_info_dict = {}
    op_custom_info_list = []
    op_vendor_info_list = []

    def __init__(self):
        pass

    # singletom pattern
    def __new__(cls, *args, **kwargs):
        _ = args
        _ = kwargs
        if not hasattr(cls, "_instance"):
            cls._instance = object.__new__(cls)
        return cls._instance

    def set_op_content(self, op_builtin_info_dict):
        """
        set_op_content
        """
        self.op_builtin_info_dict = op_builtin_info_dict

    def set_op_custom(self, op_custom_list):
        """
        set_op_custom
        """
        self.op_custom_info_list = op_custom_list

    def set_op_vendors(self, op_vendor_list):
        """
        set_op_vendors
        """
        self.op_vendor_info_list = op_vendor_list

    def construct_op_kernel_info(self, op_type):
        """
        parse compute op
        op select priority: SCEND_CUSTOM_OPP_PATH > vendors > built-in
        """
        for op_custom_dict in self.op_custom_info_list:
            if not isinstance(op_custom_dict, dict):
                logger.warn("{} is not dict type.".format(op_custom_dict))
                continue

            if self.construct_op_info_from_dict(op_type, op_custom_dict):
                logger.debug("op_type:{} is find in custom path.".format(op_type))
                return True

        for op_vendor in self.op_vendor_info_list:
            if not isinstance(op_vendor, dict):
                logger.warn("{} is not dict type.".format(op_vendor))
                continue
            if self.construct_op_info_from_dict(op_type, op_vendor):
                logger.debug("op_type:{} is find in vendor path.".format(op_type))
                return True

        return self.construct_op_info_from_dict(op_type, self.op_builtin_info_dict)

    def construct_op_info_from_dict(self, op_type, op_content_dict):
        """
        parse compute op
        """
        op_content = op_content_dict.get(op_type)
        op_kernel_info = OpKernelInfo(op_type)
        if op_content is None:
            logger.debug("op_type:{} is not exist.".format(op_type))
            return False
        else:
            if not op_kernel_info.initialize_op_kernel_info(
                op_type, op_content, op_kernel_info
            ):
                logger.debug(
                    "opKernelInfo {} initialize did not succeed.".format(op_type)
                )
                return False

            logger.debug("opKernelInfo {} initialize success.".format(op_type))
            self.op_kernel_info_dict[op_type] = op_kernel_info
            return True


class OpPathParse(object):
    """
    parse Ascend path and store
    """

    custom_opp_path_list = []
    vendors_opp_path_list = []

    def __init__(self):
        pass

    # singletom pattern
    def __new__(cls, *args, **kwargs):
        _ = args
        _ = kwargs
        if not hasattr(cls, "_instance"):
            cls._instance = object.__new__(cls)
        return cls._instance

    def start_parse(self):
        """
        generate_custom_opp_list
        generate_vendor_opp_list
        """
        self.generate_custom_opp_list()
        self.generate_vendor_opp_list()

    def generate_custom_opp_list(self):
        """
        generate_custom_opp_list
        """
        custom_opp_path = os.getenv("ASCEND_CUSTOM_OPP_PATH")

        if custom_opp_path:
            if ":" not in custom_opp_path:
                self.custom_opp_path_list.append(custom_opp_path.strip())
            else:
                self.custom_opp_path_list = custom_opp_path.split(":")
                self.custom_opp_path_list = [
                    opp_path.strip() for opp_path in self.custom_opp_path_list
                ]

        for index, custom in enumerate(self.custom_opp_path_list):
            logger.debug("index: {} custom_opp_path: {}".format(index, custom))

    def generate_vendor_opp_list(self):
        """
        generate_custom_opp_list
        """
        ascend_opp_path = os.getenv("ASCEND_OPP_PATH")
        config_path = "{}/vendors/config.ini".format(ascend_opp_path)

        content = ""
        if Path(config_path).is_file():
            with open(config_path, "r") as f:
                for line in f.readlines():
                    if "load_priority" in line:
                        content = line
                f.close()

            logger.debug("load_priority content is: {}.".format(content))
            if content.strip():
                custom_str = content.split("=")
                if "," in custom_str[1].lower():
                    custom_list = custom_str[1].split(",")
                    custom_list = [opp_path.strip() for opp_path in custom_list]
                    for custom in custom_list:
                        curr_custom = "{}/vendors/{}".format(ascend_opp_path, custom)
                        if curr_custom not in self.vendors_opp_path_list:
                            self.vendors_opp_path_list.append(curr_custom.strip())
                        else:
                            logger.debug("{}'s already exsited.".format(curr_custom))
                else:
                    curr_custom = "{}/vendors/{}".format(ascend_opp_path, custom_str[1])
                    self.vendors_opp_path_list.append(curr_custom.strip())

        for index, custom in enumerate(self.vendors_opp_path_list):
            logger.debug("index: {} custom_opp_path: {}.".format(index, custom))

    def get_custom_opp_path_list(self):
        """
        get_custom_opp_path_list
        """
        return self.custom_opp_path_list

    def get_vendors_opp_path_list(self):
        """
        get_vendors_opp_path_list
        """
        return self.vendors_opp_path_list


def load_set_op_content(json_path):
    op_builtin_info_dict = dict()
    try:
        with open(json_path, "r") as file_in:
            op_builtin_info_dict = json.load(file_in)
    except Exception as e:
        logger.warn("load file[%s] failed, reason: %s.", json_path, str(e))
    finally:
        pass
    SubOpInfoStore().set_op_content(op_builtin_info_dict)


def load_op_info_store(soc_version):
    platform_info.set_current_compile_soc_info(soc_version)
    ascend_opp_path = os.getenv("ASCEND_OPP_PATH")
    short_soc_version = platform_info.get_soc_spec("SHORT_SOC_VERSION").lower()
    json_path = "{}/built-in/op_impl/ai_core/tbe/config/{}/aic-{}-ops-info.json".format(
        ascend_opp_path, short_soc_version, short_soc_version
    )
    logger.debug("json_path is {}.".format(json_path))
    if not isinstance(json_path, str) or not os.path.exists(json_path):
        json_dir = os.getenv("ASCEND_OPP_INFO_DIR")
        json_path = (
            os.path.join(json_dir, os.path.basename(json_path))
            if isinstance(json_dir, str)
            else json_path
        )
    load_set_op_content(json_path)

    OpPathParse().start_parse()
    custom_opp_path_list = OpPathParse().get_custom_opp_path_list()
    if custom_opp_path_list:
        custom_dict_list = []
        for custom_opp_path in custom_opp_path_list:
            custom_opp_json_path = (
                "{}/op_impl/ai_core/tbe/config/{}/aic-{}-ops-info.json".format(
                    custom_opp_path, short_soc_version, short_soc_version
                )
            )
            custom_dict = read_json_file(custom_opp_json_path)
            if None:
                logger.debug("json_path is {}.".format(json_path))
                continue
            custom_dict_list.append(custom_dict)
        SubOpInfoStore().set_op_custom(custom_dict_list)

    vendors_opp_path_list = OpPathParse().get_vendors_opp_path_list()
    if vendors_opp_path_list:
        vendor_dict_list = []
        for vendor_opp_path in vendors_opp_path_list:
            vendor_opp_json_path = (
                "{}/op_impl/ai_core/tbe/config/{}/aic-{}-ops-info.json".format(
                    vendor_opp_path, short_soc_version, short_soc_version
                )
            )
            vendor_dict = read_json_file(vendor_opp_json_path)
            if None:
                logger.debug("json_path is {}.".format(json_path))
                continue
            vendor_dict_list.append(vendor_dict)
        SubOpInfoStore().set_op_vendors(vendor_dict_list)
