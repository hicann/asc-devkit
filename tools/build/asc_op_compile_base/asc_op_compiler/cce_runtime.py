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
from __future__ import absolute_import as _abs

import hashlib
import json
import os
import stat
import threading

from asc_op_compile_base.common import ccec
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)

# save tik gm tensor whether is workspace in to a list
TIK_WORKSPACE_SIZE_LIST = threading.local()
TIK_WORKSPACE_SIZE_LIST.local_list = []

# Example: pylint: disable=invalid-name
# save tik gm tensor whether is atomic add in to a list
TIK_ATOMIC_ADD_LIST = threading.local()
TIK_ATOMIC_ADD_LIST.local_list = []

# save tbe gm tensor whether is workspace in to a list
TBE_WORKSPACE_SIZE_LIST = threading.local()
TBE_WORKSPACE_SIZE_LIST.local_list = []
TBE_WORKSPACE_IND_LIST = threading.local()
TBE_WORKSPACE_IND_LIST.local_list = []
TBE_ATUO_ATOMIC_IND_LIST = threading.local()
TBE_ATUO_ATOMIC_IND_LIST.local_list = []
MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST = threading.local()
MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST.local_list = []
MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST.params_added = False

# save tik global tensor name, size to list
TIK_GLOBAL_TENSOR_LIST = threading.local()
TIK_GLOBAL_TENSOR_LIST.local_list = []
# save stamp tensor info
TIK_MEM_STAMP_TENSOR_LIST = threading.local()
TIK_MEM_STAMP_TENSOR_LIST.local_list = []


# Example: pylint: disable=useless-object-inheritance,too-few-public-methods
class CceFlag(object):
    """
    CceFlag
    """

    BatchBindOnly = False


def update_kernel_name_by_enable_vector_core(kernel_name):
    from asc_op_compile_base.common.platform.platform_info import get_soc_spec
    from asc_op_compile_base.common import cce_params

    core_type = get_soc_spec("AICORE_TYPE")
    if core_type == "AiCore":
        kernel_name += cce_params.MIX_AIC_SUFFIX
    elif core_type == "VectorCore":
        kernel_name += cce_params.MIX_AIV_SUFFIX
    return kernel_name


# Example: pylint: disable=broad-except
def write_code(js_dict, fname):
    """
    write code
    """
    try:
        with open(fname, "w") as nwe_file:
            # Only the owner and group have rights
            os.chmod(fname, stat.S_IRUSR + stat.S_IWUSR + stat.S_IRGRP)
            json.dump(
                js_dict, nwe_file, sort_keys=True, indent=4, separators=(",", ":")
            )
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("open file error, reason:" + err)
        )


def add_json_info(title_dict, json_info, json_info_tuple):
    """
    add json info
    """
    json_info = json_info or {}
    json_info_tuple = json_info_tuple or {}
    for key, val in json_info.items():
        title_dict[key.name] = val.value
        if key.name == "batchBindOnly":
            CceFlag.BatchBindOnly = True
    for key, val in json_info_tuple.items():
        list_value = []
        for args in val:
            list_value.append(int(args))
        title_dict[key.name] = list_value
    return title_dict


def _get_kernel_magic_ascend_310B(aicore_type: str, mix: str = None) -> dict:
    """
    get the magic info for Ascend310B
    Parameters
    ----------
    aicore_type: core type, string of VectorCore or AiCore

    Returns
    -------
    the magic info dict

    """
    from asc_op_compile_base.common.platform.platform_info import get_soc_spec

    value = get_soc_spec("cube_vector_combine")
    value_str_list = value.split(",")
    if (
        value_str_list[0] == "unknown"
        or ("fuse" in value_str_list and len(value_str_list)) == 1
    ):
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    elif mix == "MIX":
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    elif aicore_type == "VectorCore":
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AIVEC"}
    elif aicore_type == "AiCore":
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AICUBE"}
    else:
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    return title_dict


