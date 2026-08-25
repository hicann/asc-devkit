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
constant
"""

AttrtypeMapDict = {
    """
    OpKernel : OPC
    """
    "bool": "bool",
    "int": "int",
    "float": "float",
    "str": "string",
    "listBool": "list_bool",
    "listInt": "list_int",
    "listFloat": "list_float",
    "listStr": "list_string",
    "listListInt": "list_list_int",
    "listListFloat": "list_list_float",
}


class GraphDtype:
    INT = "int"
    BOOL = "bool"
    FLOAT = "float"
    STRING = "string"
    LIST_INT = "list_int"
    LIST_BOOL = "list_bool"
    LIST_STRING = "list_string"
    LIST_FLOAT = "list_float"
    LIST_LIST_INT = "list_list_int"
    LIST_LIST_FLOAT = "list_list_float"


class OpcCompileMode:
    """
    OPC support compile mode
    """

    UNKOWN_MODE = None
    SINGLE_OP_DICT_MODE = "single_op_compile_dict_mode"
    SINGLE_OP_CONFIG_FILE_MODE = "single_op_compile_config_file_mode"


class OpcOptions:
    """
    OPC support options
    """

    # command options
    HELP = "help"
    HELP_H = "h"
    SOC_VERSION = "soc_version"

    GRAPH = "graph"
    INPUT_PARAM = "input_param"
    OP_PARAMS = "op_params"
    LOG = "log"

    MAIN_FUNC = "main_func"
    OUTPUT = "output"
    OP_PATH = "op_path"
    DEBUG_DIR = "debug_dir"

    OP_DEBUG_LEVEL = "op_debug_level"
    OP_DEBUG_CONFIG = "op_debug_config"
    IMPL_MODE = "impl_mode"
    CORE_TYPE = "core_type"
    AICORE_NUM = "aicore_num"
    BIN_FILENAME = "bin_filename"  # The name of bin
    DETERMINISTIC = "deterministic"
    JIT_COMPILE_MODE = "jit_compile_mode"
    SIMPLE_KEY_MODE = "simplified_key_mode"

    # inner options
    OP_COMPILE_MODE = "op_compile_classify"
    SINGLE_OP_PARAM = "single_op_param"
    KERNEL_NAME = "kernel_name"
    IS_DYNAMIC = "is_dynamic"
    INT64_MODE = "int64_mode"
    OP_FUNC_ATTR = "op_func_attr"
    KERNEL_META_PATH = "kernel_meta_path"
    OPTIONAL_INPUT_MODE = "optional_input_mode"
    DYNAMIC_PARAM_MODE = "dynamic_param_mode"
    OP_IMPL_SWITCH = "op_impl_switch"
    OP_MODE = "op_mode"
    TILING_KEY = "tiling_key"
    SIMPLE_KEY = "simplified_key"
    KEEP_DIR = "keep_kernel_meta_dir"
    OPTIONAL_OUTPUT_MODE = "optional_output_mode"
    RELOCATABLE_BIN = "op_relocatable_kernel_binary"
    SPK_OPT = "op_super_kernel_options"
    KERNEL_TEMPLATE_INPUT = "kernel-template-input"
    KERNEL_SPEC = "kernel-spec"
    KERNEL_SPEC_DIR = "kernel-spec-dir"
    DETERMINISTIC_LEVEL = "deterministic_level"

    OpcOptionDefaultValueDict = {
        OUTPUT: "",
        SOC_VERSION: "",
        LOG: "",
        INPUT_PARAM: None,
        MAIN_FUNC: None,
        CORE_TYPE: None,
        DEBUG_DIR: "./",
        BIN_FILENAME: "",
        IMPL_MODE: None,
        AICORE_NUM: None,
        OP_DEBUG_LEVEL: 0,
        OP_DEBUG_CONFIG: "",
        OPTIONAL_INPUT_MODE: "",
        OP_MODE: "",
        DETERMINISTIC: "all",
        SIMPLE_KEY_MODE: None,
        DYNAMIC_PARAM_MODE: None,
        KERNEL_SPEC: "None",
        KERNEL_SPEC_DIR: None,
    }


class GraphDefParam:
    # graph def params
    GRAPH_NAME = "graph_name"
    INPUT_PATTERN = "input_pattern"
    ORIGIN_SHAPE = "origin_shape"
    ORIGIN_FORMAT = "origin_format"
    LAYER = "layer"
    INPUT_DESC = "input_desc"
    OUTPUT_DESC = "output_desc"
    VAR_ATTRS = "var_attrs"
    EXTRA_PARAMS = "extra_params"
    OPTIONS = "options"
    CURRENT_SHAPE = "current_shape"
    EXTERNEL_INPUT = "externel_input"


class CompileParam:
    # op compile key
    SOC_INFO = "SocInfo"  # Camel keep History of code
    OP_LIST = "op_list"
    NAME = "name"
    DTYPE = "dtype"
    FORMAT = "format"
    SUB_FORMAT = "sub_format"
    SHAPE = "shape"
    INPUT = "input"
    ORI_SHAPE = "ori_shape"
    ORI_FORMAT = "ori_format"
    VALUE_LIST = "value_list"
    VALUE_RANGE = "value_range"
    RANGE_MODE = "range_mode"
    SIMPLE_KEY = "simplified_key"
    DTYPE_MATCH_MODE = "dtype_match_mode"
    FORMAT_MATCH_MODE = "format_match_mode"
    FORMAT_MODE = "formatMode"
    DTYPE_FOR_BIN_QUERY = "dtypeForBinQuery"
    FORMAT_FOR_BIN_QUERY = "formatForBinQuery"

    ID = "id"
    TYPE = "type"
    PATTERN = "pattern"
    OP_TYPE = "op_type"
    VALUE = "value"
    LIST = "list"
    LIST_LIST_INT = "list_list_int"
    DYNAMIC_IMPL = "is_dynamic_impl"
    INPUTS = "inputs"
    OUTPUTS = "outputs"
    ATTRS = "attrs"  # all attrs in ir graph, include op attrs and other information expressed using attrs
    ATTR = "attr"  # unuse
    RANGE = "range"
    ORI_RANGE = "ori_range"
    SHAPE_RANGE = "shape_range"
    OUTPUT_INDEX = "output_index"
    OP_ATTRS = "op_attrs"  # op real attrs
    OP_ATTRS_DESC = "attr_desc"  # op real attrs value list

    ADDR_TYPE = "addr_type"
    VALID_SHAPE = "valid_shape"
    SLICE_OFFSET = "slice_offset"
    L1_FUSION_TYPE = "L1_fusion_type"
    L1_ADDR_FLAG = "L1_addr_flag"
    L1_ADDR_OFFSET = "L1_addr_offset"
    L1_VALID_SIZE = "L1_valid_size"
    CONST_VALUE = "const_value"
    TOTAL_SHAPE = "total_shape"
    DYN_INDEX = "dyn_index"
    USE_L1_WORKSPACE = "use_L1_workspace"
    L1_WORKSPACE_SIZE = "L1_workspace_size"
    SPLIT_INDEX = "split_index"
    IS_FIRST_LAYER = "is_first_layer"

    GRAPH_PATTERN = "graph_pattern"
    INT64_MODE = "int64mode"
    L1_FUSION = "l1Fusion"
    L2_FUSION = "l2Fusion"
    L2_MODE = "l2Mode"
    L1_SIZE = "l1_size"
    OP_L1_SPACE = "op_L1_space"
    FUSION_OP_NAME = "fusion_op_name"
    STATUS_CHECK = "status_check"
    PARAM_TYPE = "paramType"
    ENABLE_VECTOR_CORE = "enable_vector_core"
    EXTRA_SETTINGS = "extra_settings"


class SupportInfo:
    """
    class SupportInfo
    """

    SUPPORT_INFO = "SupportInfo"
    GRAPH_PATTERN = "graphPattern"
    STATIC_KEY = "staticKey"
    GRAPH_OP_PARAMS = "graphOpParams"
    INT64_MODE = "int64mode"
    INPUTS = "inputs"
    OUTPUTS = "outputs"
    BUILD_OPTIONS = "buildOptions"
    L1_FUSION = "l1_fusion"
    L2_FUSION = "l2_fusion"
    L2_MODE = "l2_mode"
    OPTIONAL_INPUT_MODE = "optionalInputMode"
    OPTIONAL_OUTPUT_MODE = "optionalOutputMode"
    DYNAMIC_PARAM_MODE = "dynamicParamMode"


class OptionalInOutMode:
    """
    OP optional input mode
    """

    DEFAULT = "no_placeholder"
    GEN_PLACEHOLDER = "gen_placeholder"


class OpParamType:
    """
    OP ParamType
    """

    DEFAULT = None
    DYN = "dynamic"
    OPT = "optional"
    REQ = "required"


class OpModeType:
    """
    OP mode type
    """

    DEFAULT = None
    DYNAMIC = "dynamic"
    STATIC = "static"
    PRE_STATIC = "pre-static"


class OpImplType:
    """
    OP ImplType
    """

    DEFAULT = None
    OPTIONAL = "optional"
    HIGH_PERFORMANCE = "high_performance"
    HIGH_PRECISION = "high_precision"
    ENABLE_HI_FLOAT_32 = "enable_hi_float_32_execution"
    ENABLE_FLOAT_32 = "enable_float_32_execution"
    SUPER_PERFORMANCE = "super_performance"
    SUPPORT_OUT_OF_BOUND_INDEX = "support_out_of_bound_index"
    KEEP_FP_16 = "keep_fp16"
    HIGH_PERFORMANCE_OPT = "high_performance,optional"
    HIGH_PRECISION_OPT = "high_precision,optional"


class OpDataType:
    DT_FLOAT = "float32"  # float type      #0
    DT_FLOAT16 = "float16"  # fp16 type       #1
    DT_INT8 = "int8"  # int8 type       #2
    DT_INT32 = "int32"  # int32 type      #3
    DT_UINT8 = "uint8"  # uint8 type      #4
    DT_INT16 = "int16"  # int16 type      #6
    DT_UINT16 = "uint16"  # uint16 type     #7
    DT_UINT32 = "uint32"  # unsigned int32  #8
    DT_INT64 = "int64"  # int64 type      #9
    DT_UINT64 = "uint64"  # unsigned int64  #10
    DT_DOUBLE = "double"  # double type     #11
    DT_BOOL = "bool"  # bool type       #12
    DT_STRING = "string"  # string type     #13
    DT_DUAL_SUB_INT8 = "dual_sub_int8"  # dual output int8 type  #14
    DT_DUAL_SUB_UINT8 = "dual_sub_uint8"  # dual output uint8 type #15
    DT_COMPLEX64 = "complex64"  # complex64 type       #16
    DT_COMPLEX128 = "complex128"  # complex128 type      #17
    DT_QINT8 = "qint8"  # qint8 type           #18
    DT_QINT16 = "qint16"  # qint16 type          #19
    DT_QINT32 = "qint32"  # qint32 type          #20
    DT_QUINT8 = "quint8"  # quint8 type          #21
    DT_QUINT16 = "quint16"  # quint16 type         #22
    DT_RESOURCE = "resource"  # resource type        #23
    DT_STRING_REF = "string_ref"  # string ref type      #24
    DT_DUAL = "dual"  # dual output type     #25
    DT_BF16 = "bfloat16"  # bf16 type            #27
    DT_INT4 = "int4"  # int4 type            #29
    DT_UINT1 = "uint1"  # uint1 type           #30
    DT_INT2 = "int2"  # int2 type            #31
    DT_UINT2 = "uint2"  # uint2 type           #32
    DT_COMPLEX32 = "complex32"  # complex32 type       #33
    DT_HIFLOAT8 = "hifloat8"
    DT_FLOAT8_E5M2 = "float8_e5m2"
    DT_FLOAT8_E4M3FN = "float8_e4m3fn"
    DT_FLOAT8_E8M0 = "float8_e8m0"
    DT_FLOAT6_E3M2 = "float6_e3m2"
    DT_FLOAT6_E2M3 = "float6_e2m3"
    DT_FLOAT4_E2M1 = "float4_e2m1"
    DT_FLOAT4_E1M2 = "float4_e1m2"

    DtypeValueDict = {
        DT_FLOAT: 0,
        DT_FLOAT16: 1,
        DT_INT8: 2,
        DT_INT32: 3,
        DT_UINT8: 4,
        DT_INT16: 6,
        DT_UINT16: 7,
        DT_UINT32: 8,
        DT_INT64: 9,
        DT_UINT64: 10,
        DT_DOUBLE: 11,
        DT_BOOL: 12,
        DT_STRING: 13,
        DT_DUAL_SUB_INT8: 14,
        DT_DUAL_SUB_UINT8: 15,
        DT_COMPLEX64: 16,
        DT_COMPLEX128: 17,
        DT_QINT8: 18,
        DT_QINT16: 19,
        DT_QINT32: 20,
        DT_QUINT8: 21,
        DT_QUINT16: 22,
        DT_RESOURCE: 23,
        DT_STRING_REF: 24,
        DT_DUAL: 25,
        DT_BF16: 27,
        DT_INT4: 29,
        DT_UINT1: 30,
        DT_INT2: 31,
        DT_UINT2: 32,
        DT_COMPLEX32: 33,
        DT_HIFLOAT8: 34,
        DT_FLOAT8_E5M2: 35,
        DT_FLOAT8_E4M3FN: 36,
        DT_FLOAT8_E8M0: 37,
        DT_FLOAT6_E3M2: 38,
        DT_FLOAT6_E2M3: 39,
        DT_FLOAT4_E2M1: 40,
        DT_FLOAT4_E1M2: 41,
    }


class OpFormatType:
    FORMAT_NCHW = "NCHW"  # NCHW       #0
    FORMAT_NHWC = "NHWC"  # NHWC       #1
    FORMAT_ND = "ND"  # ND         #2
    FORMAT_NC1HWC0 = "NC1HWC0"  # NC1HWC0    #3
    FORMAT_FRACTAL_Z = "FRACTAL_Z"  # FRACTAL_Z  #4
    FORMAT_NC1C0HWPAD = "NC1C0HWPAD"  # NC1C0HWPAD #5
    FORMAT_NHWC1C0 = "NHWC1C0"  # NHWC1C0    #6
    FORMAT_FSR_NCHW = "FSR_NCHW"  # FSR_NCHW   #7
    FORMAT_FRACTAL_DECONV = "FRACTAL_DECONV"  # FRACTAL_DECONV    #8
    FORMAT_C1HWNC0 = "C1HWNC0"  # C1HWNC0    #9
    FORMAT_FRACTAL_DECONV_TRANSPOSE = "FRACTAL_DECONV_TRANSPOSE"  # 10
    FORMAT_FRACTAL_DECONV_SP_STRIDE_TRANS = "FRACTAL_DECONV_SP_STRIDE_TRANS"  # 11
    FORMAT_NC1HWC0_C04 = "NC1HWC0_C04"  # NC1HWC0, C0 is 4  #12
    FORMAT_FRACTAL_Z_C04 = "FRACTAL_Z_C04"  # FRACZ, C0 is 4    #13
    FORMAT_CHWN = "CHWN"  # CHWN  #14
    FORMAT_FRACTAL_DECONV_SP_STRIDE8_TRANS = "FRACTAL_DECONV_SP_STRIDE8_TRANS"  # 15
    FORMAT_HWCN = "HWCN"  # HWCN  #16
    FORMAT_NC1KHKWHWC0 = (
        "NC1KHKWHWC0"  # KH,KW kernel h& kernel w maxpooling max output format #17
    )
    FORMAT_BN_WEIGHT = "BN_WEIGHT"  # BN_WEIGHT  #18
    FORMAT_FILTER_HWCK = "FILTER_HWCK"  # filter input tensor format   #19
    FORMAT_HASHTABLE_LOOKUP_LOOKUPS = "HASHTABLE_LOOKUP_LOOKUPS"  # 20
    FORMAT_HASHTABLE_LOOKUP_KEYS = "HASHTABLE_LOOKUP_KEYS"  # 21
    FORMAT_HASHTABLE_LOOKUP_VALUE = "HASHTABLE_LOOKUP_VALUE"  # 22
    FORMAT_HASHTABLE_LOOKUP_OUTPUT = "HASHTABLE_LOOKUP_OUTPUT"  # 23
    FORMAT_HASHTABLE_LOOKUP_HITS = "HASHTABLE_LOOKUP_HITS"  # 24
    FORMAT_C1HWNCoC0 = "C1HWNCoC0"  # 25
    FORMAT_MD = "MD"  # 26
    FORMAT_NDHWC = "NDHWC"  # NDHWC       #27
    FORMAT_FRACTAL_ZZ = "FRACTAL_ZZ"  # FRACTAL_ZZ           #28
    FORMAT_FRACTAL_NZ = "FRACTAL_NZ"  # FRACTAL_NZ           #29
    FORMAT_NCDHW = "NCDHW"  # NCDHW  #30
    FORMAT_DHWCN = "DHWCN"  # DHWCN  31
    FORMAT_NDC1HWC0 = "NDC1HWC0"  # NDC1HWC0         #32
    FORMAT_FRACTAL_Z_3D = "FRACTAL_Z_3D"  # 33
    FORMAT_CN = "CN"  # 34
    FORMAT_NC = "NC"  # 35
    FORMAT_DHWNC = ("DHWNC",)  # 36
    FORMAT_FRACTAL_Z_3D_TRANSPOSE = (
        "FRACTAL_Z_3D_TRANSPOSE"  # 3D filter(transpose) input tensor format  # 37
    )
    FORMAT_FRACTAL_ZN_LSTM = "FRACTAL_ZN_LSTM"  # 38
    FORMAT_FRACTAL_Z_G = "FRACTAL_Z_G"  # 39
    FORMAT_RESERVED = "RESERVED"  # 40
    FORMAT_ALL = "ALL"  # 41
    FORMAT_NULL = "NULL"  # 42
    FORMAT_ND_RNN_BIAS = "ND_RNN_BIAS"  # 43
    FORMAT_FRACTAL_ZN_RNN = "FRACTAL_ZN_RNN"  # 44
    FORMAT_NYUV = "NYUV"  # 45
    FORMAT_NYUV_A = "NYUV_A"  # 46
    FORMAT_NCL = "NCL"  # 47
    FORMAT_FRACTAL_Z_WINO = "FRACTAL_Z_WINO"  # 48
    FORMAT_C1HWC0 = "C1HWC0"  # 49
    FORMAT_FRACTAL_NZ_C0_16 = "FRACTAL_NZ_C0_16"  # 50
    FORMAT_FRACTAL_NZ_C0_32 = "FRACTAL_NZ_C0_32"  # 51

    FormatValueDict = {
        FORMAT_NCHW: 0,
        FORMAT_NHWC: 1,
        FORMAT_ND: 2,
        FORMAT_NC1HWC0: 3,
        FORMAT_FRACTAL_Z: 4,
        FORMAT_NC1C0HWPAD: 5,
        FORMAT_NHWC1C0: 6,
        FORMAT_FSR_NCHW: 7,
        FORMAT_FRACTAL_DECONV: 8,
        FORMAT_C1HWNC0: 9,
        FORMAT_NC1HWC0_C04: 12,
        FORMAT_FRACTAL_Z_C04: 13,
        FORMAT_CHWN: 14,
        FORMAT_HWCN: 16,
        FORMAT_NC1KHKWHWC0: 17,
        FORMAT_C1HWNCoC0: 25,
        FORMAT_MD: 26,
        FORMAT_NDHWC: 27,
        FORMAT_FRACTAL_ZZ: 28,
        FORMAT_FRACTAL_NZ: 29,
        FORMAT_NCDHW: 30,
        FORMAT_DHWCN: 31,
        FORMAT_NDC1HWC0: 32,
        FORMAT_FRACTAL_Z_3D: 33,
        FORMAT_CN: 34,
        FORMAT_NC: 35,
        FORMAT_DHWNC: 36,
        FORMAT_FRACTAL_Z_3D_TRANSPOSE: 37,
        FORMAT_FRACTAL_ZN_LSTM: 38,
        FORMAT_FRACTAL_Z_G: 39,
        FORMAT_RESERVED: 40,
        FORMAT_ALL: 41,
        FORMAT_NULL: 42,
        FORMAT_ND_RNN_BIAS: 43,
        FORMAT_FRACTAL_ZN_RNN: 44,
        FORMAT_NYUV: 45,
        FORMAT_NYUV_A: 46,
        FORMAT_NCL: 47,
        FORMAT_FRACTAL_Z_WINO: 48,
        FORMAT_C1HWC0: 49,
        FORMAT_FRACTAL_NZ_C0_16: 50,
        FORMAT_FRACTAL_NZ_C0_32: 51,
    }
