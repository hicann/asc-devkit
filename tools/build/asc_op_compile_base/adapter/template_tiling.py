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

import copy
import ast
import operator
import re
from enum import Enum, auto
from dataclasses import dataclass
from typing import List, Any
from tbe.common.context import get_context
from .log_utils import AscendCLogLevel
from .ascendc_common_utility import CommonUtility
from .get_op_tiling import OpInfo

TILING_DECLARE_MAP = []
TILING_SELECT_MAP = []
ONE_BYTE_BIT = 8

UI_RANGE = 0
UI_LIST = 1
UI_MIX = 2

# native datatype and dataformat support
ASCENDC_TPL_INPUT_BIAS = 1e8
ASCENDC_TPL_OUTPUT_BIAS = 9e8
ASCENDC_TPL_DATATYPE_MAX = 42
ASCENDC_COMPILE_DATATYPE_MAP = {
    "DT_FLOAT": ["float32", 0],
    "DT_FLOAT16": ["float16", 1],
    "DT_INT8": ["int8", 2],
    "DT_INT32": ["int32", 3],
    "DT_UINT8": ["uint8", 4],
    "DT_INT16": ["int16", 6],
    "DT_UINT16": ["uint16", 7],
    "DT_UINT32": ["uint32", 8],
    "DT_INT64": ["int64", 9],
    "DT_UINT64": ["uint64", 10],
    "DT_DOUBLE": ["double", 11],
    "DT_BOOL": ["bool", 12],
    "DT_COMPLEX64": ["complex64", 16],
    "DT_BF16": ["bfloat16", 27],
    "DT_INT4": ["int4", 29],
    "DT_UINT1": ["uint1", 30],
    "DT_INT2": ["int2", 31],
    "DT_COMPLEX32": ["complex32", 33],
    "DT_HIFLOAT8": ["hifloat8", 34],
    "DT_FLOAT8_E5M2": ["float8_e5m2", 35],
    "DT_FLOAT8_E4M3FN": ["float8_e4m3fn", 36],
    "DT_FLOAT4_E2M1": ["float4_e2m1", 40],
    "DT_FLOAT4_E1M2": ["float4_e1m2", 41],
    "DT_MAX": ["unknown", ASCENDC_TPL_DATATYPE_MAX],
}
ASCENDC_COMPILE_DATATYPE_REVERT_MAP = {
    v[-1]: v[0] for v in ASCENDC_COMPILE_DATATYPE_MAP.values()
}
ASCENDC_KERNEL_TEMPLATE_INPUT_DATATYPE_MAP = {
    "float": 0,
    "half": 1,
    "int8_t": 2,
    "int32_t": 3,
    "uint8_t": 4,
    "int16_t": 6,
    "uint16_t": 7,
    "uint32_t": 8,
    "int64_t": 9,
    "uint64_t": 10,
    "double": 11,
    "bool": 12,
    "complex64": 16,
    "bfloat16_t": 27,
    "int4b_t": 29,
    "hifloat8_t": 34,
    "fp8_e5m2_t": 35,
    "fp8_e4m3fn_t": 36,
    "fp4x2_e2m1_t": 40,
    "fp4x2_e1m2_t": 41,
}
ASCENDC_KERNEL_TEMPLATE_INPUT_KERNEL_TYPE_MAP = {
    "ASCENDC_TPL_AIV_ONLY": 0,
    "ASCENDC_TPL_AIC_ONLY": 1,
    "ASCENDC_TPL_MIX_AIV_1_0": 4,
    "ASCENDC_TPL_MIX_AIC_1_0": 5,
    "ASCENDC_TPL_MIX_AIC_1_1": 6,
    "ASCENDC_TPL_MIX_AIC_1_2": 7,
    "ASCENDC_TPL_AICORE": 8,
    "ASCENDC_TPL_VECTORCORE": 9,
    "ASCENDC_TPL_MIX_AICORE": 10,
    "ASCENDC_TPL_MIX_VECTOR_CORE": 11,
    "ASCENDC_TPL_MAX": 12,
}
ASCENDC_TPL_DATAFORMAT_MAX = 55
ASCENDC_COMPILE_DATAFORMAT_MAP = {
    "FORMAT_NCHW": ["NCHW", 0],
    "FORMAT_NHWC": ["NHWC", 1],
    "FORMAT_ND": ["ND", 2],
    "FORMAT_NC1HWC0": ["NC1HWC0", 3],
    "FORMAT_FRACTAL_Z": ["FRACTAL_Z", 4],
    "FORMAT_NC1C0HWPAD": ["NC1C0HWPAD", 5],
    "FORMAT_NHWC1C0": ["NHWC1C0", 6],
    "FORMAT_FSR_NCHW": ["FSR_NCHW", 7],
    "FORMAT_FRACTAL_DECONV": ["FRACTAL_DECONV", 8],
    "FORMAT_C1HWNC0": ["C1HWNC0", 9],
    "FORMAT_FRACTAL_DECONV_TRANSPOSE": ["FRACTAL_DECONV_TRANSPOSE", 10],
    "FORMAT_FRACTAL_DECONV_SP_STRIDE_TRANS": ["FRACTAL_DECONV_SP_STRIDE_TRANS", 11],
    "FORMAT_NC1HWC0_C04": ["NC1HWC0_C04", 12],
    "FORMAT_FRACTAL_Z_C04": ["FRACTAL_Z_C04", 13],
    "FORMAT_CHWN": ["CHWN", 14],
    "FORMAT_FRACTAL_DECONV_SP_STRIDE8_TRANS": ["FRACTAL_DECONV_SP_STRIDE8_TRANS", 15],
    "FORMAT_HWCN": ["HWCN", 16],
    "FORMAT_NC1KHKWHWC0": ["NC1KHKWHWC0", 17],
    "FORMAT_BN_WEIGHT": ["BN_WEIGHT", 18],
    "FORMAT_FILTER_HWCK": ["FILTER_HWCK", 19],
    "FORMAT_HASHTABLE_LOOKUP_LOOKUPS": ["HASHTABLE_LOOKUP_LOOKUPS", 20],
    "FORMAT_HASHTABLE_LOOKUP_KEYS": ["HASHTABLE_LOOKUP_KEYS", 21],
    "FORMAT_HASHTABLE_LOOKUP_VALUE": ["HASHTABLE_LOOKUP_VALUE", 22],
    "FORMAT_HASHTABLE_LOOKUP_OUTPUT": ["HASHTABLE_LOOKUP_OUTPUT", 23],
    "FORMAT_HASHTABLE_LOOKUP_HITS": ["HASHTABLE_LOOKUP_HITS", 24],
    "FORMAT_C1HWNCoC0": ["C1HWNCoC0", 25],
    "FORMAT_MD": ["MD", 26],
    "FORMAT_NDHWC": ["NDHWC", 27],
    "FORMAT_FRACTAL_ZZ": ["FRACTAL_ZZ", 28],
    "FORMAT_FRACTAL_NZ": ["FRACTAL_NZ", 29],
    "FORMAT_NCDHW": ["NCDHW", 30],
    "FORMAT_DHWCN": ["DHWCN", 31],
    "FORMAT_NDC1HWC0": ["NDC1HWC0", 32],
    "FORMAT_FRACTAL_Z_3D": ["FRACTAL_Z_3D", 33],
    "FORMAT_CN": ["CN", 34],
    "FORMAT_NC": ["NC", 35],
    "FORMAT_DHWNC": ["DHWNC", 36],
    "FORMAT_FRACTAL_Z_3D_TRANSPOSE": ["FRACTAL_Z_3D_TRANSPOSE", 37],
    "FORMAT_FRACTAL_ZN_LSTM": ["FRACTAL_ZN_LSTM", 38],
    "FORMAT_FRACTAL_Z_G": ["FRACTAL_Z_G", 39],
    "FORMAT_RESERVED": ["RESERVED", 40],
    "FORMAT_ALL": ["ALL", 41],
    "FORMAT_NULL": ["NULL", 42],
    "FORMAT_ND_RNN_BIAS": ["ND_RNN_BIAS", 43],
    "FORMAT_FRACTAL_ZN_RNN": ["FRACTAL_ZN_RNN", 44],
    "FORMAT_NYUV": ["NYUV", 45],
    "FORMAT_NYUV_A": ["NYUV_A", 46],
    "FORMAT_NCL": ["NCL", 47],
    "FORMAT_FRACTAL_Z_WINO": ["FRACTAL_Z_WINO", 48],
    "FORMAT_C1HWC0": ["C1HWC0", 49],
    "FORMAT_FRACTAL_NZ_C0_16": ["FRACTAL_NZ_C0_16", 50],
    "FORMAT_FRACTAL_NZ_C0_32": ["FRACTAL_NZ_C0_32", 51],
    "FORMAT_FRACTAL_NZ_C0_2": ["FRACTAL_NZ_C0_2", 52],
    "FORMAT_FRACTAL_NZ_C0_4": ["FRACTAL_NZ_C0_4", 53],
    "FORMAT_FRACTAL_NZ_C0_8": ["FRACTAL_NZ_C0_8", 54],
    "FORMAT_MAX": ["unknown", ASCENDC_TPL_DATAFORMAT_MAX],  # 0xff = 255
}
ASCENDC_COMPILE_DATAFORMAT_REVERT_MAP = {
    v[-1]: v[0] for v in ASCENDC_COMPILE_DATAFORMAT_MAP.values()
}
SAFE_EXPR_BIN_OPS = {
    ast.Add: operator.add,
    ast.Sub: operator.sub,
    ast.Mult: operator.mul,
    ast.FloorDiv: operator.floordiv,
    ast.Div: operator.floordiv,
    ast.Mod: operator.mod,
    ast.LShift: operator.lshift,
    ast.RShift: operator.rshift,
    ast.BitOr: operator.or_,
    ast.BitAnd: operator.and_,
    ast.BitXor: operator.xor,
}
SAFE_EXPR_UNARY_OPS = {
    ast.UAdd: operator.pos,
    ast.USub: operator.neg,
    ast.Invert: operator.invert,
}