def _get_kernel_magic_asecnd_910B(
    aicore_type: str,
    mix: str = None,
    compatible_needed: bool = False,
    mix_type_info: str = None,
) -> dict:
    """
    get the magic info for Ascend910B / Ascend910_93 / Ascend950 / MC62 / MC32DM11A
    Parameters
    ----------
    aicore_type: core type, VectorCore or AiCore
    compatible_needed: whether mix type ops
    mix_type_info: mix type info, aic_mix or aiv_mix

    Returns
    -------
    the magic dict for Ascend910B

    """
    magic_dict = {
        "aic_mix": {"magic": "FFTS_BINARY_MAGIC_ELF_MIX_AIC"},
        "aiv_mix": {"magic": "FFTS_BINARY_MAGIC_ELF_MIX_AIV"},
    }
    org_mix_flag = mix
    if compatible_needed and mix != "MIX":
        mix = "MIX"
        if aicore_type == "VectorCore":
            mix_type_info = "aiv_mix"
        elif aicore_type == "AiCore":
            mix_type_info = "aic_mix"
        return {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import enforce_mix_mode

    if ccec.current_build_config().get(enforce_mix_mode):
        return {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    if aicore_type == "VectorCore" and org_mix_flag != "MIX":
        if mix == "MIX":
            title_dict = magic_dict.get(
                mix_type_info, {"magic": "FFTS_BINARY_MAGIC_ELF_MIX_AIV"}
            )
        else:
            title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AIVEC"}
    elif aicore_type == "AiCore" and org_mix_flag != "MIX":
        if mix == "MIX":
            title_dict = magic_dict.get(
                mix_type_info, {"magic": "FFTS_BINARY_MAGIC_ELF_MIX_AIC"}
            )
        else:
            title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AICUBE"}
    else:
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    return title_dict


def _add_kernel_magic(
    short_soc_version: str,
    aicore_type: str,
    compatible_needed: bool = False,
    mix: str = None,
    mix_type_info: str = None,
) -> dict:
    """
    get the kernel magic info
    Parameters
    ----------
    short_soc_version: short soc version
    aicore_type: core type
    compatible_needed: whether mix ops
    mix_type_info: mix type info, aic_mix or aiv_mix

    Returns
    -------
    the magic dict

    """
    if (
        short_soc_version in ["Ascend610", "BS9SX1A", "Ascend610B", "Ascend310P"]
        and aicore_type == "VectorCore"
    ):
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AIVEC"}
    elif short_soc_version in [
        "Ascend910B",
        "Ascend910_93",
        "Ascend950",
        "MC62",
        "MC32DM11A",
        "Ascend350",
    ]:
        title_dict = _get_kernel_magic_asecnd_910B(
            aicore_type, mix, compatible_needed, mix_type_info
        )
    elif short_soc_version == "Ascend310B" or short_soc_version == "AS31XM1":
        title_dict = _get_kernel_magic_ascend_310B(aicore_type, mix)
    elif short_soc_version == "Ascend031":
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AICUBE"}
    else:
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF"}
    return title_dict


def _add_core_type(title_dict, core_type_info, mix):
    from asc_op_compile_base.common.platform.platform_info import get_soc_spec

    if "split" in get_soc_spec("cube_vector_combine") and mix != "MIX":
        for key, value in core_type_info.items():
            title_dict[key] = value
    return title_dict


# 'pylint: disable=too-many-arguments
def init_json_info(
    target: any,
    kernel_name: any,
    core_type_info: any,
    compatible_needed: bool = False,
    mix: str = None,
    mix_type: str = None,
) -> any:
    """
    init json info
    :param target: comple target
    :param kernel_name: kernel name of ops
    :param core_type_info: core type AIC or AIV
    :param compatible_needed: compatible for c220
    :param mix_type: mix type of ops for c220
    :return: title_dict
    :rtype dict
    """
    from asc_op_compile_base.common.platform.platform_info import get_soc_spec

    is_aicpu = False
    if target == "cce_core":
        from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
            enable_vector_core,
        )

        if ccec.current_build_config().get(enable_vector_core):
            kernel_name = update_kernel_name_by_enable_vector_core(kernel_name)

        short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
        aicore_type = get_soc_spec("AICORE_TYPE")
        title_dict = _add_kernel_magic(
            short_soc_version, aicore_type, compatible_needed, mix, mix_type
        )
        title_dict = _add_core_type(title_dict, core_type_info, mix)
    elif target == "cce_cpu":
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AICPU"}
        is_aicpu = True
    elif target == "cce_cpu_llvm":
        title_dict = {"magic": "RT_DEV_BINARY_MAGIC_ELF_AICPU"}
        if len(kernel_name) < 2:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                "Invalid code, the length of kernel_name \
                                  in IR code too short",
            )
        kernel_name = kernel_name[1:]
        is_aicpu = True
    else:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, "Unknown architecture, does not support now"
        )
    return is_aicpu, kernel_name, title_dict


