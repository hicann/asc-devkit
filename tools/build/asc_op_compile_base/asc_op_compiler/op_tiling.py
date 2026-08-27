#!/usr/bin/python
# -*- coding: utf-8 -*-
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
op tiling interface
"""

import os
import sys
import json
import math
import ctypes
import struct
import hashlib
import platform
import threading
import glob
from pathlib import Path

from asc_op_compile_base.common.utils import log
from asc_op_compile_base.common.context import get_context as get_op_context
from asc_op_compile_base.common.context.op_info import OpInfo
from asc_op_compile_base.common.context import op_context
from asc_op_compile_base.common.context.op_context import OpContext
from asc_op_compile_base.common.buildcfg import get_current_build_config

BANK_CACHE = ""

_KEY_NAME = "name"
_RT_BANK_CACHE = ""
_MAX_RUN_INFO_SIZE = 1024 * 64
_CONST_VALUE = "const_value"
_CONST_VALUE_NULL_DESC = "const_value_null_desc"
_ATTR_DTYPE = "dtype"
_ATTR_VALUE = "value"
_ATTR_VALUE_NULL_DESC = "value_null_desc"
_ASCEND_OPP_PATH_ENV = "ASCEND_OPP_PATH"
_ASCEND_OPP_PATH_DEFAULT = os.path.abspath("/usr/local/Ascend/cann/opp")
_ASCEND_OPP_PATH_DEFAULT_MDC = os.path.abspath("/usr/local/Ascend/opp")
_ASCEND_CUSTOM_OPP_PATH_ENV = "ASCEND_CUSTOM_OPP_PATH"

# Tiling is likely running in thread pool or single-threaded process,
# using thread local buffer reduces memory allocation
_TILING_DATA = threading.local()

# Initializing thread local data when importing this py module,
# which is helpful in case of single-threaded profiling test
_TILING_DATA.buf = ctypes.create_string_buffer(_MAX_RUN_INFO_SIZE)
_TILING_DATA.buf_size = ctypes.c_size_t(_MAX_RUN_INFO_SIZE)

platform_arch = platform.machine()
opp_dir = os.environ.get(_ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT)
scene_info = os.path.join(opp_dir, "scene.info")
# mdc default path
scene_info_mdc = os.path.join(_ASCEND_OPP_PATH_DEFAULT_MDC, "scene.info")
# all in one default path
scene_info_path_default = os.path.join(_ASCEND_OPP_PATH_DEFAULT, "scene.info")
# first choose all in one default path
scene_info_default_path = (
    scene_info_path_default
    if os.path.exists(scene_info_path_default)
    else scene_info_mdc
)
# first use opp path env path
scene_info_path = scene_info if os.path.exists(scene_info) else scene_info_default_path
conf_dir = os.path.join(opp_dir, "vendors")
config = os.path.join(opp_dir, "vendors", "config.ini")
op_impl_path = (
    os.path.join("built-in", "op_impl")
    if os.path.exists(conf_dir)
    else os.path.join("op_impl", "built-in")
)
tiling_full_path = os.path.join(
    opp_dir, op_impl_path, "ai_core", "tbe", "op_tiling", "liboptiling.so"
)
tiling_so_path = os.path.join(
    op_impl_path, "ai_core", "tbe", "op_tiling", "liboptiling.so"
)
tiling_cust_path = os.path.join("ai_core", "tbe", "op_tiling", "liboptiling.so")
# Get system info
if os.path.exists(scene_info_path):
    with open(scene_info_path) as f:
        scene_info = list(map(lambda x: x.strip(), f.readlines()))
        os_state = False
        for item_info in scene_info:
            if "os=" in item_info:
                sys_version = item_info.split("=")[-1]
                os_state = True
        if os_state is False:
            raise RuntimeError(
                {"errCode": "E80001", "config_name": "os", "file_name": "scene.info"}
            )
else:
    sys_version = sys.platform
tiling_so_arch_path = os.path.join(
    "ai_core", "tbe", "op_tiling", "lib", sys_version, platform_arch, "liboptiling.so"
)
tiling_so_arch_path2 = os.path.join(
    "ai_core",
    "tbe",
    "op_tiling",
    "lib",
    sys_version,
    platform_arch,
    "libopmaster_rt2.0.so",
)
so_arch_path = os.path.join(op_impl_path, tiling_so_arch_path)
so_arch_path2 = os.path.join(op_impl_path, tiling_so_arch_path2)
tiling_rtso_arch_path2 = os.path.join(
    "ai_core",
    "tbe",
    "op_tiling",
    "lib",
    sys_version,
    platform_arch,
    "libopmaster_rt.so",
)
rtso_arch_path2 = os.path.join(op_impl_path, tiling_rtso_arch_path2)
tiling_open_arch_path = os.path.join(
    "ai_core", "tbe", "op_host", "lib", sys_version, platform_arch
)
open_arch_path = os.path.join(op_impl_path, tiling_open_arch_path)
_BUILTIN_TILING_PATH = (
    tiling_so_path if os.path.exists(tiling_full_path) else so_arch_path
)

if os.path.exists(config):
    with open(config) as f:
        _VENDOR_NAME = f.readline().split("=")[1].split(",")[0].strip()
        _CUSTOM_TILING_PATH_DEFAULT = os.path.join(
            "vendors", _VENDOR_NAME, "op_impl", tiling_cust_path
        )
else:
    _VENDOR_NAME = "customize"
    _CUSTOM_TILING_PATH_DEFAULT = os.path.join("op_impl", "custom", tiling_cust_path)


def _get_default_optiling_pathlist():
    default_custom_tiling_full_path_list = []
    vendor_list = []
    if os.path.exists(config):
        with open(config) as ff:
            vdr_list = ff.readline().split("=")[1].split(",")
            for vdr in vdr_list:
                vendor_name = vdr.strip()
                if vendor_name not in vendor_list:
                    vendor_list.append(vendor_name)
                    full_path = os.path.join(
                        opp_dir, "vendors", vendor_name, "op_impl", tiling_cust_path
                    )
                    default_custom_tiling_full_path_list.append(full_path)
        return default_custom_tiling_full_path_list
    else:
        vendor_name = "customize"
        full_path = os.path.join(opp_dir, "op_impl", "custom", tiling_cust_path)
        default_custom_tiling_full_path_list.append(full_path)
        return default_custom_tiling_full_path_list


def _get_custom_opp_pathlist():
    custom_vendor_tiling_list = []
    custom_opp_dir = None
    if op_context.get_context() is None:
        log.warn("Get op_context.get_context() is None")
    else:
        custom_opp_dir = op_context.get_context().get_addition("custom_opp_path")
        log.info(f"Get custom_opp_dir from op_context: {custom_opp_dir}.")
    if custom_opp_dir is None:
        custom_opp_dir = os.environ.get(_ASCEND_CUSTOM_OPP_PATH_ENV)
        log.info(f"Get custom_opp_dir from env path: {custom_opp_dir}.")
    if custom_opp_dir is None:
        return custom_vendor_tiling_list
    _path_list = str(custom_opp_dir).split(":")
    for _path in _path_list:
        local_path = _path.strip()
        if len(local_path) != 0 and local_path not in custom_vendor_tiling_list:
            full_path = os.path.join(local_path, "op_impl", tiling_cust_path)
            custom_vendor_tiling_list.append(full_path)
    return custom_vendor_tiling_list


def _gen_null_desc(value_list):
    if not isinstance(value_list, list):
        return None
    value_null_desc = []
    is_exist_null = False
    for idx, value in enumerate(value_list):
        if not isinstance(value, float):
            continue
        if value == float("inf"):
            is_exist_null = True
            value_list[idx] = None
            value_null_desc.append("inf")
        elif value == float("-inf"):
            is_exist_null = True
            value_list[idx] = None
            value_null_desc.append("-inf")
        elif math.isnan(value):
            is_exist_null = True
            value_list[idx] = None
            value_null_desc.append("nan")
        else:
            value_null_desc.append(None)

    return value_null_desc if is_exist_null else None


def _inputs_pre_process(inputs):
    if not isinstance(inputs, (list, tuple)):
        return
    for single_input in inputs:
        if not isinstance(single_input, dict):
            continue
        const_value = single_input.get(_CONST_VALUE)
        if not isinstance(const_value, (list, tuple)):
            continue
        const_value_list = list(const_value)
        const_value_null_desc = _gen_null_desc(const_value_list)
        if const_value_null_desc is not None:
            single_input[_CONST_VALUE] = const_value_list
            single_input[_CONST_VALUE_NULL_DESC] = const_value_null_desc


def _attrs_pre_process(attrs):
    if not isinstance(attrs, (list, tuple)):
        return
    for single_attr in attrs:
        if not isinstance(single_attr, dict):
            continue
        attr_dtype = single_attr.get(_ATTR_DTYPE)
        if attr_dtype not in ("float", "float32", "list_float", "list_float32"):
            continue
        attr_value = single_attr.get(_ATTR_VALUE)
        if attr_value is None:
            continue
        is_single_element = False
        if not isinstance(attr_value, (list, tuple)):
            is_single_element = True
            attr_value = [attr_value]
        attr_value_list = list(attr_value)
        attr_null_desc = _gen_null_desc(attr_value_list)
        if attr_null_desc is not None:
            if is_single_element:
                single_attr[_ATTR_VALUE_NULL_DESC] = attr_null_desc[0]
                single_attr[_ATTR_VALUE] = attr_value_list[0]
            else:
                single_attr[_ATTR_VALUE_NULL_DESC] = attr_null_desc
                single_attr[_ATTR_VALUE] = attr_value_list


def do_op_tiling(
    optype,
    compile_info,
    inputs,
    outputs,
    compile_info_hash=None,
    timer=None,
    attrs=None,
):
    """
    do op tilinng
    """

    def _load_lib():
        opp_path = Path(os.environ.get(_ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT))
        builtin_optiling_lib_path = opp_path.joinpath(_BUILTIN_TILING_PATH)
        builtin_optiling_lib_path2 = opp_path.joinpath(so_arch_path2)
        builtin_optiling_rtlib_path2 = opp_path.joinpath(rtso_arch_path2)
        builtin_optiling_open_path = opp_path.joinpath(open_arch_path)
        default_custom_tiling_pathlist = _get_default_optiling_pathlist()
        custom_tiling_pathlist = _get_custom_opp_pathlist()
        join_list = custom_tiling_pathlist + default_custom_tiling_pathlist

        libregister = ctypes.CDLL("libregister.so")

        # 1. custom optiling 2.0 regist
        for custom_tiling_so_path in join_list:
            try:
                lib_optiling = ctypes.CDLL(custom_tiling_so_path)
                custom_opp_so_path_str = str(custom_tiling_so_path)
                lib_optiling.TbeLoadSoAndSaveToRegistry(
                    custom_opp_so_path_str.encode("utf_8")
                )
            except OSError:
                # Custom op tiling lib may not exists
                pass

        # 2. builint optiling 1.0 regist
        if os.path.exists(builtin_optiling_lib_path):
            ctypes.CDLL(builtin_optiling_lib_path)

        # 3. builtin optiling 2.0 regist
        nonlocal lib_optiling_builtin
        if os.path.exists(builtin_optiling_rtlib_path2):
            lib_optiling_builtin = ctypes.CDLL(builtin_optiling_rtlib_path2)
            builtin_optiling_rtlib_path2_str = str(builtin_optiling_rtlib_path2)
            lib_optiling_builtin.TbeLoadSoAndSaveToRegistry(
                builtin_optiling_rtlib_path2_str.encode("utf_8")
            )
        elif os.path.exists(builtin_optiling_lib_path2):
            lib_optiling_builtin = ctypes.CDLL(builtin_optiling_lib_path2)
            builtin_optiling_lib_path2_str = str(builtin_optiling_lib_path2)
            lib_optiling_builtin.TbeLoadSoAndSaveToRegistry(
                builtin_optiling_lib_path2_str.encode("utf_8")
            )
        elif os.path.exists(builtin_optiling_open_path):
            so_files = glob.glob(os.path.join(builtin_optiling_open_path, "*.so"))
            # make libophost_legacy.so loading lastly
            so_files.sort(key=lambda x: os.path.basename(x) == "libophost_legacy.so")
            for so_path in so_files:
                if "libophost_legacy.so" in so_path:
                    lib_optiling_builtin = ctypes.CDLL(so_path)
                    so_path_str = str(so_path)
                    lib_optiling_builtin.TbeLoadSoAndSaveToRegistry(
                        so_path_str.encode("utf_8")
                    )
                else:
                    lib_optiling = ctypes.CDLL(so_path)
                    so_path_str = str(so_path)
                    lib_optiling.TbeLoadSoAndSaveToRegistry(so_path_str.encode("utf_8"))

        return libregister

    def _add_private_attrs(attrs):
        if get_op_context() is None:
            return attrs

        opinfo_list = get_op_context().get_op_info()
        if not opinfo_list or len(opinfo_list) != 1:
            return attrs

        if not attrs:
            attrs = []
        elif isinstance(attrs, (list, tuple)):
            attrs = list(attrs)
            if not all(isinstance(attr, dict) and _KEY_NAME in attr for attr in attrs):
                return attrs
        else:
            return attrs

        attr_dict = {item.get(_KEY_NAME): item for item in attrs}
        if isinstance(opinfo_list[0].private_attrs, (tuple, list)):
            for private_attr in opinfo_list[0].private_attrs:
                if not isinstance(private_attr, dict):
                    continue
                if (
                    private_attr.get(_KEY_NAME)
                    and private_attr.get(_KEY_NAME) not in attr_dict
                ):
                    attrs.append(private_attr)
                    attr_dict[private_attr[_KEY_NAME]] = private_attr

        return attrs

    if (
        isinstance(op_context.get_context(), OpContext)
        and isinstance(op_context.get_context().get_graph_op_info(), OpInfo)
        and (op_context.get_context().get_graph_op_info().op_name is not None)
    ):
        op_name = op_context.get_context().get_graph_op_info().op_name
    else:
        op_name = ""

    def _safe_int_conversion(value, default=-1):
        if value is None:
            return default
        clean_value = str(value).strip()
        try:
            return int(clean_value)
        except (ValueError, TypeError):
            return default

    enable_deterministic = get_current_build_config("enable_deterministic_mode")
    extra_params = {"op_name": op_name, "deterministic": enable_deterministic}

    deterministic_level = _safe_int_conversion(
        get_current_build_config("deterministic_level")
    )
    if deterministic_level != -1:
        extra_params["deterministic_level"] = deterministic_level

    if isinstance(op_context.get_context(), OpContext):
        aicore_num = op_context.get_context().get_addition("_op_aicore_num")
        vectorcore_num = op_context.get_context().get_addition("_op_vectorcore_num")
        if aicore_num is not None and vectorcore_num is not None:
            extra_params["_op_aicore_num"] = aicore_num
            extra_params["_op_vectorcore_num"] = vectorcore_num
        pcie_through = op_context.get_context().get_addition("pcie_through_flag")
        if pcie_through is not None:
            extra_params["pcie_through_flag"] = pcie_through

        op_context_tmp = op_context.get_context()
        if op_context_tmp is not None:
            op_info_ins = op_context_tmp.get_op_info()
            if op_info_ins is not None and len(op_info_ins) > 0:
                extra_params_tmp = op_info_ins[0].extra_params
                if len(extra_params_tmp) > 0:
                    extra_params.update(extra_params_tmp)

    lib_optiling_builtin = None
    libregister = _load_lib()
    if _RT_BANK_CACHE:
        pid = os.getpid()
        pid_c = ctypes.c_uint32(pid)
        optype_c = optype.encode("utf_8")
        rt_bank_cache_str = str(_RT_BANK_CACHE).replace("'", '"')
        tiling_c = rt_bank_cache_str.encode("utf_8")
        log.info(
            f"Start to do SetTuningTiling for {optype}, tiling: {rt_bank_cache_str}."
        )
        set_tiling_func = lib_optiling_builtin.SetTuningTiling
        if set_tiling_func(pid_c, optype_c, tiling_c) != 0:
            log.error(
                f"SetTuningTiling of {optype} failed, tiling: {rt_bank_cache_str}."
            )
        compile_info.update({"enable_rt_bank_cache": True})

    _inputs_pre_process(inputs)
    _attrs_pre_process(attrs)
    attrs = _add_private_attrs(attrs)
    optype_c = optype.encode("utf_8")
    compile_info_c = json.dumps(compile_info).encode("utf_8")
    inputs_c = json.dumps(inputs).encode("utf_8")
    outputs_c = json.dumps(outputs).encode("utf_8")
    extra_params_c = json.dumps(extra_params).encode("utf_8")
    # Attrs supported format: ({name: str, dtype: str, value: Any}, ...)
    # Attrs supported dtypes: (bool, float, float32, int, int32, list_bool, list_float, list_float32, list_int,
    #                          list_int32, list_list_int, list_list_int32, list_str, str)
    if not attrs:
        attrs_c = ctypes.c_void_p()
    else:
        attrs_c = json.dumps(attrs).encode("utf_8")
    if not compile_info_hash:
        hashstr = hashlib.sha1()
        hashstr.update(compile_info_c)
        compile_info_hash = hashstr.hexdigest()
    compile_info_hash_c = compile_info_hash.encode("utf_8")

    if not hasattr(_TILING_DATA, "buf") or not hasattr(_TILING_DATA, "buf_size"):
        _TILING_DATA.buf = ctypes.create_string_buffer(_MAX_RUN_INFO_SIZE)
        _TILING_DATA.buf_size = ctypes.c_size_t(_MAX_RUN_INFO_SIZE)

    tiling_func = libregister.DoOpTilingForCompile
    tiling_func.restype = ctypes.c_char_p
    if isinstance(timer, list):
        array_c = ctypes.c_uint64 * 3
        elapse_c = array_c(0, 0, 0)
        res = tiling_func(
            optype_c,
            compile_info_c,
            compile_info_hash_c,
            inputs_c,
            outputs_c,
            attrs_c,
            _TILING_DATA.buf,
            _TILING_DATA.buf_size,
            elapse_c,
            extra_params_c,
        )
        for i in range(0, 3):
            timer.append(elapse_c[i])
    else:
        res = tiling_func(
            optype_c,
            compile_info_c,
            compile_info_hash_c,
            inputs_c,
            outputs_c,
            attrs_c,
            _TILING_DATA.buf,
            _TILING_DATA.buf_size,
            ctypes.c_void_p(),
            extra_params_c,
        )

    ret_json = json.loads(res.decode("utf-8"))

    if ret_json["ret_code"]:
        dict_args1 = {}
        inputs_str = "\n".join(tuple(map(str, inputs)))
        outputs_str = "\n".join(tuple(map(str, outputs)))
        dict_args1["type"] = 1
        dict_args1["errCode"] = "E90003"
        dict_args1["detailed_cause"] = (
            f"Tiling func of op_type {optype} failed, failure details:\n"
            f"Compile_info: {compile_info}\n"
            f"Inputs: {inputs_str}\n"
            f"Outputs: {outputs_str}\n"
            f"[OP_TILING] Attrs: {attrs}"
        )

        error_list = []
        for idx in ret_json["error_messages"]:
            dict_args2 = {}
            if "EZ0008" <= idx["errorcode"] <= "EZ0038":
                dict_args2 = idx["errormsg"]
                dict_args2["errCode"] = idx["errorcode"]
            else:
                dict_args2["type"] = idx["type"]
                dict_args2["errCode"] = idx["errorcode"]
                dict_args2["detailed_cause"] = idx["errormsg"]

            error_list.append(dict_args2)
        error_list.append(dict_args1)

        raise RuntimeError(tuple(error_list))

    run_info = json.loads(_TILING_DATA.buf.value)
    run_info["tiling_data"] = bytes.fromhex(run_info["tiling_data"])
    return run_info


def decode(tiling_data, fmt):
    """
    decode tiling data
    """
    offset = 0

    def _get_value(tiling_data, fmt, offset=0):
        """
        fmt example: [-1, "int"]   # int arrary of unknown size
                     [10, "int"]   # arrary of 10 ints
                     "int"         # single int
        """
        fmt_def = {
            "char": "c",
            "int": "i",
            "int32": "i",
            "uint": "I",
            "int8": "b",
            "uint8": "B",
            "int16": "h",
            "uint16": "H",
            "int64": "l",
            "uint64": "L",
            "double": "d",
        }
        count = 1
        unpack_size = 0
        if isinstance(fmt, (list, tuple)):
            count = fmt[0]
            if count < 0:
                fmt_size = struct.calcsize("i")
                res = struct.unpack_from("i", tiling_data, offset)
                count = res[0]
                unpack_size += fmt_size
            fmt_str = "{}{}".format(count, fmt_def.get(fmt[1]))
        else:
            fmt_str = "{}{}".format(count, fmt_def.get(fmt))

        if count == 0:
            return [unpack_size, []]

        fmt_size = struct.calcsize(fmt_str)
        res = struct.unpack_from(fmt_str, tiling_data, offset + unpack_size)
        unpack_size += fmt_size
        if isinstance(fmt, (list, tuple)):
            return [unpack_size, res]
        return [unpack_size, res[0]]

    res = {}
    for key, value in fmt.items():
        unpack_size, res[key] = _get_value(tiling_data, value, offset)
        offset += unpack_size

    return res