def safe_eval_int_expr(expr):
    def _eval(node):
        if isinstance(node, ast.Expression):
            return _eval(node.body)
        if isinstance(node, ast.Constant) and isinstance(node.value, int):
            return node.value
        if isinstance(node, ast.BinOp) and type(node.op) in SAFE_EXPR_BIN_OPS:
            return SAFE_EXPR_BIN_OPS[type(node.op)](_eval(node.left), _eval(node.right))
        if isinstance(node, ast.UnaryOp) and type(node.op) in SAFE_EXPR_UNARY_OPS:
            return SAFE_EXPR_UNARY_OPS[type(node.op)](_eval(node.operand))
        raise ValueError("unsupported integer expression")

    return _eval(ast.parse(expr, mode="eval"))


def extract_num(s):
    numbers = re.findall(r"\d+", s)
    return [int(num) for num in numbers]


def extract_str(s):
    strs = re.findall(r"\w+", s)
    return strs


def safe_parse_value(s):
    try:
        return int(s)
    except Exception:
        pass
    try:
        result = int(safe_eval_int_expr(s))
        return result
    except Exception:
        pass
    try:
        if "DT_" in s:
            if s[:5] != "C_DT_":
                CommonUtility.print_compile_log(
                    "",
                    f"datatype only support C_DT_*, but now is [{s}]",
                    AscendCLogLevel.LOG_ERROR,
                )
            datatype = s.split("::")[-1]
            if datatype[:2] == "C_":
                datatype = datatype[2:]
            result = ASCENDC_COMPILE_DATATYPE_MAP[datatype][-1]
            return result
    except Exception:
        pass
    try:
        if "FORMAT_" in s:
            if s[:9] != "C_FORMAT_":
                CommonUtility.print_compile_log(
                    "",
                    f"format only support C_FORMAT_*, but now is [{s}]",
                    AscendCLogLevel.LOG_ERROR,
                )
            dataformat = s.split("::")[-1]
            if dataformat[:2] == "C_":
                dataformat = dataformat[2:]
            result = ASCENDC_COMPILE_DATAFORMAT_MAP[dataformat][-1]
            return result
    except Exception:
        pass
    CommonUtility.print_compile_log(
        "",
        "can't convert str : [{}], it is invalid value.".format(s),
        AscendCLogLevel.LOG_ERROR,
    )
    return -1


