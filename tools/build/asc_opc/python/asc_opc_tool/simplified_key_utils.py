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


from asc_op_compile_base.common.utils import log as logger
from constant import (
    OpcOptions,
    CompileParam,
    OpImplType,
    OpFormatType,
    OpDataType,
    OpParamType,
)
from opc_common import (
    check_input_and_output_for_simplified_key,
    check_attr_for_simpilified_key,
)


SIMPLIFIED_KEY_MAX_LEN = 1024
CUSTOM_SIMPLIFIED_KEY_MAX_LEN = 256


def _check_simplified_key_length(simplified_key, max_len, key_name):
    key_len = len(simplified_key.encode("utf-8"))
    if key_len >= max_len:
        raise ValueError(
            "{} length must be less than {} bytes, but got {} bytes.".format(
                key_name, max_len, key_len
            )
        )


def init_dynamic_param_and_optional_input(simplified_key_mode):
    if simplified_key_mode == 0:
        # default values for 0
        dynamic_param_mode = "folded_with_desc"
        optional_param_mode = "gen_placeholder"
    if simplified_key_mode == 1:
        # default values for 1
        dynamic_param_mode = "unfolded"
        optional_param_mode = "no_placeholder"
    if simplified_key_mode is None:
        dynamic_param_mode = None
        optional_param_mode = None

    return dynamic_param_mode, optional_param_mode


def generate_simplified_key_mode_0(tmp_tuple, simplified_key_mode):
    has_dynamic_param, dynamic_param_mode, has_optional_param = (
        tmp_tuple[0],
        tmp_tuple[1],
        tmp_tuple[2],
    )
    optional_param_mode, has_non_null_attr = tmp_tuple[3], tmp_tuple[4]
    dynamic_param_condition = (not has_dynamic_param) or (
        has_dynamic_param and dynamic_param_mode == "folded_with_desc"
    )
    optional_param_condition = (not has_optional_param) or (
        has_optional_param and optional_param_mode == "gen_placeholder"
    )

    if (not has_non_null_attr) and dynamic_param_condition and optional_param_condition:
        infer_key_mode = 0
        logger.info(
            "simplified_key_mode is 0, dynamic_param_mode: %s, optional_param_mode: %s",
            dynamic_param_mode,
            optional_param_mode,
        )
        return infer_key_mode, optional_param_mode, dynamic_param_mode
    else:
        return simplified_key_mode, optional_param_mode, dynamic_param_mode


def generate_simplified_key_mode_1(tmp_tuple, simplified_key_mode):
    has_dynamic_param, dynamic_param_mode, has_optional_param = (
        tmp_tuple[0],
        tmp_tuple[1],
        tmp_tuple[2],
    )
    optional_param_mode, has_non_null_attr = tmp_tuple[3], tmp_tuple[4]
    dynamic_param_condition = (not has_dynamic_param) or (
        has_dynamic_param and dynamic_param_mode != "folded_with_desc"
    )
    optional_param_condition = (not has_optional_param) or (
        has_optional_param and optional_param_mode != "gen_placeholder"
    )
    if has_non_null_attr or dynamic_param_condition or optional_param_condition:
        infer_key_mode = 1
        dynamic_param_mode = (
            "unfolded" if dynamic_param_mode is None else dynamic_param_mode
        )
        optional_param_mode = (
            "no_placeholder" if optional_param_mode is None else optional_param_mode
        )
        logger.info(
            "simplified_key_mode is 1, dynamic_param_mode: %s, optional_param_mode: %s",
            dynamic_param_mode,
            optional_param_mode,
        )
        return infer_key_mode, optional_param_mode, dynamic_param_mode
    else:
        return simplified_key_mode, optional_param_mode, dynamic_param_mode