def _add_kernel_name(kernel_name, title_dict, kernel_list, kernel_list_deterministic):
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import build_fatbin

    if ccec.current_build_config().get(build_fatbin):
        title_dict["kernelName"] = kernel_name
        if len(kernel_list) != len(kernel_list_deterministic):
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                "kernel_list size is not equal to kernel_list_deterministic size",
            )
        if kernel_list:
            title_dict["kernelList"] = []
            for idx, name in enumerate(kernel_list):
                name = str(name).rstrip('"').lstrip('"')
                deterministic = (
                    str(kernel_list_deterministic[idx]).rstrip('"').lstrip('"')
                )
                title_dict["kernelList"].append(
                    {"kernelName": name, "deterministic": deterministic}
                )
        else:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, "kernelList must have kernel_tilingkey"
            )
    else:
        title_dict["kernelName"] = kernel_name + "__kernel0"


def _add_context_info(workspace_dict, fake_name="workspace_"):
    from asc_op_compile_base.common.context import get_context

    context = get_context()
    if workspace_dict["num"] == len(workspace_dict["type"]) and workspace_dict[
        "num"
    ] == len(workspace_dict["size"]):
        for idx in range(workspace_dict["num"]):
            context.add_workspace(
                fake_name + str(idx),
                workspace_dict["size"][idx],
                workspace_dict["type"][idx],
            )
    else:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            "size and type in workspace dict should share the same size " + "as num",
        )


def _add_workspace_info(
    title_dict, workspace_list, is_context=False, fake_name="workspace_"
):
    # compute workspace info from workspace_list
    ori_num = len(workspace_list)
    ori_type_list = [
        (val[2] if isinstance(val, tuple) and len(val) > 2 else 0)
        for val in workspace_list
    ]
    ori_size_list = [
        (val if not isinstance(val, tuple) else val[1] if len(val) > 1 else val[0])
        for val in workspace_list
    ]
    ori_name_list = [
        (
            val[0]
            if isinstance(val, tuple) and len(val) > 1 and val[0]
            else fake_name + str(i)
        )
        for i, val in enumerate(workspace_list)
    ]

    # workspace_list check
    for idx in range(ori_num):
        if not isinstance(ori_type_list[idx], int) or (
            ori_type_list[idx] != 0 and ori_type_list[idx] != 1
        ):
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                "workspace type size should be int value 0 or 1",
            )
        if not isinstance(ori_size_list[idx], int) or ori_size_list[idx] < -1:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                "workspace size should be int value >= -1",
            )
        if not isinstance(ori_name_list[idx], str):
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, "workspace name should be string type"
            )

    # remove the element in workspace_list, if its name is the same as element in context.
    from asc_op_compile_base.common.context import get_context

    context = get_context()
    num = 0
    type_list = []
    size_list = []
    name_list = []
    if is_context and context:
        name_set = set([name for name, _, _ in context.get_workspaces()])
        for idx in range(ori_num):
            if ori_name_list[idx] not in name_set:
                name_list.append(ori_name_list[idx])
                size_list.append(ori_size_list[idx])
                type_list.append(ori_type_list[idx])
                num += 1
    else:
        type_list = ori_type_list
        size_list = ori_size_list
        name_list = ori_name_list
        num = ori_num
    if num == 0:
        return

    # add workspace info to title_dict for json file output
    if "workspace" in title_dict:
        if (
            "num" in title_dict["workspace"]
            and "size" in title_dict["workspace"]
            and "type" in title_dict["workspace"]
        ):
            title_dict["workspace"] = {
                "num": (title_dict["workspace"]["num"] + num),
                "size": (title_dict["workspace"]["size"] + size_list),
                "type": (title_dict["workspace"]["type"] + type_list),
            }
    else:
        title_dict["workspace"] = {"num": num, "size": size_list, "type": type_list}

    # add workspace info to context
    if is_context and context:
        for idx in range(num):
            context.add_workspace(name_list[idx], size_list[idx], 0)