def extract_expr(s):
    match = re.search(r"{([^}]*)}", s)
    content = match.group(1).split(",")
    content = [x.strip() for x in content if x.strip()]
    results = [
        safe_parse_value(item) for item in content if safe_parse_value(item) >= 0
    ]
    return results


class TilingParamType(Enum):
    TPL_DTYPE = auto()
    TPL_DATATYPE = auto()
    TPL_FORMAT = auto()
    TPL_DATAFORMAT = auto()
    TPL_UINT = auto()
    TPL_BOOL = auto()
    TPL_TILING_STRUCT = auto()
    TPL_KERNEL_TYPE = auto()
    TPL_DETERMINISTIC = auto()
    TPL_SHARED_KERNEL_TYPE = auto()
    TPL_NONE = auto()


@dataclass
class TilingTemplateParams:
    name: str
    param_type: TilingParamType
    values: List[Any]
    bit_width: int = 0
    macro_type: str = ""

    def __post_init__(self):
        if self.param_type == TilingParamType.TPL_UINT:
            if len(self.values) < 2:
                raise RuntimeError(
                    "Length of ASCENDC_TPL_UINT_{} {} is too short.".format(
                        self.macro_type, self.name
                    )
                )
            if self.bit_width == 0:
                self.bit_width = self.values[0]
                uint_declare_flag = self.values[1]
                vals = self.values[2:]
            else:
                uint_declare_flag = self.values[0]
                vals = self.values[1:]
            if uint_declare_flag == UI_LIST:
                self.values = vals
                self.__check_valid()
                return
            if uint_declare_flag != UI_RANGE and uint_declare_flag != UI_MIX:
                raise RuntimeError(
                    "Cannot find flag in ASCENDC_TPL_UINT_{} {}! Value should be in"
                    " [ASCENDC_TPL_UI_RANGE, ASCENDC_TPL_UI_LIST, ASCENDC_TPL_UI_MIX].".format(
                        self.macro_type, self.name
                    )
                )
            range_num = vals[0]
            extend_num = []
            for i in range(range_num):
                if 1 + i * 2 >= len(vals) - 1:
                    raise RuntimeError(
                        "UI_RANGE declare parse failed, "
                        "because the announced length of ASCENDC_TPL_UINT_{} {}"
                        " is greater than actual values' length.".format(
                            self.macro_type, self.name
                        )
                    )
                extend_num.extend(range(vals[1 + 2 * i], vals[2 + i * 2] + 1))
            if uint_declare_flag == UI_MIX:
                extend_num.extend(vals[1 + range_num * 2 :])
            self.values = extend_num
        self.__check_valid()

    def __check_valid(self):
        if not self.values:
            raise RuntimeError(
                "values of ASCENDC_TPL_{}_{} {} is empty!".format(
                    self.get_param_type_str(), self.macro_type, self.name
                )
            )
        if len(set(self.values)) != len(self.values):
            raise RuntimeError(
                "There is duplicated number in ASCENDC_TPL_{}_{} {}!"
                " Duplicated List: {}.".format(
                    self.macro_type, self.get_param_type_str(), self.name, self.values
                )
            )
        if self.param_type == TilingParamType.TPL_BOOL:
            if not set(self.values).issubset({0, 1}):
                raise RuntimeError(
                    "There is invalid number in ASCENDC_TPL_BOOL_{} {}!"
                    " Value should only be in [0, 1].".format(
                        self.macro_type, self.name
                    )
                )
        elif self.param_type == TilingParamType.TPL_DETERMINISTIC:
            if not set(self.values).issubset({"true", "false"}):
                raise RuntimeError(
                    "There is invalid number in ASCENDC_TPL_DETERMINISTIC_SEL!"
                    "Value should only be in [0, 1, true, false]."
                )
        else:
            self.check_bit_width_valid()

    def check_bit_width_valid(self):
        bit_width_check_mode = (
            "value" if self.param_type != TilingParamType.TPL_UINT else "index"
        )
        if self.bit_width <= 0:
            raise RuntimeError(
                "Bit width in ASCENDC_TPL_{}_{} {} cannot be less than zero!".format(
                    self.get_param_type_str(), self.macro_type, self.name
                )
            )
        max_encode_num = 2**self.bit_width
        if bit_width_check_mode == "value":
            if max_encode_num < max(
                list(filter(lambda x: x < ASCENDC_TPL_INPUT_BIAS, self.values))
            ):
                raise RuntimeError(
                    "Bit width:{} in ASCENDC_TPL_{}_{} {} is not enough to represent all values: {}!"
                    " Please make sure 2^bitWidth is greater than or equal to the [max value of values].".format(
                        self.bit_width,
                        self.get_param_type_str(),
                        self.macro_type,
                        self.name,
                        self.values,
                    )
                )
        elif bit_width_check_mode == "index":
            if max_encode_num < len(
                list(filter(lambda x: x < ASCENDC_TPL_INPUT_BIAS, self.values))
            ):
                raise RuntimeError(
                    "Bit width:{} in ASCENDC_TPL_{}_{} {} is not enough to represent all values: {}!"
                    " Please make sure 2^bitWidth is greater than or equal to the [number of values].".format(
                        self.bit_width,
                        self.get_param_type_str(),
                        self.macro_type,
                        self.name,
                        self.values,
                    )
                )

    def get_encodes(self):
        encodes = dict()
        if self.param_type == TilingParamType.TPL_UINT:
            for i, value in enumerate(self.values):
                encodes[bin(i)[2:].rjust(self.bit_width, "0")] = value
        elif (
            self.param_type == TilingParamType.TPL_DTYPE
            or self.param_type == TilingParamType.TPL_FORMAT
        ):
            for value in self.values:
                encodes[bin(value)[2:].rjust(self.bit_width, "0")] = value
        elif self.param_type == TilingParamType.TPL_DATATYPE:
            for value in self.values:
                if value >= ASCENDC_TPL_DATATYPE_MAX:
                    continue
                encodes[bin(value)[2:].rjust(self.bit_width, "0")] = value
        elif self.param_type == TilingParamType.TPL_DATAFORMAT:
            for value in self.values:
                if value >= ASCENDC_TPL_DATAFORMAT_MAX:
                    continue
                encodes[bin(value)[2:].rjust(self.bit_width, "0")] = value
        elif self.param_type == TilingParamType.TPL_SHARED_KERNEL_TYPE:
            for value in self.values:
                encodes[bin(value)[2:].rjust(self.bit_width, "0")] = value
        else:
            encodes["0"] = 0
            encodes["1"] = 1
        return encodes

    def get_param_type_str(self) -> str:
        name_dict = {
            TilingParamType.TPL_DTYPE: "DTYPE",
            TilingParamType.TPL_DATATYPE: "DATATYPE",
            TilingParamType.TPL_BOOL: "BOOL",
            TilingParamType.TPL_FORMAT: "FORMAT",
            TilingParamType.TPL_DATAFORMAT: "DATAFORMAT",
            TilingParamType.TPL_UINT: "UINT",
            TilingParamType.TPL_DETERMINISTIC: "DETERMINISTIC",
            TilingParamType.TPL_NONE: "",
            TilingParamType.TPL_SHARED_KERNEL_TYPE: "SHARED_KERNEL_TYPE",
        }
        return name_dict.get(self.param_type, "")