def generate_simplified_key_mode(
    opc_compile_args, has_non_null_attr, has_dynamic_param, has_optional_param
):
    simplified_key_mode = opc_compile_args.get(OpcOptions.SIMPLE_KEY_MODE)
    if simplified_key_mode == 2:
        return (simplified_key_mode, None, None)

    # when simplified_key_mode has been configured as 0 or 1, we need confirm optional_param_mode and dynamic_param_mode
    # when simplified_key_mode has not been configured, we need infer simplified_key_mode by configured param mode
    dynamic_param_mode, optional_param_mode = init_dynamic_param_and_optional_input(
        simplified_key_mode
    )
    optional_input_mode_config_value = opc_compile_args.get(
        OpcOptions.OPTIONAL_INPUT_MODE
    )
    if (
        optional_input_mode_config_value is not None
        and optional_input_mode_config_value != optional_param_mode
    ):
        optional_param_mode = optional_input_mode_config_value
    logger.info("optional_input_mode: %s", optional_param_mode)

    dynamic_param_mode_config_value = opc_compile_args.get(
        OpcOptions.DYNAMIC_PARAM_MODE
    )
    if (dynamic_param_mode_config_value is not None) and (
        dynamic_param_mode_config_value != dynamic_param_mode
    ):
        dynamic_param_mode = dynamic_param_mode_config_value
        logger.info("dynamic_param_mode is defined: %s", dynamic_param_mode)

    if simplified_key_mode is not None:
        if (simplified_key_mode == 0) and (optional_param_mode == "no_placeholder"):
            logger.warn(
                "simplified_key_mode is defined as 0, but optional_param_mode is no_placeholder, \
                        will not generate simplified key"
            )
            return (None, None, None)
        logger.info(
            "simplified_key_mode: %d, dynamic_param_mode: %s, optional_param_mode: %s",
            simplified_key_mode,
            dynamic_param_mode,
            optional_param_mode,
        )
        return (simplified_key_mode, optional_param_mode, dynamic_param_mode)

    tmp_tuple = (
        has_dynamic_param,
        dynamic_param_mode,
        has_optional_param,
        optional_param_mode,
        has_non_null_attr,
    )
    logger.info("tmp_tuple is {}".format(tmp_tuple))
    infer_key_mode, optional_param_mode, dynamic_param_mode = (
        generate_simplified_key_mode_0(tmp_tuple, simplified_key_mode)
    )
    if infer_key_mode == 0:
        return (infer_key_mode, optional_param_mode, dynamic_param_mode)

    infer_key_mode, optional_param_mode, dynamic_param_mode = (
        generate_simplified_key_mode_1(tmp_tuple, simplified_key_mode)
    )
    if infer_key_mode == 1:
        return (infer_key_mode, optional_param_mode, dynamic_param_mode)

    logger.warn("simplified_key_mode is None")
    return (None, None, None)


def infer_simplified_key_mode(op, opc_compile_args):
    simplified_key_configured = op.get(OpcOptions.SIMPLE_KEY, None)
    if simplified_key_configured is not None and simplified_key_configured != "":
        _check_simplified_key_length(
            simplified_key_configured,
            CUSTOM_SIMPLIFIED_KEY_MAX_LEN,
            "custom simplified_key",
        )
        logger.warn("Custom simplified_key is configured; custom mode 2 will be used.")
        return (2, None, None, None, None)
    has_invalid_option, has_non_null_attr, attr_str = check_attr_for_simpilified_key(
        op.get(CompileParam.ATTRS)
    )

    if has_invalid_option and opc_compile_args.get(OpcOptions.SIMPLE_KEY_MODE) != 0:
        return (None, None, None, None, None)

    has_invalid_option, has_dynamic_param, has_optional_input, has_optional_output = (
        check_input_and_output_for_simplified_key(op)
    )
    if has_invalid_option:
        return (None, None, None, None, None)
    res_list = generate_simplified_key_mode(
        opc_compile_args, has_non_null_attr, has_dynamic_param, has_optional_input
    )
    has_optional_param = has_optional_input or has_optional_output
    return (
        res_list[0],
        res_list[1],
        has_optional_param,
        res_list[2],
        has_dynamic_param,
    )


def generate_deterministic_for_simpilified_key(deterministic):
    """
    generate d=0/1 for simplified key
    """
    determi_str = str()
    determi_list = []
    if deterministic is None or deterministic == "false":
        determi_str = "d=0"
        determi_list.append(determi_str)
    elif deterministic == "true":
        determi_str = "d=1"
        determi_list.append(determi_str)
    elif deterministic == "ignore":
        determi_str = "d=0"
        determi_list.append(determi_str)
        determi_str = "d=1"
        determi_list.append(determi_str)
    else:
        logger.error("deterministic Type [%s] not supported.", deterministic)
    logger.info("deterministic_str: %s", str(determi_list))
    return determi_list