def _add_pass_workspace(title_dict):
    if (
        hasattr(MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST, "local_list")
        and len(MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST.local_list) > 0
    ):
        _add_workspace_info(
            title_dict, MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST.local_list, True, "pass_"
        )
    return title_dict


def _add_workspace_to_title_dict(title_dict):
    # Example: pylint: disable:len-as-condition
    """
    only consider:
        1. pure TIK
        2. TIK TBE hybrid:
            a. use create_block_sync()
            b. sync tensor applied by user
        3. pure TBE: use create_block_sync()
        add worksapce info in order:
            TIK_WORKSPACE_SIZE_LIST
            TBE_WORKSPACE_SIZE_LIST
    """
    # set parameters info
    tbe_worksapce = (
        hasattr(TBE_WORKSPACE_SIZE_LIST, "local_list")
        and len(TBE_WORKSPACE_SIZE_LIST.local_list) > 0
    )
    tik_workspace = (
        hasattr(TIK_WORKSPACE_SIZE_LIST, "local_list")
        and len(TIK_WORKSPACE_SIZE_LIST.local_list) > 0
    )

    from asc_op_compile_base.common.platform.platform_info import (
        get_soc_spec,
        ASCEND_910B,
        ASCEND_910_93,
    )

    is_c220 = get_soc_spec("SHORT_SOC_VERSION") in [ASCEND_910B, ASCEND_910_93]
    if tbe_worksapce:  # use create_block_sync()
        if (
            hasattr(TIK_ATOMIC_ADD_LIST, "local_list")
            and len(TIK_ATOMIC_ADD_LIST.local_list) > 0
        ):
            # 2. TIK TBE hybrid: a. use create_block_sync()
            for _ in range(len(TBE_WORKSPACE_SIZE_LIST.local_list)):
                # need atomic_clean
                title_dict["parameters"].append({"init_value": 0.0, "dtype": "float32"})
        elif not is_c220:
            # 3. pure TBE: use create_block_sync()
            for i in TBE_WORKSPACE_IND_LIST.local_list:
                # need atomic_clean
                if i < len(title_dict["parameters"]):
                    title_dict["parameters"][i] = {
                        "init_value": 0.0,
                        "dtype": "float32",
                    }

    # add tik workspace info
    if tik_workspace:
        _add_workspace_info(
            title_dict, TIK_WORKSPACE_SIZE_LIST.local_list, True, "tik_"
        )
    # add tbe workspace from block sync
    if tbe_worksapce:  # use create_block_sync()
        _add_workspace_info(
            title_dict, TBE_WORKSPACE_SIZE_LIST.local_list, True, "tbe_"
        )

    if (
        hasattr(TBE_ATUO_ATOMIC_IND_LIST, "local_list")
        and len(TBE_ATUO_ATOMIC_IND_LIST.local_list) > 0
    ):
        for i in TBE_ATUO_ATOMIC_IND_LIST.local_list:
            title_dict["parameters"][i] = {"init_value": 0.0, "dtype": "float32"}

    # add pass workspace info
    title_dict = _add_pass_workspace(title_dict)