def extract_template_tiling_info(
    tiling_declare_str: str, tiling_select_str: str
) -> None:
    global TILING_DECLARE_MAP
    global TILING_SELECT_MAP
    declare_param_list = tiling_declare_str.split("@@")
    select_param_list = tiling_select_str.split("@@")
    TILING_DECLARE_MAP = extract_template_tiling_params(declare_param_list)[0]
    bit_map = {}
    for declare_param in TILING_DECLARE_MAP:
        bit_map[declare_param.name] = declare_param.bit_width
    TILING_SELECT_MAP = extract_template_tiling_params(select_param_list, bit_map)
    name_order = [declare_param.name for declare_param in TILING_DECLARE_MAP]
    for tiling_param_list in TILING_SELECT_MAP:
        check_valid_select_param(tiling_param_list, name_order)

    for select_params in TILING_SELECT_MAP:
        select_params.sort(
            key=lambda x: name_order.index(x.name) if x.name in name_order else -1
        )


def check_valid_select_param(
    tiling_param_list: List[TilingTemplateParams], name_order: List[str]
) -> None:
    tiling_param_name_list = [
        tiling_param.name
        for tiling_param in tiling_param_list
        if tiling_param.param_type != TilingParamType.TPL_TILING_STRUCT
    ]
    for tiling_declare_param in TILING_DECLARE_MAP:
        if tiling_declare_param.name not in tiling_param_name_list:
            raise RuntimeError(
                "There is missing marco define: {} in ASCENDC_TPL_{}_SEL.".format(
                    tiling_declare_param.name, tiling_declare_param.get_param_type_str()
                )
            )
    total_bit_length = 0

    has_shared_kernel = any(
        p.param_type == TilingParamType.TPL_SHARED_KERNEL_TYPE
        for p in tiling_param_list
    )
    has_kernel = any(
        p.param_type == TilingParamType.TPL_KERNEL_TYPE for p in tiling_param_list
    )
    if has_shared_kernel and has_kernel:
        raise RuntimeError("There exists both TPL_KERNEL_TYPE and SHARED_KERNEL_TYPE")
    for tiling_param in tiling_param_list:
        if (
            tiling_param.param_type == TilingParamType.TPL_TILING_STRUCT
            or tiling_param.param_type == TilingParamType.TPL_KERNEL_TYPE
            or tiling_param.param_type == TilingParamType.TPL_DETERMINISTIC
        ):
            continue
        name = tiling_param.name
        total_bit_length += tiling_param.bit_width
        if total_bit_length > 64:
            raise RuntimeError(
                f"name:{tiling_param.name}, type:{tiling_param.get_param_type_str()}, \
Total bit width cannot be greater than 64!"
            )
        tpl_type = tiling_param.param_type
        vals = tiling_param.values
        matched_param = None
        for param in TILING_DECLARE_MAP:
            if param.name == name:
                matched_param = param
                break
        if matched_param is None:
            raise RuntimeError(
                "Cannot find ASCENDC_TPL_{}_SEL name: {} in ASCENDC_TPL_{}_DECL.".format(
                    tiling_param.get_param_type_str(),
                    name,
                    tiling_param.get_param_type_str(),
                )
            )
        if tpl_type != matched_param.param_type:
            if (
                tpl_type == TilingParamType.TPL_FORMAT
                and matched_param.param_type == TilingParamType.TPL_DATAFORMAT
            ):
                raise RuntimeError(
                    "[Error] Not support both used custom FORMAT and native FORMAT, "
                    "please check {} in ASCENDC_TPL_FORMAT_DECL.".format(
                        name, tiling_param.get_param_type_str()
                    )
                )
            else:
                raise RuntimeError(
                    "{} has different type in ASCENDC_TPL_{}_SEL and ASCENDC_TPL_{}_DECL.".format(
                        name,
                        tiling_param.get_param_type_str(),
                        matched_param.get_param_type_str(),
                    )
                )
        if not set(vals).issubset(set(matched_param.values)):
            raise RuntimeError(
                "Cannot find value {} from ASCENDC_TPL_{}_SEL {} in ASCENDC_TPL_{}_DECL,"
                " please check your macro define.".format(
                    set(vals) - set(matched_param.values),
                    tiling_param.get_param_type_str(),
                    name,
                    tiling_param.get_param_type_str(),
                )
            )