def generate_custom_mode_simplified_key(
    op_type, op_simplified_key, deterministic_list, impl_mode_list
):
    _check_simplified_key_length(
        op_simplified_key,
        CUSTOM_SIMPLIFIED_KEY_MAX_LEN,
        "custom simplified_key",
    )
    simplified_key = []
    for deterministic_str in deterministic_list:
        for impl_mode_str in impl_mode_list:
            simplified_key_str = (
                op_type
                + "/"
                + deterministic_str
                + ","
                + impl_mode_str
                + "/"
                + op_simplified_key
            )
            _check_simplified_key_length(
                simplified_key_str, SIMPLIFIED_KEY_MAX_LEN, "simplified_key"
            )
            simplified_key.append(simplified_key_str)
            logger.info(
                "simplified_key_str=[%s], simplified_key=[%s]",
                simplified_key_str,
                str(simplified_key),
            )
    return simplified_key


def generate_inputs_outputs_simpilified_key_str(
    final_num, op, simplified_key_mode, optional_input_mode, dynamic_param_mode
):
    input_list, output_list = [], []

    for index in range(0, final_num):
        inputs_str = generate_input_or_output_for_simpilified_key(
            op.get(CompileParam.INPUTS),
            simplified_key_mode,
            optional_input_mode,
            dynamic_param_mode,
            index,
        )
        outputs_str = generate_input_or_output_for_simpilified_key(
            op.get(CompileParam.OUTPUTS),
            simplified_key_mode,
            optional_input_mode,
            dynamic_param_mode,
            index,
        )

        input_list.append(inputs_str)
        output_list.append(outputs_str)

    return input_list, output_list


def assemble_simplified_key(
    simlified_info_tuple, op_type, input_list, output_list, attr_str
):
    final_num = len(input_list)
    simplified_key = []
    deterministic_list, impl_mode_list = (
        simlified_info_tuple[0],
        simlified_info_tuple[1],
    )
    for deterministic_str in deterministic_list:
        for impl_mode_str in impl_mode_list:
            for index in range(0, final_num):
                simplified_key_str = (
                    op_type + "/" + deterministic_str + "," + impl_mode_str
                )
                if input_list:
                    simplified_key_str = simplified_key_str + input_list[index]
                if output_list:
                    simplified_key_str = simplified_key_str + output_list[index]
                if attr_str:
                    simplified_key_str = simplified_key_str + attr_str
                simplified_key.append(simplified_key_str)
                logger.info(
                    "simplified_key_str=[%s], simplified_key=[%s]",
                    simplified_key_str,
                    str(simplified_key),
                )
    return simplified_key


def generate_simplified_key_str(res_tuple, simlified_info_tuple, op, op_type):
    simplified_key_mode, optional_input_mode, dynamic_param_mode = (
        res_tuple[0],
        res_tuple[1],
        res_tuple[2],
    )
    attr_str = simlified_info_tuple[2]
    if simplified_key_mode == 0:
        attr_str = str()
    if simplified_key_mode is not None:
        inputs = op.get(CompileParam.INPUTS)
        outputs = op.get(CompileParam.OUTPUTS)
        inputs_and_outputs = inputs + outputs
        final_num = 1
        res, dtype_num, format_num = check_in_and_out_is_valid(inputs_and_outputs)
        if res:
            final_num = format_num
            if dtype_num >= format_num:
                final_num = dtype_num
        else:
            raise ValueError(
                "invalid dtypeForBinQuery and formatForBinQuery configuration,"
                "will not generate simplified_key"
            )

        logger.debug("final_num is [%d]", final_num)

        input_list, output_list = generate_inputs_outputs_simpilified_key_str(
            final_num, op, simplified_key_mode, optional_input_mode, dynamic_param_mode
        )

        logger.debug(
            "input_list is {}, output_list is {}".format(input_list, output_list)
        )
        simplified_key = assemble_simplified_key(
            simlified_info_tuple, op_type, input_list, output_list, attr_str
        )

    return simplified_key