def _add_global_tensor_title_to_dict(tensor_size_dict):
    """
    update global_tensor info
    :param tensor_size_dict:
    :type tensor_size_dict: dict
    :return: no return
    :rtype None
    """
    if (
        hasattr(TIK_GLOBAL_TENSOR_LIST, "local_list")
        and len(TIK_GLOBAL_TENSOR_LIST.local_list) > 0
    ):
        for global_workspace_tensor in TIK_GLOBAL_TENSOR_LIST.local_list:
            if global_workspace_tensor:
                tensor_size_dict[
                    "".join(["globalworkspace_", global_workspace_tensor[0]])
                ] = {"size": global_workspace_tensor[1]}


def _mkdir_json_path(dir_path: str, kernel_name: str):
    """
    mkdir json file path
    :param dir_path: base path
    :param kernel_name: kernel name
    :return: json file name
    """
    file_name = os.path.join(dir_path, kernel_name + ".json")
    from asc_op_compile_base.common import cce_params

    if not os.path.exists(dir_path):
        try:
            os.mkdir(dir_path)
        except OSError as err:
            # 17, OSError: [Errno 17] File exists
            if err.errno == cce_params.FILE_EXISTS_ERR:
                pass
            else:
                raise err
        os.chmod(dir_path, stat.S_IRWXU + stat.S_IRGRP + stat.S_IXGRP)
    return file_name


def _add_mem_stamp_tensor_to_dict(tensor_size_dict):
    """
    add memory stamp gm tensor to json
    Parameters
    ----------
    tensor_size_dict: tensor size dict

    Returns None
    -------

    """
    if hasattr(TIK_MEM_STAMP_TENSOR_LIST, "local_list"):
        tensor_size_dict["memoryStamping"] = TIK_MEM_STAMP_TENSOR_LIST.local_list