def remove_prefix(text, prefix):
    if text.startswith(prefix):
        return text[len(prefix) :]
    return text


def build_format_class(tiling_param_str: str, name: str, macro_type: str):
    sparse_values_old = extract_num(tiling_param_str)
    sparse_values_new = extract_expr(tiling_param_str)
    if sparse_values_old == sparse_values_new:
        return TilingTemplateParams(
            name, TilingParamType.TPL_FORMAT, sparse_values_old, 8, macro_type
        )
    else:
        return TilingTemplateParams(
            name, TilingParamType.TPL_DATAFORMAT, sparse_values_new, 8, macro_type
        )


def extract_template_tiling_params(
    tiling_param_list: List[str], bit_map: dict = None
) -> list:
    res = []
    tiling_param = []
    for i, _ in enumerate(tiling_param_list):
        sub_str = tiling_param_list[i]
        if not sub_str:
            continue
        if sub_str == "{" and tiling_param:
            res.append(tiling_param)
            tiling_param = []
        macro_type = (
            "SEL"
            if sub_str.startswith("ASCENDC_TPL_DTYPE_SEL_")
            or sub_str.startswith("ASCENDC_TPL_DATATYPE_SEL_")
            or sub_str.startswith("ASCENDC_TPL_FORMAT_SEL_")
            or sub_str.startswith("ASCENDC_TPL_DATAFORMAT_SEL_")
            or sub_str.startswith("ASCENDC_TPL_UINT_SEL_")
            or sub_str.startswith("ASCENDC_TPL_BOOL_SEL_")
            or sub_str.startswith("ASCENDC_TPL_TILING_STRUCT_SEL_")
            or sub_str.startswith("ASCENDC_TPL_KERNEL_TYPE_SEL")
            or sub_str.startswith("ASCENDC_TPL_DETERMINISTIC_SEL")
            or sub_str.startswith("ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL")
            else "DECL"
        )
        if sub_str.startswith("ASCENDC_TPL_DTYPE"):
            dtype_list = extract_num(tiling_param_list[i + 1])
            name = remove_prefix(sub_str, "ASCENDC_TPL_DTYPE_{}_".format(macro_type))
            tiling_param.append(
                TilingTemplateParams(
                    name, TilingParamType.TPL_DTYPE, dtype_list, 8, macro_type
                )
            )
        elif sub_str.startswith("ASCENDC_TPL_DATATYPE"):
            dtype_list = extract_expr(tiling_param_list[i + 1])
            name = remove_prefix(sub_str, "ASCENDC_TPL_DATATYPE_{}_".format(macro_type))
            tiling_param.append(
                TilingTemplateParams(
                    name, TilingParamType.TPL_DATATYPE, dtype_list, 8, macro_type
                )
            )
        elif sub_str.startswith("ASCENDC_TPL_FORMAT"):
            name = remove_prefix(sub_str, "ASCENDC_TPL_FORMAT_{}_".format(macro_type))
            tiling_param.append(
                build_format_class(tiling_param_list[i + 1], name, macro_type)
            )
        elif sub_str.startswith("ASCENDC_TPL_KERNEL_TYPE_SEL"):
            format_list = extract_num(tiling_param_list[i + 1])
            name = "KERNEL_TYPE"
            tiling_param.append(
                TilingTemplateParams(
                    name, TilingParamType.TPL_KERNEL_TYPE, format_list, 8, macro_type
                )
            )
        elif sub_str.startswith("ASCENDC_TPL_DETERMINISTIC_SEL"):
            cur_deter_flag = extract_str(tiling_param_list[i + 1])
            if len(cur_deter_flag) != 1:
                raise RuntimeError(
                    "ASCENDC_TPL_DETERMINISTIC_SEL can only one value can be specified!"
                )
            if cur_deter_flag[0] == "1":
                cur_deter_flag[0] = "true"
            elif cur_deter_flag[0] == "0":
                cur_deter_flag[0] = "false"
            tiling_param.append(
                TilingTemplateParams(
                    "DETERMINISTIC",
                    TilingParamType.TPL_DETERMINISTIC,
                    cur_deter_flag,
                    1,
                    macro_type,
                )
            )
        elif sub_str.startswith("ASCENDC_TPL_UINT"):
            uint_list = extract_num(tiling_param_list[i + 1])
            name = remove_prefix(sub_str, "ASCENDC_TPL_UINT_{}_".format(macro_type))
            if macro_type == "SEL":
                tiling_param.append(
                    TilingTemplateParams(
                        name,
                        TilingParamType.TPL_UINT,
                        uint_list,
                        bit_map[name],
                        macro_type,
                    )
                )
            else:
                tiling_param.append(
                    TilingTemplateParams(
                        name, TilingParamType.TPL_UINT, uint_list, 0, macro_type
                    )
                )
        elif "ASCENDC_TPL_BOOL" in sub_str:
            bool_list = extract_num(tiling_param_list[i + 1])
            name = remove_prefix(sub_str, "ASCENDC_TPL_BOOL_{}_".format(macro_type))
            tiling_param.append(
                TilingTemplateParams(
                    name, TilingParamType.TPL_BOOL, bool_list, 1, macro_type
                )
            )
        elif "ASCENDC_TPL_SHARED_KERNEL_TYPE" in sub_str:
            shared_kernel_type_list = extract_num(tiling_param_list[i + 1])
            name = remove_prefix(
                sub_str, "ASCENDC_TPL_SHARED_KERNEL_TYPE_{}_".format(macro_type)
            )
            tiling_param.append(
                TilingTemplateParams(
                    name,
                    TilingParamType.TPL_SHARED_KERNEL_TYPE,
                    shared_kernel_type_list,
                    8,
                    macro_type,
                )
            )
        elif "ASCENDC_TPL_KERNEL_TYPE" in sub_str:
            if macro_type == "DECL":
                shared_kernel_type_list = extract_num(tiling_param_list[i + 1])
                name = remove_prefix(
                    sub_str, "ASCENDC_TPL_KERNEL_TYPE_{}_".format(macro_type)
                )
                tiling_param.append(
                    TilingTemplateParams(
                        name,
                        TilingParamType.TPL_SHARED_KERNEL_TYPE,
                        shared_kernel_type_list,
                        8,
                        macro_type,
                    )
                )
        elif "ASCENDC_TPL_TILING_STRUCT" in sub_str:
            # 用-1占位
            name = remove_prefix(
                sub_str, "ASCENDC_TPL_TILING_STRUCT_{}_".format(macro_type)
            )
            tiling_param.append(
                TilingTemplateParams(name, TilingParamType.TPL_TILING_STRUCT, [-1], 1)
            )
    if tiling_param:
        res.append(tiling_param)
    return res


