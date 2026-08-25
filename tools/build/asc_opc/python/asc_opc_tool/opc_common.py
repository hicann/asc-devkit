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
opc common
"""

import inspect
import os
import stat
import re
import sys
import traceback
import hashlib
import json
import copy

from enum import Enum
from enum import unique

from asc_op_compile_base.common.utils import log as logger

from constant import (
    CompileParam,
    OpcOptions,
    OpImplType,
    OpcCompileMode,
    GraphDefParam,
    OpDataType,
    OpFormatType,
    OpParamType,
)

compiling_option_keys = [
    OpcOptions.OUTPUT,
    OpcOptions.SOC_VERSION,
    OpcOptions.INPUT_PARAM,
    OpcOptions.MAIN_FUNC,
    OpcOptions.OP_PATH,
    OpcOptions.OP_DEBUG_LEVEL,
    OpcOptions.OP_DEBUG_CONFIG,
    OpcOptions.IMPL_MODE,
    OpcOptions.DETERMINISTIC,
    OpcOptions.DEBUG_DIR,
    OpcOptions.CORE_TYPE,
    OpcOptions.GRAPH,
    OpcOptions.BIN_FILENAME,
    OpcOptions.AICORE_NUM,
    OpcOptions.LOG,
    OpcOptions.OPTIONAL_INPUT_MODE,
    OpcOptions.OPTIONAL_OUTPUT_MODE,
    OpcOptions.OP_MODE,
    OpcOptions.SIMPLE_KEY_MODE,
    OpcOptions.DYNAMIC_PARAM_MODE,
    OpcOptions.TILING_KEY,
    OpcOptions.RELOCATABLE_BIN,
    OpcOptions.SPK_OPT,
    OpcOptions.KERNEL_TEMPLATE_INPUT,
    OpcOptions.KERNEL_SPEC,
    OpcOptions.KERNEL_SPEC_DIR,
]

valid_format_match_mode = ["FormatAgnostic", "FormatDefault", "FormatFixed", None]
valid_format_mode = ["normal", "nd_agnostic", "static_nd_agnostic", None]
valid_dtype_match_mode = ["DtypeByte", None]

# int32 max value
INT32_MAX = 2147483647
INVALID_MULTIPLIER = (-2, -1, 0, 1)
DYNAMIC_DIM_VALUE = (-2, -1)


@unique
class LogLevel(Enum):
    """
    log level enum
    """

    DEBUG = 0
    INFO = 1
    WARNING = 2
    ERROR = 3


DATA_TYPE_SIZE = {
    "float32": 4,
    "float16": 2,
    "int8": 1,
    "int16": 2,
    "int32": 4,
    "int64": 8,
    "uint8": 1,
    "uint16": 2,
    "uint32": 4,
    "uint64": 8,
    "bool": 1,
    "double": 8,
    "dual": 25,
    "dual_sub_int8": 1,
    "dual_sub_uint8": 1,
    "string": -1,
    "complex32": 4,
    "complex64": 8,
    "complex128": 16,
    "qint8": 1,
    "qint16": 2,
    "qint32": 4,
    "quint8": 1,
    "quint16": 2,
    "resource": 8,
    "string_ref": -1,
    "int4": 1004,
    "uint1": 1001,
    "int2": 1002,
    "uint2": 1002,
    "bfloat16": 2,
    "hifloat8": 1,
    "float8_e5m2": 1,
    "float8_e4m3fn": 1,
    "float8_e8m0": 1,
    "float6_e3m2": 1006,
    "float6_e2m3": 1006,
    "float4_e2m1": 1004,
    "float4_e1m2": 1004,
}


def shapes_multiplying_over_int32(shape, shape_sum) -> bool:
    """
    multiplying every shape if it is valid
    """
    for dim in shape:
        if dim in INVALID_MULTIPLIER:
            continue
        shape_sum[0] *= dim
        if shape_sum[0] > INT32_MAX:
            return True
    return False


def get_int64_mode(inputs_or_outputs) -> bool:
    """
    get int64_mode by the product of shapes
    """
    for in_or_out in inputs_or_outputs:
        if in_or_out is None:
            continue
        if isinstance(in_or_out, list):
            if len(in_or_out) == 0:
                continue
            shape = in_or_out[0].get(CompileParam.SHAPE)
            data_size = DATA_TYPE_SIZE.get(in_or_out[0].get(CompileParam.DTYPE))
        else:
            shape = in_or_out.get(CompileParam.SHAPE)
            data_size = DATA_TYPE_SIZE.get(in_or_out.get(CompileParam.DTYPE))

        if shape is None:
            continue
        shape_sum = [1]
        if shapes_multiplying_over_int32(shape, shape_sum):
            return True

        if data_size is None:
            logger.warn(
                "Data type[%s] is not support.", in_or_out.get(CompileParam.DTYPE, "")
            )
            return True

        if shape_sum[0] * data_size > INT32_MAX:
            return True
    return False


def check_is_dynamic(inputs_or_outputs) -> bool:
    """
    check whether this op's shape is dynamic
    """

    def _is_dynamic(op_info):
        shape = op_info.get(CompileParam.SHAPE)
        if shape is None:
            logger.warn("There is no shape info.")
            return False

        for dim in shape:
            if dim in DYNAMIC_DIM_VALUE:
                return True
        return False

    for in_or_out in inputs_or_outputs:
        if in_or_out is None:
            continue
        if isinstance(in_or_out, list):
            for item in in_or_out:
                if _is_dynamic(item):
                    return True
        else:
            if _is_dynamic(in_or_out):
                return True
    return False


def modify_except_msg(evalue):
    """
    modify except msg 'No space left on device' to 'No space left on disk'
    """
    if (
        evalue
        and hasattr(evalue, "strerror")
        and evalue.strerror == "No space left on device"
    ):
        evalue.strerror = "No space left on disk"


def get_except_msg():
    """
    get exception msg
    """
    etype, evalue, tback = sys.exc_info()

    modify_except_msg(evalue)
    exc_list = traceback.format_exception(etype, evalue, tback)

    first_flag = True
    new_exc_list = []
    for exc_line in exc_list:
        if first_flag:
            msg_str = exc_line
            first_flag = False
            continue
        if len(msg_str) + len(exc_line) > 800:
            new_exc_list.append(msg_str)
            msg_str = "".join(exc_line)
        else:
            msg_str += exc_line
    new_exc_list.append(msg_str)

    return new_exc_list


def record_log_list(log_list, log_level):
    """
    log has spilted to list, need to record each
    """

    def _log_debug(log_line):
        logger.debug("%s", log_line)

    def _log_info(log_line):
        logger.info("%s", log_line)

    def _log_warn(log_line):
        logger.warn("%s", log_line)

    def _log_error(log_line):
        logger.error("%s", log_line)

    log_api_map = {
        LogLevel.DEBUG: _log_debug,
        LogLevel.INFO: _log_info,
        LogLevel.WARNING: _log_warn,
        LogLevel.ERROR: _log_error,
    }

    log_fun = log_api_map.get(log_level)
    if log_fun:
        for log_line in log_list:
            log_fun(log_line)
    else:
        logger.error("log_level is invalid.")


def get_file_real_path(path_prefix, file_name, file_postfix, middle_path=None):
    """
    get file real path by path prefix, file name, file postfix
    """
    if middle_path:
        return "{}/{}/{}.{}".format(path_prefix, middle_path, file_name, file_postfix)
    return "{}/{}.{}".format(path_prefix, file_name, file_postfix)


def get_json_file_path(args):
    kernel_name = args.get(OpcOptions.KERNEL_NAME)
    debug_dir = args.get(OpcOptions.KERNEL_META_PATH)
    # .json file path
    return get_file_real_path(debug_dir, kernel_name, "json", "kernel_meta")


def normalize_func_name(func_name):
    name_tmp = []
    sub_head = False
    for index, func in enumerate(func_name):
        if func.islower():
            sub_head = False
        if func.isdigit():
            sub_head = True
        if func.isupper() and index != 0:
            if not sub_head:
                name_tmp.append("_")
                sub_head = True
            else:
                index_next = index + 1
                if index_next < len(func_name):
                    if func_name[index_next].islower():
                        name_tmp.append("_")
        name_tmp.append(func)
    name_tmp_str = "".join(name_tmp)
    func_name = name_tmp_str.lower()
    return func_name


def str_to_sha256_hash(str_to_hash):
    """
    hash string
    """
    sha256_hash = hashlib.sha256()
    sha256_hash.update(str_to_hash.encode("utf-8"))
    return sha256_hash.hexdigest()


def check_and_normalize_impl_mode(impl_mode):
    impl_mode = "".join(impl_mode.split())
    if impl_mode not in (
        OpImplType.HIGH_PERFORMANCE,
        OpImplType.HIGH_PRECISION,
        OpImplType.HIGH_PERFORMANCE_OPT,
        OpImplType.HIGH_PRECISION_OPT,
        OpImplType.DEFAULT,
    ):
        logger.info("impl_mode is %s.", str(impl_mode))
        return False, impl_mode
    return True, impl_mode


def normalize_optional_impl_mode(impl_mode_cfg):
    impl_mode_list = impl_mode_cfg.split(",", -1)
    impl_mode_strip_list = [
        impl_mode.strip()
        for impl_mode in impl_mode_list
        if OpImplType.OPTIONAL not in impl_mode
    ]
    impl_mode_str = ",".join(impl_mode_strip_list)
    logger.debug("impl_mode_str[%s].", impl_mode_str)
    return impl_mode_str


def read_json_file(json_path):
    try:
        with open(json_path, "r") as json_file:
            json_data = json.load(json_file)
    except Exception as err:
        logger.info("Read json file[%s] failed. (%s))", json_path, str(err))
        return None

    return json_data


def update_json_file(key, value, json_path):
    """
    write info to json file
    """
    try:
        # read json file
        with open(json_path, "r") as file_in:
            # Only the owner and group have rights
            os.chmod(
                json_path, stat.S_IWGRP + stat.S_IWUSR + stat.S_IRGRP + stat.S_IRUSR
            )
            json_info = json.load(file_in)

        # update value
        json_info.update({key: value})

        # write json file
        with open(json_path, "w") as file_out:
            json.dump(json_info, file_out, indent=4)
    except IOError:
        logger.error("Open json file[%s] failed.", json_path)
    finally:
        pass


def generate_attrs_value_list(attrs_dict, attrs_value_list):
    for _, value in attrs_dict.items():
        if CompileParam.VALUE_RANGE in value:
            attr_dict = {}
            attr_dict[CompileParam.VALUE_RANGE] = value.get(CompileParam.VALUE_RANGE)
            attr_dict[CompileParam.RANGE_MODE] = value.get(
                CompileParam.RANGE_MODE, None
            )
            attrs_value_list.append(attr_dict)
        elif CompileParam.VALUE_LIST in value:
            attr_dict = {}
            attr_dict[CompileParam.VALUE_LIST] = value.get(CompileParam.VALUE_LIST)
            attrs_value_list.append(attr_dict)
        else:
            attrs_value_list.append(value.get(CompileParam.VALUE))


def record_attrs_value_list(
    value_range, value_list, range_mode, value, attrs_value_list
):
    new_attr_info = dict()
    if value_range:
        new_attr_info[CompileParam.VALUE_RANGE] = value_range
        new_attr_info[CompileParam.RANGE_MODE] = range_mode
        attrs_value_list.append(new_attr_info)
    elif value_list:
        new_attr_info[CompileParam.VALUE_LIST] = value_list
        attrs_value_list.append(new_attr_info)
    else:
        attrs_value_list.append(value)


def get_new_attrs_for_op_compile(op_node, op_func, op_compile_mode):
    """
    tbe only need attr value as list
    """
    attrs = op_node.get(CompileParam.ATTRS)
    if attrs is None:
        return []

    attrs_dict = {}
    for attr in attrs:
        if attr is None:
            continue
        if isinstance(attr, dict):
            attrs_dict.update({attr.get(CompileParam.NAME): attr})
        else:
            raise RuntimeError(
                "type of attr should be dict, but it is [{}].".format(str(type(attr)))
            )

    attrs_value_list = []
    if (
        op_compile_mode == OpcCompileMode.SINGLE_OP_CONFIG_FILE_MODE
        or op_compile_mode == OpcCompileMode.SINGLE_OP_DICT_MODE
    ):
        generate_attrs_value_list(attrs_dict, attrs_value_list)
        return attrs_value_list

    params = inspect.signature(op_func).parameters

    # record fusion op real attrs
    op_attrs = record_real_attrs(params, attrs_dict, attrs_value_list)

    if len(op_attrs) != 0:
        logger.debug("OP_ATTRS is %s.", str(op_attrs))
        op_node[CompileParam.OP_ATTRS] = op_attrs
    else:
        logger.debug("Has no OP_ATTRS.")

    return attrs_value_list


def record_real_attrs(params, attrs_dict, attrs_value_list):
    op_attrs = []  # record fusion op real attrs
    var_attrs = attrs_dict.get(GraphDefParam.VAR_ATTRS, {}).get(CompileParam.VALUE, [])
    for name, value in params.items():
        if str(name) == OpcOptions.KERNEL_NAME:
            logger.debug(
                "{} not belong to attrs, should not be added to new_attrs.".format(
                    str(name)
                )
            )
            break

        attr_info = attrs_dict.get(name, None)
        if var_attrs is not None and name in var_attrs:
            if attr_info is None:
                attrs_value_list.append(None)
                op_attrs.append(attr_info)
                continue

            # in var_attrs but not value_range/value_list
            if (
                attr_info.get(CompileParam.VALUE_RANGE) is None
                and attr_info.get(CompileParam.VALUE_LIST) is None
            ):
                attrs_value_list.append(None)
                attr_info[CompileParam.VALUE] = None
                op_attrs.append(attr_info)
                continue

        if attr_info is not None:
            record_attrs_value_list(
                attr_info.get(CompileParam.VALUE_RANGE),
                attr_info.get(CompileParam.VALUE_LIST),
                attr_info.get(CompileParam.RANGE_MODE),
                attr_info.get(CompileParam.VALUE, None),
                attrs_value_list,
            )
            op_attrs.append(attr_info)
            continue

        if value.default is not inspect.Parameter.empty:
            attrs_value_list.append(value.default)
            op_attrs.append(attr_info)

    return op_attrs


def update_compile_info(json_file_path, compile_info):
    """
    update compile info into json file if necessary
    """
    opc_log_full(
        LogLevel.DEBUG,
        "json_file_path = %s, compile_info = %s.",
        json_file_path,
        compile_info,
    )
    if not json_file_path:
        return

    if not compile_info:
        return

    json_content = {}

    try:
        with open(json_file_path) as fr:
            json_content = json.load(fr)

        # update compile info if necessary
        if "compileInfo" not in json_content:
            logger.warn(
                "[update_compile_info]: update compile info into json file path: %s.",
                json_file_path,
            )
            json_content["compileInfo"] = compile_info

            with open(json_file_path, "w") as fw:
                json.dump(
                    json_content, fw, sort_keys=True, indent=4, separators=(",", ":")
                )
    except Exception as e:
        logger.error("update_compile_info failed with error message: %s.", str(e))


def find_special_char_position(log_str):
    str_reverse = log_str[::-1]
    final_index = len(log_str) - 1
    match1 = re.search(r"\n", str_reverse)
    match2 = re.search(r"\W", str_reverse)
    if match1:
        final_index = len(str_reverse) - match1.start()
    elif match2:
        final_index = len(str_reverse) - 1 - match2.start()

    return final_index


def get_log_str_list(log_str):
    slice_len = 800
    log_str_list = []
    while len(log_str) / slice_len >= 1:
        final_index = find_special_char_position(log_str[0:slice_len])
        if final_index == 0:
            final_index = slice_len
        log_str_list.append(log_str[0:final_index])
        log_str = log_str[final_index:]

    log_str_list.append(log_str)
    return log_str_list


def opc_log_full(log_level, log_msg, *log_paras):
    """
    print full log
    """
    line_no = inspect.currentframe().f_back.f_lineno
    funcname = inspect.currentframe().f_back.f_code.co_name
    co_filename = inspect.currentframe().f_back.f_code.co_filename
    file_path = os.path.dirname(os.path.realpath(__file__))
    filename = os.path.relpath(co_filename, file_path)
    log_header = "[%s:%d][%s] " % (filename, line_no, funcname)
    log_str = log_header + log_msg % log_paras
    log_str_list = get_log_str_list(log_str)
    for log in log_str_list:
        if log_level == LogLevel.DEBUG:
            logger.debug("%s", log)
        elif log_level == LogLevel.INFO:
            logger.info("%s", log)
        elif log_level == LogLevel.WARNING:
            logger.warn("%s", log)
        elif log_level == LogLevel.ERROR:
            logger.error("%s", log)


def attrs_from_string_to_str(attrs):
    """
    attrs string change to str. for optiling adapter
    """
    if not attrs:
        return attrs
    attrs_res = copy.deepcopy(attrs)
    for attr in attrs_res:
        if attr is None:
            continue
        if "dtype" in attr and attr["dtype"] == "string":
            attr["dtype"] = "str"
    return attrs_res


def check_and_generate_single_attr_for_simpilified_key(dtype, value, attr_str):
    if (dtype != "bool") and (dtype != "string"):
        if value is not None:
            logger.warn(
                "dtype[%s] has non-null value, and simplified key won't be generated",
                dtype,
            )
            return True, attr_str
        else:
            attr_str += ","

    elif dtype == "bool":
        if value is True:
            attr_str += "1,"
        elif value is False:
            attr_str += "0,"
        elif value is None:
            logger.info("bool's value is null")
            attr_str += ","
        else:
            logger.warn("invalid bool value, will not generate simplified key")
            return True, attr_str

    else:  # dtype == string
        if value is not None:
            attr_str = attr_str + value + ","
        else:
            attr_str = attr_str + ","

    return False, attr_str


def check_attr_for_simpilified_key(attrs):
    """
    generate input or output dtype, format for simplified key
    """
    attr_str = str()
    has_invalid_option = False
    has_non_null_value = False

    if attrs is None:
        logger.info("attr is none")
        return has_invalid_option, has_non_null_value, attr_str

    # simplified key mode == 1 or undefined
    for attr in attrs:
        if attr is None:
            logger.warn("attr is none, continue")
            continue

        valid_keys = ["name", "dtype", "value"]
        for key in attr.keys():
            if key not in valid_keys:
                logger.warn(
                    "invalid key: %s, and simplified key won't be generated", key
                )
                has_invalid_option = True
                break

        value = attr.get("value")
        dtype = attr.get("dtype")
        logger.info("attr's dtype: %s, value: %s", dtype, value)
        if value is not None:
            has_non_null_value = True

        has_invalid_option, attr_str = (
            check_and_generate_single_attr_for_simpilified_key(dtype, value, attr_str)
        )
        if has_invalid_option:
            break

    if has_invalid_option:
        attr_str = str()
    else:
        attr_str = "/" + attr_str
        attr_str = attr_str[:-1]

    logger.debug("attr_str: %s", attr_str)
    return has_invalid_option, has_non_null_value, attr_str


def check_single_input_or_output_for_simplified_key(
    input_or_output, has_invalid_option
):
    if check_single_input_or_output_dtype_for_simplified_key(
        input_or_output
    ) or check_single_input_or_output_format_for_simplified_key(input_or_output):
        return True

    return has_invalid_option


def check_single_input_or_output_dtype_for_simplified_key(input_or_output):
    dtype = input_or_output.get(CompileParam.DTYPE)
    if dtype not in OpDataType.DtypeValueDict.keys():
        logger.warn("invalid dtype: %s, will not generate simplified key.", dtype)
        return True

    dtype_match_mode = input_or_output.get(CompileParam.DTYPE_MATCH_MODE)
    if dtype_match_mode not in valid_dtype_match_mode:
        logger.warn(
            "invalid dtype_match_mode: %s, will not generate siplified key.",
            dtype_match_mode,
        )
        return True

    dtype_for_bin_query = input_or_output.get(CompileParam.DTYPE_FOR_BIN_QUERY)
    if dtype_for_bin_query is not None:
        for dtype in dtype_for_bin_query:
            if dtype not in OpDataType.DtypeValueDict.keys():
                logger.warn(
                    "invalid dtype: %s in dtypeForBinQuery, will not generate simplified key.",
                    dtype,
                )
                return True
    return False


def check_single_input_or_output_format_for_simplified_key(input_or_output):
    op_format = input_or_output.get(CompileParam.FORMAT)
    if op_format not in OpFormatType.FormatValueDict.keys():
        logger.warn("invalid fromat: %s, will not generate simplified key.", op_format)
        return True

    format_match_mode = input_or_output.get(CompileParam.FORMAT_MATCH_MODE)
    if format_match_mode not in valid_format_match_mode:
        logger.warn(
            "invalid format_match_mode: %s, will not generate siplified key.",
            format_match_mode,
        )
        return True

    format_mode = input_or_output.get(CompileParam.FORMAT_MODE)
    if format_mode not in valid_format_mode:
        logger.warn(
            "invalid formatMode: %s, will not generate siplified key.", format_mode
        )
        return True

    format_for_bin_query = input_or_output.get(CompileParam.FORMAT_FOR_BIN_QUERY)
    if format_for_bin_query is not None:
        for format in format_for_bin_query:
            if format not in OpFormatType.FormatValueDict.keys():
                logger.warn(
                    "invalid format: %s in formatForBinQuery, will not generate simplified key.",
                    format,
                )
                return True

    return False


def check_input_or_output_for_simplified_key(inputs_or_outputs, is_input):
    has_invalid_option = False
    has_dynamic_param = False
    has_optional_param = False
    valid_keys = [
        "name",
        "index",
        "dtype",
        "format",
        "paramType",
        "shape",
        "format_match_mode",
        "dtype_match_mode",
        "formatMode",
        "dtypeForBinQuery",
        "formatForBinQuery",
    ]

    for input_or_output in inputs_or_outputs:
        # for dynamic input/output, only get the first one
        if isinstance(input_or_output, list):
            if len(input_or_output) == 0:
                has_invalid_option = True
                logger.warn(
                    "dynamic param count is 0, will not generate simplified key."
                )
                break
            has_dynamic_param = True
            input_or_output = input_or_output[0]

        if input_or_output is None:
            logger.warn("null input or output")
            continue

        for key in input_or_output.keys():
            if key not in valid_keys:
                logger.warn("invalid key: %s, will not generate simplified key.", key)
                has_invalid_option = True
                break

        shape = input_or_output.get(CompileParam.SHAPE)
        shape_valid_flag = (shape is not None) and isinstance(shape, list)
        if shape_valid_flag:
            flag = (len(shape) != 1) or (shape[0] != -2)
            if flag:
                has_invalid_option = True
                logger.warn(
                    "invalid shape: %s, will not generate simplified key.", str(shape)
                )
        else:
            has_invalid_option = True
            break

        has_invalid_option = check_single_input_or_output_for_simplified_key(
            input_or_output, has_invalid_option
        )
        if has_invalid_option:
            break
        param_type = input_or_output.get(CompileParam.PARAM_TYPE)
        has_optional_param = True if param_type == OpParamType.OPT else False

    if is_input:
        logger.info(
            "check input: has_invalid_option %s, has_dynamic_param %s, has_optional_input %s",
            str(has_invalid_option),
            str(has_dynamic_param),
            str(has_optional_param),
        )
    else:
        logger.info(
            "check output: has_invalid_option %s, has_dynamic_param %s, has_optional_output %s",
            str(has_invalid_option),
            str(has_dynamic_param),
            str(has_optional_param),
        )

    return has_invalid_option, has_dynamic_param, has_optional_param


def check_input_and_output_for_simplified_key(op):
    has_invalid_option, has_dynamic_param_input, has_optional_input = (
        check_input_or_output_for_simplified_key(op.get(CompileParam.INPUTS), True)
    )

    has_invalid_option, has_dynamic_param_output, has_optional_output = (
        check_input_or_output_for_simplified_key(op.get(CompileParam.OUTPUTS), False)
    )

    has_dynamic_param = has_dynamic_param_input or has_dynamic_param_output
    res_tuple = (
        has_invalid_option,
        has_dynamic_param,
        has_optional_input,
        has_optional_output,
    )
    return res_tuple