def generate_impl_mode_for_simpilified_key(op_info):
    """
    generate p=0/1/2/4/8 for simplified key
    """
    impl_mode_dict = {
        OpImplType.DEFAULT: 0,
        OpImplType.HIGH_PERFORMANCE: 1,
        OpImplType.HIGH_PRECISION: 2,
        OpImplType.SUPER_PERFORMANCE: 3,
        OpImplType.SUPPORT_OUT_OF_BOUND_INDEX: 4,
        OpImplType.ENABLE_FLOAT_32: 5,
        OpImplType.ENABLE_HI_FLOAT_32: 6,
        OpImplType.KEEP_FP_16: 7,
    }

    impl_mode_list = []
    impl_mode_param = op_info.get(OpcOptions.IMPL_MODE, None)
    logger.info("impl_mode_param: [%s].", impl_mode_param)
    if not impl_mode_param:
        impl_mode_str = "p=" + str(impl_mode_dict.get(OpImplType.DEFAULT, 0))
        impl_mode_list.append(impl_mode_str)
    else:
        impl_mode_split = impl_mode_param.split(",", -1)
        for impl_mode in impl_mode_split:
            if impl_mode not in impl_mode_dict:
                logger.error("impl_mode_str: [%s] not supported.", impl_mode)
                continue
            impl_mode_str = str()
            impl_mode_str = "p=" + str(impl_mode_dict.get(impl_mode, 0))
            impl_mode_list.append(impl_mode_str)
            logger.info("impl_mode_str: [%s].", impl_mode_str)

        opt_impl_mode_flag = op_info.get("optional_impl_mode_flag", False)
        if opt_impl_mode_flag:
            impl_mode_str = str()
            impl_mode_str = "p=" + str(impl_mode_dict.get(OpImplType.DEFAULT, 0))
            impl_mode_list.append(impl_mode_str)

    logger.info("impl_mode_list :[%s].", str(impl_mode_list))
    return impl_mode_list


def get_normalized_dtype(dtype):
    dtype_mode_8_set = [
        OpDataType.DT_INT8,
        OpDataType.DT_UINT8,
        OpDataType.DT_BOOL,
        OpDataType.DT_FLOAT8_E8M0,
        OpDataType.DT_HIFLOAT8,
        OpDataType.DT_FLOAT8_E5M2,
        OpDataType.DT_FLOAT8_E4M3FN,
    ]
    logger.debug("Curent dtype is [%s]", str(dtype))
    if dtype in [
        OpDataType.DT_INT64,
        OpDataType.DT_UINT64,
        OpDataType.DT_DOUBLE,
        OpDataType.DT_COMPLEX64,
    ]:
        normalized_dtype = OpDataType.DT_INT64
    elif dtype in [
        OpDataType.DT_INT32,
        OpDataType.DT_UINT32,
        OpDataType.DT_FLOAT,
        OpDataType.DT_COMPLEX32,
    ]:
        normalized_dtype = OpDataType.DT_INT32
    elif dtype in [
        OpDataType.DT_INT16,
        OpDataType.DT_UINT16,
        OpDataType.DT_FLOAT16,
        OpDataType.DT_BF16,
    ]:
        normalized_dtype = OpDataType.DT_INT16
    elif dtype in dtype_mode_8_set:
        normalized_dtype = OpDataType.DT_INT8
    elif dtype in [OpDataType.DT_FLOAT6_E3M2, OpDataType.DT_FLOAT6_E2M3]:
        normalized_dtype = OpDataType.DT_FLOAT6_E3M2
    elif dtype in [OpDataType.DT_FLOAT4_E2M1, OpDataType.DT_FLOAT4_E1M2]:
        normalized_dtype = OpDataType.DT_FLOAT4_E2M1
    else:
        normalized_dtype = dtype
    logger.debug("normalized_dtype is [%s]", str(normalized_dtype))
    return normalized_dtype