GROUP_ID = 0
ID_LIST = []
group_map = {}
kernel_compile_dict = {}


def get_concated_tiling_key(
    template_param_list: List[TilingTemplateParams],
    index: int,
    tiling_key: str,
    tiling_args: List[int],
    encode_book: dict,
    data: dict,
    kernel_index: int,
) -> dict:
    global ID_LIST
    global GROUP_ID
    global group_map
    global kernel_compile_dict
    result = dict()
    if index == len(template_param_list):
        data.update({"paramArgs": tiling_args})
        if kernel_index != -1:
            group_key = tuple(
                tiling_args[:kernel_index] + tiling_args[kernel_index + 1 :]
            )

            if group_key in group_map:
                group_id = group_map[group_key]
            else:
                GROUP_ID += 1
                group_map[group_key] = GROUP_ID
                group_id = GROUP_ID
            data["groupId"] = group_id
            ID_LIST.append(group_id)
        result[int(tiling_key, 2)] = copy.copy(data)
        return result
    template_param = template_param_list[index]
    for val in template_param.values:
        if (
            kernel_compile_dict
            and template_param.name in kernel_compile_dict
            and str(val) not in kernel_compile_dict[template_param.name]
        ):
            continue
        if template_param.param_type == TilingParamType.TPL_TILING_STRUCT:
            data["tilingStruct"] = template_param.name
            encode_ = ""
            tiling_args_temp = tiling_args
        elif template_param.param_type == TilingParamType.TPL_KERNEL_TYPE:
            data["kernelType"] = template_param.values[0]
            encode_ = ""
            tiling_args_temp = tiling_args
        elif template_param.param_type == TilingParamType.TPL_DETERMINISTIC:
            data["deterministic"] = template_param.values[0]
            encode_ = ""
            tiling_args_temp = tiling_args
        elif template_param.param_type == TilingParamType.TPL_DATATYPE:
            data["dtypeParams"].append(
                ASCENDC_COMPILE_DATATYPE_REVERT_MAP.get(val, "unknown")
            )
            encode_ = encode_book[template_param.name][val]
            tiling_args_temp = tiling_args + ["TypeFromId<{}>::type".format(str(val))]
        elif template_param.param_type == TilingParamType.TPL_DATAFORMAT:
            data["formatParams"].append(
                ASCENDC_COMPILE_DATAFORMAT_REVERT_MAP.get(val, "unknown")
            )
            encode_ = encode_book[template_param.name][val]
            tiling_args_temp = tiling_args + [str(val)]
        else:
            if template_param.param_type == TilingParamType.TPL_SHARED_KERNEL_TYPE:
                data["kernelType"] = val
            encode_ = encode_book[template_param.name][val]
            tiling_args_temp = tiling_args + [str(val)]
        result.update(
            get_concated_tiling_key(
                template_param_list,
                index + 1,
                encode_ + tiling_key,
                tiling_args_temp,
                encode_book,
                data,
                kernel_index,
            )
        )
    return result


