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
AscendC adapter
"""

import ctypes
import json
import os
import stat
import struct
import math
import glob
import inspect
import subprocess
import copy
from pathlib import Path
from collections import namedtuple
from asc_op_compile_base.common.platform.platform_info import get_soc_spec
from asc_op_compile_base.common.context import get_context
from asc_op_compile_base.asc_op_compiler.op_tiling import (
    do_op_tiling,
    _ASCEND_OPP_PATH_ENV,
    _ASCEND_OPP_PATH_DEFAULT,
    so_arch_path2,
    op_impl_path,
)
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)
from asc_op_compile_base.common.utils.log_utils import LogUtil, AscendCLogLevel
import asc_op_compile_base.common.context.op_context as op_context
from asc_op_compile_base.common.utils import log
from .global_storage import global_var_storage
from .ascendc_common_utility import CommonUtility
from .generate_tiling_code import (
    generate_pointer_directly_assess_data,
    generate_static_pointer_v1_constexpr,
)


OpInfo = namedtuple(
    "OpInfo",
    [
        "kernel_name",
        "op_type",
        "inputs",
        "outputs",
        "attrs",
        "impl_mode",
        "origin_inputs",
        "origin_outputs",
        "param_type_dynamic",
        "mc2_ctx",
        "param_type_list",
        "init_value_list",
        "output_shape_depend_on_compute",
    ],
)

OpInfo.__new__.__defaults__ = (
    None,
    None,
    None,
    None,
    None,
    None,
    None,
    None,
    None,
    None,
    None,
    None,
    None,
)

DEFAULT_TILING_KEY_VALUE = 0
_ASCEND_CUSTOM_OPP_PATH_ENV = "ASCEND_CUSTOM_OPP_PATH"
_TILING_SO_PATH = "op_impl/ai_core/tbe/op_tiling/liboptiling.so"
opp_dir = os.environ.get(_ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT)
config_default = os.path.join(opp_dir, "vendors", "config.ini")

_TILING_NAMESPACE = "AscendC::tiling"


def get_default_optiling_pathlist():
    vendor_list = []
    full_path_list = []
    if not os.path.exists(config_default):
        return full_path_list
    with open(config_default) as f:
        vdr_lst = f.readline().split("=")[1].split(",")
        for vdr in vdr_lst:
            _vendor = vdr.strip()
            if _vendor not in vendor_list:
                vendor_list.append(_vendor)
                full_path = os.path.join(opp_dir, "vendors", _vendor)
                full_path_list.append(full_path)
    return full_path_list


def get_custom_opp_pathlist():
    vendor_list = []
    custom_opp_dir = None
    if op_context.get_context() is None:
        log.warn("op_context.get_context() is None!")
    else:
        custom_opp_dir = op_context.get_context().get_addition("custom_opp_path")
        log.info("op_context get custom_opp_dir: %s" % custom_opp_dir)
    if custom_opp_dir is None:
        custom_opp_dir = os.environ.get(_ASCEND_CUSTOM_OPP_PATH_ENV)
        log.info("get custom_opp_dir from env path: %s" % custom_opp_dir)
    if custom_opp_dir is None:
        return vendor_list
    _path_lst = str(custom_opp_dir).split(":")
    for _path in _path_lst:
        local_path = _path.strip()
        if len(local_path) != 0 and local_path not in vendor_list:
            vendor_list.append(local_path)
    return vendor_list


def load_op_host_tiling_lib():
    LogUtil.print_compile_log("", "load op host tiling lib.", AscendCLogLevel.LOG_INFO)
    builtin_op_host_tiling_prefix = (
        os.environ.get(_ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT)
        + "/"
        + op_impl_path
        + "/ai_core/tbe/op_host/"
    )
    builtin_op_host_tiling_pattern = builtin_op_host_tiling_prefix + "**/*.so"
    builtin_op_host_tiling_file_path = glob.glob(
        builtin_op_host_tiling_pattern, recursive=True
    )
    builtin_op_host_tiling_file_path.sort(
        key=lambda x: os.path.basename(x) == "libophost_legacy.so"
    )
    import platform

    archlinux = platform.machine()
    if "x86" in archlinux:
        incompatiable_arch_name = "aarch64"
    else:
        incompatiable_arch_name = "x86_64"
    for tiling_path in builtin_op_host_tiling_file_path:
        if incompatiable_arch_name in tiling_path[len(builtin_op_host_tiling_prefix) :]:
            continue
        try:
            if os.path.exists(tiling_path):
                lib_tiling_builtin = ctypes.CDLL(tiling_path)
                tiling_path_str = str(tiling_path)
                lib_tiling_builtin.TbeLoadSoAndSaveToRegistry(
                    tiling_path_str.encode("utf-8")
                )
        except OSError as e:
            LogUtil.print_compile_log(
                "",
                f"An OSError occurred: {e}, \
tiling_path: {tiling_path}",
                AscendCLogLevel.LOG_ERROR,
            )
            raise Exception(f"An OSError occurred, tiling_path: {tiling_path}") from e

        except Exception as e:
            LogUtil.print_compile_log(
                "",
                f"An Unknown error occurred: {e}, \
tiling_path: {tiling_path}",
                AscendCLogLevel.LOG_ERROR,
            )
            raise Exception(
                f"An Unknown error occurred, tiling_path: {tiling_path}"
            ) from e
    return


def load_build_in_lib():
    LogUtil.print_compile_log("", "load build in tiling lib.", AscendCLogLevel.LOG_INFO)
    # 1. load env tiling so
    tiling_so = os.environ.get("ASCEND_OPP_TILING_SO_PATH")
    if tiling_so is not None:
        if not isinstance(tiling_so, str) or not os.path.exists(tiling_so):
            LogUtil.print_compile_log(
                "",
                f"ASCEND_OPP_TILING_SO_PATH {tiling_so} invalid",
                AscendCLogLevel.LOG_ERROR,
            )
            return False
        tiling_so_path = Path(tiling_so)
        try:
            ctlib_opp_tiling = ctypes.CDLL(tiling_so_path)
        except AttributeError as e:
            # ascend c static load builtin opmaster ct so fail
            LogUtil.print_compile_log(
                "",
                f"An AttributeError occurred: {e}, when load tiling so {tiling_so_path}",
                AscendCLogLevel.LOG_ERROR,
            )
            return False
        return True

    # 2. loat built-in tiling so in op_host dir
    load_op_host_tiling_lib()
    opp_path = Path(os.environ.get(_ASCEND_OPP_PATH_ENV, _ASCEND_OPP_PATH_DEFAULT))

    builtin_optiling_lib_path2 = opp_path.joinpath(so_arch_path2)
    parent_dir = Path(os.path.dirname(builtin_optiling_lib_path2))
    builtin_optiling_rtlib_path = parent_dir.joinpath("libopmaster_rt.so")
    builtin_optiling_ctlib_path = parent_dir.joinpath("libopmaster_ct.so")
    # 3. builtin optiling 2.0 regist compile time
    try:
        if os.path.exists(builtin_optiling_ctlib_path):
            ctlib_optiling_builtin = ctypes.CDLL(builtin_optiling_ctlib_path)
            builtin_optiling_ctlib_path_str = str(builtin_optiling_ctlib_path)
            ctlib_optiling_builtin.TbeLoadSoAndSaveToRegistry(
                builtin_optiling_ctlib_path_str.encode("utf_8")
            )
    except AttributeError as e:
        # ascend c static load builtin opmaster ct so fail
        LogUtil.print_compile_log(
            "",
            f"An AttributeError occurred: {e}, when load tiling so {builtin_optiling_ctlib_path}",
            AscendCLogLevel.LOG_ERROR,
        )
        return False

    # 4. builtin optiling 2.0 regist runtime
    try:
        if os.path.exists(builtin_optiling_rtlib_path):
            lib_optiling_builtin = ctypes.CDLL(builtin_optiling_rtlib_path)
            builtin_optiling_lib_path_str = str(builtin_optiling_rtlib_path)
            lib_optiling_builtin.TbeLoadSoAndSaveToRegistry(
                builtin_optiling_lib_path_str.encode("utf_8")
            )
        elif os.path.exists(builtin_optiling_lib_path2):
            lib_optiling_builtin = ctypes.CDLL(builtin_optiling_lib_path2)
            builtin_optiling_lib_path2_str = str(builtin_optiling_lib_path2)
            lib_optiling_builtin.TbeLoadSoAndSaveToRegistry(
                builtin_optiling_lib_path2_str.encode("utf_8")
            )
    except AttributeError as e:
        # ascend c static load builtin opmaster rt so fail, undefined symbol, then use 1.0 way
        LogUtil.print_compile_log(
            "",
            f"An AttributeError occurred: {e}, when load tiling so "
            f"{builtin_optiling_rtlib_path} or {builtin_optiling_lib_path2}",
            AscendCLogLevel.LOG_ERROR,
        )
        return False
    return True


def load_lib():
    libregister = ctypes.CDLL("libregister.so")

    # 1. custom optiling 2.0 regist
    default_lst = get_default_optiling_pathlist()
    custom_opp_list = get_custom_opp_pathlist()
    join_list = custom_opp_list + default_lst
    for _path in join_list:
        try:
            custom_opp_so_path = os.path.join(_path, _TILING_SO_PATH)
            if not os.path.exists(custom_opp_so_path):
                LogUtil.print_compile_log(
                    "", f"{custom_opp_so_path} not exists", AscendCLogLevel.LOG_INFO
                )
            else:
                lib_optiling = ctypes.CDLL(custom_opp_so_path)
                custom_opp_so_path_str = str(custom_opp_so_path)
                lib_optiling.TbeLoadSoAndSaveToRegistry(
                    custom_opp_so_path_str.encode("utf_8")
                )
        except OSError as e:
            # Custom op tiling lib may not exists
            LogUtil.print_compile_log(
                "",
                f"An OSError occurred: {e}, when load tiling so {custom_opp_so_path}",
                AscendCLogLevel.LOG_ERROR,
            )
            pass

    # load build in tiling so
    load_build_in_lib()

    return libregister


class Field:
    def __init__(self, field):
        """field: used to record information of tiling field

        field type:
            1 varialbe:
                classType: 0
                has attr: name, dtype
            2 array:
                classType: 1
                has attr: name, dtype, arrSize
            3 struct:
                classType: 2
                has attr: name, dtype(fix "struct"), structType, structSize
        """
        self.class_type = int(field["classType"])
        self.name = field["name"]
        self.dtype = field["dtype"]
        if self.class_type == 1:
            self.arr_size = int(field["arrSize"])
        elif self.class_type == 2:
            self.struct_type = field["structType"]
            self.struct_size = int(field["structSize"])


class TilingDef:
    def __init__(self, json_value):
        self.class_name = json_value["class_name"]
        self.data_size = json_value["data_size"]
        self.field_list = []
        self.is_api = False
        self.generate_code = ""
        self.class_def = ""
        self.tiling_key = 0
        self.depth = 0
        for field in json_value["fields"]:
            self.field_list.append(Field(field))


def decode(tiling_data, fmt, offset=0):
    """
    decode tiling data from binary to map

    Args:
        tiling_data: b'\x01\x00\x02\x00\n\x00\x02\x00\x08\x00'
        fmt: dict(name, type) such as {a:'uint8', b:'uint16_t'}
        offset: point to the position where the tilingdata is to be parsed.
    """

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
            "float": "f",
            "double": "d",
            "bool": "?",
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
        # when dtype is float, needs to deal with INF / NAN problem,
        # python automatically convert INF/NAN to "inf/nan", needs extra process for kernel to compile
        res = struct.unpack_from(fmt_str, tiling_data, offset + unpack_size)
        if fmt_str.endswith("f"):
            str_res = list(res)
            for i, x in enumerate(res):
                if x == float("inf"):
                    str_res[i] = "float(1.0 / 0.0)"
                elif x == float("-inf"):
                    str_res[i] = "float(-1.0 / 0.0)"
                elif math.isnan(x):
                    str_res[i] = "float(0.0 / 0.0)"
            res = tuple(str_res)
        elif fmt_str.endswith("?"):
            str_res = list(res)
            for i, x in enumerate(res):
                # python True/False -> c++ true/false
                str_res[i] = "true" if x else "false"
            res = tuple(str_res)

        unpack_size += fmt_size
        if isinstance(fmt, (list, tuple)):
            return [unpack_size, res]
        return [unpack_size, res[0]]

    res = {}
    for key, value in fmt.items():
        unpack_size, res[key] = _get_value(tiling_data, value, offset)
        offset += unpack_size

    return [res, offset]


def _decode_struct_tiling_data(
    field, binary_tiling_data, offset, struct_tiling_def_base
):
    """decode struct tiling data

    Args:
        field: struct type field, use to get struct info
        binary_tiling_data: src binary of tiling data
        offset: point to the start location of binary_tiling_data
        struct_tiling_def_base: use to store the struct field info
    Return:
        struct_tiling_data: dict(name, type) such as {var0 : val0, var0:val1}
        offset: point to the end location of binary_tiling_data
    """
    struct_tiling_def = struct_tiling_def_base[field.struct_type]
    if struct_tiling_def is None:
        msg = "get_op_tiling.py:_decode_struct_tiling_data struct_type: {} is not define, ".format(
            field.struct_type
        )
        raise msg
    struct_tiling_data, struct_offset = _decode_tiling_data(
        struct_tiling_def, binary_tiling_data[offset:], struct_tiling_def_base
    )
    return [struct_tiling_data, offset + struct_offset]


def get_bytes_by_type(dtype):
    type_bytes = {
        "int8_t": 1,
        "uint8_t": 1,
        "int16_t": 2,
        "uint16_t": 2,
        "int32_t": 4,
        "uint32_t": 4,
        "int64_t": 8,
        "uint64_t": 8,
        "float": 4,
        "bool": 1,
    }
    if dtype in type_bytes:
        return type_bytes[dtype]
    else:
        msg = "get_bytes_by_type cannot find type = " + dtype
        raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, (msg))


def _decode_tiling_data(tiling_def, run_info_tiling_data, struct_tiling_def_base):
    """decode tiling data by tiling field info and run_info_tiling_data

    Args:
        tiling_def: record tiling field info
        run_info_tiling_data: src binary of tiling data
        struct_tiling_def_base: used to decode struct tiling data
    Return:
        run_info_tiling_data: dict(name, type) such as {var0 : val0, var0:val1}
        offset: point to the end location of binary_tiling_data
    """
    tiling_cc2py_mapping = {
        "int": "int",
        "int32_t": "int",
        "uint": "uint",
        "uint32_t": "uint",
        "int8_t": "int8",
        "uint8_t": "uint8",
        "int16_t": "int16",
        "uint16_t": "uint16",
        "int64_t": "int64",
        "uint64_t": "uint64",
        "float": "float",
        "struct": "struct",
        "bool": "bool",
    }
    tiling_format = {}
    struct_binary_info_list = []
    struct_list = []
    offset = 0
    run_info_tiling_data_binary = b""
    for field in tiling_def.field_list:
        if field.dtype not in tiling_cc2py_mapping.keys():
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                "tiling format key error: {}".format(field.dtype),
            )
        if field.class_type == 0:
            tiling_format[field.name] = tiling_cc2py_mapping[field.dtype]
            data_size = get_bytes_by_type(field.dtype)
            run_info_tiling_data_binary += run_info_tiling_data[
                offset : offset + data_size
            ]
            offset += data_size
        elif field.class_type == 1:
            tiling_format[field.name] = [
                field.arr_size,
                tiling_cc2py_mapping[field.dtype],
            ]
            data_size = get_bytes_by_type(field.dtype) * field.arr_size
            run_info_tiling_data_binary += run_info_tiling_data[
                offset : offset + data_size
            ]
            offset += data_size
        elif field.class_type == 2:
            struct_list.append(field)
            struct_binary_info_list.append([offset, field.struct_size])
            offset += field.struct_size

    for offset, struct_size in struct_binary_info_list:
        run_info_tiling_data_binary += run_info_tiling_data[
            offset : offset + struct_size
        ]

    binary_tiling_data = run_info_tiling_data_binary
    # decode tiling data with out struct
    run_info_tiling_data_binary, offset = decode(binary_tiling_data, tiling_format)
    # decode struct tiling data
    for field in struct_list:
        struct_tiling_data, offset = _decode_struct_tiling_data(
            field, binary_tiling_data, offset, struct_tiling_def_base
        )
        run_info_tiling_data_binary[field.name] = struct_tiling_data
    return [run_info_tiling_data_binary, offset]


def get_tiling_def(optype):
    libregister = ctypes.CDLL("libregister.so")
    optype_c = optype.encode("utf_8")

    get_subclass_func = libregister.AscendCPyInterfaceGetTilingDefInfo

    max_run_info_size = 1024 * 64
    result_info_buf = ctypes.create_string_buffer(max_run_info_size)
    result_info_buf_size = ctypes.c_size_t(max_run_info_size)
    res = get_subclass_func(optype_c, result_info_buf, result_info_buf_size)

    # it's allowed no tiling-def
    if not res:
        LogUtil.print_compile_log(
            optype, "has no Tiling Info", AscendCLogLevel.LOG_INFO
        )
        return None
    res_info = json.loads(result_info_buf.value)
    class_info = TilingDef(res_info)
    return class_info


def get_struct_shape(struct_tiling_def_base):
    # generatre struct definition
    struct_class_list = [""]
    for _, struct_tiling_def in struct_tiling_def_base.items():
        class_body = ""
        # tiling struct in tikcpp advanced api, which has been include in kernel_tiling.h
        if struct_tiling_def.is_api:
            continue
        class_body += f"class {struct_tiling_def.class_name}\n"
        class_body += "{\n"
        class_body += "public:\n"
        for field in struct_tiling_def.field_list:
            if field.class_type == 0:
                class_body += f"    {field.dtype} {field.name} = 0;\n"
            elif field.class_type == 1:
                class_body += (
                    f"    {field.dtype} {field.name}[{field.arr_size}] = {{}};\n"
                )
            elif field.class_type == 2:
                class_body += "    "
                # if field in struct is from api, add a namespace
                if struct_tiling_def_base[field.struct_type].is_api:
                    class_body += f"{_TILING_NAMESPACE}::"
                class_body += f"{field.struct_type} {field.name};\n"
        class_body += "};\n\n"
        if struct_tiling_def.depth >= len(struct_class_list):
            struct_class_list.append("")
        struct_class_list[struct_tiling_def.depth] += class_body

    total_class_body = ""
    for struct_class in struct_class_list[::-1]:
        total_class_body += struct_class
    return total_class_body


def is_struct_have_arr(struct_tiling_def, has_arr, struct_tiling_def_base):
    # generate struct value of static shape
    struct_field_list: list = struct_tiling_def.field_list
    for _, struct_field in enumerate(struct_field_list):
        if struct_field.class_type == 0:
            continue
        elif struct_field.class_type == 1:
            has_arr = True
        else:
            sub_has_arr = is_struct_have_arr(
                struct_tiling_def_base[struct_field.struct_type],
                has_arr,
                struct_tiling_def_base,
            )
            has_arr |= sub_has_arr
    return has_arr


def gen_all_dynamic_struct_def_except_self(
    is_optype_self, tiling_key, tiling_key_list, optype, tiling_key_group_map
):
    tiling_def_list_of_key = []
    struct_tiling_def_base = {}

    optype_tiling_def = get_tiling_def(optype)
    if optype_tiling_def is None:
        LogUtil.print_compile_log(
            optype, "do not registe tiling struct!!!", AscendCLogLevel.LOG_ERROR
        )
        return ""
    struct_tiling_def_base = get_struct_tiling_info(
        optype_tiling_def, struct_tiling_def_base
    )

    for one_tiling_key in tiling_key_list:
        optype_with_tilingkey = optype + "_" + one_tiling_key
        tiling_def = get_tiling_def(optype_with_tilingkey)
        tiling_key_slave = None
        if one_tiling_key in tiling_key_group_map.keys():
            tiling_def = _get_tiling_def_with_group(
                tiling_def, optype, one_tiling_key, tiling_key_group_map
            )
            tiling_key_slave = tiling_key_group_map[one_tiling_key][0]
        if tiling_def is not None:
            struct_tiling_def_base = get_struct_tiling_info(
                tiling_def, struct_tiling_def_base
            )
            tiling_def.class_def = get_dynamic_tiling_struct(
                tiling_def, struct_tiling_def_base
            )
            tiling_def.tiling_key = one_tiling_key
            tiling_def_list_of_key.append(tiling_def)
        if tiling_key_slave is not None and tiling_key_slave != one_tiling_key:
            tiling_def_slave = copy.deepcopy(tiling_def)
            tiling_def_slave.tiling_key = tiling_key_slave
            tiling_def_list_of_key.append(tiling_def_slave)

    optype_tiling_def.class_def = get_dynamic_tiling_struct(
        optype_tiling_def, struct_tiling_def_base
    )

    # all fields of substruct and file-item defination, just like dynamic shape scene
    codes = get_struct_shape(struct_tiling_def_base)
    if not is_optype_self:
        # when the tilingkey of this shape now use the special registered tilingkey struct, not optype struct
        # then must add defination of optype struct itself, or has not defination
        codes += optype_tiling_def.class_def
    tiling_struct_dict = set()
    tiling_def_class_name = ""
    if not is_optype_self:
        for tiling_def in tiling_def_list_of_key:
            if tiling_def.tiling_key == tiling_key:
                tiling_def_class_name = tiling_def.class_name

    for tiling_def in tiling_def_list_of_key:
        if not is_optype_self and tiling_def.tiling_key == tiling_key:
            # when this static shape tilingkey registed special tilingkey and is just this tilingkey
            continue
        if (
            tiling_def.class_name != tiling_def_class_name
            and tiling_def.class_name not in tiling_struct_dict
        ):
            # the defination of tilingkey-registed special struct
            codes += tiling_def.class_def
            tiling_struct_dict.add(tiling_def.class_name)
    return codes


def gen_micro_assign_value_of_tiling(tiling_struct: str, tiling_raw_data: str):
    tiling_size = len(tiling_raw_data)
    tiling_format = {"tiling": [tiling_size, "uint8"]}

    # decode tiling data without struct
    tiling_data, _ = decode(tiling_raw_data, tiling_format)
    tiling_arr_data = tiling_data["tiling"]
    tiling_arr_data_str = ", ".join(f"{x}" for x in tiling_arr_data)

    short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
    tiling_assign_str = ""
    class_body = ""

    if short_soc_version == "Ascend310P":
        tiling_assign_str = f"    uint8_t __ascendc_arr_##%s[{tiling_size}] = {{{tiling_arr_data_str}}};"
        class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                                           \\\n"
        class_body += f"    uint8_t __ascendc_arr_##tiling_data[{tiling_size}] = {{{tiling_arr_data_str}}};    \\\n"
        class_body += f"    {tiling_struct} tiling_data = convert_from_bytes<{tiling_struct}>(__ascendc_arr_##tiling_data);\n\n"

        class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n"
        class_body += f"    uint8_t __ascendc_arr_##tiling_data[{tiling_size}] = {{{tiling_arr_data_str}}};    \\\n"
        class_body += "    tiling_struct tiling_data = convert_from_bytes<tiling_struct>(__ascendc_arr_##tiling_data);\n\n"

        class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                           \\\n"
        class_body += f"    uint8_t __ascendc_arr_##var[{tiling_size}] = {{{tiling_arr_data_str}}};    \\\n"
        class_body += "    tiling_type __ascendc_point##var = convert_from_bytes<tiling_type>(__ascendc_arr_##var);\\\n"
        class_body += "    auto& var = __ascendc_point##var.member;\n\n"
    else:
        tiling_assign_str = f"    const uint8_t __ascendc_arr_##%s[{tiling_size}] = {{{tiling_arr_data_str}}};"
        if global_var_storage.get_variable("ascendc_tiling_no_register"):
            class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                                         \n"
        else:
            class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                                           \\\n"
            class_body += f"    const uint8_t __ascendc_arr_##tiling_data[{tiling_size}] = {{{tiling_arr_data_str}}};   \\\n"
            class_body += f"    static constexpr {tiling_struct} tiling_data = "
            class_body += (
                f"convert_from_bytes<{tiling_struct}>(__ascendc_arr_##tiling_data);\n\n"
            )

        class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n"
        class_body += "    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                  \\\n"
        class_body += f"    const uint8_t __ascendc_arr_##tiling_data[{tiling_size}] = {{{tiling_arr_data_str}}};    \\\n"
        class_body += "    static constexpr tiling_struct tiling_data = "
        class_body += (
            "convert_from_bytes<tiling_struct>(__ascendc_arr_##tiling_data); \n\n"
        )

        class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                            \\\n"
        class_body += "    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                                  \\\n"
        class_body += f"    const uint8_t __ascendc_arr_##var[{tiling_size}] = {{{tiling_arr_data_str}}}; \\\n"
        class_body += "    static constexpr tiling_type __ascendc_point##var = "
        class_body += "convert_from_bytes<tiling_type>(__ascendc_arr_##var);\\\n"
        class_body += "    auto& var = __ascendc_point##var.member;\n\n"
    return class_body, tiling_assign_str


def gen_static_struct_body_v1(
    class_name, field_list, tiling_raw_data, struct_tiling_def_base
):
    # the only one top-level struct of static-shape one itself
    class_body = f"class {class_name}\n"
    class_body += "{\n"
    class_body += "public:\n"
    has_arr = False
    for field in field_list:
        if field.class_type == 0:
            class_body += f"    {field.dtype} {field.name};\n"
        elif field.class_type == 1:
            has_arr = True
            class_body += f"    {field.dtype} {field.name}[{field.arr_size}];\n"
        elif field.class_type == 2:
            has_arr |= is_struct_have_arr(
                struct_tiling_def_base[field.struct_type],
                has_arr,
                struct_tiling_def_base,
            )
            # if struct is from api, add a namespace
            if struct_tiling_def_base[field.struct_type].is_api:
                class_body += f"{_TILING_NAMESPACE}::"
            class_body += f"{field.struct_type} {field.name};\n"

    class_body += "};\n\n"
    body, tiling_assign_str = gen_micro_assign_value_of_tiling(
        class_name, tiling_raw_data
    )
    return class_body, body, tiling_assign_str


def gen_micro_assign_value_of_tiling_force_constexpr(tiling_struct: str):
    short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
    class_body = ""

    if short_soc_version == "Ascend310P":
        class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                                           \\\n"
        class_body += f"    static constexpr {tiling_struct} tiling_data;\n\n"

        class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n"
        class_body += "    static constexpr tiling_struct tiling_data;\n\n"

        class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                           \\\n"
        class_body += "    static constexpr tiling_type __ascendc_point##var;                                     \\\n"
        class_body += "    auto& var = __ascendc_point##var.member;\n\n"
    else:
        if global_var_storage.get_variable("ascendc_tiling_no_register"):
            class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                                         \n"
        else:
            class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                                           \\\n"
            class_body += f"    static constexpr {tiling_struct} tiling_data;\n\n"

        class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                \\\n"
        class_body += "    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                  \\\n"
        class_body += "    static constexpr tiling_struct tiling_data;\n\n"

        class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                            \\\n"
        class_body += "    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                                  \\\n"
        class_body += "    static constexpr tiling_type __ascendc_point##var;                                   \\\n"
        class_body += "    auto& var = __ascendc_point##var.member;\n\n"
    return class_body


def gen_static_struct_body_v1_force_constexpr(
    class_name, field_list, tiling_raw_data, struct_tiling_def_base
):
    tiling_size = len(tiling_raw_data)
    tiling_format = {"tiling": [tiling_size, "uint8"]}

    # decode tiling data without struct
    tiling_data, _ = decode(tiling_raw_data, tiling_format)
    tiling_arr_data = tiling_data["tiling"]
    tiling_arr_data_str = ", ".join(f"{x}" for x in tiling_arr_data)

    class_body = f"static constexpr uint8_t __ascendc_arr_tiling_data[{tiling_size}] = {{{tiling_arr_data_str}}};\n"

    class_body += f"class {class_name}Aux\n"
    class_body += "{\n"
    class_body += "public:\n"
    has_arr_aux = False
    need_std_array = False
    for field in field_list:
        if field.class_type == 0:
            class_body += f"    {field.dtype} {field.name};\n"
        elif field.class_type == 1:
            has_arr_aux = True
            need_std_array = True
            class_body += (
                f"    std::array<{field.dtype}, {field.arr_size}> {field.name};\n"
            )
        elif field.class_type == 2:
            has_arr_aux |= is_struct_have_arr(
                struct_tiling_def_base[field.struct_type],
                has_arr_aux,
                struct_tiling_def_base,
            )
            # if struct is from api, add a namespace
            if struct_tiling_def_base[field.struct_type].is_api:
                class_body += f"{_TILING_NAMESPACE}::"
            class_body += f"{field.struct_type} {field.name};\n"
    class_body += "};\n\n"

    class_body += (
        f"static constexpr {class_name}Aux __ascendc_tiling_aux = "
        f"convert_from_bytes<{class_name}Aux>(__ascendc_arr_tiling_data);\n"
    )

    # the only one top-level struct of static-shape one itself
    class_body += f"class {class_name}\n"
    class_body += "{\n"
    class_body += "public:\n"
    has_arr = False
    for field in field_list:
        if field.class_type == 0:
            class_body += f"    static constexpr {field.dtype} {field.name} = __ascendc_tiling_aux.{field.name};\n"
        elif field.class_type == 1:
            has_arr = True
            class_body += (
                f"    static constexpr std::array<{field.dtype}, {field.arr_size}> {field.name} = "
                f"__ascendc_tiling_aux.{field.name};\n"
            )
        elif field.class_type == 2:
            has_arr |= is_struct_have_arr(
                struct_tiling_def_base[field.struct_type],
                has_arr,
                struct_tiling_def_base,
            )
            # if struct is from api, add a namespace
            class_body += "static constexpr "
            if struct_tiling_def_base[field.struct_type].is_api:
                class_body += f"{_TILING_NAMESPACE}::"
            class_body += f"{field.struct_type} {field.name} = __ascendc_tiling_aux.{field.name};\n"

    class_body += "};\n\n"
    body = gen_micro_assign_value_of_tiling_force_constexpr(class_name)
    return class_body, body, need_std_array


def gen_static_shape(
    tiling_def,
    tiling_raw_data,
    struct_tiling_def_base,
    all_dynamic_struct_def_except_self,
    tiling_const_propagation: bool = False,
):
    field_list: list = tiling_def.field_list
    class_name_upper = tiling_def.class_name.upper()
    class_body_header = f"#ifndef __{class_name_upper}_HEADER__\n"
    class_body_header += f"#define __{class_name_upper}_HEADER__\n"
    class_body = '#include "kernel_tiling/kernel_tiling.h"\n'
    class_body += "#ifdef ASCENDC_CPU_DEBUG\n"
    class_body += '#include "kernel_log.h"\n'
    class_body += "#else\n"
    class_body += "#ifndef __aicore__\n"
    class_body += "#define __aicore__ [aicore]\n"
    class_body += "#endif\n"
    class_body += "#endif\n"
    if not global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \n\n"
    # all tiling struct info by dynamic, except the only one top-level struct of static-shape one itself
    class_body += all_dynamic_struct_def_except_self
    # the only one top-level struct of static-shape one itself
    if not tiling_const_propagation:
        class_body_v1, body_v1, tiling_assign_str = gen_static_struct_body_v1(
            tiling_def.class_name, field_list, tiling_raw_data, struct_tiling_def_base
        )
        class_body += class_body_v1
        class_body += body_v1
        class_body += generate_pointer_directly_assess_data(
            False, True, tiling_assign_str
        )
    else:
        class_body_v1_expr, body_v1_constexpr, need_std_array = (
            gen_static_struct_body_v1_force_constexpr(
                tiling_def.class_name,
                field_list,
                tiling_raw_data,
                struct_tiling_def_base,
            )
        )
        if need_std_array:
            class_body_header += "#include <array>\n"
        class_body += class_body_v1_expr
        class_body += body_v1_constexpr
        class_body += generate_static_pointer_v1_constexpr()
    class_body_ender = f"#endif // __{class_name_upper}_HEADER__\n"
    return class_body_header + class_body + class_body_ender


def get_dynamic_cpu_assign_tiling_data(
    struct_tiling_def_base, field_list, left_value, offset=0
):
    # generate cpu code for assigning values to variables
    class_body = ""
    for field in field_list:
        if field.class_type == 0:
            class_body += f"    {left_value}{field.name} = (*(const __gm__ {field.dtype} *)(p_tilingdata + {offset}));\n"
            offset += get_bytes_by_type(field.dtype)
        elif field.class_type == 1:
            class_body += f"    for (int i = 0 ; i < {field.arr_size}; i++) {{\n"
            class_body += (
                f"        {left_value}{field.name}[i] = (*(const __gm__ {field.dtype} *)\
(p_tilingdata + {offset} + i * sizeof({field.dtype})));\n"
            )
            class_body += "    }\n"
            offset += get_bytes_by_type(field.dtype) * field.arr_size
        elif field.class_type == 2:
            struct_field_list = struct_tiling_def_base[field.struct_type].field_list
            struct_body, offset = get_dynamic_cpu_assign_tiling_data(
                struct_tiling_def_base,
                struct_field_list,
                f"{left_value}{field.name}.",
                offset,
            )
            class_body += struct_body
    return class_body, offset


def get_dynamic_npu_assign_tiling_data(
    struct_tiling_def_base, field_list, left_value, offset=0
):
    # generate npu code for assigning values to variables
    class_body = ""
    for field in field_list:
        if field.class_type == 0:
            class_body += f"    {left_value}{field.name} = (*(__ubuf__ {field.dtype} *)\
((__ubuf__ uint8_t *)tilingdata_in_ub + {offset}));\n"
            offset += get_bytes_by_type(field.dtype)
        elif field.class_type == 1:
            class_body += f"    for (int i = 0 ; i < {field.arr_size}; i++) {{\n"
            class_body += (
                f"        {left_value}{field.name}[i] = (*(__ubuf__ {field.dtype} *)\
((__ubuf__ uint8_t *)tilingdata_in_ub + {offset} + i * sizeof({field.dtype})));\n"
            )
            class_body += "    }\n"
            offset += get_bytes_by_type(field.dtype) * field.arr_size
        elif field.class_type == 2:
            struct_field_list = struct_tiling_def_base[field.struct_type].field_list
            struct_body, offset = get_dynamic_npu_assign_tiling_data(
                struct_tiling_def_base,
                struct_field_list,
                f"{left_value}{field.name}.",
                offset,
            )
            class_body += struct_body
    return class_body, offset


def get_dynamic_assign_tiling_data_by_bytes(total_bytes, prefix_0, prefix_1):
    class_body = ""
    assign_8byte_time = total_bytes // 8
    left_bytes = total_bytes % 8
    current_offset = total_bytes - left_bytes
    # for i in range(0, current_offset, 8):
    class_body += f"    for (int i = 0 ; i < {current_offset}; i += 8) {{ \n"
    class_body += f"        (*(uint64_t*)((uint8_t*)tilingdata + i)) = (*({prefix_0} uint64_t*)({prefix_1} + i));\n"
    class_body += "    }\n"

    if left_bytes // 4:
        class_body += f"    (*(uint32_t*)((uint8_t*)tilingdata + {current_offset})) = \
                            (*({prefix_0} uint32_t *)({prefix_1} + {current_offset}));\n"
        current_offset += 4
        left_bytes = left_bytes % 4

    if left_bytes // 2:
        class_body += f"    (*(uint16_t*)((uint8_t*)tilingdata + {current_offset})) = \
                            (*({prefix_0} uint16_t *)({prefix_1} + {current_offset}));\n"
        current_offset += 2
        left_bytes = left_bytes % 2

    if left_bytes:
        class_body += f"    (*(uint8_t*)((uint8_t*)tilingdata + {current_offset})) = \
                            (*({prefix_0} uint8_t *)({prefix_1} + {current_offset}));\n"
    return class_body


def get_dynamic_assign_tiling_data_by_size(offset, prefix_0, prefix_1):
    class_body = ""
    class_body += (
        "    constexpr uint32_t judge_bytes = all_bytes > 15 ? all_bytes - 15 : 0;\n"
    )
    class_body += "    uint32_t i = 0;\n"
    class_body += "    if (judge_bytes > 0) {\n"
    class_body += "        for (; i < judge_bytes; i += 16) { \n"
    class_body += f"            (*(uint64_t*)((uint8_t*)tilingdata + i)) = (*({prefix_0} uint64_t*)({prefix_1} + i));\n"
    class_body += f"            (*(uint64_t*)((uint8_t*)tilingdata + i + 8)) = (*({prefix_0} uint64_t*)({prefix_1} + i + 8));\n"
    class_body += "        }\n"
    class_body += "    }\n"
    class_body += "    if (all_bytes & 0x00000008) {\n"
    class_body += f"        (*(uint64_t*)((uint8_t*)tilingdata + i)) = (*({prefix_0} uint64_t *)({prefix_1} + i));\n"
    class_body += "        i += 8;\n"
    class_body += "    }\n"
    class_body += "    if (all_bytes & 0x00000004) {\n"
    class_body += f"        (*(uint32_t*)((uint8_t*)tilingdata + i)) = (*({prefix_0} uint32_t *)({prefix_1} + i));\n"
    class_body += "        i += 4;\n"
    class_body += "    }\n"
    class_body += "    if (all_bytes & 0x00000002) {\n"
    class_body += f"        (*(uint16_t*)((uint8_t*)tilingdata + i)) = (*({prefix_0} uint16_t *)({prefix_1} + i));\n"
    class_body += "        i += 2;\n"
    class_body += "    }\n"
    class_body += "    if (all_bytes & 0x00000001) {\n"
    class_body += f"        (*(uint8_t*)((uint8_t*)tilingdata + i)) = (*({prefix_0} uint8_t *)({prefix_1} + i));\n"
    class_body += "    }\n"
    return class_body


def gen_dynamic_shape(tiling_def, struct_tiling_def_base):
    class_name_upper = tiling_def.class_name.upper()
    class_body = ""
    class_body += f"#ifndef __{class_name_upper}_HEADER__\n"
    class_body += f"#define __{class_name_upper}_HEADER__\n\n"
    class_body += '#include "kernel_tiling/kernel_tiling.h"\n'
    class_body += "#ifdef ASCENDC_CPU_DEBUG\n"
    class_body += '#include "kernel_log.h"\n'
    class_body += "#else\n"
    class_body += "#ifndef __aicore__\n"
    class_body += "#define __aicore__ [aicore]\n"
    class_body += "#endif\n"
    class_body += "#endif\n"
    if not global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \n\n"
    class_body += get_struct_shape(struct_tiling_def_base)
    class_body += get_dynamic_tiling_struct(tiling_def, struct_tiling_def_base)
    class_body += get_tiling_copy_func_and_micro(tiling_def.class_name)
    class_body += generate_pointer_directly_assess_data()
    class_body += f"#endif // __{class_name_upper}_HEADER__\n"
    return class_body


class TilingInfo:
    def __init__(self):
        self.block_num: int = -1
        self.task_ration: int = 2  # AscendC only support 1:2
        self.file_content: str = ""
        self.tiling_data: bytes = bytes()
        self.tiling_data_file_path: str = ""
        self.tiling_data_size: int = 0  # TilingDef::getDataSize
        self.static_shape_flag: bool = True
        self.tiling_key: int = DEFAULT_TILING_KEY_VALUE
        self.static_workspace_size: int = 0
        self.tiling_key_list = []
        self.tiling_key_data_size = {}
        self.default_tiling_size: int = 0
        self.clear_atomic: bool = True
        self.schedule_mode: int = 0
        self.raw_run_info: dict = None
        self.local_memory_size: int = -1
        self.tiling_info_completed: bool = False

    def __str__(self):
        return ",".join(
            "{}={}".format(key, getattr(self, key)) for key in self.__dict__.keys()
        )

    def init_from_dict(self, info_dict):
        self.block_num = info_dict["block_num"]
        self.task_ration = info_dict["task_ration"]
        self.file_content = info_dict["file_content"]
        self.tiling_data = bytes.fromhex(info_dict["tiling_data"])
        self.tiling_data_file_path = info_dict["tiling_data_file_path"]
        self.tiling_data_size = info_dict["tiling_data_size"]
        self.static_shape_flag = info_dict["static_shape_flag"]
        self.tiling_key = info_dict["tiling_key"]
        self.static_workspace_size = info_dict["static_workspace_size"]
        self.tiling_key_list = info_dict["tiling_key_list"]
        self.tiling_key_data_size = info_dict["tiling_key_data_size"]
        self.default_tiling_size = info_dict["default_tiling_size"]
        self.clear_atomic = info_dict["clear_atomic"]
        self.schedule_mode = info_dict["schedule_mode"]
        self.raw_run_info = info_dict["raw_run_info"]
        self.local_memory_size = info_dict["local_memory_size"]
        self.tiling_info_completed = info_dict["tiling_info_completed"]

    def dump_to_dict(self):
        info_dict = {}
        info_dict["block_num"] = self.block_num
        info_dict["task_ration"] = self.task_ration
        info_dict["file_content"] = self.file_content
        info_dict["tiling_data"] = self.tiling_data.hex()
        info_dict["tiling_data_file_path"] = self.tiling_data_file_path
        info_dict["tiling_data_size"] = self.tiling_data_size
        info_dict["static_shape_flag"] = self.static_shape_flag
        info_dict["tiling_key"] = self.tiling_key
        info_dict["static_workspace_size"] = self.static_workspace_size
        info_dict["tiling_key_list"] = self.tiling_key_list
        info_dict["tiling_key_data_size"] = self.tiling_key_data_size
        info_dict["default_tiling_size"] = self.default_tiling_size
        info_dict["clear_atomic"] = self.clear_atomic
        info_dict["schedule_mode"] = self.schedule_mode
        info_dict["raw_run_info"] = self.raw_run_info
        info_dict["local_memory_size"] = self.local_memory_size
        info_dict["tiling_info_completed"] = self.tiling_info_completed
        return info_dict

    def save_file(self, file_path):
        self.tiling_data_file_path = file_path
        try:
            with open(file_path, "w") as new_file:
                # Only the owner and group have rights
                os.chmod(file_path, stat.S_IRUSR + stat.S_IWUSR + stat.S_IRGRP)
                new_file.write(self.file_content)
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, ("open file error, reason:", err)
            )

    def remove_file(self):
        os.remove(self.tiling_data_file_path)


def is_static_input_base_value_depend(input_ele, idx, value_depends: dict, param_types):
    if value_depends is None or value_depends.get(idx) is None or param_types is None:
        return True
    value_depend = value_depends.get(idx)
    param_type = param_types[idx]
    # if value depend but no const value, is not static
    if param_type == "required" or param_type == "dynamic":
        if value_depend == "required":
            if input_ele.get("const_value") is None:
                raise (
                    Exception(
                        "failed to compile the kernel with Required and ValueDepend Input, but without const value"
                    )
                )
            else:
                return True
        # if it's optional then check whether it has const value.
        return input_ele.get("const_value") is not None

    if param_type == "optional":
        if value_depend == "required":
            if input_ele.get("const_value") is None:
                raise (
                    Exception(
                        "failed to compile the kernel with Required and ValueDepend Input, but without const value"
                    )
                )
            else:
                return True
        elif value_depend == "optional":
            return (
                (input_ele.get("shape") is not None)
                and (input_ele.get("const_value") is None)
            ) == False
    return True


def _check_negative_shape(shapes):
    for shape in shapes:
        if shape < 0:
            return True
    return False


def _check_single_element_shape(ele, idx, enable_vd, value_depends, param_types):
    if _check_negative_shape(ele["shape"]):
        return True
    if enable_vd:
        if not is_static_input_base_value_depend(ele, idx, value_depends, param_types):
            return True
    return False


def _check_input_list_shape(input_list, idx, enable_vd, value_depends, param_types):
    for single in input_list:
        if _check_single_element_shape(
            single, idx, enable_vd, value_depends, param_types
        ):
            return True
    return False


def _check_inputs_shape(inputs, enable_vd, value_depends, param_types):
    for idx, input_ele in enumerate(inputs):
        if input_ele is None:
            continue
        if isinstance(input_ele, (list, tuple)):
            if _check_input_list_shape(
                input_ele, idx, enable_vd, value_depends, param_types
            ):
                return True
        else:
            if _check_single_element_shape(
                input_ele, idx, enable_vd, value_depends, param_types
            ):
                return True
    return False


def _check_outputs_shape(outputs):
    if outputs is None:
        return False
    for output_ele in outputs:
        if output_ele is None:
            continue
        if _check_negative_shape(output_ele["shape"]):
            return True
    return False


def is_static_shape(
    inputs: list,
    outputs: list,
    value_depends: dict = None,
    param_types: list = None,
    enable_vd=False,
):
    """check if static shape, find dynamic shape if shape<0 in inputs

    Args:
    inputs (list): shape/ori_shape/format/ori_format/dtype/name, type including required/optional/dynamic,
                        if shape of inputs is const, there will be an extra field "const value".
    value_depends: whether inputs are value depened.
    Returns:
        res (Boolean): True means static_shape, False means dynamic shape
    """
    if not inputs and not outputs:
        mode = op_context.get_op_mode()
        return mode != "dynamic"
    if _check_inputs_shape(inputs, enable_vd, value_depends, param_types):
        return False
    if _check_outputs_shape(outputs):
        return False
    return True


def get_struct_tiling_info(tiling_def, struct_tiling_def_base, depth=0):
    depth += 1
    for field in tiling_def.field_list:
        if field.class_type == 2:
            if field.struct_type not in struct_tiling_def_base:
                struct_optype = field.struct_type + "OpApi"
                struct_tiling_def = get_tiling_def(struct_optype)
                # tiling struct in tikcpp advance api
                if struct_tiling_def is not None:
                    struct_tiling_def.is_api = True
                # op used struct, defined by user
                else:
                    struct_optype = field.struct_type + "Op"
                    struct_tiling_def = get_tiling_def(struct_optype)
                    if struct_tiling_def is None:
                        msg = "get_op_tiling.py:_decode_struct_tiling_data optype: {} is not define, ".format(
                            struct_optype
                        )
                        raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, (msg))
                struct_tiling_def.depth = depth
                struct_tiling_def_base[field.struct_type] = struct_tiling_def
                struct_tiling_def_base = get_struct_tiling_info(
                    struct_tiling_def, struct_tiling_def_base, depth
                )
    return struct_tiling_def_base


def get_dynamic_tiling_struct(tiling_def, struct_tiling_def_base):
    """
    get tiling class def, init func and macro
    :param tiling_def:
    :param struct_tiling_def_base:
    :return:
    """
    class_def = f"class {tiling_def.class_name}\n"
    class_def += "{\n"
    class_def += "public:\n"
    field_list: list = tiling_def.field_list
    for field in field_list:
        if field.class_type == 0:
            class_def += f"    {field.dtype} {field.name};\n"
        elif field.class_type == 1:
            class_def += f"    {field.dtype} {field.name}[{field.arr_size}];\n"
        elif field.class_type == 2:
            class_def += "    "
            # if field in struct is from api, add a namespace
            if struct_tiling_def_base[field.struct_type].is_api:
                class_def += f"{_TILING_NAMESPACE}::"
            class_def += f"{field.struct_type} {field.name};\n"
    class_def += "}__attribute__((__may_alias__));\n\n"
    return class_def


def get_header_and_sub_struct_def(tiling_def, struct_tiling_def_base):
    class_name_upper = tiling_def.class_name.upper()
    start_body = ""
    start_body += f"#ifndef __{class_name_upper}_HEADER__\n"
    start_body += f"#define __{class_name_upper}_HEADER__\n\n"
    start_body += '#include "kernel_tiling/kernel_tiling.h"\n'
    start_body += "#ifdef ASCENDC_CPU_DEBUG\n"
    start_body += '#include "kernel_log.h"\n'
    start_body += "#else\n"
    start_body += "#ifndef __aicore__\n"
    start_body += "#define __aicore__ [aicore]\n"
    start_body += "#endif\n"
    start_body += "#endif\n"
    if not global_var_storage.get_variable("ascendc_tiling_no_register"):
        start_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \n\n"
    start_body += get_struct_shape(struct_tiling_def_base)
    start_body += generate_pointer_directly_assess_data()
    end_body = f"#endif // __{class_name_upper}_HEADER__\n"
    return start_body, end_body


def get_tilingdata_preload():
    class_body = "constexpr uint32_t DC_PRLOAD_LOOP = (all_bytes)/512;\n"
    class_body += "for (uint64_t loop_dc=0; loop_dc < DC_PRLOAD_LOOP; loop_dc++) {\n"
    class_body += "    uint64_t offset = loop_dc*512;\n"
    class_body += "    dc_preload((uint64_t *)tilingdata, offset);\n"
    class_body += "}\n"
    class_body += "uint64_t tiling_offset = ((uint64_t)(&tilingdata))%64;\n"
    class_body += "uint32_t DC_PRLOAD_LOOP1 = (all_bytes+63+tiling_offset)/64;\n"
    class_body += "for (uint64_t loop_dc=0; loop_dc < DC_PRLOAD_LOOP1; loop_dc++) {\n"
    class_body += "    uint64_t offset = loop_dc*64;\n"
    class_body += "    dc_preload((uint64_t *)tilingdata, offset);\n"
    class_body += "}\n\n"
    return class_body


def _gen_tiling_copy_through_reserved_ub():
    class_body = get_tilingdata_preload()
    class_body += """
    __ubuf__ uint8_t *tilingdata_in_ub = (__ubuf__ uint8_t *)get_imm(248 * 1024);
    constexpr uint32_t copy_tile_size = 8192;
    for (uint32_t stack_offset = 0; stack_offset < all_bytes; stack_offset += copy_tile_size) {
        uint32_t copy_size = stack_offset + copy_tile_size < all_bytes ? copy_tile_size : all_bytes - stack_offset;
        copy_gm_to_ubuf_align_v2(((__ubuf__ uint8_t *)tilingdata_in_ub),
        ((__gm__ uint8_t *)p_tilingdata) + stack_offset, 0, 1, copy_size, 0, 0, false, 0, 0, 0);
        set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
        wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
        copy_data_align64(((uint8_t*)tilingdata) + stack_offset, ((__ubuf__ uint8_t *)tilingdata_in_ub), copy_size);
        set_flag(PIPE_S, PIPE_MTE2, EVENT_ID0);
        wait_flag(PIPE_S, PIPE_MTE2, EVENT_ID0);
    }
    """
    return class_body


def _get_tiling_data_without_time_stamp(class_name):
    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body = "#define GET_TILING_DATA(tiling_data, tiling_arg)                             \n"
    else:
        class_body = "#define GET_TILING_DATA(tiling_data, tiling_arg)                             \\\n"
        class_body += "    struct __AUX__ascendc_struct_mocker_##tiling_data{                     \\\n"
        class_body += f"        uint8_t data[sizeof({class_name})];                                 \\\n"
        class_body += "    }__attribute__((__may_alias__));                                       \\\n"
        class_body += "    __AUX__ascendc_struct_mocker_##tiling_data __aux__tiling_##tiling_data; \\\n"
        class_body += f"    InitTilingData<{class_name}>(tiling_arg,                                \\\n"
        class_body += f"        reinterpret_cast<{class_name}*>(&__aux__tiling_##tiling_data));     \\\n"
        class_body += f"    {class_name} &tiling_data =                                             \\\n"
        class_body += f"        reinterpret_cast<{class_name} &>(*(&__aux__tiling_##tiling_data));  \n\n"

    class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)                 \\\n"
    class_body += "    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                                   \\\n"
    class_body += "    struct __AUX__STRUCT_ascendc_struct_mocker_##tiling_data{                               \\\n"
    class_body += "        uint8_t data[sizeof(tiling_struct)];                                                \\\n"
    class_body += "    }__attribute__((__may_alias__));                                                        \\\n"
    class_body += "    __AUX__STRUCT_ascendc_struct_mocker_##tiling_data __aux__tiling_##tiling_data;          \\\n"
    class_body += "    InitTilingData<tiling_struct>(tiling_arg,                                               \\\n"
    class_body += "        reinterpret_cast<tiling_struct*>(&__aux__tiling_##tiling_data));                    \\\n"
    class_body += "    tiling_struct &tiling_data = reinterpret_cast<tiling_struct &>(*(&__aux__tiling_##tiling_data));  \n\n"

    class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)                     \\\n"
    class_body += "    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                              \\\n"
    class_body += "    struct __AUX__MEMBER##var {                                                      \\\n"
    class_body += "        uint8_t data[sizeof(decltype(((tiling_type *)0)->member))];                  \\\n"
    class_body += "    }__attribute__((__may_alias__));                                                 \\\n"
    class_body += "    __AUX__MEMBER##var __aux__##var;                                                 \\\n"
    class_body += "    size_t offset##var = (size_t)(&((tiling_type*)0)->member);                       \\\n"
    class_body += "    InitTilingData<decltype(((tiling_type *)0)->member)>(tiling + offset##var,       \\\n"
    class_body += "        reinterpret_cast<decltype(((tiling_type *)0)->member)*>(&__aux__##var));     \\\n"
    class_body += "    decltype(((tiling_type *)0)->member) &var =                                      \\\n"
    class_body += "        reinterpret_cast<decltype(((tiling_type *)0)->member) &>(*(&__aux__##var));  \n\n"
    return class_body


def _get_tiling_data_with_time_stamp(class_name):
    class_body = "#ifdef ASCENDC_TIME_STAMP_ON\n"
    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                             \n"
    else:
        class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                            \\\n"
        class_body += (
            f"    {class_name} tiling_data;                                    \\\n"
        )
        class_body += f"    InitTilingData<{class_name}>(tiling_arg, &tiling_data);\n"
    class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg)  \\\n"
    class_body += "    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                    \\\n"
    class_body += "    tiling_struct tiling_data;                                               \\\n"
    class_body += "    InitTilingData<tiling_struct>(tiling_arg, &tiling_data);\n"

    class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)            \\\n"
    class_body += "    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                     \\\n"
    class_body += "    decltype(((tiling_type *)0)->member) var;                               \\\n"
    class_body += "    size_t offset##var = (size_t)(&((tiling_type*)0)->member);              \\\n"
    class_body += "    InitTilingData<decltype(((tiling_type *)0)->member)>(tiling + offset##var, &var);\n"

    class_body += "#else\n"

    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                             \n"
    else:
        class_body += "#define GET_TILING_DATA(tiling_data, tiling_arg)                            \\\n"
        class_body += (
            f"    {class_name} tiling_data;                                    \\\n"
        )
        class_body += f"    InitTilingData<{class_name}>(tiling_arg, &tiling_data);\n"

    class_body += "#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg) \\\n"
    class_body += "    REGISTER_TILINGDATA_SIZE(tiling_struct, __COUNTER__);                    \\\n"
    class_body += "    tiling_struct tiling_data;                                              \\\n"
    class_body += "    InitTilingData<tiling_struct>(tiling_arg, &tiling_data);\n"

    class_body += "#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling)            \\\n"
    class_body += "    REGISTER_TILINGDATA_SIZE(tiling_type, __COUNTER__);                     \\\n"
    class_body += "    decltype(((tiling_type *)0)->member) var;                               \\\n"
    class_body += "    size_t offset##var = (size_t)(&((tiling_type*)0)->member);              \\\n"
    class_body += "    InitTilingData<decltype(((tiling_type *)0)->member)>(tiling + offset##var, &var);\n"
    class_body += "#endif\n"
    return class_body


def get_tiling_data_func_head():
    class_body = "#if defined(ASCENDC_CPU_DEBUG)\n"
    class_body += "template <class T>\n"
    class_body += "inline __aicore__ void InitTilingData(const __gm__ uint8_t *p_tilingdata, T *tilingdata)\n"
    class_body += "#else\n"
    class_body += "template <class T>\n"
    class_body += (
        "__inline__ __attribute__((always_inline)) __aicore__ void InitTilingData(const __gm__ uint8_t \
*p_tilingdata, T *tilingdata)\n"
    )
    class_body += "#endif\n"
    return class_body


def get_tiling_data_func():
    class_body = "{\n"
    class_body += "    constexpr uint64_t all_bytes = sizeof(T);\n"
    class_body += (
        "#if defined(ASCENDC_CPU_DEBUG) || (defined(__DAV_CUBE__) && __NPU_ARCH__ == 2201) || (defined \
    (__DAV_CUBE__) && __NPU_ARCH__ == 3510) || defined(__GET_CODE_CHANNEL__)\n"
    )
    class_body += "#if defined(__DAV_C100__) || defined(ASCENDC_CPU_DEBUG)\n"
    class_body += get_dynamic_assign_tiling_data_by_size(
        "all_bytes",
        "const __gm__",
        "(const __gm__ uint8_t *)\
p_tilingdata",
    )
    class_body += "#else\n"
    class_body += "    copy_data_align64((uint8_t*)tilingdata, (__gm__ uint8_t *)p_tilingdata, all_bytes);\n"
    class_body += "#endif\n"
    class_body += "#else\n"
    class_body += "#if __NPU_ARCH__ == 3510 && defined(__ASCENDC_ENABLE_VEC_TAIL_TILING_COPY__) \n"
    class_body += "#if defined(__ASC_DISABLE_RESERVED_UBUF__)\n"
    class_body += (
        '#error "GET_TILING_DATA macros using reserved UB are forbidden when compile option '
        '--cce-disable-asc-reserved-ubuf is enabled"\n'
    )
    class_body += "#endif\n"
    class_body += _gen_tiling_copy_through_reserved_ub()
    class_body += "#else \n"
    class_body += (
        "    __ubuf__ uint8_t *tilingdata_in_ub = (__ubuf__ uint8_t *)get_imm(0);\n"
    )
    class_body += "    constexpr uint32_t len_burst = (all_bytes + 31) / 32;\n"
    class_body += "#if __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102\n"
    class_body += "    copy_gm_to_ubuf_align_v2((__ubuf__ uint8_t *)tilingdata_in_ub, \
(__gm__ uint8_t *)p_tilingdata, 0, 1, len_burst * 32, 0, 0, false, 0, 0, 0);\n"
    class_body += get_tilingdata_preload()
    class_body += (
        "#elif __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || \
__NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163\n"
    )
    class_body += "    copy_gm_to_ubuf_align_v2((__ubuf__ uint8_t *)tilingdata_in_ub, \
(__gm__ uint8_t *)p_tilingdata, 0, 1, len_burst * 32, 0, 0, false, 0, 0);\n"
    class_body += "#elif __NPU_ARCH__ != 3102\n"
    class_body += (
        "    copy_gm_to_ubuf(((__ubuf__ uint8_t *)tilingdata_in_ub), p_tilingdata, 0, 1,\
len_burst, 0, 0);\n"
    )
    class_body += "#else\n"
    class_body += (
        "    copy_gm_to_ubuf_align(((__ubuf__ uint8_t *)tilingdata_in_ub), (__gm__ uint8_t *)p_tilingdata,\
0, 1, all_bytes, 0, 0, 0, 0);\n"
    )
    class_body += "#endif\n"
    class_body += "    set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);\n"
    class_body += "    wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);\n"
    class_body += "#if defined __DAV_C100__\n"
    class_body += get_dynamic_assign_tiling_data_by_size(
        "all_bytes", "__ubuf__", "(__ubuf__ uint8_t *)tilingdata_in_ub"
    )
    class_body += "#else\n"
    class_body += (
        "#if __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || \
__NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163\n"
    )
    class_body += (
        "    constexpr uint64_t bytes_align64 = (all_bytes + 63) / 64 * 64 + 8;\n"
    )
    class_body += "    copy_data_align64((uint8_t*)tilingdata, (__ubuf__ uint8_t *)tilingdata_in_ub, bytes_align64);\n"
    class_body += "#else\n"
    class_body += "    copy_data_align64((uint8_t*)tilingdata, (__ubuf__ uint8_t *)tilingdata_in_ub, all_bytes);\n"
    class_body += "#endif\n"
    class_body += "#endif\n"
    class_body += "#endif // __ASCENDC_ENABLE_VEC_TAIL_TILING_COPY__ \n"
    class_body += "#endif\n"
    class_body += "#ifndef ASCENDC_CPU_DEBUG\n"
    class_body += "    pipe_barrier(PIPE_ALL);\n"
    class_body += "#endif\n"
    class_body += "}\n\n"
    return class_body


def get_tiling_copy_func_and_micro(class_name):
    class_body = get_tiling_data_func_head()
    class_body += get_tiling_data_func()

    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    if short_soc_version in ["Ascend950", "MC62", "MC32DM11A"]:
        # use __AUX__ to create a new struct to reduce running time. __AUX__ name does not matter
        # original: initialize, then write value    use __AUX__: write value, then interpret_cast to needed struct
        class_body += _get_tiling_data_without_time_stamp(class_name)
    else:
        class_body += _get_tiling_data_with_time_stamp(class_name)
    return class_body


def _change_param_name_to_name(inputs):
    for input_ele in inputs:
        if input_ele is None:
            continue
        if isinstance(input_ele, (list, tuple)):
            for single in input_ele:
                if "const_value" in single.keys():
                    single["name"] = single.get("param_name")
        else:
            if "const_value" in input_ele.keys():
                input_ele["name"] = input_ele.get("param_name")


def gen_static_shape_v2(optype: str, tiling_struct: str, tiling_raw_data: str):
    class_name_upper = optype.upper()
    class_body = f"#ifndef __{class_name_upper}_HEADER__\n"
    class_body += f"#define __{class_name_upper}_HEADER__\n"
    class_body += '#include "kernel_tiling/kernel_tiling.h"\n'
    class_body += "#ifdef ASCENDC_CPU_DEBUG\n"
    class_body += '#include "kernel_log.h"\n'
    class_body += "#else\n"
    class_body += "#ifndef __aicore__\n"
    class_body += "#define __aicore__ [aicore]\n"
    class_body += "#endif\n"
    class_body += "#endif\n"
    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body += "#define ASCENDC_INTERNAL_STR(x) #x \n"
        class_body += "#define ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(x) ASCENDC_INTERNAL_STR(x) \n"
        class_body += "#define ASCENDC_INTERNAL_CONCAT_IMPL(x, y) x##y \n"
        class_body += "#define ASCENDC_INTERNAL_CONCAT(x, y) ASCENDC_INTERNAL_CONCAT_IMPL(x, y) \n"
        class_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \\\n"
        class_body += (
            "    static constexpr uint64_t ASCENDC_INTERNAL_CONCAT(__ascend_tiling_struct_, \
            ASCENDC_INTERNAL_CONCAT(TILING_KEY_VAR, counter)) \\\n"
        )
        class_body += "__attribute__((used, section( \\\n"
        class_body += '        ".ascendc_tiling." \\\n'
        class_body += '        ASCENDC_INTERNAL_STR(tiling_struct) "_" \\\n'
        class_body += (
            '        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(TILING_KEY_VAR) "." \\\n'
        )
        class_body += "        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(counter) \\\n"
        class_body += "    ))) = sizeof(tiling_struct); \n"
    else:
        class_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \n"

    body, tiling_assign_str = gen_micro_assign_value_of_tiling(
        tiling_struct, tiling_raw_data
    )
    class_body += body

    class_body += generate_pointer_directly_assess_data(False, False, tiling_assign_str)
    class_body += f"#endif // __{class_name_upper}_HEADER__\n"
    return class_body


def gen_dynamic_shape_v2(optype: str, tiling_struct: str):
    class_name_upper = optype.upper()
    class_body = f"#ifndef __{class_name_upper}_HEADER__\n"
    class_body += f"#define __{class_name_upper}_HEADER__\n\n"
    class_body += '#include "kernel_tiling/kernel_tiling.h"\n'
    class_body += "#ifdef ASCENDC_CPU_DEBUG\n"
    class_body += '#include "kernel_log.h"\n'
    class_body += "#else\n"
    class_body += "#ifndef __aicore__\n"
    class_body += "#define __aicore__ [aicore]\n"
    class_body += "#endif\n"
    class_body += "#endif\n"
    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        class_body += "#define ASCENDC_INTERNAL_STR(x) #x \n"
        class_body += "#define ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(x) ASCENDC_INTERNAL_STR(x) \n"
        class_body += "#define ASCENDC_INTERNAL_CONCAT_IMPL(x, y) x##y \n"
        class_body += "#define ASCENDC_INTERNAL_CONCAT(x, y) ASCENDC_INTERNAL_CONCAT_IMPL(x, y) \n"
        class_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \\\n"
        class_body += (
            "    static constexpr uint64_t ASCENDC_INTERNAL_CONCAT(__ascend_tiling_struct_, \
            ASCENDC_INTERNAL_CONCAT(TILING_KEY_VAR, counter)) \\\n"
        )
        class_body += "__attribute__((used, section( \\\n"
        class_body += '        ".ascendc_tiling." \\\n'
        class_body += '        ASCENDC_INTERNAL_STR(tiling_struct) "_" \\\n'
        class_body += (
            '        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(TILING_KEY_VAR) "." \\\n'
        )
        class_body += "        ASCENDC_INTERNAL_EXPAND_AND_STRINGIFY(counter) \\\n"
        class_body += "    ))) = sizeof(tiling_struct); \n"
    else:
        class_body += "#define REGISTER_TILINGDATA_SIZE(tiling_struct, counter) \n"
    class_body += get_tiling_copy_func_and_micro(tiling_struct)
    class_body += generate_pointer_directly_assess_data()
    class_body += f"#endif // __{class_name_upper}_HEADER__\n"
    return class_body


def _set_runtime_soc_version():
    try:
        full_soc = get_soc_spec("FULL_SOC_VERSION")
        mylib = ctypes.CDLL("libruntime.so")
        mylib.rtSetSocVersion.argtypes = [ctypes.c_char_p]
        mylib.rtSetSocVersion.restype = ctypes.c_int
        rt_ret = mylib.rtSetSocVersion(full_soc.encode("utf-8"))
        if rt_ret != 0:
            LogUtil.print_compile_log(
                "",
                f"ascendc cannot rtSetSoCVersion: {full_soc}",
                AscendCLogLevel.LOG_INFO,
            )
    except OSError:
        LogUtil.print_compile_log(
            "", "ascendc cannot load libruntime.so", AscendCLogLevel.LOG_INFO
        )
        return
    except Exception as e:
        LogUtil.print_compile_log(
            "", f"ascendc cannot set runtime soc version: {e}", AscendCLogLevel.LOG_INFO
        )
        return


def get_tiling_info_v2(
    op_info: OpInfo,
    tiling_key_list: list,
    default_tiling_struct: str,
    tiling_struct_expr_map: dict,
    value_depends: dict = None,
    enable_vd=False,
):
    """get tiling define v2

    Args:
        optype (str): operator type
        tiling_key_list (list): tiling_key used by developer
        default_tiling_struct (str): default tiling struct
        tiling_struct_expr_map (dict): map of (tiling, tiling_struct)

    Returns:
        tiling_info (TilingInfo): tiling info formatted by tiling define and tiling data
    """
    optype = op_info.op_type
    inputs = op_info.inputs
    outputs = op_info.outputs
    attrs = op_info.attrs
    tiling_info = TilingInfo()
    tiling_info.tiling_key_list = tiling_key_list
    static_shape = is_static_shape(
        op_info.origin_inputs,
        outputs,
        value_depends,
        op_info.param_type_list,
        enable_vd,
    )
    context = get_context()
    if static_shape:
        _set_runtime_soc_version()
        _change_param_name_to_name(inputs)
        _change_param_name_to_name(op_info.origin_inputs)
        compile_info = context.get_compile_info()
        tiling_config = {
            "name": "ascendc_op_para_size",
            "dtype": "int",
            "value": 2 * 1024 * 1024,
        }
        attrs.append(tiling_config)
        run_info = do_op_tiling(
            optype,
            compile_info,
            op_info.origin_inputs,
            op_info.origin_outputs,
            None,
            None,
            attrs,
        )
        # bytes.fromhex(run_info['tiling_data']) can deserialization DumpByteBuffer
        # save undecoded tiling data for replay
        tiling_info.tiling_data = run_info["tiling_data"]
        tiling_info.tiling_key = run_info["tiling_key"]
        if "local_memory_size" in run_info:
            tiling_info.local_memory_size = run_info["local_memory_size"]
        tiling_info.block_num = run_info["block_dim"]
        tiling_info.clear_atomic = run_info["clear_atomic"]
        tiling_info.schedule_mode = run_info.get("schedule_mode", 0)
        total_workspace_size = sum(run_info["workspaces"])
        if not global_var_storage.get_variable("ascendc_tiling_no_register"):
            tiling_info.file_content = gen_static_shape_v2(
                optype,
                tiling_struct_expr_map[str(tiling_info.tiling_key)],
                run_info["tiling_data"],
            )
        else:
            tiling_info.file_content = gen_static_shape_v2(
                optype, "ascendc_trigger_tiling_struct", run_info["tiling_data"]
            )
        tiling_info.static_workspace_size = total_workspace_size
        run_info["tiling_data"] = tiling_info.tiling_data.hex()
        tiling_info.raw_run_info = run_info
        if total_workspace_size > 0:
            if len(run_info["workspaces"]) > 1:
                msg = "the num of workspace can not large than 1"
                raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, msg)
            context.add_workspace("total_workspace", size=total_workspace_size)
    else:
        tiling_info.static_shape_flag = False
        context.add_workspace("total_workspace", size=-1)
        if not global_var_storage.get_variable("ascendc_tiling_no_register"):
            tiling_info.file_content = gen_dynamic_shape_v2(
                optype, default_tiling_struct
            )
        else:
            tiling_info.file_content = gen_dynamic_shape_v2(
                optype, "ascendc_trigger_tiling_struct"
            )
    return tiling_info


def get_isolate_tiling_info(op_type, json_file):
    tiling_info = TilingInfo()
    try:
        if not os.path.exists(json_file):
            LogUtil.print_compile_log(
                op_type,
                f"[Main process] isolate tiling json file {json_file} not existed",
                AscendCLogLevel.LOG_ERROR,
            )

        with open(json_file, "r", encoding="utf-8") as f:
            info_dict = json.load(f)
            tiling_info.init_from_dict(info_dict["tiling_info"])
    except Exception as e:
        LogUtil.print_compile_log(
            op_type,
            f"[Main process] load isolate tiling json file: {json_file} failed, exception info: {e}",
            AscendCLogLevel.LOG_ERROR,
        )
    return tiling_info


def get_info_by_traverse_py_stack(op_info: OpInfo, input_tiling_info_dict: dict):
    # offline compile python file name, i.e. AddCustom.py
    offline_op_compile_file = op_info.op_type + ".py"

    # online compile python file name, i.e. add_custom.py
    origin_func_name = input_tiling_info_dict["origin_func_name"]
    online_op_compile_file = origin_func_name + ".py"

    is_offline_op = False
    is_build_in_op = False
    custom_op_tiling_path = None

    # traverse python stack files
    op_compile_stack = inspect.stack()
    for frame_info in op_compile_stack:
        frame_file = os.path.basename(frame_info.filename)
        op_compile_dir = os.path.dirname(os.path.abspath(frame_info.filename))
        if offline_op_compile_file == frame_file:
            # offline op compile
            custom_opp_offline_path = os.path.join(
                op_compile_dir, "../customize", _TILING_SO_PATH
            )
            if os.path.exists(custom_opp_offline_path):
                custom_op_tiling_path = custom_opp_offline_path
                is_offline_op = True
            else:
                LogUtil.print_compile_log(
                    op_info.op_type,
                    f"[Main process] Custom tiling so not existed: {custom_opp_offline_path} in offline compile op.",
                    AscendCLogLevel.LOG_WARNING,
                )
            break
        elif online_op_compile_file == frame_file:
            # online op compile
            # built-in op
            build_in_compile_file_dir = os.path.join(
                op_impl_path, "ai_core", "tbe", "impl"
            )
            if build_in_compile_file_dir in op_compile_dir:
                LogUtil.print_compile_log(
                    op_info.op_type,
                    "[Main process] Identified in Build-in online compile.",
                    AscendCLogLevel.LOG_INFO,
                )
                is_build_in_op = True
                break

            # custom op
            custom_opp_online_path = os.path.join(
                op_compile_dir, "../../op_tiling/liboptiling.so"
            )
            if os.path.exists(custom_opp_online_path):
                custom_op_tiling_path = custom_opp_online_path
            else:
                LogUtil.print_compile_log(
                    op_info.op_type,
                    f"[Main process] Custom tiling so not existed: {custom_opp_online_path} in online compile op.",
                    AscendCLogLevel.LOG_WARNING,
                )
            break
    return is_offline_op, is_build_in_op, custom_op_tiling_path


def get_tiling_info_isolate(op_info: OpInfo, input_tiling_info_dict: dict):
    is_offline_op, is_build_in_op, custom_op_tiling_path = (
        get_info_by_traverse_py_stack(op_info, input_tiling_info_dict)
    )

    if is_offline_op:
        tiling_info = get_custom_tiling_info(
            op_info, input_tiling_info_dict, custom_op_tiling_path
        )
        if tiling_info.tiling_info_completed:
            return tiling_info
        else:
            CommonUtility.print_compile_log(
                op_info.op_type,
                "offline build op generate tiling_info failed.",
                AscendCLogLevel.LOG_WARNING,
            )
    elif is_build_in_op or custom_op_tiling_path is not None:
        kernel_meta_path = CommonUtility.get_kernel_meta_dir()
        isolate_json = {
            "optype": op_info.op_type,
            "is_build_in_op": is_build_in_op,
            "custom_op_tiling_path": custom_op_tiling_path,
            "kernel_meta_path": kernel_meta_path,
            "tiling_key_list": input_tiling_info_dict["tiling_key_list"],
            "tiling_key_group_map": input_tiling_info_dict["tiling_key_group_map"],
            "is_static_shape": False,
            "tiling_const_propagation": global_var_storage.get_variable(
                "ascendc_tiling_const_propagation"
            ),
        }
        is_static_flag = is_static_shape(
            op_info.origin_inputs,
            op_info.outputs,
            input_tiling_info_dict["value_depends"],
            op_info.param_type_list,
            input_tiling_info_dict["enable_vd"],
        )
        context = get_context()

        if is_static_flag:
            run_info = get_static_run_info(op_info, context)
            isolate_json["is_static_shape"] = True
            run_info["tiling_data"] = run_info["tiling_data"].hex()
            isolate_json["run_info"] = run_info

        isolate_json_str = json.dumps(isolate_json, ensure_ascii=False, indent=2)
        isolate_json_path = os.path.join(
            kernel_meta_path, op_info.op_type + f"_isolate_tiling_{os.getpid()}.json"
        )
        with open(isolate_json_path, "w", encoding="utf-8") as f:
            f.write(isolate_json_str)

        isolate_python_path = os.path.join(
            os.path.dirname(os.path.abspath(__file__)),
            "ascendc_gen_tiling_struct_isolate.py",
        )
        soc_version = get_soc_spec("FULL_SOC_VERSION")
        result = subprocess.run(
            [
                "python3",
                isolate_python_path,
                op_info.op_type,
                isolate_json_path,
                soc_version,
            ]
        )

        if result.returncode == 0:
            tiling_info = get_isolate_tiling_info(op_info.op_type, isolate_json_path)
            if tiling_info.tiling_info_completed:
                if is_static_flag:
                    static_post_process_of_workspace(
                        context, run_info, tiling_info.static_workspace_size
                    )
                else:
                    context.add_workspace("total_workspace", size=-1)
                CommonUtility.print_compile_log(
                    op_info.op_type,
                    "online build op generate tiling_info success.",
                    AscendCLogLevel.LOG_INFO,
                )
                return tiling_info
            else:
                CommonUtility.print_compile_log(
                    op_info.op_type,
                    "online build op generate tiling_info failed.",
                    AscendCLogLevel.LOG_WARNING,
                )

    # get tiling through old version
    CommonUtility.print_compile_log(
        op_info.op_type,
        "isolate gen tiling file failed, retry gen tiling file through old version.",
        AscendCLogLevel.LOG_INFO,
    )
    return get_tiling_info(
        op_info,
        input_tiling_info_dict["tiling_key_list"],
        input_tiling_info_dict["value_depends"],
        input_tiling_info_dict["enable_vd"],
        input_tiling_info_dict["tiling_key_group_map"],
    )


def get_tiling_info(
    op_info: OpInfo,
    tiling_key_list: list = None,
    value_depends: dict = None,
    enable_vd=False,
    tiling_key_group_map: dict = None,
):
    """get tiling define and tiling data registered by operator developer

    Args:
        optype (str): operator type
        tiling_key_list (list): tiling_key_list of op
        value_depends (list): value_depends info of op
        enable_vd : is support value depends

    Returns:
        tiling_info (TilingInfo): tiling info formatted by tiling define and tiling data
    """
    load_lib()
    return process_tiling_info(
        op_info, tiling_key_list, value_depends, enable_vd, tiling_key_group_map
    )


def load_custom_lib(custom_op_tiling_path):
    try:
        if not os.path.exists(custom_op_tiling_path):
            LogUtil.print_compile_log(
                "", f"{custom_op_tiling_path} not exists", AscendCLogLevel.LOG_INFO
            )
            return False
        else:
            lib_optiling = ctypes.CDLL(custom_op_tiling_path)
            custom_op_tiling_path_str = str(custom_op_tiling_path)
            lib_optiling.TbeLoadSoAndSaveToRegistry(
                custom_op_tiling_path_str.encode("utf_8")
            )
            return True
    except OSError as e:
        # Custom op tiling lib may not exists
        LogUtil.print_compile_log(
            "",
            f"An OSError occurred: {e}, \
tiling_path: {custom_op_tiling_path}",
            AscendCLogLevel.LOG_ERROR,
        )
        raise Exception(
            f"An OSError occurred, tiling_path: {custom_op_tiling_path}"
        ) from e
    except Exception as e:
        # Custom op tiling lib may not exists
        LogUtil.print_compile_log(
            "",
            f"An Unknown error occurred: {e}, \
tiling_path: {custom_op_tiling_path}",
            AscendCLogLevel.LOG_ERROR,
        )
        raise Exception(
            f"An Unknown error occurred, tiling_path: {custom_op_tiling_path}"
        ) from e
    return False


def get_custom_tiling_info(
    op_info: OpInfo, input_tiling_info_dict: dict, custom_op_tiling_path
):
    """get tiling define and tiling data registered by operator developer

    Args:
        op_info(tuple): operator infos
        input_tiling_info_dict(dict): extra tiling info of op
        custom_op_tiling_path: tiling so path

    Returns:
        tiling_info (TilingInfo): tiling info formatted by tiling define and tiling data
    """
    load_custom_lib(custom_op_tiling_path)
    return process_tiling_info(
        op_info,
        input_tiling_info_dict["tiling_key_list"],
        input_tiling_info_dict["value_depends"],
        input_tiling_info_dict["enable_vd"],
        input_tiling_info_dict["tiling_key_group_map"],
    )


def generate_dynamic_tiling_struct_file(
    optype, tiling_info, tiling_key_list, tiling_key_group_map
):
    struct_tiling_def_base = {}
    tiling_def_list_of_key = []
    tiling_max_data_size = 0

    optype_tiling_def = get_tiling_def(optype)
    if optype_tiling_def is None:
        LogUtil.print_compile_log(
            optype, "do not registe tiling struct!!!", AscendCLogLevel.LOG_ERROR
        )
        return

    struct_tiling_def_base = get_struct_tiling_info(
        optype_tiling_def, struct_tiling_def_base
    )

    for tiling_key in tiling_key_list:
        optype_with_tilingkey = optype + "_" + tiling_key
        tiling_def = get_tiling_def(optype_with_tilingkey)
        if tiling_key in tiling_key_group_map.keys():
            tiling_def = _get_tiling_def_with_group(
                tiling_def, optype, tiling_key, tiling_key_group_map
            )
        if tiling_def is not None:
            struct_tiling_def_base = get_struct_tiling_info(
                tiling_def, struct_tiling_def_base
            )
            tiling_def.class_def = get_dynamic_tiling_struct(
                tiling_def, struct_tiling_def_base
            )
            tiling_def.tiling_key = tiling_key
            tiling_def_list_of_key.append(tiling_def)
            tiling_max_data_size = max(tiling_max_data_size, tiling_def.data_size)
            tiling_info.tiling_key_data_size[tiling_key] = tiling_def.data_size

    # do not have tiling key return tiling header file of optype
    if len(tiling_def_list_of_key) == 0:
        optype_tiling_def.generate_code = gen_dynamic_shape(
            optype_tiling_def, struct_tiling_def_base
        )
        tiling_info.file_content = optype_tiling_def.generate_code
    else:  # deal with tiling header file wiht tiling key
        optype_tiling_def.class_def = get_dynamic_tiling_struct(
            optype_tiling_def, struct_tiling_def_base
        )

        # begin tiling header file
        tiling_info.file_content, end_body = get_header_and_sub_struct_def(
            optype_tiling_def, struct_tiling_def_base
        )

        tiling_info.file_content += "// begin def of all tiling struct\n"
        tiling_info.file_content += optype_tiling_def.class_def
        tiling_struct_dict = set()
        for tiling_def in tiling_def_list_of_key:
            if tiling_def.class_name not in tiling_struct_dict:
                tiling_info.file_content += tiling_def.class_def
                tiling_struct_dict.add(tiling_def.class_name)
        tiling_info.file_content += get_tiling_copy_func_and_micro(
            optype_tiling_def.class_name
        )
        tiling_info.file_content += end_body
    tiling_info.tiling_data_size = max(
        tiling_max_data_size, optype_tiling_def.data_size
    )
    tiling_info.default_tiling_size = optype_tiling_def.data_size
    tiling_info.tiling_info_completed = True
    return


def get_static_run_info(op_info, context):
    _set_runtime_soc_version()
    _change_param_name_to_name(op_info.inputs)
    _change_param_name_to_name(op_info.origin_inputs)
    compile_info = context.get_compile_info()

    pre_run_info = compile_info.get("run_info_op_tiling") if compile_info else None
    if pre_run_info is None:
        try:
            import tbe.common.context.op_context as _tbe_ctx

            _tbe_context = _tbe_ctx.get_context()
            if _tbe_context is not None:
                pre_run_info = _tbe_context.get_compile_info("run_info_op_tiling")
        except Exception:
            pre_run_info = None
    if pre_run_info is not None:
        pre_run_info_dict = (
            pre_run_info if isinstance(pre_run_info, dict) else json.loads(pre_run_info)
        )
        tiling_data = pre_run_info_dict.get("tiling_data", [])
        if not isinstance(tiling_data, (bytes, bytearray)):
            tiling_data = bytes(int(x) & 0xFF for x in tiling_data)
        pre_run_info_dict["tiling_data"] = tiling_data
        pre_run_info_dict.setdefault("clear_atomic", False)
        pre_run_info_dict.setdefault("workspaces", [])
        return pre_run_info_dict

    tiling_config = {
        "name": "ascendc_op_para_size",
        "dtype": "int",
        "value": 2 * 1024 * 1024,
    }
    op_info.attrs.append(tiling_config)
    return do_op_tiling(
        op_info.op_type,
        compile_info,
        op_info.origin_inputs,
        op_info.origin_outputs,
        None,
        None,
        op_info.attrs,
    )


def generate_static_tiling_struct_file(
    optype,
    run_info,
    tiling_info,
    tiling_key_list,
    tiling_key_group_map,
    tiling_const_propagation: bool = False,
):
    # bytes.fromhex(run_info["tiling_data"]) can deserialization DumpBytesBuffer
    # save undecoded tiling data for replay
    tiling_info.tiling_data = run_info["tiling_data"]
    tiling_info.tiling_key = run_info["tiling_key"]
    if "local_memory_size" in run_info:
        tiling_info.local_memory_size = run_info["local_memory_size"]

    tiling_key_master, has_tiling_key_group = get_master_tiling_key_from_group(
        str(tiling_info.tiling_key), tiling_key_group_map
    )
    if has_tiling_key_group:
        optype_with_tilingkey = optype + "_" + tiling_key_master
        tiling_def = get_tiling_def(optype_with_tilingkey)
        tiling_def = _get_tiling_def_with_group(
            tiling_def, optype, tiling_key_master, tiling_key_group_map
        )
    else:
        optype_with_tilingkey = optype + "_" + str(tiling_info.tiling_key)
        tiling_def = get_tiling_def(optype_with_tilingkey)

    # judge the tilingkey of this shape now use if the tiling struct of optype itslef
    # or the special registered tilingkey struct
    is_optype_self = False
    if tiling_def is None:
        is_optype_self = True
        tiling_def = get_tiling_def(optype)
    if tiling_def is None:
        LogUtil.print_compile_log(
            optype, "do not registe tiling struct!!!", AscendCLogLevel.LOG_ERROR
        )
        return
    struct_tiling_def_base: dict = {}
    struct_tiling_def_base = get_struct_tiling_info(tiling_def, struct_tiling_def_base)
    tiling_info.block_num = run_info["block_dim"]
    tiling_info.clear_atomic = run_info["clear_atomic"]
    tiling_info.schedule_mode = run_info.get("schedule_mode", 0)
    # all tiling struct info by dynamic, except the only one top-level struct of static-shape one itself
    all_dynamic_struct_def_except_self = gen_all_dynamic_struct_def_except_self(
        is_optype_self,
        str(tiling_info.tiling_key),
        tiling_key_list,
        optype,
        tiling_key_group_map,
    )
    tiling_info.file_content = gen_static_shape(
        tiling_def,
        run_info["tiling_data"],
        struct_tiling_def_base,
        all_dynamic_struct_def_except_self,
        tiling_const_propagation,
    )
    tiling_info.tiling_data_size = tiling_def.data_size
    total_workspace_size = sum(run_info["workspaces"])
    tiling_info.static_workspace_size = total_workspace_size
    run_info["tiling_data"] = tiling_info.tiling_data.hex()
    tiling_info.raw_run_info = run_info
    tiling_info.tiling_info_completed = True
    return


def static_post_process_of_workspace(context, run_info, workspace_size):
    if workspace_size >= 0:
        if len(run_info["workspaces"]) > 1:
            msg = "the num of workspace can not large than 1"
            raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, msg)
        context.add_workspace("total_workspace", size=workspace_size)


def process_tiling_info(
    op_info: OpInfo,
    tiling_key_list: list = None,
    value_depends: dict = None,
    enable_vd=False,
    tiling_key_group_map: dict = None,
):
    """get tiling define and tiling data registered by operator developer

    Args:
        op_info(tuple): operator infos
        tiling_key_list(list): tiling_key_list of op
        value_depends(dict): value_depends info of op
        enable_vd(bool): is support value depends
    Returns:
        tiling_info (TilingInfo): tiling info formatted by tiling define and tiling data
    """
    optype = op_info.op_type
    outputs = op_info.outputs
    tiling_info = TilingInfo()
    tiling_info.tiling_key_list = tiling_key_list
    static_shape = is_static_shape(
        op_info.origin_inputs,
        outputs,
        value_depends,
        op_info.param_type_list,
        enable_vd,
    )
    context = get_context()
    if static_shape:
        run_info = get_static_run_info(op_info, context)
        tiling_const_propagation = global_var_storage.get_variable(
            "ascendc_tiling_const_propagation"
        )
        generate_static_tiling_struct_file(
            optype,
            run_info,
            tiling_info,
            tiling_key_list,
            tiling_key_group_map,
            tiling_const_propagation,
        )
        static_post_process_of_workspace(
            context, run_info, tiling_info.static_workspace_size
        )
    else:
        tiling_info.static_shape_flag = False
        generate_dynamic_tiling_struct_file(
            optype, tiling_info, tiling_key_list, tiling_key_group_map
        )
        context.add_workspace("total_workspace", size=-1)
    return tiling_info


def get_master_tiling_key_from_group(
    tiling_key: str, tiling_key_group_map: dict = None
):
    if tiling_key_group_map is None:
        return "", False
    if tiling_key in tiling_key_group_map.keys():
        return tiling_key, True
    for tiling_key_master, tiling_key_slave_list in tiling_key_group_map.items():
        for tiling_key_slave in tiling_key_slave_list:
            if tiling_key_slave == tiling_key:
                return tiling_key_master, True
    return "", False


def _get_tiling_def_with_group(
    tiling_def: TilingDef,
    optype: str,
    tiling_key_master: str,
    tiling_key_group_map: dict,
):
    """Get tiling definition considering tiling key groups."""
    for tiling_key_slave in tiling_key_group_map[tiling_key_master]:
        optype_with_tilingkey_slave = optype + "_" + tiling_key_slave
        tiling_def_slave = get_tiling_def(optype_with_tilingkey_slave)
        if tiling_def is None:
            tiling_def = tiling_def_slave
        elif tiling_def_slave is None:
            continue
        else:
            if tiling_def.class_name != tiling_def_slave.class_name:
                raise Exception(
                    f"tiling struct in tiling_key_group is different, the tiling \
                                struct of tiling_key: {tiling_key_master} is {tiling_def.class_name}, \
                                but the tiling struct of tiling_key: {tiling_key_slave} is \
                                {tiling_def_slave.class_name}"
                )

    return tiling_def


def get_tiling_declaration(optype: str):
    load_lib()
    tiling_info = TilingInfo()
    tiling_def = get_tiling_def(optype)
    if tiling_def is None:
        return tiling_info
    struct_tiling_def_base: dict = {}
    struct_tiling_def_base = get_struct_tiling_info(tiling_def, struct_tiling_def_base)
    tiling_info.file_content = gen_dynamic_shape(tiling_def, struct_tiling_def_base)
    tiling_info.tiling_info_completed = True
    return tiling_info


def _add_time_stamp_codes(desc_id: str, space_len: int = 1):
    source = "    " * space_len
    return (
        source
        + f"AscendC::PrintTimeStamp(static_cast<uint32_t>(AscendC::TimeStampId::{desc_id}));\n\n"
    )


def get_tiling_info_by_tiling(
    op_info: OpInfo, infered_info_from_ifile, value_depends: dict, origin_func_name
):
    CommonUtility.print_compile_log(
        op_info.kernel_name, "get tiling info...", AscendCLogLevel.LOG_INFO
    )
    # temp enable avoid
    enable_vd = CommonUtility.is_c310()
    if (
        infered_info_from_ifile.default_tiling_struct != ""
        or global_var_storage.get_variable("ascendc_tiling_no_register")
    ):
        return get_tiling_info_v2(
            op_info,
            infered_info_from_ifile.tiling_key_list,
            infered_info_from_ifile.default_tiling_struct,
            infered_info_from_ifile.tiling_key_struct_map,
            value_depends,
            enable_vd,
        )
    else:
        input_tiling_info_dict = {
            "tiling_key_list": infered_info_from_ifile.tiling_key_list,
            "value_depends": value_depends,
            "enable_vd": enable_vd,
            "tiling_key_group_map": infered_info_from_ifile.tiling_key_group_map,
            "origin_func_name": origin_func_name,
        }
        return get_tiling_info_isolate(op_info, input_tiling_info_dict)
