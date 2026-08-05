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
ascendc compile base
"""

import os
import stat
import shutil
import subprocess
import multiprocessing
import re
from collections import namedtuple
from dataclasses import dataclass
from tbe.tvm.runtime import cce_runtime
from tbe.common.buildcfg import get_current_build_config
from tbe.tvm.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from tbe.tvm.contrib.ccec import CCECInfo
from .ascendc_common_utility import (
    CommonUtility,
    write_mk,
    is_enable_ascendc_cov,
    is_enable_build_log,
    is_enable_sanitizer,
)
from .global_storage import global_var_storage
from .log_utils import AscendCLogLevel, CompileStage
from .get_op_tiling import OpInfo
from .ascendc_constants import (
    KernelMetaType,
    CORE_TYPE_MIX,
    CORE_TYPE_CUBE,
    CORE_TYPE_VEC,
)
from .ascendc_kernel_feature_manager import global_ascendc_kernel_feature_manager


def compile_pre_process(op_info: OpInfo, compile_options: list):
    cce_runtime.TBE_WORKSPACE_SIZE_LIST.local_list = []
    cce_runtime.TBE_WORKSPACE_IND_LIST.local_list = []
    cce_runtime.MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST.local_list = []
    cce_runtime.TBE_ATUO_ATOMIC_IND_LIST.local_list = []
    CommonUtility.get_ascendc_compiler_path()
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        CommonUtility.remove_options(compile_options, ["-x", "cce"])
        compile_options.append("--cce-aicore-lang")
    from tbe.common.buildcfg.buildcfg_mapping import op_debug_config

    op_debug_config_val = get_current_build_config(op_debug_config)
    compile_options.append("--cce-disable-kernel-global-attr-check")
    global_var_storage.set_variable("ascendc_enable_super_kernel", False)
    global_var_storage.set_variable("ascendc_sk_double_compile", False)
    global_var_storage.set_variable("ascendc_sk_sub_combine_norm_workflow", False)
    global_var_storage.set_variable("ascendc_sub_super_kernel_params", "")
    global_var_storage.set_variable("ascendc_sub_super_kernel_type", "")
    global_var_storage.set_variable("ascendc_sub_super_kernel_fun_names", {})
    global_var_storage.set_variable(
        "ascendc_compile_debug_config", "dump_cce" in op_debug_config_val
    )
    global_var_storage.set_variable(
        "ascendc_dump_disable_compile_options", "-DASCENDC_DUMP=0" in compile_options
    )
    global_var_storage.set_variable(
        "ascendc_debug_compile_options", "-DASCENDC_DEBUG" in compile_options
    )
    global_var_storage.set_variable(
        "ascendc_enable_sanitizer", is_enable_sanitizer(compile_options)
    )
    global_var_storage.set_variable("ascendc_enable_build_log", is_enable_build_log())
    global_var_storage.set_variable("ascendc_enable_coverage", is_enable_ascendc_cov())
    global_var_storage.set_variable(
        "ascendc_time_stamp_compile_options",
        "-DASCENDC_TIME_STAMP_ON" in compile_options,
    )
    global_var_storage.set_variable(
        "ascendc_enable_super_kernel",
        (
            bool(get_current_build_config("enable_super_kernel"))
            and CommonUtility.is_support_super_kernel()
        ),
    )
    global_var_storage.set_variable(
        "ascendc_enable_aicore_exception_restart",
        "-DAICORE_EXCEPTION_RESTART" in compile_options,
    )
    if global_var_storage.get_variable("ascendc_enable_coverage"):
        compile_options.append("-g")
    global_ascendc_kernel_feature_manager.init_available_and_enable_features()
    return compile_options


def get_actual_kernel_type(tiling_key, compile_info, need_ffts, kernel_name):
    code_type = compile_info.code_channel
    default_kernel_type = compile_info.default_kernel_type
    kernel_type = (
        compile_info.tiling_key_kernel_type[tiling_key]
        if tiling_key in compile_info.tiling_key_kernel_type
        else default_kernel_type
    )
    if kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0]:
        return CORE_TYPE_CUBE
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0]:
        return CORE_TYPE_VEC
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2]:
        return CORE_TYPE_MIX
    if compile_info.no_set_kernel_type and need_ffts:
        return code_type
    else:
        CommonUtility.print_compile_log(
            kernel_name,
            "Aicore Exception Restart not support this kernel type",
            AscendCLogLevel.LOG_ERROR,
        )
        raise Exception("Aicore Exception Restart not support this kernel type")


SingleTilingKeyCompileParams = namedtuple(
    "SingleTilingKeyCompileParams",
    [
        "tiling_key",
        "compile_info",
        "sub_arch",
        "tiling_info",
        "code_channel",
        "compile_option_tuple",
    ],
)


def fatbin_objs(obj_files: list, dst_file: str, is_debug: bool, compile_log_path=None):
    if global_var_storage.get_variable(
        "ascendc_enable_super_kernel"
    ) is True and global_var_storage.get_variable("ascendc_is_static_op"):
        return
    compile_cmd = [
        CCECInfo.get_exe("ld.lld"),
        "-m",
        "aicorelinux",
        "-r",
        "-Ttext=0",
        "-q",
    ]
    if not is_debug:
        compile_cmd.append("-x")
    for obj in obj_files:
        compile_cmd += [obj]
    compile_cmd += ["-static", "-o", "%s" % dst_file]
    CommonUtility.run_cmd_inner(compile_cmd, CompileStage.FATBIN, compile_log_path)
    if not global_var_storage.get_variable(
        "ascendc_compile_debug_config"
    ) and not global_var_storage.get_variable("super_kenel_save_sub_op_files"):
        for obj in obj_files:
            os.remove(obj)


def link_relocatable(bin_file_path, compile_log_path=None):
    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    if short_soc_version == "Ascend310B":
        link_cmd = [
            CCECInfo.get_exe("ld.lld"),
            "-m",
            "aicorelinux",
            "-Ttext=0",
            "%s" % bin_file_path,
            "-static",
            "-o",
            "%s" % bin_file_path,
            "-q",
        ]
    else:
        link_cmd = [
            CCECInfo.get_exe("ld.lld"),
            "-m",
            "aicorelinux",
            "-Ttext=0",
            "%s" % bin_file_path,
            "-static",
            "-o",
            "%s" % bin_file_path,
            "-q",
        ]
    CommonUtility.run_cmd_inner(link_cmd, CompileStage.LINKRELOCATE, compile_log_path)


def link_relocatable_meta_file(bin_file_path, meta_file_path, compile_log_path=None):
    link_cmd = [
        CCECInfo.get_exe("ld.lld"),
        "-m",
        "aicorelinux",
        "-Ttext=0",
        "%s" % bin_file_path,
        "%s" % meta_file_path,
        "-static",
        "-o",
        "%s" % bin_file_path,
        "-q",
    ]
    CommonUtility.run_cmd_inner(link_cmd, CompileStage.LINKRELOCATE, compile_log_path)


def link_sk_norm_combine(
    sk_bin_file,
    norm_bin_file,
    sk_bind_dst_file,
    meta_file_path,
    compile_log_path=None,
):
    # Step 1: 解压 sk_bin_file (它是由 ar crs 打包的 .o 文件)
    # 创建临时目录用于解压
    temp_extract_dir = os.path.join(
        os.path.dirname(sk_bin_file), "temp_extract_" + str(os.getpid())
    )
    os.makedirs(temp_extract_dir, exist_ok=True)

    try:
        # 解压 sk_bin_file 到临时目录
        CommonUtility.print_compile_log(
            "",
            f"Extracting sk_bin_file: {sk_bin_file} to {temp_extract_dir}",
            AscendCLogLevel.LOG_DEBUG,
        )
        extract_cmd = ["ar", "x", sk_bin_file]
        CommonUtility.dump_compile_log(
            extract_cmd, CompileStage.LINKRELOCATE, compile_log_path
        )
        result = subprocess.run(
            extract_cmd, cwd=temp_extract_dir, capture_output=True, text=True
        )
        if result.returncode != 0:
            raise Exception(f"Failed to extract sk_bin_file: {result.stderr}")

        # 列出解压出的所有 .o 文件
        extracted_objs = [
            os.path.join(temp_extract_dir, f)
            for f in os.listdir(temp_extract_dir)
            if f.endswith(".o")
        ]
        CommonUtility.print_compile_log(
            "",
            f"Extracted object files: {[os.path.basename(f) for f in extracted_objs]}",
            AscendCLogLevel.LOG_DEBUG,
        )

        # Step 2: 将解压的 .o 文件、norm_bin_file、sk_bind_dst_file 和可选的 meta_file_path 合并
        merged_obj = os.path.join(temp_extract_dir, "merged_sk_norm_bind.o")
        link_cmd = [CCECInfo.get_exe("ld.lld"), "-r", "-o", merged_obj]
        link_cmd.extend(extracted_objs)
        link_cmd.extend([norm_bin_file, sk_bind_dst_file])
        if meta_file_path:
            link_cmd.append(meta_file_path)
        CommonUtility.dump_compile_log(
            link_cmd, CompileStage.LINKRELOCATE, compile_log_path
        )
        result = subprocess.run(link_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f"Failed to link merged objects: {result.stderr}")

        # Step 3: 自连接 merged_obj 生成最终的 sk_bin_file
        link_cmd = [
            CCECInfo.get_exe("ld.lld"),
            "-m",
            "aicorelinux",
            "-Ttext=0",
            merged_obj,
            "-static",
            "-o",
            sk_bin_file,
            "-q",
        ]
        CommonUtility.dump_compile_log(
            link_cmd, CompileStage.LINKRELOCATE, compile_log_path
        )
        result = subprocess.run(link_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f"Failed to self-link: {result.stderr}")

        CommonUtility.print_compile_log(
            "",
            f"Successfully created final sk_bin_file: {sk_bin_file}",
            AscendCLogLevel.LOG_INFO,
        )

    finally:
        CommonUtility.print_compile_log(
            "", "Successfully sk combine link", AscendCLogLevel.LOG_INFO
        )


def _get_max_parallel_num():
    cpu_db_num = max(1, 2 * multiprocessing.cpu_count() - 2)
    cpu_sched_num = max(1, 2 * len(os.sched_getaffinity(0)) - 2)
    return min(cpu_db_num, cpu_sched_num)


def _ignore_parallel_job_self_set():
    ascend_self_par_job = os.getenv("TILINGKEY_PARALLEL_JOB")
    ascend_self_par_job_compatible = os.getenv("ASCENDC_PAR_COMPILE_JOB")
    if ascend_self_par_job is not None:
        CommonUtility.print_compile_log(
            "",
            "Detected that the TILINGKEY_PAR_COMPILE enviroment variable has been set, \
ignoring TILINGKEY_PARALLEL_JOB.",
            AscendCLogLevel.LOG_WARNING,
        )
    elif ascend_self_par_job_compatible is not None:
        CommonUtility.print_compile_log(
            "",
            "Detected that the TILINGKEY_PAR_COMPILE enviroment variable has been set, \
ignoring ASCENDC_PAR_COMPILE_JOB.",
            AscendCLogLevel.LOG_WARNING,
        )


def _get_parallel_job_without_op_project(tilingkey_num: int = 1):
    ascend_self_par_job_compatible = os.getenv("ASCENDC_PAR_COMPILE_JOB")
    if ascend_self_par_job_compatible is not None:
        CommonUtility.print_compile_log(
            "",
            "ASCENDC_PAR_COMPILE_JOB enviroment variable is deprecated, \
please use TILINGKEY_PARALLEL_JOB instead!",
            AscendCLogLevel.LOG_WARNING,
        )
    # formally
    ascend_self_par_job = os.getenv("TILINGKEY_PARALLEL_JOB")
    max_job_num = _get_max_parallel_num()
    ascendc_self_par_job_num = 1
    if ascend_self_par_job_compatible is not None:
        ascendc_self_par_job_num = int(ascend_self_par_job_compatible)
        if ascendc_self_par_job_num == 1:
            ascendc_self_par_job_num = max_job_num
        else:
            ascendc_self_par_job_num = min(
                max(1, ascendc_self_par_job_num), max_job_num
            )
    elif ascend_self_par_job is not None:
        ascendc_self_par_job_num = min(max(1, int(ascend_self_par_job)), max_job_num)
    else:
        # default is 64 or max job, which may cause host memory exaust
        default_job = 8 if (CommonUtility.is_v100() or CommonUtility.is_v200()) else 64
        ascendc_self_par_job_num = max(1, min(default_job, max_job_num, tilingkey_num))
        CommonUtility.print_compile_log(
            "",
            f"tiling key num is {tilingkey_num}, \
tilingkey parallel compile num is {ascendc_self_par_job_num}",
            AscendCLogLevel.LOG_INFO,
        )
    return ascendc_self_par_job_num


def compile_multi_tilingkey(tiling_key_list, cmds_list, dstfile_name, compile_log_path):
    parallel_compile_check = os.getenv("TILINGKEY_PAR_COMPILE")
    if parallel_compile_check not in [None, "1", "0"]:
        CommonUtility.print_compile_log(
            "",
            "TILINGKEY_PAR_COMPILE ONLY SUPPORT 0 OR 1, current \
TILINGKEY_PAR_COMPILE is {}".format(parallel_compile_check),
            AscendCLogLevel.LOG_WARNING,
        )

    dstfile_with_pid = os.path.join(
        CommonUtility.get_kernel_meta_dir(), dstfile_name + "_" + str(os.getpid())
    )
    write_mk(tiling_key_list, cmds_list, dstfile_with_pid, compile_log_path)
    mk_file = f"{dstfile_with_pid}.mk"
    if parallel_compile_check == "1":
        _ignore_parallel_job_self_set()
        cmd = ["make", "-f", mk_file]
    else:
        ascendc_self_par_job_num = _get_parallel_job_without_op_project(
            len(tiling_key_list)
        )
        cmd = ["make", "-f", mk_file, "-j", f"{ascendc_self_par_job_num}"]
    cmd_str = " ".join(cmd)
    file_name = ""
    if global_var_storage.get_variable("ascendc_enable_build_log") is True:
        file_name, kernel_name, hash_name = CommonUtility.get_build_file_name(
            cmds_list[0], CompileStage.COMPILE
        )
        try:
            with open(file_name, mode="at") as f:
                os.chmod(file_name, stat.S_IRUSR + stat.S_IWUSR)
                f.write("%s\n" % (cmd_str))
                cmd.append("2>&1")
                cmd.append("|")
                cmd.append("tee -a")
                cmd.append(file_name)
                cmd_str = " ".join(cmd)
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, ("write log failed, reason is:", err)
            )
    ret = os.system(f"{cmd_str} > /dev/null")
    if ret != 0 and global_var_storage.get_variable("ascendc_enable_build_log") is True:
        file_name_parts = file_name.split(".")
        new_file_name = file_name_parts[0] + "_error." + file_name_parts[-1]
        os.rename(file_name, new_file_name)
        CommonUtility.print_compile_log(
            "",
            "Operator {}_{}: errors occurred during compile phase \
of {}, See also {}".format(
                kernel_name, hash_name, str(CompileStage.COMPILE), new_file_name
            ),
            AscendCLogLevel.LOG_ERROR,
        )
        raise Exception(
            "An error occurred during compile phases of {}".format(
                str(CompileStage.COMPILE)
            )
        )
    if not global_var_storage.get_variable("ascendc_compile_debug_config"):
        CommonUtility.remove_temp_file(mk_file)


def search_in_line(line, keywords):
    pattern = re.compile(
        r"\b(" + "|".join(re.escape(keyword) for keyword in keywords) + r")\b"
    )
    matches = pattern.findall(line)
    if matches:
        return True, f"{', '.join(matches)}"
    return False, ""


def extract_file_path(line):
    pattern = re.compile(r'"([^"]+)"')
    matches = pattern.findall(line)
    return matches[0]
