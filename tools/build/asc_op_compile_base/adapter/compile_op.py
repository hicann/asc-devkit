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
compile operator
"""

import os
import stat
import subprocess
import copy
import json
import hashlib
import re
import shutil
import sys
import struct
from tbe.tvm.contrib.ccec import CCECInfo
from tbe.tvm.runtime.cce_runtime import tvm_callback_cce_postproc
from tbe.common.buildcfg import get_current_build_config
from tbe.common.platform.platform_info import get_soc_spec, set_soc_spec
from tbe.tvm.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from tbe.tvm import var
from tbe.common.context import get_context
from .get_op_tiling import (
    TilingInfo,
    is_static_shape,
    OpInfo,
    get_tiling_info_by_tiling,
)
from .log_utils import LogUtil, AscendCLogLevel, CompileStage, COMPILE_STAGE_MSG_INFO
from .global_storage import global_var_storage
from .ascendc_constants import (
    InferChannelParamsFromIFile,
    InferChannelParams,
    KernelMetaType,
    CompileOptionTuple,
    CORE_TYPE_MIX,
    CORE_TYPE_CUBE,
    CORE_TYPE_VEC,
    TILING_KEY_MACRO,
    ASCENDC_OOM,
    MIX_CORE_MACRO,
    CustomizedConfig,
)
from .ascendc_common_utility import (
    CommonUtility,
    CompileCommandMode,
    CompileCommandSession,
    CompileInfo,
    KernelCompileCommand,
    SkBindCommand,
    process_ascendc_api_version,
    gen_func_align_attribute,
    convert_customized_config_to_inferchannel,
    get_kernel_fun_name_with_tiling_key_and_kernel_type,
)
from .ascendc_compile_dfx import (
    DFXParamType,
    DFXPointType,
    DFXArgInfo,
    DFXSectionGenerator,
)
from .ascendc_compile_v220 import (
    gen_compile_cmd_v220,
    get_compile_core_types,
    get_compile_target_name,
    get_compile_target_options,
    get_v220_kernel_type_mix_flag,
    call_bisheng_v220,
    get_ktype_section_variable,
    get_code_channel_v220_by_first_tiling_key,
    set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group,
    gen_compile_cmd_for_meta_info,
)
from .ascendc_compile_v200 import call_bisheng_v200_static, call_bisheng_v200_dynamic
from .ascendc_compile_gen_code import (
    get_code_for_l2_cache,
    gen_usr_origin_kernel_function_call,
    gen_template_tiling_params,
    add_time_stamp_codes,
    gen_init_dump_code,
    add_op_param_to_workspace,
    _gen_compile_cmd,
    get_tiling_key_struct_size_map,
    gen_tiling_struct_and_dfx_section_head,
    gen_tiling_struct_size_for_group_key,
    gen_dfx_section_for_one_tiling_key_dynamic,
    gen_dfx_section_for_one_tiling_key_static,
    gen_tiling_struct_size_for_group_key_no_size,
    gen_global_isolation_macro,
)
from .ascendc_compile_gen_json import (
    _gen_mix_json_from_seperate_json,
    _gen_mix_json_from_seperate_json_for_kernel_type,
    _dynamic_kernel_list_to_json,
    _dynamic_regbase_kernel_list_to_json,
    _static_regbase_kernel_list_to_json,
    _gen_mix_sub_json,
    _gen_static_json_for_no_mix_v200,
    _gen_non_mix_sub_json,
    _gen_static_json_for_mix_v200,
    _gen_dynamic_json_for_v200,
    _generate_final_json,
    _get_simt_type_in_staic,
)
from .ascendc_compile_base import (
    compile_multi_tilingkey,
    link_relocatable,
    fatbin_objs,
    SingleTilingKeyCompileParams,
    get_actual_kernel_type,
    compile_pre_process,
    link_relocatable_meta_file,
    link_sk_norm_combine,
)
from .super_kernel_sub_op_compile import (
    gen_sub_kernel_name,
    split_sub_kernel_objs,
    gen_sub_super_kernel_compile_options,
    add_sub_super_kernel_info,
)
from .static_compile_resource_generator import (
    KernelSpecCompilation,
)
from .super_kernel_constants import SuperKernelStreamFusionMode
from .super_kernel_option_parse import parse_super_kernel_options
from .kernel_info_infer import KernelInfoInfer
from .ascendc_compile_utils import check_custom_dcci_end_false, check_if_gen_placehoder
from .ascendc_kernel_feature_manager import global_ascendc_kernel_feature_manager

DEFAULT_TILING_KEY = "0"
COMPILE_INFO_KEY = "compileInfo"
GEN_PLACE_HOLDER_STR = "gen_placeholder"
TILING_KEY_SEARCH_KEYWORD = (
    "Contents of section"  # used in new tiling to search tiling section lines
)


def _set_compile_info(op_info: OpInfo, value_depends: dict = None):
    """set compile info in order to let AOE tools set tune params into compile info
    only support static shape ops
    """
    context = get_context()
    if is_static_shape(
        op_info.inputs, op_info.outputs, value_depends, op_info.param_type_list
    ):
        from tbe.common.tiling import BANK_CACHE

        if BANK_CACHE is not None and len(BANK_CACHE) != 0:
            tiling = context.get_addition("tune_param")
            if tiling is None:
                from tbe.common.utils.create_kb_query_key import (
                    get_op_compile_unique_key,
                )
                from tbe.common.repository_manager.interface import cann_kb_search

                info_dict = get_op_compile_unique_key(
                    op_info.op_type,
                    op_info.inputs,
                    op_info.outputs,
                    op_info.attrs,
                    op_info.impl_mode,
                    False,
                )
                tiling = cann_kb_search(
                    info_dict,
                    search_config={"op_type": op_info.op_type, "full_info": True},
                    option={},
                )
            if tiling is not None:
                context.add_compile_info("tune_param", tiling)


def _infer_name(key, sub_operater_infos, chip_version):
    if key == "stream":
        if (
            sub_operater_infos["sub_operator_kernel_type"] == "KERNEL_TYPE_AIV_ONLY"
            or sub_operater_infos["sub_operator_kernel_type"]
            == "KERNEL_TYPE_MIX_AIV_1_0"
        ):
            name = f"dav-{chip_version}-vec"
        elif (
            sub_operater_infos["sub_operator_kernel_type"] == "KERNEL_TYPE_MIX_AIC_1_1"
            or sub_operater_infos["sub_operator_kernel_type"]
            == "KERNEL_TYPE_MIX_AIC_1_2"
        ):
            name = f"dav-{chip_version}-mix"
        else:
            name = f"dav-{chip_version}-cube"
    else:
        name = "aicore"
    return name


def _update_super_dfx_info(name, chip_version, sub_dfx_info, super_dfx_info):
    if name == f"dav-{chip_version}-mix":
        name_list = [f"dav-{chip_version}-vec", f"dav-{chip_version}-cube"]
        for sub_name in name_list:
            if sub_name in super_dfx_info and isinstance(
                super_dfx_info[sub_name], list
            ):
                super_dfx_info[sub_name].append(sub_dfx_info)
            else:
                super_dfx_info[sub_name] = [sub_dfx_info]
    else:
        if name in super_dfx_info and isinstance(super_dfx_info[name], list):
            super_dfx_info[name].append(sub_dfx_info)
        else:
            super_dfx_info[name] = [sub_dfx_info]


def _json_except_info(compile_info: CompileInfo):
    super_dfx_info = {}
    super_dfx_list = {}
    key = "aicore"
    chip_version = CommonUtility.get_chip_version()
    if "stream-fusion" in compile_info.super_kernel_info["sp_options"]:
        stream_fusion = compile_info.super_kernel_info["sp_options"]["stream-fusion"]
        if stream_fusion.value == SuperKernelStreamFusionMode.StreamFusionEnable.value:
            key = "stream"
    i = 0
    for sub_op in compile_info.super_kernel_info["op_list"]:
        sub_json_path = sub_op.get("json_path")
        sub_dfx_info = {}
        arg_list = {}
        with open(sub_json_path, "r") as fd:
            sub_operater_infos = json.load(fd)
            sub_dfx_info["func_name"] = sub_operater_infos["kernelName"]
            sub_dfx_info["split_mode"] = sub_operater_infos.get("split_mode")
            sub_dfx_info["blockNum"] = sub_operater_infos["blockDim"]
            sub_dfx_info["sub_operator_op_type"] = sub_operater_infos.get(
                "sub_operator_op_type", ""
            )
            sub_dfx_info["sub_operator_kernel_type"] = sub_operater_infos[
                "sub_operator_kernel_type"
            ]
            sub_dfx_info["sub_operator_early_start_set_flag"] = sub_operater_infos[
                "sub_operator_early_start_set_flag"
            ]
            sub_dfx_info["sub_operator_early_start_wait_flag"] = sub_operater_infos[
                "sub_operator_early_start_wait_flag"
            ]
            sub_dfx_info["sub_operator_call_dcci_before_kernel_start"] = (
                sub_operater_infos.get(
                    "sub_operator_call_dcci_before_kernel_start", False
                )
            )
            sub_dfx_info["sub_operator_call_dcci_after_kernel_end"] = (
                sub_operater_infos.get("sub_operator_call_dcci_after_kernel_end", False)
            )
            sub_dfx_info["sub_operator_call_dcci_disable_on_kernel"] = (
                sub_operater_infos.get(
                    "sub_operator_call_dcci_disable_on_kernel", False
                )
            )
            sub_dfx_info["streamid"] = sub_op.get("stream_id")
            sub_dfx_info["send_event_list"] = compile_info.super_kernel_info[
                "send_event_list"
            ][i]
            sub_dfx_info["recv_event_list"] = compile_info.super_kernel_info[
                "recv_event_list"
            ][i]
            arg_list["param_offset"] = compile_info.super_kernel_info["param_offset"][i]
            if compile_info.super_kernel_info["send_event_list"][i]:
                arg_list["notify_param_offset"] = compile_info.super_kernel_info[
                    "notify_param_offset"
                ][i]
            else:
                arg_list["notify_param_offset"] = None
            if compile_info.super_kernel_info["recv_event_list"][i]:
                arg_list["wait_param_offset"] = compile_info.super_kernel_info[
                    "wait_param_offset"
                ][i]
            else:
                arg_list["wait_param_offset"] = None
            sub_dfx_info["arg_list"] = arg_list
            if "debugOptions" in sub_operater_infos:
                sub_dfx_info["debug_option"] = sub_operater_infos["debugOptions"]
                sub_dfx_info["debug_size"] = sub_operater_infos["debugBufSize"]
            name = _infer_name(key, sub_operater_infos, chip_version)
        _update_super_dfx_info(name, chip_version, sub_dfx_info, super_dfx_info)
        i += 1
    super_dfx_list["kernelList"] = super_dfx_info
    return super_dfx_list


def _init_param_value(op_info: OpInfo, tiling_info: TilingInfo, js):
    if op_info.init_value_list is not None:
        # generate clear output for atomic instrs
        param_of_init_values = [None for _ in op_info.inputs]
        if tiling_info.clear_atomic:
            for output, init_value in zip(op_info.outputs, op_info.init_value_list):
                if init_value is not None:
                    if init_value.isdigit():
                        # generate init value for InitValue(uint64_t)
                        param_init_value = {
                            "dtype": output["dtype"],
                            "init_value": int(init_value),
                        }
                    else:
                        try:
                            init_value_json = json.loads(init_value)
                            # generate init value for InitValue(std::vector<ScalarVar>)
                            if init_value_json["is_list"]:
                                param_init_value = {
                                    "dtype": init_value_json[output["dtype"]]["type"],
                                    "init_value": init_value_json[output["dtype"]][
                                        "value"
                                    ],
                                }
                            else:
                                #  generate init value for InitValue(ScalarVar)
                                param_init_value = {
                                    "dtype": init_value_json["type"],
                                    "init_value": init_value_json["value"],
                                }
                        except Exception as err:
                            raise_tbe_python_err(
                                TBE_DEFAULT_PYTHON_ERROR_CODE,
                                ("read initValue error, reason is:", err),
                            )
                    param_of_init_values.append(param_init_value)
                else:
                    param_of_init_values.append(None)
        else:
            param_of_init_values += [None for _ in op_info.outputs]
        # generate null for workspace
        param_of_init_values.append(None)
        js["parameters"] = param_of_init_values


def _json_post_process(
    compile_info: CompileInfo,
    op_info: OpInfo,
    tiling_info: TilingInfo,
    input_gen_placehoder: bool,
    output_gen_placehoder: bool,
    compile_log_path,
):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    json_path = os.path.join(kernel_meta_path, compile_info.kernel_name + ".json")
    obj_path = os.path.join(kernel_meta_path, compile_info.kernel_name + ".o")

    try:
        with open(json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    if input_gen_placehoder:
        js["optionalInputMode"] = GEN_PLACE_HOLDER_STR
    if output_gen_placehoder:
        js["optionalOutputMode"] = GEN_PLACE_HOLDER_STR
    if compile_info.enable_deterministic:
        if get_current_build_config("enable_deterministic_mode") == 1:
            js["deterministic"] = "true"
        else:
            js["deterministic"] = "false"

    superkernel_black_op_list = [
        "MoeInitRoutingV3",
        "MoeInitRoutingV2",
        "MoeInitRoutingQuant",
    ]
    if op_info.op_type not in superkernel_black_op_list or not CommonUtility.is_v220():
        js["supportSuperKernel"] = 1
    else:
        CommonUtility.print_compile_log(
            compile_info.kernel_name,
            f"The operator {op_info.op_type} has not been adapted to superkernel. Therefore, \
the superkernel cannot be integrated with the operator.",
            AscendCLogLevel.LOG_WARNING,
        )

    if CommonUtility.is_c310():
        if tiling_info.local_memory_size > 0 or _get_simt_type_in_staic(
            tiling_info, compile_info, obj_path
        ):
            js["supportSuperKernel"] = 0
            CommonUtility.print_compile_log(
                compile_info.kernel_name,
                f"The current soc version does not support merging simt type operator:{op_info.op_type} \
    into superkernel",
                AscendCLogLevel.LOG_INFO,
            )

    def _safe_int_conversion(value, default=-1):
        if value is None:
            return default
        clean_value = str(value).strip()
        try:
            return int(clean_value)
        except (ValueError, TypeError):
            return default

    aicore_num = get_context().get_addition("_op_aicore_num")
    vectorcore_num = get_context().get_addition("_op_vectorcore_num")
    has_platform_info = False
    if aicore_num is not None and vectorcore_num is not None:
        js["platformInfo"] = {
            "cubeCoreCnt": int(aicore_num),
            "vectorCoreCnt": int(vectorcore_num),
        }
        has_platform_info = True
    deterministic_level = _safe_int_conversion(
        get_current_build_config("deterministic_level")
    )
    if deterministic_level != -1:
        if not has_platform_info:
            js["platformInfo"] = {}
        js["platformInfo"]["deterministicLevel"] = int(deterministic_level)
    pcie_through = get_context().get_addition("pcie_through_flag")
    if pcie_through is not None:
        if not has_platform_info:
            js["platformInfo"] = {}
        js["platformInfo"]["pcie_through_flag"] = pcie_through

    # set tilingdata of mc2 operator when online static compile
    if (
        tiling_info.static_shape_flag is True
        and op_info.mc2_ctx is not None
        and len(op_info.mc2_ctx) != 0
    ):
        js["runInfo"] = tiling_info.raw_run_info

    # gen sub operator infos for super kernel feature
    js = add_sub_super_kernel_info(js, tiling_info.static_shape_flag, compile_info)

    if compile_info.super_kernel_info.get(
        "timestamp_option"
    ) is not None and compile_info.super_kernel_info.get("timestamp_option"):
        del js["workspace"]
        js["debugOptions"] = compile_info.super_kernel_info["debug_option"]
        js["debugBufSize"] = compile_info.super_kernel_info["debug_size"]

    if (
        compile_info.super_kernel_info.get("workspace_size") is not None
        and compile_info.super_kernel_info.get("workspace_size") > 0
    ):
        js["workspace"] = {
            "num": 1,
            "size": [compile_info.super_kernel_info.get("workspace_size")],
            "type": [0],
        }

    # get max tiling size when use tiling new
    if len(compile_info.tiling_key_struct_map) > 0:
        max_tiling_size = _get_tiling_struct_size(compile_info)
    # get max tiling size without register tiling
    elif global_var_storage.get_variable("ascendc_tiling_no_register"):
        max_tiling_size = compile_info.max_tiling_size
        delete_tiling_section(compile_info)
    # get max tiling size when use tiling old
    else:
        max_tiling_size = tiling_info.tiling_data_size
    compile_info.max_tiling_size = int(max_tiling_size)

    # updata op_param size by flag of oom
    if "oom" in get_current_build_config("tir.op_debug_config"):
        # tiling need align to 8 bytes, dfx need 8 bytes for dfx point,
        # oom need allocate 8 * (input + output + shape_tensor+ workspace)
        op_param_size = (
            ((max_tiling_size + 7) // 8) * 8
            + 8
            + 8 * DFXSectionGenerator().param_placeholder_num
        )
    else:
        op_param_size = max_tiling_size + 8

    js["opParaSize"] = int(op_param_size)

    if COMPILE_INFO_KEY not in js:
        js[COMPILE_INFO_KEY] = {}
    if tiling_info.static_shape_flag:
        del js[COMPILE_INFO_KEY]
        # generate schedule_mode for static shape
        if tiling_info.static_shape_flag and tiling_info.schedule_mode != 0:
            js["schedule_mode"] = tiling_info.schedule_mode
    if tiling_info.local_memory_size != -1:
        js["localMemorySize"] = tiling_info.local_memory_size
    if "param_type_dynamic" in op_info._fields and op_info.param_type_dynamic:
        js["dynamicParamMode"] = "folded_with_desc"

    _init_param_value(op_info, tiling_info, js)

    if compile_info.super_kernel_info.get("kernel_name") is not None:
        js["SuperkernelInfo"] = _json_except_info(compile_info)
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        js["feature_list"] = (
            global_ascendc_kernel_feature_manager.get_available_feature_versions()
        )

    try:
        with open(obj_path, "rb") as obj_file:
            js["sha256"] = hashlib.sha256(obj_file.read()).hexdigest()
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read obj_file failed, reason is:", err)
        )
    try:
        with open(json_path, "w") as fd_write:
            os.chmod(json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _gen_kernel_func_declare_head_with_workspace(
    tiling_info: TilingInfo, super_kernel_params, func_params
):
    dfx_generator = DFXSectionGenerator()
    # static shape do not have tiling
    if CommonUtility.is_v100() or CommonUtility.is_v200():
        if tiling_info.static_shape_flag:
            func_params.append("GM_ADDR workspace")
            func_params.append("GM_ADDR overflowStatus")
        else:
            func_params.append("GM_ADDR workspace")
            func_params.append("GM_ADDR tiling")
            func_params.append("GM_ADDR overflowStatus")
            dfx_generator.insert_param(DFXArgInfo("tiling", DFXParamType.TILING))
    else:
        if tiling_info.static_shape_flag:
            func_params.append("GM_ADDR workspace")
            super_kernel_params.append("workspace")
        else:
            func_params.append("GM_ADDR workspace")
            func_params.append("GM_ADDR tiling")
            super_kernel_params.append("workspace")
            super_kernel_params.append("tiling")
            dfx_generator.insert_param(DFXArgInfo("tiling", DFXParamType.TILING))
    return super_kernel_params, func_params


def _gen_kernel_func_declare_head(
    is_mix: bool,
    is_single_and_using_hard_sync: bool,
    opinfo: OpInfo,
    tiling_info: TilingInfo,
):
    # generate kernel function
    source = ""
    dfx_generator = DFXSectionGenerator()
    func_params = []
    super_kernel_params = []
    needs_ffts = (is_mix or is_single_and_using_hard_sync) and not (
        CommonUtility.is_c310() or CommonUtility.is_m510()
    )
    workspace_idx = 0
    if needs_ffts:
        func_params.append("GM_ADDR ffts_addr")
        workspace_idx += 1
        super_kernel_params.append("ffts_addr")
        dfx_generator.insert_param(DFXArgInfo("ffts", DFXParamType.FFTS))

    if opinfo.mc2_ctx:
        for ctx_name in opinfo.mc2_ctx:
            func_params.append("GM_ADDR {}".format(ctx_name))
            workspace_idx += 1
            super_kernel_params.append(str(ctx_name))
            dfx_generator.insert_param(DFXArgInfo(ctx_name, DFXParamType.MC2CTX))

    for input in opinfo.inputs:
        if input is None:
            continue
        func_params.append("GM_ADDR {}".format(input["param_name"]))
        workspace_idx += 1
        super_kernel_params.append(input["param_name"])
        dfx_generator.insert_param(DFXArgInfo(input["param_name"], DFXParamType.INPUT))

    for output in opinfo.outputs:
        if output is None:
            continue
        func_params.append("GM_ADDR {}".format(output["param_name"]))
        workspace_idx += 1
        super_kernel_params.append(output["param_name"])
        dfx_generator.insert_param(
            DFXArgInfo(output["param_name"], DFXParamType.OUTPUT)
        )

    if (
        opinfo.output_shape_depend_on_compute is not None
        and len(opinfo.output_shape_depend_on_compute) > 0
    ):
        func_params.append("GM_ADDR __ascendc_output_shape")
        workspace_idx += 1
        super_kernel_params.append("__ascendc_output_shape")
        dfx_generator.insert_param(
            DFXArgInfo("shape_tensor", DFXParamType.SHAPE_TENSOR)
        )
        # modify point type for OutputShapeDependOnCompute output
        for index in opinfo.output_shape_depend_on_compute:
            parameter: DFXArgInfo = dfx_generator.get_param(
                opinfo.outputs[index]["param_name"]
            )
            parameter.point_type = DFXPointType.LEVEL_1_FOR_SHAPE_TENSOR
        # for static shape, set size to max value, len(OutputShapeDependOnCompute output) * mix dim(8) * uint64_t(8)
        if tiling_info.static_shape_flag:
            dfx_generator.set_size_of_dfx_info(
                "shape_tensor", len(opinfo.output_shape_depend_on_compute) * 8 * 8
            )

    # dynamic: must add workspace, static: if workspace_size >= 0 add workspace
    if not tiling_info.static_shape_flag or tiling_info.static_workspace_size >= 0:
        dfx_generator.insert_param(DFXArgInfo("workspace", DFXParamType.WORKSPACE))

    super_kernel_params, func_params = _gen_kernel_func_declare_head_with_workspace(
        tiling_info, super_kernel_params, func_params
    )
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_params", super_kernel_params
        )
        context = get_context()
        if (
            context is None
            or context.get_addition("super_kernel_sub_combine") is not True
        ):
            called_func_params = "args_offset"
            called_func_params_type = "uint64_t args_offset"
            source += "uint64_t args_offset) {\n"
            source += "    GM_ADDR *param_base = (GM_ADDR *)get_para_base();\n"
            for param in func_params:
                source += f"    {param} = param_base[args_offset++];\n"
        else:
            called_func_params = "param, sargs"
            called_func_params_type = "__gm__ uint64_t *param, sk::SkSystemArgs *sargs"
            source += "__gm__ uint64_t *param, sk::SkSystemArgs *sargs) {\n"
            source += (
                "    g_super_kernel_early_start_config = sargs->SkGetTaskSyncCfg();\n"
            )
            source += "    uint32_t __asc_index = 0;\n"
            for param in func_params:
                source += f"    {param} = (GM_ADDR)param[__asc_index++];\n"

    else:
        source += ", ".join(func_params) + ") {\n"
        called_func_params_type = ", ".join(func_params)
        called_func_params = called_func_params_type.replace("GM_ADDR ", "")
    return source, workspace_idx, called_func_params, called_func_params_type


def _gen_set_workspace_codes(
    is_mix: bool,
    is_single_and_using_hard_sync: bool,
    opinfo: OpInfo,
    tiling_info: TilingInfo,
    compile_options: list,
    compile_info: CompileInfo,
):
    # set workspace
    source = ""
    if global_var_storage.get_variable("ascendc_enable_dump_workspace") is True or (
        not CommonUtility.is_support_workspace_offset()
    ):
        source += "    GM_ADDR usrWorkspace = AscendC::GetUserWorkspace(workspace);\n"
    else:
        source += (
            "#if ENABLE_CV_COMM_VIA_SSBUF != 0 && __MIX_CORE_AIC_RATION__ != 1\n"
            "    GM_ADDR usrWorkspace = workspace;\n"
            "#else\n"
            "    GM_ADDR usrWorkspace = workspace + AscendC::RESERVED_WORKSPACE;\n"
            "#endif\n"
        )
    if "oom" in get_current_build_config("tir.op_debug_config"):
        source = add_op_param_to_workspace(
            opinfo, tiling_info, source, compile_options, compile_info
        )

    needs_ffts = (
        is_mix or is_single_and_using_hard_sync
    ) and not CommonUtility.is_c310()
    # set ffts_addr for ascend910b mix op or is_single_and_using_hard_sync scene
    if needs_ffts:
        source += "    icache_preload(1);\n"
        source += "    if (ffts_addr != nullptr) {\n"
        source += "        set_ffts_base_addr((uint64_t)ffts_addr);\n"
        source += "    }\n"

    # restart enable begin position
    if global_var_storage.get_variable("ascendc_enable_aicore_exception_restart"):
        source += "do {\n"

    # is_single_and_using_hard_sync scene not need clear workspace
    if is_mix and (not CommonUtility.is_c310()):  # c310 doesn't need clearWorkspace
        source += f"#ifdef {MIX_CORE_MACRO} \n"
        source += "    if constexpr (g_coreType == AscendC::AIC) {\n"
        source += "        matmul::clearWorkspace(workspace);\n"
        source += "    }\n"
        source += "#endif\n"
    return source


def _gen_set_mc2_ctx_param(opinfo: OpInfo):
    if opinfo.mc2_ctx is None:
        return ""
    source = ""
    index = 0
    for ctx_name in opinfo.mc2_ctx:
        source += f"    AscendC::SetHcclContext<{index}>({ctx_name});\n"
        index += 1
    return source


# When TPL struct is not registered through macro REGISTER_TILING, need to insert section code
def _gen_tpl_tiling_struct_section(compile_info: CompileInfo, tiling_info: TilingInfo):
    source = gen_global_isolation_macro(compile_info, tiling_info)
    tiling_struct_set = set()
    counter = 0
    for tiling_key in compile_info.tiling_key_list:
        original_tiling_struct = compile_info.tiling_key_struct_map[tiling_key]
        if (
            original_tiling_struct in compile_info.tpl_tiling_struct
            and original_tiling_struct not in compile_info.register_tiling_struct
            and original_tiling_struct not in tiling_struct_set
        ):
            source += f"static const uint64_t __ascendc_TPL_tiling_struct_{counter} __attribute__"
            source += f'((used, section(".ascendc_tiling.{original_tiling_struct}"))) = sizeof({original_tiling_struct});\n'
            counter += 1
        tiling_struct_set.add(original_tiling_struct)
    source += "#endif\n\n"
    return source


def gen_meta_info_section(compile_info, op_info):
    meta_info = {}
    section_var = ""
    out_file = compile_info.gen_kernel_func_file
    kernel_name = op_info.kernel_name

    # version
    section_var += f"static const struct BinaryMetaVersion {kernel_name}_kernel_metainfo_version_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += " {{B_TYPE_BIN_VERSION_INFO, sizeof(unsigned int)}, 0x01};\n"

    # debug
    debug_options = 0
    debug_buf_size = 0
    debug_options_table = {
        "printf": 0x001,
        "dumptensor": 0x001,
        "assert": 0x002,
        "timestamp": 0x004,
        "oom": 0x008,
    }

    if "oom" in get_current_build_config("tir.op_debug_config"):
        debug_options |= debug_options_table["oom"]
    section_var += f"static const struct BinaryMetaDebug {kernel_name}_kernel_metainfo_debug_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += (
        f" {{{{B_TYPE_DEBUG_INFO, 8}}, {debug_buf_size}, {debug_options}}};\n"
    )

    # dynamicparam
    dynamic_param = (
        1
        if "param_type_dynamic" in op_info._fields and op_info.param_type_dynamic
        else 0
    )
    section_var += "static const struct BinaryMetaDynamicParam "
    section_var += f"{kernel_name}_kernel_metainfo_dynamicparam_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += f" {{{{B_TYPE_DYNAMIC_PARAM, 4}}, 0, {dynamic_param}}};\n"

    # optinalparam
    optional_input_mode = 1 if check_if_gen_placehoder(op_info, True) else 0
    optional_output_mode = 1 if check_if_gen_placehoder(op_info, False) else 0
    section_var += "static const struct BinaryMetaOptionalParam "
    section_var += f"{kernel_name}_kernel_metainfo_optionalparam_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += f" {{{{B_TYPE_OPTIONAL_PARAM, 4}}, {optional_input_mode}, {optional_output_mode}}};\n"

    global_var_storage.set_variable("ascendc_meta_info", section_var)


def gen_kernel_fun(
    compile_info: CompileInfo,
    func_name: str,
    opinfo: OpInfo,
    tiling_info: TilingInfo,
    compile_option_tuple,
):
    compile_options = compile_option_tuple.compile_options
    src_file = compile_info.src_file
    out_file = compile_info.gen_kernel_func_file

    file_name = os.path.basename(src_file)
    file_name_without_ext = os.path.splitext(file_name)[0]
    # begin generate code
    # File Isolation Macro
    source = f"#ifndef __{file_name_without_ext.upper()}__KERNEL_FUN_H__\n"
    source += f"#define __{file_name_without_ext.upper()}__KERNEL_FUN_H__\n\n"
    # replace __global micro for usr kernel function, and recover after usr kernel function
    source += "#undef __global__\n"
    source += "#define __global__ inline\n"
    source += f'#include "{src_file}"\n'
    source += '#include "kernel_common.h"\n'

    source += "#undef __global__\n"
    source += "#if ASCENDC_CPU_DEBUG\n"
    source += "#define __global__\n"
    source += "#else\n"
    source += "#define __global__ __attribute__((cce_kernel))\n"
    source += "#endif\n\n"

    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        source += gen_tiling_struct_size_for_group_key_no_size(compile_info)

    # add template_param
    source += gen_template_tiling_params(compile_info)

    is_mix, is_single_and_using_hard_sync = get_v220_kernel_type_mix_flag(
        compile_info, tiling_info
    )

    # generate code for l2 cache
    if (
        global_var_storage.get_variable("ascendc_enable_sanitizer") is False
        and global_var_storage.get_variable("ascendc_debug_compile_options") is False
        and global_var_storage.get_variable("ascendc_enable_super_kernel") is False
    ):
        if CommonUtility.is_v220() or CommonUtility.is_v200():
            source = get_code_for_l2_cache(compile_info, source, tiling_info)

    # generate kernel function
    auto_gen_kernel_func = f"auto_gen_{func_name}_kernel"

    gen_func_attributes = "__global__"
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        align_size = compile_info.super_kernel_info["sp_options"].get("func-align", 512)
        gen_func_attributes = gen_func_align_attribute(align_size)
        gen_func_attributes += " __sk__"
    else:
        gen_func_attributes += " [aicore]"

    kernel_func_dec = f'extern "C" {gen_func_attributes} void {auto_gen_kernel_func}('
    compile_info.global_kernel_attribute = gen_func_attributes
    kernel_func_dec_pub = f"__aicore__ inline __attribute__((always_inline)) void ascendc_{auto_gen_kernel_func}("

    source_declare_pub, workspace_idx, called_func_params, called_func_params_type = (
        _gen_kernel_func_declare_head(
            is_mix, is_single_and_using_hard_sync, opinfo, tiling_info
        )
    )
    source += kernel_func_dec_pub
    source += source_declare_pub
    source_declare_pub_fun = (
        f"ascendc_{auto_gen_kernel_func}(" + called_func_params + ");"
    )

    # init dump and system workspace
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is False:
        source += gen_init_dump_code()
        # set mc2 context
        source += _gen_set_mc2_ctx_param(opinfo)
        # implicit add aicore exception restart begin position
        # set workspace
        source += _gen_set_workspace_codes(
            is_mix,
            is_single_and_using_hard_sync,
            opinfo,
            tiling_info,
            compile_options,
            compile_info,
        )
    else:
        source += _gen_set_mc2_ctx_param(opinfo)
        source += "    AscendC::SetSysWorkspaceForce(workspace);\n"
        source += "    GM_ADDR usrWorkspace = AscendC::GetUserWorkspace(workspace);\n"

        # restart enable begin position
        if global_var_storage.get_variable("ascendc_enable_aicore_exception_restart"):
            source += "do {\n"

    need_ffts = is_mix or is_single_and_using_hard_sync
    context = get_context()
    enable_inner_core_sync_check = (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and context is not None
        and context.get_addition("super_kernel_sub_combine") is True
        and compile_info.super_kernel_info["sp_options"].get(
            "debug-per-op-max-core-num", "0"
        )
        == "1"
    )

    # call usr kernel function call
    if enable_inner_core_sync_check:
        source += "    AscendC::g_superKernelSetWaitFlagCountDifference = "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE;\n"
    source += "#if defined(TEMPLATE_PARAMS_LEN) && TEMPLATE_PARAMS_LEN != 0\n"
    source += gen_usr_origin_kernel_function_call(
        func_name, opinfo, tiling_info, has_template=True
    )
    source += "#else\n"
    source += gen_usr_origin_kernel_function_call(
        func_name, opinfo, tiling_info, has_template=False
    )
    source += "#endif\n"
    if enable_inner_core_sync_check:
        source += "    if (AscendC::g_superKernelSetWaitFlagCountDifference > "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE) {\n"
        source += '        assert(false, "SuperKernel: current operator has %d more '
        source += 'SetFlag calls than WaitFlag calls; Please check synchronization within the current operator.\\n", '
        source += "AscendC::g_superKernelSetWaitFlagCountDifference - "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE);\n"
        source += "    } else if (AscendC::g_superKernelSetWaitFlagCountDifference < "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE) {\n"
        source += '        assert(false, "SuperKernel: current operator has %d more '
        source += 'WaitFlag calls than SetFlag calls; Please check synchronization within the current operator.\\n", '
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE - "
        source += "AscendC::g_superKernelSetWaitFlagCountDifference);\n"
        source += "    }\n"

    if len(compile_info.tiling_key_struct_map) > 0:
        source += _gen_tpl_tiling_struct_section(compile_info, tiling_info)

    # aicore exception restart main block
    if global_var_storage.get_variable("ascendc_enable_aicore_exception_restart"):
        for key in tiling_info.tiling_key_list:
            source += f"#if {TILING_KEY_MACRO} == {key}UL"
            source += "\n"

            actual_kernel_type = get_actual_kernel_type(
                key, compile_info, need_ffts, opinfo.kernel_name
            )

            if actual_kernel_type == CORE_TYPE_CUBE:
                source += "    if ASCEND_IS_AIC {\n"
                source += "        AscendC::PipeBarrier<PIPE_ALL>();\n"
                source += "        AscendC::CrossCoreSetFlag<0, PIPE_FIX>(AscendC::SYNC_AIC_FLAG);\n"
                source += (
                    "        AscendC::CrossCoreWaitFlag(AscendC::SYNC_AIC_FLAG);\n"
                )
                source += "    }\n"
            elif actual_kernel_type == CORE_TYPE_VEC:
                source += "    AscendC::SyncAll();\n"
            elif actual_kernel_type == CORE_TYPE_MIX:
                source += "    AscendC::SyncAll<false>();\n"
            source += "#endif\n"

        ctx_num = 0
        if opinfo.mc2_ctx is not None:
            ctx_num = len(opinfo.mc2_ctx)
        source += f"    auto __ascendc_is_restart = AscendC::GetRestart({ctx_num});\n"
        source += "    if (__ascendc_is_restart > 0) {\n"
        source += "        AscendC::PipeBarrier<PIPE_ALL>();\n"
        source += "        dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE);\n"
        # add corresponding sync all by kernel type
        for key in tiling_info.tiling_key_list:
            source += f"#if {TILING_KEY_MACRO} == {key}UL"
            source += "\n"

            actual_kernel_type = get_actual_kernel_type(
                key, compile_info, need_ffts, opinfo.kernel_name
            )

            if actual_kernel_type == CORE_TYPE_CUBE:
                source += "        if ASCEND_IS_AIC {\n"
                source += "            AscendC::PipeBarrier<PIPE_ALL>();\n"
                source += "            AscendC::CrossCoreSetFlag<0, PIPE_FIX>(AscendC::SYNC_AIC_FLAG);\n"
                source += (
                    "            AscendC::CrossCoreWaitFlag(AscendC::SYNC_AIC_FLAG);\n"
                )
                source += "        }\n"
            elif actual_kernel_type == CORE_TYPE_VEC:
                source += "        AscendC::SyncAll();\n"
            elif actual_kernel_type == CORE_TYPE_MIX:
                source += "        AscendC::SyncAll<false>();\n"
            source += "#endif\n"
        source += f"        AscendC::SetRestart({ctx_num});\n"
        source += "    } else {\n"
        source += "        break;\n"
        source += "    }\n"
        source += "} while(1);\n"

    from tbe.common.buildcfg.buildcfg_mapping import status_check

    if get_current_build_config(status_check) and (
        CommonUtility.is_v200() or CommonUtility.is_v100()
    ):
        source += "    AscendC::WriteBackOverflow(overflowStatus);\n"

    if not global_var_storage.get_variable("ascendc_enable_super_kernel") and (
        CommonUtility.is_c310() or CommonUtility.is_m510()
    ):
        check_custom_dcci_end_false(compile_option_tuple)

    source += "}\n\n"
    source += kernel_func_dec
    source += called_func_params_type + ") {\n"
    source += "    " + source_declare_pub_fun + "\n"
    source += "}\n\n"

    for tiling_key in compile_info.tiling_key_list:
        if compile_info.tiling_key_group_map is not None:
            if tiling_key in compile_info.tiling_key_group_map.keys():
                source += gen_kernel_fun_with_tiling_key_slave(
                    compile_info,
                    tiling_key,
                    source_declare_pub_fun,
                    gen_func_attributes,
                    source_declare_pub,
                )
    source += "#endif\n"
    # write code into file
    try:
        with os.fdopen(
            os.open(
                out_file,
                os.O_TRUNC | os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(source)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            ("gen kernel func file failed, reason is:", err),
        )
    return workspace_idx


def gen_kernel_fun_with_tiling_key_slave(
    compile_info: CompileInfo,
    tiling_key: int,
    source_declare_pub_fun: str,
    gen_func_attributes: str,
    source_declare: str,
):
    source = ""
    if CommonUtility.is_v220() or CommonUtility.is_c310():
        chip_version = CommonUtility.get_chip_version().upper()
        cube_core_type = f"__DAV_{chip_version}_CUBE__"
        vec_core_type = f"__DAV_{chip_version}_VEC__"
    else:
        cube_core_type = "__DAV_M200__"
        vec_core_type = "__DAV_M200_VEC__"
    if compile_info.tiling_key_group_map is not None:
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                if compile_info.sub_core_type == CORE_TYPE_CUBE:
                    source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({cube_core_type})\n"
                elif compile_info.sub_core_type == CORE_TYPE_VEC:
                    source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({vec_core_type})\n"
                if tiling_key not in compile_info.tiling_key_kernel_type.keys():
                    raise Exception(f"kernel type of tiling key {tiling_key} not found")
                else:
                    kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
                    if kernel_type in [
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
                    ]:
                        source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({cube_core_type})\n"
                        kernel_name_of_tk = (
                            get_kernel_fun_name_with_tiling_key_and_kernel_type(
                                compile_info, tiling_key_slave
                            )
                        )
                        kernel_func_dec = f'extern "C" {gen_func_attributes} void {kernel_name_of_tk}('
                        source += kernel_func_dec
                        if (
                            global_var_storage.get_variable(
                                "ascendc_enable_super_kernel"
                            )
                            is True
                        ):
                            source += "uint64_t args_offset) {\n"
                        else:
                            source += source_declare
                        source += f"    {source_declare_pub_fun}\n"
                        source += "}\n"
                        source += "#endif\n"
                        source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({vec_core_type})\n"
                        kernel_name_of_tk = (
                            get_kernel_fun_name_with_tiling_key_and_kernel_type(
                                compile_info, tiling_key_slave
                            )
                        )
                        kernel_name_of_tk = kernel_name_of_tk[:-1] + "v"
                        kernel_func_dec = f'extern "C" {gen_func_attributes} void {kernel_name_of_tk}('
                        source += kernel_func_dec
                        if (
                            global_var_storage.get_variable(
                                "ascendc_enable_super_kernel"
                            )
                            is True
                        ):
                            source += "uint64_t args_offset) {\n"
                        else:
                            source += source_declare
                        source += f"    {source_declare_pub_fun}\n"
                        source += "}\n"
                        source += "#endif\n"
                    else:
                        raise Exception(
                            f"unsupported kernel type {kernel_type} for tiling key {tiling_key}"
                        )
    return source


def gen_tiling_struct_size_and_dfx_section_file(
    compile_info: CompileInfo, tiling_info: TilingInfo, tiling_key_struct_size_map: dict
):
    out_file = compile_info.tiling_and_dfx_utils_file
    source = gen_tiling_struct_and_dfx_section_head()
    source += gen_tiling_struct_size_for_group_key(
        compile_info, tiling_key_struct_size_map
    )

    if tiling_info.static_shape_flag:
        source += gen_dfx_section_for_one_tiling_key_static(
            compile_info,
            tiling_info.tiling_key,
            tiling_info,
            tiling_key_struct_size_map,
        )
        if compile_info.tiling_key_group_map is not None:
            if tiling_info.tiling_key in compile_info.tiling_key_group_map.keys():
                for tiling_key_slave in compile_info.tiling_key_group_map[
                    tiling_info.tiling_key
                ]:
                    source += gen_dfx_section_for_one_tiling_key_static(
                        compile_info,
                        tiling_key_slave,
                        tiling_info,
                        tiling_key_struct_size_map,
                    )
    else:
        for tiling_key in compile_info.tiling_key_list:
            source += gen_dfx_section_for_one_tiling_key_dynamic(
                compile_info, tiling_key, tiling_info, tiling_key_struct_size_map
            )
            if compile_info.tiling_key_group_map is not None:
                if tiling_key in compile_info.tiling_key_group_map.keys():
                    for tiling_key_slave in compile_info.tiling_key_group_map[
                        tiling_key
                    ]:
                        source += gen_dfx_section_for_one_tiling_key_dynamic(
                            compile_info,
                            tiling_key_slave,
                            tiling_info,
                            tiling_key_struct_size_map,
                        )

    try:
        with os.fdopen(
            os.open(
                out_file,
                os.O_TRUNC | os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(source)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            ("gen kernel func file failed, reason is:", err),
        )


def _add_op_compile_options_by_customized_json(
    op_compile_option: str, compile_option_tuple: CompileOptionTuple
):
    js = json.loads(op_compile_option)
    if (
        "--cce-auto-sync=off" not in compile_option_tuple.compile_options
        and js.get("auto_sync") is not False
    ):
        compile_option_tuple.compile_options.append("--cce-auto-sync")
        compile_option_tuple.compile_options.append("-mllvm")
        compile_option_tuple.compile_options.append("-api-deps-filter")
    short_soc_version = global_var_storage.get_variable(
        "ascendc_short_soc_version"
    ).lower()
    compile_options_custom = js.get("compile_options")
    if compile_options_custom is not None:
        if "__ALL__" in compile_options_custom:
            for opt in compile_options_custom.get("__ALL__"):
                if opt.startswith("-mllvm"):
                    compile_option_tuple.mllvm_options.append("-mllvm")
                    compile_option_tuple.mllvm_options.append(opt[7:])
                else:
                    compile_option_tuple.compile_options.append(opt)
        if short_soc_version in compile_options_custom:
            for opt in compile_options_custom.get(short_soc_version):
                if opt.startswith("-mllvm"):
                    compile_option_tuple.mllvm_options.append("-mllvm")
                    compile_option_tuple.mllvm_options.append(opt[7:])
                else:
                    compile_option_tuple.compile_options.append(opt)


def _get_tiling_struct_size(compile_info):
    tiling_struct_set = set()
    tiling_struct_size_map = {}
    max_tiling_size = 0
    for _, tiling_struct in compile_info.tiling_key_struct_map.items():
        tiling_struct_set.add(tiling_struct)

    for tiling_struct in tiling_struct_set:
        objdump_cmd = [
            "llvm-objdump",
            "-s",
            "-j",
            ".ascendc_tiling.{}".format(tiling_struct),
            "{}".format(compile_info.dst_file),
        ]
        proc = subprocess.Popen(objdump_cmd, stdout=subprocess.PIPE, stderr=None)
        (out, _) = proc.communicate()
        """
        e.g.
        Contents of section .ascend.meta.TilingData:         # main_tiling_info[0]
        0000 50000000 00000000                    P.......   # main_tiling_info[1] that needs to be parsed
        """
        tiling_str_info = out.decode("utf-8")
        if (
            TILING_KEY_SEARCH_KEYWORD in tiling_str_info
        ):  # key words from llvm-objdump .ascendc_tiling.
            main_line_start_index = tiling_str_info.index(TILING_KEY_SEARCH_KEYWORD)
            main_tiling_info = tiling_str_info[main_line_start_index:].split("\n")
            hex_num = main_tiling_info[1].split(" ")[2:4]
            hex_num_str = CommonUtility.parser_uint64_hex_num(hex_num)
            bytes_data = bytes.fromhex(hex_num_str)
            dec_data = struct.unpack(">Q", bytes_data)[0]
            tiling_struct_size_map[tiling_struct] = dec_data
            max_tiling_size = max(max_tiling_size, dec_data)

    # Preserve exact per-key sizes for Manifest constants before removing sections.
    compile_info.compiled_tiling_key_data_size_map = {
        str(tiling_key): int(tiling_struct_size_map[tiling_struct])
        for tiling_key, tiling_struct in compile_info.tiling_key_struct_map.items()
        if tiling_struct in tiling_struct_size_map
    }
    # The sk sub operator failed to rm tiling section because llvm-objcopy could not correctly process the ar obj.
    if (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and compile_info.is_super_kernel_compile is False
    ):
        CommonUtility.print_compile_log(
            compile_info.kernel_name,
            "[Superkernel]In sk sub kernel compile, do not need rm tiling seciton!",
            AscendCLogLevel.LOG_INFO,
        )
        return max_tiling_size
    # remove ascendc_tiling section
    objdump_cmd = [
        "llvm-objcopy",
        "--remove-section=.ascendc_tiling.*",
        "{}".format(compile_info.dst_file),
    ]
    proc = subprocess.Popen(
        objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    CommonUtility.print_compile_log(
        compile_info.kernel_name, "need rm tiling seciton!", AscendCLogLevel.LOG_INFO
    )
    (out, _) = proc.communicate()
    return max_tiling_size


def _get_tiling_struct_without_register_size(compile_info: CompileInfo):
    section_name_set = set()
    tiling_key_struct_size_map = {}  # tiling_key -> (tiling_struct, struct_size)
    max_tiling_size = 0
    objdump_cmd = ["llvm-objdump", "-s", f"{compile_info.dst_file}"]
    proc = subprocess.Popen(objdump_cmd, stdout=subprocess.PIPE, stderr=None)
    (out, _) = proc.communicate()
    tiling_str_info = out.decode("utf-8")
    tiling_lines = [
        line for line in tiling_str_info.splitlines() if ".ascendc_tiling" in line
    ]
    pattern = re.compile(r"\.ascendc_tiling\.[^\s]+")

    for line in tiling_lines:
        for match in pattern.findall(line):
            # match eg. ".ascendc_tiling.optiling::TilingData1_2UL.0"
            match = match.rstrip(":;,")
            section_name_set.add(match)
            name_part = match.split(".ascendc_tiling.", 1)[1]
            name_part = name_part.rsplit(".", 1)[0]

            tiling_key_struct_size_map = get_tiling_key_struct_size_map(
                tiling_key_struct_size_map, name_part, compile_info, 0
            )

    for section_name in section_name_set:
        objdump_cmd = [
            "llvm-objdump",
            "-s",
            "-j",
            "{}".format(section_name),
            "{}".format(compile_info.dst_file),
        ]
        proc = subprocess.Popen(objdump_cmd, stdout=subprocess.PIPE, stderr=None)
        (out, _) = proc.communicate()
        tiling_str_info = out.decode("utf-8")
        if (
            TILING_KEY_SEARCH_KEYWORD in tiling_str_info
        ):  # key words from llvm-objdump .ascendc_tiling.
            main_line_start_index = tiling_str_info.index(TILING_KEY_SEARCH_KEYWORD)
            main_tiling_info = tiling_str_info[main_line_start_index:].split("\n")
            hex_num = main_tiling_info[1].split(" ")[2:4]
            hex_num_str = CommonUtility.parser_uint64_hex_num(hex_num)
            bytes_data = bytes.fromhex(hex_num_str)
            dec_data = struct.unpack(">Q", bytes_data)[0]
            name_part = section_name.split(".ascendc_tiling.", 1)[1].rsplit(".", 1)[0]
            tiling_key_struct_size_map = get_tiling_key_struct_size_map(
                tiling_key_struct_size_map, name_part, compile_info, dec_data
            )
            max_tiling_size = max(max_tiling_size, dec_data)
    compile_info.max_tiling_size = max_tiling_size
    # REGISTER_NONE_TILING sections encode both the struct name and byte size.
    compile_info.compiled_tiling_key_data_size_map = {
        str(tiling_key): int(value[1])
        for tiling_key, value in tiling_key_struct_size_map.items()
        if isinstance(value, tuple) and len(value) == 2 and value[1] > 0
    }
    return tiling_key_struct_size_map


def delete_tiling_section(compile_info: CompileInfo):
    # The sk sub operator failed to rm tiling section because llvm-objcopy could not correctly process the ar obj.
    if (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and compile_info.is_super_kernel_compile is False
    ):
        CommonUtility.print_compile_log(
            compile_info.kernel_name,
            "[Superkernel]In sk sub kernel compile, do not need rm tiling seciton!",
            AscendCLogLevel.LOG_INFO,
        )
        return
    # remove ascendc_tiling section
    objdump_cmd = [
        "llvm-objcopy",
        "--remove-section=.ascendc_tiling.*",
        "{}".format(compile_info.dst_file),
    ]
    proc = subprocess.Popen(
        objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    CommonUtility.print_compile_log(
        compile_info.kernel_name, "need rm tiling seciton!", AscendCLogLevel.LOG_INFO
    )
    (out, _) = proc.communicate()
    return


def _update_compile_option(
    kernel_name: str, compile_options: list, extend_options: dict
):
    ascend_home_path = os.environ.get("ASCEND_HOME_PATH")
    import platform

    archlinux = platform.machine()
    if ascend_home_path is None or ascend_home_path == "":
        asc_opc_path = shutil.which("asc_opc")
        if asc_opc_path is not None:
            asc_opc_path_link = os.path.dirname(asc_opc_path)
            asc_opc_real_path = os.path.realpath(asc_opc_path_link)
            ascend_home_path = os.path.realpath(
                os.path.join(asc_opc_real_path, "..", "..")
            )
        else:
            ascend_home_path = "/usr/local/Ascend/cann"

    if "x86" in archlinux:
        asc_path = os.path.realpath(
            os.path.join(ascend_home_path, "x86_64-linux", "asc")
        )
    else:
        asc_path = os.path.realpath(
            os.path.join(ascend_home_path, "aarch64-linux", "asc")
        )
    if asc_path is None:
        asc_path = os.path.realpath(os.path.join(ascend_home_path, "compiler", "asc"))
    if os.path.exists(asc_path):
        cann_version_file_path = os.path.join(
            asc_path, "..", "..", "include", "version", "asc_devkit_version.h"
        )
        compile_options.append("-I" + os.path.join(asc_path, "impl", "adv_api"))
        compile_options.append("-I" + os.path.join(asc_path, "impl", "basic_api"))
        compile_options.append("-I" + os.path.join(asc_path, "impl", "c_api"))
        compile_options.append(
            "-I" + os.path.join(asc_path, "impl", "basic_api", "reg_compute")
        )
        compile_options.append("-I" + os.path.join(asc_path, "impl", "simt_api"))
        compile_options.append("-I" + os.path.join(asc_path, "impl", "utils"))
        compile_options.append("-I" + asc_path)
        compile_options.append("-I" + os.path.join(asc_path, "include"))
        compile_options.append("-I" + os.path.join(asc_path, "include", "adv_api"))
        compile_options.append("-I" + os.path.join(asc_path, "include", "basic_api"))
        compile_options.append("-I" + os.path.join(asc_path, "include", "aicpu_api"))
        compile_options.append("-I" + os.path.join(asc_path, "include", "c_api"))
        compile_options.append(
            "-I" + os.path.join(asc_path, "include", "basic_api", "reg_compute")
        )
        compile_options.append("-I" + os.path.join(asc_path, "include", "simt_api"))
        compile_options.append("-I" + os.path.join(asc_path, "include", "utils"))
        compile_options.append("-I" + os.path.join(asc_path, "..", "..", "include"))
        compile_options.append(
            "-I" + os.path.join(asc_path, "..", "..", "include", "ascendc")
        )
        compile_options.append("-I" + os.path.join(asc_path, "..", "ascendc", "act"))
        compile_options.append("-I" + os.path.join(asc_path, "..", "tikcpp"))
        compile_options.append("-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw"))
        compile_options.append(
            "-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw", "impl")
        )
        compile_options.append(
            "-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw", "interface")
        )
        if os.path.exists(cann_version_file_path):
            compile_options.append("-include" + cann_version_file_path)
        else:
            CommonUtility.print_compile_log(
                kernel_name,
                "not found asc_devkit_version.h",
                AscendCLogLevel.LOG_WARNING,
            )

    if extend_options.get("opp_kernel_hidden_dat_path", None) is not None:
        compile_options.append("-cce-vfs")
        compile_options.append(extend_options.get("opp_kernel_hidden_dat_path"))


def handle_sk_codegen_options(
    compile_info: CompileInfo, infered_info_from_ifile: InferChannelParamsFromIFile
):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        compile_info.super_kernel_early_start_set_flag = (
            infered_info_from_ifile.super_kernel_early_start_set_flag
        )
        compile_info.super_kernel_early_start_wait_flag = (
            infered_info_from_ifile.super_kernel_early_start_wait_flag
        )
        sp_info = get_context().get_addition("super_kernel_sub_info")
        if sp_info is not None:
            compile_info.super_kernel_info["sp_options"] = parse_super_kernel_options(
                sp_info.get("super_kernel_options", "")
            )
        else:
            compile_info.super_kernel_info["sp_options"] = {}


def gen_op_stub_kernel_func(
    compile_info: CompileInfo,
    op_info: OpInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    distinct_tag,
    kernel_meta_dir,
):
    # generate kernel fun for ffts_addr, overflow, workspace
    msg_info = "<{}> <{}> generate kernel stub start".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    file_name_tag = distinct_tag + "_kernel.cpp"

    # for aclnn sk sub operator combine norm workflow, generate norm kernel file
    if global_var_storage.get_variable("ascendc_sk_sub_combine_norm_workflow") is True:
        file_name_tag = distinct_tag + "_norm_kernel.cpp"
    compile_info.gen_kernel_func_file = os.path.join(
        kernel_meta_dir, op_info.kernel_name + file_name_tag
    )

    if CommonUtility.is_c310():
        gen_meta_info_section(compile_info, op_info)
    workspace_idx = gen_kernel_fun(
        compile_info,
        compile_info.origin_func_name,
        op_info,
        tiling_info,
        compile_option_tuple,
    )

    msg_info = "<{}> <{}> generate kernel stub end".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    return workspace_idx


def handle_compile_options(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    workspace_idx,
):
    # no dump and no superkernel
    if (
        CommonUtility.is_support_workspace_offset()
        and (not global_var_storage.get_variable("ascendc_enable_dump_workspace"))
        and (global_var_storage.get_variable("ascendc_enable_super_kernel") is False)
    ):
        compile_option_tuple.compile_options.append(
            f"-DWORKSPACE_PARAM_OFFSET={workspace_idx}"
        )

    # generate compile option for sub operator when enable super kernel
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        gen_sub_super_kernel_compile_options(
            compile_option_tuple, tiling_info, compile_info
        )

    # check whether ccec_O0 or ccec_g opend in compile context
    compile_info.is_debug = CommonUtility.check_debug_options(
        compile_option_tuple.compile_options
    )
    compile_option_tuple.compile_options.append(
        "-DONE_CORE_DUMP_SIZE=" + str(compile_info.dump_info["dump_size"])
    )
    if global_var_storage.get_variable("ascendc_recognize_simtvf") is True:
        compile_option_tuple.compile_options.append("-DASCENDC_RECOGNIZE_SIMT_VF")

    if (
        global_var_storage.get_variable("ascendc_enable_sanitizer") is False
        and global_var_storage.get_variable("ascendc_debug_compile_options") is False
    ):
        compile_option_tuple.compile_options.append("-DL2_CACHE_HINT")

    if tiling_info.static_shape_flag:
        compile_option_tuple.compile_options.append("-DCONST_TILING")


def compile_kernel_and_meta(
    compile_info: CompileInfo,
    op_info: OpInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
):
    CommonUtility.print_compile_log(
        op_info.kernel_name, "start to compile cce file...", AscendCLogLevel.LOG_INFO
    )
    msg_info = "<{}> <{}> compile kernel start".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)

    DFXSectionGenerator().generate_dfx_binary(compile_info, op_info, tiling_info)

    if CommonUtility.is_v220() or CommonUtility.is_c310():
        if compile_info.no_set_kernel_type is True:
            _compile_ascendc_cce_v220(compile_info, compile_option_tuple, tiling_info)
        else:
            _compile_ascendc_cce_v220_with_kernel_type(
                compile_info, compile_option_tuple, tiling_info
            )
    elif CommonUtility.is_m510():
        compile_info.code_channel = CORE_TYPE_CUBE
        compile_info.hard_sync = False
        _compile_ascendc_cce_m510(compile_info, compile_option_tuple, tiling_info)
    elif CommonUtility.is_regbase():
        _compile_ascendc_cce_regbase(compile_info, compile_option_tuple, tiling_info)
    elif CommonUtility.is_v200() and compile_info.no_set_kernel_type is False:
        _compile_ascendc_cce_v200_with_kernel_type(
            compile_info, compile_option_tuple, tiling_info
        )
    else:
        _compile_ascendc_cce(compile_info, compile_option_tuple, tiling_info)

    # Record-only replay stops before inspecting or generating physical objects.
    if not compile_info.compile_command_session.should_execute:
        return

    # get tiling struct and size in .asendc.tiling section and generate meta_info.o when using REGISTER_NONE_TILING
    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        tiling_key_struct_size_map = _get_tiling_struct_without_register_size(
            compile_info
        )
        gen_tiling_struct_size_and_dfx_section_file(
            compile_info, tiling_info, tiling_key_struct_size_map
        )
        chip_version = CommonUtility.get_chip_version()
        if CommonUtility.is_c310() or CommonUtility.is_v220():
            arch = f"dav-{chip_version}-vec"
        else:
            arch = f"dav-{chip_version}"
        compile_cmd = gen_compile_cmd_for_meta_info(
            compile_info.tiling_and_dfx_utils_file,
            compile_info.tiling_and_dfx_utils_bin_path,
            compile_option_tuple,
            arch,
        )
        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
    msg_info = "<{}> <{}> compile kernel end".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    CommonUtility.print_compile_log(
        op_info.kernel_name, "compile cce file success", AscendCLogLevel.LOG_INFO
    )


def link_kernel_obj(
    compile_info: CompileInfo, op_info: OpInfo, tiling_info: TilingInfo
):
    msg_info = "<{}> <{}> link kernel start".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    if global_var_storage.get_variable("ascendc_enable_sanitizer"):
        _mssanitizer_link(
            compile_info.dst_file, compile_info.dst_file, compile_info.compile_log_path
        )
    # split .o 4
    split_sub_kernel_objs(compile_info.dst_file, tiling_info, compile_info)
    CommonUtility.print_compile_log(
        op_info.kernel_name,
        "start to link relocatable for dst obj...",
        AscendCLogLevel.LOG_INFO,
    )
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is False:
        if not global_var_storage.get_variable("ascendc_tiling_no_register"):
            link_relocatable(compile_info.dst_file, compile_info.compile_log_path)
        else:
            link_relocatable_meta_file(
                compile_info.dst_file,
                compile_info.tiling_and_dfx_utils_bin_path,
                compile_info.compile_log_path,
            )
            if not global_var_storage.get_variable("ascendc_compile_debug_config"):
                CommonUtility.remove_temp_file(
                    compile_info.tiling_and_dfx_utils_bin_path
                )
    msg_info = "<{}> <{}> link kernel end".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    CommonUtility.print_compile_log(
        op_info.kernel_name, "link relocatable success", AscendCLogLevel.LOG_INFO
    )


def _match_regex(pattern: str, op_name: str) -> bool:
    """
    Match regex pattern with op_name.
    Supports '.' (matches any single char) and '*' (matches zero or more of preceding element).
    """
    if len(pattern) > 0 and pattern[0] == "*":
        return False

    m, n = len(op_name), len(pattern)

    def matches(i: int, j: int) -> bool:
        if i == 0 or j == 0:
            return False
        if pattern[j - 1] == ".":
            return True
        return op_name[i - 1] == pattern[j - 1]

    dp = [[False] * (n + 1) for _ in range(m + 1)]
    dp[0][0] = True
    for i in range(m + 1):
        for j in range(1, n + 1):
            if pattern[j - 1] == "*":
                if j >= 2:
                    dp[i][j] |= dp[i][j - 2]
                    if matches(i, j - 1):
                        dp[i][j] |= dp[i - 1][j]
            elif matches(i, j):
                dp[i][j] |= dp[i - 1][j - 1]
    return dp[m][n]


def _get_dcci_disable_cap_bitmap(
    compile_info: CompileInfo, kernel_symbols: list
) -> int:
    """
    Check if DCCI should be disabled for any kernel in kernel_symbols.
    Returns 4 if matched, 0 otherwise.
    """
    sp_options = compile_info.super_kernel_info.get("sp_options", {})
    patterns = sp_options.get("dcci-disable-on-kernel", [])
    if isinstance(patterns, list) and patterns:
        for kernel_name in kernel_symbols:
            if any(_match_regex(p, kernel_name) for p in patterns):
                return 4
    return 0


def _get_sk_cap_bitmap(compile_info: CompileInfo, basic_kernel_symbols: list) -> int:
    """Encode early-start and DCCI capabilities for SK_BIND generation."""

    cap_bitmap = 0
    if (
        global_var_storage.get_variable(
            "ascendc_sub_super_kernel_early_start_wait_flag"
        )
        is True
    ):
        cap_bitmap |= 1
    if (
        global_var_storage.get_variable("ascendc_sub_super_kernel_early_start_set_flag")
        is True
    ):
        cap_bitmap |= 2
    return cap_bitmap | _get_dcci_disable_cap_bitmap(compile_info, basic_kernel_symbols)


def compile_sk_bind(
    compile_info: CompileInfo,
    compile_info_origin: CompileInfo,
    compile_option_tuple,
    kernel_meta_dir,
):
    sk_bind_src_file = os.path.join(kernel_meta_dir, "sk_bind.cpp")
    sk_bind_dst_file = os.path.join(kernel_meta_dir, "sk_bind.o")
    source = '#include "kernel_operator.h"\n'

    # bitmap definition: bit0:wait_flag(1), bit1:set_flag(2), bit2:dcci_disable(4)
    cap_bitmap = _get_sk_cap_bitmap(
        compile_info, compile_info_origin.global_kernel_symbols
    )

    for idx, global_syb in enumerate(compile_info_origin.global_kernel_symbols):
        sk_syb = compile_info.global_kernel_symbols[idx]
        source += f'extern "C" {compile_info_origin.global_kernel_attribute} void {global_syb}();\n'
        source += (
            f'extern "C" {compile_info.global_kernel_attribute} void {sk_syb}();\n'
        )
        source += f'extern "C" {compile_info.global_kernel_attribute} void {sk_syb}_split1();\n'
        source += f'extern "C" {compile_info.global_kernel_attribute} void {sk_syb}_split2();\n'
        source += f'extern "C" {compile_info.global_kernel_attribute} void {sk_syb}_split3();\n'
        source += f"SK_BIND({global_syb}, {cap_bitmap}, {sk_syb}, {sk_syb}_split1, {sk_syb}_split2, {sk_syb}_split3);\n"

    # write code into file
    try:
        with os.fdopen(
            os.open(
                sk_bind_src_file,
                os.O_TRUNC | os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(source)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("gen sk bind file failed, reason is:", err)
        )

    arch = None
    if CommonUtility.is_c310():
        arch = "dav-c310-cube"
    elif CommonUtility.is_v220():
        arch = "dav-c220-cube"
    else:
        raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, "Unsupported architecture")

    compile_cmd = None
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        compile_cmd = [
            os.environ.get("ASCENDC_CCACHE_EXECUTABLE"),
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
        ]
    else:
        compile_cmd = [
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
        ]

    for option in compile_option_tuple.compile_options:
        compile_cmd += [option]
    compile_cmd += [
        "-xcce",
        sk_bind_src_file,
        f"--cce-aicore-arch={arch}",
        "--cce-aicore-only",
        "-std=c++17",
        "-DTILING_KEY_VAR=0",
        "-o",
        sk_bind_dst_file,
    ]

    compile_info.compile_command_session.submit_sk_bind(
        SkBindCommand(
            source_path=sk_bind_src_file,
            argv=tuple(compile_cmd),
            output_path=sk_bind_dst_file,
        )
    )
    if compile_info.compile_command_session.should_execute:
        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
    return sk_bind_dst_file


def _record_kernel_spec_sk_commands(
    kernel_spec, op_info, infered_info, tiling_info, distinct_tag
):
    if not kernel_spec.begin_sk_recording():
        return
    kernel_meta_dir = os.path.dirname(kernel_spec.compile_info.dst_file)
    # Replay the original SK workflow with a record-only command session.
    compile_info = kernel_spec.sk_compile_info
    basic_compile_info = kernel_spec.basic_compile_info
    compile_options = kernel_spec.sk_compile_option_tuple
    DFXSectionGenerator().dfx_info_reset(op_info)
    DFXSectionGenerator().update_is_support(op_info)
    compile_info.raw_tiling_key_kernel_type = copy.deepcopy(
        compile_info.tiling_key_kernel_type
    )
    handle_sk_codegen_options(compile_info, infered_info)
    workspace_idx = gen_op_stub_kernel_func(
        compile_info,
        op_info,
        compile_options,
        tiling_info,
        distinct_tag,
        kernel_meta_dir,
    )
    handle_compile_options(
        compile_info,
        compile_options,
        tiling_info,
        workspace_idx,
    )
    compile_kernel_and_meta(
        compile_info,
        op_info,
        compile_options,
        tiling_info,
    )
    # Use the recorded symbols to build the SK_BIND command and capability bitmap.
    compile_info.global_kernel_symbols = [
        command.compiled_symbol
        for command in compile_info.compile_command_session.records
    ]
    compile_sk_bind(
        compile_info,
        basic_compile_info,
        compile_options,
        kernel_meta_dir,
    )
    kernel_spec.finish_sk_recording(
        _get_sk_cap_bitmap(
            compile_info,
            basic_compile_info.global_kernel_symbols,
        )
    )


def compile_op_common_part(
    cce_file: str,
    origin_func_name: str,
    op_info: OpInfo,
    compile_option_tuple,
    infered_info_from_ifile: InferChannelParamsFromIFile,
    extend_options: dict,
):
    value_depend_dict = extend_options.get("valueDepend")
    kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
    distinct_tag = CommonUtility.get_distinct_filename_tag()
    compile_log_path = None
    if global_var_storage.get_variable("ascendc_compile_debug_config"):
        compile_log_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + distinct_tag + ".log"
        )

    input_gen_placehoder = check_if_gen_placehoder(op_info, True)
    output_gen_placehoder = check_if_gen_placehoder(op_info, False)

    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["generate_tiling_start"],
        AscendCLogLevel.LOG_INFO,
    )

    is_const_propagation = (
        "-DFORCE_TILING_CONST_PROPAGATION" in compile_option_tuple.compile_options
    )
    global_var_storage.set_variable(
        "ascendc_tiling_const_propagation", is_const_propagation
    )

    tiling_info: TilingInfo = get_tiling_info_by_tiling(
        op_info, infered_info_from_ifile, value_depend_dict, origin_func_name
    )

    CommonUtility.print_compile_log(
        op_info.kernel_name, "get tiling info success", AscendCLogLevel.LOG_INFO
    )

    file_name_tag = distinct_tag + "_tiling_data.h"
    tiling_data_file_path = os.path.join(
        kernel_meta_dir, op_info.kernel_name + file_name_tag
    )
    tiling_info.save_file(tiling_data_file_path)
    global_var_storage.set_variable(
        "ascendc_is_static_op", tiling_info.static_shape_flag
    )
    # replace tiling key when tiling_key is set in compile params
    tiling_key_list = infered_info_from_ifile.tiling_key_list
    tiling_key_group_map = infered_info_from_ifile.tiling_key_group_map
    context_tiling_key = get_context().get_addition("tiling_key")
    # override customized tiling key list if the input is passed from
    customize_tiling_key = "customized_tiling_key_list"
    if customize_tiling_key in extend_options and isinstance(
        extend_options[customize_tiling_key], list
    ):
        context_tiling_key = extend_options[customize_tiling_key]
    if context_tiling_key:
        new_tiling_keys = []
        for tiling_key in context_tiling_key:
            if tiling_key in tiling_key_list:
                new_tiling_keys.append(tiling_key)
            else:
                CommonUtility.print_compile_log(
                    op_info.kernel_name,
                    f"given tiling key {tiling_key} is not in supported tiling_key list",
                    AscendCLogLevel.LOG_WARNING,
                )
        tiling_key_list = new_tiling_keys
        if len(tiling_key_list) == 0:
            msg_info = "None of the given tiling keys are in the supported list."
            LogUtil.log_print(
                op_info.kernel_name, msg_info, AscendCLogLevel.LOG_WARNING
            )
            sys.exit(1)
    code_channel = infered_info_from_ifile.code_channel
    hardware_sync_in_asm = False
    # code channel can not infer by .i neet infer by .o
    if code_channel == -1 and infered_info_from_ifile.no_set_kernel_type is True:
        dst_file_header = os.path.join(
            kernel_meta_dir, op_info.kernel_name + "_infer_channel"
        )
        CommonUtility.print_compile_log(
            op_info.kernel_name,
            "get kernel type by infer channel...",
            AscendCLogLevel.LOG_INFO,
        )
        code_channel, hardware_sync_in_asm = get_code_channel_v220_by_first_tiling_key(
            InferChannelParams(
                cce_file,
                dst_file_header,
                compile_option_tuple,
                tiling_key_list[0],
                tiling_info,
                compile_log_path,
                infered_info_from_ifile.no_kfc_server_flag,
            )
        )
        CommonUtility.print_compile_log(
            op_info.kernel_name,
            "get kernel type by infer channel success",
            AscendCLogLevel.LOG_INFO,
        )

    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["generate_tiling_end"],
        AscendCLogLevel.LOG_INFO,
    )

    compile_info = CompileInfo()
    compile_info.src_file = cce_file
    compile_info.dst_file = os.path.join(kernel_meta_dir, op_info.kernel_name + ".o")
    compile_info.kernel_name = op_info.kernel_name
    compile_info.origin_func_name = origin_func_name
    compile_info.op_type = op_info.op_type
    compile_info.code_channel = code_channel
    compile_info.tiling_key_list = tiling_key_list
    compile_info.tiling_key_group_map = tiling_key_group_map
    compile_info.compile_log_path = compile_log_path
    compile_info.hard_sync = infered_info_from_ifile.hard_sync or hardware_sync_in_asm
    compile_info.enable_deterministic = infered_info_from_ifile.enable_deterministic
    compile_info.tiling_key_deterministic = (
        infered_info_from_ifile.tiling_key_deterministic
    )
    compile_info.tiling_key_kernel_type = infered_info_from_ifile.tiling_key_kernel_type
    compile_info.no_set_kernel_type = infered_info_from_ifile.no_set_kernel_type
    compile_info.default_kernel_type = infered_info_from_ifile.default_kernel_type
    compile_info.dump_info = {"dump_type": "", "dump_size": 1024}
    compile_info.template_tiling_info = infered_info_from_ifile.template_tiling_info
    compile_info.tiling_key_struct_map = infered_info_from_ifile.tiling_key_struct_map
    compile_info.register_tiling_struct = infered_info_from_ifile.register_tiling_struct
    compile_info.tpl_tiling_struct = infered_info_from_ifile.tpl_tiling_struct

    kernel_spec = KernelSpecCompilation.create(
        compile_info, compile_option_tuple, tiling_info
    )

    with kernel_spec:
        # generate tiling struct size, dfx section
        if global_var_storage.get_variable("ascendc_tiling_no_register"):
            file_name_tag = distinct_tag + "_meta_info.cpp"
            compile_info.tiling_and_dfx_utils_file = os.path.join(
                kernel_meta_dir, op_info.kernel_name + file_name_tag
            )
            file_name_tag = distinct_tag + "_meta_info.o"
            compile_info.tiling_and_dfx_utils_bin_path = os.path.join(
                kernel_meta_dir, op_info.kernel_name + file_name_tag
            )
        compile_info_origin = CompileInfo()
        compile_option_tuple_origin = None
        if (
            get_context().get_addition("super_kernel_sub_combine") is True
            and global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        ):
            global_var_storage.set_variable("ascendc_sk_double_compile", True)
            compile_info_origin = copy.deepcopy(compile_info)
            compile_option_tuple_origin = copy.deepcopy(compile_option_tuple)
        kernel_spec.select_basic_compile(
            compile_info_origin, compile_option_tuple_origin
        )

        # dump ktype handle
        compile_info.raw_tiling_key_kernel_type = copy.deepcopy(
            compile_info.tiling_key_kernel_type
        )

        # get super kernel option to compile info when enable super kernel
        handle_sk_codegen_options(compile_info, infered_info_from_ifile)

        # stub kernel func generation
        workspace_idx = gen_op_stub_kernel_func(
            compile_info,
            op_info,
            compile_option_tuple,
            tiling_info,
            distinct_tag,
            kernel_meta_dir,
        )
        # handle compile options
        handle_compile_options(
            compile_info, compile_option_tuple, tiling_info, workspace_idx
        )

        # compile cce file and set meta info in .o
        compile_kernel_and_meta(
            compile_info, op_info, compile_option_tuple, tiling_info
        )

        # link kernel obj
        link_kernel_obj(compile_info, op_info, tiling_info)

        # aclnn sk combine compile workflow
        if (
            get_context().get_addition("super_kernel_sub_combine") is True
            and global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        ):
            # reset sk opt
            global_var_storage.set_variable("ascendc_enable_super_kernel", False)
            global_var_storage.set_variable(
                "ascendc_sk_sub_combine_norm_workflow", True
            )
            DFXSectionGenerator().dfx_info_reset(op_info)
            DFXSectionGenerator().update_is_support(op_info)
            compile_info_origin.dst_file = os.path.join(
                kernel_meta_dir, op_info.kernel_name + "_norm.o"
            )

            # dump ktype handle
            compile_info.raw_tiling_key_kernel_type = copy.deepcopy(
                compile_info_origin.tiling_key_kernel_type
            )
            handle_sk_codegen_options(compile_info_origin, infered_info_from_ifile)
            workspace_idx = gen_op_stub_kernel_func(
                compile_info_origin,
                op_info,
                compile_option_tuple_origin,
                tiling_info,
                distinct_tag,
                kernel_meta_dir,
            )
            handle_compile_options(
                compile_info_origin,
                compile_option_tuple_origin,
                tiling_info,
                workspace_idx,
            )
            compile_kernel_and_meta(
                compile_info_origin, op_info, compile_option_tuple_origin, tiling_info
            )

            # compile_sk_bind
            sk_bind_dst_file = compile_sk_bind(
                compile_info, compile_info_origin, compile_option_tuple, kernel_meta_dir
            )

            # link norm.o, sk.o, sk_bind.o and optional meta_info.o
            link_sk_norm_combine(
                compile_info.dst_file,
                compile_info_origin.dst_file,
                sk_bind_dst_file,
                compile_info_origin.tiling_and_dfx_utils_bin_path,
                compile_info.compile_log_path,
            )

            global_var_storage.set_variable("ascendc_enable_super_kernel", True)
            global_var_storage.set_variable(
                "ascendc_sk_sub_combine_norm_workflow", False
            )
            DFXSectionGenerator().update_is_support(op_info)

        kernel_spec.attach_resource_id()

        # generate opinfo json
        _json_post_process(
            compile_info,
            op_info,
            tiling_info,
            input_gen_placehoder,
            output_gen_placehoder,
            compile_log_path,
        )
        if kernel_spec.record_sk_commands:
            _record_kernel_spec_sk_commands(
                kernel_spec,
                op_info,
                infered_info_from_ifile,
                tiling_info,
                distinct_tag,
            )
        kernel_spec.publish_manifest(tiling_info, workspace_idx)
        if not global_var_storage.get_variable("ascendc_compile_debug_config"):
            tiling_info.remove_file()
            CommonUtility.remove_temp_file(compile_info.gen_kernel_func_file)
            CommonUtility.remove_temp_file(compile_info.tiling_and_dfx_utils_file)
        CommonUtility.print_compile_log(
            "",
            "compile Ascend C operator {} success".format(op_info.op_type),
            AscendCLogLevel.LOG_INFO,
        )
        msg_info = "<{}> <{}> compile op end".format(
            compile_info.op_type, compile_info.tiling_key_list
        )
        LogUtil.detail_log_print(
            op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO
        )


def compile_op(
    cce_file: str,
    origin_func_name: str,
    op_info: OpInfo,
    compile_options: list = None,
    code_channel: int = -1,
    op_compile_option: str = "{}",
    extend_options: dict = {},
):
    """get tiling_data/ generate tiling_data file/ compile cce to .o / generate .json file
    Args:
        cce_file (str): cce file to be compiled
        origin_func_name (str): func_name written by user, without md5
        op_info (OpInfo): operator info
        compile_options (list): compile options for bisheng
        code_channel (int): one of CORE_TYPE_MIX/CORE_TYPE_CUBE/CORE_TYPE_VEC
    """
    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["compile_op_start"],
        AscendCLogLevel.LOG_INFO,
    )
    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["preprocess_start"],
        AscendCLogLevel.LOG_INFO,
    )
    # online compile reuses thread, dfx infos need to be reset.
    global_var_storage.global_storage_reset()
    if extend_options.get(
        "opp_kernel_hidden_dat_path", None
    ) is None and not os.path.exists(cce_file):
        raise Exception("input cce file is not exists, file name: " + cce_file)

    compile_option_tuple = CompileOptionTuple(
        [] if compile_options is None else compile_options, []
    )
    need_impl_mode_macro = (
        (CommonUtility.is_c310() or CommonUtility.is_m510())
        and isinstance(op_info.impl_mode, str)
        and op_info.impl_mode != ""
    )
    if need_impl_mode_macro:
        impl_mode_def = f"-D{op_info.impl_mode.upper()}_"  # IMPL_MODE_IS
        if impl_mode_def not in compile_option_tuple.compile_options:
            compile_option_tuple.compile_options.append(impl_mode_def)

    _add_op_compile_options_by_customized_json(op_compile_option, compile_option_tuple)

    compile_option_tuple.compile_options = compile_pre_process(
        op_info, compile_option_tuple.compile_options
    )

    DFXSectionGenerator().dfx_info_reset(op_info)

    _update_compile_option(
        op_info.kernel_name, compile_option_tuple.compile_options, extend_options
    )

    value_depend_dict = extend_options.get("valueDepend")
    _set_compile_info(op_info, value_depend_dict)
    kernel_meta_dir = CommonUtility.get_kernel_meta_dir()

    compile_option_tuple.compile_options.append("-DASCENDC_TPL_KERNEL")
    distinct_tag = CommonUtility.get_distinct_filename_tag()
    compile_log_path = None
    if global_var_storage.get_variable("ascendc_compile_debug_config"):
        compile_log_path = os.path.join(
            kernel_meta_dir, op_info.kernel_name + distinct_tag + ".log"
        )

    # get tilingkeylist and simple infer code_channel
    CommonUtility.print_compile_log(
        op_info.kernel_name,
        "precompile to get some simple kernel info...",
        AscendCLogLevel.LOG_INFO,
    )
    infered_info_from_ifile = (
        KernelInfoInfer.get_tiling_key_list_and_simple_infer_code_channel(
            op_info,
            cce_file,
            os.path.join(kernel_meta_dir, op_info.kernel_name + ".i"),
            compile_option_tuple,
            compile_log_path,
            origin_func_name,
        )
    )
    CommonUtility.print_compile_log(
        op_info.kernel_name,
        "precompile to get some simple kernel info success",
        AscendCLogLevel.LOG_INFO,
    )
    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["preprocess_end"],
        AscendCLogLevel.LOG_INFO,
    )

    compile_op_common_part(
        cce_file,
        origin_func_name,
        op_info,
        compile_option_tuple,
        infered_info_from_ifile,
        extend_options,
    )


def compile_op_with_customized_config(
    cce_file: str,
    origin_func_name: str,
    op_info: OpInfo,
    compile_options: list = None,
    code_channel: int = -1,
    op_compile_option: str = "{}",
    extend_options: dict = {},
    customized_config: CustomizedConfig = None,
):
    """get tiling_data/ generate tiling_data file/ compile cce to .o / generate .json file
    Args:
        cce_file (str): cce file to be compiled
        origin_func_name (str): func_name written by user, without md5
        op_info (OpInfo): operator info
        compile_options (list): compile options for bisheng
        code_channel (int): one of CORE_TYPE_MIX/CORE_TYPE_CUBE/CORE_TYPE_VEC
    """
    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["compile_op_start"],
        AscendCLogLevel.LOG_INFO,
    )
    LogUtil.detail_log_print(
        op_info.kernel_name,
        COMPILE_STAGE_MSG_INFO["preprocess_start"],
        AscendCLogLevel.LOG_INFO,
    )
    # online compile reuses thread, dfx infos need to be reset.
    global_var_storage.global_storage_reset()
    if extend_options.get(
        "opp_kernel_hidden_dat_path", None
    ) is None and not os.path.exists(cce_file):
        raise Exception("input cce file is not exists, file name: " + cce_file)

    compile_option_tuple = CompileOptionTuple(
        [] if compile_options is None else compile_options, []
    )
    need_impl_mode_macro = (
        (CommonUtility.is_c310() or CommonUtility.is_m510())
        and isinstance(op_info.impl_mode, str)
        and op_info.impl_mode != ""
    )
    if need_impl_mode_macro:
        impl_mode_def = f"-D{op_info.impl_mode.upper()}_"  # IMPL_MODE_IS
        if impl_mode_def not in compile_option_tuple.compile_options:
            compile_option_tuple.compile_options.append(impl_mode_def)

    _add_op_compile_options_by_customized_json(op_compile_option, compile_option_tuple)

    compile_option_tuple.compile_options = compile_pre_process(
        op_info, compile_option_tuple.compile_options
    )

    DFXSectionGenerator().dfx_info_reset(op_info)

    _update_compile_option(
        op_info.kernel_name, compile_option_tuple.compile_options, extend_options
    )

    if customized_config is None:
        raise Exception(
            "must provide infer infos for compile op with customized informations"
        )
    infered_info_from_ifile = convert_customized_config_to_inferchannel(
        customized_config
    )
    if infered_info_from_ifile.tiling_key_group_map is None:
        infered_info_from_ifile = infered_info_from_ifile._replace(
            tiling_key_group_map={}
        )

    compile_option_tuple.compile_options.append("-DASCENDC_TPL_KERNEL")
    value_depend_dict = extend_options.get("valueDepend")
    _set_compile_info(op_info, value_depend_dict)

    compile_op_common_part(
        cce_file,
        origin_func_name,
        op_info,
        compile_option_tuple,
        infered_info_from_ifile,
        extend_options,
    )


def _compile_single_tiling(tiling_key, compile_info, tiling_info, compile_option_tuple):
    dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
    compile_cmd = _gen_compile_cmd(
        compile_info.gen_kernel_func_file,
        dst_file,
        compile_option_tuple,
        tiling_info.tiling_data_file_path,
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{compile_info.origin_func_name}="
            f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [
            f"-D{compile_info.origin_func_name}={compile_info.origin_func_name}_{tiling_key}_tilingkey"
        ]
    kernel_func_name = compile_info.kernel_name + "_%s" % tiling_key
    compile_cmd += [
        f"-Dauto_gen_{compile_info.origin_func_name}_kernel={kernel_func_name}"
    ]
    section_content = DFXSectionGenerator().generate_dfx_section(
        tiling_key, tiling_info, kernel_func_name, compile_info, True
    )
    return compile_cmd, section_content


def _compile_ascendc_cce(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)

    new_sources = sources[:-1]
    if tiling_info.static_shape_flag:
        compile_cmd = _gen_compile_cmd(
            compile_info.gen_kernel_func_file,
            compile_info.dst_file,
            compile_option_tuple,
            tiling_info.tiling_data_file_path,
        )
        # tbe-pass add "__kernel0" in tbe-codegen and json, we use -D to change function name
        compile_cmd += [
            f"-Dauto_gen_{compile_info.origin_func_name}_kernel={compile_info.get_kernel_func_name()}"
        ]
        if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
            tiling_data_hash_src = tiling_info.tiling_data
            if isinstance(tiling_data_hash_src, str):
                tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
            elif not tiling_data_hash_src:
                tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
            tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
            compile_cmd += [
                f"-D{compile_info.origin_func_name}="
                f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_info.tiling_key}_tilingkey"
            ]
        compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_info.tiling_key}UL"]
        new_sources += DFXSectionGenerator().generate_dfx_section(
            str(tiling_info.tiling_key),
            tiling_info,
            compile_info.get_kernel_func_name(),
            compile_info,
            True,
        )
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )

        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
        target = "cce_core"
        tvm_callback_cce_postproc(
            target, compile_info.kernel_name, tiling_info.block_num
        )
    else:
        obj_files = []
        for tiling_key in compile_info.tiling_key_list:
            dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            obj_files.append(dst_file)
        cmds_list = []
        for tiling_key in compile_info.tiling_key_list:
            compile_cmd, section_content = _compile_single_tiling(
                tiling_key, compile_info, tiling_info, compile_option_tuple
            )
            cmds_list.append(compile_cmd)
            new_sources += section_content
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )
        # compile binary
        compile_multi_tilingkey(
            compile_info.tiling_key_list,
            cmds_list,
            os.path.basename(compile_info.dst_file)[:-2],
            compile_info.compile_log_path,
        )
        fatbin_objs(
            obj_files,
            compile_info.dst_file,
            compile_info.is_debug,
            compile_info.compile_log_path,
        )
        target = "cce_core"
        tvm_callback_cce_postproc(
            target, compile_info.kernel_name, tiling_info.block_num
        )
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            compile_info.tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )


def _get_sub_kernel_name(compile_info: CompileInfo, core_type: int):
    core_type_marker = "_mix_aic" if core_type == CORE_TYPE_CUBE else "_mix_aiv"
    # i.e. change demo_kernel.o to demo_kernel_mix_aic.o
    sub_kernel_name = compile_info.kernel_name + core_type_marker
    return sub_kernel_name


def _generate_section_content(
    kernel_name: str,
    tiling_key: str,
    kernel_type: KernelMetaType,
    tiling_info: TilingInfo,
    compile_info: CompileInfo,
):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        return ""
    section_content = ""
    section_content += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL\n"
    section_content += get_ktype_section_variable(
        f"{kernel_name}_section", f"{kernel_name}", kernel_type
    )
    if compile_info.tiling_key_group_map is not None:
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                kernel_name_slave = get_kernel_fun_name_with_tiling_key_and_kernel_type(
                    compile_info, tiling_key_slave
                )
                if (
                    compile_info.tiling_key_kernel_type.get(tiling_key_slave)
                    is not None
                ):
                    kernel_type_slave = compile_info.tiling_key_kernel_type.get(
                        str(tiling_key_slave)
                    )
                else:
                    raise Exception(
                        f"the kernel type of tiling key {tiling_key_slave} is None"
                    )
                section_content += get_ktype_section_variable(
                    f"{kernel_name_slave}_section",
                    f"{kernel_name_slave}",
                    kernel_type_slave,
                )
    section_content += "#endif\n"
    section_content += DFXSectionGenerator().generate_dfx_section(
        tiling_key, tiling_info, kernel_name, compile_info
    )
    return section_content


def _get_compile_cmd_and_section_content(
    compile_info: CompileInfo,
    arch: str,
    compile_option_tuple,
    tiling_info: TilingInfo,
    tiling_key: str,
):
    compile_cmd = gen_compile_cmd_v220(
        compile_info.gen_kernel_func_file,
        compile_info.dst_file,
        compile_option_tuple,
        arch,
        tiling_info.tiling_data_file_path,
    )

    current_kernel_name = ""
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
    if tiling_info.static_shape_flag:
        if kernel_type.value >= 2:
            current_kernel_name = compile_info.kernel_name
            current_kernel_name = gen_sub_kernel_name(
                current_kernel_name, arch, kernel_type.name, compile_info.dst_file
            )
            compile_cmd += [
                f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
            ]
        else:
            current_kernel_name = compile_info.get_kernel_func_name()
            current_kernel_name = gen_sub_kernel_name(
                current_kernel_name, "AiCore", kernel_type.name, compile_info.dst_file
            )
            compile_cmd += [
                f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
            ]
    else:
        core_type = "cube" if arch.endswith("-cube") else "vec"
        current_kernel_name = get_compile_target_name(
            compile_info, tiling_key, core_type
        )
        compile_cmd += [
            f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
        ]
        if kernel_type.value >= 2:
            set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group(
                tiling_key, arch, kernel_type.name, current_kernel_name, compile_info
            )
        else:
            set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group(
                tiling_key,
                "AiCore",
                kernel_type.name,
                current_kernel_name,
                compile_info,
            )
    compile_cmd.extend(
        get_compile_target_options(compile_info, tiling_key, CommonUtility.is_c310())
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{compile_info.origin_func_name}="
            f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [
            f"-D{compile_info.origin_func_name}={compile_info.origin_func_name}_{tiling_key}_tilingkey"
        ]
    section_content = _generate_section_content(
        current_kernel_name, tiling_key, kernel_type, tiling_info, compile_info
    )
    if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
        compile_info.global_kernel_symbols.append(current_kernel_name)
    compile_info.last_compiled_symbol = current_kernel_name
    return compile_cmd, section_content


def _is_mix_aic_1x_kernel_type(kernel_type):
    return kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
    ]


def _is_hard_sync_or_mix_1x_kernel_type(kernel_type):
    return kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    ]


def _is_single_core_kernel_type(kernel_type):
    return kernel_type in [
        KernelMetaType.KERNEL_TYPE_AIV_ONLY,
        KernelMetaType.KERNEL_TYPE_AIC_ONLY,
    ]


def _get_arch_and_code_type(kernel_type, chip_version):
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    ]:
        arch = f"dav-{chip_version}-cube"
        code_type = CORE_TYPE_CUBE
    else:
        arch = f"dav-{chip_version}-vec"
        code_type = CORE_TYPE_VEC
    return arch, code_type


def _compile_core(
    compile_info, arch, code_type, compile_option_tuple, tiling_info, tiling_key
):
    sub_compile_info = _get_sub_compile_info(compile_info, code_type)
    compile_cmd, section_content = _get_compile_cmd_and_section_content(
        sub_compile_info, arch, compile_option_tuple, tiling_info, tiling_key
    )
    if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
        compile_info.global_kernel_symbols.extend(
            sub_compile_info.global_kernel_symbols
        )
    return sub_compile_info, compile_cmd, section_content


def _finalize_and_write_sources(new_sources):
    new_sources += global_var_storage.get_variable("ascendc_meta_info")
    new_sources += "#endif\n"
    return new_sources


def _handle_mix_objs(compile_info, mix_objs, dst_file):
    if compile_info.enable_final_super_kernel_compile is True:
        compile_info.super_kernel_objs = mix_objs
    else:
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )


def _compile_mix_aic_1x_kernel(
    compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
):
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    cmds_list = []
    dst_file = compile_info.dst_file

    cube_compile_info, cube_compile_cmd, cube_section = _compile_core(
        compile_info,
        f"dav-{chip_version}-cube",
        CORE_TYPE_CUBE,
        compile_option_tuple,
        tiling_info,
        tiling_info.tiling_key,
    )
    new_sources += cube_section
    cmds_list.append(cube_compile_cmd)

    vec_compile_info, vec_compile_cmd, vec_section = _compile_core(
        compile_info,
        f"dav-{chip_version}-vec",
        CORE_TYPE_VEC,
        compile_option_tuple,
        tiling_info,
        tiling_info.tiling_key,
    )
    new_sources += vec_section
    cmds_list.append(vec_compile_cmd)

    new_sources = _finalize_and_write_sources(new_sources)
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    for cmd in cmds_list:
        CommonUtility.run_cmd_inner(
            cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )

    mix_objs = [cube_compile_info.dst_file, vec_compile_info.dst_file]
    _handle_mix_objs(compile_info, mix_objs, dst_file)

    _gen_mix_sub_json(compile_info, tiling_info, CORE_TYPE_CUBE)
    if kernel_type.value == 6:
        tiling_info.task_ration = 1
    task_ration_str = f"1:{tiling_info.task_ration}"
    _gen_mix_json_from_seperate_json_for_kernel_type(
        compile_info.kernel_name, task_ration_str, CORE_TYPE_CUBE, True
    )
    set_soc_spec("AiCore")


def _compile_hard_sync_or_mix_1x_kernel(
    compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
):
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    arch, code_type = _get_arch_and_code_type(kernel_type, chip_version)

    sub_compile_info, compile_cmd, section_content = _compile_core(
        compile_info,
        arch,
        code_type,
        compile_option_tuple,
        tiling_info,
        tiling_info.tiling_key,
    )
    new_sources += section_content
    new_sources = _finalize_and_write_sources(new_sources)
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    CommonUtility.run_cmd_inner(
        compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
    )

    _gen_mix_sub_json(sub_compile_info, tiling_info, code_type)
    mix_objs = [sub_compile_info.dst_file]
    _handle_mix_objs(compile_info, mix_objs, compile_info.dst_file)

    task_ration_str = "1:0" if code_type == CORE_TYPE_CUBE else "0:1"
    _gen_mix_json_from_seperate_json(
        compile_info.kernel_name, task_ration_str, code_type, True
    )
    set_soc_spec("AiCore")


def _compile_single_core_kernel(
    compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
):
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    arch = (
        f"dav-{chip_version}-cube"
        if kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY
        else f"dav-{chip_version}-vec"
    )
    sub_code_type = (
        "AIC" if kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY else "AIV"
    )
    optional_core = (
        "AiCore" if kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY else "VectorCore"
    )
    set_soc_spec(optional_core)

    compile_cmd, section_content = _get_compile_cmd_and_section_content(
        compile_info, arch, compile_option_tuple, tiling_info, tiling_info.tiling_key
    )
    new_sources += section_content
    new_sources = _finalize_and_write_sources(new_sources)
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    CommonUtility.run_cmd_inner(
        compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
    )
    _gen_non_mix_sub_json(compile_info, tiling_info, sub_code_type)


def _compile_ascendc_cce_v220_with_kernel_type_for_static(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for staic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)
    chip_version = CommonUtility.get_chip_version()
    new_sources = sources[:-1]
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]

    if _is_mix_aic_1x_kernel_type(kernel_type):
        _compile_mix_aic_1x_kernel(
            compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
        )
    elif _is_hard_sync_or_mix_1x_kernel_type(kernel_type):
        _compile_hard_sync_or_mix_1x_kernel(
            compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
        )
    elif _is_single_core_kernel_type(kernel_type):
        _compile_single_core_kernel(
            compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
        )


def _compile_ascendc_cce_v220_with_kernel_type_for_dynamic(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for dynamic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)
    chip_version = CommonUtility.get_chip_version()
    new_sources = sources[:-1]
    obj_files = []
    cmds_list_vec = []
    tiling_key_vec = []
    cmds_list_cube = []
    tiling_key_cube = []
    for tiling_key in compile_info.tiling_key_list:
        kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
        for core_type in get_compile_core_types(compile_info, tiling_key):
            code_type = CORE_TYPE_CUBE if core_type == "cube" else CORE_TYPE_VEC
            sub_compile_info = (
                _get_sub_compile_info(compile_info, code_type)
                if kernel_type.value >= 2
                else copy.deepcopy(compile_info)
            )
            sub_compile_info.dst_file = (
                sub_compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            )
            arch = f"dav-{chip_version}-{core_type}"
            compile_cmd, section_content = _get_compile_cmd_and_section_content(
                sub_compile_info, arch, compile_option_tuple, tiling_info, tiling_key
            )
            # Keep one command per physical core target for later Manifest replay.
            compile_info.compile_command_session.submit(
                KernelCompileCommand(
                    tiling_key=str(tiling_key),
                    compiled_symbol=sub_compile_info.last_compiled_symbol,
                    core_type=core_type,
                    source_path=sub_compile_info.gen_kernel_func_file,
                    argv=tuple(compile_cmd),
                    output_path=sub_compile_info.dst_file,
                )
            )
            if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
                compile_info.global_kernel_symbols.extend(
                    sub_compile_info.global_kernel_symbols
                )
            new_sources += section_content
            obj_files.append(sub_compile_info.dst_file)
            if code_type == CORE_TYPE_CUBE:
                cmds_list_cube.append(compile_cmd)
                tiling_key_cube.append(tiling_key)
            else:
                cmds_list_vec.append(compile_cmd)
                tiling_key_vec.append(tiling_key)
    new_sources += global_var_storage.get_variable("ascendc_meta_info")
    new_sources += "#endif\n"
    # add dfx info section to sourse file
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    # The rewritten wrapper is retained as a resource; no object exists in replay.
    if not compile_info.compile_command_session.should_execute:
        return

    if len(cmds_list_vec) != 0:
        compile_multi_tilingkey(
            tiling_key_vec,
            cmds_list_vec,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aiv",
            compile_info.compile_log_path,
        )

    if len(cmds_list_cube) != 0:
        compile_multi_tilingkey(
            tiling_key_cube,
            cmds_list_cube,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aic",
            compile_info.compile_log_path,
        )
    fatbin_objs(
        obj_files,
        compile_info.dst_file,
        compile_info.is_debug,
        compile_info.compile_log_path,
    )
    _generate_final_json(compile_info, tiling_info)


def _compile_ascendc_cce_v220_with_kernel_type(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file with kernel type

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    if tiling_info.static_shape_flag:
        _compile_ascendc_cce_v220_with_kernel_type_for_static(
            compile_info, compile_option_tuple, tiling_info
        )
    else:
        _compile_ascendc_cce_v220_with_kernel_type_for_dynamic(
            compile_info, compile_option_tuple, tiling_info
        )


def _compile_ascendc_cce_v200_with_kernel_type_for_static(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for staic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AICORE,
        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
    ]:
        # build Aicore
        set_soc_spec("AiCore")
        dst_file = compile_info.dst_file
        aicore_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_CUBE)
        arch = "dav-m200"
        call_bisheng_v200_static(
            aicore_compile_info, compile_option_tuple, tiling_info, arch, kernel_type
        )
        # build vector
        set_soc_spec("VectorCore")
        vec_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_VEC)
        arch = "dav-m200-vec"
        if kernel_type is KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE:
            compile_option_tuple.compile_options.append("-D__ENABLE_VECTOR_CORE__")
        call_bisheng_v200_static(
            vec_compile_info, compile_option_tuple, tiling_info, arch, kernel_type
        )
        # fatbin 2o->1o
        mix_objs = [aicore_compile_info.dst_file, vec_compile_info.dst_file]
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )
        # gen main json
        _gen_static_json_for_mix_v200(compile_info, tiling_info, kernel_type)
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_AICORE]:
        arch = "dav-m200"
        set_soc_spec("AiCore")
        call_bisheng_v200_static(
            compile_info, compile_option_tuple, tiling_info, arch, kernel_type
        )
        # gen json for v200
        _gen_static_json_for_no_mix_v200(compile_info, tiling_info, kernel_type)
    else:
        raise Exception("current kernel core type is not support")
    return


def _compile_ascendc_cce_v200_with_kernel_type_for_dynamic(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    final_kernel_type,
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for dynamic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    obj_files = []
    cmds_list_vec = []
    tiling_key_vec = []
    cmds_list_aicore = []
    tiling_key_aicore = []
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)

    new_sources = sources[:-1]
    for tiling_key in compile_info.tiling_key_list:
        kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
        if kernel_type in [
            KernelMetaType.KERNEL_TYPE_MIX_AICORE,
            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
        ]:
            # build Aicore
            set_soc_spec("AiCore")
            dst_file = compile_info.dst_file
            aicore_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_CUBE)
            arch = "dav-m200"
            param = SingleTilingKeyCompileParams(
                tiling_key,
                aicore_compile_info,
                arch,
                tiling_info,
                compile_info.code_channel,
                compile_option_tuple,
            )
            dst_file, compile_cmd, section_content = call_bisheng_v200_dynamic(
                param, kernel_type
            )
            new_sources += section_content
            cmds_list_aicore.append(compile_cmd)
            obj_files.append(dst_file)
            tiling_key_aicore.append(tiling_key)
            # build vector
            set_soc_spec("VectorCore")
            vec_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_VEC)
            arch = "dav-m200-vec"
            if kernel_type is KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE:
                compile_option_tuple.compile_options.append("-D__ENABLE_VECTOR_CORE__")
            param = SingleTilingKeyCompileParams(
                tiling_key,
                vec_compile_info,
                arch,
                tiling_info,
                compile_info.code_channel,
                compile_option_tuple,
            )
            dst_file, compile_cmd, section_content = call_bisheng_v200_dynamic(
                param, kernel_type
            )
            new_sources += section_content
            cmds_list_vec.append(compile_cmd)
            obj_files.append(dst_file)
            tiling_key_vec.append(tiling_key)
        elif kernel_type in [KernelMetaType.KERNEL_TYPE_AICORE]:
            arch = "dav-m200"
            set_soc_spec("AiCore")
            param = SingleTilingKeyCompileParams(
                tiling_key,
                compile_info,
                arch,
                tiling_info,
                compile_info.code_channel,
                compile_option_tuple,
            )
            dst_file, compile_cmd, section_content = call_bisheng_v200_dynamic(
                param, kernel_type
            )
            new_sources += section_content
            cmds_list_aicore.append(compile_cmd)
            obj_files.append(dst_file)
            tiling_key_aicore.append(tiling_key)
        else:
            raise Exception("current kernel core type is not support")
            # gen main json
    new_sources += "#endif\n"
    # add dfx info section to sourse file
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    if len(cmds_list_vec) != 0:
        compile_multi_tilingkey(
            tiling_key_vec,
            cmds_list_vec,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aiv",
            compile_info.compile_log_path,
        )

    if len(cmds_list_aicore) != 0:
        compile_multi_tilingkey(
            tiling_key_aicore,
            cmds_list_aicore,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aic",
            compile_info.compile_log_path,
        )

    fatbin_objs(
        obj_files,
        compile_info.dst_file,
        compile_info.is_debug,
        compile_info.compile_log_path,
    )
    _gen_dynamic_json_for_v200(compile_info, tiling_info, final_kernel_type)
    return


def _compile_ascendc_cce_v200_with_kernel_type(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    from .ascendc_compile_v200 import judge_valid_for_v200

    final_kernel_type = judge_valid_for_v200(compile_info.tiling_key_kernel_type)
    if tiling_info.static_shape_flag:
        _compile_ascendc_cce_v200_with_kernel_type_for_static(
            compile_info, compile_option_tuple, tiling_info
        )
    else:
        _compile_ascendc_cce_v200_with_kernel_type_for_dynamic(
            compile_info, compile_option_tuple, tiling_info, final_kernel_type
        )


def _compile_ascendc_cce_v220(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    chip_version = CommonUtility.get_chip_version()
    if compile_info.code_channel == CORE_TYPE_MIX:
        # build cube
        set_soc_spec("AiCore")
        dst_file = compile_info.dst_file
        cube_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_CUBE)
        arch = f"dav-{chip_version}-cube"
        tiling_key_list = call_bisheng_v220(
            cube_compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.extend(
                cube_compile_info.global_kernel_symbols
            )
        compile_info.compile_command_session.extend(
            cube_compile_info.compile_command_session.records
        )
        if compile_info.compile_command_session.should_execute:
            _gen_mix_sub_json(cube_compile_info, tiling_info, CORE_TYPE_CUBE)
        # build vector
        set_soc_spec("VectorCore")
        vec_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_VEC)
        arch = f"dav-{chip_version}-vec"
        call_bisheng_v220(
            vec_compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.extend(
                vec_compile_info.global_kernel_symbols
            )
        compile_info.compile_command_session.extend(
            vec_compile_info.compile_command_session.records
        )
        # Record-only MIX replay merges both command streams and restores AiCore.
        if not compile_info.compile_command_session.should_execute:
            set_soc_spec("AiCore")
            return
        # fatbin 2o->1o
        mix_objs = [cube_compile_info.dst_file, vec_compile_info.dst_file]
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )
        # gen main json
        task_ration_str = f"1:{tiling_info.task_ration}"
        _gen_mix_json_from_seperate_json(
            compile_info.kernel_name, task_ration_str, CORE_TYPE_CUBE, True
        )
        set_soc_spec("AiCore")
    elif compile_info.hard_sync and compile_info.code_channel in [
        CORE_TYPE_VEC,
        CORE_TYPE_CUBE,
    ]:
        dst_file = compile_info.dst_file
        single_side_compile_info = _get_sub_compile_info(
            compile_info, compile_info.code_channel
        )
        arch = (
            f"dav-{chip_version}-vec"
            if compile_info.code_channel == CORE_TYPE_VEC
            else f"dav-{chip_version}-cube"
        )
        tiling_key_list = call_bisheng_v220(
            single_side_compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.extend(
                single_side_compile_info.global_kernel_symbols
            )
        compile_info.compile_command_session.extend(
            single_side_compile_info.compile_command_session.records
        )
        # No JSON or fatbin is produced when only the command plan is requested.
        if not compile_info.compile_command_session.should_execute:
            set_soc_spec("AiCore")
            return
        _gen_mix_sub_json(
            single_side_compile_info, tiling_info, compile_info.code_channel
        )
        mix_objs = [single_side_compile_info.dst_file]
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )
        # gen main json
        task_ration_str = (
            "1:0" if compile_info.code_channel == CORE_TYPE_CUBE else "0:1"
        )
        _gen_mix_json_from_seperate_json(
            compile_info.kernel_name, task_ration_str, compile_info.code_channel, True
        )
        set_soc_spec("AiCore")
    else:
        arch, sub_core_type, optional_core = get_core_info(compile_info)
        set_soc_spec(optional_core)
        tiling_key_list = call_bisheng_v220(
            compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        # call_bisheng_v220 already recorded the command and generated wrapper.
        if not compile_info.compile_command_session.should_execute:
            set_soc_spec("AiCore")
            return
        _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
    if not tiling_info.static_shape_flag:
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )


def get_core_info(compile_info: CompileInfo):
    chip_version = CommonUtility.get_chip_version()
    if compile_info.code_channel == CORE_TYPE_CUBE:
        arch = f"dav-{chip_version}-cube"
        sub_core_type = "AIC"
        optional_core = "AiCore"
        return arch, sub_core_type, optional_core
    elif compile_info.code_channel == CORE_TYPE_VEC:
        arch = f"dav-{chip_version}-vec"
        sub_core_type = "AIV"
        optional_core = "VectorCore"  # do the same work with SetOptionalCoreType in cpp
        return arch, sub_core_type, optional_core
    else:
        raise Exception(f"invalid code_channel = {compile_info.code_channel}")


def _compile_ascendc_cce_m510(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sub_core_type = "AIC"
    optional_core = "AiCore"
    arch = None
    set_soc_spec(optional_core)
    tiling_key_list = call_bisheng_v220(
        compile_info, compile_option_tuple, tiling_info, arch, compile_info.code_channel
    )
    _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
    if not tiling_info.static_shape_flag:
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )


def _compile_ascendc_cce_regbase(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    soc_arch_map = {"Ascend310B": "dav-m300", "Ascend610Lite": "dav-m310"}
    arch = soc_arch_map.get(
        global_var_storage.get_variable("ascendc_short_soc_version")
    )
    value = get_soc_spec("cube_vector_combine")
    value_str_list = value.split(",")
    enable_mix_for_profiling = False
    if (
        value_str_list[0] == "unknown"
        or ("fuse" in value_str_list and len(value_str_list)) == 1
    ):
        enable_mix_for_profiling = True
    if enable_mix_for_profiling:
        sub_core_type = "AIC"
        optional_core = "AiCore"
    else:
        sub_core_type = "AIV"
        optional_core = "VectorCore"  # do the same work with SetOptionalCoreType in cpp
    set_soc_spec(optional_core)
    tiling_key_list = _call_bisheng_regbase(
        compile_info, compile_option_tuple, tiling_info, arch, compile_info.code_channel
    )
    _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
    if not tiling_info.static_shape_flag:
        _dynamic_regbase_kernel_list_to_json(
            compile_info.kernel_name,
            tiling_key_list,
            compile_info.enable_deterministic,
            enable_mix_for_profiling,
            compile_info.tiling_key_deterministic,
        )
    else:
        _static_regbase_kernel_list_to_json(compile_info.kernel_name)


def _get_sub_compile_info(compile_info: CompileInfo, core_type: int):
    sub_compile_info = copy.deepcopy(compile_info)
    core_type_marker = "_mix_aic" if core_type == CORE_TYPE_CUBE else "_mix_aiv"
    # i.e. change demo_kernel.o to demo_kernel_mix_aic.o
    sub_compile_info.dst_file = (
        compile_info.dst_file[:-2] + core_type_marker + compile_info.dst_file[-2:]
    )
    sub_compile_info.kernel_name = compile_info.kernel_name + core_type_marker
    sub_compile_info.sub_core_type = core_type
    # Clear global_kernel_symbols to avoid accumulation from parent compile_info
    sub_compile_info.global_kernel_symbols = []
    # Child core compilation inherits the mode without sharing recorded commands.
    sub_compile_info.compile_command_session = (
        compile_info.compile_command_session.fork()
    )
    return sub_compile_info


def _gen_compile_cmd_regbase(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    sub_arch: str,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the V300 compiler.
    :param src_file: the source file
    :param dst_file: the destination file
    :param extra_options: the extra options
    :param with_tiling_file: whether with the tiling file
    :return: the compile command
    """
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        compile_cmd = [
            os.environ.get("ASCENDC_CCACHE_EXECUTABLE"),
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
        ]
    else:
        compile_cmd = [
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
        ]

    for option in compile_option_tuple.compile_options:
        compile_cmd += [option]
    compile_cmd += [
        src_file,
        "--cce-aicore-arch=%s" % sub_arch,
        "--cce-aicore-only",
        "-o",
        dst_file,
        "-mllvm",
        "-cce-aicore-function-stack-size=16000",
        "--cce-long-call=true",
        "-mllvm",
        "-cce-aicore-addr-transform",
        "-mllvm",
        "--cce-aicore-or-combine=false",
        "-mllvm",
        "-instcombine-code-sinking=false",
        "-mllvm",
        "-cce-aicore-jump-expand=false",
        "-mllvm",
        "-cce-aicore-mask-opt=false",
    ]
    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]

    if with_tiling_file:
        compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    return compile_cmd


