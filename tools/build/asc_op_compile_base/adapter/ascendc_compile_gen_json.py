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
ascendc compile gen json
"""

import os
import stat
import json
from tbe.common.buildcfg import get_current_build_config
from tbe.common.buildcfg.buildcfg_mapping import enable_vector_core
from tbe.common.platform.platform_info import set_soc_spec
from tbe.tvm.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from tbe.tvm.runtime.cce_runtime import tvm_callback_cce_postproc
from tbe.tvm import var
from .log_utils import LogUtil, AscendCLogLevel
from .ascendc_common_utility import CommonUtility, CompileInfo
from .ascendc_constants import CORE_TYPE_CUBE, CORE_TYPE_VEC, CORE_TYPE_MIX
from .get_op_tiling import TilingInfo
from .ascendc_constants import KernelMetaType
from .super_kernel_sub_op_compile import save_kernel_type
from .global_storage import global_var_storage
from .ascendc_identify_meta_section_info import check_op_type_is_simt


def _get_kernel_type_dict(compile_info: CompileInfo, tiling_key: int):
    kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
    tiling_key_dict = {}
    tiling_key_dict["tilingKey"] = int(tiling_key)
    if kernel_type.value == 0:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["taskRation"] = "0:1"
        tiling_key_dict["crossCoreSync"] = 0
    elif kernel_type.value == 1:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["taskRation"] = "1:0"
        tiling_key_dict["crossCoreSync"] = 0
    elif kernel_type.value == 2:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["crossCoreSync"] = 1
        tiling_key_dict["taskRation"] = "0:1"
    elif kernel_type.value == 3:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["crossCoreSync"] = 1
        tiling_key_dict["taskRation"] = "1:0"
    elif kernel_type.value == 4:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["crossCoreSync"] = 1
        tiling_key_dict["taskRation"] = "0:1"
    elif kernel_type.value == 5:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["crossCoreSync"] = 1
        tiling_key_dict["taskRation"] = "1:0"
    elif kernel_type.value == 6:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["crossCoreSync"] = 1
        tiling_key_dict["taskRation"] = "1:1"
    elif kernel_type.value == 7:
        tiling_key_dict["kernelType"] = "MIX_AIC"
        tiling_key_dict["crossCoreSync"] = 1
        tiling_key_dict["taskRation"] = "1:2"
    elif kernel_type.value == 8:
        tiling_key_dict["kernelType"] = "AiCore"
        tiling_key_dict["taskRation"] = "1:0"
    elif kernel_type.value == 9:
        tiling_key_dict["kernelType"] = "VectorCore"
        tiling_key_dict["taskRation"] = "0:1"
    elif kernel_type.value == 10:
        tiling_key_dict["kernelType"] = "MIX_AICORE"
        tiling_key_dict["taskRation"] = "1:1"
    elif kernel_type.value == 11:
        tiling_key_dict["kernelType"] = "MIX_VECTOR_CORE"
        tiling_key_dict["taskRation"] = "1:1"
    else:
        raise Exception(f"current kernel type is not suport {kernel_type}")
    return tiling_key_dict


def _gen_mix_json_from_seperate_json(
    kernel_name: str, task_ration_str: str, core_type: int, no_set_kernel_type: bool
):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    core_type_marker = "_mix_aic" if core_type == CORE_TYPE_CUBE else "_mix_aiv"
    seperate_json_path = os.path.join(
        kernel_meta_path, kernel_name + f"{core_type_marker}.json"
    )
    mix_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
    os.rename(seperate_json_path, mix_json_path)
    try:
        with open(mix_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["binFileName"] = kernel_name
    js["kernelName"] = kernel_name
    js["coreType"] = "MIX"
    if no_set_kernel_type is True:
        js["taskRation"] = task_ration_str
    try:
        with open(mix_json_path, "w") as fd_write:
            os.chmod(mix_json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _gen_mix_json_from_seperate_json_for_kernel_type(
    kernel_name: str, task_ration_str: str, core_type: int, no_set_kernel_type: bool
):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
    try:
        with open(json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["binFileName"] = kernel_name
    js["kernelName"] = kernel_name
    js["coreType"] = "MIX"
    if no_set_kernel_type is True:
        js["taskRation"] = task_ration_str
    try:
        with open(json_path, "w") as fd_write:
            os.chmod(json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _dynamic_kernel_list_to_json(
    kernel_name: str,
    tiling_key_list: list,
    enable_deterministic: bool,
    tiling_key_deterministic: dict,
):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    dynamic_kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
    try:
        with open(dynamic_kernel_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["kernelName"] = kernel_name
    js["kernelList"] = []
    for tiling_key in tiling_key_list:
        if tiling_key in tiling_key_deterministic:
            js["kernelList"].append(
                {
                    "deterministic": tiling_key_deterministic[tiling_key],
                    "kernelName": kernel_name + "_" + tiling_key,
                }
            )
        elif enable_deterministic:
            if get_current_build_config("enable_deterministic_mode") == 1:
                js["kernelList"].append(
                    {
                        "deterministic": "true",
                        "kernelName": kernel_name + "_" + tiling_key,
                    }
                )
            else:
                js["kernelList"].append(
                    {
                        "deterministic": "false",
                        "kernelName": kernel_name + "_" + tiling_key,
                    }
                )
        else:
            js["kernelList"].append({"kernelName": kernel_name + "_" + tiling_key})
    try:
        with open(dynamic_kernel_json_path, "w") as fd_write:
            os.chmod(dynamic_kernel_json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _dynamic_regbase_kernel_list_to_json(
    kernel_name: str,
    tiling_key_list: list,
    enable_deterministic: bool,
    enable_mix_for_profiling: bool,
    tiling_key_deterministic: dict,
):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    dynamic_kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
    try:
        with open(dynamic_kernel_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["kernelName"] = kernel_name
    js["kernelList"] = []
    js["magic"] = "RT_DEV_BINARY_MAGIC_ELF"
    if enable_mix_for_profiling:
        js["magic"] = "RT_DEV_BINARY_MAGIC_ELF"
    for tiling_key in tiling_key_list:
        if tiling_key in tiling_key_deterministic:
            js["kernelList"].append(
                {
                    "deterministic": tiling_key_deterministic[tiling_key],
                    "kernelName": kernel_name + "_" + tiling_key,
                }
            )
        elif enable_deterministic:
            if get_current_build_config("enable_deterministic_mode") == 1:
                js["kernelList"].append(
                    {
                        "deterministic": "true",
                        "kernelName": kernel_name + "_" + tiling_key,
                    }
                )
            else:
                js["kernelList"].append(
                    {
                        "deterministic": "false",
                        "kernelName": kernel_name + "_" + tiling_key,
                    }
                )
        else:
            js["kernelList"].append({"kernelName": kernel_name + "_" + tiling_key})
    try:
        with open(dynamic_kernel_json_path, "w") as fd_write:
            os.chmod(dynamic_kernel_json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _static_regbase_kernel_list_to_json(kernel_name: str):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
    try:
        with open(kernel_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["magic"] = "RT_DEV_BINARY_MAGIC_ELF"
    try:
        with open(kernel_json_path, "w") as fd_write:
            os.chmod(kernel_json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _gen_mix_sub_json(
    compile_info: CompileInfo, tiling_info: TilingInfo, core_type: int = CORE_TYPE_CUBE
):
    """generate cube/vector json file in code_channel_mix

    Args:
        compile_info (CompileInfo): compile variables to call bisheng
        tiling_info (TilingInfo): variables got from tiling
    """
    target = "cce_core"
    # in tik1 and tbe, we user "parameters" in json file to tell rts initialization workspace data
    # in AscendC, operator developers do initialization work, so atomic_args should be empty
    atomic_args = ""
    # json info is used to infer "pragma_json_info_deterministic", AscendC not supported
    json_info = {}
    json_info_tuple = {}
    # core_type_info, should be "" in code_channel_mix
    core_type_info = {var("core_type"): var("")}
    # kernel_list and kernel_list_deterministic is for fatbin
    kernel_list = None
    kernel_list_deterministic = None
    # AscendC mix-channel need ffts sync, tbe use MultiCoreSync visit stmt to find if exists op->call_name == "st_dev"
    is_ffts_id_needed = True
    subblocknum: int = tiling_info.task_ration
    # AscendC only support cube:vector = 1:1 or 1:2, so mix_type should be kAicMix
    mix: str = "MIX"
    mix_type_info: str = "aic_mix" if core_type == CORE_TYPE_CUBE else "aiv_mix"
    tvm_callback_cce_postproc(
        target,
        compile_info.kernel_name,
        tiling_info.block_num,
        0,
        atomic_args,
        json_info,
        json_info_tuple,
        core_type_info,
        kernel_list,
        kernel_list_deterministic,
        is_ffts_id_needed,
        subblocknum,
        mix,
        mix_type_info,
    )


def _gen_static_json_for_no_mix_v200(
    compile_info: CompileInfo, tiling_info: TilingInfo, kernel_type
):
    target = "cce_core"
    tvm_callback_cce_postproc(target, compile_info.kernel_name, tiling_info.block_num)
    # if enable_vector_core, json has _mix_aic suffix
    if get_current_build_config(enable_vector_core):
        kernel_meta_path = CommonUtility.get_kernel_meta_dir()
        kernel_name = compile_info.kernel_name
        kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
        core_type_marker = (
            "_mix_aic"
            if kernel_type is KernelMetaType.KERNEL_TYPE_AICORE
            else "_mix_aiv"
        )
        seperate_json_path = os.path.join(
            kernel_meta_path, kernel_name + f"{core_type_marker}.json"
        )
        os.rename(seperate_json_path, kernel_json_path)


def _gen_non_mix_sub_json(
    compile_info: CompileInfo, tiling_info: TilingInfo, sub_core_type: str
):
    """generate json file if operator code only has cube or vector code in v220

    Args:
        compile_info (CompileInfo): compile variables to call bisheng
        tiling_info (TilingInfo): variables got from tiling
        sub_core_type (str): core_type_info, should be AIV/AIC in code_channel_aiv/aic
    """
    target = "cce_core"
    # in tik1 and tbe, we user "parameters" in json file to tell rts initialization workspace data
    # in AscendC, operator developers do initialization work, so atomic_args should be empty
    atomic_args = ""
    # json info is used to infer "pragma_json_info_deterministic", AscendC not supported
    json_info = {}
    json_info_tuple = {}
    core_type_info = {var("core_type"): var(sub_core_type)}
    # kernel_list and kernel_list_deterministic is for fatbin
    kernel_list = None
    kernel_list_deterministic = None
    is_ffts_id_needed = False
    subblocknum: int = tiling_info.task_ration
    # AscendC only support cube:vector = 1:1 or 1:2, so mix_type should be kAicMix
    mix: str = ""
    mix_type_info: str = ""
    tvm_callback_cce_postproc(
        target,
        compile_info.kernel_name,
        tiling_info.block_num,
        0,
        atomic_args,
        json_info,
        json_info_tuple,
        core_type_info,
        kernel_list,
        kernel_list_deterministic,
        is_ffts_id_needed,
        subblocknum,
        mix,
        mix_type_info,
    )


def _gen_static_json_for_mix_v200(
    compile_info: CompileInfo, tiling_info: TilingInfo, kernel_type
):
    set_soc_spec("AiCore")
    target = "cce_core"
    tvm_callback_cce_postproc(target, compile_info.kernel_name, tiling_info.block_num)

    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    kernel_name = compile_info.kernel_name
    kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")

    # if enable_vector_core, json has _mix_aic suffix
    if get_current_build_config(enable_vector_core):
        core_type_marker = "_mix_aic"
        seperate_json_path = os.path.join(
            kernel_meta_path, kernel_name + f"{core_type_marker}.json"
        )
        os.rename(seperate_json_path, kernel_json_path)

    try:
        with open(kernel_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["binFileName"] = kernel_name
    js["kernelName"] = kernel_name
    if kernel_type is KernelMetaType.KERNEL_TYPE_MIX_AICORE:
        js["coreType"] = "MIX_AICORE"
    elif kernel_type is KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE:
        js["coreType"] = "MIX_VECTOR_CORE"
    else:
        raise Exception("kernel_type is not support")
    try:
        with open(kernel_json_path, "w") as fd_write:
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _dynamic_kernel_list_to_json_for_kernel_type_one(
    compile_info: CompileInfo,
    kernel_name: str,
    tiling_key: str,
    enable_deterministic: bool,
    final_kernel_type: int,
):
    tiling_key_dict = {}
    if final_kernel_type != 0x1 and final_kernel_type != 0x2:
        tiling_key_dict = _get_kernel_type_dict(compile_info, tiling_key)
    if tiling_key in compile_info.tiling_key_deterministic:
        tiling_key_dict["deterministic"] = compile_info.tiling_key_deterministic[
            tiling_key
        ]
        tiling_key_dict["kernelName"] = kernel_name + "_" + tiling_key
    elif enable_deterministic:
        if get_current_build_config("enable_deterministic_mode") == 1:
            tiling_key_dict["deterministic"] = "true"
            tiling_key_dict["kernelName"] = kernel_name + "_" + tiling_key
        else:
            tiling_key_dict["deterministic"] = "false"
            tiling_key_dict["kernelName"] = kernel_name + "_" + tiling_key
    else:
        tiling_key_dict["kernelName"] = kernel_name + "_" + tiling_key
    return tiling_key_dict


def _dynamic_kernel_list_to_json_for_kernel_type(
    compile_info: CompileInfo,
    kernel_name: str,
    tiling_key_list: list,
    enable_deterministic: bool,
    final_kernel_type: int,
):
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    dynamic_kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")
    try:
        with open(dynamic_kernel_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )
    js["kernelName"] = kernel_name
    js["kernelList"] = []
    if final_kernel_type != 0x1 and final_kernel_type != 0x2:
        js["taskRation"] = "tilingKey"
    for tiling_key in tiling_key_list:
        tiling_key_dict = _dynamic_kernel_list_to_json_for_kernel_type_one(
            compile_info,
            kernel_name,
            tiling_key,
            enable_deterministic,
            final_kernel_type,
        )
        js["kernelList"].append(tiling_key_dict)
        if compile_info.tiling_key_group_map is not None:
            if tiling_key in compile_info.tiling_key_group_map.keys():
                for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                    tiling_key_dict_slave = (
                        _dynamic_kernel_list_to_json_for_kernel_type_one(
                            compile_info,
                            kernel_name,
                            tiling_key_slave,
                            enable_deterministic,
                            final_kernel_type,
                        )
                    )
                    js["kernelList"].append(tiling_key_dict_slave)

    try:
        with open(dynamic_kernel_json_path, "w") as fd_write:
            os.chmod(dynamic_kernel_json_path, stat.S_IRUSR + stat.S_IWUSR)
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )


def _gen_dynamic_json_for_v200(
    compile_info: CompileInfo, tiling_info: TilingInfo, final_kernel_type: str
):
    """according to the kernel type of each tiling key, get the finel kernel type
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        tiling_info (TilingInfo): tiling info
    """
    if final_kernel_type == "VectorCore":
        set_soc_spec("VectorCore")
    else:
        set_soc_spec("AiCore")
    target = "cce_core"
    tvm_callback_cce_postproc(target, compile_info.kernel_name, tiling_info.block_num)
    kernel_meta_path = CommonUtility.get_kernel_meta_dir()
    kernel_name = compile_info.kernel_name
    kernel_json_path = os.path.join(kernel_meta_path, kernel_name + ".json")

    # if enable_vector_core, json has _mix_aic suffix
    if get_current_build_config(enable_vector_core):
        core_type_marker = "_mix_aic"
        seperate_json_path = os.path.join(
            kernel_meta_path, kernel_name + f"{core_type_marker}.json"
        )
        os.rename(seperate_json_path, kernel_json_path)
    try:
        with open(kernel_json_path, "r") as fd:
            js = json.load(fd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("read json file failed, reason is:", err)
        )

    js["binFileName"] = kernel_name
    js["kernelName"] = kernel_name
    js["coreType"] = final_kernel_type

    try:
        with open(kernel_json_path, "w") as fd_write:
            json.dump(js, fd_write, indent=2)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write json file failed, reason is:", err)
        )

    _dynamic_kernel_list_to_json_for_kernel_type(
        compile_info,
        compile_info.kernel_name,
        compile_info.tiling_key_list,
        compile_info.enable_deterministic,
        final_kernel_type,
    )


def _generate_final_json(compile_info: CompileInfo, tiling_info: TilingInfo):
    """according to the kernel type of each tiling key, get the finel kernel type
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        tiling_info (TilingInfo): tiling info
    """
    final_kernel_type = 0
    for tiling_key in compile_info.tiling_key_list:
        kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
        if kernel_type == KernelMetaType.KERNEL_TYPE_AIV_ONLY:
            final_kernel_type = final_kernel_type | 0x1
        elif kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY:
            final_kernel_type = final_kernel_type | 0x2
        elif (
            kernel_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC
            or kernel_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
        ):
            final_kernel_type = final_kernel_type | 0x4
        else:
            final_kernel_type = final_kernel_type | 0x8
    if final_kernel_type == 0x1:
        sub_core_type = "AIV"
        optional_core = "VectorCore"
        set_soc_spec(optional_core)
        _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
        save_kernel_type("KERNEL_TYPE_AIV_ONLY")
    elif final_kernel_type == 0x2:
        sub_core_type = "AIC"
        optional_core = "AiCore"
        set_soc_spec(optional_core)
        _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
        save_kernel_type("KERNEL_TYPE_AIC_ONLY")
    elif final_kernel_type == 0x4 or final_kernel_type == 0x5:
        set_soc_spec("AiCore")
        _gen_mix_sub_json(compile_info, tiling_info, CORE_TYPE_VEC)
        task_ration_str = "0:1"
        _gen_mix_json_from_seperate_json_for_kernel_type(
            compile_info.kernel_name, task_ration_str, CORE_TYPE_VEC, False
        )
        save_kernel_type("KERNEL_TYPE_MIX_AIC_1_2")
    else:
        set_soc_spec("AiCore")
        _gen_mix_sub_json(compile_info, tiling_info, CORE_TYPE_CUBE)
        task_ration_str = "1:0"
        _gen_mix_json_from_seperate_json_for_kernel_type(
            compile_info.kernel_name, task_ration_str, CORE_TYPE_CUBE, False
        )
        save_kernel_type("KERNEL_TYPE_MIX_AIC_1_2")
    if not tiling_info.static_shape_flag:
        _dynamic_kernel_list_to_json_for_kernel_type(
            compile_info,
            compile_info.kernel_name,
            compile_info.tiling_key_list,
            compile_info.enable_deterministic,
            final_kernel_type,
        )


def _get_simt_type_in_staic(
    tiling_info: TilingInfo, compile_info: CompileInfo, obj_path
):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        return False
    if tiling_info.static_shape_flag is False:
        LogUtil.print_compile_log(
            compile_info.kernel_name,
            "non static scenarios \
not support detecting SIMT type",
            AscendCLogLevel.LOG_INFO,
        )
        return False
    if compile_info.kernel_name.startswith("te_superkernel"):
        LogUtil.print_compile_log(
            compile_info.kernel_name,
            "current op is superkernel, \
no need to detect SIMT type",
            AscendCLogLevel.LOG_INFO,
        )
        return False
    vec_marker = "_mix_aiv"
    if compile_info.no_set_kernel_type is False:
        kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
        if kernel_type in [KernelMetaType.KERNEL_TYPE_AIV_ONLY]:
            kernel_name = compile_info.get_kernel_func_name()
            return check_op_type_is_simt(obj_path, kernel_name)
        elif kernel_type in [
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
        ]:
            kernel_name = compile_info.kernel_name + vec_marker
            return check_op_type_is_simt(obj_path, kernel_name)
    else:
        if compile_info.code_channel == CORE_TYPE_MIX:
            kernel_name = compile_info.kernel_name + vec_marker
            return check_op_type_is_simt(obj_path, kernel_name)
        elif compile_info.code_channel == CORE_TYPE_VEC:
            if compile_info.hard_sync:
                kernel_name = compile_info.kernel_name + vec_marker
            else:
                kernel_name = compile_info.get_kernel_func_name()
            return check_op_type_is_simt(obj_path, kernel_name)

    return False
