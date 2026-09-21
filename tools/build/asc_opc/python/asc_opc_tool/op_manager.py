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
op manager
"""

import importlib
from importlib import util
import copy
import os
import sys
from pathlib import Path
import asc_op_compile_base.common.register as tbe_register
from asc_op_compile_base.common.utils import log as logger
from constant import OpcOptions
from opc_common import normalize_func_name, get_file_real_path, LogLevel, opc_log_full
from op_info_store import SubOpInfoStore, OpPathParse

MIDDLE_PATH_LIST = ("op_impl/ai_core/tbe", "op_impl/vector_core/tbe")


def op_register_get_func(sub_op_info_store, op_type, impl_type):
    """
    query the operator information base and finally return op func
    """
    result = sub_op_info_store.construct_op_kernel_info(op_type)
    if result:
        op_kernel_info = sub_op_info_store.op_kernel_info_dict.get(op_type)
        if op_kernel_info is not None:
            op_file_name = op_kernel_info.op_info.get("opFileName")
            op_func_name = op_kernel_info.op_info.get("opFuncName")
            if op_file_name != "":
                op_path = "{}.{}".format(impl_type, op_file_name)
                opm = importlib.import_module(op_path)
                if op_func_name != "":
                    return getattr(opm, op_func_name)
                else:
                    op_func = normalize_func_name(op_type)
                    return getattr(opm, op_func)
            else:
                op_path = "{}.{}".format(impl_type, normalize_func_name(op_type))
                opm = importlib.import_module(op_path)
                if op_func_name != "":
                    return getattr(opm, op_func_name)
                else:
                    op_func = normalize_func_name(op_type)
                    return getattr(opm, op_func)
        else:
            logger.debug("{}'s op_kernel_info is null.".format(op_type))
            return None

    else:
        logger.debug("[Graph] Unable to parse the operator information")
        return None


def get_inout_info_from_opstore(op_type):
    """
    get_inout_info_from_opstore
    """
    result = SubOpInfoStore().construct_op_kernel_info(op_type)
    if not result:
        logger.warn("Op {} is not found in opstore.".format(op_type))
        return None, None
    op_kernel_info = SubOpInfoStore().op_kernel_info_dict.get(op_type)
    if op_kernel_info is None:
        logger.warn("Op {} kernel_info is None.".format(op_type))
        return None, None
    return op_kernel_info.input_infos_, op_kernel_info.output_infos_


def get_attr_info_from_opstore(op_type):
    """
    get_attr_info_from_opstore
    """
    result = SubOpInfoStore().construct_op_kernel_info(op_type)
    if not result:
        logger.warn("Op {} is not found in opstore.".format(op_type))
        return None
    op_kernel_info = SubOpInfoStore().op_kernel_info_dict.get(op_type)
    if op_kernel_info is None:
        logger.warn("Op {} kernel_info is None.".format(op_type))
        return None
    return op_kernel_info.attr_infos_


def get_op_impl_switch_from_opstore(op_type):
    """
    get_op_impl_switch_from_opstore
    """
    result = SubOpInfoStore().construct_op_kernel_info(op_type)
    if not result:
        logger.warn("Op {} is not found in opstore.".format(op_type))
        return None
    op_kernel_info = SubOpInfoStore().op_kernel_info_dict.get(op_type)
    if op_kernel_info is None:
        logger.warn("Op {} kernel_info is None.".format(op_type))
        return None
    return op_kernel_info.op_info.get("opImplSwitch", None)


def get_enable_vector_core_from_opstore(op_type):
    """
    get_enable_vector_core_from_opstore
    """
    result = SubOpInfoStore().construct_op_kernel_info(op_type)
    if not result:
        logger.warn("Op {} is not found in opstore.".format(op_type))
        return None
    op_kernel_info = SubOpInfoStore().op_kernel_info_dict.get(op_type)
    if op_kernel_info is None:
        logger.warn("Op {} kernel_info is None.".format(op_type))
        return None
    return op_kernel_info.enable_vector_core


def get_dynamic_compile_static_from_opstore(op_type):
    """
    get_dynamic_compile_static_from_opstore
    """
    result = SubOpInfoStore().construct_op_kernel_info(op_type)
    if not result:
        logger.warn("Op {} is not found in opstore.".format(op_type))
        return None
    op_kernel_info = SubOpInfoStore().op_kernel_info_dict.get(op_type)
    if op_kernel_info is None:
        logger.warn("Op {} kernel_info is None.".format(op_type))
        return None
    return op_kernel_info.dynamic_compile_static


def get_dynamic_compile_static(op_type, op_info):
    """
    get_dynamic_compile_static
    """
    dynamic_compile_static = get_dynamic_compile_static_from_opstore(op_type)
    logger.debug("Op {} dynamic_compile_static is {}.".format(op_type, dynamic_compile_static))

    if dynamic_compile_static == "tune":
        dynamic_compile_static_update, _ = get_dynamic_compile_static_from_kb(op_type, op_info)
        if dynamic_compile_static_update not in {"true", "false", None}:
            logger.error("Op {} dynamic_compile_static {} invalid.".format(op_type, dynamic_comiple_static))
            return None
        elif dynamic_compile_static_update is None:
            return dynamic_compile_static
        else:
            logger.debug("{}'s dynamic_compile_static update to {}.".format(op_type, dynamic_compile_static_update))
            return dynamic_compile_static_update
    else:
        return dynamic_compile_static


def get_op_impl_switch(op_type, op_info):
    """
    get_op_impl_switch
    """
    op_impl_switch = get_op_impl_switch_from_opstore(op_type)
    if op_impl_switch:
        lst = op_impl_switch.split(",")
        if len(lst) > 1:
            _, op_impl_switch = get_dynamic_compile_static_from_kb(op_type, op_info)
            return op_impl_switch

    logger.debug("{}'s op_impl_switch is {}.".format(op_type, op_impl_switch))
    return op_impl_switch


def get_mode_name_from_vendors_path(vendor_path):
    """
    get_mode_name_from_vendors_path
    """
    index = vendor_path.find("vendors/") + len("vendors/")
    op_mode_name = vendor_path[index:]
    op_mode_name = op_mode_name + "_impl"
    logger.info("vendor_path is {}, op_mode_name is {}.".format(vendor_path, op_mode_name))
    return op_mode_name


def find_mode_file_from_custom(op_type, custom_opp_path_list):
    """
    find_mode_name_op_py_file
    """
    op_type_name = normalize_func_name(op_type)
    # In the custom_opp_path_list header, the priority is the highest
    for op_path_custom in custom_opp_path_list:
        index = op_path_custom.rfind("/") + 1
        op_mode_name = op_path_custom[index:]
        if not op_mode_name:
            logger.info("{} find op op_mode_name from {} is None.".format(op_type, op_path_custom))
            continue
        logger.info("op {} op_mode_name is {}.".format(op_type, op_mode_name))
        for middle_path in MIDDLE_PATH_LIST:
            middle_path = "{}/{}".format(middle_path, op_mode_name)
            py_module_path = "{}/{}".format(op_path_custom, middle_path)
            op_py_file = get_file_real_path(op_path_custom, op_type_name, "py", middle_path)
            logger.debug("op: {} op file is {}, py_module_path is {}.".format(op_type, op_py_file, py_module_path))
            if op_py_file is not None and Path(op_py_file).is_file():
                ogger.debug("op: {} op file is {}.".format(op_type, op_py_file))
                if py_module_path not in sys.path:
                    logger.debug("op: {} add py_module_path is {}.".format(op_type, py_module_path))
                    sys.path.append(py_module_path)
                return op_mode_name, op_py_file

    return None, None


def find_mode_file_from_vendors(op_type, vendors_opp_path_list):
    """
    find_mode_file_from_vendors
    """
    op_type_name = normalize_func_name(op_type)
    # In the custom_opp_path_list header, the priority is the highest
    for op_path_custom in vendors_opp_path_list:
        op_mode_name = get_mode_name_from_vendors_path(op_path_custom)
        if not op_mode_name:
            logger.debug("{} find op op_mode_name from {} is None.".format(op_type, op_path_custom))
            continue
        logger.debug("op {} op_mode_name is {}.".format(op_type, op_mode_name))
        for middle_path in MIDDLE_PATH_LIST:
            py_module_path = "{}/{}".format(op_path_custom, middle_path)
            middle_file_path = "{}/{}".format(middle_path, op_mode_name)
            op_py_file = get_file_real_path(op_path_custom, op_type_name, "py", middle_file_path)
            logger.debug("op: {} op file is {}, py_module_path is {}.".format(op_type, op_py_file, py_module_path))
            if op_py_file is not None and Path(op_py_file).is_file():
                logger.debug("op: {} op file is {}.".format(op_type, op_py_file))
                if py_module_path not in sys.path:
                    sys.path.append(py_module_path)
                return op_mode_name, op_py_file

            # dynamic
            py_module_path = "{}/{}".format(op_path_custom, middle_path)
            middle_file_path = "{}/{}/dynamic".format(middle_path, op_mode_name)
            op_py_file = get_file_real_path(op_path_custom, op_type_name, "py", middle_file_path)
            logger.debug("op: {} op file is {}, py_module_path is {}.".format(op_type, op_py_file, py_module_path))
            if op_py_file is not None and Path(op_py_file).is_file():
                if py_module_path not in sys.path:
                    sys.path.append(py_module_path)
                op_mode_name = "{}.dynamic".format(op_mode_name)
                return op_mode_name, op_py_file

    return None, None


def get_dynamic_compile_static_from_kb(op_type, op_info):
    """
    get_dynamic_compile_static_from_cann
    """
    return None, None


def get_built_in_op_operator(op_type, dynamic_compile_static, is_dynamic):
    """
    get_built_in_op_operator
    """
    if dynamic_compile_static == "true" or is_dynamic:
        importlib.import_module("impl.dynamic")
        op_operator = tbe_register.get_operator(op_type)
        if op_operator is not None:
            logger.debug("{}'s op_operator is not null.".format(op_type))
            return op_operator.get_func()
        else:
            logger.debug("{}'s op_compute is None, this is an unregistered operator.".format(op_type))
            return op_register_get_func(SubOpInfoStore(), op_type, "impl.dynamic")
    elif dynamic_compile_static == "false":
        return op_register_get_func(SubOpInfoStore(), op_type, "impl")
    else:
        logger.warn("{} dynamic_compile_static is None.".format(op_type))
        return None


def get_single_op_operator(op_type, dynamic_compile_static, is_dynamic):
    """
    get_single_op_operator
    """
    op_type_name = normalize_func_name(op_type)
    custom_opp_path_list = OpPathParse().get_custom_opp_path_list()
    if custom_opp_path_list:
        op_mode_name, op_py_file = find_mode_file_from_custom(op_type, custom_opp_path_list)
        logger.debug("{}'s op_mode_name is {}, op_py_file is {}.".format(op_type, op_mode_name, op_py_file))
        if op_py_file is not None and Path(op_py_file).is_file():
            op_mode = "{}.{}".format(op_mode_name, op_type_name)
            logger.debug("{} op module {}.".format(op_type, op_mode))
            opm = importlib.import_module(op_mode)
            return getattr(opm, op_type_name)

    vendors_opp_path_list = OpPathParse().get_vendors_opp_path_list()
    if vendors_opp_path_list:
        op_mode_name, op_py_file = find_mode_file_from_vendors(op_type, vendors_opp_path_list)
        logger.debug("{}'s op_mode_name is {}, op_py_file is {}.".format(op_type, op_mode_name, op_py_file))
        if op_py_file is not None and Path(op_py_file).is_file():
            op_mode = "{}.{}".format(op_mode_name, op_type_name)
            logger.debug("{} op module {}.".format(op_type, op_mode))
            opm = importlib.import_module(op_mode)
            return getattr(opm, op_type_name)

    return get_built_in_op_operator(op_type, dynamic_compile_static, is_dynamic)


def get_core_type_from_op_content(op_type):
    """
    get core_type from op content
    """
    op_content = SubOpInfoStore().op_builtin_info_dict.get(op_type)
    if op_content is None:
        logger.debug("op_content %s is not exist.", op_type)
        return None
    else:
        core_type_dict = op_content.get("coreType")
        if core_type_dict is None:
            logger.debug("%s coreType is not exist.", op_type)
            return None
        else:
            core_type = core_type_dict["value"]
            return core_type


def is_valid_module_path(module_path):
    if not os.path.isabs(module_path):
        logger.info("path is not abs path.")
        return None
    module_dir, module_name = os.path.split(module_path)
    module_name = os.path.splitext(module_name)[0]
    try:
        spec = util.spec_from_file_location(module_name, module_path)
        opm = util.module_from_spec(spec)
        spec.loader.exec_module(opm)
        return opm
    except ImportError as e:
        logger.debug("Import op_path {} did not succeed".format(module_path))
        return None