def get_dtype_value(input_or_output, index):
    dtype = input_or_output.get(CompileParam.DTYPE)
    dtype_match_mode = input_or_output.get(CompileParam.DTYPE_MATCH_MODE)
    if dtype_match_mode == "DtypeByte":
        # priority dtype_match_mode > dtypeForBinQuery > dtype
        dtype_value = get_normalized_dtype(dtype)
    else:
        dtype_list = input_or_output.get(CompileParam.DTYPE_FOR_BIN_QUERY)
        if dtype_list is not None:
            dtype_value = dtype_list[index]
        else:
            dtype_value = dtype

    dtype_value = OpDataType.DtypeValueDict.get(dtype_value)
    return dtype_value


def get_format_value(input_or_output, index):
    format_match_mode = input_or_output.get(CompileParam.FORMAT_MATCH_MODE)
    if format_match_mode is not None:
        # priority format_match_mode > formatForBinQuery > format
        op_format_value = input_or_output.get(CompileParam.FORMAT)
    else:
        op_format_list = input_or_output.get(CompileParam.FORMAT_FOR_BIN_QUERY)
        if op_format_list is None:
            op_format_value = input_or_output.get(CompileParam.FORMAT)
        else:
            op_format_value = op_format_list[index]

    op_format_value = OpFormatType.FormatValueDict.get(op_format_value)
    return op_format_value


def check_dtype_num_is_same(input_or_output, dtype_num, dtype_list_num):
    dtype_match_mode = input_or_output.get(CompileParam.DTYPE_MATCH_MODE)
    if dtype_match_mode is None:
        dtype_list = input_or_output.get(CompileParam.DTYPE_FOR_BIN_QUERY)
        if dtype_list is not None:
            dtype_list_num = len(dtype_list)
    if dtype_num != 1 and dtype_list_num != 1 and dtype_list_num != dtype_num:
        logger.error(
            "current input dtype num is not same as previous input dtype num, won't generate simplifiedKey"
        )
        return False, dtype_num

    return True, dtype_list_num


def check_format_num_is_same(input_or_output, format_num, op_format_list_num):
    format_match_mode = input_or_output.get(CompileParam.FORMAT_MATCH_MODE)
    if format_match_mode is None:
        op_format_list = input_or_output.get(CompileParam.FORMAT_FOR_BIN_QUERY)
        if op_format_list is not None:
            op_format_list_num = len(op_format_list)

    if format_num != 1 and op_format_list_num != 1 and op_format_list_num != format_num:
        logger.error(
            "current input format num is not same as previous input format num, won't generate simplifiedKey"
        )
        return False, format_num

    return True, op_format_list_num


def check_format_dtype_num_is_same(
    dtype_num, format_num, dtype_list_num, op_format_list_num
):
    if (
        dtype_list_num != 1
        and op_format_list_num != 1
        and dtype_list_num != op_format_list_num
    ):
        logger.error(
            "current input format num is not same as dtype num, won't generate simplifiedKey"
        )
        return False, dtype_num, format_num

    if dtype_list_num > dtype_num:
        dtype_num = dtype_list_num

    if op_format_list_num > format_num:
        format_num = op_format_list_num

    return True, dtype_num, format_num


def check_single_input_or_output_is_valid(input_or_output, dtype_num, format_num):
    dtype_list_num, op_format_list_num = 1, 1
    res, dtype_list_num = check_dtype_num_is_same(
        input_or_output, dtype_num, dtype_list_num
    )
    if not res:
        return False, dtype_num, format_num

    res, op_format_list_num = check_format_num_is_same(
        input_or_output, format_num, op_format_list_num
    )
    if not res:
        return False, dtype_num, format_num

    res, dtype_num, format_num = check_format_dtype_num_is_same(
        dtype_num, format_num, dtype_list_num, op_format_list_num
    )
    if not res:
        return False, dtype_num, format_num

    return True, dtype_num, format_num


def check_in_and_out_is_valid(inputs_or_outputs):
    dtype_num, format_num = 1, 1
    for input_or_output in inputs_or_outputs:
        if input_or_output is None:
            continue
        if isinstance(input_or_output, list):
            for in_or_out in input_or_output:
                res, dtype_num, format_num = check_single_input_or_output_is_valid(
                    in_or_out, dtype_num, format_num
                )
                if not res:
                    return False, dtype_num, format_num
        else:
            res, dtype_num, format_num = check_single_input_or_output_is_valid(
                input_or_output, dtype_num, format_num
            )
            if not res:
                return False, dtype_num, format_num

    return True, dtype_num, format_num