def _save_operator_json_info(
    kernel_name,
    is_aicpu,
    title_dict,
    kernel_list,
    kernel_list_deterministic,
    mix,
    is_ffts_id_needed,
):
    from asc_op_compile_base.common.platform.platform_info import get_soc_spec
    from asc_op_compile_base.common.platform.platform_info import COMPILER_ARCH
    from asc_op_compile_base.common.platform.platform_info import KernelName
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
        kernel_meta_parent_dir,
    )
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import build_fatbin
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import tbe_debug_level
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import output_dir
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import status_check
    from asc_op_compile_base.common.platform.platform_info import (
        ASCEND_910,
        ASCEND_310P,
        HI3796CV300ES,
        HI3796CV300CS,
    )

    # for aicpu supports os only
    aicpu_support_os = get_soc_spec(COMPILER_ARCH) == "true"

    # bin file without suffix
    bin_file_name = ""
    bin_file_suffix = ".o"
    header_file_suffix = ".h"
    bin_file_name = kernel_name
    if is_aicpu and aicpu_support_os:
        bin_file_name = "lib" + bin_file_name
        bin_file_suffix = ".so"

    # user set output directory in debug mode
    from .ascendc_common_utility import CommonUtility

    parent_dir = CommonUtility.get_kernel_meta_parent_dir()
    dir_path = os.path.join(parent_dir, "kernel_meta")
    if ccec.current_build_config().get(build_fatbin):
        dir_path = os.path.join(
            parent_dir,
            "kernel_meta",
            KernelName.get_kernel_name(),
        )
    if (
        ccec.current_build_config().get(tbe_debug_level) > 0
        and output_dir in ccec.current_build_config()
        and ccec.current_build_config().get(output_dir) != ""
    ):
        dir_path = ccec.current_build_config().get(output_dir)

    # compute the sha256 of a given tvm_cce_op file used by domi
    sha256_hash = ""
    bin_file_path = os.path.join(dir_path, bin_file_name + bin_file_suffix)
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
        tbe_debug_level,
        op_debug_config,
    )

    if os.path.exists(bin_file_path):
        with open(bin_file_path, "rb") as nwe_file:
            # Only the owner and group have rights
            os.chmod(bin_file_path, stat.S_IRUSR + stat.S_IWUSR + stat.S_IRGRP)
            sha256_hash = hashlib.sha256(nwe_file.read()).hexdigest()
    elif ccec.current_build_config().get(
        tbe_debug_level
    ) > 0 or "dump_bin" in ccec.current_build_config().get(op_debug_config):
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, "Open operator file failed."
        )

    # new parameters in aicpuos feature
    _add_kernel_name(kernel_name, title_dict, kernel_list, kernel_list_deterministic)
    title_dict["binFileSuffix"] = bin_file_suffix
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import enable_model_fusion

    if ccec.current_build_config().get(enable_model_fusion):
        title_dict["headerFileSuffix"] = header_file_suffix
    title_dict["binFileName"] = bin_file_name
    title_dict["sha256"] = sha256_hash
    short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
    support_soc_list = [ASCEND_310P, ASCEND_910, HI3796CV300ES, HI3796CV300CS]
    if (
        ccec.current_build_config().get(status_check)
        and short_soc_version in support_soc_list
    ):
        title_dict["globalworkspace_spec_workspace"] = {"size": 32, "type": 0}
    # add workspace info to .json
    _add_workspace_to_title_dict(title_dict)

    # add tensor size info to json
    _add_global_tensor_title_to_dict(title_dict)

    # add stamp tensor info to json
    _add_mem_stamp_tensor_to_dict(title_dict)

    # the tvm_cce_op json file used by domi
    # kernel_name has been checked, it only contains letters, numbers and underscores
    file_name = _mkdir_json_path(dir_path, kernel_name)
    from asc_op_compile_base.common.context import get_context
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import dynamic_tik

    context = get_context()
    if context and context.get_compile_info("dfx_size"):
        dfx_size = int(context.get_compile_info("dfx_size"))
        op_para_size = 0
        if "oriOpParaSize" in title_dict:
            op_para_size = title_dict["oriOpParaSize"]
        elif "opParaSize" in title_dict:
            op_para_size = title_dict["opParaSize"]
        if "oom" in ccec.current_build_config().get(op_debug_config):
            # tiling data size should align to 8
            op_para_size = (op_para_size + 7) // 8 * 8
        if (
            ccec.current_build_config().get(build_fatbin)
            or ccec.current_build_config().get(dynamic_tik)
            or dfx_size != -1
        ):
            title_dict["opParaSize"] = int(op_para_size + dfx_size)

    final_dict = title_dict.copy()
    write_code(final_dict, file_name)

    if context is not None:
        if mix != "MIX" and not ccec.current_build_config().get(build_fatbin):
            _json_abs = os.path.abspath(file_name)
            context.add_build_res("json_file_path", _json_abs)
            try:
                import tbe.common.context.op_context as _tbe_op_context

                _tbe_ctx = _tbe_op_context.get_context()
                if _tbe_ctx is not None:
                    _tbe_ctx.add_build_res("json_file_path", _json_abs)
            except Exception:
                pass
        context.add_addition("tik_mix_core_mode", mix)


def compile_buffer_random(kernel_name):
    from asc_op_compile_base.common.context import get_context
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
        kernel_meta_parent_dir,
    )

    context = get_context()
    if not context:
        return False
    if not context.get_addition("compile_reset_op"):
        return False
    else:
        buffer_type = context.get_addition("compile_reset_op")
        bin_path = (
            ccec.current_build_config().get(kernel_meta_parent_dir)
            + "/kernel_meta/"
            + kernel_name
            + ".o"
        )
        reset_op_info = {
            "type": buffer_type,
            "bin_path": bin_path,
            "kernel_name": kernel_name,
        }
        if context.get_addition("reset_op_info"):
            context.get_addition("reset_op_info").append(reset_op_info)
        else:
            context.add_addition("reset_op_info", [reset_op_info])
        return True


def _get_atomic_init_args(atomic_args):
    list_value = []
    for args in atomic_args.split("_"):
        if args.isnumeric():
            list_value.append(None)
        else:
            tmp = args.split("*")
            if "." not in tmp[0]:
                list_value.append({"init_value": int(tmp[0]), "dtype": tmp[1]})
            else:
                list_value.append({"init_value": float(tmp[0]), "dtype": tmp[1]})
    return list_value