def _compile_single_tiling_regbase(param: SingleTilingKeyCompileParams):
    dst_file = param.compile_info.dst_file[:-2] + "_%s.o" % param.tiling_key
    compile_cmd = _gen_compile_cmd_regbase(
        param.compile_info.gen_kernel_func_file,
        dst_file,
        param.compile_option_tuple,
        param.sub_arch,
        param.tiling_info.tiling_data_file_path,
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={param.tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = param.tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = param.tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{param.compile_info.origin_func_name}="
            f"{param.compile_info.origin_func_name}_{tiling_data_hash}_{param.tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [
            f"-D{param.compile_info.origin_func_name}="
            f"{param.compile_info.origin_func_name}_{param.tiling_key}_tilingkey"
        ]
    kernel_func_name = param.compile_info.kernel_name + "_%s" % param.tiling_key
    compile_cmd += [
        f"-Dauto_gen_{param.compile_info.origin_func_name}_kernel={kernel_func_name}"
    ]
    section_content = DFXSectionGenerator().generate_dfx_section(
        param.tiling_key, param.tiling_info, kernel_func_name, param.compile_info, True
    )
    return compile_cmd, section_content


def _mssanitizer_link(src_file, dst_file, compile_log_path=None):
    """Build the mssanitize link command before link.
    Parameters
    ----------
    src_file : str
        The src object file.

    dst_file : str
        The dst object file.
    """
    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    if short_soc_version not in global_var_storage.get_variable(
        "ascendc_asan_obj_path"
    ):
        raise Exception("asan config file not support asan.a path")
    asan_obj_paths = global_var_storage.get_variable("ascendc_asan_obj_path")[
        short_soc_version
    ]
    if asan_obj_paths == []:
        return
    if not isinstance(src_file, list):
        src_file = [src_file]
    cmd = [CCECInfo.get_exe("ld.lld"), "-m", "aicorelinux", "-r", "-Ttext=0"]
    cmd.extend(src_file)
    cmd.extend(["--dependent-libraries"])
    cmd.extend(asan_obj_paths)
    cmd.extend(
        [
            "-r",
            "-o",
            "%s" % dst_file,
        ]
    )
    CommonUtility.run_cmd_inner(cmd, CompileStage.FATBIN, compile_log_path)


def _call_bisheng_regbase(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    sub_arch: str,
    code_channel: int,
):
    """generate bisheng cmd instead of _build_aicore_compile_cmd, since tbe set davinci-m300-{sub_core} in build_cce.cc

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
        sub_arch (str): m300 arch info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)

    new_sources = sources[:-1]
    if tiling_info.static_shape_flag:
        compile_cmd = _gen_compile_cmd(
            compile_info.gen_kernel_func_file,
            compile_info.dst_file,
            compile_option_tuple,
            tiling_info.tiling_data_file_path,
        )
        # tbe-pass add "__kernel0" in tbe-codegen and json, we use -D to change function name
        compile_cmd += [
            f"-Dauto_gen_{compile_info.origin_func_name}_kernel={compile_info.get_kernel_func_name()}"
        ]
        if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
            tiling_data_hash_src = tiling_info.tiling_data
            if isinstance(tiling_data_hash_src, str):
                tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
            elif not tiling_data_hash_src:
                tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
            tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
            compile_cmd += [
                f"-D{compile_info.origin_func_name}="
                f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_info.tiling_key}_tilingkey"
            ]
        compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_info.tiling_key}UL"]
        new_sources += DFXSectionGenerator().generate_dfx_section(
            str(tiling_info.tiling_key),
            tiling_info,
            compile_info.get_kernel_func_name(),
            compile_info,
            True,
        )
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )

        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
        target = "cce_core"
        core_type_info = {var("core_type"): var("")}

        tvm_callback_cce_postproc(
            target,
            compile_info.kernel_name,
            tiling_info.block_num,
            0,
            "",
            None,
            None,
            core_type_info,
            None,
            None,
            False,
            1,
            None,
            None,
        )
    else:
        obj_files = []
        for tiling_key in compile_info.tiling_key_list:
            dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            obj_files.append(dst_file)
        cmds_list = []
        for tiling_key in compile_info.tiling_key_list:
            param = SingleTilingKeyCompileParams(
                tiling_key,
                compile_info,
                sub_arch,
                tiling_info,
                code_channel,
                compile_option_tuple,
            )
            compile_cmd, section_content = _compile_single_tiling_regbase(param)
            cmds_list.append(compile_cmd)
            new_sources += section_content
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )
        # compile binary
        compile_multi_tilingkey(
            compile_info.tiling_key_list,
            cmds_list,
            os.path.basename(compile_info.dst_file)[:-2],
            compile_info.compile_log_path,
        )
        fatbin_objs(
            obj_files,
            compile_info.dst_file,
            compile_info.is_debug,
            compile_info.compile_log_path,
        )
        return compile_info.tiling_key_list


def replay_op(
    op_info: OpInfo,
    entry_obj: str,
    code_channel: int,
    src_file: str,
    compile_options: list,
):
    """replay_op feature is at sunset"""
    return True, "success"


def get_code_channel(
    src_file: str, kernel_name: str, optype: str, compile_options_input: list = None
):
    # replay function needs, so it is reserved
    return CORE_TYPE_MIX