def generate_input_or_output_for_simpilified_key(
    inputs_or_outputs,
    simplified_key_mode,
    optional_input_mode,
    dynamic_param_mode,
    index,
):
    inputs_or_outputs_str = str()
    dynamic_param_count = 0

    for input_or_output in inputs_or_outputs:
        # for dynamic input/output, only get the first one
        if isinstance(input_or_output, list):
            dynamic_param_count = len(input_or_output)
            input_or_output = input_or_output[0]

        if input_or_output is None:
            logger.warn("null input or output")
            inputs_or_outputs_str = inputs_or_outputs_str + "/,"
            continue

        logger.debug("input_or_output [%s].", str(input_or_output))

        dtype_value = get_dtype_value(input_or_output, index)
        op_format_value = get_format_value(input_or_output, index)

        param_type = input_or_output.get(CompileParam.PARAM_TYPE)
        if param_type == OpParamType.OPT and (
            simplified_key_mode == 0
            or (simplified_key_mode == 1 and optional_input_mode == "gen_placeholder")
        ):
            logger.info("paramType is optional, don't need generate input/output str.")
            continue

        inputs_or_outputs_str = (
            inputs_or_outputs_str + "/" + str(dtype_value) + "," + str(op_format_value)
        )
        if (
            simplified_key_mode == 1
            and dynamic_param_mode == "unfolded"
            and dynamic_param_count != 0
        ):
            inputs_or_outputs_str = (
                inputs_or_outputs_str + "," + str(dynamic_param_count)
            )

    return inputs_or_outputs_str


def generate_simplified_key(
    opc_compile_args, op, op_info, deterministic, simplified_key_mode
):
    """
    generate simplified key from op info
    """
    simplified_key = []
    op_type = op_info.get(CompileParam.OP_TYPE)
    if op_type is None:
        logger.warn("op_type is null, will not generate simplified key.")
        return simplified_key, simplified_key_mode

    deterministic_list = generate_deterministic_for_simpilified_key(deterministic)
    if len(deterministic_list) == 0:
        logger.warn("deterministic_list is null, will not generate simplified key.")
        return simplified_key, simplified_key_mode

    impl_mode_list = generate_impl_mode_for_simpilified_key(op_info)
    if len(impl_mode_list) == 0:
        logger.warn("impl_mode_list is null, will not generate simplified key.")
        return simplified_key, simplified_key_mode

    # simplified key is defined by Op, add op_type and deterministic
    op_simplified_key = op.get(CompileParam.SIMPLE_KEY)
    if op_simplified_key is not None and op_simplified_key != "":
        simplified_key_mode = 2
        simplified_key = generate_custom_mode_simplified_key(
            op_type, op_simplified_key, deterministic_list, impl_mode_list
        )

        return simplified_key, simplified_key_mode

    if opc_compile_args.get(OpcOptions.SIMPLE_KEY_MODE) == 0:
        attr_str = str()
        has_invalid_option = False
        has_non_null_attr = False
        logger.info("simple_key_mode is 0, ignore attr values")
    else:
        has_invalid_option, has_non_null_attr, attr_str = (
            check_attr_for_simpilified_key(op.get(CompileParam.ATTRS))
        )

    if has_invalid_option and opc_compile_args.get(OpcOptions.SIMPLE_KEY_MODE) != 0:
        simplified_key_mode = None
        return simplified_key, simplified_key_mode

    has_invalid_option, has_dynamic_param, has_optional_input, _ = (
        check_input_and_output_for_simplified_key(op)
    )
    if has_invalid_option:
        simplified_key_mode = None
        return simplified_key, simplified_key_mode
    res_tuple = generate_simplified_key_mode(
        opc_compile_args, has_non_null_attr, has_dynamic_param, has_optional_input
    )
    simplified_key_mode = res_tuple[0]
    simlified_info_tuple = (deterministic_list, impl_mode_list, attr_str)
    simplified_key = generate_simplified_key_str(
        res_tuple, simlified_info_tuple, op, op_type
    )

    return simplified_key, simplified_key_mode