# Example: pylint: disable=too-many-locals,too-many-branches,too-many-statements,too-many-arguments
# numblocks: cpu num,default value is 1.
# @register_func
def tvm_callback_cce_postproc(
    target,
    kernel_name,
    numblocks=1,
    args_size=0,
    atomic_args="",
    json_info=None,
    json_info_tuple=None,
    core_type_info=None,
    kernel_list=None,
    kernel_list_deterministic=None,
    is_ffts_id_needed: bool = False,
    subnumblocks: int = 1,
    mix: str = None,
    mix_type_info: str = None,
) -> any:
    """
    cce postproc
    """
    if compile_buffer_random(kernel_name):
        return

    is_aicpu, kernel_name, title_dict = init_json_info(
        target, kernel_name, core_type_info, is_ffts_id_needed, mix, mix_type_info
    )
    if mix != "MIX":
        subnumblocks = 0
    # to short_soc_version : add taskRation and modeInArgsFirstField params
    from asc_op_compile_base.common.platform.platform_info import get_soc_spec
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import enforce_mix_mode

    core_type = get_soc_spec("AICORE_TYPE")
    if mix != "MIX":
        title_dict["coreType"] = core_type

    from asc_op_compile_base.common.platform.platform_info import ASCEND_910B
    from asc_op_compile_base.common.platform.platform_info import ASCEND_910_93
    from asc_op_compile_base.common.platform.platform_info import ASCEND_950
    from asc_op_compile_base.common.platform.platform_info import MC62
    from asc_op_compile_base.common.platform.platform_info import MC32DM11A

    short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
    if short_soc_version in [ASCEND_910B, ASCEND_910_93, ASCEND_950, MC62, MC32DM11A]:
        if (
            is_ffts_id_needed
            or mix == "MIX"
            or ccec.current_build_config().get(enforce_mix_mode)
        ):
            title_dict["coreType"] = "MIX"
            from asc_op_compile_base.common import cce_params

            if core_type == "AiCore" and mix != "MIX":
                title_dict[cce_params.JSON_KEY_TASK_RATION] = "1:0"
            elif core_type == "VectorCore" and mix != "MIX":
                title_dict[cce_params.JSON_KEY_TASK_RATION] = "0:1"

    title_dict["blockDim"] = numblocks
    title_dict["opParaSize"] = args_size

    if short_soc_version in [
        "Ascend910B",
        "Ascend910_93",
        "Ascend950",
        "MC62",
        "MC32DM11A",
    ]:
        mode_in_args_first_field = 0
        if is_ffts_id_needed or ccec.current_build_config().get(enforce_mix_mode):
            mode_in_args_first_field = 1
        title_dict["intercoreSync"] = mode_in_args_first_field
    title_dict = add_json_info(title_dict, json_info, json_info_tuple)
    # Example: pylint: disable:len-as-condition
    list_value = []
    if atomic_args:
        list_value = _get_atomic_init_args(atomic_args)
    title_dict["parameters"] = list_value
    if (
        hasattr(TIK_ATOMIC_ADD_LIST, "local_list")
        and len(TIK_ATOMIC_ADD_LIST.local_list) > 0
    ):
        title_dict["parameters"] = TIK_ATOMIC_ADD_LIST.local_list
    from asc_op_compile_base.common.context import get_context

    context = get_context()
    if context:
        # get workspace list from dsl
        workspace_list = context.get_workspaces()
        _add_workspace_info(title_dict, workspace_list)

        # get json info from dsl
        jsons = context.get_build_json_result(None)
        if_get_workspace_from_json = "workspace" in jsons and not workspace_list
        for key in jsons.keys():
            if workspace_list and key == "workspace":
                continue
            title_dict[key] = jsons[key]

        # add workspace info to context if the info comes from get_build_json_result
        if if_get_workspace_from_json:
            if (
                "size" in jsons["workspace"]
                and "type" in jsons["workspace"]
                and "num" in jsons["workspace"]
            ):
                _add_context_info(jsons["workspace"])

    _save_operator_json_info(
        kernel_name,
        is_aicpu,
        title_dict,
        kernel_list,
        kernel_list_deterministic,
        mix,
        is_ffts_id_needed,
    )
