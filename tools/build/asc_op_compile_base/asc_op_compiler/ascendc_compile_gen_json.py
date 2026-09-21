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
from asc_op_compile_base.common.buildcfg import get_current_build_config
from asc_op_compile_base.common.buildcfg.buildcfg_mapping import enable_vector_core
from asc_op_compile_base.common.platform.platform_info import set_soc_spec
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)
from asc_op_compile_base.asc_op_compiler.cce_runtime import tvm_callback_cce_postproc
from asc_op_compile_base.common.utils.log_utils import LogUtil, AscendCLogLevel
from .ascendc_common_utility import CommonUtility, CompileInfo
from .ascendc_constants import CORE_TYPE_CUBE, CORE_TYPE_VEC, CORE_TYPE_MIX
from .get_op_tiling import TilingInfo
from .ascendc_constants import KernelMetaType
from .super_kernel_sub_op_compile import save_kernel_type
from .global_storage import global_var_storage
from .ascendc_identify_meta_section_info import check_op_type_is_simt
import hashlib

from asc_op_compile_base.common.context import get_context
from .get_op_tiling import OpInfo
from .ascendc_constants import COMPILE_INFO_KEY, GEN_PLACE_HOLDER_STR
from .ascendc_compile_dfx import DFXSectionGenerator
from .ascendc_kernel_feature_manager import global_ascendc_kernel_feature_manager
from .super_kernel_constants import SuperKernelStreamFusionMode
from .super_kernel_sub_op_compile import add_sub_super_kernel_info
from .ascendc_compile_gen_code import _get_tiling_struct_size, delete_tiling_section


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
    core_type_info = {"core_type": ""}
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
    core_type_info = {"core_type": sub_core_type}
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
            notify_before_call_event_list = compile_info.super_kernel_info.get(
                "notify_before_call_event_list", []
            )
            if notify_before_call_event_list and notify_before_call_event_list[i]:
                sub_dfx_info["notify_before_call_event_list"] = (
                    notify_before_call_event_list[i]
                )
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
            if notify_before_call_event_list and notify_before_call_event_list[i]:
                arg_list["notify_before_call_param_offset"] = (
                    compile_info.super_kernel_info["notify_before_call_param_offset"][i]
                )
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