def extract_decl_param_options(op_info: OpInfo, option_name="dtype"):
    if op_info is None:
        return [], []
    support_option_dict = {
        "dtype": TilingParamType.TPL_DATATYPE,
        "format": TilingParamType.TPL_DATAFORMAT,
    }
    decl_input_indexes = []
    decl_output_indexes = []
    deck_select_indexes = []
    checked_param_type = support_option_dict[option_name]
    for declare_param in TILING_DECLARE_MAP:
        if declare_param.param_type != checked_param_type:
            continue
        deck_select_indexes.append(False)
        for val in declare_param.values:
            if val >= ASCENDC_TPL_INPUT_BIAS and val < ASCENDC_TPL_OUTPUT_BIAS:
                decl_input_indexes.append(int(val % ASCENDC_TPL_INPUT_BIAS))
                deck_select_indexes[-1] = True
                break
            elif val >= ASCENDC_TPL_OUTPUT_BIAS:
                decl_output_indexes.append(int(val % ASCENDC_TPL_OUTPUT_BIAS))
                deck_select_indexes[-1] = True
                break

    def _check_indexes(indexes: List[int], value_list, desc=""):
        if len(indexes) != 0 and (max(indexes) >= len(value_list) or min(indexes) < 0):
            raise ValueError(
                f"exist invalid {desc} index : {indexes}, please check it."
            )
        if len(indexes) != len(set(indexes)):
            raise ValueError(f"exist duplicate {desc} index : {indexes}")

    _check_indexes(decl_input_indexes, op_info.inputs, "input")
    _check_indexes(decl_output_indexes, op_info.outputs, "output")
    decl_input_options = [
        op_info.inputs[idx][option_name] for idx in decl_input_indexes
    ]
    decl_output_options = [
        op_info.outputs[idx][option_name] for idx in decl_output_indexes
    ]
    decl_output_indexes = [int(x + len(op_info.inputs)) for x in decl_output_indexes]
    return decl_input_options + decl_output_options, deck_select_indexes


