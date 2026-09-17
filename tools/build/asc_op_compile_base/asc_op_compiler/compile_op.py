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
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)
from asc_op_compile_base.common.context import get_context

# compile_op 自身已不再使用，但 test_compile_op.py 有一处
# patch.object(compile_op_module, "get_current_build_config")，删掉会变成 AttributeError
from asc_op_compile_base.common.buildcfg import get_current_build_config  # noqa: F401
from .get_op_tiling import TilingInfo, OpInfo, get_tiling_info_by_tiling
from asc_op_compile_base.common.utils.log_utils import (
    LogUtil,
    AscendCLogLevel,
    CompileStage,
    COMPILE_STAGE_MSG_INFO,
)
from .global_storage import global_var_storage
from .ascendc_constants import (
    InferChannelParamsFromIFile,
    InferChannelParams,
    CompileOptionTuple,
    CORE_TYPE_MIX,
    CORE_TYPE_CUBE,
    CORE_TYPE_VEC,
    MIX_CORE_MACRO,
    CustomizedConfig,
    GEN_PLACE_HOLDER_STR,
)
from .ascendc_common_utility import (
    CommonUtility,
    CompileInfo,
    SkBindCommand,
    convert_customized_config_to_inferchannel,
)
from .ascendc_compile_dfx import (
    DFXParamType,
    DFXPointType,
    DFXArgInfo,
    DFXSectionGenerator,
)
from .ascendc_compile_v220 import (
    gen_compile_cmd_v220,
    get_v220_kernel_type_mix_flag,
    get_code_channel_v220_by_first_tiling_key,
    gen_compile_cmd_for_meta_info,
)
from .ascendc_compile_gen_code import (
    _gen_compile_cmd,
    delete_tiling_section,
    gen_kernel_fun,
    gen_meta_info_section,
    gen_op_stub_kernel_func,
    gen_tiling_struct_size_and_dfx_section_file,
    _gen_kernel_func_declare_head,
    _get_tiling_struct_size,
    _get_tiling_struct_without_register_size,
)
from .ascendc_compile_gen_json import (
    _gen_mix_json_from_seperate_json,
    _gen_mix_json_from_seperate_json_for_kernel_type,
    _dynamic_kernel_list_to_json,
    _dynamic_regbase_kernel_list_to_json,
    _gen_mix_sub_json,
    _gen_static_json_for_no_mix_v200,
    _gen_non_mix_sub_json,
    _gen_static_json_for_mix_v200,
    _gen_dynamic_json_for_v200,
    _json_post_process,
    _json_except_info,
)
from .ascendc_compile_base import (
    compile_multi_tilingkey,
    link_relocatable,
    SingleTilingKeyCompileParams,
    compile_pre_process,
    link_relocatable_meta_file,
    link_sk_norm_combine,
)
from .super_kernel_sub_op_compile import (
    split_sub_kernel_objs,
    gen_sub_super_kernel_compile_options,
)
from .static_compile_resource_generator import (
    KernelSpecCompilation,
)
from .super_kernel_option_parse import parse_super_kernel_options
from .kernel_info_infer import KernelInfoInfer
from .ascendc_compile_utils import check_custom_dcci_end_false, check_if_gen_placehoder
from .ascendc_compile_regbase import (  # noqa: F401
    _compile_ascendc_cce_regbase,
    _mssanitizer_link,
)
from .ascendc_compile_backend_kernel_type import (  # noqa: F401
    _compile_ascendc_cce,
    _compile_ascendc_cce_m510,
    _compile_ascendc_cce_v220,
    _compile_ascendc_cce_v200_with_kernel_type,
    _compile_ascendc_cce_v220_with_kernel_type,
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

    compile_op_common_part(
        cce_file,
        origin_func_name,
        op_info,
        compile_option_tuple,
        infered_info_from_ifile,
        extend_options,
    )


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
