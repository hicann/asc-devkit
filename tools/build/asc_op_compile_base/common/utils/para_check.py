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
common function for check ops parameter
"""

import math
import re
import warnings
from enum import Enum
from functools import reduce as _reduce
from functools import wraps

from asc_op_compile_base.common.error_mgr import error_manager_util as error_manager

SHAPE_SIZE_LIMIT = 2**31 - 1
SHAPE_SIZE_LIMIT_INT64 = 2**63 - 1
DIM_LIMIT_INT64 = SHAPE_SIZE_LIMIT_INT64
MIN_UNKNOWN_SHAPE_RANK = 0
MAX_UNKNOWN_SHAPE_NUM_INT64 = 2**63 - 1
DYNAMIC_SHAPE_FLAG = -1

RANK_LIMIT = 8

# the max len of kernel_name
MAX_KERNEL_NAME_LEN = 200
KERNEL_NAME = "kernel_name"


REQUIRED_INPUT = "required_input"
OPTION_INPUT = "option_input"
DYNAMIC_INPUT = "dynamic_input"

REQUIRED_OUTPUT = "required_output"
OPTION_OUTPUT = "option_output"
DYNAMIC_OUTPUT = "dynamic_output"

# in proto attr can be a Tensor/BYTES/LIST_TYPE Type, but not te fusion don't support this type
REQUIRED_ATTR_INT = "REQUIRED_ATTR_INT"
REQUIRED_ATTR_FLOAT = "REQUIRED_ATTR_FLOAT"
REQUIRED_ATTR_STR = "REQUIRED_ATTR_STR"
REQUIRED_ATTR_BOOL = "REQUIRED_ATTR_BOOL"
REQUIRED_ATTR_TYPE = "REQUIRED_ATTR_TYPE"
REQUIRED_ATTR_LIST_INT = "REQUIRED_ATTR_LIST_INT"
REQUIRED_ATTR_LIST_FLOAT = "REQUIRED_ATTR_LIST_FLOAT"
REQUIRED_ATTR_LIST_BOOL = "REQUIRED_ATTR_LIST_BOOL"
REQUIRED_ATTR_LIST_LIST_INT = "REQUIRED_ATTR_LIST_LIST_INT"

OPTION_ATTR_INT = "OPTION_ATTR_INT"
OPTION_ATTR_FLOAT = "OPTION_ATTR_FLOAT"
OPTION_ATTR_STR = "OPTION_ATTR_STR"
OPTION_ATTR_BOOL = "OPTION_ATTR_BOOL"
OPTION_ATTR_TYPE = "OPTION_ATTR_TYPE"
OPTION_ATTR_LIST_INT = "OPTION_ATTR_LIST_INT"
OPTION_ATTR_LIST_FLOAT = "OPTION_ATTR_LIST_FLOAT"
OPTION_ATTR_LIST_BOOL = "OPTION_ATTR_LIST_BOOL"
OPTION_ATTR_LIST_LIST_INT = "OPTION_ATTR_LIST_LIST_INT"

OP_ERROR_CODE_000 = "E80000"
OP_ERROR_CODE_001 = "E80001"
OP_ERROR_CODE_002 = "E80002"
OP_ERROR_CODE_003 = "E80003"
OP_ERROR_CODE_004 = "E80004"
OP_ERROR_CODE_005 = "E80005"
OP_ERROR_CODE_006 = "E80006"
OP_ERROR_CODE_007 = "E80007"
OP_ERROR_CODE_008 = "E80008"
OP_ERROR_CODE_009 = "E80009"
OP_ERROR_CODE_010 = "E80010"
OP_ERROR_CODE_011 = "E80011"
OP_ERROR_CODE_012 = "E80012"
OP_ERROR_CODE_013 = "E80013"
OP_ERROR_CODE_014 = "E80014"
OP_ERROR_CODE_015 = "E80015"
OP_ERROR_CODE_016 = "E80016"
OP_ERROR_CODE_017 = "E80017"
OP_ERROR_CODE_018 = "E80018"
OP_ERROR_CODE_019 = "E80019"
OP_ERROR_CODE_020 = "E80020"
OP_ERROR_CODE_021 = "E80021"
OP_ERROR_CODE_022 = "E80022"
OP_ERROR_CODE_023 = "E80023"
OP_ERROR_CODE_024 = "E80024"
OP_ERROR_CODE_025 = "E80025"
OP_ERROR_CODE_026 = "E80026"
OP_ERROR_CODE_027 = "E80027"


# OpParamInfoKey && TensorFormat :Internal Use Only
class OpParamInfoKey(Enum):
    """
    Op Parameter Info enum
    """

    SHAPE = "shape"
    FORMAT = "format"
    ORI_SHAPE = "ori_shape"
    ORI_FORMAT = "ori_format"
    D_TYPE = "dtype"
    RANGE = "range"


class TensorFormat(Enum):
    """
    Tensor Format enum
    """

    ND = "ND"
    NC = "NC"
    NCL = "NCL"
    NCHW = "NCHW"
    NHWC = "NHWC"
    NDHWC = "NDHWC"
    NCDHW = "NCDHW"
    CHWN = "CHWN"
    NC1HWC0 = "NC1HWC0"
    NC1HWC0_C04 = "NC1HWC0_C04"
    NDC1HWC0 = "NDC1HWC0"
    FRACTAL_NZ = "FRACTAL_NZ"
    C1HWC0 = "C1HWC0"
    HWCN = "HWCN"
    DHWCN = "DHWCN"
    FRACTAL_Z = "FRACTAL_Z"
    FRACTAL_Z_C04 = "FRACTAL_Z_C04"
    C1HWNCoC0 = "C1HWNCoC0"
    FRACTAL_Z_3D = "FRACTAL_Z_3D"
    FRACTAL_ZN_LSTM = "FRACTAL_ZN_LSTM"
    FRACTAL_ZN_RNN = "FRACTAL_ZN_RNN"
    ND_RNN_BIAS = "ND_RNN_BIAS"
    FRACTAL_NZ_C0_16 = "FRACTAL_NZ_C0_16"
    FRACTAL_NZ_C0_32 = "FRACTAL_NZ_C0_32"
    FRACTAL_NZ_C0_2 = "FRACTAL_NZ_C0_2"
    FRACTAL_NZ_C0_4 = "FRACTAL_NZ_C0_4"
    FRACTAL_NZ_C0_8 = "FRACTAL_NZ_C0_8"


ALL_FORMAT_LIST = [entry.value for entry in TensorFormat]
ALL_DTYPE_LIST = (
    "int4",
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "bfloat16",
    "int64",
    "uint64",
    "float16",
    "float32",
    "float64",
    "bool",
    "uint1",
    "double",
    "complex32",
    "complex64",
    "complex128",
    "hifloat8",
    "float8_e4m3fn",
    "float8_e5m2",
    "float8_e8m0",
    "float4_e2m1",
    "float4_e1m2",
    "int2",
    "hifloat4",
    "hifloat4_scale",
)
OP_NAME = ""
PARAM_NAME = ""


def check_op_params(*type_args, **type_kwargs):
    """
    check op params
    """
    from asc_op_compile_base.common.context import in_dynamic

    input_params = [REQUIRED_INPUT, OPTION_INPUT, DYNAMIC_INPUT]
    output_params = [REQUIRED_OUTPUT, OPTION_OUTPUT, DYNAMIC_OUTPUT]
    required_attr_params = [
        REQUIRED_ATTR_STR,
        REQUIRED_ATTR_FLOAT,
        REQUIRED_ATTR_INT,
        REQUIRED_ATTR_BOOL,
        REQUIRED_ATTR_TYPE,
        REQUIRED_ATTR_LIST_INT,
        REQUIRED_ATTR_LIST_BOOL,
        REQUIRED_ATTR_LIST_FLOAT,
        REQUIRED_ATTR_LIST_LIST_INT,
    ]
    list_type_attr = [
        REQUIRED_ATTR_LIST_BOOL,
        REQUIRED_ATTR_LIST_INT,
        REQUIRED_ATTR_LIST_FLOAT,
        REQUIRED_ATTR_LIST_LIST_INT,
        OPTION_ATTR_LIST_BOOL,
        OPTION_ATTR_LIST_INT,
        OPTION_ATTR_LIST_FLOAT,
        OPTION_ATTR_LIST_LIST_INT,
    ]

    def _check_input_output_key(op_param, param_name, op_name=OP_NAME):
        # check all necessary information
        # (shape, format, ori_shape, ori_format, dtype)
        if not isinstance(op_param, dict):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": op_name,
                "param_name": param_name,
                "param_type": "dict",
                "actual_type": op_param.__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s]'s type should be [%s], "
                "but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["param_type"],
                    error_info["actual_type"],
                ),
            )
        if -2 in op_param.get("shape", []):
            op_param["range"] = [(0, None)]
        error_info = {}

        if OpParamInfoKey.SHAPE.value not in op_param.keys():
            error_info["key"] = OpParamInfoKey.SHAPE.value
        elif OpParamInfoKey.FORMAT.value not in op_param.keys():
            error_info["key"] = OpParamInfoKey.FORMAT.value
        elif OpParamInfoKey.ORI_SHAPE.value not in op_param.keys():
            error_info["key"] = OpParamInfoKey.ORI_SHAPE.value
        elif OpParamInfoKey.ORI_FORMAT.value not in op_param.keys():
            error_info["key"] = OpParamInfoKey.ORI_FORMAT.value
        elif OpParamInfoKey.D_TYPE.value not in op_param.keys():
            error_info["key"] = OpParamInfoKey.D_TYPE.value
        elif in_dynamic() and OpParamInfoKey.RANGE.value not in op_param.keys():
            shape = op_param.get(OpParamInfoKey.ORI_SHAPE.value)
            if isinstance(shape, (tuple, list)) and DYNAMIC_SHAPE_FLAG in shape:
                error_info["key"] = OpParamInfoKey.RANGE.value

        if "key" in error_info.keys():
            error_info["errCode"] = OP_ERROR_CODE_004
            error_info["op_name"] = op_name
            error_info["param_name"] = param_name
            raise RuntimeError(
                error_info,
                "In op[%s], the input[%s] does not contain the item[%s]."
                % (error_info["op_name"], error_info["param_name"], error_info["key"]),
            )

    def _check_input_output_dict(op_param, param_name, op_name=OP_NAME):
        _check_input_output_key(op_param, param_name, op_name)
        check_shape(op_param[OpParamInfoKey.SHAPE.value], param_name=param_name)
        check_shape(op_param[OpParamInfoKey.ORI_SHAPE.value], param_name=param_name)
        if in_dynamic() and DYNAMIC_SHAPE_FLAG in op_param.get(OpParamInfoKey.ORI_SHAPE.value):
            _check_range(
                op_param[OpParamInfoKey.SHAPE.value], op_param[OpParamInfoKey.RANGE.value], param_name=param_name
            )

        if op_param[OpParamInfoKey.FORMAT.value] not in ALL_FORMAT_LIST:
            error_info = {
                "errCode": OP_ERROR_CODE_015,
                "op_name": op_name,
                "param_name": param_name,
                "excepted_format_list": ",".join(ALL_FORMAT_LIST),
                "format": op_param[OpParamInfoKey.FORMAT.value],
            }

            raise RuntimeError(
                error_info,
                "In op[%s], the format of input[%s] should be one of [%s],"
                " but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["excepted_format_list"],
                    error_info["format"],
                ),
            )

        if op_param[OpParamInfoKey.ORI_FORMAT.value] not in ALL_FORMAT_LIST:
            error_info = {
                "errCode": OP_ERROR_CODE_014,
                "op_name": op_name,
                "param_name": param_name,
                "excepted_format_list": ",".join(ALL_FORMAT_LIST),
                "format": op_param[OpParamInfoKey.ORI_FORMAT.value],
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the ori format of input[%s] should be one of [%s]"
                ", but actually is [%s]."
                % (
                    error_info.get("op_name"),
                    error_info["param_name"],
                    ",".join(ALL_FORMAT_LIST),
                    error_info["format"],
                ),
            )

        if not isinstance(op_param[OpParamInfoKey.D_TYPE.value], str):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": op_name,
                "param_name": param_name,
                "param_type": "str",
                "actual_type": op_param[OpParamInfoKey.D_TYPE.value].__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s]'s type should be [%s],  "
                "but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["param_type"],
                    error_info["actual_type"],
                ),
            )

        if (
            op_param[OpParamInfoKey.D_TYPE.value] is None
            or op_param[OpParamInfoKey.D_TYPE.value].lower() not in ALL_DTYPE_LIST
        ):
            error_info = {
                "errCode": OP_ERROR_CODE_008,
                "op_name": op_name,
                "param_name": param_name,
                "excepted_dtype_list": ",".join(ALL_DTYPE_LIST),
                "dtype": op_param[OpParamInfoKey.D_TYPE.value],
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s]'s dtype should be one of [%s], "
                "but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["excepted_dtype_list"],
                    error_info["dtype"],
                ),
            )
        if "param_name" not in op_param.keys():
            op_param["param_name"] = param_name

    def _check_input(op_param, param_name, param_type, op_name=OP_NAME):
        if param_type == REQUIRED_INPUT:
            error_info = {"errCode": OP_ERROR_CODE_001, "op_name": op_name, "param_name": param_name}
            if op_param is None:
                raise RuntimeError(
                    error_info,
                    "In op[%s], the mandatory parameter[%s] is missed."
                    % (error_info["op_name"], error_info["param_name"]),
                )
            _check_input_output_dict(op_param, param_name, op_name)
        elif param_type == OPTION_INPUT:
            if op_param is not None:
                _check_input_output_dict(op_param, param_name, op_name)
        else:
            if not isinstance(op_param, (list, tuple)):
                error_info = {
                    "errCode": OP_ERROR_CODE_003,
                    "op_name": op_name,
                    "param_name": param_name,
                    "param_type": "list truple",
                    "actual_type": op_param.__class__.__name__,
                }
                raise RuntimeError(
                    error_info,
                    "In op[%s], the parameter[%s]'s type should be [%s],  "
                    "but actually is [%s]."
                    % (op_name, param_name, error_info["param_type"], error_info["actual_type"]),
                )
            if not op_param:
                error_info = {"errCode": OP_ERROR_CODE_001, "op_name": op_name, "param_name": param_name}
                raise RuntimeError(
                    error_info, "In op[%s], the mandatory parameter[%s] is missed." % (op_name, param_name)
                )
            for one_input in op_param:
                _check_input_output_dict(one_input, param_name, op_name)

    def _check_output(op_param, param_name, param_type, op_name=OP_NAME):
        if param_type == REQUIRED_OUTPUT:
            if op_param is None:
                error_info = {"errCode": OP_ERROR_CODE_001, "op_name": op_name, "param_name": param_name}
                raise RuntimeError(
                    error_info, "In op[%s], the mandatory parameter[%s] is missed." % (op_name, param_name)
                )

            _check_input_output_dict(op_param, param_name, op_name)
        elif param_type == OPTION_OUTPUT:
            if op_param is not None:
                _check_input_output_dict(op_param, param_name, op_name)
        else:
            if not isinstance(op_param, (list, tuple)):
                error_info = {
                    "errCode": OP_ERROR_CODE_003,
                    "op_name": op_name,
                    "param_name": param_name,
                    "param_type": "list tuple",
                    "actual_type": op_param.__class__.__name__,
                }
                raise RuntimeError(
                    error_info,
                    "In op[%s], the parameter[%s]'s type should be [%s],  "
                    "but actually is [%s]."
                    % (op_name, param_name, error_info["param_type"], error_info["actual_type"]),
                )
            if not op_param:
                error_info = {"errCode": OP_ERROR_CODE_001, "op_name": op_name, "param_name": param_name}
                raise RuntimeError(
                    error_info, "In op[%s], the mandatory  parameter[%s] is missed." % (op_name, param_name)
                )
            for one_input in op_param:
                _check_input_output_dict(one_input, param_name, op_name)

    def _check_attr_type(op_param, param_name, py_type, py_type_name, op_name=OP_NAME):
        if not isinstance(op_param, py_type):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": op_name,
                "param_name": param_name,
                "param_type": str(py_type),
                "actual_type": op_param.__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s]'s type should be [%s],"
                " but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["param_type"],
                    error_info["actual_type"],
                ),
            )

    def _check_list_attr(op_param, param_name, param_type, op_name=OP_NAME):
        if not isinstance(op_param, (list, tuple)):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": op_name,
                "param_name": param_name,
                "param_type": "list tuple",
                "actual_type": op_param.__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s]'s type should be [%s],"
                "  but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["param_type"],
                    error_info["actual_type"],
                ),
            )

        if param_type in [REQUIRED_ATTR_LIST_BOOL, OPTION_ATTR_LIST_BOOL]:
            for one_attr in op_param:
                _check_attr_type(one_attr, param_name, bool, "bool", op_name)

        if param_type in [REQUIRED_ATTR_LIST_INT, OPTION_ATTR_LIST_INT]:
            for one_attr in op_param:
                _check_attr_type(one_attr, param_name, int, "int", op_name)

        if param_type in [REQUIRED_ATTR_LIST_FLOAT, OPTION_ATTR_LIST_FLOAT]:
            for one_attr in op_param:
                _check_attr_type(one_attr, param_name, float, "float", op_name)

        if param_type in [REQUIRED_ATTR_LIST_LIST_INT, OPTION_ATTR_LIST_LIST_INT]:
            for one_attr in op_param:
                if not isinstance(one_attr, (list, tuple)):
                    error_info = {
                        "errCode": OP_ERROR_CODE_003,
                        "op_name": op_name,
                        "param_name": param_name,
                        "param_type": "list tuple",
                        "actual_type": op_param.__class__.__name__,
                    }
                    raise RuntimeError(
                        error_info,
                        "In op[%s], the parameter[%s]'s type should be [%s],"
                        " but actually is [%s]."
                        % (
                            error_info["op_name"],
                            error_info["param_name"],
                            error_info["param_type"],
                            error_info["actual_type"],
                        ),
                    )

                for ele in one_attr:
                    _check_attr_type(ele, param_name, int, "int", op_name)

    def _check_attr(op_param, param_name, param_type, op_name=OP_NAME):
        if op_param is None and param_type in required_attr_params:
            error_info = {"errCode": OP_ERROR_CODE_001, "op_name": op_name, "param_name": param_name}
            raise RuntimeError(error_info, "In op[%s], the mandatory parameter[%s] is missed." % (op_name, param_name))
        if not op_param:
            return

        if param_type in [REQUIRED_ATTR_INT, OPTION_ATTR_INT]:
            _check_attr_type(op_param, param_name, int, "int", op_name)

        if param_type in [REQUIRED_ATTR_FLOAT, OPTION_ATTR_FLOAT]:
            _check_attr_type(op_param, param_name, float, "float", op_name)

        if param_type in [REQUIRED_ATTR_STR, OPTION_ATTR_STR]:
            _check_attr_type(op_param, param_name, str, "string", op_name)

        if param_type in [REQUIRED_ATTR_BOOL, OPTION_ATTR_BOOL]:
            _check_attr_type(op_param, param_name, bool, "bool", op_name)

        if param_type in [REQUIRED_ATTR_TYPE, OPTION_ATTR_TYPE]:
            if op_param not in ALL_DTYPE_LIST:
                error_info = {
                    "errCode": OP_ERROR_CODE_003,
                    "op_name": op_name,
                    "param_name": param_name,
                    "param_type": " ".join(ALL_DTYPE_LIST),
                    "actual_type": op_param.__class__.__name__,
                }
                raise RuntimeError(
                    error_info,
                    "In op[%s], the parameter[%s]'s dtype "
                    "should be one of [%s], but actually is [%s]."
                    % (
                        error_info["op_name"],
                        error_info["param_name"],
                        error_info["param_type"],
                        error_info["actual_type"],
                    ),
                )

        if param_type in list_type_attr:
            _check_list_attr(op_param, param_name, param_type, op_name)

    def _check_kernel_name(kernel_name, param_name, op_name):
        """
        check kernel_name
        """
        if not isinstance(kernel_name, str):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": op_name,
                "param_name": param_name,
                "param_type": "str",
                "actual_type": kernel_name.__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s]'s type should be [%s], "
                "but actually is [%s]."
                % (
                    error_info["op_name"],
                    error_info["param_name"],
                    error_info["param_type"],
                    error_info["actual_type"],
                ),
            )

        if len(kernel_name) > MAX_KERNEL_NAME_LEN:
            error_info = {
                "errCode": OP_ERROR_CODE_002,
                "op_name": op_name,
                "param_name": param_name,
                "min_value": "0",
                "max_value": str(MAX_KERNEL_NAME_LEN),
                "real_value": str(len(kernel_name)),
            }
            raise RuntimeError(
                error_info,
                "In op[%s], the parameter[%s] should be in "
                "the range of [%s, %s], but actually is [%s]."
                % (
                    error_info.get("op_name"),
                    error_info.get("param_name"),
                    error_info["min_value"],
                    error_info["max_value"],
                    error_info["real_value"],
                ),
            )

        pattern = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
        if not pattern.match(kernel_name):
            error_info = {"errCode": OP_ERROR_CODE_020, "op_name": op_name}
            raise RuntimeError(
                error_info,
                "In op[%s],kernel_name can only contain letters, numbers and "
                "underscores, and begin with underscores or letters" % (error_info["op_name"]),
            )

    def _check_one_op_param(op_param, param_name, param_type, op_name=OP_NAME):
        if param_type in input_params:
            _check_input(op_param, param_name, param_type, op_name)
        elif param_type in output_params:
            _check_output(op_param, param_name, param_type, op_name)
        elif param_type == KERNEL_NAME:
            if op_param is None:
                return
            _check_kernel_name(op_param, param_name, op_name)
        else:  # else is attr_params:
            _check_attr(op_param, param_name, param_type, op_name)

    def _out_wrapper(func):
        formal_parameter = func.__code__.co_varnames
        formal_parameter_list = list(zip(formal_parameter, type_args))

        @wraps(func)
        def _in_wrapper(*args, **kwargs):
            global OP_NAME
            OP_NAME = func.__name__
            for i, one_args in enumerate(args):
                op_name = func.__name__
                _check_one_op_param(one_args, formal_parameter_list[i][0], formal_parameter_list[i][1], op_name)

            for arg_key in kwargs:
                op_name = func.__name__
                for name_type in formal_parameter_list:
                    if arg_key == name_type[0]:
                        _check_one_op_param(kwargs[arg_key], arg_key, name_type[1], op_name)
                        break

            return func(*args, **kwargs)

        return _in_wrapper

    return _out_wrapper


def _check_range(
    shape, shape_range, min_dim=0, max_dim=RANK_LIMIT, max_shape_num=MAX_UNKNOWN_SHAPE_NUM_INT64, param_name=PARAM_NAME
):
    """
    check rule for tensor shape
    """
    if not isinstance(shape_range, (tuple, list)):
        error_info = {
            "errCode": OP_ERROR_CODE_003,
            "op_name": OP_NAME,
            "param_name": param_name,
            "param_type": "list tuple",
            "actual_type": shape_range.__class__.__name__,
        }
        raise RuntimeError(
            error_info,
            "In op, the parameter[%s]'s type should be [%s],"
            "but actually is [%s]." % (error_info["param_name"], error_info["param_type"], error_info["actual_type"]),
        )
    if len(shape) != len(shape_range):
        error_info = {
            "errCode": OP_ERROR_CODE_021,
            "op_name": OP_NAME,
            "param_name": param_name,
            "shape_len": len(shape),
            "range_len": len(shape_range),
        }
        raise RuntimeError(
            error_info,
            "In op, the length of shape[%s] and the length of range[%s] "
            "must be the same." % (error_info["shape_len"], error_info["range_len"]),
        )

    for range_i in shape_range:
        if len(range_i) != 2:
            error_info = {"errCode": OP_ERROR_CODE_023, "op_name": OP_NAME, "param_name": param_name}
            raise RuntimeError(
                error_info,
                "In op[%s],the length of each element in the range must be two" % (error_info.get("op_name")),
            )

        if (range_i[1] is None) and isinstance(range_i[0], int) and 0 <= range_i[0] <= max_shape_num:
            continue
        if not isinstance(range_i[0], int):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": OP_NAME,
                "param_name": param_name,
                "param_type": "int",
                "actual_type": range_i[0].__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op, the parameter[%s]'s type should be [%s], "
                "but actually is [%s]."
                % (error_info["param_name"], error_info["param_type"], error_info["actual_type"]),
            )
        if not isinstance(range_i[1], int):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": OP_NAME,
                "param_name": param_name,
                "param_type": "int",
                "actual_type": range_i[1].__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op, the parameter[%s]'s type should be [%s],"
                "but actually is [%s]."
                % (error_info["param_name"], error_info["param_type"], error_info["actual_type"]),
            )
        valid_type = isinstance(range_i[0], int) and isinstance(range_i[1], int)
        valid_range = 0 <= range_i[0] <= range_i[1] <= max_shape_num
        if valid_type and valid_range:
            continue
        else:
            error_info = {
                "errCode": OP_ERROR_CODE_022,
                "op_name": OP_NAME,
                "param_name": param_name,
                "first_real_value": range_i[0],
                "second_real_value": range_i[1],
                "min_range_value": 0,
                "max_range_value": max_shape_num,
            }
            raise RuntimeError(
                error_info,
                "In op, the dim of first range input[%s] is less than "
                "that of the second range input[%s], and the dim of range "
                "should be in the range of [%s, %s]."
                % (error_info["first_real_value"], error_info["second_real_value"], 0, max_shape_num),
            )


def _check_dynamic_shape(shape, max_dim=DIM_LIMIT_INT64, max_rank=RANK_LIMIT, param_name=PARAM_NAME):
    _check_shape_range(max_rank, MIN_UNKNOWN_SHAPE_RANK, param_name, shape)
    for _, dim in enumerate(shape):
        valid_dim = -2 <= dim <= max_dim
        if not valid_dim:
            error_info = {
                "errCode": OP_ERROR_CODE_002,
                "op_name": OP_NAME,
                "param_name": param_name,
                "min_value": "-2",
                "max_value": max_dim,
                "real_value": dim,
            }
            raise RuntimeError(
                error_info,
                "In op, the parameter[%s] should be in "
                "the range of [%s, %s], "
                "but actually is [%s]." % (error_info["param_name"], -2, max_dim, dim),
            )


def check_shape(
    shape,
    min_dim=0,
    max_dim=DIM_LIMIT_INT64,
    min_rank=0,
    max_rank=RANK_LIMIT,
    min_size=0,
    max_size=SHAPE_SIZE_LIMIT,
    param_name=PARAM_NAME,
):
    """
    check shape size
    """
    from asc_op_compile_base.common.context import in_dynamic

    if not isinstance(shape, (tuple, list)):
        error_info = {
            "errCode": OP_ERROR_CODE_003,
            "op_name": OP_NAME,
            "param_name": param_name,
            "param_type": "list tuple",
            "actual_type": shape.__class__.__name__,
        }
        raise RuntimeError(
            error_info,
            "In op, the parameter[%s]'s type should be [%s], "
            "but actually is [%s]." % (error_info["param_name"], error_info["param_type"], error_info["actual_type"]),
        )

    for dim in shape:
        if not isinstance(dim, int):
            error_info = {
                "errCode": OP_ERROR_CODE_003,
                "op_name": OP_NAME,
                "param_name": param_name,
                "param_type": "int",
                "actual_type": dim.__class__.__name__,
            }
            raise RuntimeError(
                error_info,
                "In op, the parameter[%s]'s type should be [%s],  "
                "but actually is [%s]."
                % (error_info["param_name"], error_info["param_type"], error_info["actual_type"]),
            )

    if in_dynamic():
        _check_dynamic_shape(shape, max_dim, max_rank, param_name)
    else:
        _check_shape_range(max_rank, min_rank, param_name, shape)

        for _, dim in enumerate(shape):
            if dim < min_dim:
                error_info = {
                    "errCode": OP_ERROR_CODE_002,
                    "op_name": OP_NAME,
                    "param_name": param_name,
                    "min_value": min_dim,
                    "real_value": dim,
                }
                raise RuntimeError(
                    error_info,
                    "In op, the dim value[%s] should more than [%s],"
                    " but actually is [%s]." % (error_info["param_name"], min_dim, dim),
                )
        if shape:
            shape_size = _reduce(lambda x, y: x * y, shape[:])
        else:
            shape_size = 1
        if shape_size < min_size:
            error_info = {
                "errCode": OP_ERROR_CODE_011,
                "op_name": OP_NAME,
                "param_name": param_name,
                "min_value": min_size,
                "real_value": shape_size,
            }
            raise RuntimeError(
                error_info,
                "In op, the shape size(product of all dimensions) of "
                "input[%s] should more than [%s], but actually is [%s]."
                % (error_info["min_value"], min_size, shape_size),
            )


def _check_shape_range(max_rank, min_rank, param_name, shape):
    if len(shape) < min_rank or len(shape) > max_rank:
        error_info = {
            "errCode": OP_ERROR_CODE_012,
            "op_name": OP_NAME,
            "param_name": param_name,
            "min_value": min_rank,
            "max_value": max_rank,
            "real_value": len(shape),
        }
        raise RuntimeError(
            error_info,
            "In op, the num of dimensions of input/output[%s] should be in "
            "the range of [%s, %s], but actually is [%s]." % (error_info["param_name"], min_rank, max_rank, len(shape)),
        )