def check_kernel_template_input(kernel_compile_dict_input: dict):
    tiling_map_dict = {item.name: item for item in TILING_DECLARE_MAP}
    for key, values in kernel_compile_dict_input.items():
        if key not in tiling_map_dict:
            raise RuntimeError("kernel-template-input key {} is invalid!".format(key))
        valid_values = tiling_map_dict[key].values
        valid_values_str = set(str(v) for v in valid_values)
        for value in values:
            if value not in valid_values_str:
                raise RuntimeError(
                    "kernel-template-input key: {} value: {} is invalid!".format(
                        key, value
                    )
                )


def decode_tiling(tiling_key: int = None) -> dict:
    global kernel_compile_dict
    _ctx = get_context()
    kernel_template_input = (
        _ctx.get_addition("kernel-template-input") if _ctx is not None else None
    )
    if kernel_template_input and "=" not in kernel_template_input:
        raise RuntimeError(
            "Invalid kernel-template-input format! Please use key value pairs!"
        )
    if kernel_template_input:
        if kernel_template_input.startswith("'") and kernel_template_input.endswith(
            "'"
        ):
            kernel_template_input = kernel_template_input[1:-1]
        pairs = kernel_template_input.split(";")
        for pair in pairs:
            if pair.count("=") > 1:
                raise RuntimeError("kernel-template-input value has invalid format!")
            if "=" in pair:
                key, value = pair.split("=", 1)
                value_list = value.split(",")
                format_value_list = []
                for v in value_list:
                    if v in ASCENDC_KERNEL_TEMPLATE_INPUT_DATATYPE_MAP.keys():
                        format_value_list.append(
                            str(ASCENDC_KERNEL_TEMPLATE_INPUT_DATATYPE_MAP[v])
                        )
                    elif v == "false":
                        format_value_list.append("0")
                    elif v == "true":
                        format_value_list.append("1")
                    elif v in ASCENDC_KERNEL_TEMPLATE_INPUT_KERNEL_TYPE_MAP.keys():
                        format_value_list.append(
                            str(ASCENDC_KERNEL_TEMPLATE_INPUT_KERNEL_TYPE_MAP[v])
                        )
                    else:
                        format_value_list.append(v)
                kernel_compile_dict[key] = format_value_list
        check_kernel_template_input(kernel_compile_dict)
    encode_book = dict()
    for param in TILING_DECLARE_MAP:
        encode_book[param.name] = param.get_encodes()
    reversed_encode_book = {}
    CommonUtility.print_compile_log(
        "",
        "[Template Tiling] Encode Book is {}.".format(encode_book),
        AscendCLogLevel.LOG_INFO,
    )
    for k, v in encode_book.items():
        reversed_book = dict()
        for bin_encodes, actual_val in v.items():
            reversed_book[actual_val] = bin_encodes
        reversed_encode_book[k] = reversed_book
    decode_results = dict()
    for tiling_template_select_param in TILING_SELECT_MAP:
        kernel_index = next(
            (
                i
                for i, param in enumerate(tiling_template_select_param)
                if param.param_type == TilingParamType.TPL_SHARED_KERNEL_TYPE
            ),
            -1,
        )
        decode_result = get_concated_tiling_key(
            tiling_template_select_param,
            0,
            "",
            [],
            reversed_encode_book,
            {"dtypeParams": [], "formatParams": []},
            kernel_index,
        )
        length_before = len(decode_result) + len(decode_results)
        decode_results.update(decode_result)
        if length_before != len(decode_results):
            raise RuntimeError("ASCENDC_TPL_SELECT has duplicated definitions!")
    if tiling_key is None:
        return decode_results
    if tiling_key not in decode_results.keys():
        CommonUtility.print_compile_log(
            "",
            "Cannot find any matched template tiling keys.",
            AscendCLogLevel.LOG_WARNING,
        )
        return {}
    else:
        return {tiling_key: decode_results[tiling_key]}
